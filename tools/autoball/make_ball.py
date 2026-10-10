#!/usr/bin/env python3
"""Generate the Autoball ball: models/autoball/ball.md3 plus its textures.

    python3 tools/autoball/make_ball.py [--radius 75] [--out baseq3r/models/autoball]

  ball.md3       UV sphere, shader "models/autoball/ball" (scripts/autoball.shader)
  ball.tga       base texture: classic 32-panel ball (truncated icosahedron),
                 light hexagons, graphite pentagons, recessed seams
  ball_glow.tga  additive mask (seams + pentagons). The shader adds it with
                 rgbGen entity; cgame colours it with the team of the last
                 car that touched the ball (white when nobody has).
  trail.tga      soft round glow sprite for the ball trail (autoballTrail)

The panel layout is computed per texel from the exact 3D direction, so the
equirectangular mapping does not distort the pattern. Needs numpy.
"""
import argparse
import math
import os
import struct

import numpy as np

TEX_W, TEX_H = 1024, 512
# The renderer refuses MD3 surfaces with SHADER_MAX_VERTEXES (1000) verts or
# more, or SHADER_MAX_INDEXES (6000) indexes or more: the model is then not
# drawn at all. 32 x 24 gives 825 verts and 1472 triangles.
SEGMENTS, RINGS = 32, 24
MAX_VERTS, MAX_INDEXES = 1000, 6000
SHADER = "models/autoball/ball"


# ---------------------------------------------------------------- pattern

def panel_centres():
    p = (1 + 5 ** 0.5) / 2
    ico = []
    for a in (-1, 1):
        for b in (-p, p):
            ico += [(0, a, b), (a, b, 0), (b, 0, a)]
    ico = np.array(ico, float)
    ico /= np.linalg.norm(ico, axis=1)[:, None]
    # faces: triples of mutually adjacent vertices (edge length 2)
    faces = []
    n = len(ico)
    edge = np.min([np.linalg.norm(ico[i] - ico[j]) for i in range(n) for j in range(n) if i != j])
    for i in range(n):
        for j in range(i + 1, n):
            for k in range(j + 1, n):
                if all(abs(np.linalg.norm(ico[x] - ico[y]) - edge) < 1e-6
                       for x, y in ((i, j), (j, k), (i, k))):
                    faces.append(ico[i] + ico[j] + ico[k])
    hexes = np.array(faces)
    hexes /= np.linalg.norm(hexes, axis=1)[:, None]
    return ico, hexes          # 12 pentagon centres, 20 hexagon centres


def texel_dirs():
    u = (np.arange(TEX_W) + 0.5) / TEX_W
    v = (np.arange(TEX_H) + 0.5) / TEX_H
    phi = u[None, :] * 2 * math.pi
    theta = v[:, None] * math.pi
    return np.stack([np.sin(theta) * np.cos(phi),
                     np.sin(theta) * np.sin(phi),
                     np.cos(theta) * np.ones_like(phi)], axis=-1)


def smoothstep(e0, e1, x):
    t = np.clip((x - e0) / (e1 - e0), 0, 1)
    return t * t * (3 - 2 * t)


def make_textures():
    pent, hexa = panel_centres()
    centres = np.vstack([pent, hexa])
    d = texel_dirs().reshape(-1, 3)
    dots = np.clip(d @ centres.T, -1, 1)
    order = np.argsort(-dots, axis=1)
    first, second = order[:, 0], order[:, 1]
    ang1 = np.arccos(dots[np.arange(len(d)), first])
    ang2 = np.arccos(dots[np.arange(len(d)), second])
    gap = ang2 - ang1                       # 0 on a seam
    is_pent = first < 12

    seam_w = 0.022
    seam = 1 - smoothstep(seam_w * 0.4, seam_w, gap)        # 1 on the seam
    bevel = smoothstep(seam_w, seam_w * 4, gap)             # panels bulge a bit

    rng = np.random.default_rng(75)
    grain = rng.normal(0, 1, len(d)) * 3

    hex_col = np.array([236, 236, 231], float)
    pent_col = np.array([38, 40, 46], float)
    seam_col = np.array([70, 70, 74], float)
    base = np.where(is_pent[:, None], pent_col, hex_col)
    base = base * (0.86 + 0.14 * bevel)[:, None]
    base = base * (1 - seam[:, None]) + seam_col * seam[:, None]
    base = np.clip(base + grain[:, None], 0, 255)

    glow = np.maximum(seam * 0.9, is_pent * 0.32 * bevel)
    glow = np.clip(glow * 255, 0, 255)

    return (base.reshape(TEX_H, TEX_W, 3).astype(np.uint8),
            glow.reshape(TEX_H, TEX_W).astype(np.uint8))


def make_trail_sprite(size=64):
    """soft radial glow, white on black; the shader colours it per puff"""
    c = (np.arange(size) + 0.5) / size * 2 - 1
    r = np.sqrt(c[None, :] ** 2 + c[:, None] ** 2)
    glow = np.exp(-(r / 0.42) ** 2) * (1 - smoothstep(0.85, 1.0, r))
    return (np.clip(glow, 0, 1) * 255).astype(np.uint8)


