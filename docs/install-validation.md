# Install, persistence and connection validation

Checked on Windows x86_64 on 2026-11-02, using the current v0.7d development
version marker. This is not a final v0.8 release or cross-platform sign-off.
Runtime probes, logs and JSON results are in `output/install-validation/`.

## Setup and results

Extracted a generated package into a path containing spaces, with a separate
fs_homepath and only PK3 assets. Personal configuration and developer assets
were outside the VFS. PATH contained only the test directory and Windows
system directories. The test installation was then updated with corrected
engine/renderer/UI binaries and classic PK3/QVM packaging as defects were fixed.
The original ZIP in `output/install-validation/packages/` predates those fixes
and must not be distributed or treated as a validated candidate.

| Check | Result |
|---|---|
| Initial launch without q3config or an existing profile | Passed; config created and normal exit |
| Save/reload sensitivity, FPS setting and F8 binding | Passed |
| Synthetic offline profile loaded on restart | Passed; UUID and checked kill count retained, active profile name restored |
| OpenGL1 and OpenGL2 vid_restart | Passed; checked settings/identity retained, normal exit |
| Windows dedicated startup with packaged assets | Passed, loopback only |
| Client connection with sv_pure 1 | Passed; ClientConnect and ClientBegin logged |
| Disconnect and normal shutdown | Passed; exit status 0 |
| Reconnect to the same running server | Passed; two ClientBegin events and two cgame loads; probe stopped client after observing second connection |
| Server survival and RCON shutdown | Passed |

The profile fixture was isolated from the user's data. This checks existing
profile loading/preservation, not the creation wizard or Ladder registration.
Sound was disabled. Connections used packaged QVMs, not native game DLLs.

## Defects corrected

1. Forced ZIP64 local headers in asset PK3s were incompatible with the engine's
   legacy minizip reader, appearing as an invalid bigchars TGA. Asset PK3s now
   use classic ZIP and reject ZIP64 requirements.
2. UI file enumeration relied on the VFS listing `.` as a child directory.
   PK3-only installs missed root files, leaving the menu-picture list empty
   and causing modulo-by-zero. Root enumeration is now explicit, with empty
   random-selection and missing-picture fallbacks.
3. Loose QVMs were rejected by pure servers. Packaging now places the selected
   build's three VMs in baseq3r/qvms.pk3 (the equivalent app path on macOS).
4. Renderer homepath reads allocated files outside FS's load counter. A later
   FS_FreeFile cleared memory still used by an MD3 loading its textures. The
   fallback loader now shares normal filesystem allocation accounting;
   disconnect/reconnect no longer crashes.

TGA decoder failures now include the filename. Current Windows client, server,
GL1/GL2 renderers, UI DLL and UI QVM build. Ten regression tests pass:

```powershell
$env:PATH = 'C:\msys64\mingw64\bin;' + $env:PATH
python -m unittest tests.test_make_release tests.test_ui_filelist tests.test_renderer_file_fallback -v
```

## Native Linux client checks (WSLg)

Ubuntu 24.04 / WSLg built and ran the native x86_64 Linux client and both
renderers. Data and executable were staged in a separate path containing
spaces, with only pak0.pk3 (5350 assets) and qvms.pk3 (three freshly built VMs)
under baseq3r. The search-path log confirms no developer loose assets/QVMs.
The synthetic InstallProbe profile and home were separate from real user data.

OpenGL 1 and OpenGL 2 both passed:

- Requested renderer loaded without falling back to the other renderer.
- Frontend and Country01 screenshots produced; inspected images render correctly.
- vid_restart reloaded the renderer and continued rendering the track.
- Normal process shutdown, saved/reloaded sensitivity 7.25, FPS setting and F8 binding.
- Profile UUID and checked statistics retained, with the active profile loaded.
- SDL opened PulseAudio, initialized playback and closed the audio device normally.

