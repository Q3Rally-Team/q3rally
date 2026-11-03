#!/usr/bin/env python3
"""Generate the railway models for the maps (misc_model) and their textures.

    python3 tools/mapobjects/make_rail.py [--out baseq3r]

  models/mapobjects/q3r_rail/
    track.md3          256 units of track along x, centred on the origin:
                       two rails (gauge 72, rail top at z 7) on eight wooden
                       sleepers with tie plates. Ground at z 0, the sleepers
                       sit 1 unit into it. Stretch with modelscale_vec.
    boxcar_brown.md3, boxcar_grey.md3, boxcar_red.md3
                       covered freight wagon, 360 long, 104 wide, 194 high:
                       two bogies, underframe, buffers, ribbed body, sliding
                       doors, arched roof, ladders, brake wheel at +x.
    tankcar.md3        tank wagon, 340 long: barrel r 54 at z 104 with dished
                       ends, saddles, dome with walkway and railing.
    flatcar.md3        flat wagon, 300 x 80, plank deck at z 40 (loads such
                       as containers go on top of it).
  plus textures; shaders in scripts/q3r_rail.shader.

All wagons stand on the track (wheels on the rail top at z 7), origin on the
ground at the centre, length along x. misc_models do not collide: give each
wagon an invisible solid brush (see docs/rail.md). Needs numpy.
"""
import argparse
import math
import os

import numpy as np

from meshlib import box_uv as _box_uv
from meshlib import (Mesh, arc, frustum, grid, lathe_pts, noise, place, retexture, rot_x,
                     rot_y, sphere, tube, write_md3, write_tga)

D = "models/mapobjects/q3r_rail/"
RAIL, WOOD, STEEL, WHEEL = D + "rail", D + "sleeper", D + "steel", D + "wheel"
BODY, ROOF, TANK, DECK, HAND = D + "body_brown", D + "roof", D + "tank", D + "deck", D + "handrail"
COLORS = ("brown", "grey", "red")

RAIL_TOP = 7.0


def box_uv(m, shader, lo, hi, scale=1 / 64, skip=("bot",)):
    """box_uv without the bottom face by default (never seen under a wagon)."""
    _box_uv(m, shader, lo, hi, scale, skip)

GAUGE = 36.0                       # half distance of the rails


# ---------------------------------------------------------------- track

def rail_profile(m, x0, x1, y):
    """Flat-bottom rail from x0 to x1 at y: foot, web, head (top at z 7)."""
    ends = ("+x", "-x")
    box_uv(m, RAIL, (x0, y - 3.0, 3.0), (x1, y + 3.0, 3.8), 1 / 32, skip=("bot",) + ends)
    box_uv(m, RAIL, (x0, y - 0.8, 3.8), (x1, y + 0.8, 5.4), 1 / 32, skip=("bot", "top") + ends)
    box_uv(m, RAIL, (x0, y - 1.7, 5.4), (x1, y + 1.7, RAIL_TOP), 1 / 32, skip=("bot",) + ends)


def track():
    m = Mesh()
    for sy in (-1, 1):
        rail_profile(m, -128.0, 128.0, sy * GAUGE)
    for k in range(8):
        x = -112.0 + 32.0 * k
        box_uv(m, WOOD, (x - 7.0, -56.0, -1.0), (x + 7.0, 56.0, 3.0), 1 / 64, skip=("bot",))
        for sy in (-1, 1):                             # tie plates
            box_uv(m, STEEL, (x - 5.0, sy * GAUGE - 6.0, 3.0), (x + 5.0, sy * GAUGE + 6.0, 3.6), 1 / 32,
                   skip=("bot", "+y", "-y"))
    return m


# ---------------------------------------------------------------- running gear

def _wheel(m):
    """Wheel set half: wheel around the local z axis, flange at -z (inside)."""
    lathe_pts(m, WHEEL, [(0.0, -3.4), (13.4, -3.4), (11.6, -1.4), (11.6, 3.2), (4.0, 3.6), (0.0, 5.0)], 8)


