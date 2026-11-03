# Release procedure

1. Start from official master and assign the intended source version in
   `engine/Makefile` and the fallback in `engine/code/qcommon/q_shared.h`.
   Update the changelog. Never relabel old binaries as a new game version.
2. Run focused tests, then dispatch **Release packages** on the exact candidate
   branch/commit. It derives the version from the source and builds Windows/Linux
   x86/x86_64 and macOS Intel/ARM. Audits and installed-package checks must pass.
3. Review all six artifacts and retain their source commit/tree, validation
   reports and remaining manual-test limitations. A failed job's partial artifact
   is not a release package. Physical Mac checks remain separate.
4. Merge the reviewed source into official master. Create a release tag whose
   source tree matches the tested candidate. Publish the six exact ZIPs with
   `release.json`, `SHA256SUMS` and collected source/provenance metadata. Verify
   uploaded sizes/hashes and the release tag's tree before changing the catalog.
5. Use `tools/publish_launcher_catalog.py --help` to prepare the signed catalog
   with all compatible versions. Keep the private key outside Git. Increment the
   current production/test sequence numbers independently; do not reuse old
   release-specific sequence numbers. Confirm download URLs, expiry and hashes.
6. Deploy the reviewed signed catalog files to `/var/www/q3rally-launcher/` and
   atomically replace `catalog.json`/`catalog-test.json` with readable files.
   Update `/var/www/ladder/version.txt` with version, date and website URL.
   Static metadata changes do not require an Apache restart or a PHP deployment.
7. Verify the public signatures, sequence/version, downloads and real launcher
   install/start flow. Preserve old published packages and per-version user data.
   Announce downloads through https://www.q3rally.com/ so players use the launcher.

The version feed has three lines, for example:

```text
v0.8.1
2026-11-03
https://www.q3rally.com/
```

See [packaging details](releasing-v0.8.md), [launcher](launcher.md),
[roadmap](roadmap.md), [asset review](asset-audit-v0.8.md) and
[repository workflow](repository-workflow.md). A game patch does not implicitly
authorize a Ladder data reset or deployment of historical PHP prototypes.
