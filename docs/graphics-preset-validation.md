# v0.8 graphics preset validation

## Automated checks

Run `python -m pytest tests/test_graphics_presets.py`.
The test exercises the production preset table and pending-cvar functions for
both renderer paths and all four quality levels, with renderer changes treated
as latched. It checks preset detection before restart, MSAA routing, reset of
previous colour overrides and shadow-caster mode, and detection of custom values.

Corrections made during this audit:

- Pending cvar names now accommodate `r_ext_texture_filter_anisotropic` in full.
- Presets reset `r_colorLut` so a previously selected warm/cold/night override
  does not silently survive a new quality preset; the map-defined look is used.
- Presets and the Sun shadows switch select model-only sun shadow casters,
  avoiding the older whole-world mode and duplicate darkening of baked shadows.
- Presets restore normal tyre-smoke density. Rear-weapon smoke remains independent
  of this option.

## Current quality ladder

| Setting | Low | Medium | High | Ultra |
|---|---|---|---|---|
| Anisotropy | Off | 4x | 8x | 16x |
| MSAA | Off | Off | 2x | 4x |
| HDR / bloom | Off | On | On | On |
| Live car reflections | Off | One face/frame, 256 | One face/frame, 256 | All faces/frame, 512 |
| Water reflections | Off | On | On | On |
| Ambient occlusion | Off | Off | On | Fine |
| Sun shadows | Off | 1024 | 1024 | 2048, finer filter |
| Shadow-casting lamps | 0 | 1 | 2 | 4 |
| Shadow-casting headlights | 0 | 1 | 2 | 2 |
| Lamp/headlight shadow resolution | 256 / 512 | 512 / 1024 | 512 / 1024 | 1024 / 2048 |

GL2-only effects do not run in OpenGL1. FXAA is currently enabled in every
preset, including alongside MSAA on High/Ultra. This combination should be
compared visually before changing its defaults. Medium and High intentionally
share some settings; a preset label does not imply that every row uses the
identically named sub-option.

## In-game matrix (visual smoke tests passed; performance measurements pending)

Use one fixed recording per scene, the same resolution and the same renderer.
Compare each quality level after Apply/restart. Record hardware, renderer,
resolution, demo filename, average FPS and visible defects. Repeat the same
recording to distinguish asset-loading stalls from sustained rendering cost.

| Scene | Purpose | Status |
|---|---|---|
| q3r_country01, sunlit road and trees | Car/model sun shadows; avoid double-darkened baked shade | All four presets working, user-confirmed |
| q3r_valley, water pool | Water reflections, car reflections and transitions | All four presets working, user-confirmed |
| q3r_nightcity, several cars with lights on | Lamp and headlight shadows; light selection/pop-in | All four presets working, user-confirmed |
| q3r_ridgepass, snow and multiple cars | Weather, road effects, particles and rear-weapon smoke | All four presets working, user-confirmed |

Also check rear-view mirror and minimap against the main view, restart/preset
persistence, and an OpenGL1 fallback pass. Do not disable gameplay smoke or hide
important race information to improve Low performance.

No measured performance claims or final performance-based retuning have been
made yet. This document separates source-level correctness from the remaining
visual/performance checks.

## User validation and Low regression

The user confirmed Medium, High and Ultra work on all four matrix maps.
Low produced a black image and "Tried to blit from a NULL FBO" on those maps.
Cause: disabling HDR and MSAA prevented creation of the scene FBO, while
postprocessing/FXAA still expected it. The renderer now creates an RGBA8 scene
FBO for LDR postprocessing, and skips postprocessing safely if no scene FBO is
available. `tests/test_ldr_framebuffer.py` covers Low/LDR, HDR, MSAA and the
missing-FBO fallback. The user verified the Low fix on all four matrix maps. Low, Medium, High and Ultra now work on those maps.
No frame-time/FPS comparison has been recorded yet.
