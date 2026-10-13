"""Convert the GT-R MilkShape source (.ms3d) into Q3Rally body/wheel MD3s.
usage: build_gtr.py <GTR.ms3d> <wheel.md3 to reuse> <outdir>

The source is a high-detail scene: a second copy of the body (Box17x), an
interior, projector headlights made of tori and spheres, plates and badges
textured with images that are not included, and no wheels. Only the visible
outer shell is kept, sorted by material into Q3Rally surfaces; the paint gets
the atlas projection from layout.py, surfaces stay under the MD3 vertex limit."""
import os, sys, math, copy
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md3, ms3d, layout
layout.configure(x0=-58.5, x1=58.5, y0=-25.5, y1=25.5, z0=-14.5, z1=16.0, sc=3.36, lx=34)
from layout import *
SRC, WHEEL, OUT = sys.argv[1:4]
P = 'models/players/gtr/'
SCALE, CZ, CY = 0.525, 17.8, 18.0       # ms3d units -> Q3 units, length centre, wheel centre height
MAXV = 990

# group -> surface (dropped groups are simply not listed)
SURF = {
    'Box021': 'body', 'Box03': 'body', 'Box32': 'body',          # paint, mirrors, spoiler
    'Box022': 'trim', 'Box33': 'trim', 'Box34': 'trim', 'Box35': 'trim',
    'Box027': 'black',
    'Box023': 'glass',
    'Box024': 'tred', 'Plane03': 'tlights', 'Plane04': 'tlights',
    'Box025': 'hlights', 'Box026': 'ilights',
    'Box028': 'chrome', 'Tube13': 'chrome', 'Tube14': 'chrome', 'Tube15': 'chrome', 'Tube16': 'chrome',
}
TEX = {'body': 'red_main', 'trim': 'trim', 'black': 'black', 'glass': 'glass', 'tred': 'tred', 'tlights': 'tlite',
       'hlights': 'hlite', 'ilights': 'ilite', 'chrome': 'chrome'}

def to_q3(p):
    x, y, z = p
    return [SCALE*(z - CZ), SCALE*x, SCALE*(y - CY)]
def to_q3n(n):
    x, y, z = n
    return [z, x, y]
def norm(v):
    l = math.sqrt(sum(c*c for c in v)) or 1
    return [c/l for c in v]
def paint_region(n):
    nx, ny, nz = n
    ax = max(abs(nx), abs(ny), abs(nz))
    if nz < -0.55: return 'U'
    if ax == abs(nz): return 'T'
    if ax == abs(ny): return 'L' if ny > 0 else 'R'
    return 'F' if nx > 0 else 'B'

src = ms3d.load(SRC)
corners = {}                              # surface -> list of triangles [(pos, normal, uv) x3]
for g in src.groups:
    sname = SURF.get(g.name)
    if not sname: continue
    for ti in g.tris:
        t = src.tris[ti]
        p = [to_q3(src.verts[i]) for i in t.v]
        n = [norm(to_q3n(c)) for c in t.n]
        fn = norm([(p[1][1]-p[0][1])*(p[2][2]-p[0][2]) - (p[1][2]-p[0][2])*(p[2][1]-p[0][1]),
                   (p[1][2]-p[0][2])*(p[2][0]-p[0][0]) - (p[1][0]-p[0][0])*(p[2][2]-p[0][2]),
                   (p[1][0]-p[0][0])*(p[2][1]-p[0][1]) - (p[1][1]-p[0][1])*(p[2][0]-p[0][0])])
        if sname == 'body':
            r = paint_region(fn); uv = [atlas(r, q) for q in p]
        elif sname == 'tlights':
            uv = [[t.s[k], t.t[k]] for k in range(3)]
        elif sname in ('glass', 'chrome'):
            uv = [[(q[0] - X0)/(X1 - X0), (Z1 - q[2])/(Z1 - Z0)] for q in p]
        else:
            uv = [[0.5, 0.5]]*3
        # ms3d is counter-clockwise, md3 clockwise
        corners.setdefault(sname, []).append([(p[k], n[k], uv[k]) for k in (0, 2, 1)])

def make_surfaces(name, tris):
    out = []; cur = None
    for tri in tris:
        if cur is None or len(cur['v']) > MAXV - 3:
            cur = {'v': [], 'idx': {}, 't': []}; out.append(cur)
        ids = []
        for p, n, uv in tri:
            key = (tuple(round(c, 3) for c in p), tuple(round(c, 2) for c in n), tuple(round(c, 4) for c in uv))
            if key not in cur['idx']:
                cur['idx'][key] = len(cur['v']); cur['v'].append((p, n, uv))
            ids.append(cur['idx'][key])
        cur['t'].append(ids)
    surfs = []
    for k, c in enumerate(out):
        s = md3.Surf(); s.name = name if k == 0 else f'{name}{k+1}'; s.flags = 0
        s.shaders = [(P + TEX[name] + '.tga', 0)]
        s.tris = c['t']; s.st = [list(uv) for _, _, uv in c['v']]
        s.frames = [[[int(round(q*64)) for q in p] + [md3.enc_normal(*n)] for p, n, _ in c['v']]]
        surfs.append(s)
    return surfs

