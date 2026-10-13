"""Tiny textured software renderer for Q3Rally car md3s (icons + previews).
usage: render3d.py <cardir> <skin> <wheelskin> <out.png> [size] [campos x,y,z] [badge.tga]"""
import sys, math, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import md3
import numpy as np
from PIL import Image
ROOT = os.environ.get('Q3R_BASE', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'baseq3r')) + '/'
CARDIR, SKIN, WSKIN, OUTP = sys.argv[1:5]
SIZE = int(sys.argv[5]) if len(sys.argv) > 5 else 384
CAM = np.array([float(c) for c in sys.argv[6].split(',')]) if len(sys.argv) > 6 else np.array([200., 60., 45.])
BADGE = sys.argv[7] if len(sys.argv) > 7 else None
SS = 2; W = SIZE*SS

def load_skin(path):
    d = {}
    for line in open(path):
        if ',' in line:
            a, b = line.strip().split(',', 1)
            if b: d[a.lower()] = b
    return d
texcache = {}
def tex(path):
    if path not in texcache:
        p = None
        for base in (CARDIR + '/', ROOT):
            cand = base + (os.path.basename(path) if base != ROOT else path)
            if os.path.exists(cand): p = cand; break
        im = Image.open(p).convert('RGBA') if p else Image.new('RGBA', (4, 4), (255, 0, 255, 255))
        texcache[path] = np.asarray(im).astype(np.float32)/255
    return texcache[path]

def surfaces(m, skin, xf):
    out = []
    for s in m.surfs:
        tp = skin.get(s.name.lower())
        if not tp: continue
        V = np.array([[c/64 for c in v[:3]] for v in s.frames[0]])
        Nn = np.array([md3.dec_normal(v[3]) for v in s.frames[0]])
        V, Nn = xf(V, Nn)
        out.append((V, Nn, np.array(s.st), np.array(s.tris), tex(tp), tp))
    return out

body = md3.load(CARDIR + '/body.md3'); wheel = md3.load(CARDIR + '/wheel.md3')
skin = load_skin(f'{CARDIR}/{SKIN}.skin'); wskin = load_skin(f'{ROOT}models/players/wheels/{WSKIN}.skin')
items = surfaces(body, skin, lambda V, N: (V, N))
for n, o, ax in body.tags[0]:
    if n.startswith('tag_wheel'):
        R = np.array(ax); O = np.array(o)
        items += surfaces(wheel, wskin, lambda V, N, R=R, O=O: (V @ R + O, N @ R))

# camera
target = np.array([float(c) for c in os.environ.get('Q3R_TARGET', '0,0,-1').split(',')]); fwd = target - CAM; fwd /= np.linalg.norm(fwd)
right = np.cross(fwd, [0, 0, 1]); right /= np.linalg.norm(right); up = np.cross(right, fwd)
FOV = 30; f = W/2/math.tan(math.radians(FOV/2))
L = np.array([0.4, 0.5, 0.75]); L /= np.linalg.norm(L)
color = np.zeros((W, W, 3), np.float32); alpha = np.zeros((W, W), np.float32); zbuf = np.full((W, W), 1e9, np.float32)

def env(rdir):
    zz = rdir[..., 2:3]
    sky = np.array([0.80, 0.86, 0.95]); hor = np.array([0.95, 0.95, 0.95]); gnd = np.array([0.22, 0.20, 0.19])
    return np.where(zz > 0, hor + (sky-hor)*np.clip(zz*2, 0, 1), hor*0.6 + (gnd-hor*0.6)*np.clip(-zz*4, 0, 1))

