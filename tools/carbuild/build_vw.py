"""Rebuild the VW Typ 82 (Kuebelwagen) body/wheel MD3 for Q3Rally.
usage: build_vw.py <src dir with body.md3, wheel.md3, red.tga> <outdir>

The source body has one 'body' surface whose texture is a flat colour template
(red paint, grey trim, baked-in wireframe lines) with both sides sharing UVs.
The triangles are sorted into materials by sampling that template, the paint
gets the atlas projection from layout.py so each side can carry its own decals,
everything else keeps its UVs on small shared textures."""
import os, sys, math, copy
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import md3
from layout import *
import numpy as np
from PIL import Image, ImageDraw
SRC, OUT = sys.argv[1], sys.argv[2]
P = 'models/players/vw_typ_82/'
DX = -391            # 1/64 units: centres the body on the origin (-6.11 units)
D = DX/64

def verts(s): return [[c/64 for c in v[:3]] for v in s.frames[0]]
def tri_normal(s, t):
    n = [sum(md3.dec_normal(s.frames[0][i][3])[k] for i in t) for k in range(3)]
    l = math.sqrt(sum(c*c for c in n)) or 1
    return [c/l for c in n]
def paint_region(n):
    nx, ny, nz = n
    ax = max(abs(nx), abs(ny), abs(nz))
    if nz < -0.55: return 'U'
    if ax == abs(nz): return 'T'
    if ax == abs(ny): return 'L' if ny > 0 else 'R'
    return 'F' if nx > 0 else 'B'
def subsurf(s, name, tris):
    n = copy.deepcopy(s); n.name = name; n.tris = [list(t) for t in tris]
    used = sorted({i for t in n.tris for i in t}); remap = {o: k for k, o in enumerate(used)}
    n.frames = [[s.frames[0][i] for i in used]]; n.st = [list(s.st[i]) for i in used]
    n.tris = [[remap[i] for i in t] for t in n.tris]
    return n
def reproject(s, key_and_uv):
    V = verts(s); newv = []; newst = []; idx = {}; tris = []
    for t in s.tris:
        key, fn = key_and_uv(t); nt = []
        for i in t:
            k = (i, key)
            if k not in idx:
                idx[k] = len(newv); newv.append(s.frames[0][i]); newst.append(fn(V[i]))
            nt.append(idx[k])
        tris.append(nt)
    s.frames = [newv]; s.st = newst; s.tris = tris

m = md3.load(SRC + '/body.md3')
m.name = P + 'body.md3'
TPL = np.asarray(Image.open(SRC + '/red.tga').convert('RGB')).astype(int); TN = TPL.shape[0]
def material(s, t):
    """median template colour under the triangle, ignoring the white wireframe"""
    im = Image.new('L', (TN, TN), 0)
    ImageDraw.Draw(im).polygon([(s.st[i][0]*TN, s.st[i][1]*TN) for i in t], fill=1)
    px = TPL[np.asarray(im).astype(bool)]
    if len(px) == 0:
        u = np.mean([s.st[i][0] for i in t]); v = np.mean([s.st[i][1] for i in t])
        px = TPL[int(np.clip(v*TN, 0, TN-1)), int(np.clip(u*TN, 0, TN-1))][None]
    good = px[px.sum(1) < 600]
    c = np.median(good if len(good) else px, 0)
    if c[0] > 200 and c[1] < 80: return 'paint'
    p = [verts(s)[i] for i in t]
    if all(q[0] > 44 and abs(q[1]) > 14 and q[2] > 1.5 for q in p) and tri_normal(s, t)[0] > 0.5:
        return 'lens'
    return 'trim'

DRV = np.asarray(Image.open(SRC + '/driver.tga').convert('RGB')).astype(int)
def driver_part(s, t):
    u = np.mean([s.st[i][0] for i in t]); v = np.mean([s.st[i][1] for i in t])
    c = DRV[int(np.clip(v*DRV.shape[0], 0, DRV.shape[0]-1)), int(np.clip(u*DRV.shape[1], 0, DRV.shape[1]-1))]
    return 'driver' if c[2] > c[0] else 'hands'

surfs = []
for s in m.surfs:
    s.frames = s.frames[:1]
    if s.name == 'body':
        groups = {}
        for t in s.tris: groups.setdefault(material(s, t), []).append(t)
        surfs += [subsurf(s, {'paint': 'body', 'trim': 'trim', 'lens': 'hlights'}[k], v) for k, v in sorted(groups.items())]
    elif s.name == 'driver':
        # blue overall vs. skin coloured hands, so the overall can follow the livery
        groups = {}
        for t in s.tris: groups.setdefault(driver_part(s, t), []).append(t)
        surfs += [subsurf(s, k, v) for k, v in sorted(groups.items())]
    else:
        surfs.append(s)
