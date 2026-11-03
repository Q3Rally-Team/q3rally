"""Shared helpers for building Q3Rally car models (body.md3 / wheel.md3).

Q3 axes: X = forward, Y = left, Z = up. Triangles are passed in with
counter-clockwise winding (OBJ/FBX convention); Builder flips them to the
clockwise order md3 uses.
"""
import math

import md3

ID = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
BACKWARD = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]   # same as the other cars' turbo/smoke tags
WL = [[0, 1, 0], [1, 0, 0], [0, 0, -1]]          # left wheels: wheel model +x points out of the car
WR = [[0, -1, 0], [-1, 0, 0], [0, 0, -1]]        # right wheels

# The physics puts the wheel centres one spring length below the body origin and
# lets them touch the ground with WHEEL_RADIUS 8 (bg_physics.h); cgame draws a
# wheel at its tag moved up by 8.2 - spring length (cg_players.c). A tyre of
# radius r therefore stands on the ground when its tag is at Z = r - GROUND.
GROUND = 16.2


def wheel_tag_z(radius):
    return radius - GROUND


def tri_normal(pts):
    a, b, c = pts
    u = [b[k] - a[k] for k in range(3)]
    v = [c[k] - a[k] for k in range(3)]
    n = (u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0])
    l = math.sqrt(sum(x * x for x in n)) or 1.0
    return tuple(x / l for x in n)


class Builder:
    """collects one md3 surface; vertices are unique per (pos, normal, uv)"""

    def __init__(self, name, shader, offset=(0.0, 0.0, 0.0)):
        self.offset = offset                       # added to every position when it is written
        self.s = md3.Surf()
        self.s.name, self.s.flags, self.s.shaders = name, 0, [(shader, 0)]
        self.s.tris, self.s.st, self.verts, self.idx = [], [], [], {}

    def vert(self, pos, n, uv):
        q = [int(round((c + o) * 64)) for c, o in zip(pos, self.offset)]
        key = (tuple(q), md3.enc_normal(*n), round(uv[0], 5), round(uv[1], 5))
        if key not in self.idx:
            self.idx[key] = len(self.verts)
            self.verts.append(q + [key[1]])
            self.s.st.append([uv[0], uv[1]])
        return self.idx[key]

    def tri(self, corners):
        # md3 triangles run the other way round than OBJ/FBX faces
        self.s.tris.append([corners[0], corners[2], corners[1]])

    def done(self):
        self.s.frames = [self.verts]
        return self.s


def snap_lights(tags, builders, gap=0.5, around=1.0, reach=1.0):
    """Move the light tags just outside the car along X.

    The game draws a glow sprite (gfx/flares, 13.2 units across, autosprite:
    always facing the camera, depth tested) on every tag_hlite*/blite*/rlite*.
    A tag inside the lamp housing hides the sprite in the body except for the
    bits that stick out at the edges, and on a curved nose or tail the panels
    next to the lamp hide its centre as soon as the camera looks from an angle.
    So rays along X at the tag's Y/Z and around it (within `around` units) find
    the outermost surface of the written model near the lamp (front for
    headlights, rear for brake and reverse lights); the tag goes gap units in
    front of that. Surfaces more than `reach` units out from the lamp (a bull
    bar, a wing) are not counted. Tags whose rays all miss the car stay where
    they are.
    """
    import numpy as np
    tris = []
    for b in builders:
        P = np.array([[c / 64 for c in v[:3]] for v in b.verts])
        if len(b.s.tris):
            tris.append(P[np.array(b.s.tris)])
    T = np.concatenate(tris)
    a, bb, c = T[:, 0], T[:, 1], T[:, 2]
    d = (bb[:, 1] - a[:, 1]) * (c[:, 2] - a[:, 2]) - (c[:, 1] - a[:, 1]) * (bb[:, 2] - a[:, 2])
    ok = np.abs(d) > 1e-9
    d = np.where(ok, d, 1)

    def ray(y, z):
        u = ((y - a[:, 1]) * (c[:, 2] - a[:, 2]) - (c[:, 1] - a[:, 1]) * (z - a[:, 2])) / d
        v = ((bb[:, 1] - a[:, 1]) * (z - a[:, 2]) - (y - a[:, 1]) * (bb[:, 2] - a[:, 2])) / d
        hit = ok & (u >= 0) & (v >= 0) & (u + v <= 1)
        return (a[:, 0] + u * (bb[:, 0] - a[:, 0]) + v * (c[:, 0] - a[:, 0]))[hit]

    offsets = [(dy, dz) for dy in np.linspace(-around, around, 5) for dz in np.linspace(-around, around, 5)
               if dy * dy + dz * dz <= around * around + 1e-9]
    out = []
    for name, o, axis in tags:
        front = name.startswith('tag_hlite')
        if not (front or name.startswith('tag_blite') or name.startswith('tag_rlite')):
            out.append((name, o, axis))
            continue
        lamp = ray(o[1], o[2])
        if not len(lamp):
            out.append((name, o, axis))
            continue
        # the lamp: the surface on the tag's own ray closest to the old tag
        x0 = lamp[np.argmin(np.abs(lamp - o[0]))]
        xs = np.concatenate([ray(o[1] + dy, o[2] + dz) for dy, dz in offsets])
        xs = xs[(xs - x0) * (1 if front else -1) <= reach]           # not the bull bar in front
        xs = np.append(xs, x0)
        out.append((name, [float(xs.max() + gap) if front else float(xs.min() - gap), o[1], o[2]], axis))
    return out


