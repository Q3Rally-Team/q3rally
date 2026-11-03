"""Build the Ridgeback (lifted 1985 pickup) body.md3 and wheel.md3 from the FBX.

usage: build_model.py <krmlgtbdy.fbx> <source textures dir> <outdir> [cachedir]

Source: the project's "lightbody lifted 85" model (37,407 triangles, FBX
with textures). Only what can be seen from outside is kept (cull.py); the
model is small enough to keep that at full detail. The paint gets the
projection atlas of layout.py; chassis, bed, bull bar, roll bar, lamps and
interior keep their own UVs and textures.

FBX axes: x = length (+x front), y = up, z = width (-z is the car's left).
Q3 axes:  X = forward, Y = left, Z = up.
"""
import hashlib
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # ridgeback/layout.py before ../layout.py
import numpy as np
from PIL import Image
import cull
import fbx
import mesh
from carlib import BACKWARD, ID, WL, WR, Builder, build_wheel, save, snap_lights, tri_normal, wheel_tag_z
from layout import atlas, region, X0, X1, Y0, Y1, Z0, Z1

SRC, TEX_DIR, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
CACHE = sys.argv[4] if len(sys.argv) > 4 else OUT
P = 'models/players/ridgeback/'

LENGTH = 112.0            # world units, bull bar to rear bumper
WHEEL_Z = -9.7            # wheel centre height the model is laid out at (paint atlas, decals)
MAX_VERTS = 9000

PAINT = 'krmlgtbdy85_Bodymat'
TIRE = 'RO1a_Tire'
# source material -> (surface group, texture name, source texture file); None = dropped
PARTS = {
    'UCB_BOTTOM': ('chassis', 'chassis', 'UCB_BOTTOM_DIF.jpg'),
    'UCB_Interiors_1': ('interior', 'interior', 'UCB_Interiors_1_Dif.jpg'),
    'UCB_Lights_and_Glass': ('lights', 'lights', 'UCB_Lights_and_Glass_Dif.jpg'),
    'Numberplates_Misk_U': None,            # the game draws its own plate on tag_plate
    'Carbadges_misc_U': None,               # manufacturer badges
}
# the transparent material holds the windows and the lamp lenses
LENS_OBJECTS = ('krmlgtbdy85_Headlights_Glass', 'krmlgtbdy85_Taillights_Glass')
GLASS = 'UCB_Lights_and_Glass_Transperent'

# ---------------- load and cull ----------------------------------------------------------
V, VT, VN, G = fbx.load(SRC)
V = np.asarray(V, float)
T = np.array([[c[0] for c in t] for _, tris in G for t in tris], int)
TUV = [[c[1] for c in t] for _, tris in G for t in tris]
OBJ = np.array([o for (o, m), tris in G for _ in tris])
MAT = np.array([m for (o, m), tris in G for _ in tris])

key = hashlib.sha1(open(SRC, 'rb').read()).hexdigest()[:16]
cache = os.path.join(CACHE, 'ridgeback_visible_%s.npy' % key)
if os.path.exists(cache):
    vis = np.load(cache)
else:
    vis = cull.visible(V, T, progress=lambda n, k, samples, seen: print(
        'visibility view %d/%d: %d samples, %d triangles seen' % (n, k, samples, seen), flush=True))
    np.save(cache, vis)

dropped = [m for m, v in PARTS.items() if v is None] + [TIRE]
body = vis & ~np.isin(MAT, dropped)


def to_q3(p):
    p = np.asarray(p)
    return np.stack([p[..., 0], -p[..., 2], p[..., 1]], -1)


# ---------------- placement ----------------------------------------------------------------
bp = to_q3(V[np.unique(T[body])])
mn, mx = bp.min(0), bp.max(0)
S = LENGTH / (mx[0] - mn[0])
CX = (mn[0] + mx[0]) / 2

wheels = {}
for name in ('FL', 'FR', 'RL', 'RR'):
    sel = (OBJ == 'krmlgtbdy85_WheelStock_' + name) & (MAT == TIRE)
    p = to_q3(V[np.unique(T[sel])])
    wheels[name.lower()] = ((p.min(0) + p.max(0)) / 2, (p.max(0)[2] - p.min(0)[2]) / 2, T[sel])
CZ = np.mean([w[0][2] for w in wheels.values()]) - WHEEL_Z / S


