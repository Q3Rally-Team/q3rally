#!/usr/bin/env python3
"""
Generates baseq3r/maps/q3r_autoball_test.map, the Autoball phase-1 test arena.

Closed box field with two goals, 45-degree ramps along the boards (stand-ins
for quarter pipes), a sky ceiling, an autoball_ball on the centre spot, two
autoball_goal volumes, team kick-off and respawn spots (red defends -X, blue
defends +X), eight deathmatch spawns, four big and six small turbo pickups. Dimensions follow the design doc and
are meant to be tweaked here, then recompiled in Q3RallyRadiant / q3map2:

    q3map2 -meta maps/q3r_autoball_test.map
    q3map2 -vis -saveprt maps/q3r_autoball_test.map
    q3map2 -light -fast -samples 2 -bounce 1 maps/q3r_autoball_test.map

Usage: python3 tools/autoball/make_test_arena.py [output.map]
"""
import os
import sys
from itertools import combinations

HALF_X = 2560          # field half length (goal to goal = 5120)
HALF_Y = 1792          # field half width (3584)
HEIGHT = 1024          # sky ceiling; Bullet treats it as solid
THICK = 64             # wall thickness
GOAL_HALF_W = 512      # goal mouth 1024 wide
GOAL_H = 320           # goal mouth height
GOAL_D = 512           # goal depth
RAMP = 192             # 45-degree board ramp, height = depth
BALL_RADIUS = 75       # the goal volume starts one radius behind the line

FLOOR_TEX = "base_floor/concretefloor1"
WALL_TEX = "base_wall/concrete"
RAMP_TEX = "base_floor/concrete"
GOAL_TEX = "base_wall/concrete"
SKY_TEX = "skies/ely_sunset"
TRIGGER_TEX = "common/trigger"


def cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def add(a, b, s=1):
    return (a[0] + s * b[0], a[1] + s * b[1], a[2] + s * b[2])


def plane_points(normal, point):
    """Three integer points for a plane with an OUTWARD normal.
    q3map2 computes normal = (c - a) x (b - a), so b = a + t2, c = a + t1
    with t1 x t2 parallel to the normal."""
    up = (0, 0, 1) if abs(normal[2]) < max(abs(normal[0]), abs(normal[1]), 1e-9) * 2 else (1, 0, 0)
    if normal[0] == 0 and normal[1] == 0:
        up = (1, 0, 0)
    t1 = cross(up, normal)
    t2 = cross(normal, t1)
    assert dot(cross(t1, t2), normal) > 0
    scale = 64
    a = point
    b = add(a, t2, scale)
    c = add(a, t1, scale)
    return a, b, c


def face(normal, point, tex):
    return (normal, point, tex)


def box(mins, maxs, tex, side_tex=None):
    x1, y1, z1 = mins
    x2, y2, z2 = maxs
    st = side_tex or tex
    return [
        face((0, 0, -1), (x1, y1, z1), tex),
        face((0, 0, 1), (x1, y1, z2), tex),
        face((-1, 0, 0), (x1, y1, z1), st),
        face((1, 0, 0), (x2, y1, z1), st),
        face((0, -1, 0), (x1, y1, z1), st),
        face((0, 1, 0), (x1, y2, z1), st),
    ]


def ramp_along_y(x1, x2, wall_y, sign):
    """Wedge against the wall y = wall_y; sign = +1 for the +Y wall."""
    toe_y = wall_y - sign * RAMP
    return [
        face((0, 0, -1), (x1, toe_y, 0), RAMP_TEX),
        face((0, sign, 0), (x1, wall_y, 0), RAMP_TEX),
        face((0, -sign, 1), (x1, toe_y, 0), RAMP_TEX),
        face((-1, 0, 0), (x1, toe_y, 0), RAMP_TEX),
        face((1, 0, 0), (x2, toe_y, 0), RAMP_TEX),
    ]


def ramp_along_x(y1, y2, wall_x, sign):
    toe_x = wall_x - sign * RAMP
    return [
        face((0, 0, -1), (toe_x, y1, 0), RAMP_TEX),
        face((sign, 0, 0), (wall_x, y1, 0), RAMP_TEX),
        face((-sign, 0, 1), (toe_x, y1, 0), RAMP_TEX),
        face((0, -1, 0), (toe_x, y1, 0), RAMP_TEX),
        face((0, 1, 0), (toe_x, y2, 0), RAMP_TEX),
    ]


def brush_vertices(faces):
    """Corner points of a convex brush, used to reject degenerate brushes."""
    planes = [(f[0], dot(f[0], f[1])) for f in faces]
    verts = []
    for (n1, d1), (n2, d2), (n3, d3) in combinations(planes, 3):
        det = dot(n1, cross(n2, n3))
        if abs(det) < 1e-9:
            continue
        p = add(add(tuple(d1 * c for c in cross(n2, n3)), tuple(d2 * c for c in cross(n3, n1))),
                tuple(d3 * c for c in cross(n1, n2)))
        p = tuple(c / det for c in p)
        if all(dot(n, p) <= d + 1e-6 for n, d in planes):
            verts.append(tuple(round(c, 3) for c in p))
    return set(verts)


