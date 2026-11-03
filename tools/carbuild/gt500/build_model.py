"""Build the GT500 body.md3 and wheel.md3 from the FBX.

usage: build_model.py <upload.fbx> <outdir> [cachedir]

The source (348,009 triangles, a Shelby GT500 with roll cage and supercharger)
carries a full interior, engine and suspension. Only what
can be seen from outside is kept (cull.py; the result is cached in cachedir
because it takes a while). More than TARGET visible triangles would be
decimated per material with the borders kept in place; this model stays
below it. The tyre is turned on a lathe from the source tyre's outline. The source has no textures: the paint gets the projection atlas
of layout.py, all other parts take their material colour from a palette.

Requires pyfqmr (pip install pyfqmr).

FBX axes: x = width (+x is the car's left), y = up, z = length (+z front).
Q3 axes:  X = forward, Y = left, Z = up.
"""
import hashlib
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # gtr/layout.py before ../layout.py
import numpy as np
import cull
import fbx
import mesh
from carlib import BACKWARD, ID, WL, WR, Builder, build_wheel, save, snap_lights, tri_normal, wheel_tag_z
from layout import atlas, region, X0, X1, Y0, Y1, Z0, Z1

SRC, OUT = sys.argv[1], sys.argv[2]
CACHE = sys.argv[3] if len(sys.argv) > 3 else OUT
P = 'models/players/gt500/'

LENGTH = 117.0            # world units; the Skyline is 112 for a 4.6 m car, the GT500 is 4.8 m
WHEEL_Z = -9.7            # wheel centre height the model is laid out at (paint atlas, decals)
TARGET = 100000           # body triangles after decimation
TIRE_SEG, TIRE_BINS = 64, 16   # tyre lathe: segments around, profile steps across the tread
KEEP_SMALL = 200          # parts below this many triangles are not decimated

PAINT = ('carpaint', 'carpaint.001')   # body colour and the source's stripes: both get the atlas
KIND = {
    'front_glass': 'glass',
    'mirror': 'chrome',
    'light': 'hlights', 'tinted_glass': 'hlights',                   # headlamps and their lenses
    'rearlight': 'rlights', 'orange': 'rlights',                     # tail lamps, indicators
}
DROP_OBJ = ('Plane', 'GT500.Wheel.', 'GT500.WheelBrake.')            # ground plane, wheels and brakes
BADGES = ('GT500.Badge.',)                                            # snake badges on the front wings
TIRE = 'tire'

# ---------------- load and cull --------------------------------------------------------
V, VT, VN, G = fbx.load(SRC)
V = np.asarray(V, float)
T = np.array([[c[0] for c in t] for _, tris in G for t in tris], int)
MAT = np.array([m for (o, m), tris in G for _ in tris])
OBJ = np.array([o for (o, m), tris in G for _ in tris])
DROP = np.array([o.startswith(DROP_OBJ) for o in OBJ])
# manufacturer badges and lettering: the wing badges, the GT500 badge in the grille
# (white_gloss), the six raised letters on the front splitter and the snake in the
# middle of the grille
lo, hi = V[T].min(1), V[T].max(1)
BADGE = (np.array([o.startswith(BADGES) for o in OBJ]) | (MAT == 'white_gloss')
         | ((OBJ == 'GT500.Bumper.Ft_carpaint_0') & (MAT == 'carpaint') & (lo[:, 1] > 23.7) & (hi[:, 1] < 26.1)
            & (lo[:, 2] > 367.5) & (np.abs(V[T][:, :, 0]).max(1) < 43))
         | ((OBJ == 'GT500.Bumper.Ft_glossblack_0') & (lo[:, 0] > -6) & (hi[:, 0] < 7) & (lo[:, 1] > 79)
            & (hi[:, 1] < 99.5) & (lo[:, 2] > 355.7) & (hi[:, 2] < 359.1)))
scene = ~np.char.startswith(OBJ.astype(str), 'Plane')             # the ground plane would hide the underside

key = hashlib.sha1(open(SRC, 'rb').read()).hexdigest()[:16]
cache = os.path.join(CACHE, 'gt500_visible_%s.npz' % key)
if os.path.exists(cache):
    vis, facing = np.load(cache)['vis'], np.load(cache)['facing']
else:
    vis, facing = np.zeros(len(T), bool), np.zeros(len(T), np.int64)
    used = np.unique(T[scene])                # cull frames the car by its vertices
    vis[scene], facing[scene] = cull.visible_facing(
        V[used], np.searchsorted(used, T[scene]), progress=lambda n, k, samples, seen: print(
            'visibility view %d/%d: %d samples, %d triangles seen' % (n, k, samples, seen), flush=True))
    np.savez(cache, vis=vis, facing=facing)
