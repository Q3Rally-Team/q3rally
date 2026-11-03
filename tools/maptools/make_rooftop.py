#!/usr/bin/env python3
"""Generate q3r_rooftop (Rooftop Arena): Autoball on a skyscraper roof at night.

A 4 vs 4 car football pitch on top of a tower in a neon city (skybox
env/ely_darkcity). Mirror symmetric (x: red west, blue east; y).

- Pitch 5120 x 3072 (goal lines at x = +-2560, side glass at y = +-1536) with
  chamfered corners, a dark rubber surface and glowing markings.
- Long sides: half-high glass boards (144). Above them invisible player clip
  keeps the cars on the roof; the ball is not stopped by it (the Bullet ball
  only collides with solid brushes). A ball shot over the glass drops onto the
  ledge or off the building: autoball_reset volumes put it back (new kick-off).
- Ends: two-storey structures (640 high) with the goals (1088 x 320), LED
  screens above them, team-coloured neon, two floodlight masts on each roof.
- Cable-cam rails on outriggers beyond the long edges: they also give the
  Autoball TV camera the side wall it looks for (see camera_rails).
- Around it: the tower's own facade going down 4000 units, a ring of lit
  towers, billboards and aircraft warning lights, streets far below. All of
  that is non-solid scenery.

Writes the map, scripts/q3r_rooftop.shader and .arena, the textures in
textures/q3r_rooftop/ and docs/maps/rooftop/ (layout and validation).
Brushes only. Run with Python 3 + numpy + Pillow.
"""
from __future__ import annotations

import json
import math
import random
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
NAME = "q3r_rooftop"
TEX = "q3r_rooftop/"
DETAIL = 134217728

X = 2560            # goal line
Y = 1536            # inner face of the side glass
C = 512             # corner chamfer (along each axis)
GW = 544            # half width of the goal mouth
GH = 320            # goal height
GD = 320            # goal depth
EH = 640            # height of the end structures
EX = 3328           # outer end of the building (x)
BY = 1888           # outer side of the building (y)
GLASS = 144         # height of the side glass
FIL = 48            # fillet at the foot of the walls (45 degrees)
STREET = -4000      # street level far below
BALL_R = 75

BRUSHES, ENTITIES, SOLIDS = [], [], []
TEAM = {1: "blue", -1: "red"}      # east end defended by blue
TEAM_RGB = {"red": (1.0, 0.16, 0.12), "blue": (0.12, 0.42, 1.0)}


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


def brush(faces, target=None):
    (target if target is not None else BRUSHES).append("{\n" + "\n".join(faces) + "\n}")


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


def convex(planes, solid=True, scale=0.5, detail=True, target=None):
    """(outward normal, point, material) planes."""
    brush([face(plane(n, p), m, scale=scale, detail=detail) for n, p, m in planes], target)
    if solid:
        corners = polytope([(n, p) for n, p, _ in planes])
        if corners:
            SOLIDS.append((tuple(min(c[i] for c in corners) for i in range(3)),
                           tuple(max(c[i] for c in corners) for i in range(3))))


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


NORMALS = {"bot": (0, 0, -1), "top": (0, 0, 1), "-x": (-1, 0, 0), "+x": (1, 0, 0), "-y": (0, -1, 0),
           "+y": (0, 1, 0)}


def box(lo, hi, mat, faces=None, solid=True, scale=0.5, detail=True, target=None):
    """Axial box. faces: {"+x": material or (image, tile_w, tile_h, img_w, img_h)
    fitted to that upright face, or ("tex", material, (xo, yo, ss, ts)); "top"/"bot"}."""
    x1, y1, z1 = (min(a, b) for a, b in zip(lo, hi))
    x2, y2, z2 = (max(a, b) for a, b in zip(lo, hi))
    assert x2 > x1 and y2 > y1 and z2 > z1, (lo, hi)
    out = []
    for key, n in NORMALS.items():
        p = (x2 if n[0] > 0 else x1, y2 if n[1] > 0 else y1, z2 if n[2] > 0 else z1)
        spec = (faces or {}).get(key, mat)
        if isinstance(spec, tuple) and spec[0] == "tex":
            out.append(face(plane(n, p), spec[1], detail=detail, tex=spec[2]))
        elif isinstance(spec, tuple):
            image, tw, th, iw, ih = spec
            right = (-n[1], n[0])
            left = (x1 if right[0] > 0 else x2 if right[0] < 0 else p[0],
                    y1 if right[1] > 0 else y2 if right[1] < 0 else p[1], 0.0)
            out.append(face(plane(n, p), image, detail=detail,
                            tex=fitted((float(n[0]), float(n[1]), 0.0), left, z2, tw, th, iw, ih)))
        else:
            out.append(face(plane(n, p), spec, scale=scale, detail=detail))
    brush(out, target)
    if solid:
        SOLIDS.append(((x1, y1, z1), (x2, y2, z2)))


def prism(xy, z0, z1, mat, top=None, bot=None, solid=True, target=None):
    """Upright convex prism over the polygon xy (counter-clockwise)."""
    planes = [((0, 0, 1), (xy[0][0], xy[0][1], z1), top or mat), ((0, 0, -1), (xy[0][0], xy[0][1], z0), bot or mat)]
    for (ax, ay), (bx, by) in zip(xy, xy[1:] + xy[:1]):
        n = (by - ay, -(bx - ax), 0.0)
        planes.append((n, (ax, ay, z0), mat))
    convex(planes, solid=solid, target=target)


def wedge(p0, p1, inward, h, mat, solid=True):
    """Fillet along the wall line p0-p1: a 45-degree ramp of height h rising
    towards the wall; inward is the unit vector from the wall into the pitch."""
    ix, iy = inward
    a0 = (p0[0] + ix * h, p0[1] + iy * h)
    nz = (0.0, 0.0, -1.0)
    slope = (ix, iy, 1.0)
    d = (p1[0] - p0[0], p1[1] - p0[1])
    side0 = (-d[0], -d[1], 0.0)
    side1 = (d[0], d[1], 0.0)
    back = (-ix, -iy, 0.0)
    planes = [(nz, (p0[0], p0[1], 0), mat), (slope, (a0[0], a0[1], 0), mat), (back, (p0[0], p0[1], 0), mat),
              (side0, (p0[0], p0[1], 0), mat), (side1, (p1[0], p1[1], 0), mat)]
    convex(planes, solid=False)      # solid brush; left out of the spawn checks (the pitch outline covers it)


def entity(classname, x, y, z, **keys):
    fields = {"classname": classname, "origin": f"{round(x)} {round(y)} {round(z)}", **keys}
    ENTITIES.append(fields)
    return fields


def brush_entity(classname, brushes, **keys):
    ENTITIES.append({"classname": classname, **keys, "_brushes": brushes})


def xs(sx, a, b):
    return min(sx * a, sx * b), max(sx * a, sx * b)


# ---------------------------------------------------------------- the roof

