#!/usr/bin/env python3
"""Isolated racing run; optionally copy existing local ghosts into its private home."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--map', default='q3r_valley')
    parser.add_argument('--run', required=True)
    parser.add_argument('--ghost-dir', type=Path)
    parser.add_argument('--bpd-file', type=Path,
                        help='copy this map .bpd into the isolated server home')
    parser.add_argument('--duration', type=int, default=180)
    parser.add_argument('--bots', type=int, choices=[1, 4, 5, 8], default=4)
    parser.add_argument('--bot-thinktime', type=int, default=100,
                        help='bot decision interval in milliseconds (50 recommended for car handling)')
    parser.add_argument('--port', type=int, default=27976)
    parser.add_argument('--baseline', action='store_true')
    parser.add_argument('--bot-turbo', action='store_true',
                        help='allow racing bots to use NOS; disabled by default for human-time comparisons')
    parser.add_argument('--skip-intro', action='store_true',
                        help='disable the race intro while retaining BPD bot guidance')
    args = parser.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9_-]+', args.run) or not re.fullmatch(r'[A-Za-z0-9_]+', args.map):
        parser.error('Use plain map/run names')
    if args.duration <= 0 or not 1024 <= args.port <= 65535 or not 25 <= args.bot_thinktime <= 200:
        parser.error('Invalid duration/port')
    root = Path(__file__).resolve().parents[1]
    build = root / ('dist/bot-racing-build/baseline' if args.baseline else 'dist/bot-dm-build/current/release-linux-x86_64')
    home = root / 'dist/bot-racing-runs' / args.run
    game = home / 'baseq3r'
    game.mkdir(parents=True, exist_ok=False)
    shutil.copy2(build / 'baseq3r/qagamex86_64.so', game / 'qagamex86_64.so')
    ghosts = {}
    if args.ghost_dir:
        (game / 'ghosts').mkdir()
        for path in sorted(args.ghost_dir.glob(f'{args.map}_tl1_rev0_*.ghost')):
            shutil.copy2(path, game / 'ghosts' / path.name)
            ghosts[path.name] = hashlib.sha256(path.read_bytes()).hexdigest()
    bpd = None
    if args.bpd_file:
        bpd = game / 'maps' / f'{args.map}.bpd'
        bpd.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(args.bpd_file, bpd)
    names = (['Tony', 'Ivan', 'Steve', 'Bee', 'Tobias'] if args.bots == 5
             else (['Emma', 'Ivan', 'Paul', 'Steve'] * 2)[:args.bots])
    config = '\n'.join([
        'set sv_ladderEnabled 0', *[f'set sv_master{i} ""' for i in range(1, 6)],
        'set sv_pure 0', f'set sv_maxclients {args.bots}', 'set sv_fps 20',
        'set g_gametype 0', 'set g_trackLength 1', 'set g_trackReversed 0',
        'set bot_enable 1', 'set bot_minplayers 0', 'set bot_nochat 1',
        f'set g_botRaceUseTurbo {int(args.bot_turbo)}',
        f'set bot_thinktime {args.bot_thinktime}', 'set g_synchronousClients 1', 'set g_doWarmup 0',
        'set g_log match.log', 'set g_logsync 1', 'set laplimit 100', 'set timelimit 0',
        'set g_aiDmnetDebugExport 2', 'set g_aiDmnetDebugExportPath racing.jsonl',
        f'set g_rallyIntroEnabled {0 if args.skip_intro else 1}',
        'set g_rallyIntroCamClients 0', f'devmap {args.map}', 'timescale 1',
        *[f'addbot {name} 5' for name in names], '',
    ])
    (game / 'benchmark.cfg').write_text(config)
    command = [str(build / 'q3rally-server.x86_64'), '+set', 'dedicated', '1',
        '+set', 'fs_basepath', str(root), '+set', 'fs_homepath', str(home),
        '+set', 'vm_game', '0', '+set', 'net_ip', '127.0.0.1', '+set', 'net_port', str(args.port),
        '+set', 'sv_ladderEnabled', '0', '+exec', 'benchmark.cfg']
    environment = os.environ.copy()
    environment['LD_PRELOAD'] = str(root / 'dist/bot-dm-seed.so')
    environment['Q3RALLY_BENCHMARK_SEED'] = '1337'
    (home / 'run.json').write_text(json.dumps({'argv': command, 'config': config, 'ghosts': ghosts,
        'bpd_file': str(args.bpd_file.resolve()) if args.bpd_file else None,
        'bpd_sha256': hashlib.sha256(bpd.read_bytes()).hexdigest() if bpd else None,
        'build_sha256': {name: hashlib.sha256((build / name).read_bytes()).hexdigest()
        for name in ['q3rally-server.x86_64', 'baseq3r/qagamex86_64.so']}, 'timescale': 1}, indent=2))
    start = time.monotonic()
    simulated = 0
    with (home / 'console.log').open('w') as log:
        process = subprocess.Popen(command, cwd=root, stdin=subprocess.PIPE, stdout=log,
            stderr=subprocess.STDOUT, env=environment, text=True)
        offset = 0
        try:
            while process.poll() is None and time.monotonic() - start < args.duration * 2 + 60:
                samples = game / 'racing.jsonl'
                if samples.exists():
                    with samples.open() as stream:
                        stream.seek(offset)
                        lines = stream.readlines()
                        for line in lines:
                            try:
                                simulated = json.loads(line)['time']
                            except json.JSONDecodeError:
                                break
                            offset += len(line)
                if simulated >= args.duration:
                    break
                time.sleep(.2)
        finally:
            if process.poll() is None:
                process.stdin.write('status\nquit\n')
                process.stdin.flush()
            try:
                code = process.wait(timeout=10)
            except subprocess.TimeoutExpired:
                process.terminate()
                code = process.wait(timeout=5)
    samples = game / 'racing.jsonl'
    if not samples.exists():
        raise SystemExit('No racing telemetry: inspect route selection and console.log')
    rows = [json.loads(line) for line in samples.read_text().splitlines()]
    clients = {}
    for client in sorted({row['client'] for row in rows}):
        all_rows = [row for row in rows if row['client'] == client]
        grid = [row for row in all_rows if row['launchGate'] == 1]
        data = [row for row in all_rows if row['launchGate'] == 0]
        transitions = [row['time'] for before, row in zip(data, data[1:]) if row['lap'] > before['lap']]
        lap_start = data[0]['time'] if data else 0
        lap_times = [round(end - start, 2) for start, end in zip([lap_start] + transitions, transitions)]
        clients[client] = {
            'samples': len(data), 'max_lap': max((row.get('lap', 0) for row in data), default=0),
            'checkpoints': sorted({row.get('nextCheckpoint', 0) for row in data}),
            'mean_speed': round(sum(row['actualSpeed'] for row in data) / max(1, len(data)), 1),
            'recovery_samples': sum(row['recoveryState'] != 'none' for row in data),
            'resets': sum(row['recoveryEvent'] == 'emergency_route_reset' for row in data),
            'grid_origin': [round(grid[0][axis], 1) for axis in ('ox', 'oy', 'oz')] if grid else None,
            'max_grid_xy_drift': round(max((sum((row[axis] - grid[0][axis])**2 for axis in ('ox', 'oy'))**.5 for row in grid), default=0), 1),
            'backward_seconds': round(sum(row.get('signedSpeed', 0) < -35 for row in data) * .1, 1),
            'reverse_command_seconds': round(sum(row.get('driveCommand', 0) < 0 for row in data) * .1, 1),
            'recovery_reverse_seconds': round(sum(row['recoveryState'] == 'reverse_unwind' for row in data) * .1, 1),
            'completed_lap_seconds': lap_times,
        }
    result = {'run': args.run, 'exit': code, 'simulated_seconds': simulated,
        'sources': sorted({row['routeSource'] for row in rows}), 'clients': clients}
    (home / 'result.json').write_text(json.dumps(result, indent=2))
    print(json.dumps(result), flush=True)
    if code or simulated < args.duration:
        raise SystemExit('Incomplete benchmark; inspect console.log')


if __name__ == '__main__':
    main()
