"""Check real engine registration and saved credentials against a loopback fixture."""
import argparse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
from pathlib import Path
import subprocess
import threading
import time
from urllib.parse import parse_qs


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable',type=Path,required=True)
    parser.add_argument('--basepath',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    executable=args.executable.resolve(strict=True);basepath=args.basepath.resolve(strict=True)
    output=args.output.resolve();output.mkdir(parents=True,exist_ok=False)
    results=[]
    for status,digit in [('pending','a'),('active','b')]:
        key=digit*64;requests=[]
        class Handler(BaseHTTPRequestHandler):
            def log_message(self,*args):pass
            def do_POST(self):
                fields=parse_qs(self.rfile.read(int(self.headers.get('Content-Length','0'))).decode())
                requests.append({'path':self.path,'fields':fields,'accept':self.headers.get('Accept','')})
                body=json.dumps({'status':status,'key':key}).encode()
                self.send_response(200);self.send_header('Content-Type','application/json')
                self.send_header('Content-Length',str(len(body)));self.end_headers();self.wfile.write(body)
        fixture=ThreadingHTTPServer(('127.0.0.1',0),Handler)
        thread=threading.Thread(target=fixture.serve_forever,daemon=True);thread.start()
        home=output/status;home.mkdir()
        command=[str(executable),'+set','fs_basepath',str(basepath),'+set','fs_homepath',str(home),
            '+set','net_enabled','0','+set','dedicated','1','+set','profile_active','RegistrationProbe',
            '+set','sv_ladderUrl',f'http://127.0.0.1:{fixture.server_port}/matches',
            '+map','q3r_country01','+ladder_register','RegistrationProbe','probe@example.invalid','RegistrationProbe_OFFLINE','agree']
        saved=False;code=None
        with (home/'stdout.log').open('wb') as log:
            proc=subprocess.Popen(command,cwd=basepath,stdout=log,stderr=subprocess.STDOUT,
                stdin=subprocess.DEVNULL,**({'creationflags':subprocess.CREATE_NO_WINDOW} if __import__('os').name=='nt' else {}))
            try:
                deadline=time.monotonic()+45
                while time.monotonic()<deadline and proc.poll() is None:
                    for config in (home/'baseq3r').glob('q3config*.cfg'):
                        if key in config.read_text(errors='replace'):saved=True;break
                    if saved:break
                    time.sleep(.1)
                code=proc.poll()
            finally:
                if proc.poll() is None:
                    proc.terminate()
                    try:proc.wait(timeout=10)
                    except subprocess.TimeoutExpired:proc.kill();proc.wait(timeout=10)
                fixture.shutdown();fixture.server_close();thread.join(timeout=5)
        assert saved,(status,code,(home/'stdout.log').read_text(errors='replace')[-4000:])
        assert len(requests)==1,requests
        request=requests[0]
        assert request['path']=='/register.php' and 'application/json' in request['accept']
        assert request['fields'].get('server_name')==['RegistrationProbe_OFFLINE']
        assert request['fields'].get('agree')==['1']
        results.append({'status':status,'http_registration_verified':True,'offline_name_verified':True,'credentials_persisted':True})
        print(status+': native offline registration and saved credentials PASS',flush=True)
    (output/'registration-results.json').write_text(json.dumps(results,indent=2)+'\n')


if __name__=='__main__':main()
