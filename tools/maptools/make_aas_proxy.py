#!/usr/bin/env python3
"""Coarse stand-in map for building the bot AAS of a generated terrain map.

The terrain generators (Crossroads, Canyon Rush, Mesa King) build the ground
from tens of thousands of triangle prisms. bspc then runs into its AAS_MAX_*
limits or needs hours. Bots only need the walkable surface, so this script
writes a second map with the same entities and objects, but with the terrain
re-meshed by an error-driven quadtree: big cells where the ground is flat,
small ones only where the height changes (track edges, canyon walls, creek).

The proxy surface keeps the exact height at its vertices and is never more
than ABOVE units above the real ground, so a bot standing on the real floor
is always in open AAS space.

Steps (paths relative to the repository root):

  python tools/maptools/make_aas_proxy.py baseq3r/maps/q3r_canyonrush.map
  q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -bsp -custinfoparms baseq3r/maps/q3r_canyonrush_aasproxy.map
  bspc -forcesidesvisible -optimize -bsp2aas baseq3r/maps/q3r_canyonrush_aasproxy.bsp
  python tools/maptools/make_aas_proxy.py --checksum baseq3r/maps/q3r_canyonrush_aasproxy.aas baseq3r/maps/q3r_canyonrush.bsp

The last step writes baseq3r/maps/q3r_canyonrush.aas with the checksum of the
real BSP (the engine refuses an AAS whose checksum does not match the map).
Rebuild the AAS whenever the real BSP is recompiled.
"""
import argparse
import math
import re
import struct
import sys
from pathlib import Path

G = 64          # smallest cell and vertex lattice
ROOT_CELL = 1024
SAMPLE = 32     # spacing of the real-height samples used for the error test
ABOVE = 8       # proxy may lie at most this far above the real ground
BELOW = 24      # and at most this far below it
STEEP = 0.8     # ground steeper than this (rise per unit) is not checked
CLIMB = 0.6     # cars do not get up slopes steeper than this (about 31 degrees)
WALL = 1.0      # bspc counts ground up to 45 degrees as walkable; real walls next to the
NEAR = 256      # drivable area (within NEAR units) must stay steeper than that in the proxy
CAULK = "common/caulk"
DETAIL = 134217728

POINT = re.compile(r"\(\s*([-\d.e]+)\s+([-\d.e]+)\s+([-\d.e]+)\s*\)")


# ---------------------------------------------------------------- .map parsing

def parse_map(text):
    """Entities as (key lines, [brush texts]). Brush text includes braces."""
    ents = []
    lines = text.splitlines()
    k = 0
    while k < len(lines):
        line = lines[k].strip()
        k += 1
        if line != "{":
            continue
        keys, brushes = [], []
        while k < len(lines):
            line = lines[k].strip()
            k += 1
            if line == "}":
                break
            if line == "{":
                body = ["{"]
                depth = 1
                while depth:
                    raw = lines[k]
                    k += 1
                    s = raw.strip()
                    if s == "{":
                        depth += 1
                    elif s == "}":
                        depth -= 1
                    body.append(raw)
                brushes.append("\n".join(body))
            elif line and not line.startswith("//"):
                keys.append(line)
        ents.append((keys, brushes))
    return ents


def face_parts(line):
    pts = [tuple(float(v) for v in m.groups()) for m in POINT.finditer(line)]
    rest = line[line.rfind(")") + 1:].split()
    return pts, (rest[0] if rest else "")


def terrain_prism(brush):
    """Top triangle (3 xyz) and floor z of a generator prism, else None."""
    faces = [l.strip() for l in brush.splitlines()[1:-1] if l.strip().startswith("(")]
    if len(faces) != 5:
        return None
    parsed = [face_parts(f) for f in faces]
    if any(len(p) != 3 for p, _ in parsed):
        return None
    (top, top_mat), (bottom, bottom_mat) = parsed[0], parsed[1]
    if top_mat == CAULK or bottom_mat != CAULK or len({p[2] for p in bottom}) != 1:
        return None
    floor = bottom[0][2]
    for pts, mat in parsed[2:]:
        if mat != CAULK or pts[0][2] != floor or pts[0][:2] != pts[1][:2]:
            return None
    return top, floor


