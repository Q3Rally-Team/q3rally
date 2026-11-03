"""Minimal Wavefront OBJ reader (positions, uvs, normals, objects, materials).

load(path) -> (V, VT, VN, groups)
groups is a list of ((object, material), tris) in file order; every tri is
three (v, vt, vn) index triples (0-based, vt/vn may be None). Polygons are
fan-triangulated in the file's counter-clockwise order.
"""


def load(path):
    V, VT, VN = [], [], []
    groups, order = {}, []
    obj = mtl = None
    for line in open(path):
        p = line.split()
        if not p:
            continue
        t = p[0]
        if t == 'v':
            V.append(tuple(map(float, p[1:4])))
        elif t == 'vt':
            VT.append(tuple(map(float, p[1:3])))
        elif t == 'vn':
            VN.append(tuple(map(float, p[1:4])))
        elif t == 'o':
            obj = p[1]
        elif t == 'usemtl':
            mtl = p[1]
        elif t == 'f':
            key = (obj, mtl)
            if key not in groups:
                groups[key] = []
                order.append(key)
            corners = []
            for c in p[1:]:
                a = (c.split('/') + ['', ''])[:3]
                corners.append(tuple(int(x) - 1 if x else None for x in a))
            for i in range(1, len(corners) - 1):
                groups[key].append((corners[0], corners[i], corners[i + 1]))
    return V, VT, VN, [(k, groups[k]) for k in order]
