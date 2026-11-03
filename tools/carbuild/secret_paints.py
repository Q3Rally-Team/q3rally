"""Secret paints for the new cars: Retro, Camo, Carbon, Chrome, Neon, Hot Rod, Gold.

usage: secret_paints.py <baseq3r dir> <outdir> [car ...]

The 13 original cars got their secret paints in zz_secret_skins.pk3; this
script makes the same seven paints for the cars built with carbuild
(Mustang, Skyline, GT-R, Ridgeback, Cutlass, GT500, DB12, Bountyhunter). It writes
into <outdir>, laid out like the pk3:

  models/players/<car>/<paint>_main.tga      paint atlas
  models/players/<car>/<paint>_main_tint.tga chrome / gold: tint of the reflection
  models/players/<car>/<paint>_main_glow.tga neon: pulsing glow (additive)
  models/players/<car>/<paint>_main_fl1..6   hot rod: animated flames (blended)
  models/players/<car>/<paint>.skin, icon_<paint>.tga
  scripts/cars_secret_newcars.shader         shaders, to merge into cars_secret.shader

Every pattern is a function of the world position of a texel, so stripes and
camouflage run on over the seams between the atlas regions. The projection
cars get the position from their atlas layout, the Bountyhunter (hand-made
UVs) by rasterising its body surface in texture space.

The painted plain red atlas (red_main / sheriff_main) is the reference: its
red pixels are paint, everything else (panel gaps are dark red and stay
lines, grilles, markers, the underside patch, rust) is kept as it is, and the
brightness of the red carries the shading over to the new paint.
"""
import ast
import importlib.util
import os
import shutil
import subprocess
import sys
import tempfile

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import md3
import paint

K = 2                                       # supersampling of the patterns
SECRETS = ['retro', 'camo', 'carbon', 'chrome', 'neon', 'hotrod', 'gold']
FRAMES = 6                                  # hot rod flame frames

# decals of the Mustang (gen_textures.py paints them inline, not via paint.Painter)
MUSTANG_DECALS = {
    'stripe_front': -9.0,
    'stripe_back': -8.6,
    'side_stripes': [([(52.0, -4.6), (-50.0, -4.6), (-52.0, -6.6), (54.0, -6.6)], 'stripe'),
                     ([(52.0, -7.2), (-52.0, -7.2), (-52.0, -7.9), (52.0, -7.9)], 'edge')],
    'side_text': (41.0, 0.4, 2.3),
    'door_roundel': (0.0, -1.0, 5.4),
    'roof_roundel': (-12.0, 0.0, 7.0),
    'hood_text': None,                      # the hood scoop sits where the text would go
}
# car: (layout module, decals source, wheel skin for the icons)
CARS = {
    'mustang': ('layout.py', None, 'svt_cobra'),
    'skyline': ('r34/layout.py', 'r34/gen_textures.py', 'amg_163'),
    'gtr': ('gtr/layout.py', 'gtr/gen_textures.py', 'amg_163'),
    'ridgeback': ('ridgeback/layout.py', 'ridgeback/gen_textures.py', 'amg_163'),
    'cutlass': ('cutlass/layout.py', 'cutlass/gen_textures.py', 'amg_163'),
    'gt500': ('gt500/layout.py', 'gt500/gen_textures.py', 'svt_cobra'),
    'db12': ('db12/layout.py', 'db12/gen_textures.py', 'amg_163'),
    'bountyhunter': (None, None, 'amg_163'),
}
# Bountyhunter: world-space decal positions (it has no atlas layout)
BH = {'zc': -2.0, 'h': 5.0,                 # flame band on the flanks
      'stripe': (-4.6, -3.0), 'edge': (-6.0, -5.4)}

