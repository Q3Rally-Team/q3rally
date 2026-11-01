#!/usr/bin/env python3
"""Generate q3r_quarry (Quarry King): King of the Hill in a limestone quarry.

The hill is the floor of a terraced quarry pit. Around it three benches of
pale limestone step up to the rim, each face 160 units high and too steep to
drive up. Haul roads cut into the faces lead from bench to bench; anyone can
drop down a face, but the way back up is the ramps. So the defenders sit low,
the attackers come over the edge, and every fall into the pit has to be
driven out again.

Red spawns on the east rim, blue on the west rim. Everything that matters for
play (ramps, benches, cover, pickups) is point symmetric (rotated 180 degrees
about the centre), so both teams have the same ways in and out. Around the
pit: excavators and dump trucks on the benches, conveyors from the pit floor up
to two crushing plants beyond the rim, stacks of cut blocks as cover, a berm
with an invisible wall at the edge of the site, meadows and trees beyond.

Ground: one height field, collision as closed triangular detail brushes on a
64/128-unit grid, layers blended with a q3map2 index map; tyre friction from
the parent shaders. Writes the .map, index map, shader script, arena, the
generated textures, docs/maps/quarry/quarry_layout.png and validation.json.
Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from itertools import combinations
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_quarry"
TEX = "q3r_quarry/"
DETAIL = 134217728
STEP = 64
FLOOR = -1024
HALF = 3392                      # half size of the detailed square
OUTER = (HALF, 4400, 5600, 7168)  # rings of hills beyond it, out to the sky walls

# the pit: rounded rectangle floor, benches by distance from its edge
FX, FY, RC = 1100, 1000, 350
LEVELS = (-480, -320, -160, 0)   # floor, bench 1, bench 2, rim
BENCH = 420                      # distance from one face to the next
FACE = 72                        # horizontal run of a face (160 up: about 66 degrees)
RIM_D = 2 * BENCH + FACE         # where the rim plateau starts
EDGE_D = 1460                    # invisible wall around the site
HILL_HX, HILL_HY = 560, 420      # half size of the King of the Hill zone
RAMP_W = 260

# haul roads: (side, face number, along from, along to). The road climbs from
# the lower level at "from" to the upper one at "to". Each one is also built
# turned by 180 degrees (N <-> S, E <-> W) with the same numbers.
RAMPS = (("N", 0, 680, -260), ("E", 1, -600, 330), ("N", 2, -700, 230))
PILES = ((900, 760, 200, 100),)     # x, y, radius, height (and turned copies)

LAYERS = ("grass", "dust", "gravel", "rock", "facex", "facey")
L_GRASS, L_DUST, L_GRAVEL, L_ROCK, L_FACEX, L_FACEY = range(6)
FRICTION = {L_GRASS: "grass", L_DUST: "dirt", L_GRAVEL: "gravel", L_ROCK: "plain", L_FACEX: "plain",
            L_FACEY: "plain"}

GRID, MAT, LEAVES, LEAFOF, TRIS = {}, {}, [], {}, []
BRUSHES, GROUPS, ENTITIES, SOLIDS, TRIGGERS = [], {}, [], [], []
BOUNDS = {"x0": -HALF, "x1": HALF, "y0": -HALF, "y1": HALF}
REPORT = {}


def smooth(a, b, x):
    t = min(1.0, max(0.0, (x - a) / (b - a)))
    return t * t * (3 - 2 * t)


def pit_d(x, y):
    """Signed distance from the edge of the pit floor (+ outside)."""
    qx, qy = abs(x) - (FX - RC), abs(y) - (FY - RC)
    return math.hypot(max(qx, 0.0), max(qy, 0.0)) + min(max(qx, qy), 0.0) - RC


def noise(x, y):
    """Smooth -1..1, point symmetric like the rest of the play area."""
    return 0.5 * math.cos(x / 173) * math.cos(y / 211) + 0.3 * math.cos(x / 89 - y / 97) + 0.2 * math.cos((x + y) / 61)


def ramp_frame(side, x, y):
    """(along, across) in the frame of one side; across = distance from the floor edge."""
    if side == "N":
        return x, y - FY
    if side == "S":
        return -x, -y - FY
    if side == "E":
        return y, x - FX
    return -y, -x - FX                         # W


TURNED = {"N": "S", "S": "N", "E": "W", "W": "E"}


def all_ramps():
    for side, k, a, b in RAMPS:
        yield side, k, a, b
        yield TURNED[side], k, a, b


def ramp_at(x, y):
    """Height of a haul road here, or None."""
    for side, k, a, b in all_ramps():
        along, across = ramp_frame(side, x, y)
        centre = k * BENCH + FACE / 2
        if abs(across - centre) > RAMP_W / 2 or not (min(a, b) - 1 <= along <= max(a, b) + 1):
            continue
        t = (along - a) / (b - a)
        return LEVELS[k] + (LEVELS[k + 1] - LEVELS[k]) * min(1.0, max(0.0, t))
    return None


def on_ramp(x, y, pad=0):
    for side, k, a, b in all_ramps():
        along, across = ramp_frame(side, x, y)
        centre = k * BENCH + FACE / 2
        if abs(across - centre) <= RAMP_W / 2 + pad and min(a, b) - pad <= along <= max(a, b) + pad:
            return True
    return False


def piles():
    for x, y, r, h in PILES:
        yield x, y, r, h
        yield -x, -y, r, h


def height(x, y):
    d = pit_d(x, y)
    rough = noise(x, y)
    # faces: a little ragged where they run, flat benches between
    dd = d + 22 * rough
    z = LEVELS[0]
    for k in range(3):
        z += (LEVELS[k + 1] - LEVELS[k]) * smooth(k * BENCH, k * BENCH + FACE, dd)
    # the floor and benches are not perfectly flat, except under the hill zone
    flat_zone = abs(x) < HILL_HX + 140 and abs(y) < HILL_HY + 140
    if d < RIM_D and not flat_zone:
        z += 3 * rough
    # rim plateau, berm at the edge of the site, meadows rising beyond
    z += 70 * smooth(EDGE_D - 60, EDGE_D + 120, d)
    z += (240 + 80 * math.cos(x / 700) * math.cos(y / 640)) * smooth(EDGE_D + 300, EDGE_D + 1700, d)
    if d > RIM_D + 40:
        z += 6 * rough * smooth(RIM_D + 40, RIM_D + 200, d)
    for px, py, r, h in piles():
        rr = math.hypot(x - px, y - py)
        if rr < r:
            base = height_level(px, py)
            z = max(z, base + h * (1 - smooth(0, r, rr)) ** 1.3)
    ramp = ramp_at(x, y)
    if ramp is not None:
        z = ramp
    return z


def height_level(x, y):
    """Level of the bench a point is on (no ramps, no noise)."""
    d = pit_d(x, y)
    for k in range(3):
        if d < k * BENCH + FACE / 2:
            return LEVELS[k]
    return LEVELS[3]


def build_terrain():
    raw = {}
    for y in range(-HALF, HALF + 1, STEP):
        for x in range(-HALF, HALF + 1, STEP):
            raw[x, y] = height(x, y)
    GRID.update(raw)
    for (x, y), z in raw.items():
        gx = (raw.get((x + STEP, y), z) - raw.get((x - STEP, y), z)) / (2 * STEP)
        gy = (raw.get((x, y + STEP), z) - raw.get((x, y - STEP), z)) / (2 * STEP)
        slope = math.hypot(gx, gy)
        d = pit_d(x, y)
        patch = noise(x * 0.7, y * 0.7)
        if slope > 0.8:
            m = L_FACEX if abs(gx) > abs(gy) else L_FACEY
        elif on_ramp(x, y):
            m = L_DUST
        elif any(math.hypot(x - px, y - py) < r * 0.92 for px, py, r, _ in piles()):
            m = L_GRAVEL
        elif d < RIM_D:
            # rubble at the foot of each face, dust elsewhere, some bare rock
            if any(-70 < d - k * BENCH < 0 for k in range(3)):      # rubble at the foot of a face
                m = L_GRAVEL
            elif slope > 0.35:
                m = L_ROCK
            else:
                m = L_ROCK if patch > 0.62 else L_DUST
        elif d < EDGE_D - 80:
            m = L_DUST if (d < RIM_D + 260 or patch > 0.15) else L_GRASS
        else:
            m = L_ROCK if slope > 0.45 else L_GRASS
        MAT[x, y] = m
    for cy in range(-HALF, HALF, 128):
        for cx in range(-HALF, HALF, 128):
            pts = [(cx + i * STEP, cy + j * STEP) for i in range(3) for j in range(3)]
            a, b = raw[cx, cy], raw[cx + 128, cy]
            c, d = raw[cx + 128, cy + 128], raw[cx, cy + 128]
            hc = raw[cx + 64, cy + 64]
            diag = "ac" if abs((a + c) / 2 - hc) <= abs((b + d) / 2 - hc) else "bd"
            flat = max(abs(raw[p] - cell_height(cx, cy, 128, diag, *p)) for p in pts) < 0.75
            if flat and len({MAT[p] for p in pts}) == 1:
                LEAVES.append((cx, cy, 128))
            else:
                for fy in (cy, cy + 64):
                    for fx in (cx, cx + 64):
                        LEAVES.append((fx, fy, 64))
    for (lx, ly, size) in sorted(LEAVES, key=lambda c: -c[2]):
        if size == STEP:
            continue
        for (ax, ay), (bx, by) in (((lx, ly), (lx + size, ly)), ((lx + size, ly), (lx + size, ly + size)),
                                   ((lx, ly + size), (lx + size, ly + size)), ((lx, ly), (lx, ly + size))):
            GRID[(ax + bx) // 2, (ay + by) // 2] = (GRID[ax, ay] + GRID[bx, by]) / 2
    for k in GRID:
        GRID[k] = round(GRID[k] * 8) / 8
    order = ("plain", "grass", "gravel", "dirt")
    for (lx, ly, size) in LEAVES:
        a, b = (lx, ly, GRID[lx, ly]), (lx + size, ly, GRID[lx + size, ly])
        c, d = (lx + size, ly + size, GRID[lx + size, ly + size]), (lx, ly + size, GRID[lx, ly + size])
        hc = height(lx + size / 2, ly + size / 2)
        ac, bd = abs((a[2] + c[2]) / 2 - hc), abs((b[2] + d[2]) / 2 - hc)
        if abs(ac - bd) < 0.05:
            diag = "ac" if (lx + size / 2) * (ly + size / 2) >= 0 else "bd"
        else:
            diag = "ac" if ac < bd else "bd"
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


def ground_min(x, y, r):
    return min(ground(x + dx, y + dy) for dx in (-r, 0, r) for dy in (-r, 0, r))


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
        if corners:
            SOLIDS.append((tuple(min(c[i] for c in corners) for i in range(3)),
                           tuple(max(c[i] for c in corners) for i in range(3))))


def obox(cx, cy, t, along, across, z0, z1, mat, side=None, top=None, solid=True, scale=0.5):
    """Box turned to the direction t (unit), centred on cx, cy."""
    n = (-t[1], t[0])
    side = side or mat
    convex([((t[0], t[1], 0), (cx + t[0] * along, cy + t[1] * along, z0), side),
            ((-t[0], -t[1], 0), (cx - t[0] * along, cy - t[1] * along, z0), side),
            ((n[0], n[1], 0), (cx + n[0] * across, cy + n[1] * across, z0), mat),
            ((-n[0], -n[1], 0), (cx - n[0] * across, cy - n[1] * across, z0), mat),
            ((0, 0, 1), (cx, cy, z1), top or side), ((0, 0, -1), (cx, cy, z0), side)],
           solid=solid, scale=scale)


def hexa(corners, mat, solid=True, scale=0.5):
    """Convex 8-corner block: corners 0-3 one end (ring order), 4-7 the other."""
    c = [sum(p[i] for p in corners) / 8 for i in range(3)]
    quads = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
    planes = []
    for k, q in enumerate(quads):
        p0, p1, p2 = corners[q[0]], corners[q[1]], corners[q[2]]
        n = cross(tuple(b - a for a, b in zip(p0, p1)), tuple(b - a for a, b in zip(p0, p2)))
        if sum(nc * (pc - cc) for nc, pc, cc in zip(n, p0, c)) < 0:
            n = tuple(-v for v in n)
        m = mat.get(k, mat.get("default")) if isinstance(mat, dict) else mat
        planes.append((n, p0, m))
    convex(planes, solid=solid, scale=scale)


def beam(p0, p1, w, h, mat, solid=True, up=(0.0, 0.0, 1.0), scale=0.5):
    """Square-ish bar from p0 to p1, w wide and h high (centred on the line)."""
    d = [b - a for a, b in zip(p0, p1)]
    L = math.sqrt(sum(v * v for v in d))
    d = [v / L for v in d]
    side = cross(d, up)
    sl = math.sqrt(sum(v * v for v in side))
    if sl < 1e-6:                               # vertical bar
        side, sl = cross(d, (1.0, 0.0, 0.0)), 1.0
        sl = math.sqrt(sum(v * v for v in side))
    side = [v / sl for v in side]
    upv = cross(side, d)
    corners = []
    for p in (p0, p1):
        for s, u in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
            corners.append(tuple(p[i] + side[i] * s * w / 2 + upv[i] * u * h / 2 for i in range(3)))
    hexa(corners, mat, solid=solid, scale=scale)


class Frame:
    """Local frame of a placed object: x forward, y left, z up from base_z."""

    def __init__(self, x, y, yaw, base_z, size=1.0):
        self.x, self.y, self.z, self.k = x, y, base_z, size
        self.c, self.s = math.cos(math.radians(yaw)), math.sin(math.radians(yaw))

    def p(self, lx, ly, lz):
        lx, ly, lz = lx * self.k, ly * self.k, lz * self.k
        return (self.x + lx * self.c - ly * self.s, self.y + lx * self.s + ly * self.c, self.z + lz)

    def box(self, x0, x1, y0, y1, z0, z1, mat, solid=True, scale=0.5):
        cx, cy, _ = self.p((x0 + x1) / 2, (y0 + y1) / 2, 0)
        k = self.k
        obox(cx, cy, (self.c, self.s), (x1 - x0) / 2 * k, (y1 - y0) / 2 * k, self.z + z0 * k, self.z + z1 * k, mat,
             solid=solid, scale=scale)

    def beam(self, a, b, w, h, mat, solid=True):
        beam(self.p(*a), self.p(*b), w * self.k, h * self.k, mat, solid=solid)

    def wheel(self, lx, ly, lz, r, w, mat):
        """Wheel turning about the local y axis."""
        r, w = r * self.k, w * self.k
        cx, cy, cz = self.p(lx, ly, lz)
        ax = (-self.s, self.c, 0.0)               # axle direction
        fw = (self.c, self.s, 0.0)
        planes = [(ax, tuple(c + a * w / 2 for c, a in zip((cx, cy, cz), ax)), mat),
                  (tuple(-a for a in ax), tuple(c - a * w / 2 for c, a in zip((cx, cy, cz), ax)), mat)]
        for k in range(8):
            a = 2 * math.pi * (k + 0.5) / 8
            n = (fw[0] * math.cos(a), fw[1] * math.cos(a), math.sin(a))
            planes.append((n, (cx + n[0] * r, cy + n[1] * r, cz + n[2] * r), mat))
        convex(planes)


def entity(classname, x, y, z, **keys):
    fields = {"classname": classname, "origin": f"{round(x)} {round(y)} {round(z)}", **keys}
    ENTITIES.append(fields)
    return fields


def both(fn, x, y, yaw, *args, **kw):
    """Place a play-area object and its copy turned by 180 degrees."""
    fn(x, y, yaw, *args, **kw)
    fn(-x, -y, (yaw + 180) % 360, *args, **kw)


# ---------------------------------------------------------------- objects

def block_stack(x, y, yaw, rows=((0, 0), (1, 0), (0, 1)), size=(112, 64, 64)):
    """Stack of sawn limestone blocks: rows of (column, level) in the local frame."""
    bx, by, bz = size
    z0 = ground_min(x, y, 90)
    f = Frame(x, y, yaw, z0 - 4)
    for col, lev in rows:
        lx = (col - 0.5) * (bx + 6)
        f.box(lx - bx / 2, lx + bx / 2, -by / 2, by / 2, lev * bz + (2 if lev else 0), (lev + 1) * bz, TEX + "block",
              scale=0.25)


def excavator(x, y, yaw):
    """Tracked excavator facing the face it digs (local +x), arm lowered."""
    z = ground_min(x, y, 90)
    f = Frame(x, y, yaw, z, 0.62)
    for side in (-1, 1):                                   # tracks
        f.box(-150, 150, side * 66 - 34, side * 66 + 34, 0, 56, TEX + "rubber")
    f.box(-70, 70, -60, 60, 56, 76, TEX + "steel")          # slewing ring
    f.box(-150, 80, -96, 96, 76, 176, TEX + "paint")        # house
    f.box(-200, -130, -90, 90, 76, 190, TEX + "paint")     # counterweight
    f.box(10, 92, 30, 96, 176, 270, TEX + "paint")          # cab
    f.box(88, 94, 36, 90, 190, 262, TEX + "glass", solid=False)
    f.box(-20, 40, -60, 20, 176, 196, TEX + "steel")         # engine hood
    f.beam((60, -8, 150), (250, -8, 330), 30, 40, TEX + "paint")    # boom
    f.beam((250, -8, 330), (330, -8, 110), 24, 30, TEX + "paint")   # stick
    f.box(300, 372, -40, 24, 40, 100, TEX + "steel")        # bucket
    f.beam((100, -8, 180), (220, -8, 300), 10, 10, TEX + "steel")   # hydraulic ram


def dump_truck(x, y, yaw):
    """Rigid dump truck, bed up a little, nose towards local +x."""
    z = ground_min(x, y, 100)
    f = Frame(x, y, yaw, z, 0.62)
    for lx in (120, -110, -175):
        for side in (-1, 1):
            f.wheel(lx, side * 92, 52, 54, 46, TEX + "rubber")
    f.box(-220, 190, -60, 60, 40, 80, TEX + "steel")         # frame
    f.box(100, 200, -84, 84, 80, 170, TEX + "paint")        # engine and front
    f.box(100, 170, 20, 84, 170, 240, TEX + "paint")        # cab
    f.box(166, 172, 26, 78, 182, 232, TEX + "glass", solid=False)
    f.box(204, 210, -70, 70, 90, 150, TEX + "steel")         # grille
    bed = [f.p(-250, -100, 84), f.p(80, -100, 96), f.p(80, 100, 96), f.p(-250, 100, 84),
           f.p(-250, -110, 190), f.p(110, -110, 220), f.p(110, 110, 220), f.p(-250, 110, 190)]
    # corners 0-3 bottom ring, 4-7 top ring: reorder into two ends of a hexa
    hexa([bed[0], bed[3], bed[7], bed[4], bed[1], bed[2], bed[6], bed[5]], TEX + "paint")


CONVEYORS = []


def conveyor(p0, p1, spacing=420):
    """Belt conveyor on a lattice of legs from p0 (low, hopper) to p1 (high)."""
    d = [b - a for a, b in zip(p0, p1)]
    L = math.sqrt(sum(v * v for v in d))
    u = [v / L for v in d]
    side = cross(u, (0, 0, 1))
    sl = math.hypot(side[0], side[1])
    side = (side[0] / sl, side[1] / sl, 0.0)
    CONVEYORS.append((p0, p1))
    # the long inclined parts have huge bounding boxes: validate() checks them as lines
    for s in (-1, 1):                               # stringers
        a = tuple(p0[i] + side[i] * s * 46 for i in range(3))
        b = tuple(p1[i] + side[i] * s * 46 for i in range(3))
        beam(a, b, 10, 30, TEX + "steelrust", solid=False)
    beam(tuple(p0[i] + (0, 0, 8)[i] for i in range(3)), tuple(p1[i] + (0, 0, 8)[i] for i in range(3)),
         80, 8, TEX + "belt", solid=False)
    beam(tuple(p0[i] + (0, 0, 52)[i] for i in range(3)), tuple(p1[i] + (0, 0, 52)[i] for i in range(3)),
         96, 4, TEX + "steelrust", solid=False)     # rain cover
    n = int(L // spacing)
    for k in range(1, n + 1):
        t = k / (n + 1)
        c = tuple(p0[i] + d[i] * t for i in range(3))
        g = min(ground(c[0] + side[0] * s * 46, c[1] + side[1] * s * 46) for s in (-1, 1))
        if c[2] - g < 40:
            continue
        for s in (-1, 1):
            x, y = c[0] + side[0] * s * 46, c[1] + side[1] * s * 46
            beam((x, y, g - 16), (x, y, c[2] - 10), 14, 14, TEX + "steelrust")
        beam((c[0] - side[0] * 46, c[1] - side[1] * 46, c[2] - 20),
             (c[0] + side[0] * 46, c[1] + side[1] * 46, c[2] - 20), 10, 10, TEX + "steelrust")
    # hopper at the low end: a funnel on four legs
    hx, hy, hz = p0
    g = ground_min(hx, hy, 80)
    for sx in (-1, 1):
        for sy in (-1, 1):
            beam((hx + sx * 60, hy + sy * 60, g - 10), (hx + sx * 60, hy + sy * 60, hz + 20), 14, 14,
                 TEX + "steelrust")
    obox(hx, hy, (u[0] / math.hypot(u[0], u[1]), u[1] / math.hypot(u[0], u[1])), 90, 90, hz + 20, hz + 110,
         TEX + "paint", top="common/caulk")


def crusher_plant(x, y, yaw):
    """Crushing and screening plant beyond the rim (backdrop)."""
    g = ground_min(x, y, 260) - 30
    f = Frame(x, y, yaw, g)
    f.box(-260, 260, -200, 200, 0, 60, TEX + "concrete")
    for sx in (-1, 1):
        for sy in (-1, 1):
            f.beam((sx * 220, sy * 160, 60), (sx * 220, sy * 160, 420), 24, 24, TEX + "steelrust")
    f.box(-240, 240, -180, 180, 420, 720, TEX + "corrugated")
    f.box(-250, 250, -190, 190, 720, 740, TEX + "steel")
    f.box(-120, 120, -100, 100, 300, 420, TEX + "paint")      # crusher
    f.box(-80, 80, -80, 80, 740, 860, TEX + "corrugated")    # screen house
    for k, lx in enumerate((-440, -640)):                     # silos
        cx, cy, _ = f.p(lx, 40 * (-1) ** k, 0)
        octagon(cx, cy, 110, g, g + 520 + 60 * k, TEX + "silo")
        octagon(cx, cy, 116, g + 520 + 60 * k, g + 560 + 60 * k, TEX + "steel")
    # chute down to a stock pile
    f.beam((240, 0, 520), (480, 0, 220), 60, 20, TEX + "steelrust")


def octagon(cx, cy, r, z0, z1, mat, solid=True):
    planes = [((0, 0, -1), (cx, cy, z0), mat), ((0, 0, 1), (cx, cy, z1), mat)]
    for k in range(8):
        a = math.radians(22.5 + 45 * k)
        planes.append(((math.cos(a), math.sin(a), 0), (cx + r * math.cos(a), cy + r * math.sin(a), z0), mat))
    convex(planes, solid=solid)


def site_cabin(x, y, yaw):
    """Office container near a spawn."""
    z = ground_min(x, y, 140) - 6
    f = Frame(x, y, yaw, z)
    f.box(-130, 130, -52, 52, 0, 16, TEX + "steel")
    f.box(-128, 128, -50, 50, 16, 136, TEX + "cabin")
    f.box(-132, 132, -54, 54, 136, 144, TEX + "steel")
    f.box(-60, -20, 50, 53, 50, 110, TEX + "glass", solid=False)
    f.box(20, 60, 50, 53, 50, 110, TEX + "glass", solid=False)


def floodlight(x, y, yaw):
    z = ground_min(x, y, 30)
    entity("misc_model", x, y, z - 4, model="models/mapobjects/q3r_lanterns/floodmast.md3",
           modelscale="0.9", angle=f"{yaw:.0f}")
    obox(x, y, (1, 0), 14, 14, z - 10, z + 460, TEX + "boundary", solid=False)


# ---------------------------------------------------------------- the play area

def hill():
    """King of the Hill zone over the pit floor.

    The game reports the point (zone centre, top of the trigger +
    indicatorHeight) as the hill position, also to the bots. indicatorHeight
    puts it 40 above the floor; the crown ignores it. (The AAS area of the
    floor reaches up to the sky, so the bots find the zone either way.)"""
    z = LEVELS[0]
    top = z + 128
    planes = [((1, 0, 0), (HILL_HX, 0, 0)), ((-1, 0, 0), (-HILL_HX, 0, 0)), ((0, 1, 0), (0, HILL_HY, 0)),
              ((0, -1, 0), (0, -HILL_HY, 0)), ((0, 0, 1), (0, 0, top)), ((0, 0, -1), (0, 0, z - 16))]
    body = "\n".join(face(plane(n, p), "common/trigger", detail=False) for n, p in planes)
    TRIGGERS.append('{\n"classname" "trigger_koth_hill"\n"indicatorHeight" "%d"\n{\n' % (z + 40 - top)
                    + body + "\n}\n}")


def play_objects():
    # cover on the pit floor, some 300 units off the zone, lanes at least 220 wide
    # (bots got wedged between a stack, a gravel pile and the zone in a tighter layout)
    both(block_stack, -450, 720, 0, ((0, 0), (1, 0), (2, 0), (1, 1)))
    both(block_stack, 850, -150, 90, ((0, 0), (1, 0), (0, 1)))
    both(excavator, -700, 800, 90)            # digging at the floor face, next to the hopper
    # benches: trucks and blocks against the outer side, the inner lane stays free
    both(dump_truck, 600, FY + 2 * BENCH - 80, 180)
    both(block_stack, FX + 340, 720, 80, ((0, 0), (1, 0), (2, 0), (0, 1), (1, 1)))
    both(block_stack, FX + 760, -760, 100, ((0, 0), (1, 0), (0, 1)))
    # conveyors from the pit floor to the crushing plants beyond the north and south rim
    for s in (1, -1):
        p0 = (s * -860, s * 640, LEVELS[0] + 150)
        p1 = (s * -700, s * (FY + EDGE_D + 520), 600)
        conveyor(p0, p1)
        crusher_plant(s * -620, s * (FY + EDGE_D + 640), 0 if s > 0 else 180)
    # site offices by the spawns (red east, blue west)
    both(site_cabin, FX + 1350, 1000, 90)
    both(site_cabin, FX + 1350, -1000, 90)
    # floodlights on the rim corners pointing into the pit (dressing)
    r = (RC + RIM_D + 160) * 0.7071
    for sx in (-1, 1):
        for sy in (-1, 1):
            floodlight(sx * (FX - RC + r), sy * (FY - RC + r), math.degrees(math.atan2(-sy, -sx)))


def spawns_and_items():
    for team, sx in (("red", 1), ("blue", -1)):
        ang = "180" if sx > 0 else "0"
        x = sx * (FX + 1250)
        for y in (-450, -150, 150, 450):
            yy = y * sx
            entity(f"team_KOTH_{team}player", x, yy, ground(x, yy) + 48, angle=ang)
        x2 = sx * (FX + 1080)
        for y in (-1250, -820, 820, 1250):
            yy = y * sx
            xx = x2 if abs(y) < 1000 else sx * (FX + 980)
            entity(f"team_KOTH_{team}spawn", xx, yy, ground(xx, yy) + 48, angle=ang)
    entity("info_player_deathmatch", 0, FY + 1250, ground(0, FY + 1250) + 48, angle="270")
    items = (("weapon_rocketlauncher", (0, FY + 300)),
             ("ammo_rockets", (-260, FY + 300)), ("ammo_rockets", (260, FY + 300)),
             ("weapon_shotgun", (0, -760)),
             ("item_armor_yellow", (FX + 670, 0)),
             ("item_health_large", (FX + 300, -300)), ("item_health_large", (-1150, FY + 650)),
             ("item_fuelcan", (-FX - 300, 0)), ("item_fuelcan", (1000, -FY - 650)),
             ("rally_item_turbo", (FX + 900, FY + 500)))
    for cls, (x, y) in items:
        for s in (1, -1):
            entity(cls, s * x, s * y, ground(s * x, s * y) + 40)
    entity("info_player_intermission", 2300, -2100, 900, angles="20 135 0")


# ---------------------------------------------------------------- around it

def boundary():
    """Invisible wall on the berm around the site: cars stay on the rim."""
    segs = []
    hx, hy, r = FX - RC, FY - RC, RC + EDGE_D
    # straight sides
    for (x0, y0, x1, y1) in ((-hx, hy + r, hx, hy + r), (hx + r, hy, hx + r, -hy),
                             (hx, -hy - r, -hx, -hy - r), (-hx - r, -hy, -hx - r, hy)):
        segs.append(((x0, y0), (x1, y1)))
    for cx, cy, a0 in ((hx, hy, 0), (-hx, hy, 90), (-hx, -hy, 180), (hx, -hy, 270)):
        n = 10
        for k in range(n):
            a, b = math.radians(a0 + 90 * k / n), math.radians(a0 + 90 * (k + 1) / n)
            segs.append(((cx + r * math.cos(a), cy + r * math.sin(a)), (cx + r * math.cos(b), cy + r * math.sin(b))))
    for (x0, y0), (x1, y1) in segs:
        L = math.hypot(x1 - x0, y1 - y0)
        t = ((x1 - x0) / L, (y1 - y0) / L)
        obox((x0 + x1) / 2, (y0 + y1) / 2, t, L / 2 + 8, 16, -200, 1600, TEX + "boundary", solid=False)


def trees():
    rng = random.Random(5)
    kinds = ["GR_trees/tree6", "GR_trees/tree6", "GR_trees/tree4", "GR_trees/nadelbaum", "GR_trees/nadelbaum2"]
    placed = []
    for _ in range(30000):
        x, y = rng.uniform(-HALF + 200, HALF - 200), rng.uniform(-HALF + 200, HALF - 200)
        d = pit_d(x, y)
        if d < EDGE_D + 260:
            continue
        clump = math.cos(x / 600 + 0.4) * math.cos(y / 520) + 0.5 * math.cos(x / 230) * math.sin(y / 260)
        if clump < 0.1 + rng.random() * 0.5 or any(math.dist((x, y), p) < 170 for p in placed):
            continue
        if any(lo[0] - 200 < x < hi[0] + 200 and lo[1] - 200 < y < hi[1] + 200 for lo, hi in BIG):
            continue
        placed.append((x, y))
        entity("misc_model", x, y, ground(x, y) - 10, model=f"models/mapobjects/{rng.choice(kinds)}.md3",
               modelscale=f"{0.85 + rng.random() * 0.4:.2f}", angle=str(rng.randrange(360)))
        if len(placed) >= 300:
            break
    REPORT["trees"] = len(placed)


BIG = []        # footprints of the plants, no trees there


def hill_height(x, y):
    r = max(abs(x), abs(y))
    bx, by = x * HALF / r, y * HALF / r
    z = GRID.get((round(bx / STEP) * STEP, round(by / STEP) * STEP), 300)
    d = r - HALF
    z += 700 * smooth(0, 3800, d) + 220 * math.sin(x / 830) ** 2 * math.cos(y / 710) ** 2 * smooth(0, 1500, d)
    return z


def backdrop():
    n = 2 * HALF // STEP
    for inner, outer in zip(OUTER[:-1], OUTER[1:]):
        for side in range(4):
            def point(extent, k):
                t = -extent + k * 2 * extent / n
                x, y = ((t, -extent), (extent, t), (-t, extent), (-extent, -t))[side]
                z = GRID[round(x), round(y)] if extent == HALF else hill_height(x, y)
                return (x, y, round(z * 8) / 8)
            for k in range(n):
                a, b, c, d = point(inner, k), point(outer, k), point(outer, k + 1), point(inner, k + 1)
                prism((a, b, c), TEX + "hills")
                prism((a, c, d), TEX + "hills")


def shell():
    backdrop()
    m, top = 64, max(GRID.values()) + 3000
    H = OUTER[-1]
    sky = TEX + "sky"
    for lo, hi, mat in (((-H - m, -H - m, FLOOR - 64), (H + m, H + m, FLOOR), "common/caulk"),
                        ((-H - m, -H - m, FLOOR), (-H, H + m, top), sky),
                        ((H, -H - m, FLOOR), (H + m, H + m, top), sky),
                        ((-H, -H - m, FLOOR), (H, -H, top), sky),
                        ((-H, H, FLOOR), (H, H + m, top), sky),
                        ((-H - m, -H - m, top), (H + m, H + m, top + m), sky)):
        (ax, ay, az), (bx, by, bz) = lo, hi
        brush([face(plane(nrm, pt), mat, detail=False) for nrm, pt in
               (((0, 0, -1), (ax, ay, az)), ((0, 0, 1), (bx, by, bz)), ((-1, 0, 0), (ax, ay, az)),
                ((1, 0, 0), (bx, by, bz)), ((0, -1, 0), (ax, ay, az)), ((0, 1, 0), (bx, by, bz)))])
    e = FX + EDGE_D + 200
    brush([face(plane(nrm, pt), "common/lightgrid") for nrm, pt in
           (((0, 0, -1), (-e, -e, -560)), ((0, 0, 1), (e, e, 900)), ((-1, 0, 0), (-e, -e, 0)),
            ((1, 0, 0), (e, e, 0)), ((0, -1, 0), (-e, -e, 0)), ((0, 1, 0), (e, e, 0)))])


# ---------------------------------------------------------------- shaders, textures

LAYER_TEX = (("grass", "textures/q3r_dirt_grass/grass07.tga", 320),
             ("dust", f"textures/{TEX}dust.jpg", 256),
             ("gravel", f"textures/{TEX}gravel.jpg", 192),
             ("rock", f"textures/{TEX}limestone.jpg", 288),
             ("facex", f"textures/{TEX}limestone.jpg", 320),
             ("facey", f"textures/{TEX}limestone.jpg", 320))
# pale limestone set, derived from textures in the repository by write_textures():
# (output, source, mean colour, saturation kept, contrast)
DERIVED = (("dust.jpg", "q3r_dirt_grass/dirt02.tga", (164, 157, 144), 0.4, 0.85),
           ("gravel.jpg", "q3r_ground/pebble01.jpg", (146, 142, 134), 0.45, 0.9),
           ("limestone.jpg", "q3r_dirt_grass/rock03.tga", (160, 156, 146), 0.6, 0.9),
           ("block.jpg", "q3r_dirt_grass/rock03.tga", (186, 182, 172), 0.4, 0.55))
PARENTS = (("grass", "surfaceparm SURF_GRASS", 0), ("dirt", "surfaceparm SURF_DIRT", 1),
           ("gravel", "surfaceparm SURF_GRAVEL", 2), ("plain", None, 3))
TINT = {}


def tcgen(layer):
    name, _, tile = LAYER_TEX[layer]
    k = 1.0 / tile
    if name == "facex":                     # faces looking along x: project on y and z
        return f"\t\ttcGen vector ( 0 {k:.7f} 0 ) ( 0 0 {k:.7f} )"
    if name == "facey":
        return f"\t\ttcGen vector ( {k:.7f} 0 0 ) ( 0 0 {k:.7f} )"
    return f"\t\ttcGen vector ( {k:.7f} 0 0 ) ( 0 {k:.7f} 0 )"


def stage(layer, blend):
    lines = ["\t{", f"\t\tmap {LAYER_TEX[layer][1]}"]
    if blend:
        lines += ["\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\talphaGen vertex"]
    return lines + [tcgen(layer), TINT.get(LAYER_TEX[layer][0], "\t\trgbGen identity"), "\t}"]


def tail():
    return ["\t{", f"\t\tmap textures/{TEX}macro.jpg", "\t\tblendFunc GL_DST_COLOR GL_SRC_COLOR",
            "\t\ttcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )", "\t\trgbGen identity", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}"]


def lit(name, image, extra=(), tex_extra=()):
    rg = [] if any("rgbGen" in s for s in tex_extra) else ["\t\trgbGen identity"]
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *tex_extra, *rg, "\t}", "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO",
            "\t\trgbGen identity", "\t}", "}", ""]


def tinted(name, image, rgb, extra=()):
    return lit(name, image, extra, (f"\t\trgbGen const ( {rgb[0]:.2f} {rgb[1]:.2f} {rgb[2]:.2f} )",))


def write_shader():
    T = f"textures/{TEX}"
    out = ["// Quarry King - King of the Hill in a limestone quarry.",
           "// Written by tools/maptools/make_quarry.py. Compile with -custinfoparms.", ""]
    out += [f"{T}sky", "{", "\tqer_editorimage textures/q3r_skies/env/trainyard/trainyard_ft.tga",
            "\tsurfaceparm noimpact", "\tsurfaceparm nolightmap", "\tsurfaceparm sky", "\tq3map_globaltexture",
            "\tq3map_sun 1.0 0.97 0.91 230 310 52", "\tq3map_skylight 90 3",
            "\tq3gl2_sun 1.0 0.97 0.91 160 310 52 0.5",
            "\tskyparms textures/q3r_skies/env/trainyard/trainyard - -",
            "\tq3r_haze 0.70 0.75 0.82 6500\t// Q3Rally: summer haze over the quarry", "}", ""]
    out += lit("block", f"{T}block.jpg")
    out += tinted("paint", "textures/q3r_metals/plate01.tga", (1.0, 0.74, 0.12), ("\tsurfaceparm SURF_METAL",))
    out += tinted("steel", "textures/q3r_metals/plate01.tga", (0.62, 0.62, 0.64), ("\tsurfaceparm SURF_METAL",))
    out += tinted("steelrust", "textures/q3r_metals/metrust01.tga", (0.85, 0.8, 0.75), ("\tsurfaceparm SURF_METAL",))
    out += tinted("rubber", "textures/q3r_metals/plate01.tga", (0.16, 0.16, 0.17))
    out += tinted("belt", "textures/q3r_metals/plate01.tga", (0.12, 0.12, 0.12), ("\tsurfaceparm SURF_METAL",))
    out += tinted("glass", "textures/q3r_metals/plate01.tga", (0.18, 0.24, 0.30), ("\tsurfaceparm nonsolid",))
    out += tinted("corrugated", "textures/q3r_building/wellblech.jpg", (0.82, 0.84, 0.86))
    out += tinted("cabin", "textures/q3r_building/wellblech.jpg", (0.42, 0.62, 0.78))
    out += tinted("silo", "textures/q3r_building/concrete03.jpg", (0.92, 0.92, 0.9))
    out += lit("concrete", "textures/q3r_building/concrete01.jpg")
    out += [f"{T}hills", "{", "\tqer_editorimage textures/q3r_dirt_grass/grass07.tga", "\tq3map_nonplanar",
            "\tq3map_shadeAngle 120", "\tq3map_lightmapSampleSize 64", "\tsurfaceparm SURF_GRASS"]
    out += stage(L_GRASS, False) + tail() + ["}", ""]
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
                "\tq3map_lightmapSampleSize 24", "\tq3map_nonplanar"] + stage(i, False) + tail() + ["}", ""]
    for i, j in combinations(range(len(LAYER_TEX)), 2):
        out += [f"{T}ter_{i}to{j}", "{", f"\tqer_editorimage {LAYER_TEX[i][1]}", "\tq3map_shadeAngle 120",
                "\tq3map_lightmapSampleSize 24",
                "\tq3map_nonplanar"] + stage(i, False) + stage(j, True) + tail() + ["}", ""]
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.shader").write_text("\n".join(out), encoding="utf-8")


def write_textures():
    from PIL import Image, ImageFilter
    out = ROOT / "baseq3r" / "textures" / "q3r_quarry"
    out.mkdir(parents=True, exist_ok=True)
    rnd = random.Random(17)
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
    m.putdata([int(128 + ((v - lo) / (hi - lo) - 0.5) * 70) for v in acc])
    m.filter(ImageFilter.GaussianBlur(1.2)).convert("RGB").save(out / "macro.jpg", quality=92)
    for name, src, mean, sat, contrast in DERIVED:
        im = Image.open(ROOT / "baseq3r" / "textures" / src).convert("RGB")
        px = list(im.getdata())
        n = len(px)
        avg = [sum(p[i] for p in px) / n for i in range(3)]
        lum_avg = sum(avg) / 3
        res = []
        for r, g, b in px:
            lum = (r + g + b) / 3
            c = [lum + (v - lum) * sat for v in (r, g, b)]                 # less colour
            c = [lum_avg + (v - lum_avg) * contrast for v in c]             # less contrast
            res.append(tuple(max(0, min(255, int(v * m_ / lum_avg))) for v, m_ in zip(c, mean)))
        im2 = Image.new("RGB", im.size)
        im2.putdata(res)
        im2.save(out / name, quality=92)


def index_map():
    from PIL import Image
    n = 2 * HALF // STEP + 1
    im = Image.new("L", (n, n))
    im.putdata([MAT[-HALF + i * STEP, HALF - j * STEP] for j in range(n) for i in range(n)])
    im.save(ROOT / "baseq3r" / "maps" / f"{NAME}_index.pcx")


# ---------------------------------------------------------------- checks, overview, output

def validate():
    grads = []
    for (ax, ay, az), (bx, by, bz), (cx, cy, cz) in TRIS:
        det = (bx - ax) * (cy - ay) - (cx - ax) * (by - ay)
        gx = ((bz - az) * (cy - ay) - (cz - az) * (by - ay)) / det
        gy = ((cz - az) * (bx - ax) - (bz - az) * (cx - ax)) / det
        grads.append(math.hypot(gx, gy))
    # the zone floor is flat
    zone = [ground(x, y) for x in range(-HILL_HX, HILL_HX + 1, 40) for y in range(-HILL_HY, HILL_HY + 1, 40)]
    assert max(zone) - min(zone) < 1, (min(zone), max(zone))
    # point symmetry of the play area
    for (x, y), z in GRID.items():
        if pit_d(x, y) < EDGE_D:
            assert abs(z - GRID[-x, -y]) < 0.2, (x, y, z, GRID[-x, -y])
    # ramps: grade and continuity along the middle of each road
    worst = 0.0
    for side, k, a, b in all_ramps():
        pts = []
        for along in range(int(min(a, b)) + 64, int(max(a, b)) - 63, 16):     # ends meet the face
            centre = k * BENCH + FACE / 2
            if side == "N":
                x, y = along, FY + centre
            elif side == "S":
                x, y = -along, -FY - centre
            elif side == "E":
                x, y = FX + centre, along
            else:
                x, y = -FX - centre, -along
            pts.append(ground(x, y))
        worst = max(worst, max(abs(q - p) / 16 for p, q in zip(pts, pts[1:])))
    # faces away from the ramps are too steep to drive up
    face_grade = []
    for k in range(3):
        prof = [ground(740, FY + r) for r in range(k * BENCH - 40, k * BENCH + FACE + 40, 16)]   # no ramp here
        face_grade.append(max(abs(q - p) / 16 for p, q in zip(prof, prof[1:])))
    starts = [e for e in ENTITIES if "KOTH" in e["classname"] or e["classname"] == "info_player_deathmatch"]
    things = [e for e in ENTITIES if e["classname"].startswith(("weapon", "ammo", "item", "rally_item"))]
    for e in starts + things:
        x, y, z = map(float, e["origin"].split())
        assert pit_d(x, y) < EDGE_D - 40, e
        for lo, hi in SOLIDS:
            assert not (x + 60 > lo[0] and x - 60 < hi[0] and y + 60 > lo[1] and y - 60 < hi[1]
                        and z + 30 > lo[2] and z - 60 < hi[2]), (e, lo, hi)
        for p0, p1 in CONVEYORS:
            dx, dy = p1[0] - p0[0], p1[1] - p0[1]
            t = max(0.0, min(1.0, ((x - p0[0]) * dx + (y - p0[1]) * dy) / (dx * dx + dy * dy)))
            assert math.hypot(x - p0[0] - t * dx, y - p0[1] - t * dy) > 110, ("under a conveyor", e)
    red = [tuple(map(float, e["origin"].split()))[:2] for e in ENTITIES if e["classname"] == "team_KOTH_redplayer"]
    blue = [tuple(map(float, e["origin"].split()))[:2] for e in ENTITIES if e["classname"] == "team_KOTH_blueplayer"]
    assert sorted((-x, -y) for x, y in red) == sorted(blue)
    # nothing solid inside the hill zone
    for lo, hi in SOLIDS:
        assert not (lo[0] < HILL_HX and hi[0] > -HILL_HX and lo[1] < HILL_HY and hi[1] > -HILL_HY
                    and lo[2] < LEVELS[0] + 100), ("solid in the zone", lo, hi)
    REPORT.update({"map": NAME, "terrain_triangles": len(TRIS),
                   "leaves": {s: sum(1 for c in LEAVES if c[2] == s) for s in (64, 128)},
                   "brushes": len(BRUSHES) + sum(len(v) for v in GROUPS.values()),
                   "levels": LEVELS, "hill_zone": [2 * HILL_HX, 2 * HILL_HY],
                   "ramp_max_grade_deg": round(math.degrees(math.atan(worst)), 1),
                   "face_grade_deg": [round(math.degrees(math.atan(g)), 1) for g in face_grade],
                   "max_slope_deg": round(math.degrees(math.atan(max(grads))), 1)})
    return REPORT


def overview(report):
    from PIL import Image, ImageDraw
    size = 900
    ext = FX + EDGE_D + 200
    sc = 2 * ext / size
    im = Image.new("RGB", (size, size))
    pix = im.load()
    colors = {L_GRASS: (120, 140, 80), L_DUST: (196, 188, 170), L_GRAVEL: (160, 156, 150),
              L_ROCK: (210, 208, 200), L_FACEX: (150, 146, 140), L_FACEY: (150, 146, 140)}
    zmin, zmax = -480, 300
    for v in range(size):
        y = ext - v * sc
        gy = -HALF + round((y + HALF) / STEP) * STEP
        for u in range(size):
            x = -ext + u * sc
            gx = -HALF + round((x + HALF) / STEP) * STEP
            z = GRID[gx, gy]
            zx = GRID.get((gx + STEP, gy), z)
            shade = 0.55 + 0.5 * (z - zmin) / (zmax - zmin) - (zx - z) / STEP * 0.25
            pix[u, v] = tuple(max(0, min(255, int(k * shade))) for k in colors[MAT[gx, gy]])
    d = ImageDraw.Draw(im)
    f = lambda x, y: ((x + ext) / sc, (ext - y) / sc)
    d.rectangle([f(-HILL_HX, HILL_HY), f(HILL_HX, -HILL_HY)], outline=(255, 215, 0), width=2)
    for lo, hi in SOLIDS:
        if pit_d((lo[0] + hi[0]) / 2, (lo[1] + hi[1]) / 2) < EDGE_D:
            d.rectangle([f(lo[0], hi[1]), f(hi[0], lo[1])], outline=(60, 60, 60))
    for e in ENTITIES:
        if e["classname"] in ("misc_model", "info_player_intermission"):
            continue
        x, y, _ = map(float, e["origin"].split())
        u, v = f(x, y)
        c = "white"
        if "KOTH" in e["classname"]:
            c = "#ff5040" if "red" in e["classname"] else "#4080ff"
        elif e["classname"].startswith(("weapon", "ammo", "item", "rally_item")):
            c = "#30d040"
        d.ellipse((u - 4, v - 4, u + 4, v + 4), fill=c)
    out = ROOT / "docs" / "maps" / "quarry"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "quarry_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def write_map():
    fields = {"classname": "worldspawn", "message": "Quarry King", "_ambient": "18", "_minlight": "12",
              "gridsize": "64 64 128", "_blocksize": "1024 1024 4096"}
    text = ["// Generated by tools/maptools/make_quarry.py. Q3 brush format.",
            f"// Ground: maps/{NAME}_index.pcx with the q3r_quarry ground shaders.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    for group in ("grass", "dirt", "gravel", "plain"):
        keys = {"classname": "func_group", "_terrain": f"quarry ground, {group} friction",
                "_indexmap": f"maps/{NAME}_index.pcx", "_layers": str(len(LAYERS)),
                "_shader": TEX + "ter", "min": f"{-HALF} {-HALF} {FLOOR}", "max": f"{HALF} {HALF} 4096"}
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in keys.items()))
        text += GROUPS.get(group, [])
        text.append("}")
    text += TRIGGERS
    for ent in ENTITIES:
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in ent.items()) + "\n}")
    (ROOT / "baseq3r" / "maps" / f"{NAME}.map").write_text("\n".join(text) + "\n", encoding="ascii")
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.arena").write_text(
        f'{{\n\tmap "{NAME}"\n\tlongname "Quarry King"\n\ttype "q3r_koth"\n\tbotsupport "None"\n}}\n',
        encoding="ascii")


def main():
    build_terrain()
    hill()
    play_objects()
    spawns_and_items()
    boundary()
    for lo, hi in SOLIDS:
        if hi[2] - lo[2] > 300:
            BIG.append((lo, hi))
    trees()
    shell()
    report = validate()
    write_textures()
    write_shader()
    index_map()
    write_map()
    overview(report)
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
