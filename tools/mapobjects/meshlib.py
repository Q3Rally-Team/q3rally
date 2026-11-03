"""Small mesh toolkit for the generated map models (md3): mesh builder with
automatic surface splitting, lathe, tubes, frusta, spheres, boxes, md3 and
TGA writers, tiling noise. Used by make_lanterns.py and make_rail.py.

Triangles are turned to face along the vertex normals (Q3: clockwise seen
from outside). Needs numpy.
"""
import math
import struct

import numpy as np

MAX_VERTS, MAX_INDEXES = 900, 5400       # renderer limits are 1000 / 6000 per surface


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
        Q = np.round(V * 64)                          # md3 stores 1/64 units
        for a, b, c in tris:
            n = np.cross(V[b] - V[a], V[c] - V[a])
            if np.linalg.norm(np.cross(Q[b] - Q[a], Q[c] - Q[a])) < 0.5:
                continue                              # degenerate once stored (lathe/sphere poles): drop
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


def box_uv(mesh, shader, lo, hi, scale=1 / 64, skip=()):
    """Axial box with world-scaled texture coordinates (scale per unit).
    skip: faces left out ("top", "bot", "+x", "-x", "+y", "-y")."""
    x0, y0, z0 = lo
    x1, y1, z1 = hi
    sc = scale

    def st(p, n):
        if n[2]:
            return (p[0] * sc, -p[1] * sc)
        if n[0]:
            return (p[1] * sc * n[0], -p[2] * sc)
        return (-p[0] * sc * n[1], -p[2] * sc)
    faces = [((0, 0, 1), [(x0, y0, z1), (x1, y0, z1), (x1, y1, z1), (x0, y1, z1)]),
             ((0, 0, -1), [(x0, y0, z0), (x1, y0, z0), (x1, y1, z0), (x0, y1, z0)]),
             ((1, 0, 0), [(x1, y0, z0), (x1, y1, z0), (x1, y1, z1), (x1, y0, z1)]),
             ((-1, 0, 0), [(x0, y0, z0), (x0, y1, z0), (x0, y1, z1), (x0, y0, z1)]),
             ((0, 1, 0), [(x0, y1, z0), (x1, y1, z0), (x1, y1, z1), (x0, y1, z1)]),
             ((0, -1, 0), [(x0, y0, z0), (x1, y0, z0), (x1, y0, z1), (x0, y0, z1)])]
    keys = ("top", "bot", "+x", "-x", "+y", "-y")
    for key, (n, q) in zip(keys, faces):
        if key in skip:
            continue
        mesh.add(shader, q, [n] * 4, [st(p, n) for p in q], [(0, 1, 2), (0, 2, 3)])


