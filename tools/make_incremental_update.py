"""Build optional, byte-exact game updates from two audited release indexes."""
import argparse
import hashlib
import io
import json
from pathlib import Path
import shutil
import stat
import sys
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))
from launcher.core import TARGET, digest_file, safe_path, version_key


def _manifest(archive):
    name, = [n for n in archive.namelist() if n.endswith('/release-manifest.json')]
    raw = archive.read(name)
    manifest = json.loads(raw)
    version_key(manifest['version'])
    if not TARGET.fullmatch(manifest['target']): raise ValueError('Invalid package target')
    prefix = f"Q3Rally-v{manifest['version']}-{manifest['target']}/"
    if manifest['product'] != 'Q3Rally' or name != prefix+'release-manifest.json':
        raise ValueError('Unexpected package identity')
    if any(not n.startswith(prefix) for n in archive.namelist()):
        raise ValueError('Unexpected package contents')
    files = {}
    for record in manifest['files']:
        safe_path(record['path'])
        if record['path'] in files:
            raise ValueError('Duplicate manifest path')
        files[record['path']] = record
    return raw,manifest,files,prefix


def _payload(archive, source, size, counter):
    name = f'data/{counter[0]:06d}'; counter[0] += 1
    info = zipfile.ZipInfo(name,(2000,1,1,0,0,0))
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = (stat.S_IFREG | 0o644) << 16
    with archive.open(info,'w') as stream:
        while size:
            block = source.read(min(size,1024*1024))
            if not block: raise ValueError('Truncated input package')
            stream.write(block); size -= len(block)
    return name


def _range_hash(stream, offset, size):
    stream.seek(offset); digest = hashlib.sha256()
    while size:
        block=stream.read(min(size,1024*1024))
        if not block: raise ValueError('Truncated ZIP range')
        digest.update(block); size-=len(block)
    return digest.digest()


def _zip_chunks(old_path,new_path,output,counter):
    # Copy complete unchanged compressed ZIP members, not decompressed assets.
    # Transfer the new central directory verbatim so the final PK3 is byte-exact.
    with zipfile.ZipFile(old_path) as old, zipfile.ZipFile(new_path) as new:
        def spans(archive):
            infos=sorted(archive.infolist(),key=lambda i:i.header_offset)
            if len({i.filename for i in infos}) != len(infos):
                raise ValueError('Duplicate ZIP members')
            return [(i.filename,i.header_offset,(infos[n+1].header_offset if n+1<len(infos) else archive.start_dir)-i.header_offset)
                    for n,i in enumerate(infos)]
        old_spans={name:(offset,size) for name,offset,size in spans(old)}
        new_spans=spans(new)
        if not new_spans or new_spans[0][1] != 0:
            raise ValueError('Unsupported ZIP prefix')
        chunks=[]
        with old_path.open('rb') as src, new_path.open('rb') as dst:
            for name,offset,size in new_spans:
                previous=old_spans.get(name)
                if previous and previous[1]==size and _range_hash(src,*previous)==_range_hash(dst,offset,size):
                    old_offset=previous[0]
                    if chunks and set(chunks[-1])=={'offset','size'} and chunks[-1]['offset']+chunks[-1]['size']==old_offset:
                        chunks[-1]['size']+=size
                    else: chunks.append({'offset':old_offset,'size':size})
                else:
                    dst.seek(offset)
                    chunks.append({'data':_payload(output,dst,size,counter),'size':size})
            size=new_path.stat().st_size-new.start_dir
            dst.seek(new.start_dir)
            chunks.append({'data':_payload(output,dst,size,counter),'size':size})
        return chunks


