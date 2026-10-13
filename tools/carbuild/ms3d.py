"""Minimal MilkShape 3D (.ms3d, v3/v4) reader: vertices, triangles, groups, materials."""
import struct

class Tri: pass
class Group: pass
class Material: pass

def load(path):
    d = open(path, 'rb').read(); o = 0
    ident, ver = struct.unpack_from('<10si', d, o); o += 14
    assert ident == b'MS3D000000', ident
    m = type('MS3D', (), {})(); m.version = ver
    nv, = struct.unpack_from('<H', d, o); o += 2
    m.verts = []
    for _ in range(nv):
        fl, x, y, z, bone, ref = struct.unpack_from('<B3fbB', d, o); o += 15
        m.verts.append((x, y, z))
    nt, = struct.unpack_from('<H', d, o); o += 2
    m.tris = []
    for _ in range(nt):
        t = Tri()
        f = struct.unpack_from('<H3H9f3f3fBB', d, o); o += 70
        t.v = f[1:4]; t.n = [f[4:7], f[7:10], f[10:13]]; t.s = f[13:16]; t.t = f[16:19]
        t.smooth, t.group = f[19], f[20]
        m.tris.append(t)
    ng, = struct.unpack_from('<H', d, o); o += 2
    m.groups = []
    for _ in range(ng):
        g = Group()
        fl, name, n = struct.unpack_from('<B32sH', d, o); o += 35
        g.name = name.split(b'\0')[0].decode('latin-1')
        g.tris = list(struct.unpack_from(f'<{n}H', d, o)); o += 2*n
        g.material, = struct.unpack_from('<b', d, o); o += 1
        m.groups.append(g)
    nm, = struct.unpack_from('<H', d, o); o += 2
    m.materials = []
    for _ in range(nm):
        mt = Material()
        f = struct.unpack_from('<32s4f4f4f4fffb128s128s', d, o); o += 361
        mt.name = f[0].split(b'\0')[0].decode('latin-1')
        mt.diffuse = f[5:9]; mt.texture = f[20].split(b'\0')[0].decode('latin-1'); mt.alpha = f[21].split(b'\0')[0].decode('latin-1')
        mt.transparency = f[18]
        m.materials.append(mt)
    return m
