#!/usr/bin/env python3
"""Change how large textures are on the surfaces of a compiled Q3Rally map.

  python rescale_textures.py maps/q3r_mk.bsp --report
  python rescale_textures.py maps/q3r_mk.bsp base_wall/concrete1=3 --map maps/q3r_mk.map

--report lists the texel density of every texture on brush faces and
patches (texels per game unit, area weighted, largest area first). Quake 3
textures at the default scale 0.5 have 2 texels per unit; much less looks
blurry, much more looks grainy and repeats visibly.

shader=factor makes the texture repeat factor times as often (2 = half as
large, 0.5 = twice as large) on the brush faces and patches of the map;
shader=fs:ft scales the two texture directions apart (s across, t down the
image), e.g. 1:3 for a trim that fits across but is stretched along;
models (misc_model) are left alone. The shader is given without
"textures/". In the BSP only the texture coordinates of those surfaces
change (lightmaps, bot navigation and everything else stay); the BSP
checksum in the .aas next to the map is updated so botlib still loads it.
Surfaces that continue each other keep continuing: before the scaling, the
whole turns of the texture are taken off the coordinates in the same way
on every face of the same plane.

--map also changes the .map source the same way (old style and brush
primitive faces, patches), so the next compile keeps the new size. A copy
of each changed file is kept as .bak.
"""
import argparse
import glob
import math
import os
import re
import shutil
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import q3bsp  # noqa: E402

LUMP_SHADERS = 1
LUMP_DRAWVERTS = 10
LUMP_DRAWINDEXES = 11
LUMP_SURFACES = 13
VERT_SIZE = 44
SURF_SIZE = 104
MST_PLANAR, MST_PATCH, MST_TRIANGLE_SOUP = 1, 2, 3
SURF_SKY, SURF_NODRAW = 0x4, 0x80