def build_update(base_path, target_path, destination):
    base_path,target_path,destination=map(Path,(base_path,target_path,destination))
    if destination.exists(): raise ValueError('Refusing to overwrite an update')
    with zipfile.ZipFile(base_path) as base, zipfile.ZipFile(target_path) as target:
        old_raw,old,old_files,old_prefix=_manifest(base)
        raw,new,files,prefix=_manifest(target)
        if old['target']!=new['target'] or version_key(old['version'])[:2]!=version_key(new['version'])[:2] or version_key(old['version'])>=version_key(new['version']):
            raise ValueError('Updates require an older version in the same release line and target')
        plan={'schema':1,'product':'Q3Rally','from_version':old['version'],'version':new['version'],
              'target':new['target'],'from_manifest_sha256':hashlib.sha256(old_raw).hexdigest(),'files':{}}
        destination.parent.mkdir(parents=True,exist_ok=True)
        partial=destination.with_suffix(destination.suffix+'.partial')
        try:
            with zipfile.ZipFile(partial,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as output, tempfile.TemporaryDirectory() as temporary:
                temporary=Path(temporary); counter=[0]
                for name,record in files.items():
                    old_record=old_files.get(name)
                    mode=target.getinfo(prefix+name).external_attr >> 16
                    recipe={'executable':bool(mode & 0o111)}
                    if old_record and all(old_record[k]==record[k] for k in ('sha256','size')) and bool(old_record.get('symlink'))==bool(record.get('symlink')):
                        recipe['copy']=True
                    elif name.lower().endswith('.pk3') and old_record and not old_record.get('symlink') and not record.get('symlink'):
                        paths=[]
                        for archive,archive_prefix,expected,label in [(base,old_prefix,old_record,'old'),(target,prefix,record,'new')]:
                            path=temporary/label
                            with archive.open(archive_prefix+name) as source, path.open('wb') as stream:
                                shutil.copyfileobj(source,stream,1024*1024)
                            if path.stat().st_size!=expected['size'] or digest_file(path)!=expected['sha256']:
                                raise ValueError('Inner archive does not match manifest')
                            paths.append(path)
                        try: recipe['chunks']=_zip_chunks(*paths,output,counter)
                        except (zipfile.BadZipFile,ValueError):
                            with target.open(prefix+name) as source:
                                recipe['data']=_payload(output,source,record['size'],counter)
                    else:
                        with target.open(prefix+name) as source:
                            recipe['data']=_payload(output,source,record['size'],counter)
                    plan['files'][name]=recipe
                output.writestr('release-manifest.json',raw)
                output.writestr('update.json',json.dumps(plan,sort_keys=True,separators=(',',':')))
            partial.rename(destination)
        finally:
            if partial.exists(): partial.unlink()
    return {'version':new['version'],'target':new['target'],'from_version':old['version'],
            'from_manifest_sha256':plan['from_manifest_sha256'],'filename':destination.name,
            'size':destination.stat().st_size,'sha256':digest_file(destination)}


def build_indexes(base_index, release_index, output):
    base_index,release_index,output=map(Path,(base_index,release_index,output))
    old=json.loads(base_index.read_text()); new=json.loads(release_index.read_text())
    version_key(old['version']); version_key(new['version'])
    if old.get('product')!='Q3Rally' or new.get('product')!='Q3Rally':
        raise ValueError('Only game releases support incremental updates')
    output.mkdir(parents=True,exist_ok=True)
    previous={a['target']:a for a in old['artifacts']}; updates=[]
    for artifact in new['artifacts']:
        target=artifact['target']
        if not TARGET.fullmatch(target): raise ValueError('Invalid platform target')
        if target not in previous: continue
        before=previous[target]
        for index,entry in [(base_index,before),(release_index,artifact)]:
            if Path(entry['filename']).name!=entry['filename'] or '/' in entry['filename'] or '\\' in entry['filename']:
                raise ValueError('Invalid release filename')
            path=index.parent/entry['filename']
            if path.stat().st_size!=entry['size'] or digest_file(path)!=entry['sha256']:
                raise ValueError('Release archive checksum mismatch')
        filename=f"Q3Rally-v{old['version']}-to-v{new['version']}-{target}.update.zip"
        record=build_update(base_index.parent/before['filename'],release_index.parent/artifact['filename'],output/filename)
        if record['size'] < artifact['size']:
            updates.append(record)
            print(target, f"update {record['size']/1048576:.2f} MiB / full {artifact['size']/1048576:.2f} MiB",flush=True)
        else:
            (output/filename).unlink()
            print(target,'full package is smaller; no update emitted',flush=True)
    result={'schema':1,'product':'Q3Rally','version':new['version'],'updates':updates}
    (output/'updates.json').write_text(json.dumps(result,indent=2)+'\n')
    sums=output/'SHA256SUMS'
    previous={line.split('  ',1)[1]:line.split('  ',1)[0] for line in sums.read_text().splitlines() if '  ' in line} if sums.exists() else {}
    previous.update({r['filename']:r['sha256'] for r in updates})
    sums.write_text(''.join(digest+'  '+name+'\n' for name,digest in sorted(previous.items())))
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base-index',type=Path,required=True)
    parser.add_argument('--release-index',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    build_indexes(args.base_index,args.release_index,args.output)


if __name__=='__main__': main()
