"""Rebuild the Mustang body/wheel MD3 for Q3Rally.
usage: build_model.py <src body.md3> <src wheel.md3> <outdir>"""
import os, sys, math, copy
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import md3
SRC, WSRC, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
P = 'models/players/mustang/'
DX = -626            # shift in 1/64 units: centres the body on the origin (-9.78 units)

from layout import *
def paint_region(n):
    nx, ny, nz = n
    ax = max(abs(nx), abs(ny), abs(nz))
    if nz < -0.55: return 'U'
    if ax == abs(nz): return 'T'
    if ax == abs(ny): return 'L' if ny > 0 else 'R'
    return 'F' if nx > 0 else 'B'

def verts(s):
    return [[c/64 for c in v[:3]] for v in s.frames[0]]
def tri_normal(s, t):
    # averaged stored vertex normals (geometric winding is opposite in md3)
    n = [sum(md3.dec_normal(s.frames[0][i][3])[k] for i in t) for k in range(3)]
    l = math.sqrt(sum(c*c for c in n)) or 1
    return [c/l for c in n]

def remap(s, key_and_uv):
    """key_and_uv(tri) -> (key, fn(vertexpos)->uv). Splits shared verts per key."""
    V = verts(s); newv = []; newst = []; idx = {}; tris = []
    for t in s.tris:
        key, fn = key_and_uv(t)
        nt = []
        for i in t:
            k = (i, key)
            if k not in idx:
                idx[k] = len(newv); newv.append(s.frames[0][i]); newst.append(fn(V[i]))
            nt.append(idx[k])
        tris.append(nt)
    s.frames = [newv]; s.st = newst; s.tris = tris

def cluster_box(s, axis_u, axis_v, flip_u=False):
    """per side (y>0 / y<0) bounding box normalisation -> uv 0..1"""
    V = verts(s); boxes = {}
    for p in V:
        side = p[1] > 0
        b = boxes.setdefault(side, [1e9, -1e9, 1e9, -1e9])
        b[0] = min(b[0], p[axis_u]); b[1] = max(b[1], p[axis_u]); b[2] = min(b[2], p[axis_v]); b[3] = max(b[3], p[axis_v])
    def uv(p):
        b = boxes[p[1] > 0]
        u = (p[axis_u]-b[0])/((b[1]-b[0]) or 1); v = 1-(p[axis_v]-b[2])/((b[3]-b[2]) or 1)
        if flip_u and p[1] < 0: u = 1-u
        return [u, v]
    return uv

m = md3.load(SRC)
m.name = P + 'body.md3'
m.frames = m.frames[:1]; m.tags = m.tags[:1]
for s in m.surfs:
    s.frames = s.frames[:1]
    for v in s.frames[0]: v[0] += DX

RENAME = {'tlights': 'ilights'}   # front indicators, not tail lights
TEX = {'glass': 'glass', 'bumpers': 'chrome', 'body': 'red_main', 'roof': 'red_main', 'ldoor': 'red_main',
       'rdoor': 'red_main', 'hood': 'red_main', 'ilights': 'ilite', 'hlights': 'hlite', 'rlights': 'tlite', 'bottom': 'under'}
for s in m.surfs:
    s.name = RENAME.get(s.name, s.name)
    s.shaders = [(P + TEX[s.name] + '.tga', 0)]
    if TEX[s.name] == 'red_main':
        def gen(t, s=s):
            r = paint_region(tri_normal(s, t))
            return r, lambda p: atlas(r, p)
        remap(s, gen)
    elif s.name in ('glass', 'bumpers', 'bottom'):
        def gen(t, s=s):
            r = paint_region(tri_normal(s, t))
            if r in ('L', 'R'): return r, lambda p: [(p[0]-X0)/(X1-X0), (Z1-p[2])/(Z1-Z0)]
            if r in ('F', 'B'): return r, lambda p: [(p[1]-Y0)/(Y1-Y0), (Z1-p[2])/(Z1-Z0)]
            return r, lambda p: [(p[0]-X0)/(X1-X0), (Y1-p[1])/(Y1-Y0)]
        remap(s, gen)
    elif s.name == 'hlights':
        fn = cluster_box(s, 1, 2); remap(s, lambda t: (0, fn))
    elif s.name == 'rlights':
        fn = cluster_box(s, 1, 2, flip_u=True); remap(s, lambda t: (0, fn))
    else:
        fn = cluster_box(s, 1, 2); remap(s, lambda t: (0, fn))

# ---------------- tags ------------------------------------------------------
ID = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
BACKWARD = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]          # same as the other cars' turbo/smoke tags
WL = [[0, 1, 0], [1, 0, 0], [0, 0, -1]]                  # left wheels  (alpine convention)
WR = [[0, -1, 0], [-1, 0, 0], [0, 0, -1]]                # right wheels
FX, RX = 44.15 + DX/64, -21.65 + DX/64                   # wheel arch centres measured from the mesh
WY, WZ = 18.25, -9.6
tags = [
    ('tag_wheelfl', [FX,  WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX,  WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon',  [-10.5, 0.0, 15.0], ID),            # roof centre
    ('tag_plate',   [-54.35, 0.0, -6.3], ID),            # rear plate recess
    ('tag_turbo',   [-56.0, 0.0, -9.0], BACKWARD),       # below rear valance (exhaust)
    ('tag_smoke',   [36.0, 0.0, 5.4], BACKWARD),         # hood
    ('tag_hlite1',  [49.6,  17.85, -1.85], ID), ('tag_hlite2', [49.6, -17.85, -1.85], ID),   # headlight lens centres
    ('tag_blite1',  [-54.4, -16.6, -0.5], ID), ('tag_blite2', [-54.4, 16.6, -0.5], ID),   # outer tail light bars
    ('tag_rlite1',  [-54.4, -13.5, -0.5], ID), ('tag_rlite2', [-54.4, 13.5, -0.5], ID),   # inner tail light bars
]
m.tags = [[[n, list(o), copy.deepcopy(a)] for n, o, a in tags]]

# ---------------- frame bounds ------------------------------------------------
allp = [p for s in m.surfs for p in verts(s)]
mn = [min(p[k] for p in allp) for k in range(3)]; mx = [max(p[k] for p in allp) for k in range(3)]
rad = max(math.sqrt(sum(c*c for c in p)) for p in allp)
m.frames = [mn + mx + [0.0, 0.0, 0.0, rad, b'mustang']]
md3.save(m, OUT + '/body.md3')
print('body bounds', [round(c, 2) for c in mn], [round(c, 2) for c in mx], 'radius', round(rad, 2))

# ---------------- wheel: mini wheel scaled up to fill the arches --------------
w = md3.load(WSRC); w.name = P + 'wheel.md3'
WS = 7.2/6.5
for s in w.surfs:
    for v in s.frames[0]:
        for k in range(3): v[k] = int(round(v[k]*WS))
wp = [p for s in w.surfs for p in verts(s)]
wmn = [min(p[k] for p in wp) for k in range(3)]; wmx = [max(p[k] for p in wp) for k in range(3)]
w.frames = [wmn + wmx + [0.0, 0.0, 0.0, max(math.sqrt(sum(c*c for c in p)) for p in wp), b'wheel']]
md3.save(w, OUT + '/wheel.md3')
print('wheel bounds', [round(c, 2) for c in wmn], [round(c, 2) for c in wmx])