def pitch():
    # one brush for the whole pitch; the line picture is fitted over it
    tex = (1024.0, 512.0, 2 * X / 2048.0, 2 * Y / 1024.0)
    box((-X, -Y, -32), (X, Y, 0), "common/caulk", faces={"top": ("tex", TEX + "pitch", tex)})
    # ledges along the sides, roof under the end structures
    for sy in (-1, 1):
        y0, y1 = sorted((sy * Y, sy * BY))
        box((-(X - C), y0, -32), (X - C, y1, 0), "common/caulk", faces={"top": TEX + "roof"})
    for sx in (-1, 1):
        x0, x1 = xs(sx, X, EX)
        box((x0, -BY, -32), (x1, BY, 0), "common/caulk", faces={"top": TEX + "roof"})
        for sy in (-1, 1):
            a0, a1 = xs(sx, X - C, X)
            b0, b1 = sorted((sy * Y, sy * BY))
            box((a0, b0, -32), (a1, b1, 0), "common/caulk", faces={"top": TEX + "roof"})


def side_glass():
    """Glass boards on the long sides, posts and rail outside, player clip above,
    fillet inside; parapet at the building edge."""
    L = X - C
    for sy in (-1, 1):
        g0, g1 = sorted((sy * Y, sy * (Y + 10)))
        box((-L, g0, 0), (L, g1, GLASS), TEX + "glass")
        r0, r1 = sorted((sy * (Y - 2), sy * (Y + 14)))
        box((-L, r0, GLASS), (L, r1, GLASS + 8), TEX + "rail")
        p0, p1 = sorted((sy * (Y + 10), sy * (Y + 24)))
        for x in range(-L, L + 1, 256):
            box((x - 6, p0, 0), (x + 6, p1, GLASS + 8), TEX + "rail")
        c0, c1 = sorted((sy * Y, sy * (Y + 24)))
        box((-L, c0, GLASS + 8), (L, c1, 2048), "common/clip", solid=False)
        wedge((-L, sy * Y), (L, sy * Y), (0, -sy), FIL, TEX + "fillet")
        # neon line along the foot of the glass (team colour by half)
        for sx in (-1, 1):
            a0, a1 = xs(sx, 0, L)
            n0, n1 = sorted((sy * (Y + 10), sy * (Y + 13)))
            box((a0, n0, 2), (a1, n1, 8), TEX + f"neon_{TEAM[sx]}", solid=False)
        # AC units and vents on the ledge (behind the glass)
        for x in (-1700, -900, 900, 1700):
            a0, a1 = sorted((sy * (Y + 90), sy * (Y + 230)))
            box((x - 110, a0, 0), (x + 110, a1, 96), TEX + "acunit", faces={"top": TEX + "grille"})
        for x in (-300, 300):
            a0, a1 = sorted((sy * (Y + 120), sy * (Y + 200)))
            box((x - 40, a0, 0), (x + 40, a1, 140), TEX + "rail")
        # parapet at the outer edge of the ledge
        q0, q1 = sorted((sy * (BY - 32), sy * BY))
        box((-L, q0, 0), (L, q1, 64), TEX + "concrete", faces={"top": TEX + "rail"})


def camera_rails():
    """Cable-cam rails on outriggers 600 units beyond each long edge of the roof.
    Besides the look they give the Autoball TV camera (cg_autoball.c) a side
    wall: it traces sideways from 150 above the goal centres (z 310) and puts
    the sideline camera 260 inside the first solid hit, here over the street,
    about 2200 out from the halfway line."""
    for sy in (-1, 1):
        r0, r1 = sorted((sy * (BY + 580), sy * (BY + 620)))
        box((-X, r0, 280), (X, r1, 340), TEX + "rail", faces={"top": TEX + "rail"})
        l0, l1 = sorted((sy * (BY + 576), sy * (BY + 580)))
        box((-X, l0, 300), (X, l1, 306), TEX + "neon_cyan", solid=False)
        for x in (-2048, 0, 2048):
            o0, o1 = sorted((sy * BY, sy * (BY + 580)))
            box((x - 16, o0, 296), (x + 16, o1, 336), TEX + "rail")
            # diagonal brace from the facade up to the rail
            k = sy
            corners = [(x - 10, k * BY, -300), (x + 10, k * BY, -300), (x + 10, k * BY, -260), (x - 10, k * BY, -260),
                       (x - 10, k * (BY + 560), 300), (x + 10, k * (BY + 560), 300), (x + 10, k * (BY + 560), 260),
                       (x - 10, k * (BY + 560), 260)]
            hexa(corners, TEX + "rail")
        # the camera dolly hanging from the rail, lens towards the pitch
        d0, d1 = sorted((sy * (BY + 560), sy * (BY + 640)))
        box((-60, d0, 200), (60, d1, 280), TEX + "acunit")
        e0, e1 = sorted((sy * (BY + 556), sy * (BY + 560)))
        box((-20, e0, 220), (20, e1, 250), TEX + "neon_red", solid=False)


def hexa(corners, mats, solid=True):
    """Convex 8-corner block: corners 0-3 one end (ring order), 4-7 the other."""
    c = [sum(p[i] for p in corners) / 8 for i in range(3)]
    quads = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
    planes = []
    for q in quads:
        p0, p1, p2 = corners[q[0]], corners[q[1]], corners[q[2]]
        n = cross(tuple(b - a for a, b in zip(p0, p1)), tuple(b - a for a, b in zip(p0, p2)))
        if sum(nc * (pc - cc) for nc, pc, cc in zip(n, p0, c)) < 0:
            n = tuple(-v for v in n)
        planes.append((n, p0, mats))
    convex(planes, solid=solid)


