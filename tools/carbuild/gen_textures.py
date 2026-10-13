"""Paint the Mustang textures. usage: gen_textures.py <body.md3> <outdir>"""
import os, sys, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import md3
from layout import *
from paintlib import *
from PIL import Image, ImageDraw, ImageFont, ImageFilter
import numpy as np
BODY, OUT = sys.argv[1], sys.argv[2]
def boundary_edges(m, names):
    out = []
    for s in m.surfs:
        if s.name not in names: continue
        pos = [tuple(v[:3]) for v in s.frames[0]]
        cnt = {}
        for t in s.tris:
            for a, b in ((t[0], t[1]), (t[1], t[2]), (t[2], t[0])):
                k = tuple(sorted((pos[a], pos[b]))); cnt.setdefault(k, []).append((a, b))
        for k, lst in cnt.items():
            if len(lst) == 1:
                a, b = lst[0]; out.append((s.st[a], s.st[b]))
    return out

m = md3.load(BODY)
EDGES = boundary_edges(m, ('ldoor', 'rdoor', 'hood'))

def paint(name, base, sc=None):
    img = Image.new('RGBA', (N, N), base + (205,))
    dr = ImageDraw.Draw(img)
    # soft vertical shading on the flanks: darker towards the sills
    for side in 'LR':
        for i in range(40):
            z0 = Z0 + (Z1-Z0)*i/40; z1 = Z0 + (Z1-Z0)*(i+1)/40
            k = 0.80 + 0.22*min(1, max(0, (z0+11)/16)) + (0.06 if z0 > 3 else 0)
            rect(dr, side, (X0, z0), (X1, z1), shade(base, k) + (205,))
    # black sills / rocker trim and lower valances
    for side in 'LR':
        rect(dr, side, (X0, Z0), (X1, -9.6), TRIM + (235,))
    rect(dr, 'F', (Y0, Z0), (Y1, -9.0), TRIM + (235,))
    rect(dr, 'B', (Y0, Z0), (Y1, -8.6), TRIM + (235,))

    # hood scoop body (under the stripes)
    poly(dr, 'T', [(31, 4.2), (31, -4.2), (38, -3.4), (38, 3.4)], shade(base, 0.85) + (210,))
    if sc:
        st, ed = sc['stripe'], sc['edge']
        # wide offset racing stripe + thin pin stripe over nose, hood, roof, trunk and tail
        bands = [(-1.2, 6.8, st), (-3.4, -2.2, ed)]
        for y0, y1, c in bands:
            rect(dr, 'T', (X0, y0), (X1, y1), c + (215,))
            rect(dr, 'F', (y0, -9.0), (y1, Z1), c + (215,))
            rect(dr, 'B', (y0, -8.6), (y1, Z1), c + (215,))
        # side stripe along the doors
        for side in 'LR':
            poly(dr, side, [(52, -4.6), (-50, -4.6), (-52, -6.6), (54, -6.6)], st + (215,))
            poly(dr, side, [(52, -7.2), (-52, -7.2), (-52, -7.9), (52, -7.9)], ed + (215,))

    # panel gaps (doors, hood) straight from the mesh outline
    for a, b in EDGES:
        dr.line([(a[0]*N, a[1]*N), (b[0]*N, b[1]*N)], fill=shade(base, 0.35) + (240,), width=K)
    # trunk lid outline
    dr.rectangle([*P('T', -53.6, 17.0), *P('T', -37.2, -17.0)], outline=shade(base, 0.4) + (240,), width=K)
    for side in 'LR':
        # door handle
        rect(dr, side, (-12.6, 1.6), (-9.2, 2.4), (200, 200, 205, 140))
        # rear quarter side scoop (fastback style)
        poly(dr, side, [(-15.9, 1.2), (-21.0, 0.6), (-21.6, -2.6), (-16.2, -1.0)], TRIM + (240,))
        # side markers
        rect(dr, side, (44.0, -3.6), (46.6, -2.6), (250, 150, 20, 255))
        rect(dr, side, (-49.8, -2.4), (-47.2, -1.4), (200, 10, 10, 255))
    # grille with chrome surround, honeycomb mesh and centre bar
    rect(dr, 'F', (-15.2, -4.3), (15.2, 1.2), (165, 168, 175, 150))
    rect(dr, 'F', (-14.5, -3.7), (14.5, 0.6), (14, 14, 16, 245))
    for gy in np.arange(-14.0, 14.5, 1.2):
        for gz in np.arange(-3.4, 0.5, 0.8):
            o = 0.6 if int(round((gz+3.4)/0.8)) % 2 else 0
            x, y = P('F', y=gy+o, z=gz)
            dr.ellipse([x-1.3*K, y-1.0*K, x+1.3*K, y+1.0*K], fill=(48, 48, 54, 245))
    rect(dr, 'F', (-6.5, -1.75), (6.5, -1.15), (190, 192, 198, 150))
    cx, cy = P('F', y=0, z=-1.45); dr.ellipse([cx-2.6*K*SC/2, cy-1.8*K*SC/2, cx+2.6*K*SC/2, cy+1.8*K*SC/2], fill=(190, 192, 198, 150))
    # lower intake
    rect(dr, 'F', (-10.0, -8.6), (10.0, -6.6), (12, 12, 14, 245))
    # black tail panel between the lights with centre fuel cap
    rect(dr, 'B', (-12.4, -2.6), (12.4, 1.6), (18, 18, 20, 245))
    cx, cy = P('B', y=0, z=-0.5); r = 1.5*SC*K
    dr.ellipse([cx-r, cy-r, cx+r, cy+r], fill=(185, 188, 195, 150), outline=(70, 70, 75, 240), width=K)
    # hood scoop opening
    poly(dr, 'T', [(31.6, 3.3), (31.6, -3.3), (33, -3.1), (33, 3.1)], (12, 12, 14, 245))
    # underside patch
    dr.rectangle([R_UNDER[0]*K, R_UNDER[1]*K, N, N], fill=(20, 20, 22, 250))

    if sc:
        # Q3RALLY on the front fenders, numbers on the doors and the roof
        for side in 'LR':
            x, y = P(side, 41.0, z=0.4); text(img, 'Q3RALLY', x, y, 2.3*SC*K, sc['stripe'] if sum(base) < 400 else sc['edge'])
            x, y = P(side, 0.0, z=-1.0); im, pos = roundel(dr, x, y, 5.4*SC*K, sc); img.alpha_composite(im, pos)
        x, y = P('T', -12.0, 0.0); im, pos = roundel(dr, x, y, 7.0*SC*K, sc, rot=-90); img.alpha_composite(im, pos)   # readable from the chase cam
    img = img.resize((int(A), int(A)), Image.LANCZOS)
    img.save(f'{OUT}/{name}.tga')
    return img