def shader_names(data):
    o, l = q3bsp.read_lumps(data)[LUMP_SHADERS]
    out = []
    for i in range(l // 72):
        p = o + i * 72
        name = data[p:p + 64].split(b'\0')[0].decode('latin-1')
        flags = struct.unpack_from('<i', data, p + 64)[0]
        out.append((name, flags))
    return out


def surfaces(data):
    o, l = q3bsp.read_lumps(data)[LUMP_SURFACES]
    for i in range(l // SURF_SIZE):
        yield struct.unpack_from('<7i', data, o + i * SURF_SIZE)


def short_name(name):
    name = name.replace('\\', '/')
    return name[9:] if name.lower().startswith('textures/') else name


# ── texture sizes for the report ────────────────────────────────────────────

def first_images(scripts):
    """shader name (lower case) -> image of its first map stage."""
    images = {}
    for path in glob.glob(os.path.join(scripts, '*.shader')):
        text = re.sub(r'//[^\n]*', '', open(path, encoding='latin-1').read())
        depth, name, last, first, prev = 0, None, None, None, None
        for m in re.finditer(r'\{|\}|[^\s{}]+', text):
            tok = m.group(0)
            if tok == '{':
                if depth == 0:
                    name, first = last, None
                depth += 1
            elif tok == '}':
                depth -= 1
                if depth == 0 and name:
                    images.setdefault(name.lower(), first)
            else:
                if depth >= 1 and prev and prev.lower() in ('map', 'clampmap') and first is None \
                        and not tok.startswith('$'):
                    first = tok
                prev, last = tok, tok
    return images


def image_size(base, name):
    stem = os.path.splitext(name)[0]
    for ext in ('.tga', '.jpg', '.png', '.TGA', '.JPG', '.PNG'):
        path = os.path.join(base, stem + ext)
        if os.path.exists(path):
            return read_size(path)
    return None


def read_size(path):
    with open(path, 'rb') as f:
        head = f.read(32)
        if path.lower().endswith('.tga'):
            return struct.unpack_from('<HH', head, 12)
        if head[:8] == b'\x89PNG\r\n\x1a\n':
            return struct.unpack_from('>II', head, 16)
        f.seek(2)
        while True:  # JPEG: find a start-of-frame marker
            marker = f.read(2)
            if len(marker) < 2 or marker[0] != 0xFF:
                return None
            if marker[1] in (0xC0, 0xC1, 0xC2):
                f.read(3)
                h, w = struct.unpack('>HH', f.read(4))
                return w, h
            size = struct.unpack('>H', f.read(2))[0]
            f.seek(size - 2, 1)


def report(data, bsp_path):
    base = os.path.join(os.path.dirname(os.path.abspath(bsp_path)), '..')
    images = first_images(os.path.join(base, 'scripts'))
    names = shader_names(data)
    vo = q3bsp.read_lumps(data)[LUMP_DRAWVERTS][0]
    io = q3bsp.read_lumps(data)[LUMP_DRAWINDEXES][0]
    acc = {}
    for sh, _, typ, fv, nv, fi, ni in surfaces(data):
        name, flags = names[sh]
        if typ not in (MST_PLANAR, MST_PATCH) or flags & (SURF_SKY | SURF_NODRAW):
            continue
        verts = [struct.unpack_from('<5f', data, vo + (fv + k) * VERT_SIZE) for k in range(nv)]
        if typ == MST_PATCH:
            continue  # control points: no triangles; patches use the faces' density
        world = tex = 0.0
        for t in range(0, ni, 3):
            a, b, c = (verts[struct.unpack_from('<i', data, io + 4 * (fi + t + j))[0]] for j in range(3))
            u = [b[j] - a[j] for j in range(3)]
            v = [c[j] - a[j] for j in range(3)]
            cross = (u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0])
            world += 0.5 * math.sqrt(sum(x * x for x in cross))
            tex += 0.5 * abs((b[3] - a[3]) * (c[4] - a[4]) - (c[3] - a[3]) * (b[4] - a[4]))
        e = acc.setdefault(name.lower(), [0.0, 0.0])
        e[0] += world
        e[1] += tex
    total = sum(w for w, _ in acc.values()) or 1.0
    print('%s: texels per unit (2 = Quake 3 default scale 0.5)' % os.path.basename(bsp_path))
    for name, (world, tex) in sorted(acc.items(), key=lambda kv: -kv[1][0]):
        size = image_size(base, images.get(name) or name)
        if not size or world <= 0:
            continue
        density = math.sqrt(tex * size[0] * size[1] / world)
        flag = '  blurry' if density < 1.0 else ('  grainy' if density > 4.5 else '')
        print('  %5.1f%%  %-44s %4dx%-4d %5.2f%s' % (100 * world / total, short_name(name), size[0], size[1],
                                                    density, flag))


# ── BSP ─────────────────────────────────────────────────────────────────────

def plane_of(points):
    """Unit normal and distance of the plane through the points."""
    best = None
    a = points[0]
    for b in points[1:]:
        for c in points[2:]:
            u = [b[j] - a[j] for j in range(3)]
            v = [c[j] - a[j] for j in range(3)]
            n = (u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0])
            length = math.sqrt(sum(x * x for x in n))
            if not best or length > best[0]:
                best = (length, n)
        if best and best[0] > 1.0:
            break
    if not best or best[0] < 1e-6:
        return None
    n = [x / best[0] for x in best[1]]
    return n, sum(n[j] * a[j] for j in range(3))


def value_at(points, values, p):
    """The affine function through (points, values) on their plane, at p."""
    a, b, c = points[0], None, None
    for i in range(1, len(points)):
        for j in range(i + 1, len(points)):
            u = [points[i][k] - a[k] for k in range(3)]
            v = [points[j][k] - a[k] for k in range(3)]
            g11 = sum(x * x for x in u)
            g12 = sum(x * y for x, y in zip(u, v))
            g22 = sum(x * x for x in v)
            det = g11 * g22 - g12 * g12
            if det > 1e-3 * max(g11 * g22, 1e-9):
                b, c = i, j
                break
        if b:
            break
    if not b:
        return values[0]
    u = [points[b][k] - a[k] for k in range(3)]
    v = [points[c][k] - a[k] for k in range(3)]
    w = [p[k] - a[k] for k in range(3)]
    g11 = sum(x * x for x in u)
    g12 = sum(x * y for x, y in zip(u, v))
    g22 = sum(x * x for x in v)
    wu = sum(x * y for x, y in zip(w, u))
    wv = sum(x * y for x, y in zip(w, v))
    det = g11 * g22 - g12 * g12
    alpha = (wu * g22 - wv * g12) / det
    beta = (wv * g11 - wu * g12) / det
    return values[0] + alpha * (values[b] - values[0]) + beta * (values[c] - values[0])