def end_structure(sx):
    team = TEAM[sx]
    con, wall = TEX + "concrete", TEX + "facade_" + team
    net, post = f"q3r_sundown/net_{team}", f"q3r_sundown/goalpost_{team}"
    # side blocks next to the chamfer (outside the glass) and the end block
    for sy in (-1, 1):
        a0, a1 = xs(sx, X - C, EX)
        b0, b1 = sorted((sy * Y, sy * BY))
        box((a0, b0, 0), (a1, b1, EH), con, faces={"top": TEX + "roof"})
        e0, e1 = xs(sx, X, EX)
        f0, f1 = sorted((sy * (GW + 32), sy * Y))
        box((e0, f0, 0), (e1, f1, EH), con, faces={("-x" if sx > 0 else "+x"): wall, "top": TEX + "roof"})
        # chamfer: triangle prism in the pitch corner
        tri = [(sx * (X - C), sy * Y), (sx * X, sy * (Y - C)), (sx * X, sy * Y)]
        if sx * sy < 0:
            tri = tri[::-1]
        prism(tri, 0, EH, wall, top=TEX + "roof", solid=False)    # solid; spawn checks use the pitch outline
        # fillets along the end wall and the chamfer
        wedge((sx * X, sy * (GW + 32)), (sx * X, sy * (Y - C)), (-sx, 0), FIL, TEX + "fillet")
        k = 1 / math.sqrt(2)
        wedge((sx * (X - C), sy * Y), (sx * X, sy * (Y - C)), (-sx * k, -sy * k), FIL, TEX + "fillet")
        # goal: post, side net
        p0, p1 = sorted((sy * GW, sy * (GW + 32)))
        x0, x1 = xs(sx, X, X + 32)
        box((x0, p0, 0), (x1, p1, GH + 32), post)
        n0, n1 = xs(sx, X + 32, X + GD + 32)
        box((n0, p0, 0), (n1, p1, GH), net)
    x0, x1 = xs(sx, X, X + 32)
    box((x0, -GW - 32, GH), (x1, GW + 32, GH + 32), post)                  # crossbar
    r0, r1 = xs(sx, X + 32, X + GD + 32)
    box((r0, -GW - 32, GH), (r1, GW + 32, GH + 32), net)                    # goal roof
    b0, b1 = xs(sx, X + GD, X + GD + 32)
    box((b0, -GW, 0), (b1, GW, GH), net)                                    # back net
    # above and behind the goal; the LED screen faces the pitch
    a0, a1 = xs(sx, X, EX)
    screen = (TEX + "ledscreen", 2 * GW + 64, EH - GH - 32, 1024, 256)
    box((a0, -GW - 32, GH + 32), (a1, GW + 32, EH), con,
        faces={("-x" if sx > 0 else "+x"): screen, "top": TEX + "roof"})
    c0, c1 = xs(sx, X + GD + 32, EX)
    box((c0, -GW - 32, 0), (c1, GW + 32, GH + 32), con)
    # goal floor (under the net roof): darker
    g0, g1 = xs(sx, X, X + GD)
    box((g0, -GW, -32), (g1, GW, 0), "common/caulk", faces={"top": TEX + "goalfloor"})
    # neon bands on the facade and the chamfers (not solid, 2 units proud)
    for z0 in (196, 600):
        for sy in (-1, 1):
            f0, f1 = sorted((sy * (GW + 40), sy * (Y - C)))
            n0, n1 = xs(sx, X - 2, X)
            box((n0, f0, z0), (n1, f1, z0 + 8), TEX + f"neon_{team}", solid=False)
    # rim light along the top of the end structure
    n0, n1 = xs(sx, X, X + 16)
    box((n0, -Y + C, EH), (n1, Y - C, EH + 6), TEX + f"neon_{team}", solid=False)
    # goal volume: one ball radius behind the line, to the back net
    v0, v1 = xs(sx, X + BALL_R, X + GD)
    goal = []
    box((v0, -GW, 0), (v1, GW, GH), "common/trigger", solid=False, target=goal)
    brush_entity("autoball_goal", goal, team=team)
    roof_dressing(sx)


def roof_dressing(sx):
    """On the end roof: stair house with EXIT light, antenna, two floodlight masts."""
    hx = sx * (EX - 260)
    x0, x1 = sorted((hx - 128, hx + 128))
    box((x0, -192, EH), (x1, 192, EH + 224), TEX + "concrete", faces={"top": TEX + "roof"})
    d0, d1 = xs(sx, EX - 260 - 130, EX - 260 - 128)
    box((d0, -48, EH), (d1, 48, EH + 128), TEX + "door", solid=False)
    box((d0, -40, EH + 140), (d1, 40, EH + 160), TEX + "exitsign", solid=False)
    # antenna with a blinking aircraft light
    ax = sx * (EX - 120)
    box((ax - 6, -6, EH), (ax + 6, 6, EH + 760), TEX + "rail")
    box((ax - 12, -12, EH + 760), (ax + 12, 12, EH + 784), TEX + "beacon", solid=False)
    # floodlight masts at the outer corners, aimed at the middle of the pitch
    for sy in (-1, 1):
        mx, my = sx * (EX - 220), sy * (BY - 220)
        aim = math.atan2(-my * 0.4, -mx)
        entity("misc_model", mx, my, EH, model="models/mapobjects/q3r_lanterns/floodmast.md3",
               angle=f"{math.degrees(aim) % 360:.0f}")
        target = f"flood_{'e' if sx > 0 else 'w'}{'n' if sy > 0 else 's'}"
        entity("light", mx + 80 * math.cos(aim), my + 80 * math.sin(aim), EH + 430, light="30000",
               _color="0.85 0.9 1", target=target, radius="1400")
        entity("info_null", -sx * 600, sy * 300, 0, targetname=target)
    # AC units along the end roof edge facing the pitch
    for sy in (-1, 1):
        for k in range(2):
            y = sy * (600 + 360 * k)
            u0, u1 = xs(sx, X + 120, X + 280)
            box((u0, y - 70, EH), (u1, y + 70, EH + 90), TEX + "acunit", faces={"top": TEX + "grille"})


def resets():
    """autoball_reset volumes: everything a ball can reach but no car. The game
    tests the ball centre against each entity's bounding box, so every volume
    is its own entity and axis aligned (the corner ones cover the triangle
    above the chamfer tops, from 620 up)."""
    vols = []
    big = 6000
    for sy in (-1, 1):
        y0, y1 = sorted((sy * (Y + 30), sy * (BY + big)))
        box((-EX - big, y0, STREET - 200), (EX + big, y1, 110), "common/trigger", solid=False, target=vols)
    for sx in (-1, 1):
        x0, x1 = xs(sx, X, EX + big)
        box((x0, -BY - big, EH - 20), (x1, BY + big, 6000), "common/trigger", solid=False, target=vols)
        x0, x1 = xs(sx, EX, EX + big)
        box((x0, -BY - big, STREET - 200), (x1, BY + big, EH - 20), "common/trigger", solid=False, target=vols)
        for sy in (-1, 1):
            a0, a1 = xs(sx, X - C, X)
            b0, b1 = sorted((sy * Y, sy * (BY + big)))
            box((a0, b0, EH - 20), (a1, b1, 6000), "common/trigger", solid=False, target=vols)
            tri = [(sx * (X - C), sy * Y), (sx * X, sy * (Y - C)), (sx * X, sy * Y)]
            if sx * sy < 0:
                tri = tri[::-1]
            prism(tri, EH - 20, 6000, "common/trigger", solid=False, target=vols)
    for v in vols:
        brush_entity("autoball_reset", [v])


# ---------------------------------------------------------------- the tower and the city

def tower_facade():
    """The arena tower below the roof: lit office floors, an LED crown band."""
    win = TEX + "win_a"
    for sy in (-1, 1):
        y0, y1 = sorted((sy * (BY - 32), sy * BY))
        box((-EX, y0, STREET), (EX, y1, -32), "common/nodraw",
            faces={"+y" if sy > 0 else "-y": win}, solid=False, scale=3.5)
        c0, c1 = sorted((sy * BY, sy * (BY + 4)))
        box((-EX, c0, -96), (EX, c1, -40), TEX + "neon_cyan", solid=False)
    for sx in (-1, 1):
        x0, x1 = xs(sx, EX - 32, EX)
        box((x0, -BY, STREET), (x1, BY, -32), "common/nodraw",
            faces={"+x" if sx > 0 else "-x": win}, solid=False, scale=3.5)
        c0, c1 = xs(sx, EX, EX + 4)
        box((c0, -BY, -96), (c1, BY, -40), TEX + "neon_cyan", solid=False)


