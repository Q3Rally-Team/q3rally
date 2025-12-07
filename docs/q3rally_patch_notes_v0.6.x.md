# Q3Rally – Änderungen bis zum nächsten Patch / Q3Rally – Changes for the Next Patch

## 🇩🇪 Deutsch

1. **Fix / Core Gameplay:** Temporäre Entfernung der **individuellen Fahrzeug-Configs**, da sie die Spawn-Logik in Team-Gamemodes vollständig durcheinanderbrachten.  
   - Mit diesem Fix funktionieren nun wieder **alle Gamemodes** — sowohl lokal als auch auf **dedizierten Servern** — zuverlässig.  
   - Die Funktionalität soll zu einem späteren Zeitpunkt überarbeitet und sicher reimplementiert werden.  

2. **Fix:** Der Derby-Countdown startet nicht mehr fälschlicherweise nach jeder Eliminierung. Mehrere `raceFinishTime`-Events während eines laufenden Derbys werden nun korrekt ignoriert, bis tatsächlich alle Gegner ausgeschieden sind.  

3. **Fix:** Ungültige Fahrzeugmodelle verursachen keinen `RE:RegisterModel: 0`-Fehler mehr. Fehlende oder fehlerhafte `model`-Einträge im Userinfo werden nun geprüft und mit einem sicheren Standardmodell ersetzt, bevor sie an `CG_RegisterClientModelname` übergeben werden.  

4. **Fix / Improvement:** Scripted-Object-Modelle werden nun korrekt initialisiert und registriert.  
   - Alle Model-Handles werden beim Start auf 0 gesetzt.  
   - Der Parser liest Modelpfade wahlweise aus benannten Sektionen oder verwendet die übergebenen Pfad-Strings korrekt für Live- und Dead-Modelle.  
   - Doku: `model`/`deadmodel` können direkte Pfade oder Sektionen sein.  

5. **Feature / Audio:** Scripted-Object-Sounds server- und clientseitig verdrahtet (Hit/Destroy, Pain/Death/Spawn-Loops, Event-Handling in `centity_t`).  

6. **Fix / Improvement:** Soundnamen unmittelbar nach Registrierung in `centity` persistiert; Loop-Logik schaltet `scriptDestroyPending` korrekt und wechselt Pre-/Post-Loop.  

7. **Feature / Gameplay:** CTF4-Scoring verbessert (`AddTeamScore` mit Sound-Mapping für alle vier Teams, neuen Green-/Yellow-Signalwegen, Tie/Lead-Checks über alle Gegner, farbigen Chat-/Centerprint-Ansagen); Scoring-Referenz aktualisiert. Die benötigten WAVs (`voc_green_scores.wav`, `voc_yellow_scores.wav`, `voc_green_leads.wav`, `voc_yellow_leads.wav`) sind dokumentiert und müssen separat in `baseq3` abgelegt werden.【F:engine/code/game/g_team.c†L447-L559】【F:engine/code/cgame/cg_main.c†L737-L760】【F:engine/code/cgame/cg_event.c†L1367-L1395】【F:baseq3/sound/teamplay/README.md†L1-L9】

8. **Fix / Domination:** Drittes Sigil wird sofort als neutral registriert (Configstring-Update) → korrekter HUD-Status.  

9. **Feature / System:** Asynchrones Update-Check-System (CVars, Response-Parsing, cURL-Downloads), in Client-Loop & UI-Lifecycle integriert; Menü-Dialog bei veralteter Version.  

10. **Feature / Gameplay & UI:** Derby-Grundschaden + Clamping, angeglichene Kollisions-CVars; Scoreboard zeigt *Wrecks* für `GT_DERBY` und getrennte Dealt/Taken-Spalten; Review-Notiz aktualisiert.  

11. **Fix / HUD & Scoring:** Team-HUD-Aggregator nutzt Vollrennen mit Sprint-Fallback; ignoriert Fahrer ohne gültigen Start (keine Divide-by-Zero); Review-Eintrag als resolved.  

12. **Fix / Racing Logic:** Positionsvergleich mit Epsilon-Toleranz & Sentinel-Handling; Near-Ties per Checkpoint-Splits statt FP-Jitter; Review-Tie-Breaker abgeschlossen.  

