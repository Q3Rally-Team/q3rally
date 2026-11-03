# Incremental game updates (Launcher v0.4.0)

Game v0.8.2 includes the three user-confirmed plate, Canyon Cruise and Add Bots
fixes. Launcher v0.4.0 can install it from a verified v0.8.1 base using an optional
signed update instead of transferring the full archive. Full packages remain
available for new installations, older launchers and failed/missing bases.

## Format and integrity

Full package records retain their schema-1 fields. The optional `updates` list
contains `from_version`, `from_manifest_sha256`, `url`, `size` and `sha256`.
Only older versions within the same major/minor line qualify. The complete
catalog is signed as before; old launchers ignore the extra fields and use full
downloads. The publisher binds each update to the published base manifest and
the full target manifest.

The update ZIP contains `update.json`, the exact target `release-manifest.json`
and numbered `data/` payloads. Each target file is either copied from the verified
base, replaced, or rebuilt from bounded byte ranges plus new payloads. For PK3s,
the builder reuses byte-identical compressed member records and transfers the
new central directory verbatim. The reconstructed PK3 is byte-for-byte identical
to the full release; no compression library or binary-diff dependency is added.

Installation holds the normal version locks, verifies the source installation,
constructs a separate staging directory and verifies every final file against
the signed target manifest before activation. The old installation is never
modified. Invalid deltas fall back to the full package; user cancellation stops
without silently starting a full download. Copying complete files avoids hard
links that could accidentally mutate older versions. Paths, sizes, operations
and symlinks remain bounded and checked.

On the first start of a new patch version, existing settings, profiles,
credentials, demos and user content are copied into its separate home if that
home does not already exist. The source game must be closed. Existing target
data is never overwritten. Major/minor line changes do not auto-copy user data.

## Build and publish

1. Build/audit/validate the full target packages using `make_release.py`.
2. Download and hash-check the matching published base packages.
3. Run `tools/make_incremental_update.py --base-index BASE/release.json
   --release-index TARGET/release.json --output TARGET`.
4. Validate installation of both full and incremental packages. The release
   workflow accepts `base_version` (default `0.8.1`); empty disables updates.
   Every native target checks a real incremental install without a full target
   download and re-verifies its unchanged base.
5. Publish the full ZIPs and `*.update.zip` assets, `release.json`, `updates.json`,
   `SHA256SUMS` and build evidence on the game GitHub release.
6. Upload Launcher v0.4.0 ZIPs to own webspace. Use the publisher's
   `--update-index TARGET/updates.json` when preparing the signed game catalog.
   `--update-base-url` may specify a separate update directory.

Checksums and source/tag provenance must match before catalog activation. No
Ladder PHP deployment, score reset or profile-format migration is required.
The launcher native matrix tests the actual frozen executable doing an update
with external Python/Tk environment variables removed.
