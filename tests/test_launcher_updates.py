"""Signed incremental installs through HTTP, including byte-exact PK3 reconstruction."""
import base64
from datetime import datetime, timedelta, timezone
import hashlib
import http.server
import io
import json
import os
from pathlib import Path
import random
import stat
import tempfile
import threading
import unittest
from unittest.mock import patch
import zipfile

from nacl.signing import SigningKey
from launcher.core import Manager, LauncherError, decode_catalog, file_lock
from tools.make_incremental_update import build_update
from tools.publish_launcher_catalog import sign


def sha(data): return hashlib.sha256(data).hexdigest()


def info(name,mode=0o644):
    item=zipfile.ZipInfo(name,(2000,1,1,0,0,0))
    item.compress_type=zipfile.ZIP_DEFLATED
    item.external_attr=(stat.S_IFREG|mode)<<16
    return item


class IncrementalUpdateTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory(); self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name).resolve()
        self.key=SigningKey.generate(); self.keys={'test':base64.b64encode(bytes(self.key.verify_key)).decode()}
        self.files={}; self.requests=[]
        owner=self
        class Handler(http.server.BaseHTTPRequestHandler):
            def do_GET(self):
                owner.requests.append(self.path)
                if self.path not in owner.files: self.send_error(404); return
                data=owner.files[self.path]
                offset=int(self.headers.get('Range','bytes=0-').split('=')[1].split('-')[0])
                self.send_response(206 if offset else 200)
                if offset: self.send_header('Content-Range',f'bytes {offset}-{len(data)-1}/{len(data)}')
                self.send_header('Content-Length',str(len(data)-offset)); self.end_headers()
                self.wfile.write(data[offset:])
            def log_message(self,*args): pass
        self.server=http.server.ThreadingHTTPServer(('127.0.0.1',0),Handler)
        threading.Thread(target=self.server.serve_forever,daemon=True).start()
        self.addCleanup(self.server.server_close); self.addCleanup(self.server.shutdown)
        self.url=f'http://127.0.0.1:{self.server.server_port}'
        self.random=random.Random(42).randbytes(256*1024)
        self.old,self.old_manifest=self.package('0.8.1',False)
        self.new,self.manifest=self.package('0.8.2',True)
        self.delta_path=self.root/'patch.zip'
        self.delta=build_update(self.root/'old.zip',self.root/'new.zip',self.delta_path)
        self.files['/patch.zip']=self.delta_path.read_bytes()
        self.envelope=self.catalog()
        self.manager=Manager(self.root/'data',self.keys,targets=['windows-x86_64'],development=True)

    def package(self,version,new):
        packed=io.BytesIO()
        with zipfile.ZipFile(packed,'w') as z:
            z.writestr(info('textures/common.bin'),self.random)
            z.writestr(info('scripts/track.arena'),b'tracklengths "0 1"' if new else b'tracklengths "1"')
        files={'q3rally.x86_64.exe':b'new executable' if new else b'old executable',
               'baseq3r/pak0.pk3':packed.getvalue(),'shared.dat':self.random}
        files['added.txt' if new else 'removed.txt']=b'release-specific'
        records=[{'path':p,'size':len(d),'sha256':sha(d)} for p,d in files.items()]
        manifest=json.dumps({'schema':1,'product':'Q3Rally','version':version,'target':'windows-x86_64','files':records}).encode()
        archive=io.BytesIO(); prefix=f'Q3Rally-v{version}-windows-x86_64/'
        with zipfile.ZipFile(archive,'w') as z:
            for p,data in files.items(): z.writestr(info(prefix+p,0o755 if p.endswith('.exe') else 0o644),data)
            z.writestr(info(prefix+'release-manifest.json'),manifest)
        data=archive.getvalue(); name='/new.zip' if new else '/old.zip'; self.files[name]=data
        (self.root/name.lstrip('/')).write_bytes(data)
        return {'url':self.url+name,'size':len(data),'sha256':sha(data),'manifest_sha256':sha(manifest),'entrypoint':'q3rally.x86_64.exe'},manifest

    def catalog(self,update=None):
        new=dict(self.new)
        new['updates']=[update or {**{k:self.delta[k] for k in ('from_version','from_manifest_sha256','size','sha256')},'url':self.url+'/patch.zip'}]
        return sign({'schema':1,'product':'Q3Rally','sequence':2,'expires':(datetime.now(timezone.utc)+timedelta(days=1)).isoformat(),
                     'releases':[{'version':'0.8.2','packages':{'windows-x86_64':new}},
                                 {'version':'0.8.1','packages':{'windows-x86_64':self.old}}],'launchers':[]},self.key,'test')

    def install_old(self):
        self.manager.install(self.envelope,'0.8.1','windows-x86_64'); self.requests.clear()

    def test_only_delta_downloaded_pk3_exact_and_old_version_untouched(self):
        self.install_old()
        old=self.manager.path('0.8.1','windows-x86_64')
        snapshot={p.relative_to(old).as_posix():p.read_bytes() for p in old.rglob('*') if p.is_file()}
        self.assertLess(self.delta['size'],self.new['size']//10)
        directory=self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        self.assertEqual(self.requests,['/patch.zip']); self.assertEqual(self.manager.last_install_method,'incremental')
        self.manager.verify('0.8.2','windows-x86_64')
        with zipfile.ZipFile(self.root/'new.zip') as z:
            self.assertEqual((directory/'baseq3r/pak0.pk3').read_bytes(),z.read('Q3Rally-v0.8.2-windows-x86_64/baseq3r/pak0.pk3'))
        self.assertFalse((directory/'removed.txt').exists()); self.assertTrue((directory/'added.txt').exists())
        self.assertEqual(snapshot,{p.relative_to(old).as_posix():p.read_bytes() for p in old.rglob('*') if p.is_file()})
        if os.name!='nt': self.assertTrue(os.access(directory/'q3rally.x86_64.exe',os.X_OK))

    def test_first_install_uses_full_package(self):
        self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        self.assertEqual(self.requests,['/new.zip']); self.assertEqual(self.manager.last_install_method,'full')

    def test_corrupt_or_missing_base_falls_back_without_downloading_delta(self):
        self.install_old()
        (self.manager.path('0.8.1','windows-x86_64')/'shared.dat').write_bytes(b'corrupt')
        self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        self.assertEqual(self.requests,['/new.zip']); self.assertEqual(self.manager.last_install_method,'full')

    def test_corrupt_download_falls_back_to_full(self):
        self.install_old(); self.files['/patch.zip']=b'x'*self.delta['size']
        self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        self.assertEqual(self.requests,['/patch.zip','/new.zip'])
        self.assertEqual(self.manager.last_install_method,'full'); self.manager.verify('0.8.2','windows-x86_64')

    def test_busy_base_uses_full_package(self):
        self.install_old(); base=self.manager.path('0.8.1','windows-x86_64')
        with file_lock(self.manager.locks/('Q3Rally-'+base.name+'.lock')):
            self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        self.assertEqual(self.requests,['/new.zip']); self.assertEqual(self.manager.last_install_method,'full')

    def altered_patch(self,modify):
        original=zipfile.ZipFile(io.BytesIO(self.files['/patch.zip'])); output=io.BytesIO()
        plan=json.loads(original.read('update.json')); modify(plan)
        with zipfile.ZipFile(output,'w',zipfile.ZIP_DEFLATED) as target:
            for name in original.namelist(): target.writestr(name,json.dumps(plan).encode() if name=='update.json' else original.read(name))
        data=output.getvalue(); self.files['/patch.zip']=data
        update={k:self.delta[k] for k in ('from_version','from_manifest_sha256')}
        update.update(url=self.url+'/patch.zip',size=len(data),sha256=sha(data))
        return self.catalog(update)

    def test_signed_but_invalid_range_or_path_falls_back_safely(self):
        self.install_old()
        original=self.files['/patch.zip']
        for kind in ['range','path','manifest','size']:
            with self.subTest(kind=kind):
                self.files['/patch.zip']=original
                def modify(plan):
                    if kind=='range': plan['files']['baseq3r/pak0.pk3']['chunks'][0]={'offset':-1,'size':2}
                    elif kind=='path': plan['files']['../escaped.txt']=plan['files'].pop('shared.dat')
                    elif kind=='manifest': plan['from_manifest_sha256']='0'*64
                    else: plan['files']['shared.dat']={'chunks':[{'offset':0,'size':99999999999999}]}
                envelope=self.altered_patch(modify)
                self.manager.install(envelope,'0.8.2','windows-x86_64')
                self.assertEqual(self.manager.last_install_method,'full')
                self.manager.uninstall('0.8.2','windows-x86_64')
                self.assertFalse((self.manager.versions/'escaped.txt').exists())

    def test_pause_does_not_fall_back_or_publish_partial_install(self):
        self.install_old(); cancel=threading.Event()
        def progress(done,total):
            if done: cancel.set()
        with self.assertRaises(LauncherError):
            self.manager.install(self.envelope,'0.8.2','windows-x86_64',cancel=cancel,progress=progress)
        self.assertNotIn('/new.zip',self.requests)
        self.assertFalse(self.manager.path('0.8.2','windows-x86_64').exists())
        self.manager.verify('0.8.1','windows-x86_64')
        self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        self.assertEqual(self.manager.last_install_method,'incremental')

    def test_failed_update_and_full_fallback_keep_existing_target(self):
        self.install_old(); directory=self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        before={p.relative_to(directory).as_posix():p.read_bytes() for p in directory.rglob('*') if p.is_file()}
        envelope=self.altered_patch(lambda plan:plan['files'].pop('shared.dat'))
        self.files['/new.zip']=b'x'*self.new['size']
        with self.assertRaises(LauncherError): self.manager.install(envelope,'0.8.2','windows-x86_64')
        self.assertEqual(before,{p.relative_to(directory).as_posix():p.read_bytes() for p in directory.rglob('*') if p.is_file()})
        self.manager.verify('0.8.2','windows-x86_64')

    def test_publisher_binds_update_to_full_package_and_published_base(self):
        try:
            from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
            from cryptography.hazmat.primitives import serialization
        except ImportError:
            self.skipTest('publishing-only dependency')
        from tools.publish_launcher_catalog import main
        key=Ed25519PrivateKey.from_private_bytes(bytes(self.key))
        private=self.root/'test.pem'
        private.write_bytes(key.private_bytes(serialization.Encoding.PEM,serialization.PrivateFormat.PKCS8,serialization.NoEncryption()))
        config=self.root/'config.json'; config.write_text(json.dumps({'public_keys':self.keys}))
        previous=json.loads(base64.b64decode(self.envelope['payload'])); previous['releases']=previous['releases'][1:]
        old=self.root/'previous.json'; old.write_text(json.dumps(sign(previous,self.key,'test')))
        index=self.root/'release.json'
        index.write_text(json.dumps({'schema':1,'product':'Q3Rally','version':'0.8.2','artifacts':[
            {'target':'windows-x86_64','filename':'new.zip','size':self.new['size'],'sha256':self.new['sha256']}]}))
        update=self.root/'updates.json'; update.write_text(json.dumps({'schema':1,'product':'Q3Rally','updates':[self.delta]}))
        output=self.root/'published.json'
        args=['catalog','--key-id','test','--private-key',str(private),'--public-config',str(config),'--previous',str(old),
              '--release-index',str(index),'--update-index',str(update),'--base-url','https://example.invalid',
              '--sequence','3','--output',str(output)]
        main(args)
        published=decode_catalog(json.loads(output.read_text()),self.keys)
        package=published['releases'][0]['packages']['windows-x86_64']
        self.assertEqual(package['updates'][0]['sha256'],self.delta['sha256'])
        self.assertEqual(published['releases'][1],previous['releases'][0])
        invalid=dict(self.delta); invalid['from_manifest_sha256']='0'*64
        update.write_text(json.dumps({'schema':1,'product':'Q3Rally','updates':[invalid]}))
        with self.assertRaises(ValueError): main(args)

    def test_patch_data_copied_once_before_launch_and_old_home_kept(self):
        self.install_old(); old=self.manager.root/'user-data/0.8.1'; old.mkdir(parents=True)
        (old/'profile.json').write_text('existing identity and credentials')
        self.manager.install(self.envelope,'0.8.2','windows-x86_64')
        new=self.manager.root/'user-data/0.8.2'
        self.assertFalse(new.exists())
        with patch('launcher.core.subprocess.Popen') as process:
            process.return_value.wait.return_value=0
            self.assertEqual(self.manager.run('0.8.2','windows-x86_64'),0)
        self.assertEqual((new/'profile.json').read_text(),(old/'profile.json').read_text())
        (new/'profile.json').write_text('new progress'); (old/'profile.json').write_text('old progress')
        self.manager._prepare_patch_data('0.8.2','windows-x86_64')
        self.assertEqual((new/'profile.json').read_text(),'new progress')
        self.assertEqual((old/'profile.json').read_text(),'old progress')

    def test_catalog_rejects_cross_line_newer_duplicate_and_bad_hash_updates(self):
        original=json.loads(base64.b64decode(self.envelope['payload']))
        for value in ['0.9.0','0.8.2','0.8.3']:
            payload=json.loads(json.dumps(original)); payload['releases'][0]['packages']['windows-x86_64']['updates'][0]['from_version']=value
            with self.assertRaises(LauncherError): decode_catalog(sign(payload,self.key,'test'),self.keys)
        for kind in ['duplicate','hash','size']:
            payload=json.loads(json.dumps(original)); updates=payload['releases'][0]['packages']['windows-x86_64']['updates']
            if kind=='duplicate': updates.append(dict(updates[0]))
            elif kind=='hash': updates[0]['from_manifest_sha256']='invalid'
            else: updates[0]['size']=True
            with self.assertRaises(LauncherError): decode_catalog(sign(payload,self.key,'test'),self.keys)


if __name__=='__main__': unittest.main()
