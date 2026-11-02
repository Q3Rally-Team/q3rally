# Dedicated-server validation

Latest result (2026-11-02): targeted native macOS checks pass on Intel/macOS 15
and Apple Silicon/macOS 26 after the interpreted float-cast and legacy Bezier
recovery corrections. Both lap and Sprint runs have two distinct finishers,
valid telemetry, local Ladder HTTP and clean server shutdown. The final run is
https://github.com/P3rlE/q3rally_tryout/actions/runs/41447569570. The user also
confirmed successful network tests and long matches. Final release-candidate
and desktop-client validation remain open.

The initial Windows x86_64 development matrix below used the actual
dedicated executable, packaged QVMs/assets, a separate home directory and
loopback-only networking. Ladder submission was disabled. This is not a final
v0.8 release package. Subsequent native Linux/macOS results follow below.

## Runtime matrix

| Check | Evidence/status |
|---|---|
| Configured bots start without humans | Chloe and Janine connected from the config; race entered COUNTDOWN at 8 seconds and RUNNING at 14 seconds |
| Automatic fill | bot_minplayers 3 produced three bots; waiting bots now count, preventing further fill |
| Slot limit | Four explicit bots fit sv_maxclients 4; a fifth addbot was rejected |
| Reduce population | Lowering to two removed waiting bots and kept the original active drivers |
| Human joins/leaves | DedicatedHumanProbe connected alongside two bots, disconnected and exited normally; server retained the two bots |
| Country01 checkpoints and finish | Telemetry reached nextCheckpoint 20 for both original bots; Chloe won and Janine finished second |
| Natural rotation | Results/intermission led from Country01 to Riviera with g_gametype 7, without a human or manual map command |
| Sprint restart and progress | Riviera map_restart restarted the countdown; BPD guidance loaded 14/14 handles and a 483-waypoint route |
| Waiting bot after restart | Jennifer, added during a race, drove and passed checkpoints after restart |
| Intentional spectator after restart | Paul remained TEAM_SPECTATOR and had no driving telemetry |
| Sprint repeated finish contacts | After the guard fix, three active bots generated exactly three finish announcements, then Race finished; no repeated awards |
| Final finish-placement rerun | Chloe first, Janine second, Jennifer third; exactly three announcements, still three five seconds later; normal server exit |
| RCON shutdown | Probes ended the server with quit and observed exit status 0 |

Country01 exercised checkpoint-based route fallback without a BPD file;
Riviera exercised a BPD route. Both loaded their AAS files. Tests kept normal
timescale and skill 3 for explicitly configured bots. Intro playback was
disabled to isolate racing/server behavior. The population/rotation run used
a five-second finish grace period; the final placement check uses the default
30-second grace period. These are bounded behavioral checks, not lap-time or
AI driving-quality benchmarks.

## Corrections from these tests

- Count waiting spectator bots toward non-team bot_minplayers. Previously a
  late bot did not count and repeated fill attempts consumed the reserved slot.
- When reducing non-team population, remove spectator bots before active racers.
- Persist spectatorWilling as a ninth session field. On restart/map load, only
  involuntarily waiting non-team racing bots rejoin TEAM_FREE. Intentional
  spectators remain spectators; legacy eight-field sessions are treated
  conservatively as intentional spectators. Human and team-game autojoin
  behavior is unchanged.
- Ignore repeated Sprint finish touches once finishRaceTime is set, preventing
  repeated finish notifications, lap/profile awards and ghost-finish hooks.
- Recalculate player positions immediately after recording finishRaceTime,
  before announcing/recording placement. The old intermediate position could
  otherwise announce two drivers as third.
- Fix the optional Sprint branch in server_racing_modes_rotation.cfg: mode 7
  instead of single-player mode 2, and Riviera instead of an excluded test map.

## Evidence and regression checks

`output/dedicated-validation/fixed/` contains the population, mixed-session,
Country01 completion and natural rotation logs. This run exposed the finish
bugs, so its Sprint messages are historical evidence, not the final result.
`final-sprint/` confirms queued/intentional spectator handling and unique
finish events before the placement correction. `placement-check/` is the
last runtime check with the corrected placement calculation.

Each run includes its config, stdout, game log, status samples and JSON driving
telemetry under the isolated home directory. Race progress is assessed from
telemetry and finish events, not just bot presence in getstatus.

Production-code C harnesses pass:

