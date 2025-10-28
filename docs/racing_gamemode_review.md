# Racing-Gamemode – Verbesserungen und offene Punkte

## Verbesserungsvorschläge

1. **Teamzeit am HUD konsistent berechnen** ✅
   Die Teamzeit berücksichtigt jetzt standardmäßig `finishRaceTime - startRaceTime` (bzw. den laufenden Split) und fällt nur bei Sprintstrecken (`laplimit <= 1`) auf die Lap-Uhr zurück. Spieler ohne gültigen Rennstart werden aus der Durchschnittsberechnung ausgeschlossen, um Null-Divisionen und verzerrte Werte zu verhindern.【F:engine/code/cgame/cg_rally_hud2.c†L108-L151】

2. **Stabilere Positions-Tie-Breaker** ✅
   Der Positionsvergleich nutzt jetzt eine 1-Unit-Toleranz (`RALLY_POSITION_DIST_EPSILON`), bevor Splits (`lastCheckpointTime`) und
   Restdistanzen als Tie-Breaker herangezogen werden. Fahrer mit fehlendem Marker (Sentinel `1 << 30`) fallen automatisch hinter
   Konkurrenten mit gültigen Daten zurück, womit Gleichstandssituationen deterministischer aufgelöst werden.【F:engine/code/game/g_rally_racetools.c†L16-L75】【F:engine/code/game/g_rally_racetools.c†L451-L520】

3. **Startaufstellung fehlertoleranter machen** ✅
   Wenn kein passender `info_player_start` gefunden wird, informiert die Spawnlogik betroffene Fahrer jetzt per Centerprint und
   Chat, dass alle Startplätze belegt sind, bevor der Deathmatch-Fallback greift. So wissen Spieler sofort, warum sie nicht auf
   dem Grid stehen, ohne zusätzliche Slots erzeugen zu müssen.【F:engine/code/game/g_rally_racetools.c†L823-L836】

## Bekannte Issues

* ~~**Team-Zeit Anzeige inkorrekt:** Wie oben beschrieben, summiert der HUD-Code nur die letzte Runde pro Teammitglied und zeigt daher falsche Zeiten an.~~ Behoben: Der HUD-Code ermittelt die Teamzeit inzwischen über den gesamten Rennlauf.【F:engine/code/cgame/cg_rally_hud2.c†L108-L151】
* **Fallback-Spawns gefährden faire Starts:** Fehlende Grid-Marker zwingen Spieler weiterhin auf Deathmatch-Spawns; immerhin
  weist nun eine Einblendung darauf hin, dass das Grid voll ist. Langfristig sollte trotzdem eine sichere Alternative ohne
  Telefrag-Risiko umgesetzt werden.【F:engine/code/game/g_rally_racetools.c†L823-L836】
