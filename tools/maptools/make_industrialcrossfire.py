#!/usr/bin/env python3
"""Generate q3r_ctf4_03 (Industrial Crossfire): four-team CTF in a harbour.

A square pier in the middle of a harbour basin at dusk. The four team bases
sit in its corners, each between two quays. The layout is rotationally
symmetric (90 degrees), so every team has the same distances, cover and
pickups; only the look of the bases differs:

  blue   (north-east)  container terminal
  red    (north-west)  steel works
  green  (south-west)  rail depot
  yellow (south-east)  bulk terminal

Per quadrant (built once in local coordinates, then rotated):
- base yard with an L-shaped hall around the flag nook and a landmark,
- an edge road along each quay to the neighbouring bases,
- on the quays a gantry crane over container stacks, or a siding with wagons,
- a warehouse with a ramp onto its roof (jump down onto the edge road),
- a tank farm with a pipe bridge, a container maze, a small rail yard,
- a diagonal road to the centre and one arm of the elevated cross ("the
  crossfire"): viaducts from all four edge roads meet on a platform above
  the central plaza.

Around the pier: harbour water, moored ships, and an industrial skyline on the
far shores. Brushes only (no terrain); every face is aligned to the grid.
Run with Python 3 + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_ctf4_03"
TEX = "q3r_ctf4_03/"
DETAIL = 134217728

PIER = 2800                    # half size of the pier
SEA = 5600                     # half size of the water ring
WATER_Z, SEABED = -56, -256
DECK = 224                     # top of the elevated cross
BASE = 2250                    # flag position (local x = y)
TEAMS = ("blue", "red", "green", "yellow")    # rotation 0..3: NE, NW, SW, SE
COLORS = {"red": (0.78, 0.20, 0.16), "blue": (0.18, 0.36, 0.80), "green": (0.22, 0.58, 0.26),
          "yellow": (0.92, 0.72, 0.12)}
CONTAINER_COLORS = ((0.80, 0.36, 0.12), (0.72, 0.72, 0.70), (0.20, 0.42, 0.50), (0.48, 0.26, 0.16),
                    (0.62, 0.14, 0.12), (0.18, 0.30, 0.56), (0.30, 0.48, 0.24), (0.86, 0.66, 0.16))

BRUSHES, ENTITIES, SOLIDS = [], [], []
STATS = {}


# ---------------------------------------------------------------- brush basics

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
    p = " ".join("( %s )" % " ".join(num(v) for v in xyz) for xyz in points)
    xo, yo, ss, ts = tex if tex else (0, 0, scale, scale)
    return f"{p} {material} {num(xo)} {num(yo)} 0 {num(ss)} {num(ts)} {DETAIL if detail else 0} 0 0"


def brush(faces):
    BRUSHES.append("{\n" + "\n".join(faces) + "\n}")


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


def convex(planes, solid=True, scale=0.5):
    """(outward normal, point, material) planes."""
    brush([face(plane(n, p), m, scale=scale) for n, p, m in planes])
    if solid:
        corners = polytope([(n, p) for n, p, _ in planes])
        if corners:
            SOLIDS.append((tuple(min(c[i] for c in corners) for i in range(3)),
                           tuple(max(c[i] for c in corners) for i in range(3))))


def hexa(corners, mats, solid=True, scale=0.5):
    """Convex 8-corner block: corners 0-3 one end (ring order), 4-7 the other.
    mats: material or dict (0 end a, 1 end b, 2-5 sides, "default")."""
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
    convex(planes, solid=solid, scale=scale)


BASE_AXES = (((0, 0, 1), (1, 0, 0), (0, -1, 0)), ((0, 0, -1), (1, 0, 0), (0, -1, 0)),
             ((1, 0, 0), (0, 1, 0), (0, 0, -1)), ((-1, 0, 0), (0, 1, 0), (0, 0, -1)),
             ((0, 1, 0), (1, 0, 0), (0, 0, -1)), ((0, -1, 0), (1, 0, 0), (0, 0, -1)))


def fitted(normal, left, top_z, width, height, img_w, img_h):
    """Shift/scale of the Q3 axial projection that puts an image unmirrored onto
    an upright face: width x height units per copy, from its left/top edge."""
    best = max(BASE_AXES, key=lambda b: sum(x * y for x, y in zip(normal, b[0])))
    sv = best[1]
    right = (-normal[1], normal[0], 0.0)
    ss = width * sum(x * y for x, y in zip(right, sv)) / img_w
    dl = sum(x * y for x, y in zip(left, sv))
    ts = height / img_h
    return (-dl / ss) % img_w, (top_z / ts) % img_h, ss, ts


# ---------------------------------------------------------------- rotation

ROT = 0          # current quadrant: 0 NE, 1 NW, 2 SW, 3 SE (90 degree steps CCW)


def R(x, y):
    for _ in range(ROT):
        x, y = -y, x
    return x, y


def RN(key):
    order = ["+x", "+y", "-x", "-y"]
    return order[(order.index(key) + ROT) % 4] if key in order else key


def box(lo, hi, mat, faces=None, solid=True, scale=0.5):
    """Axial box in local (quadrant) coordinates. faces: {"+x": material or
    (image, tile_w, tile_h, img_w, img_h) fitted; "top"/"bot": material}."""
    (ax, ay), (bx, by) = R(lo[0], lo[1]), R(hi[0], hi[1])
    x1, x2, y1, y2 = min(ax, bx), max(ax, bx), min(ay, by), max(ay, by)
    z1, z2 = lo[2], hi[2]
    assert x2 > x1 and y2 > y1 and z2 > z1, (lo, hi)
    world = {RN(k): v for k, v in (faces or {}).items()}
    normals = {"bot": (0, 0, -1), "top": (0, 0, 1), "-x": (-1, 0, 0), "+x": (1, 0, 0), "-y": (0, -1, 0),
               "+y": (0, 1, 0)}
    out = []
    for key, n in normals.items():
        p = (x2 if n[0] > 0 else x1, y2 if n[1] > 0 else y1, z2 if n[2] > 0 else z1)
        spec = world.get(key, mat)
        if isinstance(spec, tuple):
            image, tw, th, iw, ih = spec
            right = (-n[1], n[0])
            left = (x1 if right[0] > 0 else x2 if right[0] < 0 else p[0],
                    y1 if right[1] > 0 else y2 if right[1] < 0 else p[1], 0.0)
            out.append(face(plane(n, p), image, tex=fitted((float(n[0]), float(n[1]), 0.0), left, z2, tw, th, iw, ih)))
        else:
            out.append(face(plane(n, p), spec, scale=scale))
    brush(out)
    if solid:
        SOLIDS.append(((x1, y1, z1), (x2, y2, z2)))


def lhexa(corners, mats, solid=True, scale=0.5):
    hexa([(*R(x, y), z) for x, y, z in corners], mats, solid=solid, scale=scale)


def lconvex(planes, solid=True, scale=0.5):
    """Local (normal, point, material); normals rotate with the quadrant."""
    world = []
    for (nx, ny, nz), (px, py, pz), m in planes:
        wx, wy = R(nx, ny)
        qx, qy = R(px, py)
        world.append(((wx, wy, nz), (qx, qy, pz), m))
    convex(world, solid=solid, scale=scale)


def entity(classname, x, y, z, angle=None, **keys):
    wx, wy = R(x, y)
    fields = {"classname": classname, "origin": f"{round(wx)} {round(wy)} {round(z)}"}
    if angle is not None:
        fields["angle"] = f"{(angle + 90 * ROT) % 360:g}"
    fields.update(keys)
    ENTITIES.append(fields)
    return fields


def decal(lo, hi, mat, z=0.0):
    """Flat nonsolid paint on the ground (local axial rectangle). Markings on
    top of the road paint use z=1, so the two never share a plane (flicker)."""
    box((lo[0], lo[1], z), (hi[0], hi[1], z + 1.0), "common/nodraw", faces={"top": mat}, solid=False)


def ddecal(p0, p1, half, mat, z=0.0):
    """Paint strip from p0 to p1 (local), half width `half`."""
    dx, dy = p1[0] - p0[0], p1[1] - p0[1]
    ln = math.hypot(dx, dy)
    t = (dx / ln, dy / ln)
    n = (-t[1], t[0])
    corners = []
    for p in (p0, p1):
        for lat, dz in ((-half, 0.0), (half, 0.0), (half, 1.0), (-half, 1.0)):
            corners.append((p[0] + n[0] * lat, p[1] + n[1] * lat, z + dz))
    lhexa(corners, {"default": "common/nodraw", 4: mat}, solid=False)


def octagon(cx, cy, r, z0, z1, mat, top=None, solid=True, tile=(128, 128, 256, 256)):
    """Upright octagonal prism (r = apothem). Every side gets its image fitted
    (tile_w x tile_h units per copy), so the slanted sides are not stretched."""
    wx, wy = R(cx, cy)
    side = 2 * r * math.tan(math.radians(22.5))
    faces = [face(plane((0, 0, -1), (wx, wy, z0)), mat), face(plane((0, 0, 1), (wx, wy, z1)), top or mat)]
    tw, th, iw, ih = tile
    for k in range(8):
        a = math.radians(22.5 + 45 * k)
        n = (math.cos(a), math.sin(a), 0.0)
        p = (wx + r * n[0], wy + r * n[1], z0)
        right = (-n[1], n[0])
        left = (p[0] - right[0] * side / 2, p[1] - right[1] * side / 2, 0.0)
        faces.append(face(plane(n, p), mat, tex=fitted(n, left, z1, tw, th, iw, ih)))
    brush(faces)
    if solid:
        SOLIDS.append(((wx - r, wy - r, z0), (wx + r, wy + r, z1)))


# ---------------------------------------------------------------- props

def container(x, y, along_x, level, color_index, z0=0.0):
    """20 ft container: 240 x 96 x 104, corrugated sides, door ends."""
    L, W, H = 240, 96, 104
    z = z0 + level * H
    side = TEX + f"cont{color_index}"
    if along_x:
        box((x - L / 2, y - W / 2, z), (x + L / 2, y + W / 2, z + H), side,
            faces={"+x": side + "_end", "-x": side + "_end", "top": TEX + "controof"})
    else:
        box((x - W / 2, y - L / 2, z), (x + W / 2, y + L / 2, z + H), side,
            faces={"+y": side + "_end", "-y": side + "_end", "top": TEX + "controof"})


RAILM = "models/mapobjects/q3r_rail/"


def track(x0, y0, x1, y1):
    """Rail track along x or y (local): track.md3 segments (each stretched a
    little so they fill the length exactly) on a ballast bed, invisible
    collision boxes for the two rails."""
    along_x = abs(x1 - x0) > abs(y1 - y0)
    for off in (-36, 36):
        if along_x:
            box((x0, y0 + off - 4, 0), (x1, y0 + off + 4, 7), TEX + "propclip")
        else:
            box((x0 + off - 4, y0, 0), (x0 + off + 4, y1, 7), TEX + "propclip")
    a, b = (x0, x1) if along_x else (y0, y1)
    a, b = min(a, b), max(a, b)
    n = max(1, round((b - a) / 256))
    seg = (b - a) / n
    for i in range(n):
        c = a + (i + 0.5) * seg
        x, y = (c, y0) if along_x else (x0, c)
        entity("misc_model", x, y, 0, angle=0 if along_x else 90, model=RAILM + "track.md3",
               modelscale_vec=f"{seg / 256:.4f} 1 1")
    if along_x:
        decal((x0, y0 - 64), (x1, y0 + 64), TEX + "ballast")
    else:
        decal((x0 - 64, y0), (x0 + 64, y1), TEX + "ballast")


def wagon_model(x, y, along_x, name):
    """Place a wagon model; every other wagon is turned round (brake end)."""
    flip = 180 if (int(x + y) // 10) % 2 else 0
    entity("misc_model", x, y, 0, angle=(0 if along_x else 90) + flip, model=RAILM + name + ".md3")


def boxcar(x, y, along_x, color):
    """Covered wagon (boxcar_<color>.md3) with invisible collision for the
    bogies and the body."""
    L, W = 360, 104
    if along_x:
        box((x - L / 2 + 30, y - 40, 7), (x + L / 2 - 30, y + 40, 44), TEX + "propclip")
        box((x - L / 2, y - W / 2, 44), (x + L / 2, y + W / 2, 194), TEX + "propclip")
    else:
        box((x - 40, y - L / 2 + 30, 7), (x + 40, y + L / 2 - 30, 44), TEX + "propclip")
        box((x - W / 2, y - L / 2, 44), (x + W / 2, y + L / 2, 194), TEX + "propclip")
    wagon_model(x, y, along_x, f"boxcar_{color}")


def tankcar(x, y, along_x):
    """Tank wagon (tankcar.md3); invisible collision: bogie box and an
    octagonal prism around the barrel."""
    L = 340
    box(((x - L / 2 + 30, y - 40, 7) if along_x else (x - 40, y - L / 2 + 30, 7)),
        ((x + L / 2 - 30, y + 40, 44) if along_x else (x + 40, y + L / 2 - 30, 44)), TEX + "propclip")
    r, zc = 54, 104
    planes = []
    ax = (1, 0, 0) if along_x else (0, 1, 0)
    planes.append((ax, (x + ax[0] * L / 2, y + ax[1] * L / 2, zc), TEX + "propclip"))
    planes.append(((-ax[0], -ax[1], 0), (x - ax[0] * L / 2, y - ax[1] * L / 2, zc), TEX + "propclip"))
    for k in range(8):
        a = math.radians(22.5 + 45 * k)
        lat, up = math.cos(a), math.sin(a)
        n = (0, lat, up) if along_x else (lat, 0, up)
        planes.append((n, (x + n[0] * r, y + n[1] * r, zc + up * r), TEX + "propclip"))
    lconvex(planes)
    wagon_model(x, y, along_x, "tankcar")


def flatcar(x, y, along_x):
    """Flat wagon (flatcar.md3), deck top at z 40; invisible collision box."""
    if along_x:
        box((x - 150, y - 40, 7), (x + 150, y + 40, 40), TEX + "propclip")
    else:
        box((x - 40, y - 150, 7), (x + 40, y + 150, 40), TEX + "propclip")
    wagon_model(x, y, along_x, "flatcar")


def lamp_post(x, y, head_dx, head_dy):
    """Sodium street lamp (model, arm towards the head offset), invisible
    collision around the pole, light under the head."""
    hx, hy = x + head_dx, y + head_dy
    entity("misc_model", x, y, 0, angle=math.degrees(math.atan2(head_dy, head_dx)),
           model="models/mapobjects/q3r_lanterns/streetlamp.md3")
    box((x - 6, y - 6, 0), (x + 6, y + 6, 330), TEX + "boundary")
    entity("light", hx, hy, 290, light="320", _color="1 0.72 0.38")


def floodlight(x, y, aim):
    """Floodlight mast (floodmast.md3 scaled to 650 high) with its heads aimed
    at the point aim, invisible collision around the mast, light in front of
    the heads."""
    ang = math.atan2(aim[1] - y, aim[0] - x)
    entity("misc_model", x, y, 0, angle=math.degrees(ang), model="models/mapobjects/q3r_lanterns/floodmast.md3",
           modelscale="1.3")
    box((x - 14, y - 14, 0), (x + 14, y + 14, 560), TEX + "propclip")
    entity("light", x + 80 * math.cos(ang), y + 80 * math.sin(ang), 580, light="700", _color="0.95 0.95 1")


# ---------------------------------------------------------------- one quadrant (local NE)

def quadrant(team):
    rnd = random.Random(7)                  # same for every quadrant: fair cover
    paint = TEX + f"paint_{team}"
    # pier ground (concrete) with quay walls on the outer faces
    box((0, 0, SEABED), (PIER, PIER, 0), "common/caulk",
        faces={"top": TEX + "ground", "+x": TEX + "quaywall", "+y": TEX + "quaywall"})
    # quay coping, bollards
    box((0, PIER - 24, 0), (PIER, PIER, 20), TEX + "coping")
    box((PIER - 24, 0, 0), (PIER, PIER - 24, 20), TEX + "coping")
    for k in range(200, PIER, 400):
        box((k - 14, PIER - 60, 0), (k + 14, PIER - 32, 36), TEX + "bollard")
        box((PIER - 60, k - 14, 0), (PIER - 32, k + 14, 36), TEX + "bollard")
    # roads (paint): north edge road, east edge road, diagonal to the centre
    decal((0, 1900), (1650, 2400), TEX + "asphalt")
    decal((1900, 0), (2400, 1650), TEX + "asphalt")
    ddecal((330, 330), (1650, 1650), 210, TEX + "asphalt")
    for x in range(80, 1600, 220):
        decal((x, 2144), (x + 120, 2156), TEX + "line", z=1.0)
    for y in range(80, 1600, 220):
        decal((2144, y), (2156, y + 120), TEX + "line", z=1.0)
    for k in range(5):
        a = 460 + k * 230
        ddecal((a, a), (a + 110, a + 110), 6, TEX + "line", z=1.0)

    # ---- base yard, L-shaped hall, flag nook
    b = BASE
    hall = {"default": TEX + "hallwall", "top": TEX + "roof"}
    box((1700, 2440, 0), (2700, 2700, 360), TEX + "hallwall",
        faces={"-y": (TEX + "hallwin", 256, 128, 256, 128), "top": TEX + "roof"})
    box((2440, 1700, 0), (2700, 2440, 360), TEX + "hallwall",
        faces={"-x": (TEX + "hallwin", 256, 128, 256, 128), "top": TEX + "roof"})
    # team-coloured gates on the nook, a canopy over the flag
    box((2100, 2436, 0), (2400, 2440, 220), TEX + "gate", faces={"-y": (paint + "_gate", 220, 220, 256, 256)},
        solid=False)
    box((2436, 2100, 0), (2440, 2400, 220), TEX + "gate", faces={"-x": (paint + "_gate", 220, 220, 256, 256)},
        solid=False)
    box((2080, 2080, 300), (2440, 2440, 320), paint, faces={"bot": TEX + "metal"})
    for px, py in ((2090, 2090), (2090, 2420), (2420, 2090)):
        box((px - 10, py - 10, 0), (px + 10, py + 10, 300), TEX + "metal")
    decal((b - 140, b - 140), (b + 140, b + 140), paint + "_floor")
    # landmark on the hall corner (identity differs, footprint is the same)
    landmark(team)
    # floodlights at the yard corners, low walls along the quays
    floodlight(1690, 2410, (b, b))
    floodlight(2410, 1690, (b, b))
    # team pickups and spawns
    ang = 225
    for u in (-110, 110):
        entity(f"team_CTF_{team}player", b - 220 + u, b - 220 - u, 48, angle=ang)
    for x, y in ((1840, 2300), (2300, 1840), (1950, 1980), (1980, 1950)):
        entity(f"team_CTF_{team}spawn", x, y, 48, angle=ang)
    entity(f"team_CTF_{team}flag", b, b, 24)
    entity("info_player_deathmatch", 1800, 1800, 48, angle=ang)
    for cls, x, y in (("weapon_rocketlauncher", 1760, 2140), ("ammo_rockets", 2140, 1760),
                      ("item_health_large", 1960, 2330), ("item_fuelcan", 2330, 1960),
                      ("rally_item_turbo", 1700, 1700)):
        entity(cls, x, y, 24)

    # ---- north quay strip: gantry crane over container stacks
    crane(team)
    colors = len(CONTAINER_COLORS)
    for x0, x1 in ((140, 840), (1440, 1640)):
        x = x0 + 120
        while x + 120 <= x1:
            for row, y in enumerate((2470, 2575, 2680)):
                levels = 1 + (rnd.random() < 0.55) + (rnd.random() < 0.2)
                for lv in range(levels):
                    container(x, y, True, lv, rnd.randrange(colors))
            x += 262
    # ---- east quay strip: siding with wagons
    track(2530, 120, 2530, 1640)
    track(2680, 120, 2680, 1640)
    boxcar(2680, 340, False, "brown")
    boxcar(2680, 720, False, "grey")
    tankcar(2680, 1120, False)
    boxcar(2530, 1450, False, "brown")

    # ---- warehouse with a roof ramp (inner side of the north road)
    roof_z = 220
    box((300, 1480, 0), (1250, 1860, roof_z), TEX + "brick",
        faces={"+y": (TEX + "brickwin", 190, roof_z, 256, 256), "top": TEX + "roofdeck"})
    for x in range(330, 1230, 190):
        box((x, 1860, 0), (x + 130, 1864, 150), TEX + "brick",
            faces={"+y": (TEX + "garage", 130, 150, 256, 256)}, solid=False)
    # parapet on the roof (open to the north road and the ramp)
    box((300, 1480, roof_z), (700, 1496, roof_z + 18), TEX + "coping")
    box((960, 1480, roof_z), (1250, 1496, roof_z + 18), TEX + "coping")
    box((1234, 1496, roof_z), (1250, 1860, roof_z + 18), TEX + "coping")
    box((300, 1496, roof_z), (316, 1860, roof_z + 18), TEX + "coping")
    lhexa([(700, 1080, 0), (960, 1080, 0), (960, 1080, 1), (700, 1080, 1),
           (700, 1480, 0), (960, 1480, 0), (960, 1480, roof_z), (700, 1480, roof_z)],
          {"default": TEX + "concrete", 4: TEX + "ramp"})
    entity("weapon_railgun", 775, 1680, roof_z + 24)
    entity("ammo_slugs", 830, 1010, 24)

    # ---- tank farm (inner side of the east road) with a pipe bridge
    for y in (450, 900, 1350):
        octagon(1665, y, 150, 0, 260, TEX + "tank", top=TEX + "tanktop", tile=(124, 124, 1024, 1024))
        box((1585, y - 8, 260), (1745, y + 8, 300), TEX + "metal")      # top walkway
    box((1470, 300, 180), (1510, 1500, 214), TEX + "pipes")
    for y in (330, 675, 1125, 1470):
        box((1480, y - 10, 0), (1500, y + 10, 180), TEX + "metal")
    entity("weapon_shotgun", 1665, 675, 24)
    entity("ammo_shells", 1665, 1125, 24)
    entity("rally_weapon_bio", 2455, 1000, 24)

    # ---- container maze (between the diagonal road and the warehouse)
    maze = ((300, 760, False, 2), (300, 1010, False, 1), (470, 1240, True, 2), (470, 1340, True, 1),
            (430, 920, True, 1), (300, 1300, False, 2), (560, 1100, False, 2))
    for x, y, ax, lv in maze:
        for k in range(lv):
            container(x, y, ax, k, (int(x + y) // 7 + k) % colors)
    entity("weapon_grenadelauncher", 430, 1060, 24)
    entity("ammo_grenades", 380, 1150, 24)

    # ---- small rail yard (between the diagonal road and the tank farm)
    track(720, 380, 1400, 380)
    track(720, 560, 1400, 560)
    boxcar(1150, 560, True, "red")
    for x in (950, 1250):
        flatcar(x, 380, True)
        container(x, 380, True, 0, (x // 50) % colors, z0=40)
    entity("weapon_plasmagun", 1000, 470, 24)
    entity("ammo_cells", 1290, 470, 24)

    # ---- arm of the elevated cross (north) and its ramp
    arm()
    entity("item_armor_yellow", 0, 700, DECK + 24)

    # ---- neutral pickups on the roads
    entity("rally_weapon_oil", 900, 900, 24)
    entity("item_health", 1000, 2150, 24)
    entity("rally_weapon_smoke", 1140, 2560, 24)
    entity("rally_weapon_mine", 2150, 1000, 24)

    # ---- street lamps
    for x in (250, 750, 1250):
        lamp_post(x, 1880, 0, 90)
    for y in (250, 750, 1250):
        lamp_post(1880, y, 90, 0)
    for k in (600, 1200):
        entity("light", k, k, 300, light="300", _color="1 0.72 0.38")     # overhead lights
    # crowd of barrels and a dumpster next to the warehouse
    for x, y in ((1280, 1560), (1310, 1600), (1290, 1640)):
        entity("misc_model", x, y, 0, model="models/mapobjects/barrels/b_oil.md3", angle=rnd.randrange(360))
        box((x - 18, y - 18, 0), (x + 18, y + 18, 54), TEX + "clip", solid=True)

    # ---- moored ship and the water ring segment, far shore
    water_and_shore()


def landmark(team):
    """Tall identity structure on the hall corner (2440..2700)."""
    x0, x1 = 2470, 2670
    cx = cy = (x0 + x1) / 2
    if team == "red":            # steel works: two brick chimneys
        for dx in (-50, 50):
            octagon(cx + dx, cy - dx, 44, 360, 1100, TEX + "chimney", top=TEX + "soot")
            octagon(cx + dx, cy - dx, 52, 1040, 1070, TEX + "paint_red")
        entity("misc_model", cx, cy, 1100, model="models/mapobjects/barrels/b_default.md3")
    elif team == "blue":         # container terminal: stacked containers and a radar mast
        for lv in range(4):
            container(cx, cy - 60 + (lv % 2) * 120, lv % 2 == 0, 0, 5 if lv % 2 else 2, z0=360 + lv * 104)
        box((cx - 6, cy - 6, 776), (cx + 6, cy + 6, 1060), TEX + "metal")
    elif team == "green":        # rail depot: water tower
        for dx in (-70, 70):
            for dy in (-70, 70):
                box((cx + dx - 8, cy + dy - 8, 360), (cx + dx + 8, cy + dy + 8, 780), TEX + "metal")
        octagon(cx, cy, 110, 780, 960, TEX + "paint_green", top=TEX + "roof")
        octagon(cx, cy, 60, 960, 1010, TEX + "paint_green", top=TEX + "roof")
    else:                        # bulk terminal: two silos with a conveyor
        octagon(cx - 40, cy + 40, 72, 360, 960, TEX + "silo", top=TEX + "paint_yellow")
        octagon(cx + 40, cy - 40, 72, 360, 960, TEX + "silo", top=TEX + "paint_yellow")
        lhexa([(2300, 2300, 520), (2300, 2300, 540), (2300, 2340, 540), (2300, 2340, 520),
               (cx, cy, 940), (cx, cy, 960), (cx, cy + 40, 960), (cx, cy + 40, 940)],
              TEX + "paint_yellow", solid=False)
    box((x0, x0, 360), (x1, x1, 370), TEX + "roof")


def crane(team):
    """Rail-mounted gantry crane straddling the north road and quay; boom over the water."""
    paint = TEX + "crane"
    for x in (900, 1380):
        for y in (1840, 2720):
            box((x - 18, y - 18, 0), (x + 18, y + 18, 720), paint)
        box((x - 18, 1840, 690), (x + 18, 2720, 740), paint)                      # portal beam
        # diagonal braces
        lhexa([(x - 8, 1858, 120), (x + 8, 1858, 120), (x + 8, 1858, 150), (x - 8, 1858, 150),
               (x - 8, 2280, 690), (x + 8, 2280, 690), (x + 8, 2280, 720), (x - 8, 2280, 720)], paint)
        lhexa([(x - 8, 2702, 120), (x + 8, 2702, 120), (x + 8, 2702, 150), (x - 8, 2702, 150),
               (x - 8, 2280, 690), (x + 8, 2280, 690), (x + 8, 2280, 720), (x - 8, 2280, 720)], paint)
    for x in (1020, 1250):
        box((x - 15, 1700, 740), (x + 15, 3900, 800), paint)                       # girders / boom
    for y in (1760, 2280, 2900, 3400):
        box((1035, y - 12, 740), (1235, y + 12, 790), paint)
    box((1030, 1900, 800), (1240, 2200, 940), TEX + "cranehouse")
    box((1070, 2850, 690), (1200, 2950, 740), TEX + "cranehouse")               # trolley cabin
    box((1110, 2880, 300), (1160, 2920, 690), TEX + "metal", solid=False)          # hoist cable bundle
    box((1060, 2840, 260), (1210, 2960, 300), paint, solid=False)                  # spreader
    for x in (900, 1380):                                                          # crane rails
        decal((x - 30, 1840), (x + 30, 2760), TEX + "railplate", z=1.0)
    entity("light", 1135, 2280, 680, light="500", _color="1 0.85 0.6")


def arm():
    """North arm of the elevated cross: deck on pillars and a ramp down to the edge road."""
    w = 192
    deck = {"top": TEX + "deck"}
    box((-w, w, DECK - 28), (w, 1060, DECK), TEX + "concrete", faces=deck)
    for side in (-1, 1):
        x0, x1 = sorted((side * w, side * (w - 16)))
        box((x0, w, DECK), (x1, 1060, DECK + 30), TEX + "railing")
        lhexa([(x0, 1060, DECK), (x1, 1060, DECK), (x1, 1060, DECK + 30), (x0, 1060, DECK + 30),
               (x0, 1880, 0), (x1, 1880, 0), (x1, 1880, 30), (x0, 1880, 30)], TEX + "railing")
    lhexa([(-w, 1060, 0), (w, 1060, 0), (w, 1060, DECK), (-w, 1060, DECK),
           (-w, 1880, 0), (w, 1880, 0), (w, 1880, 1), (-w, 1880, 1)],
          {"default": TEX + "concrete", 4: TEX + "deck"})
    for y in (560, 900):
        for x in (-140, 140):
            box((x - 22, y - 22, 0), (x + 22, y + 22, DECK - 28), TEX + "concrete")
    entity("light", 0, 600, DECK - 60, light="260", _color="1 0.72 0.38")


def water_and_shore():
    """Water ring segment (pinwheel), a moored ship, the far shore skyline."""
    rnd = random.Random(31)
    # water and seabed: x -PIER..SEA, y PIER..SEA in local coordinates
    box((-PIER, PIER, SEABED - 64), (SEA, SEA, SEABED), TEX + "seabed", faces={"top": TEX + "seabed"})
    box((-PIER, PIER, SEABED), (SEA, SEA, WATER_Z), TEX + "water", solid=False)
    # ship alongside the north quay
    hull = TEX + "hull"
    y0, y1 = 2960, 3460
    lhexa([(150, y0, -200), (2350, y0, -200), (2350, y0, 150), (150, y0, 150),
           (150, y1, -200), (2350, y1, -200), (2350, y1, 150), (150, y1, 150)],
          {"default": hull, 4: TEX + "shipdeck"})
    lconvex([((0, 0, -1), (2600, 3210, -200), hull), ((0, 0, 1), (2600, 3210, 150), TEX + "shipdeck"),
             ((-1, 0, 0), (2350, 3210, 0), hull), ((0.64, 0.77, 0), (2650, 3210, 0), hull),
             ((0.64, -0.77, 0), (2650, 3210, 0), hull)])
    box((180, y0 + 40, 150), (520, y1 - 40, 470), TEX + "shiphouse",
        faces={"+x": (TEX + "hallwin", 200, 100, 256, 128)})
    box((300, 3180, 470), (360, 3240, 640), TEX + "chimney_ship")
    colors = len(CONTAINER_COLORS)
    for x in range(700, 2200, 250):
        for y in (3040, 3150, 3260, 3370):
            for lv in range(1 + (rnd.random() < 0.6) + (rnd.random() < 0.4)):
                container(x, y, True, lv, rnd.randrange(colors), z0=150)
    # far shore: quay, warehouses, chimneys and cranes
    box((-PIER, SEA, SEABED - 64), (SEA, SEA + 1400, 24), "common/caulk",
        faces={"top": TEX + "ground", "-y": TEX + "quaywall"})
    x = -PIER + 100
    while x < SEA - 200:
        w = rnd.randrange(300, 900)
        h = rnd.randrange(260, 1100)
        d = rnd.randrange(300, 900)
        y = SEA + 80 + rnd.randrange(0, 300)
        tex = rnd.choice((TEX + "farwin", TEX + "farbrick", TEX + "hallwall"))
        box((x, y, 24), (x + w, y + d, 24 + h), tex, faces={"top": TEX + "roof"}, solid=False, scale=1.0)
        if rnd.random() < 0.35:
            octagon(x + w / 2, y + d / 2, 40, 24 + h, 24 + h + rnd.randrange(400, 900), TEX + "chimney",
                    top=TEX + "soot", solid=False)
        x += w + rnd.randrange(60, 300)
    for x in (600, 2400, 4200):
        for y in (SEA + 20, SEA + 220):
            box((x - 16, y - 16, 24), (x + 16, y + 16, 760), TEX + "crane", solid=False)
            box((x + 300 - 16, y - 16, 24), (x + 300 + 16, y + 16, 760), TEX + "crane", solid=False)
        box((x - 16, SEA - 900, 760), (x + 316, SEA + 240, 820), TEX + "crane", solid=False)


def centre():
    """Platform where the four arms meet, the plaza under it."""
    w = 192
    box((-w, -w, DECK - 28), (w, w, DECK), TEX + "concrete", faces={"top": TEX + "deck"})
    for x in (-150, 150):
        for y in (-150, 150):
            box((x - 26, y - 26, 0), (x + 26, y + 26, DECK - 28), TEX + "concrete")
    box((-300, -300, 0), (300, 300, 1), "common/nodraw", faces={"top": TEX + "asphalt"}, solid=False)
    entity("rally_item_quad", 0, 0, DECK + 24)
    entity("item_armor_red", 0, 0, 24)
    entity("light", 0, 0, 160, light="300", _color="1 0.72 0.38")


def shell():
    m, top = 64, 2400
    ext = SEA + 1600
    sky = TEX + "sky"
    for lo, hi, mat in (((-ext - m, -ext - m, SEABED - 128), (ext + m, ext + m, SEABED - 64), "common/caulk"),
                        ((-ext - m, -ext - m, SEABED - 64), (-ext, ext + m, top), sky),
                        ((ext, -ext - m, SEABED - 64), (ext + m, ext + m, top), sky),
                        ((-ext, -ext - m, SEABED - 64), (ext, -ext, top), sky),
                        ((-ext, ext, SEABED - 64), (ext, ext + m, top), sky),
                        ((-ext - m, -ext - m, top), (ext + m, ext + m, top + m), sky)):
        (ax, ay, az), (bx, by, bz) = lo, hi
        brush([face(plane(n, p), mat, detail=False) for n, p in
               (((0, 0, -1), (ax, ay, az)), ((0, 0, 1), (bx, by, bz)), ((-1, 0, 0), (ax, ay, az)),
                ((1, 0, 0), (bx, by, bz)), ((0, -1, 0), (ax, ay, az)), ((0, 1, 0), (bx, by, bz)))])
    # invisible walls along the quays keep the cars on the pier
    for lo, hi in (((-PIER - 40, -PIER - 40, -100), (-PIER, PIER + 40, 1400)),
                   ((PIER, -PIER - 40, -100), (PIER + 40, PIER + 40, 1400)),
                   ((-PIER, -PIER - 40, -100), (PIER, -PIER, 1400)),
                   ((-PIER, PIER, -100), (PIER, PIER + 40, 1400))):
        brush([face(plane(n, p), TEX + "boundary") for n, p in
               (((0, 0, -1), lo), ((0, 0, 1), hi), ((-1, 0, 0), lo), ((1, 0, 0), hi), ((0, -1, 0), lo),
                ((0, 1, 0), hi))])
    brush([face(plane(n, p), "common/lightgrid") for n, p in
           (((0, 0, -1), (-PIER, -PIER, -100)), ((0, 0, 1), (PIER, PIER, 1200)), ((-1, 0, 0), (-PIER, 0, 0)),
            ((1, 0, 0), (PIER, 0, 0)), ((0, -1, 0), (0, -PIER, 0)), ((0, 1, 0), (0, PIER, 0)))])


# ---------------------------------------------------------------- shaders

def lit(name, image, extra=(), stage_extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *stage_extra, "\t\trgbGen identity" if not any("rgbGen" in s for s in stage_extra) else "", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}", "}", ""]


def tinted(name, image, rgb, extra=()):
    return lit(name, image, extra, (f"\t\trgbGen const ( {rgb[0]:.2f} {rgb[1]:.2f} {rgb[2]:.2f} )",))


def world(name, image, tile, extra=(), rgb=None):
    """World-aligned ground texture (no seams between brushes)."""
    k = 1.0 / tile
    st = [f"\t\ttcGen vector ( {k:.6f} 0 0 ) ( 0 {k:.6f} 0 )"]
    if rgb:
        st.append(f"\t\trgbGen const ( {rgb[0]:.2f} {rgb[1]:.2f} {rgb[2]:.2f} )")
    return lit(name, image, extra, tuple(st))


def decal_shader(name, image, extra=(), tile=None, rgb=None):
    st = []
    if tile:
        st.append(f"\t\ttcGen vector ( {1 / tile:.6f} 0 0 ) ( 0 {1 / tile:.6f} 0 )")
    if rgb:
        st.append(f"\t\trgbGen const ( {rgb[0]:.2f} {rgb[1]:.2f} {rgb[2]:.2f} )")
    return lit(name, image, ("\tsurfaceparm nonsolid", "\tsurfaceparm nomarks", "\tsurfaceparm trans",
                             "\tpolygonOffset", *extra), tuple(st))


def glow(name, rgb, light):
    return [f"textures/{TEX}{name}", "{", "\tqer_editorimage textures/q3r_signage/white.jpg",
            "\tsurfaceparm nomarks", "\tsurfaceparm nolightmap", f"\tq3map_surfacelight {light}",
            f"\tq3map_lightRGB {rgb[0]} {rgb[1]} {rgb[2]}", "\t{", "\t\tmap textures/q3r_signage/white.jpg",
            f"\t\trgbGen const ( {rgb[0]} {rgb[1]} {rgb[2]} )", "\t}", "}", ""]


def write_textures():
    """Ballast bed for the tracks (256 px = 128 units): grey-brown stones."""
    from PIL import Image, ImageFilter
    out = ROOT / "baseq3r" / "textures" / "q3r_ctf4_03"
    out.mkdir(parents=True, exist_ok=True)
    rnd = random.Random(5)
    size = 256
    img = Image.new("RGB", (size, size))
    px = img.load()
    for y in range(size):
        for x in range(size):
            g = rnd.randint(62, 122)
            px[x, y] = (g, int(g * 0.95), int(g * 0.88))
    img = img.filter(ImageFilter.GaussianBlur(0.7))
    px = img.load()
    for _ in range(900):                       # stones: light top, dark rim
        cx, cy, r = rnd.randrange(size), rnd.randrange(size), rnd.uniform(2.5, 5.5)
        tone = rnd.uniform(0.85, 1.2)
        for dy in range(-6, 7):
            for dx in range(-6, 7):
                d = math.hypot(dx, dy * 1.2) / r
                if d < 1:
                    p = ((cx + dx) % size, (cy + dy) % size)
                    k = tone * (1.08 - 0.25 * d) * (0.92 if dy > r * 0.4 else 1.0)
                    v = px[p]
                    px[p] = tuple(min(255, int(c * k)) for c in v)
    img.save(out / "ballast.jpg", quality=90)


def write_shader():
    out = ["// Industrial Crossfire - four-team CTF in a harbour at dusk.",
           "// Written by tools/maptools/make_industrialcrossfire.py. Compile with -custinfoparms.", ""]
    out += [f"textures/{TEX}sky", "{", "\tqer_editorimage env/ely_sunset/sunset_ft.tga", "\tsurfaceparm noimpact",
            "\tsurfaceparm nolightmap", "\tsurfaceparm sky", "\tq3map_globaltexture",
            f"\tq3map_sun 1 0.62 0.36 150 {SUN_YAW} 14", "\tq3map_skylight 70 4",
            f"\tq3gl2_sun 1 0.62 0.36 110 {SUN_YAW} 14 0.6", "\tskyparms env/ely_sunset/sunset - -",
            "\tq3r_haze 0.42 0.36 0.38 5200\t// Q3Rally: harbour dusk haze", "}", ""]
    out += world("ground", "textures/q3r_road/concrete.jpg", 192, ("\tq3map_nonplanar",))
    out += decal_shader("asphalt", "textures/q3r_road/asph02.jpg", ("\tsurfaceparm SURF_ASPHALT",), tile=256,
                        rgb=(0.50, 0.50, 0.53))
    out += decal_shader("line", "textures/q3r_signage/white.jpg", rgb=(0.85, 0.80, 0.55))
    out += decal_shader("ballast", f"textures/{TEX}ballast.jpg", tile=128)
    out += decal_shader("railplate", "textures/q3r_metals/metrust01.tga")
    for team, rgb in COLORS.items():
        out += tinted(f"paint_{team}", "textures/q3r_building/wellblech.jpg", rgb, ("\tsurfaceparm SURF_METAL",))
        out += tinted(f"paint_{team}_gate", "textures/q3r_building/garage_np.jpg",
                      tuple(min(1.0, 0.45 + c * 0.7) for c in rgb))
        out += decal_shader(f"paint_{team}_floor", "textures/q3r_signage/white.jpg", rgb=rgb)
    for k, rgb in enumerate(CONTAINER_COLORS):
        out += tinted(f"cont{k}", "textures/q3r_building/wellblech.jpg", rgb, ("\tsurfaceparm SURF_METAL",))
        # door ends: the same corrugation, a little darker
        out += tinted(f"cont{k}_end", "textures/q3r_building/wellblech.jpg", tuple(c * 0.78 for c in rgb),
                      ("\tsurfaceparm SURF_METAL",))
    out += lit("controof", "textures/q3r_metals/metrust01.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("metal", "textures/base_trim/tin.jpg", ("\tsurfaceparm SURF_METAL",))
    out += tinted("crane", "textures/q3r_metals/cormetal03.tga", (0.95, 0.62, 0.15), ("\tsurfaceparm SURF_METAL",))
    out += tinted("cranehouse", "textures/q3r_building/wellblech.jpg", (0.92, 0.88, 0.80), ("\tsurfaceparm SURF_METAL",))
    out += tinted("tank", "textures/q3r_building/wellblech.jpg", (0.86, 0.86, 0.82), ("\tsurfaceparm SURF_METAL",))
    out += lit("tanktop", "textures/base_floor/diamond2.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("pipes", "textures/q3r_metals/rustblumet01.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("hallwall", "textures/q3r_building/concrete01.jpg")
    out += lit("hallwin", "textures/q3r_building/ind-windows-1.jpg")
    out += lit("gate", "textures/q3r_building/garage_np.jpg")
    out += lit("garage", "textures/q3r_building/garage_np.jpg")
    out += lit("roof", "textures/q3r_metals/cormetal01.tga", ("\tsurfaceparm SURF_METAL",))
    out += lit("roofdeck", "textures/base_floor/diamond2.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("brick", "textures/q3r_building/brick4.jpg")
    out += lit("brickwin", "textures/q3r_building/externalwall50a.jpg")
    out += lit("farwin", "textures/q3r_building/ind-windows-1.jpg", ("\tq3map_lightmapSampleSize 64",))
    out += lit("farbrick", "textures/q3r_building/externalwall50a.jpg", ("\tq3map_lightmapSampleSize 64",))
    out += lit("concrete", "textures/q3r_building/concrete02.jpg")
    out += lit("deck", "textures/q3r_road/asph02.jpg", ("\tsurfaceparm SURF_ASPHALT",),
               ("\t\trgbGen const ( 0.55 0.55 0.58 )",))
    out += lit("ramp", "textures/base_floor/diamond2.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("railing", "textures/base_trim/yellow_rustbx128.jpg")
    out += lit("coping", "textures/q3r_road/concrete2.jpg")
    out += tinted("bollard", "textures/q3r_metals/Rost.tga", (0.30, 0.30, 0.32), ("\tsurfaceparm SURF_METAL",))
    out += lit("quaywall", "textures/q3r_building/bricks03.jpg")
    out += lit("seabed", "textures/q3r_road/concrete.jpg", ("\tq3map_lightmapSampleSize 64",),
               ("\t\trgbGen const ( 0.25 0.27 0.25 )",))
    out += lit("chimney", "textures/q3r_building/brick4.jpg")
    out += lit("chimney_ship", "textures/q3r_metals/Rost.tga")
    out += tinted("soot", "textures/q3r_road/concrete.jpg", (0.12, 0.11, 0.10))
    out += lit("silo", "textures/q3r_building/concrete03.jpg")
    out += tinted("hull", "textures/q3r_metals/metrust01.tga", (0.55, 0.22, 0.18), ("\tsurfaceparm SURF_METAL",))
    out += lit("shipdeck", "textures/base_floor/diamond2.jpg", ("\tsurfaceparm SURF_METAL",))
    out += tinted("shiphouse", "textures/q3r_building/concrete01.jpg", (0.95, 0.95, 0.92))
    out += [f"textures/{TEX}clip", "{", "\tqer_trans 0.3", "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap",
            "\tsurfaceparm trans", "\tsurfaceparm nomarks", "\tsurfaceparm detail", "}", ""]
    out += [f"textures/{TEX}propclip", "{", "\tqer_trans 0.15",
            "\t// invisible solid hull of a model (wagons, masts): stops cars and shots, lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm detail", "\tsurfaceparm SURF_METAL", "}", ""]
    out += [f"textures/{TEX}boundary", "{", "\tqer_trans 0.15",
            "\t// invisible but SOLID (Bullet only takes solid brushes); trans lets light through",
            "\tsurfaceparm nodraw", "\tsurfaceparm nolightmap", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm noimpact", "\tsurfaceparm detail", "}", ""]
    out += [f"textures/{TEX}water", "{", "\tqer_editorimage textures/q3r_liquids/water.tga", "\tqer_trans 0.35",
            "\tsurfaceparm water", "\tsurfaceparm nonsolid", "\tsurfaceparm trans", "\tsurfaceparm nomarks",
            "\tsurfaceparm nolightmap", "\tq3map_globaltexture", "\tcull disable",
            "\tq3r_water textures/q3r_liquids/water_normal.tga 320 0.02 0.018", "\tq3r_waterColor 0.06 0.08 0.10 0.72",
            "\tq3r_waterFresnel 0.08 4", "\tq3r_waterSpecular 1.0",
            "\t{", "\t\tmap textures/q3r_liquids/water.tga", "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA",
            "\t\trgbGen const ( 0.20 0.26 0.30 )", "\t\talphaGen const 0.75", "\t\ttcMod scale 0.3 0.3",
            "\t\ttcMod scroll 0.006 0.009", "\t}", "\t{", "\t\tmap textures/q3r_liquids/water.tga",
            "\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\trgbGen const ( 0.62 0.50 0.42 )",
            "\t\talphaGen const 0.15", "\t\ttcMod scale 0.37 0.37", "\t\ttcMod scroll -0.005 0.007", "\t}", "}", ""]
    (ROOT / "baseq3r" / "scripts" / f"{NAME}.shader").write_text("\n".join(l for l in out if l != "" or True),
                                                                 encoding="utf-8")


SUN_YAW = 0


# ---------------------------------------------------------------- checks and output

def validate():
    starts = [e for e in ENTITIES if e["classname"].endswith(("player", "spawn"))
              or e["classname"] == "info_player_deathmatch"]
    for e in starts:
        x, y, z = map(float, e["origin"].split())
        for lo, hi in SOLIDS:
            assert not (x + 60 > lo[0] and x - 60 < hi[0] and y + 60 > lo[1] and y - 60 < hi[1]
                        and z + 30 > lo[2] and z - 30 < hi[2]), (e, lo, hi)
    items = [e for e in ENTITIES if e["classname"].startswith(("weapon_", "ammo_", "item_", "rally_", "team_CTF_"))
             and not e["classname"].endswith(("player", "spawn"))]
    for e in items:
        x, y, z = map(float, e["origin"].split())
        for lo, hi in SOLIDS:
            assert not (x + 24 > lo[0] and x - 24 < hi[0] and y + 24 > lo[1] and y - 24 < hi[1]
                        and z + 16 > lo[2] and z - 16 < hi[2]), (e, lo, hi)
    flags = [e for e in ENTITIES if e["classname"].endswith("flag")]
    pos = [tuple(map(float, f["origin"].split()[:2])) for f in flags]
    d_neighbour = math.dist(pos[0], pos[1])
    d_opposite = math.dist(pos[0], pos[2])
    return {"map": NAME, "flags": len(flags), "flag_to_neighbour": round(d_neighbour),
            "flag_to_opposite": round(d_opposite), "brushes": len(BRUSHES),
            "entities": len(ENTITIES), "spawns_per_team": sum(1 for e in starts if "blue" in e["classname"]),
            "pickups": len(items) - len(flags)}


def write_map():
    fields = {"classname": "worldspawn", "message": "Industrial Crossfire", "_ambient": "14", "_minlight": "10",
              "gridsize": "96 96 128", "_blocksize": "1024 1024 4096",
              "weather": "rain", "weatherChance": "45", "weatherWind": "40 200"}
    text = ["// Generated by tools/maptools/make_industrialcrossfire.py. Q3 brush format.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    for ent in ENTITIES:
        text.append("{\n" + "\n".join(f'"{k}" "{v}"' for k, v in ent.items()) + "\n}")
    target = ROOT / "baseq3r" / "maps" / f"{NAME}.map"
    target.write_text("\n".join(text) + "\n", encoding="ascii")
    return target


def overview(report):
    """Top view of the solid footprints and the pickups."""
    from PIL import Image, ImageDraw
    size = 1100
    s = size / (2 * PIER + 400)
    im = Image.new("RGB", (size, size), (28, 44, 58))
    d = ImageDraw.Draw(im)

    def P(x, y):
        return ((x + PIER + 200) * s, (PIER + 200 - y) * s)
    d.rectangle([P(-PIER, PIER), P(PIER, -PIER)], fill=(120, 118, 112))
    for lo, hi in sorted(SOLIDS, key=lambda b: b[1][2]):
        if hi[2] <= 1 or lo[2] < -100 or abs(lo[0]) > PIER + 50 or abs(lo[1]) > PIER + 50:
            continue
        shade = int(min(235, 90 + hi[2] / 5))
        d.rectangle([P(lo[0], hi[1]), P(hi[0], lo[1])], fill=(shade, shade - 10, shade - 30))
    for e in ENTITIES:
        x, y, _ = map(float, e["origin"].split())
        c = e["classname"]
        u, v = P(x, y)
        if c.endswith("flag"):
            col = {"red": (230, 40, 40), "blue": (40, 90, 240), "green": (40, 200, 60), "yellow": (240, 210, 30)}
            d.ellipse([u - 9, v - 9, u + 9, v + 9], fill=col[c.split("_")[2][:-4]])
        elif c.startswith(("weapon_", "rally_weapon")):
            d.rectangle([u - 4, v - 4, u + 4, v + 4], fill=(230, 40, 40))
        elif c.startswith(("item_", "ammo_", "rally_item")):
            d.rectangle([u - 3, v - 3, u + 3, v + 3], fill=(250, 170, 0))
    out = ROOT / "docs" / "maps" / "industrialcrossfire"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "industrialcrossfire_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def main():
    global ROT
    for ROT, team in enumerate(TEAMS):
        quadrant(team)
    ROT = 0
    centre()
    shell()
    entity("info_player_intermission", -1400, -1400, 1300, angles="28 45 0")
    report = validate()
    write_textures()
    write_shader()
    target = write_map()
    overview(report)
    print(json.dumps(report, indent=2))
    print("Wrote", target)


if __name__ == "__main__":
    main()