def rescale_bsp(data, factors):
    names = shader_names(data)
    buf = bytearray(data)
    vo = q3bsp.read_lumps(data)[LUMP_DRAWVERTS][0]
    counts = {}
    for sh, _, typ, fv, nv, _, _ in surfaces(data):
        key = short_name(names[sh][0]).lower()
        if key not in factors or typ not in (MST_PLANAR, MST_PATCH) or nv < 1:
            continue
        ks, kt = factors[key]
        verts = [struct.unpack_from('<5f', data, vo + (fv + i) * VERT_SIZE) for i in range(nv)]
        points = [v[:3] for v in verts]
        if typ == MST_PLANAR and nv >= 3 and plane_of(points):
            # whole turns of the texture at the point of the plane nearest to
            # the world origin: the same for every face of the plane
            n, dist = plane_of(points)
            foot = [n[j] * dist for j in range(3)]
            s0 = math.floor(value_at(points, [v[3] for v in verts], foot))
            t0 = math.floor(value_at(points, [v[4] for v in verts], foot))
        else:
            s0 = math.floor(min(v[3] for v in verts))
            t0 = math.floor(min(v[4] for v in verts))
        for i, v in enumerate(verts):
            struct.pack_into('<2f', buf, vo + (fv + i) * VERT_SIZE + 12, (v[3] - s0) * ks, (v[4] - t0) * kt)
        counts[key] = counts.get(key, 0) + 1
    return bytes(buf), counts


# ── .map source ─────────────────────────────────────────────────────────────

NUM = r'-?[0-9.eE+-]+'
OLD_FACE = re.compile(r'^(\s*\(.*?\)\s*\(.*?\)\s*\(.*?\)\s*)(\S+)(\s+)(' + NUM + r')(\s+)(' + NUM + r')(\s+)(' + NUM +
                      r')(\s+)(' + NUM + r')(\s+)(' + NUM + r')(.*)$')
BP_FACE = re.compile(r'^(\s*\(.*?\)\s*\(.*?\)\s*\(.*?\)\s*)\(\s*\(\s*(' + NUM + r')\s+(' + NUM + r')\s+(' + NUM +
                     r')\s*\)\s*\(\s*(' + NUM + r')\s+(' + NUM + r')\s+(' + NUM + r')\s*\)\s*\)(\s*)(\S+)(.*)$')
PATCH_ROW = re.compile(r'\(\s*(' + NUM + r')\s+(' + NUM + r')\s+(' + NUM + r')\s+(' + NUM + r')\s+(' + NUM + r')\s*\)')


def fmt(x):
    if abs(x) < 1e-9:
        return '0'
    return '%.10g' % x


