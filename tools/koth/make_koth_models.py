#!/usr/bin/env python3
"""Generate the King of the Hill indicator models and their textures.

    python3 tools/koth/make_koth_models.py [--out baseq3r]

  models/mapobjects/koth/crown.md3   floating crown above the hill centre:
      surfaces crown_gold (gold band, spikes, arches, orb and cross),
      crown_gem (gems on the band, spike tips, pearls on the arches) and
      crown_velvet (cap inside). Gems and velvet use rgbGen entity: cgame
      colours them with the hill owner.
  models/mapobjects/koth/post.md3    bollard on the hill border: post_metal
      (dark body) and post_lamp (lamp ring, rgbGen entity: lights up in the
      team colour as the hill is captured).
  textures for both and gfx/misc/koth_glow.tga (soft additive sprite).

Shaders: scripts/q3r_koth.shader. Code: cgame/cg_hud_vehicle.c
(CG_AddKOTHHillIndicatorToScene). Needs numpy.
Units: crown about 84 high and 80 wide at scale 1 (cgame scales it up);
post 53 high.
"""
import argparse
import math
import os
import struct

import numpy as np

MAX_VERTS, MAX_INDEXES = 900, 5400       # renderer limits are 1000 / 6000 per surface
GOLD, GEM, VELVET = ("models/mapobjects/koth/crown_gold", "models/mapobjects/koth/crown_gem",
                     "models/mapobjects/koth/crown_velvet")
METAL, LAMP = "models/mapobjects/koth/post_metal", "models/mapobjects/koth/post_lamp"

R0 = 36.0          # crown radius at the bottom of the band
FLARE = 0.12       # radius grows with height
BAND = 19.0        # band height


def cone_r(z):
    return R0 + FLARE * z


# ---------------------------------------------------------------- mesh builder

class Mesh:
    """Triangles per shader; split into several md3 surfaces when needed."""

    def __init__(self):
        self.parts = {}

    def add(self, shader, verts, normals, sts, tris, outward=None):
        """verts/normals/sts: lists; tris index into them. Triangles are turned
        so that they face along the vertex normals (Q3: clockwise from outside)."""
        lst = self.parts.setdefault(shader, [])
        V = np.array(verts, float)
        N = np.array(normals, float)
        N /= np.maximum(np.linalg.norm(N, axis=1), 1e-9)[:, None]
        fixed = []
        for a, b, c in tris:
            n = np.cross(V[b] - V[a], V[c] - V[a])
            if np.dot(n, N[a] + N[b] + N[c]) > 0:
                b, c = c, b
            fixed.append((a, b, c))
        lst.append((V, N, np.array(sts, float), fixed))

    def surfaces(self):
        out = []
        for shader, pieces in self.parts.items():
            cur = None
            for V, N, S, T in pieces:
                if cur is None or len(cur[0]) + len(V) > MAX_VERTS or 3 * (len(cur[3]) + len(T)) > MAX_INDEXES:
                    cur = [[], [], [], []]
                    out.append((shader, cur))
                base = len(cur[0])
                cur[0] += list(V)
                cur[1] += list(N)
                cur[2] += list(S)
                cur[3] += [(a + base, b + base, c + base) for a, b, c in T]
        return out


def grid(mesh, shader, P, Nrm, U, Vv, closed_u=False):
    """Quad grid from arrays [rows][cols] of points, normals and uv."""
    rows, cols = len(P), len(P[0])
    verts, norms, sts, tris = [], [], [], []
    for i in range(rows):
        for j in range(cols):
            verts.append(P[i][j])
            norms.append(Nrm[i][j])
            sts.append((U[i][j], Vv[i][j]))
    for i in range(rows - 1):
        for j in range(cols - 1):
            a, b = i * cols + j, i * cols + j + 1
            c, d = a + cols, b + cols
            tris += [(a, b, d), (a, d, c)]
    mesh.add(shader, verts, norms, sts, tris)


