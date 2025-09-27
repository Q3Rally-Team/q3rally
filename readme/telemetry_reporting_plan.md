# Q3Rally Telemetry Reporting Plan

This document captures the planned telemetry payloads and delivery mechanics for automated match reporting. It targets the dedicated server build described in this repository and focuses on race-centric modes introduced by Q3Rally alongside the inherited arena game types.

## 1. Metrics by Game Mode

All payloads share a common player object structure:

```json
{
  "playerId": "sha256:...",   // hash of cl_guid + name
  "displayName": "Player",
  "team": "red" | "blue" | null,
  "car": "carmodel/skin",
  "normalizedScore": 0.0,
  "rawScore": 0,
  "damageDealt": { "raw": 1234, "normalized": 0.52 },
  "damageTaken": { "raw": 800, "normalized": 0.33 },
  "timeAlive": "PT12M3.542S"
}
```

* **Player identification:** use the server-side `cl_guid` (see `cl_main.c`), salt it with the server-provided namespace, and hash it with SHA-256 to keep PII out of transit.
* **Time format:** represent all absolute timestamps as RFC 3339 UTC strings and all durations as ISO 8601 durations (`PTmmMss.mmmS`). Game logic already keeps race timers in milliseconds, so conversion happens right before serialization.
* **Normalization:** map raw metrics to `0.0–1.0` floats per match scope (e.g., divide by session max or configured cap). Where the engine already reports percentages (`accuracy`), normalization divides by `100`.

### 1.1 Racing (GT_RACING, GT_RACING_DM)

| Metric | Description | Normalization |
| --- | --- | --- |
| `completedLaps` | Number of completed laps | Divide by `track.totalLaps` from the level info |
| `bestLap` | Fastest lap time | Duration as ISO 8601, plus `normalizedBestLap = bestLapMs / sessionBestLapMs` |
| `totalTime` | Total time until finish flag | Duration; `normalizedTotalTime = winnerTimeMs / playerTimeMs` (>= 1.0 capped at 1.0) |
| `position` | Placement according to scoreboard | `normalizedPosition = 1 - ((position-1)/(gridSize-1))` |
| `startReaction` | Time between green light and start | Normalize against `g_eliminationStartDelay` if set |
| `boostUsage` | Percentage of boost time used | Raw value / `g_maxBoostMs` (new server CVar, see section 3) |

For `GT_RACING_DM`, `damageDealt` and `damageTaken` are also taken from the scoreboard.

### 1.2 Team Racing (GT_TEAM_RACING, GT_TEAM_RACING_DM)

In addition to individual racing metrics:

* `teamScoreFraction` = team total `score` / sum of all team scores
* `teamDamageShare` = player’s share of team damage (`playerDamage / teamDamage`)
* `relaySegments` = number of relay hand-offs (for potential relay modes if `g_teamRelay` is enabled)

### 1.3 Elimination & Last Car Standing (GT_ELIMINATION, GT_LCS)

| Metric | Description | Normalization |
| --- | --- | --- |
| `survivalTime` | Duration until elimination | Duration + share relative to longest survival time |
| `elimOrder` | Order of elimination | `normalizedElimOrder = 1 - ((elimOrder-1)/(gridSize-1))` |
| `lapsAtElim` | Laps completed before elimination | Fraction of `track.totalLaps` |
| `warningResponses` | Reaction time to elimination warnings | Average delay / `g_eliminationWarning` |

### 1.4 Demolition Derby (GT_DERBY)

| Metric | Description | Normalization |
| --- | --- | --- |
| `knockouts` | Number of opponents eliminated | Divide by `maxOpponents` |
| `ringOuts` | Self-inflicted eliminations | Inverse normalized: `1 - min(selfRingOuts, limit)/limit` |
| `armorIntegrity` | Average remaining HP | Average / `maxHealth` (from car definition) |
| `damageEfficiency` | `damageDealt / damageTaken`, scaled 0–1 via `tanh` |

### 1.5 Deathmatch & Team Deathmatch (GT_DEATHMATCH, GT_TEAM)

| Metric | Description | Normalization |
| --- | --- | --- |
| `frags` | Kills | Divide by highest frag count in match |
| `deaths` | Deaths | `1 - (deaths / maxDeaths)` |
| `accuracy` | Hit accuracy | `cg.scores[i].accuracy / 100` |
| `streaks` | Longest kill streak | Divide by `fraglimit` (or session max) |
| `powerupUptime` | Time with active power-ups | Duration / match duration |

Team variant adds `teamScoreFraction` and `teamDamageShare` as in team racing.

### 1.6 Capture the Flag & Variants (GT_CTF, GT_CTF4)

| Metric | Description | Normalization |
| --- | --- | --- |
| `captures` | Flags captured | Divide by `capturelimit` or match max |
| `returns` | Flags returned | Divide by highest number in match |
| `carrierKills` | Flag carrier kills | Divide by session max |
| `escortTime` | Time near own carrier | Duration / match duration |
| `assistCount` | Scoreboard `assistCount` divided by session max |

### 1.7 Domination (GT_DOMINATION)

| Metric | Description | Normalization |
| --- | --- | --- |
| `controlTicks` | Controlled time slots | Divide by total number of slots |
| `contestedEvents` | Number of forced neutralizations | Divide by session max |
| `objectiveDamage` | Damage dealt to control points | Damage / highest point damage |

