# Deathmatch-Bots – Runde 3

Teststand vom 25. Oktober 2026. Ausgangspunkt ist Runde 2. Schwerpunkt: Stabilität, Fahrzeuggruppen und die niedrigeren Skillstufen. Die Nightmare-Feuerparameter bleiben erhalten.

## Änderungen

- **Gemeinsame Fahrphysik:** Bei zwei gleichzeitigen Kontakten wurde die Geschwindigkeit fälschlich zweimal gegen die erste Flächennormale korrigiert. Jetzt werden beide Flächen und gegebenenfalls eine dritte blockierte Richtung berücksichtigt. Eine zusätzliche Prüfung des vorgeschlagenen Chassiswegs verhindert bestimmte Durchtritte durch dünne Weltflächen. Sie versetzt keine bereits außerhalb der Karte befindlichen Fahrzeuge und verhindert keinen tatsächlichen Absturz über einen Abgrund. Diese gemeinsame Physikkorrektur betrifft auch andere Modi und die Client-Vorhersage.
- **DM-Fahrprüfung:** Mitte und beide Radspuren werden entlang der Vorschau geprüft. Die Prüfung folgt dem Bodenverlauf auf Rampen und berücksichtigt dieselben Botclip-Flächen wie die Bot-Bewegung. Beim Wenden wird die tatsächlich angeforderte Richtung von 70 Grad geprüft.
- **Fahrzeuggruppen:** Eine anfängliche Überlappung mit konservativen Fahrzeugboxen sperrt eine freie Ausfahrt nicht mehr grundsätzlich. Erlaubt wird nur ein Weg, der den Abstand zu allen überlappenden Fahrzeugen vergrößert und keine Weltgeometrie durchquert. Die vorhandene Recovery und Routensuche bleiben zuständig.
- **Skill 1–4:** Klarere Grenzen für Reaktion, Feuerfolgen und Pausen. Besonders feuerfreudige Charakterprofile können diese Grenzen nicht mehr überspringen. Nightmare behält seine bisherigen Feuerparameter; das wird ausdrücklich getestet. Waffenschäden, Motorleistung und Reifenparameter wurden nicht geändert.
- **Diagnose:** Zusätzlich werden Kraftstoff, Geschwindigkeit, Ausrichtung und Gang protokolliert. Die Auswertung misst nun auch horizontalen Positionsfortschritt statt allein der Geschwindigkeit entlang der Karosserie.

## Korrektur der Messmethode

Die bisherigen Läufe mit `timescale 10` verändern das Kampfverhalten stark. Sie eignen sich als Belastungstest, **nicht als verlässlicher Vergleich der Spielstärke**. Die früheren Abschusstabellen müssen entsprechend eingeordnet werden. Neue Stärkevergleiche verwenden normale Spielgeschwindigkeit; der Runner nutzt sie jetzt standardmäßig und protokolliert die verwendeten Binärdateien mit SHA256.

## Normales Spieltempo

Jeweils 180 Spielsekunden, dieselben Botprofile, Skill 5, 20 Serverframes/s, synchrone Clients, kein Warmup, Chat oder Ladder. Vergleich mit den gesicherten Runde-2-Binaries:

| Arena | Seed | Bots | Abschüsse vorher → nachher | Samples unter Z = −4096 vorher → nachher |
|---|---|---|---|---|
| DM02 | 1337 | 4 | 61 → 52 | 0 → 0 |
| DM02 | 2026 | 4 | 49 → 62 | 152 → 0 |
| Demobowl | 2026 | 4 | 66 → 71 | 0 → 0 |
| DM02 | 1337 | 8 | 138 → 142 | 0 → 0 |

In allen acht aktuellen Normalläufen einschließlich der Skill-Stichprobe gab es keine Samples unter Z = −4096. Bei acht Nightmare-Bots sank die längste Phase mit wenig Positionsfortschritt von 10 auf 4,5 Sekunden. Einzelne lange Blockaden bleiben: Im Vier-Bot-Lauf auf DM02/1337 waren es maximal 36,5 Sekunden; dort sank auch die Abschusszahl. Eine generelle Stärke- oder Stabilitätsgarantie folgt daraus nicht.