def world_brushes():
    X, Y, H, T = HALF_X, HALF_Y, HEIGHT, THICK
    G, GH, D = GOAL_HALF_W, GOAL_H, GOAL_D
    brushes = [
        box((-X - D - T, -Y - T, -T), (X + D + T, Y + T, 0), FLOOR_TEX),
        box((-X - D - T, -Y - T, H), (X + D + T, Y + T, H + T), SKY_TEX),
        box((-X - T, Y, 0), (X + T, Y + T, H), WALL_TEX),
        box((-X - T, -Y - T, 0), (X + T, -Y, H), WALL_TEX),
    ]
    for s in (1, -1):
        inner, outer = (X, X + T) if s > 0 else (-X - T, -X)
        brushes += [
            box((inner, -Y, 0), (outer, -G, H), WALL_TEX),
            box((inner, G, 0), (outer, Y, H), WALL_TEX),
            box((inner, -G, GH), (outer, G, H), WALL_TEX),
        ]
        back = (X + D, X + D + T) if s > 0 else (-X - D - T, -X - D)
        side = (X + T, X + D) if s > 0 else (-X - D, -X - T)
        roof = (X + T, X + D + T) if s > 0 else (-X - D - T, -X - T)
        brushes += [
            box((back[0], -G - T, 0), (back[1], G + T, GH + T), GOAL_TEX),
            box((side[0], G, 0), (side[1], G + T, GH), GOAL_TEX),
            box((side[0], -G - T, 0), (side[1], -G, GH), GOAL_TEX),
            box((roof[0], -G - T, GH), (roof[1], G + T, GH + T), GOAL_TEX),
        ]
        # board ramps: full length on the long walls, beside the goal mouth on the ends
        brushes.append(ramp_along_y(-X, X, s * Y, s))
        brushes.append(ramp_along_x(-Y, -G, s * X, s))
        brushes.append(ramp_along_x(G, Y, s * X, s))
    return brushes


def format_brush(faces, index):
    verts = brush_vertices(faces)
    if len(verts) < 4:
        raise SystemExit(f"brush {index} is degenerate")
    lines = [f"// brush {index}", "{"]
    for normal, point, tex in faces:
        a, b, c = plane_points(normal, point)
        pts = " ".join("( %d %d %d )" % tuple(int(round(v)) for v in p) for p in (a, b, c))
        lines.append(f"{pts} {tex} 0 0 0 0.5 0.5 0 0 0")
    lines.append("}")
    return "\n".join(lines)


def entity(fields, brushes=None):
    body = "\n".join(f'"{k}" "{v}"' for k, v in fields)
    if brushes:
        body += "\n" + "\n".join(format_brush(b, i) for i, b in enumerate(brushes))
    return "{\n" + body + "\n}"


def goal_entities():
    """Red defends the goal at -X, blue the one at +X."""
    X, G, GH, D = HALF_X, GOAL_HALF_W, GOAL_H, GOAL_D
    ents = []
    for team, s in (("red", -1), ("blue", 1)):
        x1, x2 = sorted((s * (X + BALL_RADIUS), s * (X + D)))
        ents.append(entity([("classname", "autoball_goal"), ("team", team)],
                           [box((x1, -G, 0), (x2, G, GH), TRIGGER_TEX)]))
    return ents


def team_spawns():
    """Kick-off spots (team_CTF_*player) as in the design doc, respawns near the own goal."""
    kickoff = [(-730, 640), (-730, -640), (-1550, 0), (-2150, 300)]
    respawn = [(-2200, 1100), (-2200, -1100), (-2200, 600), (-2200, -600)]
    ents = []
    for team, s, angle in (("red", 1, 0), ("blue", -1, 180)):
        for x, y in kickoff:
            ents.append(entity([("classname", f"team_CTF_{team}player"),
                                ("origin", f"{s * x} {s * y} 48"), ("angle", str(angle))]))
        for x, y in respawn:
            ents.append(entity([("classname", f"team_CTF_{team}spawn"),
                                ("origin", f"{s * x} {s * y} 48"), ("angle", str(angle))]))
    return ents


def point_entities():
    ents = [entity([("classname", "autoball_ball"), ("origin", "0 0 100")])]
    for sx, angle in ((-1, 0), (1, 180)):
        for y in (-900, -300, 300, 900):
            ents.append(entity([("classname", "info_player_deathmatch"),
                                ("origin", f"{sx * 1800} {y} 48"), ("angle", str(angle))]))
    # big turbo pads in the corners: 5 s, back after 10 s
    for sx in (-1, 1):
        for sy in (-1, 1):
            ents.append(entity([("classname", "rally_item_turbo"),
                                ("origin", f"{sx * (HALF_X - 360)} {sy * (HALF_Y - 360)} 48"),
                                ("count", "5000"), ("wait", "10")]))
    # small ones on the centre line and the halves: 1.5 s, back after 4 s
    for x, y in ((0, 1300), (0, -1300), (-1280, 1240), (1280, 1240), (-1280, -1240), (1280, -1240)):
        ents.append(entity([("classname", "rally_item_turbo"), ("origin", f"{x} {y} 48"),
                            ("count", "1500"), ("wait", "4")]))
    for x in (-1600, 0, 1600):
        for y in (-1000, 0, 1000):
            ents.append(entity([("classname", "light"), ("origin", f"{x} {y} 760"), ("light", "1400")]))
    return ents + team_spawns() + goal_entities()


def main():
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(root, "baseq3r", "maps", "q3r_autoball_test.map")
    parts = ["// Generated by tools/autoball/make_test_arena.py - edit the script, not this file",
             "// entity 0", "{",
             '"classname" "worldspawn"',
             '"message" "Autoball Test Arena"',
             '"_ambient" "15"']
    for i, b in enumerate(world_brushes()):
        parts.append(format_brush(b, i))
    parts.append("}")
    for i, e in enumerate(point_entities(), start=1):
        parts.append(f"// entity {i}")
        parts.append(e)
    with open(out, "w", newline="\n") as f:
        f.write("\n".join(parts) + "\n")
    print(f"wrote {out}")


if __name__ == "__main__":
    main()
