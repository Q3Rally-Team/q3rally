# v0.8 ladder rollout and reset

The prototype source and reset tooling described below have been removed from
the active engine/services. This is historical documentation only.

> **SUPERSEDED DESIGN / NICHT ZUR VERÖFFENTLICHUNG:** This document describes the
> old rollout/reset prototype. The accepted direction is now a central
> profile with regular offline progress after basic validation. Dedicated-only
> and the replacement scoring formula are not approved product rules. Follow
> [central-profile-model.md](central-profile-model.md) before any implementation,
> deployment or reset. No production reset is authorized by this document.

The v0.8 ladder is a **test ladder**. Before v1.0, scoring can change and
ranked data can be reset. From v1.0 the calculation/ranking rules are frozen;
new profile ranks, achievements and displays may be added without changing
existing points or comparability. The reset CLI refuses stable periods.

## Implemented policy

Only approved dedicated-server reports for the configured period, rules and
release are eligible. v0.7d, unknown releases, missing metadata, old spools,
listen servers and offline keys do not change ranked data. Legacy clients
cannot join ranked dedicated servers; legacy participation while ranking was
disabled leaves the current map unranked. Public reads and local gameplay
remain available. Local profile registration is optional and does not enable
offline ranking. Local careers are not imported into the ranked profile.

PHP installations do not silently initialize a period on upgrade. Until an
administrator initializes it, new reports receive HTTP 503. The web page shows
a test/stable/unavailable notice; GET /api/v1/ladder/status exposes the state.

## Production switch (prepared; not executed)

1. Schedule the switch to v0.8 and pause uploads/administrative changes. Deploy
   the PHP files together, including period.php and reset_ladder.php. Do not
   copy test data or test keys. Retain the existing private accounts/keys.
2. On the server, inspect the default read-only reset plan:

   ```sh
   php reset_ladder.php --period=v0.8-test-1
   ```

3. Apply only at the agreed release switch. Choose a new backup directory
   outside the public web root; its parent must already exist:

   ```sh
   php reset_ladder.php --period=v0.8-test-1 --backup=/srv/backups/q3rally-before-v08 --apply
   ```

   The CLI acquires the same write lock as report ingestion, enables
   maintenance, copies and SHA256-verifies persistent data, then clears
   matches, indices, ghosts and ranked profiles. Private accounts/API keys
   (including approval and identity bindings) and legacy key files remain.
   Runtime lock/maintenance files are excluded from backups. Keep the backup
   private: it includes API credentials. The CLI is inaccessible over HTTP.
   A failure leaves maintenance enabled, retaining the backup for recovery.
   Pause requests and restore the data backup before resuming after a failure.
4. Run v0.8 dedicated servers with `sv_ladderEnabled 1`, their approved server
   key and `sv_ladderPeriod v0.8-test-1`. Reload the full map after changing
   the latched period; do not relabel queued reports. Verify status, one real
   accepted match and profile/index points, and rejection of an old report.
5. Resume service. Existing player UUIDs and local careers remain intact;
   ranked profiles are populated afresh from accepted matches. Prior results
   exist only in the backup until separately published as an archive.

Later test resets require a never-used period id, e.g. v0.8-test-2, followed
by the same id on reporting servers. The tool tracks previous ids to prevent
reuse. The current reset initializer targets v0.8 test rules/releases; v0.9
and v1.0 need an explicit release-policy update. Do not mark v0.8 stable.

## Alternative Python API

New writes are disabled unless LADDER_PERIOD_CONFIG points to a valid period
JSON and LADDER_REPORT_KEY supplies the trusted reporter credential. All
writers with that credential must be approved dedicated servers. Public reads
remain unauthenticated; deletes require the reporter credential. Use a fresh
backed-up SQLite database for each period, not a config-only switch over old
rows. An old database is rejected when its first stored report has another
period. This API stores accepted matches; PHP owns the deployed profile and
frontend implementation. No production Python database was reset.

Example period state (PHP data/private/ladder_period.json):

```json
{"id":"v0.8-test-1","phase":"test","rules":"v0.8-test-1","releases":["v0.8"],"previousPeriods":[]}
```

Version/revision markers are public compatibility data, not authentication of
an executable. A modified client/reporter can forge them. The trust boundary
is the approved reporting server; this work does not implement anti-cheat.

## Local validation

PHP/engine and Python regression logs, native Windows UDP admission probes,
real HTTP report capture and Windows/Linux build logs are retained under
output/ladder-version-review/. All service data and credentials used in those
checks are synthetic. No production endpoint was contacted or reset.

Latest checks (2026-11-02):

- 60 PHP/engine tests pass, including 15 scoring modes, current/legacy period
  admission, ignored local snapshots, old profile baselines, ghosts, reset,
  private key retention, idempotency and departed legacy clients.
- 34 alternative Python API tests pass against the pinned synchronous API
  dependencies in an isolated WSL Python environment.
- 25 additional existing PHP profile/mode cases pass. On Windows their PHP-only
  AST nodes were executed separately from the same shared test source because
  the Python API dependencies are tested under WSL.
- Windows and Linux client/server/native game/QVM release builds pass with
  v0.8 in both the Makefile and the fallback product header. Optional profile
  registration text now explicitly says local games do not affect ranking.
- A native Windows ranked server rejects a legacy-shaped protocol-71 handshake
  without the new client markers and accepts a current-marker handshake.
  This is a wire-level probe, not a full old-binary graphics/gameplay test.
- A real one-minute native Windows bot match emits a valid v0.8 report with
  zero payload validation errors. The unchanged captured report receives
  HTTP 201 from a separate actual PHP service, with no human profile credit
  for the two bots. See wire-confirmed/report.json and
  native-payload-backend/results.json under the evidence directory.

macOS binaries/hardware were not retested for this change. The earlier
physical-Mac graphics/audio checklist remains separate.
