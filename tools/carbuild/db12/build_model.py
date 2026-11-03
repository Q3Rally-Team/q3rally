"""Build the DB12 body.md3 and wheel.md3 from the OBJ.

usage: build_model.py <Aston Martin DB12 2024.obj> <outdir> [cachedir]

The source (1,859,182 triangles, an Aston Martin DB12 from 2024) carries a
full interior, brakes and very detailed wheels. Only what can be seen from
outside is kept (cull.py; the result is cached in cachedir because it takes a
while), and that is decimated per material to about TARGET triangles with
the borders kept in place. The wheels are replaced by the shared rim
selection: the tyre is turned on a lathe from the source tyre's outline. The
source's textures are ambient-occlusion maps over flat colours: the paint
gets the projection atlas of layout.py, all other parts take their colour
(material colour or the flat colour texture) from a palette.

Requires pyfqmr (pip install pyfqmr).

OBJ axes: x = width (+x is the car's left), y = up, z = length (+z front), metres.
Q3 axes:  X = forward, Y = left, Z = up.
"""
import hashlib
import math
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # db12/layout.py before ../layout.py
import numpy as np
from PIL import Image
import cull
import mesh
import objnp
from carlib import BACKWARD, ID, WL, WR, Builder, build_wheel, save, snap_lights, tri_normal, wheel_tag_z
from layout import atlas, region, X0, X1, Y0, Y1, Z0, Z1

SRC, OUT = sys.argv[1], sys.argv[2]
CACHE = sys.argv[3] if len(sys.argv) > 3 else OUT
P = 'models/players/db12/'

LENGTH = 116.5            # world units; the GT500 is 117 for 4.8 m, the DB12 is 4.73 m
WHEEL_Z = -9.7            # wheel centre height the model is laid out at (paint atlas, decals)
TARGET = 85000            # body triangles after decimation; the two-sided parts add their backs on top
TIRE_SEG, TIRE_BINS = 64, 16   # tyre lathe: segments around, profile steps across the tread
KEEP_SMALL = 200          # parts below this many triangles are not decimated

PAINT = ('Material_#2',)                                   # the body colour
KIND = {
    'Material_#3': 'glass', 'Material_#56': 'glass',      # outer and inner glass
    'Material_#55': 'chrome',                              # mirror glass
    'Material_#5': 'hlights', 'ledlamp': 'hlights', 'led_dark': 'hlights',   # headlamps, LED strips
    'Material_#7': 'rlights', 'Material_#14': 'rlights',  # tail lamps, light bar across the tail
}
WHEEL_MATS = ('Material_#15', 'Material_#16', 'Material_#17', 'Material_#18')   # wheels and brakes
BADGES = ('Object053', 'Object054', 'UId13')              # wings on the nose, wings and lettering on the tail
PLATES = ('UId00', 'UId00.001', 'UId00.002', 'UId00.003')  # plates and their holders (the game draws its own)
TIRE = 'Material_#18'
LENSES = ('UId09',)              # one glass shell over all lamps: clear, so the lamps behind it count
INNER_GLASS = ('vidrio-interior',)   # the windows' inner face, a second layer behind the outer glass
JOBS = int(os.environ.get('CULL_JOBS', '2'))
# parts seen only through narrow openings (grille slats, the gills behind the front
# wheels, the hood vents, the lamps behind their lenses): culling misses them at its
# resolution and the openings would show the background, so they are kept whole
KEEP_ALL = ('UId18', 'UId06', 'UId41',                        # grille, its frame and what is behind it
            'UId07', 'UId60',                                  # gills behind the front wheels
            'UId15', 'UId27',                                  # vent meshes (hood, bumper)
            'UId03', 'UId42', 'UId43', 'UId44', 'UId45', 'UId46', 'UId57', 'UId58',   # headlamps
            'UId10', 'UId47', 'UId48', 'UId49', 'UId50', 'UId51')                     # tail lamps
# thin parts that are seen from both sides (grille slats, gill and vent blades): they
# get a second, reversed copy of every triangle with the normals turned round, so the
# back is lit like a back (cull none would light it with the front's normal, and
# undersides facing up would flash white)
TWO_SIDED = ('UId18', 'UId06', 'UId41', 'UId07', 'UId60', 'UId15', 'UId27',
             'UId52', 'UId54', 'UId55', 'UId56', 'UId04')     # diffuser, tail pipe surrounds, spoiler lip