for n, c in PLAIN.items(): paint(f'{n}_main', c)
for n, sc in RALLY.items(): paint(f'{n}_main', sc['body'], sc)

# ---------------- shared parts --------------------------------------------------
def save(img, name): img.save(f'{OUT}/{name}.tga')
yy = np.linspace(0, 1, 128)[:, None] * np.ones((1, 128))
# glass: dark tint, a bit lighter at the top, mostly reflective
g = np.zeros((128, 128, 4), np.uint8)
g[..., 0] = 18 + 30*(1-yy); g[..., 1] = 24 + 34*(1-yy); g[..., 2] = 32 + 40*(1-yy); g[..., 3] = 165
save(Image.fromarray(g), 'glass')
# chrome: sky / horizon / ground gradient
c = np.zeros((128, 128, 4), np.uint8)
prof = np.interp(yy, [0, 0.45, 0.5, 0.55, 1], [235, 170, 70, 140, 200])
c[..., 0] = prof; c[..., 1] = prof; c[..., 2] = np.minimum(255, prof*1.05); c[..., 3] = 120
save(Image.fromarray(c), 'chrome')
# headlight: round reflector with chrome bezel
h = Image.new('RGBA', (128, 128), (60, 60, 64, 255)); d = ImageDraw.Draw(h)
d.ellipse([6, 6, 122, 122], fill=(190, 192, 198, 255)); d.ellipse([16, 16, 112, 112], fill=(236, 238, 230, 255))
for r in range(40, 0, -4): d.ellipse([64-r, 64-r, 64+r, 64+r], outline=(205, 208, 200, 255), width=2)
d.ellipse([52, 52, 76, 76], fill=(255, 255, 248, 255))
save(h, 'hlite')
# amber indicator
i = Image.new('RGBA', (64, 32), (255, 150, 20, 255)); d = ImageDraw.Draw(i)
d.rectangle([0, 0, 63, 31], outline=(170, 170, 175, 255), width=3)
for x in range(8, 60, 8): d.line([(x, 4), (x, 28)], fill=(230, 120, 10, 255), width=2)
save(i, 'ilite')
# tail light: three vertical red bars in a chrome bezel (u: outer->inner on both sides)
t = Image.new('RGBA', (128, 64), (170, 172, 178, 255)); d = ImageDraw.Draw(t)
for k in range(3):
    x0 = 6 + k*41; d.rectangle([x0, 6, x0+34, 57], fill=(150, 8, 8, 255))
    d.rectangle([x0+4, 10, x0+30, 53], fill=(210, 20, 16, 255))
    for y in range(14, 52, 6): d.line([(x0+5, y), (x0+29, y)], fill=(180, 12, 10, 255))
save(t, 'tlite')
# underside
u = (np.random.RandomState(7).rand(64, 64)*10 + 22).astype(np.uint8)
save(Image.fromarray(np.dstack([u, u, u+2])).convert('RGB'), 'under')
print('ok')
