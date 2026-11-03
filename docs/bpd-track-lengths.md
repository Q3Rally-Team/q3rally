# BPD files and track lengths

The `.bpd` files in `baseq3r/maps/` are the authoritative Bezier route of a race
track. Bots, the BPD race route and the race intro camera all read them. Mappers
refine them in Q3RallyRadiant (RallyRoute plugin) and ship them with the game.

## Track lengths

A race map can offer up to three lengths, selected by `g_trackLength`
(0 short, 1 medium, 2 long):

- worldspawn `allowShortTrack`, `allowMediumTrack`, `allowLongTrack` (default 1)
  enable the lengths.
- `onlyTrackLength` on any entity (`0`, `1`, `2` or combinations such as `12`;
  default `012`) spawns it only for those lengths. Barriers that differ between
  lengths must be entities (for example `func_static`), not world brushes.
- Checkpoint `number`s must run 1..N without gaps for every length.

## Which BPD file is used

| File | Used when |
|---|---|
| `maps/<map>_tl<n>.bpd` | It exists for the active length `n`. Always wins. |
| `maps/<map>.bpd` | No file for the active length exists and every `rally_checkpoint` is shared by all lengths. |
| none | Otherwise. The race uses the plain checkpoint route and has no intro. |

A map has length variants as soon as one `rally_checkpoint` carries an
`onlyTrackLength` that excludes a length. On such a map the generic file is
ignored even if it exists, because its records would be applied to the wrong
checkpoints; the server prints `[BPD] ... ignored: this map has track length
variants`.

Records in a BPD are keyed by the authored checkpoint `number`. Reversed races
use the same file, so one file per length covers both directions.

The rule lives in `engine/code/game/bg_bpd_path.h` and is shared by the server
loader (`g_spawn.c`), the intro route (`g_rally_observer_cams.c`) and the
client intro camera (`cg_rally_intro_cam.c`).

## Writing files in game

`saveBPoints` writes the BPD the server loaded for the current track (the path
announced for the intro). If none was loaded, it writes
`maps/<map>_tl<n>.bpd` for the active length. Run it on the forward direction
only: checkpoint numbers are renumbered for reversed races.

## Maps with track lengths (v0.8)

| Map | Lengths | Checkpoints |
|---|---|---|
| Canyon Cruise (`q3r_sk1`) | short, medium | All 16 shared; the lengths differ only by barriers between checkpoints 15 and 16. |
| Night City (`q3r_nightcity`) | short, medium | 1-3 shared, 4-8 per length, medium adds 9-11. |
| Dirty Fun (`q3r_dirtyfun`) | short, medium, long | Only 1 shared, 2-4 per length, long adds 5-8. |

Night City and Dirty Fun therefore need `_tl<n>.bpd` files. Canyon Cruise can
use one generic file, but both lengths then follow the same curve between
checkpoints 15 and 16; separate curves need a length-specific checkpoint there.
