> Historical development record, archived after v0.8.1 on 2026-11-03.
> Pending items below describe earlier stages; use the current roadmap and launcher guide for status.

# Q3Rally Launcher

The launcher and its signed catalog are hosted on Q3Rally's own webspace. Game
packages come from GitHub Releases. `launcher/config.json` configures the catalog
URL and trusted Ed25519 public keys. The URL currently uses the proposed path
`https://www.q3rally.com/launcher/catalog.json`; confirm it before publishing.

## Current implementation

- Lists compatible versions, recommends the latest stable version and remembers
  the selected version. Older versions can be installed and played independently.
- Downloads on a worker thread, supports pause/resume and validates SHA-256 plus
  the signed catalog and the package's internal release manifest.
- Repairs by reinstalling into a staging directory before swapping installations.
  A previous installation is restored after a failed or interrupted directory swap.
- Verifies files before starting. Installed games work offline even when the
  catalog has expired; new installations require a current signed catalog.
- Prevents concurrent installation/removal while that version is running.
- Keeps profiles, demos and configuration under `user-data/<version>`, separate
  from installed files. Removing a game does not remove its user data.
- Imports existing profile identity and favorites explicitly, without old points,
  remote credentials or account bindings. Legacy versions cannot share that profile.
- Installs signed launcher updates alongside the original launcher; the original
  shortcut delegates to a verified newer version and falls back if it is damaged.

Default data location is `%LOCALAPPDATA%/Q3RallyLauncher` on Windows,
`~/Library/Application Support/Q3RallyLauncher` on macOS, and
`${XDG_DATA_HOME:-~/.local/share}/q3rally-launcher` on Linux.

## Develop and test

Use Python 3.13 with Tk and `python -m pip install -r launcher/requirements.txt`.
Run `python run_launcher.py`, or use `--action list|install|play|verify|remove`
with `--version 0.8`. `--data-dir` selects an isolated test installation.
`--development` permits HTTP only on loopback; signatures remain mandatory.

`python -m unittest discover -s tests -p test_launcher.py -v` exercises the real
HTTP download and install paths using temporary signing keys and small packages.
The native CI matrix covers Windows, Linux, Intel macOS and ARM macOS, with
development artifacts only. No workflow uploads public releases.

## Validation on 2026-11-02

- Windows x86_64: 17 launcher integration tests pass, including signed publishing,
  resumed HTTP transfers, rejected corrupt packages, swap rollback/recovery,
  retained user data, explicit identity import and a real downloaded executable.
- Linux x86_64 under WSL: the same 17 tests pass using an isolated 64-bit Python
  runtime. The machine's existing 32-bit Python installation was not replaced.
- Native Tk selection/layout checks pass on Windows and Linux/WSLg, including
  the minimum window size. Both native development ZIPs build successfully;
  the packaged Linux GUI starts without Python/Tk environment overrides.
- The reverted Ladder prototype is covered by 29 Windows serializer/PHP tests
  and 26 Python-service tests under WSL. The latter skip 39 optional mode-matrix
  scenarios; these are not claimed as newly executed full game tests.
- Six profile regressions remain green. Engine client/server/native modules/QVMs
  rebuild successfully on Windows and Linux after prototype removal.
- macOS native jobs are prepared but have not run for this change. Signing,
  notarization and hardware launch/audio/graphics checks remain open.

Logs and development artifacts are in `output/launcher-validation/`. The final
review packages are under `windows-complete/` and `linux-complete/`; their bundled
configuration deliberately has no production trust key or published catalog yet.

## Hosted catalog and real game-package checks

The fixed endpoint is now `https://www.q3rally.com/launcher/catalog.json`,
served by Apache from `/var/www/q3rally-launcher/catalog.json`. This directory is
outside the Astro build. The user deployed sequence 2; its signature and the
Windows launcher v0.1.0 download were independently verified after publication.
The exact launcher URL is:
`https://www.q3rally.com/files/Q3RallyLauncher-v0.1.0-windows-x86_64.zip`.
No game package has been added to the public catalog yet.

The release signing key was generated outside the repository at
`%LOCALAPPDATA%/Q3RallyRelease/Keys/release-1.pem`; `launcher/config.json` contains
only its public key. Retain a secure backup of the private key. It must never be
uploaded with the catalog or game packages.

The real v0.8 Windows/Linux test packages are in
`output/launcher-publishing/game-test-candidate/`, with SHA256SUMS, release.json
and the separate audit report. They are intermediate test builds, not final v0.8
releases. macOS packages and the remaining release checks remain separate work.

