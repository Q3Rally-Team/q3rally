#!/usr/bin/env python3
"""Balance report for Autoball matches from a server games.log.

    python3 tools/autoball/analyze_log.py games.log [more.log ...]

The server writes the events on its own (g_log must be on, it is by
default). For ball speed, height, territory, turbo stock and car speed
also set g_autoballStats 1 on the server.

Lines used:
  InitGame / ShutdownGame / Exit           match boundaries, mutator cvars
  ClientUserinfoChanged                    player names
  AutoballKickoff / AutoballLive           start of every play
  AutoballGoal  team scorer owngoal kmh
  AutoballTouch client team carKmh ballKmh
  AutoballShot / AutoballSave / AutoballAssist  client team
  AutoballDemo  rammer victim kmh
  AutoballSample ballKmh height x y redTurbo blueTurbo redKmh blueKmh
  AutoballRescue ball reason              ball put back (left the arena)
The hints at the end are rules of thumb, not truths: compare several
matches and change one cvar at a time.
"""
import re
import statistics
import sys

TEAM = {1: "red", 2: "blue"}
LINE = re.compile(r"^\s*(\d+):(\d)(\d)\s+(\w+):\s?(.*)$")


def new_match(info):
    return {"info": info, "names": {}, "plays": [], "goals": [], "touches": [], "shots": [],
            "saves": [], "assists": [], "demos": [], "samples": [], "rescues": [], "live_since": None,
            "live_time": 0.0, "end": None}


def parse(paths):
    matches = []
    cur = None
    for path in paths:
        with open(path, encoding="utf-8", errors="replace") as f:
            for raw in f:
                m = LINE.match(raw.rstrip("\n"))
                if not m:
                    continue
                t = int(m.group(1)) * 60 + int(m.group(2)) * 10 + int(m.group(3))
                kind, rest = m.group(4), m.group(5)
                if kind == "InitGame":
                    info = dict(zip(*[iter(rest.strip("\\").split("\\"))] * 2))
                    if info.get("g_gametype") != "23":
                        cur = None
                        continue
                    cur = new_match(info)
                    matches.append(cur)
                    continue
                if cur is None:
                    continue
                if kind in ("ShutdownGame", "Exit"):
                    if cur["live_since"] is not None:
                        cur["live_time"] += t - cur["live_since"]
                        cur["live_since"] = None
                    cur["end"] = t
                    if kind == "ShutdownGame":
                        cur = None
                    continue
                parts = [p.rstrip(":") for p in rest.split()]
                if kind == "ClientUserinfoChanged":
                    nm = re.search(r"n\\([^\\]*)", rest)
                    if nm:
                        cur["names"][int(parts[0])] = re.sub(r"\^.", "", nm.group(1))
                elif kind == "AutoballKickoff":
                    # a kick-off ends a running play (e.g. after a ball rescue)
                    if cur["live_since"] is not None:
                        cur["live_time"] += t - cur["live_since"]
                        cur["live_since"] = None
                elif kind == "AutoballLive":
                    cur["live_since"] = t
                    cur["plays"].append({"start": t, "goal": None})
                elif kind == "AutoballGoal":
                    team, scorer, own, kmh = (int(x) for x in parts[:4])
                    if cur["live_since"] is not None:
                        cur["live_time"] += t - cur["live_since"]
                        cur["live_since"] = None
                    play = cur["plays"][-1] if cur["plays"] else None
                    since = t - play["start"] if play else None
                    if play:
                        play["goal"] = t
                    cur["goals"].append({"t": t, "team": team, "scorer": scorer, "own": own,
                                         "kmh": kmh, "play": since})
                elif kind == "AutoballTouch":
                    cur["touches"].append(tuple(int(x) for x in parts[:4]))
                elif kind == "AutoballShot":
                    cur["shots"].append(tuple(int(x) for x in parts[:2]))
                elif kind == "AutoballSave":
                    cur["saves"].append(tuple(int(x) for x in parts[:2]))
                elif kind == "AutoballAssist":
                    cur["assists"].append(tuple(int(x) for x in parts[:2]))
                elif kind == "AutoballDemo":
                    cur["demos"].append(tuple(int(x) for x in parts[:3]))
                elif kind == "AutoballRescue":
                    cur["rescues"].append(" ".join(parts[1:]))
                elif kind == "AutoballSample":
                    cur["samples"].append(tuple(int(x) for x in parts[:8]))
    return matches


def fmt_time(sec):
    return f"{int(sec) // 60}:{int(sec) % 60:02d}"


def mean(values, default=0.0):
    return statistics.mean(values) if values else default


