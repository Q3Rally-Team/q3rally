"""Convert the stock car MilkShape source (.ms3d) into Q3Rally body/wheel MD3s.
usage: build_stockcar.py <stockcar.ms3d> <wheel.md3 to reuse> <outdir>

The source is a clean low-poly shell (BODY + UNDERNEATH, one material whose
STOCKCAR.BMP is lost) at 1/12 scale, nose towards -z, without wheels or tags.
Its UV layout mirrors the left and right halves of roof, hood, trunk and
tail onto the same texels, which breaks any asymmetric livery, so BODY gets
the unique atlas projection from layout.py instead (the texture is painted
by gen_stockcar_textures.py); the tags are placed from ray casts on the shell."""
import os, sys, math, copy
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md3, ms3d, measure, layout
layout.configure(x0=-56.5, x1=56.5, y0=-21.0, y1=21.0, z0=-16.5, z1=13.0, sc=3.66, lx=34)
from layout import atlas
import numpy as np
SRC, WHEEL, OUT = sys.argv[1:4]
P = 'models/players/stockcar/'
S = 12.0                                     # source -> Q3 units (length ~110)
DZ = -3.7                                    # lowers the body so the arch centres sit at z -8.6

src = ms3d.load(SRC)
cz = (min(v[2] for v in src.verts) + max(v[2] for v in src.verts))/2
def to_q3(p):  return [-S*(p[2] - cz), -S*p[0], S*p[1] + DZ]       # nose to +x
def to_q3n(n): return [-n[2], -n[0], n[1]]
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

surf_tris = {'body': [], 'under': []}
for g in src.groups:
    name = 'body' if g.name.strip() == 'BODY' else 'under'
    for ti in g.tris:
        t = src.tris[ti]
        p = [to_q3(src.verts[t.v[k]]) for k in (0, 2, 1)]          # ms3d CCW -> md3 CW
        fn = norm(np.cross(np.subtract(p[2], p[0]), np.subtract(p[1], p[0])))
        r = paint_region(fn)
        c = [(p[j], norm(to_q3n(t.n[k])), tuple(atlas(r, p[j])) if name == 'body' else (0.5, 0.5))
             for j, k in enumerate((0, 2, 1))]
        surf_tris[name].append(c)

m = md3.Model(); m.name = P + 'body.md3'; m.flags = 0; m.surfs = []
for name, tex in (('body', 'red'), ('under', 'under')):
    verts, idx, tris = [], {}, []
    for tri in surf_tris[name]:
        ids = []
        for p, n, uv in tri:
            key = (tuple(round(c, 3) for c in p), tuple(round(c, 2) for c in n), tuple(round(c, 4) for c in uv))
            if key not in idx: idx[key] = len(verts); verts.append((p, n, uv))
            ids.append(idx[key])
        tris.append(ids)
    s = md3.Surf(); s.name = name; s.flags = 0; s.shaders = [(P + tex + '.tga', 0)]
    s.tris = tris; s.st = [list(uv) for _, _, uv in verts]
    s.frames = [[[int(round(q*64)) for q in p] + [md3.enc_normal(*n)] for p, n, _ in verts]]
    assert len(verts) < 1000
    m.surfs.append(s)

# ---------------- measurements ------------------------------------------------
TR = [np.array([p for p, _, _ in tri]) for name in surf_tris for tri in surf_tris[name]]
xs, zs, H = measure.side_depth(TR, zr=(-20, 16))
flank = float(H[(zs > -8) & (zs < -2)].max())
fx, fz, fr, _ = measure.arch(xs, zs, H, 10, 52, 15, zbot=-17, zmin_fit=-10)
rx, rz, rr, _ = measure.arch(xs, zs, H, -52, -10, 15, zbot=-17, zmin_fit=-10)
print(f'flank {flank:.2f}  front arch x {fx:.2f} z {fz:.2f} r {fr:.2f}  rear arch x {rx:.2f} z {rz:.2f} r {rr:.2f}')

def ray_x(y, z, front=True):
    """x of the outermost surface along the length axis at (y, z)"""
    best = None
    for p in TR:
        (ax, ay, az), (bx, by, bz), (cx, cy, cz2) = p
        den = (bz-cz2)*(ay-cy) + (cy-by)*(az-cz2)
        if abs(den) < 1e-9: continue
        a = ((bz-cz2)*(y-cy) + (cy-by)*(z-cz2))/den; b = ((cz2-az)*(y-cy) + (ay-cy)*(z-cz2))/den; c = 1-a-b
        if min(a, b, c) < 0: continue
        x = a*ax + b*bx + c*cx
        if best is None or (x > best if front else x < best): best = x
    return best
