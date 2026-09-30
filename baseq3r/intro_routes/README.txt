Packaged racing intro camera routes
===================================

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

Frame times must increase from 0 to the route duration. The server measures
the lap length from the map checkpoints, then retimes each route at 30 ms per
meter, clamped to 10-30 seconds. The `g_trackLength` variant selects a route
file; it does not determine the preview duration. The camera collision-traces
its chase position. A race intro is shown only when a matching packaged route
is available.
Packaged route heights are lowered by 24 units from the source Ghost route.

No A-to-B route variants are included for the v0.8 release yet.
