#!/usr/bin/env python3
"""Generate the lantern models for the maps (misc_model) and their textures.

    python3 tools/mapobjects/make_lanterns.py [--out baseq3r]

  models/mapobjects/q3r_lanterns/
    lantern.md3        hanging lantern: ring, chimney, roof, tapered glass
                       with an iron frame, base and finial, a flame inside.
                       Origin at the top of the ring (it hangs below it).
    lantern_post.md3   cast-iron post with a scrolled arm and the lantern
                       hanging from it. Origin on the ground, arm along +x.
    lantern_wall.md3   wall bracket with the lantern. Origin on the wall,
                       arm along +x.
    streetlamp.md3     sodium street lamp: tapered pole, swan-neck arm and a
                       cobra head with a glowing lens. Origin on the ground,
                       arm along +x (lens 88 units out, at z 316).
    floodmast.md3      floodlight mast: tapered galvanised mast, ladder with
                       a cage, ringed platform at z 440 and six tilted flood
                       heads on a frame, facing +x. Origin on the ground,
                       about 500 high (md3 coordinates end at 512: use modelscale
                       for taller masts).
    floodmast_red.md3, floodmast_blue.md3   the same with team-coloured lenses.
  plus textures; shaders in scripts/q3r_lanterns.shader.

The models carry no light: put a light entity at the glass (see
docs/lanterns.md for the offsets) and, where cars can hit the post, an invisible
solid brush around it (misc_models do not collide). Needs numpy.
"""
import argparse
import math
import os
import struct

import numpy as np

from meshlib import (Mesh, arc, box, frustum, grid, lathe_pts, noise, place, retexture, rot_y,
                     rot_z, sphere, tube, write_md3, write_tga)

D = "models/mapobjects/q3r_lanterns/"
IRON, GLASS, FLAME = D + "iron", D + "glass", D + "flame"
POLE, LENS = D + "pole", D + "lens"
GALV, HOUSING, FLOOD = D + "galv", D + "housing", D + "floodlens"


# ---------------------------------------------------------------- lantern

def add_lantern(mesh, ox=0.0, oy=0.0, oz=0.0):
    """Hanging lantern; (ox, oy, oz) is the top of the ring."""
    def at(p):
        return (p[0] + ox, p[1] + oy, p[2] + oz)
    # ring (in the xz plane) and knob
    tube(mesh, IRON, [at(p) for p in arc((0, 0, -3.6), 3.0, 0, 2 * math.pi, 14)[:-1]], 0.8, 6, closed=True)
    sphere(mesh, IRON, at((0, 0, -7.6)), 1.6, 8, 5)
    # chimney with a little cap
    for args in ((oz - 8.6, 2.2, oz - 9.6, 3.8, True, False), (oz - 9.6, 2.6, oz - 13.4, 3.0, False, False)):
        _sq(mesh, IRON, ox, oy, *args)
    # roof: overhanging pyramid, rim band, flat underside
    _sq(mesh, IRON, ox, oy, oz - 13.4, 3.0, oz - 19.0, 11.0, False, False)
    _sq(mesh, IRON, ox, oy, oz - 19.0, 11.0, oz - 20.6, 11.0, False, True)
    # glass: tapered to the bottom, slightly inside the frame
    _sq(mesh, GLASS, ox, oy, oz - 20.6, 8.6, oz - 38.0, 6.4, False, False, uv=1 / 17.4)
    # frame: corner bars, a top and a bottom collar
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        ca, sa = math.cos(a), math.sin(a)
        top = (9.0 * math.sqrt(2) * ca, 9.0 * math.sqrt(2) * sa, -20.6)
        bot = (6.9 * math.sqrt(2) * ca, 6.9 * math.sqrt(2) * sa, -38.0)
        tube(mesh, IRON, [at(top), at(bot)], 0.75, 5)
    for k in range(4):                        # mid bars across each pane
        a0, a1 = math.pi / 4 + k * math.pi / 2, math.pi / 4 + (k + 1) * math.pi / 2
        rr = 7.9 * math.sqrt(2)
        tube(mesh, IRON, [at((rr * math.cos(a0), rr * math.sin(a0), -28.0)),
                          at((rr * math.cos(a1), rr * math.sin(a1), -28.0))], 0.45, 4)
    _sq(mesh, IRON, ox, oy, oz - 20.6, 9.4, oz - 21.8, 9.2, False, False)
    _sq(mesh, IRON, ox, oy, oz - 37.4, 7.2, oz - 39.2, 7.6, False, False)
    # base and finial
    _sq(mesh, IRON, ox, oy, oz - 39.2, 7.6, oz - 42.0, 5.0, True, True)
    tube(mesh, IRON, [at((0, 0, -42.0)), at((0, 0, -44.5))], 1.6, 8)
    sphere(mesh, IRON, at((0, 0, -46.0)), 1.8, 8, 5)
    # flame / bulb inside
    sphere(mesh, FLAME, at((0, 0, -30.5)), 2.2, 8, 6, squash=(1, 1, 1.7))
    tube(mesh, IRON, [at((0, 0, -38.0)), at((0, 0, -34.0))], 1.2, 6)


