# Ladder Service Architecture / Ladder-Service Architektur

## English

### Overview
This design defines the server-side façade that submits match data to a ladder service via `POST /api/v1/matches`. The supported game modes align with the existing `gametype_t` values such as `GT_RACING`, `GT_TEAM_RACING_DM`, or `GT_DOMINATION`.【F:engine/code/game/bg_public.h†L141-L159】 The payload is built from existing level timestamps (`level.startRaceTime`, `level.finishRaceTime`), race and team metadata, plus player information sourced from `gclient_t`, the level state, and the scoreboard.【F:engine/code/game/g_local.h†L500-L537】【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/cgame/cg_local.h†L400-L410】 Player identities derive from the `cl_guid` mechanism that is already known to the server.【F:engine/code/client/cl_main.c†L124-L133】【F:engine/code/client/cl_main.c†L3654-L3674】

### Shared payload structure
* **Match metadata:** `matchId`, `mode`, `startTime`, `endTime`, `duration`, `map`, and `server` are taken from the level state and configuration strings. The start and end times come from `level.startRaceTime` and `level.finishRaceTime`; `level.numberOfLaps` augments race-oriented modes.【F:engine/code/game/g_local.h†L500-L537】
* **Player object:** Contains `playerId` (hash of `cl_guid` + display name), `displayName`, `team`, vehicle/skin data (`gclient_t.car` and `clientInfo_t.modelName`, `rimName`, etc.), and the counters and timers tracked per mode.【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/cgame/cg_local.h†L419-L468】
* **Scoreboard data:** `score`, `ping`, `time`, `accuracy`, `impressiveCount`, `assistCount`, `captures`, `damageDealt`, `damageTaken`, and `position` map directly from `score_t` and are translated into KPIs per mode.【F:engine/code/cgame/cg_local.h†L400-L410】
* **Team aggregates:** Team-level metrics rely on `level.teamScores` and `level.teamTimes` to compute totals, averages, and objective-focused values.【F:engine/code/game/g_local.h†L447-L520】

### Fields per `gametype_t`
#### GT_RACING & GT_RACING_DM
* `raceStart`, `raceFinish`, `winnerTime`: read from `level.startRaceTime`, `level.finishRaceTime`, and the winning client (`level.winnerNumber`).【F:engine/code/game/g_local.h†L500-L519】
* `completedLaps`, `bestLap`, `totalTime`: calculated via `gclient_t.finishRaceTime` relative to `level.startRaceTime`; during an ongoing race the fallback is `level.time - level.startRaceTime`.【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_rally_racetools.c†L104-L120】
* `position`: final placements from `score_t.position`.【F:engine/code/cgame/cg_local.h†L400-L410】
* `damageDealt`/`damageTaken`: available only for `GT_RACING_DM`, again taken from `score_t`.

#### GT_TEAM_RACING & GT_TEAM_RACING_DM
* Team lap times are stored in `level.teamTimes` and normalized by participant count.【F:engine/code/game/g_local.h†L447-L520】【F:engine/code/game/g_rally_racetools.c†L122-L146】
* `teamScoreFraction` comes from `level.teamScores`; weapon-enabled variants also publish `damageDealt`/`damageTaken`.
* Player fields mirror the solo racing payload with additional team membership via `score_t.team`.【F:engine/code/cgame/cg_local.h†L400-L410】

#### GT_ELIMINATION & GT_LCS
* `eliminationSchedule` carries `level.eliminationStartDelay`, `level.eliminationInterval`, and `level.eliminationWarning` for client-side reconciliation.【F:engine/code/game/g_local.h†L521-L528】
* `survivalTime` and `elimOrder` use `gclient_t.finishRaceTime` alongside the remaining player count (`level.eliminationRemainingPlayers`).【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_local.h†L521-L528】
* `lapsAtElim` is derived from `level.numberOfLaps` plus the final checkpoint timestamp per player (`gclient_t.lastCheckpointTime`).【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_local.h†L500-L523】

#### GT_DERBY
* Shares the same timing fields as racing and supplements them with knockout and damage efficiency metrics via `score_t.score`, `damageDealt`, and `damageTaken`.【F:engine/code/cgame/cg_local.h†L400-L410】
* Winner cooldown handling mirrors the rally modes through `level.finishRaceTime`.【F:engine/code/game/g_local.h†L500-L519】

