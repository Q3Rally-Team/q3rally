# Q3Rally Telemetry Reporting Plan

## English

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

---

## Deutsch

Dieses Dokument beschreibt die geplanten Telemetrie-Payloads und die Auslieferungsmechanismen für die automatische Match-Berichterstattung. Es richtet sich an den in diesem Repository dokumentierten Dedicated-Server-Build und legt den Schwerpunkt auf die von Q3Rally eingeführten rennzentrierten Modi sowie die übernommenen Arena-Spieltypen.

## 1. Kennzahlen pro Spielmodus

Alle Payloads teilen sich eine gemeinsame Spielerobjekt-Struktur:

```json
{
  "playerId": "sha256:...",   // Hash aus cl_guid + Name
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

* **Spieleridentifikation:** Verwende das serverseitige `cl_guid` (siehe `cl_main.c`), salze es mit dem Server-Namespace und hashe es mit SHA-256, um personenbezogene Daten aus der Übertragung herauszuhalten.
* **Zeitformat:** Stelle absolute Zeitpunkte als RFC-3339-UTC-Strings und Dauern als ISO-8601-Dauern (`PTmmMss.mmmS`) dar. Die Spiel-Logik verwaltet Rennzeiten bereits in Millisekunden; die Umwandlung erfolgt unmittelbar vor der Serialisierung.
* **Normalisierung:** Mappe Rohwerte auf Floats `0,0–1,0` pro Match-Kontext (z. B. Division durch Sitzungsmaximum oder konfigurierte Obergrenze). Wo die Engine bereits Prozentwerte liefert (`accuracy`), erfolgt die Normalisierung durch Division durch `100`.

### 1.1 Racing (GT_RACING, GT_RACING_DM)

| Kennzahl | Beschreibung | Normalisierung |
| --- | --- | --- |
| `completedLaps` | Anzahl absolvierter Runden | Division durch `track.totalLaps` aus den Level-Infos |
| `bestLap` | Schnellste Rundenzeit | Dauer als ISO 8601 sowie `normalizedBestLap = bestLapMs / sessionBestLapMs` |
| `totalTime` | Gesamtzeit bis zur Zielflagge | Dauer; `normalizedTotalTime = winnerTimeMs / playerTimeMs` (>= 1,0 wird auf 1,0 gekappt) |
| `position` | Platzierung laut Scoreboard | `normalizedPosition = 1 - ((position-1)/(gridSize-1))` |
| `startReaction` | Zeit zwischen grünem Licht und Start | Normalisierung gegen `g_eliminationStartDelay`, sofern gesetzt |
| `boostUsage` | Prozentual genutzte Boost-Zeit | Rohwert / `g_maxBoostMs` (neue Server-CVar, siehe Abschnitt 3) |

Für `GT_RACING_DM` werden `damageDealt` und `damageTaken` zusätzlich aus dem Scoreboard übernommen.

### 1.2 Team Racing (GT_TEAM_RACING, GT_TEAM_RACING_DM)

Zusätzlich zu den individuellen Rennmetriken:

* `teamScoreFraction` = Team-Gesamt-`score` / Summe aller Team-Scores
* `teamDamageShare` = Anteil des Team-Schadens (`playerDamage / teamDamage`)
* `relaySegments` = Anzahl der Staffelstab-Übergaben (für mögliche Relay-Modi bei aktivem `g_teamRelay`)

### 1.3 Elimination & Last Car Standing (GT_ELIMINATION, GT_LCS)

| Kennzahl | Beschreibung | Normalisierung |
| --- | --- | --- |
| `survivalTime` | Dauer bis zur Eliminierung | Dauer + Anteil relativ zur längsten Überlebenszeit |
| `elimOrder` | Reihenfolge der Eliminierung | `normalizedElimOrder = 1 - ((elimOrder-1)/(gridSize-1))` |
| `lapsAtElim` | Runden bei der Eliminierung | Bruchteil von `track.totalLaps` |
| `warningResponses` | Reaktionszeit auf Eliminierungswarnungen | Durchschnittliche Verzögerung / `g_eliminationWarning` |

### 1.4 Demolition Derby (GT_DERBY)

| Kennzahl | Beschreibung | Normalisierung |
| --- | --- | --- |
| `knockouts` | Anzahl ausgeschalteter Gegner | Division durch `maxOpponents` |
| `ringOuts` | Selbst verursachte Eliminierungen | Invers normalisiert: `1 - min(selfRingOuts, limit)/limit` |
| `armorIntegrity` | Durchschnittliche Rest-HP | Mittelwert / `maxHealth` (aus der Fahrzeugdefinition) |
| `damageEfficiency` | `damageDealt / damageTaken`, auf 0–1 skaliert via `tanh` |

### 1.5 Deathmatch & Team Deathmatch (GT_DEATHMATCH, GT_TEAM)

| Kennzahl | Beschreibung | Normalisierung |
| --- | --- | --- |
| `frags` | Kills | Division durch höchste Fragzahl des Matches |
| `deaths` | Tode | `1 - (deaths / maxDeaths)` |
| `accuracy` | Treffergenauigkeit | `cg.scores[i].accuracy / 100` |
| `streaks` | Längste Kill-Serie | Division durch `fraglimit` (oder Sitzungsmaximum) |
| `powerupUptime` | Zeit mit aktiven Power-Ups | Dauer / Matchdauer |

Team-Varianten ergänzen `teamScoreFraction` und `teamDamageShare` wie beim Team-Racing.

### 1.6 Capture the Flag & Varianten (GT_CTF, GT_CTF4)

| Kennzahl | Beschreibung | Normalisierung |
| --- | --- | --- |
| `captures` | Eroberte Flaggen | Division durch `capturelimit` oder Match-Maximum |
| `returns` | Zurückgebrachte Flaggen | Division durch höchste Zahl im Match |
| `carrierKills` | Kills am Flaggen-Träger | Division durch Sitzungsmaximum |
| `escortTime` | Zeit in der Nähe des eigenen Trägers | Dauer / Matchdauer |
| `assistCount` | Scoreboard-`assistCount` geteilt durch Sitzungsmaximum |

### 1.7 Domination (GT_DOMINATION)

| Kennzahl | Beschreibung | Normalisierung |
| --- | --- | --- |
| `controlTicks` | Kontrollierte Zeitslots | Division durch Gesamtanzahl der Slots |
| `contestedEvents` | Anzahl erzwungener Neutralisierungen | Division durch Sitzungsmaximum |
| `objectiveDamage` | Schaden an Kontrollpunkten | Schaden / höchster Punkt-Schaden |

### 1.8 Single Player (GT_SINGLE_PLAYER)

Der Einzelspielermodus nutzt die Rennmetriken sowie `aiDifficulty` und `retryCount` (Anzahl der Neustarts).

## 2. Web-API-Design

### 2.1 Transport & Authentifizierung

* **Protokoll:** HTTPS mit TLS 1.2+
* **Methode:** `POST /v1/matches` für Match-Endberichte; optional `POST /v1/pings` für Heartbeats
* **Authentifizierung:** Statischer API-Schlüssel über den Header `Authorization: Bearer <key>`. Wiedergabeschutz mittels `X-Q3R-Timestamp` (Unix-ms) und HMAC-Signatur im Header `X-Q3R-Signature` (`HMAC-SHA256` über den Body)

### 2.2 Request-Schema (`/v1/matches`)

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

* `mode` entspricht dem `gametype_t`-Enum im Code (z. B. `GT_RACING`).
* `settings` spiegelt relevante CVars wider, damit Analysten die Spielparameter nachvollziehen können (z. B. `g_eliminationInterval`).
* `events` ist optional und kann nur dann aktiviert werden, wenn ein feingranulares Tracking gewünscht ist (konfigurierbarer Schalter `sv_telemetryEvents`).

### 2.3 Response-Schema

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

* `status`-Werte: `accepted`, `queued`, `rejected`
* Bei `rejected` enthält die Antwort `errors` mit Feldpfaden
* HTTP-Status 202 für `queued`, 200 für `accepted`, 400 für Schemafehler, 401/403 für Auth-Probleme, 429 für Ratenbegrenzung

### 2.4 Fehler- und Retry-Strategie

* Der Server speichert die letzten 10 Payloads in einem lokalen Spool (`telemetry/outbox`).
* Bei HTTP-5xx oder Netzwerkfehlern: exponentieller Backoff (Start 5 s, Kappung 5 min) und erneuter POST-Versuch.
* Ein `202 queued` erfordert optionales Polling über `GET /v1/matches/{matchId}` – standardmäßig deaktiviert.

## 3. Serverkonfiguration

### 3.1 Neue CVars

| CVar | Standard | Beschreibung |
| --- | --- | --- |
| `sv_telemetryEnabled` | `0` | Globaler Schalter – nur bei `1` werden Payloads erzeugt und versendet |
| `sv_telemetryUrl` | `` | HTTPS-Endpoint (z. B. `https://telemetry.example.com/v1/matches`) |
| `sv_telemetryApiKey` | `` | Secret für den `Authorization`-Header; als gelatchte CVar gespeichert (nicht in `serverinfo`) |
| `sv_telemetryTimeoutMs` | `5000` | Netzwerk-Timeout für POSTs |
| `sv_telemetryEvents` | `0` | Aktiviert das optionale `events`-Array |
| `sv_telemetryMaxBatch` | `8` | Anzahl der Match-Reports im Spool |
| `sv_telemetryNamespace` | `"default"` | Salt für Spieler-Hashes, um GUID-Kollisionen zwischen Communities zu vermeiden |
| `sv_telemetryMaxBoostMs` | `15000` | Obergrenze zur Normalisierung von Boost-Zeiten (verwendet in Rennmetriken) |