def save(path, name, surfs, tags, frame_name):
    """write a single-frame md3 with bounds computed from the surfaces"""
    m = md3.Model()
    m.name, m.flags = name, 0
    m.surfs = surfs
    m.tags = [[[n, list(o), [list(r) for r in a]] for n, o, a in tags]] if tags else []
    allp = [[c / 64 for c in v[:3]] for s in m.surfs for v in s.frames[0]]
    mn = [min(p[k] for p in allp) for k in range(3)]
    mx = [max(p[k] for p in allp) for k in range(3)]
    rad = max(math.sqrt(sum(c * c for c in p)) for p in allp)
    m.frames = [mn + mx + [0.0, 0.0, 0.0, rad, frame_name]]
    md3.save(m, path)
    return mn, mx, rad


def build_wheel(tire, radius, path, name):
    """Wheel md3 from a tyre mesh for the shared rim selection.

    tire: list of (pts, normals) triangles in wheel model space: x = out of the
    car (left wheel), y = forward, z = down, centred on the axle.
    The tread and shoulders get the cylindrical mapping of tread.tga
    (w_tread); the sidewalls and a dished disc closing the rim opening get the
    planar mapping of the rim textures (w_rim), like the other cars' wheels.
    """
    tread = Builder('w_tread', 'models/players/wheels/tread.tga')
    rim = Builder('w_rim', 'models/players/wheels/rim_mm_m11_2.tga')
    R = radius

    def rim_uv(p):
        return [0.5 - p[1] / (2 * R), 0.5 + p[2] / (2 * R)]

    wx = [p[0] for pts, _ in tire for p in pts]
    XMIN, XMAX = min(wx), max(wx)
    open_out = open_in = R
    for pts, ns in tire:
        fn = tri_normal(pts)
        if abs(fn[0]) < 0.6:                          # tread and shoulders
            ang = [math.atan2(p[2], p[1]) for p in pts]
            for k in (1, 2):                          # keep the triangle on one branch
                while ang[k] - ang[0] > math.pi:
                    ang[k] -= 2 * math.pi
                while ang[k] - ang[0] < -math.pi:
                    ang[k] += 2 * math.pi
            # tread.tga repeats twice around the wheel; u runs with the angle
            uvs = [[a / math.pi + 1.0, (p[0] - XMIN) / (XMAX - XMIN)] for a, p in zip(ang, pts)]
            tread.tri([tread.vert(p, n, uv) for p, n, uv in zip(pts, ns, uvs)])
        else:                                         # sidewalls
            rim.tri([rim.vert(p, n, rim_uv(p)) for p, n in zip(pts, ns)])
            for p in pts:
                r = math.hypot(p[1], p[2])
                if p[0] > 0:
                    open_out = min(open_out, r)
                else:
                    open_in = min(open_in, r)

    def disc(x_edge, r_edge, depth, outward):
        """fine dished disc closing the rim opening; outward = +1 (outer face) or -1"""
        SEG, RINGS = 96, 6
        n_face = (outward, 0.0, 0.0)
        rings = []
        for i in range(RINGS + 1):
            f = 1 - i / RINGS                          # 1 at the tyre, 0 in the centre
            r = (r_edge + 0.15) * f
            x = x_edge - outward * depth * (1 - f * f)
            rings.append([(x, r * math.cos(2 * math.pi * j / SEG), r * math.sin(2 * math.pi * j / SEG))
                          for j in range(SEG)])
        for i in range(RINGS):
            for j in range(SEG):
                a, b = rings[i][j], rings[i][(j + 1) % SEG]
                c, d = rings[i + 1][j], rings[i + 1][(j + 1) % SEG]
                quad = [(a, b, d), (a, d, c)] if outward > 0 else [(a, d, b), (a, c, d)]
                for tri in quad:
                    if tri_normal(tri)[0] * outward < 0:
                        tri = (tri[0], tri[2], tri[1])
                    rim.tri([rim.vert(p, n_face, rim_uv(p)) for p in tri])

    x_out = max(p[0] for pts, _ in tire for p in pts if math.hypot(p[1], p[2]) < open_out + 0.3)
    x_in = min(p[0] for pts, _ in tire for p in pts if math.hypot(p[1], p[2]) < open_in + 0.3)
    disc(x_out, open_out, 1.2, +1)
    disc(x_in, open_in, 0.0, -1)
    mn, mx, _ = save(path, name, [tread.done(), rim.done()], None, b'wheel')
    return open_out, open_in, mn, mx
