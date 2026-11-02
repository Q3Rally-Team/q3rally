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
3. Race feedback: missed-checkpoint messages now identify the required gate and are throttled per player; wrong-way checks use movement with a low-speed/start/finish guard; successful automatic car recovery and non-winning race finishes receive center messages. Native/QVM build and production-code regression checks pass; in-game verification pending. See race-feedback.md.
4. Validate graphics presets across representative maps and effects. Preset/pending-value regressions corrected and automated GL1/GL2 checks pass. User verified all four presets on Country01, Valley, Nightcity and Ridgepass after the Low/LDR framebuffer correction. Basic visual checks pass; measured performance comparisons remain open. See graphics-preset-validation.md.
5. Verify clean release installs, settings persistence, restart and multiplayer connectivity. Initial Windows PK3-only startup, settings/profile persistence, GL1/GL2 renderer restarts, pure-server connection, disconnect and reconnect pass after correcting four packaging/runtime defects. Windows libcurl dependency resolved through static linkage; local client/server Ladder HTTP and PK3 download/entry tests plus a separate HTTPS certificate-validation probe pass. Native Linux/macOS, production-service integration, UI/audio checks and an untouched final candidate remain open. See install-validation.md and windows-libcurl.md.
6. Add a lean standalone launcher for Windows, Linux and macOS: check for a
   new release before starting Q3Rally, download and install an available
   update, then launch the game.
7. Test dedicated servers before release: headless startup, server configuration,
   bot population and race startup without human players, client connections,
   complete matches, map rotation and restarts. Windows lap/Sprint bot-only
   startup, population/slot limits, mixed client join/leave, checkpoint progress,
   finish, natural rotation, restart and RCON shutdown checked. Corrected waiting
   bot population/rejoin, repeated Sprint finishes and stale finish placement;
   Sprint rotation config fixed. Windows/native Linux mode startup, team
   assignments and default intro pass; four-team automatic bot fill corrected.
   Linux lap/Sprint completion and local Ladder HTTP pass. Native macOS Intel
   and Apple Silicon builds, 17 mode probes, local HTTP and Sprint pass.
   Country01 two-bot completion fails on both Macs; ARM debug telemetry is
   malformed. These findings, longer objective games and external networking remain open.
   See dedicated-validation.md.

### v0.8 dedicated server validation

Required release checks on Windows, Linux and macOS server builds:

- Start the dedicated executable without a graphical client, using a clean
  installation and a server configuration file; check logs for startup errors.
- Start with configured bots and test automatic population via `bot_minplayers`,
  adding/removing bots, slot limits and humans joining/leaving.
- Confirm bots start races without a human player, navigate, pass checkpoints
  and finish on representative lap and sprint maps with navigation data.
- Connect a separate client, join as a player or spectator, disconnect and
  reconnect; verify the server continues running normally.
- Complete races and other supported game modes; check results, next-map
  rotation, `map_restart`, configuration retention and bot repopulation.
- Check remote administration via RCON and basic LAN/public-server discovery
  under the relevant network configuration.

Record tested platform/build, configuration, maps, bot/player counts and logs.
Windows non-team lap/Sprint checks above now pass, including explicit versus
waiting spectator behavior and unique finish placements. Intro was disabled
for the original race probes; separate default-intro checks now pass on Windows
and native Linux under WSL. Mode startup/team checks and Linux lap/Sprint
completion also pass. Native macOS Intel/ARM mode probes and Sprint now pass;
Country01 two-bot completion and ARM debug telemetry need investigation.
Longer combat/objective games, rotations and external discovery remain open.
See dedicated-validation.md.

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

- 19: `make_release.py` implemented: audited, reproducible Windows/Linux/macOS packages, manifests, hashes, preflight/version checks and dry run. Eight packaging regressions pass, including classic-PK3 compatibility, packaged QVMs and missing dynamic libcurl rejection. Requires version-correct native builds and final runtime validation; see releasing-v0.8.md.
- 20: Missing/invalid asset cleanup completed: 5,350 selected assets, zero blocking asset errors and 153 warnings still to review. Original images/music restored, corrupt unused MD3s removed, Harpy feathers remapped to the compatible Hunter material, and Lavafalls map/BSP corrected (user verified Lavafalls in-game). Harpy appearance still needs a visual check. See asset-audit-v0.8.md.
- 21: SMAA (optional).
- 26: Q3RallyRadiant: add a dialog for worldspawn keys. Editor repository: https://github.com/P3rlE/Q3RallyRadiant. Local checkout: D:/Q3RallyDev/Q3RallyRadiant. Agreed workflow: open worldspawn settings when creating a new Q3Rally map, then expose the same dialog through Map Settings for later edits. Canceling new-map settings must preserve the current map. Implemented in the Radiant checkout: 50 predefined keys plus an Additional keys editor, runtime validation, default handling, and one undoable edit for existing maps. Windows build and Qt dialog regressions pass; user verified the dialog in the editor. Complete.
- 27: country01: camera remains tilted after the intro. Camera pitch/roll handover corrected; regression test passes, verified in-game by the user; complete.
- 28: country01: racing line briefly disappears at the UFO, where the route crosses itself at the same elevation. Route-continuity scoring added to nearest-point selection; crossing, teleport and loop regression tests pass, verified in-game by the user; complete.
- 29: Fix post-race intermission on all tracks: finished cars remain visibly suspended in midair and the player camera ends up inside vehicle geometry. Observer transition now hides/unlinks the vehicle and clears motion; intermission prevents observer updates from replacing the results camera. Regression tests pass, verified in-game by the user; complete.
- 30: Rear-weapon smoke is strongly dispersed by map wind, reducing weapon effectiveness. Agreed tuning implemented: wind coupling is 0.5 for rear-weapon smoke (previously 1.0), with other particle types unchanged. User verified the adjustment in-game; complete.
- 33: Replace remaining legacy fonts. Following/spectator labels and centerprints now use the current ingame atlas; build passes, in-game verification pending. Continue checking less-used screens for remaining legacy text.

### Remaining work before the final v0.8 release

- Verify the new race feedback in-game; finish UI/font checks on less-used screens.
- Review the remaining 153 non-blocking asset warnings and record any intentional references.
- Validate clean installs, settings/profile persistence, restart and multiplayer connections.
- Validate dedicated servers on Windows/Linux/macOS, including bot-only race
  startup, mixed human/bot sessions, map rotation, restarts and administration.
- Implement and test the agreed Windows/Linux/macOS launcher.
- Record graphics performance comparisons; consider SMAA only after required work.
- Build version-correct native platform binaries, test release candidates, then create final packages.

Do not produce the final release while required fixes and checks remain open.
Intermediate builds for testing and launcher/update validation are appropriate.