Skill-Stichprobe auf DM02/1337, jeweils vier Bots derselben Stufe:

| Skill | Abschüsse | PERS_HITS |
|---|---|---|
| 1 | 16 | 455 |
| 2 | 29 | 707 |
| 3 | 51 | 843 |
| 4 | 59 | 1131 |
| 5 | 52 | 956 |

Die unteren Stufen zeigen hier zunehmendes Matchtempo. Skill 5 liegt in dieser Stichprobe unter Skill 4. Solche Selbstspiele mit ebenfalls stärkeren Gegnern beweisen keine direkte Stärkerangfolge. Kontrollierte Produktionstests bestätigen die beabsichtigte Staffelung der Schussfreigabe; eine über alle Situationen monotone Siegquote ist weiterhin nicht nachgewiesen. `PERS_HITS` ist ein Spielzähler, keine berechnete Trefferquote.

## Belastungstest und Grenzen

Jeweils 600 simulierte Sekunden bei zehnfacher Geschwindigkeit, Skill 5:

| Arena | Bots | Samples unter Z = −4096 vorher → nachher | längste Phase mit wenig Fortschritt vorher → nachher |
|---|---|---|---|
| DM02, Seed 1337 | 4 | 1520 → 0 | 39,5 s → 89,5 s |
| Demobowl, Seed 2026 | 4 | 0 → 0 | 494 s → 39,5 s |
| DM02, Seed 1337 | 8 | 1599 → 704 | 22 s → 45,5 s |

Ausreißer und lange Blockaden sind unter dieser künstlichen Belastung reduziert, aber weiterhin vorhanden. Viele Samples können zu einem einzigen lange außerhalb der Karte befindlichen Fahrzeug gehören. Wenig Positionsfortschritt ist außerdem keine automatische Feststeckdiagnose: Bremsen, Warten und Kämpfen zählen ebenfalls mit. Die ursprüngliche AAS-Navigation und weitere Physikgrenzfälle bleiben Gegenstand weiterer Spieltests.

## Prüfung und Paket

**22 Testfunktionen bestanden:** Bot-Kampf, Schussgeometrie, Skill-Freigabe, Gegnerwahl, Fahrzeugregler, Itemfilter, Modusklassifikation, Routenhilfen, Kontaktflächen, dünne Böden, Positionsauswertung sowie vorhandene Racing-, Zustands- und Windschattentests. Die neuen C-Tests verwenden Produktionscode.

Linux-Build sowie Windows-Client, Dedicated Server, beide nativen Spielmodule und beide QVMs wurden erfolgreich gebaut. Windows-Dedicated-Starts mit Spiel-DLL und JIT-QVM bestanden mit 30 bzw. 36 Bot-Samples und sauberem Shutdown. Ein interaktiver Windows-Client-Spieltest wurde nicht durchgeführt. Wegen der gemeinsamen Physikkorrektur benötigen auch die anderen Modi zusätzliche Spieltests; deren Bot-Entscheidungen wurden hier nicht neu abgestimmt.

**Alle sechs Programm- und Moduldateien gemeinsam übernehmen**, einschließlich der beiden `cgame`-Dateien. Karten und Fahrzeuge werden aus einer vorhandenen Installation verwendet. Die Ladder und die Actions-Konfiguration bleiben unverändert. Die lokalen Tests senden keine Ladder-Ergebnisse.

Rohmetriken: im Repository `dist/bot-dm-round3-results.json`, im ZIP `docs/bot-dm-round3-results.json`. Finale Läufe: `r3-normal-final3` und `r3-stress-final3`, Kontrollen `r3-realtime-control` bzw. `r3-stress-control`. Die Dateihashes der gemessenen Linux-Spielbibliothek stimmen mit dem abschließenden Build überein.