m = md3.Model(); m.name = P + 'body.md3'; m.flags = 0
m.surfs = []
for name in TEX:
    if name in corners: m.surfs += make_surfaces(name, corners[name])

def vs(names):
    return [p for n in names for tri in corners.get(n, []) for p, _, _ in tri]
body = vs(['body'])
roof = max(p[2] for p in body if abs(p[1]) < 3 and abs(p[0]) < 12)
hood_x = 40.0
hood = max(p[2] for p in body if abs(p[1]) < 3 and abs(p[0] - hood_x) < 2)
rear_x = min(p[0] for p in body)
hl = vs(['hlights'])
def cluster(pts, side):
    q = [p for p in pts if p[1]*side > 0]
    return [sum(c[k] for c in q)/len(q) for k in range(3)], max(c[0] for c in q), min(c[0] for c in q)
hlc, hlx, _ = cluster(hl, 1)
# tail lights: two round lights per textured plane; outer = brake, inner = reverse
tl = {}
for g in src.groups:
    if g.name not in ('Plane03', 'Plane04'): continue
    for ti in g.tris:
        t = src.tris[ti]
        for k in range(3):
            tl.setdefault(g.name, []).append((t.s[k], t.t[k], to_q3(src.verts[t.v[k]])))
def at_uv(samples, u, v):
    best = min(samples, key=lambda s: (s[0]-u)**2 + (s[1]-v)**2)
    # nearest-corner inverse distance blend is enough on a flat quad
    w = [(1/((s[0]-u)**2 + (s[1]-v)**2 + 1e-6), s[2]) for s in samples]
    tot = sum(a for a, _ in w)
    return [sum(a*p[k] for a, p in w)/tot for k in range(3)]
lights = []
for name, smp in tl.items():
    a, b = at_uv(smp, 0.25, 0.5), at_uv(smp, 0.75, 0.5)
    outer, inner = (a, b) if abs(a[1]) > abs(b[1]) else (b, a)
    lights.append((outer, inner))
lights.sort(key=lambda l: l[0][1])        # right side (-y) first

ID = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
BACKWARD = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]
WL = [[0, 1, 0], [1, 0, 0], [0, 0, -1]]
WR = [[0, -1, 0], [-1, 0, 0], [0, 0, -1]]
FX, RX = SCALE*(79.0 - CZ), SCALE*(-47.0 - CZ)       # arch centres measured on the side silhouette
WY, WZ = 20.0, SCALE*(2.0 - CY)
plate = to_q3((0.0, -0.15, -89.0))
tags = [
    ('tag_wheelfl', [FX,  WY, WZ], WL), ('tag_wheelfr', [FX, -WY, WZ], WR),
    ('tag_wheelrl', [RX,  WY, WZ], WL), ('tag_wheelrr', [RX, -WY, WZ], WR),
    ('tag_weapon',  [-2.0, 0.0, roof + 0.4], ID),
    ('tag_plate',   [plate[0] - 0.2, 0.0, plate[2]], ID),
    ('tag_turbo',   [rear_x + 1.0, 0.0, SCALE*(0.6 - CY) - 2.6], BACKWARD),   # below the low rear plate
    ('tag_smoke',   [hood_x, 0.0, hood + 0.3], BACKWARD),
    ('tag_hlite1',  [hlx + 0.2,  hlc[1], hlc[2]], ID), ('tag_hlite2', [hlx + 0.2, -hlc[1], hlc[2]], ID),
    ('tag_blite1',  [lights[0][0][0] - 0.3, lights[0][0][1], lights[0][0][2]], ID),
    ('tag_blite2',  [lights[1][0][0] - 0.3, lights[1][0][1], lights[1][0][2]], ID),
    ('tag_rlite1',  [lights[0][1][0] - 0.3, lights[0][1][1], lights[0][1][2]], ID),
    ('tag_rlite2',  [lights[1][1][0] - 0.3, lights[1][1][1], lights[1][1][2]], ID),
]
m.tags = [[[n, [round(c, 3) for c in o], copy.deepcopy(a)] for n, o, a in tags]]
allp = [[c/64 for c in v[:3]] for s in m.surfs for v in s.frames[0]]
mn = [min(p[k] for p in allp) for k in range(3)]; mx = [max(p[k] for p in allp) for k in range(3)]
m.frames = [mn + mx + [0.0, 0.0, 0.0, max(math.sqrt(sum(c*c for c in p)) for p in allp), b'gtr']]
md3.save(m, OUT + '/body.md3')
print('body bounds', [round(c, 2) for c in mn], [round(c, 2) for c in mx])
for s in m.surfs: print('  ', s.name, len(s.st), 'verts', len(s.tris), 'tris')
for n, o, _ in m.tags[0]: print('  ', n, o)

w = md3.load(WHEEL); w.name = P + 'wheel.md3'
w.frames = w.frames[:1]; w.frames[0][10] = b'wheel'
for s in w.surfs: s.frames = s.frames[:1]
w.tags = [w.tags[0]] if w.tags else [[]]
md3.save(w, OUT + '/wheel.md3')
