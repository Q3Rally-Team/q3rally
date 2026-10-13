"""Paint the stock car texture (STOCKCAR.BMP is lost) for the atlas UVs of build_stockcar.py.
usage: gen_stockcar_textures.py <body.md3> <outdir>

Every texel is mapped back to its position and face normal on the body, so
windows, sticker headlights, grille, tail lights, stripes and the big door and
roof numbers are all placed in model space."""
import os, sys, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md3
from paintlib import PLAIN, RALLY, FONT_NUM, FONT_TXT
from PIL import Image, ImageDraw, ImageFont
import numpy as np, zlib
BODY, OUT = sys.argv[1:3]
R = 1024                                   # painted at 2x, saved at 512

m = md3.load(BODY)
s = [x for x in m.surfs if x.name == 'body'][0]
tags = {n: o for n, o, _ in m.tags[0]}
V = np.array([[c/64 for c in v[:3]] for v in s.frames[0]]); ST = np.array(s.st)
pos = np.zeros((R, R, 3), np.float32); nrm = np.zeros((R, R, 3), np.float32); used = np.zeros((R, R), bool)
tri_id = np.full((R, R), -1)
TRI = []                                   # (centroid, face normal) per triangle
for ti, t in enumerate(s.tris):
    uv = ST[t]*R; p = V[t]
    n = np.cross(p[2]-p[0], p[1]-p[0]); n /= (np.linalg.norm(n) or 1)
    TRI.append((p.mean(0), n))
    x0, y0 = np.floor(uv.min(0)).astype(int) - 1; x1, y1 = np.ceil(uv.max(0)).astype(int) + 1
    x0, y0 = max(x0, 0), max(y0, 0); x1, y1 = min(x1, R-1), min(y1, R-1)
    gx, gy = np.meshgrid(np.arange(x0, x1+1)+0.5, np.arange(y0, y1+1)+0.5)
    (ua, va), (ub, vb), (uc, vc) = uv
    den = (vb-vc)*(ua-uc) + (uc-ub)*(va-vc)
    if abs(den) < 1e-9: continue
    a = ((vb-vc)*(gx-uc) + (uc-ub)*(gy-vc))/den; b = ((vc-va)*(gx-uc) + (ua-uc)*(gy-vc))/den; c = 1-a-b
    inside = (a >= -0.04) & (b >= -0.04) & (c >= -0.04)          # small bleed against seams
    w = np.stack([a, b, c], -1)
    pos[y0:y1+1, x0:x1+1][inside] = (w @ p)[inside]
    nrm[y0:y1+1, x0:x1+1][inside] = n
    tri_id[y0:y1+1, x0:x1+1][inside] = ti
    used[y0:y1+1, x0:x1+1] |= inside
x, y, z = pos[..., 0], pos[..., 1], pos[..., 2]
nx, ny, nz = nrm[..., 0], nrm[..., 1], nrm[..., 2]
front, rear, top = nx > 0.35, nx < -0.35, nz > 0.6
side = np.abs(ny) > 0.5

# ---------------- masks ---------------------------------------------------------------
# windows follow the mesh: whole triangles of the greenhouse
def is_glass(c, n):
    cx, cy, cz = c; nx_, ny_, nz_ = n
    if cz < 2.6 or cz > 12.0: return False
    if abs(ny_) > 0.8 and -12 < cx < 18: return True                      # side windows
    if nx_ > 0.3 and nz_ > 0.3 and abs(cy) < 14 and cx > 0: return True    # windscreen
    if nx_ < -0.3 and nz_ > 0.3 and abs(cy) < 9 and cz > 4.4: return True  # rear window
    return False
glass_tris = np.array([is_glass(c, n) for c, n in TRI] + [False])
glass = used & glass_tris[tri_id]
hl = tags['tag_hlite1']
headl = used & front & ~glass & ((((np.abs(y) - hl[1])/3.4)**2 + ((z - hl[2])/1.8)**2) < 1)
grille = used & front & ~glass & (np.abs(y) < 8.5) & (z > -7.4) & (z < -3.6)
bl, rl = tags['tag_blite2'], tags['tag_rlite2']
# tail lights: band on the vertical upper tail panel
tail = used & (nx < -0.7) & (np.abs(z - bl[2]) < 1.5) & (np.abs(y) > rl[1] - 1.6) & (np.abs(y) < bl[1] + 2.0)
reverse = tail & (np.abs(np.abs(y) - rl[1]) < 1.1)
sill = used & side & (z < -13.2)

def decal(img, msk, u0, v0, width, rgba, U, V):
    """project an RGBA image (width in world units, centred on u0/v0) onto the
    texels in msk; U/V are the world quantities along the image's x/y axes"""
    h, w = rgba.shape[:2]
    iu = (U - u0)/width*w + w/2; iv = (V - v0)/width*w + h/2
    ok = msk & (iu >= 0) & (iu < w) & (iv >= 0) & (iv < h)
    px = rgba[np.clip(iv.astype(int), 0, h-1), np.clip(iu.astype(int), 0, w-1)].astype(np.float32)
    a = (px[..., 3:4]/255)*ok[..., None]
    img[..., :3] = img[..., :3]*(1-a) + px[..., :3]*a
    img[..., 3:4] = np.maximum(img[..., 3:4], 255*a)