def ray_z(x, y):
    best = None
    for p in TR:
        (ax, ay, az), (bx, by, bz), (cx, cy, cz2) = p
        den = (by-cy)*(ax-cx) + (cx-bx)*(ay-cy)
        if abs(den) < 1e-9: continue
        a = ((by-cy)*(x-cx) + (cx-bx)*(y-cy))/den; b = ((cy-ay)*(x-cx) + (ax-cx)*(y-cy))/den; c = 1-a-b
        if min(a, b, c) < 0: continue
        z = a*az + b*bz + c*cz2
        if best is None or z > best: best = z
    return best

ID = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
BACKWARD = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]
WL = [[0, 1, 0], [1, 0, 0], [0, 0, -1]]
WR = [[0, -1, 0], [-1, 0, 0], [0, 0, -1]]
WY = round(flank - 3.3, 2)                 # tyre face flush with the flank
HL = (12.5, -2.0)                          # painted headlights (y, z) - gen_stockcar_textures.py uses the tags
TL = ((15.0, -1.0), (10.5, -1.0))          # painted tail lights on the upper tail panel: outer brake, inner reverse
roof = ray_z(-4.0, 0.0)
hood = ray_z(36.0, 0.0)
deck = ray_z(-46.0, 0.0)
tags = [
    ('tag_wheelfl', [fx,  WY, fz], WL), ('tag_wheelfr', [fx, -WY, fz], WR),
    ('tag_wheelrl', [rx,  WY, rz], WL), ('tag_wheelrr', [rx, -WY, rz], WR),
    ('tag_weapon',  [-4.0, 0.0, roof + 0.4], ID),
    ('tag_plate',   [ray_x(0.0, -6.0, False) - 0.1, 0.0, -6.0], ID),
    ('tag_turbo',   [ray_x(-6.5, -11.5, False) - 0.5, -6.5, -11.5], BACKWARD),    # side exhaust exit
    ('tag_smoke',   [36.0, 0.0, hood + 0.3], BACKWARD),
    ('tag_hlite1',  [ray_x(HL[0], HL[1]) + 0.1,  HL[0], HL[1]], ID),
    ('tag_hlite2',  [ray_x(-HL[0], HL[1]) + 0.1, -HL[0], HL[1]], ID),
    ('tag_blite1',  [ray_x(-TL[0][0], TL[0][1], False) - 0.1, -TL[0][0], TL[0][1]], ID),
    ('tag_blite2',  [ray_x(TL[0][0], TL[0][1], False) - 0.1, TL[0][0], TL[0][1]], ID),
    ('tag_rlite1',  [ray_x(-TL[1][0], TL[1][1], False) - 0.1, -TL[1][0], TL[1][1]], ID),
    ('tag_rlite2',  [ray_x(TL[1][0], TL[1][1], False) - 0.1, TL[1][0], TL[1][1]], ID),
]
m.tags = [[[n, [round(c, 3) for c in o], copy.deepcopy(a)] for n, o, a in tags]]
allp = [[c/64 for c in v[:3]] for s in m.surfs for v in s.frames[0]]
mn = [min(p[k] for p in allp) for k in range(3)]; mx = [max(p[k] for p in allp) for k in range(3)]
m.frames = [mn + mx + [0.0, 0.0, 0.0, max(math.sqrt(sum(c*c for c in p)) for p in allp), b'stockcar']]
md3.save(m, OUT + '/body.md3')
print('body bounds', [round(c, 2) for c in mn], [round(c, 2) for c in mx], 'deck', round(deck, 2))
for s in m.surfs: print('  ', s.name, len(s.st), 'verts', len(s.tris), 'tris')
for n, o, _ in m.tags[0]: print('  ', n, o)

w = md3.load(WHEEL); w.name = P + 'wheel.md3'
w.frames = w.frames[:1]; w.frames[0][10] = b'wheel'
for s in w.surfs: s.frames = s.frames[:1]
md3.save(w, OUT + '/wheel.md3')