def report(match, index):
    info, names = match["info"], match["names"]
    live_min = max(match["live_time"], 1) / 60.0
    red = sum(1 for g in match["goals"] if g["team"] == 1)
    blue = sum(1 for g in match["goals"] if g["team"] == 2)
    hints = []

    print(f"=== Match {index}: {info.get('mapname', '?')}  red {red} : {blue} blue  "
          f"(live play {fmt_time(match['live_time'])})")
    mut = []
    if info.get("g_autoballBalls", "1") != "1":
        mut.append(f"{info['g_autoballBalls']} balls")
    if info.get("g_autoballBallScale", "1") not in ("1", "1.0"):
        mut.append(f"ball size {info['g_autoballBallScale']}x")
    if info.get("g_autoballBallGravity", "1") not in ("1", "1.0"):
        mut.append(f"ball gravity {info['g_autoballBallGravity']}x")
    if mut:
        print("    mutators: " + ", ".join(mut))

    for g in match["goals"]:
        who = names.get(g["scorer"], "?") if g["scorer"] >= 0 else "-"
        own = " (own goal)" if g["own"] else ""
        play = f", {g['play']} s after kick-off" if g["play"] is not None else ""
        print(f"    {fmt_time(g['t'])}  {TEAM.get(g['team'], '?'):4} {who}{own}  {g['kmh']} km/h{play}")

    goals = len(match["goals"])
    plays = [g["play"] for g in match["goals"] if g["play"] is not None]
    quick = [p for p in plays if p <= 8]
    print(f"  goals/min {goals / live_min:.2f}   avg play to goal {mean(plays):.0f} s   "
          f"kick-off goals (<=8 s) {len(quick)}")
    if goals:
        print(f"  goal speed avg {mean([g['kmh'] for g in match['goals']]):.0f} km/h, "
              f"max {max(g['kmh'] for g in match['goals'])} km/h, "
              f"own goals {sum(g['own'] for g in match['goals'])}")

    touches = match["touches"]
    by_team = {1: [t for t in touches if t[1] == 1], 2: [t for t in touches if t[1] == 2]}
    shots, saves = len(match["shots"]), len(match["saves"])
    print(f"  touches/min red {len(by_team[1]) / live_min:.1f}  blue {len(by_team[2]) / live_min:.1f}   "
          f"ball after touch avg {mean([t[3] for t in touches]):.0f} km/h   "
          f"car at touch avg {mean([t[2] for t in touches]):.0f} km/h")
    print(f"  shots {shots}  saves {saves}  assists {len(match['assists'])}  "
          f"goals per shot {goals / shots if shots else 0:.2f}")
    demos = match["demos"]
    print(f"  demolitions {len(demos)} ({len(demos) / live_min:.2f}/min), "
          f"avg ram speed {mean([d[2] for d in demos]):.0f} km/h")

    if match["rescues"]:
        print(f"  ball rescues {len(match['rescues'])}: " + ", ".join(match["rescues"]))
        hints.append("The ball had to be rescued: check the map for spots no car can reach "
                     "(autoball_reset volumes) or holes in the arena.")
    samples = match["samples"]
    if samples:
        air = sum(1 for s in samples if s[1] > 40) / len(samples)
        red_half = sum(1 for s in samples if s[2] < 0) / len(samples)
        print(f"  ball speed avg {mean([s[0] for s in samples]):.0f} km/h, airborne {air:.0%}, "
              f"in x<0 half {red_half:.0%}")
        print(f"  turbo stock avg red {mean([s[4] for s in samples]) / 1000:.1f} s, "
              f"blue {mean([s[5] for s in samples]) / 1000:.1f} s   "
              f"car speed avg red {mean([s[6] for s in samples]):.0f}, blue {mean([s[7] for s in samples]):.0f} km/h")
        if mean([s[4] for s in samples] + [s[5] for s in samples]) > 15000:
            hints.append("Turbo piles up (avg stock > 15 s): fewer/smaller turbo pickups "
                         "or a lower g_autoballStartTurbo.")
        if air > 0.35:
            hints.append("Ball is in the air a lot: lower g_autoballLift or g_autoballVerticalScale.")
    else:
        print("  (no AutoballSample lines - set g_autoballStats 1 for ball/turbo/territory data)")

    if goals and len(quick) / goals > 0.3:
        hints.append("Many kick-off goals: kick-off spots too close, or the ball flies too far "
                     "(g_autoballImpactScale).")
    if goals / live_min > 1.2:
        hints.append("Goals come very fast: smaller goals, more defence time "
                     "(g_autoballImpactScale down) or a longer g_autoballGoalDelay.")
    if live_min >= 3 and goals / live_min < 0.15:
        hints.append("Very few goals: bigger goals, stronger hits (g_autoballImpactScale up), "
                     "or more turbo.")
    if len(demos) / live_min > 1.5:
        hints.append("Lots of demolitions: raise g_autoballDemoSpeed.")
    if shots >= 5 and saves / shots > 0.7:
        hints.append("Keepers save most shots: goals may be too small for the ball size.")
    for h in hints:
        print("  hint: " + h)
    print()


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 1
    matches = parse(sys.argv[1:])
    if not matches:
        print("No Autoball matches (g_gametype 23) found.")
        return 1
    for i, m in enumerate(matches, 1):
        report(m, i)
    return 0


if __name__ == "__main__":
    sys.exit(main())
