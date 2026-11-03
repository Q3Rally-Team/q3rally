"""Minimal 3D Studio (.3ds) reader: meshes, uvs, materials and their texture maps.

load(path) -> (V, VT, VN, groups, materials)
like obj.load; VN is empty (3ds files carry smoothing groups, not normals),
groups is a list of ((object, material), tris) with every tri three
(v, vt, None) index triples (vt is the same index as v: 3ds stores one uv per
vertex). materials maps a material name to {'diffuse': (r, g, b), 'texture':
file name or None}. Positions are the stored world positions.
"""
import struct


def _chunks(d, o, end):
    while o + 6 <= end:
        cid, ln = struct.unpack_from('<HI', d, o)
        if ln < 6:
            break
        yield cid, o + 6, o + ln
        o += ln


def _cstr(d, o):
    e = d.index(b'\0', o)
    return d[o:e].decode('latin-1'), e + 1


def _colour(d, o, end):
    for cid, s, e in _chunks(d, o, end):
        if cid == 0x0011:                     # 24 bit colour
            return tuple(c / 255 for c in d[s:s + 3])
        if cid == 0x0010:                     # float colour
            return struct.unpack_from('<3f', d, s)
    return None


def load(path):
    d = open(path, 'rb').read()
    cid, ln = struct.unpack_from('<HI', d, 0)
    if cid != 0x4D4D:
        raise ValueError('not a 3ds file')
    V, VT, groups, order, materials = [], [], {}, [], {}

    def material(o, end):
        name, m = None, {'diffuse': None, 'texture': None}
        for c, s, e in _chunks(d, o, end):
            if c == 0xA000:
                name = _cstr(d, s)[0]
            elif c == 0xA020:
                m['diffuse'] = _colour(d, s, e)
            elif c == 0xA200:
                for c2, s2, e2 in _chunks(d, s, e):
                    if c2 == 0xA300:
                        m['texture'] = _cstr(d, s2)[0]
        if name is not None:
            materials[name] = m

    def trimesh(name, o, end):
        base = len(V)
        verts, uvs, faces, groups_ = [], [], [], []
        for c, s, e in _chunks(d, o, end):
            if c == 0x4110:
                n, = struct.unpack_from('<H', d, s)
                verts = [struct.unpack_from('<3f', d, s + 2 + 12 * i) for i in range(n)]
            elif c == 0x4140:
                n, = struct.unpack_from('<H', d, s)
                uvs = [struct.unpack_from('<2f', d, s + 2 + 8 * i) for i in range(n)]
            elif c == 0x4120:
                n, = struct.unpack_from('<H', d, s)
                faces = [struct.unpack_from('<4H', d, s + 2 + 8 * i)[:3] for i in range(n)]
                for c2, s2, e2 in _chunks(d, s + 2 + 8 * n, e):
                    if c2 == 0x4130:          # faces of one material
                        mname, p = _cstr(d, s2)
                        k, = struct.unpack_from('<H', d, p)
                        groups_.append((mname, struct.unpack_from('<%dH' % k, d, p + 2)))
        V.extend(verts)
        VT.extend(uvs if len(uvs) == len(verts) else [(0.0, 0.0)] * len(verts))
        assigned = {}
        for mname, ids in groups_:
            for f in ids:
                assigned[f] = mname
        for f, (a, b, c) in enumerate(faces):
            key = (name, assigned.get(f))
            if key not in groups:
                groups[key] = []
                order.append(key)
            groups[key].append(tuple((base + i, base + i, None) for i in (a, b, c)))

    for c, s, e in _chunks(d, 6, ln):
        if c != 0x3D3D:
            continue
        for c2, s2, e2 in _chunks(d, s, e):
            if c2 == 0xAFFF:
                material(s2, e2)
            elif c2 == 0x4000:
                name, p = _cstr(d, s2)
                for c3, s3, e3 in _chunks(d, p, e2):
                    if c3 == 0x4100:
                        trimesh(name, s3, e3)
    return V, VT, [], [(k, groups[k]) for k in order], materials