def bogie(m, cx):
    """Two-axle bogie centred at x = cx: wheels on the rails, axle boxes,
    side frames with springs, bolster."""
    zc = RAIL_TOP + 12.0
    for ax in (cx - 26.0, cx + 26.0):
        for sy in (-1, 1):
            R = rot_x(-math.pi / 2) if sy > 0 else rot_x(math.pi / 2)
            place(m, _wheel, R, (ax, sy * (GAUGE - 0.2), zc))
            box_uv(m, STEEL, (ax - 5.5, sy * 41.0 - 3.0, zc - 6.0), (ax + 5.5, sy * 41.0 + 3.0, zc + 6.0), 1 / 32)
        tube(m, STEEL, [(ax, -38.0, zc), (ax, 38.0, zc)], 2.2, 5, caps=False)
    for sy in (-1, 1):
        y0, y1 = sorted((sy * 44.0, sy * 47.5))
        box_uv(m, STEEL, (cx - 40.0, y0, zc - 3.0), (cx + 40.0, y1, zc + 2.0), 1 / 32)        # lower chord
        box_uv(m, STEEL, (cx - 34.0, y0, zc + 9.0), (cx + 34.0, y1, zc + 14.0), 1 / 32)       # top chord
        for dx in (-34.0, -18.0, 18.0, 34.0):
            box_uv(m, STEEL, (cx + dx - 2.5, y0, zc), (cx + dx + 2.5, y1, zc + 10.0), 1 / 32)
        for dx in (-7.0, 0.0, 7.0):                    # springs
            tube(m, WHEEL, [(cx + dx, sy * 45.7, zc + 1.0), (cx + dx, sy * 45.7, zc + 10.0)], 2.6, 5, caps=False)
    box_uv(m, STEEL, (cx - 10.0, -47.0, zc + 10.0), (cx + 10.0, 47.0, zc + 17.0), 1 / 32)     # bolster
    tube(m, STEEL, [(cx, 0.0, zc + 17.0), (cx, 0.0, 38.0)], 7.0, 8, caps=False)             # centre pivot


def underframe(m, half_len, half_w=50.0, top=46.0):
    """Centre sill, side sills, end beams, buffers and coupler hooks."""
    box_uv(m, STEEL, (-half_len, -9.0, top - 10.0), (half_len, 9.0, top - 2.0), 1 / 32)
    for sy in (-1, 1):
        y0, y1 = sorted((sy * half_w, sy * (half_w - 4.0)))
        box_uv(m, STEEL, (-half_len, y0, top - 9.0), (half_len, y1, top), 1 / 32)
    for sx in (-1, 1):
        x0, x1 = sorted((sx * half_len, sx * (half_len - 5.0)))
        box_uv(m, STEEL, (x0, -half_w, top - 14.0), (x1, half_w, top), 1 / 32)
        for sy in (-1, 1):                              # buffers
            bx = sx * half_len
            tube(m, STEEL, [(bx, sy * 30.0, top - 7.0), (bx + sx * 8.0, sy * 30.0, top - 7.0)], 3.2, 6, caps=False)
            tube(m, STEEL, [(bx + sx * 8.0, sy * 30.0, top - 7.0), (bx + sx * 13.0, sy * 30.0, top - 7.0)], 2.0, 6,
                 caps=False)
            frustum_x(m, STEEL, bx + sx * 13.0, bx + sx * 15.0, sy * 30.0, top - 7.0, 7.0)
        tube(m, STEEL, [(sx * half_len, 0.0, top - 8.0), (sx * (half_len + 10.0), 0.0, top - 8.0)], 1.6, 5,
             caps=False)
        tube(m, STEEL, arc((sx * (half_len + 12.0), 0.0, top - 8.0), 2.4, 0, 2 * math.pi, 6)[:-1], 0.8, 4,
             closed=True)


def frustum_x(m, shader, x0, x1, y, z, r):
    """Short disc along x (buffer plate) from x0 to x1."""
    a, b = sorted((x0, x1))

    def build(mm):
        frustum(mm, shader, 8, 0.0, r, b - a, r, cap0=True, cap1=True, uv=1 / 16)
    place(m, build, rot_y(-math.pi / 2), (a, y, z))