`tools/validate_launcher_runtime.py` serves an actual game ZIP from a loopback
server, signs a temporary test catalog with an in-memory key, interrupts and
resumes its download, installs it through Manager, then shuts down the server.
It imports identity into a fresh per-version home and starts the actual game
through Manager.run, exercising the menu and Country01 with screenshots. Finally
it detects a deliberately modified README, repairs from the verified download
cache, and checks that the profile and user configuration survive. It never
changes the production catalog or uploads a package.

```text
python tools/validate_launcher_runtime.py --release-index output/launcher-publishing/game-test-candidate/release.json --target windows-x86_64 --output output/launcher-publishing/runtime-windows
```

Use a fresh output path for each run. Linux uses `--target linux-x86_64` on a native
desktop/WSLg. Reports and game logs are retained under the chosen output path.
The real package tests passed on Windows x86_64 and Linux x86_64/WSLg, including
Country01, offline start, resumed downloads and repair with retained profile data.
Combined evidence: `output/launcher-publishing/game-test-candidate-validation.json`.
Native macOS package tests are still pending. The user-authorized GitHub
pre-release is published at
`https://github.com/P3rlE/q3rally_tryout/releases/tag/v0.8-launcher-test.1`.
Both uploaded game packages have matching GitHub SHA-256 digests. Public HTTPS
range downloads return 206 and match the local package bytes; see
`output/launcher-publishing/github-download-validation.json`. A complete manual
download/install via the hosted test catalog is the next user check.
For Sanity assets whose URLs differ from package filenames, the catalog publisher
accepts `--download-urls urls.json`, mapping artifact filenames to exact HTTPS URLs.

The user selected `P3rlE/q3rally_tryout` for launcher-test pre-releases. The first
test tag is `v0.8-launcher-test.1`. `output/launcher-publishing/catalog-test.json`
is signed sequence 3 and retains the published launcher entry while adding the
Windows/Linux game packages. Deploy it as `/launcher/catalog-test.json`, leaving
the production `/launcher/catalog.json` at sequence 2. The matching local
`launcher-test-config.json` points to the test URL. Always use a separate
`--data-dir` for this test so catalog sequence tracking and profiles remain
independent of normal launcher use.

Launcher v0.1.1 fixes catalog requests blocked by Cloudflare error 1010: urllib's
default Python User-Agent was rejected while the launcher-specific User-Agent was
accepted. The shared HTTP opener now identifies all requests as
`Q3RallyLauncher/<version>`, including catalogs and redirects. Regression coverage
checks both catalog and package request headers. The original v0.1.0 needs a manual
replacement because its blocked catalog request also prevents discovering updates.
Windows v0.1.1 artifacts are under `output/launcher-publishing/windows-0.1.1/`.

After the user's successful manual download/install/play test, first-run renderer
defaults were changed in GL1 and GL2 to `r_fullscreen=1` and `r_mode=-2`
(desktop resolution). Existing archived settings retain priority. Windows/Linux
renderer builds pass; this change belongs to the next game test package and does
not modify the already published `v0.8-launcher-test.1` artifacts. The user's
manual run used Intel UHD Graphics 770; Windows per-application GPU preference is
separate from fullscreen mode.

## Website design / launcher v0.2.0

The homepage is the approved visual reference. Palette: #0a0a0a / #111111 /
#181818 with #ff5500 accents. Barlow, Barlow Condensed Bold and Share Tech Mono
are bundled with their SIL Open Font License notices from google/fonts. Fonts
are registered only for the launcher process; no system installation is needed.
The native fallback remains usable if font registration fails.

The resizable 1180x800 window has a 1000x700 minimum, a separate version list,
large selection/release-note area and dedicated start bar. Install-only, verify,
profile import and removal are grouped under Version verwalten. Launcher update,
progress and pause controls appear only when relevant; selection and download
state handling preserve the underlying installer behavior. Both text panes have
scrollbars. No website or tracking requests were added to the launcher.

Windows/Linux native tests pass (18 core integration cases plus the extended GUI
layout/state case). The GUI case checks minimum-size bounds, available/installed
actions, pause visibility and launcher-update visibility. Windows visual checks
also covered the real catalog and the management menu. Native macOS font loading
and appearance remain pending hardware/CI validation. The new local Windows
package is under output/launcher-publishing/windows-0.2.0; it has not been uploaded
to Sanity or added to a public catalog.

