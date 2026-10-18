#!/usr/bin/env python3
"""Copy the light entities of a .map into its compiled .bsp.

q3map2 removes "light" entities from the BSP after the light stage, so the
game never sees where the map's lamps are. The OpenGL2 renderer needs them
for lamp shadows (r_lampShadows): models get real-time shadows from the
lamps near them. New maps can simply be compiled with "q3map2 -bsp
-keeplights"; this tool does the same for maps that are already compiled,
without touching geometry or lighting.

Spotlights get their target resolved here and stored as "_q3r_spotorigin",
because q3map2 also strips the info_null targets.

Only the entity lump changes. The BSP checksum the bots check changes with
it, so the matching .aas header is patched (the navigation data itself does
not depend on light entities).

usage: inject_lights.py maps/q3r_nightcity.map [--bsp X.bsp] [--aas X.aas] [--dry-run]
       (defaults: the .bsp/.aas next to the .map)
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import q3bsp  # noqa: E402

KEEP_KEYS = ('classname', 'origin', 'light', '_light', '_color', 'color', 'target',
             'targetname', 'radius', 'spawnflags', '_sun')


def parse_map_entities(text):
    """Key/value pairs of every entity in a .map (brushes are skipped)."""
    ents = []
    depth = 0
    cur = None
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c == '/' and text.startswith('//', i):
            j = text.find('\n', i)
            i = n if j < 0 else j
            continue
        if c == '"':
            j = text.index('"', i + 1)
            tok = text[i + 1:j]
            i = j + 1
            if depth == 1 and cur is not None:
                cur.append(tok)
            continue
        if c == '{':
            depth += 1
            if depth == 1:
                cur = []
        elif c == '}':
            if depth == 1 and cur is not None:
                ents.append(list(zip(cur[0::2], cur[1::2])))
                cur = None
            depth -= 1
        i += 1
    return ents


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('map')
    ap.add_argument('--bsp')
    ap.add_argument('--aas')
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()

    base = os.path.splitext(a.map)[0]
    bsp = a.bsp or base + '.bsp'
    aas = a.aas or base + '.aas'

    mapents = parse_map_entities(open(a.map, 'rb').read().decode('latin-1'))
    targets = {}
    for e in mapents:
        d = dict(e)
        if 'targetname' in d and 'origin' in d:
            targets[d['targetname']] = d['origin']

    lights = []
    for e in mapents:
        d = dict(e)
        if d.get('classname', '').lower() != 'light' or 'origin' not in d:
            continue
        out = [(k, v) for k, v in e if k.lower() in KEEP_KEYS]
        if 'target' in d and d['target'] in targets:
            out.append(('_q3r_spotorigin', targets[d['target']]))
        lights.append(out)

    data = open(bsp, 'rb').read()
    ents = [e for e in q3bsp.parse_entities(q3bsp.get_entities(data))
            if dict(e).get('classname', '').lower() != 'light']
    spots = sum(1 for l in lights if any(k == '_q3r_spotorigin' for k, _ in l))
    print('%s: %d lights (%d spots) from %s' % (os.path.basename(bsp), len(lights), spots, os.path.basename(a.map)))
    if a.dry_run:
        return

    new = q3bsp.set_entities(data, q3bsp.format_entities(ents + lights))
    open(bsp, 'wb').write(new)
    cs = q3bsp.bsp_checksum(new)
    if os.path.exists(aas):
        q3bsp.aas_set_checksum(aas, cs)
        print('%s: checksum %d' % (os.path.basename(aas), cs))


if __name__ == '__main__':
    main()
