"""Per-surface vertex/triangle counts of MD3 models against the engine limit.

usage: python3 md3info.py <model.md3> [...]

The renderer rejects a whole model if one surface has SHADER_MAX_VERTEXES or
more vertices, or SHADER_MAX_INDEXES / 3 or more triangles
(engine/code/qcommon/qfiles.h). Older engines had SHADER_MAX_VERTEXES 1000.
Exit code 1 if any model is over the limit.
"""
import sys

import md3

SHADER_MAX_VERTEXES = 10000
SHADER_MAX_INDEXES = 6 * SHADER_MAX_VERTEXES
OLD_MAX_VERTEXES = 1000
OLD_MAX_INDEXES = 6 * OLD_MAX_VERTEXES


def report(path):
    m = md3.load(path)
    ok = True
    old_ok = True
    tv = tt = 0
    print(path)
    for s in m.surfs:
        nv, nt = len(s.st), len(s.tris)
        tv += nv
        tt += nt
        note = ''
        if nv >= SHADER_MAX_VERTEXES or nt * 3 >= SHADER_MAX_INDEXES:
            ok = False
            note = '  OVER LIMIT'
        elif nv >= OLD_MAX_VERTEXES or nt * 3 >= OLD_MAX_INDEXES:
            old_ok = False
            note = '  (needs new engine)'
        print('  %-24s %6d verts %6d tris%s' % (s.name, nv, nt, note))
    print('  %-24s %6d verts %6d tris, %d surfaces, %d frames'
          % ('total', tv, tt, len(m.surfs), len(m.frames)))
    print('  limit per surface: %d verts / %d tris -> %s'
          % (SHADER_MAX_VERTEXES - 1, SHADER_MAX_INDEXES // 3 - 1,
             'OK' if ok and old_ok else 'OK (not on engines before the limit raise)' if ok else 'TOO BIG'))
    return ok


if __name__ == '__main__':
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    results = [report(p) for p in sys.argv[1:]]
    sys.exit(0 if all(results) else 1)