```powershell
$env:PATH = 'C:\msys64\mingw64\bin;' + $env:PATH
python -m unittest tests.test_bot_minplayers tests.test_queued_race_bots tests.test_sprint_finish -v
```

Native game DLLs and QVMs build. The broader packaging/UI/filesystem/Ladder
regression set also passes (14 tests total).

## Extended Windows and native Linux checks (2026-11-02)

WSL 2 / Ubuntu 24.04.4 built and ran the native x86_64 Linux dedicated
executable and its QVMs. This tests Linux compilation, filesystem behavior and
server execution; it does not replace desktop Linux graphics/audio testing or
public-network discovery testing.

Windows and Linux mode probes passed for default Sprint intro transitions,
team racing, team racing DM, team DM, CTF, four-team CTF, Domination, KOTH,
Autoball, Derby, LCS, Elimination, racing DM and Ghost. Probes check startup,
continued operation, bot counts, team assignments and clean RCON shutdown.
Ghost correctly rejects bots. These short runs do not establish that bots
complete every objective mode.

Automatic four-team CTF population initially failed on both platforms:
G_CheckMinimumPlayers only considered red/blue, and G_AddRandomBot did not
translate green/yellow. Both paths now support all four teams. Independent
fixed-build reruns filled red, blue, green and yellow successfully.

Linux also completed Country01 lap racing and Riviera Sprint with two bots,
exactly one first and second placement each, and clean shutdown. Both runs
exercised actual server-side libcurl HTTP through a loopback Ladder fixture;
the expected fixture error was parsed correctly. No production registration
or match submission was made. This is HTTP coverage, not a Linux TLS test.

Evidence is in output/dedicated-validation/: windows-modes/ and linux-modes/
retain the initial CTF4 failure; windows-ctf4-fixed/ and linux-ctf4-fixed/
supersede that case. linux-complete-races/ contains finish and HTTP evidence.
The additional Linux team-racing-DM probe is linux-team-racing-dm-final/.

tools/prepare_dedicated_test_data.py builds isolated classic-PK3-only data;
tools/validate_dedicated_server.py runs portable mode checks. Use a new output
directory per invocation. Optional --complete-races requires --case racing
and/or --case sprint; --http-probe uses only a local HTTP fixture. The macOS
workflow now runs native mode and HTTP checks on Intel and Apple Silicon and
uploads their logs, followed by completed lap/Sprint race checks. The native
macOS run is https://github.com/P3rlE/q3rally_tryout/actions/runs/41440521760,
testing commit 5c20efa825387a37b68b029662be098858396404 on the isolated
codex/macos-dedicated-validation branch. Both native builds and all 17 mode
probes (including local Ladder HTTP) passed. Apple Silicon Sprint completed
with two distinct finish placements. Its Country01 lap probe failed the
two-finisher assertion: Chloe won, then the normal 30-second finish grace
expired before Janine finished. The server logged Race finished and shut down
cleanly. This is not a server crash, but two-bot completion is unproven on ARM.
ARM driving JSON also contains malformed/truncated records and cannot be used
as reliable checkpoint evidence; the cause remains unconfirmed.

GitHub's original fail-fast matrix cancelled the Intel completion step after
the ARM assertion failed. An independent headless race workflow with fail-fast
disabled now runs at
https://github.com/P3rlE/q3rally_tryout/actions/runs/41443131140 (commit
38208877758bdb467d3c7e979a96b01b2fca2218). Intel finished both cases:
Sprint passed with two distinct placements; Country01 again produced only
Chloe's winning finish before the grace period ended. All 20 checkpoints
appear in Chloe's driving telemetry; Janine reached checkpoint 12 but did
not finish. Intel race telemetry contains no malformed records. This confirms
that the Country01 two-finisher failure is not specific to ARM. Whether it
is AI driving/recovery behavior or platform-specific simulation remains open;
do not weaken the assertion to mark it green.

The redundant ARM retry remained queued and was cancelled after Intel's
reports were uploaded. ARM results come from the original completed job,
Intel completion results from the independent follow-up. Both runs therefore
contain failures/cancellations, not an all-green macOS certification. Logs
are also saved locally under output/dedicated-validation/macos-arm64/,
macos-intel/ and macos-intel-races/. Runtime used macOS 15 runners; older
macOS compatibility and interactive client graphics/audio are untested.

## Targeted correction and retest