#### GT_DEATHMATCH & GT_TEAM
* `frags` come from `score_t.score`; `deaths` are taken from per-client game stats (additional plumbing required in the game code); streak counters rely on `score_t.accuracy`, `impressiveCount`, `excellentCount`, etc.【F:engine/code/cgame/cg_local.h†L400-L410】
* Team variants expose `teamScoreFraction` via `level.teamScores` and the team assignment from `score_t.team`.【F:engine/code/game/g_local.h†L447-L456】【F:engine/code/cgame/cg_local.h†L400-L410】

#### GT_CTF & GT_CTF4
* Flag metrics stem from `captures`, `assistCount`, `defendCount`, and damage values tied to the carrier in the scoreboard.【F:engine/code/cgame/cg_local.h†L400-L410】
* Team objectives again use `level.teamScores` together with `score_t.team`.

#### GT_DOMINATION
* Control points are represented through the team score; additional tick counters are generated from `level.teamScores`/`level.teamTimes` and paired with per-player `score_t.score` and damage figures.【F:engine/code/game/g_local.h†L447-L520】【F:engine/code/cgame/cg_local.h†L400-L410】

#### GT_SINGLE_PLAYER
* Combines racing fields (`finishRaceTime`, `numberOfLaps`) with retry counters from the game scripting (e.g., `client->respawnTime`).【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_local.h†L500-L519】

### JSON schema for `POST /api/v1/matches`
Each match submission follows this structure:

```json
{
  "matchId": "srv-20240405-183011-42",
  "mode": "GT_RACING",
  "startTime": "2024-04-05T18:30:11Z",
  "endTime": "2024-04-05T18:42:39Z",
  "duration": "PT12M28S",
  "map": "q3r_country01",
  "server": { "name": "Q3Rally EU #1", "host": "203.0.113.10:27960", "build": "1.3.0" },
  "settings": { "g_gametype": 141, "g_eliminationInterval": 15000 },
  "players": [ { "playerId": "sha256:...", "team": "red", "totalTime": "PT12M02.250S", ... } ],
  "teams": [ { "team": "red", "rawScore": 123, "normalizedScore": 0.64, ... } ],
  "events": [ { "timestamp": "2024-04-05T18:33:15.210Z", "type": "lap_completed", ... } ]
}
```

* Required fields: `matchId`, `mode`, `startTime`, `endTime`, `players[*].playerId`, `players[*].rawScore`, `settings.g_gametype`.
* `settings` mirrors the relevant CVars (elimination timers, lap counts, etc.) based on level and server configuration.【F:engine/code/game/g_local.h†L500-L523】
* `events` is optional and reuses the event schema from the telemetry reporting plan for detailed race events.【F:readme/telemetry_reporting_plan.md†L160-L209】

### Configuration & CVars
The ladder integration follows the pattern of the existing rankings reporter: activation and status are controlled via CVars (`sv_enableRankings`, `sv_rankingsActive`).【F:engine/code/server/sv_rankings.c†L74-L137】 The new service introduces three dedicated CVars:

| CVar | Purpose | Flags |
| --- | --- | --- |
| `sv_ladderEnabled` | Globally enable or disable the HTTP reporter. | `CVAR_ARCHIVE` |
| `sv_ladderUrl` | Target endpoint for `POST /api/v1/matches`. | `CVAR_ARCHIVE` |
| `sv_ladderApiKey` | Secret used for the `Authorization` header. | `CVAR_TEMP | CVAR_PROTECTED` |

Implementation mirrors the existing `SV_Init` flow: secrets are treated like `rconPassword`, stored as temporary protected CVars so they never appear in status responses.【F:engine/code/server/sv_main.c†L34-L43】【F:engine/code/server/sv_init.c†L673-L685】

### HTTP façade & transport
The HTTP façade lives inside the server (`engine/code/server/sv_ladder.c`) because it needs direct access to the level and client state maintained exclusively by the dedicated server process (`level.startRaceTime`, `gclient_t.finishRaceTime`, `score_t` snapshots).【F:engine/code/game/g_local.h†L401-L523】【F:engine/code/cgame/cg_local.h†L400-L410】 Placing it in shared code (`qcommon`) would require additional callbacks for gameplay state and would tighten client builds to the ladder service unnecessarily.

Communication relies on libcurl, which is already available in the build system (`USE_CURL`, `USE_CURL_DLOPEN`).【F:engine/docs/README.md†L147-L166】 The façade initializes its own `CURLM` handle for asynchronous POSTs and applies these policies:

