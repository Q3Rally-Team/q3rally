"""Put a car's tyres on the ground (see carlib.wheel_tag_z).

usage: ground_fix.py [--dry-run] <car dir> [...]

Measures the tyre radius r in wheel.md3 and raises (or lowers) the whole
body.md3 - every vertex, every tag and the frame bounds - so the wheel tags
sit at r - 16.2 on average, then puts each wheel tag exactly there. The file
is patched in place: only those numbers change, every other byte stays as
it was (the old car models do not survive a load/save through md3.py
byte for byte).
"""
import math
import struct
import sys

import md3
from carlib import wheel_tag_z

WHEEL_TAGS = ('tag_wheelfl', 'tag_wheelfr', 'tag_wheelrl', 'tag_wheelrr')


def tyre_radius(path):
    w = md3.load(path)
    return max(math.hypot(v[1], v[2]) / 64 for s in w.surfs for f in s.frames for v in f)


def patch(path, dz, wheel_z):
    d = bytearray(open(path, 'rb').read())
    (_, _, _, _, nf, nt, ns, _, ofsf, ofst, ofss, _) = struct.unpack_from('<4si64siiiiiiiii', d, 0)
    q = int(round(dz * 64))
    dz = q / 64                                   # move by whole xyz steps so vertices stay exact
    for f in range(nf):                           # frame bounds: mins z, maxs z, radius
        o = ofsf + f * 56
        mnz, = struct.unpack_from('<f', d, o + 8)
        mxz, = struct.unpack_from('<f', d, o + 20)
        rad, = struct.unpack_from('<f', d, o + 36)
        struct.pack_into('<f', d, o + 8, mnz + dz)
        struct.pack_into('<f', d, o + 20, mxz + dz)
        struct.pack_into('<f', d, o + 36, rad + abs(dz))
    for i in range(nf * nt):                      # tags: origin z
        o = ofst + i * 112
        name = d[o:o + 64].split(b'\0')[0].decode()
        z, = struct.unpack_from('<f', d, o + 72)
        struct.pack_into('<f', d, o + 72, wheel_z if name in WHEEL_TAGS else z + dz)
    o = ofss
    for _ in range(ns):                           # vertices: z of every xyz
        (_, _, _, snf, _, nv, _, _, _, _, ofsxyz, ofsend) = struct.unpack_from('<4s64siiiiiiiiii', d, o)
        for k in range(snf * nv):
            p = o + ofsxyz + k * 8 + 4
            z, = struct.unpack_from('<h', d, p)
            if not -32768 <= z + q <= 32767:
                raise ValueError('vertex out of range in %s' % path)
            struct.pack_into('<h', d, p, z + q)
        o += ofsend
    return bytes(d), dz


def main():
    args = [a for a in sys.argv[1:] if a != '--dry-run']
    dry = '--dry-run' in sys.argv
    if not args:
        sys.exit(__doc__)
    for car in args:
        body = md3.load(car + '/body.md3')
        r = tyre_radius(car + '/wheel.md3')
        target = wheel_tag_z(r)
        zs = {t[0]: t[1][2] for t in body.tags[0] if t[0] in WHEEL_TAGS}
        dz = target - sum(zs.values()) / len(zs)
        if abs(dz) < 0.05 and all(abs(z - target) < 0.05 for z in zs.values()):
            print('%-60s r %.2f  ok, tags at %.2f' % (car, r, target))
            continue
        data, dz = patch(car + '/body.md3', dz, target)
        print('%-60s r %.2f  tags %s -> %.2f, body %+.2f' % (
            car, r, '/'.join('%.2f' % zs[t] for t in WHEEL_TAGS), target, dz))
        if not dry:
            open(car + '/body.md3', 'wb').write(data)


if __name__ == '__main__':
    main()
