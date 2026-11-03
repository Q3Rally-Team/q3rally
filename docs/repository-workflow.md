# Repository workflow

## Source of truth

`Q3Rally-Team/q3rally` is the authoritative source for fixes, releases and tags.
`P3rlE/q3rally_tryout` is retained for experiments and isolated CI investigations.
Synchronize Tryout's master from the official master by fast-forward; do not
maintain two independently changing release branches. Submit accepted experiments
to the official repository through focused pull requests.

Before a patch, fetch both repositories, start a `codex/` or normal feature branch
from official master, keep changes scoped and run the relevant checks. Update
the source version only when preparing that patch. Release tags and published
archives remain immutable.

## Active CI

- `build.yml`: unit tests and engine builds on master/dev pushes and pull requests.
- `launcher.yml`: native six-target launcher builds on relevant changes, or manually.
- `release-packages.yml`: manual six-target game packages with source-derived
  version, asset audit, signed-install validation, dedicated lap/Sprint and
  loopback Offline Ladder registration probes. This never publishes.
- `client-runtime.yml` and `macos-dedicated-races.yml`: manual Mac diagnostics.

The one-off v0.8/v0.8.1 packaging and legacy duplicate macOS workflows are removed;
their original definitions and evidence remain in the release tags and GitHub
Actions history. The successful v0.8.1 six-platform workflow is the basis for
`release-packages.yml`.

## Local files and recovery

`output/`, `dist/` and the root `ladder.db`/SQLite sidecars are local artifacts,
not source. Keep useful release packages and diagnostic evidence locally. Private
signing keys, credentials and player data must not be committed.

The 2026-11-03 cleanup saved the Tryout changes in
`output/repository-cleanup/tryout-before-cleanup.zip` and a named Git stash.
All 92 release-source files in that snapshot matched official v0.8.1 after line
ending normalization; two additional files were obsolete launcher diagnostics.
The second checkout's five pre-existing deleted maptools files are preserved
in a separate named stash and patch. Do not apply those deletions to a release.

Keep backups/stashes until the cleanup is accepted. Retire an integrated branch
only after checking open PRs and proving that its exact tip is an ancestor of the
released official master. Its commits remain reachable through master and the
v0.8.1 tag. Retain the branch-name/tip inventory in `docs/history/` for recovery.
Unmerged branches, protected branches and master/dev/main/develop are retained.
Do not rewrite master history.