# ---------------------------------------------------------------- real height field

class Field:
    def __init__(self, tris, bucket=256):
        self.bucket, self.cells = bucket, {}
        for t in tris:
            xs, ys = [p[0] for p in t], [p[1] for p in t]
            for bx in range(math.floor(min(xs) / bucket), math.floor(max(xs) / bucket) + 1):
                for by in range(math.floor(min(ys) / bucket), math.floor(max(ys) / bucket) + 1):
                    self.cells.setdefault((bx, by), []).append(t)
        self.memo = {}
        self.reach = None
        self.near = None

    def wall(self, x, y):
        """Real ground too steep for the AAS, close to where cars drive."""
        if self.near is not None and (x, y) not in self.near:
            return False
        h = SAMPLE / 2
        zs = [self.height(x + dx, y + dy) for dx, dy in ((h, 0), (-h, 0), (0, h), (0, -h))]
        if None in zs:
            return False
        return math.hypot(zs[0] - zs[1], zs[2] - zs[3]) / (2 * h) > WALL

    def walkable(self, x, y):
        """Only drivable ground needs an exact proxy; walls just have to stay walls."""
        if self.reach is not None and (x, y) not in self.reach:
            return False
        z = self.height(x, y)
        if z is None:
            return False
        h = SAMPLE / 2
        for dx, dy in ((h, 0), (-h, 0), (0, h), (0, -h)):
            n = self.height(x + dx, y + dy)
            if n is not None and abs(n - z) / h > STEEP:
                return False
        return True

    def height(self, x, y):
        key = (x, y)
        if key in self.memo:
            return self.memo[key]
        best = None
        for (ax, ay, az), (bx, by, bz), (cx, cy, cz) in self.cells.get(
                (math.floor(x / self.bucket), math.floor(y / self.bucket)), ()):
            det = (by - cy) * (ax - cx) + (cx - bx) * (ay - cy)
            if abs(det) < 1e-9:
                continue
            u = ((by - cy) * (x - cx) + (cx - bx) * (y - cy)) / det
            v = ((cy - ay) * (x - cx) + (ax - cx) * (y - cy)) / det
            w = 1 - u - v
            if u < -1e-6 or v < -1e-6 or w < -1e-6:
                continue
            z = u * az + v * bz + w * cz
            best = z if best is None else max(best, z)
        self.memo[key] = best
        return best


# ---------------------------------------------------------------- drivable region

SEEDS = ("info_player_start", "info_player_deathmatch", "team_", "rally_", "weapon_", "item_", "ammo_", "holdable_")


def brush_faces(brush):
    return [face_parts(l.strip()) + (l.strip(),) for l in brush.splitlines() if l.strip().startswith("(")]


def brush_vertices(brush):
    planes = []
    for pts, _, _ in brush_faces(brush):
        p0, p1, p2 = pts
        d1 = [b - a for a, b in zip(p0, p1)]
        d2 = [b - a for a, b in zip(p0, p2)]
        n = (d2[1] * d1[2] - d2[2] * d1[1], d2[2] * d1[0] - d2[0] * d1[2], d2[0] * d1[1] - d2[1] * d1[0])
        ln = math.sqrt(sum(c * c for c in n)) or 1
        n = tuple(c / ln for c in n)
        planes.append((n, sum(a * b for a, b in zip(n, p0))))
    verts = []
    for i in range(len(planes)):
        for j in range(i + 1, len(planes)):
            for k in range(j + 1, len(planes)):
                (a, da), (b, db), (c, dc) = planes[i], planes[j], planes[k]
                det = (a[0] * (b[1] * c[2] - b[2] * c[1]) - a[1] * (b[0] * c[2] - b[2] * c[0])
                       + a[2] * (b[0] * c[1] - b[1] * c[0]))
                if abs(det) < 1e-9:
                    continue
                v = [0.0, 0.0, 0.0]
                for axis in range(3):
                    m = [list(a), list(b), list(c)]
                    for row, d in zip(m, (da, db, dc)):
                        row[axis] = d
                    v[axis] = (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
                               - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                               + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0])) / det
                if all(sum(q * w for q, w in zip(n, v)) <= dist + 0.5 for n, dist in planes):
                    verts.append(v)
    return verts


