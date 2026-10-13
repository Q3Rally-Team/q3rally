"""Geometry measurements on built bodies."""
import numpy as np

def side_depth(tris, res=0.25, xr=(-60, 60), zr=(-16, 20)):
    """Orthographic view of the left flank: outermost y per (z, x) cell.
    tris: iterable of 3x3 arrays in Q3 coordinates."""
    xs = np.arange(*xr, res); zs = np.arange(*zr, res)
    H = np.full((len(zs), len(xs)), -99.0)
    for p in tris:
        p = np.asarray(p)
        if p[:, 1].max() < 0: continue
        ix = np.where((xs >= p[:, 0].min()) & (xs <= p[:, 0].max()))[0]
        iz = np.where((zs >= p[:, 2].min()) & (zs <= p[:, 2].max()))[0]
        if not len(ix) or not len(iz): continue
        gx, gz = np.meshgrid(xs[ix], zs[iz])
        (ax, ay, az), (bx, by, bz), (cx, cy, cz) = p
        den = (bz-cz)*(ax-cx) + (cx-bx)*(az-cz)
        if abs(den) < 1e-9: continue
        a = ((bz-cz)*(gx-cx) + (cx-bx)*(gz-cz))/den; b = ((cz-az)*(gx-cx) + (ax-cx)*(gz-cz))/den; c = 1-a-b
        ins = (a >= 0) & (b >= 0) & (c >= 0)
        y = a*ay + b*by + c*cy
        sub = H[np.ix_(iz, ix)]; upd = ins & (y > sub); sub[upd] = y[upd]; H[np.ix_(iz, ix)] = sub
    return xs, zs, H

def arch(xs, zs, H, lo, hi, skin_y, ztop=8, zbot=-13, zmin_fit=-9):
    """Circle fitted to the wheel arch opening between x lo..hi: cells whose
    outermost surface lies more than 2 units inside the flank (skin_y), i.e.
    the wheel well behind the opening. zbot must lie inside the well."""
    open_ = (H < skin_y - 2) & (H > -50)
    ix = (xs > lo) & (xs < hi); iz = (zs < ztop) & (zs > zbot)
    sub = open_[np.ix_(iz, ix)]; X = xs[ix]; Z = zs[iz]
    pts = []
    valid = (H > -50)[np.ix_(iz, ix)]
    for j, x in enumerate(X):
        col = sub[:, j]
        k = int(np.argmax(valid[:, j]))          # lowest surface cell of this column
        if not valid[k, j] or not col[k]: continue
        while k + 1 < len(col) and col[k+1]: k += 1
        pts.append((x, Z[k]))
    pts = np.array(pts); top = pts[pts[:, 1] > zmin_fit]
    A = np.c_[2*top[:, 0], 2*top[:, 1], np.ones(len(top))]; bb = top[:, 0]**2 + top[:, 1]**2
    c = np.linalg.lstsq(A, bb, rcond=None)[0]; r = float(np.sqrt(c[2] + c[0]**2 + c[1]**2))
    return float(c[0]), float(c[1]), r, float(pts[:, 1].max())