The user subsequently uploaded Windows v0.2.0 to
`https://www.q3rally.com/files/Q3RallyLauncher-v0.2.0-windows-x86_64.zip`.
The downloaded file matches SHA-256
`3c32f14949aea1b2520e37f22e7c94e221288eb6c6f050ca318038ddb29c54b4`.
Its installer manifest/files and three bundled fonts were verified from that
download. Prepared catalog updates are `catalog-production-0.2.0.json` (sequence
3, still no game packages) and `catalog-test-0.2.0.json` (sequence 4, retaining the
v0.8 test packages). Server deployment and an interactive update from v0.1.1 to
v0.2.0 are pending user execution. Files/evidence are under
`output/launcher-publishing/`.

## Native platform validation follow-up

The user verified the Windows v0.1.1 -> v0.2.0 self-update successfully.
Linux v0.2.0 was built under WSL and on Ubuntu 22.04 CI. Both packages pass a
signed installation/update check, old v0.1.0 shortcut handoff to the managed
v0.2.0 GUI, live catalog access and retained user data. The Ubuntu-22.04 package
is preferred for distribution over the local Ubuntu-24.04 build. Evidence:
`output/launcher-publishing/linux-ci-selfupdate/result.json` and
`output/launcher-publishing/linux-ci-package-smoke/result.json`.

The isolated remote branch `codex/launcher-v020-platform-validation` contains the
launcher and its native workflow; master was not changed. The two shared profile
format source files were also included because the general game build exposed
that they were missing from a clean remote checkout. The workflow uses native
Windows, Ubuntu 22.04, Intel macOS 15 and ARM macOS 26 runners. It now installs
and starts each produced ZIP after core/layout tests and the build.

The initial Intel macOS package built successfully but FAILED actual packaged
startup: source-built cryptography 50 dynamically linked Homebrew OpenSSL, while
PyInstaller collected an older libssl.3.dylib from Python. Do not distribute the
initial `ci-macos-intel-0.2.0` artifact. The corrected Intel workflow uses
`OPENSSL_STATIC=1` and `OPENSSL_DIR=$(brew --prefix openssl@3)`, disables pip cache
reuse and asserts via otool that cryptography no longer depends on dynamic
libssl/libcrypto. This follows the upstream static-build instructions:
https://cryptography.io/en/latest/installation/#building-cryptography-on-macos
The installed-package smoke test is a required release gate, not just a build.
Apple Developer ID signing/notarization and external hardware checks remain
separate requirements; CI startup does not cover Gatekeeper download behavior.

The Mac launcher additionally bundles Mozilla CA roots from python-certifi under
its MPL license in launcher/assets/certificates. Its HTTPS context keeps default
certificate/hostname verification and loads those roots, avoiding a dependency
on an external Python installation's CA file. The package check also fetches the
actual signed HTTPS catalog and requires a verified cache file. Windows/Linux
pass this complete package/HTTPS test. Intel Mac network diagnostics are retained
in the CI artifacts if the live check fails; the initial static-OpenSSL Mac
package passed CLI/GUI startup before the HTTPS test was added.

Mac source-level Python/curl requests returned HTTP 200 while the packaged app's
custom CA context received 403. The custom context now explicitly negotiates
`http/1.1` via ALPN and enables post-handshake authentication to match Python's
standard HTTPS context. Python documents the ALPN requirement for custom contexts:
https://docs.python.org/3.13/library/urllib.request.html#urllib.request.urlopen
Local Windows core/layout regressions remain green (19 tests), and the simulated
Mac HTTPS context fetches the live catalog successfully. Native Mac CI remains
the deciding check; do not mark it passed solely from these local results.

Final native results (2026-11-02): Windows and Ubuntu 22.04 packages pass the
complete checks; Intel macOS 15 and ARM macOS 14 packages also pass signed
installation, installed CLI/GUI startup and the real signed HTTPS catalog fetch.
The dedicated Mac network comparison reproduced HTTP 403 without standard
negotiation and HTTP 200 with it. No Cloudflare/server relaxation was required.
macOS 26 runner jobs remained queued; ARM validation used an explicitly approved
macOS 14 fallback instead. Superseded own runs were cancelled with user approval.

Ready files, aggregate release.json, SHA256SUMS and validation.json are under
`output/launcher-publishing/launcher-platforms-0.2.0/`. The original already
published Windows ZIP was retained byte-for-byte. The final Linux ZIP also passes
the v0.1.0 -> v0.2.0 managed launcher handoff/update test; see
`output/launcher-publishing/linux-final-selfupdate/result.json`.
New Linux/Mac ZIPs have NOT been uploaded to the website, and public catalogs
have NOT been changed. Upload/URL verification is the next publishing step.
Physical Mac/Gatekeeper checks and Developer ID/notarization remain pending.

Native CI evidence: primary run 41537911067 (successful native jobs retained;
the remaining redundant queued macOS-26 job was cancelled), ARM fallback run
41538327169, and HTTP comparison run 41537910995 in P3rlE/q3rally_tryout.