# some panels (the top of the front wings) are wound inside out in the source:
# turn the body triangles that are seen from their back more than from their front
# (the wheels are not used; the right ones are mirrored in the source)
flip = (facing < 0) & ~DROP
T[flip] = T[flip][:, ::-1]

# ---------------- placement ----------------------------------------------------------------
body = vis & ~DROP & ~BADGE
bp = V[np.unique(T[body])]
mn, mx = bp.min(0), bp.max(0)
S = LENGTH / (mx[2] - mn[2])
CX, CZ = (mn[0] + mx[0]) / 2, (mn[2] + mx[2]) / 2

tire_t = T[(MAT == TIRE) & DROP]
tc = V[tire_t].mean(1)                                   # triangle centres
left, front = tc[:, 0] > CX, tc[:, 2] > CZ
wheels = {}
for name, sel in (('fl', left & front), ('fr', ~left & front), ('rl', left & ~front), ('rr', ~left & ~front)):
    p = V[np.unique(tire_t[sel])]
    wheels[name] = ((p.min(0) + p.max(0)) / 2, (p.max(0)[1] - p.min(0)[1]) / 2, tire_t[sel])
CY = np.mean([w[0][1] for w in wheels.values()]) - WHEEL_Z / S

# tyre for wheel.md3: the front left one (built at the end)
c, r, tt = wheels['fl']


def to_wheel(p):
    # model x = out of the car (left wheel), y = forward, z = down (see WL)
    return np.stack([(p[..., 0] - c[0]) * S, (p[..., 2] - c[2]) * S, -(p[..., 1] - c[1]) * S], -1)


# Its tread is made of separate blocks that do not decimate, so the tyre is
# turned on a lathe from its outline instead: the outer radius across the
# tread, the sidewalls down to the rim. The tread pattern comes from tread.tga.
tp = to_wheel(V[np.unique(tt)])
tx, tr = tp[:, 0], np.hypot(tp[:, 1], tp[:, 2])
xs = np.linspace(tx.min(), tx.max(), TIRE_BINS + 1)
step = xs[1] - xs[0]
outline = [(xs[0], tr[tx < xs[0] + step / 2].min())]
outline += [(x, tr[np.abs(tx - x) < step / 2].max()) for x in xs]
outline += [(xs[-1], tr[tx > xs[-1] - step / 2].min())]
ang = 2 * np.pi * np.arange(TIRE_SEG) / TIRE_SEG
wv = np.array([(x, r * np.cos(a), r * np.sin(a)) for x, r in outline for a in ang])
wf = []
for i in range(len(outline) - 1):
    for j in range(TIRE_SEG):
        a, b = i * TIRE_SEG + j, i * TIRE_SEG + (j + 1) % TIRE_SEG
        c, d = a + TIRE_SEG, b + TIRE_SEG
        wf += [(a, d, c), (a, b, d)]                   # counter-clockwise seen from outside
wf = np.array(wf)
# the written model is raised so the tyres stand on the ground (carlib.wheel_tag_z);
# measured on the lathe tyre
LIFT = (0.0, 0.0, wheel_tag_z(np.hypot(wv[:, 1], wv[:, 2]).max()) - WHEEL_Z)


def to_q3(p):
    p = np.asarray(p)
    return np.stack([(p[..., 2] - CZ) * S, (p[..., 0] - CX) * S, (p[..., 1] - CY) * S], -1)


# ---------------- palette for the parts without a paint atlas ------------------------------
root = fbx.parse(SRC)
colour = {}
for c in root.find('Objects').all('Material'):
    p = fbx.props70(c)
    colour[c.props[1].split('\x00')[0]] = p.get('DiffuseColor', [0.5, 0.5, 0.5])[:3]
PAL_N = 8                                                 # 8 x 8 cells
# lamps get brighter colours than the dark source materials, so they read as lamps in game
# and the pure black materials a little grey, so their shape still shows
colour.update({'tinted_glass': (0.86, 0.88, 0.92), 'light': (0.95, 0.95, 0.92),
               'rearlight': (0.72, 0.02, 0.02), 'orange': (1.0, 0.36, 0.02),
               'glossblack': (0.025, 0.025, 0.028), 'leather': (0.05, 0.045, 0.04),
               'seat': (0.06, 0.06, 0.065), 'roll': (0.12, 0.12, 0.13), 'plate': (0.16, 0.16, 0.17),
               'lambert1': (0.03, 0.03, 0.03)})
