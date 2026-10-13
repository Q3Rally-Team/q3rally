"""F360 skins from the photo texture. usage: gen_f360_textures.py <src red.tga> <body.md3> <outdir>

- clears everything the mesh does not use (watermark, painted rims, tread strip)
- removes the prancing horse and the painted 'PSIONIC' plate (the game's own
  plate model sits on tag_plate)
- recolours the red paint for the other liveries while keeping the photo
  shading and highlights, and adds an alpha channel so the paint reflects
- rally liveries: stripes and number roundels placed in world space through a
  texel -> model position map (the flanks share one mirrored texture region,
  so numbers only go on hood and engine cover)"""
import os, sys, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import md3
from paintlib import PLAIN, RALLY, FONT_NUM
from PIL import Image, ImageDraw, ImageFont
import numpy as np
SRC, BODY, OUT = sys.argv[1:4]

tex = np.asarray(Image.open(SRC).convert('RGB')).astype(np.float32)
H, W = tex.shape[:2]

# ---------------- texel -> world position / face normal ------------------------
m = md3.load(BODY); s = m.surfs[0]
V = np.array([[c/64 for c in v[:3]] for v in s.frames[0]]); ST = np.array(s.st)
pos = np.zeros((H, W, 3), np.float32); nrm = np.zeros((H, W, 3), np.float32); used = np.zeros((H, W), bool)
for t in s.tris:
    uv = ST[t]*[W, H]; p = V[t]
    n = np.cross(p[2]-p[0], p[1]-p[0]); n /= (np.linalg.norm(n) or 1)    # md3 winding
    x0, y0 = np.floor(uv.min(0)).astype(int); x1, y1 = np.ceil(uv.max(0)).astype(int)
    x0, y0 = max(x0, 0), max(y0, 0); x1, y1 = min(x1, W-1), min(y1, H-1)
    if x1 < x0 or y1 < y0: continue
    gx, gy = np.meshgrid(np.arange(x0, x1+1)+0.5, np.arange(y0, y1+1)+0.5)
    (ua, va), (ub, vb), (uc, vc) = uv
    den = (vb-vc)*(ua-uc) + (uc-ub)*(va-vc)
    if abs(den) < 1e-9: continue
    a = ((vb-vc)*(gx-uc) + (uc-ub)*(gy-vc))/den; b = ((vc-va)*(gx-uc) + (ua-uc)*(gy-vc))/den; c = 1-a-b
    inside = (a >= -0.02) & (b >= -0.02) & (c >= -0.02)
    w = np.stack([a, b, c], -1)
    pos[y0:y1+1, x0:x1+1][inside] = (w @ p)[inside]
    nrm[y0:y1+1, x0:x1+1][inside] = n
    used[y0:y1+1, x0:x1+1] |= inside

def dilate(msk, r):
    out = msk.copy()
    for dy in range(-r, r+1):
        for dx in range(-r, r+1):
            out |= np.roll(np.roll(msk, dy, 0), dx, 1)
    return out
def inpaint(img, msk, iters=400):
    """fill msk by repeated neighbour averaging (small areas only)"""
    img = img.copy()
    known = ~msk
    for _ in range(iters):
        acc = np.zeros_like(img); cnt = np.zeros(img.shape[:2], np.float32)
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            k = np.roll(np.roll(known, dy, 0), dx, 1); v = np.roll(np.roll(img, dy, 0), dx, 1)
            acc += v*k[..., None]; cnt += k
        upd = msk & (cnt > 0)
        img[upd] = acc[upd]/cnt[upd][:, None]
        known = known | upd
    return img

# ---------------- clean-up -------------------------------------------------------
keep = dilate(used, 4)
tex[~keep] = (8, 8, 9)
horse = np.zeros((H, W), bool); horse[108:134, 440:460] = True
tex = inpaint(tex, horse & keep)
plate = np.zeros((H, W), bool); plate[84:158, 412:435] = True
tex = inpaint(tex, plate & keep)               # body paint under the game's plate model