* **Retry & backoff:** Transport errors or HTTP ≥ 500 enqueue requests into a local spool (`telemetry/outbox`) and retry them with exponential backoff (starting at 5 s, capped at 5 min).【F:readme/telemetry_reporting_plan.md†L175-L209】 Successful responses remove the spool entry.
* **Spool directory:** Stored underneath `fs_homepath` and limited by `sv_telemetryMaxBatch`, leveraging the existing telemetry plan for reuse.【F:readme/telemetry_reporting_plan.md†L175-L209】
* **TLS certificates:** By default libcurl trusts the system store; optionally a dedicated CA path (`sv_ladderCaPath`, future work) can be exposed. Certificate failures surface as warnings and trigger the backoff logic without marking the match as delivered.
* **Auth:** `sv_ladderApiKey` is sent as a bearer token header. Like `rconPassword`, it never appears in normal log output.【F:engine/code/server/sv_init.c†L681-L685】

This architecture keeps coupling to the game code minimal, remains operator friendly via CVars, and reuses existing resiliency and security mechanisms.

---

## Deutsch

### Überblick
Dieser Entwurf beschreibt die serverseitige Fassade, die Matchdaten als `POST /api/v1/matches` an einen Ladder-Dienst überträgt. Die abzudeckenden Spielmodi orientieren sich an den vorhandenen `gametype_t`-Werten wie `GT_RACING`, `GT_TEAM_RACING_DM` oder `GT_DOMINATION`.【F:engine/code/game/bg_public.h†L141-L159】 Basis für alle Berichte sind die vorhandenen Zeitstempel des Levels (`level.startRaceTime`, `level.finishRaceTime`), die Renn- und Team-Metadaten sowie Spielerinformationen aus `gclient_t`, dem Levelzustand und dem Scoreboard.【F:engine/code/game/g_local.h†L500-L537】【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/cgame/cg_local.h†L400-L410】 Identitäten werden aus dem serverseitig bekannten `cl_guid`-Mechanismus abgeleitet.【F:engine/code/client/cl_main.c†L124-L133】【F:engine/code/client/cl_main.c†L3654-L3674】

### Gemeinsame Nutzdatenstruktur
* **Match-Metadaten**: `matchId`, `mode`, `startTime`, `endTime`, `duration`, `map`, `server` resultieren aus Level-Zustand und Konfigurations-Strings. Start- und Endzeiten stammen aus `level.startRaceTime` bzw. `level.finishRaceTime`; die Anzahl der Runden (`level.numberOfLaps`) ergänzt Rennmodi.【F:engine/code/game/g_local.h†L500-L537】
* **Spielerobjekt**: Enthält `playerId` (Hash aus `cl_guid` + Anzeigename), `displayName`, `team`, Fahrzeug-/Skin-Informationen (`gclient_t.car` sowie `clientInfo_t.modelName`, `rimName`, etc.) und die pro Modus erhobenen Zähler und Zeiten.【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/cgame/cg_local.h†L419-L468】
* **Scoreboard-Daten**: `score`, `ping`, `time`, `accuracy`, `impressiveCount`, `assistCount`, `captures`, `damageDealt`, `damageTaken` und `position` werden direkt aus `score_t` übernommen und je nach Modus in Kennzahlen übersetzt.【F:engine/code/cgame/cg_local.h†L400-L410】
* **Team-Aggregate**: Teambezogene Werte greifen auf `level.teamScores` und `level.teamTimes` zurück, um Gesamtpunkte, Durchschnittszeiten und objektbasierte Werte aufzubereiten.【F:engine/code/game/g_local.h†L447-L520】

### Felder pro `gametype_t`
#### GT_RACING & GT_RACING_DM
* `raceStart`, `raceFinish`, `winnerTime`: direkt aus `level.startRaceTime`, `level.finishRaceTime` und dem Sieger-Client (`level.winnerNumber`).【F:engine/code/game/g_local.h†L500-L519】
* `completedLaps`, `bestLap`, `totalTime`: über `gclient_t.finishRaceTime` relativ zu `level.startRaceTime`; während laufender Rennen wird `level.time - level.startRaceTime` verwendet.【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_rally_racetools.c†L104-L120】
* `position`: aus dem Scoreboard-Feld `score_t.position` für finale Platzierungen.【F:engine/code/cgame/cg_local.h†L400-L410】
* `damageDealt`/`damageTaken`: nur für `GT_RACING_DM`, direkt aus `score_t`.