def city():
    """Ring of towers around the arena tower. Not solid. Deterministic."""
    rnd = random.Random(1987)
    wins = ["win_a", "win_b", "win_c"]
    towers = []
    span, cell = 13000, 2000
    for gx in range(-span, span + 1, cell):
        for gy in range(-span, span + 1, cell):
            if abs(gx) < 5000 and abs(gy) < 3600:
                continue
            if rnd.random() < 0.18:
                continue
            w = rnd.randint(7, 13) * 64
            d = rnd.randint(7, 13) * 64
            cx = gx + rnd.randint(-4, 4) * 64
            cy = gy + rnd.randint(-4, 4) * 64
            dist = math.hypot(cx, cy)
            hi = rnd.choice((-2600, -1600, -900, -300, 300, 900, 1600, 2600, 3600)) + rnd.randint(-3, 3) * 64
            if dist < 7000 and hi > 1600:
                hi = rnd.choice((-600, 200, 700))
            towers.append((cx, cy, w, d, hi, rnd.choice(wins), rnd.random()))
    # mirror the towers (x and y) so both teams look at the same skyline
    full = []
    for cx, cy, w, d, hi, wt, r in towers:
        if cx < 0 or cy < 0:
            continue
        for mx in (1, -1):
            for my in (1, -1):
                if (cx == 0 and mx < 0) or (cy == 0 and my < 0):
                    continue
                full.append((cx * mx, cy * my, w, d, hi, wt, r))
    for cx, cy, w, d, hi, wt, r in full:
        lo = (cx - w / 2, cy - d / 2, STREET)
        top = (cx + w / 2, cy + d / 2, hi)
        box(lo, top, TEX + wt, faces={"top": TEX + "cityroof", "bot": "common/nodraw"}, solid=False, scale=3.5)
        if r < 0.35:     # neon crown
            box((cx - w / 2 - 4, cy - d / 2 - 4, hi - 40), (cx + w / 2 + 4, cy + d / 2 + 4, hi - 24),
                TEX + ("neon_magenta" if r < 0.18 else "neon_cyan"), solid=False)
        if hi > 1200:    # aircraft warning light
            box((cx - 10, cy - 10, hi), (cx + 10, cy + 10, hi + 160), TEX + "rail", solid=False)
            box((cx - 14, cy - 14, hi + 160), (cx + 14, cy + 14, hi + 188), TEX + "beacon", solid=False)
    # billboards on four nearby towers, facing the arena (mirror symmetric)
    signs = [(0, 5600, "-y", "sign_q3rally"), (0, -5600, "+y", "sign_q3rally"),
             (6400, 2600, "-x", "sign_autoball"), (-6400, -2600, "+x", "sign_autoball"),
             (6400, -2600, "-x", "sign_nitro"), (-6400, 2600, "+x", "sign_nitro")]
    for sx_, sy_, facing, img in signs:
        w, h, z0 = 1600, 400, 1000
        back = 1 if facing in ("-y", "-x") else -1          # the tower stands behind the sign
        if facing in ("-y", "+y"):
            t0, t1 = sorted((sy_, sy_ + back * 32))
            box((sx_ - w / 2, t0, z0), (sx_ + w / 2, t1, z0 + h), TEX + "rail",
                faces={facing: (TEX + img, w, h, 1024, 256)}, solid=False)
            for lx in (-w / 3, w / 3):
                box((sx_ + lx - 16, t0, 860), (sx_ + lx + 16, t1, z0), TEX + "rail", solid=False)
            b0, b1 = sorted((sy_ + back * 32, sy_ + back * 900))
            box((sx_ - 900, b0, STREET), (sx_ + 900, b1, 860), TEX + "win_b",
                faces={"top": TEX + "cityroof", "bot": "common/nodraw"}, solid=False, scale=3.5)
        else:
            t0, t1 = sorted((sx_, sx_ + back * 32))
            box((t0, sy_ - w / 2, z0), (t1, sy_ + w / 2, z0 + h), TEX + "rail",
                faces={facing: (TEX + img, w, h, 1024, 256)}, solid=False)
            for ly in (-w / 3, w / 3):
                box((t0, sy_ + ly - 16, 860), (t1, sy_ + ly + 16, z0), TEX + "rail", solid=False)
            b0, b1 = sorted((sx_ + back * 32, sx_ + back * 900))
            box((b0, sy_ - 900, STREET), (b1, sy_ + 900, 860), TEX + "win_c",
                faces={"top": TEX + "cityroof", "bot": "common/nodraw"}, solid=False, scale=3.5)
    # streets far below
    box((-16000, -16000, STREET - 64), (16000, 16000, STREET), "common/nodraw",
        faces={"top": TEX + "streets"}, solid=False)
    return len(full)


def shell():
    """Sky box around everything (structural)."""
    s, z0, z1 = 17000, STREET - 400, 9000
    sky = TEX + "sky"
    for lo, hi in (((-s - 64, -s, z0), (-s, s, z1)), ((s, -s, z0), (s + 64, s, z1)),
                   ((-s, -s - 64, z0), (s, -s, z1)), ((-s, s, z0), (s, s + 64, z1)),
                   ((-s, -s, z0 - 64), (s, s, z0)), ((-s, -s, z1), (s, s, z1 + 64))):
        box(lo, hi, sky, solid=False, detail=False)


# ---------------------------------------------------------------- entities

def players():
    entity("autoball_ball", 0, 0, 100)
    # kick-off (match start) positions and respawns, point symmetric
    kick = [(-730, 640), (-730, -640), (-1550, 0), (-2150, 300)]
    spawn = [(-2200, 1000), (-2200, -1000), (-2200, 520), (-2200, -520)]
    for team, s in (("red", 1), ("blue", -1)):
        ang = "0" if team == "red" else "180"
        for x, y in kick:
            entity(f"team_CTF_{team}player", s * x, s * y, 48, angle=ang)
        for x, y in spawn:
            entity(f"team_CTF_{team}spawn", s * x, s * y, 48, angle=ang)
    for x in (-1800, 1800):
        for y in (-900, -300, 300, 900):
            entity("info_player_deathmatch", x, y, 48, angle="0" if x < 0 else "180")
    # turbo: four big ones near the corners, six small ones
    for x, y in ((-2050, -1220), (-2050, 1220), (2050, -1220), (2050, 1220)):
        entity("rally_item_turbo", x, y, 48, count="5000", wait="10")
    for x, y in ((0, 1250), (0, -1250), (-1280, 1180), (1280, 1180), (-1280, -1180), (1280, -1180)):
        entity("rally_item_turbo", x, y, 48, count="1500", wait="4")
    # hovering scoreboard over the centre spot
    entity("misc_model", 0, 0, 760, model="models/mapobjects/q3r_scoreboard/scoreboard.md3", angle="270")
    entity("rally_scoreboard", 0, 0, 760)
    entity("info_player_intermission", -3000, -1500, 1500, angles="22 30 0")


def lights():
    """Fill over the pitch (cool white), neon glows come from the shaders,
    floodlight spots are set with the masts."""
    for x in (-1800, -600, 600, 1800):
        for y in (-750, 750):
            entity("light", x, y, 700, light="3500", _color="0.82 0.88 1")
    for sx in (-1, 1):
        entity("light", sx * (X + GD / 2), 0, GH - 40, light="260", _color=" ".join(f"{c:.2f}" for c in TEAM_RGB[TEAM[sx]]))