def world(p):
    q = to_q3(p)
    return np.stack([(q[..., 0] - CX) * S, q[..., 1] * S, (q[..., 2] - CZ) * S], -1)


# tyre for wheel.md3: the front left one, at full detail
c, r, tt = wheels['fl']


def to_wheel(p):
    # model x = out of the car (left wheel), y = forward, z = down (see WL)
    q = to_q3(p)
    return np.stack([(q[..., 1] - c[1]) * S, (q[..., 0] - c[0]) * S, -(q[..., 2] - c[2]) * S], -1)


wv, wf = mesh.weld(to_wheel(V), tt)
LIFT = (0.0, 0.0, wheel_tag_z(np.hypot(wv[:, 1], wv[:, 2]).max()) - WHEEL_Z)

# ---------------- surfaces --------------------------------------------------------------------
surfs, current = {}, {}


def surf(group, tex):
    """builder of a surface group; a full surface continues as group2, group3, ..."""
    name = current.get(group, group)
    b = surfs.get(name)
    if b is not None and (len(b.verts) > MAX_VERTS or len(b.s.tris) > 19000):
        n = 2
        while '%s%d' % (group, n) in surfs:
            n += 1
        name, b = '%s%d' % (group, n), None
    if b is None:
        b = surfs[name] = Builder(name, P + tex + '.tga', LIFT)
        current[group] = name
    return b


box = {'X': (X0, X1), 'Y': (Y0, Y1), 'Z': (Z0, Z1)}


def box_uv(reg, p):
    """planar projection over the whole car for the glass texture"""
    (a0, a1), (b0, b1) = {'L': (box['X'], box['Z']), 'R': (box['X'], box['Z']), 'F': (box['Y'], box['Z']),
                          'B': (box['Y'], box['Z'])}.get(reg, (box['X'], box['Y']))
    i, j = {'L': (0, 2), 'R': (0, 2), 'F': (1, 2), 'B': (1, 2)}.get(reg, (0, 1))
    return [(p[i] - a0) / (a1 - a0), (b1 - p[j]) / (b1 - b0)]


WP = world(V)
idx = np.nonzero(body)[0]
# smooth normals per material over the welded mesh
corner_n = {}
for m in sorted(set(MAT[idx])):
    sel = idx[MAT[idx] == m]
    verts, faces, kept = mesh.weld(WP, T[sel], keep_mask=True)
    for t, n in zip(sel[kept], mesh.corner_normals(verts, faces)):
        corner_n[t] = n

ORDER_GROUPS = ['body_fl', 'body_fr', 'body_rl', 'body_rr', 'glass', 'chassis', 'interior', 'lights']
for t in idx:
    if t not in corner_n:
        continue
    m, o = MAT[t], OBJ[t]
    pts = [tuple(WP[v]) for v in T[t]]
    fn = tri_normal(pts)
    ns = [tuple(n) for n in corner_n[t]]
    if m == PAINT:
        cx = sum(p[0] for p in pts) / 3
        cy = sum(p[1] for p in pts) / 3
        b = surf('body_%s%s' % ('f' if cx >= 0 else 'r', 'l' if cy >= 0 else 'r'), 'red_main')
        reg = region(fn)
        uvs = [atlas(reg, p) for p in pts]
    elif m == GLASS and o not in LENS_OBJECTS:
        b = surf('glass', 'glass')
        uvs = [box_uv(region(fn), p) for p in pts]
    else:
        group, tex, _ = PARTS.get(m) or PARTS['UCB_Lights_and_Glass']   # lenses use the lamp texture
        b = surf(group, tex)
        uvs = [[VT[i][0], 1 - VT[i][1]] if i is not None else [0.5, 0.5] for i in TUV[t]]
    b.tri([b.vert(pts[k], ns[k], uvs[k]) for k in range(3)])

ORDER = sorted(surfs, key=lambda n: (ORDER_GROUPS.index(n.rstrip('0123456789')), n))

# ---------------- source textures ------------------------------------------------------------
for m, part in PARTS.items():
    if part:
        im = Image.open(os.path.join(TEX_DIR, part[2])).convert('RGB').resize((512, 512), Image.LANCZOS)
        im.save(os.path.join(OUT, part[1] + '.tga'), compression='tga_rle')