The interpreted VM's OP_CVFI used Q_ftol. On ARM this maps to lrintf (rounding
to nearest), whereas a C float-to-int conversion must truncate toward zero.
The VM printf formatter relies on truncation when separating integer/fractional
parts; the old conversion could produce negative fractional digit indices and
embedded NULs. OP_CVFI now uses an explicit C int cast. The production-operation
regression tests positive/negative fractions in four host rounding modes.

Country01 uses the legacy checkpoint Bezier follower without BPD data. Its
fallback previously cleared recovery state every frame and had no leg-progress
watchdog, allowing a moving car to circle indefinitely at the same checkpoint.
The fallback now tracks fraction advancement, checkpoint and lap. Eight seconds
without leg advancement trigger a bounded recovery attempt behind observed
progress on the same unpassed leg. Ground, slope, occupancy, headroom and
distance checks must succeed; required checkpoint and lap remain unchanged.
The existing route-reset commit logic is shared with the BPD follower.
Regression tests cover progress/restart/lap transitions and unsafe recovery spots.

The runtime tool now supports explicit interpreter mode and validates every
driving JSON record. Windows interpreter lap and Sprint runs passed with two
distinct finishes each, local HTTP success and zero malformed telemetry lines
(1540 lap / 1180 Sprint records). All 17 local regressions pass.

Native macOS retest: https://github.com/P3rlE/q3rally_tryout/actions/runs/41445981218,
commit 7f7517537b59b89657a1768837247588318e18c8. This repeats lap/Sprint completion
and strict telemetry checks on Intel/ARM, plus an explicit interpreter probe.
The macOS-15 ARM runner remains queued. A second ARM image is testing the same
code on macOS 26 at
https://github.com/P3rlE/q3rally_tryout/actions/runs/41446477573 (commit
d61dfb45184408bc1641050c86fd43c56f4db01d; only the additional workflow differs).
These intermediate results are superseded by the final retest below.

The macOS-26 ARM correction run completed successfully: lap racing and Sprint
both had two distinct finishers; all 1577 lap and 1189 Sprint telemetry records
were valid, and the explicit interpreted-VM telemetry probe also passed.
Intel's first corrected run still rejected recovery spots. BSP inspection
identified the reason: a roof/playerclip brush above the Country01 bridge
starts at z=256; the old recovery sweep started at route z=120 + 128, so its
18-unit upper half-extent overlapped that brush. The sweep now starts 64 units
above the route, with the independent ground/clearance/occupancy checks intact.
A regression reproduces this ceiling and rejects the original high probe.

Final native retest on Intel/macOS 15 and ARM/macOS 26:
https://github.com/P3rlE/q3rally_tryout/actions/runs/41447569570, commit
9005a3e9428b5c530a59c7f536904d2c6c1df523. Both jobs passed, including the explicit
interpreter telemetry probe. Intel recorded 1509 valid lap / 1258 Sprint /
102 interpreter-probe records; ARM recorded 1518 / 1253 / 99. Neither had a
malformed record. Intel's lap run includes one successful bezier_route_reset
at checkpoint 12 (64.9 seconds), with checkpoint 12 and lap 1 unchanged;
both bots subsequently finished. ARM also finished both races with two unique
placements. Final evidence is saved in macos-final-intel/ and macos-final-arm/
under output/dedicated-validation/. The queued macOS-15 ARM retry was cancelled;
the successful ARM retest used macOS 26, so older ARM OS compatibility is not
established by this run. Final Windows
native game modules/QVMs/server build successfully. Earlier Windows interpreter
evidence is recovery-win-interpreter-network/ (the sandbox-only initial run had
blocked loopback communication); recovery-win-jit/ confirms two lap finishers
and valid telemetry with the normal JIT.

## User-confirmed manual checks

The user confirmed that network tests with a second local computer and long
matches are green. These results complement the automated native server
checks above. Platform/mode/duration details and public-discovery logs were
not supplied; the confirmation records the user's tested scenarios.

## Remaining release validation

- Broader map/vehicle stress coverage for the corrected legacy Bezier recovery.
- Linux/macOS desktop client, graphics/audio and remaining TLS integration checks.
- Final native build versions and validation of untouched release candidates.

The dedicated functional checks, including the user-confirmed network and long
match tests, pass for the exercised scenarios. Client and final-package checks
remain separate release tasks; broader stress coverage is an additional check.