#### GT_TEAM_RACING & GT_TEAM_RACING_DM
* Teamzeiten basieren auf `level.teamTimes` und werden auf Teilnehmerzahl normalisiert.【F:engine/code/game/g_local.h†L447-L520】【F:engine/code/game/g_rally_racetools.c†L122-L146】
* `teamScoreFraction` greift auf `level.teamScores` zu; Waffenvarianten übernehmen zusätzlich `damageDealt`/`damageTaken`.
* Spielerfelder entsprechen den Einzelrennen, ergänzt um Teamzuordnung (`score_t.team`).【F:engine/code/cgame/cg_local.h†L400-L410】

#### GT_ELIMINATION & GT_LCS
* `eliminationSchedule` enthält `level.eliminationStartDelay`, `level.eliminationInterval` und `level.eliminationWarning` für Client-/Auswerterabgleich.【F:engine/code/game/g_local.h†L521-L528】
* `survivalTime` und `elimOrder` nutzen `gclient_t.finishRaceTime` sowie die aktuelle Spieleranzahl (`level.eliminationRemainingPlayers`).【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_local.h†L521-L528】
* `lapsAtElim` errechnet sich aus `level.numberOfLaps` und dem letzten Checkpoint-Zeitstempel der Spieler (`gclient_t.lastCheckpointTime`).【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_local.h†L500-L523】

#### GT_DERBY
* Nutzt dieselben Zeitfelder wie Rennen; zusätzlich werden Knockouts und Schadenseffizienz über `score_t.score`, `damageDealt`, `damageTaken` abgebildet.【F:engine/code/cgame/cg_local.h†L400-L410】
* Das Sieger-Cooldown-Handling verwendet `level.finishRaceTime` analog zu den Rallye-Modi.【F:engine/code/game/g_local.h†L500-L519】

#### GT_DEATHMATCH & GT_TEAM
* `frags` aus `score_t.score`, `deaths` über die Spielstatistiken der Clients (aus dem Gamecode zu ergänzen), `accuracy` und Serienzähler über `score_t.accuracy`, `impressiveCount`, `excellentCount` etc.【F:engine/code/cgame/cg_local.h†L400-L410】
* Teamvarianten spiegeln `teamScoreFraction` mittels `level.teamScores` und nutzen `score_t.team` zur Zuordnung.【F:engine/code/game/g_local.h†L447-L456】【F:engine/code/cgame/cg_local.h†L400-L410】

#### GT_CTF & GT_CTF4
* Flagspezifische Metriken entstammen den Scoreboard-Feldern `captures`, `assistCount`, `defendCount` und `carrier`-abhängigen Schadenswerten.【F:engine/code/cgame/cg_local.h†L400-L410】
* Teamziele nutzen erneut `level.teamScores` und `score_t.team`.

#### GT_DOMINATION
* Kontrollpunkte spiegeln sich im Team-Score; zusätzliche Tick-Zähler werden aus `level.teamScores`/`level.teamTimes` generiert, ergänzt um pro Spieler aufbereitete `score_t.score` und Schadenswerte.【F:engine/code/game/g_local.h†L447-L520】【F:engine/code/cgame/cg_local.h†L400-L410】

#### GT_SINGLE_PLAYER
* Kombiniert Rennfelder (`finishRaceTime`, `numberOfLaps`) mit zusätzlichen Wiederholungszählern aus dem Spielscripting (z.B. `client->respawnTime`).【F:engine/code/game/g_local.h†L401-L417】【F:engine/code/game/g_local.h†L500-L519】

### JSON-Schema für `POST /api/v1/matches`
Der Service akzeptiert pro Match ein Objekt mit folgenden Hauptelementen:

```json
{
  "matchId": "srv-20240405-183011-42",
  "mode": "GT_RACING",
  "startTime": "2024-04-05T18:30:11Z",
  "endTime": "2024-04-05T18:42:39Z",
  "duration": "PT12M28S",
  "map": "q3r_country01",
  "server": { "name": "Q3Rally EU #1", "host": "203.0.113.10:27960", "build": "1.3.0" },
  "settings": { "g_gametype": 141, "g_eliminationInterval": 15000 },
  "players": [ { "playerId": "sha256:...", "team": "red", "totalTime": "PT12M02.250S", ... } ],
  "teams": [ { "team": "red", "rawScore": 123, "normalizedScore": 0.64, ... } ],
  "events": [ { "timestamp": "2024-04-05T18:33:15.210Z", "type": "lap_completed", ... } ]
}
```

