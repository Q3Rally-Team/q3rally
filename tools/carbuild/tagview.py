"""Front and rear orthographic views of a car with its light and plate tags drawn in.

usage: tagview.py <cardir> <skin> <out.png>

Each tag is drawn as a dot with its name and the outline of the glow sprite the
game puts there (gfx/flares: 13.2 units for head/brake lights, 6.6 for reverse).
"""
import math
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md3

CAR, SKIN, OUT = sys.argv[1:4]
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'baseq3r')
SC = 8.0                                     # pixels per unit
GLOW = {'hlite': 13.2, 'blite': 13.2, 'rlite': 6.6, 'plate': 0}
COL = {'hlite': (255, 255, 120), 'blite': (255, 60, 60), 'rlite': (120, 220, 255), 'plate': (120, 255, 120)}


def load_skin(path):
    d = {}
    for line in open(path):
        if ',' in line:
            a, b = line.strip().split(',', 1)
            if b:
                d[a.lower()] = b
    return d


body = md3.load(CAR + '/body.md3')
skin = load_skin('%s/%s.skin' % (CAR, SKIN))
tex = {}


def colour(path):
    if path not in tex:
        p = os.path.join(ROOT, path)
        if not os.path.exists(p):
            p = os.path.join(CAR, os.path.basename(path))
        im = Image.open(p).convert('RGB') if os.path.exists(p) else Image.new('RGB', (2, 2), (255, 0, 255))
        tex[path] = np.asarray(im).astype(np.float32)
    return tex[path]


def view(front):
    """orthographic view along X: looking at the front (from +X) or the rear (from -X)"""
    tris = []
    for s in body.surfs:
        tp = skin.get(s.name.lower())
        if not tp:
            continue
        T = colour(tp)
        P = np.array([[c / 64 for c in v[:3]] for v in s.frames[0]])
        ST = np.array(s.st)
        for t in s.tris:
            p = P[t]
            depth = p[:, 0].mean() * (1 if front else -1)
            uv = ST[t].mean(0) % 1.0
            c = T[int(uv[1] * (T.shape[0] - 1)), int(uv[0] * (T.shape[1] - 1))]
            n = np.cross(p[2] - p[0], p[1] - p[0])
            l = np.linalg.norm(n) or 1
            shade = 0.45 + 0.55 * abs(n[0] / l)
            tris.append((depth, p, tuple(int(x * shade) for x in c)))
    tris.sort(key=lambda x: x[0])
    W, H = int(60 * 2 * SC), int(48 * SC)
    im = Image.new('RGB', (W, H), (60, 60, 64))
    d = ImageDraw.Draw(im)

    def px(y, z):
        return (W / 2 + (-y if front else y) * SC, H - (z + 20) * SC)
    for _, p, c in tris:
        d.polygon([px(q[1], q[2]) for q in p], fill=c)
    for name, o, a in body.tags[0]:
        kind = next((k for k in GLOW if name.startswith('tag_' + k)), None)
        if kind is None:
            continue
        facing_front = kind == 'hlite'
        if facing_front != front and kind != 'plate':
            continue
        if kind == 'plate' and front:
            continue
        x, y = px(o[1], o[2])
        g = GLOW[kind] / 2 * SC
        if g:
            d.rectangle([x - g, y - g, x + g, y + g], outline=COL[kind], width=2)
        d.ellipse([x - 5, y - 5, x + 5, y + 5], fill=COL[kind])
        d.text((x + 7, y - 16), name[4:], fill=COL[kind])
    d.text((8, 8), 'front' if front else 'rear', fill=(255, 255, 255))
    return im


a, b = view(True), view(False)
out = Image.new('RGB', (a.width + b.width, a.height))
out.paste(a, (0, 0))
out.paste(b, (a.width, 0))
out.save(OUT)
