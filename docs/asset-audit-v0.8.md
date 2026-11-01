# Initial v0.8 asset audit

Checked on 2026-11-01 against the current development tree and release policy.
The inventory includes loose assets and the existing PK3, with VFS overrides.

- Selected assets: 5344
- Blocking asset findings: 13
- Additional warnings: 156
- Windows preflight also rejects the current v0.7d client as a v0.8 build.

| Referencing file | Missing/invalid asset | Kind |
|---|---|---|
| `maps/q3r_babel.bsp` | `music/haze.ogg` | sound |
| `maps/q3r_lavafalls.bsp` | `textures/common/WTF` | material |
| `maps/q3r_lavafalls.bsp` | `pak0.pk3` | sound |
| `models/mapobjects/roadblock/roadblock02.md3` | `invalid MD3 header` | format |
| `models/mapobjects/streetlights/streetlight01.md3` | `invalid MD3 header` | format |
| `models/players/heads/harpy.skin` | `models/players/heads/quake3/harpy_f.tga` | material |
| `scripts/common.shader` | `textures/common/invisible.tga` | image |
| `scripts/common.shader` | `textures/common/mirror1.tga` | image |
| `scripts/common.shader` | `textures/common/mirrortut.tga` | image |
| `scripts/common.shader` | `textures/common/portal.tga` | image |
| `scripts/common.shader` | `textures/common/q3r_checkpoint_a.tga` | image |
| `scripts/common.shader` | `textures/common/q3r_checkpoint_ainv.tga` | image |
| `scripts/common.shader` | `textures/sfx/mirror.tga` | image |

The two invalid MD3 files contain HTML documents rather than model data. They
must be replaced with valid intended models or deliberately removed together
with their references; the packager does not fabricate substitute assets.

Resolve missing references by restoring the intended asset, correcting a typo,
or removing an obsolete definition after verifying it is unused. Map entity
references such as music must be corrected in map sources and rebuilt where
necessary. Do not simply suppress all audit failures.

Warnings need review but are not equivalent to missing runtime assets: embedded
MD3 materials can be overridden by skins, editor images are not runtime inputs,
and compiled misc_model source paths can remain in BSP entity text.

Recreate the full machine-readable report with:

```powershell
python make_release.py --audit-only --report dist/release-asset-audit.json
```

This inventory records findings; asset cleanup is still open. The selected
content and counts can change as maps and assets are added.

## Follow-up investigation (2026-11-01)

The static audit is deliberately conservative: a missing image in an unused
shader still blocks packaging. The 13 asset findings break down as follows.

| Finding | Verified usage and recommended resolution |
|---|---|
| Babel `music/haze.ogg` | Referenced by both source-map and compiled BSP worldspawn. No matching asset exists in the release VFS, including PK3 contents. Restore the intended track or explicitly change/remove the music setting and update the BSP. |
| Lavafalls `textures/common/WTF` | Seven compiled draw surfaces use this missing material. Source brushes also use it in Lavafalls, Demobowl and Nightcity, but only Lavafalls has drawn BSP surfaces using it. Determine the intended material for these faces before replacing it; blindly converting every source brush to caulk could hide intended geometry. |
| Lavafalls sound `pak0.pk3` | A `target_speaker` at `-1768 2168 -200` has this invalid `noise` value in both map and BSP. An archive is not a sound. The entity has no targetname, wait or enabled loop flag, so it appears inert, although its sound is registered at spawn. Remove the orphan speaker or assign the intended sound and activation behavior; update the BSP too. |
| Harpy `harpy_f.tga` | The selectable Harpy head skin maps `h_feathers` to this missing image. That MD3 surface has 23 vertices and 26 triangles, so this is a real model dependency. The other skin reference, `harpy_h.tga`, resolves through the existing `heads.shader` alias to Hunter's head texture and is not missing. Restore the feather texture or deliberately remap this surface. |
| Seven images in `common.shader` | Used by shader blocks `mirrortut`, `mirror1`, `mirror2`, `invisible`, `portal`, `timportal` and `q3r_checkpoint_a`. No uses were found in selected BSP draw surfaces, current map sources, release skin/menu/config/script/arena files, or direct engine-code references. These are unused definitions in the checked content, rather than demonstrated current map rendering failures. Remove obsolete blocks if they are no longer supported, or restore their images if retaining them for future/custom maps. |
| Two invalid MD3 files | Both contain HTML. No references were found in checked code, maps/BSP entities, scripts or packaged content. They can be excluded/removed as unused corrupt leftovers; there is no evidence that they need replacement for current maps. |

Head choices are discovered from `.skin` files in
`engine/code/q3_ui/ui_playermodel.c`, so Harpy does not need a hardcoded name
reference in code to be reachable. Speaker registration and activation were
checked in `engine/code/game/g_target.c`.

