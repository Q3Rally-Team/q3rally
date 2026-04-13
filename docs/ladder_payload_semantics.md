# Ladder Payload Semantics (verbindlich)

Diese Spezifikation ist **Review-Gate** für Änderungen an Ladder-Payloads. Jede Änderung an
`ladderMatchPayload_t`, `ladderPlayerPayload_t` oder `ladderProfileSnapshot_t` muss diese Datei
mit aktualisieren (inkl. Modus-Status, Datenquelle, Semantik, Fallback).

## Status-Legende

- **P** = Pflichtfeld (muss semantisch befüllt sein)
- **O** = Optionales Feld (kann befüllt sein, darf neutral bleiben)
- **V** = Verbotenes Feld (darf für den Modus nicht semantisch befüllt werden)
- **B** = Berechnetes Feld (serverseitig abgeleitet; kein Client-Input)

> Hinweis (Ist-Implementierung): Der Serializer in `sv_ladder.c` schreibt fast alle Felder immer aus.
> Für **V** gilt daher aktuell: Feldwert muss im Neutral-Fallback bleiben (`0`, `-1`, `[]`, `""`),
> bis ein modussensitiver Omit-Serializer eingeführt ist.

## Modus-Kürzel

- **R** Racing
- **RDM** Racing DM
- **SPR** Sprint
- **DER** Derby
- **LCS** LCS
- **ELI** Elimination
- **DM** Deathmatch
- **TEAM** Team DM
- **TR** Team Racing
- **TRDM** Team Racing DM
- **CTF** CTF
- **CTF4** CTF4
- **DOM** Domination
- **KOTH** KOTH

---

## 1) `ladderMatchPayload_t`

| Feld | Status pro Modus | Datenquelle im Code | Semantik | Fallback |
|---|---|---|---|---|
| `valid` | alle: **P+B** | `g_main.c::G_LadderSubmitMatchReport` (`payload->valid=qtrue`) | Payload gültig/submitbar | `false` => Upload abbrechen |
| `matchId` | alle: **P+B** | `level.ladderMatchId` (gesetzt in `G_LadderBuildMatchId`) | Match-Korrelation/Idempotenz | leer -> Serializer sendet `""` |
| `mode` | alle: **P+B** | `G_LadderModeForGametype(g_gametype)` | String-Repräsentation des Gametype | unbekannt -> `GT_ELIMINATION` |
| `gametype` | alle: **P+B** | `g_gametype.integer` | numerischer Gametype | `0` wenn uninitialisiert |
| `mapName` (JSON: `map`) | alle: **P+B** | `serverinfo mapname` | technische Map-ID | `""` |
| `startTimeIso`/`endTimeIso`/`durationIso` | alle: **P+B** | `G_LadderFormatIsoTime`, `Com_sprintf("PT%iS")` | Zeitstempel und Dauer | leere Strings / `PT0S` |
| `startEpoch`/`endEpoch`/`durationSeconds` | alle: **P+B** | `trap_RealTime`, Differenzbildung | Epoch-Sekunden und Matchdauer | `0` |
| `serverName` | alle: **P+B** | CVar `sv_hostname` | Anzeigename Server | `""` |
| `serverHost` | alle: **O+B** | `net_ip:net_port` else `sv_fqdn` else `serverName` | Host/Adresse des Servers | stufenweise Fallback-Kette |
| `serverBuild` | alle: **O+B** | CVar `version` | Engine/Build-Kennung | `""` |
| `isDedicated` | alle: **P+B** | `g_dedicated.integer` | Online/Dedicated Kennzeichnung | `false` |
| `levelStartTime`/`levelEndTime` | alle: **P+B** | `level.startTime`, `level.time` | Serverlevel-Ticks | `0` |
| `raceStartTime`/`raceEndTime`/`finishRaceTime` | R,RDM,SPR,DER,LCS,ELI,TR,TRDM: **P+B**; DM,TEAM,CTF,CTF4,DOM,KOTH: **V+B** | `level.startRaceTime`, `level.finishRaceTime` | Rennzeit-Anker (ms Tickbasis) | `0` |
| `winnerClientNum` | alle: **P+B** | `level.winnerNumber`, für scorebasierte Modi Re-Derive per Highscore-Scan | Gewinner-Clientslot | `-1`/`0` je Zustand |
| `numberOfLaps` | R,RDM,SPR,TR,TRDM,ELI: **P+B**; DER,LCS,DM,TEAM,CTF,CTF4,DOM,KOTH: **V+B** | `level.numberOfLaps`, Sprint default via `G_RallyApplySprintDefaults` | konfigurierte Rundenzahl | `0` |
| `trackReversed` | R,RDM,SPR,TR,TRDM,ELI: **O+B**; sonst: **V+B** | CVar `g_trackReversed` | Strecke reversed | `false` |
| `eliminationStartDelay`/`eliminationInterval`/`eliminationWarning` | ELI: **P+B**; alle anderen: **V+B** | CVars `g_elimination*` | Eliminations-Timerkonfiguration | `0` |
| `teamScores[TEAM_NUM_TEAMS]` | TEAM,TRDM,CTF,CTF4,DOM,KOTH: **P+B**; R,RDM,SPR,DER,LCS,ELI,DM,TR: **O+B** | `level.teamScores[]` | Team-Score pro Teamindex | Array mit `0` |
| `teamTimes[TEAM_NUM_TEAMS]` | TR,TRDM: **P+B**; R,RDM,SPR,DER,LCS,ELI,DM,TEAM,CTF,CTF4,DOM,KOTH: **O+B** | `level.teamTimes[]` | Team-Rennzeitenaggregat | Array mit `0` |
| `playerCount` | alle: **P+B** | inkrementiert in `G_LadderPopulatePlayer`; im Serializer auf `[0..MAX_CLIENTS]` geclamped | Anzahl aktiver Nicht-Spectator-Clients | Clamp auf 0/MAX |
| `players[MAX_CLIENTS]` | alle: **P+B** | befüllt via `G_LadderPopulatePlayer`, serialisiert bis `playerCount` | Teilnehmerobjekte | leeres Array |