Neue CVars folgen demselben Muster wie bestehende Server-Optionen (siehe `sv_enableRankings` & `sv_rankingsActive`).

### 3.2 Konfigurationsdatei

* Erweitere `server_example.cfg` um kommentierte Defaults und empfehle, Secrets in eine separate `telemetry_secrets.cfg` auszulagern, die per `exec telemetry_secrets.cfg` eingebunden wird.
* Die Deployment-Dokumentation verweist auf diese Datei und weist auf Dateiberechtigungen hin (nur für den Servernutzer lesbar)

### 3.3 Aktivierungsablauf

1. Administrator trägt Ziel-URL und API-Schlüssel in `telemetry_secrets.cfg` ein
2. Telemetrie wird mit `seta sv_telemetryEnabled "1"` und optional `sv_telemetryEvents` aktiviert
3. Der Server initialisiert beim Start das Outbox-Verzeichnis, setzt Namespaces (`sv_telemetryNamespace`) und beginnt mit dem Versand ab dem nächsten Match
4. Die neue In-Game-Abfrage `telemetryStatus` zeigt die letzten Antwortcodes

### 3.4 Sicherheitsaspekte

* `sv_telemetryApiKey` ist wie `rconPassword` als latched, schreibgeschützte CVar umgesetzt und erscheint weder in `serverinfo` noch in Status-Antworten
* Logging ist minimal: Erfolgsmeldungen nur auf Debug-Level, Fehler (mit HTTP-Status) auf Warn-Level
* Eine optionale IP-Allowlist (`sv_telemetryAllowedIPs`) kann später ergänzt werden

---

This plan provides a consistent data basis across all game modes, takes existing engine data structures into account, and describes infrastructure extensions compatible with existing server configuration mechanisms.
