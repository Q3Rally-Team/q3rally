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

## v0.9 — Central web profile and synchronization

Website login and explicit in-game linkage to the central web profile belong
here, along with authoritative profile retrieval, offline-progress sync after
basic validation, device management and the further profile/web integrations.
v0.8 prepares only the profile structure; it does not require a running central
login/sync service. See central-profile-model.md for the target architecture.

## v0.8 - Release priorities

1. UI consistency: review remaining legacy dialogs, fonts, hit targets and overlapping overlays. Shared bottom-left Back buttons implemented across regular menus; Config defaults moved to the right. In-game verification pending.
2. Complete the demo player: elapsed playback time and pause/resume clock correction implemented; regression tests pass and the user has verified both in-game. Seeking/EOF regression tests and browser error recovery for missing maps, unsupported protocols, damaged or truncated recordings implemented; regression tests pass and the user has verified the new behavior in-game. Demo player work complete for v0.8.
3. Race feedback: missed-checkpoint messages now identify the required gate and are throttled per player; wrong-way checks use movement with a low-speed/start/finish guard; successful automatic car recovery and non-winning race finishes receive center messages. Native/QVM build and production-code regression checks pass; in-game verification pending. See race-feedback.md.
4. Validate graphics presets across representative maps and effects. Preset/pending-value regressions corrected and automated GL1/GL2 checks pass. User verified all four presets on Country01, Valley, Nightcity and Ridgepass after the Low/LDR framebuffer correction. Basic visual checks pass; measured performance comparisons remain open. See graphics-preset-validation.md.
5. Verify clean release installs, settings persistence, restart and multiplayer connectivity. Initial Windows PK3-only startup, settings/profile persistence, GL1/GL2 renderer restarts, pure-server connection, disconnect and reconnect pass after correcting four packaging/runtime defects. Windows libcurl dependency resolved through static linkage; local client/server Ladder HTTP and PK3 download/entry tests plus a separate HTTPS certificate-validation probe pass. Native Linux/WSLg client GL1/GL2 startup, renderer restarts, settings/profile persistence and PulseAudio initialization also pass. Native Intel/macOS 15 client Low checks pass for both renderers, including verified preset values; ARM Low checks await GitHub runner capacity. Production-service integration, interactive UI/audio-quality checks and an untouched final candidate remain open. See install-validation.md and windows-libcurl.md.
6. Must-have: a lean standalone launcher for Windows, Linux and macOS.
   List compatible available releases, recommend the newest stable release,
   and allow installing and playing older releases side by side. Check for
   updates before launch without forcing an upgrade.
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
   Country01 completion and ARM telemetry defects corrected; native Intel/macOS 15
   and ARM/macOS 26 lap/Sprint, strict JSON telemetry and interpreter probes pass.
   User confirmed network tests with a second computer and long matches pass.
   Dedicated functional validation passes for the exercised scenarios;
   final release-package validation remains part of release preparation.
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
completion also pass. Native macOS Intel/ARM mode probes pass; after correcting
VM casts and legacy Bezier recovery, both lap/Sprint completion and strict
telemetry checks pass on Intel/macOS 15 and ARM/macOS 26.
User also confirmed successful network tests and long matches. Dedicated
functional checks pass for the exercised scenarios; client and final-package
validation remain separate release tasks.
See dedicated-validation.md.

### Ladder policy through v1.0

- Before v1.0: visibly marked test operation; scoring/data resets are allowed.
- From v1.0: calculation rules are stable with no regular resets. Additional
  profile ranks, achievements and displays may extend the existing point scale.
- Agreed central profile: the website/profile service owns confirmed native
  career progress; the client caches it and keeps a durable offline journal.
  Regular offline matches are accepted after basic validation, with explicitly
  limited tamper resistance. No second native career point balance.
- Preserve the existing native score rules before moving authority centrally;
  do not substitute the prototype's match-score formula. Arcade leaderboards
  remain separate; identity, garage and cosmetic permissions are shared.
- The earlier Dedicated-only/reset implementation is a superseded local
  prototype, not approved for deployment. Profile sync and migration are not
  implemented. See central-profile-model.md and contracts/README.md.

### v0.8 launcher scope

Agreed: a lean launcher for all three desktop platforms (Windows, Linux, macOS).

- The launcher is the primary download and entry point for v0.8.
- Isolate legacy installations/user data and prepare versioned profile identity,
  resettable progress and unlinked web-profile references in v0.8. Actual web
  login/linking and central synchronization move to v0.9. No historical point
  migration is required; retain existing profile identities where possible.
  The Dedicated-only prototype is superseded; see central-profile-model.md.
- Show compatible installed/available versions and release notes. Highlight the
  newest stable release as Recommended; keep older published releases selectable.
- Install versions side by side and launch the selected version explicitly.
  An update must not overwrite an older version or silently change the selection.
- Offer Install and play for an uninstalled version, Play for an installed one,
  and an explicit option to remove an installed version.