def lathe(mesh, shader, profile, segs, u_rep=1.0, phase=0.0, radial=None):
    """profile: list of (r, z, nr, nz, v). Rotated around z."""
    P, N, U, V = [], [], [], []
    for r, z, nr, nz, v in profile:
        rowp, rown, rowu, rowv = [], [], [], []
        for s in range(segs + 1):
            a = phase + 2 * math.pi * s / segs
            k = radial(a) if radial else 1.0
            rowp.append((r * k * math.cos(a), r * k * math.sin(a), z))
            rown.append((nr * math.cos(a), nr * math.sin(a), nz))
            rowu.append(u_rep * s / segs)
            rowv.append(v)
        P.append(rowp), N.append(rown), U.append(rowu), V.append(rowv)
    grid(mesh, shader, P, N, U, V)


def profile_from_points(pts, smooth=True):
    """(r, z) polyline -> (r, z, nr, nz, v) with outward (right-hand) normals.
    Corners are split when smooth is False or the turn is sharp."""
    out = []
    length = 0.0
    seg_n = []
    for (r0, z0), (r1, z1) in zip(pts, pts[1:]):
        dr, dz = r1 - r0, z1 - z0
        ln = math.hypot(dr, dz) or 1
        seg_n.append((dz / ln, -dr / ln))           # right normal of the direction of travel
    acc = [0.0]
    for (r0, z0), (r1, z1) in zip(pts, pts[1:]):
        length += math.hypot(r1 - r0, z1 - z0)
        acc.append(length)
    for i, (r, z) in enumerate(pts):
        ns = []
        if i > 0:
            ns.append(seg_n[i - 1])
        if i < len(seg_n):
            ns.append(seg_n[i])
        if len(ns) == 2 and (ns[0][0] * ns[1][0] + ns[0][1] * ns[1][1] < 0.7 or not smooth):
            out.append((r, z, ns[0][0], ns[0][1], acc[i] / 24))
            out.append((r, z, ns[1][0], ns[1][1], acc[i] / 24))
        else:
            nr = sum(n[0] for n in ns)
            nz = sum(n[1] for n in ns)
            ln = math.hypot(nr, nz) or 1
            out.append((r, z, nr / ln, nz / ln, acc[i] / 24))
    return out


def lathe_pts(mesh, shader, pts, segs, u_rep=1.0, radial=None):
    """Rotate a polyline; split at sharp corners so each run is smooth."""
    prof = profile_from_points(pts)
    # cut into runs at duplicated points (sharp corners)
    runs, cur = [], [prof[0]]
    for p in prof[1:]:
        if abs(p[0] - cur[-1][0]) < 1e-9 and abs(p[1] - cur[-1][1]) < 1e-9:
            runs.append(cur)
            cur = [p]
        else:
            cur.append(p)
    runs.append(cur)
    for run in runs:
        if len(run) >= 2:
            lathe(mesh, shader, run, segs, u_rep, radial=radial)


def sphere(mesh, shader, c, r, segs=10, rings=7, squash=(1, 1, 1), axis=None):
    """UV sphere; squash scales x y z; axis (ex, ey, ez) rotates it."""
    ex, ey, ez = axis if axis else ((1, 0, 0), (0, 1, 0), (0, 0, 1))
    P, N, U, V = [], [], [], []
    for i in range(rings + 1):
        th = math.pi * i / rings
        rp, rn, ru, rv = [], [], [], []
        for j in range(segs + 1):
            ph = 2 * math.pi * j / segs
            lx, ly, lz = math.sin(th) * math.cos(ph), math.sin(th) * math.sin(ph), math.cos(th)
            px, py, pz = lx * squash[0] * r, ly * squash[1] * r, lz * squash[2] * r
            nx, ny, nz = lx / squash[0], ly / squash[1], lz / squash[2]
            rp.append(tuple(c[k] + px * ex[k] + py * ey[k] + pz * ez[k] for k in range(3)))
            rn.append(tuple(nx * ex[k] + ny * ey[k] + nz * ez[k] for k in range(3)))
            ru.append(j / segs)
            rv.append(i / rings)
        P.append(rp), N.append(rn), U.append(ru), V.append(rv)
    grid(mesh, shader, P, N, U, V)


