# Deathmatch-Bots: erste Überarbeitung vom 24. Oktober 2026

Dieses Paket bearbeitet die vier vereinbarten Punkte für Deathmatch: Funktionsfehler, Fahrzeugregler/Recovery, Kampfentscheidungen und gemessene Abstimmung. Der neue Fahr- und Kampfpfad ist auf `GT_DEATHMATCH` begrenzt. Die anderen Modi behalten ihre bisherigen Entscheidungen. Eine gemeinsame Modusdefinition korrigiert allerdings auch deren zuvor falsche Botlib-Itemklassifikation.

## Änderungen

1. **Funktionsfehler:** Die Schussfreigabe verwendet die wirkliche Fahrzeugausrichtung und dieselbe gerundete, karosseriebezogene Mündung wie `FireWeapon`. Lenken hängt nicht mehr von `rightmove` ab, das der normale Maussteuerungsmodus ignoriert. Die Itemfilter von Engine-Botlib und Spiel verwenden dieselben expliziten Modusnummern.
2. **Fahrzeugregler:** Kampf, Verfolgung, Rückzug, Itemsuche und Aktivierungsnavigation liefern Zielpunkt und Zieltempo an `ai_vehicle.c`. Der Regler gibt den endgültigen Fahrbefehl nach der normalen Bot-Eingabekonvertierung aus. Zielen kann ihn dadurch nicht überschreiben. Er benutzt Fahrzeugbreite, geschwindigkeitsabhängige Vorschau, Bremsweg, geprüfte Seitenkorridore und Boden-/Gefahrenprüfung. Bei fehlendem Fortschritt folgen eine geprüfte Rückwärtsfahrt, ein Wendeversuch und nach wiederholtem Scheitern eine neue Routensuche. Es gibt keinen zusätzlichen Teleport oder Sonder-Respawn.
3. **Kampfentscheidungen:** Alle wahrnehmbaren zulässigen Gegner werden nach Entfernung, Fahrzeugausrichtung, sichtbarer Bedrohung und Sichtbarkeit bewertet. Ein Haltebonus stabilisiert das bestehende Ziel. Waffenabhängige Abstände und Anfahrt, Feuerphase, Absetzen und Wenden ersetzen die bisher im Deathmatch wirkungslose Phasenlenkung. Geschwindigkeit wird auch nach Skill abgestuft. Während der Reaktionszeit bleibt das aktuelle Gegnerziel als Fahrziel verfügbar.
4. **Messung:** Optionale `g_botDmDebug 1`-Samples, Auswerter und isolierter Headless-Lauf ermöglichen Vorher-/Nachher-Vergleiche. Anfangsabstände und Geschwindigkeiten wurden als erste Abstimmung gewählt; sie sind keine abschließend kalibrierte Schwierigkeitskurve.

## Prüfung

Die fünf neuen Testfunktionen prüfen tatsächlichen Produktionscode mit kontrollierter Welt: Schussgeometrie, Gegnerwahl, Kampfplanung, Fahrzeugregler und Botlib-Itemfilter. Die Fahrzeugwelt berechnet Sweeps gegen Hindernisse, Boden, Abgrund und Lava. Sie deckt insbesondere eine unsichere diagonale Abzweigung bei gleichzeitig freier Geradeausspur ab.

Zusammen mit Modusklassifikation, CLI-Abgleich und bestehenden Bot-Hilfstests bestehen **16 Testfunktionen**. Native Linux-Builds des alten und neuen Stands sowie Windows-Client, Dedicated Server, native Spiel-DLL und Spiel-QVM wurden erfolgreich gebaut. Unter Windows bestanden echte Dedicated-Server-Starts mit Bots sowohl mit der nativen Spiel-DLL als auch mit der JIT-kompilierten Spiel-QVM und endeten mit sauberem Shutdown. Das separate Test-Overlay enthält keine Karten- oder Fahrzeugdaten.

Der Vergleichsstand ist Git `35067eb0`, ergänzt ausschließlich um passive Protokollierung aus `tools/bot_dm_baseline_logging.patch`. Beide Stände verwenden identische vier Skill-4-Bots (Emma, Ivan, Paul, Steve), eine Initialisierung des libc-Zufallszahlengenerators mit demselben Seed, 20 Serverframes/s, synchrone Clients, keinen Chat, kein Warmup und keine Ladder-Übertragung. Gemessen werden jeweils die ersten 180 simulierten Sekunden. `timescale 10` verkürzt die Laufzeit; Scheduling und Physik sind dadurch nicht garantiert identisch oder vollständig deterministisch.