for V, Nn, ST, T, TX, tp in items:
    rel = V - CAM; cz = rel @ fwd; cx = rel @ right; cy = rel @ up
    sx = W/2 + f*cx/cz; sy = W/2 - f*cy/cz
    reflective = any(k in tp for k in ('_main', 'glass', 'chrome'))
    th, tw = TX.shape[:2]
    for t in T:
        a, b, c = t
        xs = np.array([sx[a], sx[b], sx[c]]); ys = np.array([sy[a], sy[b], sy[c]])
        den = (ys[1]-ys[2])*(xs[0]-xs[2]) + (xs[2]-xs[1])*(ys[0]-ys[2])
        if abs(den) < 1e-9: continue
        x0 = max(int(xs.min()), 0); x1 = min(int(xs.max())+1, W-1); y0 = max(int(ys.min()), 0); y1 = min(int(ys.max())+1, W-1)
        if x1 < x0 or y1 < y0: continue
        gx, gy = np.meshgrid(np.arange(x0, x1+1)+0.5, np.arange(y0, y1+1)+0.5)
        w0 = ((ys[1]-ys[2])*(gx-xs[2]) + (xs[2]-xs[1])*(gy-ys[2]))/den
        w1 = ((ys[2]-ys[0])*(gx-xs[2]) + (xs[0]-xs[2])*(gy-ys[2]))/den
        w2 = 1-w0-w1
        inside = (w0 >= -1e-4) & (w1 >= -1e-4) & (w2 >= -1e-4)
        if not inside.any(): continue
        iz = np.array([1/cz[a], 1/cz[b], 1/cz[c]])
        pw = np.stack([w0*iz[0], w1*iz[1], w2*iz[2]], -1); zi = pw.sum(-1); pw = pw/zi[..., None]
        depth = 1/zi
        sub = zbuf[y0:y1+1, x0:x1+1]
        mask = inside & (depth < sub)
        if not mask.any(): continue
        uv = pw @ ST[[a, b, c]]
        tu = (uv[..., 0] % 1.0)*tw; tv = (uv[..., 1] % 1.0)*th
        texel = TX[np.clip(tv.astype(int), 0, th-1), np.clip(tu.astype(int), 0, tw-1)]
        n = pw @ Nn[[a, b, c]]; n /= np.linalg.norm(n, axis=-1, keepdims=True)+1e-9
        p = pw @ V[[a, b, c]]
        vdir = p - CAM; vdir /= np.linalg.norm(vdir, axis=-1, keepdims=True)
        if (n*vdir).sum(-1).mean() > 0.2: n = -n            # two sided safety
        diff = np.clip(n @ L, 0, 1)[..., None]
        rgb = texel[..., :3]*(0.42 + 0.68*diff)
        if reflective:
            r = vdir - 2*(vdir*n).sum(-1, keepdims=True)*n
            spec = np.clip(r @ L, 0, 1)[..., None]**40
            k = (1-texel[..., 3:4])
            rgb = rgb*texel[..., 3:4] + env(r)*k*(0.35+0.65*diff) + spec*0.55
        color[y0:y1+1, x0:x1+1][mask] = np.clip(rgb, 0, 1)[mask]
        alpha[y0:y1+1, x0:x1+1][mask] = 1
        sub[mask] = depth[mask]

img = np.dstack([color, alpha])
im = Image.fromarray((img*255).astype(np.uint8), 'RGBA')
if os.environ.get('Q3R_FIT'):
    # icon framing like the other cars: car centred, lower part of the square
    bx = im.getbbox(); car = im.crop(bx)
    sc = min(W*0.95/car.width, W*0.66/car.height)
    car = car.resize((int(car.width*sc), int(car.height*sc)), Image.LANCZOS)
    im = Image.new('RGBA', (W, W), (0, 0, 0, 0))
    im.alpha_composite(car, ((W-car.width)//2, int(W*0.93) - car.height))
im = im.resize((SIZE, SIZE), Image.LANCZOS)
if BADGE:
    b = np.asarray(Image.open(BADGE).convert('RGBA').resize((384, 384))).copy()
    yy, xx = np.mgrid[:384, :384]; b[((xx-52)**2 + (yy-52)**2) > 50**2] = 0
    bi = Image.fromarray(b).resize((SIZE, SIZE), Image.LANCZOS)
    im.alpha_composite(bi)
im.save(OUTP)