def octahedron(mesh, shader, c, r, axis, tall=1.3):
    """Cut gem: eight flat facets, pointing along axis[2]."""
    ex, ey, ez = axis
    pts = [tuple(c[k] + r * (math.cos(a) * ex[k] + math.sin(a) * ey[k]) for k in range(3))
           for a in (0, math.pi / 2, math.pi, 1.5 * math.pi)]
    top = tuple(c[k] + r * tall * ez[k] for k in range(3))
    bot = tuple(c[k] - r * 0.5 * ez[k] for k in range(3))
    for apex in (top, bot):
        for i in range(4):
            a, b = pts[i], pts[(i + 1) % 4]
            n = np.cross(np.subtract(b, a), np.subtract(apex, a))
            mid = np.mean([a, b, apex], axis=0)
            if np.dot(n, mid - np.array(c)) < 0:
                n = -n
            mesh.add(shader, [a, b, apex], [n, n, n], [(0, 1), (1, 1), (0.5, 0)], [(0, 1, 2)])


def box_sweep(mesh, shader, path, side, w, t, v_scale=1 / 24):
    """Rectangular bar along path (list of points); side: unit vector that
    stays sideways (w across it, t along the bend normal)."""
    P = [np.array(p, float) for p in path]
    S = np.array(side, float)
    corners_rows = []
    acc = 0.0
    vs = []
    for i, p in enumerate(P):
        tan = P[min(i + 1, len(P) - 1)] - P[max(i - 1, 0)]
        tan /= np.linalg.norm(tan)
        n = np.cross(S, tan)
        n /= np.linalg.norm(n)
        s = np.cross(tan, n)
        if i:
            acc += np.linalg.norm(P[i] - P[i - 1])
        vs.append(acc * v_scale)
        corners_rows.append((p + s * w + n * t, p - s * w + n * t, p - s * w - n * t, p + s * w - n * t, s, n))
    faces = ((0, 1, lambda s, n: n), (1, 2, lambda s, n: -s), (2, 3, lambda s, n: -n), (3, 0, lambda s, n: s))
    for a, b, fn in faces:
        Pp, Nn, U, V = [], [], [], []
        for row, v in zip(corners_rows, vs):
            nrm = fn(row[4], row[5])
            Pp.append([row[a], row[b]])
            Nn.append([nrm, nrm])
            U.append([0, 0.25])
            V.append([v, v])
        grid(mesh, shader, Pp, Nn, U, V)


# ---------------------------------------------------------------- crown

def spike(mesh, k, apex, half_ang, steps=9, cols=7, thick=2.2):
    th0 = 2 * math.pi * k / 8
    z0 = BAND - 1.5
    outer, inner = [], []
    for i in range(steps + 1):
        t = i / steps
        z = z0 + (apex - z0) * t
        hw = half_ang * (1 - t) ** 0.62
        ro, ri = [], []
        for j in range(cols + 1):
            u = -1 + 2 * j / cols
            a = th0 + u * hw
            bulge = 1.3 * (1 - u * u) * (1 - 0.6 * t)
            r = cone_r(z)
            ro.append((a, z, r + bulge))
            ri.append((a, z, r - thick))
        outer.append(ro)
        inner.append(ri)

    def pt(a, z, r):
        return (r * math.cos(a), r * math.sin(a), z)
    for shell, sign in ((outer, 1), (inner, -1)):
        P = [[pt(*q) for q in row] for row in shell]
        N = [[(sign * math.cos(q[0]), sign * math.sin(q[0]), sign * 0.25) for q in row] for row in shell]
        U = [[j / cols for j in range(cols + 1)] for _ in shell]
        V = [[i / steps for _ in range(cols + 1)] for i in range(steps + 1)]
        grid(mesh, GOLD, P, N, U, V)
    # side walls along both edges up to the tip
    for edge, sgn in ((0, -1), (cols, 1)):
        P, N, U, V = [], [], [], []
        for i in range(steps + 1):
            o, n_ = outer[i][edge], inner[i][edge]
            a = o[0]
            tang = (-math.sin(a) * sgn, math.cos(a) * sgn, 0.35)
            P.append([pt(*o), pt(*n_)])
            N.append([tang, tang])
            U.append([0, 0.1])
            V.append([i / steps, i / steps])
        grid(mesh, GOLD, P, N, U, V)
    # gem on the tip
    a = th0
    r = cone_r(apex)
    ex = (-math.sin(a), math.cos(a), 0)
    ez = (math.cos(a), math.sin(a), 0)
    ey = tuple(np.cross(ez, ex))
    size = 3.8 if apex > 40 else 2.8
    sphere(mesh, GEM, (r * math.cos(a), r * math.sin(a), apex + size * 0.6), size, 8, 6)