def ladder(m, x0, x1, y, z0, z1, outward, step=16.0, shader=HAND):
    """Ladder on a wall at y (stand-off 2.5 towards outward = +1 / -1)."""
    yy = y + outward * 2.5
    for x in (x0, x1):
        tube(m, shader, [(x, yy, z0), (x, yy, z1)], 0.8, 4)
        tube(m, shader, [(x, y, z1 - 4.0), (x, yy, z1 - 4.0)], 0.7, 4, caps=False)
        tube(m, shader, [(x, y, z0 + 4.0), (x, yy, z0 + 4.0)], 0.7, 4, caps=False)
    for z in np.arange(z0 + 8.0, z1 - 2.0, step):
        tube(m, shader, [(x0, yy, z), (x1, yy, z)], 0.6, 4, caps=False)


# ---------------------------------------------------------------- wagons

def boxcar(body=BODY):
    m = Mesh()
    HL, HW, Z0, Z1 = 180.0, 52.0, 46.0, 184.0
    for cx in (-125.0, 125.0):
        bogie(m, cx)
    underframe(m, HL)
    box_uv(m, body, (-HL + 4.0, -HW, Z0), (HL - 4.0, HW, Z1), 1 / 96)
    # side ribs, top and bottom chords
    for sy in (-1, 1):
        y0, y1 = sorted((sy * HW, sy * (HW + 1.6)))
        for x in np.arange(-HL + 18.0, HL - 10.0, 28.0):
            if abs(x) < 50.0:
                continue
            box_uv(m, body, (x - 1.6, y0, Z0 + 4.0), (x + 1.6, y1, Z1 - 4.0), 1 / 96)
        box_uv(m, body, (-HL + 4.0, y0, Z1 - 4.0), (HL - 4.0, y1, Z1), 1 / 96)
        box_uv(m, STEEL, (-HL + 4.0, y0, Z0), (HL - 4.0, y1, Z0 + 4.0), 1 / 32)
        # sliding door on rails, with handles
        d0, d1 = sorted((sy * (HW + 1.6), sy * (HW + 3.6)))
        box_uv(m, body, (-46.0, d0, Z0 + 6.0), (46.0, d1, Z1 - 8.0), 1 / 96)
        e0, e1 = sorted((sy * (HW + 3.6), sy * (HW + 4.6)))
        for x in (-44.6, -15.0, 15.0, 44.6):           # door stiffeners
            box_uv(m, body, (x - 1.4, e0, Z0 + 6.0), (x + 1.4, e1, Z1 - 8.0), 1 / 96)
        r0, r1 = sorted((sy * HW, sy * (HW + 5.0)))
        box_uv(m, STEEL, (-100.0, r0, Z1 - 8.0), (100.0, r1, Z1 - 4.0), 1 / 32)               # door rail
        box_uv(m, STEEL, (-100.0, r0, Z0 + 2.0), (100.0, r1, Z0 + 5.0), 1 / 32)
        for x in (-38.0, 38.0):
            tube(m, HAND, [(x, sy * (HW + 3.6), 100.0), (x, sy * (HW + 6.4), 102.0), (x, sy * (HW + 6.4), 126.0),
                           (x, sy * (HW + 3.6), 128.0)], 0.8, 5)
        # corner ladders
        ladder(m, HL - 22.0, HL - 9.0, sy * (HW + 1.6), Z0 - 14.0, Z1 - 2.0, sy)
        ladder(m, -HL + 9.0, -HL + 22.0, sy * (HW + 1.6), Z0 - 14.0, Z1 - 2.0, sy)
    # end ribs
    for sx in (-1, 1):
        x0, x1 = sorted((sx * (HL - 4.0), sx * (HL - 2.4)))
        for y in (-34.0, -12.0, 12.0, 34.0):
            box_uv(m, body, (x0, y - 1.6, Z0), (x1, y + 1.6, Z1), 1 / 96)
    # brake wheel on the +x end
    bw = (HL - 1.0, 26.0, 150.0)
    tube(m, HAND, [(bw[0] + 4.0 + 9.0 * 0, bw[1] + 9.0 * math.cos(a), bw[2] + 9.0 * math.sin(a))
                   for a in np.linspace(0, 2 * math.pi, 15)][:-1], 1.0, 5, closed=True)
    for a in (0, math.pi / 3, 2 * math.pi / 3):
        tube(m, HAND, [(bw[0] + 4.0, bw[1] + 9.0 * math.cos(a), bw[2] + 9.0 * math.sin(a)),
                       (bw[0] + 4.0, bw[1] - 9.0 * math.cos(a), bw[2] - 9.0 * math.sin(a))], 0.6, 4)
    tube(m, STEEL, [(bw[0] - 2.0, bw[1], bw[2]), (bw[0] + 5.0, bw[1], bw[2])], 1.4, 6)
    tube(m, STEEL, [(bw[0] + 1.0, bw[1], bw[2]), (bw[0] + 1.0, bw[1], Z0 + 4.0)], 1.2, 6)
    # arched roof with seams and a little overhang
    P, N, U, V = [], [], [], []
    ys = np.linspace(-HW - 3.0, HW + 3.0, 11)
    for x in (-HL, HL):
        rp, rn, ru, rv = [], [], [], []
        for y in ys:
            t = y / (HW + 3.0)
            z = Z1 + 11.0 * (1 - t * t)
            rp.append((x, y, z))
            nrm = np.array([0.0, 22.0 * t / (HW + 3.0), 1.0])
            rn.append(tuple(nrm / np.linalg.norm(nrm)))
            ru.append(x / 96)
            rv.append(y / 96)
        P.append(rp), N.append(rn), U.append(ru), V.append(rv)
    grid(m, ROOF, P, N, U, V)
    # roof underside / end caps (thin)
    for x, sx in ((-HL, -1), (HL, 1)):
        verts = [(x, y, Z1 + 11.0 * (1 - (y / (HW + 3.0)) ** 2)) for y in ys] + [(x, HW + 3.0, Z1 - 1.0),
                                                                                  (x, -HW - 3.0, Z1 - 1.0)]
        n = len(ys)
        tris = [(n + 1, k, k + 1) for k in range(n - 1)] + [(n + 1, n - 1, n)]
        m.add(ROOF, verts, [(sx, 0, 0)] * len(verts), [(v[1] / 96, v[2] / 96) for v in verts], tris)
    box_uv(m, ROOF, (-HL, -HW - 3.0, Z1 - 1.0), (HL, HW + 3.0, Z1), 1 / 96)
    return m


