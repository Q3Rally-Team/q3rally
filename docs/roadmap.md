# Release roadmap

## v0.9 — Responsive UI

Agreed scope: replace fixed 4:3 menu layouts with a responsive layout foundation.
Keep logical UI coordinates while adapting content to the available aspect ratio.

- Unify rendering and mouse coordinate transforms across menus, HUD and engine overlays.
- Add flexible content widths, edge anchors, minimum spacing and configurable UI scale.
- Migrate the main menu and demo player first, then remaining menus, dialogs, loading screens and HUD.
- Verify 4:3, 16:9, 16:10 and ultrawide layouts, including high-resolution readability and mouse targets.

Initial estimate: 4–7 developer weeks for the full migration; approximately one
week for the shared foundation plus the main menu and demo player. Estimates
need refinement after a complete inventory of existing screens.

## v0.8 - Release priorities

1. UI consistency: review remaining legacy dialogs, fonts, hit targets and overlapping overlays. Shared bottom-left Back buttons implemented across regular menus; Config defaults moved to the right. In-game verification pending.
2. Complete the demo player: elapsed playback time and pause/resume clock correction implemented; regression tests pass and the user has verified both in-game. Seeking/EOF regression tests and browser error recovery for missing maps, unsupported protocols, damaged or truncated recordings implemented; regression tests pass and the user has verified the new behavior in-game. Demo player work complete for v0.8.
3. Race feedback: missed checkpoints, wrong direction, respawn and race completion.
4. Validate graphics presets across representative maps and effects. Preset/pending-value regressions corrected and automated GL1/GL2 checks pass. User verified all four presets on Country01, Valley, Nightcity and Ridgepass after the Low/LDR framebuffer correction. Basic visual checks pass; measured performance comparisons remain open. See graphics-preset-validation.md.
5. Verify clean release installs, settings persistence, restart and multiplayer connectivity.
6. Add a lean standalone launcher for Windows, Linux and macOS: check for a
   new release before starting Q3Rally, download and install an available
   update, then launch the game.

### v0.8 launcher scope

Agreed: a lean launcher for all three desktop platforms (Windows, Linux, macOS).

- Show installed/available versions and release notes.
- Show download progress and support resuming interrupted downloads.
- Verify signed release metadata and downloaded file checksums before installation.
- Stage updates before applying them and retain a working installation if an update fails.
- Preserve settings, profiles, demos and user-added content.
- Allow offline launch of an intact installed version if the update service is unavailable.
- Offer retry or launch of the intact installed version after update failure.
- Detect a running game and avoid replacing its files.
- Provide file verification/repair, accessible diagnostic logs and launcher self-updates.
- Start Q3Rally automatically after a successful update.

Keep server browsing, additional-map management and multiple release channels
outside the v0.8 launcher scope. Implementation technology, supported OS versions,
CPU architectures, update hosting and platform signing/distribution details still
need to be specified during implementation planning.


### Additional v0.8 work items

User-provided tracking numbers are preserved below.

- 19: `make_release.py` implemented: audited, reproducible Windows/Linux/macOS packages, manifests, hashes, preflight/version checks and dry run. Seven packaging regressions pass. Requires release-version native builds and resolved asset blockers before final packaging; see releasing-v0.8.md.
- 20: Initial release asset audit completed: 5,344 selected assets, 13 blocking asset findings and 156 warnings to review. Missing references and two HTML files masquerading as MD3s are recorded in asset-audit-v0.8.md. Cleanup remains open.
- 21: SMAA (optional).
- 26: Q3RallyRadiant: add a dialog for worldspawn keys. Editor repository: https://github.com/P3rlE/Q3RallyRadiant. Local checkout: D:/Q3RallyDev/Q3RallyRadiant. Agreed workflow: open worldspawn settings when creating a new Q3Rally map, then expose the same dialog through Map Settings for later edits. Canceling new-map settings must preserve the current map. Implemented in the Radiant checkout: 50 predefined keys plus an Additional keys editor, runtime validation, default handling, and one undoable edit for existing maps. Windows build and Qt dialog regressions pass; user verified the dialog in the editor. Complete.
- 27: country01: camera remains tilted after the intro. Camera pitch/roll handover corrected; regression test passes, verified in-game by the user; complete.
- 28: country01: racing line briefly disappears at the UFO, where the route crosses itself at the same elevation. Route-continuity scoring added to nearest-point selection; crossing, teleport and loop regression tests pass, verified in-game by the user; complete.
- 29: Fix post-race intermission on all tracks: finished cars remain visibly suspended in midair and the player camera ends up inside vehicle geometry. Observer transition now hides/unlinks the vehicle and clears motion; intermission prevents observer updates from replacing the results camera. Regression tests pass, verified in-game by the user; complete.
- 30: Rear-weapon smoke is strongly dispersed by map wind, reducing weapon effectiveness. Agreed tuning implemented: wind coupling is 0.5 for rear-weapon smoke (previously 1.0), with other particle types unchanged. User verified the adjustment in-game; complete.
- 33: Replace remaining legacy fonts. Following/spectator labels and centerprints now use the current ingame atlas; build passes, in-game verification pending. Continue checking less-used screens for remaining legacy text.
