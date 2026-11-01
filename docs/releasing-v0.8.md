# Q3Rally v0.8 release packaging

`make_release.py` uses Python 3.11+ and the standard library. It packages existing
builds; it does not compile, publish, download, sign or install anything.

## Audit first

```powershell
python make_release.py --audit-only --report dist/release-asset-audit.json
python make_release.py --dry-run --version 0.8 --target windows-x86_64=engine/build/release-mingw32-x86_64 --report dist/release-preflight.json
```

A failed audit/preflight exits with status 1 and creates no release archive. The
report contains all findings; the console prints the first ten errors. Warnings
are separate: MD3 embedded materials may be replaced by skins; compiler-only
references and editor preview images are not runtime blockers.

Static checks cover shader textures and skyboxes, skin materials (including
extensionless shader lookup), arena map names, BSP surface materials/entity
assets, MD3 structure/materials, scripted object models/sounds and cfg execs.
Only drawn BSP material references are mandatory; unused/collision-only materials
are warnings. `misc_model` source files are compiler-only references. Dynamic
code paths, menu/bot scripts and runtime dependency closure still need launch
tests. Existence checks are not a complete image/audio decoder validation.

## Build inputs

Build each platform with the intended version, for example `make ... VERSION=v0.8`.
Current development binaries still identify as v0.7d; the script deliberately
refuses to label them v0.8. Version matching checks the client's embedded
`Q3Rally vVERSION` marker, not a cryptographic build attestation. Verify QVM
provenance separately and do not mix code from different release builds.

Windows input: client, dedicated server, both renderer DLLs, SDL2.dll and all
required runtime DLLs. Linux input: client, dedicated server, both renderer .so
files and any vendored libraries in the root or lib/. System dependencies are
not collected automatically. Linux ARM64 needs a client build; the existing
server-only CI artifact is insufficient for a full game release.

macOS input: a prepared unsigned Q3Rally.app, using the existing
`engine/make-macosx-app.sh` workflow. Frameworks and internal relative symlinks
are retained. Bundled data goes into Contents/MacOS/baseq3r, matching the existing
bundle layout. Signed app inputs are rejected because inserting data would
invalidate their signatures. Signing/notarization of the assembled app and its
final distribution package remains a native macOS release-pipeline step.

All targets require cgame.qvm, qagame.qvm and ui.qvm. By default these come from
the build's baseq3r/vm directory. `--qvm-dir` can specify one common directory
of platform-independent VMs; for a direct .app input the default is its
Contents/MacOS/baseq3r/vm directory.

## Package

```powershell
python make_release.py --version 0.8 --target windows-x86_64=engine/build/release-mingw32-x86_64 --output dist/releases/v0.8
```

Multiple `--target` arguments assemble one release set, for example
`linux-x86_64=/path/to/linux-build` and `macos-arm64=/path/to/macos-build`.
Supported target names are windows-x86/windows-x86_64, linux-x86/linux-x86_64/
linux-arm64 and macos-x86/macos-x86_64/macos-arm64/macos-universal. These declare
package targets; architecture compatibility must also be checked by the build
pipeline. Prepared native files are never executed by the packager.

Each platform ZIP has its own top-level folder, runtime files, all three VMs,
baseq3r/pak0.pk3 (inside the app on macOS), an end-user README, changelog,
engine license, available top-level vendor license files, and release-manifest.json
with per-file sizes and SHA-256 hashes. The manifest also lists individual
virtual asset hashes inside the PK3.

The output directory also contains SHA256SUMS and release.json with archive
sizes/hashes. That catalog explicitly declares signed=false; checksums alone
are not authentication and this is not yet the launcher's trusted update feed.
A signing step is needed when implementing the updater.

Existing release artifacts are never overwritten. Files are built in a temporary
staging directory, published only when complete, and the catalog is published
last. A publication failure rolls back files created by that invocation. Timestamps
and ordering are fixed; identical inputs and toolchain produce identical ZIPs.
Keep inputs unchanged during packaging. Use a fresh output directory for retries.

## Content policy

`tools/release_policy.json` explicitly controls asset directories, file extensions,
an explicit list of root configs and exclusions. Loose files override PK3 entries, matching this
engine's VFS; later PK3 names override earlier packages. Existing packages are
flattened into the audited pak0.pk3. Bytecode comes exclusively from --qvm-dir or
the selected build, not an old asset package.

Profiles, personal q3config.cfg, demos, logs, editor map sources, backups, build
objects and known test maps are excluded. Runtime object scripts, OGV cinematics,
bot script source files and extensionless asset credits/readmes are retained.
Check the policy whenever adding a new runtime file format. Unsafe archive paths,
asset symlinks and case-colliding loose assets are rejected. Developer source
packages and Q3RallyRadiant are separate deliverables, not included here.

## Tests and remaining release gate

```powershell
python -m unittest tests.test_make_release -v
```

Seven integration/regression tests exercise synthetic Windows/Linux/macOS builds,
archive contents, per-file hashes, reproducibility, executable modes, PK3 overlays,
private-file exclusion, malformed assets, version mismatch, missing files,
publication rollback and dry-run behavior. This does not replace actual native
launch/install tests. No final v0.8 distribution was produced during implementation:
see asset-audit-v0.8.md for current blockers.
