# Profile top-speed measurement

The lifetime record now uses the same absolute longitudinal velocity as the
HUD (velocity projected onto the view-angle forward vector), converted to km/h.
154 mph is approximately 247.8 km/h. Sideways and vertical motion perpendicular
to the car no longer inflate the record; slopes and reversing still work.

A record must be sustained over a continuous 250 ms window. The minimum reading
in that window is eligible, rather than a one-frame collision peak. Spawn,
spectator/dead states, invalid readings, backwards time and sampling gaps above
250 ms interrupt confirmation. The existing 400 km/h plausibility limit remains.

This is a game-module source change, not a website deployment. Rebuild and ship
the game modules for the platforms in use. A full Linux native-module build and
`tests/test_profile_speed.py` were verified. A Windows build and in-game driving
check are still required before release.

Previously saved records cannot be reconstructed from the old maximum. They are
intentionally preserved. To correct a known bad historical record, first back up
both the local profile and Ladder player data, then reset only `topSpeedKph` in
both while the game is closed, after installing the corrected build. Resetting
only one copy is insufficient because synchronisation keeps the larger value.
Do not replace it with a guessed speed or reset unrelated career statistics.
