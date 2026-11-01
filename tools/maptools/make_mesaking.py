#!/usr/bin/env python3
"""Generate q3r_mesaking: King of the Hill in a red rock arena.

A round sand arena inside a ring of steep sandstone walls. In the middle
stands a mesa with a flat top (the hill, one trigger_koth_hill) that is
reached by four ramps on the diagonals; everywhere else its sides are cliffs.
Red spawns in the east, blue in the west, both the same distance from every
ramp (the map is mirror symmetric in x and y). Cover rocks, worn tracks from
the spawns to the ramps, pickups placed in mirrored pairs.

Uses the ground layers, shaders and textures of q3r_canyonrush (the same
desert set): run tools/maptools/make_canyonrush.py first. Collision is closed
triangular detail brushes on a 64/128-unit grid; tyre friction from the
parent shaders. Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_mesaking"
TEX = "q3r_canyonrush/"          # shared desert set
DETAIL = 134217728
STEP = 64
FLOOR = -192
HALF = 3328                      # half size of the detailed square
OUTER = (HALF, 4300, 5500, 7168)  # rings of hills around it, out to the sky
R_ARENA, WALL_RUN, WALL_H = 2150, 420, 480
MESA_TOP, R_TOP, R_CLIFF = 260, 400, 150
RAMP_DIRS = (45, 135, 225, 315)
RAMP_HW, RAMP_LEN = 150, 1050
SPAWN_X = 1850

LAYERS = ("sand", "track", "gravel", "rock", "cliff")
L_SAND, L_TRACK, L_GRAVEL, L_ROCK, L_CLIFF = range(5)
FRICTION = {L_SAND: "sand", L_TRACK: "dirt", L_GRAVEL: "gravel", L_ROCK: "plain", L_CLIFF: "plain"}

GRID, MAT, LEAVES, LEAFOF, TRIS = {}, {}, [], {}, []
BRUSHES, GROUPS, ENTITIES, SOLIDS, TRIGGERS = [], {}, [], [], []
BOUNDS = {"x0": -HALF, "x1": HALF, "y0": -HALF, "y1": HALF}


def smooth(a, b, x):
    t = min(1.0, max(0.0, (x - a) / (b - a)))
    return t * t * (3 - 2 * t)


def ramp_height(x, y):
    """Highest ramp surface at this point (0 outside the ramps)."""
    best = 0.0
    for a in RAMP_DIRS:
        ca, sa = math.cos(math.radians(a)), math.sin(math.radians(a))
        along = x * ca + y * sa
        lateral = abs(-x * sa + y * ca)
        if along < R_TOP - 60:
            continue
        z = MESA_TOP * min(1.0, max(0.0, 1 - (along - R_TOP) / RAMP_LEN))
        z *= 1 - smooth(RAMP_HW, RAMP_HW + 80, lateral)
        best = max(best, z)
    return best


def floor(x, y):
    """Gentle dunes, flat around the mesa so its top and ramps stay true."""
    r = math.hypot(x, y)
    dunes = 10 * math.cos(x / 530) * math.cos(y / 610) + 14 * math.cos(x / 1300) * math.cos(y / 1100)
    return dunes * smooth(1500, 1900, r) - 14


def height(x, y):
    r = math.hypot(x, y)
    base = floor(x, y)
    rim = WALL_H + 70 * math.cos(x / 900) * math.cos(y / 800)
    z = base + (rim - base) * smooth(R_ARENA, R_ARENA + WALL_RUN, r)
    mesa = MESA_TOP * (1 - smooth(R_TOP, R_TOP + R_CLIFF, r))
    return max(z, base + max(mesa, ramp_height(x, y)) if r < R_TOP + RAMP_LEN + 120 else z)


def track_weight(x, y):
    """Worn tracks from both spawn areas to the feet of the ramps."""
    best = 0.0
    for sx in (-1, 1):
        for a in RAMP_DIRS:
            fx, fy = math.cos(math.radians(a)) * (R_TOP + RAMP_LEN), math.sin(math.radians(a)) * (R_TOP + RAMP_LEN)
            if fx * sx < 0:
                continue
            ax, ay = sx * (SPAWN_X - 250), 0.0
            dx, dy = fx - ax, fy - ay
            t = max(0.0, min(1.0, ((x - ax) * dx + (y - ay) * dy) / (dx * dx + dy * dy)))
            d = math.hypot(x - ax - t * dx, y - ay - t * dy)
            best = max(best, 1 - smooth(60, 120, d))
    return best


def on_ramp(x, y):
    for a in RAMP_DIRS:
        ca, sa = math.cos(math.radians(a)), math.sin(math.radians(a))
        if x * ca + y * sa > R_TOP - 40 and abs(-x * sa + y * ca) < RAMP_HW + 20:
            return True
    return False


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
        r = math.hypot(x, y)
        if slope > 0.85:
            m = L_CLIFF
        elif slope > 0.42 and not on_ramp(x, y):
            m = L_ROCK
        elif r < R_TOP - 20 or (on_ramp(x, y) and r < R_TOP + RAMP_LEN):
            m = L_TRACK
        elif R_TOP + R_CLIFF - 20 < r < R_TOP + R_CLIFF + 140:
            m = L_GRAVEL
        elif r < R_ARENA and track_weight(x, y) > 0.5:
            m = L_TRACK
        else:
            patch = math.cos(x / 370) * math.cos(y / 430) + 0.5 * math.cos(x / 157) * math.cos(y / 181)
            m = L_ROCK if (patch > 1.0 and r > R_ARENA + WALL_RUN) else L_SAND
        MAT[x, y] = m
    # 128-unit cells where 64 adds nothing (flat, one layer)
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
    order = ("plain", "sand", "gravel", "dirt")
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




# ---------------------------------------------------------------- dressing

def dressing():
    top = MESA_TOP + floor(0, 0)
    # the hill: one trigger box over the flat top
    t = R_TOP - 40
    planes = [((1, 0, 0), (t, 0, 0)), ((-1, 0, 0), (-t, 0, 0)), ((0, 1, 0), (0, t, 0)), ((0, -1, 0), (0, -t, 0)),
              ((0, 0, 1), (0, 0, top + 200)), ((0, 0, -1), (0, 0, top - 10))]
    body = "\n".join(face(plane(n, p), "common/checkpoint", detail=False) for n, p in planes)
    TRIGGERS.append('{\n"classname" "trigger_koth_hill"\n"indicatorHeight" "-40"\n{\n' + body + "\n}\n}")
    # no pole or marker posts on the top: cgame draws the floating crown above the
    # hill and bollards on the border of the trigger (cg_hud_vehicle.c)
    # cover rocks in the arena, mirror symmetric in x and y
    rng = random.Random(3)
    seed = 0
    for r, a, size in ((1250, 20, 70), (1250, 68, 60), (1650, 45, 55), (900, 0, 45), (1500, 90, 75)):
        for sx in (-1, 1):
            for sy in (-1, 1):
                seed += 1
                x, y = sx * r * math.cos(math.radians(a)), sy * r * math.sin(math.radians(a))
                if a in (0, 90) and ((a == 0 and sy < 0) or (a == 90 and sx < 0)):
                    continue                       # on an axis: one rock per side
                boulder(x, y, size, size * 1.1, 2.3 + (seed % 4) * 1.7, TEX + "boulder")
    # hoodoos on the rim plateau (backdrop)
    for k in range(14):
        a = math.radians(k * 360 / 14 + 9)
        r = R_ARENA + WALL_RUN + 280 + (k % 3) * 120
        hoodoo(r * math.cos(a), r * math.sin(a), 60 + (k % 4) * 12, 300 + (k % 5) * 70, TEX + "rock", TEX + "caprock")
    # spawns: red east, blue west, facing the mesa
    for team, sx in (("red", 1), ("blue", -1)):
        ang = "180" if sx > 0 else "0"
        for y in (-450, -150, 150, 450):
            entity(f"team_KOTH_{team}player", sx * SPAWN_X, y, ground(sx * SPAWN_X, y) + 48, angle=ang)
        for y in (-1250, -800, 800, 1250):
            x = sx * (SPAWN_X - 250)
            entity(f"team_KOTH_{team}spawn", x, y, ground(x, y) + 48, angle=ang)
    entity("info_player_deathmatch", 0, -1850, ground(0, -1850) + 48, angle="90")
    # pickups in mirrored pairs
    for cls, pts in (("weapon_rocketlauncher", [(0, 1850), (0, -1850)]),
                     ("ammo_rockets", [(450, 1650), (-450, 1650), (450, -1650), (-450, -1650)]),
                     ("weapon_shotgun", [(1050, 0), (-1050, 0)]),
                     ("item_armor_yellow", [(0, 1100), (0, -1100)]),
                     ("item_health_large", [(1500, 700), (-1500, 700), (1500, -700), (-1500, -700)]),
                     ("item_fuelcan", [(1700, 1150), (-1700, 1150), (1700, -1150), (-1700, -1150)]),
                     ("rally_item_turbo", [(1200, 1450), (-1200, -1450)])):
        for x, y in pts:
            entity(cls, x, y, ground(x, y) + 48)
    entity("info_player_intermission", -1500, -1500, 900, angles="22 45 0")


def hill_height(x, y):
    """Desert hills beyond the arena rim; rise towards the sky walls."""
    r = max(abs(x), abs(y))
    bx, by = x * HALF / r, y * HALF / r
    z = GRID.get((round(bx / STEP) * STEP, round(by / STEP) * STEP), WALL_H)
    d = r - HALF
    z += 900 * smooth(0, 3800, d) + 260 * math.sin(x / 830) ** 2 * math.cos(y / 710) ** 2 * smooth(0, 1500, d)
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
    # invisible solid ring on the rim: cars can reach the top but not drive off
    r, segs = R_ARENA + WALL_RUN + 160, 40
    for k in range(segs):
        a = 2 * math.pi * (k + 0.5) / segs
        t = (-math.sin(a), math.cos(a))
        half = r * math.tan(math.pi / segs) + 4
        obox(r * math.cos(a), r * math.sin(a), t, half, 16, -200, 2600, TEX + "boundary", solid=False)


def shell():
    backdrop()
    m, top = 64, max(GRID.values()) + 2400
    H = OUTER[-1]
    sky = TEX + "sky"
    top += 900
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
    e = R_ARENA + WALL_RUN
    brush([face(plane(nrm, pt), "common/lightgrid") for nrm, pt in
           (((0, 0, -1), (-e, -e, -64)), ((0, 0, 1), (e, e, 1100)), ((-1, 0, 0), (-e, -e, 0)),
            ((1, 0, 0), (e, e, 0)), ((0, -1, 0), (-e, -e, 0)), ((0, 1, 0), (e, e, 0)))])


def index_map():
    from PIL import Image
    n = 2 * HALF // STEP + 1
    im = Image.new("L", (n, n))
    im.putdata([MAT[-HALF + i * STEP, HALF - j * STEP] for j in range(n) for i in range(n)])
    im.save(ROOT / "baseq3r" / "maps" / f"{NAME}_index.pcx")


def validate():
    top = MESA_TOP + floor(0, 0)
    grads = []
    for (ax, ay, az), (bx, by, bz), (cx, cy, cz) in TRIS:
        det = (bx - ax) * (cy - ay) - (cx - ax) * (by - ay)
        gx = ((bz - az) * (cy - ay) - (cz - az) * (by - ay)) / det
        gy = ((cz - az) * (bx - ax) - (bz - az) * (cx - ax)) / det
        grads.append(math.hypot(gx, gy))
    ramp = [ground(r * math.cos(math.radians(45)), r * math.sin(math.radians(45))) for r in range(R_TOP, R_TOP + RAMP_LEN, 32)]
    ramp_grade = max(abs(b - a) / 32 for a, b in zip(ramp, ramp[1:]))
    assert abs(ground(0, 0) - top) < 1 and abs(ground(R_TOP - 60, 0) - top) < 2
    assert all(abs(GRID[x, y] - GRID[-x, y]) < 0.01 and abs(GRID[x, y] - GRID[x, -y]) < 0.01 for x, y in GRID)
    # the side of the mesa away from the ramps must be too steep to drive up
    side = [ground(r, 0) for r in range(R_TOP, R_TOP + R_CLIFF + 64, 16)]
    cliff = max(abs(b - a) / 16 for a, b in zip(side, side[1:]))
    starts = [e for e in ENTITIES if "KOTH" in e["classname"] or e["classname"] == "info_player_deathmatch"]
    for e in starts + [e for e in ENTITIES if e["classname"].startswith(("weapon", "ammo", "item", "rally_item"))]:
        x, y, z = map(float, e["origin"].split())
        for lo, hi in SOLIDS:
            assert not (x + 60 > lo[0] and x - 60 < hi[0] and y + 60 > lo[1] and y - 60 < hi[1]
                        and z + 30 > lo[2] and z - 60 < hi[2]), e
    red = [tuple(map(float, e["origin"].split()))[:2] for e in ENTITIES if e["classname"] == "team_KOTH_redplayer"]
    blue = [tuple(map(float, e["origin"].split()))[:2] for e in ENTITIES if e["classname"] == "team_KOTH_blueplayer"]
    assert sorted((-x, y) for x, y in red) == sorted(blue)
    return {"map": NAME, "terrain_triangles": len(TRIS),
            "leaves": {s: sum(1 for c in LEAVES if c[2] == s) for s in (64, 128)},
            "brushes": len(BRUSHES) + sum(len(v) for v in GROUPS.values()),
            "hill_top_z": round(top, 1), "ramp_max_grade_deg": round(math.degrees(math.atan(ramp_grade)), 1),
            "mesa_cliff_grade_deg": round(math.degrees(math.atan(cliff)), 1),
            "max_slope_deg": round(math.degrees(math.atan(max(grads))), 1)}


def overview(report):
    from PIL import Image, ImageDraw
    size = 900
    sc = 2 * HALF / size
    im = Image.new("RGB", (size, size))
    pix = im.load()
    colors = {L_SAND: (196, 140, 92), L_TRACK: (150, 112, 80), L_GRAVEL: (150, 146, 132),
              L_ROCK: (140, 74, 52), L_CLIFF: (168, 92, 62)}
    zmin, zmax = min(GRID.values()), max(GRID.values())
    for v in range(size):
        y = HALF - v * sc
        gy = -HALF + round((y + HALF) / STEP) * STEP
        for u in range(size):
            x = -HALF + u * sc
            gx = -HALF + round((x + HALF) / STEP) * STEP
            z = GRID[gx, gy]
            zx = GRID.get((gx + STEP, gy), z)
            shade = 0.6 + 0.5 * (z - zmin) / (zmax - zmin) - (zx - z) / STEP * 0.2
            pix[u, v] = tuple(max(0, min(255, int(k * shade))) for k in colors[MAT[gx, gy]])
    d = ImageDraw.Draw(im)
    for e in ENTITIES:
        x, y, _ = map(float, e["origin"].split())
        u, v = (x + HALF) / sc, (HALF - y) / sc
        c = {"r": "#ff5040", "b": "#4080ff"}.get(e["classname"][10:11], "white") if "KOTH" in e["classname"] else "white"
        if e["classname"] != "info_player_intermission":
            d.ellipse((u - 4, v - 4, u + 4, v + 4), fill=c)
    out = ROOT / "docs" / "maps" / "mesaking"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "mesaking_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def write_map():
    fields = {"classname": "worldspawn", "message": "Mesa King", "gravity": "1400", "_ambient": "22",
              "_minlight": "14", "gridsize": "64 64 128", "_blocksize": "512 512 4096"}
    text = ["// Generated by tools/maptools/make_mesaking.py. Q3 brush format.",
            "// Ground: maps/q3r_mesaking_index.pcx with the q3r_canyonrush ground shaders.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    for group in ("sand", "dirt", "gravel", "plain"):
        keys = {"classname": "func_group", "_terrain": f"arena ground, {group} friction",
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
        '{\n'
        '\tmap "q3r_mesaking"\n'
        '\tlongname "Mesa King"\n'
        '\tauthor "Q3Rally Team"\n'
        '\ttype "q3r_koth"\n'
        '\tstarts "8"\n'
        '\tobserverspots "0"\n'
        '\tweapons "SG,RL"\n'
        '\tpowerups "2"\n'
        '\tbotsupport "None"\n'
        '\tteams "2"\n'
        '}\n'
        , encoding="ascii")


def main():
    build_terrain()
    dressing()
    shell()
    report = validate()
    index_map()
    write_map()
    overview(report)
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
