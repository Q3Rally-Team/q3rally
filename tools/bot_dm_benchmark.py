#!/usr/bin/env python3
"""Run an isolated, local Linux dedicated DM match at a selected bot skill."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("variant", choices=["baseline", "round1", "round2", "current"])
    parser.add_argument("--skill", type=int, choices=range(1, 6), default=4)
    parser.add_argument("--bots", type=int, choices=[4, 8], default=4)
    parser.add_argument("--map", default="q3r_dm02")
    parser.add_argument("--seed", type=int, default=1337)
    parser.add_argument("--run", default="1", help="Unique name; existing results are never overwritten")
    parser.add_argument("--duration", type=int, default=180)
    parser.add_argument("--timescale", type=int, default=1, help="Use 1 for strength comparisons; higher values are stress tests")
    parser.add_argument("--port", type=int, help="Override the variant's loopback port for independent parallel runs")
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9_]+", args.map) or not re.fullmatch(r"[A-Za-z0-9_-]+", args.run):
        parser.error("Map/run names must contain only letters, digits, underscores or run-name hyphens")
    if args.duration <= 0 or args.timescale <= 0:
        parser.error("Duration and timescale must be positive")
    if args.port is not None and not 1024 <= args.port <= 65535:
        parser.error("Port must be between 1024 and 65535")
    root = Path(__file__).resolve().parents[1]
    build = root / "dist" / "bot-dm-build" / args.variant / "release-linux-x86_64"
    seed_library = root / "dist" / "bot-dm-seed.so"
    if not seed_library.is_file():
        parser.error("Build dist/bot-dm-seed.so from tools/bot_dm_seed.c first")
    home = root / "dist" / "bot-dm-runs" / f"{args.variant}-{args.map}-{args.seed}-{args.run}"
    game = home / "baseq3r"
    game.mkdir(parents=True, exist_ok=False)
    shutil.copy2(build / "baseq3r" / "qagamex86_64.so", game / "qagamex86_64.so")
    config = "\n".join([
        'set sv_ladderEnabled 0', *[f'set sv_master{i} ""' for i in range(1, 6)],
        'set sv_pure 0', f'set sv_maxclients {args.bots}', 'set sv_fps 20',
        'set g_gametype 6', 'set bot_enable 1', 'set bot_minplayers 0',
        'set bot_nochat 1', 'set bot_thinktime 100', 'set g_synchronousClients 1',
        'set g_doWarmup 0', 'set g_warmup 0', 'set g_log match.log',
        'set g_logsync 1', 'set g_botDmDebug 1', 'set fraglimit 0', 'set timelimit 0',
        'set g_rallyIntroCamClients 0', f'devmap {args.map}', f'timescale {args.timescale}',
        'set nextmap quit',
        *[f'addbot {name} {args.skill}' for name in ['Emma', 'Ivan', 'Paul', 'Steve'] * (args.bots // 4)], '',
    ])
    (game / "benchmark.cfg").write_text(config, encoding="utf-8")
    environment = os.environ.copy()
    environment["LD_PRELOAD"] = str(seed_library)
    environment["Q3RALLY_BENCHMARK_SEED"] = str(args.seed)
    command = [str(build / "q3rally-server.x86_64"), '+set', 'dedicated', '1',
        '+set', 'fs_basepath', str(root), '+set', 'fs_homepath', str(home),
        '+set', 'vm_game', '0', '+set', 'net_ip', '127.0.0.1',
        '+set', 'net_port', str(args.port or {'baseline': 27971, 'current': 27972, 'round1': 27973, 'round2': 27974}[args.variant]),
        '+set', 'sv_ladderEnabled', '0', '+exec', 'benchmark.cfg']
    (home / "run.json").write_text(json.dumps({
        "argv": command, "seed": args.seed, "config": config,
        "skill": args.skill, "bots": args.bots, "timescale": args.timescale,
        "build_sha256": {name: hashlib.sha256((build / name).read_bytes()).hexdigest()
            for name in ["q3rally-server.x86_64", "baseq3r/qagamex86_64.so"]},
        "randomness": "Fixed libc RNG initializer; wall-clock scheduling remains variable",
    }, indent=2), encoding="utf-8")
    start = time.monotonic()
    simulated = 0
    with (home / "console.log").open("w", encoding="utf-8") as output:
        process = subprocess.Popen(command, cwd=root, stdin=subprocess.PIPE, stdout=output,
            stderr=subprocess.STDOUT, env=environment, text=True)
        position = 0
        timeout = max(110, args.duration / args.timescale * 4)
        try:
            while process.poll() is None and time.monotonic() - start < timeout:
                log = game / "match.log"
                if log.exists():
                    with log.open(encoding="utf-8", errors="replace") as data:
                        data.seek(position)
                        chunk = data.read()
                        position = data.tell()
                    samples = re.findall(r"BotDM: time=(\d+)", chunk)
                    if samples:
                        simulated = int(samples[-1])
                if simulated >= args.duration * 1000:
                    break
                time.sleep(0.1)
        finally:
            if process.poll() is None:
                process.stdin.write("status\nquit\n")
                process.stdin.flush()
            try:
                code = process.wait(timeout=10)
            except subprocess.TimeoutExpired:
                process.terminate()
                code = process.wait(timeout=5)
    print(json.dumps({"variant": args.variant, "home": str(home), "exit": code,
        "last_simulated_ms": simulated, "wall_seconds": round(time.monotonic() - start, 2)}))
    if code or simulated < args.duration * 1000:
        raise SystemExit("Benchmark did not complete the requested interval; inspect console.log")


if __name__ == "__main__":
    main()