The renderer was llvmpipe (LLVM 20.1.2), OpenGL 4.5 / Mesa 25.2.8, so this is
software-rendered functionality coverage, not GPU performance validation.
The fresh-profile frontend displayed the expected Ladder setup prompt; no
registration was performed. Audio initialization does not prove perceived
sound quality, and scripted console commands do not verify mouse/keyboard
interaction or complete profile creation through the UI.

Audio-enabled runs also expose missing legacy custom player voice lookups:
death/pain/jump/gasp/drown/fall/taunt sounds are attempted for the car model,
DEFAULT_MODEL and the old sarge fallback in cg_players.c. These are retained
as runtime findings for the asset/reference review; they are not established
as correct by the audio-device initialization check. No gameplay crash occurred.

Evidence: output/install-validation/linux-client-final/summary.json, per-renderer
stdout/qconsole logs, and home/baseq3r/screenshots/. The earlier runtime/ directory
has a loading-screen first capture and is superseded by the final run.
tools/validate_client_runtime.py implements these bounded checks.

## Native macOS client checks

The initial native macOS Intel/ARM client checks ran at
https://github.com/P3rlE/q3rally_tryout/actions/runs/41454566736, commit
f2fcb31b70f8e8ec5b27706be8f0b0b62c26baee. OpenGL 1 passed on both native
architectures. Both OpenGL 2 runs exposed a real shader compatibility defect:
the Apple Software Renderer uses GLSL 1.20, where textureCubeLod is not a core
fragment-shader operation. The car-reflection shader was compiled whenever
FBOs were supported and failed even with general r_cubeMapping disabled.

carreflect_fp.glsl now uses cube sampling with a mip bias on legacy GLSL and
retains the existing explicit-LOD operation on GLSL 1.30+. Linux runtime probes
passed both the forced OpenGL 2.1 / GLSL 1.20 context (linux-glsl120/) and the
normal OpenGL 4.5 context (linux-glsl-modern/), including restart and screenshots.
The runtime checker now records shader failures/timeouts reliably and kills
an unresponsive test child if graceful termination fails.

Native macOS retest: https://github.com/P3rlE/q3rally_tryout/actions/runs/41456465376,
commit cd99216d1ab37c6a6b69ac7e61fad4ed69942dd1. The Intel run timed out after
successful shader initialization and map loading. The runner
audio devices are virtual (Intel uses a Null Audio Device; ARM's initial run
used Apple Virtual Sound Device). CoreAudio initialization is testable, but
these runs do not establish audible output quality on a physical Mac.

Intel's corrected GLSL 1.20 renderer initialized, produced a frontend image
and loaded Country01, but the 300-frame track warmup exceeded the 180-second
limit on Apple Software Renderer. It responded to termination and shut down
normally; this is retained as an unsuccessful runtime run, not a passing case.
The smoke test now uses 30 loaded-world frames before each track capture and
after vid_restart (instead of 300/90), while retaining all image, restart,
settings, profile, audio and normal-exit assertions. Linux validated this
bounded version for both renderers and its captured images were inspected.

The shorter-warmup native client retest was:
https://github.com/P3rlE/q3rally_tryout/actions/runs/41458068920, commit
8c087f66e322565d628b2d0c39f769e7a4268b2a. Intel produced a track screenshot and
restarted the renderer but still exceeded the deadline before its last capture.
The screenshot shows less than one frame/second and visual artifacts with the
expensive default effects on Apple Software Renderer. This does not certify
High/Ultra rendering on a physical Mac; hardware-GPU checks remain open.

Low software-renderer coverage uses the production UI preset table. The first
Low attempt exceeded the engine's limit of 32 startup +commands, so validation
was not executed. Preset values now go through the isolated home's autoexec.cfg
before renderer initialization. The corrected Linux run passed both renderers
(linux-client-low-autoexec/); its final saved graphics values match the Low
table exactly. The checker now additionally asserts every requested preset
value against each renderer's saved config and retains those config snapshots.