def drivable(field, kept, bx0, by0, bx1, by1):
    """Samples a car can reach from the spawns and pickups: walkable steps, not
    through the invisible boundary brushes (shader name contains "boundary")."""
    blocked = set()
    for _, brushes in kept:
        for b in brushes:
            if any("boundary" in mat for _, mat, _ in brush_faces(b)):
                vs = brush_vertices(b)
                if not vs:
                    continue
                x0 = math.floor((min(v[0] for v in vs) - 24 - bx0) / SAMPLE) * SAMPLE + bx0
                y0 = math.floor((min(v[1] for v in vs) - 24 - by0) / SAMPLE) * SAMPLE + by0
                for x in range(x0, int(max(v[0] for v in vs) + 24) + 1, SAMPLE):
                    for y in range(y0, int(max(v[1] for v in vs) + 24) + 1, SAMPLE):
                        blocked.add((x, y))
    stack = []
    for keys, brushes in kept:
        cls = next((k.split('"')[3] for k in keys if k.startswith('"classname"')), "")
        org = next((k.split('"')[3] for k in keys if k.startswith('"origin"')), None)
        if org and not brushes and cls.startswith(SEEDS):
            x, y = (float(v) for v in org.split()[:2])
            sx = bx0 + round((x - bx0) / SAMPLE) * SAMPLE
            sy = by0 + round((y - by0) / SAMPLE) * SAMPLE
            if bx0 <= sx <= bx1 and by0 <= sy <= by1 and (sx, sy) not in blocked:
                stack.append((sx, sy))
    reach = set(stack)
    limit = CLIMB * SAMPLE
    while stack:
        x, y = stack.pop()
        z = field.height(x, y)
        if z is None:
            continue
        for nx, ny in ((x + SAMPLE, y), (x - SAMPLE, y), (x, y + SAMPLE), (x, y - SAMPLE)):
            if (nx, ny) in reach or (nx, ny) in blocked or not (bx0 <= nx <= bx1 and by0 <= ny <= by1):
                continue
            nz = field.height(nx, ny)
            if nz is not None and abs(nz - z) <= limit:
                reach.add((nx, ny))
                stack.append((nx, ny))
    return reach


def keep_brush(brush, reach, bx0, by0, margin=192):
    """Structural, boundary and drivable-area detail brushes stay; scenery far
    outside the drivable region (forests on the hills) is dropped."""
    faces = brush_faces(brush)
    flags = faces[0][2][faces[0][2].rfind(")") + 1:].split()
    if len(flags) < 7 or not int(flags[6]) & DETAIL or any("boundary" in m for _, m, _ in faces):
        return True
    pts = [p for f in faces for p in f[0]]
    lo = [min(p[i] for p in pts) - margin for i in range(2)]
    hi = [max(p[i] for p in pts) + margin for i in range(2)]
    if (hi[0] - lo[0]) * (hi[1] - lo[1]) > 4096 * 4096:
        return True
    x = bx0 + math.floor((lo[0] - bx0) / SAMPLE) * SAMPLE
    while x <= hi[0]:
        y = by0 + math.floor((lo[1] - by0) / SAMPLE) * SAMPLE
        while y <= hi[1]:
            if (x, y) in reach:
                return True
            y += SAMPLE
        x += SAMPLE
    return False


# ---------------------------------------------------------------- proxy mesh

def surface(corners, diag, u, v):
    a, b, c, d = corners  # (x0,y0) (x1,y0) (x1,y1) (x0,y1)
    if diag == "ac":
        return a + (b - a) * u + (c - b) * v if v <= u else a + (c - d) * u + (d - a) * v
    return a + (b - a) * u + (d - a) * v if u + v <= 1 else c + (d - c) * (1 - u) + (b - c) * (1 - v)