def tankcar():
    m = Mesh()
    HL, R, ZC = 170.0, 54.0, 104.0
    for cx in (-115.0, 115.0):
        bogie(m, cx)
    underframe(m, HL, half_w=44.0)
    # barrel along x with dished ends (lathe around z, turned onto x)
    prof = [(0.01, -168.0)]
    for k in range(1, 7):
        a = math.pi / 2 * k / 6
        prof.append((R * math.sin(a), -150.0 - 18.0 * math.cos(a)))
    prof += [(R, 150.0)]
    for k in range(1, 7):
        a = math.pi / 2 * (6 - k) / 6
        prof.append((R * math.sin(a), 150.0 + 18.0 * math.cos(a)))
    prof[-1] = (0.01, 168.0)

    def barrel(mm):
        lathe_pts(mm, TANK, prof, 24, u_rep=3)
    place(m, barrel, rot_y(math.pi / 2), (0.0, 0.0, ZC))
    for x in (-110.0, -40.0, 40.0, 110.0):            # straps
        def band(mm, x=x):
            frustum(mm, STEEL, 24, x - 1.5, R + 0.5, x + 1.5, R + 0.5, uv=1 / 16)
        place(m, band, rot_y(math.pi / 2), (0.0, 0.0, ZC))
    for x in (-110.0, 110.0):                         # saddles
        box_uv(m, STEEL, (x - 8.0, -40.0, 40.0), (x + 8.0, 40.0, ZC - 40.0), 1 / 32)
    # dome, hatch and safety valves
    lathe_pts(m, TANK, [(0.01, ZC + 40.0), (19.0, ZC + 40.0), (19.0, ZC + 64.0), (17.0, ZC + 67.0),
                        (8.0, ZC + 69.0), (0.01, ZC + 69.0)], 18)
    tube(m, STEEL, [(0.0, 0.0, ZC + 69.0), (0.0, 0.0, ZC + 72.0)], 9.0, 12)
    for y in (-11.0, 11.0):
        tube(m, STEEL, [(-11.0, y, ZC + 50.0), (-11.0, y, ZC + 78.0)], 1.8, 6)
    # walkway around the dome with railing, ladders to it on both sides
    WZ = ZC + 50.0
    box_uv(m, STEEL, (-40.0, -38.0, WZ - 1.5), (40.0, 38.0, WZ), 1 / 32)
    for sy in (-1, 1):
        for x in (-30.0, 30.0):
            tube(m, STEEL, [(x, sy * 36.0, WZ - 1.5), (x, sy * 30.0, ZC + 32.0)], 1.2, 5)
    corners = [(-38.0, -36.0), (38.0, -36.0), (38.0, 36.0), (-38.0, 36.0)]
    for x, y in corners:
        tube(m, HAND, [(x, y, WZ), (x, y, WZ + 28.0)], 0.9, 5)
    for z in (WZ + 14.0, WZ + 28.0):
        for (xa, ya), (xb, yb) in zip(corners, corners[1:] + corners[:1]):
            if abs(ya - yb) < 1:                       # long sides only; ends stay open for the ladders
                tube(m, HAND, [(xa, ya, z), (xb, yb, z)], 0.8, 5)
    for sy in (-1, 1):
        y_top, y_bot = sy * 40.0, sy * 58.0
        for x in (-6.0, 6.0):
            tube(m, HAND, [(x, y_top, WZ), (x, y_bot, 40.0)], 0.8, 5)
        for t in np.linspace(0.1, 0.9, 7):
            y, z = y_top + (y_bot - y_top) * t, WZ + (40.0 - WZ) * t
            tube(m, HAND, [(-6.0, y, z), (6.0, y, z)], 0.6, 4, caps=False)
    # end platforms with handrails
    for sx in (-1, 1):
        x0, x1 = sorted((sx * (HL - 18.0), sx * HL))
        box_uv(m, STEEL, (x0, -44.0, 46.0), (x1, 44.0, 48.0), 1 / 32)
        for sy in (-1, 1):
            tube(m, HAND, [(sx * (HL - 2.0), sy * 42.0, 48.0), (sx * (HL - 2.0), sy * 42.0, 74.0),
                           (sx * (HL - 16.0), sy * 42.0, 74.0), (sx * (HL - 16.0), sy * 42.0, 60.0)], 0.8, 5)
    return m