Die finalen Messergebnisse stehen in `dist/bot-dm-results.json`. `sampled_pers_hits` ist der gesampelte Spielzähler `PERS_HITS`, keine berechnete Trefferquote; `firing_sample_share` ist der Anteil mit gedrücktem Feuerknopf, keine Zahl abgegebener Schüsse. Stillstandsanteile sind ebenfalls keine automatische Feststeck-Erkennung: Bremsen, Kontakte und gewolltes Warten zählen mit.

Finaler Stand nach der Korrektur der diagonalen Gefahrenprüfung, jeweils vier Bots und 180 Sekunden:

| Arena | Seed | Abschüsse vorher → nachher | PERS_HITS vorher → nachher | mittlere Richtungsabweichung beim Feuern vorher → nachher |
|---|---|---|---|---|
| q3r_dm02 | 1337 | 2 → 4 | 43 → 85 | 57,5° → 13,5° |
| q3r_dm02 | 2026 | 1 → 8 | 32 → 123 | 52,2° → 13,5° |
| q3r_demobowl | 1337 | 6 → 11 | 78 → 173 | 59,2° → 14,3° |
| q3r_demobowl | 2026 | 2 → 12 | 46 → 163 | 52,9° → 13,8° |

Das zeigt in diesen Läufen wirksameren Kampf und weniger Feuer in falsche Richtung. Auf Demobowl sank der Anteil langsamer Samples von 45,7 auf 39,6 % beziehungsweise 51,7 auf 28,7 %. Auf dm02 stieg er dagegen; beide Stände haben außerdem Map-Ausreißer. Daraus folgt keine pauschal behobene Navigation oder allgemeine Spielstärke. Die jetzigen Werte sind die erste belastbare Basis für weitere Abstimmung.

## Wiederholen

Unter Linux/WSL den kleinen Test-Seed-Wrapper bauen:

```sh
gcc -shared -fPIC tools/bot_dm_seed.c -o dist/bot-dm-seed.so -ldl
```

Der Runner erwartet die beiden gebauten Stände unter `dist/bot-dm-build/{baseline,current}/release-linux-x86_64/`, jeweils mit Dedicated Server und `baseq3r/qagamex86_64.so`. Er kopiert nur die Spielbibliothek in einen neuen isolierten Homepath, liest die vorhandenen Karten aus dem Workspace und bindet den Server an Loopback. Bereits vorhandene Laufordner werden nicht überschrieben.

```sh
python3 tools/bot_dm_benchmark.py baseline --map q3r_demobowl --seed 1337 --run example
python3 tools/bot_dm_benchmark.py current --map q3r_demobowl --seed 1337 --run example
python3 tools/bot_dm_metrics.py \
  dist/bot-dm-runs/baseline-q3r_demobowl-1337-example/baseq3r/match.log \
  dist/bot-dm-runs/current-q3r_demobowl-1337-example/baseq3r/match.log
```

Für neue Builds den vorhandenen Makefile verwenden. Die schlanken nativen Ziele sind `q3rally-server.x86_64` und `baseq3r/qagamex86_64.so`; `TARGETS` begrenzt den Build auf diese Dateien. Baseline-Quellen lassen sich mit `git archive 35067eb0 engine/Makefile engine/code engine/ui` separat entpacken und dort mit dem passiven Logging-Patch ergänzen. Änderungen werden nicht in eine laufende Installation kopiert.

## Grenzen dieses Stands

- Kurze Bot-gegen-Bot-Läufe messen Aktivität und Stabilität, keine allgemeine Stärke gegen Menschen und kein fertiges Spielgefühl. Weitere Fahrzeugtypen, Skillstufen, acht oder mehr Bots und lange Matches brauchen eigene Läufe.
- Zusammenfahren und langsame Gruppen in der Arenamitte bleiben möglich. Verlässliches Vorbeifahren an dynamischen Gegnergruppen ist ein nächster Abstimmungspunkt.
- Auf `q3r_dm02` können sowohl alte als auch neue Bots weiterhin außerhalb der Map geraten. Der Regler besitzt Bodenprüfung; er garantiert bei Sprüngen, Kontakten und unzureichenden Kartengrenzen keine Rückkehr. Dieser Befund wird nicht als behoben gewertet.
- Bestehende Grenzen der Schussfreigabe und des vertikalen Autoaim bleiben bestehen. Die eigentliche Schussausrichtung ist korrigiert; ±25 Grad des allgemeinen entfernten Sichtfelds können weiterhin Fehlschüsse erlauben.
- Shootable-Aktivierungsziele verwenden teilweise weiterhin alte Blick-/Ausrichtungsregeln. Das ist keine neue, vollständig gelöste Fahrzeuginteraktion.
- Der neue Regler ist als gemeinsame Grundlage vorbereitet. Racing, LCS, Autoball und Teammodi werden erst in ihren eigenen Arbeitspaketen angeschlossen.
