#!/usr/bin/env python3
"""Generate q3r_ridgepass: a snowy mountain pass circuit (successor of Ridge Racer).

One closed lap shaped like a figure eight. The start/finish straight lies in
the valley with grandstands and the pit building; the climb runs through an
underpass and three switchback hairpins up to the ridge, then fast bends with
a crest along the ridge, the descent through an avalanche gallery and over a
bridge across the climb back into the valley.

The ground is one height field: a mountain landscape that is cut and filled to
the road (rock cuttings above, snow embankments below, plowed snow banks at the
edges). Ground layers are blended per vertex with a q3map2 index map; collision
is closed triangular detail brushes on a 64/128/256/512-unit grid, fine only
near the road. Tyre friction comes from the parent shaders of the brush sides.

Checkpoints sit in front of every corner. Pickups (weapons, ammo, armour,
health, NOS) make the racing deathmatch modes work; plain races remove the
weapons by themselves.

Writes the .map, index map, shader script, arena, intro camera route,
generated textures, an overview image and docs/maps/ridgepass/validation.json.
Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from itertools import combinations
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_ridgepass"
TEX = "q3r_ridgepass/"
DETAIL = 134217728
STEP = 64
FLOOR = -384
SAMPLE = 32
SHOULDER = 48           # flat slush strip beside the asphalt
CUT, FILL = 1.05, 0.68   # side slopes (rise per unit) of rock cuttings and snow embankments

# centre line control points (closed loop), starting on the start/finish straight
CTRL = [(-1300, -2300), (0, -2300), (1200, -2300), (1750, -2100), (1950, -1550), (1550, -1000), (800, -450), (0, 0), (-750, 450), (-1300, 700), (-1850, 700), (-2133, 817), (-2250, 1100), (-2133, 1383), (-1850, 1500), (-1150, 1500), (-450, 1500), (-167, 1617), (-50, 1900), (-167, 2183), (-450, 2300), (-1150, 2300), (-1850, 2300), (-2133, 2417), (-2250, 2700), (-2133, 2983), (-1850, 3100), (-1100, 3250), (-300, 3450), (450, 3300), (1000, 3450), (1650, 3350), (2200, 3000), (2350, 2250), (1950, 1500), (1200, 700), (0, 0), (-850, -500), (-1550, -950), (-1950, -1500), (-1850, -2100)]

# keyframes along the lap (fraction of its length)
HALF_WIDTH = [(0.00, 280), (0.10, 280), (0.16, 250), (0.22, 230), (0.30, 225), (0.32, 235), (0.36, 235),
              (0.40, 220), (0.42, 235), (0.45, 235), (0.49, 220), (0.51, 235), (0.545, 235), (0.57, 215),
              (0.69, 215), (0.72, 235), (0.74, 215), (0.80, 215), (0.86, 225), (0.94, 260), (1.00, 280)]
ROAD_Z = [(0.00, 0), (0.09, 0), (0.15, 20), (0.20, 60), (0.2316, 110), (0.30, 190), (0.337, 240), (0.36, 260),
          (0.433, 420), (0.46, 450), (0.527, 600), (0.55, 650), (0.60, 800), (0.63, 860), (0.66, 850),
          (0.70, 820), (0.75, 700), (0.80, 540), (0.861, 410), (0.91, 240), (0.975, 40), (1.00, 0)]
CRESTS = [(0.625, 26, 300)]            # (fraction, height, width)
# how far the road shapes the landscape (small = sharp ridge, large = open valley)
INFLUENCE = [(0.00, 1500), (0.10, 1500), (0.20, 900), (0.28, 650), (0.55, 650), (0.58, 380), (0.69, 380),
             (0.73, 650), (0.87, 800), (0.95, 1400), (1.00, 1500)]
BERM = [(0.00, 0), (0.12, 0), (0.16, 30), (0.57, 34), (0.59, 46), (0.70, 46), (0.73, 34), (0.94, 30),
        (0.97, 0), (1.00, 0)]
GALLERY = (0.76, 0.81)

LAYERS = ("snow", "slush", "asphalt", "rock", "cliff")
L_SNOW, L_SLUSH, L_ASPHALT, L_ROCK, L_CLIFF = range(5)
FRICTION = {L_SNOW: "snow", L_SLUSH: "snow", L_ASPHALT: "asphalt", L_ROCK: "rock", L_CLIFF: "rock"}

PATH, S, TAN, NOR, KAPPA = [], [], [], [], []
GRID, MAT, LEAVES, LEAFOF, TRIS = {}, {}, [], {}, []
BRUSHES, GROUPS, ENTITIES, SOLIDS, TRIGGERS, DECALS = [], {}, [], [], [], []
BOUNDS, CROSS = {}, {}
SPAN = set()


def smooth(a, b, x):
    t = min(1.0, max(0.0, (x - a) / (b - a)))
    return t * t * (3 - 2 * t)


def keyed(keys, f):
    f %= 1.0
    for (f0, v0), (f1, v1) in zip(keys, keys[1:]):
        if f <= f1:
            return v0 + (v1 - v0) * smooth(f0, f1, f)
    return keys[-1][1]


# ---------------------------------------------------------------- course

def build_path():
    n = len(CTRL)
    dense = []
    for i in range(n):
        p0, p1, p2, p3 = CTRL[i - 1], CTRL[i], CTRL[(i + 1) % n], CTRL[(i + 2) % n]
        m = max(4, int(math.dist(p1, p2) / 8))
        for k in range(m):
            t = k / m
            dense.append(tuple(0.5 * ((2 * p1[j]) + (-p0[j] + p2[j]) * t
                                      + (2 * p0[j] - 5 * p1[j] + 4 * p2[j] - p3[j]) * t * t
                                      + (-p0[j] + 3 * p1[j] - 3 * p2[j] + p3[j]) * t ** 3) for j in range(2)))
    dense.append(dense[0])
    total = sum(math.dist(a, b) for a, b in zip(dense, dense[1:]))
    count = round(total / SAMPLE)
    step = total / count                       # closes the loop exactly
    PATH.append(dense[0])
    acc = 0.0
    for a, b in zip(dense, dense[1:]):
        seg = math.dist(a, b)
        while acc + seg >= step and len(PATH) < count:
            t = (step - acc) / seg
            a = (a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t)
            seg = math.dist(a, b)
            PATH.append(a)
            acc = 0.0
        acc += seg
    n = len(PATH)
    BOUNDS["step"] = step
    for i, p in enumerate(PATH):
        S.append(i * step)
        q0, q1 = PATH[i - 1], PATH[(i + 1) % n]
        d = math.dist(q0, q1)
        t = ((q1[0] - q0[0]) / d, (q1[1] - q0[1]) / d)
        TAN.append(t)
        NOR.append((-t[1], t[0]))                     # left of the driving direction
    heading = [math.atan2(t[1], t[0]) for t in TAN]
    raw = []
    for i in range(n):
        db = (heading[(i + 1) % n] - heading[i - 1] + math.pi) % (2 * math.pi) - math.pi
        raw.append(db / (2 * step))
    w = 8
    for i in range(n):
        KAPPA.append(sum(raw[(i + j) % n] for j in range(-w, w + 1)) / (2 * w + 1))


def length():
    return len(PATH) * BOUNDS["step"]


def frac(s):
    return (s / length()) % 1.0


def half_width(s):
    return keyed(HALF_WIDTH, frac(s))


ZTAB = []


def linear(keys, f):
    for (f0, v0), (f1, v1) in zip(keys, keys[1:]):
        if f <= f1:
            return v0 + (v1 - v0) * (f - f0) / (f1 - f0)
    return keys[-1][1]


def build_profile():
    """Road height per sample: the keyframes joined linearly, then smoothed
    over +-640 units, so the grade changes gently; crests on top."""
    n = len(PATH)
    L = length()
    lin = [linear(ROAD_Z, i / n) for i in range(n)]
    w = 20
    for i in range(n):
        z = sum(lin[(i + j) % n] for j in range(-w, w + 1)) / (2 * w + 1)
        for cf, ch, cw in CRESTS:
            ds = (S[i] - cf * L + L / 2) % L - L / 2
            z += ch * math.exp(-(ds / cw) ** 2)
        ZTAB.append(z)


def road_z(s):
    n = len(PATH)
    u = (s / BOUNDS["step"]) % n
    i = int(u)
    t = u - i
    return ZTAB[i] * (1 - t) + ZTAB[(i + 1) % n] * t


def bank(i):
    return max(-0.08, min(0.08, -130 * KAPPA[i]))


def idx(i):
    return i % len(PATH)


# ---------------------------------------------------------------- spatial lookups

BUCKET = 512
HASH = {}


def hash_path():
    for i, (x, y) in enumerate(PATH):
        HASH.setdefault((int(x // BUCKET), int(y // BUCKET)), []).append(i)


def nearby(x, y, radius):
    r = int(radius // BUCKET) + 1
    bx, by = int(x // BUCKET), int(y // BUCKET)
    for gx in range(bx - r, bx + r + 1):
        for gy in range(by - r, by + r + 1):
            yield from HASH.get((gx, gy), ())


def leg_nearest(x, y, radius):
    """The closest centre line sample of every part of the lap in reach."""
    cand = sorted(i for i in nearby(x, y, radius)
                  if abs(x - PATH[i][0]) <= radius and abs(y - PATH[i][1]) <= radius)
    groups, prev = [], -10
    for i in cand:
        if i - prev > 2:
            groups.append([])
        groups[-1].append(i)
        prev = i
    n = len(PATH)
    if len(groups) > 1 and groups[0][0] == 0 and groups[-1][-1] == n - 1:
        groups[0] = groups.pop() + groups[0]
    out = []
    for g in groups:
        out.append(min(g, key=lambda i: (x - PATH[i][0]) ** 2 + (y - PATH[i][1]) ** 2))
    return out


def nearest(x, y, radius=2000):
    best, bi = 1e18, 0
    for i in nearby(x, y, radius):
        d = (x - PATH[i][0]) ** 2 + (y - PATH[i][1]) ** 2
        if d < best:
            best, bi = d, i
    return math.sqrt(best), bi


def local(i, x, y):
    """Continuous position along the lap, signed lateral offset (left +)."""
    px, py = PATH[i]
    dx, dy = x - px, y - py
    along = dx * TAN[i][0] + dy * TAN[i][1]
    s = S[i] + max(-BOUNDS["step"] / 2, min(BOUNDS["step"] / 2, along))
    return s, dx * NOR[i][0] + dy * NOR[i][1]


# ---------------------------------------------------------------- landscape

def macro(x, y):
    """Mountains without the road: valley in the south, ridge in the north,
    an east shoulder above the descent, high peaks all around the edge."""
    ry = 3300 + 160 * math.sin(x / 1100)
    if y < ry:
        t = smooth(-2900, ry, y)
        z = -60 + 700 * t ** 1.25
    else:
        z = 640 - 520 * smooth(ry, ry + 1700, y)
    z += 420 * smooth(1300, 3000, x) * smooth(-900, 1400, y) * (1 - smooth(2700, 3900, y))
    z -= 120 * smooth(-1500, -2600, x) * (1 - smooth(-200, 900, y))
    wob = 260 * math.sin(x / 830 + 0.4) * math.cos(y / 760) + 140 * math.sin(y / 390 + x / 610)
    ex, ey = max(abs(x) - 2900, 0), max(-2900 - y, y - 4200, 0)
    out = max(0.0, math.hypot(ex, ey) + wob)
    z += 2100 * smooth(0, 2600, out) + 260 * smooth(1200, 3800, out) * math.sin(x / 700 + y / 900)
    z += 70 * math.cos(x / 530 + 0.7) * math.cos(y / 610) + 34 * math.cos(x / 230) * math.sin(y / 270 + 1.0)
    return z


def road_bounds(i, x, y):
    """(lower bound, upper bound, distance beyond the asphalt edge) for one leg."""
    s, dsig = local(i, x, y)
    d = abs(dsig)
    hw = half_width(s)
    h = road_z(s)
    if i in SPAN:      # under the bridge deck: keep the ground clear of it
        return -1e9, h - 72 + max(0.0, d - hw - 60) * CUT, d - hw
    ze = h + bank(i) * max(-hw - SHOULDER, min(hw + SHOULDER, dsig))
    if d <= hw + SHOULDER:
        return ze, ze, d - hw
    e = d - hw - SHOULDER
    berm = keyed(BERM, frac(s)) * max(0.0, 1 - ((e - 56) / 56) ** 2)
    return ze - e * FILL + berm, ze + e * CUT, d - hw


def base(x, y, legs):
    """Landscape that the road has shaped: near the road it follows the road."""
    m = macro(x, y)
    if not legs:
        return m
    acc = wsum = 0.0
    pull = 0.0
    for i in legs:
        s, dsig = local(i, x, y)
        e = max(0.0, abs(dsig) - half_width(s))
        w = math.exp(-(e / 420) ** 2) + 1e-9
        acc += w * road_z(s)
        wsum += w
        pull = max(pull, math.exp(-(e / keyed(INFLUENCE, frac(s))) ** 2))
    return m + (acc / wsum - m) * pull


def height(x, y):
    legs = leg_nearest(x, y, 1500)
    z = base(x, y, legs)
    lo, hi = -1e9, 1e9
    for lo_i, hi_i, _ in sorted((road_bounds(i, x, y) for i in legs), key=lambda c: c[2]):
        nl, nh = max(lo, lo_i), min(hi, hi_i)
        if nl <= nh:
            lo, hi = nl, nh
    return min(max(z, lo), hi)


def find_crossing():
    n = len(PATH)

    def cr(o, p, q):
        return (p[0] - o[0]) * (q[1] - o[1]) - (p[1] - o[1]) * (q[0] - o[0])
    for i in range(n):
        a, b = PATH[i], PATH[(i + 1) % n]
        for j in range(i + 40, n):
            if (i - j) % n < 40:
                continue
            c, d = PATH[j], PATH[(j + 1) % n]
            if cr(a, b, c) * cr(a, b, d) < 0 and cr(c, d, a) * cr(c, d, b) < 0:
                lo, up = (i, j) if road_z(S[i]) < road_z(S[j]) else (j, i)
                CROSS.update(low=lo, up=up)
                hw = half_width(S[lo])
                for k in range(up - 40, up + 41):
                    kk = idx(k)
                    dlat = abs(local(lo, *PATH[kk])[1]) if abs(local(lo, *PATH[kk])[1]) < 2000 else 1e9
                    # distance of the upper centre line from the lower road axis
                    dd = min(abs((PATH[kk][0] - PATH[idx(lo + m)][0]) * NOR[idx(lo + m)][0]
                                 + (PATH[kk][1] - PATH[idx(lo + m)][1]) * NOR[idx(lo + m)][1])
                             for m in range(-12, 13))
                    if min(dd, dlat) < hw + SHOULDER + 110:
                        SPAN.add(kk)
                return
    raise SystemExit("no crossing found")


# ---------------------------------------------------------------- terrain mesh

def build_terrain():
    xs = [p[0] for p in PATH]
    ys = [p[1] for p in PATH]
    x0 = math.floor((min(xs) - 2800) / 512) * 512
    x1 = math.ceil((max(xs) + 2800) / 512) * 512
    y0 = math.floor((min(ys) - 2600) / 512) * 512
    y1 = math.ceil((max(ys) + 2800) / 512) * 512
    BOUNDS.update(x0=x0, x1=x1, y0=y0, y1=y1)
    edge = {}
    for y in range(y0, y1 + 1, STEP):
        for x in range(x0, x1 + 1, STEP):
            GRID[x, y] = height(x, y)
            d, i = nearest(x, y, 2400)
            edge[x, y] = d - half_width(S[i]) if d < 1e8 else 1e9
    raw = dict(GRID)

    def split(cx, cy, size):
        pts = [(cx + a * STEP, cy + b * STEP) for a in range(size // STEP + 1) for b in range(size // STEP + 1)]
        near = min(edge[p] for p in pts)
        limit = {512: 2200, 256: 700, 128: 150}.get(size)
        if size == STEP or near > limit:
            LEAVES.append((cx, cy, size))
            return
        h = size // 2
        for qy in (cy, cy + h):
            for qx in (cx, cx + h):
                split(qx, qy, h)
    for cy in range(y0, y1, 512):
        for cx in range(x0, x1, 512):
            split(cx, cy, 512)
    # straighten the edges of bigger cells so finer neighbours meet them without gaps
    for (lx, ly, size) in sorted(LEAVES, key=lambda c: -c[2]):
        for (ax, ay), (bx, by) in (((lx, ly), (lx + size, ly)), ((lx + size, ly), (lx + size, ly + size)),
                                   ((lx, ly + size), (lx + size, ly + size)), ((lx, ly), (lx, ly + size))):
            n = size // STEP
            for k in range(1, n):
                GRID[ax + (bx - ax) * k // n, ay + (by - ay) * k // n] = \
                    GRID[ax, ay] + (GRID[bx, by] - GRID[ax, ay]) * k / n
    for k in GRID:
        GRID[k] = round(GRID[k] * 8) / 8
    # materials from the unflattened field
    for (x, y), z in raw.items():
        gx = (raw.get((x + STEP, y), z) - raw.get((x - STEP, y), z)) / (2 * STEP)
        gy = (raw.get((x, y + STEP), z) - raw.get((x, y - STEP), z)) / (2 * STEP)
        slope = math.hypot(gx, gy)
        e = edge[x, y]
        if e < -8:
            m = L_ASPHALT
        elif e < SHOULDER + 8:
            m = L_SLUSH
        elif slope > 1.3:
            m = L_CLIFF
        elif slope > 0.78:
            m = L_ROCK
        else:
            patch = math.cos(x / 410) * math.cos(y / 370) + 0.5 * math.cos(x / 157) * math.cos(y / 181)
            m = L_ROCK if (patch > 1.15 and slope > 0.35 and e > 300) else L_SNOW
        MAT[x, y] = m
    order = ("rock", "snow", "asphalt")
    for (lx, ly, size) in LEAVES:
        a, b = (lx, ly, GRID[lx, ly]), (lx + size, ly, GRID[lx + size, ly])
        c, d = (lx + size, ly + size, GRID[lx + size, ly + size]), (lx, ly + size, GRID[lx, ly + size])
        hc = height(lx + size / 2, ly + size / 2) if size == STEP else raw[lx + size // 2, ly + size // 2]
        ac, bd = abs((a[2] + c[2]) / 2 - hc), abs((b[2] + d[2]) / 2 - hc)
        diag = "ac" if ac <= bd else "bd"
        for fx in range(lx, lx + size, STEP):
            for fy in range(ly, ly + size, STEP):
                LEAFOF[fx, fy] = (lx, ly, size, diag)
        tris = ((a, b, c), (a, c, d)) if diag == "ac" else ((a, b, d), (b, c, d))
        for tri in tris:
            TRIS.append(tri)
            classes = [FRICTION[MAT[p[0], p[1]]] for p in tri]
            major = max(set(classes), key=lambda k: (classes.count(k), -order.index(k)))
            if L_ASPHALT in [MAT[p[0], p[1]] for p in tri] and classes.count("asphalt") >= 2:
                major = "asphalt"
            prism(tri, TEX + "t_" + major, group=major)


def cell_height(x0, y0, size, diag, x, y):
    u, v = (x - x0) / size, (y - y0) / size
    a, b = GRID[x0, y0], GRID[x0 + size, y0]
    c, d = GRID[x0 + size, y0 + size], GRID[x0, y0 + size]
    if diag == "ac":
        if v <= u:
            return a + (b - a) * u + (c - b) * v
        return a + (c - d) * u + (d - a) * v
    if u + v <= 1:
        return a + (b - a) * u + (d - a) * v
    return c + (d - c) * (1 - u) + (b - c) * (1 - v)


def ground(x, y):
    fx = BOUNDS["x0"] + math.floor((x - BOUNDS["x0"]) / STEP) * STEP
    fy = BOUNDS["y0"] + math.floor((y - BOUNDS["y0"]) / STEP) * STEP
    lx, ly, size, diag = LEAFOF[fx, fy]
    return cell_height(lx, ly, size, diag, x, y)


# ---------------------------------------------------------------- brushes

def cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def plane(normal, point):
    up = (1, 0, 0) if abs(normal[0]) < 1e-9 and abs(normal[1]) < 1e-9 else (0, 0, 1)
    t1 = cross(up, normal)
    t2 = cross(normal, t1)
    size = max(abs(c) for c in t1 + t2)
    return (point, tuple(p + c * 128 / size for p, c in zip(point, t2)),
            tuple(p + c * 128 / size for p, c in zip(point, t1)))


def num(v):
    s = f"{v:.4f}".rstrip("0").rstrip(".")
    return s if s not in ("", "-0") else "0"


def face(points, material, scale=0.5, detail=True):
    p = " ".join("( %s )" % " ".join(num(v) for v in xyz) for xyz in points)
    return f"{p} {material} 0 0 0 {num(scale)} {num(scale)} {DETAIL if detail else 0} 0 0"


def brush(faces, group=None):
    text = "{\n" + "\n".join(faces) + "\n}"
    (BRUSHES if group is None else GROUPS.setdefault(group, [])).append(text)


def prism(vertices, material, group=None):
    a, b, c = vertices
    faces = [face((a, c, b), material, scale=1),
             face(plane((0, 0, -1), (0, 0, FLOOR)), "common/caulk")]
    for p, q in ((a, b), (b, c), (c, a)):
        faces.append(face(((p[0], p[1], FLOOR), p, q), "common/caulk"))
    brush(faces, group)


def polytope(planes):
    eq = [(n, sum(a * b for a, b in zip(n, p))) for n, p in planes]
    out = []
    for i in range(len(eq)):
        for j in range(i + 1, len(eq)):
            for k in range(j + 1, len(eq)):
                (n1, d1), (n2, d2), (n3, d3) = eq[i], eq[j], eq[k]
                c23, c31, c12 = cross(n2, n3), cross(n3, n1), cross(n1, n2)
                det = sum(a * b for a, b in zip(n1, c23))
                if abs(det) < 1e-9:
                    continue
                p = tuple((d1 * c23[m] + d2 * c31[m] + d3 * c12[m]) / det for m in range(3))
                if all(sum(a * b for a, b in zip(n, p)) <= d + 0.01 for n, d in eq):
                    out.append(p)
    return out


def convex(planes, solid=True, scale=0.5, detail=True):
    """(outward normal, point, material) planes."""
    brush([face(plane(n, p), m, scale=scale, detail=detail) for n, p, m in planes])
    if solid:
        corners = polytope([(n, p) for n, p, _ in planes])
        SOLIDS.append((tuple(min(c[i] for c in corners) for i in range(3)),
                       tuple(max(c[i] for c in corners) for i in range(3))))


def obox(cx, cy, t, along, across, z0, z1, mat, side=None, solid=True, top=None, bottom=None):
    """Box rotated to the direction t (unit), centred on cx, cy."""
    n = (-t[1], t[0])
    side = side or mat
    convex([((t[0], t[1], 0), (cx + t[0] * along, cy + t[1] * along, z0), side),
            ((-t[0], -t[1], 0), (cx - t[0] * along, cy - t[1] * along, z0), side),
            ((n[0], n[1], 0), (cx + n[0] * across, cy + n[1] * across, z0), mat),
            ((-n[0], -n[1], 0), (cx - n[0] * across, cy - n[1] * across, z0), mat),
            ((0, 0, 1), (cx, cy, z1), top or side), ((0, 0, -1), (cx, cy, z0), bottom or side)], solid=solid)


def hexa(corners, mats, solid=True):
    """Convex 8-corner block: corners 0-3 one end (ring order), 4-7 the other.
    mats: one material or a dict per face index (0 end a, 1 end b, 2-5 sides)."""
    c = [sum(p[i] for p in corners) / 8 for i in range(3)]
    quads = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
    planes = []
    for k, q in enumerate(quads):
        p0, p1, p2 = corners[q[0]], corners[q[1]], corners[q[2]]
        n = cross(tuple(b - a for a, b in zip(p0, p1)), tuple(b - a for a, b in zip(p0, p2)))
        if sum(nc * (pc - cc) for nc, pc, cc in zip(n, p0, c)) < 0:
            n = tuple(-v for v in n)
        m = mats.get(k, mats.get("default")) if isinstance(mats, dict) else mats
        planes.append((n, p0, m))
    convex(planes, solid=solid)


def strip(i0, i1, lat0, lat1, z0, z1, mats, step=2, follow=True, solid=True):
    """Blocks along the centre line from sample i0 to i1, lateral lat0..lat1
    (left +). follow: heights relative to the road surface."""
    for k in range(i0, i1, step):
        a, b = idx(k), idx(min(k + step, i1))
        corners = []
        for j in (a, b):
            (px, py), n = PATH[j], NOR[j]
            for lat, z in ((lat0, z0), (lat1, z0), (lat1, z1), (lat0, z1)):
                zz = z + (road_z(S[j]) + bank(j) * lat if follow else 0)
                corners.append((px + n[0] * lat, py + n[1] * lat, zz))
        # ring order per end: (lat0,z0) (lat1,z0) (lat1,z1) (lat0,z1)
        hexa(corners, mats, solid=solid)


def boulder(x, y, radius, tall, seed, mat):
    z0 = min(ground(x + dx, y + dy) for dx in (-radius, 0, radius) for dy in (-radius, 0, radius))
    centre = (x, y, z0)
    planes = [((0, 0, -1), (x, y, z0 - 24), mat), ((0, 0, 1), (x, y, z0 + tall), mat)]
    rnd = abs(math.sin(seed * 12.9898) * 43758.5453) % 233280
    for k in range(14):
        rnd = (rnd * 9301 + 49297) % 233280
        jitter = rnd / 233280
        if k < 7:
            azimuth = (k + 0.4 * jitter) * 2 * math.pi / 7 + seed
            elevation = math.radians(4 + 10 * jitter)
            reach = radius * (0.85 + 0.25 * jitter)
        else:
            azimuth = (k - 7 + 0.5 * jitter) * 2 * math.pi / 7 + seed + 0.3
            elevation = math.radians(30 + 30 * ((k * 7 + int(seed * 3)) % 5) / 4)
            reach = radius * (0.7 + 0.25 * jitter)
        n = (math.cos(azimuth) * math.cos(elevation), math.sin(azimuth) * math.cos(elevation), math.sin(elevation))
        reach = reach * (1 - n[2]) + tall * 0.92 * n[2]
        planes.append((n, tuple(c + nc * reach for c, nc in zip(centre, n)), mat))
    convex(planes)


BASE_AXES = (((0, 0, 1), (1, 0, 0), (0, -1, 0)), ((0, 0, -1), (1, 0, 0), (0, -1, 0)),
             ((1, 0, 0), (0, 1, 0), (0, 0, -1)), ((-1, 0, 0), (0, 1, 0), (0, 0, -1)),
             ((0, 1, 0), (1, 0, 0), (0, 0, -1)), ((0, -1, 0), (1, 0, 0), (0, 0, -1)))


def fitted(normal, left, top_z, width, height, img_w, img_h, mirror=False):
    """Shift/scale of the Q3 axial texture projection that fits an image exactly
    onto an upright board of any yaw (the projection is linear, so a scaled
    axis compensates the slant)."""
    best = max(BASE_AXES, key=lambda b: sum(x * y for x, y in zip(normal, b[0])))
    sv, tv = best[1], best[2]
    right = (-normal[1], normal[0], 0.0)       # board's right seen from the front
    span = width * sum(x * y for x, y in zip(right, sv))
    ss = span / img_w
    if mirror:
        ss = -ss
    dl = sum(x * y for x, y in zip(left, sv))
    if mirror:
        dl = sum(x * y for x, y in zip((left[0] + right[0] * width, left[1] + right[1] * width, 0), sv))
    sh_s = (-dl / ss) % img_w
    ts = height / img_h
    sh_t = (top_z / ts) % img_h
    return sh_s, sh_t, ss, ts


def sign_board(x, y, z0, facing, image, mirror=False, w=64, h=128, post=TEX + "metal"):
    """Upright board on a post; the image faces the direction `facing` (unit,
    towards the driver)."""
    fx, fy = facing
    right = (-fy, fx)
    zb, zt = z0 + 96, z0 + 96 + h
    left = (x - right[0] * w / 2, y - right[1] * w / 2, 0.0)
    xo, yo, ss, ts = fitted((fx, fy, 0.0), left, zt, w, h, 128, 256, mirror)
    front = f"{' '.join('( %s )' % ' '.join(num(v) for v in p) for p in plane((fx, fy, 0), (x, y, zb)))} " \
            f"{image} {num(xo)} {num(yo)} 0 {num(ss)} {num(ts)} {DETAIL} 0 0"
    faces = [front,
             face(plane((-fx, -fy, 0), (x - fx * 6, y - fy * 6, zb)), post),
             face(plane((right[0], right[1], 0), (x + right[0] * w / 2, y + right[1] * w / 2, zb)), post),
             face(plane((-right[0], -right[1], 0), (x - right[0] * w / 2, y - right[1] * w / 2, zb)), post),
             face(plane((0, 0, 1), (x, y, zt)), post), face(plane((0, 0, -1), (x, y, zb)), post)]
    brush(faces)
    obox(x - fx * 12, y - fy * 12, (fx, fy), 5, 5, z0 - 40, zb + 8, post)


def entity(classname, x, y, z, **keys):
    fields = {"classname": classname, "origin": f"{round(x)} {round(y)} {round(z)}", **keys}
    ENTITIES.append(fields)
    return fields


def at(fraction):
    return idx(int(round(fraction * len(PATH))))


def heading_deg(i):
    return math.degrees(math.atan2(TAN[i][1], TAN[i][0])) % 360


def road_point(i, lat=0.0):
    (px, py), n = PATH[i], NOR[i]
    x, y = px + n[0] * lat, py + n[1] * lat
    return x, y, road_z(S[i]) + bank(i) * lat


# ---------------------------------------------------------------- course objects

def trigger(classname, i, number=None, target=None, extra=None):
    """Thin trigger across the road; it must not reach a neighbouring leg."""
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i])
    z = road_z(S[i])
    reach = hw + 200
    for j in nearby(px, py, 1200):
        if min(abs(j - i), len(PATH) - abs(j - i)) * SAMPLE > 1500:
            dlat = (PATH[j][0] - px) * n[0] + (PATH[j][1] - py) * n[1]
            dalong = (PATH[j][0] - px) * t[0] + (PATH[j][1] - py) * t[1]
            if abs(dalong) < half_width(S[j]) + 80 and abs(road_z(S[j]) - z) < 420:
                reach = min(reach, abs(dlat) - half_width(S[j]) - 40)
    reach = max(reach, hw + 40)
    keys = {"classname": classname}
    if number is not None:
        keys["number"] = str(number)
    if target:
        keys["target"] = target
    keys.update(extra or {})
    tex = "common/checkpoint" if classname == "rally_checkpoint" else "common/startfinish"
    planes = [((t[0], t[1], 0), (px + t[0] * 24, py + t[1] * 24, z), tex),
              ((-t[0], -t[1], 0), (px - t[0] * 24, py - t[1] * 24, z), tex),
              ((n[0], n[1], 0), (px + n[0] * reach, py + n[1] * reach, z), tex),
              ((-n[0], -n[1], 0), (px - n[0] * reach, py - n[1] * reach, z), tex),
              ((0, 0, 1), (px, py, z + 360), tex),
              ((0, 0, -1), (px, py, z - 90), tex)]
    body = "\n".join(face(plane(nn, p), m, detail=False) for nn, p, m in planes)
    TRIGGERS.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()) + "\n{\n" + body + "\n}\n}")
    if target:
        x, y, zz = road_point(i)
        entity("info_notnull", x, y, zz + 40, targetname=target, angle=f"{heading_deg(i):.0f}")
    return reach


def corners():
    """Apexes of the corners: local curvature peaks tighter than 1800 units."""
    n = len(PATH)
    out = []
    for i in range(n):
        k = abs(KAPPA[i])
        if k > 1 / 1800 and k >= abs(KAPPA[i - 1]) and k > abs(KAPPA[(i + 1) % n]):
            # entry: walk back to where the curvature drops below a third
            j = i
            while abs(KAPPA[idx(j - 1)]) > k / 3 and (i - j) < 18:
                j -= 1
            out.append((i, idx(j), 1 / k, "left" if KAPPA[i] > 0 else "right"))
    # merge apexes of the same corner (hairpins, long bends)
    merged = []
    for c in out:
        if merged and (c[0] - merged[-1][0]) % n < 24 and c[3] == merged[-1][3]:
            if c[2] < merged[-1][2]:
                merged[-1] = (c[0], merged[-1][1], c[2], c[3])
            continue
        merged.append(c)
    return merged


CHECKPOINTS = []


def course():
    n = len(PATH)
    step = BOUNDS["step"]
    i_sf = min(range(n), key=lambda i: abs(PATH[i][0] - 120) + 4 * abs(PATH[i][1] + 2300))
    BOUNDS["i_sf"] = i_sf
    # checkpoints: about 350 units in front of every corner entry
    wanted = []
    for apex, entry, r, side in corners():
        wanted.append((idx(entry - int(350 / step)), apex, r, side))
    wanted.sort()
    picks = []
    for k, (i, apex, r, side) in enumerate(wanted):
        dist_sf = min((i - i_sf) % n, (i_sf - i) % n) * step
        if dist_sf < 700:
            continue
        if picks and (i - picks[-1][0]) % n * step < 600:
            continue
        picks.append((i, apex, r, side))
    # number them from the start line on; long gaps get extra checkpoints
    # (never near the crossing, where a trigger would reach the other level)
    picks.sort(key=lambda p: (p[0] - i_sf) % n)
    keep_out = [CROSS["low"], CROSS["up"]]
    marks = [i_sf] + [p[0] for p in picks] + [i_sf + n]
    extra = []
    for a, b in zip(marks, marks[1:]):
        a2 = a if a >= i_sf else a + n
        b2 = b if b > a2 else b + n
        gap = (b2 - a2) * step
        parts = int(gap // 2600)
        for k in range(1, parts + 1):
            c = a2 + (b2 - a2) * k // (parts + 1)
            for _ in range(40):
                if all(min(abs(idx(c) - q), n - abs(idx(c) - q)) * step > 700 for q in keep_out):
                    break
                c += 2
            extra.append((idx(c), idx(c), 99999, "straight"))
    picks = sorted(picks + extra, key=lambda p: (p[0] - i_sf) % n)
    for num_, (i, apex, r, side) in enumerate(picks, 1):
        reach = trigger("rally_checkpoint", i, number=num_, target=f"cp{num_}")
        CHECKPOINTS.append({"number": num_, "sample": i, "fraction": round(i / n, 3), "apex_radius": round(r),
                            "side": side, "half_reach": round(reach)})
    trigger("rally_startfinish", i_sf, target="sf", extra={"laps": "3"})
    # start grid: 6 rows of 2, staggered, behind the line
    for row in range(6):
        for col, lat in enumerate((-95, 95)):
            j = idx(i_sf - int((150 + row * 170 + col * 85) / step))
            x, y, z = road_point(j, lat)
            entity("info_player_start", x, y, z + 48, angle=f"{heading_deg(j):.0f}")
    j = idx(i_sf - int(1300 / step))
    x, y, z = road_point(j)
    entity("info_player_deathmatch", x, y, z + 48, angle=f"{heading_deg(j):.0f}")
    return picks


# ---------------------------------------------------------------- structures

def start_finish():
    i = BOUNDS["i_sf"]
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i])
    z = road_z(S[i])
    # portal over the line: posts, beam with the Q3Rally banner, chequered underside
    for side in (-1, 1):
        cx, cy = px + n[0] * side * (hw + 70), py + n[1] * side * (hw + 70)
        obox(cx, cy, t, 22, 22, ground(cx, cy) - 30, z + 400, TEX + "metal")
    span = hw + 100
    banner = (TEX + "startbanner", 2 * span, 2 * span / 8, 512, 64)
    fitted_box((px - 18, py - span, z + 410 - 2 * span / 8), (px + 18, py + span, z + 410), TEX + "metal",
               {"-x": banner, "+x": banner})
    # start lights under the beam, facing the grid
    zb = z + 410 - 2 * span / 8
    box((px - 14, py - 150, zb - 60), (px + 14, py + 150, zb), TEX + "metal")
    for k in range(5):
        ly = py - 120 + k * 60
        box((px - 22, ly - 16, zb - 46), (px - 14, ly + 16, zb - 14), TEX + "redlamp", collision=False)
    # chequered panels on the posts
    for side in (-1, 1):
        cy = py + n[1] * side * (hw + 70)
        fitted_box((px - 26, cy - 26, z + 60), (px + 26, cy + 26, z + 300), TEX + "metal",
                   {"-x": (TEX + "checker", 52, 52, 256, 256), "+x": (TEX + "checker", 52, 52, 256, 256)})
    # painted start line (decal on the asphalt)
    decal(i, -hw, hw, -10, 10, TEX + "line")
    for row in range(6):
        for col, lat in enumerate((-95, 95)):
            j = idx(i - int((150 + row * 170 + col * 85) / BOUNDS["step"]))
            decal(j, lat - 60, lat + 60, -3, 3, TEX + "line")


def decal(i, lat0, lat1, a0, a1, mat):
    """Flat nonsolid paint just above the road at sample i (along offset a0..a1).
    Top and bottom are parallel planes, so the thin slab cannot turn into a wedge."""
    (px, py), t, n = PATH[i], TAN[i], NOR[i]

    def pt(lat, along):
        x = px + n[0] * lat + t[0] * along
        y = py + n[1] * lat + t[1] * along
        return (x, y, ground(x, y) + 0.5)
    p0, p1, p2 = pt(lat0, a0), pt(lat1, a0), pt(lat0, a1)
    nrm = cross(tuple(b - a for a, b in zip(p0, p1)), tuple(b - a for a, b in zip(p0, p2)))
    if nrm[2] < 0:
        nrm = tuple(-v for v in nrm)
    ln = math.sqrt(sum(v * v for v in nrm))
    nrm = tuple(v / ln for v in nrm)
    top = tuple(c + 1.2 * v for c, v in zip(p0, nrm))
    planes = [(nrm, top, mat), (tuple(-v for v in nrm), p0, "common/nodraw"),
              ((t[0], t[1], 0), (px + t[0] * a1, py + t[1] * a1, 0), "common/nodraw"),
              ((-t[0], -t[1], 0), (px + t[0] * a0, py + t[1] * a0, 0), "common/nodraw"),
              ((n[0], n[1], 0), (px + n[0] * max(lat0, lat1), py + n[1] * max(lat0, lat1), 0), mat),
              ((-n[0], -n[1], 0), (px + n[0] * min(lat0, lat1), py + n[1] * min(lat0, lat1), 0), mat)]
    convex(planes, solid=False)


def slab(x1, x2, y_front, z_front, y_back, z_back, thick, mat_top, mat_under, mat_edge=None):
    """Sloping roof slab spanning x1..x2, from (y_front, z_front) to (y_back, z_back)."""
    corners = [(x1, y_front, z_front - thick), (x1, y_back, z_back - thick), (x1, y_back, z_back),
               (x1, y_front, z_front),
               (x2, y_front, z_front - thick), (x2, y_back, z_back - thick), (x2, y_back, z_back),
               (x2, y_front, z_front)]
    hexa(corners, {"default": mat_edge or mat_under, 2: mat_under, 4: mat_top})


def grandstand():
    """Start/finish area. The straight runs along +x at about y=-2300.
    Outside (south): covered grandstand with catch fence, floodlights and
    billboards. Inside (north): pit building with garages, glazed upper floor,
    sponsor band and a race control tower."""
    i = BOUNDS["i_sf"]
    y_edge = PATH[i][1] - half_width(S[i]) - SHOULDER
    zr = road_z(S[i])
    x_from, x_to = -800, 1000
    lo_y = y_edge - 120
    # safety wall with a catch fence on posts
    box((x_from, lo_y - 24, zr - 40), (x_to, lo_y, zr + 56), TEX + "barrier")
    box((x_from, lo_y - 14, zr + 56), (x_to, lo_y - 10, zr + 236), "q3r_metals/grate", collision=False)
    for x in range(x_from, x_to + 1, 300):
        box((x - 5, lo_y - 18, zr + 56), (x + 5, lo_y - 6, zr + 244), TEX + "metal")
    box((x_from, lo_y - 18, zr + 236), (x_to, lo_y - 6, zr + 244), TEX + "metal")
    # stands: 6 tiers, crowd on the risers
    y = lo_y - 120
    tiers = []
    for tier in range(6):
        z0 = zr + 20 + tier * 44
        box((x_from, y - 70, zr - 40), (x_to, y, z0), TEX + "concrete")
        tiers.append((y, z0, tier))
        y -= 70
    y_back = y
    for yy, z0, tier in tiers:
        img = f"q3r_people/peeprow0{1 + tier % 4}a"
        fitted_box((x_from, yy - 30, z0), (x_to, yy - 28, z0 + 44), img, {"+y": (img, 352, 44, 256, 32)},
                   collision=False)
    # walkway in front of the stands, back wall, end walls with billboards
    box((x_from - 60, lo_y - 120, zr - 40), (x_to + 60, lo_y - 24, zr + 4), TEX + "concrete")
    box((x_from, y_back - 30, zr - 40), (x_to, y_back, zr + 540), TEX + "concrete")
    poster = (TEX + "poster", 300, 180, 1200, 720)
    for xa, xb, key in ((x_from - 30, x_from, "-x"), (x_to, x_to + 30, "+x")):
        fitted_box((xa, y_back - 30, zr - 40), (xb, lo_y - 120, zr + 330), TEX + "concrete",
                   {key: (TEX + "concrete", 256, 256, 256, 256)})
        fitted_box((xa, lo_y - 380, zr + 120), (xb, lo_y - 80, zr + 300), TEX + "concrete", {key: poster},
                   collision=False)
    # cantilever roof: steel girders from the back wall, sloping slab, sponsor fascia
    y_front = lo_y - 90
    for x in range(x_from, x_to + 1, 225):
        hexa([(x - 8, y_back, zr + 470), (x - 8, y_back, zr + 560), (x - 8, y_front, zr + 500),
              (x - 8, y_front, zr + 476),
              (x + 8, y_back, zr + 470), (x + 8, y_back, zr + 560), (x + 8, y_front, zr + 500),
              (x + 8, y_front, zr + 476)], TEX + "metal")
    slab(x_from - 40, x_to + 40, y_front, zr + 504, y_back - 30, zr + 568, 14, TEX + "roofmetal", TEX + "concrete")
    fitted_box((x_from - 40, y_front - 12, zr + 440), (x_to + 40, y_front, zr + 492), TEX + "metal",
               {"+y": (TEX + "pitsign", 416, 52, 512, 64)})
    for x in range(x_from + 225, x_to, 450):
        box((x - 8, lo_y - 112, zr + 4), (x + 8, lo_y - 96, zr + 440), TEX + "metal")
    # floodlight masts at both ends (floodmast model, heads aimed at the middle of the
    # start straight, scaled to the old height of about 960 above the road), invisible
    # collision where the old pole stood
    for x in (x_from - 120, x_to + 120):
        my = lo_y - 190
        gz = ground(x, my)
        scale = min(2.0, (zr + 960 - gz) / 500.0)
        aim = math.atan2(y_edge + 200 - my, (x_from + x_to) / 2 - x)
        entity("misc_model", x, my, gz - 2, model="models/mapobjects/q3r_lanterns/floodmast.md3",
               angle=f"{math.degrees(aim) % 360:.0f}", modelscale=f"{scale:.2f}")
        box((x - 10, lo_y - 200, zr - 40), (x + 10, lo_y - 180, zr + 900), TEX + "propclip")
        entity("light", x + 200 * math.cos(aim), my + 200 * math.sin(aim), gz + 440 * scale, light="900",
               _color="1 0.96 0.88")
    for x in (-400, 100, 600):
        entity("light", x, lo_y - 200, zr + 440, light="220", _color="1 0.95 0.85")
    entity("target_speaker", 100, lo_y - 300, zr + 260, noise="sound/world/crowdlng.wav", spawnflags="1")

    # ---- pit building on the inside
    y_in = PATH[i][1] + half_width(S[i]) + SHOULDER + 180
    box((-600, y_in - 24, zr - 40), (900, y_in, zr + 48), TEX + "barrier")
    box((-600, y_in - 14, zr + 48), (900, y_in - 10, zr + 160), "q3r_metals/grate", collision=False)
    for x in range(-600, 901, 300):
        box((x - 5, y_in - 18, zr + 48), (x + 5, y_in - 6, zr + 166), TEX + "metal")
    box((-600, y_in, zr - 40), (900, y_in + 140, zr + 2), TEX + "concrete")           # pit apron
    yb = y_in + 140
    x0, x1 = -520, 820
    # ground floor with 8 garages between pilasters
    box((x0, yb, zr - 30), (x1, yb + 280, zr + 150), TEX + "pitwall")
    pitch = (x1 - x0) / 8
    for k in range(8):
        gx = x0 + k * pitch + (pitch - 136) / 2
        fitted_box((gx, yb - 2, zr), (gx + 136, yb, zr + 120), TEX + "garage",
                   {"-y": (TEX + "garage", 136, 120, 256, 256)}, collision=False)
        box((x0 + k * pitch - 12, yb - 14, zr - 30), (x0 + k * pitch + 12, yb, zr + 150), TEX + "concrete")
    box((x1 - 12, yb - 14, zr - 30), (x1 + 12, yb, zr + 150), TEX + "concrete")
    # balcony slab and railing
    box((x0 - 20, yb - 70, zr + 150), (x1 + 20, yb + 280, zr + 168), TEX + "concrete")
    box((x0 - 20, yb - 72, zr + 204), (x1 + 20, yb - 64, zr + 210), TEX + "metal")
    for x in range(x0, x1 + 1, 110):
        box((x - 3, yb - 71, zr + 168), (x + 3, yb - 65, zr + 204), TEX + "metal")
    # glazed upper floor, sponsor band, roof with overhang
    fitted_box((x0, yb + 20, zr + 168), (x1, yb + 280, zr + 300), TEX + "pitwall",
               {"-y": (TEX + "glass", 132, 132, 256, 256)})
    fitted_box((x0 - 20, yb - 4, zr + 300), (x1 + 20, yb + 280, zr + 352), TEX + "pitwall",
               {"-y": (TEX + "pitsign", 416, 52, 512, 64)})
    box((x0 - 40, yb - 50, zr + 352), (x1 + 40, yb + 300, zr + 368), TEX + "concrete")
    # race control tower at the east end
    tx0, tx1, ty0, ty1 = x1 + 30, x1 + 210, yb + 30, yb + 210
    box((tx0, ty0, zr - 30), (tx1, ty1, zr + 430), TEX + "pitwall")
    fitted_box((tx0 + 10, ty0 - 4, zr + 190), (tx1 - 10, ty0, zr + 350), TEX + "pitwall",
               {"-y": (TEX + "logo", 160, 160, 256, 256)}, collision=False)
    glass = (TEX + "darkglass", 110, 110, 128, 128)
    fitted_box((tx0 - 20, ty0 - 20, zr + 430), (tx1 + 20, ty1 + 20, zr + 540), TEX + "pitwall",
               {"-y": glass, "+y": glass, "-x": glass, "+x": glass})
    box((tx0 - 36, ty0 - 36, zr + 540), (tx1 + 36, ty1 + 36, zr + 560), TEX + "concrete")
    box((tx0 + 80, ty0 + 80, zr + 560), (tx0 + 92, ty0 + 92, zr + 760), TEX + "metal")
    entity("light", (tx0 + tx1) / 2, ty0 - 120, zr + 380, light="200", _color="0.9 0.95 1")
    for x in (-300, 300):
        entity("light", x, yb - 120, zr + 130, light="180", _color="1 0.95 0.85")


def box(lo, hi, material, collision=True):
    x1, y1, z1 = lo
    x2, y2, z2 = hi
    assert x2 > x1 and y2 > y1 and z2 > z1, (lo, hi)
    normals = ((0, 0, -1), (0, 0, 1), (-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0))
    points = ((x1, y1, z1), (x2, y2, z2), (x1, y1, z1), (x2, y2, z2), (x1, y1, z1), (x2, y2, z2))
    brush([face(plane(n, p), material) for n, p in zip(normals, points)])
    if collision:
        SOLIDS.append((lo, hi))


def fitted_box(lo, hi, material, fits, collision=True):
    """Axial box; fits: {"+x"|"-x"|"+y"|"-y": (image, tile_w, tile_h, img_w, img_h)} puts an
    image unmirrored and unstretched on that face, tile_w x tile_h units per copy,
    starting at the face's left edge (seen from the front) and its top edge.
    "-z": image fitted on the underside (tile_w x tile_h)."""
    x1, y1, z1 = lo
    x2, y2, z2 = hi
    normals = {"-z": (0, 0, -1), "+z": (0, 0, 1), "-x": (-1, 0, 0), "+x": (1, 0, 0), "-y": (0, -1, 0),
               "+y": (0, 1, 0)}
    faces = []
    for key, n in normals.items():
        p = tuple(hi[k] if n[k] > 0 else lo[k] for k in range(3))
        pts = " ".join("( %s )" % " ".join(num(v) for v in q) for q in plane(n, p))
        if key in fits and key != "-z":
            image, tw, th, iw, ih = fits[key]
            right = (-n[1], n[0])
            # left edge seen from the front
            left = (x1 if right[0] > 0 else x2 if right[0] < 0 else p[0],
                    y1 if right[1] > 0 else y2 if right[1] < 0 else p[1], 0.0)
            xo, yo, ss, ts = fitted((float(n[0]), float(n[1]), 0.0), left, z2, tw, th, iw, ih)
            faces.append(f"{pts} {image} {num(xo)} {num(yo)} 0 {num(ss)} {num(ts)} {DETAIL} 0 0")
        elif key == "-z" and key in fits:
            image, tw, th, iw, ih = fits[key]
            ss, ts = tw / iw, th / ih          # ceiling axes: s = x, t = -y
            faces.append(f"{pts} {image} {num((-x1 / ss) % iw)} {num((y2 / ts) % ih)} 0 {num(ss)} {num(ts)} "
                         f"{DETAIL} 0 0")
        else:
            faces.append(face(plane(n, p), material))
    brush(faces)
    if collision:
        SOLIDS.append((lo, hi))


def bridge():
    """Deck over the lower road; parapets; retaining walls of the underpass."""
    up = sorted(SPAN, key=lambda k: (k - CROSS["up"] + len(PATH) // 2) % len(PATH))
    i0, i1 = up[0] - 3, up[-1] + 3
    hw = half_width(S[CROSS["up"]])
    deck = {"default": TEX + "concrete", 4: TEX + "deck", 0: "common/caulk", 1: "common/caulk"}
    strip(i0, i1, -hw - 40, hw + 40, -48, 0, deck, step=2)
    wall = {"default": TEX + "concrete", 0: "common/caulk", 1: "common/caulk"}
    strip(i0, i1, hw + 16, hw + 40, 0, 52, wall, step=2)
    strip(i0, i1, -hw - 40, -hw - 16, 0, 52, wall, step=2)
    # retaining walls beside the lower road, under the deck edges
    lo = CROSS["low"]
    hl = half_width(S[lo])
    hu = half_width(S[CROSS["up"]])
    ups = [idx(CROSS["up"] + k) for k in range(-30, 31)]

    def under_deck(x, y):
        """Lateral distance from the upper road and its height there."""
        j = min(ups, key=lambda u: (PATH[u][0] - x) ** 2 + (PATH[u][1] - y) ** 2)
        s_j, lat = local(j, x, y)
        return abs(lat), road_z(s_j)
    for side in (-1, 1):
        lat0, lat1 = side * (hl + SHOULDER + 90), side * (hl + SHOULDER + 130)
        for k in range(lo - 16, lo + 17, 2):
            # only under the deck; the top follows the deck, so the wall never
            # reaches the road surface where the upper road leaves the bridge
            tops = []
            for kk in (k, k + 2):
                x, y, _ = road_point(idx(kk), (lat0 + lat1) / 2)
                d, zu = under_deck(x, y)
                tops.append(None if d > hu + 40 else zu - 56 - road_z(S[idx(kk)]))
            if None in tops:
                continue
            strip(k, k + 2, min(lat0, lat1), max(lat0, lat1), -20, min(tops), TEX + "concrete", step=2)
    for k in (-8, 0, 8):
        x, y, z = road_point(idx(lo + k))
        entity("light", x, y, z + 150, light="260", _color="1 0.92 0.8")


def gallery():
    """Avalanche gallery: mountain-side wall, roof slab, open pillars to the valley."""
    n = len(PATH)
    i0, i1 = int(GALLERY[0] * n), int(GALLERY[1] * n)
    mid = (i0 + i1) // 2
    (px, py), nn = PATH[mid], NOR[mid]
    hw = half_width(S[mid])
    left_high = macro(px + nn[0] * 900, py + nn[1] * 900) > macro(px - nn[0] * 900, py - nn[1] * 900)
    m = 1 if left_high else -1
    wall0, wall1 = sorted((m * (hw + SHOULDER + 10), m * (hw + SHOULDER + 50)))
    strip(i0, i1, wall0, wall1, -30, 300, TEX + "concrete", step=4)
    roof = {"default": TEX + "concrete", 4: TEX + "roofsnow"}
    lat_a, lat_b = sorted((-m * (hw + SHOULDER + 50), m * (hw + SHOULDER + 50)))
    strip(i0, i1, lat_a, lat_b, 300, 348, roof, step=4)
    out0, out1 = sorted((-m * (hw + SHOULDER + 10), -m * (hw + SHOULDER + 50)))
    for k in range(i0, i1 + 1, 6):
        strip(k, k + 1, out0, out1, -30, 300, TEX + "concrete", step=1)
    # low wall between the pillars
    strip(i0, i1, out0, out1, -30, 36, TEX + "concrete", step=4)
    for k in range(i0 + 3, i1, 8):
        x, y, z = road_point(idx(k))
        entity("light", x, y, z + 250, light="380", _color="1 0.9 0.72")
        lamp = (TEX + "lamp", 48, 48, 64, 64)
        fitted_box((x - 24, y - 24, z + 290), (x + 24, y + 24, z + 301), TEX + "concrete", {"-z": lamp},
                   collision=False)
    BOUNDS["gallery_side"] = m


def kerbs(picks):
    """Red and white kerbs on the inside of every corner apex."""
    for i, apex, r, side in picks:
        if r > 900:
            continue
        hw = half_width(S[apex])
        lat = hw if side == "left" else -hw
        a, b = sorted((lat - 30, lat + 30))
        span = int(min(14, max(6, 4000 / r)))
        for k in range(apex - span, apex + span, 2):
            kk = idx(k)
            decal(kk, a, b, -BOUNDS["step"], BOUNDS["step"], TEX + "kerb")


def signs(picks):
    """Turn signs (the Ridge Racer boards) in front of the corners."""
    for i, apex, r, side in picks:
        if side not in ("left", "right"):
            continue
        j = idx(i - 4)
        hw = half_width(S[j])
        out_lat = -(hw + SHOULDER + 40) if side == "left" else (hw + SHOULDER + 40)
        x, y, z = road_point(j, out_lat)
        z = ground(x, y)
        image = "ridgeracer/turns_02" if r < 420 else "ridgeracer/turns_01"
        facing = (-TAN[j][0], -TAN[j][1])
        sign_board(x, y, z, facing, image, mirror=(side == "left"))


def pickups(picks):
    """Weapons and items for the racing deathmatch modes, on and beside the line."""
    n = len(PATH)
    plan = [(0.085, -120, "weapon_rocketlauncher"), (0.085, 120, "ammo_rockets"),
            (0.115, 0, "rally_item_turbo"), (0.19, -90, "weapon_shotgun"), (0.19, 90, "ammo_shells"),
            (0.27, 0, "rally_weapon_mine"), (0.305, 110, "weapon_plasmagun"), (0.305, -110, "ammo_cells"),
            (0.355, 0, "item_fuelcan"), (0.385, 0, "item_armor_yellow"), (0.475, -100, "rally_weapon_oil"),
            (0.475, 100, "item_health"), (0.565, 0, "weapon_grenadelauncher"), (0.565, -130, "ammo_grenades"),
            (0.60, 120, "rally_item_turbo"), (0.645, -100, "weapon_rocketlauncher"), (0.645, 100, "ammo_rockets"),
            (0.68, 0, "rally_weapon_smoke"), (0.72, -110, "item_health"), (0.72, 110, "ammo_shells"),
            (0.775, 0, "item_health_large"), (0.82, -110, "weapon_flamethrower"), (0.82, 110, "ammo_flame"),
            (0.89, 0, "rally_weapon_bio"), (0.93, -100, "item_armor_green"), (0.93, 100, "rally_item_turbo"),
            (0.985, 0, "item_fuelcan")]
    for f, lat, cls in plan:
        i = at(f)
        if any(min(abs(i - p[0]), n - abs(i - p[0])) < 8 for p in picks):
            i = idx(i + 10)
        x, y, z = road_point(i, lat)
        entity(cls, x, y, z + 24)
    BOUNDS["pickups"] = len(plan)


def trees():
    """Snow-covered pines below the tree line, clear of the road; trunks are solid."""
    rng = random.Random(9)
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    placed = []
    for _ in range(30000):
        x = rng.uniform(x0 + 300, x1 - 300)
        y = rng.uniform(y0 + 300, y1 - 300)
        z = ground(x, y)
        if z > 900 or z < -40:
            continue
        cluster = math.cos(x / 640 + 1.3) * math.cos(y / 520) + 0.6 * math.cos(x / 260) * math.sin(y / 300)
        if cluster < 0.05 + rng.random() * 0.6:
            continue
        d, i = nearest(x, y, 1600)
        edge = d - half_width(S[i])
        if edge < 260:
            continue
        sl = max(abs(ground(x + 48, y) - ground(x - 48, y)), abs(ground(x, y + 48) - ground(x, y - 48))) / 96
        if sl > 0.6 or any(math.dist((x, y), p) < 170 for p in placed):
            continue
        if -1400 < x < 1400 and -3400 < y < -1200:
            continue          # start/finish grounds
        placed.append((x, y))
        scale = 1.3 + rng.random() * 0.8
        entity("misc_model", x, y, z - 10, model="models/mapobjects/GR_trees/nbsnow.md3",
               modelscale=f"{scale:.2f}", angle=str(rng.randrange(360)))
        if edge < 1400:
            box((x - 14, y - 14, z - 12), (x + 14, y + 14, z + 80), TEX + "boundary", collision=False)
        if len(placed) >= 520:
            break
    BOUNDS["trees"] = len(placed)


def rocks():
    rng = random.Random(4)
    count = 0
    for k in range(0, len(PATH), 9):
        hw = half_width(S[k])
        for side in (-1, 1):
            if rng.random() > 0.33:
                continue
            off = hw + SHOULDER + 170 + rng.random() * 220
            x, y, _ = road_point(k, side * off)
            sl = abs(ground(x + 64 * NOR[k][0] * side, y + 64 * NOR[k][1] * side) - ground(x, y)) / 64
            if sl < 0.25:
                continue
            r = 34 + rng.random() * 46
            if any(math.dist((x, y), road_point(j)[:2]) < half_width(S[j]) + SHOULDER + r + 90
                   for j in nearby(x, y, 800)):
                continue
            boulder(x, y, r, r * (0.7 + rng.random() * 0.5), k * 1.37 + side, TEX + "boulder")
            count += 1
    BOUNDS["boulders"] = count


def boundary():
    """Invisible solid walls far outside the road keep cars in the valley."""
    x0, x1 = min(p[0] for p in PATH) - 1100, max(p[0] for p in PATH) + 1100
    y0, y1 = min(p[1] for p in PATH) - 1100, max(p[1] for p in PATH) + 1300
    zt = 4000
    for lo, hi in (((x0 - 32, y0 - 32, FLOOR), (x0, y1 + 32, zt)), ((x1, y0 - 32, FLOOR), (x1 + 32, y1 + 32, zt)),
                   ((x0, y0 - 32, FLOOR), (x1, y0, zt)), ((x0, y1, FLOOR), (x1, y1 + 32, zt))):
        box(lo, hi, TEX + "boundary", collision=False)
    BOUNDS["play"] = (x0, x1, y0, y1)


def shell():
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    top = max(GRID.values()) + 1800
    m = 64
    sky = TEX + "sky"
    for lo, hi, mat in (((x0 - m, y0 - m, FLOOR - 64), (x1 + m, y1 + m, FLOOR), "common/caulk"),
                        ((x0 - m, y0 - m, FLOOR), (x0, y1 + m, top), sky),
                        ((x1, y0 - m, FLOOR), (x1 + m, y1 + m, top), sky),
                        ((x0, y0 - m, FLOOR), (x1, y0, top), sky),
                        ((x0, y1, FLOOR), (x1, y1 + m, top), sky),
                        ((x0 - m, y0 - m, top), (x1 + m, y1 + m, top + m), sky)):
        (ax, ay, az), (bx, by, bz) = lo, hi
        brush([face(plane(nrm, pt), mat, detail=False) for nrm, pt in
               (((0, 0, -1), (ax, ay, az)), ((0, 0, 1), (bx, by, bz)), ((-1, 0, 0), (ax, ay, az)),
                ((1, 0, 0), (bx, by, bz)), ((0, -1, 0), (ax, ay, az)), ((0, 1, 0), (bx, by, bz)))])
    px0, px1, py0, py1 = BOUNDS["play"]
    brush([face(plane(nrm, pt), "common/lightgrid") for nrm, pt in
           (((0, 0, -1), (px0, py0, -200)), ((0, 0, 1), (px1, py1, 1500)), ((-1, 0, 0), (px0, py0, 0)),
            ((1, 0, 0), (px1, py1, 0)), ((0, -1, 0), (px0, py0, 0)), ((0, 1, 0), (px1, py1, 0)))])


def misc_entities():
    n = len(PATH)
    spots = [(0.06, -700, 300, 15), (0.355, 700, 260, 20), (0.43, -650, 260, 20), (0.62, 900, 380, 20),
             (0.80, 800, 260, 15)]
    for f, lat, up, pitch in spots:
        i = at(f)
        x, y, z = road_point(i, lat)
        ang = math.degrees(math.atan2(PATH[i][1] - y, PATH[i][0] - x))
        entity("info_observer_spot", x, y, max(z, ground(x, y)) + up, angles=f"{pitch} {ang:.0f} 0",
               spawnflags="1")
    i = at(0.30)
    x, y, z = road_point(i, -900)
    ang = math.degrees(math.atan2(0 - y, 0 - x))
    entity("info_player_intermission", x, y, ground(x, y) + 700, angles=f"28 {ang:.0f} 0")


# ---------------------------------------------------------------- shaders, textures

LAYER_TEX = (("snow", "textures/q3r_ground/snowy02_slick.jpg", 320),
             ("slush", "textures/q3r_ground/snowy04.jpg", 192),
             ("asphalt", "textures/q3r_road/asph02.jpg", 256),
             ("rock", "textures/arctic/rock1_1.jpg", 256),
             ("cliff", "textures/arctic/rock1_1.jpg", 320))
PARENTS = (("snow", "surfaceparm SURF_SNOW", 0), ("asphalt", "surfaceparm SURF_ASPHALT", 2),
           ("rock", None, 3))


def tcgen(layer):
    name, _, tile = LAYER_TEX[layer]
    k = 1.0 / tile
    if name == "cliff":         # rock faces projected sideways
        return f"\t\ttcGen vector ( {0.71 * k:.7f} {0.71 * k:.7f} 0 ) ( 0 0 {k:.7f} )"
    return f"\t\ttcGen vector ( {k:.7f} 0 0 ) ( 0 {k:.7f} 0 )"


TINT = {"asphalt": "\t\trgbGen const ( 0.52 0.53 0.56 )"}


def stage(layer, blend):
    lines = ["\t{", f"\t\tmap {LAYER_TEX[layer][1]}"]
    if blend:
        lines += ["\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\talphaGen vertex"]
    return lines + [tcgen(layer), TINT.get(LAYER_TEX[layer][0], "\t\trgbGen identity"), "\t}"]


def tail():
    return ["\t{", f"\t\tmap textures/{TEX}macro.jpg", "\t\tblendFunc GL_DST_COLOR GL_SRC_COLOR",
            "\t\ttcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )", "\t\trgbGen identity", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}"]


def lit(name, image, extra=(), tex_extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *tex_extra, "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\trgbGen identity", "\t}", "}", ""]


def decal_shader(name, image, extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", "\tsurfaceparm nonsolid",
            "\tsurfaceparm nomarks", "\tsurfaceparm trans", "\tpolygonOffset", *extra, "\t{", f"\t\tmap {image}",
            "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\trgbGen identity", "\t}", "}", ""]


def write_shader():
    out = ["// Ridge Pass - snowy mountain pass circuit.",
           "// Written by tools/maptools/make_ridgepass.py. Compile with -custinfoparms.", ""]
    out += [f"textures/{TEX}sky", "{", "\tqer_editorimage textures/arctic/arc_sky3", "\tsurfaceparm noimpact",
            "\tsurfaceparm nolightmap", "\tsurfaceparm sky", "\tq3map_globaltexture",
            "\tq3map_sun 1 0.97 0.92 210 215 34", "\tq3map_skylight 150 4",
            "\tq3gl2_sun 1 0.97 0.92 150 215 34 0.5", "\tskyparms env/arcsky/arcsky2 1024 -",
            "\tq3r_haze 0.70 0.74 0.82 6500\t// Q3Rally: cold mountain haze",
            "\tq3r_colorLut gfx/luts/cold\t// Q3Rally: colour grading", "\t{", "\t\tmap textures/skies/dimclouds.tga",
            "\t\ttcMod scroll 0.01 0.005", "\t\ttcMod scale 2 2", "\t\tblendFunc GL_ONE GL_ONE", "\t}", "}", ""]
    out += lit("concrete", "textures/arctic/concrete1_1.jpg", ("\tq3map_lightmapSampleSize 32",))
    out += lit("barrier", "textures/q3r_road/cement_redwhite.tga")
    out += lit("metal", "textures/base_trim/tin.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("checker", "textures/q3r_dirt_grass/checker01.tga")
    out += lit("startbanner", "textures/q3r_signage/q3rstart.jpg")
    out += lit("roof", "textures/q3r_building/roof01.jpg")
    out += lit("pitwall", "textures/q3r_building/concrete02.jpg")
    out += lit("pitsign", "textures/q3r_sponsors/q3rlong01.tga")
    out += lit("garage", "textures/q3r_building/garage_np.jpg")
    out += lit("glass", "textures/q3r_building/windows21A.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("darkglass", "textures/q3r_glass/darkglass.tga")
    out += lit("poster", "textures/q3r_sponsors/q3rposter.tga")
    out += lit("roofmetal", "textures/q3r_metals/cormetal03.tga")
    out += [f"textures/{TEX}logo", "{", "\tqer_editorimage textures/q3r_sponsors/q3rlogo01a.tga",
            "\tsurfaceparm trans", "\tsurfaceparm nonsolid", "\tsurfaceparm nomarks", "\tsurfaceparm nolightmap",
            "\tpolygonOffset",
            "\t{", "\t\tmap textures/q3r_sponsors/q3rlogo01a.tga", "\t\tblendFunc blend", "\t\trgbGen identity", "\t}",
            "}", ""]
    out += [f"textures/{TEX}redlamp", "{", "\tqer_editorimage textures/q3r_signage/white.jpg", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", "\t{", "\t\tmap textures/q3r_signage/white.jpg", "\t\trgbGen const ( 0.95 0.08 0.05 )", "\t}", "}", ""]
    out += lit("bark", "models/mapobjects/GR_trees/stamm.jpg")
    out += lit("boulder", "textures/arctic/rock1_1.jpg", ("\tq3map_shadeAngle 70",))
    out += lit("lamp", "textures/arctic/light2_1.jpg", ("\tq3map_surfacelight 900", "\tsurfaceparm nomarks"),
               ())
    out += [f"textures/{TEX}deck", "{", "\tqer_editorimage textures/q3r_road/asph02.jpg",
            "\tq3map_nonplanar", "\tq3map_shadeAngle 30",
            "\tsurfaceparm SURF_ASPHALT", "\t{", "\t\tmap textures/q3r_road/asph02.jpg",
            "\t\ttcGen vector ( 0.0039063 0 0 ) ( 0 0.0039063 0 )", TINT["asphalt"], "\t}"] + tail() + ["}", ""]
    out += [f"textures/{TEX}roofsnow", "{", "\tqer_editorimage textures/q3r_ground/snowy02_slick.jpg",
            "\tsurfaceparm SURF_SNOW", "\t{", "\t\tmap textures/q3r_ground/snowy02_slick.jpg",
            "\t\ttcGen vector ( 0.003125 0 0 ) ( 0 0.003125 0 )", "\t\trgbGen identity", "\t}"] + tail() + ["}", ""]
    out += decal_shader("line", "textures/q3r_signage/white.jpg")
    out += decal_shader("kerb", "textures/q3r_road/cement_redwhite.tga")
    out += [f"textures/{TEX}propclip", "{", "\tqer_editorimage textures/base_floor/concrete.jpg", "\tqer_trans 0.15",
            "\t// invisible solid hull of a model (floodlight masts): stops cars and shots, lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm detail", "}", ""]
    out += [f"textures/{TEX}boundary", "{", "\tqer_editorimage textures/base_floor/concrete.jpg", "\tqer_trans 0.15",
            "\t// invisible but SOLID (Bullet only takes solid brushes); trans lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm noimpact", "\tsurfaceparm detail", "}", ""]
    out += ["// ground: t_* parents on the brush sides (collision, friction); q3map2 draws",
            f"// ter_<layer> / ter_<a>to<b> from maps/{NAME}_index.pcx. Layers: " +
            ", ".join(f"{i} {n}" for i, (n, _, _) in enumerate(LAYER_TEX)), ""]
    for name, parm, layer in PARENTS:
        out += [f"textures/{TEX}t_{name}", "{", f"\tqer_editorimage {LAYER_TEX[layer][1]}", "\tq3map_indexed",
                "\tq3map_shadeAngle 120", "\tq3map_nonplanar"] + ([f"\t{parm}"] if parm else []) + ["}", ""]
    for i in range(len(LAYER_TEX)):
        out += [f"textures/{TEX}ter_{i}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_nonplanar"] + stage(i, False) + tail() + ["}", ""]
    for i, j in combinations(range(len(LAYER_TEX)), 2):
        out += [f"textures/{TEX}ter_{i}to{j}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_nonplanar"] + stage(i, False) + stage(j, True) + tail() + ["}", ""]
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.shader").write_text("\n".join(out), encoding="utf-8")


def write_textures():
    from PIL import Image, ImageFilter
    out = ROOT / "baseq3r" / "textures" / "q3r_ridgepass"
    out.mkdir(parents=True, exist_ok=True)
    rnd = random.Random(21)
    size = 512
    acc = [0.0] * (size * size)
    for cells, weight in ((4, 1.0), (8, 0.6), (16, 0.35), (32, 0.18)):
        small = Image.new("F", (cells, cells))
        small.putdata([rnd.random() for _ in range(cells * cells)])
        tiled = Image.new("F", (cells * 3, cells * 3))
        for i in range(3):
            for j in range(3):
                tiled.paste(small, (i * cells, j * cells))
        big = tiled.resize((size * 3, size * 3), Image.BICUBIC).crop((size, size, 2 * size, 2 * size))
        acc = [a + weight * v for a, v in zip(acc, big.get_flattened_data())]
    lo, hi = min(acc), max(acc)
    m = Image.new("L", (size, size))
    m.putdata([int(128 + ((v - lo) / (hi - lo) - 0.5) * 84) for v in acc])
    m.filter(ImageFilter.GaussianBlur(1.2)).convert("RGB").save(out / "macro.jpg", quality=92)


def index_map():
    from PIL import Image
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    w, h = (x1 - x0) // STEP + 1, (y1 - y0) // STEP + 1
    im = Image.new("L", (w, h))
    im.putdata([MAT[x0 + i * STEP, y1 - j * STEP] for j in range(h) for i in range(w)])
    im.save(ROOT / "baseq3r" / "maps" / f"{NAME}_index.pcx")


# ---------------------------------------------------------------- checks and output

def speed_profile():
    vmax, alat, acc, brake = 560.0, 900.0, 160.0, 450.0
    n = len(PATH)
    step = BOUNDS["step"]
    lim = []
    for i in range(n):
        grade = (road_z(S[(i + 1) % n if i + 1 < n else 0] + 0) - road_z(S[i])) / step
        lim.append(min(vmax, math.sqrt(alat / max(1e-6, abs(KAPPA[i])))))
    v = lim[:]
    for _ in range(3):
        for i in range(n):
            g = (road_z(S[i]) - road_z(S[i - 1])) / step
            v[i] = min(lim[i], math.sqrt(max(0.0, v[i - 1] ** 2 + 2 * (acc - 800 * g) * step)))
        for i in range(n - 1, -1, -1):
            v[i] = min(v[i], math.sqrt(v[(i + 1) % n] ** 2 + 2 * brake * step))
    return v


def validate(picks):
    n = len(PATH)
    step = BOUNDS["step"]
    grades = [abs(road_z(S[(i + 1) % n]) - road_z(S[i])) / step for i in range(n)]
    near_span = {idx(k + d) for k in SPAN for d in range(-4, 5)}
    worst = max(abs(ground(*PATH[i]) - road_z(S[i])) for i in range(0, n, 3) if i not in near_span)
    v = speed_profile()
    lap = sum(step / max(30.0, (v[i] + v[(i + 1) % n]) / 2) for i in range(n))
    # clearance at the crossing
    lo, up = CROSS["low"], CROSS["up"]
    clearance = road_z(S[up]) - 48 - road_z(S[lo])
    assert clearance > 180, clearance
    near_span |= {idx(CROSS["low"] + d) for d in range(-20, 21)}
    # stacked legs: distance between road edges of far-apart parts of the lap
    close = 1e9
    for i in range(0, n, 3):
        for j in nearby(*PATH[i], 1400):
            if min(abs(j - i), n - abs(j - i)) * step > 2500 and not (i in near_span or j in near_span):
                gap = math.dist(PATH[i], PATH[j]) - half_width(S[i]) - half_width(S[j])
                if gap > -50:
                    close = min(close, gap)
    starts = [e for e in ENTITIES if e["classname"] in ("info_player_start", "info_player_deathmatch")]
    for e in starts:
        x, y, z = map(float, e["origin"].split())
        for slo, shi in SOLIDS:
            assert not (x + 60 > slo[0] and x - 60 < shi[0] and y + 60 > slo[1] and y - 60 < shi[1]
                        and z + 30 > slo[2] and z - 30 < shi[2]), e
        assert z - 26 > max(ground(x + dx, y + dy) for dx in (-50, 50) for dy in (-50, 50)), e
    items = [e for e in ENTITIES if e["classname"].startswith(("weapon_", "ammo_", "item_", "rally_"))]
    for e in items:
        x, y, z = map(float, e["origin"].split())
        assert abs(z - 24 - ground(x, y)) < 30, e
    return {"map": NAME, "lap_length": round(length()), "estimated_lap_s": round(lap, 1),
            "min_speed_estimate": round(min(v)), "max_grade_pct": round(100 * max(grades), 1),
            "road_mesh_error": round(worst, 2), "bridge_clearance": round(clearance),
            "closest_legs_gap": round(close), "terrain_triangles": len(TRIS),
            "leaves": {s: sum(1 for leaf in LEAVES if leaf[2] == s) for s in (64, 128, 256, 512)},
            "brushes": len(BRUSHES) + sum(len(g) for g in GROUPS.values()), "checkpoints": CHECKPOINTS,
            "trees": BOUNDS["trees"], "boulders": BOUNDS["boulders"], "pickups": BOUNDS["pickups"]}


def overview(report):
    from PIL import Image, ImageDraw
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    scale = max(x1 - x0, y1 - y0) / 1200
    w, h = int((x1 - x0) / scale), int((y1 - y0) / scale)
    im = Image.new("RGB", (w, h))
    pix = im.load()
    colors = {L_SNOW: (232, 236, 244), L_SLUSH: (170, 170, 172), L_ASPHALT: (70, 72, 78),
              L_ROCK: (120, 112, 104), L_CLIFF: (96, 88, 84)}
    zmin, zmax = min(GRID.values()), max(GRID.values())
    for v in range(h):
        y = y1 - v * scale
        gy = y0 + round((y - y0) / STEP) * STEP
        for u in range(w):
            x = x0 + u * scale
            gx = x0 + round((x - x0) / STEP) * STEP
            z = GRID.get((gx, gy), zmin)
            zx = GRID.get((gx + STEP, gy), z)
            zy = GRID.get((gx, gy + STEP), z)
            shade = 0.62 + 0.38 * (z - zmin) / (zmax - zmin) - (zx - z) / STEP * 0.25 + (zy - z) / STEP * 0.12
            c = colors[MAT.get((gx, gy), L_SNOW)]
            pix[u, v] = tuple(max(0, min(255, int(k * shade))) for k in c)
    d = ImageDraw.Draw(im)
    for e in ENTITIES:
        x, y, _ = map(float, e["origin"].split())
        u, vv = (x - x0) / scale, (y1 - y) / scale
        if e["classname"] == "info_notnull":
            d.ellipse((u - 7, vv - 7, u + 7, vv + 7), outline=(0, 120, 255), width=3)
            d.text((u + 9, vv - 9), e["targetname"], fill=(0, 60, 200))
        elif e["classname"] == "misc_model":
            d.ellipse((u - 2, vv - 2, u + 2, vv + 2), fill=(40, 90, 50))
        elif e["classname"].startswith(("weapon_", "rally_weapon")):
            d.rectangle((u - 3, vv - 3, u + 3, vv + 3), fill=(220, 40, 40))
        elif e["classname"].startswith(("item_", "rally_item", "ammo_")):
            d.rectangle((u - 3, vv - 3, u + 3, vv + 3), fill=(230, 160, 0))
    out = ROOT / "docs" / "maps" / "ridgepass"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "ridgepass_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def write_route():
    """Intro camera flight along the lap (same format as the recorded routes)."""
    n = len(PATH)
    step = BOUNDS["step"]
    v = speed_profile()
    lines = ["Q3RALLY_INTRO_ROUTE 1", f"map {NAME}", "track_length 1", "track_reversed 0"]
    frames, t, i = [], 0.0, BOUNDS["i_sf"]
    target = 18.0
    lap = sum(step / max(30.0, v[k]) for k in range(n))
    k = 0
    while k <= n:
        j = idx(i + k)
        x, y, z = road_point(j)
        pitch = -math.degrees(math.atan2(road_z(S[idx(j + 1)]) - road_z(S[j]), step))
        frames.append(f"{round(t * 1000)} {x:.3f} {y:.3f} {z + 40:.3f} {pitch:.3f} {heading_deg(j):.3f} 0.000")
        dt = 0.0
        for _ in range(2):
            dt += step / max(30.0, v[idx(j + _)]) * target / lap
            k += 1
        t += dt
    lines.append(f"frames {len(frames)}")
    out = ROOT / "baseq3r" / "intro_routes"
    out.mkdir(parents=True, exist_ok=True)
    (out / f"{NAME}_tl1_rev0.route").write_text("\n".join(lines + frames) + "\n", encoding="ascii")


def write_map(picks):
    fields = {"classname": "worldspawn", "message": "Ridge Pass", "_ambient": "24", "_minlight": "16",
              "gridsize": "128 128 256", "_blocksize": "512 512 4096",
              "reflectionImage": "/textures/reflect/ridgeracer.jpg",
              "weather": "snow", "weatherChance": "60", "weatherWind": "60 300"}
    text = ["// Generated by tools/maptools/make_ridgepass.py. Q3 brush format (signs: brush primitives).",
            f"// The terrain func_groups use maps/{NAME}_index.pcx at compile time.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    for group in ("snow", "asphalt", "rock"):
        keys = {"classname": "func_group", "_terrain": f"ridge pass ground, {group} friction",
                "_indexmap": f"maps/{NAME}_index.pcx", "_layers": str(len(LAYER_TEX)),
                "_shader": TEX + "ter", "min": f"{x0} {y0} {FLOOR}", "max": f"{x1} {y1} 4096"}
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()))
        text += GROUPS.get(group, [])
        text.append("}")
    text += TRIGGERS
    for ent in ENTITIES:
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in ent.items()) + "\n}")
    target = ROOT / "baseq3r" / "maps" / f"{NAME}.map"
    target.write_text("\n".join(text) + "\n", encoding="ascii")
    arena = ROOT / "baseq3r" / "scripts" / f"{NAME}.arena"
    arena.write_text(
        "{\n"
        f'\tmap "{NAME}"\n'
        '\tlongname "Ridge Pass"\n'
        '\tauthor "Q3Rally Team"\n'
        '\ttype "q3r_racing q3r_team_racing q3r_racing_dm q3r_team_racing_dm q3r_elimination"\n'
        '\telimination "1"\n'
        '\tstarts "12"\n'
        '\tlaps "3"\n'
        f'\tcheckpoints "{len(picks)}"\n'
        '\tobserverspots "5"\n'
        '\tweapons "SG,GL,RL,PG,FT"\n'
        '\tpowerups "3"\n'
        '\treversable "0"\n'
        '\ttracklengths "1"\n'
        '\tbotsupport "None"\n'
        '\tteams "All"\n'
        "}\n", encoding="ascii")
    return target


def main():
    build_path()
    build_profile()
    hash_path()
    find_crossing()
    build_terrain()
    picks = course()
    start_finish()
    grandstand()
    bridge()
    gallery()
    kerbs(picks)
    signs(picks)
    pickups(picks)
    trees()
    rocks()
    boundary()
    misc_entities()
    shell()
    report = validate(picks)
    write_textures()
    write_shader()
    index_map()
    target = write_map(picks)
    write_route()
    overview(report)
    print(json.dumps({k: v for k, v in report.items() if k != "checkpoints"}, indent=2))
    for c in report["checkpoints"]:
        print(c)
    print("Wrote", target)


if __name__ == "__main__":
    main()
