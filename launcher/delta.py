"""Reconstruct a verified game version without modifying its base installation."""
from pathlib import Path, PurePosixPath
import hashlib
import json
import os
import re
import shutil
import unicodedata
import zipfile

MAX_SIZE = 20 * 1024**3
MAX_METADATA = 16 * 1024**2


def _copy(source, destination, size, cancel):
    while size:
        if cancel.is_set():
            raise InterruptedError('Update paused.')
        block = source.read(min(size, 256 * 1024))
        if not block:
            raise ValueError('Truncated update data.')
        destination.write(block)
        size -= len(block)


def apply_update(archive_path, stage, base, version, target, package, update, cancel):
    # Import lazily: core invokes this module only for supported game updates.
    from .core import digest_file, safe_path, within
    stage, base = Path(stage), Path(base)
    with zipfile.ZipFile(archive_path) as archive:
        names = set()
        for item in archive.infolist():
            if item.filename in names or item.file_size > MAX_SIZE:
                raise ValueError('Invalid update archive entries.')
            names.add(item.filename)
            if item.filename not in ('update.json', 'release-manifest.json') and not re.fullmatch(r'data/\d+', item.filename):
                raise ValueError('Unexpected update archive entry.')
        if len(names) > 100000 or sum(i.file_size for i in archive.infolist()) > MAX_SIZE:
            raise ValueError('Update archive exceeds limits.')
        def metadata(name):
            if archive.getinfo(name).file_size > MAX_METADATA:
                raise ValueError('Update metadata exceeds limits.')
            return archive.read(name)
        raw_manifest = metadata('release-manifest.json')
        if hashlib.sha256(raw_manifest).hexdigest() != package['manifest_sha256']:
            raise ValueError('Wrong target manifest.')
        manifest = json.loads(raw_manifest)
        plan = json.loads(metadata('update.json'))
        identity = ('Q3Rally', version, target)
        if tuple(manifest.get(k) for k in ('product','version','target')) != identity:
            raise ValueError('Wrong target identity.')
        if plan.get('schema') != 1 or tuple(plan.get(k) for k in ('product','version','target')) != identity:
            raise ValueError('Wrong update identity.')
        if plan.get('from_version') != update['from_version'] or plan.get('from_manifest_sha256') != update['from_manifest_sha256']:
            raise ValueError('Wrong update base.')
        if digest_file(base/'release-manifest.json') != update['from_manifest_sha256']:
            raise ValueError('Base manifest changed.')
        old_manifest = json.loads((base/'release-manifest.json').read_text())
        old = {r['path']:r for r in old_manifest['files']}
        records, folded = {}, set()
        total = 0
        for record in manifest['files']:
            name = record['path']; safe_path(name)
            key = unicodedata.normalize('NFC',name).casefold()
            if key in folded or name.casefold() in ('release-manifest.json','.launcher-install.json'):
                raise ValueError('Duplicate or reserved target path.')
            size = record['size']
            if type(size) is not int or not 0 <= size <= MAX_SIZE:
                raise ValueError('Invalid target size.')
            folded.add(key); records[name] = record; total += size
        if total > MAX_SIZE or set(plan['files']) != set(records):
            raise ValueError('Update does not describe the complete target.')
        links = []
        operations = 0
        with (stage/'release-manifest.json').open('xb') as stream:
            stream.write(raw_manifest)
        for name, record in records.items():
            if cancel.is_set():
                raise InterruptedError('Update paused.')
            recipe = plan['files'][name]
            if not isinstance(recipe,dict) or type(recipe.get('executable',False)) is not bool:
                raise ValueError('Invalid update recipe.')
            methods = [k for k in ('copy','data','chunks') if k in recipe]
            if len(methods) != 1:
                raise ValueError('Ambiguous update recipe.')
            path = within(stage,name)
            path.parent.mkdir(parents=True,exist_ok=True)
            old_record = old.get(name)
            if methods[0] == 'copy':
                if recipe['copy'] is not True or old_record is None or any(
                        old_record.get(k) != record.get(k) for k in ('sha256','size')) or bool(old_record.get('symlink')) != bool(record.get('symlink')):
                    raise ValueError('Copy does not match the base manifest.')
                source = within(base,name)
                if record.get('symlink'):
                    data = os.readlink(source).encode('utf-8')
                else:
                    with source.open('rb') as src, path.open('xb') as dst:
                        _copy(src,dst,record['size'],cancel)
            elif methods[0] == 'data':
                payload = recipe['data']
                if not isinstance(payload,str) or not re.fullmatch(r'data/\d+',payload) or archive.getinfo(payload).file_size != record['size']:
                    raise ValueError('Invalid file payload.')
                if record.get('symlink'):
                    if record['size'] > 4096:
                        raise ValueError('Link target too long.')
                    data = archive.read(payload)
                else:
                    with archive.open(payload) as src, path.open('xb') as dst:
                        _copy(src,dst,record['size'],cancel)
            else:
                chunks = recipe['chunks']
                if record.get('symlink') or old_record is None or old_record.get('symlink') or not isinstance(chunks,list):
                    raise ValueError('Invalid range-copy source.')
                operations += len(chunks)
                if operations > 200000:
                    raise ValueError('Too many update operations.')
                remaining = record['size']
                with within(base,name).open('rb') as src, path.open('xb') as dst:
                    for chunk in chunks:
                        size = chunk['size']
                        if type(size) is not int or not 0 < size <= remaining:
                            raise ValueError('Invalid update range length.')
                        if set(chunk) == {'offset','size'}:
                            offset = chunk['offset']
                            if type(offset) is not int or offset < 0 or offset+size > old_record['size']:
                                raise ValueError('Update range leaves base file.')
                            src.seek(offset); _copy(src,dst,size,cancel)
                        elif set(chunk) == {'data','size'}:
                            payload = chunk['data']
                            if not isinstance(payload,str) or not re.fullmatch(r'data/\d+',payload) or archive.getinfo(payload).file_size != size:
                                raise ValueError('Invalid update range payload.')
                            with archive.open(payload) as block:
                                _copy(block,dst,size,cancel)
                        else:
                            raise ValueError('Invalid update range operation.')
                        remaining -= size
                if remaining:
                    raise ValueError('Incomplete reconstructed file.')
            if record.get('symlink'):
                if os.name == 'nt':
                    raise ValueError('Windows updates must not contain links.')
                destination = data.decode('utf-8')
                if '\\' in destination or PurePosixPath(destination).is_absolute() or not (path.parent/destination).resolve().is_relative_to(stage.resolve()):
                    raise ValueError('Unsafe link target.')
                links.append((path,destination))
            else:
                path.chmod(0o755 if recipe.get('executable') else 0o644)
        # Do not allow a newly created link to redirect subsequent file writes.
        for path,destination in links:
            path.symlink_to(destination)