# ---------------------------------------------------------------- colours
CREAM = (236, 224, 196)
R_BROWN, R_ORANGE, R_YELLOW = (112, 56, 28), (224, 106, 32), (238, 180, 52)
OLIVE, C_GREEN, C_BROWN, C_BLACK, C_SAND = (98, 104, 64), (50, 64, 36), (104, 80, 52), (32, 32, 28), (156, 146, 104)
CARB_RED = (200, 22, 18)
SILVER, SILVER_TINT = (186, 192, 200), (232, 236, 244)
GOLD, GOLD_TINT = (198, 158, 72), (247, 218, 154)
INK = (18, 18, 20)
NEON_BASE, CYAN, MAGENTA = (12, 12, 18), (40, 236, 255), (255, 56, 214)
HOT_BASE = (16, 11, 18)
FL_EDGE, FL_RED, FL_ORANGE, FL_YELLOW = (70, 6, 0), (214, 52, 10), (250, 138, 24), (255, 222, 80)
# paint opacity over the reflection (the 13 old cars: chrome 40, gold 86, decals 240)
ALPHA_LOW = {'chrome': 40, 'gold': 86}
ALPHA_DECAL = 240


def load_module(path, name):
    spec = importlib.util.spec_from_file_location(name, os.path.join(HERE, path))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def read_decals(path):
    """DECALS dict of a gen_textures.py, without running the script"""
    tree = ast.parse(open(os.path.join(HERE, path)).read())
    for node in tree.body:
        if isinstance(node, ast.Assign) and getattr(node.targets[0], 'id', '') == 'DECALS':
            return ast.literal_eval(node.value)
    raise SystemExit('no DECALS in ' + path)


# ---------------------------------------------------------------- geometry
class Geo:
    """world position (X, Y, Z) and region per supersampled texel.
    reg: 0 unused, 1 flank, 2 top, 3 front, 4 back"""

    def __init__(self, N):
        self.N = N
        self.X = np.zeros((N, N), np.float32)
        self.Y = np.zeros((N, N), np.float32)
        self.Z = np.zeros((N, N), np.float32)
        self.reg = np.zeros((N, N), np.uint8)


def geo_from_layout(L):
    N = int(L.A) * K
    g = Geo(N)
    v, u = np.mgrid[:N, :N].astype(np.float32) / K + 0.5 / K      # atlas pixels
    S = L.SC

    def box(r0, w, h):
        return (u >= r0[0]) & (u < r0[0] + w * S) & (v >= r0[1]) & (v < r0[1] + h * S)
    lx, wy, hz = L.X1 - L.X0, L.Y1 - L.Y0, L.Z1 - L.Z0
    for code, r0, w, h in (('L', L.R_LSIDE, lx, hz), ('R', L.R_RSIDE, lx, hz), ('T', L.R_TOP, lx, wy),
                           ('F', L.R_FRONT, wy, hz), ('B', L.R_BACK, wy, hz)):
        m = box(r0, w, h)
        du, dv = (u[m] - r0[0]) / S, (v[m] - r0[1]) / S
        if code == 'L':
            g.X[m], g.Y[m], g.Z[m], g.reg[m] = L.X1 - du, L.Y1, L.Z1 - dv, 1
        elif code == 'R':
            g.X[m], g.Y[m], g.Z[m], g.reg[m] = L.X0 + du, L.Y0, L.Z1 - dv, 1
        elif code == 'T':
            g.X[m], g.Y[m], g.Z[m], g.reg[m] = L.X0 + du, L.Y1 - dv, L.Z1, 2
        elif code == 'F':
            g.X[m], g.Y[m], g.Z[m], g.reg[m] = L.X1, L.Y0 + du, L.Z1 - dv, 3
        else:
            g.X[m], g.Y[m], g.Z[m], g.reg[m] = L.X0, L.Y1 - du, L.Z1 - dv, 4
    g.bounds = (L.X0, L.X1, L.Y0, L.Y1, L.Z0, L.Z1)
    g.ppu = S * K                                                  # texels per world unit
    return g


