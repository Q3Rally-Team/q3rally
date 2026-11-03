"""Native headless mode checks; loopback only, with independent user directories.

These are startup/AI/team/restart checks, not proof of completed objective games.
Use the longer documented race probes for lap/Sprint completion validation.
"""
import argparse
import json
import os
from pathlib import Path
import re
import secrets
import socket
import subprocess
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

CASES = {
    'racing': (0, 'q3r_country01', 2, False),
    'sprint': (7, 'q3r_riviera', 2, False),
    'intro': (7, 'q3r_riviera', 2, True),
    'racing_dm': (1, 'q3r_downtown', 2, False),
    'team_racing': (17, 'q3r_country01', 2, False),
    'team_racing_dm': (18, 'q3r_downtown', 2, False),
    'team_dm': (16, 'q3r_battlecanyon', 2, False),
    'ctf': (19, 'q3r_ctf_01', 2, False),
    'ctf4': (20, 'q3r_ctf4_01', 4, False),
    'ctf4_auto': (20, 'q3r_ctf4_01', 4, False),
    'domination': (21, 'q3r_babel', 2, False),
    'koth': (22, 'q3r_mesaking', 2, False),
    'autoball': (23, 'q3r_rooftop', 2, False),
    'derby': (3, 'q3r_david5', 2, False),
    'lcs': (4, 'q3r_dd_02', 2, False),
    'elimination': (5, 'q3r_country01', 2, False),
    'ghost': (8, 'q3r_country01', 0, False),
}


def query(port, command, timeout=.6):
    # Each query owns its socket, so verbose RCON replies cannot pollute status.
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as channel:
        channel.settimeout(timeout)
        channel.sendto(b'\xff' * 4 + command.encode(), ('127.0.0.1', port))
        try:
            return channel.recv(65535).decode('utf-8', 'replace')
        except (socket.timeout, ConnectionResetError):
            return ''