def gradient(corners, diag, u, v, w, h):
    a, b, c, d = corners
    if diag == "ac":
        gx, gy = ((b - a), (c - b)) if v <= u else ((c - d), (d - a))
    else:
        gx, gy = ((b - a), (d - a)) if u + v <= 1 else ((c - d), (c - b))
    return math.hypot(gx / w, gy / h)


def fit(field, x0, y0, x1, y1, corners):
    """Best diagonal and its (above, below) error against the real samples.
    A wall next to the drivable area that the proxy flattens into AAS ground
    counts as an error too; bots would route over it."""
    best = None
    for diag in ("ac", "bd"):
        above = below = 0.0
        flattened = False
        y = y0
        while y <= y1:
            x = x0
            while x <= x1:
                z = field.height(x, y)
                u, v = (x - x0) / (x1 - x0), (y - y0) / (y1 - y0)
                if z is not None and field.walkable(x, y):
                    p = surface(corners, diag, u, v)
                    above, below = max(above, p - z), max(below, z - p)
                elif (z is not None and not flattened and field.wall(x, y)
                      and gradient(corners, diag, min(u, 0.999), min(v, 0.999), x1 - x0, y1 - y0) <= WALL):
                    flattened = True
                x += SAMPLE
            y += SAMPLE
        score = max(above / ABOVE, below / BELOW) + (100 if flattened else 0)
        if best is None or score < best[0]:
            best = (score, diag, above, below)
    return best


