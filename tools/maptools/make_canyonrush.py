#!/usr/bin/env python3
"""Generate q3r_canyonrush: a point-to-point (A2B) sprint through a red desert canyon.

The course is one smoothed centre line from the start gate (A) to the finish
gate (B). A plateau height field is carved along it: a flat, slightly banked
track, steep sandstone walls up to the plateau rim. Where two legs of the
course run side by side, the ridge between them is what the two carvings
leave standing. Features along the way: descent from the start, a narrow
gorge with a natural rock arch, an open basin with hoodoos and a dry gravel
riverbed, crests that lift the cars, banked bends, six checkpoints and a
climb out of the canyon to the finish.

Ground layers are blended per vertex with a q3map2 index map (as in
q3r_crossroads); collision is closed triangular detail brushes on a
64/128/256-unit grid (fine only near the track). Tyre friction comes from the
parent shaders of the brush sides. Writes the .map, the index map, the
shader script, the generated textures and an overview image.
Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_canyonrush"
TEX = "q3r_canyonrush/"
DETAIL = 134217728
STEP = 64
FLOOR = -128
SAMPLE = 32                     # spacing of the centre line samples

# centre line control points, start (A) to finish (B)
CTRL = [(-7600, 3700), (-6200, 3750), (-4700, 3350), (-3500, 2300), (-3150, 700),
        (-3650, -900), (-2700, -2550), (-900, -3250), (900, -2900), (2100, -1700),
        (1900, -100), (450, 900), (450, 2500), (1800, 3700), (3700, 3500),
        (4600, 2100), (4300, 400), (5300, -900), (6700, -1700), (7300, -3300),
        (7400, -4700)]

# keyframes along the course (fraction of its length)
HALF_WIDTH = [(0.00, 430), (0.08, 420), (0.14, 360), (0.20, 300), (0.28, 300), (0.32, 400),
              (0.37, 780), (0.50, 780), (0.54, 440), (0.70, 460), (0.80, 400), (0.90, 360),
              (0.96, 460), (1.00, 480)]
FLOOR_Z = [(0.00, 660), (0.05, 650), (0.15, 380), (0.28, 300), (0.37, 220), (0.52, 200),
           (0.62, 260), (0.74, 240), (0.85, 320), (0.95, 590), (1.00, 610)]
CRESTS = [(0.345, 40, 260), (0.645, 44, 280), (0.775, 40, 260)]   # (fraction, height, width)
RIVER = (0.455, 22, 260)                                          # dry riverbed in the basin
ARCH_AT = 0.235
CHECKPOINTS = (0.15, 0.30, 0.45, 0.60, 0.74, 0.87)
START_AT, FINISH_AT = 650, 500                                   # units from either end

LAYERS = ("sand", "track", "gravel", "rock", "cliff")
L_SAND, L_TRACK, L_GRAVEL, L_ROCK, L_CLIFF = range(5)
FRICTION = {L_SAND: "sand", L_TRACK: "dirt", L_GRAVEL: "gravel", L_ROCK: "plain", L_CLIFF: "plain"}

PATH, S, TAN, NOR, KAPPA = [], [], [], [], []
GRID, MAT, LEAVES, LEAFOF, TRIS = {}, {}, [], {}, []
BRUSHES, GROUPS, ENTITIES, SOLIDS = [], {}, [], []
BOUNDS = {}


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
    pts = [CTRL[0]] + CTRL + [CTRL[-1]]
    dense = []
    for i in range(1, len(pts) - 2):
        p0, p1, p2, p3 = pts[i - 1], pts[i], pts[i + 1], pts[i + 2]
        n = max(4, int(math.dist(p1, p2) / 8))
        for k in range(n):
            t = k / n
            dense.append(tuple(0.5 * ((2 * p1[j]) + (-p0[j] + p2[j]) * t
                                      + (2 * p0[j] - 5 * p1[j] + 4 * p2[j] - p3[j]) * t * t
                                      + (-p0[j] + 3 * p1[j] - 3 * p2[j] + p3[j]) * t ** 3)
                               for j in range(2)))
    dense.append(CTRL[-1])
    # resample at SAMPLE spacing
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
    for i, p in enumerate(PATH):
        S.append(i * SAMPLE)
        q0, q1 = PATH[max(0, i - 1)], PATH[min(len(PATH) - 1, i + 1)]
        d = math.dist(q0, q1)
        t = ((q1[0] - q0[0]) / d, (q1[1] - q0[1]) / d)
        TAN.append(t)
        NOR.append((-t[1], t[0]))                     # left of the driving direction
    heading = [math.atan2(t[1], t[0]) for t in TAN]
    raw = []
    for i in range(len(PATH)):
        a, b = heading[max(0, i - 1)], heading[min(len(PATH) - 1, i + 1)]
        db = (b - a + math.pi) % (2 * math.pi) - math.pi
        raw.append(db / (2 * SAMPLE))
    w = 10                                            # +-320 units
    for i in range(len(PATH)):
        part = raw[max(0, i - w): i + w + 1]
        KAPPA.append(sum(part) / len(part))


def length():
    return S[-1]


def half_width(s):
    return keyed(HALF_WIDTH, s / length())


def floor_z(s):
    f = s / length()
    z = keyed(FLOOR_Z, f)
    for cf, ch, cw in CRESTS:
        z += ch * math.exp(-((s - cf * length()) / cw) ** 2)
    rf, rd, rw = RIVER
    z -= rd * math.exp(-((s - rf * length()) / rw) ** 2)
    return z


def bank(i):
    return max(-0.11, min(0.11, -150 * KAPPA[i]))


def plateau(x, y):
    return (1150 + 110 * math.cos(x / 1900) * math.cos(y / 1500)
            + 45 * math.cos(x / 610 + 1.1) * math.cos(y / 730 + 0.4))


def carve(i, x, y, rim_base):
    px, py = PATH[i]
    dx, dy = x - px, y - py
    # position along the course, between the samples (continuous floor)
    along = dx * TAN[i][0] + dy * TAN[i][1]
    s = S[i] + max(-SAMPLE / 2, min(SAMPLE / 2, along))
    dsig = dx * NOR[i][0] + dy * NOR[i][1]
    d = abs(dsig)
    # the canyon ends in a round head behind the start and after the finish
    if (i == 0 and along < 0) or (i == len(PATH) - 1 and along > 0):
        d = math.hypot(dsig, along)
        dsig = math.copysign(d, dsig)
    hw = half_width(s)
    h = floor_z(s)
    rim = max(rim_base, h + 320)
    run = 0.5 * (rim - h) + 90
    b = bank(i)
    if d <= hw:
        return h + b * dsig
    edge = h + b * math.copysign(hw, dsig)
    t = min(1.0, (d - hw) / run)
    # foot slope, steep wall, rounded rim; small ledges break the face up
    prof = t * t * (3 - 2 * t)
    prof += 0.035 * math.sin(t * math.pi * 3) * t * (1 - t) * 4
    return edge + (rim - edge) * min(1.0, prof)


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


def height(x, y):
    """Plateau minus every carving that reaches this point."""
    rim_base = plateau(x, y)
    z = rim_base
    for i in leg_nearest(x, y, 1600):
        z = min(z, carve(i, x, y, rim_base))
    return z


def leg_nearest(x, y, radius):
    """The closest centre line sample of every part of the course in reach."""
    cand = sorted(i for i in nearby(x, y, radius)
                  if abs(x - PATH[i][0]) <= radius and abs(y - PATH[i][1]) <= radius)
    out, best, bd, prev = [], None, 0.0, -10
    for i in cand:
        d = (x - PATH[i][0]) ** 2 + (y - PATH[i][1]) ** 2
        if i - prev > 2 and best is not None:
            out.append(best)
            best = None
        if best is None or d < bd:
            best, bd = i, d
        prev = i
    if best is not None:
        out.append(best)
    return out


def nearest(x, y):
    best, bi = 1e18, 0
    for i in nearby(x, y, 1800):
        d = (x - PATH[i][0]) ** 2 + (y - PATH[i][1]) ** 2
        if d < best:
            best, bi = d, i
    return math.sqrt(best), bi


def wall_top_distance(x, y):
    """How far beyond the top of the canyon wall (negative inside)."""
    d, i = nearest(x, y)
    if d > 1e8:
        return 1e9
    h = floor_z(S[i])
    rim = max(plateau(x, y), h + 320)
    return d - half_width(S[i]) - (0.5 * (rim - h) + 90)


# ---------------------------------------------------------------- terrain

def build_terrain():
    xs = [p[0] for p in PATH]
    ys = [p[1] for p in PATH]
    margin = 1800
    x0 = math.floor((min(xs) - margin) / 256) * 256
    x1 = math.ceil((max(xs) + margin) / 256) * 256
    y0 = math.floor((min(ys) - margin) / 256) * 256
    y1 = math.ceil((max(ys) + margin) / 256) * 256
    BOUNDS.update(x0=x0, x1=x1, y0=y0, y1=y1)
    zone, edge = {}, {}
    for y in range(y0, y1 + 1, STEP):
        for x in range(x0, x1 + 1, STEP):
            GRID[x, y] = height(x, y)
            zone[x, y] = wall_top_distance(x, y)
            d, i = nearest(x, y)
            edge[x, y] = d - half_width(S[i]) if d < 1e8 else 1e9
    raw = dict(GRID)
    # quadtree leaves: 256 far from the canyon, 128 near it, 64 at the track and walls
    for cy in range(y0, y1, 256):
        for cx in range(x0, x1, 256):
            pts = [(cx + i * STEP, cy + j * STEP) for i in range(5) for j in range(5)]
            if min(zone[p] for p in pts) > 900:
                LEAVES.append((cx, cy, 256))
                continue
            for qy in (cy, cy + 128):
                for qx in (cx, cx + 128):
                    sub = [(qx + i * STEP, qy + j * STEP) for i in range(3) for j in range(3)]
                    if min(edge[p] for p in sub) > 140:
                        LEAVES.append((qx, qy, 128))
                    else:
                        for fy in (qy, qy + 64):
                            for fx in (qx, qx + 64):
                                LEAVES.append((fx, fy, 64))
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
        d, i = nearest(x, y)
        hw = half_width(S[i]) if d < 1e8 else 0
        if slope > 0.85:
            m = L_CLIFF
        elif slope > 0.42:
            m = L_ROCK
        elif d < hw - 30:
            rf, _, rw = RIVER
            m = L_GRAVEL if abs(S[i] - rf * length()) < rw * 1.1 else L_TRACK
        else:
            patch = math.cos(x / 370) * math.cos(y / 430) + 0.5 * math.cos(x / 157) * math.cos(y / 181)
            m = L_ROCK if (patch > 1.0 and d > hw + 200) else L_SAND
        MAT[x, y] = m
    order = ("plain", "sand", "gravel", "dirt")
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
            major = max(set(classes), key=lambda k: (classes.count(k), order.index(k)))
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


def obox(cx, cy, t, along, across, z0, z1, mat, side=None, solid=True, detail=True):
    """Box rotated to the direction t (unit), centred on cx, cy."""
    n = (-t[1], t[0])
    side = side or mat
    convex([((t[0], t[1], 0), (cx + t[0] * along, cy + t[1] * along, z0), side),
            ((-t[0], -t[1], 0), (cx - t[0] * along, cy - t[1] * along, z0), side),
            ((n[0], n[1], 0), (cx + n[0] * across, cy + n[1] * across, z0), mat),
            ((-n[0], -n[1], 0), (cx - n[0] * across, cy - n[1] * across, z0), mat),
            ((0, 0, 1), (cx, cy, z1), side), ((0, 0, -1), (cx, cy, z0), side)],
           solid=solid, detail=detail)


def hexa(corners, mat, solid=True):
    """Convex 8-corner block: corners 0-3 one end (ring order), 4-7 the other."""
    c = [sum(p[i] for p in corners) / 8 for i in range(3)]
    quads = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
    planes = []
    for q in quads:
        p0, p1, p2 = corners[q[0]], corners[q[1]], corners[q[2]]
        n = cross(tuple(b - a for a, b in zip(p0, p1)), tuple(b - a for a, b in zip(p0, p2)))
        if sum(nc * (pc - cc) for nc, pc, cc in zip(n, p0, c)) < 0:
            n = tuple(-v for v in n)
        planes.append((n, p0, mat))
    convex(planes, solid=solid)


def boulder(x, y, radius, tall, seed, mat):
    z0 = min(ground(x + dx, y + dy) for dx in (-radius, 0, radius) for dy in (-radius, 0, radius))
    centre = (x, y, z0)
    planes = [((0, 0, -1), (x, y, z0 - 24), mat), ((0, 0, 1), (x, y, z0 + tall), mat)]
    rnd = abs(math.sin(seed * 12.9898) * 43758.5453) % 233280
    for k in range(16):
        rnd = (rnd * 9301 + 49297) % 233280
        jitter = rnd / 233280
        if k < 7:
            azimuth = (k + 0.4 * jitter) * 2 * math.pi / 7 + seed
            elevation = math.radians(4 + 10 * jitter)
            reach = radius * (0.85 + 0.25 * jitter)
        else:
            azimuth = (k - 7 + 0.5 * jitter) * 2 * math.pi / 9 + seed + 0.3
            elevation = math.radians(28 + 34 * ((k * 7 + int(seed * 3)) % 5) / 4)
            reach = radius * (0.7 + 0.25 * jitter)
        n = (math.cos(azimuth) * math.cos(elevation), math.sin(azimuth) * math.cos(elevation),
             math.sin(elevation))
        reach = reach * (1 - n[2]) + tall * 0.92 * n[2]
        planes.append((n, tuple(c + nc * reach for c, nc in zip(centre, n)), mat))
    convex(planes)


def hoodoo(x, y, r, tall, mat, cap):
    """Tapered sandstone pillar (octagonal frustum) with a wider cap rock."""
    z0 = min(ground(x + dx, y + dy) for dx in (-r, 0, r) for dy in (-r, 0, r)) - 24
    planes = [((0, 0, -1), (x, y, z0), mat), ((0, 0, 1), (x, y, z0 + tall), mat)]
    taper = 0.28 * r / tall
    for k in range(8):
        a = math.radians(22.5 + 45 * k)
        planes.append(((math.cos(a), math.sin(a), taper), (x + r * math.cos(a), y + r * math.sin(a), z0), mat))
    convex(planes)
    top = z0 + tall
    cr = r * 0.95
    planes = [((0, 0, -1), (x, y, top - 8), cap), ((0, 0, 1), (x, y, top + 46), cap)]
    for k in range(8):
        a = math.radians(45 * k + 10)
        planes.append(((math.cos(a), math.sin(a), 0.15), (x + cr * math.cos(a), y + cr * math.sin(a), top), cap))
    convex(planes)


def entity(classname, x, y, z, **keys):
    fields = {"classname": classname, "origin": f"{round(x)} {round(y)} {round(z)}", **keys}
    ENTITIES.append(fields)
    return fields


def at(fraction=None, s=None):
    s = fraction * length() if s is None else s
    i = max(0, min(len(PATH) - 1, int(round(s / SAMPLE))))
    return i


def heading_deg(i):
    return math.degrees(math.atan2(TAN[i][1], TAN[i][0])) % 360


# ---------------------------------------------------------------- course objects

def gantry(i, label_mat):
    """Start/finish portal across the track: two posts and a chequered beam."""
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i])
    z = floor_z(S[i])
    for side in (-1, 1):
        cx, cy = px + n[0] * side * (hw + 40), py + n[1] * side * (hw + 40)
        obox(cx, cy, t, 18, 18, ground(cx, cy) - 30, z + 420, TEX + "metal")
    obox(px, py, t, 14, hw + 70, z + 340, z + 420, label_mat, side=TEX + "metal")


def trigger(classname, i, number=None, target=None):
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i]) + 260
    z = floor_z(S[i])
    keys = {"classname": classname}
    if number is not None:
        keys["number"] = str(number)
    if target:
        keys["target"] = target
    # common/trigger is solid in this repo; the rally shaders are nonsolid
    tex = "common/checkpoint" if classname == "rally_checkpoint" else "common/startfinish"
    planes = [((t[0], t[1], 0), (px + t[0] * 24, py + t[1] * 24, z), tex),
              ((-t[0], -t[1], 0), (px - t[0] * 24, py - t[1] * 24, z), tex),
              ((n[0], n[1], 0), (px + n[0] * hw, py + n[1] * hw, z), tex),
              ((-n[0], -n[1], 0), (px - n[0] * hw, py - n[1] * hw, z), tex),
              ((0, 0, 1), (px, py, z + 700), tex),
              ((0, 0, -1), (px, py, z - 120), tex)]
    body = "\n".join(face(plane(nn, p), m, detail=False) for nn, p, m in planes)
    TRIGGERS.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()) + "\n{\n" + body + "\n}\n}")
    if target:
        entity("info_notnull", px, py, floor_z(S[i]) + 40, targetname=target, angle=f"{heading_deg(i):.0f}")


TRIGGERS = []


def arch(i):
    """Natural sandstone arch spanning the gorge."""
    (px, py), t, n = PATH[i], TAN[i], NOR[i]
    hw = half_width(S[i])
    z = floor_z(S[i])
    span = hw + 120
    rise = 420
    thick_in, thick_out = 0.0, 70.0
    segs = 9
    pts = []
    for k in range(segs + 1):
        a = math.pi * k / segs
        pts.append((math.cos(a), math.sin(a)))
    for k in range(segs):
        ring = []
        for (cu, su) in (pts[k], pts[k + 1]):
            for r_off in (thick_in, thick_out):
                lateral = (span + r_off) * cu
                up = z + 60 + (rise + r_off) * su
                ring.append((lateral, up))
        # ring: inner0, outer0, inner1, outer1  -> corners in order
        order = [ring[0], ring[1], ring[3], ring[2]]
        corners = []
        for along in (-90, 90):
            for lat, up in order:
                corners.append((px + n[0] * lat + t[0] * along, py + n[1] * lat + t[1] * along, up))
        hexa(corners, TEX + "rock")
    # the arch feet sink into the walls
    for side in (-1, 1):
        cx, cy = px + n[0] * side * (span + 40), py + n[1] * side * (span + 40)
        obox(cx, cy, t, 90, 70, z - 40, z + 110, TEX + "rock")


def dressing():
    L = length()
    i0, i1 = at(s=START_AT), at(s=L - FINISH_AT)
    # start grid (2 x 2 behind the line), deathmatch fallback
    for row, back in enumerate((170, 360)):
        for lat in (-120, 120):
            j = at(s=START_AT - back)
            (px, py), n = PATH[j], NOR[j]
            entity("info_player_start", px + n[0] * lat, py + n[1] * lat, floor_z(S[j]) + 48,
                   angle=f"{heading_deg(j):.0f}")
    j = at(s=START_AT - 540)
    entity("info_player_deathmatch", PATH[j][0], PATH[j][1], floor_z(S[j]) + 48, angle=f"{heading_deg(j):.0f}")
    trigger("rally_start", i0, target="start")
    for k, f in enumerate(CHECKPOINTS, 1):
        trigger("rally_checkpoint", at(f), number=k, target=f"cp{k}")
    trigger("rally_finish", i1, target="finish")
    gantry(i0, TEX + "checker")
    gantry(i1, TEX + "checker")
    arch(at(ARCH_AT))
    # hoodoos in the open basin, clear of the racing line
    rng = random.Random(5)
    for f, side, off, r, tall in ((0.385, 1, 520, 70, 520), (0.405, -1, 460, 60, 430), (0.425, 1, 430, 50, 360),
                                   (0.47, -1, 540, 80, 600), (0.49, 1, 500, 55, 400), (0.515, -1, 470, 65, 480)):
        i = at(f)
        (px, py), n = PATH[i], NOR[i]
        hoodoo(px + n[0] * side * off, py + n[1] * side * off, r, tall, TEX + "rock", TEX + "caprock")
    # boulders at the foot of the walls
    seed = 0
    for f in [k / 90 for k in range(3, 88)]:
        seed += 1
        i = at(f)
        if any(abs(f - c) < 0.012 for c in CHECKPOINTS + (ARCH_AT,)) or 0.37 < f < 0.53:
            continue
        side = 1 if seed % 2 else -1
        hw = half_width(S[i])
        r = 30 + rng.random() * 40
        (px, py), n = PATH[i], NOR[i]
        off = hw + r * 0.6 + rng.random() * 60
        boulder(px + n[0] * side * off, py + n[1] * side * off, r, r * (0.8 + rng.random() * 0.5),
                seed * 1.37, TEX + "boulder")
    entity("info_player_intermission", PATH[at(0.42)][0], PATH[at(0.42)][1], floor_z(at(0.42) * SAMPLE) + 900,
           angles=f"25 {heading_deg(at(0.42)):.0f} 0")


# ---------------------------------------------------------------- shell, shaders, textures

def shell():
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    top = max(GRID.values()) + 2600
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
    # light grid only over the canyon floor band (whole box, coarse grid)
    brush([face(plane(nrm, pt), "common/lightgrid") for nrm, pt in
           (((0, 0, -1), (x0, y0, 0)), ((0, 0, 1), (x1, y1, 1700)), ((-1, 0, 0), (x0, y0, 0)),
            ((1, 0, 0), (x1, y1, 0)), ((0, -1, 0), (x0, y0, 0)), ((0, 1, 0), (x1, y1, 0)))])


LAYER_TEX = (("sand", "textures/dip_valley/sand.jpg", 256),
             ("track", "textures/q3r_dirt_grass/dirt01.tga", 192),
             ("gravel", "textures/q3r_dirt_grass/stones02.tga", 160),
             ("rock", "textures/q3r_dirt_grass/rock01.tga", 224),
             ("cliff", "textures/q3r_canyonrush/strata.jpg", 768))
PARENTS = (("sand", "surfaceparm SURF_SAND", 0), ("dirt", "surfaceparm SURF_DIRT", 1),
           ("gravel", "surfaceparm SURF_GRAVEL", 2), ("plain", None, 3))


def tcgen(layer):
    name, _, tile = LAYER_TEX[layer]
    k = 1.0 / tile
    if name == "cliff":         # sandstone strata run horizontally on the walls
        return f"\t\ttcGen vector ( {0.82 * k:.7f} {0.57 * k:.7f} 0 ) ( 0 0 {k:.7f} )"
    return f"\t\ttcGen vector ( {k:.7f} 0 0 ) ( 0 {k:.7f} 0 )"


def stage(layer, blend):
    lines = ["\t{", f"\t\tmap {LAYER_TEX[layer][1]}"]
    if blend:
        lines += ["\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\talphaGen vertex"]
    return lines + [tcgen(layer), "\t\trgbGen identity", "\t}"]


def tail():
    return ["\t{", f"\t\tmap textures/q3r_canyonrush/macro.jpg", "\t\tblendFunc GL_DST_COLOR GL_SRC_COLOR",
            "\t\ttcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )", "\t\trgbGen identity", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}"]


def lit(name, image, extra=(), tex_extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *tex_extra, "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\trgbGen identity", "\t}", "}", ""]


def write_shader():
    out = ["// Canyon Rush - A2B sprint through a red desert canyon.",
           "// Written by tools/maptools/make_canyonrush.py. Compile with -custinfoparms.", ""]
    out += [f"textures/{TEX}sky", "{", "\tqer_editorimage env/ame_desert/desertsky_ft.tga",
            "\tsurfaceparm sky", "\tsurfaceparm noimpact", "\tsurfaceparm nolightmap", "\tq3map_globaltexture",
            "\tq3map_sun 1.0 0.88 0.72 240 230 36", "\tq3map_skylight 170 4",
            "\tq3gl2_sun 1.0 0.88 0.72 160 230 36 0.5", "\tskyparms env/ame_desert/desertsky - -",
            "\tq3r_haze 0.74 0.62 0.50 7000\t// Q3Rally: warm desert haze", "}", ""]
    out += lit("rock", "textures/q3r_canyonrush/strata.jpg", ("\tq3map_shadeAngle 50",))
    out += lit("caprock", "textures/q3r_dirt_grass/rock01.tga", ("\tq3map_shadeAngle 50",))
    out += lit("boulder", "textures/q3r_dirt_grass/rock01.tga", ("\tq3map_shadeAngle 70",))
    out += lit("metal", "textures/base_trim/tin.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("checker", "textures/q3r_dirt_grass/checker01.tga")
    out += [f"textures/{TEX}hills", "{", "\tqer_editorimage textures/dip_valley/sand.jpg",
            "\t// unreachable desert hills (q3r_mesaking backdrop), coarse lightmap",
            "\tq3map_lightmapSampleSize 64", "\tq3map_nonplanar", "\tq3map_shadeAngle 120",
            "\t{", "\t\tmap textures/dip_valley/sand.jpg", "\t\ttcGen vector ( 0.002 0 0 ) ( 0 0.002 0 )",
            "\t\trgbGen identity", "\t}"] + tail() + ["}", ""]
    out += [f"textures/{TEX}boundary", "{", "\tqer_editorimage textures/base_floor/concrete.jpg", "\tqer_trans 0.15",
            "\t// invisible but SOLID (Bullet only takes solid brushes); trans lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm noimpact", "\tsurfaceparm detail", "}", ""]
    out += ["// ground: t_* parents on the brush sides (collision, friction); q3map2 draws",
            "// ter_<layer> / ter_<a>to<b> from maps/q3r_canyonrush_index.pcx. Layers: " +
            ", ".join(f"{i} {n}" for i, (n, _, _) in enumerate(LAYER_TEX)), ""]
    for name, parm, layer in PARENTS:
        out += [f"textures/{TEX}t_{name}", "{", f"\tqer_editorimage {LAYER_TEX[layer][1]}", "\tq3map_indexed",
                "\tq3map_shadeAngle 70", "\tq3map_nonplanar"] + ([f"\t{parm}"] if parm else []) + ["}", ""]
    from itertools import combinations
    for i in range(len(LAYER_TEX)):
        out += [f"textures/{TEX}ter_{i}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 70",
                "\tq3map_nonplanar"] + stage(i, False) + tail() + ["}", ""]
    for i, j in combinations(range(len(LAYER_TEX)), 2):
        out += [f"textures/{TEX}ter_{i}to{j}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 70",
                "\tq3map_nonplanar"] + stage(i, False) + stage(j, True) + tail() + ["}", ""]
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.shader").write_text("\n".join(out), encoding="utf-8")


def write_textures():
    from PIL import Image, ImageFilter
    out = ROOT / "baseq3r" / "textures" / "q3r_canyonrush"
    out.mkdir(parents=True, exist_ok=True)
    rnd = random.Random(11)
    size = 512
    # sandstone strata: horizontal bands, gently wavy, fine grain; tiles both ways
    vivid = [(176, 88, 56), (198, 112, 70), (158, 74, 50), (214, 150, 104), (140, 62, 44), (190, 128, 86)]
    # muted: 40 % towards grey, the bands differ less than raw paint would
    palette = []
    for c in vivid:
        grey = 0.3 * c[0] + 0.59 * c[1] + 0.11 * c[2]
        palette.append(tuple(int(0.6 * v + 0.4 * grey) for v in c))
    bands = []
    pos = 0
    while pos < size:
        w = rnd.choice((6, 10, 14, 22, 30, 44))
        bands.append((pos, min(size, pos + w), rnd.randrange(len(palette))))
        pos += w
    band_of = [0] * size
    for a, b, c in bands:
        for y in range(a, b):
            band_of[y] = c
    img = Image.new("RGB", (size, size))
    px = img.load()
    for x in range(size):
        wave = (6 * math.sin(2 * math.pi * x / size * 2) + 3 * math.sin(2 * math.pi * x / size * 7 + 1.3))
        for y in range(size):
            yy = int(y + wave) % size
            c = palette[band_of[yy]]
            g = rnd.randint(-22, 22)
            shade = 1.0 - 0.08 * math.sin(2 * math.pi * (x / size * 3 + y / size * 5))
            px[x, y] = tuple(max(0, min(255, int(v * shade) + g)) for v in c)
    img = img.filter(ImageFilter.GaussianBlur(0.8))
    img.save(out / "strata.jpg", quality=90)
    # macro brightness noise (same recipe as q3r_crossroads)
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
    m.putdata([int(128 + ((v - lo) / (hi - lo) - 0.5) * 112) for v in acc])
    m.filter(ImageFilter.GaussianBlur(1.2)).convert("RGB").save(out / "macro.jpg", quality=92)


def index_map():
    from PIL import Image
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    w, h = (x1 - x0) // STEP + 1, (y1 - y0) // STEP + 1
    im = Image.new("L", (w, h))
    im.putdata([MAT[x0 + i * STEP, y1 - j * STEP] for j in range(h) for i in range(w)])
    im.save(ROOT / "baseq3r" / "maps" / f"{NAME}_index.pcx")


# ---------------------------------------------------------------- checks and output

def estimate_time():
    """Rough lap time: speed limited by curvature, acceleration and braking."""
    vmax, alat, acc, brake = 560.0, 900.0, 160.0, 450.0
    lim = [min(vmax, math.sqrt(alat / max(1e-6, abs(k)))) for k in KAPPA]
    v = [0.0] * len(lim)
    for i in range(1, len(lim)):
        v[i] = min(lim[i], math.sqrt(v[i - 1] ** 2 + 2 * acc * SAMPLE))
    for i in range(len(lim) - 2, -1, -1):
        v[i] = min(v[i], math.sqrt(v[i + 1] ** 2 + 2 * brake * SAMPLE))
    t = sum(SAMPLE / max(30.0, (a + b) / 2) for a, b in zip(v, v[1:]))
    return t, min(math.sqrt(1 / max(1e-6, abs(k))) ** 2 for k in KAPPA)


def validate():
    L = length()
    grades = [abs(floor_z(s + 32) - floor_z(s)) / 32 for s in range(0, int(L) - 32, 32)]
    assert max(grades) < 0.3, max(grades)
    # track floor in the mesh vs. design along the centre line
    worst = max(abs(ground(*PATH[i]) - floor_z(S[i])) for i in range(0, len(PATH), 4))
    # walls: the climb from the track edge to the rim must contain a steep part
    flat_walls = []
    for i in range(0, len(PATH), 12):
        for side in (-1, 1):
            (px, py), n = PATH[i], NOR[i]
            hw = half_width(S[i])
            prof = [ground(px + n[0] * side * (hw + k), py + n[1] * side * (hw + k)) for k in range(0, 900, 32)]
            steep = max((b - a) / 32 for a, b in zip(prof, prof[1:]))
            if prof[-1] - prof[0] > 150 and steep < 1.1:
                flat_walls.append((round(S[i]), side, round(steep, 2)))
    # legs must not merge: distance between far-apart parts of the course
    close = 1e9
    for i in range(0, len(PATH), 4):
        for j in nearby(*PATH[i], 1600):
            if abs(S[j] - S[i]) > 4000:
                close = min(close, math.dist(PATH[i], PATH[j]) - half_width(S[i]) - half_width(S[j]))
    assert close > 700, ("legs too close", close)
    starts = [e for e in ENTITIES if e["classname"] in ("info_player_start", "info_player_deathmatch")]
    for e in starts:
        x, y, z = map(float, e["origin"].split())
        for lo, hi in SOLIDS:
            assert not (x + 60 > lo[0] and x - 60 < hi[0] and y + 60 > lo[1] and y - 60 < hi[1]
                        and z + 30 > lo[2] and z - 30 < hi[2]), e
        assert z - 26 > max(ground(x + dx, y + dy) for dx in (-50, 50) for dy in (-50, 50)), e
    t, rmin = estimate_time()
    return {"map": NAME, "course_length": round(L), "estimated_time_s": round(t, 1),
            "min_turn_radius": round(rmin), "max_grade_deg": round(math.degrees(math.atan(max(grades))), 1),
            "track_mesh_error": round(worst, 2), "walls_without_steep_part": flat_walls[:12],
            "closest_legs_gap": round(close), "terrain_triangles": len(TRIS),
            "leaves": {s: sum(1 for leaf in LEAVES if leaf[2] == s) for s in (64, 128, 256)},
            "brushes": len(BRUSHES) + sum(len(v) for v in GROUPS.values()), "checkpoints": len(CHECKPOINTS)}


def overview(report):
    from PIL import Image, ImageDraw
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    scale = max(x1 - x0, y1 - y0) / 1100
    w, h = int((x1 - x0) / scale), int((y1 - y0) / scale)
    im = Image.new("RGB", (w, h))
    pix = im.load()
    colors = {L_SAND: (196, 140, 92), L_TRACK: (170, 140, 104), L_GRAVEL: (150, 146, 132),
              L_ROCK: (140, 74, 52), L_CLIFF: (168, 92, 62)}
    zmin, zmax = min(GRID.values()), max(GRID.values())
    for v in range(h):
        y = y1 - v * scale
        gy = y0 + round((y - y0) / STEP) * STEP
        for u in range(w):
            x = x0 + u * scale
            gx = x0 + round((x - x0) / STEP) * STEP
            z = GRID.get((gx, gy), zmin)
            zx = GRID.get((gx + STEP, gy), z)
            shade = 0.55 + 0.6 * (z - zmin) / (zmax - zmin) - (zx - z) / STEP * 0.15
            c = colors[MAT.get((gx, gy), L_SAND)]
            pix[u, v] = tuple(max(0, min(255, int(k * shade))) for k in c)
    d = ImageDraw.Draw(im)
    for e in ENTITIES:
        if e["classname"] == "info_notnull":
            x, y, _ = map(float, e["origin"].split())
            u, v = (x - x0) / scale, (y1 - y) / scale
            d.ellipse((u - 6, v - 6, u + 6, v + 6), outline="white", width=2)
            d.text((u + 8, v - 8), e["targetname"], fill="white")
    out = ROOT / "docs" / "maps" / "canyonrush"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "canyonrush_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def write_map():
    fields = {"classname": "worldspawn", "message": "Canyon Rush", "gravity": "1400", "_ambient": "22",
              "_minlight": "14", "gridsize": "128 128 256", "_blocksize": "512 512 4096"}
    text = ["// Generated by tools/maptools/make_canyonrush.py. Q3 brush format.",
            "// The terrain func_groups use maps/q3r_canyonrush_index.pcx at compile time.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    x0, x1, y0, y1 = BOUNDS["x0"], BOUNDS["x1"], BOUNDS["y0"], BOUNDS["y1"]
    for group in ("sand", "dirt", "gravel", "plain"):
        keys = {"classname": "func_group", "_terrain": f"canyon ground, {group} friction",
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
        '{\n'
        '\tmap "q3r_canyonrush"\n'
        '\tlongname "Canyon Rush"\n'
        '\tauthor "Q3Rally Team"\n'
        '\ttype "q3r_sprint"\n'
        '\tstarts "4"\n'
        '\tlaps "1"\n'
        '\tcheckpoints "6"\n'
        '\tobserverspots "0"\n'
        '\tweapons "0"\n'
        '\tpowerups "0"\n'
        '\tbotsupport "None"\n'
        '}\n'
        , encoding="ascii")
    return target


def main():
    build_path()
    hash_path()
    build_terrain()
    dressing()
    shell()
    report = validate()
    write_textures()
    write_shader()
    index_map()
    target = write_map()
    overview(report)
    print(json.dumps(report, indent=2))
    print("Wrote", target)


if __name__ == "__main__":
    main()
