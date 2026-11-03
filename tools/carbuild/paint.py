"""Paint atlases, shared part textures and skins for projection-mapped cars.

A car supplies its atlas layout module (A, SC, X0..Z1, R_UNDER, atlas()) and a
dict of decal positions in world units; see r34/gen_textures.py. All TGAs are
written run-length encoded.
"""
import os

import numpy as np
from PIL import Image, ImageDraw, ImageFont

K = 2                                   # supersampling
FONT_NUM = '/usr/share/fonts/opentype/inter/Inter-BlackItalic.otf'
FONT_TXT = '/usr/share/fonts/opentype/inter/Inter-ExtraBoldItalic.otf'

PLAIN = {'red': (196, 8, 8), 'blue': (10, 36, 196), 'green': (24, 176, 16), 'yellow': (250, 186, 4)}
# house rally colours, same as the Mustang
RALLY = {
    'rally01': dict(num='1',  body=(34, 35, 42),    stripe=(228, 178, 88),  edge=(150, 152, 160), ring=(228, 178, 88),  ink=(25, 25, 30)),
    'rally07': dict(num='7',  body=(28, 50, 84),    stripe=(238, 240, 242), edge=(105, 160, 225), ring=(238, 240, 242), ink=(28, 50, 84)),
    'rally13': dict(num='13', body=(212, 214, 222), stripe=(236, 112, 30),  edge=(48, 104, 196),  ring=(226, 80, 20),   ink=(25, 25, 30)),
    'rally27': dict(num='27', body=(204, 48, 36),   stripe=(240, 240, 240), edge=(20, 20, 24),    ring=(20, 20, 24),    ink=(20, 20, 24)),
    'rally44': dict(num='44', body=(56, 178, 80),   stripe=(244, 226, 40),  edge=(18, 40, 24),    ring=(230, 220, 30),  ink=(20, 30, 22)),
    'rally88': dict(num='88', body=(100, 56, 180),  stripe=(240, 118, 196), edge=(245, 245, 250), ring=(232, 80, 170),  ink=(36, 16, 70)),
}
TAGS = ['tag_weapon', 'tag_wheelfr', 'tag_wheelfl', 'tag_wheelrr', 'tag_wheelrl', 'tag_turbo', 'tag_smoke',
        'tag_plate', 'tag_hlite1', 'tag_hlite2', 'tag_blite1', 'tag_blite2', 'tag_rlite1', 'tag_rlite2']


def save(out, img, name):
    img.save(os.path.join(out, name + '.tga'), compression='tga_rle')


def shade(rgb, k):
    return tuple(max(0, min(255, int(c * k))) for c in rgb)


