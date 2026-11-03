"""Subdivide an MD3 model into a smoother, higher-poly version.

usage: python3 subdivide.py <in.md3> <out.md3> [passes] [--flat]

Each pass splits every triangle into four. New vertices sit on PN-triangle
curves built from the vertex normals, so smooth panels get rounder while the
silhouette stays the same. Edges whose normals differ between the faces that
share them (hard edges, seams between surfaces with different normals) are
split on the straight line, so they stay sharp and never crack open.
--flat splits every edge on the straight line (only adds triangles).

Tags are copied unchanged, frame bounds are recomputed. Prints the per-surface
counts against the engine limit (see md3info.py).
"""
import math
import sys

import md3
from md3info import report

XYZ_SCALE = 1.0 / 64
HARD_DOT = 0.999   # endpoint normals closer than this count as "the same"


def dot(a, b): return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
def sub(a, b): return (a[0] - b[0], a[1] - b[1], a[2] - b[2])
def norm(a):
    l = math.sqrt(dot(a, a)) or 1.0
    return (a[0] / l, a[1] / l, a[2] / l)


def decode(v):
    return (v[0] * XYZ_SCALE, v[1] * XYZ_SCALE, v[2] * XYZ_SCALE), md3.dec_normal(v[3])


def encode(p, n):
    q = [max(-32768, min(32767, int(round(c / XYZ_SCALE)))) for c in p]
    return q + [md3.enc_normal(*n)]


def pn_midpoint(p1, n1, p2, n2):
    """Point at t=0.5 on the cubic PN-triangle edge curve."""
    w12 = dot(sub(p2, p1), n1)
    w21 = dot(sub(p1, p2), n2)
    return tuple((p1[k] + p2[k]) * 0.5 - (w12 * n1[k] + w21 * n2[k]) / 8.0 for k in range(3))


def subdivide_pass(m, flat):
    nframes = len(m.frames)
    for f in range(nframes):
        # every occurrence of a position edge across the whole model, with the
        # normals at its two ends; an edge is smooth only if they all agree
        normals_at = {}
        for s in m.surfs:
            fr = s.frames[f]
            for tri in s.tris:
                for a, b in ((tri[0], tri[1]), (tri[1], tri[2]), (tri[2], tri[0])):
                    ka, kb = tuple(fr[a][:3]), tuple(fr[b][:3])
                    if ka == kb:
                        continue
                    if kb < ka:
                        ka, kb, a, b = kb, ka, b, a
                    normals_at.setdefault((ka, kb), []).append(
                        (md3.dec_normal(fr[a][3]), md3.dec_normal(fr[b][3])))
        smooth = set()
        if not flat:
            for key, occ in normals_at.items():
                na0, nb0 = occ[0]
                if all(dot(na, na0) > HARD_DOT and dot(nb, nb0) > HARD_DOT for na, nb in occ):
                    smooth.add(key)
        for s in m.surfs:
            _split_surface_frame(s, f, smooth)
    for s in m.surfs:
        _finish_surface(s)


def _split_surface_frame(s, f, smooth):
    """Adds the edge midpoints of frame f; the vertex layout is shared by all frames."""
    fr = s.frames[f]
    if f == 0:
        s._mid = {}           # (i, j) local vertex pair -> new vertex index
        s._newst = []
        s._newtris = []
        nv = len(s.st)
        for tri in s.tris:
            ids = []
            for a, b in ((tri[0], tri[1]), (tri[1], tri[2]), (tri[2], tri[0])):
                e = (min(a, b), max(a, b))
                if e not in s._mid:
                    s._mid[e] = nv + len(s._newst)
                    s._newst.append([(s.st[a][0] + s.st[b][0]) * 0.5, (s.st[a][1] + s.st[b][1]) * 0.5])
                ids.append(s._mid[e])
            m01, m12, m20 = ids
            v0, v1, v2 = tri
            s._newtris += [[v0, m01, m20], [m01, v1, m12], [m20, m12, v2], [m01, m12, m20]]
        s._newframes = []
    added = [None] * len(s._newst)
    for (a, b), idx in s._mid.items():
        ka, kb = tuple(fr[a][:3]), tuple(fr[b][:3])
        if kb < ka:
            ka, kb, a, b = kb, ka, b, a
        p1, n1 = decode(fr[a])
        p2, n2 = decode(fr[b])
        if (ka, kb) in smooth:
            p = pn_midpoint(p1, n1, p2, n2)
        else:
            p = tuple((p1[k] + p2[k]) * 0.5 for k in range(3))
        added[idx - len(s.st)] = encode(p, norm(tuple(n1[k] + n2[k] for k in range(3))))
    s._newframes.append(fr + added)


def _finish_surface(s):
    s.st = s.st + s._newst
    s.tris = s._newtris
    s.frames = s._newframes
    for k in ('_mid', '_newst', '_newtris', '_newframes'):
        s.__dict__.pop(k, None)


def recompute_bounds(m):
    for f, frame in enumerate(m.frames):
        pts = [decode(v)[0] for s in m.surfs for v in s.frames[f]]
        if not pts:
            continue
        mins = [min(p[k] for p in pts) for k in range(3)]
        maxs = [max(p[k] for p in pts) for k in range(3)]
        origin = frame[6:9]
        radius = max(math.sqrt(dot(sub(p, origin), sub(p, origin))) for p in pts)
        frame[0:6] = mins + maxs
        frame[9] = radius


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    flat = '--flat' in sys.argv
    if len(args) < 2:
        sys.exit(__doc__)
    passes = int(args[2]) if len(args) > 2 else 1
    m = md3.load(args[0])
    for _ in range(passes):
        subdivide_pass(m, flat)
    recompute_bounds(m)
    md3.save(m, args[1])
    sys.exit(0 if report(args[1]) else 1)


if __name__ == '__main__':
    main()