- Remember the last selected version and retain an offline version inventory.
- Separate writable user data by game version to prevent older games from
  modifying newer profiles/settings. Offer explicit import on first launch.
- List older releases only when a compatible, verified package is published;
  historical releases are not automatically supported on every platform.
- Show download progress and support resuming interrupted downloads.
- Verify signed release metadata and downloaded file checksums before installation.
- Stage updates before applying them and retain a working installation if an update fails.
- Preserve settings, profiles, demos and user-added content.
- Allow offline launch of an intact installed version if the update service is unavailable.
- Offer retry or launch of the intact installed version after update failure.
- Detect a running game and avoid replacing its files.
- Provide file verification/repair, accessible diagnostic logs and launcher self-updates.
- Start Q3Rally after a successful installation only when requested through
  Install and play; a download-only action must not launch the game.

Keep server browsing, additional-map management and multiple release channels
outside the v0.8 launcher scope. Implementation technology, supported OS versions,
CPU architectures, update hosting and platform signing/distribution details still
need to be specified during implementation planning.


### Additional v0.8 work items

User-provided tracking numbers are preserved below.

- 19: `make_release.py` implemented: audited, reproducible Windows/Linux/macOS packages, manifests, hashes, preflight/version checks and dry run. Eight packaging regressions pass, including classic-PK3 compatibility, packaged QVMs and missing dynamic libcurl rejection. Requires version-correct native builds and final runtime validation; see releasing-v0.8.md.
- 20: Missing/invalid asset cleanup completed: 5,350 selected assets, zero blocking asset errors and 144 reviewed warnings retained (nine references repaired). Original images/music restored, corrupt unused MD3s removed, Harpy feathers remapped to the compatible Hunter material, and Lavafalls map/BSP corrected (user verified Lavafalls in-game). Harpy appearance still needs a visual check. See asset-audit-v0.8.md.
- 21: SMAA (optional).
- 26: Q3RallyRadiant: add a dialog for worldspawn keys. Editor repository: https://github.com/P3rlE/Q3RallyRadiant. Local checkout: D:/Q3RallyDev/Q3RallyRadiant. Agreed workflow: open worldspawn settings when creating a new Q3Rally map, then expose the same dialog through Map Settings for later edits. Canceling new-map settings must preserve the current map. Implemented in the Radiant checkout: 50 predefined keys plus an Additional keys editor, runtime validation, default handling, and one undoable edit for existing maps. Windows build and Qt dialog regressions pass; user verified the dialog in the editor. Complete.
- 27: country01: camera remains tilted after the intro. Camera pitch/roll handover corrected; regression test passes, verified in-game by the user; complete.
- 28: country01: racing line briefly disappears at the UFO, where the route crosses itself at the same elevation. Route-continuity scoring added to nearest-point selection; crossing, teleport and loop regression tests pass, verified in-game by the user; complete.
- 29: Fix post-race intermission on all tracks: finished cars remain visibly suspended in midair and the player camera ends up inside vehicle geometry. Observer transition now hides/unlinks the vehicle and clears motion; intermission prevents observer updates from replacing the results camera. Regression tests pass, verified in-game by the user; complete.
- 30: Rear-weapon smoke is strongly dispersed by map wind, reducing weapon effectiveness. Agreed tuning implemented: wind coupling is 0.5 for rear-weapon smoke (previously 1.0), with other particle types unchanged. User verified the adjustment in-game; complete.
- 33: Replace remaining legacy fonts. Following/spectator labels and centerprints now use the current ingame atlas; build passes, in-game verification pending. Continue checking less-used screens for remaining legacy text.

### Remaining work before the final v0.8 release

- Defer physical Mac graphics/audio checks to a separate manual test session
  on a possibly borrowed Intel Mac. Keep these checks pending; an Intel result
  does not cover Apple Silicon. See mac-client-manual-checklist.md.
- Verify the new race feedback in-game; finish UI/font checks on less-used screens.
- Asset warning review completed: all 153 classified, nine repaired, 144 documented in asset-warning-review.json. Optional legacy player sound lookups now use VFS checks and the default vehicle fallback.
- Validate clean installs, settings/profile persistence, restart and multiplayer connections.
- Dedicated functional checks pass on the tested platforms/scenarios, including
  user-confirmed network tests and long matches; retain these checks for the final candidate.
- v0.8 profile structure implemented: version 2 separates identity, local
  progress and unlinked web references. Legacy identity/favorites are retained;
  old points/ranks/stats reset once. Native/UI writers share the format;
  future/linked or damaged files are not overwritten. Windows/Linux native/QVM
  builds and profile checks pass; see profile-format-v08.md. Actual website
  login/linking and offline synchronization remain v0.9 work. Supersede the
  Dedicated-only prototype separately before deployment.
- Implement and test the agreed Windows/Linux/macOS launcher.
- Record graphics performance comparisons; consider SMAA only after required work.
- Build version-correct native platform binaries, test release candidates, then create final packages.

Do not produce the final release while required fixes and checks remain open.
Intermediate builds for testing and launcher/update validation are appropriate.
