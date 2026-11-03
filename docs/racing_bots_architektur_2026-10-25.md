# Bot Racing: Ghost-Routen als Grundlage

Entscheidung vom 25.10.2026: Aufgezeichnete Fahrten liefern das Streckenwissen. Die Bots berechnen daraus ihre eigene Führung, Geschwindigkeit und Reaktion auf andere Autos. Ein Ghost ist eine Fahrvorlage, keine Wiedergabe seiner Eingaben.

## Zielarchitektur

1. **Routenquellen:** mitgelieferte Referenzrunden und später geprüfte Spielerrunden; vorhandene `bot_path_node`-Pfade bleiben eine optionale Ersatzquelle.
2. **Gemeinsame Streckenroute:** passende Map, Länge und Richtung; geordnete Checkpoints; offener Sprint oder geschlossene Runde; anschließend Fahrkorridor und Bremsprofil.
3. **Fahrer:** Führung entlang der Route, Fahrzeugsteuerung, Umgang mit Verkehr und Wiederaufnahme der Fahrt. Die Routenquelle entscheidet nicht mehr darüber, welche Recovery der Bot bekommt.
4. **Lernen und Schwierigkeit:** neue Runden werden zwischen Rennen bewertet und übernommen. Eine bessere Streckenvorlage und der gewählte Bot-Skill bleiben getrennte Größen.

Mapper sollen dafür keine zusätzliche Ideallinie von Hand setzen müssen. Eine saubere Referenzfahrt pro Streckenvariante bleibt notwendig. Für eine neue Map ohne Aufnahme sind vorhandene Map-Pfade beziehungsweise Checkpoint-Kurven der Übergang, keine gleichwertige Qualitätsgarantie.

## Erster implementierter Schritt

`g_race_route.c` stellt einen gemeinsamen Adapter und geometrische Führung bereit. Das bestehende kompakte Ghost-Routenformat wird dafür als `botRaceRoute_t` wiederverwendet. Ghosts haben Vorrang. Ohne brauchbaren Ghost werden registrierte Map-Pfade in dieses Format überführt. Deren bisheriger eigener Fahrzweig wurde aus `ai_dmnet.c` entfernt. Fehlt auch ein brauchbarer Map-Pfad, bleibt die Checkpoint-Führung.

Map-Pfade werden nach Streckenlänge abgetastet und bei Rundkursen geschlossen. Ihr frei gewählter erster Knoten wird auf einen Rundenbeginn hinter Start/Ziel normiert. Die ursprünglichen Pfadnamen bleiben erhalten; ein komprimierter Registrierungsindex wird nicht mehr als „aggressive“ oder „sichere“ Linie interpretiert.

Die Führung projiziert die Fahrzeugposition auf Streckenabschnitte, berücksichtigt dabei auch die Höhe und bestimmt den Zielpunkt nach Wegstrecke. Die gemeldete Spurabweichung ist der Abstand zum Abschnitt. Die bisherige verdeckte zweite Variable, durch die die Recovery außen immer null Meter Abweichung sah, entfällt.

Route und Recovery berücksichtigen den nächsten tatsächlich offenen Checkpoint. Ein verpasstes Tor darf nicht durch den nächstgelegenen späteren Streckenabschnitt ersetzt werden. Rücksetzungen bleiben auf demselben Abschnitt zwischen Checkpoints. In der Toranfahrt wird auf die Referenzlinie zurückgekehrt. Sprint-Fallbacks verwenden den separaten Start und das Ziel hinter dem letzten Checkpoint.

Die Geschwindigkeitsplanung berücksichtigt kommende langsamere Abschnitte und Kurven mit einer Bremsvorschau. Eine notwendige Geschwindigkeitsabsenkung wird sofort wirksam; nur Beschleunigung wird geglättet. Das ist ein erstes konservatives Modell mit festen Annahmen, noch keine fahrzeug- und untergrundabhängige Optimierung. Die bestehende Recovery darf geringe Ortsänderung nur bei tatsächlich niedriger Geschwindigkeit als Feststecken werten.

Der Ghost-Import prüft zusätzlich:

