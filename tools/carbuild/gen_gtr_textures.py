"""Paint the GT-R textures. usage: gen_gtr_textures.py <outdir>
World coordinates are those of the converted body.md3 (centred, front = +x)."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import layout
layout.configure(x0=-58.5, x1=58.5, y0=-25.5, y1=25.5, z0=-14.5, z1=16.0, sc=3.36, lx=34)   # same as build_gtr.py
from layout import *
from paintlib import *
from PIL import Image, ImageDraw
import numpy as np, zlib
OUT = sys.argv[1]
TRIM = (20, 20, 22)
EXTRA = {'silver': (176, 180, 186)}

def paint(name, base, sc=None):
    img = Image.new('RGBA', (N, N), base + (210,))
    dr = ImageDraw.Draw(img)
    for side in 'LR':
        for i in range(40):
            z0 = Z0 + (Z1-Z0)*i/40; z1 = Z0 + (Z1-Z0)*(i+1)/40
            k = 0.80 + 0.22*min(1, max(0, (z0+12)/18))
            rect(dr, side, (X0, z0), (X1, z1), shade(base, k) + (210,))
    if sc:
        st, ed = sc['stripe'], sc['edge']
        for y0, y1, c in [(-1.4, 7.4, st), (-3.8, -2.6, ed)]:
            rect(dr, 'T', (X0, y0), (X1, y1), c + (220,))
            rect(dr, 'F', (y0, Z0), (y1, Z1), c + (220,))
            rect(dr, 'B', (y0, Z0), (y1, Z1), c + (220,))
        for side in 'LR':
            poly(dr, side, [(50, -6.2), (-52, -6.2), (-54, -8.4), (52, -8.4)], st + (220,))
            rect(dr, side, (-54, -9.6), (52, -9.0), ed + (220,))
    for side in 'LR':
        rect(dr, side, (-6.6, 3.4), (-3.4, 4.2), (190, 190, 195, 150))     # door handle
    # big front grille (part of the paint mesh): dark honeycomb
    poly(dr, 'F', [(-11.6, -1.7), (11.6, -1.7), (10.2, -10.6), (-10.2, -10.6)], (14, 14, 16, 245))
    for gy in np.arange(-11.0, 11.5, 1.1):
        for gz in np.arange(-10.0, -2.0, 0.75):
            o = 0.55 if int(round((gz+10.0)/0.75)) % 2 else 0
            x, y = P('F', y=gy+o, z=gz)
            dr.ellipse([x-1.2*K, y-0.9*K, x+1.2*K, y+0.9*K], fill=(44, 44, 50, 245))
    dr.rectangle([R_UNDER[0]*K, R_UNDER[1]*K, N, N], fill=TRIM + (250,))
    if sc:
        for side in 'LR':
            x, y = P(side, 6.0, z=-1.0); im, pos = roundel(dr, x, y, 5.6*SC*K, sc); img.alpha_composite(im, pos)
            x, y = P(side, 38.0, z=0.5); text(img, 'Q3RALLY', x, y, 2.2*SC*K, st if sum(base) < 400 else ed)
        x, y = P('T', -4.0, 0.0); im, pos = roundel(dr, x, y, 7.0*SC*K, sc, rot=-90); img.alpha_composite(im, pos)
    img.resize((int(A), int(A)), Image.LANCZOS).save(f'{OUT}/{name}_main.tga')

def flat(name, rgb, size=32, alpha=255):
    rnd = np.random.RandomState(zlib.crc32(name.encode()) % 1000).rand(size, size, 1)*10 - 5
    px = np.clip(np.array(rgb)[None, None, :] + rnd, 0, 255).astype(np.uint8)
    Image.fromarray(np.dstack([px, np.full((size, size), alpha, np.uint8)]), 'RGBA').save(f'{OUT}/{name}.tga')

for n, c in PLAIN.items(): paint(n, c)
for n, c in EXTRA.items(): paint(n, c)
for n, sc in RALLY.items(): paint(n, sc['body'], sc)
flat('trim', (62, 62, 66))
flat('black', TRIM)
flat('tred', (150, 10, 10))
flat('ilite', (250, 140, 20))
flat('hlite', (225, 228, 232), alpha=150)       # reflective projector housing

yy = np.linspace(0, 1, 128)[:, None]*np.ones((1, 128))
g = np.zeros((128, 128, 4), np.uint8)
g[..., 0] = 16 + 26*(1-yy); g[..., 1] = 20 + 30*(1-yy); g[..., 2] = 28 + 36*(1-yy); g[..., 3] = 165
Image.fromarray(g).save(f'{OUT}/glass.tga')
c = np.zeros((128, 128, 4), np.uint8)
prof = np.interp(yy, [0, 0.45, 0.5, 0.55, 1], [235, 170, 70, 140, 200])
c[..., 0] = prof; c[..., 1] = prof; c[..., 2] = np.minimum(255, prof*1.05); c[..., 3] = 120
Image.fromarray(c).save(f'{OUT}/chrome.tga')

# the two round tail lights on each light plane (u: 0..0.5 and 0.5..1)
t = Image.new('RGBA', (256, 128), TRIM + (255,)); d = ImageDraw.Draw(t)
for cx in (64, 192):
    d.ellipse([cx-58, 6, cx+58, 122], fill=(150, 152, 158, 255))
    d.ellipse([cx-50, 14, cx+50, 114], fill=(120, 6, 6, 255))
    d.ellipse([cx-40, 24, cx+40, 104], fill=(215, 22, 18, 255))
    d.ellipse([cx-16, 48, cx+16, 80], fill=(245, 120, 110, 255))
t.save(f'{OUT}/tlite.tga')
print('ok')