---

## 2) `ladderPlayerPayload_t`

| Feld | Status pro Modus | Datenquelle im Code | Semantik | Fallback |
|---|---|---|---|---|
| `clientNum` | alle: **P+B** | Schleifenindex in `G_LadderPopulatePlayer` | Client-Slotnummer | n/a |
| `playerId` | alle: **P+B** | `cl_uuid` > lokales Profil-UUID > `cl_guid` > Remote-IP > `cleanName` > `client-N` | stabile Spieleridentität | mehrstufige Fallback-Kette |
| `guid` | alle: **O+B** | `cl_guid` sonst IP | sekundäre Identität | `""` |
| `name`/`cleanName` | alle: **P+B** | Userinfo `name`, `Q_CleanStr` | Raw-/bereinigter Anzeigename | `""` |
| `model`/`vehicle` | alle: **O+B** | Userinfo `model`, `chassis`/`vehicle` | kosmetische Auswahl | `""` |
| `team`/`teamName` | alle: **P+B** | `client->sess.sessionTeam`, `SV_LadderTeamName` | Teamzuordnung numerisch + Label | numerisch + `"FREE"` |
| `isBot` | alle: **P+B** | `SVF_BOT` Flag | Bot-Kennzeichnung | `false` |
| `score` / JSON-Aliase `rawScore`,`playerScore` | alle: **P+B** | `client->ps.persistant[PERS_SCORE]` | Engine-Score (modusspezifische Bedeutung) | `0` |
| `rankTier`/`rankName` (JSON-only) | alle: **O+B** | `SV_LadderGetRankForScore(score)` | aus Score abgeleitete Ladder-Rangstufe | Feld wird weggelassen bei Fehler |
| `ping` | alle: **P+B** | `client->ps.ping` | Ping in ms | `0` |
| `time` | alle: **P+B** | `level.time - client->pers.enterTime` (min 0) | Sitzungsdauer des Clients | `0` |
| `scoreFlags` | alle: **O+B** | `PERS_PLAYEREVENTS` | Eventbitfeld | `0` |
| `powerUps` | alle: **O+B** | aktive `ps.powerups[]` Bits | Powerup-Bitmaske | `0` |
| `accuracy` | DM,TEAM,CTF,CTF4,DOM,KOTH,DER,LCS,ELI,RDM,TRDM: **O+B**; R,SPR,TR: **V+B** | `accuracy_hits/shots` | Trefferquote Prozent (Ganzzahl) | `0` |
| `impressiveCount`/`impressiveTelefragCount`/`excellentCount`/`gauntletCount` | DM,TEAM,CTF,CTF4,DOM,KOTH,DER,LCS,ELI,RDM,TRDM: **O+B**; R,SPR,TR: **V+B** | `PERS_*` Counter | Medaillen-/Eventzähler | `0` |
| `defendCount`/`assistCount` | CTF,CTF4,DOM,KOTH,TEAM: **O+B**; R,RDM,SPR,DER,LCS,ELI,DM,TR,TRDM: **V+B** | `PERS_DEFEND_COUNT`, `PERS_ASSIST_COUNT` | Objective-Teamhilfe | `0` |
| `perfect` | DM,TEAM,DER,LCS,ELI,RDM,TRDM: **O+B**; R,SPR,TR,CTF,CTF4,DOM,KOTH: **V+B** | abgeleitet: `kills>0 && deaths==0` | flawless ohne Tode | `false` |
| `captures` | CTF,CTF4: **P+B**; DOM,KOTH: **O+B**; übrige: **V+B** | `PERS_CAPTURES` | Flag-Captures (kein Teamscore) | `0` |
| `damageDealt`/`damageTaken` | RDM,DER,LCS,ELI,DM,TEAM,TRDM,CTF,CTF4,DOM,KOTH: **O+B**; R,SPR,TR: **V+B** | `STAT_DAMAGE_DEALT/TAKEN` | reale Damagewerte | `0` |
| `position` | R,RDM,SPR,TR,TRDM,ELI: **P+B**; sonst: **O+B** | `STAT_POSITION` | Renn-/Platzierungsrang | `0` |
| `bestLapMs` | R,RDM,SPR,TR,TRDM,ELI: **P+B**; DER,LCS: **O+B**; DM,TEAM,CTF,CTF4,DOM,KOTH: **V+B** | `client->bestLapMs` | beste Rundenzeit (ms) | `0` |
| `totalRaceMs` | R,RDM,SPR,DER,LCS,ELI,TR,TRDM: **P+B**; DM,TEAM,CTF,CTF4,DOM,KOTH: **V+B** | `finishRaceTime-startRaceTime` sonst `level.time-startRaceTime` | gesamte Renn-/Überlebenszeit in ms | `0` |
| `lapCount`/`lapTimes[]` | R,RDM,SPR,TR,TRDM,ELI: **P+B**; DER,LCS: **O+B**; DM,TEAM,CTF,CTF4,DOM,KOTH: **V+B** | `recordedLapCount` + `recordedLaps[]`, im Serializer geclamped | erfasste Rundezeiten | `0` / `[]` |
| `kills` | DM,TEAM,DER,LCS,ELI,RDM,TRDM,CTF,CTF4,DOM,KOTH: **P+B**; R,SPR,TR: **V+B** | **aktuell** aus `PERS_SCORE` gesetzt | Score-basierter Killwert (aktuell kein separater Fragcounter) | `0` |
| `deaths` | DM,TEAM,DER,LCS,ELI,RDM,TRDM,CTF,CTF4,DOM,KOTH: **P+B**; R,SPR,TR: **V+B** | `PERS_KILLED` | Anzahl Tode | `0` |
| `zoneHoldMs` | DOM,KOTH: **P+B**; sonst: **V+B** | Feld vorhanden, in `G_LadderPopulatePlayer` derzeit nicht befüllt | reserviert für Zonenhaltezeit | `0` |
| `zoneActiveSigil` | DOM: **O+B**; sonst: **V+B** | in Populate hart auf `-1` gesetzt | aktiver Sigil/Zone-Index | `-1` |
| `survivalMs` | ELI,LCS,DER: **P+B**; R,RDM,SPR,TR,TRDM: **O+B**; DM,TEAM,CTF,CTF4,DOM,KOTH: **V+B** | aus Start-/Finish-Zeitfenster berechnet | Überlebenszeit seit Rennstart | `0` |
| `eliminationRound`/`eliminationPlayersRemaining`/`eliminationMetric` | ELI: **P+B**; sonst: **V+B** | `client->elimination*` | Eliminations-Metadaten + Tie-Break-Metrik | `0`/`0.0` |
| `finishRaceTime` | R,RDM,SPR,DER,LCS,ELI,TR,TRDM: **P+B**; übrige: **V+B** | `client->finishRaceTime` | absolute Finish-Tickzeit | `0` |
| `kdRatio` | DM,TEAM,DER,LCS,ELI,RDM,TRDM,CTF,CTF4,DOM,KOTH: **O+B**; R,SPR,TR: **V+B** | aus `kills/deaths` berechnet | K/D Verhältnis (bei deaths=0: kills) | `0.0` |
| `profile` | alle: **O+B** | nur für `client->pers.localClient`; sonst weggelassen | Karriere-Snapshot eingebettet | Feld omitted |