* Pflichtfelder: `matchId`, `mode`, `startTime`, `endTime`, `players[*].playerId`, `players[*].rawScore`, `settings.g_gametype`.
* `settings` spiegelt relevante CVars wider (z.B. Eliminierungs-Intervalle, Rundenzahl) und basiert auf Level- sowie Server-Konfiguration.【F:engine/code/game/g_local.h†L500-L523】
* `events` ist optional und folgt dem in der Telemetrie-Planung beschriebenen Schema für detaillierte Rennereignisse.【F:readme/telemetry_reporting_plan.md†L160-L209】

### Konfiguration & CVars
Der Ladder-Abgleich folgt dem Muster der bestehenden Rankings-Anbindung: Aktivierung und Status werden über CVars gesteuert (`sv_enableRankings`, `sv_rankingsActive`).【F:engine/code/server/sv_rankings.c†L74-L137】 Für den neuen Dienst stellt der Server drei CVars bereit:

| CVar | Zweck | Flags |
| --- | --- | --- |
| `sv_ladderEnabled` | Aktiviert/Deaktiviert den HTTP-Reporter global. | `CVAR_ARCHIVE` |
| `sv_ladderUrl` | Ziel-Endpoint für `POST /api/v1/matches`. | `CVAR_ARCHIVE` |
| `sv_ladderApiKey` | Geheimnis für den `Authorization`-Header. | `CVAR_TEMP | CVAR_PROTECTED` |

Die Implementierung folgt dem bestehenden Muster in `SV_Init`: Secrets werden analog zu `rconPassword` als temporäre, geschützte CVars behandelt, damit sie nicht in Status-Antworten landen.【F:engine/code/server/sv_main.c†L34-L43】【F:engine/code/server/sv_init.c†L673-L685】

### HTTP-Fassade & Transport
Die HTTP-Fassade lebt im Server (`engine/code/server/sv_ladder.c`), weil sie direkt auf Level- und Clientzustände zugreifen muss, die nur im dedizierten Serverprozess gepflegt werden (`level.startRaceTime`, `gclient_t.finishRaceTime`, `score_t`-Snapshots).【F:engine/code/game/g_local.h†L401-L523】【F:engine/code/cgame/cg_local.h†L400-L410】 Eine Platzierung im gemeinsamen Code (`qcommon`) würde zusätzliche Callbacks für Spielzustände erfordern und die Abhängigkeiten des Client-Builds auf den Ladder-Dienst erhöhen.

Für die Kommunikation wird libcurl eingesetzt, das in der Build-Umgebung bereits optional vorgesehen ist (`USE_CURL`, `USE_CURL_DLOPEN`).【F:engine/docs/README.md†L147-L166】 Die Fassade initialisiert einen eigenen `CURLM`-Handle für asynchrone POSTs und setzt folgende Policies:

* **Retry & Backoff**: Bei Transportfehlern oder HTTP ≥ 500 werden Requests in einem lokalen Spool (`telemetry/outbox`) abgelegt und mit exponentiellem Backoff (Start 5 s, Cap 5 min) erneut versendet.【F:readme/telemetry_reporting_plan.md†L175-L209】 Erfolgreiche Antworten löschen den Spool-Eintrag.
* **Spool-Verzeichnis**: Das Verzeichnis liegt unterhalb von `fs_homepath` und enthält maximal `sv_telemetryMaxBatch` Elemente (übernimmt die bestehende Telemetrie-Planung und erleichtert Re-Use).【F:readme/telemetry_reporting_plan.md†L175-L209】
* **TLS-Zertifikate**: Standardmäßig nutzt libcurl den System-Truststore; optional kann ein dedizierter CA-Pfad (`sv_ladderCaPath`, später) erlaubt werden. Zertifikatsfehler führen zu Fehlermeldungen und Triggern der Backoff-Logik, ohne das Match als zugestellt zu markieren.
* **Auth**: `sv_ladderApiKey` wird als Bearer-Token im Header gesetzt. Analog zu `rconPassword` erfolgt keine Ausgabe in Logs auf Normal-Level.【F:engine/code/server/sv_init.c†L681-L685】

Diese Architektur hält die Kopplung zum Spielcode gering, erlaubt operatorfreundliche Konfiguration per CVar und nutzt vorhandene Mechanismen für Resilienz und Sicherheit.