These checks establish static usage in the current tree, not a runtime test of
every map or compatibility with external maps. No assets or map files were
changed during this investigation. The audit remains strict until cleanup is
performed. Separately, release preflight still requires a client built with
the v0.8 version marker; the existing v0.7d executable is not a v0.8 candidate.

## Original images restored (2026-11-01)

Copied six original assets from `Q3R_MappingKit.rar` into `baseq3r`:
`textures/common/invisible.tga`, `textures/common/mirror1.tga`,
`textures/common/mirrortut.tga`, `textures/common/q3r_checkpoint_a.jpg`,
`textures/common/q3r_checkpoint_ainv.jpg`, and `textures/sfx/mirror.jpg`.
The image resolver accepts the original JPG alternatives for TGA references.

Re-running `python make_release.py --audit-only --report
dist/release-asset-audit.json` now finds 5350 assets, seven asset errors and
153 warnings. All six corresponding missing-image errors are resolved.
The remaining common-shader image error is `textures/common/portal.tga`.
This audit-only run does not validate a platform executable's version.

## Lavafalls repaired (2026-11-01)

Removed the orphan `target_speaker` whose noise was `pak0.pk3` from the map
source and BSP. Replaced all 2231 source `common/WTF` faces: four exposed trim
faces use `base_trim/yellow_rustbx128`, the lamp housing uses its adjacent
`base_support/pip03_2`, two rock faces use adjacent `stone/pjrock1`, and the
remaining 2224 non-drawn brush faces use `common/caulk`. Four trim surfaces
have matching coplanar geometry with that material in the old
`q3rally_v12.rar` Lavafalls BSP; the lamp and rock choices are inferred from
their source brushes, rather than recovered historical materials.

The current BSP was patched directly, not fully recompiled. Its seven drawn
WTF surfaces now reference those materials with planar UVs at a 128-unit
repeat. The former WTF shader slot now uses the existing caulk entry for
remaining brush-side references. Geometry, lightmap coordinates, normals,
vertex colors, lightmaps, collision planes, indices and PVS were verified
unchanged. The AAS header checksum was updated to match the BSP; navigation
data was retained. Pre-repair files, the repair script and structural check
results are in `output/lavafalls-fix/`.

The release asset audit now reports five errors and 153 warnings; both
Lavafalls errors are resolved. Visual appearance still needs an in-game check,
especially the inferred lamp/rock materials and the new texture coordinates.

## Cloud recovery and cleanup (2026-11-01)

User verified Lavafalls in-game. A search of `q3rallycloud` covered 3035 files
and 697 top-level archives, including nested packages. One encrypted nested
archive (`Gold Cup/team.zip` -> `gcb0.1.zip`) could not be searched. Detailed
results and follow-up checks are in `output/cloud-asset-search/results.json`.

Restored `music/haze.ogg` from
`release/standalone/Nightlies/q3rally_git_490.zip` ->
`q3rally_git_490/baseq3r/pak0.pk3` -> `music/haze.ogg`.
Eleven copies in old releases share the same SHA256; ffprobe confirmed
156.226372 seconds of stereo 44100 Hz Vorbis audio.

Added the missing `textures/common/portal.tga` from
`sourcecodes/mfq3-to-mos-latest.7z` -> `mfq3/pak0replaced.pk3` ->
`textures/common/portal.tga`. This is a successfully decoded 64x64 RGBA TGA
from another Quake 3 project, not a verified historical Q3Rally original.

Removed the two unused HTML files masquerading as
`models/mapobjects/roadblock/roadblock02.md3` and
`models/mapobjects/streetlights/streetlight01.md3`, as requested. The cloud
search found 30 copies of each model filename, all with invalid HTML content.

The asset audit now reports **5350 assets, one error and 153 warnings**.
Only `models/players/heads/quake3/harpy_f.tga` remains missing. The separate
v0.8 executable version requirement is unchanged.

## Harpy fallback resolved (2026-11-01)

Changed Harpy's `h_feathers` skin reference from the unavailable `harpy_f.tga`
to the existing `hunter_f.tga` material. Hunter and Harpy both have 23 vertices
and 26 triangles in this surface, with byte-identical UV coordinates. Harpy's
head shader already uses Hunter's head image. Reusing the existing Hunter
feather shader also preserves alpha testing, two-sided rendering and feather
deformation. This is an explicit fallback, not a recovered Harpy original;
its appearance still needs an in-game check.

The release asset audit now passes: **5350 assets, zero errors, 153 warnings**.
Warnings and platform release checks remain separate from these resolved
missing/invalid asset errors.