def geo_from_uvs(body, surf, size):
    """rasterise one md3 surface in texture space (mirrored UVs keep |y|)"""
    N = size * K
    g = Geo(N)
    s = [s for s in body.surfs if s.name == surf][0]
    V = np.array([[c / 64 for c in v[:3]] for v in s.frames[0]], np.float32)
    ST = np.array(s.st, np.float32) * N
    for a, b, c in s.tris:
        P = V[[a, b, c]]
        n = np.cross(P[2] - P[0], P[1] - P[0])           # md3 winding is clockwise
        ln = np.linalg.norm(n)
        if ln < 1e-9:
            continue
        n /= ln
        if abs(n[2]) >= max(abs(n[0]), abs(n[1])):
            # the source roof faces down; above the belt line it is still the top
            reg = 2 if n[2] > 0 or P[:, 2].mean() > 0 else 0
        elif abs(n[1]) >= abs(n[0]):
            reg = 1
        else:
            reg = 3 if n[0] > 0 else 4
        xs, ys = ST[[a, b, c], 0], ST[[a, b, c], 1]
        den = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2])
        if abs(den) < 1e-9:
            continue
        x0, x1 = max(int(xs.min()) - 1, 0), min(int(xs.max()) + 2, N)
        y0, y1 = max(int(ys.min()) - 1, 0), min(int(ys.max()) + 2, N)
        if x1 <= x0 or y1 <= y0:
            continue
        gy, gx = np.mgrid[y0:y1, x0:x1].astype(np.float32) + 0.5
        w0 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / den
        w1 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / den
        w2 = 1 - w0 - w1
        ins = (w0 >= -0.02) & (w1 >= -0.02) & (w2 >= -0.02)    # a little bleed over the edges
        if not ins.any():
            continue
        p = w0[..., None] * P[0] + w1[..., None] * P[1] + w2[..., None] * P[2]
        sl = (slice(y0, y1), slice(x0, x1))
        g.X[sl][ins], g.Y[sl][ins], g.Z[sl][ins] = p[..., 0][ins], np.abs(p[..., 1][ins]), p[..., 2][ins]
        g.reg[sl][ins] = reg
    lo, hi = V.min(0), V.max(0)
    g.bounds = (lo[0], hi[0], -hi[1], hi[1], lo[2], hi[2])
    g.ppu = N / 112.0
    return g


# ---------------------------------------------------------------- patterns
def band(lo, hi, a):
    return (a >= lo) & (a <= hi)


def side_band(pts, X, Z, grow=0.0):
    """inside a side stripe polygon [(xf, z top), (xb, z top), (xb, z bottom), (xf, z bottom)]"""
    (xa, za), (xb, zb), (xc, zc), (xd, zd) = pts
    t = np.clip((X - xa) / (xb - xa), 0, 1)
    top = za + (zb - za) * t
    t2 = np.clip((X - xd) / (xc - xd), 0, 1)
    bot = zd + (zc - zd) * t2
    xmin, xmax = min(xa, xb, xc, xd), max(xa, xb, xc, xd)
    return (X >= xmin - grow) & (X <= xmax + grow) & (Z <= top + grow) & (Z >= bot - grow), top, bot


class Car:
    """world-space decal positions shared by the patterns"""

    def __init__(self, g, D):
        self.g = g
        X0, X1, Y0, Y1, Z0, Z1 = g.bounds
        self.len = X1 - X0
        if D is not None:
            self.stripe = D['side_stripes'][0][0]
            self.edge = D['side_stripes'][1][0]
            self.front_z, self.back_z = D['stripe_front'], D['stripe_back']
            self.zc, self.h = D['door_roundel'][1], 5.5
        else:                               # Bountyhunter
            (a, b), (c, d) = BH['stripe'], BH['edge']
            self.stripe = [(X1, b), (X0, b), (X0, a), (X1, a)]
            self.edge = [(X1, d), (X0, d), (X0, c), (X1, c)]
            self.front_z = self.back_z = Z0
            self.zc, self.h = BH['zc'], BH['h']

    def top(self):
        """texels that carry the stripes running over nose, roof and tail"""
        g = self.g
        return (g.reg == 2) | ((g.reg == 3) & (g.Z >= self.front_z)) | ((g.reg == 4) & (g.Z >= self.back_z))

    def lines_top(self, bands):
        return self.top() & np.any([band(lo, hi, np.abs(self.g.Y)) for lo, hi in bands], 0)

    def side(self, pts, grow=0.0):
        g = self.g
        m, top, bot = side_band(pts, g.X, g.Z, grow)
        return (g.reg == 1) & m, top, bot

    def side_line(self, pts, which, w):
        g = self.g
        _, top, bot = side_band(pts, g.X, g.Z, 0)
        z = top if which == 'top' else bot
        xs = [p[0] for p in pts]
        return (g.reg == 1) & (np.abs(g.Z - z) <= w / 2) & (g.X >= min(xs)) & (g.X <= max(xs))