for s in surfs:
    for v in s.frames[0]: v[0] += DX
m.surfs = surfs

TEX = {'body': 'red_main', 'trim': 'trim', 'wheel_arches': 'trim', 'steering': 'trim',
       'hlights': 'hlite', 'driver': 'red_driver', 'hands': 'hands', 'windscreen': 'windscreen'}
for s in m.surfs:
    s.shaders = [(P + TEX[s.name] + '.tga', 0)]
    if s.name == 'body':
        def gen(t, s=s):
            r = paint_region(tri_normal(s, t))
            return r, lambda p: atlas(r, p)
        reproject(s, gen)
    elif s.name == 'hlights':
        V = verts(s); box = {}
        for p in V:
            b = box.setdefault(p[1] > 0, [1e9, -1e9, 1e9, -1e9])
            b[0] = min(b[0], p[1]); b[1] = max(b[1], p[1]); b[2] = min(b[2], p[2]); b[3] = max(b[3], p[2])
        def uv(p):
            b = box[p[1] > 0]
            return [(p[1]-b[0])/((b[1]-b[0]) or 1), 1-(p[2]-b[2])/((b[3]-b[2]) or 1)]
        reproject(s, lambda t: (0, uv))
    elif s.name in ('driver', 'hands', 'trim', 'wheel_arches', 'steering'):
        s.st = [[0.5, 0.5] for _ in s.st]      # flat colour textures

# ---------------- tags (source coordinates, shifted below) ----------------------
ID = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
BACKWARD = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]
WL = [[0, 1, 0], [1, 0, 0], [0, 0, -1]]
WR = [[0, -1, 0], [-1, 0, 0], [0, 0, -1]]
HEAD = [[-1, 0, 0], [0, 1, 0], [0, 0, -1]]
FX, RX = 38.5, -23.6          # wheel arch centres measured from the mesh
FY, RY, WZ = 19.1, 21.0, -7.5
tags = [
    ('tag_wheelfl', [FX,  FY, WZ], WL), ('tag_wheelfr', [FX, -FY, WZ], WR),
    ('tag_wheelrl', [RX,  RY, WZ], WL), ('tag_wheelrr', [RX, -RY, WZ], WR),
    ('tag_head',    [7.85, 8.12, 13.62], HEAD),            # driver sits on the left
    ('tag_weapon',  [-27.0, 0.0, 12.6], ID),               # front of the engine lid
    ('tag_plate',   [-42.9, 0.0, -3.9], ID),               # rear panel
    ('tag_turbo',   [-43.6, 0.0, -8.0], BACKWARD),         # exhaust under the rear panel
    ('tag_smoke',   [-36.0, 0.0, 12.3], BACKWARD),         # rear engine lid
    ('tag_hlite1',  [50.4,  17.8, 4.8], ID), ('tag_hlite2', [50.4, -17.8, 4.8], ID),
    ('tag_blite1',  [-40.6, -15.5, 4.5], ID), ('tag_blite2', [-40.6, 15.5, 4.5], ID),
    ('tag_rlite1',  [-40.6, -11.5, 4.5], ID), ('tag_rlite2', [-40.6, 11.5, 4.5], ID),
]
m.tags = [[[n, [o[0] + D, o[1], o[2]], copy.deepcopy(a)] for n, o, a in tags]]

allp = [p for s in m.surfs for p in verts(s)]
mn = [min(p[k] for p in allp) for k in range(3)]; mx = [max(p[k] for p in allp) for k in range(3)]
m.frames = [mn + mx + [0.0, 0.0, 0.0, max(math.sqrt(sum(c*c for c in p)) for p in allp), b'vw_typ_82']]
md3.save(m, OUT + '/body.md3')
print('body bounds', [round(c, 2) for c in mn], [round(c, 2) for c in mx])
for s in m.surfs: print('  ', s.name, len(s.st), 'verts', len(s.tris), 'tris')

# ---------------- wheel: drop the 29 duplicate frames ---------------------------
w = md3.load(SRC + '/wheel.md3'); w.name = P + 'wheel.md3'
w.frames = w.frames[:1]; w.frames[0][10] = b'wheel'
for s in w.surfs:
    s.frames = s.frames[:1]; s.shaders = [('models/players/wheels/' + ('rim' if s.name == 'w_rim' else 'tread') + '.tga', 0)]
w.tags = [[]]
md3.save(w, OUT + '/wheel.md3')
