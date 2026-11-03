"""Keep only the triangles that can be seen from outside the car.

visible(V, T, up=1, res=1024) -> bool array, one entry per triangle
visible_facing(...) -> (that array, the side each triangle is seen from)

The mesh is rendered orthographically from a ring of directions around the
car (elevations from straight above down to slightly below the sills, not
from underneath) into an ID buffer. Each triangle is sampled densely enough
that it cannot slip through between samples, so a triangle counts as visible
when it is the nearest surface at one or more pixels of one or more views.
Glass occludes like any other surface, which is what the game shows with
nearly opaque tinted windows.
"""
import math

import numpy as np

ELEVATIONS = (90, 60, 35, 15, 0, -15)
AZIMUTHS = 16


def _directions():
    out = []
    for el in ELEVATIONS:
        n = 1 if el == 90 else AZIMUTHS
        for i in range(n):
            az = 2 * math.pi * i / n
            e = math.radians(el)
            out.append((math.cos(e) * math.cos(az), math.sin(e), math.cos(e) * math.sin(az)))
    return out


def _basis(d, up):
    d = np.asarray(d, float)
    ref = np.zeros(3)
    ref[up] = 1.0
    if abs(d @ ref) > 0.99:
        ref = np.roll(ref, 1)
    u = np.cross(ref, d)
    u /= np.linalg.norm(u)
    v = np.cross(d, u)
    return u, v


def _samples(tri_px):
    """barycentric sample points for every triangle, dense enough for its pixel size"""
    edge = np.max(np.linalg.norm(tri_px - np.roll(tri_px, 1, axis=1), axis=2), axis=1)
    n = np.clip(np.ceil(edge * 1.5).astype(int), 1, 400)
    tri_ids, bary = [], []
    for k in np.unique(n):
        ids = np.nonzero(n == k)[0]
        i, j = np.meshgrid(np.arange(k + 1), np.arange(k + 1))
        keep = i + j <= k
        a = (i[keep] / k) if k else np.zeros(1)
        b = (j[keep] / k) if k else np.zeros(1)
        bc = np.stack([1 - a - b, a, b], 1)
        bc = (bc + np.array([1, 1, 1]) / 3 * 1e-3) / (1 + 1e-3)       # pull the corners inside a hair
        tri_ids.append(np.repeat(ids, len(bc)))
        bary.append(np.tile(bc.astype(np.float32), (len(ids), 1)))
    return np.concatenate(tri_ids), np.concatenate(bary)


def visible(V, T, up=1, res=1024, progress=None):
    return visible_facing(V, T, up, res, progress)[0]


def visible_facing(V, T, up=1, res=1024, progress=None, views=None):
    """like visible(), plus the side each triangle is seen from: the number of
    samples where it is the nearest surface with its front (counter-clockwise
    side) towards the camera minus those showing its back. A negative count on a
    closed outer surface means the triangle's winding is flipped in the source.
    views: indices of the directions to render (all by default), so the work can
    be split over processes and the results combined (seen: or, facing: sum)."""
    V = np.asarray(V, float)
    T = np.asarray(T, int)
    P = V[T]                                            # tris x 3 x 3
    centre = (V.min(0) + V.max(0)) / 2
    radius = np.linalg.norm(V.max(0) - V.min(0)) / 2
    scale = res / (2 * radius)
    seen = np.zeros(len(T), bool)
    facing = np.zeros(len(T), np.int64)
    dirs = _directions()
    rel = (P - centre).astype(np.float32)
    normal = np.cross(P[:, 1] - P[:, 0], P[:, 2] - P[:, 0])
    chunk = 150000                                      # triangles per block: bounds the sample memory
    for n, d in enumerate(dirs):
        if views is not None and n not in views:
            continue
        u, v = _basis(d, up)
        px = np.stack([rel @ u.astype(np.float32), rel @ v.astype(np.float32)], -1) * np.float32(scale) + res / 2
        depth = rel @ np.asarray(d, np.float32)         # larger = nearer to the camera
        zbuf = np.full(res * res, -np.inf, np.float32)
        blocks, total = [], 0
        for c0 in range(0, len(T), chunk):
            ids, bc = _samples(px[c0:c0 + chunk])
            bc = bc.astype(np.float32)
            ids = ids.astype(np.int32) + c0
            sx = np.einsum('nk,nkc->nc', bc, px[ids])
            sd = np.einsum('nk,nk->n', bc, depth[ids])
            del bc
            pix = (np.clip(sx[:, 1].astype(np.int32), 0, res - 1) * res
                   + np.clip(sx[:, 0].astype(np.int32), 0, res - 1))
            del sx
            np.maximum.at(zbuf, pix, sd)                # z-buffer: nearest depth per pixel
            blocks.append((ids, pix, sd))
            total += len(ids)
        for ids, pix, sd in blocks:
            win = ids[sd >= zbuf[pix]]
            seen[win] = True
            np.add.at(facing, win, np.where(normal[win] @ np.asarray(d) > 0, 1, -1))
        del blocks
        if progress:
            progress(n + 1, len(dirs), total, int(seen.sum()))
    return seen, facing
