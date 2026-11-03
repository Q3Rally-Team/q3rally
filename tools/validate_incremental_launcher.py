"""Exercise an actual frozen launcher installing a tiny signed incremental game update."""
import argparse
import base64
from datetime import datetime, timedelta, timezone
import hashlib
import http.server
import io
import json
import os
from pathlib import Path
import random
import subprocess
import sys
import threading
import zipfile

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))
from nacl.signing import SigningKey
from launcher.core import atomic_json, native_targets
from tools.make_incremental_update import build_update
from tools.publish_launcher_catalog import sign


def validate(executable, output):
    output.mkdir(parents=True,exist_ok=False)
    target=native_targets()[0]; shared=random.Random(19).randbytes(128*1024)
    def sha(data): return hashlib.sha256(data).hexdigest()
    files={}; requests=[]; records=[]; manifests=[]
    def member(name):
        item=zipfile.ZipInfo(name,(2000,1,1,0,0,0)); item.compress_type=zipfile.ZIP_DEFLATED
        item.external_attr=0o100644<<16
        return item
    for version,value in [('0.8.1',b'1'),('0.8.2',b'0 1')]:
        inner=io.BytesIO()
        with zipfile.ZipFile(inner,'w') as z:
            z.writestr(member('textures/shared.bin'),shared); z.writestr(member('scripts/track.arena'),value)
        contents={'probe.bin':b'not executed','baseq3r/pak0.pk3':inner.getvalue()}
        manifest=json.dumps({'schema':1,'product':'Q3Rally','version':version,'target':target,
                            'files':[{'path':p,'size':len(d),'sha256':sha(d)} for p,d in contents.items()]}).encode()
        outer=io.BytesIO(); prefix=f'Q3Rally-v{version}-{target}/'
        with zipfile.ZipFile(outer,'w') as z:
            for p,d in contents.items(): z.writestr(member(prefix+p),d)
            z.writestr(member(prefix+'release-manifest.json'),manifest)
        filename=version+'.zip'; data=outer.getvalue(); (output/filename).write_bytes(data); files['/'+filename]=data
        records.append({'size':len(data),'sha256':sha(data),'manifest_sha256':sha(manifest),'entrypoint':'probe.bin'})
        manifests.append(contents)
    delta=build_update(output/'0.8.1.zip',output/'0.8.2.zip',output/'patch.zip'); files['/patch.zip']=(output/'patch.zip').read_bytes()
    class Handler(http.server.BaseHTTPRequestHandler):
        def do_GET(self):
            requests.append(self.path)
            if self.path=='/0.8.2.zip': self.send_error(403,'Full target download forbidden in this test'); return
            data=files.get(self.path)
            if data is None: self.send_error(404); return
            offset=int(self.headers.get('Range','bytes=0-').split('=')[1].split('-')[0])
            self.send_response(206 if offset else 200)
            if offset: self.send_header('Content-Range',f'bytes {offset}-{len(data)-1}/{len(data)}')
            self.send_header('Content-Length',str(len(data)-offset)); self.end_headers(); self.wfile.write(data[offset:])
        def log_message(self,*args): pass
    server=http.server.ThreadingHTTPServer(('127.0.0.1',0),Handler)
    threading.Thread(target=server.serve_forever,daemon=True).start()
    key=SigningKey.generate(); keys={'probe':base64.b64encode(bytes(key.verify_key)).decode()}
    url=f'http://127.0.0.1:{server.server_port}'
    for version,record in zip(['0.8.1','0.8.2'],records): record['url']=url+'/'+version+'.zip'
    records[1]['updates']=[{**{k:delta[k] for k in ('from_version','from_manifest_sha256','sha256','size')},'url':url+'/patch.zip'}]
    files['/catalog.json']=json.dumps(sign({'schema':1,'product':'Q3Rally','sequence':1,
        'expires':(datetime.now(timezone.utc)+timedelta(days=1)).isoformat(),
        'releases':[{'version':v,'packages':{target:r}} for v,r in zip(['0.8.1','0.8.2'],records)],'launchers':[]},key,'probe')).encode()
    atomic_json(output/'config.json',{'public_keys':keys,'catalog_url':url+'/catalog.json'})
    env=dict(os.environ)
    for name in ('PYTHONHOME','PYTHONPATH','LD_LIBRARY_PATH','TCL_LIBRARY','TK_LIBRARY'): env.pop(name,None)
    command=[str(executable),'--development','--config',str(output/'config.json'),'--data-dir',str(output/'data')]
    options={'creationflags':subprocess.CREATE_NO_WINDOW} if os.name=='nt' else {}
    try:
        for action,version in [('install','0.8.1'),('install','0.8.2'),('verify','0.8.2')]:
            result=subprocess.run(command+['--action',action,'--version',version],env=env,capture_output=True,timeout=60,**options)
            if result.returncode:
                log=output/'data/logs/launcher.log'
                raise AssertionError(log.read_text(errors='replace') if log.exists() else repr(result.stderr))
        assert '/patch.zip' in requests and '/0.8.2.zip' not in requests,requests
        for version,contents in zip(['0.8.1','0.8.2'],manifests):
            directory=output/f'data/versions/{version}-{target}'
            assert all((directory/p).read_bytes()==data for p,data in contents.items())
        receipt=json.loads((output/f'data/versions/0.8.2-{target}/.launcher-install.json').read_text())
        assert receipt['data_source_version']=='0.8.1'
        atomic_json(output/'result.json',{'target':target,'frozen_incremental_install_verified':True,
            'full_target_download_not_used':True,'base_unchanged':True,'pk3_byte_exact':True,
            'external_python_environment_removed':True})
        print(target,'frozen incremental install and byte-exact PK3 PASS',flush=True)
    finally:
        server.shutdown(); server.server_close()


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--executable',type=Path,required=True); parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args(); validate(args.executable.resolve(),args.output.resolve())
