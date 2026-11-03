"""Build the Bountyhunter (rusty sheriff muscle car) body.md3 and wheel.md3.

usage: build_model.py <PoliceCar.3DS> <source dir with sferiff.jpg, sferiffi.jpg> <outdir>

Source: the project's PoliceCar.3DS (1,634 triangles, two hand-painted
textures). The car is kept as it is: original geometry and textures, scaled
to the size of the other cars, normals with a crease angle, tags. The lamps
are painted into the texture, so the light, plate and exhaust tags are found
by looking up where the painted parts sit on the model (uv -> 3d).

The wheel is a tyre turned on a lathe from the source wheel's outline with
the shared tread and rim textures, so the rim selection works like on the
other cars.

3ds axes: x = width (+x is the car's left), y = length (front at -y), z = up.
Q3 axes:  X = forward, Y = left, Z = up.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import numpy as np
from PIL import Image
import mesh
import tds
from carlib import BACKWARD, ID, WL, WR, Builder, build_wheel, save, snap_lights, tri_normal, wheel_tag_z

SRC, TEX_DIR, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
P = 'models/players/bountyhunter/'
LENGTH = 112.0
WHEEL_Z = -9.7
BODY_TEX = '1 - Default'                    # sferiff.jpg: paint, glass, grille, lamps, wheels
PARTS_TEX = '2 - Default'                   # sferiffi.jpg: engine, underside, interior

# painted parts in sferiff.jpg (u, v with v up, as stored in the 3ds file)
UV_HEADLIGHT = [(0.344, 0.224), (0.652, 0.224)]
UV_TAILLIGHT = [(0.469, 0.908), (0.469, 0.639)]
UV_PLATE = (0.426, 0.781)                   # the "DANGER" plate

V, VT, _, G, MAT = tds.load(SRC)
V = np.asarray(V, float)


def to_q3(p):
    p = np.asarray(p)
    return np.stack([-p[..., 1], p[..., 0], p[..., 2]], -1)


body_g = [(k, t) for k, t in G if not k[0].startswith('wheel')]
wheel_g = {k[0]: t for k, t in G if k[0].startswith('wheel')}
bt = np.array([[c[0] for c in tr] for _, t in body_g for tr in t])
bp = to_q3(V[np.unique(bt)])
S = LENGTH / (bp[:, 0].max() - bp[:, 0].min())
CX = (bp[:, 0].max() + bp[:, 0].min()) / 2

wheels = {}
for name, t in wheel_g.items():
    p = to_q3(V[np.unique([c[0] for tr in t for c in tr])])
    c = (p.min(0) + p.max(0)) / 2
    key = ('f' if c[0] > CX else 'r') + ('l' if c[1] > 0 else 'r')
    wheels[key] = (c, (p.max(0)[2] - p.min(0)[2]) / 2, t)
CZ = np.mean([w[0][2] for w in wheels.values()]) - WHEEL_Z / S
# tyre radius of the written wheel (front left): its polygon corners, not its bounds
_c = wheels['fl'][0]
_q = to_q3(V[np.unique([x[0] for tr in wheels['fl'][2] for x in tr])])
R = np.hypot(_q[:, 0] - _c[0], _q[:, 2] - _c[2]).max() * S
LIFT = (0.0, 0.0, wheel_tag_z(R) - WHEEL_Z)


def world(p):
    q = to_q3(p)
    return np.stack([(q[..., 0] - CX) * S, q[..., 1] * S, (q[..., 2] - CZ) * S], -1)


WP = world(V)

# ---------------- body ------------------------------------------------------------------
surfs = {}
for (o, m), t in body_g:
    group, tex = ('body', 'sheriff_main') if m == BODY_TEX else ('parts', 'parts')
    T = np.array([[c[0] for c in tr] for tr in t])
    verts, faces, kept = mesh.weld(WP, T, keep_mask=True)
    cn = mesh.corner_normals(verts, faces)
    b = surfs.setdefault(group, Builder(group, P + tex + '.tga', LIFT))
    for tri, ns in zip(np.array(t)[kept], cn):
        pts = [tuple(WP[c[0]]) for c in tri]
        uvs = [(VT[c[1]][0], 1 - VT[c[1]][1]) for c in tri]
        b.tri([b.vert(pts[k], tuple(ns[k]), uvs[k]) for k in range(3)])


# ---------------- tags from the painted parts ------------------------------------------------
def uv_to_world(uv, mat=BODY_TEX):
    """points of the model whose texture coordinate is uv"""
    out = []
    for (o, m), t in body_g:
        if m != mat:
            continue
        for tri in t:
            a, b, c = (np.array(VT[x[1]]) for x in tri)
            d = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1])
            if abs(d) < 1e-12:
                continue
            l1 = ((uv[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (uv[1] - a[1])) / d
            l2 = ((b[0] - a[0]) * (uv[1] - a[1]) - (uv[0] - a[0]) * (b[1] - a[1])) / d
            if l1 >= 0 and l2 >= 0 and l1 + l2 <= 1:
                pa, pb, pc = (WP[x[0]] for x in tri)
                out.append(pa + l1 * (pb - pa) + l2 * (pc - pa))
    return np.array(out)


def lamp(uv, front):
    """the painted lamp on the left side of the car (Y > 0) at the front or rear"""
    p = uv_to_world(uv)
    p = p[(p[:, 0] > 0) == front]
    return p[np.argmax(p[:, 1])] if len(p) else None


hl = [lamp(uv, True) for uv in UV_HEADLIGHT]
hl = max((h for h in hl if h is not None), key=lambda h: h[1])          # left headlight
tl = [lamp(uv, False) for uv in UV_TAILLIGHT]
tl = max((t for t in tl if t is not None), key=lambda t: t[1])          # left tail light
plate = uv_to_world(UV_PLATE)
plate = plate[plate[:, 0] < 0].mean(0)

allp = WP[np.unique(bt)]
rear_x, front_x = allp[:, 0].min(), allp[:, 0].max()
roof = allp[:, 2].max()
# exhaust: the pair of pipes under the rear bumper, left side
low = allp[(allp[:, 0] < rear_x + 3) & (allp[:, 1] > 0)]
pipe = low[np.argmin(low[:, 2])]
wc = {k: np.array([(w[0][0] - CX) * S, w[0][1] * S, (w[0][2] - CZ) * S]) for k, w in wheels.items()}   # centres are in q3 axes already
FX = (wc['fl'][0] + wc['fr'][0]) / 2
RX = (wc['rl'][0] + wc['rr'][0]) / 2
WY = np.mean([abs(w[1]) for w in wc.values()])
WZ = np.mean([w[2] for w in wc.values()])

tags = [
    ('tag_wheelfl', [FX, WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX, WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon', [-8.0, 0.0, roof], ID),
    ('tag_plate', [plate[0] - 0.3, plate[1], plate[2]], ID),
    ('tag_turbo', [pipe[0] - 1.0, pipe[1], pipe[2]], BACKWARD),
    ('tag_smoke', [front_x - 22.0, 0.0, 10.0], BACKWARD),               # the blower on the bonnet
    ('tag_hlite1', [hl[0], hl[1], hl[2]], ID), ('tag_hlite2', [hl[0], -hl[1], hl[2]], ID),
    ('tag_blite1', [tl[0], -tl[1], tl[2]], ID), ('tag_blite2', [tl[0], tl[1], tl[2]], ID),
    ('tag_rlite1', [tl[0], -tl[1], tl[2]], ID), ('tag_rlite2', [tl[0], tl[1], tl[2]], ID),
]
tags = [(n, [float(o[0]), float(o[1]), float(o[2]) + LIFT[2]], a) for n, o, a in tags]
ORDER = [n for n in ('body', 'parts') if n in surfs]
tags = snap_lights(tags, [surfs[n] for n in ORDER])
bmn, bmx, rad = save(os.path.join(OUT, 'body.md3'), P + 'body.md3', [surfs[n].done() for n in ORDER],
                     tags, b'bountyhunter')
print('scale %.4f  body bounds %s %s radius %.1f' % (S, [round(v, 1) for v in bmn], [round(v, 1) for v in bmx], rad))
for n, o, a in tags:
    print('  %-12s %s' % (n, [round(v, 2) for v in o]))

# ---------------- wheel: the shared rim selection ------------------------------------------------
# The source wheel is a low-poly cylinder with the rim painted on. The tyre is
# turned on a lathe from its outline (tread, the bevel on the outer side) with
# sidewalls down to a rim opening of RIM_OPEN x the radius; build_wheel maps the
# shared tread and rim textures onto it, so every rim of the game fits.
c, r, t = wheels['fl']
TIRE_SEG = 48
RIM_OPEN = 0.78


def to_wheel(p):
    # model x = out of the car (left wheel), y = forward, z = down (see WL)
    q = to_q3(p)
    return np.stack([(q[..., 1] - c[1]) * S, (q[..., 0] - c[0]) * S, -(q[..., 2] - c[2]) * S], -1)


wp = to_wheel(V[np.unique([x[0] for tr in t for x in tr])])
wx, wr = wp[:, 0], np.hypot(wp[:, 1], wp[:, 2])
XI, XO = wx.min(), wx.max()                                     # inner and outer face
tread_out = wx[np.isclose(wr, R, atol=1e-3)].max()              # where the bevel starts
bevel_r = wr[np.isclose(wx, XO, atol=1e-3)].max()
shoulder = (XO - XI) * 0.06
outline = [(XI, R * RIM_OPEN), (XI, R - shoulder), (XI + shoulder, R), (tread_out, R),
           (XO, bevel_r), (XO, R * RIM_OPEN)]
ang = 2 * np.pi * np.arange(TIRE_SEG) / TIRE_SEG
wv = np.array([(x, rr * np.cos(a_), rr * np.sin(a_)) for x, rr in outline for a_ in ang])
wf = []
for i in range(len(outline) - 1):
    for j in range(TIRE_SEG):
        a0, b0 = i * TIRE_SEG + j, i * TIRE_SEG + (j + 1) % TIRE_SEG
        wf += [(a0, b0 + TIRE_SEG, a0 + TIRE_SEG), (a0, b0, b0 + TIRE_SEG)]   # counter-clockwise seen from outside
wf = np.array(wf)
wn = mesh.corner_normals(wv, wf, 30.0)
tire = [([tuple(wv[i]) for i in face], [tuple(n) for n in wn[f]]) for f, face in enumerate(wf)]
open_out, open_in, wmn, wmx = build_wheel(tire, R, os.path.join(OUT, 'wheel.md3'), P + 'wheel.md3')
print('wheel radius %.2f, opening out %.2f in %.2f, bounds %s %s'
      % (R, open_out, open_in, [round(v, 2) for v in wmn], [round(v, 2) for v in wmx]))

# ---------------- source textures ----------------------------------------------------------------
Image.open(os.path.join(TEX_DIR, 'sferiffi.jpg')).convert('RGB').save(os.path.join(OUT, 'parts.tga'), compression='tga_rle')