def fill(shape, col):
    a = np.empty(shape + (3,), np.float32)
    a[...] = col
    return a


def put(img, m, col):
    img[m] = col


def wave_noise(g, scale, seed, octaves=2):
    """smooth 3d noise from random plane waves, the same across region seams"""
    rs = np.random.RandomState(seed)
    P = np.stack([g.X, g.Y, g.Z], -1)
    n = np.zeros(g.X.shape, np.float32)
    amp = 1.0
    for o in range(octaves):
        for _ in range(7):
            d = rs.normal(size=3)
            d /= np.linalg.norm(d)
            n += amp * np.sin(P @ (d * (2 * np.pi / scale)) + rs.rand() * 6.28)
        scale /= 2.1
        amp *= 0.5
    return n


def weave(g):
    """2x2 carbon weave in the plane of each region"""
    a = np.where(g.reg == 2, g.X, np.where(g.reg == 1, g.X, g.Y))
    b = np.where(g.reg == 1, g.Z, np.where(g.reg == 2, g.Y, g.Z))
    c = 0.85                                                     # tow width in world units
    ia, ib = np.floor(a / c), np.floor(b / c)
    fa, fb = a / c - ia, b / c - ib
    d = ((ia + ib) % 2).astype(bool)
    f = np.where(d, fa, fb)
    tone = 22 + 26 * np.sin(np.pi * f) * np.where(d, 1.0, 0.8)
    return np.stack([tone, tone, tone + 3], -1)


def flames(car, frame):
    """flame overlay (rgb, alpha) for one animation frame"""
    g = car.g
    X0, X1, Y0, Y1, Z0, Z1 = g.bounds
    rgb = np.zeros(g.X.shape + (3,), np.float32)
    al = np.zeros(g.X.shape, np.float32)
    ph = 2 * np.pi * frame / FRAMES
    xf = X1 + 1.0

    def tongues(sel, q, centres, h, lens):
        s = xf - g.X
        w0 = h * 0.42
        layers = [(1.14, 1.04, FL_EDGE), (1.0, 1.0, FL_RED), (0.64, 0.84, FL_ORANGE), (0.30, 0.62, FL_YELLOW)]
        for wscale, lscale, col in layers:
            inside = np.zeros(g.X.shape, bool)
            for j, (c, L) in enumerate(zip(centres, lens)):
                L = L * lscale * (1 + 0.09 * np.sin(ph + j * 1.7))
                t = s / L
                cen = c + h * 0.24 * np.sin(np.pi * 1.3 * t + j * 2.1 + ph) * t
                hw = w0 * wscale * np.clip(1 - t, 0, 1) ** 0.85
                inside |= (t >= 0) & (t <= 1) & (np.abs(q - cen) < hw)
            # the root: solid fire at the front end
            inside |= (s >= 0) & (s < car.len * 0.07 * lscale) & (np.abs(q - np.mean(centres)) < (max(centres) - min(centres)) / 2 + w0 * wscale)
            m = sel & inside
            rgb[m] = col
            al[m] = 1
    # flanks
    zc, h = car.zc, car.h
    cz = [zc + h * k for k in (-0.75, -0.38, 0.0, 0.38, 0.75)]
    tongues(g.reg == 1, g.Z, cz, h, [car.len * k for k in (0.28, 0.42, 0.52, 0.40, 0.27)])
    # bonnet
    hy = (Y1 - Y0) / 2 * 0.62
    cy = [hy * k for k in (-1.0, -0.6, -0.2, 0.2, 0.6, 1.0)]
    tongues(g.reg == 2, g.Y, cy, hy * 0.5, [car.len * k for k in (0.17, 0.25, 0.30, 0.30, 0.25, 0.17)])
    return rgb, al