### 1.8 Single Player (GT_SINGLE_PLAYER)

Single player uses racing metrics plus `aiDifficulty` and `retryCount` (number of restarts).

## 2. Web API Design

### 2.1 Transport & Authentication

* **Protocol:** HTTPS with TLS 1.2+
* **Method:** `POST /v1/matches` for end-of-match reports; optional `POST /v1/pings` for heartbeats
* **Authentication:** static API key via header `Authorization: Bearer <key>`. Replay protection via `X-Q3R-Timestamp` (Unix ms) and HMAC signature in header `X-Q3R-Signature` (`HMAC-SHA256` over body)

### 2.2 Request Schema (`/v1/matches`)

```json
{
  "matchId": "srv-20240405-183011-42",
  "server": {
    "name": "Q3Rally EU #1",
    "host": "203.0.113.10:27960",
    "build": "1.3.0",
    "map": "q3r_country01"
  },
  "mode": "GT_RACING",
  "startTime": "2024-04-05T18:30:11Z",
  "endTime": "2024-04-05T18:42:39Z",
  "duration": "PT12M28S",
  "settings": {
    "g_gametype": 141,
    "g_eliminationInterval": 15000,
    "g_vehicleHpMaxRatio": 1.2
  },
  "players": [ { ...player metrics... } ],
  "teams": [
    {
      "team": "red",
      "rawScore": 123,
      "normalizedScore": 0.64,
      "damageDealt": 4200,
      "objectives": {
        "captures": 2,
        "controlTicks": 35
      }
    }
  ],
  "events": [
    {
      "timestamp": "2024-04-05T18:33:15.210Z",
      "type": "lap_completed",
      "playerId": "sha256:...",
      "lap": 2,
      "lapTime": "PT1M12.250S"
    }
  ]
}
```

* `mode` matches `gametype_t` enum in the code (e.g., `GT_RACING`)
* `settings` reflects relevant CVars so analysts can understand game parameters (e.g., `g_eliminationInterval`)
* `events` are optional and can be enabled only when fine-grained tracking is desired (configurable switch `sv_telemetryEvents`)

### 2.3 Response Schema

```json
{
  "matchId": "srv-20240405-183011-42",
  "status": "accepted",
  "ingestedAt": "2024-04-05T18:42:40Z",
  "nextPollAfter": 0,
  "warnings": [
    { "code": "metrics/unknown", "detail": "Field settings.g_vehicleHpMaxRatio ignored" }
  ]
}
```

* `status` values: `accepted`, `queued`, `rejected`
* For `rejected`, response includes `errors` with field paths
* HTTP status 202 for `queued`, 200 for `accepted`, 400 for schema errors, 401/403 for auth issues, 429 for rate limits

### 2.4 Error & Retry Strategy

* Server stores last 10 payloads in local spool (`telemetry/outbox`)
* On HTTP 5xx or network errors: exponential backoff (start 5 s, cap 5 min) and retry POST
* A `202 queued` requires polling via `GET /v1/matches/{matchId}` (optional) – not enabled by default

## 3. Server Configuration

### 3.1 New CVars

| CVar | Default | Description |
| --- | --- | --- |
| `sv_telemetryEnabled` | `0` | Global switch – only if `1` payloads are generated and sent |
| `sv_telemetryUrl` | `` | HTTPS endpoint (e.g., `https://telemetry.example.com/v1/matches`) |
| `sv_telemetryApiKey` | `` | Secret for `Authorization` header; stored as latched CVar (not in `serverinfo`) |
| `sv_telemetryTimeoutMs` | `5000` | Network timeout for POSTs |
| `sv_telemetryEvents` | `0` | Enables optional `events` array |
| `sv_telemetryMaxBatch` | `8` | Number of match reports stored in spool |
| `sv_telemetryNamespace` | `"default"` | Salt for player hashes to avoid GUID collisions between communities |
| `sv_telemetryMaxBoostMs` | `15000` | Upper bound for normalization of boost times (used in race metrics) |

New CVars follow the same pattern as existing server control options (see `sv_enableRankings` & `sv_rankingsActive`).

### 3.2 Config File

* Extend `server_example.cfg` with commented defaults and recommend storing secrets in separate `telemetry_secrets.cfg`, included via `exec telemetry_secrets.cfg`
* Deployment documentation links this file and notes file permissions (readable only by server user)

### 3.3 Activation Procedure

1. Admin enters target URL and API key in `telemetry_secrets.cfg`
2. Enables telemetry with `seta sv_telemetryEnabled "1"` and optionally `sv_telemetryEvents`
3. Server initializes outbox at startup, generates namespaces (`sv_telemetryNamespace`), and begins POSTs starting with next match
4. Health monitor in-game (new command `telemetryStatus`) lists last response codes

### 3.4 Security Aspects

* `sv_telemetryApiKey` is implemented as a latched, write-only CVar, like `rconPassword`, so it is not leaked via `serverinfo` or status responses
* Logging minimized: success messages only on debug level, errors (with HTTP status) on warning level
* Optional IP allowlist (`sv_telemetryAllowedIPs`) can be added later

---

This plan provides a consistent data basis across all game modes, takes existing engine data structures into account, and describes infrastructure extensions compatible with existing server configuration mechanisms.
