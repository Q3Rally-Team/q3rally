#!/usr/bin/env python3
"""Summarize BotDM samples; PERS_HITS/button samples are not shot accuracy."""
import argparse
import json
from collections import defaultdict
from pathlib import Path


def summarize(path: Path, duration: float = 180) -> dict:
    clients = defaultdict(list)
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if "BotDM:" not in line:
            continue
        row = {}
        for field in line.split("BotDM:", 1)[1].split():
            if "=" in field:
                key, value = field.split("=", 1)
                row[key] = float(value)
        if row.get("time", duration * 1000) < duration * 1000:
            clients[int(row["client"])].append(row)
    rows = [row for samples in clients.values() for row in samples]
    if not rows:
        raise ValueError(f"No BotDM samples in {path}")
    firing = [row for row in rows if row.get("firing") and row.get("enemy", -1) >= 0]
    target_changes = 0
    stationary_episodes = 0
    low_progress_episodes = low_progress_samples = 0
    longest_low_progress = 0
    has_positions = all('x' in row and 'y' in row for row in rows)
    for samples in clients.values():
        previous = None
        stopped_since = None
        counted = False
        anchor = None
        progress_counted = False
        for row in samples:
            if previous and 0 < row["time"] - previous["time"] <= 1000:
                if row["enemy"] >= 0 and previous["enemy"] >= 0 and row["enemy"] != previous["enemy"]:
                    target_changes += 1
            else:
                stopped_since = None
                counted = False
                anchor = None
                progress_counted = False
            if row["speed"] < 60:
                if stopped_since is None:
                    stopped_since = row["time"]
                if not counted and row["time"] - stopped_since >= 2000:
                    stationary_episodes += 1
                    counted = True
            else:
                stopped_since = None
                counted = False
            if has_positions:
                if anchor is None or (row['x'] - anchor['x'])**2 + (row['y'] - anchor['y'])**2 > 48**2:
                    anchor = row
                    progress_counted = False
                age = row['time'] - anchor['time']
                longest_low_progress = max(longest_low_progress, age)
                if age >= 2000:
                    low_progress_samples += 1
                    if not progress_counted:
                        low_progress_episodes += 1
                        progress_counted = True
            previous = row
    return {
        "log": str(path),
        "window_seconds": duration,
        "clients": len(clients),
        "samples": len(rows),
        "last_sample_seconds": max(row["time"] for row in rows) / 1000,
        "sampled_kills": sum(max(row.get("kills", 0) for row in samples) for samples in clients.values()),
        "sampled_pers_hits": sum(max(row.get("hits", 0) for row in samples) for samples in clients.values()),
        "mean_speed_units_per_second": round(sum(row["speed"] for row in rows) / len(rows), 2),
        "stationary_sample_share": round(sum(row["speed"] < 60 for row in rows) / len(rows), 4),
        "stationary_episodes_at_least_2s": stationary_episodes,
        "low_position_progress_episodes_at_least_2s": low_progress_episodes if has_positions else None,
        "longest_low_position_progress_seconds": round(longest_low_progress / 1000, 2) if has_positions else None,
        "low_position_progress_sample_share": round(low_progress_samples / len(rows), 4) if has_positions else None,
        "recovery_sample_share": round(sum(row.get('recovery', 0) != 0 for row in rows) / len(rows), 4),
        "empty_fuel_sample_share": round(sum(row.get('fuel', 1) <= 0 for row in rows) / len(rows), 4)
            if all('fuel' in row for row in rows) else None,
        "enemy_sample_share": round(sum(row.get("enemy", -1) >= 0 for row in rows) / len(rows), 4),
        "firing_sample_share": round(sum(bool(row.get("firing")) for row in rows) / len(rows), 4),
        "mean_yaw_error_while_firing": round(sum(row["aimError"] for row in firing) / len(firing), 2) if firing else None,
        "visible_target_changes": target_changes,
        "samples_below_z_minus_4096": sum(row.get("z", 0) < -4096 for row in rows),
        "note": "Sampled counters and activity only; no shot-accuracy or general-strength claim.",
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("logs", nargs="+", type=Path)
    parser.add_argument("--duration", type=float, default=180)
    args = parser.parse_args()
    print(json.dumps([summarize(path, args.duration) for path in args.logs], indent=2))


if __name__ == "__main__":
    main()
