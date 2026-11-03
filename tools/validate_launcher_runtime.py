"""Exercise a real game package through the launcher using an isolated local catalog.

Requires a native desktop/display. Test signing keys exist only in memory;
this never publishes packages or changes the production catalog.
"""
import argparse
import base64
from datetime import datetime, timedelta, timezone
import http.server
import json
from pathlib import Path
import shutil
import sys
import threading

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from nacl.signing import SigningKey
import psutil
from launcher.core import Manager, LauncherError, atomic_json, digest_file
from launcher.profiles import import_profile
from tools.publish_launcher_catalog import Index, package_record, sign


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--release-index', type=Path, required=True)
    parser.add_argument('--target', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--timeout', type=int, default=180)
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    index = Index(args.release_index.resolve())
    version = index.data['version']
    artifact = next(a for a in index.data['artifacts'] if a['target'] == args.target)
    archive = index.parent / artifact['filename']
    package = package_record(index, artifact, 'https://example.invalid', 'Q3Rally')
    key = SigningKey.generate()
    keys = {'runtime-test-only': base64.b64encode(bytes(key.verify_key)).decode()}
    ranges = []
    envelope = None

    class Handler(http.server.BaseHTTPRequestHandler):
        def do_GET(self):
            if self.path == '/catalog.json':
                data = json.dumps(envelope).encode()
                self.send_response(200); self.send_header('Content-Length', str(len(data)))
                self.end_headers(); self.wfile.write(data)
                return
            if self.path != '/game.zip':
                self.send_error(404); return
            offset = int(self.headers.get('Range', 'bytes=0-').split('=')[1].split('-')[0])
            size = archive.stat().st_size
            ranges.append(offset)
            self.send_response(206 if offset else 200)
            self.send_header('Content-Length', str(size - offset))
            if offset:
                self.send_header('Content-Range', f'bytes {offset}-{size-1}/{size}')
            self.end_headers()
            try:
                with archive.open('rb') as stream:
                    stream.seek(offset)
                    shutil.copyfileobj(stream, self.wfile, 256 * 1024)
            except (BrokenPipeError, ConnectionResetError, ConnectionAbortedError):
                pass  # Expected when the real launcher pauses its first download.

        def log_message(self, *unused):
            pass

    server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), Handler)
    threading.Thread(target=server.serve_forever, daemon=True).start()
    try:
        package['url'] = f'http://127.0.0.1:{server.server_port}/game.zip'
        envelope = sign({'schema': 1, 'product': 'Q3Rally', 'sequence': 1,
            'expires': (datetime.now(timezone.utc) + timedelta(days=1)).isoformat(),
            'releases': [{'version': version, 'packages': {args.target: package}}]}, key, 'runtime-test-only')
        manager = Manager(output / 'data', keys, targets=[args.target], development=True)
        envelope, _ = manager.load_catalog(f'http://127.0.0.1:{server.server_port}/catalog.json')
        cancel = threading.Event()
        try:
            manager.install(envelope, version, args.target, cancel=cancel,
                            progress=lambda done, total: cancel.set())
            raise AssertionError('Download pause did not interrupt installation')
        except LauncherError:
            if not cancel.is_set():
                raise
        directory = manager.install(envelope, version, args.target)
        executable = manager.verify(version, args.target)
    finally:
        server.shutdown(); server.server_close()

    # The download service is deliberately gone before the game starts.
    home = manager.root / 'user-data' / version / 'baseq3r'
    source = output / 'LauncherProbe.json'
    source.write_text(json.dumps({'uuid': '12345678-1234-4234-8234-123456789abc',
                                 'name': 'LauncherProbe', 'info': {'country': 'DE'}, 'stats': {'playerScore': 9999}}))
    profile = import_profile(source, manager.root, version)
    autoexec = home / 'autoexec.cfg'
    autoexec.write_text('\n'.join([
        'set r_fullscreen 0', 'set r_mode -1', 'set r_customwidth 800', 'set r_customheight 600',
        'set cl_renderer opengl1', 'set in_mouse 0', 'set in_nograb 1',
        'set s_useOpenAL 0', 'set s_sdlCapture 0', 'set cl_motd 0',
        'set com_maxfps 30', 'set com_maxfpsUnfocused 30', 'set com_maxfpsMinimized 30',
        'set sv_ladderEnabled 0', 'set profile_active LauncherProbe',
        'set cl_uuid 12345678-1234-4234-8234-123456789abc', 'set developer 1',
        'echo LAUNCHER_RUNTIME_BEGIN', 'wait 360', 'screenshotJPEG launcher-menu', 'wait 2',
        'map q3r_country01', 'wait 30', 'screenshotJPEG launcher-track', 'wait 2',
        'echo LAUNCHER_RUNTIME_END', 'quit', '']))
    outcome = {}

    def play():
        try:
            outcome['exit_code'] = manager.run(version, args.target)
        except Exception as error:
            outcome['error'] = str(error)

    player = threading.Thread(target=play)
    player.start(); player.join(args.timeout)
    if player.is_alive():
        for process in psutil.process_iter(['exe']):
            try:
                if process.info['exe'] and Path(process.info['exe']).resolve() == executable.resolve():
                    process.terminate()
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                pass
        player.join(10)
        raise RuntimeError('Game did not exit before the test timeout')
    log = next((manager.root / 'logs').glob(version + '-*.log')).read_text(errors='replace')
    user_hash = digest_file(profile)
    (directory / 'README.txt').write_text('deliberate repair probe')
    tamper_detected = False
    try:
        manager.verify(version, args.target)
    except LauncherError:
        tamper_detected = True
    # Repair uses the cached verified archive; no network service is running.
    manager.install(envelope, version, args.target)
    manager.verify(version, args.target)
    checks = {'download_resumed': any(offset > 0 for offset in ranges),
              'normal_exit': outcome.get('exit_code') == 0,
              'game_completed_script': 'LAUNCHER_RUNTIME_END' in log,
              'country01_loaded': 'Server: q3r_country01' in log,
              'renderer_initialized': 'GL_RENDERER:' in log,
              'menu_screenshot': (home / 'screenshots/launcher-menu.jpg').is_file(),
              'track_screenshot': (home / 'screenshots/launcher-track.jpg').is_file(),
              'tamper_detected': tamper_detected,
              'repair_preserved_profile': digest_file(profile) == user_hash,
              'repair_preserved_autoexec': autoexec.is_file(),
              'separate_user_directory': (home / 'q3config.cfg').is_file()}
    atomic_json(output / 'result.json', {'target': args.target, 'version': version,
        'checks': checks, 'game': outcome, 'download_offsets': ranges, 'all_passed': all(checks.values())})
    print(json.dumps(checks, indent=2), flush=True)
    return 0 if all(checks.values()) else 1


if __name__ == '__main__':
    raise SystemExit(main())
