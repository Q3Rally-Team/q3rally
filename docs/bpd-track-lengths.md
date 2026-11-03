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

## Checkpoint placement

Between two consecutive checkpoints the route is **one cubic Bezier segment**.
A cubic follows one bend well. It cannot follow two bends in a row, or an S-bend
together with a corner, so the route, the bots and the race intro drift off the
road there. Place checkpoints so that every segment contains at most one bend:

- **One checkpoint on the straight before every curve** (from about 45 degrees).
  The anchor sits on the straight, the handles point along it.
- **Hairpins and loops from about 120 degrees**: add a checkpoint at the apex.
- **S-bends**: one checkpoint between the two bends.
- The trigger must span the **full drivable width**, wall to wall, including
  side passages around islands; otherwise a car can miss it.
- Keep consecutive checkpoints at least **~850 units** apart (about 24 m).
  Touches within 300 ms of the previous checkpoint are ignored.
- On tracks with several levels, limit the trigger height so it cannot be
  touched from a road above or below; touching a later checkpoint early shows
  "CHECKPOINT MISSED".
- Intermediate checkpoints should use **spawnflag 4** (silent); the default
  plays the checkpoint sound on every gate.
- Where lengths take different roads, give **each length its own checkpoint**
  (`onlyTrackLength`) and keep the numbers 1..N per length. A checkpoint shared
  by several lengths needs the same number in all of them.

New checkpoints are brush entities: the map needs a full BSP compile
(`-onlyents` is not enough). Triggers are not solid, so the `.aas` should not need rebuilding.

## Maps with track lengths (v0.8)

| Map | Lengths | Checkpoints |
|---|---|---|
| Canyon Cruise (`q3r_sk1`) | short, medium | 17 per length. 1-15 and 17 shared; 16 is per length (inner straight for short, outer loop for medium). |
| Night City (`q3r_nightcity`) | short (12), medium (16) | 1-7 shared, the rest per length. One checkpoint before every curve. |
| Dirty Fun (`q3r_dirtyfun`) | short, medium, long | Only 1 shared, 2-4 per length, long adds 5-8. |

All three need `_tl<n>.bpd` files, one per enabled length.