def _sq(mesh, shader, ox, oy, z0, a0, z1, a1, cap0, cap1, uv=1 / 16):
    m = Mesh()
    frustum(m, shader, 4, z0, a0, z1, a1, phase=math.pi / 4, cap0=cap0, cap1=cap1, uv=uv)
    for V, N, S, T in m.parts[shader]:
        mesh.add(shader, [tuple(v + np.array([ox, oy, 0])) for v in V], list(N), list(S), T)


def lantern():
    mesh = Mesh()
    add_lantern(mesh)
    return mesh


def lantern_post():
    mesh = Mesh()
    # plinth, shaft with collars, top finial (round, cast iron look)
    base = [(0.01, 0.0), (9.5, 0.0), (9.5, 3.0), (8.0, 4.5), (7.5, 9.0), (5.5, 12.0), (4.4, 15.0),
            (3.4, 22.0), (3.2, 120.0), (4.6, 121.0), (4.6, 124.0), (3.2, 125.0), (3.0, 147.0),
            (4.4, 148.0), (4.4, 151.0), (2.6, 152.0), (2.0, 155.0)]
    lathe_pts(mesh, IRON, base, 12, u_rep=2)
    sphere(mesh, IRON, (0, 0, 157.5), 2.8, 10, 6)
    # arm: slightly arched bar out to the hook, a scroll brace below it
    arm = [(1.0 + 34.0 * t, 0, 146.0 + 5.0 * math.sin(math.pi * t)) for t in np.linspace(0, 1, 9)]
    tube(mesh, IRON, arm, 1.1, 6)
    brace = arc((14.0, 0, 128.0), 13.0, math.pi, math.pi / 2, 8)
    tube(mesh, IRON, [(p[0] - 0.0, p[1], p[2]) for p in brace], 0.8, 5)
    scroll = arc((22.0, 0, 139.0), 4.5, -math.pi / 2, math.pi * 1.2, 10)
    tube(mesh, IRON, scroll, 0.7, 5)
    sphere(mesh, IRON, (35.6, 0, 146.6), 1.6, 8, 5)
    # hook and two chain links
    tube(mesh, IRON, [(34.0, 0, 145.0), (34.0, 0, 141.5)], 0.6, 5)
    tube(mesh, IRON, arc((34.0, 0, 139.5), 1.6, 0, 2 * math.pi, 10)[:-1], 0.45, 4, closed=True)
    tube(mesh, IRON, [(34.0 + 1.6 * math.cos(a), 1.6 * math.sin(a), 136.6) for a in np.linspace(0, 2 * math.pi, 11)][:-1],
         0.45, 4, closed=True)
    add_lantern(mesh, 34.0, 0.0, 135.0)
    return mesh