# ---------------- paint mask and shading -----------------------------------------
r, g, b = tex[..., 0], tex[..., 1], tex[..., 2]
mx = np.maximum(g, b)
x, y, z = pos[..., 0], pos[..., 1], pos[..., 2]
lights = np.zeros((H, W), bool)                 # the four round tail lights stay red
for ly in (14.6, 19.0, -13.4, -17.9):
    lights |= (x < -52) & (np.hypot(y - ly, z - 2.74) < 2.3)
paint = (((r - mx > 18) & (r > 1.25*mx)) | plate) & keep & ~(lights & used)
rmed = np.median(r[paint])
shadev = np.clip(r/rmed, 0, 1.5)                                # brightness of the paint
spec = np.clip((np.minimum(g, b) - 25)/150, 0, 1)               # pinkish highlights -> white

def livery(rgb):
    out = tex.copy()
    col = np.array(rgb, np.float32)
    base = col[None, None, :]*shadev[..., None]
    new = base*(1-spec[..., None]) + 255*spec[..., None]
    out[paint] = np.clip(new, 0, 255)[paint]
    return out
def stripe(out, msk, rgb):
    col = np.array(rgb, np.float32)
    new = col[None, None, :]*np.clip(shadev, 0, 1.25)[..., None]*(1-spec[..., None]) + 255*spec[..., None]
    sel = msk & paint
    out[sel] = np.clip(new, 0, 255)[sel]

def roundel_img(sc, size=256):
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0)); d = ImageDraw.Draw(img); c = size/2; r = size/2 - 2
    d.ellipse([c-r, c-r*0.86, c+r, c+r*0.86], fill=sc['ring'] + (255,))
    d.ellipse([c-r*0.82, c-r*0.70, c+r*0.82, c+r*0.70], fill=(245, 245, 245, 255))
    f = ImageFont.truetype(FONT_NUM, int(r*(1.05 if len(sc['num']) == 1 else 0.92)))
    d.text((c, c+r*0.02), sc['num'], font=f, fill=sc['ink'] + (255,), anchor='mm')
    return np.asarray(img).astype(np.float32)
def decal_top(out, sc, cx, cy, rad):
    """roundel on an upward face, readable from behind the car (chase cam)"""
    im = roundel_img(sc); n = im.shape[0]
    top = (nrm[..., 2] > 0.6) & keep
    iu = ((cy - pos[..., 1])/(2*rad) + 0.5)*n         # image right = car's right (-y)
    iv = ((cx - pos[..., 0])/(2*rad) + 0.5)*n         # image down  = towards the rear
    ok = top & (iu >= 0) & (iu < n) & (iv >= 0) & (iv < n)
    px = im[np.clip(iv.astype(int), 0, n-1), np.clip(iu.astype(int), 0, n-1)]
    a = (px[..., 3:4]/255)*ok[..., None]
    shade = np.clip(0.75 + 0.25*shadev, 0.75, 1.15)[..., None]
    out[:] = out*(1-a) + np.clip(px[..., :3]*shade, 0, 255)*a

def save(img, name, alpha=True):
    a = np.full((H, W), 255, np.uint8)
    if alpha: a[paint] = 215
    Image.fromarray(np.dstack([np.clip(img, 0, 255).astype(np.uint8), a]), 'RGBA').save(f'{OUT}/{name}.tga')

save(tex, 'red')
for n, c in PLAIN.items():
    if n != 'red': save(livery(c), n)
save(livery((200, 202, 208)), 'silver')
up, side = nrm[..., 2] > 0.55, np.abs(nrm[..., 1]) > 0.55
endf = np.abs(nrm[..., 0]) > 0.55
for n, sc in RALLY.items():
    out = livery(sc['body'])
    y, z = pos[..., 1], pos[..., 2]
    for y0, y1, c in [(-1.2, 7.0, sc['stripe']), (-3.6, -2.4, sc['edge'])]:
        stripe(out, (up | endf) & (y > y0) & (y < y1), c)
    stripe(out, side & (z > -7.4) & (z < -5.2), sc['stripe'])
    stripe(out, side & (z > -8.4) & (z < -7.8), sc['edge'])
    decal_top(out, sc, 38.0, 0.0, 6.0)     # front lid
    decal_top(out, sc, -2.0, 0.0, 6.5)     # roof
    save(out, n)
print('paint texels', int(paint.sum()))
