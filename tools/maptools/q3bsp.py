"""Q3Rally map tools: small helpers for Quake 3 BSP and AAS files.

- read / replace the entity lump of an IBSP file (the lump layout is rebuilt,
  everything else is copied unchanged)
- the BSP checksum the engine computes (sv_mapChecksum, MD4 based)
- read / patch the BSP checksum stored in an AAS header

When only the entity lump of a map changes, the bot navigation (AAS) stays
valid, but botlib refuses the AAS file because its stored checksum no longer
matches the BSP. aas_set_checksum() fixes that without running bspc.
"""
import struct

NLUMPS = 17
LUMP_ENTITIES = 0


def _md4(msg):
    def F(x, y, z): return (x & y) | (~x & z)
    def G(x, y, z): return (x & y) | (x & z) | (y & z)
    def H(x, y, z): return x ^ y ^ z

    def rol(x, n):
        x &= 0xffffffff
        return ((x << n) | (x >> (32 - n))) & 0xffffffff

    ml = len(msg) * 8
    msg = msg + b'\x80' + b'\0' * ((55 - len(msg)) % 64) + struct.pack('<Q', ml)
    a, b, c, d = 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
    for off in range(0, len(msg), 64):
        X = struct.unpack('<16I', msg[off:off + 64])
        A, B, C, D = a, b, c, d
        for i in range(16):
            s = [3, 7, 11, 19][i % 4]
            if i % 4 == 0: a = rol(a + F(b, c, d) + X[i], s)
            elif i % 4 == 1: d = rol(d + F(a, b, c) + X[i], s)
            elif i % 4 == 2: c = rol(c + F(d, a, b) + X[i], s)
            else: b = rol(b + F(c, d, a) + X[i], s)
        for i, k in enumerate([0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15]):
            s = [3, 5, 9, 13][i % 4]
            if i % 4 == 0: a = rol(a + G(b, c, d) + X[k] + 0x5a827999, s)
            elif i % 4 == 1: d = rol(d + G(a, b, c) + X[k] + 0x5a827999, s)
            elif i % 4 == 2: c = rol(c + G(d, a, b) + X[k] + 0x5a827999, s)
            else: b = rol(b + G(c, d, a) + X[k] + 0x5a827999, s)
        for i, k in enumerate([0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15]):
            s = [3, 9, 11, 15][i % 4]
            if i % 4 == 0: a = rol(a + H(b, c, d) + X[k] + 0x6ed9eba1, s)
            elif i % 4 == 1: d = rol(d + H(a, b, c) + X[k] + 0x6ed9eba1, s)
            elif i % 4 == 2: c = rol(c + H(d, a, b) + X[k] + 0x6ed9eba1, s)
            else: b = rol(b + H(c, d, a) + X[k] + 0x6ed9eba1, s)
        a = (a + A) & 0xffffffff
        b = (b + B) & 0xffffffff
        c = (c + C) & 0xffffffff
        d = (d + D) & 0xffffffff
    return a, b, c, d


def bsp_checksum(data):
    """Same value as the engine's CM_LoadMap checksum (Com_BlockChecksum)."""
    a, b, c, d = _md4(data)
    return struct.unpack('<i', struct.pack('<I', a ^ b ^ c ^ d))[0]


def read_lumps(data):
    if data[:4] != b'IBSP':
        raise ValueError('not an IBSP file')
    return [struct.unpack_from('<ii', data, 8 + 8 * i) for i in range(NLUMPS)]


def get_entities(data):
    o, l = read_lumps(data)[LUMP_ENTITIES]
    return data[o:o + l].split(b'\0')[0].decode('latin-1')


def set_entities(data, text):
    """Return a new BSP with the entity lump replaced (lump order kept)."""
    lumps = read_lumps(data)
    blobs = [data[o:o + l] for o, l in lumps]
    blobs[LUMP_ENTITIES] = text.replace('\r\n', '\n').encode('latin-1').rstrip(b'\0') + b'\0'
    order = sorted(range(NLUMPS), key=lambda i: lumps[i][0])
    out = bytearray(data[:8 + 8 * NLUMPS])
    for i in order:
        while len(out) % 4:
            out.append(0)
        struct.pack_into('<ii', out, 8 + 8 * i, len(out), len(blobs[i]))
        out += blobs[i]
    return bytes(out)


def _aas_xor(b):
    # botlib AAS_DData: header data after ident/version is xor'ed (version 5)
    return bytes(x ^ ((i * 119) & 0xff) for i, x in enumerate(b))


def aas_checksum(path):
    raw = open(path, 'rb').read(12)
    ver = struct.unpack_from('<i', raw, 4)[0]
    cs = raw[8:12]
    if ver == 5:
        cs = _aas_xor(cs)
    return struct.unpack('<i', cs)[0]


def aas_set_checksum(path, checksum):
    data = bytearray(open(path, 'rb').read())
    ver = struct.unpack_from('<i', data, 4)[0]
    cs = struct.pack('<i', checksum)
    if ver == 5:
        cs = _aas_xor(cs)
    data[8:12] = cs
    open(path, 'wb').write(data)


def parse_entities(text):
    """Entity string -> list of ordered (key, value) lists."""
    import re
    ents = []
    for block in re.findall(r'\{([^{}]*)\}', text):
        ents.append(re.findall(r'"([^"]*)"\s+"([^"]*)"', block))
    return ents


def format_entities(ents):
    out = []
    for e in ents:
        out.append('{\n' + ''.join('"%s" "%s"\n' % kv for kv in e) + '}\n')
    return ''.join(out)