def crown():
    mesh = Mesh()
    # band: bottom bead, wall, top bead, top edge, inner wall, bottom edge
    pts = []
    for i in range(7):                                   # bottom bead, half circle outwards
        ang = -math.pi / 2 + math.pi * i / 6
        pts.append((cone_r(2.6) + 1.2 + 2.6 * math.cos(ang), 2.6 + 2.6 * math.sin(ang)))
    for z in (6.5, 10.5, 14.5):
        pts.append((cone_r(z) + 0.2, z))
    for i in range(7):                                   # top bead
        ang = -math.pi / 2 + math.pi * i / 6
        pts.append((cone_r(BAND - 2) + 1.0 + 2.0 * math.cos(ang), BAND - 2 + 2.0 * math.sin(ang)))
    pts.append((cone_r(BAND) - 3.0, BAND))               # top edge, then inner wall
    pts.append((cone_r(BAND) - 3.0, BAND))
    pts.append((cone_r(0) - 3.0, 0.0))
    pts.append((cone_r(0) - 3.0, 0.0))
    pts.append((cone_r(2.6) + 1.2, 0.0))
    lathe_pts(mesh, GOLD, pts, 40, u_rep=8)
    # spikes: 4 tall (cardinal) with big tip gems, 4 short between
    for k in range(8):
        if k % 2 == 0:
            spike(mesh, k, 48, math.pi / 8 * 0.95)
        else:
            spike(mesh, k, 35, math.pi / 8 * 0.75)
    # gems on the band: ovals below the tall spikes, cut gems below the short ones
    for k in range(8):
        a = 2 * math.pi * k / 8
        r = cone_r(9.5) + 0.6
        c = (r * math.cos(a), r * math.sin(a), 9.5)
        ez = (math.cos(a), math.sin(a), 0.0)
        ex = (-math.sin(a), math.cos(a), 0.0)
        ey = (0.0, 0.0, 1.0)
        if k % 2 == 0:
            sphere(mesh, GEM, c, 4.6, 10, 7, squash=(1.0, 0.75, 0.55), axis=(ex, ey, ez))
        else:
            octahedron(mesh, GEM, c, 3.4, (ex, ey, ez), tall=0.9)
    # two arches crossing over the cap, springing from between the spikes
    top_z = 56.0
    for k in (1, 3, 5, 7):
        a = 2 * math.pi * k / 8 + 0.0
        path = []
        for i in range(13):
            t = i / 12
            rho = (cone_r(BAND) - 1.5) * math.cos(t * math.pi / 2)
            z = BAND - 1 + (top_z - BAND + 1) * math.sin(t * math.pi / 2) ** 0.8
            path.append((rho * math.cos(a), rho * math.sin(a), z))
        side = (-math.sin(a), math.cos(a), 0.0)
        box_sweep(mesh, GOLD, path, side, 2.6, 1.6)
        for i in (3, 6, 9):                               # pearls on the arch
            p = np.array(path[i])
            d = np.array(path[i + 1]) - np.array(path[i - 1])
            n = np.cross(side, d / np.linalg.norm(d))
            sphere(mesh, GEM, tuple(p + n * 2.4), 1.7, 6, 4)
    # orb and cross on top
    sphere(mesh, GOLD, (0, 0, top_z + 5.0), 6.0, 14, 9)
    sphere(mesh, GEM, (0, 0, top_z + 5.0), 6.6, 14, 9, squash=(1, 1, 0.12))   # gem belt around the orb
    for lo, hi in (((-1.6, -1.6, top_z + 10), (1.6, 1.6, top_z + 26)), ((-6.5, -1.6, top_z + 18.5), (6.5, 1.6, top_z + 21.7))):
        box(mesh, GOLD, lo, hi)
    # velvet cap with puffs between the arches
    prof = []
    ri = cone_r(BAND) - 3.2
    for i in range(10):
        ph = math.pi / 2 * i / 9
        prof.append((max(0.01, ri * math.cos(ph)), 8 + 44 * math.sin(ph)))
    prof = [(r, z) for r, z in prof]
    lathe_pts(mesh, VELVET, prof[::-1][::-1], 32, u_rep=4,
              radial=lambda a: 1.0 - 0.05 * math.cos(4 * (a - math.pi / 4)))
    return mesh


