#!/usr/bin/env python3
"""Compute KPI metrics from DMNet debug exports (CSV or JSONL)."""
from __future__ import annotations

import argparse
import csv
import json
import math
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List

OVERTAKE_STATES = {"prepare_overtake", "overtake_inside", "overtake_outside"}
ABORT_STATE = "abort_overtake"


@dataclass
class Row:
    decision_state: str
    collision_risk: int
    recovery_event: str
    route_deviation: float


def load_rows(path: Path) -> List[Row]:
    if path.suffix.lower() in {".json", ".jsonl"}:
        return _load_jsonl(path)
    return _load_csv(path)


def _load_csv(path: Path) -> List[Row]:
    rows: List[Row] = []
    with path.open("r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        for r in reader:
            rows.append(
                Row(
                    decision_state=(r.get("decisionState") or "").strip(),
                    collision_risk=int(float(r.get("collisionRisk") or 0)),
                    recovery_event=(r.get("recoveryEvent") or "").strip(),
                    route_deviation=float(r.get("routeDeviation") or 0.0),
                )
            )
    return rows


def _load_jsonl(path: Path) -> List[Row]:
    rows: List[Row] = []
    with path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            r = json.loads(line)
            rows.append(
                Row(
                    decision_state=str(r.get("decisionState", "")).strip(),
                    collision_risk=int(r.get("collisionRisk", 0)),
                    recovery_event=str(r.get("recoveryEvent", "")).strip(),
                    route_deviation=float(r.get("routeDeviation", 0.0)),
                )
            )
    return rows


def compute_kpis(rows: Iterable[Row]) -> Dict[str, float]:
    rows = list(rows)
    if not rows:
        return {
            "samples": 0,
            "avg_ideal_line_deviation": math.nan,
            "overtake_maneuvers": 0,
            "overtake_aborts": 0,
            "collision_rate": math.nan,
            "recovery_frequency": math.nan,
        }

    overtake_maneuvers = 0
    overtake_aborts = 0
    collision_ticks = 0
    recovery_events = 0

    in_overtake = False
    prev_state = ""

    for row in rows:
        state = row.decision_state
        if state in OVERTAKE_STATES and prev_state not in OVERTAKE_STATES:
            overtake_maneuvers += 1
            in_overtake = True
        if in_overtake and state == ABORT_STATE:
            overtake_aborts += 1
            in_overtake = False
        if state not in OVERTAKE_STATES and state != ABORT_STATE:
            in_overtake = False

        if row.collision_risk:
            collision_ticks += 1
        if row.recovery_event:
            recovery_events += 1

        prev_state = state

    sample_count = len(rows)
    avg_dev = sum(r.route_deviation for r in rows) / sample_count

    return {
        "samples": sample_count,
        "avg_ideal_line_deviation": avg_dev,
        "overtake_maneuvers": overtake_maneuvers,
        "overtake_aborts": overtake_aborts,
        "collision_rate": collision_ticks / sample_count,
        "recovery_frequency": recovery_events / sample_count,
    }


def evaluate_acceptance(kpis: Dict[str, float], thresholds: Dict[str, float]) -> List[str]:
    failures: List[str] = []
    for key, max_value in thresholds.items():
        if key not in kpis:
            failures.append(f"missing KPI: {key}")
            continue
        value = kpis[key]
        if not math.isfinite(value) or value > max_value:
            failures.append(f"{key}={value:.6g} exceeds max {max_value:.6g}")
    return failures


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="CSV or JSONL debug export")
    parser.add_argument(
        "--threshold",
        action="append",
        default=[],
        metavar="KPI=MAX",
        help="Acceptance criterion, repeatable (e.g. collision_rate=0.15)",
    )
    args = parser.parse_args()

    rows = load_rows(args.input)
    kpis = compute_kpis(rows)

    print(json.dumps(kpis, indent=2, sort_keys=True, ensure_ascii=False))

    thresholds: Dict[str, float] = {}
    for entry in args.threshold:
        if "=" not in entry:
            print(f"Invalid threshold format: {entry}", file=sys.stderr)
            return 2
        key, raw = entry.split("=", 1)
        thresholds[key.strip()] = float(raw)

    if thresholds:
        failures = evaluate_acceptance(kpis, thresholds)
        if failures:
            print("\nACCEPTANCE: FAIL", file=sys.stderr)
            for msg in failures:
                print(f" - {msg}", file=sys.stderr)
            return 1
        print("\nACCEPTANCE: PASS")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
