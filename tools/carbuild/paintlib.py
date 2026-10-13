"""Shared paint schemes and decal helpers for the car texture builders."""
from PIL import Image, ImageDraw, ImageFont
from layout import *

K = 2                                   # supersampling
N = int(A) * K
TRIM = (22, 22, 25)

def P(region, x=0, y=0, z=0):
    """world point -> supersampled atlas pixel"""
    if region in ('L', 'R'): uv = atlas(region, (x, 0, z))
    elif region == 'T': uv = atlas('T', (x, y, 0))
    else: uv = atlas(region, (0, y, z))
    return (uv[0]*N, uv[1]*N)
def rect(dr, region, a, b, fill, outline=None, width=1):
    """axis aligned world rectangle given by two corners (x,z) / (x,y) / (y,z)"""
    if region in ('L', 'R'): pts = [P(region, a[0], z=a[1]), P(region, b[0], z=b[1])]
    elif region == 'T': pts = [P('T', a[0], a[1]), P('T', b[0], b[1])]
    else: pts = [P(region, y=a[0], z=a[1]), P(region, y=b[0], z=b[1])]
    x0, x1 = sorted([pts[0][0], pts[1][0]]); y0, y1 = sorted([pts[0][1], pts[1][1]])
    dr.rectangle([x0, y0, x1, y1], fill=fill, outline=outline, width=width*K)
def poly(dr, region, pts, fill):
    if region in ('L', 'R'): q = [P(region, x, z=z) for x, z in pts]
    elif region == 'T': q = [P('T', x, y) for x, y in pts]
    else: q = [P(region, y=y, z=z) for y, z in pts]
    dr.polygon(q, fill=fill)


FONT_NUM = '/usr/share/fonts/opentype/inter/Inter-BlackItalic.otf'
FONT_TXT = '/usr/share/fonts/opentype/inter/Inter-ExtraBoldItalic.otf'

PLAIN = {'red': (196, 8, 8), 'blue': (6, 30, 200), 'green': (24, 176, 16), 'yellow': (250, 186, 4)}
# body, stripe, edge, roundel ring, number colour  (colours of the house rally schemes)
RALLY = {
    'rally01': dict(num='1',  body=(34, 35, 42),    stripe=(228, 178, 88),  edge=(150, 152, 160), ring=(228, 178, 88),  ink=(25, 25, 30)),
    'rally07': dict(num='7',  body=(28, 50, 84),    stripe=(238, 240, 242), edge=(105, 160, 225), ring=(238, 240, 242), ink=(28, 50, 84)),
    'rally13': dict(num='13', body=(212, 214, 222), stripe=(236, 112, 30),  edge=(48, 104, 196),  ring=(226, 80, 20),   ink=(25, 25, 30)),
    'rally27': dict(num='27', body=(204, 48, 36),   stripe=(240, 240, 240), edge=(20, 20, 24),    ring=(20, 20, 24),    ink=(20, 20, 24)),
    'rally44': dict(num='44', body=(56, 178, 80),   stripe=(244, 226, 40),  edge=(18, 40, 24),    ring=(230, 220, 30),  ink=(20, 30, 22)),
    'rally88': dict(num='88', body=(100, 56, 180),  stripe=(240, 118, 196), edge=(245, 245, 250), ring=(232, 80, 170),  ink=(36, 16, 70)),
}
def roundel(dr, cx, cy, r, sc, rot=0):
    """white number disc centred at supersampled pixel (cx,cy)"""
    img = Image.new('RGBA', (int(r*2+8), int(r*2+8)), (0, 0, 0, 0)); d = ImageDraw.Draw(img)
    c = r + 4
    d.ellipse([c-r, c-r*0.86, c+r, c+r*0.86], fill=sc['ring'] + (255,))
    d.ellipse([c-r*0.82, c-r*0.70, c+r*0.82, c+r*0.70], fill=(245, 245, 245, 255))
    f = ImageFont.truetype(FONT_NUM, int(r*(1.05 if len(sc['num']) == 1 else 0.92)))
    d.text((c, c+r*0.02), sc['num'], font=f, fill=sc['ink'] + (255,), anchor='mm')
    if rot: img = img.rotate(rot, expand=True, resample=Image.BICUBIC)
    return img, (int(cx - img.width/2), int(cy - img.height/2))

def text(canvas, s, cx, cy, size, col, rot=0):
    f = ImageFont.truetype(FONT_TXT, int(size))
    w = int(f.getlength(s)) + 20; h = int(size*1.6)
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    ImageDraw.Draw(img).text((w/2, h/2), s, font=f, fill=col + (255,), anchor='mm')
    if rot: img = img.rotate(rot, expand=True, resample=Image.BICUBIC)
    canvas.alpha_composite(img, (int(cx - img.width/2), int(cy - img.height/2)))

def shade(rgb, k):
    return tuple(max(0, min(255, int(c*k))) for c in rgb)

