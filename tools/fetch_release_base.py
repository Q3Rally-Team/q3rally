"""Fetch one published official base package for incremental-build validation."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import urllib.request


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--version',required=True)
    parser.add_argument('--target',required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    if not re.fullmatch(r'\d+\.\d+(?:\.\d+)?[a-z]?',args.version) or not re.fullmatch(r'(?:windows|linux|macos)-(?:x86|x86_64|arm64)',args.target):
        raise ValueError('Invalid base version or target')
    base=f'https://github.com/Q3Rally-Team/q3rally/releases/download/v{args.version}/'
    def request(name):
        return urllib.request.Request(base+name,headers={'User-Agent':'Q3RallyRelease/1.0'})
    with urllib.request.urlopen(request('release.json'),timeout=60) as response:
        index=json.load(response)
    if index.get('product')!='Q3Rally' or index['version']!=args.version:
        raise ValueError('Unexpected base release')
    artifact,=[a for a in index['artifacts'] if a['target']==args.target]
    name=artifact['filename']
    if Path(name).name!=name or '/' in name or '\\' in name:
        raise ValueError('Invalid package name')
    args.output.mkdir(parents=True,exist_ok=True)
    path=args.output/name
    if not path.exists():
        partial=path.with_suffix('.part')
        with urllib.request.urlopen(request(name),timeout=180) as response, partial.open('wb') as stream:
            shutil.copyfileobj(response,stream,1024*1024)
        partial.rename(path)
    with path.open('rb') as stream: digest=hashlib.file_digest(stream,'sha256').hexdigest()
    if path.stat().st_size!=artifact['size'] or digest!=artifact['sha256']:
        raise ValueError('Base release checksum mismatch')
    index['artifacts']=[artifact]
    (args.output/'release.json').write_text(json.dumps(index,indent=2)+'\n')
    print(args.target,'published base package SHA-256 PASS',flush=True)


if __name__=='__main__': main()
