"""Fast OBJ reader into numpy arrays, cached next to the output.

load(path, cache) -> dict with
  V (n,3) positions, VT (m,2) uvs, VN (k,3) normals,
  T (t,3) position indices, TT (t,3) uv indices (-1 = none), TN (t,3) normal indices,
  G (t,) group index, M (t,) material index, groups, mats (names)
Groups come from 'g' and 'o' lines; polygons are fan-triangulated.
"""
import hashlib
import os

import numpy as np


def load(path, cache_dir):
    st = os.stat(path)
    key = hashlib.sha1(('%s:%d:%d' % (os.path.basename(path), st.st_size, int(st.st_mtime))).encode()).hexdigest()[:12]
    cache = os.path.join(cache_dir, 'obj_%s.npz' % key)
    if os.path.exists(cache):
        d = np.load(cache, allow_pickle=True)
        return {k: d[k] for k in d.files}
    V, VT, VN = [], [], []
    T, TT, TN, G, M = [], [], [], [], []
    groups, mats = {}, {}
    g = groups.setdefault(None, 0)
    m = mats.setdefault(None, 0)
    with open(path, encoding='latin-1') as fh:
        for line in fh:
            c = line[:2]
            if c == 'v ':
                V.append(line[2:].split()[:3])
            elif c == 'vt':
                VT.append(line[3:].split()[:2])
            elif c == 'vn':
                VN.append(line[3:].split()[:3])
            elif c == 'f ':
                cs = [(x.split('/') + ['', ''])[:3] for x in line[2:].split()]
                for i in range(1, len(cs) - 1):
                    tri = (cs[0], cs[i], cs[i + 1])
                    T.append([int(a[0]) for a in tri])
                    TT.append([int(a[1]) if a[1] else 0 for a in tri])
                    TN.append([int(a[2]) if a[2] else 0 for a in tri])
                    G.append(g)
                    M.append(m)
            elif c in ('g ', 'o '):
                name = line[2:].strip()
                g = groups.setdefault(name, len(groups))
            elif line.startswith('usemtl'):
                name = line[7:].strip()
                m = mats.setdefault(name, len(mats))
    d = dict(V=np.array(V, np.float64), VT=np.array(VT, np.float32), VN=np.array(VN, np.float32),
             T=np.array(T, np.int64) - 1, TT=np.array(TT, np.int64) - 1, TN=np.array(TN, np.int64) - 1,
             G=np.array(G, np.int32), M=np.array(M, np.int32),
             groups=np.array(sorted(groups, key=groups.get), dtype=object),
             mats=np.array(sorted(mats, key=mats.get), dtype=object))
    np.savez(cache, **d)
    return d
