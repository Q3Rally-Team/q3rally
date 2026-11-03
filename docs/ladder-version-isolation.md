# Ladder version isolation review (2026-11-02)

The prototype source has been removed from the active engine/services. Findings
below remain historical context, not an implemented v0.8 admission policy.

> **SUPERSEDED DESIGN / NICHT ZUR VERÖFFENTLICHUNG:** This document describes the
> historical review and first prototype. The accepted direction is now a central
> profile with regular offline progress after basic validation. Dedicated-only
> and the replacement scoring formula are not approved product rules. Follow
> [central-profile-model.md](central-profile-model.md) before any implementation,
> deployment or reset. No production reset is authorized by this document.

## Scope and evidence

Current checkout: 571502d5b8329946fef3b605873904781aaf3e3c (P3rlE/q3rally_tryout).
Historical upstream snapshots examined:
- a753dd1c025b057bc473a77c90f08cd904450d03, June 6 (OGV/Theora fix).
- 351af8d62d8b3d73b2fb4491069e4af7d7c503b5, June 7 (loading screens).
Both define PRODUCT_VERSION as v0.7d. These bracket the supplied release-era
history; neither is claimed to identify the exact published binary package.
Downloaded files and the filtered upstream commit list are retained under
output/ladder-version-review/historical and upstream-commits.json.

## Findings

1. The historical and current g_main.c read the server's `version` cvar into
   serverBuild. sv_ladder.c serializes this as server.build. HTTP uses a Bearer
   server key; this authenticates a reporter, not its executable version.
2. Current PHP handle_post/normalize_ingest_payload and keys_require_auth do
   not enforce an allowed game release. An approved key can submit v0.7d,
   v0.8 or missing build metadata. A copied real PHP service was tested on
   localhost with a synthetic approved key and distinct synthetic players.
   All three returned HTTP 201 and persisted both match and profile files.
   Evidence: output/ladder-version-review/results.json and probe.py. This
   checks acceptance of representative payloads, not execution of a v0.7d binary.
3. There is no per-player client release field in the current ladder payload.
   Historical/current network protocol is 71. Protocol compatibility alone
   does not distinguish old clients on a newer server; other compatibility
   and pure-package constraints may still prevent particular combinations.
   No mixed-version connection was executed in this review.
4. PHP ghost uploads also authenticate the reporter without a game-release
   gate. Physics/map buckets do not establish the executable's release.
5. The alternative Python API accepts an optional server.build and has no
   version gate in create_match. Its shown match route also has no Bearer
   authentication dependency. Do not assume PHP's authentication exists there.
6. Profile ingestion uses cumulative career snapshots as well as match data.
   Merely blocking old direct submissions does not address old offline
   statistics carried into a later accepted profile snapshot. Ranked stats
   must be derived from accepted matches or isolated from imported career data.

## Recommended implementation before offering legacy versions

- Enforce explicit supported releases/report revisions at the backend before
  match, index, profile or ghost mutation. Missing/malformed/old metadata must
  fail closed with a clear permanent rejection. Apply equivalent policy to
  each enabled backend/write endpoint. Keep public historical reads available.
- For v0.8 ranked servers require the supported client release/protocol and
  reject legacy clients before participation. Prevent old clients from
  influencing winners, team scores or other participants' ranked results;
  merely omitting an old player's profile is insufficient. Keep legacy games
  playable on separate unranked servers.
- Separate v0.7d homes/profiles/configs/spools from v0.8 in the launcher, omit
  ladder credentials, and disable uploads as defense in depth. Do not import
  cumulative legacy career counters into ranked statistics.
- Keep a frozen v0.7d wire-format fixture and test old/missing/malformed builds,
  supported reports, ghost writes, mixed-client admission and zero stat/file
  mutation on rejection. Test offline spooled legacy reports after upgrading.
- Existing historical ladder data remains unchanged unless a separate
  migration/reset is explicitly requested. Deploy the backend gate before
  promoting v0.8 as the exclusively ranked release.

## Assurance boundary

A version field, User-Agent or new public report revision is not proof of an
unmodified binary. These measures can exclude stock legacy releases; they do
not stop a modified game or a trusted server operator forging current metadata
and results. Embedded client secrets would not solve that. Stronger adversarial
assurance requires a trusted reporting-server policy and server-derived ranked
statistics. The production service was neither contacted nor modified.

The review above describes the pre-change state. The v0.8 implementation now
adds period/release admission, match-derived profiles and a guarded reset CLI;
see ladder-v08-rollout.md. Production activation/reset remains pending.