def box(mesh, shader, lo, hi):
    x0, y0, z0 = lo
    x1, y1, z1 = hi
    faces = [((0, 0, 1), [(x0, y0, z1), (x1, y0, z1), (x1, y1, z1), (x0, y1, z1)]),
             ((0, 0, -1), [(x0, y0, z0), (x1, y0, z0), (x1, y1, z0), (x0, y1, z0)]),
             ((1, 0, 0), [(x1, y0, z0), (x1, y1, z0), (x1, y1, z1), (x1, y0, z1)]),
             ((-1, 0, 0), [(x0, y0, z0), (x0, y1, z0), (x0, y1, z1), (x0, y0, z1)]),
             ((0, 1, 0), [(x0, y1, z0), (x1, y1, z0), (x1, y1, z1), (x0, y1, z1)]),
             ((0, -1, 0), [(x0, y0, z0), (x1, y0, z0), (x1, y0, z1), (x0, y0, z1)])]
    for n, q in faces:
        mesh.add(shader, q, [n] * 4, [(0, 0), (1, 0), (1, 1), (0, 1)], [(0, 1, 2), (0, 2, 3)])


# ---------------------------------------------------------------- bollard

def post():
    mesh = Mesh()
    body = [(0.01, 0.0), (9.0, 0.0), (9.0, 1.6), (7.8, 3.2), (4.6, 3.6), (4.6, 20.0), (4.2, 20.6),
            (4.2, 22.0), (4.6, 22.6), (4.6, 34.0), (6.2, 34.6), (6.2, 36.2), (5.4, 36.4)]
    lathe_pts(mesh, METAL, body, 16, u_rep=2)
    cap = [(5.4, 47.6), (6.4, 47.8), (6.6, 49.6), (5.0, 52.0), (2.0, 53.2), (0.01, 53.4)]
    lathe_pts(mesh, METAL, cap, 16, u_rep=2)
    lamp = [(5.4, 36.4), (5.6, 39.0), (5.6, 45.0), (5.4, 47.6)]
    lathe_pts(mesh, LAMP, lamp, 16, u_rep=1)
    return mesh


# ---------------------------------------------------------------- md3 out

def encode_normal(n):
    x, y, z = n
    lat = int(round(math.atan2(y, x) * 256 / (2 * math.pi))) & 255
    lng = int(round(math.acos(max(-1.0, min(1.0, z))) * 256 / (2 * math.pi))) & 255
    return (lat << 8) | lng


def name(s, n):
    return s.encode("ascii")[:n - 1].ljust(n, b"\0")


