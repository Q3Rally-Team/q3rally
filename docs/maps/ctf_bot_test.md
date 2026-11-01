# CTF Bot Test Box

This map isolates CTF objective selection and flag pickup from arena geometry.
The sealed room has a flat floor and no cover, pickups, ramps, or obstacles. Its interior is 5120 x 3200 units, with the flags 760 units from the end walls.

- Red and blue flags sit at opposite ends of the room, at `x = 1800` and `x = -1800`.
- Each team has four `team_CTF_*player` match-start positions and four
  `team_CTF_*spawn` respawn positions.
- The generated AAS contains the room as one open driving area, so route tests
  do not depend on narrow passages or map-specific path corridors.

The checked-in `.map` is the editable source. The `.bsp` and `.aas` beside it
are compiled test assets. Rebuild all three with:

```powershell
.\tools\maptools\build_ctf_bot_test.ps1
```

If Q3RallyRadiant is installed elsewhere, pass `-Q3Map2 <path>` and
`-Bspc <path>`. The matching arena metadata is in
`baseq3r/scripts/q3r_ctf_bot_test.arena`.

For a first gameplay run, use a small CTF match with bots on both teams and
check that at least one bot on each team receives an attacker role. The console
log reports the selected flag goal and its coordinates alongside movement and
vehicle-clearance data.
