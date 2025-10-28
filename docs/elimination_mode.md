# Elimination Mode (GT_ELIMINATION)

## Überblick
Der Eliminierungsmodus kombiniert Rundrennen mit Knockout-Phasen: Nach dem Rennstart zählt der Server die verstrichenen Intervalle (`g_eliminationStartDelay`, `g_eliminationInterval`, `g_eliminationWarning`). Wer bei Ablauf der Eliminierungsrunde auf dem letzten Platz liegt, scheidet aus, bis nur noch ein Fahrer übrig ist.【F:engine/code/game/g_main.c†L870-L872】【F:engine/code/game/g_rally_mapents.c†L115-L181】

## Kartenkennzeichnung
Alle Strecken, die sicher mit GT_ELIMINATION gespielt werden können, tragen nun den Eintrag `elimination "1"` in ihrer `.arena`-Datei. Das erleichtert UI-Filter, Rotation-Skripte und automatisierte Prüfungen. Beispiel:

```
//GAME TYPE SUPPORTS
type "q3r_racing q3r_team_racing q3r_elimination"

//ELIMINATION READY
elimination "1"
```

> Siehe `baseq3r/scripts/q3r_country01.arena` für ein vollständiges Beispiel.【F:baseq3r/scripts/q3r_country01.arena†L12-L15】

## Server-Rotation
Die Beispielkonfiguration und die dedizierte Eliminierungsrotation dokumentieren die Kennzeichnung zusätzlich im Kommentar. So bleibt die Wartung auch bei größeren Map-Pools nachvollziehbar:

```cfg
// [ELIM] q3r_country01
set d1 "map q3r_country01 ; set nextmap vstr d2"
```

* `baseq3r/q3r_elimination.cfg` enthält eine komplette, vorbereitete Eliminierungsrotation.【F:baseq3r/q3r_elimination.cfg†L21-L32】
* `baseq3r/server_example.cfg` erläutert im Map-Cycle-Abschnitt, dass die `elimination "1"`-Kennzeichnung als Referenz für GT_ELIMINATION dient.【F:baseq3r/server_example.cfg†L95-L99】

## Bot-Anpassungen
Bots erhalten zusätzliche Telemetrie im Inventar, darunter Rennposition, verbleibende Distanz sowie Eliminierungsrunden und ein Risiko-Flag. Diese Werte werden im Bot-Think aktualisiert, sobald GT_ELIMINATION aktiv ist.【F:engine/code/game/inv.h†L82-L100】【F:engine/code/game/ai_dmq3.c†L1785-L1808】

Auf Basis dieser Telemetrie priorisieren Bots Geschwindigkeits-Powerups aggressiver:

* Turbos werden stark bevorzugt, wenn der Bot in Eliminationsgefahr ist oder sich im Mittelfeld befindet.【F:baseq3r/botfiles/fuzi.c†L553-L575】
* Haste erhält ein höheres Gewicht, sobald der Bot kurz vor dem Knockout steht oder aufholen muss.【F:baseq3r/botfiles/fuzi.c†L589-L599】

Die zusätzlichen Gewichte machen es unwahrscheinlicher, dass KI-Fahrer durch Passivität einen DNF kassieren und sorgen für ein offensiveres Fahrverhalten in den entscheidenden Phasen.