# ---------------- load and cull --------------------------------------------------------
d = objnp.load(SRC, CACHE)
V, T = d['V'], d['T'].copy()
MATS, GROUPS_ = list(d['mats']), list(d['groups'])
MAT = np.array(MATS, dtype=object)[d['M']].astype(str)
OBJ = np.array(GROUPS_, dtype=object)[d['G']].astype(str)
gw = {g for g in set(OBJ) if set(MAT[OBJ == g]) <= set(WHEEL_MATS)}
DROP = np.isin(OBJ, sorted(gw))
BADGE = np.isin(OBJ, BADGES + PLATES + LENSES + INNER_GLASS)
# culled without badges, plates, lenses and the inner glass, so what is behind them stays, and without the
# wheels (750,000 triangles, replaced by the shared rims): the arches stay open as in game
scene = ~BADGE & ~DROP

key = hashlib.sha1(np.packbits(scene).tobytes() + V[:1000].tobytes()).hexdigest()[:16]   # changes with the culled set
cache = os.path.join(CACHE, 'db12_visible_%s.npz' % key)
if os.path.exists(cache):
    vis, facing = np.load(cache)['vis'], np.load(cache)['facing']
else:
    vis, facing = np.zeros(len(T), bool), np.zeros(len(T), np.int64)
    used = np.unique(T[scene])                # cull frames the car by its vertices
    cv, ct = V[used], np.searchsorted(used, T[scene])

    def job(k):
        return cull.visible_facing(cv, ct, views=set(range(k, 81, JOBS)), progress=lambda n, m, samples, seen: print(
            'visibility view %d/%d: %d samples, %d triangles seen' % (n, m, samples, seen), flush=True))
    if JOBS > 1:
        import multiprocessing
        with multiprocessing.get_context('fork').Pool(JOBS) as pool:
            parts = pool.map(job, range(JOBS))
    else:
        parts = [job(0)]
    vis[scene] = np.any([p[0] for p in parts], 0)
    facing[scene] = np.sum([p[1] for p in parts], 0)
    np.savez(cache, vis=vis, facing=facing)
flip = (facing < 0) & ~DROP
T[flip] = T[flip][:, ::-1]

# ---------------- placement ----------------------------------------------------------------
body = (vis | np.isin(OBJ, KEEP_ALL)) & ~DROP & ~BADGE
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
outline += [(x, tr[np.abs(tx - x) < step / 2].max()) for x in xs if (np.abs(tx - x) < step / 2).any()]   # grooves leave gaps
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
MTL = os.path.splitext(SRC)[0] + '.mtl'
TEXDIR = os.path.join(os.path.dirname(SRC), 'texture')
colour = {}
for blk in open(MTL, encoding='latin-1').read().split('newmtl ')[1:]:
    name = blk.split('\n')[0].strip()
    kd = re.search(r'\nKd ([^\n]+)', blk)
    colour[name] = [float(v) for v in kd.group(1).split()] if kd else [0.5] * 3
    tex = re.search(r'\nmap_Kd ([^\n]+)', blk)
    path = os.path.join(TEXDIR, tex.group(1).strip().replace('\\', '/').split('/')[-1]) if tex else ''
    if path.lower().endswith('.png') and os.path.exists(path):
        im = np.asarray(Image.open(path).convert('RGBA'), float)
        if im.shape[0] <= 256 and im[..., 3].min() > 0 and np.ptp(im[..., :3]) < 2:
            # a flat colour texture (the large ones are ambient occlusion with alpha 0)
            colour[name] = list((im[0, 0, :3] / 255) ** 2.2)
PAL_N = 8                                                 # 8 x 8 cells
# lamps get brighter colours than the dark source materials, so they read as lamps in game
# and the pure black materials a little grey, so their shape still shows
colour.update({'Material_#5': (0.95, 0.95, 0.92), 'ledlamp': (0.98, 0.98, 1.0), 'led_dark': (0.30, 0.30, 0.32),
               'Material_#7': (0.72, 0.02, 0.02), 'Material_#14': (0.60, 0.02, 0.02),
               'Material_#4': (0.025, 0.025, 0.028), 'Material_#9': (0.03, 0.03, 0.03),
               'Material_#8': (0.04, 0.04, 0.045), 'Material_#13': (0.05, 0.05, 0.055),
               'Material_#12': (0.03, 0.03, 0.032), 'Material_#11': (0.035, 0.035, 0.038)})   # diffuser, rear trim: black, not the AO-lit grey
