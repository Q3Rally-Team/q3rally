Legacy packaged racing intro camera routes (unused)
===================================================

Current builds derive race-intro camera movement from `maps/<map>.bpd`.
This directory is retained for older builds only; the game no longer reads
these files. A map without a valid BPD route has no race intro.

The documentation below describes the legacy format.

The server selects one file for the active map and race variant:

  <map>_tl<track-length>_rev<reversed>.route

For example: q3r_valley_tl1_rev0.route

Routes are camera-only derivatives of a completed .ghost recording. They
contain no player name, vehicle, lap time, or control input. Each frame stores
a normalized preview time followed by origin and angles:

  Q3RALLY_INTRO_ROUTE 1
  map <map-name>
  track_length <0..2>
  track_reversed <0|1>
  frames <count>
  <time-ms> <x> <y> <z> <pitch> <yaw> <roll>

Frame times must increase from 0 to the route duration. The old server measured
the lap length from the map checkpoints, then retimed each route at 30 ms per
meter, clamped to 10-30 seconds. The `g_trackLength` variant selected a route
file; it did not determine the preview duration. The camera collision-traced
its chase position. These details apply only to the legacy format above;
current builds derive the camera from the active BPD.
Packaged route heights were lowered by 24 units from the source Ghost route.

No A-to-B route variants are included for the v0.8 release yet.
