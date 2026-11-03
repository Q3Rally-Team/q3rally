# Q3Rally roadmap

Status: 2026-11-03, after the v0.8.1 release.

## Released: v0.8 and v0.8.1

- Standalone English launcher v0.2.1, signed catalog, verified downloads,
  side-by-side installs, repair, offline launch, isolated user data and self-update.
- Game and launcher packages for Windows/Linux x86 and x86_64 and macOS Intel/ARM.
- Embedded demo player with metadata, recording names, seeking and revised controls.
- Menu fonts, dialog hit targets, Back-button placement and car-preview orientation.
- Country01 camera/racing-line fixes, intermission camera/vehicle fixes,
  checkpoint/Sprint feedback and reduced rear-weapon smoke wind coupling.
- Low graphics framebuffer fix, asset repairs, static Windows libcurl and
  dedicated-server/bot/rotation fixes.
- Local profile format v2. Web login, linkage and synchronization remain v0.9 work.
- v0.8.1 restores optional Offline Ladder registration during profile creation
  and accepts both pending and active registration responses. Existing profiles
  without a registration can use the restored prompt.

All six native v0.8.1 package validations passed, including dedicated lap/Sprint
and loopback registration checks. Windows/Linux client checks and user network,
long-match, launcher installation and game-start checks passed. Native macOS CI
does not replace physical Mac graphics, audio or Gatekeeper testing.

Release evidence: [v0.8.1 CI](https://github.com/Q3Rally-Team/q3rally/actions/runs/41556169590).
The earlier item numbers and development notes are preserved in
[the archived v0.8 roadmap](history/v08-roadmap.md).

## Next patches after v0.8.1

- Start from the official released source and keep Tryout synchronized as an
  experimental repository; see [repository workflow](repository-workflow.md).
- Collect and reproduce the user's remaining small issues before assigning
  them to a patch. No new game version is assigned by repository housekeeping.
- Retain clean-install, settings/profile persistence, native package, Ladder,
  dedicated-server and multiplayer regression checks for affected changes.
- Complete physical Mac graphics/audio/Gatekeeper checks on borrowed hardware;
  Intel results do not cover Apple Silicon. See [checklist](mac-client-manual-checklist.md).
- Continue less-used UI/font and asset appearance checks; record graphics
  performance comparisons when hardware is available. SMAA remains optional
  and uncommitted to a release.

## v0.9: Responsive UI

Replace fixed 4:3 layouts with shared rendering/mouse transforms, flexible widths,
edge anchors, minimum spacing and configurable UI scale. Migrate the main menu
and demo player first, then menus, dialogs, loading screens and HUD. Test 4:3,
16:9, 16:10 and ultrawide, including high-resolution text and hit targets.

Initial estimate: 4–7 developer weeks for the complete migration; refine after
the full screen inventory.

## v0.9: Central web profile

The website becomes the profile authority. Add website registration/login,
explicit game-profile linkage, authoritative retrieval, offline-progress sync
after basic validation and device management. Offline play remains equal, with
explicitly limited tamper protection. Preserve native career scoring; keep
Arcade leaderboards separate. See [profile model](central-profile-model.md) and
[v0.8 local format](profile-format-v08.md).

Pre-1.0 progress may be reset. Retain profile identities where possible. From
v1.0, freeze the scoring rules; additional ranks and achievements remain possible.
The retired Dedicated-only prototype must not be deployed.