def roundel(r, sc, rot=0):
    img = Image.new('RGBA', (int(r * 2 + 8), int(r * 2 + 8)), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    c = r + 4
    d.ellipse([c - r, c - r * 0.86, c + r, c + r * 0.86], fill=sc['ring'] + (255,))
    d.ellipse([c - r * 0.82, c - r * 0.70, c + r * 0.82, c + r * 0.70], fill=(245, 245, 245, 255))
    f = ImageFont.truetype(FONT_NUM, int(r * (1.05 if len(sc['num']) == 1 else 0.92)))
    d.text((c, c + r * 0.02), sc['num'], font=f, fill=sc['ink'] + (255,), anchor='mm')
    if rot:
        img = img.rotate(rot, expand=True, resample=Image.BICUBIC)
    return img


def paste(canvas, img, cx, cy):
    canvas.alpha_composite(img, (int(cx - img.width / 2), int(cy - img.height / 2)))


def text(s, size, col, rot=0):
    f = ImageFont.truetype(FONT_TXT, int(size))
    w, h = int(f.getlength(s)) + 20, int(size * 1.6)
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    ImageDraw.Draw(img).text((w / 2, h / 2), s, font=f, fill=col + (255,), anchor='mm')
    return img.rotate(rot, expand=True, resample=Image.BICUBIC) if rot else img


class Painter:
    """paints the projection atlas of one car

    decals (world units):
      flank        (z_dark, z_range, shoulder_lo, shoulder_hi)  side shading
      ends         (z_dark, z_range)                            front/back shading
      stripes      [(y0, y1, 'stripe'|'edge'), ...] over top, front and back
      stripe_front, stripe_back   lowest z of the stripes on the front / back
      side_stripes [(points, 'stripe'|'edge'), ...]  polygons (x, z) on both flanks
      side_text    (x, z, size)    Q3RALLY on both flanks
      door_roundel (x, z, radius)
      roof_roundel (x, y, radius)
      hood_text    (x, size)
    """

    def __init__(self, layout, decals, alpha):
        self.L, self.D, self.alpha = layout, decals, alpha
        self.N = int(layout.A) * K

    def P(self, region, x=0, y=0, z=0):
        """world point -> supersampled atlas pixel"""
        if region in ('L', 'R'):
            uv = self.L.atlas(region, (x, 0, z))
        elif region == 'T':
            uv = self.L.atlas('T', (x, y, 0))
        else:
            uv = self.L.atlas(region, (0, y, z))
        return (uv[0] * self.N, uv[1] * self.N)

    def rect(self, dr, region, a, b, fill):
        P = self.P
        if region in ('L', 'R'):
            pts = [P(region, a[0], z=a[1]), P(region, b[0], z=b[1])]
        elif region == 'T':
            pts = [P('T', a[0], a[1]), P('T', b[0], b[1])]
        else:
            pts = [P(region, y=a[0], z=a[1]), P(region, y=b[0], z=b[1])]
        x0, x1 = sorted([pts[0][0], pts[1][0]])
        y0, y1 = sorted([pts[0][1], pts[1][1]])
        dr.rectangle([x0, y0, x1, y1], fill=fill)

    def poly(self, dr, region, pts, fill):
        P = self.P
        if region in ('L', 'R'):
            q = [P(region, x, z=z) for x, z in pts]
        elif region == 'T':
            q = [P('T', x, y) for x, y in pts]
        else:
            q = [P(region, y=y, z=z) for y, z in pts]
        dr.polygon(q, fill=fill)

    def paint(self, out, name, base, sc=None):
        L, D, ALPHA, N = self.L, self.D, self.alpha, self.N
        X0, X1, Y0, Y1, Z0, Z1, SC = L.X0, L.X1, L.Y0, L.Y1, L.Z0, L.Z1, L.SC
        img = Image.new('RGBA', (N, N), base + (ALPHA,))
        dr = ImageDraw.Draw(img)
        # flanks a little darker towards the sills, a lighter band on the shoulder line
        zd, zr, s0, s1 = D['flank']
        for side in 'LR':
            steps = 48
            for i in range(steps):
                z0 = Z0 + (Z1 - Z0) * i / steps
                z1 = Z0 + (Z1 - Z0) * (i + 1) / steps
                k = 0.78 + 0.24 * min(1, max(0, (z0 - zd) / zr)) + (0.05 if s0 < z0 < s1 else 0)
                self.rect(dr, side, (X0, z0), (X1, z1), shade(base, k) + (ALPHA,))
        zd, zr = D['ends']
        for reg in 'FB':
            for i in range(48):
                z0 = Z0 + (Z1 - Z0) * i / 48
                z1 = Z0 + (Z1 - Z0) * (i + 1) / 48
                self.rect(dr, reg, (Y0, z0), (Y1, z1), shade(base, 0.84 + 0.16 * min(1, max(0, (z0 - zd) / zr))) + (ALPHA,))

        if sc:
            for y0, y1, c in D['stripes']:
                c = sc[c]
                self.rect(dr, 'T', (X0, y0), (X1, y1), c + (ALPHA,))
                self.rect(dr, 'F', (y0, D['stripe_front']), (y1, Z1), c + (ALPHA,))
                self.rect(dr, 'B', (y0, D['stripe_back']), (y1, Z1), c + (ALPHA,))   # same world y as on top
            for side in 'LR':
                for pts, c in D['side_stripes']:
                    self.poly(dr, side, pts, sc[c] + (ALPHA,))
        # underside patch
        dr.rectangle([L.R_UNDER[0] * K, L.R_UNDER[1] * K, N, N], fill=(20, 20, 22, 250))

        if sc:
            st = sc['stripe']
            for side in 'LR':
                x, z, size = D['side_text']
                px, py = self.P(side, x, z=z)
                paste(img, text('Q3RALLY', size * SC * K, sc['stripe'] if sum(base) < 400 else sc['edge']), px, py)
                x, z, r = D['door_roundel']
                px, py = self.P(side, x, z=z)
                paste(img, roundel(r * SC * K, sc), px, py)
            x, y, r = D['roof_roundel']
            px, py = self.P('T', x, y)
            paste(img, roundel(r * SC * K, sc, rot=-90), px, py)   # roof, readable from the chase cam
            x, size = D['hood_text']
            px, py = self.P('T', x, 0.0)
            paste(img, text('Q3RALLY', size * SC * K, sc['ink'] if sum(st) > 400 else (240, 240, 240), rot=-90), px, py)
        img = img.resize((int(L.A), int(L.A)), Image.LANCZOS)
        save(out, img, name)
        return img

    def paint_all(self, out):
        os.makedirs(out, exist_ok=True)
        for n, c in PLAIN.items():
            self.paint(out, n + '_main', c)
        for n, sc in RALLY.items():
            self.paint(out, n + '_main', sc['body'], sc)


def shared_parts(out, names=('glass', 'chrome', 'trim', 'under')):
    """glass, chrome, trim and underside textures"""
    yy = np.linspace(0, 1, 128)[:, None] * np.ones((1, 128))
    # glass: dark tint, nearly opaque (no interior behind it)
    g = np.zeros((128, 128, 4), np.uint8)
    g[..., 0] = 14 + 26 * (1 - yy)
    g[..., 1] = 18 + 30 * (1 - yy)
    g[..., 2] = 26 + 36 * (1 - yy)
    g[..., 3] = 215
    if 'glass' in names:
        save(out, Image.fromarray(g), 'glass')
    # chrome: sky / horizon / ground gradient
    c = np.zeros((128, 128, 4), np.uint8)
    prof = np.interp(yy, [0, 0.45, 0.5, 0.55, 1], [235, 170, 70, 140, 200])
    c[..., 0] = prof
    c[..., 1] = prof
    c[..., 2] = np.minimum(255, prof * 1.05)
    c[..., 3] = 120
    if 'chrome' in names:
        save(out, Image.fromarray(c), 'chrome')
    # trim: satin black plastic
    t = np.zeros((64, 64, 3), np.uint8)
    t[..., :] = (np.random.RandomState(3).rand(64, 64, 1) * 6 + 20).astype(np.uint8)
    if 'trim' in names:
        save(out, Image.fromarray(t), 'trim')
    # underside
    u = (np.random.RandomState(7).rand(64, 64) * 10 + 22).astype(np.uint8)
    if 'under' in names:
        save(out, Image.fromarray(np.dstack([u, u, u + 2])), 'under')


def write_skins(out, pdir, before, paint_surfs, after):
    """one .skin per paint scheme; before/after: [(surface, texture)] around the paint surfaces"""
    for skin in list(PLAIN) + list(RALLY):
        lines = ['%s,%s%s.tga' % (s, pdir, t) for s, t in before]
        lines += ['%s,%s%s_main.tga' % (s, pdir, skin) for s in paint_surfs]
        lines += ['%s,%s%s.tga' % (s, pdir, t) for s, t in after]
        lines += [t + ',' for t in TAGS]
        open(os.path.join(out, skin + '.skin'), 'w').write('\n'.join(lines) + '\n')