# ---------------------------------------------------------------- textures

FONT = {
    "A": ["01110", "10001", "10001", "11111", "10001", "10001", "10001"],
    "B": ["11110", "10001", "10001", "11110", "10001", "10001", "11110"],
    "C": ["01111", "10000", "10000", "10000", "10000", "10000", "01111"],
    "E": ["11111", "10000", "10000", "11110", "10000", "10000", "11111"],
    "F": ["11111", "10000", "10000", "11110", "10000", "10000", "10000"],
    "G": ["01111", "10000", "10000", "10111", "10001", "10001", "01111"],
    "I": ["11111", "00100", "00100", "00100", "00100", "00100", "11111"],
    "K": ["10001", "10010", "10100", "11000", "10100", "10010", "10001"],
    "L": ["10000", "10000", "10000", "10000", "10000", "10000", "11111"],
    "N": ["10001", "11001", "10101", "10011", "10001", "10001", "10001"],
    "O": ["01110", "10001", "10001", "10001", "10001", "10001", "01110"],
    "P": ["11110", "10001", "10001", "11110", "10000", "10000", "10000"],
    "Q": ["01110", "10001", "10001", "10001", "10101", "10010", "01101"],
    "R": ["11110", "10001", "10001", "11110", "10100", "10010", "10001"],
    "S": ["01111", "10000", "10000", "01110", "00001", "00001", "11110"],
    "T": ["11111", "00100", "00100", "00100", "00100", "00100", "00100"],
    "U": ["10001", "10001", "10001", "10001", "10001", "10001", "01110"],
    "X": ["10001", "10001", "01010", "00100", "01010", "10001", "10001"],
    "Y": ["10001", "10001", "01010", "00100", "00100", "00100", "00100"],
    "3": ["11110", "00001", "00001", "01110", "00001", "00001", "11110"],
    "!": ["00100", "00100", "00100", "00100", "00100", "00000", "00100"],
    " ": ["00000"] * 7,
    "-": ["00000", "00000", "00000", "11111", "00000", "00000", "00000"],
}


def text_mask(text, cell, gap=1):
    """Bitmap of text in the 5x7 dot font, each dot cell x cell pixels."""
    cols = len(text) * (5 + gap) - gap
    m = np.zeros((7 * cell, cols * cell), np.float32)
    for i, ch in enumerate(text):
        g = FONT[ch]
        for r in range(7):
            for c in range(5):
                if g[r][c] == "1":
                    x = (i * (5 + gap) + c) * cell
                    m[r * cell:(r + 1) * cell, x:x + cell] = 1
    return m


def blur(a, r):
    """Box blur (separable, wraps) of a 2D array, r pixels."""
    if r < 1:
        return a
    k = 2 * r + 1
    out = a.copy()
    for ax in (0, 1):
        c = np.cumsum(np.pad(out, [(r + 1, r) if i == ax else (0, 0) for i in range(2)], mode="wrap"), axis=ax)
        out = (np.take(c, range(k, c.shape[ax]), axis=ax) - np.take(c, range(0, c.shape[ax] - k), axis=ax)) / k
    return out


def noise(h, w, cells, seed):
    rng = np.random.default_rng(seed)
    g = rng.random((cells + 1, cells + 1))
    g[-1, :] = g[0, :]
    g[:, -1] = g[:, 0]
    yy = np.linspace(0, cells, h, endpoint=False)
    xx = np.linspace(0, cells, w, endpoint=False)
    y0, x0 = yy.astype(int), xx.astype(int)
    fy, fx = (yy - y0)[:, None], (xx - x0)[None, :]
    fy, fx = fy * fy * (3 - 2 * fy), fx * fx * (3 - 2 * fx)
    a = g[y0][:, x0]
    b = g[y0][:, x0 + 1]
    c = g[y0 + 1][:, x0]
    d = g[y0 + 1][:, x0 + 1]
    return a * (1 - fx) * (1 - fy) + b * fx * (1 - fy) + c * (1 - fx) * fy + d * fx * fy


def save(img, name, out, alpha=False):
    from PIL import Image
    a = (np.clip(img, 0, 1) * 255).astype(np.uint8)
    im = Image.fromarray(a, "RGBA" if alpha else "RGB")
    if name.endswith(".jpg"):
        im.save(out / name, quality=92)
    else:
        im.save(out / name)