Corrected native Low-preset retest:
https://github.com/P3rlE/q3rally_tryout/actions/runs/41485986935, commit
bee01ee49ea60724e3f67ed102e4859a49be502b. Intel/macOS 15 passed both renderers:
normal exit, all requested Low preset values persisted without mismatches,
frontend/track/restart captures, profile/UUID/statistics, settings/binding
persistence and CoreAudio initialization. Evidence is saved locally in
output/install-validation/macos-client-low-intel/. ARM/macOS 26 was not executed:
GitHub cancelled provisioning after repeated attempts with the annotation
"The job was not acquired by Runner of type hosted even after multiple attempts"
and explicit macOS arm64 capacity-constraint notices. The workflow's overall
failure therefore includes an infrastructure failure, not a failed ARM client
assertion. Do not record this ARM target as passed.

The Intel Low screenshots render the track, vehicle and HUD, but inspection
still shows raster artifacts on some reflective/multipass surfaces with Apple
Software Renderer. Functional smoke assertions are green; this does not certify
visual fidelity or performance on real Mac hardware. Physical-GPU coverage
remains necessary rather than treating the virtual renderer as that coverage.
Physical Intel-Mac graphics/audio checks are deferred to a separate manual
session. The user may be able to borrow an Intel Mac from his daughter; access
is not yet confirmed. These checks remain pending, not waived. Apple Silicon
hardware coverage is separate and is not established by an Intel-Mac result.
Use mac-client-manual-checklist.md for the handoff.

Because macOS-26 ARM runner allocation remains queued, the same code is also
being checked on macOS 15 ARM at
https://github.com/P3rlE/q3rally_tryout/actions/runs/41487743062, commit
0c506006bf96a9782ca69401ac59b4b41735d6e3. Only the additional workflow differs
from the Intel-passing bee01ee4 test code. GitHub cancelled this attempt too,
after roughly 15 minutes without runner allocation, with the same provisioning
failure and macOS arm64 capacity notices. Neither ARM Low client job executed
any build or test step. Both are infrastructure-blocked, not client test passes
or client assertion failures. Retry an ARM job when hosted capacity is available;
no further code change is needed merely to retry the prepared test.

## Remaining release checks

- Physical Intel-Mac graphics/audio checks are deferred to the user's possible
  borrowed Mac; see mac-client-manual-checklist.md. ARM client/hardware coverage
  remains a separate open item.
- The originally missing Windows libcurl dependency is now resolved by static
  linkage. Client/server Ladder HTTP, PK3 download integrity and subsequent
  pure-server entry pass against local fixtures; HTTPS certificate validation
  passes with a separate library probe. See windows-libcurl.md for evidence
  and the remaining production-service checks.
- Native Linux/WSLg and Intel/macOS 15 Low client functional checks pass.
  Repeat the prepared ARM Low client job once GitHub runner capacity is available.
- Rebuild version-correct native inputs and repeat against an untouched freshly
  generated candidate archive, including manifest/hash checks.
- Check audio, profile creation and settings through the UI, and intended
  read-only installation/user-directory permissions.
- The user confirmed network tests with another computer and long matches pass.
  Dedicated functional checks are recorded separately in dedicated-validation.md.

## Asset/player sound follow-up (2026-11-02)

Windows and Linux native cgame plus QVM builds pass with the optional player
sound fix. Nine targeted sound/packaging regression tests pass. The static
audit reports 5350 assets, zero blocking errors and 144 retained warnings;
all 153 original warnings are classified in `asset-warning-review.json`.

A fresh isolated Linux runtime installation overlays the five changed asset
files and newly built QVMs on the complete test data. WSLg/llvmpipe with SDL
PulseAudio verifies both GL1 and GL2 at Low: menu, Country01, renderer restart,
settings/profile persistence, yellow flag model loading and screenshots.
`s_list` confirms Alpine horn/falling sounds and default engine pop/shift
samples resident; missing legacy player sounds no longer produce warnings.
This validates sound loading, not listening quality or physical-Mac hardware.
Evidence: `output/asset-reference-review/runtime-low-confirmed/summary.json`
and the corresponding console logs/screenshots. The first extra sound probe
used the unsupported command `soundlist`; it is retained separately and
superseded by the corrected `s_list` run.