---

## 3) `ladderProfileSnapshot_t`

**Geltung grundsätzlich:** `profile` wird nur bei lokalem Client serialisiert (`player->profile.valid==qtrue`), sonst komplett weggelassen.

### 3.1 Allgemeine Profilfelder

| Feld | Status pro Modus | Datenquelle im Code | Semantik | Fallback |
|---|---|---|---|---|
| `valid` | alle: **P+B** (nur innerhalb `profile`) | `G_LadderPopulatePlayer` nach erfolgreichem JSON-Lesen | Snapshot vorhanden | `false` => `profile` omitted |
| `playerScore`,`currentRank`,`highestRank` | alle: **O+B** | `profiles/<active>.json` (`stats`/`info`) via `G_Profile_ParseIntPublic` | Karrierepunkte und Ranghistorie | `0` |
| `wins`,`losses`,`kills`,`deaths` | alle: **O+B** | Profil-JSON `stats` | kumulierte Karrierewerte | `0` |
| `flagCaptures`,`flagAssists` | CTF,CTF4-lastige Profile: **O+B**; sonst **O+B** | Profil-JSON `stats` | Karriere-Objective-Zähler | `0` |
| `bestLapMs` | R,RDM,SPR,TR,TRDM,ELI-lastige Profile: **O+B**; sonst **O+B** | Profil-JSON `stats` | bester historischer Lap | `0` |
| `accuracyAwards`,`excellentAwards`,`impressiveAwards`,`perfectAwards` | alle: **O+B** | Profil-JSON `stats` | Medaillen-/Award-Karrierezähler | `0` |
| `damageDealt`,`damageTaken` | alle: **O+B** | Profil-JSON `stats` | kumulierter Damage | `0` |
| `distanceKm`,`topSpeedKph`,`fuelUsed` | alle: **O+B** | `G_Profile_ParseDoublePublic` | Fahrzeug-Karriere-Metriken | `0.0` |
| `mostUsedVehicle` | alle: **O+B** | `G_Profile_ParseStringPublic` | häufigstes Fahrzeug | `""` |
| `gamesPlayed` | alle: **O+B** | Profil-JSON `stats` | gespielte Matches | `0` |
| `achievementTiers[]` | alle: **O+B** | `BG_AchievementUnlockedTiers` auf berechnetem Progress-Table | freigeschaltete Tier-Stufen pro Kategorie | `0` pro Eintrag |