def lantern_wall():
    mesh = Mesh()
    box(mesh, IRON, (0.0, -4.0, -12.0), (1.6, 4.0, 6.0))                # wall plate
    for z in (-8.0, 2.0):
        sphere(mesh, IRON, (1.8, 0, z), 1.0, 6, 4)                     # rivets
    arm = [(1.0 + 23.0 * t, 0, 0.0 + 2.5 * math.sin(math.pi * t)) for t in np.linspace(0, 1, 7)]
    tube(mesh, IRON, arm, 1.0, 6)
    tube(mesh, IRON, arc((15.0, 0, -11.0), 13.5, math.pi, math.pi * 0.55, 8), 0.7, 5)
    sphere(mesh, IRON, (24.6, 0, 0.4), 1.4, 8, 5)
    tube(mesh, IRON, [(22.0, 0, -0.8), (22.0, 0, -3.5)], 0.55, 5)
    add_lantern(mesh, 22.0, 0.0, -3.5)
    return mesh


# ---------------------------------------------------------------- street lamp

def streetlamp():
    mesh = Mesh()
    base = [(0.01, 0.0), (11.5, 0.0), (11.5, 20.0), (10.0, 22.0), (7.2, 24.0), (6.6, 30.0),
            (4.4, 296.0), (4.0, 300.0)]
    lathe_pts(mesh, POLE, base, 10, u_rep=2)
    # swan neck: up from the pole top, over and down to the head
    neck = [(0.0, 0, 296.0), (0.0, 0, 306.0), (3.0, 0, 316.0), (12.0, 0, 323.0), (26.0, 0, 326.0),
            (45.0, 0, 325.0), (62.0, 0, 322.0), (74.0, 0, 320.0)]
    tube(mesh, POLE, neck, 2.4, 8)
    # cobra head: flattened lathe along x, glowing lens underneath
    head = []
    L = 36.0
    for i in range(13):
        t = i / 12
        x = 70.0 + L * t
        w = 9.5 * math.sin(math.pi * (0.12 + 0.88 * t)) ** 0.6 + 2.0
        head.append((x, w))
    P, N, U, V = [], [], [], []
    for x, w in head:
        rp, rn, ru, rv = [], [], [], []
        for s in range(13):
            a = math.pi * s / 12                  # upper half shell
            y, z = w * math.cos(a), 0.55 * w * math.sin(a)
            rp.append((x, y, 316.0 + z))
            rn.append((0.0, math.cos(a), math.sin(a) / 0.55))
            ru.append(s / 12)
            rv.append((x - 70) / 36)
        P.append(rp), N.append(rn), U.append(ru), V.append(rv)
    grid(mesh, POLE, P, N, U, V)
    # lens: flat ellipse on the underside
    verts, sts = [(88.0, 0, 315.6)], [(0.5, 0.5)]
    ring = []
    for s in range(20):
        a = 2 * math.pi * s / 20
        verts.append((88.0 + 15.5 * math.cos(a), 7.4 * math.sin(a), 315.6))
        sts.append((0.5 + 0.5 * math.cos(a), 0.5 + 0.5 * math.sin(a)))
    tris = [(0, 1 + k, 1 + (k + 1) % 20) for k in range(20)]
    mesh.add(LENS, verts, [(0, 0, -1)] * 21, sts, tris)
    # rim around the lens (lower edge of the shell)
    rim = [(88.0 + 16.5 * math.cos(a), 8.2 * math.sin(a), 315.8) for a in np.linspace(0, 2 * math.pi, 21)][:-1]
    tube(mesh, POLE, rim, 0.9, 5, closed=True)
    return mesh


# ---------------------------------------------------------------- floodlight mast

FM_TOP = 428.0                                  # platform height


def _mast_r(z):
    return 13.0 - 6.0 * z / FM_TOP              # mast radius (apothem) at height z


