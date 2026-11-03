"""Minimal binary FBX reader (7.x) for car meshes.

parse(path)  -> root node tree (Node: name, props, children)
load(path)   -> (V, VT, VN, groups) like obj.load: world-space positions and
                normals, uvs, and ((model, material), tris) groups. Every tri
                is three (v, vt, vn) index triples into V/VT/VN. Polygons are
                fan-triangulated in file order (counter-clockwise).
The FBX axis system is reported by axes(root); positions are returned as
stored (no axis conversion), scaled by the model transforms only.
"""
import math
import struct
import zlib


class Node:
    __slots__ = ('name', 'props', 'children')

    def __init__(self, name, props, children):
        self.name, self.props, self.children = name, props, children

    def find(self, name):
        for c in self.children:
            if c.name == name:
                return c
        return None

    def all(self, name):
        return [c for c in self.children if c.name == name]


_ARR = {ord('f'): ('f', 4), ord('d'): ('d', 8), ord('l'): ('q', 8), ord('i'): ('i', 4), ord('b'): ('b', 1)}


def _read_props(d, o, n):
    props = []
    for _ in range(n):
        t = d[o]
        o += 1
        if t == ord('Y'):
            props.append(struct.unpack_from('<h', d, o)[0]); o += 2
        elif t == ord('C'):
            props.append(bool(d[o])); o += 1
        elif t == ord('I'):
            props.append(struct.unpack_from('<i', d, o)[0]); o += 4
        elif t == ord('F'):
            props.append(struct.unpack_from('<f', d, o)[0]); o += 4
        elif t == ord('D'):
            props.append(struct.unpack_from('<d', d, o)[0]); o += 8
        elif t == ord('L'):
            props.append(struct.unpack_from('<q', d, o)[0]); o += 8
        elif t in _ARR:
            length, enc, clen = struct.unpack_from('<III', d, o); o += 12
            raw = d[o:o + clen]; o += clen
            if enc == 1:
                raw = zlib.decompress(raw)
            fmt, size = _ARR[t]
            props.append(list(struct.unpack('<%d%s' % (length, fmt), raw[:length * size])))
        elif t in (ord('S'), ord('R')):
            length = struct.unpack_from('<I', d, o)[0]; o += 4
            raw = d[o:o + length]; o += length
            props.append(raw.decode('utf-8', 'replace') if t == ord('S') else raw)
        else:
            raise ValueError('unknown FBX property type %r at %d' % (chr(t), o - 1))
    return props, o


def _read_node(d, o, wide):
    if wide:
        end, nprops, plen = struct.unpack_from('<QQQ', d, o); o += 24
    else:
        end, nprops, plen = struct.unpack_from('<III', d, o); o += 12
    nlen = d[o]; o += 1
    if end == 0:
        return None, o
    name = d[o:o + nlen].decode('ascii', 'replace'); o += nlen
    props, o = _read_props(d, o, nprops)
    children = []
    null = 25 if wide else 13
    while o < end - null:              # nested nodes, closed by a null record
        c, o = _read_node(d, o, wide)
        children.append(c)
    return Node(name, props, children), end


def parse(path):
    d = open(path, 'rb').read()
    if not d.startswith(b'Kaydara FBX Binary  \x00'):
        raise ValueError('not a binary FBX file')
    version = struct.unpack_from('<I', d, 23)[0]
    wide = version >= 7500
    o = 27
    top = []
    while o < len(d):
        n, o2 = _read_node(d, o, wide)
        if n is None:
            break
        top.append(n)
        o = o2
    return Node('', [version], top)


def props70(node):
    """Properties70 of a node as {name: values}"""
    out = {}
    p = node.find('Properties70')
    if p:
        for c in p.all('P'):
            out[c.props[0]] = c.props[4:]
    return out


def axes(root):
    gs = props70(root.find('GlobalSettings'))
    return {k: gs.get(k, [None])[0] for k in ('UpAxis', 'UpAxisSign', 'FrontAxis', 'FrontAxisSign',
                                             'CoordAxis', 'CoordAxisSign', 'UnitScaleFactor')}


