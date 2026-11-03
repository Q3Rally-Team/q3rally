#!/usr/bin/env python3
"""Generate q3r_david4 "Jungle Opening": two-team CTF in a jungle clearing at night.

Rework of the original map by |CE & P3rlE; the concept stays: an open clearing
between two bases at the west (red) and east (blue) ends, the flag on a raised
platform with a cross of team-coloured ramps, the "Save Ferris" water tower and
an oak grove with the Quad in the middle, and the same weapon set.

What changed, mainly for flag access:
- The flag platform is a low octagonal mound. The four team-coloured ramps of
  the cross remain, the corners between them are ramps as well, so the flag is
  reachable from every direction over gentle, rounded slopes and a car can
  drive straight across and leave on the far side.
- No rails around the bases (they were dead ends); the base zone is a painted
  border on the ground. Behind each flag a drive-through outpost holds BFG and
  grenade launcher.
- Gentle terrain instead of a flat box: rolling ground, a creek along the
  centre line (cascade in the north, a ford in the middle, Quad on a stone slab
  in the grove, invisibility under the water tower), rock groups, two ruin
  gates and hills with trees at the edges in front of the forest backdrop.

The map is mirror symmetric about x = 0 (red west, blue east).
Writes the .map, index map, shader, arena, macro texture, an overview image
and docs/maps/david4/validation.json. Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from itertools import combinations
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_david4"
TEX = "q3r_david4/"
DETAIL = 134217728
STEP = 64
FLOOR = -512

FLAG_X = 2560            # flags at (-FLAG_X, 0) red and (FLAG_X, 0) blue
PX, PY = 4160, 1984      # half extents of the play area (ground near z 0)
BX, BY = 5632, 3584      # half extents of the terrain (multiples of 512)
WALL = 200               # invisible boundary this far outside the play area
MESA_RINGS = ((736, 0), (600, 10), (440, 30), (288, 40))   # (apothem, height) of the flag mound
PAD = 112                # apothem of the flag pad on top of the mound
WATER_Z = -10
GATE_X = 1500            # ruin gates on the centre line
TOWER = (0, -1350)
QUAD = (0, 1350)
CULVERT_Y = -2304          # front face of the culvert headwall (on a grid line)

LAYERS = (("grass", "textures/q3r_dirt_grass/grass01.tga", 256),
          ("moss", "textures/q3r_dirt_grass/darkdirtgrass01.jpg", 256),
          ("dirt", "textures/q3r_dirt_grass/darkdirt01.tga", 256),
          ("rock", "textures/q3r_dirt_grass/rock01.tga", 320),
          ("pebble", "textures/q3r_dirt_grass/stones02.tga", 160))
L_GRASS, L_MOSS, L_DIRT, L_ROCK, L_PEBBLE = range(5)
FRICTION = {L_GRASS: "grass", L_MOSS: "grass", L_DIRT: "dirt", L_ROCK: "plain", L_PEBBLE: "gravel"}
PARENTS = (("grass", "surfaceparm SURF_GRASS", L_GRASS), ("dirt", "surfaceparm SURF_DIRT", L_DIRT),
           ("gravel", "surfaceparm SURF_GRAVEL", L_PEBBLE), ("plain", None, L_ROCK))

GRID, MAT, LEAVES, LEAFOF, TRIS = {}, {}, [], {}, []
BRUSHES, GROUPS, ENTITIES, SOLIDS = [], {}, [], []
INFO = {}


# ---------------------------------------------------------------- height field

def smooth(a, b, x):
    t = max(0.0, min(1.0, (x - a) / (b - a)))
    return t * t * (3 - 2 * t)


def bump(x, y, cx, cy, r, h):
    d = math.hypot(x - cx, y - cy) / r
    return h * (1 - smooth(0, 1, d)) if d < 1 else 0.0


def creek_half(y):
    """Half width of the creek bed (bank tops lie BANK further out)."""
    w = 104 + 26 * math.cos(y / 260) * math.cos(y / 610)
    w += 110 * (1 - smooth(0, 1, abs(y - QUAD[1]) / 260))      # wide at the Quad slab
    w += 40 * (1 - smooth(0, 1, abs(y) / 420))                  # wide shallow ford in the middle
    w += 230 * smooth(PY - 420, PY - 60, y)                     # pool below the cascade
    return w


BANK = 150


def creek_depth(y):
    ford = 1 - smooth(260, 520, abs(y))
    return 22 - 8 * ford


def creek(x, y):
    """Depth of the creek channel below the surrounding ground (0 outside)."""
    if y < -PY - 520:
        return 0.0
    r = abs(x) - creek_half(y)
    d = creek_depth(y) * (1 - smooth(PY + 240, PY + 300, y))
    if r <= 0:
        return d
    if r >= BANK:
        return 0.0
    return d * (1 - smooth(0, BANK, r))


def outside(x, y):
    """Distance outside the play rectangle (rounded corners), negative inside."""
    rc = 640
    qx, qy = abs(x) - (PX - rc), abs(y) - (PY - rc)
    if qx > 0 and qy > 0:
        return math.hypot(qx, qy) - rc
    return max(qx, qy) - rc


def hills(x, y):
    s = outside(x, y)
    if s <= 0:
        return 0.0
    ax = abs(x)
    wave = 0.75 + 0.25 * math.cos(ax / 530 + 1.1) * math.cos(y / 470) + 0.12 * math.cos(ax / 190) * math.cos(y / 230)
    rise = 560 * wave * smooth(0, 1050, s) ** 1.25
    return rise + 18 * smooth(0, 160, s)


def notch(x, y):
    """The creek cuts into the north hill up to the cascade and into the south
    hill up to the culvert."""
    ax = abs(x)
    if y > 0:
        if ax > 420:
            return None
        cap = 0.0 if y < PY + 260 else min(330.0, (y - PY - 260) * 2.4)
        return cap + 220 * smooth(260, 420, ax)
    if ax > 360:
        return None
    cap = 0.0 if y > CULVERT_Y - 40 else 400.0
    return cap + 220 * smooth(220, 360, ax)


def rolling(x, y):
    ax = abs(x)                       # mirror symmetric about x = 0
    fade = 1 - smooth(1250, 1700, ax)
    base = 11 * math.sin(ax / 410 + 0.4) * math.cos(y / 350) + 7 * math.cos(ax / 233 - y / 297)
    mounds = 0.0
    for sx in (-1, 1):
        mounds += bump(x, y, sx * 1150, 520, 470, 52) + bump(x, y, sx * 1050, -760, 420, 40)
        mounds += bump(x, y, sx * 3350, 1500, 520, 46) + bump(x, y, sx * 3350, -1500, 520, 46)
    return base * fade + mounds


def height(x, y):
    h = rolling(x, y) + hills(x, y)
    n = notch(x, y)
    if n is not None:
        h = min(h, n)
    return h - creek(x, y)


# ---------------------------------------------------------------- terrain mesh

def material(x, y, raw):
    z = raw[x, y]
    gx = (raw.get((x + STEP, y), z) - raw.get((x - STEP, y), z)) / (2 * STEP)
    gy = (raw.get((x, y + STEP), z) - raw.get((x, y - STEP), z)) / (2 * STEP)
    slope = math.hypot(gx, gy)
    c = creek(x, y)
    if c > 6:
        return L_PEBBLE
    if slope > 0.75:
        return L_ROCK
    if c > 0.5 or (slope > 0.42 and outside(x, y) > 0):
        return L_MOSS
    t = track(x, y)
    if t < 70:
        return L_DIRT
    if t < 150:
        return L_MOSS
    patch = math.cos(abs(x) / 380) * math.cos(y / 330) + 0.6 * math.cos(abs(x) / 151 + 1) * math.cos(y / 173)
    if patch > 1.05:
        return L_MOSS
    return L_GRASS


TRACKS = []


def build_tracks():
    """Worn dirt lines the cars use (mirrored)."""
    half = [[(150, 0), (900, 40), (1500, 0), (1900, -60), (2200, -500), (2560, -800), (2950, -620),
             (3300, 0), (2950, 620), (2560, 800), (2200, 500), (1900, 60)],
            [(2560, 800), (2300, 1250), (1700, 1450), (1000, 1250), (380, 1330)],
            [(2560, -800), (2300, -1250), (1700, -1480), (1000, -1300), (300, -1250)],
            [(3300, 0), (3750, 0)]]
    for line in half:
        for s in (1, -1):
            TRACKS.append([(s * x, y) for x, y in line])


def seg_dist(px, py, a, b):
    ax, ay = a
    bx, by = b
    dx, dy = bx - ax, by - ay
    t = max(0.0, min(1.0, ((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy)))
    return math.hypot(px - ax - t * dx, py - ay - t * dy)


def track(x, y):
    wob = 22 * math.sin(abs(x) / 97 + y / 131)
    return min(seg_dist(x, y, a, b) for line in TRACKS for a, b in zip(line, line[1:])) + wob


def build_terrain():
    raw = {}
    for y in range(-BY, BY + 1, STEP):
        for x in range(-BX, BX + 1, STEP):
            raw[x, y] = height(x, y)
    GRID.update(raw)

    def err(cx, cy, size):
        a, b = raw[cx, cy], raw[cx + size, cy]
        c, d = raw[cx + size, cy + size], raw[cx, cy + size]
        worst = 0.0
        for i in range(size // STEP + 1):
            for j in range(size // STEP + 1):
                u, v = i * STEP / size, j * STEP / size
                bil = a * (1 - u) * (1 - v) + b * u * (1 - v) + c * u * v + d * (1 - u) * v
                worst = max(worst, abs(bil - raw[cx + i * STEP, cy + j * STEP]))
        return worst

    def split(cx, cy, size):
        inside = outside(cx + size / 2, cy + size / 2) < 300
        tol = 1.5 if inside else 10.0
        near_creek = abs(cx + size / 2) < 700 and abs(cy + size / 2) < PY + 900
        if size == STEP or (err(cx, cy, size) <= tol and not (near_creek and size > 128)
                            and not (inside and size > 256)):
            LEAVES.append((cx, cy, size))
            return
        h = size // 2
        for qy in (cy, cy + h):
            for qx in (cx, cx + h):
                split(qx, qy, h)
    for cy in range(-BY, BY, 512):
        for cx in range(-BX, BX, 512):
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
    for key in raw:
        MAT[key] = material(key[0], key[1], raw)
    order = ("plain", "grass", "dirt", "gravel")
    for (lx, ly, size) in LEAVES:
        a, b = (lx, ly, GRID[lx, ly]), (lx + size, ly, GRID[lx + size, ly])
        c, d = (lx + size, ly + size, GRID[lx + size, ly + size]), (lx, ly + size, GRID[lx, ly + size])
        hc = raw[lx + size // 2, ly + size // 2] if size > STEP else height(lx + size / 2, ly + size / 2)
        ac, bd = abs((a[2] + c[2]) / 2 - hc), abs((b[2] + d[2]) / 2 - hc)
        diag = "ac" if ac <= bd else "bd"
        for fx in range(lx, lx + size, STEP):
            for fy in range(ly, ly + size, STEP):
                LEAFOF[fx, fy] = (lx, ly, size, diag)
        tris = ((a, b, c), (a, c, d)) if diag == "ac" else ((a, b, d), (b, c, d))
        far = outside(lx + size / 2, ly + size / 2) > 700
        for tri in tris:
            TRIS.append(tri)
            if far:
                prism(tri, TEX + "backdrop", group="world")
                continue
            classes = [FRICTION[MAT[p[0], p[1]]] for p in tri]
            major = max(set(classes), key=lambda k: (classes.count(k), -order.index(k)))
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
    fx = -BX + math.floor((x + BX) / STEP) * STEP
    fy = -BY + math.floor((y + BY) / STEP) * STEP
    fx = max(-BX, min(BX - STEP, fx))
    fy = max(-BY, min(BY - STEP, fy))
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


def face(points, material, scale=0.5, detail=True, shift=(0, 0)):
    p = " ".join("( %s )" % " ".join(num(v) for v in xyz) for xyz in points)
    sx, sy = scale if isinstance(scale, tuple) else (scale, scale)
    return f"{p} {material} {num(shift[0])} {num(shift[1])} 0 {num(sx)} {num(sy)} {DETAIL if detail else 0} 0 0"


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


def convex(planes, solid=True, scale=0.5, tag=None):
    """(outward normal, point, material) planes; a material may also be
    (material, scale, shift) for a fitted texture."""
    brush([face(plane(n, p), m[0], scale=m[1], shift=m[2]) if isinstance(m, tuple) else
           face(plane(n, p), m, scale=scale) for n, p, m in planes])
    corners = polytope([(n, p) for n, p, _ in planes])
    if solid and corners:
        SOLIDS.append((tuple(min(c[i] for c in corners) for i in range(3)),
                       tuple(max(c[i] for c in corners) for i in range(3)), tag))


def box(lo, hi, mat, solid=True, side=None, top=None, bottom=None, scale=0.5, tag=None):
    (ax, ay, az), (bx, by, bz) = lo, hi
    s = side or mat
    convex([((0, 0, -1), (ax, ay, az), bottom or s), ((0, 0, 1), (bx, by, bz), top or mat),
            ((-1, 0, 0), (ax, ay, az), s), ((1, 0, 0), (bx, by, bz), s),
            ((0, -1, 0), (ax, ay, az), s), ((0, 1, 0), (bx, by, bz), s)], solid=solid, scale=scale, tag=tag)


def ngon(cx, cy, r, n, z0, z1, mat, cap=None, solid=True, phase=0.0, scale=0.5, tag=None):
    """Upright n-sided prism (apothem r)."""
    planes = [((0, 0, 1), (cx, cy, z1), cap or mat), ((0, 0, -1), (cx, cy, z0), cap or mat)]
    for k in range(n):
        a = phase + 2 * math.pi * k / n
        nn = (math.cos(a), math.sin(a), 0)
        planes.append((nn, (cx + nn[0] * r, cy + nn[1] * r, z0), mat))
    convex(planes, solid=solid, scale=scale, tag=tag)


def hpipe(x0, y0, x1, y1, z, r, mat, solid=True):
    """Horizontal octagonal pipe between two points."""
    L = math.hypot(x1 - x0, y1 - y0)
    t = ((x1 - x0) / L, (y1 - y0) / L, 0)
    nrm = (-t[1], t[0], 0)
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    planes = [(t, (x1, y1, z), mat), ((-t[0], -t[1], 0), (x0, y0, z), mat)]
    for k in range(8):
        a = math.pi / 8 + k * math.pi / 4
        n = (nrm[0] * math.cos(a), nrm[1] * math.cos(a), math.sin(a))
        planes.append((n, (cx + n[0] * r, cy + n[1] * r, z + n[2] * r), mat))
    convex(planes, solid=solid)


def boulder(x, y, radius, tall, seed, mat=TEX + "rock", sink=24):
    z0 = min(ground(x + dx, y + dy) for dx in (-radius, 0, radius) for dy in (-radius, 0, radius))
    centre = (x, y, z0)
    planes = [((0, 0, -1), (x, y, z0 - sink), mat), ((0, 0, 1), (x, y, z0 + tall), mat)]
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
    convex(planes, tag="boulder")


def entity(classname, x, y, z, **keys):
    fields = {"classname": classname, "origin": f"{round(x)} {round(y)} {round(z)}", **keys}
    ENTITIES.append(fields)
    return fields


def light(x, y, z, value, color, **keys):
    return entity("light", x, y, z, light=str(value), _color=" ".join(f"{c:.2f}" for c in color), **keys)


def item(classname, x, y, **keys):
    return entity(classname, x, y, ground(x, y) + 24, **keys)


# ---------------------------------------------------------------- bases

TEAMS = (("red", -1), ("blue", 1))


def mesa(team, sx):
    """Octagonal flag mound: team-coloured cross ramps on the axes, corner ramps
    between them, flat top with the flag pad. Pieces between two rings are
    planar trapezoids (parallel horizontal edges)."""
    cx = sx * FLAG_X
    cross_mat, corner_mat = TEX + f"ramp_{team}", TEX + "ramp_stone"
    rim = TEX + "trim"
    half = math.pi / 8
    for k in range(8):
        th = k * math.pi / 4
        n = (math.cos(th), math.sin(th))
        mat = cross_mat if k % 2 == 0 else corner_mat
        a0, a1 = th - half, th + half
        side0 = (math.sin(a0), -math.cos(a0), 0)       # radial planes, normals point away from the piece
        side1 = (-math.sin(a1), math.cos(a1), 0)
        for (ro, zo), (ri, zi) in zip(MESA_RINGS, MESA_RINGS[1:]):
            # sloped top through the two ring edges
            run = ro - ri
            slope = (zi - zo) / run
            nt = (n[0] * slope, n[1] * slope, 1.0)
            ln = math.sqrt(sum(c * c for c in nt))
            nt = tuple(c / ln for c in nt)
            p_top = (cx + n[0] * ro, n[1] * ro, zo)
            convex([(nt, p_top, mat),
                    ((0, 0, -1), (cx, 0, -24), rim),
                    ((n[0], n[1], 0), (cx + n[0] * ro, n[1] * ro, 0), rim),
                    ((-n[0], -n[1], 0), (cx + n[0] * ri, n[1] * ri, 0), rim),
                    (side0, (cx, 0, 0), rim), (side1, (cx, 0, 0), rim)], scale=0.5, tag="mesa")
    top_r, top_z = MESA_RINGS[-1]
    ngon(cx, 0, top_r, 8, -24, top_z, rim, cap=TEX + f"deck_{team}", tag="mesa")
    # pad: the 256 px swirl fitted once onto the pad, centred on the flag (Q3 axial
    # projection of an upward face: s = x / scale + shift, t = -y / scale + shift)
    img = 256
    sc = 2 * PAD / img
    fit = (TEX + f"pad_{team}", sc, ((img / 2 - cx / sc) % img, (img / 2) % img))
    planes = [((0, 0, 1), (cx, 0, top_z + 2), fit), ((0, 0, -1), (cx, 0, top_z), rim)]
    for k in range(8):
        a = 2 * math.pi * k / 8
        nn = (math.cos(a), math.sin(a), 0)
        planes.append((nn, (cx + nn[0] * PAD, nn[1] * PAD, top_z), rim))
    convex(planes, tag="pad")
    # glowing team strips on the edge of the top
    for k in range(8):
        th = k * math.pi / 4 + math.pi / 8
        r = top_r / math.cos(math.pi / 8) - 10
        x, y = cx + r * math.cos(th), r * math.sin(th)
        box((x - 6, y - 6, top_z), (x + 6, y + 6, top_z + 8), TEX + f"glow_{team}", solid=False)
    entity(f"team_CTF_{team}flag", cx, 0, top_z + 26)
    color = (1.0, 0.25, 0.15) if team == "red" else (0.2, 0.4, 1.0)
    light(cx, 0, top_z + 70, 260, color)
    for k in range(4):
        th = k * math.pi / 2 + math.pi / 4
        light(cx + 380 * math.cos(th), 380 * math.sin(th), 90, 240, color)


def border(team, sx):
    """Painted base border across the field (flat ground) and along the flanks."""
    x = sx * 1960
    mat = TEX + f"border_{team}"
    for y0 in range(-1760, 1760, 160):
        lo, hi = (x - 16, y0 + 8, 0), (x + 16, y0 + 152, 1)
        zs = [ground(x, y0 + 8), ground(x, y0 + 152)]
        box((lo[0], lo[1], max(zs) + 0.5), (hi[0], hi[1], max(zs) + 1.5), mat, solid=False)


def outpost(team, sx):
    """Drive-through hall behind the flag: roof on pillars, BFG and grenade launcher
    inside, a back wall with openings, team-painted roof edge and lamps."""
    xc = sx * 3760
    x0, x1 = xc - 170, xc + 170
    y0, y1 = -760, 760
    roof, pillar, paint = TEX + "roof", TEX + "concrete", TEX + f"paint_{team}"
    zt = 200
    for y in (-760, -380, 0, 380, 760):
        for x in (x0, x1):
            if y == 0 and x == (x1 if sx < 0 else x0):
                continue                     # wider opening on the front side centre
            box((x - 18, y - 18, -24), (x + 18, y + 18, zt), pillar, tag="outpost")
    box((x0 - 30, y0 - 30, zt), (x1 + 30, y1 + 30, zt + 20), roof, side=paint, bottom=TEX + "concrete")
    box((x0 - 34, y0 - 34, zt + 20), (x1 + 34, y1 + 34, zt + 28), paint, solid=True)
    # low walls at both ends with a slit (cover, still open front and back)
    for y in (y0 - 18, y1 + 18):
        for (a, b) in ((x0 - 18, xc - 60), (xc + 60, x1 + 18)):
            box((a, y - 10, -24), (b, y + 10, 90), pillar, tag="outpost")
    # lamps under the roof
    for y in (-570, -190, 190, 570):
        box((xc - 40, y - 8, zt - 6), (xc + 40, y + 8, zt), TEX + "lamp", solid=False)
        light(xc, y, zt - 30, 200, (1.0, 0.86, 0.62))
    # sign above the front
    fx = x1 + 34 if sx < 0 else x0 - 34
    box((min(fx, fx - sx * 4) - 2, -300, zt + 28), (max(fx, fx - sx * 4) + 2, 300, zt + 92), TEX + f"sign_{team}",
        solid=False, side=TEX + f"sign_{team}")
    # weapons inside (BFG south bay, grenade launcher north bay), ammo next to them
    item("weapon_bfg", xc, -470, count="25")
    item("ammo_bfg", xc - 80, -560)
    item("ammo_bfg", xc + 80, -560)
    item("weapon_grenadelauncher", xc, 470, count="25")
    item("ammo_grenades", xc - 80, 560)
    item("ammo_grenades", xc + 80, 560)
    item("item_health_large", xc, 0)


def masts(team, sx):
    """Two floodlight masts at the back corners of the base, aimed at the mound:
    floodmast model (team-coloured lenses, scaled to 430 high), invisible
    collision around the mast, light in front of the heads."""
    color = (1.0, 0.35, 0.25) if team == "red" else (0.35, 0.5, 1.0)
    for sy in (-1, 1):
        x, y = sx * 3250, sy * 1180
        z = ground(x, y)
        aim = math.atan2(-y, sx * FLAG_X - x)
        entity("misc_model", x, y, z - 2, model=f"models/mapobjects/q3r_lanterns/floodmast_{team}.md3",
               angle=f"{math.degrees(aim) % 360:.0f}", modelscale="0.86")
        ngon(x, y, 12, 8, z - 20, z + 420, TEX + "boundary", tag="mast")
        light(x + 170 * math.cos(aim), y + 170 * math.sin(aim), z + 360, 900, color)


def spawns(team, sx):
    """team_CTF_<team>player (match start) and team_CTF_<team>spawn (respawn),
    on both sides of the outpost and along the flanks, facing the field."""
    ang = "0" if sx < 0 else "180"
    starts = [(3420, -1000), (3420, -700), (3420, 700), (3420, 1000), (3060, -1450), (3060, 1450),
              (3600, -1300), (3600, 1300)]
    respawn = [(4010, -1060), (4010, 1060), (3420, -400), (3420, 400), (2950, -1700), (2950, 1700),
               (3750, -1600), (3750, 1600)]
    for x, y in starts:
        entity(f"team_CTF_{team}player", sx * x, y, ground(sx * x, y) + 40, angle=ang)
    for x, y in respawn:
        entity(f"team_CTF_{team}spawn", sx * x, y, ground(sx * x, y) + 40, angle=ang)
    INFO.setdefault("spawns", 0)
    INFO["spawns"] += len(starts) + len(respawn)


def base_items(team, sx):
    item("weapon_rocketlauncher", sx * 1900, 1620)
    item("ammo_rockets", sx * 1820, 1700)
    item("ammo_rockets", sx * 1980, 1700)
    item("weapon_lightning", sx * 1900, -1620)
    item("ammo_lightning", sx * 1820, -1700)
    item("ammo_lightning", sx * 1980, -1700)
    item("item_health_large", sx * 2560, 1560)
    item("item_health_large", sx * 2560, -1560)
    item("item_health_large", sx * 1150, 520)


# ---------------------------------------------------------------- middle

def ruin_gate(sx):
    """Old stone gate on the centre line between creek and base; a car passes
    under the lintel or around it."""
    x = sx * GATE_X
    stone = TEX + "stone"
    for sy in (-1, 1):
        y = sy * 300
        z = ground(x, y)
        box((x - 44, y - 44, z - 30), (x + 44, y + 44, z + 230), stone, tag="gate")
        box((x - 52, y - 52, z - 30), (x + 52, y + 52, z + 26), stone, tag="gate")
        # wall lantern on the pillar face towards the middle (arm 22 out, glass 32 below the mount)
        entity("misc_model", x - sx * 44, y, z + 150, model="models/mapobjects/q3r_lanterns/lantern_wall.md3",
               angle="0" if sx < 0 else "180")
        light(x - sx * 66, y, z + 117, 160, (1.0, 0.7, 0.4))
    zt = max(ground(x, -300), ground(x, 300)) + 230
    box((x - 50, -352, zt), (x + 50, 352, zt + 62), stone, tag="gate")
    # rubble
    rng = random.Random(7)
    for k in range(5):
        bx = x + sx * rng.uniform(-260, 260)
        by = rng.choice((-1, 1)) * rng.uniform(420, 700)
        boulder(bx, by, rng.uniform(26, 44), rng.uniform(24, 46), 3.1 + k * 1.7, TEX + "stone", sink=20)


def rock_groups():
    rng = random.Random(11)
    spots = [(800, 820, 3), (820, -980, 3), (2050, 1150, 2), (2050, -1150, 2), (3350, 1880, 3), (3350, -1880, 3)]
    count = 0
    for sx in (-1, 1):
        r2 = random.Random(11)
        for x, y, n in spots:
            for k in range(n):
                dx, dy = r2.uniform(-140, 140), r2.uniform(-120, 120)
                rad = r2.uniform(48, 92)
                boulder(sx * (x + dx), y + dy, rad, rad * r2.uniform(0.7, 1.15), 1.3 + k + x / 400, sink=30)
                count += 1
    # stones along the creek banks (outside the bed)
    for y in range(-1700, 1800, 260):
        if abs(y) < 600 or abs(y - QUAD[1]) < 380 or abs(y - TOWER[1]) < 380:
            continue
        for sx in (-1, 1):
            r = rng.uniform(22, 40)
            x = sx * (creek_half(y) + BANK * 0.55 + rng.uniform(0, 40))
            boulder(x, y + rng.uniform(-60, 60), r, r * 0.8, y / 300 + sx, sink=18)
            count += 1
    INFO["boulders"] = count


def quad_slab():
    """Flat stone slab in the creek in the grove; low bevelled edges."""
    x0, y0 = QUAD
    bed = -creek_depth(y0)
    top = WATER_Z + 6
    hx, hy = 150, 120
    bev = 70
    stone = TEX + "slab"
    planes = [((0, 0, 1), (x0, y0, top), stone), ((0, 0, -1), (x0, y0, bed - 20), stone)]
    s = (top - bed) / bev
    for n in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        reach = hx if n[1] == 0 else hy
        nt = (n[0] * s, n[1] * s, 1.0)
        ln = math.sqrt(sum(c * c for c in nt))
        nt = tuple(c / ln for c in nt)
        planes.append((nt, (x0 + n[0] * reach, y0 + n[1] * reach, top), stone))
    convex(planes, tag="slab")
    entity("rally_item_quad", x0, y0, top + 26)


def grove():
    """Oaks in two mirrored halves around the Quad slab; trunks are solid posts."""
    rng = random.Random(5)
    placed = []
    models = ("oak1", "oak2", "oak3", "oak3", "oak2")
    tries = 0
    while len(placed) < 14 and tries < 4000:
        tries += 1
        x = rng.uniform(380, 1250)
        y = rng.uniform(850, 1900)
        if seg_dist(x, y, (380, 1330), (1000, 1250)) < 200:
            continue
        if any(math.dist((x, y), p) < 300 for p in placed):
            continue
        if track(x, y) < 120 or creek(x, y) > 0:
            continue
        placed.append((x, y))
    for k, (x, y) in enumerate(placed):
        scale = 1.15 + 0.45 * ((k * 7) % 5) / 4
        for sx in (-1, 1):
            z = ground(sx * x, y)
            entity("misc_model", sx * x, y, z - 6, model=f"models/mapobjects/oak/{models[k % 5]}.md3",
                   modelscale=f"{scale:.2f}", angle=str((k * 67 + (0 if sx < 0 else 180)) % 360))
            box((sx * x - 16, y - 16, z - 20), (sx * x + 16, y + 16, z + 150), TEX + "bark", tag="tree")
    INFO["grove"] = 2 * len(placed)
    # lanterns at the slab
    for sx in (-1, 1):
        x, y = sx * (creek_half(QUAD[1]) + 90), QUAD[1]
        z = ground(x, y)
        lantern_post(x, y, z, -sx, 220)


def water_tower():
    """'Save Ferris' tower straddling the creek: four legs, bracing above head
    height, ladder, ground pipes to two valve houses; invisibility under it."""
    x0, y0 = TOWER
    leg = TEX + "metal"
    top = 608
    for sx in (-1, 1):
        for sy in (-1, 1):
            x, y = x0 + sx * 150, y0 + sy * 150
            z = ground(x, y)
            box((x - 14, y - 14, z - 30), (x + 14, y + 14, top), leg, tag="tower")
    # bracing panels (nonsolid lattice) above 230
    for (ax, ay, bx, by) in ((-150, -150, 150, -150), (-150, 150, 150, 150), (-150, -150, -150, 150),
                             (150, -150, 150, 150)):
        lo = (x0 + min(ax, bx) - 2, y0 + min(ay, by) - 2, 240)
        hi = (x0 + max(ax, bx) + 2, y0 + max(ay, by) + 2, top)
        box(lo, hi, TEX + "struts", solid=False, scale=1.0)
    box((x0 - 160, y0 - 160, top), (x0 + 160, y0 + 160, top + 8), TEX + "struts", solid=False, scale=1.0)
    entity("misc_model", x0, y0, top + 192, model="models/mapobjects/watertower/tower.md3", angle="90")
    # ladder on the south-east leg
    box((x0 + 164, y0 - 172, 40), (x0 + 168, y0 - 140, top), TEX + "ladder", solid=False, scale=0.25)
    # down pipe into the creek and ground pipes to the valve houses
    ngon(x0, y0, 20, 8, 250, top, TEX + "pipe", tag="tower")
    for sx in (-1, 1):
        hx = x0 + sx * 640
        z = ground(hx, y0 - 330)
        hpipe(x0 + sx * 200, y0 - 330, hx - sx * 70, y0 - 330, z + 26, 24, TEX + "pipe")
        for k in range(3):
            px = x0 + sx * (260 + k * 140)
            box((px - 14, y0 - 360, ground(px, y0 - 330) - 20), (px + 14, y0 - 300, ground(px, y0 - 330) + 14),
                TEX + "concrete", tag="pipe")
        zh = ground(hx, y0 - 330)
        box((hx - 70, y0 - 420, zh - 24), (hx + 70, y0 - 240, zh + 96), TEX + "concrete", tag="valve")
        box((hx - 78, y0 - 428, zh + 96), (hx + 78, y0 - 232, zh + 106), TEX + "metal", solid=False)
        light(hx, y0 - 160, zh + 150, 110, (0.95, 0.9, 0.7))
        box((hx - 20, y0 - 242, zh + 70), (hx + 20, y0 - 238, zh + 82), TEX + "lamp", solid=False)
    light(x0, y0, top - 30, 420, (0.85, 0.9, 1.0))
    light(x0, y0 + 210, 120, 160, (0.6, 0.8, 1.0))
    entity("rally_item_invis", x0, y0, ground(x0, y0) + 24)


def cascade():
    """Rock step at the north end of the creek: the water runs down the notch
    in three sheets between boulders that hide the edges."""
    y0 = PY + 260
    zt = 320
    rock = TEX + "rock"
    n = (0.0, -2.4, 1.0)
    ln = math.sqrt(sum(c * c for c in n))
    n = tuple(c / ln for c in n)
    p = (0, y0 - 6, 0)
    for (xa, xb, za, zb) in ((-120, 40, WATER_Z - 2, 130), (-60, 110, 120, 230), (-90, 70, 220, zt - 10)):
        convex([(n, p, TEX + "waterfall"), ((-n[0], -n[1], -n[2]), (0, y0 - 2, 0), "common/nodraw"),
                ((1, 0, 0), (xb, 0, 0), "common/nodraw"), ((-1, 0, 0), (xa, 0, 0), "common/nodraw"),
                ((0, 0, 1), (0, 0, zb), "common/nodraw"), ((0, 0, -1), (0, 0, za), "common/nodraw")],
               solid=False)
    rocks = [(-175, 25, 70, 120), (165, 30, 75, 130), (125, 70, 55, 150), (-130, 95, 60, 180),
             (-20, 112, 34, 190), (150, 125, 65, 250), (-160, 140, 70, 260), (40, 150, 46, 300),
             (-240, 60, 80, 200), (245, 70, 80, 210)]
    for k, (x, dy, r, top) in enumerate(rocks):
        bz = 2.4 * dy - 30
        boulder(x, y0 + dy, r, top - bz, 7.7 + k * 1.3, rock, sink=max(40, int(top - bz)))
    light(0, y0 - 220, 80, 260, (0.55, 0.75, 1.0))
    INFO["cascade_top"] = zt


def culvert():
    """Concrete headwall with a pipe mouth where the creek leaves to the south."""
    y = CULVERT_Y
    conc = TEX + "concrete"
    box((-380, y - 60, -40), (380, y, 300), conc, tag="culvert")
    box((-140, y - 5, -24), (140, y + 1, 150), TEX + "dark", solid=False)
    # ring of the pipe mouth: 16 segments, each a tangential block
    r0, r1 = 140, 170
    cz = -10
    for k in range(9):
        a0, a1 = math.pi * k / 9, math.pi * (k + 1) / 9
        am = (a0 + a1) / 2
        nm = (math.cos(am), 0.0, math.sin(am))
        planes = [((0, -1, 0), (0, y - 14, 0), TEX + "pipe"), ((0, 1, 0), (0, y + 8, 0), TEX + "pipe"),
                  (nm, (r1 * nm[0], 0, cz + r1 * nm[2]), TEX + "pipe"),
                  ((-nm[0], 0, -nm[2]), (r0 * nm[0], 0, cz + r0 * nm[2]), TEX + "pipe"),
                  ((math.sin(a0), 0, -math.cos(a0)), (0, 0, cz), TEX + "pipe"),
                  ((-math.sin(a1), 0, math.cos(a1)), (0, 0, cz), TEX + "pipe")]
        convex(planes, solid=False)


def water():
    """Water surface over the creek bed (one brush per 128-unit slice)."""
    mat = TEX + "water"
    y = -PY - 400
    while y < PY + 300:
        y1 = y + 128
        w = max(creek_half(y), creek_half(y1), creek_half((y + y1) / 2)) + BANK * 0.6
        box((-w, y, WATER_Z - 40), (w, y1, WATER_Z), mat, solid=False, side="common/nodraw",
            bottom="common/nodraw", top=mat)
        y = y1


# ---------------------------------------------------------------- edge, sky, misc

def forest_edge():
    """Trees on the hills around the clearing (no collision, out of reach)."""
    rng = random.Random(3)
    models = ("GR_trees/tree1", "GR_trees/tree4", "GR_trees/tree6", "GR_trees/tree3", "oak/oak1",
              "oak/oak3", "GR_trees/small4")
    placed = []
    for _ in range(40000):
        x = rng.uniform(-BX + 200, BX - 200)
        y = rng.uniform(-BY + 200, BY - 200)
        s = outside(x, y)
        if s < WALL + 70 or s > 1500:
            continue
        if abs(x) < 460 and abs(y) > PY - 100:
            continue
        if any(math.dist((x, y), p) < 150 + 0.12 * s for p in placed[-400:]):
            continue
        placed.append((x, y))
        m = models[rng.randrange(len(models))]
        scale = (1.6 + rng.random() * 1.1) * (1.25 if m.startswith("GR") else 1.0)
        entity("misc_model", x, y, ground(x, y) - 10, model=f"models/mapobjects/{m}.md3",
               modelscale=f"{scale:.2f}", angle=str(rng.randrange(360)))
        if len(placed) >= 360:
            break
    INFO["edge_trees"] = len(placed)


def backdrop():
    """The forest photo wall of the original, now behind the hills."""
    mat = TEX + "forest"
    z0, z1 = 100, 1300
    m = 32
    sc = (z1 - z0) / 256
    shift = (0, (z1 / sc) % 256)
    for lo, hi in (((-BX - m, -BY, z0), (-BX, BY, z1)), ((BX, -BY, z0), (BX + m, BY, z1)),
                   ((-BX, -BY - m, z0), (BX, -BY, z1)), ((-BX, BY, z0), (BX, BY + m, z1))):
        brush([face(plane(nrm, pt), mat, scale=(sc, sc), detail=False, shift=shift) for nrm, pt in
               (((0, 0, -1), lo), ((0, 0, 1), hi), ((-1, 0, 0), lo), ((1, 0, 0), hi), ((0, -1, 0), lo),
                ((0, 1, 0), hi))])


def boundary():
    """Invisible solid walls a little outside the play area; the creek notches
    are closed as well."""
    x0, x1, y0, y1 = -PX - WALL, PX + WALL, -PY - WALL, PY + WALL
    zt = 2000
    for lo, hi in (((x0 - 32, y0 - 32, FLOOR), (x0, y1 + 32, zt)), ((x1, y0 - 32, FLOOR), (x1 + 32, y1 + 32, zt)),
                   ((x0, y0 - 32, FLOOR), (x1, y0, zt)), ((x0, y1, FLOOR), (x1, y1 + 32, zt))):
        box(lo, hi, TEX + "boundary", solid=False)


def shell():
    top = 2400
    m = 64
    sky = TEX + "sky"
    for lo, hi, mat in (((-BX - 96, -BY - 96, FLOOR - 64), (BX + 96, BY + 96, FLOOR), "common/caulk"),
                        ((-BX - 96, -BY - 96, FLOOR), (-BX - 32, BY + 96, top), sky),
                        ((BX + 32, -BY - 96, FLOOR), (BX + 96, BY + 96, top), sky),
                        ((-BX - 32, -BY - 96, FLOOR), (BX + 32, -BY - 32, top), sky),
                        ((-BX - 32, BY + 32, FLOOR), (BX + 32, BY + 96, top), sky),
                        ((-BX - 96, -BY - 96, top), (BX + 96, BY + 96, top + m), sky)):
        (ax, ay, az), (bx, by, bz) = lo, hi
        brush([face(plane(nrm, pt), mat, detail=False) for nrm, pt in
               (((0, 0, -1), (ax, ay, az)), ((0, 0, 1), (bx, by, bz)), ((-1, 0, 0), (ax, ay, az)),
                ((1, 0, 0), (bx, by, bz)), ((0, -1, 0), (ax, ay, az)), ((0, 1, 0), (bx, by, bz)))])
    brush([face(plane(nrm, pt), "common/lightgrid") for nrm, pt in
           (((0, 0, -1), (-PX - 400, -PY - 400, -100)), ((0, 0, 1), (PX + 400, PY + 400, 900)),
            ((-1, 0, 0), (-PX - 400, 0, 0)), ((1, 0, 0), (PX + 400, 0, 0)),
            ((0, -1, 0), (0, -PY - 400, 0)), ((0, 1, 0), (0, PY + 400, 0)))])


def middle_items():
    entity("weapon_flamethrower", 0, 0, ground(0, 0) + 24)
    entity("ammo_flame", 0, -150, ground(0, -150) + 24)
    entity("ammo_flame", 0, 150, ground(0, 150) + 24)
    for sx in (-1, 1):
        item("ammo_bfg", sx * 420, -40)
        item("item_fuelcan", sx * 1250, 1560)
        item("item_fuelcan", sx * 1250, -1600)
        item("item_health_large", sx * 1020, -560)


def lantern_post(x, y, z, dx, value):
    """Cast-iron lantern post (model) with the lantern on the side dx (+1 / -1
    along x); invisible collision around the shaft, light at the glass."""
    entity("misc_model", x, y, z, model="models/mapobjects/q3r_lanterns/lantern_post.md3",
           angle="0" if dx > 0 else "180")
    box((x - 5, y - 5, z - 20), (x + 5, y + 5, z + 120), TEX + "boundary", tag="post")
    light(x + dx * 34, y, z + 106, value, (1.0, 0.72, 0.38))


def lanterns():
    """Warm lanterns on posts along the dirt tracks (night orientation)."""
    spots = [(2200, 920), (2200, -920), (3560, 900), (3560, -900), (650, 1150), (600, -1150)]
    for sx in (-1, 1):
        for x, y in spots:
            X = sx * x
            lantern_post(X, y, ground(X, y), -sx, 190)


def misc_entities():
    spots = [(-3300, -1500, 520, 25, 35), (3300, 1500, 520, 25, 215), (0, -1950, 700, 22, 90),
             (0, 1900, 620, 20, 270), (-3900, 1700, 520, 22, -30)]
    for x, y, z, pitch, yaw in spots:
        entity("info_observer_spot", x, y, z, angles=f"{pitch} {yaw} 0", spawnflags="1")
    entity("info_player_intermission", 0, -2150, 760, angles="24 90 0")
    for x, y in ((-900, 0), (900, 0)):
        entity("info_player_deathmatch", x, y, ground(x, y) + 40, angle="90")


# ---------------------------------------------------------------- shaders, textures

def tcgen(layer):
    _, _, tile = LAYERS[layer]
    k = 1.0 / tile
    if LAYERS[layer][0] == "rock":
        return f"\t\ttcGen vector ( {0.71 * k:.7f} {0.71 * k:.7f} 0 ) ( 0 0 {k:.7f} )"
    return f"\t\ttcGen vector ( {k:.7f} 0 0 ) ( 0 {k:.7f} 0 )"


def stage(layer, blend):
    lines = ["\t{", f"\t\tmap {LAYERS[layer][1]}"]
    if blend:
        lines += ["\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\talphaGen vertex"]
    return lines + [tcgen(layer), "\t\trgbGen identity", "\t}"]


def tail():
    return ["\t{", f"\t\tmap textures/{TEX}macro.jpg", "\t\tblendFunc GL_DST_COLOR GL_SRC_COLOR",
            "\t\ttcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )", "\t\trgbGen identity", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}"]


def lit(name, image, extra=(), tex_extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *tex_extra, "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\trgbGen identity", "\t}", "}", ""]


def tinted(name, image, rgb, extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            f"\t\trgbGen const ( {rgb[0]} {rgb[1]} {rgb[2]} )", "\t}", "\t{", "\t\tmap $lightmap",
            "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}", "}", ""]


def glow(name, image, rgb, light_value):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", f"\tq3map_surfacelight {light_value}", f"\tq3map_lightRGB {rgb[0]} {rgb[1]} {rgb[2]}",
            "\t{", f"\t\tmap {image}", f"\t\trgbGen const ( {rgb[0]} {rgb[1]} {rgb[2]} )", "\t}", "}", ""]


def decal(name, image, extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", "\tsurfaceparm nonsolid",
            "\tsurfaceparm nomarks", "\tsurfaceparm trans", "\tpolygonOffset", *extra, "\t{", f"\t\tmap {image}",
            "\t\tblendFunc blend", "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap",
            "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}", "}", ""]


def write_shader():
    out = ["// Jungle Opening (q3r_david4) - CTF in a jungle clearing at night.",
           "// Written by tools/maptools/make_david4.py. Compile with -custinfoparms.", ""]
    out += [f"textures/{TEX}sky", "{", "\tqer_editorimage textures/q3r_skies/env/ame_nebula/purplenebula_ft.tga",
            "\tsurfaceparm noimpact", "\tsurfaceparm nolightmap", "\tsurfaceparm sky", "\tq3map_globaltexture",
            "\t// moonlight from the nebula, tilted for real-time shadows",
            "\tq3map_sun 0.78 0.82 1.00 150 68 55", "\tq3map_skylight 95 3",
            "\tq3gl2_sun 0.78 0.82 1.00 70 68 55 0.5",
            "\tskyparms textures/q3r_skies/env/ame_nebula/purplenebula - -",
            "\tq3r_haze 0.09 0.11 0.14 9000\t// Q3Rally: night mist over the clearing",
            "\tq3r_colorLut gfx/luts/night\t// Q3Rally: colour grading", "}", ""]
    out += [f"textures/{TEX}forest", "{", "\tqer_editorimage textures/q3r_trees/pinewall01a.tga",
            "\tsurfaceparm nonsolid", "\tsurfaceparm nolightmap", "\tsurfaceparm nomarks",
            "\t{", "\t\tmap textures/q3r_trees/pinewall01a.tga", "\t\trgbGen const ( 0.30 0.34 0.36 )", "\t}", "}", ""]
    out += [f"textures/{TEX}backdrop", "{", "\tqer_editorimage textures/q3r_dirt_grass/grass01.tga",
            "\tq3map_lightmapSampleSize 64", "\tq3map_shadeAngle 120", "\tq3map_nonplanar",
            "\tq3map_lightmapAxis z", "\tsurfaceparm SURF_GRASS", "\t{", "\t\tmap textures/q3r_dirt_grass/grass01.tga",
            "\t\ttcGen vector ( 0.0029296875 0 0 ) ( 0 0.0029296875 0 )", "\t\trgbGen identity", "\t}"] + tail() + ["}", ""]
    out += lit("ramp_red", "textures/q3r_metals/Rost.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("ramp_blue", "textures/q3r_metals/rustblumet01.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("deck_red", "textures/q3r_metals/Rost.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("deck_blue", "textures/q3r_metals/rustblumet01.tga", ("\tsurfaceparm SURF_METAL",))
    out += tinted("ramp_stone", "textures/q3r_stone/Stein2.tga", (0.80, 0.78, 0.74), ("\tq3map_shadeAngle 50",))
    out += lit("trim", "textures/base_trim/yellow_rustb.tga")
    for team, img, rgb in (("red", "textures/ctf/red_telep.jpg", (1.0, 0.35, 0.2)),
                           ("blue", "textures/ctf/blue_telep.jpg", (0.3, 0.5, 1.0))):
        out += [f"textures/{TEX}pad_{team}", "{", f"\tqer_editorimage {img}", "\tsurfaceparm nomarks",
                "\tq3map_surfacelight 120", f"\tq3map_lightRGB {rgb[0]} {rgb[1]} {rgb[2]}",
                "\t{", f"\t\tclampMap {img}", "\t\ttcMod rotate 40", "\t\trgbGen identity", "\t}",
                "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_SRC_COLOR", "\t\trgbGen identity", "\t}", "}", ""]
        out += glow(f"glow_{team}", "textures/q3r_signage/white.jpg", rgb, 600)
        out += decal(f"border_{team}", f"textures/q3r_signage/dirsign01_{team[:3] if team == 'blue' else team}.tga")
        out += tinted(f"paint_{team}", "textures/q3r_building/wellblech.jpg",
                      (0.85, 0.22, 0.16) if team == "red" else (0.22, 0.36, 0.85), ("\tsurfaceparm SURF_METAL",))
        out += lit(f"sign_{team}", "textures/ctf/" + ("test2_r.tga" if team == "red" else "test2.tga"))
    out += lit("stone", "textures/q3r_stone/Stein2.tga", ("\tq3map_shadeAngle 40",))
    out += tinted("slab", "textures/q3r_stone/Stein2.tga", (0.55, 0.55, 0.52), ("\tq3map_shadeAngle 60",))
    out += lit("rock", "textures/q3r_dirt_grass/rock01.tga", ("\tq3map_shadeAngle 70",))
    out += tinted("concrete", "textures/q3r_building/concrete02.jpg", (0.52, 0.52, 0.50))
    out += lit("roof", "textures/q3r_metals/cormetal03.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("metal", "textures/q3r_metals/metrust01.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("pipe", "textures/base_trim/rusty_pewter_big.tga", ("\tsurfaceparm SURF_METAL", "\tq3map_shadeAngle 60"))
    out += lit("bark", "models/mapobjects/oak/oakstamm.jpg")
    out += [f"textures/{TEX}struts", "{", "\tqer_editorimage textures/q3r_metals/towerstruts.tga",
            "\tsurfaceparm nonsolid", "\tsurfaceparm trans", "\tsurfaceparm alphashadow", "\tcull none",
            "\t{", "\t\tmap textures/q3r_metals/towerstruts.tga", "\t\talphaFunc GE128", "\t\tdepthWrite",
            "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\tdepthFunc equal", "\t\trgbGen identity", "\t}", "}", ""]
    out += [f"textures/{TEX}ladder", "{", "\tqer_editorimage textures/q3r_metals/ladder01.tga",
            "\tsurfaceparm nonsolid", "\tsurfaceparm trans", "\tcull none",
            "\t{", "\t\tmap textures/q3r_metals/ladder01.tga", "\t\talphaFunc GE128", "\t\tdepthWrite",
            "\t\trgbGen identity", "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\tdepthFunc equal", "\t\trgbGen identity", "\t}", "}", ""]
    out += glow("lamp", "textures/q3r_signage/white.jpg", (1.0, 0.9, 0.72), 400)
    out += [f"textures/{TEX}dark", "{", "\tqer_editorimage textures/q3r_signage/white.jpg", "\tsurfaceparm nolightmap",
            "\tsurfaceparm nonsolid", "\t{", "\t\tmap textures/q3r_signage/white.jpg", "\t\trgbGen const ( 0.02 0.02 0.02 )",
            "\t}", "}", ""]
    out += [f"textures/{TEX}water", "{", "\tqer_editorimage textures/q3r_liquids/water.tga", "\tqer_trans 0.35",
            "\t// shallow: drawn as water, but no liquid contents (bots treat the bed as ground)",
            "\tsurfaceparm nonsolid", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", "\tq3map_globaltexture", "\tcull disable",
            "\tq3r_water textures/q3r_liquids/water_normal.tga 240 0.025 0.022", "\tq3r_waterColor 0.05 0.09 0.08 0.40",
            "\tq3r_waterFresnel 0.06 4", "\tq3r_waterSpecular 0.9",
            "\t{", "\t\tmap textures/q3r_liquids/water.tga", "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA",
            "\t\trgbGen const ( 0.30 0.42 0.42 )", "\t\talphaGen const 0.40", "\t\ttcMod scale 0.6 0.6",
            "\t\ttcMod scroll 0.0 -0.03", "\t}",
            "\t{", "\t\tmap textures/q3r_liquids/water.tga", "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA",
            "\t\trgbGen const ( 0.55 0.62 0.70 )", "\t\talphaGen const 0.12", "\t\ttcMod scale 0.7 0.7",
            "\t\ttcMod scroll 0.01 -0.045", "\t}", "}", ""]
    out += [f"textures/{TEX}waterfall", "{", "\tqer_editorimage textures/q3r_liquids/waterfall_body.jpg",
            "\tqer_trans 0.7", "\tsurfaceparm nonsolid", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", "\tcull disable",
            "\t{", "\t\tmap textures/q3r_liquids/waterfall_body.jpg", "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA",
            "\t\trgbGen const ( 0.30 0.36 0.40 )", "\t\talphaGen const 0.75", "\t\ttcMod turb 0 0.012 0 0.6",
            "\t\ttcMod scroll 0 -1.1", "\t}",
            "\t{", "\t\tmap textures/q3r_liquids/waterfall_streaks.tga", "\t\tblendFunc GL_SRC_ALPHA GL_ONE",
            "\t\trgbGen const ( 0.35 0.42 0.50 )", "\t\ttcMod scroll 0 -2.4", "\t}",
            "\t{", "\t\tmap textures/q3r_liquids/waterfall_streaks.tga", "\t\tblendFunc GL_SRC_ALPHA GL_ONE",
            "\t\trgbGen const ( 0.22 0.28 0.34 )", "\t\ttcMod scale 1.3 0.8", "\t\ttcMod scroll 0.06 -1.7", "\t}",
            "}", ""]
    out += [f"textures/{TEX}boundary", "{", "\tqer_editorimage textures/base_floor/concrete.jpg", "\tqer_trans 0.15",
            "\t// invisible but SOLID (Bullet only takes solid brushes); trans lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm noimpact", "\tsurfaceparm detail", "}", ""]
    out += ["// ground: t_* parents on the brush sides (collision, friction); q3map2 draws",
            f"// ter_<layer> / ter_<a>to<b> from maps/{NAME}_index.pcx. Layers: " +
            ", ".join(f"{i} {n}" for i, (n, _, _) in enumerate(LAYERS)), ""]
    for name, parm, layer in PARENTS:
        out += [f"textures/{TEX}t_{name}", "{", f"\tqer_editorimage {LAYERS[layer][1]}", "\tq3map_indexed",
                "\tq3map_shadeAngle 120", "\tq3map_nonplanar", "\tq3map_lightmapAxis z"] + \
               ([f"\t{parm}"] if parm else []) + ["}", ""]
    for i in range(len(LAYERS)):
        out += [f"textures/{TEX}ter_{i}", "{", f"\tqer_editorimage {LAYERS[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_nonplanar", "\tq3map_lightmapAxis z"] + stage(i, False) + tail() + ["}", ""]
    for i, j in combinations(range(len(LAYERS)), 2):
        out += [f"textures/{TEX}ter_{i}to{j}", "{", f"\tqer_editorimage {LAYERS[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_nonplanar", "\tq3map_lightmapAxis z"] + stage(i, False) + stage(j, True) + tail() + ["}", ""]
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.shader").write_text("\n".join(out), encoding="utf-8")


def write_textures():
    from PIL import Image, ImageFilter
    out = ROOT / "baseq3r" / "textures" / "q3r_david4"
    out.mkdir(parents=True, exist_ok=True)
    rnd = random.Random(44)
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
        data = big.getdata() if not hasattr(big, "get_flattened_data") else big.get_flattened_data()
        acc = [a + weight * v for a, v in zip(acc, data)]
    lo, hi = min(acc), max(acc)
    m = Image.new("L", (size, size))
    m.putdata([int(128 + ((v - lo) / (hi - lo) - 0.5) * 80) for v in acc])
    m.filter(ImageFilter.GaussianBlur(1.2)).convert("RGB").save(out / "macro.jpg", quality=92)


def index_map():
    from PIL import Image
    w, h = (2 * BX) // STEP + 1, (2 * BY) // STEP + 1
    im = Image.new("L", (w, h))
    im.putdata([MAT[-BX + i * STEP, BY - j * STEP] for j in range(h) for i in range(w)])
    im.save(ROOT / "baseq3r" / "maps" / f"{NAME}_index.pcx")


# ---------------------------------------------------------------- checks and output

def blocked(x, y, z, pad=40, tags=None):
    for lo, hi, tag in SOLIDS:
        if tags and tag not in tags:
            continue
        if lo[0] - pad < x < hi[0] + pad and lo[1] - pad < y < hi[1] + pad and lo[2] < z + 30 and hi[2] > z - 10:
            return tag or "solid"
    return None


def surface_z(x, y):
    """Drivable height including the mound."""
    z = ground(x, y)
    for team, sx in TEAMS:
        cx = sx * FLAG_X
        dx, dy = x - cx, y
        ap = max(abs(dx * math.cos(k * math.pi / 4) + dy * math.sin(k * math.pi / 4)) for k in range(8))
        rings = MESA_RINGS
        if ap <= rings[-1][0]:
            z = max(z, rings[-1][1] + (2 if ap <= PAD else 0))
        else:
            for (ro, zo), (ri, zi) in zip(rings, rings[1:]):
                if ri <= ap <= ro:
                    z = max(z, zo + (zi - zo) * (ro - ap) / (ro - ri))
    return z


def flag_access():
    """For each flag: 16 straight approaches from 1300 units out. Reports the
    steepest grade on each line and whether any solid brush is in the way."""
    report = {}
    for team, sx in TEAMS:
        cx = sx * FLAG_X
        lines = []
        for k in range(64):
            a = 2 * math.pi * k / 64
            worst, hit = 0.0, None
            prev = None
            for i in range(0, 91):
                d = 900 - i * 10
                x, y = cx + d * math.cos(a), d * math.sin(a)
                if abs(x) > PX or abs(y) > PY:
                    prev = None
                    continue
                z = surface_z(x, y)
                if prev is not None:
                    worst = max(worst, abs(z - prev) / 10)
                prev = z
                for lat in (-36, 0, 36):
                    b = blocked(x - lat * math.sin(a), y + lat * math.cos(a), z, pad=10)
                    if b and b not in ("mesa", "pad"):
                        hit = hit or b
            lines.append({"deg": round(math.degrees(a)), "max_grade_pct": round(100 * worst, 1),
                          "blocked_by": hit})
        report[team] = lines
    return report


def validate():
    starts = [e for e in ENTITIES if e["classname"].startswith(("team_CTF_", "info_player_deathmatch"))
              and "flag" not in e["classname"]]
    for e in starts:
        x, y, z = map(float, e["origin"].split())
        b = blocked(x, y, z - 40, pad=70)
        assert not b, (e, b)
        assert abs(x) < PX - 60 and abs(y) < PY - 60, e
    items = [e for e in ENTITIES if e["classname"].startswith(("weapon_", "ammo_", "item_", "rally_"))]
    bad = []
    for e in items:
        x, y, z = map(float, e["origin"].split())
        b = blocked(x, y, z - 24, pad=20, tags=("tree", "boulder", "gate", "outpost", "mast", "post", "valve", "pipe"))
        if b:
            bad.append((e["classname"], e["origin"], b))
    assert not bad, bad
    grades = []
    for x in range(-PX, PX + 1, 32):
        for y in range(-PY, PY + 1, 32):
            g = max(abs(surface_z(x + 16, y) - surface_z(x - 16, y)), abs(surface_z(x, y + 16) - surface_z(x, y - 16))) / 32
            grades.append(g)
    access = flag_access()
    worst_line = max(l["max_grade_pct"] for t in access.values() for l in t)
    blocked_lines = [(t, l["deg"], l["blocked_by"]) for t, ls in access.items() for l in ls if l["blocked_by"]]
    reds = [e for e in ENTITIES if e["classname"] == "team_CTF_redflag"]
    blues = [e for e in ENTITIES if e["classname"] == "team_CTF_blueflag"]
    rx = float(reds[0]["origin"].split()[0])
    bx = float(blues[0]["origin"].split()[0])
    count = {}
    for e in ENTITIES:
        count[e["classname"]] = count.get(e["classname"], 0) + 1
    return {"map": NAME, "flag_distance": round(bx - rx), "terrain_triangles": len(TRIS),
            "leaves": {s: sum(1 for leaf in LEAVES if leaf[2] == s) for s in (64, 128, 256, 512)},
            "brushes": len(BRUSHES) + sum(len(g) for g in GROUPS.values()),
            "max_play_grade_pct": round(100 * max(grades), 1),
            "flag_approach_max_grade_pct": worst_line, "flag_approaches_blocked": blocked_lines,
            "flag_access": access, "entities": count, **INFO}


def overview(report):
    from PIL import Image, ImageDraw
    scale = (2 * BX) / 1400
    w, h = int(2 * BX / scale), int(2 * BY / scale)
    im = Image.new("RGB", (w, h))
    pix = im.load()
    colors = {L_GRASS: (52, 96, 44), L_MOSS: (88, 98, 50), L_DIRT: (112, 84, 58), L_ROCK: (110, 96, 90),
              L_PEBBLE: (120, 120, 126)}
    zmin, zmax = min(GRID.values()), max(GRID.values())
    for v in range(h):
        y = BY - v * scale
        gy = -BY + round((y + BY) / STEP) * STEP
        for u in range(w):
            x = -BX + u * scale
            gx = -BX + round((x + BX) / STEP) * STEP
            z = GRID.get((gx, gy), 0)
            zx = GRID.get((gx + STEP, gy), z)
            zy = GRID.get((gx, gy + STEP), z)
            shade = 0.8 + 0.25 * (z - zmin) / (zmax - zmin) - (zx - z) / STEP * 0.6 + (zy - z) / STEP * 0.3
            c = colors[MAT.get((gx, gy), L_GRASS)]
            if creek(x, y) > 0 and abs(x) < creek_half(y) + BANK * 0.6 and z < WATER_Z + 1:
                c = (40, 70, 110)
            pix[u, v] = tuple(max(0, min(255, int(k * shade))) for k in c)
    d = ImageDraw.Draw(im)

    def sc(x, y):
        return (x + BX) / scale, (BY - y) / scale
    for lo, hi, tag in SOLIDS:
        a, b = sc(lo[0], hi[1]), sc(hi[0], lo[1])
        d.rectangle((a, b), outline=(230, 230, 230) if tag != "mesa" else (200, 160, 40))
    for e in ENTITIES:
        x, y, _ = map(float, e["origin"].split())
        u, vv = sc(x, y)
        c = e["classname"]
        if c == "misc_model":
            d.ellipse((u - 2, vv - 2, u + 2, vv + 2), fill=(20, 60, 20))
        elif "flag" in c:
            col = (255, 40, 40) if "red" in c else (60, 120, 255)
            d.ellipse((u - 7, vv - 7, u + 7, vv + 7), fill=col, outline=(255, 255, 255))
        elif c.startswith("team_CTF_"):
            col = (255, 90, 90) if "red" in c else (90, 150, 255)
            d.polygon([(u, vv - 5), (u - 4, vv + 4), (u + 4, vv + 4)], fill=col)
        elif c.startswith(("weapon_",)):
            d.rectangle((u - 4, vv - 4, u + 4, vv + 4), fill=(230, 40, 200))
        elif c.startswith(("item_", "rally_item", "ammo_")):
            d.rectangle((u - 3, vv - 3, u + 3, vv + 3), fill=(240, 190, 0))
        elif c == "light":
            d.point((u, vv), fill=(255, 255, 160))
    x0, y0 = sc(-PX - WALL, PY + WALL)
    x1, y1 = sc(PX + WALL, -PY - WALL)
    d.rectangle((x0, y0, x1, y1), outline=(255, 255, 255))
    out = ROOT / "docs" / "maps" / "david4"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "david4_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def write_map():
    fields = {"classname": "worldspawn", "message": "Jungle Opening", "_ambient": "20", "_minlight": "14",
              "gridsize": "128 128 256", "_blocksize": "1024 1024 4096",
              "reflectionImage": "/textures/reflect/david4.jpg", "weather": "none"}
    text = ["// Generated by tools/maptools/make_david4.py.",
            f"// The terrain func_groups use maps/{NAME}_index.pcx at compile time.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text += GROUPS.get("world", [])
    text.append("}")
    for group in ("grass", "dirt", "gravel", "plain"):
        keys = {"classname": "func_group", "_terrain": f"jungle clearing ground, {group} friction",
                "_indexmap": f"maps/{NAME}_index.pcx", "_layers": str(len(LAYERS)),
                "_shader": TEX + "ter", "min": f"{-BX} {-BY} {FLOOR}", "max": f"{BX} {BY} 4096"}
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()))
        text += GROUPS.get(group, [])
        text.append("}")
    for ent in ENTITIES:
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in ent.items()) + "\n}")
    target = ROOT / "baseq3r" / "maps" / f"{NAME}.map"
    target.write_text("\n".join(text) + "\n", encoding="ascii")
    arena = ROOT / "baseq3r" / "scripts" / f"{NAME}.arena"
    arena.write_text(
        "{\n"
        "//MAP FILE NAME\n"
        f'map "{NAME}"\n\n'
        "//IN-GAME MAP NAME\n"
        'longname "Jungle Opening"\n\n'
        "//AUTHOR OF THE MAP\n"
        'author "|CE & P3rlE"\n\n'
        "//GAME TYPE SUPPORTS\n"
        'type "q3r_ctf"\n\n'
        "//CUSTOM BOTS\n"
        'bots "Sam Dean Minori Vlad Kuro Emma Bee"\n\n'
        "//PLAYER STARTS\n"
        'starts "16"\n\n'
        "//LAPTIME\n"
        'laptime ""\n\n'
        "//CHECKPOINTS\n"
        'checkpoints ""\n\n'
        "//OBSERVER SPOTS\n"
        'observerspots "5"\n\n'
        "//WEAPONS\n"
        'weapons "BFG,GL,FT,RL,LG"\n\n'
        "//POWERUPS\n"
        'powerups "2"\n\n'
        "//REVERSABLE\n"
        'reversable ""\n\n'
        "//TRACK LENGTHS\n"
        'tracklengths ""\n\n'
        "//BOT SUPPORTS\n"
        'botsupport "Good"\n\n'
        "//TEAMS\n"
        'teams "2"\n'
        "}\n", encoding="ascii")
    return target


def main():
    build_tracks()
    build_terrain()
    for team, sx in TEAMS:
        mesa(team, sx)
        border(team, sx)
        outpost(team, sx)
        masts(team, sx)
        spawns(team, sx)
        base_items(team, sx)
        ruin_gate(sx)
    rock_groups()
    quad_slab()
    grove()
    water_tower()
    cascade()
    culvert()
    water()
    middle_items()
    lanterns()
    forest_edge()
    backdrop()
    boundary()
    shell()
    misc_entities()
    report = validate()
    write_textures()
    write_shader()
    index_map()
    target = write_map()
    overview(report)
    print(json.dumps({k: v for k, v in report.items() if k != "flag_access"}, indent=2))
    print("Wrote", target)


if __name__ == "__main__":
    main()