def write_textures():
    out = ROOT / "baseq3r" / "textures" / "q3r_rooftop"
    out.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(7)

    # pitch base: dark rubber surface with fine grain (tiles)
    n = 0.18 + 0.05 * noise(512, 512, 8, 1) + 0.04 * noise(512, 512, 64, 2) + rng.normal(0, 0.015, (512, 512))
    save(np.stack([n * 0.92, n * 0.95, n * 1.08], -1), "pitch_base.jpg", out)

    # pitch markings over the whole field (2048 x 1024 = 5120 x 3072 units, 2.5 x 3 units per pixel)
    W, H = 2048, 1024
    ux, uy = 2 * X / W, 2 * Y / H
    px = (np.arange(W) + 0.5) * ux - X
    py = Y - (np.arange(H) + 0.5) * uy
    PX, PY = np.meshgrid(px, py)
    line = np.zeros((H, W), np.float32)

    def band(dist, width):
        return np.clip(1.0 - np.abs(dist) / width, 0, 1) ** 0.6
    lw = 10
    line = np.maximum(line, band(PX, lw))                                         # halfway line
    r = np.hypot(PX, PY)
    line = np.maximum(line, band(r - 640, lw))                                    # centre circle
    line = np.maximum(line, band(r - 60, 14) * (r < 80))                          # centre spot
    for sx in (-1, 1):                                                            # boxes in front of the goals
        bx = sx * (X - 800)
        inside_y = np.abs(PY) <= 1000
        line = np.maximum(line, band(PX - bx, lw) * inside_y)
        between = (PX * sx >= X - 800) & (PX * sx <= X)
        line = np.maximum(line, band(np.abs(PY) - 1000, lw) * between)
        line = np.maximum(line, band(np.abs(PY) - GW, lw) * (PX * sx >= X - 60) * (PX * sx <= X))
        arc_r = np.hypot(PX - sx * (X - 800), PY)
        line = np.maximum(line, band(arc_r - 420, lw) * (PX * sx < X - 800))
        line = np.maximum(line, band(PX - sx * (X - 12), 16) * (np.abs(PY) <= GW))    # goal line
    # chamfer edge lines and outline
    edge = np.minimum(np.minimum(X - np.abs(PX), Y - np.abs(PY)), (X + Y - C - np.abs(PX) - np.abs(PY)) / math.sqrt(2))
    line = np.maximum(line, band(edge - 40, lw))
    # team colour halves fading from the goals
    t = np.clip((PX / X), -1, 1)
    red = np.clip(-t, 0, 1) ** 1.5
    blue = np.clip(t, 0, 1) ** 1.5
    glowc = blur(line, 6) * 0.6
    rgb = np.zeros((H, W, 3), np.float32)
    white = np.array([0.75, 0.95, 1.0])
    rgb += line[..., None] * white + glowc[..., None] * np.array([0.2, 0.7, 1.0])
    rgb += (red * 0.10)[..., None] * np.array([1.0, 0.15, 0.1]) + (blue * 0.10)[..., None] * np.array([0.1, 0.3, 1.0])
    # big ROOFTOP lettering in each half, faint
    for sx, word in ((-1, "Q3RALLY"), (1, "Q3RALLY")):
        m = text_mask(word, 14)
        m = np.rot90(m, 1 if sx > 0 else -1)
        hh, ww = m.shape
        cx = int((sx * 1500 + X) / ux)
        y0, x0 = H // 2 - hh // 2, cx - ww // 2
        col = np.array([0.15, 0.35, 0.55])
        rgb[y0:y0 + hh, x0:x0 + ww] += m[..., None] * col * 0.5
    rgb = np.clip(rgb, 0, 1)
    save(rgb, "pitch_lines.jpg", out)

    # glass: faint cyan with a frosted edge and a few reflections
    g = np.zeros((256, 256, 3), np.float32)
    yy, xx = np.mgrid[0:256, 0:256] / 256
    streak = np.clip(np.sin((xx * 2 + yy * 0.8) * math.pi * 2) ** 24, 0, 1) * 0.10
    edge = np.clip(1 - np.minimum(yy, 1 - yy) * 40, 0, 1) * 0.30
    v = 0.035 + streak + edge
    g = np.stack([v * 0.55, v * 0.85, v * 1.0], -1)
    save(g, "glass.jpg", out)

    # lit office facades, three colourways (512 = 4 bays x 8 floors)
    for name, warm, seed in (("win_a", (1.0, 0.86, 0.6), 11), ("win_b", (0.65, 0.85, 1.0), 12), ("win_c", (1.0, 0.7, 0.95), 13)):
        r2 = np.random.default_rng(seed)
        img = np.zeros((512, 512, 3), np.float32) + np.array([0.03, 0.03, 0.05])
        fl, bays = 16, 16
        for i in range(fl):
            for j in range(bays):
                y0, x0 = i * 32 + 6, j * 32 + 4
                if r2.random() < 0.42:
                    b = 0.35 + 0.65 * r2.random()
                    tint = np.array(warm) * b
                    img[y0:y0 + 20, x0:x0 + 24] = tint
                else:
                    img[y0:y0 + 20, x0:x0 + 24] = np.array([0.05, 0.06, 0.09]) + 0.04 * r2.random()
        img[:, ::32] *= 0.3
        save(img, f"{name}.jpg", out)

    # LED screen above the goals: dot matrix text on black (scrolls in the shader)
    led = np.zeros((256, 1024, 3), np.float32)
    def strip(words):
        m = text_mask(words, 9)
        return np.pad(m, ((0, 0), (0, 1024 - m.shape[1])))
    rows = [strip("ROOFTOP ARENA "), strip("Q3RALLY AUTOBALL")]
    dots = np.zeros_like(rows[0])
    dots[3::9, 3::9] = 1
    for _ in range(2):
        dots = np.maximum(dots, np.roll(dots, 1, 0))
        dots = np.maximum(dots, np.roll(dots, 1, 1))
    for (row, col), m in zip(((40, (1.0, 0.75, 0.2)), (150, (0.3, 0.9, 1.0))), rows):
        mm = m * dots
        h = mm.shape[0]
        led[row:row + h] += mm[..., None] * np.array(col)
        led[row:row + h] += blur(mm, 2)[..., None] * np.array(col) * 0.6
    grid = np.zeros((256, 1024), np.float32)
    grid[3::9, 3::9] = 0.08
    led += grid[..., None]
    save(led, "ledscreen.jpg", out)

    # neon billboards for the towers
    for name, word, c1, c2 in (("sign_q3rally", "Q3RALLY", (1.0, 0.2, 0.8), (0.2, 0.9, 1.0)),
                               ("sign_autoball", "AUTOBALL", (0.3, 1.0, 0.5), (1.0, 0.9, 0.3)),
                               ("sign_nitro", "NITRO", (1.0, 0.45, 0.1), (0.4, 0.5, 1.0))):
        img = np.zeros((256, 1024, 3), np.float32) + np.array([0.02, 0.01, 0.04])
        m = text_mask(word, min(22, 960 // (len(word) * 6 - 1)))
        hh, ww = m.shape
        y0, x0 = (256 - hh) // 2, (1024 - ww) // 2
        mask = np.zeros((256, 1024), np.float32)
        mask[y0:y0 + hh, x0:x0 + ww] = m
        tube = mask - blur(mask, 3) * 0.0
        img += blur(mask, 8)[..., None] * np.array(c1) * 0.9 + tube[..., None] * np.array(c2) * 0.2
        img += mask[..., None] * np.array([1, 1, 1]) * 0.55
        frame = np.zeros((256, 1024), np.float32)
        frame[6:12, 6:-6] = frame[-12:-6, 6:-6] = 1
        frame[6:-6, 6:12] = frame[6:-6, -12:-6] = 1
        img += blur(frame, 3)[..., None] * np.array(c2) * 0.8
        save(img, f"{name}.jpg", out)

    # streets far below: dark blocks, orange street lights, white/red traffic
    S = 1024
    st = np.zeros((S, S, 3), np.float32) + np.array([0.02, 0.02, 0.03])
    for k in range(0, S, 128):
        st[k:k + 14, :] = np.array([0.08, 0.07, 0.06])
        st[:, k:k + 14] = np.array([0.08, 0.07, 0.06])
        st[k + 6:k + 8, ::12] = np.array([1.0, 0.6, 0.2])
        st[::12, k + 6:k + 8] = np.array([1.0, 0.6, 0.2])
    r3 = np.random.default_rng(5)
    for _ in range(500):
        k = r3.integers(0, S // 128) * 128
        p = r3.integers(0, S)
        col = np.array([1, 1, 0.9]) if r3.random() < 0.5 else np.array([1, 0.15, 0.1])
        if r3.random() < 0.5:
            st[k + 2:k + 5, p:p + 3] = col
        else:
            st[p:p + 3, k + 2:k + 5] = col
    st = st + blur(st.mean(-1), 3)[..., None] * np.array([0.6, 0.4, 0.3])
    save(st, "streets.jpg", out)

    # fillet: dark rubber with a chevron of reflective paint
    f = np.zeros((128, 128, 3), np.float32)
    yy, xx = np.mgrid[0:128, 0:128]
    chev = ((xx + yy) // 16) % 2
    f += (0.10 + 0.08 * chev)[..., None] * np.array([1.0, 1.0, 1.05])
    save(f, "fillet.jpg", out)

    # facades of the end structures: dark panels with team-coloured seams
    for team in ("red", "blue"):
        c = np.array(TEAM_RGB[team])
        p = np.zeros((256, 256, 3), np.float32) + 0.30 + 0.06 * noise(256, 256, 8, 21)[..., None]
        p[::64, :] = c * 0.6
        p[:, ::128] = c * 0.6
        save(p, f"facade_{team}.jpg", out)

    # AC unit and grille
    a = np.zeros((128, 128, 3), np.float32) + 0.35 + 0.05 * noise(128, 128, 8, 31)[..., None]
    a[::8, :] *= 0.6
    save(a, "acunit.jpg", out)
    gr = np.zeros((128, 128, 3), np.float32) + 0.2
    gr[::6, :] = 0.05
    gr[:, ::6] = 0.05
    cx = np.hypot(*np.mgrid[-64:64, -64:64])
    gr[(cx > 40) & (cx < 46)] = 0.4
    save(gr, "grille.jpg", out)

    # aircraft warning light
    save(np.zeros((32, 32, 3), np.float32) + np.array([1.0, 0.08, 0.04]), "beacon.jpg", out)

    # door
    d = np.zeros((256, 128, 3), np.float32) + np.array([0.18, 0.2, 0.22])
    d[:, 60:68] = 0.08
    d[120:136, 96:110] = 0.6
    save(d, "door.jpg", out)


# ---------------------------------------------------------------- shader

def lit(name, image, extra=(), stage_extra=()):
    return [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", *extra, "\t{", f"\t\tmap {image}",
            *stage_extra, "\t\trgbGen identity" if not any("rgbGen" in s for s in stage_extra) else "", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}", "}", ""]


def tinted(name, image, rgb, extra=()):
    return lit(name, image, extra, (f"\t\trgbGen const ( {rgb[0]:.2f} {rgb[1]:.2f} {rgb[2]:.2f} )",))


def glow(name, rgb, light, extra=()):
    rg = f"\t\trgbGen const ( {rgb[0]} {rgb[1]} {rgb[2]} )"
    lines = [f"textures/{TEX}{name}", "{", "\tqer_editorimage textures/q3r_signage/white.jpg",
             "\tsurfaceparm nomarks", "\tsurfaceparm nolightmap", *extra]
    if light:
        lines += [f"\tq3map_surfacelight {light}", f"\tq3map_lightRGB {rgb[0]} {rgb[1]} {rgb[2]}"]
    lines += ["\t{", "\t\tmap textures/q3r_signage/white.jpg", rg, "\t}", "}", ""]
    return lines


def selflit(name, image, extra=(), light=0, tc=()):
    lines = [f"textures/{TEX}{name}", "{", f"\tqer_editorimage {image}", "\tsurfaceparm nomarks",
             "\tsurfaceparm nolightmap", *extra]
    if light:
        lines.append(f"\tq3map_surfacelight {light}")
    lines += ["\t{", f"\t\tmap {image}", *tc, "\t\trgbGen identity", "\t}", "}", ""]
    return lines


def write_shader():
    T = f"textures/{TEX}"
    out = ["// Rooftop Arena - Autoball on a skyscraper roof at night.",
           "// Written by tools/maptools/make_rooftop.py. Compile with -custinfoparms.", ""]
    out += [f"{T}sky", "{", "\tqer_editorimage env/ely_darkcity/darkcity_ft.tga", "\tsurfaceparm noimpact",
            "\tsurfaceparm nolightmap", "\tsurfaceparm sky", "\tq3map_globaltexture",
            "\tq3map_sun 0.75 0.55 1 45 230 40", "\tq3map_skylight 55 4",
            "\tskyparms env/ely_darkcity/darkcity - -",
            "\tq3r_haze 0.16 0.09 0.24 9000\t// Q3Rally: night haze over the city",
            "\tq3r_colorLut gfx/luts/night", "}", ""]
    # the pitch: tiled rubber, glowing markings over the whole field (clamped)
    out += [f"{T}pitch", "{", f"\tqer_editorimage {T}pitch_lines.jpg", "\tq3map_nonplanar", "\t{",
            f"\t\tmap {T}pitch_base.jpg", "\t\ttcMod scale 20 12", "\t\trgbGen identity", "\t}", "\t{",
            "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}", "\t{",
            f"\t\tclampmap {T}pitch_lines.jpg", "\t\tblendFunc GL_ONE GL_ONE", "\t\trgbGen identity", "\t}", "}", ""]
    out += lit("goalfloor", f"{T}pitch_base.jpg", (), ("\t\trgbGen const ( 0.6 0.6 0.65 )",))
    out += lit("fillet", f"{T}fillet.jpg")
    out += lit("roof", "textures/q3r_building/roof01.jpg", ("\tq3map_lightmapSampleSize 32",),
               ("\t\trgbGen const ( 0.55 0.55 0.6 )",))
    out += lit("concrete", "textures/q3r_building/concrete02.jpg", ("\tq3map_lightmapSampleSize 24",),
               ("\t\trgbGen const ( 0.55 0.55 0.62 )",))
    out += lit("rail", "textures/q3r_metals/plate01.tga", ("\tsurfaceparm SURF_METAL",))
    for team in ("red", "blue"):
        out += lit(f"facade_{team}", f"{T}facade_{team}.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("acunit", f"{T}acunit.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("grille", f"{T}grille.jpg", ("\tsurfaceparm SURF_METAL",))
    out += lit("door", f"{T}door.jpg", ("\tsurfaceparm nonsolid",))
    # glass: solid (ball and cars bounce off), see-through
    out += [f"{T}glass", "{", f"\tqer_editorimage {T}glass.jpg", "\tqer_trans 0.4", "\tsurfaceparm trans",
            "\tsurfaceparm nomarks", "\tsurfaceparm nolightmap", "\tcull none", "\t{",
            f"\t\tmap {T}glass.jpg", "\t\tblendFunc GL_ONE GL_ONE", "\t\trgbGen identity", "\t}", "}", ""]
    # neon: self-lit tubes (not solid), a little light into the scene
    nonsolid = ("\tsurfaceparm nonsolid", "\tsurfaceparm trans")
    out += glow("neon_red", (1.0, 0.16, 0.12), 300, nonsolid)
    out += glow("neon_blue", (0.12, 0.42, 1.0), 300, nonsolid)
    out += glow("neon_cyan", (0.2, 0.9, 1.0), 0, nonsolid)
    out += glow("neon_magenta", (1.0, 0.2, 0.85), 0, nonsolid)
    out += glow("exitsign", (0.2, 1.0, 0.35), 80, nonsolid)
    out += [f"{T}beacon", "{", f"\tqer_editorimage {T}beacon.jpg", "\tsurfaceparm nomarks", "\tsurfaceparm nolightmap",
            *nonsolid, "\t{", f"\t\tmap {T}beacon.jpg", "\t\trgbGen wave square 0.15 0.85 0 0.6", "\t}", "}", ""]
    out += selflit("ledscreen", f"{T}ledscreen.jpg", (), 60, ("\t\ttcMod scroll 0.06 0",))
    for name in ("win_a", "win_b", "win_c"):
        out += selflit(name, f"{T}{name}.jpg", ("\tsurfaceparm nonsolid",))
    for name in ("sign_q3rally", "sign_autoball", "sign_nitro"):
        out += selflit(name, f"{T}{name}.jpg", ("\tsurfaceparm nonsolid",))
    out += selflit("streets", f"{T}streets.jpg", ("\tsurfaceparm nonsolid",), 0,
                   ("\t\ttcGen vector ( 0.00025 0 0 ) ( 0 0.00025 0 )",))
    out += [f"{T}cityroof", "{", "\tqer_editorimage textures/q3r_building/roof01.jpg", "\tsurfaceparm nonsolid",
            "\tsurfaceparm nolightmap", "\tsurfaceparm nomarks", "\t{", "\t\tmap textures/q3r_building/roof01.jpg",
            "\t\trgbGen const ( 0.06 0.05 0.09 )", "\t}", "}", ""]
    path = ROOT / "baseq3r" / "scripts" / f"{NAME}.shader"
    path.write_text("\n".join(out), encoding="utf-8")


# ---------------------------------------------------------------- checks and output

def validate():
    starts = [e for e in ENTITIES if e["classname"].endswith(("player", "spawn")) or
              e["classname"] == "info_player_deathmatch"]
    for e in starts + [e for e in ENTITIES if e["classname"] == "rally_item_turbo"]:
        x, y, z = map(float, e["origin"].split())
        assert abs(x) < X - 100 and abs(y) < Y - 100 and abs(x) + abs(y) < X + Y - C - 150, e
        for lo, hi in SOLIDS:
            assert not (x + 60 > lo[0] and x - 60 < hi[0] and y + 60 > lo[1] and y - 60 < hi[1]
                        and z + 30 > lo[2] and z - 30 < hi[2]), (e, lo, hi)
    goals = [e for e in ENTITIES if e["classname"] == "autoball_goal"]
    return {"map": NAME, "pitch": [2 * X, 2 * Y], "goal_mouth": [2 * GW, GH], "goal_to_goal": 2 * X,
            "glass_height": GLASS, "end_height": EH, "goals": len(goals),
            "brushes": len(BRUSHES), "entities": len(ENTITIES),
            "kickoff_per_team": sum(1 for e in starts if e["classname"] == "team_CTF_redplayer"),
            "spawns_per_team": sum(1 for e in starts if e["classname"] == "team_CTF_redspawn"),
            "turbo": sum(1 for e in ENTITIES if e["classname"] == "rally_item_turbo")}


def write_map():
    fields = {"classname": "worldspawn", "message": "Rooftop Arena", "_ambient": "6", "_minlight": "6",
              "gridsize": "256 256 512", "_blocksize": "0 0 0",
              "weather": "rain", "weatherChance": "35", "weatherCycle": "120 240", "weatherStrength": "0.3 0.7",
              "weatherWind": "60 260"}
    text = ["// Generated by tools/maptools/make_rooftop.py. Q3 brush format.", "{"]
    text += [f'"{k}" "{v}"' for k, v in fields.items()]
    text += BRUSHES
    text.append("}")
    for ent in ENTITIES:
        body = [f'"{k}" "{v}"' for k, v in ent.items() if k != "_brushes"]
        text.append("{\n" + "\n".join(body + ent.get("_brushes", [])) + "\n}")
    target = ROOT / "baseq3r" / "maps" / f"{NAME}.map"
    target.write_text("\n".join(text) + "\n", encoding="ascii")
    arena = ROOT / "baseq3r" / "scripts" / f"{NAME}.arena"
    arena.write_text("\r\n".join([
        "{", "//MAP FILE NAME", f'map "{NAME}"', "", "//IN-GAME MAP NAME", 'longname "Rooftop Arena"', "",
        "//AUTHOR OF THE MAP", 'author "Q3Rally Team"', "", "//GAME TYPE SUPPORTS", 'type "q3r_autoball"', "",
        "//CUSTOM BOTS", 'bots "Chloe Jim Zack Paul Tobias Josh"', "", "//PLAYER STARTS", 'starts "8"', "",
        "//LAPTIME", 'laptime ""', "", "//CHECKPOINTS", 'checkpoints ""', "", "//OBSERVER SPOTS",
        'observerspots ""', "", "//WEAPONS", 'weapons ""', "", "//POWERUPS", 'powerups "10"', "",
        "//REVERSABLE", 'reversable ""', "", "//TRACK LENGTHS", 'tracklengths ""', "", "//BOT SUPPORTS",
        'botsupport "None"', "", "//TEAMS", 'teams "2"', "}", ""]), encoding="ascii")
    return target


def overview(report):
    from PIL import Image, ImageDraw
    s = 0.16
    W, H = int(2 * (EX + 300) * s), int(2 * (BY + 300) * s)
    im = Image.new("RGB", (W, H), (20, 14, 32))
    d = ImageDraw.Draw(im)

    def P(x, y):
        return ((x + EX + 300) * s, (BY + 300 - y) * s)
    d.rectangle([P(-EX, BY), P(EX, -BY)], fill=(60, 58, 70))
    poly = [P(-X + C, Y), P(X - C, Y), P(X, Y - C), P(X, -Y + C), P(X - C, -Y), P(-X + C, -Y), P(-X, -Y + C),
            P(-X, Y - C)]
    d.polygon(poly, fill=(34, 36, 44))
    for lo, hi in SOLIDS:
        if hi[2] <= 1 or lo[2] < -40:
            continue
        shade = int(min(220, 90 + hi[2] / 5))
        d.rectangle([P(lo[0], hi[1]), P(hi[0], lo[1])], fill=(shade, shade, shade + 10))
    d.polygon(poly, outline=(120, 220, 255))
    for sx in (-1, 1):
        col = (230, 50, 40) if TEAM[sx] == "red" else (40, 110, 255)
        d.rectangle([P(sx * (X + BALL_R) if sx > 0 else sx * (X + GD), GW), P(sx * (X + GD) if sx > 0 else sx * (X + BALL_R), -GW)],
                    fill=col)
    for e in ENTITIES:
        if "origin" not in e:
            continue
        x, y, _ = map(float, e["origin"].split())
        u, v = P(x, y)
        c = e["classname"]
        if c == "rally_item_turbo":
            r = 6 if e.get("count") == "5000" else 4
            d.ellipse([u - r, v - r, u + r, v + r], fill=(250, 190, 30))
        elif c.startswith("team_CTF_") and c.endswith("player"):
            col = (255, 90, 80) if "red" in c else (90, 150, 255)
            d.polygon([(u - 6, v + 5), (u + 6, v + 5), (u, v - 7)], fill=col)
        elif c.startswith("team_CTF_") and c.endswith("spawn"):
            col = (255, 90, 80) if "red" in c else (90, 150, 255)
            d.rectangle([u - 4, v - 4, u + 4, v + 4], outline=col)
        elif c == "autoball_ball":
            d.ellipse([u - 9, v - 9, u + 9, v + 9], fill=(240, 240, 240))
    out = ROOT / "docs" / "maps" / "rooftop"
    out.mkdir(parents=True, exist_ok=True)
    im.save(out / "rooftop_layout.png")
    (out / "validation.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


def main():
    pitch()
    side_glass()
    camera_rails()
    for sx in (-1, 1):
        end_structure(sx)
    resets()
    tower_facade()
    towers = city()
    shell()
    players()
    lights()
    report = validate()
    report["city_towers"] = towers
    write_textures()
    write_shader()
    target = write_map()
    overview(report)
    print(json.dumps(report, indent=2))
    print("Wrote", target)


if __name__ == "__main__":
    main()