def _flood_head(m):
    """Flood head in local coordinates: lens at x 0 facing +x, 28 wide, 22 high."""
    box(m, HOUSING, (-11.0, -14.0, -11.0), (0.0, 14.0, 11.0))
    for k in range(5):                          # cooling fins at the back
        z = -8.0 + 4.0 * k
        box(m, HOUSING, (-14.0, -12.5, z - 0.7), (-11.0, 12.5, z + 0.7))
    for lo, hi in (((0.0, -14.0, 9.0), (2.4, 14.0, 11.0)), ((0.0, -14.0, -11.0), (2.4, 14.0, -9.0)),
                   ((0.0, -14.0, -9.0), (2.4, -12.0, 9.0)), ((0.0, 12.0, -9.0), (2.4, 14.0, 9.0))):
        box(m, HOUSING, lo, hi)                 # bezel
    box(m, HOUSING, (0.0, -14.0, 11.0), (6.0, 14.0, 12.0))             # visor
    m.add(FLOOD, [(0.6, -12.0, -9.0), (0.6, 12.0, -9.0), (0.6, 12.0, 9.0), (0.6, -12.0, 9.0)],
          [(1, 0, 0)] * 4, [(0, 1), (1, 1), (1, 0), (0, 0)], [(0, 1, 2), (0, 2, 3)])
    for sy in (-1, 1):                          # trunnions
        tube(m, HOUSING, [(-5.0, sy * 14.0, 0.0), (-5.0, sy * 16.5, 0.0)], 2.6, 8)


def floodmast():
    mesh = Mesh()
    top = FM_TOP
    # foot flange, anchor bolts, tapered 12-sided mast, top cap
    lathe_pts(mesh, GALV, [(0.01, 0.0), (21.0, 0.0), (21.0, 2.5), (15.5, 3.5), (14.6, 8.0), (13.6, 10.0)], 12)
    for k in range(4):
        a = math.pi / 4 + k * math.pi / 2
        x, y = 17.5 * math.cos(a), 17.5 * math.sin(a)
        tube(mesh, IRON, [(x, y, 2.5), (x, y, 6.5)], 1.4, 6)
    frustum(mesh, GALV, 12, 9.0, _mast_r(9.0), 490.0, _mast_r(490.0), uv=1 / 24)
    sphere(mesh, GALV, (0, 0, 490.0), _mast_r(490.0) + 0.8, 12, 5, squash=(1, 1, 0.6))
    for z in (150.0, 300.0):                    # sleeve joints
        frustum(mesh, GALV, 12, z - 2.0, _mast_r(z) + 1.4, z + 2.0, _mast_r(z) + 1.2, cap0=True, cap1=True)
    # junction box and conduit on the +y side
    box(mesh, HOUSING, (-7.0, 11.0, 92.0), (7.0, 20.0, 118.0))
    box(mesh, HOUSING, (-7.6, 10.5, 117.0), (7.6, 20.6, 119.5))
    tube(mesh, GALV, [(0.0, 15.5, 118.0), (0.0, _mast_r(130.0) + 2.4, 130.0), (0.0, _mast_r(top) + 2.4, top)], 1.5, 6)
    # ladder on the -x side with stand-off brackets and a safety cage
    lx = -25.0
    for sy in (-1, 1):
        tube(mesh, GALV, [(lx, sy * 7.5, 14.0), (lx, sy * 7.5, top + 30.0)], 1.0, 5)
    for z in np.arange(26.0, top, 12.0):
        tube(mesh, GALV, [(lx, -7.5, z), (lx, 7.5, z)], 0.7, 4, caps=False)
    for z in np.arange(40.0, top, 80.0):
        for sy in (-1, 1):
            tube(mesh, GALV, [(-_mast_r(z) + 0.5, sy * 3.0, z), (lx, sy * 7.5, z)], 0.9, 4)
    hoops = np.arange(180.0, top - 4.0, 42.0)
    for z in hoops:
        tube(mesh, GALV, arc((lx - 6.0, 0.0, z), 11.0, math.radians(58), math.radians(302), 12, plane="xy"), 0.8, 4)
    for a in (120, 180, 240):
        x, y = lx - 6.0 + 11.0 * math.cos(math.radians(a)), 11.0 * math.sin(math.radians(a))
        tube(mesh, GALV, [(x, y, hoops[0]), (x, y, top + 30.0)], 0.6, 4)
    # platform: grating disc on brackets, railing with two rings
    frustum(mesh, IRON, 16, top - 3.0, 36.0, top, 36.0, cap0=True, cap1=True, uv=1 / 18)
    for k in range(6):
        a = 2 * math.pi * k / 6 + math.pi / 6
        tube(mesh, GALV, [(_mast_r(top - 40) * math.cos(a), _mast_r(top - 40) * math.sin(a), top - 40.0),
                          (33.0 * math.cos(a), 33.0 * math.sin(a), top - 3.5)], 1.2, 5)
    posts = [2 * math.pi * k / 12 for k in range(12)]
    for a in posts:
        if abs(math.cos(a) + 1) < 0.1:          # leave the ladder opening
            continue
        tube(mesh, GALV, [(34.0 * math.cos(a), 34.0 * math.sin(a), top), (34.0 * math.cos(a), 34.0 * math.sin(a), top + 30.0)],
             0.9, 4)
    for z in (top + 15.0, top + 30.0):
        tube(mesh, GALV, arc((0, 0, z), 34.0, math.radians(-150), math.radians(150), 20, plane="xy"), 0.9, 5)
    # head frame: two bars along y in front of the mast, struts, six heads
    rows = (top + 32.0, top + 58.0)
    for z in rows:
        tube(mesh, GALV, [(11.0, -54.0, z), (11.0, 54.0, z)], 1.8, 6)
        for sy in (-1, 1):
            tube(mesh, GALV, [(_mast_r(z) - 1.0, sy * 2.0, z), (11.0, sy * 14.0, z)], 1.4, 5)
    for sy in (-1, 1):
        tube(mesh, GALV, [(11.0, sy * 54.0, rows[0] - 6.0), (11.0, sy * 54.0, rows[1] + 6.0)], 1.8, 6)
    for z in rows:
        for y in (-36.0, 0.0, 36.0):
            yaw = math.radians(-0.25 * y)       # outer heads fan out
            pitch = math.radians(24.0 if z == rows[0] else 16.0)
            R = rot_z(yaw) @ rot_y(pitch)
            c = np.array([25.0, y, z])
            place(mesh, _flood_head, R, c)
            for sy in (-1, 1):                  # yoke: arms from the bar to the trunnions
                piv = c + R @ np.array([-5.0, sy * 16.5, 0.0])
                tube(mesh, GALV, [(11.0, piv[1], z), tuple(piv)], 1.1, 5)
    return mesh