# ---------------- transforms -------------------------------------------------------
def _mat_mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(4)) for j in range(4)] for i in range(4)]


def _translate(t):
    return [[1, 0, 0, t[0]], [0, 1, 0, t[1]], [0, 0, 1, t[2]], [0, 0, 0, 1]]


def _scale(s):
    return [[s[0], 0, 0, 0], [0, s[1], 0, 0], [0, 0, s[2], 0], [0, 0, 0, 1]]


def _rot(r):
    """Euler XYZ in degrees (FBX default order): R = Rz * Ry * Rx"""
    x, y, z = (math.radians(a) for a in r)
    cx, sx, cy, sy, cz, sz = math.cos(x), math.sin(x), math.cos(y), math.sin(y), math.cos(z), math.sin(z)
    rx = [[1, 0, 0, 0], [0, cx, -sx, 0], [0, sx, cx, 0], [0, 0, 0, 1]]
    ry = [[cy, 0, sy, 0], [0, 1, 0, 0], [-sy, 0, cy, 0], [0, 0, 0, 1]]
    rz = [[cz, -sz, 0, 0], [sz, cz, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]
    return _mat_mul(rz, _mat_mul(ry, rx))


def _inv_translate(t):
    return _translate([-t[0], -t[1], -t[2]])


def _local(p):
    z = [0.0, 0.0, 0.0]
    T = p.get('Lcl Translation', z)
    R = p.get('Lcl Rotation', z)
    S = p.get('Lcl Scaling', [1.0, 1.0, 1.0])
    Roff, Rp = p.get('RotationOffset', z), p.get('RotationPivot', z)
    Soff, Sp = p.get('ScalingOffset', z), p.get('ScalingPivot', z)
    pre, post = p.get('PreRotation', z), p.get('PostRotation', z)
    post_inv = [[_rot(post)[j][i] for j in range(4)] for i in range(4)]   # rotation: inverse = transpose
    m = _translate(T)
    for f in (_translate(Roff), _translate(Rp), _rot(pre), _rot(R), post_inv, _inv_translate(Rp),
              _translate(Soff), _translate(Sp), _scale(S), _inv_translate(Sp)):
        m = _mat_mul(m, f)
    return m


def _geometric(p):
    z = [0.0, 0.0, 0.0]
    m = _translate(p.get('GeometricTranslation', z))
    m = _mat_mul(m, _rot(p.get('GeometricRotation', z)))
    return _mat_mul(m, _scale(p.get('GeometricScaling', [1.0, 1.0, 1.0])))


def _apply(m, v, w=1.0):
    return tuple(m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2] + m[i][3] * w for i in range(3))


def _normal_matrix(m):
    a = [[m[i][j] for j in range(3)] for i in range(3)]
    det = (a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) - a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0])
           + a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]))
    inv = [[(a[(j + 1) % 3][(i + 1) % 3] * a[(j + 2) % 3][(i + 2) % 3]
             - a[(j + 1) % 3][(i + 2) % 3] * a[(j + 2) % 3][(i + 1) % 3]) / det for j in range(3)] for i in range(3)]
    return [[inv[j][i] for j in range(3)] + [0] for i in range(3)] + [[0, 0, 0, 1]], det


# ---------------- meshes ----------------------------------------------------------------
def _layer(geom, name, data_key, index_key, polys, nverts_per_poly):
    """per polygon-vertex values of a layer element (list aligned with PolygonVertexIndex)"""
    le = geom.find(name)
    if le is None:
        return None
    mapping = le.find('MappingInformationType').props[0]
    ref = le.find('ReferenceInformationType').props[0]
    data = le.find(data_key).props[0]
    idx = le.find(index_key).props[0] if le.find(index_key) else None
    return mapping, ref, data, idx