## English and 32-bit release follow-up

The user flagged the German UI and missing Windows/Linux x86 packages before
final release. v0.2.1 source now uses English throughout the interface, dialogs,
status messages and runtime errors. Local Windows and Linux core/layout tests
pass (20 cases). Validation runs in the isolated
`codex/launcher-v021-x86-validation` branch; nothing has been deployed.

The runtime now verifies Ed25519 signatures with PyNaCl, retaining the catalog
format and existing trust keys. A fixture checks the previously published signed
catalog. PEM signing remains on the publishing host using
`launcher/requirements-publishing.txt`; its interoperability test runs separately.
This avoids freezing an obsolete cryptography version for 32-bit runtime wheels.

CI builds Windows x86/x64 with matching Python interpreters, Linux x64 on Ubuntu
22.04, Linux x86 in Debian 12 i386 userspace, and macOS Intel/Apple Silicon on
native runners. The 32-bit psutil extensions are built from source. PyInstaller
explicitly bundles the CFFI backend. Package checks verify executable bitness,
signed installation, standalone CLI/GUI startup and the live HTTPS catalog.
The x86 checks use WOW64/a 32-bit container on 64-bit hosts, not physical 32-bit
computers. Linux x86 requires the Debian 12 glibc baseline (2.36); these packages
do not establish compatibility with older operating systems.

All six package jobs and the separate publishing job passed in
[Launcher CI 41543483622](https://github.com/P3rlE/q3rally_tryout/actions/runs/41543483622)
at commit `9850f1de8f415a8da1d199ee9f85d95a0af6d83b`. Collected ZIPs, their
aggregate release index, SHA256SUMS and per-platform validation reports are in
`output/launcher-publishing/launcher-platforms-0.2.1/`. The same Windows x64 source
also passed the local installed-package check. Upload the ZIPs first, verify
their public URLs, then publish a new signed catalog sequence; the existing
website/catalog remains unchanged until that step. Physical Mac download and
Gatekeeper checks remain external work.

All delivered launcher ZIPs bundle the Python interpreter and libraries with
PyInstaller. End users do not install Python; they must extract the complete ZIP
and retain the bundled dependency directories. Build-time Python is separate
from this end-user requirement.

## Production preparation

1. Choose the actual webspace path. Generate a release signing key **outside the
   repository**, retain an offline backup, and distribute only its public key:

   ```powershell
   python tools/publish_launcher_catalog.py keygen --private-key E:/Q3RallySecrets/release.pem --public-config launcher/config.json
   ```

   Set `Q3RALLY_SIGNING_PASSWORD` to encrypt the private key. Keep the private key
   off the webserver and out of game packages and CI artifacts.

2. Build natively on each platform with PyInstaller installed:

   ```text
   python tools/build_launcher.py --output output/launcher-production
   ```

   Production builds require a configured public key. `--development` permits
   building an unconfigured review artifact; the catalog publisher rejects those
   artifacts. macOS Developer ID signing/notarization and Windows Authenticode
   signing are not configured yet. Native macOS launch and Gatekeeper behavior
   still require validation on actual hardware.

3. Build game packages with `make_release.py`. The publisher consumes its
   `release.json` and ZIP manifests; old ZIPs lacking manifests need repackaging
   before being offered. This includes v0.7d. Its legacy Ladder policy must also
   be settled before listing it publicly.

4. Prepare a signed catalog. Game URLs use the GitHub release URL; launcher URLs
   use webspace. Increment `--sequence` for every catalog update. `--previous`
   retains older versions and other platform packages.

   ```text
   python tools/publish_launcher_catalog.py catalog --private-key E:/Q3RallySecrets/release.pem --public-config launcher/config.json --release-index output/game/release.json --launcher-index output/launcher-production/release.json --base-url https://github.com/Q3Rally-Team/q3rally/releases/download/v0.8 --launcher-base-url https://www.q3rally.com/launcher/downloads --sequence 1 --output output/catalog.json
   ```

5. Upload game ZIPs to GitHub and launcher ZIPs to webspace; replace the catalog
   last. Renew its expiry before the default 90 days elapse. This tool only writes
   local files and never uploads or deploys anything.

The launcher does not itself prove game authenticity to the Ladder. Web-profile
linking and synchronization remain v0.9 work; v0.8 retains local profile v2.

Implementation references: [Ed25519](https://cryptography.io/en/latest/hazmat/primitives/asymmetric/ed25519/)
and [PyInstaller](https://pyinstaller.org/en/stable/usage.html).
