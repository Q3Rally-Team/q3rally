"""Paint the VW Typ 82 textures. usage: gen_vw_textures.py <outdir>
World coordinates are those of the rebuilt body.md3 (centred, front = +x)."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from layout import *
from paintlib import *
from PIL import Image, ImageDraw
import numpy as np, zlib
OUT = sys.argv[1]
TRIM = (52, 48, 46)
EXTRA = {'sand': (196, 168, 112), 'olive': (86, 94, 58)}     # expedition liveries
DOORS = (-25.0, -4.5, 16.0)          # door gaps along the flank (x)

def paint(name, base, sc=None):
    img = Image.new('RGBA', (N, N), base + (215,))
    dr = ImageDraw.Draw(img)
    for side in 'LR':
        for i in range(40):
            z0 = Z0 + (Z1-Z0)*i/40; z1 = Z0 + (Z1-Z0)*(i+1)/40
            k = 0.82 + 0.20*min(1, max(0, (z0+10)/18))
            rect(dr, side, (X0, z0), (X1, z1), shade(base, k) + (215,))
        # pressed body crease along the flank
        rect(dr, side, (-48, 6.6), (46, 7.2), shade(base, 0.7) + (230,))
    if sc:
        st, ed = sc['stripe'], sc['edge']
        for y0, y1, c in [(-1.2, 6.8, st), (-3.4, -2.2, ed)]:
            rect(dr, 'T', (X0, y0), (X1, y1), c + (220,))
            rect(dr, 'F', (y0, Z0), (y1, Z1), c + (220,))
            rect(dr, 'B', (-y1, Z0), (-y0, Z1), c + (220,))
        for side in 'LR':
            rect(dr, side, (-49, -4.4), (49, -2.0), st + (220,))
            rect(dr, side, (-49, -5.6), (49, -4.9), ed + (220,))
    for side in 'LR':
        for x in DOORS:                                          # door gaps
            dr.line([P(side, x, z=-9.6), P(side, x, z=11.6)], fill=shade(base, 0.35) + (240,), width=K)
        dr.line([P(side, DOORS[0], z=-9.6), P(side, DOORS[-1], z=-9.6)], fill=shade(base, 0.35) + (240,), width=K)
        for x in (DOORS[1] - 3.0, DOORS[2] - 3.0):              # door handles
            rect(dr, side, (x - 1.4, 8.6), (x + 1.4, 9.4), (190, 190, 195, 150))
        rect(dr, side, (-47.5, 7.6), (-44.0, 10.6), TRIM + (240,))  # engine air intake
    # engine lid louvres and lights on the sloped tail
    for i in range(6):
        z = 6.4 + i*0.9
        rect(dr, 'B', (-7.0, z), (7.0, z + 0.45), TRIM + (245,))
    for yy, col in ((15.5, (210, 16, 12)), (-15.5, (210, 16, 12)), (11.5, (235, 235, 228)), (-11.5, (235, 235, 228))):
        x, y = P('B', y=yy, z=4.5); r = 1.3*SC*K
        dr.ellipse([x-r-K*2, y-r-K*2, x+r+K*2, y+r+K*2], fill=(170, 172, 178, 160))
        dr.ellipse([x-r, y-r, x+r, y+r], fill=col + (255,))
    rect(dr, 'T', (-44.0, -16.5), (-28.5, 16.5), None, outline=shade(base, 0.4) + (240,))   # engine lid
    rect(dr, 'T', (28.0, -12.0), (47.0, 12.0), None, outline=shade(base, 0.4) + (240,))     # front lid
    dr.rectangle([R_UNDER[0]*K, R_UNDER[1]*K, N, N], fill=(26, 25, 24, 250))
    if sc:
        for side in 'LR':
            x, y = P(side, -14.75, z=2.0); im, pos = roundel(dr, x, y, 5.6*SC*K, sc); img.alpha_composite(im, pos)
            x, y = P(side, 31.0, z=2.0); text(img, 'Q3RALLY', x, y, 2.2*SC*K, st if sum(base) < 400 else ed)
        x, y = P('T', -36.0, 0.0); im, pos = roundel(dr, x, y, 6.2*SC*K, sc, rot=-90); img.alpha_composite(im, pos)
    img.resize((int(A), int(A)), Image.LANCZOS).save(f'{OUT}/{name}_main.tga')

def flat(name, rgb, size=32, a=None):
    rnd = np.random.RandomState(zlib.crc32(name.encode()) % 1000).rand(size, size, 1)*14 - 7
    px = np.clip(np.array(rgb)[None, None, :] + rnd, 0, 255).astype(np.uint8)
    Image.fromarray(px).save(f'{OUT}/{name}.tga')

suits = {}
for n, c in PLAIN.items(): paint(n, c); suits[n] = (30, 34, 44)
for n, c in EXTRA.items(): paint(n, c); suits[n] = (64, 60, 48)
for n, sc in RALLY.items(): paint(n, sc['body'], sc); suits[n] = sc['stripe'] if sum(sc['body']) > 400 else sc['body']
for n, c in suits.items(): flat(f'{n}_driver', c)
flat('hands', (212, 176, 150))
flat('trim', TRIM, 64)

h = Image.new('RGBA', (64, 64), (60, 60, 64, 255)); d = ImageDraw.Draw(h)
d.ellipse([3, 3, 61, 61], fill=(190, 192, 198, 255)); d.ellipse([8, 8, 56, 56], fill=(236, 238, 230, 255))
for r in range(20, 0, -3): d.ellipse([32-r, 32-r, 32+r, 32+r], outline=(205, 208, 200, 255), width=1)
d.ellipse([26, 26, 38, 38], fill=(255, 255, 248, 255))
h.save(f'{OUT}/hlite.tga')

# windscreen: painted frame, tinted see-through glass
w = np.zeros((128, 128, 4), np.uint8)
yy = np.linspace(0, 1, 128)[:, None]*np.ones((1, 128))
w[..., 0] = 150 + 60*(1-yy); w[..., 1] = 168 + 50*(1-yy); w[..., 2] = 182 + 50*(1-yy); w[..., 3] = 60
w[:7, :] = TRIM + (255,); w[-7:, :] = TRIM + (255,); w[:, :7] = TRIM + (255,); w[:, -7:] = TRIM + (255,)
Image.fromarray(w).save(f'{OUT}/windscreen.tga')
print('ok')