palette = sorted({m for m in set(MAT[body]) if m not in PAINT})
cell = {m: i for i, m in enumerate(palette)}
assert len(palette) <= PAL_N * PAL_N


def pal_uv(m):
    i = cell[m]
    return [((i % PAL_N) + 0.5) / PAL_N, ((i // PAL_N) + 0.5) / PAL_N]


# ---------------- decimate and build the body surfaces -------------------------------------
total = int(body.sum())
ratio = min(1.0, TARGET / total)
surfs = {}                # surface name -> builder, in creation order
current = {}              # group name -> name of the surface being filled
MAX_VERTS = 9000          # start the next surface of a group well below the engine limit


def surf(group, tex):
    """builder of a surface group; a full surface continues as group2, group3, ..."""
    name = current.get(group, group)
    b = surfs.get(name)
    if b is not None and (len(b.verts) > MAX_VERTS or len(b.s.tris) > 19000):
        n = 2
        while '%s%d' % (group, n) in surfs:
            n += 1
        name = '%s%d' % (group, n)
        b = None
    if b is None:
        b = surfs[name] = Builder(name, P + tex + '.tga', LIFT)
        current[group] = name
    return b


box = {'X': (X0, X1), 'Y': (Y0, Y1), 'Z': (Z0, Z1)}


def box_uv(reg, p):
    """planar projection over the whole car for the shared glass/chrome textures"""
    (a0, a1), (b0, b1) = {'L': (box['X'], box['Z']), 'R': (box['X'], box['Z']), 'F': (box['Y'], box['Z']),
                          'B': (box['Y'], box['Z'])}.get(reg, (box['X'], box['Y']))
    i, j = {'L': (0, 2), 'R': (0, 2), 'F': (1, 2), 'B': (1, 2)}.get(reg, (0, 1))
    return [(p[i] - a0) / (a1 - a0), (b1 - p[j]) / (b1 - b0)]


counts = {}
for m in sorted(set(MAT[body])):
    sel = body & (MAT == m)
    verts, faces = mesh.weld(to_q3(V), T[sel])
    n0 = len(faces)
    if n0 > KEEP_SMALL:
        verts, faces = mesh.decimate(verts, faces, max(KEEP_SMALL, round(n0 * ratio)))
    counts[m] = (n0, len(faces))
    cn = mesh.corner_normals(verts, faces)
    kind = 'paint' if m in PAINT else KIND.get(m, 'parts')
    for f, face in enumerate(faces):
        pts = [tuple(verts[i]) for i in face]
        fn = tri_normal(pts)
        if fn == (0.0, 0.0, 0.0) or not all(map(math.isfinite, fn)):
            continue
        ns = [tuple(n) for n in cn[f]]
        if kind == 'paint':
            cx = sum(p[0] for p in pts) / 3
            cy = sum(p[1] for p in pts) / 3
            b = surf('body_%s%s' % ('f' if cx >= 0 else 'r', 'l' if cy >= 0 else 'r'), 'red_main')
            reg = region(fn)
            uvs = [atlas(reg, p) for p in pts]
        elif kind in ('glass', 'chrome'):
            b = surf(kind, kind)
            reg = region(fn)
            uvs = [box_uv(reg, p) for p in pts]
        else:
            b = surf(kind, 'parts')
            uvs = [pal_uv(m)] * 3
        b.tri([b.vert(pts[k], ns[k], uvs[k]) for k in range(3)])

GROUPS = ['body_fl', 'body_fr', 'body_rl', 'body_rr', 'glass', 'chrome', 'parts', 'hlights', 'rlights']
ORDER = sorted(surfs, key=lambda n: (GROUPS.index(n.rstrip('0123456789')), n))

# ---------------- tags --------------------------------------------------------------------------
def pts_of(mats, sel=lambda p: np.ones(len(p), bool), only_visible=True):
    p = to_q3(V[np.unique(T[(body if only_visible else True) & np.isin(MAT, mats)])])
    return p[sel(p)]


def mid(p):
    return (p.min(0) + p.max(0)) / 2


wq = {k: to_q3(w[0]) for k, w in wheels.items()}
FX = (wq['fl'][0] + wq['fr'][0]) / 2
RX = (wq['rl'][0] + wq['rr'][0]) / 2
WY = np.mean([abs(w[1]) for w in wq.values()])
WZ = np.mean([w[2] for w in wq.values()])
allp = to_q3(bp)
roof = allp[:, 2].max()
front_x, rear_x = allp[:, 0].max(), allp[:, 0].min()

hl = mid(pts_of(['light'], lambda p: p[:, 1] > 0, only_visible=False))       # lamp behind the lens
# tail lamps: the wide left unit (not the high centre brake light), brake light on
# its outer half, reversing light on its inner half
tl = pts_of(['rearlight'], lambda p: (p[:, 1] > 0) & (p[:, 2] < 8.0))
ymid = (tl[:, 1].min() + tl[:, 1].max()) / 2
to, ti = mid(tl[tl[:, 1] >= ymid]), mid(tl[tl[:, 1] < ymid])
# exhaust: the left tail pipe
ex_p = to_q3(V[np.unique(T[np.char.startswith(OBJ.astype(str), 'GT500.ExhaustTip.L')])])
ex = np.array([ex_p[:, 0].min(), *mid(ex_p)[1:]])
# plate: on the flat rear face at the centre of the car
PLATE_Z = float(os.environ.get('GT500_PLATE_Z', '-4.0'))
face = allp[(np.abs(allp[:, 1]) < 4.9) & (np.abs(allp[:, 2] - PLATE_Z) < 2.4)]
plate_x = face[:, 0].min()

tags = [
    ('tag_wheelfl', [FX, WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX, WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon', [-8.0, 0.0, roof], ID),
    ('tag_plate', [plate_x - 0.2, 0.0, PLATE_Z], ID),
    ('tag_turbo', [ex[0] - 0.5, ex[1], ex[2]], BACKWARD),
    ('tag_smoke', [front_x - 18.0, 0.0, 4.5], BACKWARD),
    ('tag_hlite1', [hl[0] + 0.5, hl[1], hl[2]], ID), ('tag_hlite2', [hl[0] + 0.5, -hl[1], hl[2]], ID),
    ('tag_blite1', [to[0] - 0.5, -to[1], to[2]], ID), ('tag_blite2', [to[0] - 0.5, to[1], to[2]], ID),
    ('tag_rlite1', [ti[0] - 0.5, -ti[1], ti[2]], ID), ('tag_rlite2', [ti[0] - 0.5, ti[1], ti[2]], ID),
]
tags = [(n, [float(o[0]), float(o[1]), float(o[2]) + LIFT[2]], a) for n, o, a in tags]
tags = snap_lights(tags, [surfs[n] for n in ORDER])   # glow sprites in front of the lenses
bmn, bmx, rad = save(os.path.join(OUT, 'body.md3'), P + 'body.md3',
                     [surfs[n].done() for n in ORDER], tags, b'gt500')
print('scale %.2f  body bounds %s %s radius %.1f' % (S, [round(c, 1) for c in bmn], [round(c, 1) for c in bmx], rad))
print('visible %d of %d triangles, decimated to %d' % (total, len(T), sum(c[1] for c in counts.values())))
for m, (a, b) in sorted(counts.items(), key=lambda x: -x[1][0]):
    print('  %-28s %6d -> %6d' % (m, a, b))
for n, o, a in tags:
    print('  %-12s %s' % (n, [round(c, 2) for c in o]))

# ---------------- palette texture ----------------------------------------------------------------
from PIL import Image
pal = np.zeros((PAL_N * 8, PAL_N * 8, 3), np.uint8)
for m, i in cell.items():
    c = [min(255, int(round(255 * (max(0.0, v) ** (1 / 2.2))))) for v in colour.get(m, [0.5] * 3)]
    pal[(i // PAL_N) * 8:(i // PAL_N) * 8 + 8, (i % PAL_N) * 8:(i % PAL_N) * 8 + 8] = c
Image.fromarray(pal).save(os.path.join(OUT, 'parts.tga'), compression='tga_rle')

# ---------------- wheel ---------------------------------------------------------------------------
wn = mesh.corner_normals(wv, wf, 30.0)
tire = [([tuple(wv[i]) for i in face], [tuple(n) for n in wn[f]]) for f, face in enumerate(wf)]
open_out, open_in, wmn, wmx = build_wheel(tire, r * S, os.path.join(OUT, 'wheel.md3'), P + 'wheel.md3')
print('wheel radius %.2f, opening out %.2f in %.2f, bounds %s %s'
      % (r * S, open_out, open_in, [round(v, 2) for v in wmn], [round(v, 2) for v in wmx]))