def write_md3(path, model_name, mesh):
    surfs = mesh.surfaces()
    blobs = []
    allv = []
    for idx, (shader, (V, N, S, T)) in enumerate(surfs):
        nv, nt = len(V), len(T)
        assert nv < 1000 and nt * 3 < 6000, (shader, nv, nt)
        ofs_shaders = 108
        ofs_tris = ofs_shaders + 68
        ofs_st = ofs_tris + 12 * nt
        ofs_xyz = ofs_st + 8 * nv
        end = ofs_xyz + 8 * nv
        sname = shader.split("/")[-1] + (f"_{idx}")
        b = struct.pack("<4s64s10i", b"IDP3", name(sname, 64), 0, 1, 1, nv, nt, ofs_tris, ofs_shaders,
                        ofs_st, ofs_xyz, end)
        b += name(shader, 64) + struct.pack("<i", 0)
        b += b"".join(struct.pack("<3i", *t) for t in T)
        b += b"".join(struct.pack("<2f", float(s[0]), float(s[1])) for s in S)
        for v, n in zip(V, N):
            b += struct.pack("<3hH", *(int(round(c * 64)) for c in v), encode_normal(n))
        assert len(b) == end
        blobs.append(b)
        allv += [tuple(v) for v in V]
    A = np.array(allv)
    mins, maxs = A.min(axis=0), A.max(axis=0)
    radius = float(np.max(np.linalg.norm(A, axis=1)))
    frame = struct.pack("<3f3f3ff16s", *mins, *maxs, 0, 0, 0, radius, name("frame0", 16))
    ofs_frames = 108
    ofs_surfs = ofs_frames + len(frame)
    body = b"".join(blobs)
    header = struct.pack("<4si64s9i", b"IDP3", 15, name(model_name, 64), 0, 1, 0, len(blobs), 0,
                         ofs_frames, ofs_surfs, ofs_surfs, ofs_surfs + len(body))
    with open(path, "wb") as f:
        f.write(header + frame + body)
    tris = sum(len(s[1][3]) for s in surfs)
    return len(blobs), len(A), tris, mins, maxs


# ---------------------------------------------------------------- textures

def write_tga(path, img):
    h, w = img.shape[:2]
    grey = img.ndim == 2
    alpha = (not grey) and img.shape[2] == 4
    header = struct.pack("<BBBHHBHHHHBB", 0, 0, 3 if grey else 2, 0, 0, 0, 0, 0, w, h,
                         8 if grey else (32 if alpha else 24), 8 if alpha else 0)
    if grey:
        data = img[::-1].tobytes()
    elif alpha:
        data = img[::-1][:, :, [2, 1, 0, 3]].tobytes()
    else:
        data = img[::-1, :, ::-1].tobytes()
    with open(path, "wb") as f:
        f.write(header + data)


def noise(size, cells, seed):
    rng = np.random.default_rng(seed)
    acc = np.zeros((size, size))
    for c, w in ((cells, 1.0), (cells * 2, 0.5), (cells * 4, 0.25), (cells * 8, 0.125)):
        g = rng.random((c, c))
        g = np.vstack([g, g[:1]])
        g = np.hstack([g, g[:, :1]])
        x = np.linspace(0, c, size, endpoint=False)
        i0 = x.astype(int)
        f = x - i0
        f = f * f * (3 - 2 * f)
        rows = g[i0][:, i0] * (1 - f)[None, :] + g[i0][:, i0 + 1] * f[None, :]
        rows2 = g[i0 + 1][:, i0] * (1 - f)[None, :] + g[i0 + 1][:, i0 + 1] * f[None, :]
        acc += w * (rows * (1 - f)[:, None] + rows2 * f[:, None])
    acc -= acc.min()
    return acc / acc.max()