def write_tga(path, img):
    """uncompressed TGA, 24 bit colour or 8 bit greyscale (2D array)"""
    h, w = img.shape[:2]
    grey = img.ndim == 2
    header = struct.pack("<BBBHHBHHHHBB", 0, 0, 3 if grey else 2, 0, 0, 0, 0, 0, w, h,
                         8 if grey else 24, 0)
    # bottom-up rows (BGR): row 0 of the array (v = 0) ends up at the top
    data = img[::-1].tobytes() if grey else img[::-1, :, ::-1].tobytes()
    with open(path, "wb") as f:
        f.write(header + data)


# ---------------------------------------------------------------- mesh

def encode_normal(n):
    x, y, z = n
    lat = int(round(math.atan2(y, x) * 256 / (2 * math.pi))) & 255
    lng = int(round(math.acos(max(-1.0, min(1.0, z))) * 256 / (2 * math.pi))) & 255
    return (lat << 8) | lng


def make_mesh(radius):
    verts, sts = [], []
    for r in range(RINGS + 1):
        v = r / RINGS
        theta = v * math.pi
        for s in range(SEGMENTS + 1):
            u = s / SEGMENTS
            if r in (0, RINGS):
                u = (s + 0.5) / SEGMENTS     # pole: one vertex per segment
            phi = u * 2 * math.pi
            n = (math.sin(theta) * math.cos(phi), math.sin(theta) * math.sin(phi), math.cos(theta))
            verts.append(n)
            sts.append((u, v))
    tris = []
    row = SEGMENTS + 1
    for r in range(RINGS):
        for s in range(SEGMENTS):
            a = r * row + s
            b = a + 1
            c = a + row
            d = c + 1
            if r == 0:
                tris.append((a, d, c))      # a is the north pole vertex of segment s
            elif r == RINGS - 1:
                tris.append((a, b, c))      # c is the south pole vertex of segment s
            else:
                tris.append((a, b, c))
                tris.append((b, d, c))
    # Q3 wants clockwise seen from outside: cross(b-a, c-a) must point inwards
    fixed = []
    for a, b, c in tris:
        pa, pb, pc = (np.array(verts[i]) for i in (a, b, c))
        if np.dot(np.cross(pb - pa, pc - pa), pa + pb + pc) > 0:
            b, c = c, b
        fixed.append((a, b, c))
    return verts, sts, fixed


def write_md3(path, radius):
    verts, sts, tris = make_mesh(radius)
    nv, nt = len(verts), len(tris)
    if nv >= MAX_VERTS or nt * 3 >= MAX_INDEXES:
        raise SystemExit(f"mesh too big for the renderer: {nv} verts, {nt} tris "
                         f"(limit {MAX_VERTS - 1} verts, {MAX_INDEXES // 3 - 1} tris)")

    def name(s, n):
        return s.encode("ascii")[:n - 1].ljust(n, b"\0")

    surf_hdr = 108
    ofs_shaders = surf_hdr
    ofs_tris = ofs_shaders + 68
    ofs_st = ofs_tris + 12 * nt
    ofs_xyz = ofs_st + 8 * nv
    surf_end = ofs_xyz + 8 * nv
    surface = struct.pack("<4s64s10i", b"IDP3", name("ball", 64), 0, 1, 1, nv, nt,
                          ofs_tris, ofs_shaders, ofs_st, ofs_xyz, surf_end)
    surface += name(SHADER, 64) + struct.pack("<i", 0)
    surface += b"".join(struct.pack("<3i", *t) for t in tris)
    surface += b"".join(struct.pack("<2f", *st) for st in sts)
    for n in verts:
        surface += struct.pack("<3hH", *(int(round(c * radius * 64)) for c in n), encode_normal(n))
    assert len(surface) == surf_end

    frame = struct.pack("<3f3f3ff16s", -radius, -radius, -radius, radius, radius, radius,
                        0, 0, 0, radius, name("ball", 16))
    ofs_frames = 108
    ofs_tags = ofs_frames + len(frame)
    ofs_surfaces = ofs_tags
    ofs_end = ofs_surfaces + len(surface)
    header = struct.pack("<4si64s9i", b"IDP3", 15, name("models/autoball/ball.md3", 64), 0,
                         1, 0, 1, 0, ofs_frames, ofs_tags, ofs_surfaces, ofs_end)
    with open(path, "wb") as f:
        f.write(header + frame + surface)
    return nv, nt


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    default_out = os.path.normpath(os.path.join(here, "..", "..", "baseq3r", "models", "autoball"))
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--radius", type=float, default=75.0)
    parser.add_argument("--out", default=default_out)
    args = parser.parse_args()
    os.makedirs(args.out, exist_ok=True)

    nv, nt = write_md3(os.path.join(args.out, "ball.md3"), args.radius)
    base, glow = make_textures()
    write_tga(os.path.join(args.out, "ball.tga"), base)
    write_tga(os.path.join(args.out, "ball_glow.tga"), glow)
    write_tga(os.path.join(args.out, "trail.tga"), make_trail_sprite())
    print(f"ball.md3: {nv} verts, {nt} tris, radius {args.radius:g}; textures {TEX_W}x{TEX_H}")


if __name__ == "__main__":
    main()