def rescale_map(text, factors):
    lines = text.split('\n')
    counts = {}
    in_bp = False
    patch_shader = None
    patch_k = None
    for i, line in enumerate(lines):
        stripped = line.strip()
        if stripped == 'brushDef':
            in_bp = True
            continue
        if stripped in ('patchDef2', 'patchDef3'):
            patch_shader = ''
            continue
        if patch_shader == '' and stripped not in ('{',):
            patch_shader = stripped
            patch_k = factors.get(stripped.lower())
            if patch_k:
                counts[stripped.lower()] = counts.get(stripped.lower(), 0) + 1
            continue
        if patch_shader:
            if stripped == '}':
                patch_shader = None
                continue
            if patch_k and stripped.startswith('( ('):
                def row(m):
                    s, t = float(m.group(4)), float(m.group(5))
                    return '( %s %s %s %s %s )' % (m.group(1), m.group(2), m.group(3), fmt(s * patch_k[0]),
                                                   fmt(t * patch_k[1]))
                lines[i] = PATCH_ROW.sub(row, line)
            continue
        if stripped == '}' and in_bp:
            in_bp = False
            continue
        m = BP_FACE.match(line) if in_bp else None
        if m:
            k = factors.get(m.group(9).lower())
            if k:
                a = [float(m.group(j)) * k[0 if j < 5 else 1] for j in range(2, 8)]
                # keep the shift within one turn
                a[2] -= math.floor(a[2])
                a[5] -= math.floor(a[5])
                lines[i] = '%s( ( %s %s %s ) ( %s %s %s ) )%s%s%s' % (
                    m.group(1), fmt(a[0]), fmt(a[1]), fmt(a[2]), fmt(a[3]), fmt(a[4]), fmt(a[5]),
                    m.group(8), m.group(9), m.group(10))
                counts[m.group(9).lower()] = counts.get(m.group(9).lower(), 0) + 1
            continue
        m = OLD_FACE.match(line)
        if m:
            k = factors.get(m.group(2).lower())
            if k:
                xoff, yoff = float(m.group(4)) * k[0], float(m.group(6)) * k[1]
                xs, ys = float(m.group(10)) / k[0], float(m.group(12)) / k[1]
                lines[i] = '%s%s%s%s%s%s%s%s%s%s%s%s%s' % (
                    m.group(1), m.group(2), m.group(3), fmt(xoff), m.group(5), fmt(yoff), m.group(7), m.group(8),
                    m.group(9), fmt(xs), m.group(11), fmt(ys), m.group(13))
                counts[m.group(2).lower()] = counts.get(m.group(2).lower(), 0) + 1
    return '\n'.join(lines), counts


def show(k):
    return '%g' % k[0] if k[0] == k[1] else '%g:%g' % k


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('bsp')
    ap.add_argument('factors', nargs='*', help='shader=factor, shader without textures/')
    ap.add_argument('--map', help='also change this .map source')
    ap.add_argument('--report', action='store_true', help='list the texel density of the textures')
    a = ap.parse_args()

    data = open(a.bsp, 'rb').read()
    if a.report:
        report(data, a.bsp)
        return

    factors = {}
    for item in a.factors:
        if '=' not in item:
            sys.exit('expected shader=factor, got %r' % item)
        name, value = item.rsplit('=', 1)
        name = short_name(name).lower()
        try:
            parts = [float(x) for x in value.split(':')]
        except ValueError:
            sys.exit('bad factor: %r' % item)
        if len(parts) == 1:
            parts *= 2
        if len(parts) != 2 or min(parts) <= 0:
            sys.exit('factor must be > 0, or fs:ft: %r' % item)
        factors[name] = tuple(parts)
    if not factors:
        sys.exit('nothing to do (shader=factor or --report)')

    new, counts = rescale_bsp(data, factors)
    for name in factors:
        print('%s: %s x%s on %d surfaces' % (os.path.basename(a.bsp), name, show(factors[name]), counts.get(name, 0)))
    if new != data:
        shutil.copyfile(a.bsp, a.bsp + '.bak')
        open(a.bsp, 'wb').write(new)
        aas = os.path.splitext(a.bsp)[0] + '.aas'
        if os.path.exists(aas):
            q3bsp.aas_set_checksum(aas, q3bsp.bsp_checksum(new))
            print('%s: checksum updated' % os.path.basename(aas))

    if a.map:
        raw = open(a.map, 'rb').read()
        crlf = b'\r\n' in raw
        text = raw.decode('latin-1').replace('\r\n', '\n')
        out, counts = rescale_map(text, factors)
        for name in factors:
            print('%s: %s x%s on %d faces/patches' % (os.path.basename(a.map), name, show(factors[name]),
                                                      counts.get(name, 0)))
        if out != text:
            shutil.copyfile(a.map, a.map + '.bak')
            if crlf:
                out = out.replace('\n', '\r\n')
            open(a.map, 'wb').write(out.encode('latin-1'))


if __name__ == '__main__':
    main()
