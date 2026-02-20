"""Command line helper for configuring dedicated server gametypes.

This lightweight tool normalises user provided gametype names to the
corresponding numeric `g_gametype` values used by the engine.  It mirrors the
order defined in ``bg_public.h`` and intentionally accepts a couple of relaxed
aliases so that values like ``team-racing-dm`` or ``LastCarStanding`` continue
to resolve correctly.  Unknown values fall back to the Elimination mode so that
automation scripts keep working even if they forward an unsupported name.
"""
from __future__ import annotations

import argparse
from typing import Iterable, Tuple


# Numeric values must stay in sync with ``gametype_t`` in bg_public.h.
GAMETYPES: dict[str, int] = {
    "racing": 0,
    "racing_dm": 1,
    "sprint": 2,
    "single_player": 3,
    "derby": 4,
    "lcs": 5,
    "elimination": 6,
    "deathmatch": 7,
    "team": 8,
    "team_racing": 9,
    "team_racing_dm": 10,
    "ctf": 11,
    "ctf4": 12,
    "domination": 13,
}


ALIASES: dict[str, str] = {
    "race": "racing",
    "race_dm": "racing_dm",
    "time_trial": "single_player",
    "last_car_standing": "lcs",
    "lastcarstanding": "lcs",
    "elim": "elimination",
    "team_dm": "team",
    "teamdeathmatch": "team",
    "team-racing": "team_racing",
    "team-racing-dm": "team_racing_dm",
    "capture_the_flag": "ctf",
    "ctf_4": "ctf4",
}


def canonicalise(name: str) -> str:
    key = name.strip().lower().replace("-", "_")
    return ALIASES.get(key, key)


def resolve_gametype(name: str) -> Tuple[str, int]:
    canonical = canonicalise(name)
    number = GAMETYPES.get(canonical)
    if number is None:
        canonical = "elimination"
        number = GAMETYPES[canonical]
    return canonical, number


def iter_gametypes() -> Iterable[Tuple[str, int]]:
    for key, value in sorted(GAMETYPES.items(), key=lambda item: item[1]):
        yield key, value


def main() -> int:
    parser = argparse.ArgumentParser(description="Resolve Q3Rally gametype names")
    parser.add_argument(
        "gametype",
        nargs="?",
        help="Gametype name or alias (run with --list for available values)",
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="List all known gametypes and exit",
    )
    args = parser.parse_args()

    if args.list:
        for name, number in iter_gametypes():
            print(f"{number:2d} - {name}")
        return 0

    if not args.gametype:
        parser.error("a gametype argument is required unless --list is specified")

    name, number = resolve_gametype(args.gametype)
    print(f"{name} {number}")
    return 0


if __name__ == "__main__":  # pragma: no cover - CLI entry point
    raise SystemExit(main())