def pattern(name, car):
    """paint colour per texel (rgb float), decal mask, extra layers"""
    g = car.g
    sh = g.X.shape
    dec = np.zeros(sh, bool)
    extra = {}
    if name == 'retro':
        img = fill(sh, CREAM)
        put(img, car.lines_top([(3.8, 4.6)]), R_YELLOW)
        put(img, car.lines_top([(2.0, 3.4)]), R_ORANGE)
        put(img, car.lines_top([(0.0, 1.6)]), R_BROWN)
        s, top, _ = car.side(car.stripe)
        put(img, s, R_ORANGE)
        put(img, car.side(car.edge)[0], R_BROWN)
        yel = (g.reg == 1) & (g.Z > top + 0.45) & (g.Z < top + 1.25) & car.side(car.stripe, grow=2.0)[0]
        put(img, yel, R_YELLOW)
    elif name == 'camo':
        img = fill(sh, OLIVE)
        for seed, col, q in ((11, C_SAND, 0.86), (12, C_GREEN, 0.62), (13, C_BROWN, 0.70), (14, C_BLACK, 0.84)):
            n = wave_noise(g, 26.0, seed)
            put(img, (n > np.quantile(n[g.reg > 0], q)) & (g.reg > 0), col)
    elif name == 'carbon':
        img = weave(g)
        m = car.lines_top([(2.6, 3.0)]) | car.side(car.edge)[0]
        put(img, m, CARB_RED)
        dec |= m
    elif name in ('chrome', 'gold'):
        img = fill(sh, SILVER if name == 'chrome' else GOLD)
        m = car.lines_top([(2.5, 3.0), (3.4, 3.65)]) | car.side(car.edge)[0] | car.side_line(car.stripe, 'top', 0.45)
        put(img, m, INK)
        dec |= m
    elif name == 'neon':
        img = fill(sh, NEON_BASE)
        glow = np.zeros(sh + (3,), np.float32)
        for m, col in ((car.lines_top([(3.0, 3.5)]) | car.side_line(car.stripe, 'top', 0.5), CYAN),
                       (car.lines_top([(4.4, 4.8)]) | car.side_line(car.edge, 'bottom', 0.5), MAGENTA)):
            put(img, m, col)
            put(glow, m, col)
            dec |= m
        extra['glow'] = glow
    elif name == 'hotrod':
        img = fill(sh, HOT_BASE)
        m = car.side_line(car.edge, 'bottom', 0.3)
        put(img, m, FL_ORANGE)
        dec |= m
        extra['flames'] = [flames(car, f) for f in range(FRAMES)]
    return img, dec, extra


