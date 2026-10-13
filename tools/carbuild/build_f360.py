"""Rebuild the F360 body/wheel MD3 for Q3Rally.
usage: build_f360.py <src dir with body.md3, wheel.md3> <outdir>

The source already has photo textures and a sane mesh; it only needs the
missing tags (positions read back from the texture: tail lights, exhausts,
plate, headlights, engine cover, roof), symmetric wheel tags, an upright
weapon tag and a single frame wheel."""
import os, sys, math, copy
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import md3
SRC, OUT = sys.argv[1], sys.argv[2]
P = 'models/players/f360/'
DX = 32              # 1/64 units: centres the body on the origin (+0.5 units)
D = DX/64

m = md3.load(SRC + '/body.md3')
m.name = P + 'body.md3'
for s in m.surfs:
    s.frames = s.frames[:1]
    for v in s.frames[0]: v[0] += DX
    s.shaders = [(P + 'red.tga', 0)]

ID = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
BACKWARD = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]
WL = [[0, 1, 0], [1, 0, 0], [0, 0, -1]]
WR = [[0, -1, 0], [-1, 0, 0], [0, 0, -1]]
FX, RX, WY, WZ = 32.15, -40.36, 22.0, -8.42   # source tags, made symmetric
tags = [
    ('tag_wheelfl', [FX,  WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX,  WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon',  [3.3, 0.0, 15.6], ID),                 # roof (was upside down on the hood)
    ('tag_plate',   [-59.8, 0.0, -3.75], ID),              # painted plate recess
    ('tag_turbo',   [-58.0, 0.0, -7.7], BACKWARD),         # between the exhausts
    ('tag_smoke',   [-36.7, 0.0, 11.2], BACKWARD),         # engine cover
    ('tag_hlite1',  [50.0,  17.1, 0.3], ID), ('tag_hlite2', [50.0, -17.1, 0.3], ID),
    ('tag_blite1',  [-55.4, -18.5, 2.74], ID), ('tag_blite2', [-55.4, 18.5, 2.74], ID),   # outer round lights
    ('tag_rlite1',  [-56.6, -14.0, 2.74], ID), ('tag_rlite2', [-56.6, 14.0, 2.74], ID),   # inner round lights
]
m.tags = [[[n, [o[0] + D, o[1], o[2]], copy.deepcopy(a)] for n, o, a in tags]]
allp = [[c/64 for c in v[:3]] for s in m.surfs for v in s.frames[0]]
mn = [min(p[k] for p in allp) for k in range(3)]; mx = [max(p[k] for p in allp) for k in range(3)]
m.frames = [mn + mx + [0.0, 0.0, 0.0, max(math.sqrt(sum(c*c for c in p)) for p in allp), b'f360']]
md3.save(m, OUT + '/body.md3')
print('body bounds', [round(c, 2) for c in mn], [round(c, 2) for c in mx])

w = md3.load(SRC + '/wheel.md3'); w.name = P + 'wheel.md3'
w.frames = w.frames[:1]; w.frames[0][10] = b'wheel'
for s in w.surfs:
    s.frames = s.frames[:1]; s.shaders = [('models/players/wheels/' + ('rim' if s.name == 'w_rim' else 'tread') + '.tga', 0)]
w.tags = [[]]
md3.save(w, OUT + '/wheel.md3')