# ---------------- tags -------------------------------------------------------------------------
def pts_obj(name, sel=lambda p: np.ones(len(p), bool)):
    p = world(V[np.unique(T[OBJ == name])])
    return p[sel(p)]


def mid(p):
    return (p.min(0) + p.max(0)) / 2


wq = {k: world(V[np.unique(w[2])]) for k, w in wheels.items()}
wc = {k: mid(p) for k, p in wq.items()}
FX = (wc['fl'][0] + wc['fr'][0]) / 2
RX = (wc['rl'][0] + wc['rr'][0]) / 2
WY = np.mean([abs(w[1]) for w in wc.values()])
WZ = np.mean([w[2] for w in wc.values()])
allp = world(V[np.unique(T[body])])
cab_roof = world(V[np.unique(T[OBJ == 'krmlgtbdy85_Body'])])[:, 2].max()
front_x, rear_x = allp[:, 0].max(), allp[:, 0].min()

hl = mid(pts_obj('krmlgtbdy85_Headlights', lambda p: p[:, 1] > 0))
bl = mid(pts_obj('krmlgtbdy85_Brakelights', lambda p: p[:, 1] > 0))
rv = mid(pts_obj('krmlgtbdy85_Reverse', lambda p: p[:, 1] > 0))
plate = mid(pts_obj('krmlgtbdy85_Numberplates_Rear'))
# the outer lamps of the roll bar light bar: the highest parts of Extra_A, left and right end
bar = pts_obj('krmlgtbdy85_Extra_A')
top = bar[bar[:, 2] > bar[:, 2].max() - 3.0]
lamp_l = mid(top[top[:, 1] > top[:, 1].max() - 3.0])

tags = [
    ('tag_wheelfl', [FX, WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX, WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon', [0.0, 0.0, cab_roof], ID),
    ('tag_plate', [plate[0] - 0.2, plate[1], plate[2]], ID),
    ('tag_turbo', [rear_x + 2.0, -12.0, WZ - 1.0], BACKWARD),           # under the rear bumper
    ('tag_smoke', [front_x - 16.0, 0.0, 6.0], BACKWARD),
    ('tag_hlite1', [hl[0] + 0.5, hl[1], hl[2]], ID), ('tag_hlite2', [hl[0] + 0.5, -hl[1], hl[2]], ID),
    ('tag_hlite3', [lamp_l[0] + 1.0, lamp_l[1], lamp_l[2]], ID),        # light bar on the roll bar
    ('tag_hlite4', [lamp_l[0] + 1.0, -lamp_l[1], lamp_l[2]], ID),
    ('tag_blite1', [bl[0] - 0.5, -bl[1], bl[2]], ID), ('tag_blite2', [bl[0] - 0.5, bl[1], bl[2]], ID),
    ('tag_rlite1', [rv[0] - 0.5, -rv[1], rv[2]], ID), ('tag_rlite2', [rv[0] - 0.5, rv[1], rv[2]], ID),
]
tags = [(n, [float(o[0]), float(o[1]), float(o[2]) + LIFT[2]], a) for n, o, a in tags]
tags = snap_lights(tags, [surfs[n] for n in ORDER])   # glow sprites in front of the lenses
bmn, bmx, rad = save(os.path.join(OUT, 'body.md3'), P + 'body.md3',
                     [surfs[n].done() for n in ORDER], tags, b'ridgeback')
print('scale %.2f  body bounds %s %s radius %.1f' % (S, [round(v, 1) for v in bmn], [round(v, 1) for v in bmx], rad))
print('visible %d of %d triangles (without tyres and dropped parts: %d)' % (vis.sum(), len(T), body.sum()))
for n, o, a in tags:
    print('  %-12s %s' % (n, [round(v, 2) for v in o]))

# ---------------- wheel ---------------------------------------------------------------------------
wn = mesh.corner_normals(wv, wf, 30.0)
tire = [([tuple(wv[i]) for i in face], [tuple(n) for n in wn[f]]) for f, face in enumerate(wf)]
open_out, open_in, wmn, wmx = build_wheel(tire, r * S, os.path.join(OUT, 'wheel.md3'), P + 'wheel.md3')
print('wheel radius %.2f, opening out %.2f in %.2f, bounds %s %s'
      % (r * S, open_out, open_in, [round(v, 2) for v in wmn], [round(v, 2) for v in wmx]))