def rot_x(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def rot_y(a):
    """Positive a turns +x down (towards -z)."""
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rot_z(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def place(dst, build, R=None, t=(0, 0, 0)):
    """Build a part with build(mesh) in local coordinates, then rotate it by R
    and move it by t into dst."""
    m = Mesh()
    build(m)
    R = np.eye(3) if R is None else np.asarray(R, float)
    t = np.asarray(t, float)
    for shader, pieces in m.parts.items():
        for V, N, S, T in pieces:
            dst.add(shader, [tuple(R @ v + t) for v in V], [tuple(R @ n) for n in N], list(S), T)


def merge(dst, src, t=(0, 0, 0)):
    """Copy all parts of src into dst, moved by t."""
    t = np.asarray(t, float)
    for shader, pieces in src.parts.items():
        for V, N, S, T in pieces:
            dst.add(shader, [tuple(v + t) for v in V], list(N), list(S), T)


def retexture(src, mapping):
    """Copy of src with shaders renamed by mapping (others kept)."""
    out = Mesh()
    for shader, pieces in src.parts.items():
        out.parts.setdefault(mapping.get(shader, shader), []).extend(pieces)
    return out


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
        assert np.abs(np.array(V)).max() < 511.9, (shader, "md3 coordinates must stay within +-512")
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


# ---------------------------------------------------------------- extra shapes

def frustum(mesh, shader, n, z0, a0, z1, a1, phase=None, cap0=False, cap1=False, uv=1 / 16):
    """Flat-sided n-gon frustum between z0 (apothem a0) and z1 (apothem a1).
    Each side is its own quad (sharp edges); caps optional."""
    phase = math.pi / n if phase is None else phase
    r0, r1 = a0 / math.cos(math.pi / n), a1 / math.cos(math.pi / n)
    up = 1 if z1 > z0 else -1
    for k in range(n):
        t0, t1 = phase + 2 * math.pi * k / n, phase + 2 * math.pi * (k + 1) / n
        tm = (t0 + t1) / 2
        p = [(r0 * math.cos(t0), r0 * math.sin(t0), z0), (r0 * math.cos(t1), r0 * math.sin(t1), z0),
             (r1 * math.cos(t1), r1 * math.sin(t1), z1), (r1 * math.cos(t0), r1 * math.sin(t0), z1)]
        slope = (a0 - a1) / (abs(z1 - z0) or 1) * up
        nrm = (math.cos(tm), math.sin(tm), slope)
        w0, w1 = 2 * a0 * math.tan(math.pi / n), 2 * a1 * math.tan(math.pi / n)
        h = math.hypot(z1 - z0, a1 - a0)
        st = [(0, 0), (w0 * uv, 0), (w1 * uv, h * uv), (0, h * uv)]
        mesh.add(shader, p, [nrm] * 4, st, [(0, 1, 2), (0, 2, 3)])
    for z, r, cap, sgn in ((z0, r0, cap0, -up), (z1, r1, cap1, up)):
        if not cap:
            continue
        pts = [(r * math.cos(phase + 2 * math.pi * k / n), r * math.sin(phase + 2 * math.pi * k / n), z)
               for k in range(n)]
        verts = [(0, 0, z)] + pts
        tris = [(0, 1 + k, 1 + (k + 1) % n) for k in range(n)]
        mesh.add(shader, verts, [(0, 0, sgn)] * (n + 1), [(v[0] * uv, v[1] * uv) for v in verts], tris)


def tube(mesh, shader, path, r, sides=8, closed=False, v_scale=1 / 16, caps=True):
    """Round tube along a polyline (parallel-transported frame)."""
    P = [np.array(p, float) for p in path]
    T = []
    for i in range(len(P)):
        if closed:
            d = P[(i + 1) % len(P)] - P[i - 1]
        else:
            d = P[min(i + 1, len(P) - 1)] - P[max(i - 1, 0)]
        T.append(d / np.linalg.norm(d))
    ref = np.array([0.0, 0.0, 1.0]) if abs(T[0][2]) < 0.9 else np.array([1.0, 0.0, 0.0])
    Nn = np.cross(T[0], ref)
    Nn /= np.linalg.norm(Nn)
    frames = []
    for i in range(len(P)):
        if i:
            Nn = Nn - T[i] * np.dot(Nn, T[i])
            Nn /= np.linalg.norm(Nn)
        B = np.cross(T[i], Nn)
        frames.append((Nn.copy(), B))
    pts = list(range(len(P))) + ([0] if closed else [])
    rows_p, rows_n, rows_u, rows_v = [], [], [], []
    acc = 0.0
    for idx, i in enumerate(pts):
        if idx:
            acc += np.linalg.norm(P[i] - P[pts[idx - 1]])
        Nn, B = frames[i]
        rp, rn, ru, rv = [], [], [], []
        for s in range(sides + 1):
            a = 2 * math.pi * s / sides
            d = Nn * math.cos(a) + B * math.sin(a)
            rp.append(tuple(P[i] + d * r))
            rn.append(tuple(d))
            ru.append(s / sides)
            rv.append(acc * v_scale)
        rows_p.append(rp), rows_n.append(rn), rows_u.append(ru), rows_v.append(rv)
    grid(mesh, shader, rows_p, rows_n, rows_u, rows_v)
    if caps and not closed:
        for i, sgn in ((0, -1), (len(P) - 1, 1)):
            Nn, B = frames[i]
            ring = [tuple(P[i] + (Nn * math.cos(2 * math.pi * s / sides) + B * math.sin(2 * math.pi * s / sides)) * r)
                    for s in range(sides)]
            verts = [tuple(P[i])] + ring
            tris = [(0, 1 + k, 1 + (k + 1) % sides) for k in range(sides)]
            mesh.add(shader, verts, [tuple(T[i] * sgn)] * (sides + 1), [(0, 0)] * (sides + 1), tris)


def arc(c, r, a0, a1, steps, plane="xz"):
    out = []
    for k in range(steps + 1):
        a = a0 + (a1 - a0) * k / steps
        if plane == "xz":
            out.append((c[0] + r * math.cos(a), c[1], c[2] + r * math.sin(a)))
        else:
            out.append((c[0] + r * math.cos(a), c[1] + r * math.sin(a), c[2]))
    return out


