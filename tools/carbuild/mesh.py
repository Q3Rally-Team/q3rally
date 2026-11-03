"""Mesh helpers: welding, quadric decimation and crease-angle normals.

Decimation needs pyfqmr (pip install pyfqmr).
"""
import numpy as np


def weld(P, T, eps=1e-5, keep_mask=False):
    """merge vertices closer than eps; returns (verts, faces) without degenerate faces,
    plus the mask of the triangles of T that were kept if keep_mask is set"""
    q = np.round(P[T.reshape(-1)] / eps).astype(np.int64)
    uq, inv = np.unique(q, axis=0, return_inverse=True)
    faces = inv.reshape(-1, 3)
    ok = (faces[:, 0] != faces[:, 1]) & (faces[:, 1] != faces[:, 2]) & (faces[:, 0] != faces[:, 2])
    return (uq * eps, faces[ok], ok) if keep_mask else (uq * eps, faces[ok])


def decimate(verts, faces, target, keep_border=True):
    """quadric edge collapse down to about target faces; open borders stay where they are,
    so parts that meet other parts along a border do not open up gaps (keep_border=False
    lets them move too, for parts whose holes left by culling are mostly border)"""
    if len(faces) <= target:
        return verts, faces
    import pyfqmr
    s = pyfqmr.Simplify()
    s.setMesh(np.ascontiguousarray(verts, np.float64), np.ascontiguousarray(faces, np.int32))
    s.simplify_mesh(target_count=int(target), aggressiveness=7, preserve_border=bool(keep_border), verbose=False)
    v, f, _ = s.getMesh()
    return np.asarray(v, float), np.asarray(f, int)


def face_normals(verts, faces):
    p = verts[faces]
    n = np.cross(p[:, 1] - p[:, 0], p[:, 2] - p[:, 0])
    area = np.linalg.norm(n, axis=1)
    return n / np.maximum(area, 1e-20)[:, None], area


def corner_normals(verts, faces, crease_deg=40.0):
    """per corner normals: area weighted average of the faces around the vertex whose
    normal is within crease_deg of this face's, so hard edges stay hard"""
    fn, area = face_normals(verts, faces)
    cos_c = np.cos(np.radians(crease_deg))
    nv = len(verts)
    # vertex -> incident faces (CSR)
    vf = faces.reshape(-1)
    order = np.argsort(vf, kind='stable')
    fid = order // 3
    start = np.searchsorted(vf[order], np.arange(nv + 1))
    out = np.zeros((len(faces), 3, 3))
    for f in range(len(faces)):
        for k in range(3):
            v = faces[f, k]
            inc = fid[start[v]:start[v + 1]]
            g = fn[inc]
            w = (g @ fn[f] > cos_c) * area[inc]
            n = (g * w[:, None]).sum(0)
            l = np.linalg.norm(n)
            out[f, k] = n / l if l > 0 else fn[f]
    return out
