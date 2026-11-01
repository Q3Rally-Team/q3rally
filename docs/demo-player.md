# Demo player

The demo browser plays recordings inside its preview panel. Select a demo and
choose Play demo. Pause/Resume (or Space) freezes playback; Restart reloads the
current recording; Stop returns to the empty preview. Back/Escape stops playback
and leaves the browser. The console `demo` command retains full-screen playback.

The preview uses the normal demo parser and cgame to load the recorded map and
render its scene, with game HUD, minimap and rear-view mirror suppressed. It does
not start a new race/server. Map assets still need to load. The engine restores
the browser after UI VM recreation during map loading. The preview viewport uses
the same 640x480 coordinate transform as the menu.

Engine commands: `demopreview <file>` and `demopreviewstop`. Engine-owned
`cl_demoPlayer` tells cgame and UI when preview playback is active.
Both the engine and updated ui/cgame QVMs must be installed together.

The -10 sec/+10 sec buttons and Left/Right arrow keys seek relative to the
current position. Both directions rebuild from the original gamestate and feed
every snapshot through cgame. Work is split into short batches so the UI stays
responsive. Pause is restored at the destination. A target beyond EOF ends
playback. Long recordings can take time to reconstruct; there is no index cache
or duration timeline yet.

Regression coverage: `python -m pytest tests/test_demo_io.py` exercises actual
production functions with filesystem stubs for malformed lengths, EOF, valid
messages, protocol fallback and write failure cleanup.

The idle preview shows the selected filename, file size and protocol extension.
Preview controls use their full painted rectangles as mouse targets. Recording
status is a compact bottom-left label. Stopping a recording via `stoprecord`
opens a naming menu: Save renames it without overwriting an existing demo;
Keep name or Escape retains the original file. Internal disconnect/shutdown
paths close recordings without opening this menu.

The preview header shows elapsed playback time (MM:SS, or H:MM:SS for long
recordings). The engine publishes cl_demoTime in whole seconds from the current
rendered demo timestamp relative to the first snapshot. Pause freezes the clock;
restart/stop reset it; seeking updates it to the reconstructed position. This is
elapsed time only, not total recording duration.