# ---------------------------------------------------------------- textures

def textures(mdir):
    rng = np.random.default_rng(5)
    s = 128
    n = noise(s, 4, 11)
    fine = rng.normal(0, 1, (s, s)) * 0.03
    iron = 0.13 + 0.07 * n + fine
    rust = np.clip(noise(s, 8, 12) - 0.72, 0, 1) * 1.6
    img = np.stack([iron + 0.10 * rust, iron + 0.04 * rust, iron + 0.01 * rust], -1)
    write_tga(os.path.join(mdir, "iron.tga"), (np.clip(img, 0, 1) * 255).astype(np.uint8))

    c = (np.arange(s) + 0.5) / s * 2 - 1
    X, Y = np.meshgrid(c, c)
    glow = np.exp(-(X ** 2 * 1.4 + (Y - 0.1) ** 2) / 0.45)
    frost = 0.85 + 0.15 * noise(s, 8, 13)
    edge = np.clip(1 - np.maximum(abs(X), abs(Y)) ** 8, 0, 1)
    v = glow * frost * edge
    glass = np.stack([v * 1.0, v * 0.72, v * 0.36], -1) * 0.95
    write_tga(os.path.join(mdir, "glass.tga"), (np.clip(glass, 0, 1) * 255).astype(np.uint8))

    f = 64
    c = (np.arange(f) + 0.5) / f * 2 - 1
    X, Y = np.meshgrid(c, c)
    core = np.exp(-(X ** 2 + Y ** 2) / 0.5)
    flame = np.stack([0.9 + 0.1 * core, 0.62 + 0.35 * core, 0.25 + 0.6 * core], -1)
    write_tga(os.path.join(mdir, "flame.tga"), (np.clip(flame, 0, 1) * 255).astype(np.uint8))

    p = noise(s, 4, 14)
    streak = np.repeat(rng.normal(0, 1, (1, s)), s, axis=0) * 0.03
    pole = 0.46 + 0.10 * p + streak + fine
    write_tga(os.path.join(mdir, "pole.tga"), (np.clip(np.stack([pole, pole * 1.01, pole * 1.03], -1), 0, 1) * 255).astype(np.uint8))

    c = (np.arange(s) + 0.5) / s
    X, Y = np.meshgrid(c, c)
    prism = 0.78 + 0.22 * np.abs(np.sin(X * math.pi * 10)) * np.abs(np.sin(Y * math.pi * 6))
    rr = np.sqrt((X - 0.5) ** 2 + (Y - 0.5) ** 2)
    hot = np.clip(1.2 - rr * 1.6, 0.35, 1)
    lens = np.stack([prism * hot, prism * hot * 0.66, prism * hot * 0.30], -1)
    write_tga(os.path.join(mdir, "lens.tga"), (np.clip(lens, 0, 1) * 255).astype(np.uint8))

    # galvanised steel: light grey with spangle
    sp = noise(s, 16, 21)
    g = 0.50 + 0.08 * noise(s, 4, 22) + 0.05 * (sp - 0.5) + fine
    write_tga(os.path.join(mdir, "galv.tga"), (np.clip(np.stack([g * 0.98, g, g * 1.02], -1), 0, 1) * 255).astype(np.uint8))
    # painted housing: dark grey-green, worn edges
    h = 0.20 + 0.05 * noise(s, 4, 23) + fine
    wear = np.clip(noise(s, 12, 24) - 0.8, 0, 1) * 2.0
    write_tga(os.path.join(mdir, "housing.tga"),
              (np.clip(np.stack([h + 0.25 * wear, h * 1.06 + 0.25 * wear, h * 1.0 + 0.25 * wear], -1), 0, 1) * 255).astype(np.uint8))
    # flood lens: reflector rings behind ribbed glass, hot centre; team tints
    w2, h2 = 128, 96
    cx, cy = np.meshgrid((np.arange(w2) + 0.5) / w2 * 2 - 1, (np.arange(h2) + 0.5) / h2 * 2 - 1)
    r = np.sqrt(cx ** 2 + (cy * 0.8) ** 2)
    rings = 0.82 + 0.18 * np.cos(r * 34) ** 2
    ribs = 0.9 + 0.1 * np.abs(np.sin(cx * math.pi * 9))
    hotc = np.clip(1.25 - 0.75 * r, 0.45, 1.0)
    v = rings * ribs * hotc
    for suffix, tint in (("", (1.0, 0.98, 0.92)), ("_red", (1.0, 0.42, 0.32)), ("_blue", (0.42, 0.6, 1.0))):
        img = np.stack([np.minimum(1, v * tint[0] + 0.25 * np.clip(1 - r * 2.2, 0, 1)),
                        np.minimum(1, v * tint[1] + 0.25 * np.clip(1 - r * 2.2, 0, 1)),
                        np.minimum(1, v * tint[2] + 0.25 * np.clip(1 - r * 2.2, 0, 1))], -1)
        write_tga(os.path.join(mdir, f"floodlens{suffix}.tga"), (np.clip(img, 0, 1) * 255).astype(np.uint8))


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", default=os.path.normpath(os.path.join(here, "..", "..", "baseq3r")))
    args = ap.parse_args()
    mdir = os.path.join(args.out, "models", "mapobjects", "q3r_lanterns")
    os.makedirs(mdir, exist_ok=True)
    fm = floodmast()
    for fname, mesh in (("lantern.md3", lantern()), ("lantern_post.md3", lantern_post()),
                        ("lantern_wall.md3", lantern_wall()), ("streetlamp.md3", streetlamp()),
                        ("floodmast.md3", fm), ("floodmast_red.md3", retexture(fm, {FLOOD: FLOOD + "_red"})),
                        ("floodmast_blue.md3", retexture(fm, {FLOOD: FLOOD + "_blue"}))):
        ns, nv, nt, mins, maxs = write_md3(os.path.join(mdir, fname), D + fname, mesh)
        print(f"{fname}: {ns} surfaces, {nv} verts, {nt} tris, bounds {np.round(mins, 1)} .. {np.round(maxs, 1)}")
    textures(mdir)
    print("textures written")


if __name__ == "__main__":
    main()
