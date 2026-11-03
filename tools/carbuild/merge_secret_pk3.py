"""Add the output of secret_paints.py to zz_secret_skins.pk3.

usage: merge_secret_pk3.py <zz_secret_skins.pk3> <secret_paints outdir>

Files of the outdir replace those with the same name in the pk3; the shaders
from scripts/cars_secret_newcars.shader go to the end of
scripts/cars_secret.shader, after a marker line, replacing an earlier merge.
"""
import os
import sys
import zipfile

PK3, SRC = sys.argv[1], sys.argv[2]
SHADER = 'scripts/cars_secret.shader'
MARK = '// ---- new cars (tools/carbuild/secret_paints.py) ----\n'

new = {}
for root, _, files in os.walk(SRC):
    for f in files:
        path = os.path.join(root, f)
        new[os.path.relpath(path, SRC).replace(os.sep, '/')] = path
extra = open(new.pop('scripts/cars_secret_newcars.shader')).read()

tmp = PK3 + '.tmp'
with zipfile.ZipFile(PK3) as zin, zipfile.ZipFile(tmp, 'w', zipfile.ZIP_DEFLATED) as zout:
    names = set()
    for info in zin.infolist():
        if info.filename in new:
            continue
        data = zin.read(info.filename)
        if info.filename == SHADER:
            text = data.decode('latin-1')
            text = text.split(MARK)[0].rstrip('\n') + '\n\n' + MARK + '\n' + extra
            data = text.encode('latin-1')
        zout.writestr(info, data)
        names.add(info.filename)
    dirs = set()
    for name in sorted(new):
        parts = name.split('/')
        for i in range(1, len(parts)):
            d = '/'.join(parts[:i]) + '/'
            if d not in names and d not in dirs:
                zout.writestr(d, b'')
                dirs.add(d)
        zout.write(new[name], name)
os.replace(tmp, PK3)
print('%s: %d files added or replaced' % (PK3, len(new)))