### 3.2 Modus-spezifische Profilfelder

| Feldgruppe | Status pro Modus | Datenquelle im Code | Semantik | Fallback |
|---|---|---|---|---|
| `racingWins`,`racingPodiums`,`racingCompleted`,`racingTotalMs` | R: **P+B**; andere: **V+B** | Profil-JSON `stats` | Karriere für GT_RACING | `0` |
| `racingDmWins`,`racingDmPodiums`,`racingDmCompleted`,`racingDmTotalMs` | RDM: **P+B**; andere: **V+B** | Profil-JSON `stats` | Karriere für GT_RACING_DM | `0` |
| `sprintWins`,`sprintCompleted`,`sprintBestMs` | SPR: **P+B**; andere: **V+B** | Profil-JSON `stats` | Sprint-Karriere | `0` |
| `eliminationWins`,`eliminationCompleted`,`eliminationTotalRoundsLasted` | ELI: **P+B**; andere: **V+B** | Profil-JSON `stats` | Elimination-Karriere | `0` |
| `lcsWins`,`lcsCompleted`,`lcsTotalSurvivalMs` | LCS: **P+B**; andere: **V+B** | Profil-JSON `stats` | LCS-Karriere | `0` |
| `derbyWins`,`derbyCompleted`,`derbyKills` | DER: **P+B**; andere: **V+B** | Profil-JSON `stats` | Derby-Karriere | `0` |
| `dmWins`,`dmCompleted`,`dmKills` | DM: **P+B**; andere: **V+B** | Profil-JSON `stats` | DM-Karriere | `0` |
| `ctfWins`,`ctfCompleted`,`ctfCaptures` | CTF: **P+B**; andere: **V+B** | Profil-JSON `stats` | CTF-Karriere | `0` |
| `ctf4Wins`,`ctf4Completed`,`ctf4Captures` | CTF4: **P+B**; andere: **V+B** | Profil-JSON `stats` | CTF4-Karriere | `0` |
| `teamWins`,`teamCompleted`,`teamKills` | TEAM: **P+B**; andere: **V+B** | Profil-JSON `stats` | Team-DM-Karriere | `0` |
| `teamRacingWins`,`teamRacingCompleted`,`teamRacingPodiums` | TR: **P+B**; andere: **V+B** | Profil-JSON `stats` | Team-Racing-Karriere | `0` |
| `teamRacingDmWins`,`teamRacingDmCompleted`,`teamRacingDmPodiums` | TRDM: **P+B**; andere: **V+B** | Profil-JSON `stats` | Team-Racing-DM-Karriere | `0` |
| `dominationWins`,`dominationCompleted`,`dominationZoneHoldMs` | DOM: **P+B**; andere: **V+B** | Profil-JSON `stats` | Domination-Karriere inkl. Hold-Zeit | `0` |
| `kothWins`,`kothCompleted`,`kothZoneHoldMs` | KOTH: **P+B**; andere: **V+B** | Profil-JSON `stats` | KOTH-Karriere inkl. Hold-Zeit | `0` |

---

## Review-Gate-Regeln (verbindlich)

1. **Kein Feld ohne Status:** Neue Felder müssen vor Merge in diese Spezifikation mit Modusstatus aufgenommen werden.
2. **Semantik zuerst:** Wenn ein Feld semantisch "verboten" ist, muss es neutral bleiben (oder künftig serializer-seitig omitted werden).
3. **Source of truth:** Code-Stellen in `g_main.c` (`G_LadderSubmitMatchReport`, `G_LadderPopulatePlayer`) und `sv_ladder.c` (JSON-Serializer) sind maßgeblich.
4. **Breaking Change Pflicht:** Jede Änderung an Namensgebung, Typ, Fallback oder Modusstatus ist breaking bis dokumentiert und von Ladder-Service geprüft.
5. **Testpflicht:** Änderungen an Payload-Build/Serialize müssen mindestens die Ladder-JSON-Tests aktualisieren/erweitern.