def flatcar():
    m = Mesh()
    HL = 150.0
    for cx in (-105.0, 105.0):
        bogie(m, cx)
    underframe(m, HL, half_w=40.0, top=34.0)
    box_uv(m, DECK, (-HL, -40.0, 34.0), (HL, 40.0, 40.0), 1 / 64)
    for sy in (-1, 1):
        y0, y1 = sorted((sy * 40.0, sy * 43.0))
        box_uv(m, STEEL, (-HL, y0, 24.0), (HL, y1, 40.0), 1 / 32)
        for x in np.arange(-HL + 20.0, HL - 10.0, 40.0):                         # stake pockets
            box_uv(m, STEEL, (x - 3.0, sy * 43.0 - (0 if sy > 0 else 3.0), 28.0),
                   (x + 3.0, sy * 43.0 + (3.0 if sy > 0 else 0), 38.0), 1 / 32)
    for x in (-HL + 30.0, HL - 30.0):                                            # container twist locks
        for y in (-40.0, 40.0):
            box_uv(m, STEEL, (x - 3.0, y - 3.0 + (3.0 if y < 0 else -3.0), 40.0),
                   (x + 3.0, y + 3.0 + (3.0 if y < 0 else -3.0), 41.5), 1 / 32)
    return m


# ---------------------------------------------------------------- textures

