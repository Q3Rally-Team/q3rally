"""Build the Skyline body.md3 and wheel.md3 from the Sketchfab OBJ.

usage: build_model.py <r34.obj> <outdir>

Source: "Nissan Skyline R34 GT-R" by Lexyc16 (https://skfb.ly/6TxFX),
CC BY 4.0. The model is kept at full detail; only the main paint material
is split into four surfaces (front/rear x left/right) so every surface stays
below the engine's per-surface limit.

OBJ axes: x = width (+x is the car's left), y = up, z = length (+z front).
Q3 axes:  X = forward, Y = left, Z = up.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # r34/layout.py before ../layout.py
import obj
from carlib import BACKWARD, ID, WL, WR, Builder, build_wheel, save, snap_lights, tri_normal, wheel_tag_z
from layout import atlas, region, X0, X1, Y0, Y1, Z0, Z1

SRC, OUT = sys.argv[1], sys.argv[2]
P = 'models/players/skyline/'

LENGTH = 112.0            # world units, like the other cars
WHEEL_Z = -9.7            # wheel centre height the model is laid out at (paint atlas, decals)

# material -> surface name; None = dropped
SURF = {
    'Material.001': 'paint',
    'Material': 'hlights',       # headlights
    'Material.011': 'ilights',   # indicators and side markers
    'Material.009': 'rlights',   # round tail lights
    'Material.006': 'glass',
    'Material.002': 'trim',      # window seals, lip, plastic trim
    'Material.005': 'trim',      # wing blade, plate frame
    'Material.004': 'chrome',    # exhaust, plate frame details
    'Material.013': 'chrome',    # badges
    'Material.015': 'chrome',
    'Material.032': None,        # manufacturer emblem on the boot lid, left out
    'Material.008': 'grill',
    'Material.003': 'bottom',    # underbody, fuel lid
}
TEX = {'hlights': 'lights', 'ilights': 'lights', 'rlights': 'tlite', 'glass': 'glass', 'trim': 'trim',
       'chrome': 'chrome', 'grill': 'grill', 'bottom': 'under'}
OWN_UV = ('hlights', 'ilights', 'rlights', 'grill')   # keep the model's UVs for these
WHEEL_OBJECTS = ('Circle', 'Circle.001', 'Circle.002_Circle.004', 'Circle.003_Circle.006')
DROP_OBJECTS = ('Circle.004_Circle.008',)            # manufacturer emblem
WHEEL_SRC = 'Circle'                                  # front left
TIRE_MTL = 'Material.010'

V, VT, VN, G = obj.load(SRC)

# ---------------- placement -----------------------------------------------------
body_ids = {c[0] for (o, m), tris in G if o not in WHEEL_OBJECTS for t in tris for c in t}
mn = [min(V[i][k] for i in body_ids) for k in range(3)]
mx = [max(V[i][k] for i in body_ids) for k in range(3)]
S = LENGTH / (mx[2] - mn[2])
CX, CZ = (mn[0] + mx[0]) / 2, (mn[2] + mx[2]) / 2

tire = [V[c[0]] for (o, m), tris in G if o == WHEEL_SRC and m == TIRE_MTL for t in tris for c in t]
W_C = [(min(p[k] for p in tire) + max(p[k] for p in tire)) / 2 for k in range(3)]   # wheel centre (obj)
W_R = (max(p[1] for p in tire) - min(p[1] for p in tire)) / 2 * S                    # tyre radius (world)
CY = W_C[1] - WHEEL_Z / S
# the written model is raised so the tyres stand on the ground (carlib.wheel_tag_z)
LIFT = (0.0, 0.0, wheel_tag_z(W_R) - WHEEL_Z)


def to_q3(p):
    return ((p[2] - CZ) * S, (p[0] - CX) * S, (p[1] - CY) * S)


def nrm_q3(n):
    return (n[2], n[0], n[1])


def corner(c):
    return to_q3(V[c[0]]), nrm_q3(VN[c[2]]) if c[2] is not None else None, VT[c[1]] if c[1] is not None else (0, 0)


# ---------------- body ------------------------------------------------------------
surfs = {}


def surf(name, tex):
    if name not in surfs:
        surfs[name] = Builder(name, P + tex + '.tga', LIFT)
    return surfs[name]


box = {'X': (X0, X1), 'Y': (Y0, Y1), 'Z': (Z0, Z1)}


def box_uv(reg, p):
    """planar projection over the whole car for the shared trim textures"""
    (a0, a1), (b0, b1) = {'L': (box['X'], box['Z']), 'R': (box['X'], box['Z']), 'F': (box['Y'], box['Z']),
                          'B': (box['Y'], box['Z'])}.get(reg, (box['X'], box['Y']))
    i, j = {'L': (0, 2), 'R': (0, 2), 'F': (1, 2), 'B': (1, 2)}.get(reg, (0, 1))
    return [(p[i] - a0) / (a1 - a0), (b1 - p[j]) / (b1 - b0)]


for (o, m), tris in G:
    if o in WHEEL_OBJECTS or o in DROP_OBJECTS:
        continue
    kind = SURF.get(m)
    if kind is None:
        continue
    for t in tris:
        cs = [corner(c) for c in t]
        pts = [c[0] for c in cs]
        fn = tri_normal(pts)
        ns = [c[1] or fn for c in cs]
        if kind == 'paint':
            cx = sum(p[0] for p in pts) / 3
            cy = sum(p[1] for p in pts) / 3
            b = surf('body_%s%s' % ('f' if cx >= 0 else 'r', 'l' if cy >= 0 else 'r'), 'red_main')
            reg = region(fn)
            uvs = [atlas(reg, p) for p in pts]
        elif kind in OWN_UV:
            b = surf(kind, TEX[kind])
            uvs = [[c[2][0], 1 - c[2][1]] for c in cs]
        else:
            b = surf(kind, TEX[kind])
            reg = region(fn)
            uvs = [box_uv(reg, p) for p in pts]
        b.tri([b.vert(pts[k], ns[k], uvs[k]) for k in range(3)])

ORDER = ['body_fl', 'body_fr', 'body_rl', 'body_rr', 'glass', 'trim', 'chrome', 'grill',
         'hlights', 'ilights', 'rlights', 'bottom']

# ---------------- tags --------------------------------------------------------------
def centre(mtl, sel=lambda p: True):
    pts = [to_q3(V[c[0]]) for (o, m), tris in G if m == mtl and o not in WHEEL_OBJECTS
           for t in tris for c in t]
    pts = [p for p in pts if sel(p)]
    return [(min(p[k] for p in pts) + max(p[k] for p in pts)) / 2 for k in range(3)], pts


def wheel_pos(o):
    pts = [to_q3(V[c[0]]) for (oo, m), tris in G if oo == o and m == TIRE_MTL for t in tris for c in t]
    return [(min(p[k] for p in pts) + max(p[k] for p in pts)) / 2 for k in range(3)]


wheels = {o: wheel_pos(o) for o in WHEEL_OBJECTS}
front = [w for w in wheels.values() if w[0] > 0]
rear = [w for w in wheels.values() if w[0] < 0]
FX = sum(w[0] for w in front) / len(front)
RX = sum(w[0] for w in rear) / len(rear)
WY = sum(abs(w[1]) for w in wheels.values()) / 4
WZ = sum(w[2] for w in wheels.values()) / 4

# lights: per side centres of the lamp materials
hl, _ = centre('Material', lambda p: p[1] > 0)
_, tl = centre('Material.009')
tl_left = sorted({(round(p[1], 1)) for p in tl if p[1] > 0})
# the four round tail lamps: split the left ones at the gap between inner and outer lamp
lamps_l = [p for p in tl if p[1] > 0]
gap = max((b - a, (a + b) / 2) for a, b in zip(tl_left, tl_left[1:]))[1]
inner = [p for p in lamps_l if p[1] < gap]
outer = [p for p in lamps_l if p[1] >= gap]


def mid(pts):
    return [(min(p[k] for p in pts) + max(p[k] for p in pts)) / 2 for k in range(3)]


ti, to = mid(inner), mid(outer)
ex, _ = centre('Material.004', lambda p: p[0] < -40 and p[2] < -5)   # exhaust tip
roof = max(to_q3(V[i])[2] for i in body_ids)
front_x = max(to_q3(V[i])[0] for i in body_ids)
# rearmost point of the bumper face behind the plate (plate_usa is 9.7 x 4.7 units)
plate_x = min(p[0] for p in (to_q3(V[i]) for i in body_ids) if abs(p[1]) < 4.9 and -6.9 < p[2] < -2.1)

tags = [
    ('tag_wheelfl', [FX, WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX, WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon', [-8.0, 0.0, roof], ID),                        # roof
    ('tag_plate', [plate_x - 0.2, 0.0, -4.5], ID),                # just off the flat bumper face
    ('tag_turbo', [ex[0] - 1.0, ex[1], ex[2]], BACKWARD),         # exhaust
    ('tag_smoke', [front_x - 18.0, 0.0, 4.5], BACKWARD),          # hood
    ('tag_hlite1', [hl[0] + 0.5, hl[1], hl[2]], ID), ('tag_hlite2', [hl[0] + 0.5, -hl[1], hl[2]], ID),
    ('tag_blite1', [to[0] - 0.5, -to[1], to[2]], ID), ('tag_blite2', [to[0] - 0.5, to[1], to[2]], ID),
    ('tag_rlite1', [ti[0] - 0.5, -ti[1], ti[2]], ID), ('tag_rlite2', [ti[0] - 0.5, ti[1], ti[2]], ID),
]

tags = [(n, [o[0], o[1], o[2] + LIFT[2]], a) for n, o, a in tags]
tags = snap_lights(tags, [surfs[n] for n in ORDER if n in surfs])   # glow sprites in front of the lenses
bmn, bmx, rad = save(os.path.join(OUT, 'body.md3'), P + 'body.md3',
                     [surfs[n].done() for n in ORDER if n in surfs], tags, b'skyline')
print('scale %.2f  body bounds %s %s radius %.1f' % (S, [round(c, 1) for c in bmn], [round(c, 1) for c in bmx], rad))
for n, o, a in tags:
    print('  %-12s %s' % (n, [round(c, 2) for c in o]))

# ---------------- wheel -------------------------------------------------------------
# The R34 tyre at full detail. Its sidewalls and a dished disc in the rim opening
# get the planar mapping of the shared rim textures (w_rim), the tread the
# cylindrical mapping of tread.tga (w_tread), like the other cars' wheels, so the
# rim selection keeps working. The modelled 6-spoke rim is not used: the rim
# textures paint their own spokes.
def to_wheel(p):
    # model x = out of the car (left wheel), y = forward, z = down (see WL)
    return ((p[0] - W_C[0]) * S, (p[2] - W_C[2]) * S, -(p[1] - W_C[1]) * S)


def wn(n):
    return (n[0], n[2], -n[1])


tire = [([to_wheel(V[c[0]]) for c in t], [wn(VN[c[2]]) for c in t])
        for (o, m), tris in G if o == WHEEL_SRC and m == TIRE_MTL for t in tris]
open_out, open_in, wmn, wmx = build_wheel(tire, W_R, os.path.join(OUT, 'wheel.md3'), P + 'wheel.md3')
print('wheel radius %.2f, opening out %.2f in %.2f, bounds %s %s'
      % (W_R, open_out, open_in, [round(c, 2) for c in wmn], [round(c, 2) for c in wmx]))
