#!/usr/bin/env python3
"""Change a compiled Q3Rally map without compiling it again.

  python set_worldspawn.py maps/q3r_country01.bsp weather=rain weatherChance=50
  python set_worldspawn.py maps/q3r_mk.bsp --fix-sky
  python set_worldspawn.py maps/q3r_sk1.bsp --remove-weather-zones weather=rain
  python set_worldspawn.py maps/q3r_country01.bsp --show

- key=value sets a worldspawn key, key= (empty value) removes it
- --fix-sky marks the map's sky shaders as sky in the BSP (SURF_SKY,
  SURF_NOIMPACT, SURF_NOLIGHTMAP), for maps whose sky shader was not found
  or had no "surfaceparm sky" when they were compiled. Sky shaders are the
  shaders with skyparms in the scripts folder next to the maps folder (or
  --scripts). Weather, rain and snow particles need it.
- --remove-weather-zones drops the obsolete rally_weather_rain and
  rally_weather_snow entities (their brushes stay in the BSP, unused)
- --show prints the worldspawn and the sky shaders and changes nothing

The file is changed in place, a copy is kept as <map>.bsp.bak. Only the
entity lump and the flags in the shader lump change, so the bot navigation
stays valid; the BSP checksum stored in the .aas next to the map is updated
so botlib still loads it.
"""
import argparse
import glob
import os
import re
import shutil
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import q3bsp  # noqa: E402

LUMP_SHADERS = 1
SURF_SKY = 0x4
SURF_NOIMPACT = 0x10
SURF_NOLIGHTMAP = 0x400


def sky_shaders(scripts):
    """Names (lower case) of the shaders with skyparms in the shader scripts."""
    names = set()
    for path in glob.glob(os.path.join(scripts, '*.shader')):
        text = open(path, encoding='latin-1').read()
        text = re.sub(r'//[^\n]*', '', text)
        depth, name, last, body = 0, None, None, []
        for m in re.finditer(r'\{|\}|[^\s{}]+', text):
            tok = m.group(0)
            if tok == '{':
                if depth == 0:
                    name, body = last, []
                depth += 1
            elif tok == '}':
                depth -= 1
                if depth == 0 and name and 'skyparms' in ' '.join(body).lower():
                    names.add(name.lower())
            else:
                if depth > 0:
                    body.append(tok)
                last = tok
    return names


def shader_lump(data):
    o, l = q3bsp.read_lumps(data)[LUMP_SHADERS]
    for i in range(l // 72):
        p = o + i * 72
        name = data[p:p + 64].split(b'\0')[0].decode('latin-1')
        flags, contents = struct.unpack_from('<ii', data, p + 64)
        yield p, name, flags, contents


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('bsp')
    ap.add_argument('keys', nargs='*', help='key=value, key= removes the key')
    ap.add_argument('--fix-sky', action='store_true')
    ap.add_argument('--remove-weather-zones', action='store_true')
    ap.add_argument('--scripts', help='shader scripts folder (default: ../scripts next to the map)')
    ap.add_argument('--show', action='store_true')
    a = ap.parse_args()

    data = open(a.bsp, 'rb').read()
    ents = q3bsp.parse_entities(q3bsp.get_entities(data))
    if not ents or dict(ents[0]).get('classname', '').lower() != 'worldspawn':
        sys.exit('%s: the first entity is not the worldspawn' % a.bsp)

    scripts = a.scripts or os.path.join(os.path.dirname(os.path.abspath(a.bsp)), '..', 'scripts')
    skies = sky_shaders(scripts) if os.path.isdir(scripts) else set()

    if a.show:
        print('worldspawn of %s:' % os.path.basename(a.bsp))
        for k, v in ents[0]:
            print('  "%s" "%s"' % (k, v))
        for _, name, flags, _ in shader_lump(data):
            if name.lower() in skies or flags & SURF_SKY:
                print('sky shader %s: %s' % (name, 'flagged' if flags & SURF_SKY else 'NOT flagged (use --fix-sky)'))
        zones = [dict(e).get('classname') for e in ents if dict(e).get('classname', '').startswith('rally_weather_')]
        if zones:
            print('obsolete weather zones: %s' % ', '.join(zones))
        return

    changes = []
    world = ents[0]
    for kv in a.keys:
        if '=' not in kv:
            sys.exit('expected key=value, got %r' % kv)
        key, value = kv.split('=', 1)
        world[:] = [(k, v) for k, v in world if k.lower() != key.lower()]
        if value:
            world.append((key, value))
            changes.append('%s = %s' % (key, value))
        else:
            changes.append('%s removed' % key)

    if a.remove_weather_zones:
        before = len(ents)
        ents[:] = [e for e in ents if not dict(e).get('classname', '').startswith('rally_weather_')]
        changes.append('%d weather zones removed' % (before - len(ents)))

    new = q3bsp.set_entities(data, q3bsp.format_entities(ents))

    if a.fix_sky:
        if not skies:
            sys.exit('no sky shaders found in %s (use --scripts)' % scripts)
        buf = bytearray(new)
        for p, name, flags, contents in shader_lump(new):
            if name.lower() in skies and not flags & SURF_SKY:
                struct.pack_into('<i', buf, p + 64, flags | SURF_SKY | SURF_NOIMPACT | SURF_NOLIGHTMAP)
                changes.append('sky flag on %s' % name)
        new = bytes(buf)

    if new == data:
        print('%s: nothing to change' % os.path.basename(a.bsp))
        return

    shutil.copyfile(a.bsp, a.bsp + '.bak')
    open(a.bsp, 'wb').write(new)
    print('%s: %s' % (os.path.basename(a.bsp), '; '.join(changes)))

    aas = os.path.splitext(a.bsp)[0] + '.aas'
    if os.path.exists(aas):
        q3bsp.aas_set_checksum(aas, q3bsp.bsp_checksum(new))
        print('%s: checksum updated' % os.path.basename(aas))


if __name__ == '__main__':
    main()