def textures(mdir):
    rng = np.random.default_rng(31)
    s = 128
    fine = rng.normal(0, 1, (s, s)) * 0.025
    yy, xx = np.mgrid[0:s, 0:s] / s

    def save(name, img):
        write_tga(os.path.join(mdir, name + ".tga"), (np.clip(img, 0, 1) * 255).astype(np.uint8))

    # rail: rusty brown steel
    r = 0.30 + 0.08 * noise(s, 8, 1) + fine
    save("rail", np.stack([r * 1.15, r * 0.82, r * 0.62], -1))
    # sleeper: dark creosoted wood, grain along u
    grain = np.sin(yy * 2 * math.pi * 9 + 3.0 * noise(s, 4, 2)) * 0.04
    w = 0.22 + 0.06 * noise(s, 8, 3) + grain + fine
    save("sleeper", np.stack([w * 1.10, w * 0.88, w * 0.70], -1))
    # steel: dark grey, a little rust
    st = 0.20 + 0.05 * noise(s, 8, 4) + fine
    rust = np.clip(noise(s, 8, 5) - 0.7, 0, 1) * 1.2
    save("steel", np.stack([st + 0.12 * rust, st + 0.05 * rust, st + 0.02 * rust], -1))
    # wheel: steel with a lighter worn tread
    wh = 0.28 + 0.06 * noise(s, 8, 6) + fine
    save("wheel", np.stack([wh * 1.05, wh * 1.0, wh * 0.96], -1))
    # handrails: worn safety yellow
    hy = 0.75 + 0.1 * noise(s, 8, 7) + fine
    dirt = np.clip(noise(s, 8, 8) - 0.5, 0, 1)
    save("handrail", np.stack([hy - 0.3 * dirt, hy * 0.72 - 0.3 * dirt, hy * 0.12], -1))
    # roof: weathered galvanised sheet
    rf = 0.42 + 0.08 * noise(s, 4, 9) + 0.03 * np.sin(xx * 2 * math.pi * 4) + fine
    save("roof", np.stack([rf * 0.98, rf, rf * 1.01], -1))
    # tank: off-white paint with grime running down
    streak = np.repeat(noise(s, 16, 10)[:1], s, axis=0)
    tk = 0.80 + 0.05 * noise(s, 4, 11) - 0.12 * np.clip(streak - 0.4, 0, 1) * yy + fine
    save("tank", np.stack([tk, tk * 0.98, tk * 0.92], -1))
    # deck: planks across the wagon
    plank = (np.floor(xx * 8) % 2) * 0.03 + 0.04 * np.sin(yy * 2 * math.pi * 3 + noise(s, 4, 12) * 4)
    gap = (np.abs((xx * 8) % 1 - 0.5) > 0.47) * -0.12
    dk = 0.36 + 0.06 * noise(s, 8, 13) + plank + gap + fine
    save("deck", np.stack([dk * 1.08, dk * 0.92, dk * 0.74], -1))
    # wagon bodies: vertical boards / panels with weathering, three colours
    S2 = 256
    yy2, xx2 = np.mgrid[0:S2, 0:S2] / S2
    fine2 = rng.normal(0, 1, (S2, S2)) * 0.02
    boards = 0.04 * np.cos(xx2 * 2 * math.pi * 6) ** 8
    run = np.repeat(noise(S2, 32, 14)[:1], S2, axis=0)
    grime = 0.14 * np.clip(run - 0.45, 0, 1) * (0.3 + yy2)
    base = 0.85 + 0.08 * noise(S2, 4, 15) - boards - grime + fine2
    flake = np.clip(noise(S2, 16, 16) - 0.78, 0, 1) * 2.0
    for name, col in (("brown", (0.50, 0.30, 0.20)), ("grey", (0.52, 0.53, 0.55)), ("red", (0.62, 0.20, 0.15))):
        img = np.stack([base * col[0] + 0.18 * flake * 0.55, base * col[1] + 0.18 * flake * 0.30,
                        base * col[2] + 0.18 * flake * 0.18], -1)
        save("body_" + name, img)


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", default=os.path.normpath(os.path.join(here, "..", "..", "baseq3r")))
    args = ap.parse_args()
    mdir = os.path.join(args.out, "models", "mapobjects", "q3r_rail")
    os.makedirs(mdir, exist_ok=True)
    bc = boxcar()
    models = [("track.md3", track())]
    models += [(f"boxcar_{c}.md3", retexture(bc, {BODY: D + "body_" + c})) for c in COLORS]
    models += [("tankcar.md3", tankcar()), ("flatcar.md3", flatcar())]
    for fname, mesh in models:
        ns, nv, nt, mins, maxs = write_md3(os.path.join(mdir, fname), D + fname, mesh)
        print(f"{fname}: {ns} surfaces, {nv} verts, {nt} tris, bounds {np.round(mins, 1)} .. {np.round(maxs, 1)}")
    textures(mdir)
    print("textures written")


if __name__ == "__main__":
    main()