def run_case(args, name, index):
    mode, mapname, expected, intro = CASES[name]
    folder = args.output / name
    home = folder / 'home'
    (home / 'baseq3r').mkdir(parents=True, exist_ok=False)
    password = secrets.token_hex(12)
    port = args.port + index
    teams = ('red', 'blue', 'green', 'yellow') if mode >= 16 else ('free',) * 4
    bot_names = ('Chloe', 'Janine', 'Jennifer', 'Paul')
    lines = [
        'set sv_hostname DedicatedModeProbe', 'set sv_maxclients 8',
        f'set rconPassword {password}', 'set bot_enable 1',
        f'set bot_minplayers {1 if name == "ctf4_auto" else 0}',
        f'set g_gametype {mode}', 'set laplimit 1', 'set timelimit 0',
        f'set g_rallyIntroEnabled {int(intro)}', 'set g_debugIntroCam 1',
        'set g_log games.log', 'set g_logsync 1', 'set sv_ladderEnabled 0',
        'set g_aiDmnetDebugExport 2', 'set g_aiDmnetDebugExportPath logs/drive.jsonl',
        f'map {mapname}',
    ]
    if name != 'ctf4_auto':
        for n in range(expected or 1):
            lines.append(f'addbot {bot_names[n]} 3 {teams[n]}')
    if args.http_probe:
        lines.extend([f'set sv_ladderUrl "{args.http_url}/matches"',
                      'ladder_register NativeProbe probe@example.invalid NativeTest agree'])
    (home / 'baseq3r/probe.cfg').write_text('\n'.join(lines) + '\n', encoding='utf-8')
    cmd = [str(args.executable), '+set', 'fs_basepath', str(args.basepath),
           '+set', 'vm_game', str(args.vm_mode),
           '+set', 'fs_homepath', str(home), '+set', 'dedicated', '1',
           '+set', 'net_ip', '127.0.0.1', '+set', 'net_enabled', '1',
           '+set', 'net_port', str(port), '+set', 'logfile', '2', '+exec', 'probe.cfg']
    kwargs = {}
    if os.name == 'nt':
        startup = subprocess.STARTUPINFO()
        startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        startup.wShowWindow = 0
        kwargs['startupinfo'] = startup
    samples = []
    print(f'START {name} mode={mode} map={mapname}', flush=True)
    with (folder / 'stdout.log').open('w', encoding='utf-8') as log:
        proc = subprocess.Popen(cmd, cwd=args.executable.parent, stdout=log,
                                stderr=subprocess.STDOUT, **kwargs)
        try:
            deadline = time.monotonic() + 25
            ready = False
            while time.monotonic() < deadline and proc.poll() is None:
                if 'statusResponse' in query(port, 'getstatus init'):
                    ready = True
                    break
                time.sleep(.2)
            deadline = time.monotonic() + max(args.seconds, 45 if intro else 0,
                                             150 if args.complete_races else 0)
            while ready and time.monotonic() < deadline and proc.poll() is None:
                status = query(port, 'getstatus modeprobe')
                players = re.findall(r'^(-?\d+) (-?\d+) "(.*)"$', status, re.M)
                samples.append({'players': players, 'count': len(players)})
                if args.complete_races:
                    live = (folder / 'stdout.log').read_text(encoding='utf-8', errors='replace')
                    if len(re.findall(r'broadcast: print "[^\r\n]*(?:won the race!|finished the race in)', live)) >= expected:
                        break
                time.sleep(1)
            (folder / 'status.txt').write_text(query(port, 'rcon ' + password + ' status'), encoding='utf-8')
            alive = proc.poll() is None
        finally:
            if proc.poll() is None:
                query(port, 'rcon ' + password + ' quit')
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                proc.terminate()
                proc.wait(timeout=5)
    text = (folder / 'stdout.log').read_text(encoding='utf-8', errors='replace')
    team_info = {}
    for match in re.finditer(r'ClientUserinfoChanged: (\d+) ([^\r\n]*)', text):
        fields = match[2].split('\\')
        values = dict(zip(fields[::2], fields[1::2]))
        if 't' in values:
            team_info[match[1]] = values['t']
    present_teams = sorted(set(team_info.values()))
    team_check = mode < 16 or set(present_teams) >= ({'1', '2', '3', '4'} if mode == 20 else {'1', '2'})
    count_check = bool(samples) and (samples[-1]['count'] == expected)
    intro_check = not intro or ('-> INTRO_CAM' in text and '-> RUNNING' in text)
    finishes = re.findall(r'broadcast: print "[^\r\n]*(?:won the race!|finished the race in)[^\r\n]*', text)
    finish_check = not args.complete_races or len(finishes) == expected
    http_check = not args.http_probe or 'registration failed: DEDICATED_HTTP_PROBE' in text
    telemetry_records = telemetry_errors = 0
    recovery_events = []
    telemetry = home / 'baseq3r/logs/drive.jsonl'
    if telemetry.is_file():
        for line in telemetry.read_text(encoding='utf-8', errors='replace').splitlines():
            try:
                record = json.loads(line)
                telemetry_records += 1
                if record.get('recoveryEvent'):
                    recovery_events.append(record['recoveryEvent'])
            except (ValueError, AttributeError):
                telemetry_errors += 1
    telemetry_check = not args.check_telemetry or (telemetry_errors == 0 and (expected == 0 or telemetry_records > 0))
    result = {'case': name, 'mode': mode, 'map': mapname, 'ready': ready,
              'alive_before_shutdown': alive, 'exit': proc.returncode,
              'expected_bots': expected, 'samples': samples, 'teams': team_info,
              'count_ok': count_check, 'teams_ok': team_check, 'intro_ok': intro_check,
              'finishes': finishes, 'finish_ok': finish_check, 'http_ok': http_check,
              'telemetry_records': telemetry_records, 'telemetry_errors': telemetry_errors,
              'recovery_events': recovery_events, 'telemetry_ok': telemetry_check,
              'passed': ready and alive and proc.returncode == 0 and count_check and team_check and intro_check and finish_check and http_check and telemetry_check}
    (folder / 'result.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k: v for k, v in result.items() if k != 'samples'}), flush=True)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path, required=True)
    parser.add_argument('--basepath', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--case', action='append', choices=CASES)
    parser.add_argument('--seconds', type=int, default=25)
    parser.add_argument('--port', type=int, default=29100)
    parser.add_argument('--complete-races', action='store_true', help='Wait for race finishes; use racing/sprint cases only')
    parser.add_argument('--http-probe', action='store_true', help='Exercise native Ladder HTTP against a local fixture')
    parser.add_argument('--vm-mode', type=int, choices=(1, 2), default=2, help='1: interpreter, 2: native JIT where available')
    parser.add_argument('--check-telemetry', action='store_true', help='Require nonempty, valid driving JSON for bot cases')
    args = parser.parse_args()
    args.executable = args.executable.resolve(strict=True)
    args.basepath = args.basepath.resolve(strict=True)
    args.output = args.output.resolve()
    args.output.mkdir(parents=True, exist_ok=True)
    names = args.case or list(CASES)
    if args.complete_races and any(name not in ('racing', 'sprint') for name in names):
        parser.error('--complete-races requires explicit --case racing and/or --case sprint')
    http = None
    if args.http_probe:
        class Handler(BaseHTTPRequestHandler):
            def log_message(self, *_):
                pass

            def do_POST(self):
                self.rfile.read(int(self.headers.get('Content-Length', '0')))
                body = b'{"status":"error","error":"DEDICATED_HTTP_PROBE"}'
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Content-Length', str(len(body)))
                self.end_headers()
                self.wfile.write(body)
        http = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
        args.http_url = f'http://127.0.0.1:{http.server_port}'
        threading.Thread(target=http.serve_forever, daemon=True).start()
    results = []
    try:
        for index, name in enumerate(names):
            results.append(run_case(args, name, index))
            (args.output / 'summary.json').write_text(json.dumps(results, indent=2) + '\n', encoding='utf-8')
    finally:
        if http:
            http.shutdown()
            http.server_close()
    return 0 if all(r['passed'] for r in results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