def load(path):
    root = parse(path)
    objects = root.find('Objects')
    by_id = {o.props[0]: o for o in objects.children}
    parent, geo_of, mats_of = {}, {}, {}
    for c in root.find('Connections').all('C'):
        kind, child, par = c.props[0], c.props[1], c.props[2]
        if kind != 'OO' or child not in by_id:
            continue
        cn = by_id[child]
        pn = by_id.get(par)
        if cn.name == 'Model' and (pn is None or pn.name == 'Model'):
            parent[child] = par if pn is not None else None
        elif cn.name == 'Geometry' and pn is not None and pn.name == 'Model':
            geo_of.setdefault(par, []).append(child)
        elif cn.name == 'Material' and pn is not None and pn.name == 'Model':
            mats_of.setdefault(par, []).append(child)

    world_cache = {}

    def world(mid):
        if mid not in world_cache:
            m = _local(props70(by_id[mid]))
            p = parent.get(mid)
            world_cache[mid] = _mat_mul(world(p), m) if p in by_id else m
        return world_cache[mid]

    def clean(n):
        return n.split('\x00\x01')[0]

    V, VT, VN, groups, order = [], [], [], {}, []
    for mid, gids in geo_of.items():
        model = by_id[mid]
        mname = clean(model.props[1])
        M = _mat_mul(world(mid), _geometric(props70(model)))
        NM, det = _normal_matrix(M)
        mats = [clean(by_id[x].props[1]) for x in mats_of.get(mid, [])]
        for gid in gids:
            g = by_id[gid]
            if g.find('Vertices') is None:
                continue
            pos = g.find('Vertices').props[0]
            pvi = g.find('PolygonVertexIndex').props[0]
            base_v = len(V)
            for i in range(0, len(pos), 3):
                V.append(_apply(M, pos[i:i + 3]))
            # split into polygons
            polys, cur = [], []
            for k, i in enumerate(pvi):
                if i < 0:
                    cur.append((k, ~i))
                    polys.append(cur)
                    cur = []
                else:
                    cur.append((k, i))
            nrm = _layer(g, 'LayerElementNormal', 'Normals', 'NormalsIndex', polys, None)
            uvl = _layer(g, 'LayerElementUV', 'UV', 'UVIndex', polys, None)
            mat = _layer(g, 'LayerElementMaterial', 'Materials', None, polys, None)

            def value_index(layer, k, vi, pi):
                mapping, ref, data, idx = layer
                if mapping == 'ByPolygonVertex':
                    j = k
                elif mapping in ('ByVertice', 'ByVertex'):
                    j = vi
                elif mapping == 'ByPolygon':
                    j = pi
                else:                                   # AllSame
                    j = 0
                return idx[j] if ref == 'IndexToDirect' and idx is not None else j

            base_n, base_t = len(VN), len(VT)
            if nrm:
                d = nrm[2]
                for i in range(0, len(d), 3):
                    n = _apply(NM, d[i:i + 3], 0.0)
                    l = math.sqrt(sum(c * c for c in n)) or 1.0
                    VN.append(tuple(c / l for c in n))
            if uvl:
                d = uvl[2]
                for i in range(0, len(d), 2):
                    VT.append((d[i], d[i + 1]))
            flip = det < 0
            for pi, poly in enumerate(polys):
                if mat and mat[2]:
                    mi = mat[2][0] if mat[0] == 'AllSame' else mat[2][pi]
                    mname_ = mats[mi] if mi < len(mats) else 'mat%d' % mi
                else:
                    mname_ = mats[0] if mats else None
                key = (mname, mname_)
                if key not in groups:
                    groups[key] = []
                    order.append(key)
                cs = []
                for k, vi in poly:
                    ni = base_n + value_index(nrm, k, vi, pi) if nrm else None
                    ti = base_t + value_index(uvl, k, vi, pi) if uvl else None
                    cs.append((base_v + vi, ti, ni))
                for i in range(1, len(cs) - 1):
                    t = (cs[0], cs[i], cs[i + 1])
                    groups[key].append((t[0], t[2], t[1]) if flip else t)
    return V, VT, VN, [(k, groups[k]) for k in order]