def textures(out):
    mdir = os.path.join(out, "models", "mapobjects", "koth")
    os.makedirs(mdir, exist_ok=True)
    os.makedirs(os.path.join(out, "gfx", "misc"), exist_ok=True)
    s = 256
    n = noise(s, 4, 1)
    rng = np.random.default_rng(2)
    streak = np.repeat(rng.normal(0, 1, (s, 1)), s, axis=1)
    streak = (streak - streak.min()) / (streak.max() - streak.min())
    yy = np.arange(s)[:, None] / s
    band = 0.5 + 0.5 * np.cos(yy * 2 * math.pi * 3)
    lum = 0.78 + 0.16 * n + 0.06 * streak - 0.05 * band
    gold = np.stack([lum * 236, lum * 178, lum * 70], axis=-1)
    write_tga(os.path.join(mdir, "crown_gold.tga"), np.clip(gold, 0, 255).astype(np.uint8))

    e = 128
    c = (np.arange(e) + 0.5) / e * 2 - 1
    X, Y = np.meshgrid(c, c)
    env = 0.08 + 0.9 * np.exp(-((X + 0.35) ** 2 + (Y + 0.45) ** 2) / 0.05)
    env += 0.5 * np.exp(-((X - 0.45) ** 2 + (Y - 0.2) ** 2) / 0.12)
    env += 0.35 * np.exp(-(Y - 0.15) ** 2 / 0.01)
    env = np.clip(env, 0, 1)
    write_tga(os.path.join(mdir, "crown_env.tga"), (np.stack([env] * 3, -1) * 255).astype(np.uint8))

    ang = np.arctan2(Y, X)
    rad = np.sqrt(X * X + Y * Y)
    facet = 0.62 + 0.38 * (np.floor((ang + math.pi) / (2 * math.pi) * 10) % 2)
    facet = facet * (1 - 0.25 * np.clip(rad, 0, 1)) + 0.25 * (np.floor(rad * 4) % 2) * 0.4
    gem = np.clip(facet, 0, 1)
    write_tga(os.path.join(mdir, "crown_gem.tga"), (np.stack([gem] * 3, -1) * 255).astype(np.uint8))

    vn = noise(e, 4, 3)
    fold = 0.5 + 0.5 * np.sin(np.arange(e)[None, :] / e * 2 * math.pi * 4 + vn * 3)
    vel = 0.72 + 0.18 * fold + 0.10 * vn
    write_tga(os.path.join(mdir, "crown_velvet.tga"), (np.stack([vel] * 3, -1) * 255).astype(np.uint8))

    pm = noise(64, 4, 4)
    metal = 0.20 + 0.08 * pm
    pimg = np.stack([metal * 255, metal * 260, metal * 270], -1)
    pimg[:, :, :] = np.clip(pimg, 0, 255)
    pimg = np.repeat(pimg[:, :, None, :], 1, axis=2).reshape(64, 64, 3)
    write_tga(os.path.join(mdir, "post_metal.tga"), np.clip(pimg, 0, 255).astype(np.uint8))

    lc = np.linspace(0, 1, 64)[:, None] * np.ones((1, 64))
    lamp = 0.75 + 0.25 * np.sin(lc * math.pi)
    write_tga(os.path.join(mdir, "post_lamp.tga"), (np.stack([lamp] * 3, -1) * 255).astype(np.uint8))

    g = 64
    c = (np.arange(g) + 0.5) / g * 2 - 1
    X, Y = np.meshgrid(c, c)
    r = np.sqrt(X * X + Y * Y)
    glow = np.exp(-(r / 0.38) ** 2) * np.clip(1 - r, 0, 1)
    write_tga(os.path.join(out, "gfx", "misc", "koth_glow.tga"), (glow * 255).astype(np.uint8))


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", default=os.path.normpath(os.path.join(here, "..", "..", "baseq3r")))
    args = ap.parse_args()
    mdir = os.path.join(args.out, "models", "mapobjects", "koth")
    os.makedirs(mdir, exist_ok=True)
    for fname, mesh in (("crown.md3", crown()), ("post.md3", post())):
        ns, nv, nt, mins, maxs = write_md3(os.path.join(mdir, fname), "models/mapobjects/koth/" + fname, mesh)
        print(f"{fname}: {ns} surfaces, {nv} verts, {nt} tris, bounds {np.round(mins, 1)} .. {np.round(maxs, 1)}")
    textures(args.out)
    print("textures written")


if __name__ == "__main__":
    main()
