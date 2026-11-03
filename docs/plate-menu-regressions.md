# License plates, bot difficulty and Canyon Cruise

These corrections form the v0.8.2 candidate. The profile format is unchanged.

## License plates

Generated plate images contain the driver name, but pure-server filesystem
filtering rejected these loose TGA files. An implicit 2D shader also expects
vertex colors that MD3 models do not supply. Both renderers now select normal
model lighting/depth behavior for strictly recognized generated plate paths.
The pure exception is limited to player slots 0–63, an optional eight-digit
hexadecimal identity suffix and a TGA extension. Other loose images remain blocked.

The image identity incorporates driver name and plate skin, so renames and slot
reuse do not retain cached text. Sharing a car model regenerates the recipient's
personal plate instead of copying another driver's plate handle. Missing/invalid
plate images use the packaged default material.

## Custom spin controls

The Add Bots menu marks its skill/team controls with `QMF_NODEFAULTINIT` to retain
custom hit boxes. That also skipped the choice count, so the initial value and
zero were effectively the only reachable skill values. Custom spin controls now
count their null-terminated choice lists independently of default layout setup.
Five skill values and team choices remain accessible with mouse/keyboard input.

## Canyon Cruise

`q3r_sk1.arena` advertised only Medium (`1`), while the shipped BSP permits Short
and Medium and disables Long. The BSP contains two Short-only barrier models,
one Medium-only barrier model and numbered checkpoints 1–16 common to both.
The arena now advertises `0 1`. Night City already advertises `0 1`; the user
retested its selection successfully and no Night City changes are made.

The native server checks both Canyon variants against the shipped BSP: each has
17 checkpoint entities including start/finish zero, and the expected variant
barriers. This validates map setup; driving a complete lap remains a separate
in-game check.

Local Windows client checks pass with both OpenGL renderers and `sv_pure=1`:
the generated plate shaders load without defaulting, a live driver rename
registers a new texture identity and changes the generated image pixels, and
the client exits normally after capturing a screenshot. Complete visual plate
legibility in a normal race remains part of the user's in-game verification.

Regression coverage: `tests/test_plate_and_menu_regressions.py` compiles real C
functions for custom spin initialization/key handling, UI length mapping,
per-driver plate registration and cache-safe filenames. It also checks the BSP
and arena length agreement. Local native runtime evidence is retained under
`output/race-bug-runtime/` and build logs under `output/repository-cleanup/`.
