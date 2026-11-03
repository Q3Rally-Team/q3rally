#!/usr/bin/env python3
"""Generate q3r_riviera (Riviera Run): an A to B sprint along a Mediterranean coast.

From the lighthouse on a cape down to a fishing harbour, at sunset, the sea
always on the right (south):

  1. start on the cape plateau next to the lighthouse,
  2. two hairpins down the cliff,
  3. the cliff road above the sea, guard rails on the sea side,
  4. a tunnel through a headland,
  5. a bridge on arches across a bay,
  6. a climb over a second headland through olive trees,
  7. a horseshoe bend around a ravine down to the old town,
  8. narrow streets between the houses,
  9. the harbour quay to the finish.

The ground is one height field (as in Ridge Pass): a coast landscape that is
cut and filled to the road. Near the road the cuttings are steep rock, the sea
side ends in guard rails with invisible walls above them, so nobody can drive
off the cliff or cut a corner. Ground layers are blended per vertex with a
q3map2 index map; collision is closed triangular detail brushes on a
64/128/256/512 grid, fine only near the road.

Writes the .map, index map, shader script, arena, intro camera route, the
generated textures, an overview image and docs/maps/riviera/validation.json.
Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from itertools import combinations
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_riviera"
TEX = "q3r_riviera/"
DETAIL = 134217728
STEP = 64
FLOOR = -640
SAMPLE = 32
SHOULDER = 40           # gravel strip beside the asphalt
CUT, FILL = 1.15, 1.0    # side slopes of the cuttings and embankments
SEA_Z = 0
START_AT, FINISH_AT = 900, 520      # units from either end of the centre line

# centre line control points with the road height there: (x, y, z), start (A) to finish (B)
CTRL = [(-14800, -1650, 900), (-13800, -1450, 890), (-12700, -1200, 870), (-11600, -1080, 830),
        (-10500, -1060, 790),
        (-9750, -1250, 760), (-9520, -1720, 740), (-9850, -2120, 715), (-10450, -2260, 690),          # hairpin 1
        (-11300, -2320, 620),
        (-11950, -2560, 580), (-12080, -3030, 560), (-11650, -3370, 535), (-10900, -3460, 500),     # hairpin 2
        (-9800, -3380, 430), (-8600, -3150, 380), (-7400, -3380, 340), (-6200, -3150, 310),        # cliff road
        (-5000, -3330, 290),
        (-4000, -3250, 280), (-3000, -3180, 275),                                                  # tunnel
        (-2000, -3120, 270), (-1000, -3160, 268), (-100, -3300, 270),                              # bridge
        (900, -3150, 300), (1900, -2700, 380), (2900, -2700, 440), (3700, -2300, 450),            # headland
        (4300, -1600, 400), (4500, -900, 340), (5000, -450, 300), (5550, -780, 260),             # horseshoe
        (5750, -1550, 200), (6050, -2350, 120),
        (6800, -2780, 70), (7700, -2900, 50), (8400, -3020, 44), (8950, -3350, 40),               # old town
        (9600, -3720, 36), (10600, -3780, 34), (11700, -3760, 34)]                                # quay

# keyframes along the course by position (fraction of its length)
HALF_WIDTH = [(0.00, 300), (0.04, 280), (0.10, 240), (0.14, 270), (0.22, 270), (0.26, 235), (0.40, 230),
              (0.44, 240), (0.56, 230), (0.60, 225), (0.75, 240), (0.82, 225), (0.86, 190), (0.93, 190),
              (0.95, 260), (1.00, 300)]

LAYERS = ("grass", "gravel", "asphalt", "rock", "cliff", "sand")
L_GRASS, L_GRAVEL, L_ASPHALT, L_ROCK, L_CLIFF, L_SAND = range(6)
FRICTION = {L_GRASS: "grass", L_GRAVEL: "gravel", L_ASPHALT: "asphalt", L_ROCK: "rock", L_CLIFF: "rock",
            L_SAND: "sand"}

PATH, S, TAN, NOR, KAPPA, ZTAB = [], [], [], [], [], []
CTRL_S = []
GRID, MAT, LEAVES, LEAFOF, TRIS = {}, {}, [], {}, []
BRUSHES, GROUPS, ENTITIES, SOLIDS, TRIGGERS = [], {}, [], [], []
BOUNDS = {}
SECTIONS = {}       # name -> (first sample, last sample)
BRIDGE, TUNNEL, TOWN, QUAY = set(), set(), set(), set()
SEASIDE = {}        # sample -> side of the sea (+1 left, -1 right) where the ground drops away
COVERED = {-1: set(), 1: set()}     # side -> samples that already have a wall on that side
PLACED = {}                         # side -> sample -> lateral distance of the land side wall


def smooth(a, b, x):
    t = min(1.0, max(0.0, (x - a) / (b - a)))
    return t * t * (3 - 2 * t)


def keyed(keys, f):
    for (f0, v0), (f1, v1) in zip(keys, keys[1:]):
        if f <= f1:
            return v0 + (v1 - v0) * smooth(f0, f1, f)
    return keys[-1][1]


# ---------------------------------------------------------------- course

def build_path():
    pts2 = [(p[0], p[1]) for p in CTRL]
    pts = [pts2[0]] + pts2 + [pts2[-1]]
    dense, marks = [], []
    for i in range(1, len(pts) - 2):
        p0, p1, p2, p3 = pts[i - 1], pts[i], pts[i + 1], pts[i + 2]
        marks.append(len(dense))
        n = max(4, int(math.dist(p1, p2) / 8))
        for k in range(n):
            t = k / n
            dense.append(tuple(0.5 * ((2 * p1[j]) + (-p0[j] + p2[j]) * t
                                      + (2 * p0[j] - 5 * p1[j] + 4 * p2[j] - p3[j]) * t * t
                                      + (-p0[j] + 3 * p1[j] - 3 * p2[j] + p3[j]) * t ** 3)
                               for j in range(2)))
    marks.append(len(dense))
    dense.append(pts2[-1])
    # arc length of the dense polyline, the control points' positions on it
    acc_d = [0.0]
    for a, b in zip(dense, dense[1:]):
        acc_d.append(acc_d[-1] + math.dist(a, b))
    CTRL_S.extend(acc_d[m] for m in marks)
    PATH.append(dense[0])
    acc = 0.0
    for a, b in zip(dense, dense[1:]):
        seg = math.dist(a, b)
        while acc + seg >= SAMPLE:
            t = (SAMPLE - acc) / seg
            a = (a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t)
            seg = math.dist(a, b)
            PATH.append(a)
            acc = 0.0
        acc += seg
    n = len(PATH)
    for i, p in enumerate(PATH):
        S.append(i * SAMPLE)
        q0, q1 = PATH[max(0, i - 1)], PATH[min(n - 1, i + 1)]
        d = math.dist(q0, q1)
        t = ((q1[0] - q0[0]) / d, (q1[1] - q0[1]) / d)
        TAN.append(t)
        NOR.append((-t[1], t[0]))                     # left of the driving direction
    heading = [math.atan2(t[1], t[0]) for t in TAN]
    raw = []
    for i in range(n):
        a, b = heading[max(0, i - 1)], heading[min(n - 1, i + 1)]
        db = (b - a + math.pi) % (2 * math.pi) - math.pi
        raw.append(db / (2 * SAMPLE))
    w = 8
    for i in range(n):
        part = raw[max(0, i - w): i + w + 1]
        KAPPA.append(sum(part) / len(part))


def length():
    return S[-1]


def frac(s):
    return min(1.0, max(0.0, s / length()))


def half_width(s):
    return keyed(HALF_WIDTH, frac(s))


def build_profile():
    """Road height per sample: the control point heights joined linearly over
    the arc length, then smoothed over +-640 units."""
    n = len(PATH)
    zs = [p[2] for p in CTRL]
    lin = []
    for i in range(n):
        s = S[i]
        k = 0
        while k < len(CTRL_S) - 2 and CTRL_S[k + 1] < s:
            k += 1
        s0, s1 = CTRL_S[k], CTRL_S[k + 1]
        t = min(1.0, max(0.0, (s - s0) / max(1.0, s1 - s0)))
        lin.append(zs[k] + (zs[k + 1] - zs[k]) * t)
    w = 20
    for i in range(n):
        part = lin[max(0, i - w): i + w + 1]
        ZTAB.append(sum(part) / len(part))


def road_z(s):
    n = len(PATH)
    u = min(n - 1.0, max(0.0, s / SAMPLE))
    i = min(n - 2, int(u))
    t = u - i
    return ZTAB[i] * (1 - t) + ZTAB[i + 1] * t


def bank(i):
    return max(-0.08, min(0.08, -120 * KAPPA[i]))


def idx(i):
    return max(0, min(len(PATH) - 1, i))


def sample_at(x, y):
    """Nearest sample to a point (used to find the sections from the control points)."""
    return min(range(len(PATH)), key=lambda i: (PATH[i][0] - x) ** 2 + (PATH[i][1] - y) ** 2)


def sections():
    def span(a, b):
        return sample_at(*CTRL[a][:2]), sample_at(*CTRL[b][:2])
    SECTIONS.update(start=(0, sample_at(*CTRL[4][:2])), hairpins=span(4, 13), cliff=span(13, 18),
                    tunnel=span(19, 20), bridge=span(21, 23), headland=span(23, 27), horseshoe=span(27, 33),
                    town=span(34, 37), quay=(sample_at(*CTRL[38][:2]), len(PATH) - 1))
    t0, t1 = SECTIONS["tunnel"]
    TUNNEL.update(range(t0 - 6, t1 + 7))
    b0, b1 = SECTIONS["bridge"]
    BRIDGE.update(range(b0 - 2, b1 + 3))         # exactly the deck (bridge() builds a0..a1)
    w0, w1 = SECTIONS["town"]
    TOWN.update(range(w0 - 20, w1 + 1))
    q0, q1 = SECTIONS["quay"]
    QUAY.update(range(q0 - 24, q1 + 1))


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
    """The closest centre line sample of every part of the course in reach."""
    cand = sorted(i for i in nearby(x, y, radius)
                  if abs(x - PATH[i][0]) <= radius and abs(y - PATH[i][1]) <= radius)
    groups, prev = [], -10
    for i in cand:
        if i - prev > 2:
            groups.append([])
        groups[-1].append(i)
        prev = i
    return [min(g, key=lambda i: (x - PATH[i][0]) ** 2 + (y - PATH[i][1]) ** 2) for g in groups]


def nearest(x, y, radius=2000):
    best, bi = 1e18, 0
    for i in nearby(x, y, radius):
        d = (x - PATH[i][0]) ** 2 + (y - PATH[i][1]) ** 2
        if d < best:
            best, bi = d, i
    return math.sqrt(best), bi


def local(i, x, y):
    """Continuous position along the course, signed lateral offset (left +).
    Beyond the ends the road ends in a round head."""
    px, py = PATH[i]
    dx, dy = x - px, y - py
    along = dx * TAN[i][0] + dy * TAN[i][1]
    lat = dx * NOR[i][0] + dy * NOR[i][1]
    if (i == 0 and along < 0) or (i == len(PATH) - 1 and along > 0):
        return S[i], math.copysign(math.hypot(lat, along), lat or 1)
    s = S[i] + max(-SAMPLE / 2, min(SAMPLE / 2, along))
    return s, lat


# ---------------------------------------------------------------- landscape

COAST = [(-17000, -2700), (-15900, -2600), (-15250, -2750), (-14500, -3600), (-13200, -4000),
         (-11000, -4000), (-9000, -3850), (-7500, -3900),
         (-6000, -3700), (-4800, -3800), (-4300, -4500), (-3000, -4700), (-2300, -4100),
         (-2000, -2500), (-1000, -2150), (-200, -2500), (300, -3700), (1500, -3800), (2600, -3600),
         (3800, -3400), (5200, -3200), (6500, -3300), (7600, -3450), (8600, -3550), (9300, -4000),
         (11000, -4120), (14000, -4100)]


def coast_distance(x, y):
    """Signed distance to the coast line: + on land (north), - in the sea."""
    best, sign = 1e18, 1
    for (ax, ay), (bx, by) in zip(COAST, COAST[1:]):
        dx, dy = bx - ax, by - ay
        L2 = dx * dx + dy * dy
        t = max(0.0, min(1.0, ((x - ax) * dx + (y - ay) * dy) / L2))
        px, py = ax + dx * t, ay + dy * t
        d = math.hypot(x - px, y - py)
        if d < best:
            best = d
            sign = 1 if (dx * (y - ay) - dy * (x - ax)) > 0 else -1
    return sign * best


def land_height(x, y):
    """Height of the land without the road: high cape in the west, headlands,
    low ground at the town in the east, hills inland."""
    h = 820 * (1 - smooth(-11000, -8500, x)) + 300 * smooth(-11000, -8500, x)
    h += 260 * math.exp(-((x + 3500) / 1500) ** 2)                  # tunnel headland
    h += 180 * math.exp(-((x - 2600) / 1700) ** 2)                  # second headland
    h -= 260 * smooth(4500, 7000, x)                                # down to the town
    h += 0.22 * max(0.0, y + 2600) * (1 - 0.6 * smooth(5500, 8000, x))   # hills inland
    h += 90 * math.sin(x / 900 + 0.3) * math.cos(y / 700) + 40 * math.cos(x / 310) * math.sin(y / 270)
    return max(h, 30.0)


def macro(x, y):
    c = coast_distance(x, y)
    hl = land_height(x, y)
    if c >= 0:
        # cliffs: steep where the land is high, a beach where it is low
        width = 120 + 900 * (1 - smooth(150, 500, hl))
        return SEA_Z - 30 + (hl + 30) * smooth(0, width, c) ** 0.7
    return SEA_Z - 30 - 330 * smooth(0, 1400, -c)


def road_bounds(i, x, y):
    """(lower bound, upper bound, distance beyond the asphalt edge) for one leg."""
    s, dsig = local(i, x, y)
    d = abs(dsig)
    hw = half_width(s)
    h = road_z(s)
    if i in BRIDGE:
        # under the deck the ground stays clear of it; beside it the ground may come
        # up steeply against the deck side (abutment), so no pit opens in a hillside
        # At both ends the deck lies on the road for a few samples with the ground
        # just inside it (no gap before the deck, no flicker on it), then the ground
        # drops away inside the solid deck.
        e = max(0.0, d - hw - SHOULDER - 34)
        r = min(i - min(BRIDGE), max(BRIDGE) - i)
        if r <= 1:
            # the first deck piece has no drawn top: the road surface runs on under it
            ze = h + bank(i) * max(-hw - SHOULDER, min(hw + SHOULDER, dsig))
            return ze - e * FILL, ze + e * 2.6, d - hw
        if r < 5:
            # banked like the deck above it, 10 units down: below the deck surface
            # (the mesh strays up to 7 units on the banked end) with only a small step
            zd = h + bank(i) * max(-hw - SHOULDER - 30, min(hw + SHOULDER + 30, dsig)) - 10
            return zd - e * FILL, zd + e * 2.6, d - hw
        return -1e9, h - 120 + e * 2.6, d - hw
    ze = h + bank(i) * max(-hw - SHOULDER, min(hw + SHOULDER, dsig))
    if d <= hw + SHOULDER:
        return ze, ze, d - hw
    e = d - hw - SHOULDER
    cut = 2.6 if i in TUNNEL else CUT
    fill = 0.7 if (i in TOWN or i in QUAY) else FILL
    return ze - e * fill, ze + e * cut, d - hw


def base(x, y, legs):
    m = macro(x, y)
    legs = [i for i in legs if i not in BRIDGE]      # the bay stays a bay under the bridge
    if not legs:
        return m
    acc = wsum = 0.0
    pull = 0.0
    for i in legs:
        s, dsig = local(i, x, y)
        e = max(0.0, abs(dsig) - half_width(s))
        w = math.exp(-(e / 380) ** 2) + 1e-9
        acc += w * road_z(s)
        wsum += w
        reach = 220 if i in BRIDGE else (900 if i in TOWN else 520)
        pull = max(pull, math.exp(-(e / reach) ** 2))
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


# ---------------------------------------------------------------- terrain mesh

def build_terrain():
    xs = [p[0] for p in PATH]
    ys = [p[1] for p in PATH]
    x0 = math.floor((min(xs) - 2600) / 512) * 512
    x1 = math.ceil((max(xs) + 2600) / 512) * 512
    y0 = math.floor((min(ys) - 2800) / 512) * 512
    y1 = math.ceil((max(ys) + 2600) / 512) * 512
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
        limit = {512: 2000, 256: 650, 128: 160}.get(size)
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
    for (lx, ly, size) in sorted(LEAVES, key=lambda c: -c[2]):
        for (ax, ay), (bx, by) in (((lx, ly), (lx + size, ly)), ((lx + size, ly), (lx + size, ly + size)),
                                   ((lx, ly + size), (lx + size, ly + size)), ((lx, ly), (lx, ly + size))):
            n = size // STEP
            for k in range(1, n):
                GRID[ax + (bx - ax) * k // n, ay + (by - ay) * k // n] = \
                    GRID[ax, ay] + (GRID[bx, by] - GRID[ax, ay]) * k / n
    for k in GRID:
        GRID[k] = round(GRID[k] * 8) / 8
    for (x, y), z in raw.items():
        gx = (raw.get((x + STEP, y), z) - raw.get((x - STEP, y), z)) / (2 * STEP)
        gy = (raw.get((x, y + STEP), z) - raw.get((x, y - STEP), z)) / (2 * STEP)
        slope = math.hypot(gx, gy)
        e = edge[x, y]
        if e < -8:
            m = L_ASPHALT
        elif e < SHOULDER + 8:
            m = L_GRAVEL
        elif slope > 1.25:
            m = L_CLIFF
        elif slope > 0.7:
            m = L_ROCK
        elif z < SEA_Z + 45:
            m = L_GRAVEL if e < 700 else L_SAND           # the quay and the town are paved
        else:
            patch = math.cos(x / 410) * math.cos(y / 370) + 0.5 * math.cos(x / 157) * math.cos(y / 181)
            m = L_ROCK if (patch > 1.1 and slope > 0.3 and e > 300) else L_GRASS
        MAT[x, y] = m
    order = ("rock", "grass", "sand", "gravel", "asphalt")
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
            if classes.count("asphalt") >= 2:
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


def face(points, material, scale=0.5, detail=True, tex=None):
    """tex: (x offset, y offset, x scale, y scale) instead of the plain scale."""
    p = " ".join("( %s )" % " ".join(num(v) for v in xyz) for xyz in points)
    xo, yo, ss, ts = tex or (0, 0, scale, scale)
    return f"{p} {material} {num(xo)} {num(yo)} 0 {num(ss)} {num(ts)} {DETAIL if detail else 0} 0 0"


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
    """(outward normal, point, material[, texture fit]) planes."""
    brush([face(plane(q[0], q[1]), q[2], scale=scale, detail=detail, tex=q[3] if len(q) > 3 else None)
           for q in planes])
    if solid:
        corners = polytope([(q[0], q[1]) for q in planes])
        if corners:
            SOLIDS.append((tuple(min(c[i] for c in corners) for i in range(3)),
                           tuple(max(c[i] for c in corners) for i in range(3))))


def obox(cx, cy, t, along, across, z0, z1, mat, side=None, solid=True, top=None, bottom=None, scale=0.5):
    """Box turned to the direction t (unit), centred on cx, cy."""
    n = (-t[1], t[0])
    side = side or mat
    convex([((t[0], t[1], 0), (cx + t[0] * along, cy + t[1] * along, z0), side),
            ((-t[0], -t[1], 0), (cx - t[0] * along, cy - t[1] * along, z0), side),
            ((n[0], n[1], 0), (cx + n[0] * across, cy + n[1] * across, z0), mat),
            ((-n[0], -n[1], 0), (cx - n[0] * across, cy - n[1] * across, z0), mat),
            ((0, 0, 1), (cx, cy, z1), top or side), ((0, 0, -1), (cx, cy, z0), bottom or side)],
           solid=solid, scale=scale)


def hexa(corners, mats, solid=True, scale=0.5, texfit=None):
    """Convex 8-corner block: corners 0-3 one end (ring order), 4-7 the other.
    mats: one material or a dict per face index (0 end a, 1 end b, 2-5 sides).
    texfit: face index -> (x offset, y offset, x scale, y scale)."""
    c = [sum(p[i] for p in corners) / 8 for i in range(3)]
    quads = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
    planes = []
    for k, q in enumerate(quads):
        p0, p1, p2 = corners[q[0]], corners[q[1]], corners[q[2]]
        n = cross(tuple(b - a for a, b in zip(p0, p1)), tuple(b - a for a, b in zip(p0, p2)))
        if sum(nc * (pc - cc) for nc, pc, cc in zip(n, p0, c)) < 0:
            n = tuple(-v for v in n)
        m = mats.get(k, mats.get("default")) if isinstance(mats, dict) else mats
        planes.append((n, p0, m, (texfit or {}).get(k)))
    convex(planes, solid=solid, scale=scale)


def strip(i0, i1, lat0, lat1, z0, z1, mats, step=2, follow=True, solid=True, scale=0.5):
    """Blocks along the centre line from sample i0 to i1, lateral lat0..lat1
    (left +; either may be a function of the sample, so the block follows a
    changing road width). follow: heights relative to the road surface."""
    for k in range(i0, i1, step):
        a, b = idx(k), idx(min(k + step, i1))
        if a == b:
            continue
        corners = []
        for j in (a, b):
            (px, py), n = PATH[j], NOR[j]
            l0 = lat0(j) if callable(lat0) else lat0
            l1 = lat1(j) if callable(lat1) else lat1
            for lat, z in ((l0, z0), (l1, z0), (l1, z1), (l0, z1)):
                zz = z + (road_z(S[j]) + bank(j) * lat if follow else 0)
                corners.append((px + n[0] * lat, py + n[1] * lat, zz))
        hexa(corners, mats, solid=solid, scale=scale)


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


def box(lo, hi, material, collision=True, faces=None, scale=0.5):
    x1, y1, z1 = lo
    x2, y2, z2 = hi
    assert x2 > x1 and y2 > y1 and z2 > z1, (lo, hi)
    keys = ("bot", "top", "-x", "+x", "-y", "+y")
    normals = ((0, 0, -1), (0, 0, 1), (-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0))
    points = ((x1, y1, z1), (x2, y2, z2), (x1, y1, z1), (x2, y2, z2), (x1, y1, z1), (x2, y2, z2))
    brush([face(plane(n, p), (faces or {}).get(k, material), scale=scale) for k, n, p in zip(keys, normals, points)])
    if collision:
        SOLIDS.append((lo, hi))


def entity(classname, x, y, z, **keys):
    fields = {"classname": classname, "origin": f"{round(x)} {round(y)} {round(z)}", **keys}
    ENTITIES.append(fields)
    return fields


def heading_deg(i):
    return math.degrees(math.atan2(TAN[i][1], TAN[i][0])) % 360


def road_point(i, lat=0.0):
    (px, py), n = PATH[i], NOR[i]
    x, y = px + n[0] * lat, py + n[1] * lat
    return x, y, road_z(S[i]) + bank(i) * lat


def at(s):
    return idx(int(round(s / SAMPLE)))


# ---------------------------------------------------------------- the sea side

def find_seaside():
    """Per sample: the side where the ground falls away (sea, bay, ravine).
    Guard rails and the invisible walls go there."""
    for i in range(len(PATH)):
        if i in TOWN:
            continue
        hw = half_width(S[i])
        drops = {}
        for side in (-1, 1):
            x, y, z = road_point(i, side * (hw + SHOULDER + 260))
            drops[side] = z - ground(x, y)
        side = max(drops, key=drops.get)
        if drops[side] > 90 or i in BRIDGE or i in QUAY:
            SEASIDE[i] = side


def runs(indices):
    """Consecutive runs of sample numbers."""
    out = []
    for i in sorted(indices):
        if out and i == out[-1][1] + 1:
            out[-1][1] = i
        else:
            out.append([i, i])
    return out


def guard_rails():
    """Steel guard rail on posts on the drop side, invisible wall above it;
    on the bridge and the quay stone parapets do the same job."""
    plain = [i for i, s in SEASIDE.items() if i not in BRIDGE and i not in QUAY and i not in TUNNEL]
    for side in (-1, 1):
        for a, b in runs(i for i in plain if SEASIDE[i] == side):
            if b - a < 4:
                continue
            def edge(off, side=side):
                return lambda j: side * (half_width(S[j]) + SHOULDER + off)
            strip(a, b, edge(-6), edge(0), 26, 44, TEX + "rail", step=4)
            for k in range(a, b, 4):
                x, y, z = road_point(k, side * (half_width(S[k]) + SHOULDER - 2))
                obox(x, y, TAN[k], 4, 4, ground(x, y) - 30, z + 40, TEX + "railpost")
            strip(a, b, edge(0), edge(24), -20, 420, TEX + "boundary", step=4, solid=False)
            COVERED[side].update(range(a, b + 1))
    # the town: house fronts do it; the ends of the course: walls across
    for i, back in ((0, -1), (len(PATH) - 1, 1)):
        (px, py), t = PATH[i], TAN[i]
        hw = half_width(S[i])
        x, y = px + t[0] * back * 140, py + t[1] * back * 140
        obox(x, y, t, 16, hw + 400, road_z(S[i]) - 200, road_z(S[i]) + 500, TEX + "boundary", solid=False)


LAND_GAP = 110      # verge between the gravel shoulder and the invisible wall on the land side


def corridor():
    """Invisible walls on every side of the road that has none yet (the land side),
    so nobody cuts across the slopes between the hairpin legs or wanders over the
    headland down to the bay. Where another leg is close the wall moves in to sit
    between the two; on the inside of a bend it stays short of the centre of it.
    In the old town the wall stands at the house fronts and closes the alleys."""
    w0, w1 = SECTIONS["town"]
    houses = set(range(w0 - 10, w1 + 31))
    done = 0
    for side in (-1, 1):
        placed = {}
        for i in range(len(PATH)):
            if i in COVERED[side]:
                continue
            hw = half_width(S[i])
            gap = 50 if i in houses else LAND_GAP
            while gap >= 10:
                lat = side * (hw + SHOULDER + gap)
                x, y, _ = road_point(i, lat)
                d, j = nearest(x, y, abs(lat) + 600)
                ok = abs(j - i) <= 10 and d >= abs(lat) - 30
                if ok:
                    for k in leg_nearest(x, y, 900):
                        if abs(k - i) > 10:
                            s, dsig = local(k, x, y)
                            if abs(dsig) - half_width(s) - SHOULDER < 30:
                                ok = False
                                break
                if ok:
                    placed[i] = hw + SHOULDER + gap
                    break
                gap -= 10
        PLACED[side] = placed
        for a, b in runs(placed):
            a0, b0 = max(0, a - 1), min(len(PATH) - 1, b + 1)       # overlap the neighbouring walls
            for k in range(a0, b0, 2):
                e = min(k + 2, b0)
                lat = side * min(placed.get(k, placed.get(k + 1, 0)), placed.get(e, placed.get(e - 1, 1e9)))
                strip(k, e, *sorted((lat, lat + side * 24)), -420, 420, TEX + "boundary", step=2, solid=False)
                done += 1
            # caps across the verge at both ends, so no gap opens where the wall
            # steps out from a guard rail or ends short of a bend
            for k, e in ((a0, a0 + 1), (b0 - 1, b0)):
                j = a if k == a0 else b
                hw = half_width(S[j])
                strip(k, e, *sorted((side * (hw + SHOULDER - 4), side * (placed[j] + 24))), -420, 420,
                      TEX + "boundary", step=1, solid=False)
        # at both ends of the course carry the wall on to the end wall behind it
        for i, back in ((0, -1), (len(PATH) - 1, 1)):
            lat = placed.get(i) or (half_width(S[i]) + SHOULDER)
            x, y, z = road_point(i, side * (lat + 12))
            t = TAN[i]
            obox(x + t[0] * back * 80, y + t[1] * back * 80, t, 88, 12, z - 420, z + 420, TEX + "boundary",
                 solid=False)
    BOUNDS["corridor_blocks"] = done


# ---------------------------------------------------------------- course objects

def trigger(classname, i, number=None, target=None):
    """Thin trigger across the road; it must not reach a neighbouring leg."""
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i])
    z = road_z(S[i])
    reach = hw + 200
    for j in nearby(px, py, 1400):
        if abs(j - i) * SAMPLE > 1500:
            dlat = (PATH[j][0] - px) * n[0] + (PATH[j][1] - py) * n[1]
            dalong = (PATH[j][0] - px) * t[0] + (PATH[j][1] - py) * t[1]
            if abs(dalong) < half_width(S[j]) + 80:
                reach = min(reach, abs(dlat) - half_width(S[j]) - 40)
    reach = max(reach, hw + 40)
    keys = {"classname": classname}
    if number is not None:
        keys["number"] = str(number)
    if target:
        keys["target"] = target
    tex = "common/checkpoint" if classname == "rally_checkpoint" else "common/startfinish"
    planes = [((t[0], t[1], 0), (px + t[0] * 24, py + t[1] * 24, z), tex),
              ((-t[0], -t[1], 0), (px - t[0] * 24, py - t[1] * 24, z), tex),
              ((n[0], n[1], 0), (px + n[0] * reach, py + n[1] * reach, z), tex),
              ((-n[0], -n[1], 0), (px - n[0] * reach, py - n[1] * reach, z), tex),
              ((0, 0, 1), (px, py, z + 400), tex),
              ((0, 0, -1), (px, py, z - 100), tex)]
    body = "\n".join(face(plane(nn, p), m, detail=False) for nn, p, m in planes)
    TRIGGERS.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()) + "\n{\n" + body + "\n}\n}")
    if target:
        x, y, zz = road_point(i)
        entity("info_notnull", x, y, zz + 40, targetname=target, angle=f"{heading_deg(i):.0f}")
    return reach


CHECKPOINTS = []
ROUTE_TOL = 64          # how far the bot route (Bezier between checkpoints) may stray from the centre line
ROUTE_SPAN = 110        # at most this many samples between two checkpoints
HANDLE_MAX = 900        # longest Bezier handle (it is shared by the segments either side)


def bez_point(i):
    x, y, z = road_point(i)
    return (x, y, z + 40)            # same as the info_notnull target the trigger moves to


def bez_handle(i, length):
    """Handle along the road at sample i (3D: it follows the gradient)."""
    j0, j1 = idx(i - 1), idx(i + 1)
    dz = (road_z(S[j1]) - road_z(S[j0])) / max(1.0, S[j1] - S[j0])
    return (TAN[i][0] * length, TAN[i][1] * length, dz * length)


def bez_error(a, b, ha, hb):
    """Largest gap between the cubic a..b (handles ha out of a, hb into b, as
    g_race_route.c builds it) and the centre line between the two samples."""
    import numpy as np
    p0, p3 = np.array(bez_point(a)[:2]), np.array(bez_point(b)[:2])
    c1, c2 = p0 + np.array(ha[:2]), p3 - np.array(hb[:2])
    n = max(24, (b - a) * 2)
    t = np.linspace(0, 1, n)[:, None]
    curve = ((1 - t) ** 3) * p0 + 3 * t * (1 - t) ** 2 * c1 + 3 * t * t * (1 - t) * c2 + t ** 3 * p3
    line = np.array(PATH[a:b + 1])
    d = np.sqrt(((curve[:, None, :] - line[None, :, :]) ** 2).sum(-1))
    return max(d.min(1).max(), d.min(0).max())


def plan_checkpoints(i0, i1):
    """Checkpoints where the bot route needs them: from the start line on, each
    one as far along as a single cubic (the BPD segment the bots drive) still
    follows the road within ROUTE_TOL. Returns [(sample, handle length)] and
    keeps the handles for write_bpd()."""
    def straight(a, b):
        pa, pb = bez_point(a), bez_point(b)
        return tuple((q - p) / 3 for p, q in zip(pa, pb))
    def candidates(prev, hprev):
        cands = []
        for j in range(prev + 6, min(i1 - 6, prev + ROUTE_SPAN) + 1, 2):
            chord = math.dist(bez_point(prev)[:2], bez_point(j)[:2])
            ho = straight(prev, j) if hprev is None else hprev
            for f in (0.15, 0.22, 0.3, 0.36, 0.42, 0.5, 0.6):
                length_j = min(chord * f, HANDLE_MAX)
                hj = bez_handle(j, length_j)
                if bez_error(prev, j, ho, hj) <= ROUTE_TOL:
                    cands.append((j, length_j, hj))
                    break
        return sorted(cands, key=lambda c: -c[0])

    def finishes(prev, hprev):
        h_out = straight(prev, i1) if hprev is None else hprev
        return i1 - prev <= ROUTE_SPAN and bez_error(prev, i1, h_out, straight(prev, i1)) <= ROUTE_TOL

    # depth first, farthest checkpoint first; back up when a handle leaves no way on
    stack = [(i0, None, candidates(i0, None))]
    chosen = []
    while True:
        prev, hprev, cands = stack[-1]
        if finishes(prev, hprev):
            break
        if not cands:
            stack.pop()
            if not stack:
                raise SystemExit("no checkpoint plan fits the road")
            chosen.pop()
            continue
        j, length_j, hj = cands.pop(0)
        chosen.append((j, length_j, hj))
        stack.append((j, hj, candidates(j, hj)))
    ROUTE.extend((j, hj) for j, _, hj in chosen)
    return [(j, length_j) for j, length_j, _ in chosen]


ROUTE = []      # (sample, handle) per checkpoint, for the .bpd


def write_bpd():
    """maps/<name>.bpd: checkpoint number : point : handle (see g_rally_tools.c)."""
    lines = []
    for k, (i, h) in enumerate(ROUTE, 1):
        p = bez_point(i)
        lines.append(f"{k} : {p[0]:.3f} {p[1]:.3f} {p[2]:.3f} : {h[0]:.3f} {h[1]:.3f} {h[2]:.3f}")
    target = ROOT / "baseq3r" / "maps" / f"{NAME}.bpd"
    target.write_text("\n".join(lines) + "\n", encoding="ascii")


def course():
    """Start, checkpoints (one per section, more on long ones), finish, grid."""
    L = length()
    i0, i1 = at(START_AT), at(L - FINISH_AT)
    BOUNDS.update(i_start=i0, i_finish=i1)
    trigger("rally_start", i0, target="start")
    for k, (i, h) in enumerate(plan_checkpoints(i0, i1), 1):
        reach = trigger("rally_checkpoint", i, number=k, target=f"cp{k}")
        CHECKPOINTS.append({"number": k, "sample": i, "fraction": round(i / len(PATH), 3),
                            "half_reach": round(reach), "handle": round(h)})
    trigger("rally_finish", i1, target="finish")
    # start grid: 4 rows of 2, staggered, behind the line
    for row in range(4):
        for col, lat in enumerate((-110, 110)):
            j = idx(i0 - int((170 + row * 180 + col * 90) / SAMPLE))
            x, y, z = road_point(j, lat)
            entity("info_player_start", x, y, z + 48, angle=f"{heading_deg(j):.0f}")
    j = idx(i0 - int(880 / SAMPLE))
    x, y, z = road_point(j)
    entity("info_player_deathmatch", x, y, z + 48, angle=f"{heading_deg(j):.0f}")


BASE_AXES = (((0, 0, 1), (1, 0, 0), (0, -1, 0)), ((0, 0, -1), (1, 0, 0), (0, -1, 0)),
             ((1, 0, 0), (0, 1, 0), (0, 0, -1)), ((-1, 0, 0), (0, 1, 0), (0, 0, -1)),
             ((0, 1, 0), (1, 0, 0), (0, 0, -1)), ((0, -1, 0), (1, 0, 0), (0, 0, -1)))


def fitted(normal, left, top_z, width, height, img_w, img_h):
    """Shift/scale of the Q3 axial texture projection that fits an image
    unmirrored onto an upright face of any yaw."""
    best = max(BASE_AXES, key=lambda b: sum(x * y for x, y in zip(normal, b[0])))
    sv = best[1]
    right = (-normal[1], normal[0], 0.0)
    ss = width * sum(x * y for x, y in zip(right, sv)) / img_w
    dl = sum(x * y for x, y in zip(left, sv))
    ts = height / img_h
    return (-dl / ss) % img_w, (top_z / ts) % img_h, ss, ts


def gantry(i, banner):
    """Portal across the road: posts and a beam with the banner fitted on both
    faces (one copy across the road)."""
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i])
    z = road_z(S[i])
    for side in (-1, 1):
        cx, cy = px + n[0] * side * (hw + 60), py + n[1] * side * (hw + 60)
        obox(cx, cy, t, 18, 18, ground(cx, cy) - 40, z + 420, TEX + "metal")
    half, z0, z1 = hw + 78, z + 330, z + 420
    faces = []
    for f in (t, (-t[0], -t[1])):
        right = (-f[1], f[0])
        c = (px + f[0] * 12, py + f[1] * 12)
        left = (c[0] - right[0] * half, c[1] - right[1] * half, 0.0)
        xo, yo, ss, ts = fitted((f[0], f[1], 0.0), left, z1, 2 * half, z1 - z0, 512, 64)
        pts = " ".join("( %s )" % " ".join(num(v) for v in q) for q in plane((f[0], f[1], 0), (c[0], c[1], z0)))
        faces.append(f"{pts} {TEX + banner} {num(xo)} {num(yo)} 0 {num(ss)} {num(ts)} {DETAIL} 0 0")
    for nn, p in (((n[0], n[1], 0), (px + n[0] * half, py + n[1] * half, z0)),
                  ((-n[0], -n[1], 0), (px - n[0] * half, py - n[1] * half, z0)),
                  ((0, 0, 1), (px, py, z1)), ((0, 0, -1), (px, py, z0))):
        faces.append(face(plane(nn, p), TEX + "metal"))
    brush(faces)


def lighthouse():
    """White tower with a red band and the lantern on the cape tip behind the start."""
    x, y = -15350, -2250
    z = ground(x, y)
    for k, (r, z0, z1, mat) in enumerate(((150, z - 40, z + 80, "stonewall"), (110, z + 80, z + 520, "white"),
                                         (112, z + 520, z + 600, "redband"), (100, z + 600, z + 820, "white"),
                                         (130, z + 820, z + 846, "metal"))):
        octagon(x, y, r, z0, z1, TEX + mat)
    octagon(x, y, 78, z + 846, z + 950, TEX + "lantern", solid=False)
    octagon(x, y, 90, z + 950, z + 980, TEX + "metal")
    octagon(x, y, 40, z + 980, z + 1040, TEX + "metal")
    entity("light", x, y, z + 900, light="2500", _color="1 0.92 0.7")
    # keeper's house
    obox(x + 300, y + 260, (1, 0), 160, 120, z - 40, z + 200, TEX + "house", top=TEX + "roof")
    BOUNDS["lighthouse"] = (x, y, z)


def octagon(cx, cy, r, z0, z1, mat, solid=True):
    planes = [((0, 0, 1), (cx, cy, z1), mat), ((0, 0, -1), (cx, cy, z0), mat)]
    for k in range(8):
        a = math.radians(22.5 + 45 * k)
        planes.append(((math.cos(a), math.sin(a), 0), (cx + r * math.cos(a), cy + r * math.sin(a), z0), mat))
    convex(planes, solid=solid)


def tunnel():
    """Tunnel through the headland: walls, vault, rock mass over it, portals, lights."""
    t0, t1 = SECTIONS["tunnel"]
    hw = max(half_width(S[k]) for k in range(t0, t1 + 1))
    w = hw + SHOULDER
    for side in (-1, 1):
        strip(t0, t1, *sorted((side * (w + 6), side * (w + 46))), -30, 260, TEX + "tunnelwall", step=4)
        # rounded shoulders of the vault
        strip(t0, t1, *sorted((side * (w - 60), side * (w + 46))), 260, 330, TEX + "tunnelwall", step=4)
    strip(t0, t1, -w - 46, w + 46, 330, 380, TEX + "tunnelwall", step=4)
    strip(t0, t1, -w - 520, w + 520, 380, 520, TEX + "rockmass", step=4, scale=1.0)
    strip(t0 + 8, t1 - 8, -w - 300, w + 300, 520, 640, TEX + "rockmass", step=4, scale=1.0)
    for i, d in ((t0, -1), (t1, 1)):
        a, b = (i - 3, i) if d < 0 else (i, i + 3)
        strip(a, b, -w - 520, -w - 6, -30, 520, TEX + "portal", step=3)
        strip(a, b, w + 6, w + 520, -30, 520, TEX + "portal", step=3)
        strip(a, b, -w - 6, w + 6, 330, 520, TEX + "portal", step=3)
    for side in (-1, 1):
        COVERED[side].update(range(t0 - 3, t1 + 4))
    for k in range(t0 + 6, t1 - 2, 10):
        x, y, z = road_point(k)
        entity("light", x, y, z + 280, light="300", _color="1 0.82 0.55")
        strip(k, k + 1, -40, 40, 322, 330, TEX + "tunnellamp", step=1, solid=False)


def bridge():
    """Bridge across the bay: deck, stone parapets, arches on piers down to the sea."""
    b0, b1 = SECTIONS["bridge"]
    a0, a1 = b0 - 2, b1 + 2
    hw = max(half_width(S[k]) for k in range(a0, a1 + 1))
    w = hw + SHOULDER
    # stone faces only at the two outer ends; the joints between the pieces stay
    # caulk (drawn joints catch light seams across the deck)
    deck = {"default": TEX + "stonewall", 4: TEX + "deck", 0: "common/caulk", 1: "common/caulk"}
    # The first and last piece lie on the road surface, which shows through them
    # (no drawn top), so the road runs onto the deck without a step.
    # The drawn top ends at the parapets (a top running on under them got light
    # seams where its luxels fell inside the stone); under them a plain base.
    strip(a0, a0 + 1, -w, w, -60, 0, {**deck, 4: "common/caulk"}, step=1)
    strip(a0 + 1, a1 - 1, -w, w, -60, 0, deck, step=2)
    strip(a1 - 1, a1, -w, w, -60, 0, {**deck, 4: "common/caulk"}, step=1)
    for side in (-1, 1):
        strip(a0, a1, *sorted((side * w, side * (w + 30))), -60, -20,
              {"default": TEX + "stonewall", 0: "common/caulk", 1: "common/caulk", 4: "common/caulk"}, step=2)
    for side in (-1, 1):
        strip(a0, a1, *sorted((side * w, side * (w + 30))), -20, 60, TEX + "parapet", step=2)
        strip(a0, a1, *sorted((side * (w + 4), side * (w + 30))), 60, 420, TEX + "boundary", step=4, solid=False)
        COVERED[side].update(range(a0, a1 + 1))
    # piers every ~512 units with arches between (spandrel blocks)
    span = 16
    piers = list(range(a0 + 6, a1 - 4, span))
    for p in piers:
        x, y, z = road_point(p)
        g = min(SEA_Z - 120, ground(x, y) - 20)
        obox(x, y, TAN[p], 36, w + 24, g, z - 60, TEX + "stonewall")
    for p, q in zip(piers, piers[1:]):
        for k in range(p, q):
            u = (k - p) / (q - p)
            u2 = (k + 1 - p) / (q - p)
            r0 = 1 - math.sin(math.pi * u) ** 0.5
            r1 = 1 - math.sin(math.pi * u2) ** 0.5
            depth = 60 + 260 * min(r0, r1) + 10
            strip(k, k + 1, -w - 20, w + 20, -depth, -60, TEX + "stonewall", step=1)
    for p in piers[::2]:
        x, y, z = road_point(p, 0)
        entity("light", x, y, z + 220, light="200", _color="1 0.85 0.6")


FLOOR_H = 110       # storey height on the house fronts (one window per storey and bay)
BAY_W = 100         # width of one window bay


def facade_fit(pa, pb, toward, top_z, wall_h):
    """Texture fit for a house front between the bottom corners pa, pb: whole
    window bays across the front, whole storeys from the eaves down, so no
    window is cut at a corner or by the roof. toward: a point the face looks at."""
    dx, dy = pb[0] - pa[0], pb[1] - pa[1]
    length = math.hypot(dx, dy)
    nx, ny = -dy / length, dx / length
    mx, my = (pa[0] + pb[0]) / 2, (pa[1] + pb[1]) / 2
    if nx * (toward[0] - mx) + ny * (toward[1] - my) < 0:
        nx, ny = -nx, -ny
    right = (-ny, nx)
    left = min((pa, pb), key=lambda q: q[0] * right[0] + q[1] * right[1])
    bays = max(1, round(length / BAY_W))
    floors = max(2, round(wall_h / FLOOR_H))
    return fitted((nx, ny, 0.0), (left[0], left[1], 0.0), top_z, length / bays, wall_h / floors, 128, 128)


def town():
    """Old town: house fronts along both sides of the street, pitched roofs,
    a few lit windows and street lamps."""
    w0, w1 = SECTIONS["town"]
    rnd = random.Random(31)
    facades = ["housea", "houseb", "housec", "housed"]
    count = 0
    for side in (-1, 1):
        k = w0 - 10
        while k < w1 + 30:
            run = rnd.randint(7, 12)
            if rnd.random() < 0.18:              # alley
                k += 4
                continue
            a, b = idx(k), idx(min(k + run, w1 + 30))
            hw = max(half_width(S[j]) for j in range(a, b + 1))
            front = side * (hw + SHOULDER + 50)
            back = side * (hw + SHOULDER + 50 + rnd.randint(260, 420))
            h = rnd.choice((230, 270, 310, 350, 390))
            zb = min(road_z(S[j]) for j in range(a, b + 1)) - 60
            mat = TEX + rnd.choice(facades)
            corners = []
            for j in (a, b):
                (px, py), n = PATH[j], NOR[j]
                for lat, z in ((front, zb), (back, zb), (back, zb + 60 + h), (front, zb + 60 + h)):
                    corners.append((px + n[0] * lat, py + n[1] * lat, z))
            top = zb + 60 + h
            road_mid = PATH[(a + b) // 2]
            fit = {}
            for fk, (c0, c1) in ((5, (corners[0], corners[4])), (3, (corners[1], corners[5]))):
                look = road_mid if fk == 5 else (2 * c0[0] - road_mid[0], 2 * c0[1] - road_mid[1])
                fit[fk] = facade_fit(c0, c1, look, top, h)
            hexa(corners, {"default": mat, 0: TEX + "houseside", 1: TEX + "houseside"}, scale=0.75, texfit=fit)
            # pitched roof along the street
            mid = (front + back) / 2
            rc = []
            for j in (a, b):
                (px, py), n = PATH[j], NOR[j]
                for lat, z in ((front - side * 14, zb + 60 + h), (back + side * 14, zb + 60 + h),
                               (mid, zb + 60 + h + 110), (mid, zb + 60 + h + 110)):
                    rc.append((px + n[0] * lat, py + n[1] * lat, z))
            rc[3] = (rc[3][0] + 0.01, rc[3][1], rc[3][2] - 0.01)
            rc[7] = (rc[7][0] + 0.01, rc[7][1], rc[7][2] - 0.01)
            prism_roof(rc, TEX + "rooftiles")
            count += 1
            k += run + rnd.randint(0, 2)
    # sidewalk kerbs
    for side in (-1, 1):
        for a, b in runs(range(w0 - 10, w1 + 30)):
            # per sample: the kerb follows the street where it widens towards the quay
            strip(a, b, lambda j, side=side: side * (half_width(S[j]) + 6),
                  lambda j, side=side: side * (half_width(S[j]) + SHOULDER + 50), 0, 10,
                  {"default": TEX + "kerbstone", 4: TEX + "pavement"}, step=2)
    # street lamps on the house fronts every few houses, lights under them
    for k in range(w0, w1 + 30, 22):
        side = 1 if (k // 22) % 2 else -1
        hw = half_width(S[k])
        x, y, z = road_point(k, side * (hw + SHOULDER + 44))
        ang = math.degrees(math.atan2(-NOR[k][1] * side, -NOR[k][0] * side))
        entity("misc_model", x, y, z + 260, model="models/mapobjects/q3r_lanterns/lantern_wall.md3",
               angle=f"{ang:.0f}")
        lx, ly, lz = road_point(k, side * (hw + SHOULDER + 22))
        entity("light", lx, ly, lz + 228, light="220", _color="1 0.72 0.4")
    BOUNDS["houses"] = count


def prism_roof(c, mat):
    """Roof block from eight corners where 2/3 and 6/7 meet at the ridge."""
    a, b, r, _, a2, b2, r2, _ = c
    cen = [sum(p[i] for p in (a, b, r, a2, b2, r2)) / 6 for i in range(3)]
    planes = []
    for p0, p1, p2 in ((a, r, a2), (b, r, b2), (a, b, a2), (a, b, r), (a2, b2, r2)):
        n = cross(tuple(q - p for p, q in zip(p0, p1)), tuple(q - p for p, q in zip(p0, p2)))
        if sum(nc * (pc - cc) for nc, pc, cc in zip(n, p0, cen)) < 0:
            n = tuple(-v for v in n)
        planes.append((n, p0, mat))
    convex(planes, solid=False)


def harbour():
    """Quay wall along the water, bollards, moored boats, the harbour light."""
    q0, q1 = SECTIONS["quay"]
    for k in range(q0 - 24, q1, 4):
        side = SEASIDE.get(k, -1)
        hw = half_width(S[k])
        lat = side * (hw + SHOULDER)
        def edge(off, side=side):
            return lambda j: side * (half_width(S[j]) + SHOULDER + off)
        strip(k, min(k + 4, q1), edge(0), edge(50), -260, 8, TEX + "quaywall", step=4)
        strip(k, min(k + 4, q1), edge(4), edge(30), 8, 420, TEX + "boundary", step=4, solid=False)
        COVERED[side].update(range(k, min(k + 4, q1) + 1))
        if (k - q0) % 24 == 0:
            x, y, z = road_point(k, lat + side * 22)
            obox(x, y, TAN[k], 12, 12, z, z + 34, TEX + "bollard")
    rnd = random.Random(4)
    for k in range(q0 - 10, q1 - 10, 26):
        side = SEASIDE.get(k, -1)
        hw = half_width(S[k])
        x, y, _ = road_point(k, side * (hw + SHOULDER + 260 + rnd.randint(0, 120)))
        boat(x, y, TAN[k], rnd.randint(0, 3))
    # the harbour light at the end of a mole beyond the finish
    x, y = PATH[-1][0] + 900, PATH[-1][1] - 500
    box((x - 1100, y - 80, -260), (x + 120, y + 80, 24), TEX + "quaywall", faces={"top": TEX + "pavement"})
    octagon(x, y, 70, 24, 330, TEX + "redband")
    octagon(x, y, 50, 330, 400, TEX + "lantern", solid=False)
    entity("light", x, y, 370, light="600", _color="1 0.3 0.25")


def boat(x, y, t, kind):
    """Small fishing boat: hull (two hexas), cabin, mast."""
    L, W = (180, 60) if kind < 2 else (260, 80)
    n = (-t[1], t[0])
    z0, z1 = SEA_Z - 30, SEA_Z + 40
    def p(a, b, z):
        return (x + t[0] * a + n[0] * b, y + t[1] * a + n[1] * b, z)
    hull = {"default": TEX + ("hullblue" if kind % 2 else "hullred"), 4: TEX + "boatdeck"}
    hexa([p(-L, -W * 0.6, z0), p(-L, W * 0.6, z0), p(-L, W, z1), p(-L, -W, z1),
          p(L * 0.6, -W * 0.5, z0), p(L * 0.6, W * 0.5, z0), p(L * 0.6, W, z1), p(L * 0.6, -W, z1)], hull, solid=False)
    hexa([p(L * 0.6, -W * 0.5, z0), p(L * 0.6, W * 0.5, z0), p(L * 0.6, W, z1), p(L * 0.6, -W, z1),
          p(L, -2, z0 + 30), p(L, 2, z0 + 30), p(L, 2, z1 + 10), p(L, -2, z1 + 10)], hull, solid=False)
    obox(x - t[0] * L * 0.3, y - t[1] * L * 0.3, t, L * 0.25, W * 0.6, z1, z1 + 70, TEX + "white", solid=False)
    obox(x + t[0] * L * 0.15, y + t[1] * L * 0.15, t, 3, 3, z1, z1 + 260, TEX + "mastwood", solid=False)


def trees():
    """Olive trees on the hills, palms near the sea and in the town; trunks
    near the road are solid."""
    rng = random.Random(9)
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    placed = []
    olives = ["tree6", "tree6", "tree4"]           # dark green, small: olive trees
    # houses, quay, lighthouse...: no tree inside them
    tall = [(lo, hi) for lo, hi in SOLIDS if hi[2] - lo[2] > 100]
    for _ in range(40000):
        x = rng.uniform(x0 + 300, x1 - 300)
        y = rng.uniform(y0 + 300, y1 - 300)
        z = ground(x, y)
        if z < SEA_Z + 20:
            continue
        d, i = nearest(x, y, 1800)
        edge = d - half_width(S[i])
        if edge < 230:
            continue
        sl = max(abs(ground(x + 48, y) - ground(x - 48, y)), abs(ground(x, y + 48) - ground(x, y - 48))) / 96
        if sl > 0.5 or any(math.dist((x, y), p) < 190 for p in placed):
            continue
        if math.dist((x, y), BOUNDS["lighthouse"][:2]) < 700:
            continue
        if any(lo[0] - 90 < x < hi[0] + 90 and lo[1] - 90 < y < hi[1] + 90 and lo[2] - 60 < z < hi[2]
               for lo, hi in tall):
            continue
        near_sea = z < 140 or i in TOWN or i in QUAY
        cluster = math.cos(x / 700 + 1.3) * math.cos(y / 560) + 0.6 * math.cos(x / 260) * math.sin(y / 300)
        if not near_sea and cluster < 0.15 + rng.random() * 0.5:
            continue
        placed.append((x, y))
        if near_sea:
            # the palm models are only 15 units tall: scale them to 330-440
            entity("misc_model", x, y, z - 8, model=f"models/mapobjects/palmier/palmier{rng.choice((1, 2))}.md3",
                   modelscale=f"{22 + rng.random() * 7.5:.1f}", angle=str(rng.randrange(360)),
                   _remap="*;models/mapobjects/palmier/palmier1")
        else:
            entity("misc_model", x, y, z - 10, model=f"models/mapobjects/GR_trees/{rng.choice(olives)}.md3",
                   modelscale=f"{0.8 + rng.random() * 0.35:.2f}", angle=str(rng.randrange(360)))
        if edge < 1200:
            box((x - 14, y - 14, z - 12), (x + 14, y + 14, z + 80), TEX + "boundary", collision=False)
        if len(placed) >= 460:
            break
    BOUNDS["trees"] = len(placed)


def rocks():
    rng = random.Random(4)
    count = 0
    for k in range(0, len(PATH), 9):
        if k in TOWN or k in QUAY or k in BRIDGE or k in TUNNEL:
            continue
        hw = half_width(S[k])
        for side in (-1, 1):
            if rng.random() > 0.3 or SEASIDE.get(k) == side:
                continue
            off = hw + SHOULDER + 170 + rng.random() * 200
            x, y, _ = road_point(k, side * off)
            r = 34 + rng.random() * 46
            if any(math.dist((x, y), road_point(j)[:2]) < half_width(S[j]) + SHOULDER + r + 90
                   for j in nearby(x, y, 800)):
                continue
            boulder(x, y, r, r * (0.7 + rng.random() * 0.5), k * 1.37 + side, TEX + "boulder")
            count += 1
    BOUNDS["boulders"] = count


def sea():
    """Water over the whole map at sea level (not solid, no liquid physics)."""
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    box((x0, y0, SEA_Z - 40), (x1, y1, SEA_Z), "common/nodraw", collision=False, faces={"top": TEX + "sea"})


def sign_panel(x, y, t, z0, z1, image):
    """Curve sign board: the chevrons fitted once, unmirrored, on the face the
    drivers see (it looks back along the road); metal elsewhere."""
    half, thick = 56, 6
    f = (-t[0], -t[1])                       # towards the oncoming cars
    right = (-f[1], f[0])
    c = (x + f[0] * thick, y + f[1] * thick)
    left = (c[0] - right[0] * half, c[1] - right[1] * half, 0.0)
    fit = fitted((f[0], f[1], 0.0), left, z1, 2 * half, z1 - z0, 256, 128)
    n = (-t[1], t[0])
    convex([((f[0], f[1], 0), (c[0], c[1], z0), TEX + image, fit),
            ((t[0], t[1], 0), (x + t[0] * thick, y + t[1] * thick, z0), TEX + "metal"),
            ((n[0], n[1], 0), (x + n[0] * half, y + n[1] * half, z0), TEX + "metal"),
            ((-n[0], -n[1], 0), (x - n[0] * half, y - n[1] * half, z0), TEX + "metal"),
            ((0, 0, 1), (x, y, z1), TEX + "metal"), ((0, 0, -1), (x, y, z0), TEX + "metal")])


def signs():
    """Curve signs before the hairpins and the horseshoe (red/white chevrons)."""
    n = len(PATH)
    count = 0
    for i in range(20, n - 20):
        k = abs(KAPPA[i])
        if k < 1 / 520 or not (k >= abs(KAPPA[i - 1]) and k > abs(KAPPA[i + 1])):
            continue
        side = 1 if KAPPA[i] > 0 else -1           # inside of the bend
        for d in (-6, 0, 6):
            j = idx(i + d)
            if j in TOWN:
                continue
            hw = half_width(S[j])
            x, y, z = road_point(j, -side * (hw + SHOULDER + 30))
            obox(x, y, TAN[j], 4, 46, ground(x, y) - 20, z + 50, TEX + "metal", top=TEX + "metal",
                 side=TEX + "metal")
            sign_panel(x, y, TAN[j], z + 50, z + 110, "chevron_l" if KAPPA[i] > 0 else "chevron_r")
            count += 1
    BOUNDS["signs"] = count


def shell():
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    top = max(GRID.values()) + 2400
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
    xs = [p[0] for p in PATH]
    ys = [p[1] for p in PATH]
    brush([face(plane(nrm, pt), "common/lightgrid") for nrm, pt in
           (((0, 0, -1), (min(xs) - 800, min(ys) - 800, -100)), ((0, 0, 1), (max(xs) + 800, max(ys) + 800, 1500)),
            ((-1, 0, 0), (min(xs) - 800, 0, 0)), ((1, 0, 0), (max(xs) + 800, 0, 0)),
            ((0, -1, 0), (0, min(ys) - 800, 0)), ((0, 1, 0), (0, max(ys) + 800, 0)))])


def misc_entities():
    n = len(PATH)
    for f, lat, up in ((0.03, 700, 300), (0.13, -900, 400), (0.36, 800, 300), (0.47, -900, 260),
                       (0.70, 900, 300), (0.92, -700, 260)):
        i = idx(int(f * n))
        x, y, z = road_point(i, lat)
        ang = math.degrees(math.atan2(PATH[i][1] - y, PATH[i][0] - x))
        entity("info_observer_spot", x, y, max(z, ground(x, y)) + up, angles=f"15 {ang:.0f} 0", spawnflags="1")
    i = SECTIONS["bridge"][0]
    x, y, z = road_point(i, -1400)
    ang = math.degrees(math.atan2(PATH[i][1] - y, PATH[i][0] - x))
    entity("info_player_intermission", x, y, 700, angles=f"14 {ang:.0f} 0")


# ---------------------------------------------------------------- shaders, textures

LAYER_TEX = (("grass", "textures/q3r_dirt_grass/grass07.tga", 320),
             ("gravel", "textures/q3r_dirt_grass/stones01.tga", 160),
             ("asphalt", "textures/q3r_road/asph02.jpg", 256),
             ("rock", "textures/q3r_dirt_grass/rock03.tga", 288),
             ("cliff", "textures/q3r_dirt_grass/rock03.tga", 320),
             ("sand", "textures/dip_valley/sand.jpg", 256))
PARENTS = (("grass", "surfaceparm SURF_GRASS", 0), ("gravel", "surfaceparm SURF_GRAVEL", 1),
           ("asphalt", "surfaceparm SURF_ASPHALT", 2), ("rock", None, 3), ("sand", "surfaceparm SURF_SAND", 5))
TINT = {"asphalt": "\t\trgbGen const ( 0.60 0.60 0.62 )", "rock": "\t\trgbGen const ( 0.92 0.88 0.80 )",
        "cliff": "\t\trgbGen const ( 0.92 0.86 0.76 )"}


def tcgen(layer):
    name, _, tile = LAYER_TEX[layer]
    k = 1.0 / tile
    if name == "cliff":
        return f"\t\ttcGen vector ( {0.71 * k:.7f} {0.71 * k:.7f} 0 ) ( 0 0 {k:.7f} )"
    return f"\t\ttcGen vector ( {k:.7f} 0 0 ) ( 0 {k:.7f} 0 )"


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
    rg = [] if any("rgbGen" in s for s in tex_extra) else ["\t\trgbGen identity"]
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *tex_extra, *rg, "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\trgbGen identity", "\t}", "}", ""]


def tinted(name, image, rgb, extra=()):
    return lit(name, image, extra, (f"\t\trgbGen const ( {rgb[0]:.2f} {rgb[1]:.2f} {rgb[2]:.2f} )",))


def glow(name, rgb, light):
    return [f"textures/{TEX}{name}", "{", "\tqer_editorimage textures/q3r_signage/white.jpg", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", "\tsurfaceparm nonsolid", f"\tq3map_surfacelight {light}",
            f"\tq3map_lightRGB {rgb[0]} {rgb[1]} {rgb[2]}", "\t{", "\t\tmap textures/q3r_signage/white.jpg",
            f"\t\trgbGen const ( {rgb[0]} {rgb[1]} {rgb[2]} )", "\t}", "}", ""]


def write_shader():
    T = f"textures/{TEX}"
    out = ["// Riviera Run - A to B sprint along a Mediterranean coast at sunset.",
           "// Written by tools/maptools/make_riviera.py. Compile with -custinfoparms.", ""]
    out += [f"{T}sky", "{", "\tqer_editorimage env/ely_sunset/sunset_ft.tga", "\tsurfaceparm noimpact",
            "\tsurfaceparm nolightmap", "\tsurfaceparm sky", "\tq3map_globaltexture",
            "\tq3map_sun 1 0.66 0.42 170 350 12", "\tq3map_skylight 80 4",
            "\tq3gl2_sun 1 0.66 0.42 120 350 12 0.6", "\tskyparms env/ely_sunset/sunset - -",
            "\tq3r_haze 0.62 0.48 0.44 7500\t// Q3Rally: warm evening haze over the sea", "}", ""]
    out += [f"{T}sea", "{", "\tqer_editorimage textures/q3r_liquids/water.tga", "\tqer_trans 0.35",
            "\tsurfaceparm nonsolid", "\tsurfaceparm trans", "\tsurfaceparm nomarks", "\tsurfaceparm nolightmap",
            "\tq3map_globaltexture", "\tcull disable",
            "\tq3r_water textures/q3r_liquids/water_normal.tga 320 0.02 0.018", "\tq3r_waterColor 0.05 0.16 0.22 0.70",
            "\tq3r_waterFresnel 0.10 4", "\tq3r_waterSpecular 1.0",
            "\t{", "\t\tmap textures/q3r_liquids/water.tga", "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA",
            "\t\trgbGen const ( 0.14 0.30 0.38 )", "\t\talphaGen const 0.82", "\t\ttcMod scale 0.25 0.25",
            "\t\ttcMod scroll 0.006 0.009", "\t}", "\t{", "\t\tmap textures/q3r_liquids/water.tga",
            "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\trgbGen const ( 0.95 0.62 0.42 )",
            "\t\talphaGen const 0.14", "\t\ttcMod scale 0.33 0.33", "\t\ttcMod scroll -0.005 0.007", "\t}", "}", ""]
    out += ["// palm models (models/mapobjects/palmier) name a shader that does not exist; the map remaps them",
            "models/mapobjects/palmier/palmier1", "{", "\tqer_editorimage models/mapobjects/palmier/palmier1.tga",
            "\tsurfaceparm nomarks", "\tsurfaceparm alphashadow", "\tcull none", "\t{",
            "\t\tmap models/mapobjects/palmier/palmier1.tga", "\t\talphaFunc GE128", "\t\tdepthWrite",
            "\t\trgbGen vertex", "\t}", "}", ""]
    out += lit("metal", "textures/base_trim/tin.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("rail", "textures/q3r_metals/plate01.tga", ("\tsurfaceparm SURF_METAL",))
    out += tinted("railpost", "textures/base_trim/tin.jpg", (0.6, 0.6, 0.62), ("\tsurfaceparm SURF_METAL",))
    out += tinted("stonewall", "textures/q3r_dirt_grass/redrock03.tga", (0.95, 0.88, 0.78),
                  ("\tq3map_lightmapSampleSize 24",))
    # the parapets cast no shadow: the low sun leaked through the joints between
    # their blocks and painted light stripes across the deck
    out += tinted("parapet", "textures/q3r_dirt_grass/redrock03.tga", (1.0, 0.94, 0.84), ("\tsurfaceparm trans",))
    out += tinted("quaywall", "textures/q3r_stone/Stein2.tga", (0.95, 0.9, 0.82))
    out += lit("pavement", "textures/q3r_road/cobbles01.jpg")
    out += tinted("kerbstone", "textures/q3r_road/concrete2.jpg", (0.85, 0.83, 0.80))
    out += [f"{T}deck", "{", "\tqer_editorimage textures/q3r_road/asph02.jpg", "\tq3map_nonplanar",
            "\tq3map_lightmapMergable", "\tq3map_lightmapSampleOffset 4",
            "\tq3map_shadeAngle 30", "\tsurfaceparm SURF_ASPHALT", "\t{", "\t\tmap textures/q3r_road/asph02.jpg",
            "\t\ttcGen vector ( 0.0039063 0 0 ) ( 0 0.0039063 0 )", TINT["asphalt"], "\t}"] + tail() + ["}", ""]
    out += tinted("tunnelwall", "textures/q3r_building/concrete02.jpg", (0.85, 0.82, 0.78),
                  ("\tq3map_lightmapSampleSize 24",))
    out += tinted("portal", "textures/q3r_dirt_grass/redrock03.tga", (0.9, 0.84, 0.74))
    out += [f"{T}rockmass", "{", "\tqer_editorimage textures/q3r_dirt_grass/rock03.tga", "\tq3map_shadeAngle 70",
            "\tq3map_lightmapSampleSize 32", "\t{", "\t\tmap textures/q3r_dirt_grass/rock03.tga",
            "\t\ttcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )", TINT["rock"], "\t}"] + tail() + ["}", ""]
    out += glow("tunnellamp", (1.0, 0.85, 0.6), 400)
    out += lit("boulder", "textures/q3r_dirt_grass/rock03.tga", ("\tq3map_shadeAngle 70",))
    out += lit("white", "textures/q3r_building/stucco01.jpg")
    out += tinted("redband", "textures/q3r_building/stucco01.jpg", (0.85, 0.18, 0.14))
    out += [f"{T}lantern", "{", "\tqer_editorimage textures/q3r_signage/white.jpg", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tq3map_surfacelight 600", "\tq3map_lightRGB 1 0.9 0.7",
            "\t{", "\t\tmap textures/q3r_signage/white.jpg", "\t\trgbGen wave sin 0.75 0.25 0 0.2", "\t}", "}", ""]
    # house fronts: one window per bay (stuccowin02 has only small transoms and looked
    # like a cellar, so "houseb" repeats 01)
    for k, img in enumerate(("stuccowin01", "stuccowin01", "stuccowin03", "stuccowin04")):
        out += lit("house" + "abcd"[k], f"textures/q3r_building/{img}.jpg")
    # the gable ends face the low sun head on: a little darker, or they burn out white
    out += tinted("houseside", "textures/q3r_building/stucco02.jpg", (0.74, 0.68, 0.60))
    out += lit("house", "textures/q3r_building/stuccowin02.jpg")
    out += lit("roof", "textures/q3r_building/roof3.jpg")
    out += lit("rooftiles", "textures/q3r_building/roof3.jpg", ("\tsurfaceparm nonsolid",))
    out += tinted("bollard", "textures/q3r_metals/plate01.tga", (0.35, 0.35, 0.38), ("\tsurfaceparm SURF_METAL",))
    out += tinted("hullblue", "textures/q3r_building/stucco01.jpg", (0.25, 0.45, 0.75), ("\tsurfaceparm nonsolid",))
    out += tinted("hullred", "textures/q3r_building/stucco01.jpg", (0.80, 0.25, 0.18), ("\tsurfaceparm nonsolid",))
    out += lit("boatdeck", "textures/q3r_wood/Holz.jpg", ("\tsurfaceparm nonsolid",))
    out += lit("mastwood", "textures/q3r_wood/Holz.jpg", ("\tsurfaceparm nonsolid",))
    out += lit("chevron_r", f"{T}chevron_r.jpg")
    out += lit("chevron_l", f"{T}chevron_l.jpg")
    out += lit("banner_start", f"{T}banner_start.jpg")
    out += lit("banner_finish", f"{T}banner_finish.jpg")
    out += [f"{T}boundary", "{", "\tqer_editorimage textures/base_floor/concrete.jpg", "\tqer_trans 0.15",
            "\t// invisible but SOLID; trans lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm noimpact", "\tsurfaceparm detail", "}", ""]
    out += ["// ground: t_* parents on the brush sides (collision, friction); q3map2 draws",
            f"// ter_<layer> / ter_<a>to<b> from maps/{NAME}_index.pcx. Layers: " +
            ", ".join(f"{i} {n}" for i, (n, _, _) in enumerate(LAYER_TEX)), ""]
    for name, parm, layer in PARENTS:
        out += [f"{T}t_{name}", "{", f"\tqer_editorimage {LAYER_TEX[layer][1]}", "\tq3map_indexed",
                "\tq3map_shadeAngle 120", "\tq3map_nonplanar"] + ([f"\t{parm}"] if parm else []) + ["}", ""]
    for i in range(len(LAYER_TEX)):
        out += [f"{T}ter_{i}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_lightmapSampleSize 24",
                "\tq3map_nonplanar"] + stage(i, False) + tail() + ["}", ""]
    for i, j in combinations(range(len(LAYER_TEX)), 2):
        out += [f"{T}ter_{i}to{j}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_lightmapSampleSize 24",
                "\tq3map_nonplanar"] + stage(i, False) + stage(j, True) + tail() + ["}", ""]
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.shader").write_text("\n".join(out), encoding="utf-8")


def write_textures():
    from PIL import Image, ImageDraw, ImageFilter
    out = ROOT / "baseq3r" / "textures" / "q3r_riviera"
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
        acc = [a + weight * v for a, v in zip(acc, list(big.get_flattened_data()))]
    lo, hi = min(acc), max(acc)
    m = Image.new("L", (size, size))
    m.putdata([int(128 + ((v - lo) / (hi - lo) - 0.5) * 84) for v in acc])
    m.filter(ImageFilter.GaussianBlur(1.2)).convert("RGB").save(out / "macro.jpg", quality=92)
    # curve sign: red/white chevrons
    im = Image.new("RGB", (256, 128), (235, 235, 230))
    d = ImageDraw.Draw(im)
    for k in range(3):
        x = 30 + k * 75
        d.polygon([(x, 10), (x + 40, 64), (x, 118), (x + 30, 118), (x + 70, 64), (x + 30, 10)], fill=(200, 30, 25))
    im.save(out / "chevron_r.jpg", quality=92)                       # arrows to the right
    im.transpose(Image.FLIP_LEFT_RIGHT).save(out / "chevron_l.jpg", quality=92)
    # start / finish banners: chequer band with a word (5x7 dot font)
    for name, word in (("banner_start", "START"), ("banner_finish", "FINISH")):
        im = Image.new("RGB", (512, 64), (250, 250, 245))
        d = ImageDraw.Draw(im)
        for k in range(0, 512, 16):
            for r in (0, 1):
                if (k // 16 + r) % 2 == 0:
                    d.rectangle([k, r * 8, k + 15, r * 8 + 7], fill=(20, 20, 20))
                    d.rectangle([k, 48 + r * 8, k + 15, 48 + r * 8 + 7], fill=(20, 20, 20))
        cell = 4
        wx = len(word) * 6 * cell - cell
        x0 = (512 - wx) // 2
        for i, ch in enumerate(word):
            for r, row in enumerate(FONT[ch]):
                for c, bit in enumerate(row):
                    if bit == "1":
                        x = x0 + (i * 6 + c) * cell
                        y = 18 + r * cell
                        d.rectangle([x, y, x + cell - 1, y + cell - 1], fill=(200, 30, 25))
        im.save(out / f"{name}.jpg", quality=92)


FONT = {
    "S": ["01111", "10000", "10000", "01110", "00001", "00001", "11110"],
    "T": ["11111", "00100", "00100", "00100", "00100", "00100", "00100"],
    "A": ["01110", "10001", "10001", "11111", "10001", "10001", "10001"],
    "R": ["11110", "10001", "10001", "11110", "10100", "10010", "10001"],
    "F": ["11111", "10000", "10000", "11110", "10000", "10000", "10000"],
    "I": ["11111", "00100", "00100", "00100", "00100", "00100", "11111"],
    "N": ["10001", "11001", "10101", "10011", "10001", "10001", "10001"],
    "H": ["10001", "10001", "10001", "11111", "10001", "10001", "10001"],
}


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
    lim = [min(vmax, math.sqrt(alat / max(1e-6, abs(KAPPA[i])))) for i in range(n)]
    v = lim[:]
    v[0] = 0.0
    for i in range(1, n):
        g = (ZTAB[i] - ZTAB[i - 1]) / SAMPLE
        v[i] = min(lim[i], math.sqrt(max(0.0, v[i - 1] ** 2 + 2 * (acc - 800 * g) * SAMPLE)))
    for i in range(n - 2, -1, -1):
        v[i] = min(v[i], math.sqrt(v[i + 1] ** 2 + 2 * brake * SAMPLE))
    return v


def validate():
    n = len(PATH)
    grades = [abs(ZTAB[i + 1] - ZTAB[i]) / SAMPLE for i in range(n - 1)]
    assert max(grades) < 0.2, max(grades)
    worst = max(abs(ground(*PATH[i]) - road_z(S[i])) for i in range(0, n, 3) if i not in BRIDGE)
    # legs must not merge: gap between the road edges of far-apart parts
    close = 1e9
    for i in range(0, n, 3):
        for j in nearby(*PATH[i], 1600):
            if abs(j - i) * SAMPLE > 3000:
                close = min(close, math.dist(PATH[i], PATH[j]) - half_width(S[i]) - half_width(S[j]))
    assert close > 450, ("legs too close", close)
    starts = [e for e in ENTITIES if e["classname"] in ("info_player_start", "info_player_deathmatch")]
    for e in starts:
        x, y, z = map(float, e["origin"].split())
        for slo, shi in SOLIDS:
            assert not (x + 60 > slo[0] and x - 60 < shi[0] and y + 60 > slo[1] and y - 60 < shi[1]
                        and z + 30 > slo[2] and z - 30 < shi[2]), (e, slo, shi)
        assert z - 26 > max(ground(x + dx, y + dy) for dx in (-50, 50) for dy in (-50, 50)), e
    # bridge clearance over the water
    b0, b1 = SECTIONS["bridge"]
    v = speed_profile()
    t = sum(SAMPLE / max(30.0, (v[i] + v[i + 1]) / 2) for i in range(n - 1))
    return {"map": NAME, "course_length": round(length()), "estimated_time_s": round(t, 1),
            "start_z": round(ZTAB[0]), "finish_z": round(ZTAB[-1]), "max_grade_pct": round(100 * max(grades), 1),
            "min_turn_radius": round(1 / max(abs(k) for k in KAPPA)),
            "road_mesh_error": round(worst, 2), "closest_legs_gap": round(close),
            "bridge_height_over_sea": round(min(ZTAB[b0:b1]) - SEA_Z),
            "sections": {k: [round(S[a]), round(S[b])] for k, (a, b) in SECTIONS.items()},
            "terrain_triangles": len(TRIS),
            "leaves": {s: sum(1 for leaf in LEAVES if leaf[2] == s) for s in (64, 128, 256, 512)},
            "brushes": len(BRUSHES) + sum(len(g) for g in GROUPS.values()), "checkpoints": CHECKPOINTS,
            "houses": BOUNDS.get("houses"), "trees": BOUNDS.get("trees"), "boulders": BOUNDS.get("boulders"),
            "signs": BOUNDS.get("signs")}


def overview(report):
    from PIL import Image, ImageDraw
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    scale = max(x1 - x0, y1 - y0) / 1600
    w, h = int((x1 - x0) / scale), int((y1 - y0) / scale)
    im = Image.new("RGB", (w, h))
    pix = im.load()
    colors = {L_GRASS: (150, 150, 90), L_GRAVEL: (170, 160, 140), L_ASPHALT: (60, 62, 68),
              L_ROCK: (200, 192, 176), L_CLIFF: (186, 172, 150), L_SAND: (220, 196, 150)}
    zmax = max(GRID.values())
    for v in range(h):
        y = y1 - v * scale
        gy = y0 + round((y - y0) / STEP) * STEP
        for u in range(w):
            x = x0 + u * scale
            gx = x0 + round((x - x0) / STEP) * STEP
            z = GRID.get((gx, gy), 0)
            if z < SEA_Z:
                depth = min(1.0, (SEA_Z - z) / 300)
                pix[u, v] = (int(60 - 30 * depth), int(130 - 50 * depth), int(160 - 40 * depth))
                continue
            zx = GRID.get((gx + STEP, gy), z)
            shade = 0.6 + 0.4 * z / zmax - (zx - z) / STEP * 0.2
            c = colors[MAT.get((gx, gy), L_GRASS)]
            pix[u, v] = tuple(max(0, min(255, int(k * shade))) for k in c)
    d = ImageDraw.Draw(im)
    for e in ENTITIES:
        x, y, _ = map(float, e["origin"].split())
        u, vv = (x - x0) / scale, (y1 - y) / scale
        if e["classname"] == "info_notnull":
            d.ellipse((u - 7, vv - 7, u + 7, vv + 7), outline=(255, 255, 255), width=3)
            d.text((u + 9, vv - 9), e["targetname"], fill=(255, 255, 255))
        elif e["classname"] == "misc_model" and "palmier" in e.get("model", ""):
            d.ellipse((u - 2, vv - 2, u + 2, vv + 2), fill=(40, 120, 60))
        elif e["classname"] == "misc_model" and "GR_trees" in e.get("model", ""):
            d.ellipse((u - 2, vv - 2, u + 2, vv + 2), fill=(70, 90, 40))
    out = ROOT / "docs" / "maps" / "riviera"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "riviera_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def write_route():
    """Intro camera flight along the course (same format as the recorded routes)."""
    n = len(PATH)
    v = speed_profile()
    lines = ["Q3RALLY_INTRO_ROUTE 1", f"map {NAME}", "track_length 1", "track_reversed 0"]
    frames, t = [], 0.0
    target = 20.0
    total = sum(SAMPLE / max(120.0, v[k]) for k in range(n))
    for k in range(0, n, 2):
        x, y, z = road_point(k)
        j = idx(k + 1)
        pitch = -math.degrees(math.atan2(ZTAB[j] - ZTAB[k], SAMPLE))
        frames.append(f"{round(t * 1000)} {x:.3f} {y:.3f} {z + 60:.3f} {pitch:.3f} {heading_deg(k):.3f} 0.000")
        t += sum(SAMPLE / max(120.0, v[idx(k + m)]) for m in range(2)) * target / total
    lines.append(f"frames {len(frames)}")
    out = ROOT / "baseq3r" / "intro_routes"
    out.mkdir(parents=True, exist_ok=True)
    (out / f"{NAME}_tl1_rev0.route").write_text("\n".join(lines + frames) + "\n", encoding="ascii")


def write_map():
    fields = {"classname": "worldspawn", "message": "Riviera Run", "_ambient": "20", "_minlight": "14",
              "gridsize": "128 128 256", "_blocksize": "1024 1024 4096"}
    text = ["// Generated by tools/maptools/make_riviera.py. Q3 brush format.",
            f"// The terrain func_groups use maps/{NAME}_index.pcx at compile time.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    for group in ("grass", "gravel", "asphalt", "rock", "sand"):
        if group not in GROUPS:
            continue
        keys = {"classname": "func_group", "_terrain": f"riviera ground, {group} friction",
                "_indexmap": f"maps/{NAME}_index.pcx", "_layers": str(len(LAYER_TEX)),
                "_shader": TEX + "ter", "min": f"{x0} {y0} {FLOOR}", "max": f"{x1} {y1} 4096"}
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()))
        text += GROUPS[group]
        text.append("}")
    text += TRIGGERS
    for ent in ENTITIES:
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in ent.items()) + "\n}")
    target = ROOT / "baseq3r" / "maps" / f"{NAME}.map"
    target.write_text("\n".join(text) + "\n", encoding="ascii")
    arena = ROOT / "baseq3r" / "scripts" / f"{NAME}.arena"
    arena.write_text(
        '{\n'
        '\tmap "q3r_riviera"\n'
        '\tlongname "Riviera Run"\n'
        '\tauthor "Q3Rally Team"\n'
        '\ttype "q3r_sprint"\n'
        '\tstarts "8"\n'
        '\tlaps "1"\n'
        '\tcheckpoints "14"\n'
        '\tobserverspots "6"\n'
        '\tweapons "0"\n'
        '\tpowerups "0"\n'
        '\tbotsupport "Ok"\n'
        '}\n'
        , encoding="ascii")
    return target


def main():
    build_path()
    build_profile()
    hash_path()
    sections()
    build_terrain()
    find_seaside()
    course()
    gantry(BOUNDS["i_start"], "banner_start")
    gantry(BOUNDS["i_finish"], "banner_finish")
    lighthouse()
    guard_rails()
    tunnel()
    bridge()
    town()
    harbour()
    corridor()
    signs()
    trees()
    rocks()
    sea()
    misc_entities()
    shell()
    report = validate()
    write_textures()
    write_shader()
    index_map()
    target = write_map()
    write_bpd()
    write_route()
    overview(report)
    print(json.dumps({k: v for k, v in report.items() if k != "checkpoints"}, indent=2))
    for c in report["checkpoints"]:
        print(c)
    print("Wrote", target)


if __name__ == "__main__":
    main()