13. **UX / Racing:** Hinweis bei **überfüllter Startaufstellung**.  
    - `CenterPrint`/Chat-Notification erscheint, wenn die Grid-Auswahl auf Deathmatch-Spawns zurückfällt, damit Spieler wissen, dass alle Grid-Slots belegt sind.  
    - Racing-Gamemode-Review: Grid-Overflow-Item als erledigt markiert; neue Warnung beschrieben und verbleibendes Fallback-Spawn-Risiko vermerkt.  


14. **UX / Ghosts:** Neues `cg_ghostAlpha`-CVar (0–255, Standard 160), das die Transparenz von Geisterfahrzeug und Rädern steuert (jetzt über einen dedizierten Shader direkt sichtbar).

## 🇬🇧 English

1. **Fix / Core Gameplay:** Temporarily removed **individual vehicle configs** as they completely broke the spawn logic in team gamemodes.  
   - With this fix, **all gamemodes** now work again — both locally and on **dedicated servers**.  
   - The feature will be reworked and safely reintroduced later.  

2. **Fix:** The Derby countdown no longer starts incorrectly after each elimination. Multiple `raceFinishTime` events during an active derby are now properly ignored until all opponents are eliminated.  

3. **Fix:** Invalid vehicle models no longer trigger `RE:RegisterModel: 0`. Missing or malformed `model` entries in userinfo are now validated and replaced with a safe default before being passed to `CG_RegisterClientModelname`.  

4. **Fix / Improvement:** Scripted object models are now properly initialized and registered.  
   - All model handles are initialized to zero on startup.  
   - The parser can now read model paths from named sections or fall back to provided path strings for live and dead models.  
   - Docs updated: `model` and `deadmodel` can reference either direct paths or section-based tokens.  

5. **Feature / Audio:** Scripted-object sounds are now fully handled server- and client-side (hit/destroy events, pain/death/spawn loops, event routing through `centity_t`).  

6. **Fix / Improvement:** Persisted parsed sound tokens into `centity` buffers immediately after registration; loop logic now toggles `scriptDestroyPending` correctly and switches pre/post looping audio as intended.  

7. **Feature / Gameplay:** Improved **CTF4 team scoring** — `AddTeamScore` now bundles four-team sound routing (including green/yellow score cues), tie/lead detection against every opponent, and colorized chat/center-screen announcements. Updated scoring documentation accordingly; ship the documented `voc_green_scores.wav` / `voc_yellow_scores.wav` / `voc_green_leads.wav` / `voc_yellow_leads.wav` announcer assets alongside your `baseq3` data.【F:engine/code/game/g_team.c†L447-L559】【F:engine/code/cgame/cg_main.c†L737-L760】【F:engine/code/cgame/cg_event.c†L1367-L1395】【F:baseq3/sound/teamplay/README.md†L1-L9】

8. **Fix / Domination:** The automatically generated **third domination sigil** now registers as neutral immediately, ensuring clients receive the correct HUD state.  

9. **Feature / System:** Implemented asynchronous **update-checking system** using CVars, response parsing, and cURL-based downloads. Integrated into the main client loop and UI lifecycle. Added menu dialog to notify users when the installation is outdated.  

10. **Feature / Gameplay & UI:** Enhanced **Demolition Derby** — added ram base damage term with clamping, aligned collision CVars, and updated scoreboard with *Wrecks* column for `GT_DERBY`, separated damage dealt/taken, and preserved death counts in other modes. Updated review notes accordingly.  

11. **Fix / HUD & Scoring:** Reworked **team HUD aggregator** to base averages on full-race timing with sprint fallback and to skip racers without a valid start, preventing divide-by-zero cases. Updated racing gamemode review to mark timing issue resolved.  

12. **Fix / Racing Logic:** Added epsilon-based tolerance and sentinel-aware comparison to race position logic. Near ties now defer to checkpoint splits instead of floating-point jitter. Updated review to mark tie-breaker complete.  

13. **UX / Racing:** Added a **CenterPrint/chat notification** when start-grid selection falls back to deathmatch spawns, alerting players that all grid slots are occupied. Updated review entry to mark grid overflow resolved and note remaining fallback risks.  

14. **UX / Ghosts:** Added `cg_ghostAlpha` CVar (0–255, default 160) to let players tune ghost car and wheel transparency (now driven through a dedicated shader for immediate effect).