def number_img(sc, w=384, h=300):
    im = Image.new('RGBA', (w, h), (0, 0, 0, 0)); d = ImageDraw.Draw(im)
    d.rounded_rectangle([4, 4, w-4, h-4], radius=40, fill=(245, 245, 245, 255), outline=sc['ring'] + (255,), width=16)
    f = ImageFont.truetype(FONT_NUM, int(h*0.72))
    d.text((w/2, h/2 + 6), sc['num'], font=f, fill=sc['ink'] + (255,), anchor='mm', stroke_width=4, stroke_fill=sc['ring'])
    return np.asarray(im)
def text_img(txt, col, h=120):
    f = ImageFont.truetype(FONT_TXT, int(h*0.8)); w = int(f.getlength(txt)) + 30
    im = Image.new('RGBA', (w, h), (0, 0, 0, 0)); ImageDraw.Draw(im).text((w/2, h/2), txt, font=f, fill=col + (255,), anchor='mm')
    return np.asarray(im)

def paint(name, base, sc=None):
    img = np.zeros((R, R, 4), np.float32)
    k = (0.80 + 0.22*np.clip((z + 14)/22, 0, 1))[..., None]
    img[..., :3] = np.array(base, np.float32)*k; img[..., 3] = 215
    if sc:
        st, ed = np.array(sc['stripe'], np.float32), np.array(sc['edge'], np.float32)
        for y0, y1, c in [(-1.4, 6.6, st), (-3.6, -2.4, ed)]:
            msk = used & (top | front | rear) & (y > y0) & (y < y1)
            img[msk, :3] = c*k[msk]
        img[used & side & (z > -9.6) & (z < -7.2), :3] = st
        img[used & side & (z > -10.6) & (z < -10.0), :3] = ed
    img[sill, :3] = (24, 24, 26); img[sill, 3] = 240
    # windows: dark tint, strongly reflective
    img[glass, :3] = np.array((22, 26, 34), np.float32)*np.clip(1.0 + 0.05*(z[glass] - 6), 0.8, 1.3)[..., None]
    img[glass, 3] = 165
    # sticker headlights, grille, tail lights
    img[headl, :3] = (238, 238, 222); img[headl, 3] = 255
    ring = used & front & ~glass & ((((np.abs(y) - hl[1])/3.4)**2 + ((z - hl[2])/1.8)**2) > 0.72) & headl
    img[ring, :3] = (60, 60, 64)
    img[grille, :3] = (16, 16, 18); img[grille, 3] = 245
    mesh = grille & ((np.floor(y*1.4) + np.floor(z*1.8)) % 2 == 0)
    img[mesh, :3] = (46, 46, 52)
    # tail light cluster: black housing, red lamps, white reverse lamp on the inside
    img[tail, :3] = (18, 18, 20); img[tail, 3] = 255
    lamp = tail & (np.abs(z - bl[2]) < 1.05) & (np.abs(np.abs(y) - (bl[1] + rl[1])/2 - 0.6) < 3.1)
    img[lamp, :3] = (215, 20, 16)
    img[lamp & ((np.abs(y)*1.2) % 1.0 < 0.12), :3] = (150, 10, 8)          # lens ribs
    img[reverse & (np.abs(z - bl[2]) < 1.05), :3] = (238, 238, 232)
    if sc:
        num = number_img(sc)
        L, Rr = used & side & (y > 0) & ~glass, used & side & (y < 0) & ~glass
        decal(img, L, 0.5, 2.2, 15.0, num, -x, -z)                    # door number, upright from outside
        decal(img, Rr, -0.5, 2.2, 15.0, num, x, -z)
        roofm = used & top & ~glass & (np.abs(x + 4) < 9)
        decal(img, roofm, 0.0, 4.0, 16.0, num, -y, -x)               # roof: readable from the chase cam (x centre -4)
        tc = tuple(int(c) for c in (sc['stripe'] if sum(sc['body']) < 400 else sc['edge']))
        q3 = text_img('Q3RALLY', tc)
        decal(img, L, 38.0, -1.0, 12.0, q3, -x, -z)
        decal(img, Rr, -38.0, -1.0, 12.0, q3, x, -z)
    img[~used] = (img[used].reshape(-1, 4).mean(0) if used.any() else 0)
    out = Image.fromarray(np.clip(img, 0, 255).astype(np.uint8), 'RGBA').resize((R//2, R//2), Image.LANCZOS)
    out.save(f'{OUT}/{name}.tga')

for n, c in PLAIN.items(): paint(n, c)
paint('white', (232, 232, 236))
for n, sc in RALLY.items(): paint(n, sc['body'], sc)
u = (np.random.RandomState(zlib.crc32(b'under') % 1000).rand(32, 32)*10 + 20).astype(np.uint8)
Image.fromarray(np.dstack([u, u, u])).save(f'{OUT}/under.tga')
print('ok')
