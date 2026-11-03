# Manual Mac client validation

Status: deferred, not yet executed. The user may be able to borrow an Intel
Mac. Apple Silicon hardware validation remains separate.

## Record the test environment

- Mac model, CPU architecture, macOS version and GPU.
- Tested build/version and archive checksum.
- Display resolution and scaling; windowed or fullscreen mode.
- GL renderer/version from the game log; confirm whether rendering is hardware
  accelerated. Keep software-renderer results explicitly labelled.
- Audio output device (speakers/headphones).

Use the matching Intel build and a separate test profile. Test the assembled
.app candidate when available; CI's standalone binary checks do not establish
app-bundle installation behavior.

## Checks

- [ ] Launch the .app from its installed location and reach the frontend.
- [ ] Create a test profile through the UI; verify menus, dialogs, text and fonts.
- [ ] Check mouse targets, keyboard navigation, Back buttons and save dialogs.
- [ ] Open the demo player; verify playback controls without cursor capture.
- [ ] Run Country01 and at least one other representative track with OpenGL 1.
- [ ] Repeat with OpenGL 2, including a renderer restart and map reload.
- [ ] Compare Low, Medium, High and Ultra where supported; inspect vehicle
      reflections, water, shadows, post-processing, particles and HUD. Record
      unsupported settings and any artifacts instead of treating them as passes.
- [ ] Compare windowed/fullscreen behavior and UI alignment at native resolution.
- [ ] Listen to menu sounds, engine/gear changes, horn, braking and weapon/effect
      audio; check for missing, distorted or interrupted playback.
- [ ] Save settings and a binding, quit normally, relaunch and verify persistence.
- [ ] Verify that profile identity/statistics remain intact across restarts.
- [ ] Confirm user data is writable independently of application files.

## Return results

Record pass/fail/not tested for each check. For failures, include reproduction
steps, renderer/preset, screenshots and relevant log excerpts. Note frame rate
and visual quality separately from successful startup. Intel results cover
the tested Intel configuration, not Apple Silicon or every Mac/GPU.