def build_leaves(field, bx0, by0, bx1, by1):
    vh = {}

    def vert(x, y):
        if (x, y) not in vh:
            z = field.height(x, y)
            if z is None:  # hole: nearest real sample around it
                for r in range(SAMPLE, 8 * G, SAMPLE):
                    cand = [field.height(x + dx, y + dy) for dx in (-r, 0, r) for dy in (-r, 0, r)]
                    cand = [c for c in cand if c is not None]
                    if cand:
                        z = min(cand)
                        break
            vh[x, y] = 0.0 if z is None else z
        return vh[x, y]

    leaves, stack = [], []
    for y in range(by0, by1, ROOT_CELL):
        for x in range(bx0, bx1, ROOT_CELL):
            stack.append((x, y, min(x + ROOT_CELL, bx1), min(y + ROOT_CELL, by1)))
    while stack:
        x0, y0, x1, y1 = stack.pop()
        corners = (vert(x0, y0), vert(x1, y0), vert(x1, y1), vert(x0, y1))
        score, diag, _, _ = fit(field, x0, y0, x1, y1, corners)
        w, h = x1 - x0, y1 - y0
        if score <= 1 or (w <= G and h <= G):
            leaves.append((x0, y0, x1, y1))
            continue
        xm = x0 + (w // 2 // G) * G if w > G else x1
        ym = y0 + (h // 2 // G) * G if h > G else y1
        for cell in ((x0, y0, xm, ym), (xm, y0, x1, ym), (x0, ym, xm, y1), (xm, ym, x1, y1)):
            if cell[0] < cell[2] and cell[1] < cell[3]:
                stack.append(cell)
    # straighten the edges of bigger cells at lattice points so finer neighbours meet them
    for (x0, y0, x1, y1) in sorted(leaves, key=lambda c: -max(c[2] - c[0], c[3] - c[1])):
        for (ax, ay), (bx, by) in (((x0, y0), (x1, y0)), ((x1, y0), (x1, y1)),
                                   ((x0, y1), (x1, y1)), ((x0, y0), (x0, y1))):
            n = max(abs(bx - ax), abs(by - ay)) // G
            za, zb = vert(ax, ay), vert(bx, by)
            for k in range(1, n):
                p = (ax + (bx - ax) * k // n, ay + (by - ay) * k // n)
                vh[p] = za + (zb - za) * k / n
    out = []
    for (x0, y0, x1, y1) in leaves:
        corners = (vh[x0, y0], vh[x1, y0], vh[x1, y1], vh[x0, y1])
        _, diag, above, below = fit(field, x0, y0, x1, y1, corners)
        out.append((x0, y0, x1, y1, diag, above, below))
    return out, vh


def num(v):
    s = f"{v:.3f}".rstrip("0").rstrip(".")
    return s if s not in ("", "-0") else "0"


def face(points):
    p = " ".join("( %s )" % " ".join(num(c) for c in xyz) for xyz in points)
    return f"{p} {CAULK} 0 0 0 0.5 0.5 {DETAIL} 0 0"


def prism(a, b, c, floor):
    faces = [face((a, c, b)), face(((0, 0, floor), (128, 0, floor), (0, 128, floor)))]
    for p, q in ((a, b), (b, c), (c, a)):
        faces.append(face(((p[0], p[1], floor), p, q)))
    return "{\n" + "\n".join(faces) + "\n}"


def proxy_brushes(leaves, vh, floor):
    out = []
    for (x0, y0, x1, y1, diag, _, _) in leaves:
        a, b = (x0, y0, vh[x0, y0]), (x1, y0, vh[x1, y0])
        c, d = (x1, y1, vh[x1, y1]), (x0, y1, vh[x0, y1])
        tris = ((a, b, c), (a, c, d)) if diag == "ac" else ((a, b, d), (b, c, d))
        out += [prism(*t, floor) for t in tris]
    return out


# ---------------------------------------------------------------- checksum

def md4(msg):
    def rol(x, n):
        x &= 0xffffffff
        return ((x << n) | (x >> (32 - n))) & 0xffffffff
    ml = len(msg) * 8
    msg = msg + b"\x80" + b"\0" * ((55 - len(msg)) % 64) + struct.pack("<Q", ml)
    a, b, c, d = 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
    rounds = ((lambda x, y, z: (x & y) | (~x & z), range(16), (3, 7, 11, 19), 0),
              (lambda x, y, z: (x & y) | (x & z) | (y & z), (0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15),
               (3, 5, 9, 13), 0x5a827999),
              (lambda x, y, z: x ^ y ^ z, (0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15),
               (3, 9, 11, 15), 0x6ed9eba1))
    for off in range(0, len(msg), 64):
        X = struct.unpack("<16I", msg[off:off + 64])
        s = [a, b, c, d]
        for fn, order, shifts, const in rounds:
            for i, k in enumerate(order):
                j = (-i) % 4
                w, x, y, z = s[j], s[(j + 1) % 4], s[(j + 2) % 4], s[(j + 3) % 4]
                s[j] = rol(w + fn(x, y, z) + X[k] + const, shifts[i % 4])
        a, b, c, d = [(p + q) & 0xffffffff for p, q in zip((a, b, c, d), s)]
    return a, b, c, d


def bsp_checksum(path):
    a, b, c, d = md4(Path(path).read_bytes())
    return struct.unpack("<i", struct.pack("<I", a ^ b ^ c ^ d))[0]


def write_checksum(aas_in, bsp, aas_out):
    data = bytearray(Path(aas_in).read_bytes())
    ident, version = struct.unpack_from("<4si", data, 0)
    if ident != b"EAAS":
        sys.exit(f"{aas_in}: not an AAS file")
    cs = struct.pack("<i", bsp_checksum(bsp))
    data[8:12] = bytes(v ^ ((i * 119) & 0xff) for i, v in enumerate(cs)) if version == 5 else cs
    Path(aas_out).write_bytes(data)
    return struct.unpack("<i", cs)[0]


# ---------------------------------------------------------------- main

def make_proxy(src):
    ents = parse_map(Path(src).read_text(encoding="ascii", errors="replace"))
    tris, floors, kept = [], set(), []
    for keys, brushes in ents:
        rest = []
        for b in brushes:
            pr = terrain_prism(b)
            if pr:
                tris.append(pr[0])
                floors.add(pr[1])
            else:
                rest.append(b)
        terrain_group = any(k.startswith('"_indexmap"') for k in keys)
        if terrain_group and not rest:
            continue
        keys = [k for k in keys if not re.match(r'"(_indexmap|_layers|_shader|_terrain)"', k)]
        kept.append((keys, rest))
    if not tris:
        sys.exit(f"{src}: no terrain prisms found")
    floor = min(floors)
    xs = [p[0] for t in tris for p in t]
    ys = [p[1] for t in tris for p in t]
    bx0, by0 = math.floor(min(xs) / G) * G, math.floor(min(ys) / G) * G
    bx1, by1 = math.ceil(max(xs) / G) * G, math.ceil(max(ys) / G) * G
    field = Field(tris)
    field.reach = drivable(field, kept, bx0, by0, bx1, by1)
    near, ring = set(field.reach), set(field.reach)
    for _ in range(NEAR // SAMPLE):
        ring = {(x + dx, y + dy) for x, y in ring for dx, dy in
                ((SAMPLE, 0), (-SAMPLE, 0), (0, SAMPLE), (0, -SAMPLE))} - near
        near |= ring
    field.near = near
    dropped = 0
    for idx, (keys, brushes) in enumerate(kept):
        cls = next((k.split('"')[3] for k in keys if k.startswith('"classname"')), "")
        if cls in ("worldspawn", "func_group"):
            keep = [b for b in brushes if keep_brush(b, field.reach, bx0, by0)]
            dropped += len(brushes) - len(keep)
            kept[idx] = (keys, keep)
    kept = [e for i, e in enumerate(kept) if i == 0 or e[1] or not any(
        k == '"classname" "func_group"' for k in e[0])]
    leaves, vh = build_leaves(field, bx0, by0, bx1, by1)
    proxy = proxy_brushes(leaves, vh, floor)
    text = [f"// AAS proxy of {Path(src).name}, written by tools/maptools/make_aas_proxy.py.",
            "// Only for bspc. Do not ship or edit; regenerate from the real map."]
    for idx, (keys, brushes) in enumerate(kept):
        body = list(brushes) + (proxy if idx == 0 else [])
        text.append("{\n" + "\n".join(keys + body) + "\n}")
    out = Path(src).with_name(Path(src).stem + "_aasproxy.map")
    out.write_text("\n".join(text) + "\n", encoding="ascii")
    sizes = {}
    for (x0, y0, x1, y1, *_r) in leaves:
        s = max(x1 - x0, y1 - y0)
        sizes[s] = sizes.get(s, 0) + 1
    worst_above = max(l[5] for l in leaves)
    worst_below = max(l[6] for l in leaves)
    print(f"{src}: {len(tris)} terrain triangles -> {len(proxy)} proxy triangles")
    print(f"  drivable samples: {len(field.reach)}, scenery brushes dropped: {dropped}")
    print("  cells by size:", dict(sorted(sizes.items())))
    print(f"  worst deviation: {worst_above:.1f} above, {worst_below:.1f} below the real ground")
    print("  wrote", out)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("map", nargs="?", help="generated .map to make the proxy of")
    ap.add_argument("--checksum", nargs=2, metavar=("PROXY_AAS", "REAL_BSP"),
                    help="copy the proxy AAS to <real map>.aas with the checksum of the real BSP")
    ap.add_argument("--above", type=float, default=ABOVE, help="max proxy height above the real ground (%(default)s)")
    ap.add_argument("--below", type=float, default=BELOW, help="max proxy height below the real ground (%(default)s)")
    args = ap.parse_args()
    globals().update(ABOVE=args.above, BELOW=args.below)
    if args.checksum:
        aas, bsp = args.checksum
        out = Path(bsp).with_suffix(".aas")
        cs = write_checksum(aas, bsp, out)
        print(f"wrote {out} (bsp checksum {cs})")
    elif args.map:
        make_proxy(args.map)
    else:
        ap.print_help()


if __name__ == "__main__":
    main()