- explizit angegebene Physikversion und Map-Prüfsumme;
- vollständige Koordinaten, begrenzte Werte, Zeitreihenfolge und grobe Positionssprünge;
- die Checkpoint-Reihenfolge der aktiven Strecke, bei Sprint einschließlich des separaten Ziels;
- keine Übernahme alter Aufnahmen ohne Variantenangabe auf andere Streckenlängen oder Rückwärtsvarianten.

Alte Dateien ohne Physikversion oder Prüfsumme bleiben verwendbar. Sie sind damit **nicht vollständig auf Kompatibilität geprüft**. Checkpoint-Prüfung und Plausibilitätsgrenzen ersetzen außerdem keine vollständige Kollisionsprüfung einer Aufnahme. Saubere Aufnahmen werden weiterhin vor schnelleren unsauberen bevorzugt; innerhalb derselben Klasse zählt die Rundenzeit.

Die Dateiliste der Ghost-Suche wurde von 2 KiB auf 64 KiB erweitert. Damit werden Aufnahmen einer Map in einem größeren lokalen Ghost-Verzeichnis nicht schon nach wenigen Dutzend anderen Dateien übergangen. Die bestehende Vorauswahl der fünf besten Header-Kandidaten bleibt bestehen; eine künftig umfassende Qualitätsauswahl muss auch langsamere Kandidaten berücksichtigen, wenn diese fünf unbrauchbare Aufnahmen enthalten.

## Noch folgende Arbeit

- **AAS entkoppeln:** Die allgemeine Bot-Initialisierung verlangt weiterhin AAS. Das Mapper-Ziel ist erst nach dieser Entkopplung vollständig erreicht.
- **Fahrzeugsteuerung und Verkehr:** Racing benutzt jetzt für Ghosts und Map-Pfade dieselbe Führung/Recovery, aber noch nicht den gesamten neuen DM-Fahrkern. Bremsen, Lenkung, Überholkorridore und Verhalten nach Kontakten müssen weiter abgestimmt werden.
- **Referenzrouten:** Gute Runden pro Map und Variante auswählen, geometrisch prüfen, mit passender Metadatenversion ausliefern. Die lokalen Spieleraufnahmen in den Tests sind keine neu veröffentlichte Referenzsammlung.
- **Lernen:** Es gibt noch keine automatische Übernahme einer gerade gefahrenen Spielerrunde in den Bot-Pool. Dazu gehören lokale Speicherung unabhängig von Ladder, Qualitätsbewertung, Server-Bereitstellung und Wechsel zwischen Rennen.
- **Schwierigkeit:** Der neue gemeinsame Fahrer ist noch nicht über alle fünf Schwierigkeitsgrade kalibriert. Racing-DM benötigt zusätzlich seine Waffen-/Kampfplanung.

Dieser Schritt entscheidet und implementiert die gemeinsame Grundlage. Er schließt die gesamte Racing-Überarbeitung noch nicht ab.

## Prüfung

Die C-Regressionen prüfen unter anderem Ghost-Vorrang, Map-Konvertierung, frei platzierten Rundenbeginn, Loop-/Sprint-Ende, Abstand zum Segment, Brückenhöhe, Bremsvorschau, verpasste Checkpoints, ungültige Aufnahmen, Kompatibilitätsdaten und größere Ghost-Verzeichnisse. Dazu laufen die bisherigen Ghost-, Racing-, DM- und Physiktests; insgesamt 15 Python-Testfunktionen mit ihren C-Testfällen.

Native Linux-Spielmodule sowie Windows-EXE, DLLs und QVMs werden gebaut. Die Spielmessung benutzt `tools/bot_racing_benchmark.py`: vier Bots auf Skill 5, 180 Sekunden, `timescale 1`, feste RNG-Initialisierung 1337, private Serververzeichnisse und Loopback. Betriebssystem-Scheduling und Rennen mit mehreren Autos bleiben variabel; ein einzelner Lauf beweist keine allgemeine Spielstärke.

Die Ergebnisse und Windows-Ladetests stehen in `dist/bot-racing-round1-results.json` und im beiliegenden Testpaket. `max_lap` ist die erreichte aktuelle Runde; abgeschlossene Runden sind hier `max_lap - 1`. Recovery-Anteile zählen Messpunkte, Rücksetzungen einzelne Ereignisse.