# ---------------------------------------------------------------- decals
def star_roundel(r, ring, star, inner=None):
    s = int(r * 2 + 8)
    img = Image.new('RGBA', (s, s), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    c = r + 4
    if inner:
        d.ellipse([c - r, c - r, c + r, c + r], fill=inner + (255,))
    d.ellipse([c - r, c - r, c + r, c + r], outline=ring + (255,), width=max(2, int(r * 0.13)))
    pts = []
    for i in range(10):
        a = -np.pi / 2 + i * np.pi / 5
        rr = r * (0.70 if i % 2 == 0 else 0.28)
        pts.append((c + rr * np.cos(a), c + rr * np.sin(a)))
    d.polygon(pts, fill=star + (255,))
    return img


def decals(name, pp, D):
    """roundels and lettering, painted in atlas pixels (projection cars only)"""
    N = pp.N
    lay = Image.new('RGBA', (N, N), (0, 0, 0, 0))
    SC = pp.L.SC
    if name == 'retro':
        sc = dict(num='77', ring=R_BROWN, ink=R_BROWN)
        rd = lambda r, rot=0: paint.roundel(r, sc, rot)
        txt = R_BROWN
    elif name == 'camo':
        rd = lambda r, rot=0: star_roundel(r, C_SAND, (236, 232, 214))
        txt = C_SAND
    elif name == 'gold':
        rd = lambda r, rot=0: star_roundel(r, INK, INK)
        txt = INK
    else:
        rd = None
        txt = {'carbon': CARB_RED, 'chrome': INK, 'neon': CYAN, 'hotrod': FL_ORANGE}[name]
    word = 'Q3R' if name == 'camo' else 'Q3RALLY'
    for side in 'LR':
        x, z, size = D['side_text']
        paint.paste(lay, paint.text(word, size * SC * K, txt), *pp.P(side, x, z=z))
        if rd:
            x, z, r = D['door_roundel']
            paint.paste(lay, rd(r * SC * K), *pp.P(side, x, z=z))
    if rd:
        x, y, r = D['roof_roundel']
        paint.paste(lay, rd(r * SC * K, -90) if name == 'retro' else rd(r * SC * K), *pp.P('T', x, y))
    if D.get('hood_text') and name != 'camo':
        x, size = D['hood_text']
        paint.paste(lay, paint.text(word, size * SC * K, txt, rot=-90), *pp.P('T', x, 0.0))
    return np.asarray(lay).astype(np.float32) / 255


# ---------------------------------------------------------------- output
def down(a, A):
    """supersampled float array (N,N[,c]) -> atlas resolution"""
    if a.ndim == 2:
        a = a[..., None]
    N = a.shape[0]
    f = N // A
    return a.reshape(A, f, A, f, a.shape[-1]).mean((1, 3))


def save_tga(path, arr):
    arr = np.clip(arr + 0.5, 0, 255).astype(np.uint8)
    Image.fromarray(arr, 'RGBA' if arr.shape[-1] == 4 else 'RGB').save(path, compression='tga_rle')


def reference(car, base):
    """paint mask, shading and the original pixels of the plain reference paint"""
    if car == 'bountyhunter':
        ref = np.asarray(Image.open(os.path.join(base, 'models/players/bountyhunter/sheriff_main.tga')).convert('RGB')).astype(np.float32)
        # same weighting as bountyhunter/gen_textures.py: grey paint, not the kept areas
        src = ref / 255
        mx, mn = src.max(-1), src.min(-1)
        sat = np.where(mx > 0, (mx - mn) / np.maximum(mx, 1e-6), 0)
        val = mx
        keep = np.zeros(sat.shape, bool)
        H, W = sat.shape
        for u0, v0, u1, v1 in BH_KEEP:
            keep[int(v0 * H):int(v1 * H), int(u0 * W):int(u1 * W)] = True
        m = np.clip((0.32 - sat) / 0.12, 0, 1) * ~keep
        t = np.clip((val - 0.18) / 0.55, 0, 1)
        k = np.clip(val / np.where(t > 0.5, 0.85, 0.12), 0.25, 1.4)
        a = np.full(sat.shape, 255.0)
        return m, k, ref, a, t
    ref = np.asarray(Image.open(os.path.join(base, 'models/players/%s/red_main.tga' % car)).convert('RGBA')).astype(np.float32)
    R, G, B, a = ref[..., 0], ref[..., 1], ref[..., 2], ref[..., 3]
    sat = (R - np.maximum(G, B)) / np.maximum(R, 1)
    m = np.clip((sat - 0.55) / 0.3, 0, 1) * (R > 6)
    k = np.clip(R / 196.0, 0, 1.3)
    return m, k, ref[..., :3], a, None


# texture areas of the Bountyhunter that keep their colours (bountyhunter/gen_textures.py)
BH_KEEP = [(0.75, 0.75, 1.00, 1.00), (0.84, 0.66, 1.00, 0.75), (0.72, 0.56, 0.82, 0.78),
           (0.30, 0.74, 0.39, 0.82), (0.61, 0.74, 0.70, 0.82), (0.37, 0.75, 0.63, 0.81),
           (0.31, 0.83, 0.55, 0.90), (0.44, 0.04, 0.50, 0.17), (0.44, 0.29, 0.50, 0.43),
           (0.40, 0.16, 0.46, 0.28)]
# Bountyhunter: colour of the white door/roof panels per paint (the dark body takes the pattern)
BH_LIGHT = {'retro': R_ORANGE, 'camo': C_SAND, 'carbon': CARB_RED, 'chrome': (222, 226, 234),
            'neon': (22, 22, 32), 'hotrod': (120, 14, 10), 'gold': (30, 28, 24)}


def build_car(car, base, out):
    lay_path, dec_path, rim = CARS[car]
    pdir = 'models/players/%s/' % car
    odir = os.path.join(out, pdir)
    os.makedirs(odir, exist_ok=True)
    if lay_path:
        L = load_module(lay_path, car + '_layout')
        D = MUSTANG_DECALS if dec_path is None else read_decals(dec_path)
        g = geo_from_layout(L)
        pp = paint.Painter(L, D, 255)
        A = int(L.A)
        ref_skin = 'red'
    else:
        D, pp = None, None
        body = md3.load(os.path.join(base, pdir, 'body.md3'))
        A = 512
        g = geo_from_uvs(body, 'body', A)
        ref_skin = 'sheriff'
    car_geo = Car(g, D)
    m, k, ref, ref_a, t = reference(car, base)
    shaders = ['// ' + car.upper() + ' - secret paints (tools/carbuild/secret_paints.py)\n']
    for name in SECRETS:
        img, dec, extra = pattern(name, car_geo)
        img, dec = down(img, A), down(dec.astype(np.float32), A)[..., 0]
        if pp is not None:
            lay = down(decals(name, pp, D), A)
            la = lay[..., 3:4]
            img = img * (1 - la) + lay[..., :3] * 255 * la
            dec = np.maximum(dec, la[..., 0])
        else:
            light = np.array(BH_LIGHT[name], np.float32)
            img = img * (1 - t[..., None]) + light * t[..., None]
        mm = m[..., None]
        rgb = mm * np.clip(img * k[..., None], 0, 255) + (1 - mm) * ref
        if name in ALPHA_LOW:
            pa = ALPHA_LOW[name] * (1 - dec) + ALPHA_DECAL * dec
            alpha = m * pa + (1 - m) * ref_a
        else:
            alpha = ref_a
        tex = name + '_main'
        save_tga(os.path.join(odir, tex + '.tga'), np.dstack([rgb, alpha]))
        stages = []
        if name in ('chrome', 'gold'):
            tint = np.array(SILVER_TINT if name == 'chrome' else GOLD_TINT, np.float32)
            tt = mm * tint + (1 - mm) * 255
            save_tga(os.path.join(odir, tex + '_tint.tga'), np.dstack([tt, np.full(m.shape, 255.0)]))
        if name == 'neon':
            gl = extra['glow']
            gi = Image.fromarray(np.clip(gl, 0, 255).astype(np.uint8))
            blur = np.asarray(gi.filter(ImageFilter.GaussianBlur(g.ppu * 0.55))).astype(np.float32)
            gl = np.clip(gl * 0.8 + blur * 1.8, 0, 255)
            gl = down(gl, A) * mm
            save_tga(os.path.join(odir, tex + '_glow.tga'), np.dstack([gl, np.full(m.shape, 255.0)]))
        if name == 'hotrod':
            for f, (frgb, fal) in enumerate(extra['flames']):
                frgb, fal = down(frgb, A), down(fal, A)[..., 0] * m
                frgb = np.clip(frgb * np.clip(k, 0.6, 1.2)[..., None], 0, 255)
                save_tga(os.path.join(odir, '%s_fl%d.tga' % (tex, f + 1)), np.dstack([frgb, fal * 255]))
        shaders.append(shader(pdir, tex, name, car == 'bountyhunter'))
        # skin: the plain reference skin with the paint swapped
        lines = open(os.path.join(base, pdir, ref_skin + '.skin')).read()
        open(os.path.join(odir, name + '.skin'), 'w').write(lines.replace(ref_skin + '_main.tga', tex + '.tga'))
    return '\n'.join(shaders)


def shader(pdir, tex, name, opaque):
    p = pdir + tex
    refl = ('\t{\n\t\tmap $dynamicreflection\n\t\ttcGen environment\n\t\trgbGen lightingdiffuse\n\t}\n')
    paint_stage = ('\t{\n\t\tmap %s.tga\n\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA\n'
                   '\t\tdepthWrite\n\t\trgbGen lightingdiffuse\n\t}\n' % p)
    s = '%s\n{\n' % p
    if opaque and name not in ('chrome', 'gold'):
        # the Bountyhunter's paint is matt and opaque like its sheriff texture
        if name in ('retro', 'camo', 'carbon'):
            return ''
        s += '\t{\n\t\tmap %s.tga\n\t\trgbGen lightingdiffuse\n\t}\n' % p
    else:
        s += refl
        if name in ('chrome', 'gold'):
            s += '\t{\n\t\tmap %s_tint.tga\n\t\tblendFunc GL_DST_COLOR GL_ZERO\n\t\trgbGen identity\n\t}\n' % p
        s += paint_stage
    if name == 'neon':
        s += '\t{\n\t\tmap %s_glow.tga\n\t\tblendFunc add\n\t\trgbGen wave sin 0.65 0.35 0 0.6\n\t}\n' % p
    if name == 'hotrod':
        s += ('\t{\n\t\tanimMap 8 %s\n\t\tblendFunc blend\n\t\trgbGen lightingDiffuse\n\t}\n'
              % ' '.join('%s_fl%d.tga' % (p, f) for f in range(1, FRAMES + 1)))
    if name == 'gold':
        s += ('\t{\n\t\tmap gfx/secret/glint.tga\n\t\tblendFunc GL_DST_COLOR GL_ONE\n\t\ttcGen environment\n'
              '\t\ttcMod scroll 0.09 0.03\n\t\trgbGen identity\n\t}\n')
    return '// %s %s (secret paint)\n' % (pdir.split('/')[2].upper(), name.upper()) + s + '}\n'


def icons(car, base, out, badges):
    """menu icons with the star badge of the paint, rendered like the plain icons"""
    lay_path, dec_path, rim = CARS[car]
    pdir = 'models/players/%s/' % car
    odir = os.path.join(out, pdir)
    for name in SECRETS:
        tmp = tempfile.mkdtemp()
        for f in ('body.md3', 'wheel.md3'):
            shutil.copy(os.path.join(base, pdir, f), tmp)
        shutil.copy(os.path.join(odir, name + '.skin'), tmp)
        # what the shader shows, baked into one texture for the software renderer
        im = np.asarray(Image.open(os.path.join(odir, name + '_main.tga')).convert('RGBA')).astype(np.float32)
        if name in ('chrome', 'gold'):
            tint = np.asarray(Image.open(os.path.join(odir, name + '_main_tint.tga')).convert('RGB')).astype(np.float32)
            low = im[..., 3] < 200
            im[..., :3][low] = (im[..., :3] * 0.55 + tint * 0.45)[low]
            im[..., 3][low] = 150 if name == 'chrome' else 205
        if name == 'neon':
            gl = np.asarray(Image.open(os.path.join(odir, name + '_main_glow.tga')).convert('RGB')).astype(np.float32)
            im[..., :3] = np.clip(im[..., :3] + gl * 0.8, 0, 255)
        if name == 'hotrod':
            fl = np.asarray(Image.open(os.path.join(odir, name + '_main_fl1.tga')).convert('RGBA')).astype(np.float32)
            fa = fl[..., 3:4] / 255
            im[..., :3] = im[..., :3] * (1 - fa) + fl[..., :3] * fa
        Image.fromarray(im.astype(np.uint8)).save(os.path.join(tmp, name + '_main.tga'))
        png = os.path.join(tmp, 'icon.png')
        env = dict(os.environ, Q3R_FIT='1', Q3R_BASE=base)
        subprocess.run([sys.executable, os.path.join(HERE, 'render3d.py'), tmp, name, rim, png, '384', '200,60,45',
                        os.path.join(badges, 'icon_%s.tga' % name)], check=True, env=env)
        Image.open(png).convert('RGBA').save(os.path.join(odir, 'icon_%s.tga' % name), compression='tga_rle')
        shutil.rmtree(tmp)


def main():
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    base, out = sys.argv[1], sys.argv[2]
    cars = sys.argv[3:] or list(CARS)
    badges = os.environ.get('Q3R_BADGES', os.path.join(base, 'models/players/sidepipe'))
    os.makedirs(os.path.join(out, 'scripts'), exist_ok=True)
    text = []
    for car in cars:
        text.append(build_car(car, base, out))
        if not os.environ.get('Q3R_NO_ICONS'):
            icons(car, base, out, badges)
        print(car, 'ok')
    open(os.path.join(out, 'scripts', 'cars_secret_newcars.shader'), 'w').write('\n'.join(text))


if __name__ == '__main__':
    main()