palette = sorted({m for m in set(MAT[body]) if m not in PAINT})
cell = {m: i for i, m in enumerate(palette)}
assert len(palette) <= PAL_N * PAL_N


def pal_uv(m):
    i = cell[m]
    return [((i % PAL_N) + 0.5) / PAL_N, ((i // PAL_N) + 0.5) / PAL_N]


# ---------------- decimate and build the body surfaces -------------------------------------
total = int(body.sum())
PAINT_RATIO = 0.32        # the paint keeps about a third: coarser panels show as dents and holes
n_paint = int((body & np.isin(MAT, PAINT)).sum())
ratio = min(1.0, (TARGET - n_paint * PAINT_RATIO) / max(1, total - n_paint))
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
TS = np.isin(OBJ, TWO_SIDED)
for m in sorted(set(MAT[body])):
  for two in (False, True):
    sel = body & (MAT == m) & (TS == two)
    if not sel.any():
        continue
    verts, faces = mesh.weld(to_q3(V), T[sel])
    n0 = len(faces)
    if n0 > KEEP_SMALL:
        rt = PAINT_RATIO if m in PAINT else ratio   # (r is the tyre radius further down)
        verts, faces = mesh.decimate(verts, faces, max(KEEP_SMALL, round(n0 * rt)))
        goal = max(KEEP_SMALL, round(n0 * rt))
        if m not in PAINT and len(faces) > 1.5 * goal:
            # black trim: culling cut it into pieces whose borders cannot move; small
            # gaps along them do not show on black parts
            verts, faces = mesh.decimate(verts, faces, goal, keep_border=False)
    a0, b0 = counts.get(m, (0, 0))
    counts[m] = (a0 + n0, b0 + len(faces) * (2 if two else 1))
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
        if two:
            back = [b.vert(pts[k], tuple(-c for c in ns[k]), uvs[k]) for k in range(3)]
            b.tri([back[0], back[2], back[1]])

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

hl = mid(pts_of(['Material_#5'], lambda p: p[:, 1] > 0, only_visible=False))  # the left lamp unit
# tail lamps: the slim left unit, brake light on its outer half, reversing light on its inner half
tl = pts_of(['Material_#7'], lambda p: p[:, 1] > 0)
ymid = (tl[:, 1].min() + tl[:, 1].max()) / 2
to, ti = mid(tl[tl[:, 1] >= ymid]), mid(tl[tl[:, 1] < ymid])
# exhaust: the left pair of tail pipes
ex_p = to_q3(V[np.unique(T[OBJ == 'UId54'])])
ex_p = ex_p[ex_p[:, 1] > 0]
ex = np.array([ex_p[:, 0].min(), *mid(ex_p)[1:]])
# plate: where the source's rear plate sat, on the face behind it
pl = to_q3(V[np.unique(T[OBJ == 'UId00.003'])])
PLATE_Z = float(mid(pl)[2])
plate_x = pl[:, 0].min()

tags = [
    ('tag_wheelfl', [FX, WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX, WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon', [-8.0, 0.0, roof], ID),
    ('tag_plate', [plate_x, 0.0, PLATE_Z], ID),
    ('tag_turbo', [ex[0] - 0.5, ex[1], ex[2]], BACKWARD),
    ('tag_smoke', [front_x - 18.0, 0.0, 4.5], BACKWARD),
    ('tag_hlite1', [hl[0] + 0.5, hl[1], hl[2]], ID), ('tag_hlite2', [hl[0] + 0.5, -hl[1], hl[2]], ID),
    ('tag_blite1', [to[0] - 0.5, -to[1], to[2]], ID), ('tag_blite2', [to[0] - 0.5, to[1], to[2]], ID),
    ('tag_rlite1', [ti[0] - 0.5, -ti[1], ti[2]], ID), ('tag_rlite2', [ti[0] - 0.5, ti[1], ti[2]], ID),
]
tags = [(n, [float(o[0]), float(o[1]), float(o[2]) + LIFT[2]], a) for n, o, a in tags]
tags = snap_lights(tags, [surfs[n] for n in ORDER])   # glow sprites in front of the lenses
bmn, bmx, rad = save(os.path.join(OUT, 'body.md3'), P + 'body.md3',
                     [surfs[n].done() for n in ORDER], tags, b'db12')
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
