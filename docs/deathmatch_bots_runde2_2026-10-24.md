# Deathmatch-Bots, Runde 2 – 24. Oktober 2026

Ausgangspunkt ist der vom Spieler getestete Stand aus Runde 1: stimmigere Entscheidungen, aber zu wenig Angriffsdruck. Runde 2 bearbeitet Kampfbereitschaft, Kampftempo, Feuerfolgen und das Verhalten in Fahrzeuggruppen. Alle neuen Verhaltensänderungen sind auf `GT_DEATHMATCH` begrenzt; die Actions- und Ladder-Konfiguration bleibt unverändert.

## Was geändert wurde

- **Kampfbereitschaft:** Die bisherige Quake-3-Aggressionsbewertung gab der geladenen Startwaffe selbst bei voller Gesundheit keinen Kampfwert. Der neue DM-Pfad bewertet nutzbare Frontwaffen einschließlich Maschinengewehr. Kritische Gesundheit, fehlende Munition und ein alleiniger Nahkampf ohne nahe Gelegenheit bleiben Rückzugsgründe. Skill und Persönlichkeit beeinflussen die Entscheidung. Eine geladene Schrotflinte verlangt außerdem nicht mehr allein wegen fehlender Maschinengewehrmunition sofortigen Nachschub.
- **Angriff und Verfolgung:** Der pauschale Ausbruch nach 3,5 Sekunden ohne Treffer entfällt. Für Hitscan- und Nahkampfwaffen können nahe, frontal liegende Ziele weiter bekämpft werden. Explosivwaffen behalten ihren Sicherheitsabstand. Absetzen dauert kürzer; Kampfgeschwindigkeiten sind moderat erhöht. Bei einer beobachteten Flucht wird das Zieltempo anhand der AAS-Bewegungsdaten angehoben und begrenzt. Verdeckte Spielerzustände werden dafür nicht gelesen.
- **Skill:** Die Reaktionszeit skaliert mit Skill, mit mindestens 120 ms vor der Schussfreigabe. Höhere Stufen erhalten längere Feuerfolgen und kürzere, seltenere Pausen. Die alte Regel koppelte Pausenwahrscheinlichkeit und Pausenlänge gegensinnig; eine größere Kennzahl bedeutete dadurch nicht zuverlässig mehr Feuer. Tatsächliche Karosserieausrichtung, Mündung, Deckung und bestehende Waffensicherheitsprüfungen bleiben maßgeblich.
- **Fahrzeuggruppen:** Eine sichere Ausweichseite wird bis zu 650 ms beibehalten, solange sie weiterhin sicher ist. Boden, Lava und Hindernisse werden trotzdem in jedem AI-Tick neu geprüft. Ein weiter entfernter Gegner in der frontalen Feuerbahn führt nicht sofort zum Ausweichen; der Regler hält Bremsraum frei. Nahe Kontakte, andere Fahrzeuge und Wände bleiben Hindernisse.
- **Boden und Recovery:** Der Boden wird an drei Stellen über den geschwindigkeitsabhängigen Bremskorridor geprüft. Vorwärts steuert die verbleibende sichere Strecke die Bremsung; Ausweich- und Rückwärtskorridore müssen über ihre geprüfte Länge sicher sein. Fahrzeugkörper können die aktuelle Bodenprüfung nicht mehr verdecken und dadurch die Recovery abschalten.
- **Diagnose:** Die optionalen `BotDM`-Samples enthalten zusätzlich Waffe, Gesundheit, Schusszähler und Ausweichseite. Der Runner erlaubt `--skill 1` bis `--skill 5`, `--bots 4` oder `--bots 8` sowie den gesicherten Vergleichsstand `round1`.

## Prüfung und Vergleich

Die 16 relevanten Python-Testfunktionen prüfen den tatsächlichen C-Produktionscode. Erweiterte Fälle betreffen Startwaffe und kritische Gesundheit, brauchbare Waffenmunition, Waffenabstände, ausbleibende Treffer ohne pauschalen Rückzug, begrenzte Verfolgung, Skill-Reaktionszeit und Feuerpausen, stabile Ausweichrichtung, neue Gefahren während eines Ausweichmanövers, entfernte Abgründe, Löcher vor einem sicheren Gegenhang und Recovery bei überlappenden Fahrzeugen. Bestehende Schussgeometrie-, Gegnerwahl-, Itemfilter- und Modusprüfungen bleiben enthalten.

Native Linux-Server und Spielmodul sowie Windows-Client, Dedicated Server, Spiel-DLL und QVM wurden erfolgreich gebaut. Unter Windows liefen echte lokale Bot-Matches sowohl mit der DLL (`vm_game 0`) als auch mit der JIT-QVM (`vm_game 2`) bis zum sauberen Shutdown. Ein interaktiver Client-Spieltest und ein Ladder-Upload wurden nicht durchgeführt.

Verglichen wird mit den vor Runde 2 gesicherten Runde-1-Binaries. Vier Bots (Emma, Ivan, Paul, Steve), Skill 4, Seeds 1337 und 2026, 20 Serverframes/s, synchrone Clients und jeweils die ersten 180 simulierten Sekunden. Chat, Warmup und Ladder sind abgeschaltet. `timescale 10` beschleunigt die Messung. Fester libc-Startseed macht Scheduling und Physik nicht vollständig deterministisch. Zusätzliche Läufe verwenden Skill 1/5 beziehungsweise acht Bots.

Die endgültigen Rohmetriken stehen im Repository in `dist/bot-dm-round2-results.json` und im Test-ZIP unter `docs/bot-dm-round2-results.json`. `PERS_HITS` ist der gesampelte Spielzähler, keine Trefferquote. Der Anteil sehr langsamer Samples misst den Betrag der Geschwindigkeit entlang der Karosserie unter 60 Einheiten/s; er ist keine vollständige Feststeck- oder Kollisionsmessung. Die neuen Schusszähler werden mitgeschrieben, aber nicht als waffenübergreifende Genauigkeitsquote ausgewiesen.

Finaler Stand, vier Skill-4-Bots, 180 Sekunden; jeweils Runde 1 → Runde 2:

| Arena | Seed | Abschüsse | PERS_HITS | Anteil sehr langsamer Samples |
|---|---|---|---|---|
| q3r_dm02 | 1337 | 4 → 5 | 85 → 61 | 34,4 % → 31,8 % |
| q3r_dm02 | 2026 | 8 → 13 | 123 → 214 | 36,1 % → 40,7 % |
| q3r_demobowl | 1337 | 11 → 20 | 173 → 216 | 39,6 % → 20,7 % |
| q3r_demobowl | 2026 | 12 → 9 | 163 → 122 | 28,7 % → 26,2 % |

In drei der vier Läufe steigen die Abschüsse. Das ist kein durchgehender Stärkegewinn: Demobowl/2026 wird schlechter, und dm02/1337 hat weniger `PERS_HITS`. Die mittlere Geschwindigkeit entlang der Karosserie steigt in diesen Läufen von 192,5 → 208,2, 185,2 → 186,8, 185,1 → 255,6 beziehungsweise 219,4 → 226,9 Einheiten/s. Weniger langsame Samples und mehr Tempo garantieren keine bessere Navigation.

Map-Ausreißer bleiben ein wesentlicher Befund. Die Zahl gesampelter Positionen unter Z = −4096 beträgt auf dm02 245 → 222 beziehungsweise 87 → 0; auf Demobowl 0 → 0 beziehungsweise **0 → 309**. Letzterer Lauf ist damit ausdrücklich eine Stabilitätsregression gegenüber Runde 1. Ohne Kill-Volume bzw. zuverlässige Rückkehr kann ein einzelner Ausreißer lange aus dem Kampf fehlen. Diese Daten rechtfertigen einen weiteren Spieltest, keine Freigabe als durchgehend stabilerer Botstand.

Zusätzlicher Vergleich auf Demobowl, Seed 1337, acht Skill-4-Bots:

| Stand | Abschüsse | PERS_HITS | Sehr langsame Samples | Samples unter Z = −4096 |
|---|---|---|---|---|
| Runde 1 | 19 | 314 | 36,8 % | 0 |
| Runde 2 | 23 | 362 | 33,4 % | 0 |

Skill-Stichprobe auf derselben Arena mit vier Bots, jeweils 180 Sekunden:

| Skill | Abschüsse | PERS_HITS | Sehr langsame Samples |
|---|---|---|---|
| 1 | 7 | 105 | 22,9 % |
| 4 | 20 | 216 | 20,7 % |
| 5 | 11 | 155 | 56,9 % |

Skill 5 ist hier aktiver im Feuer und erfolgreicher als Skill 1, erreicht aber nicht Skill 4 und hat mehr langsame Samples. Eine monotone Schwierigkeitskurve ist damit **noch nicht nachgewiesen**. Charakterprofile, Waffenwahl und Fahrzeugbewegung brauchen dafür weitere gemeinsame Abstimmung. In diesen drei Skill-Läufen gab es keine Samples unter Z = −4096.

Die finalen Runde-2-Läufe tragen den Runner-Suffix `r2-final2`, die Skill-Läufe `r2-final2-skill1` bzw. `r2-final2-skill5`, der Acht-Bot-Lauf `r2-final2-eight`. Frühere Entwicklungsversuche sind nicht in der Ergebnistabelle enthalten. Der gesicherte Runde-1-Lauf mit vier Bots auf Demobowl/1337 wurde zusätzlich erneut ausgeführt und ergab dieselben 11 Abschüsse und 173 `PERS_HITS` wie die ursprüngliche Messung.

Beispiel zum Wiederholen mit vorhandenem Linux-Build und Seed-Wrapper:

```sh
python3 tools/bot_dm_benchmark.py round1 --map q3r_demobowl --seed 1337 --run neuer-vergleich
python3 tools/bot_dm_benchmark.py current --map q3r_demobowl --seed 1337 --run neuer-vergleich
python3 tools/bot_dm_benchmark.py current --map q3r_demobowl --seed 1337 --skill 5 --bots 8 --run neuer-skill5-acht
```

Der Runner legt pro Lauf einen neuen Homepath an und überschreibt keine vorhandenen Ergebnisse. `round1` erwartet die vor der Überarbeitung gesicherten Binaries unter `dist/bot-dm-build/round1/release-linux-x86_64/`; `current` verwendet den zuletzt gebauten Stand.

## Grenzen

Die Abstimmung zielt auf höheren Angriffsdruck, nicht auf eine Garantie höherer Abschusszahlen in jedem Lauf. Fahrzeugkontakte, Gruppen und Ausreißer außerhalb der Karte bleiben offen. Die Bodenprüfung ersetzt weder eine Kartenbegrenzung noch eine vollständige Fahrphysik- oder AAS-Überarbeitung. Sichtfeld und vertikales Autoaim wurden in dieser Runde nicht neu gebaut. Mehrere Fahrzeugtypen, längere Matches und das Spielgefühl gegen Menschen brauchen weitere Spieltests.

Runde 1 bleibt als separates Test-ZIP erhalten. Das neue Paket ersetzt nur die vier Programm-/Spielmoduldateien einer vorhandenen Installation; Karten und Fahrzeuge werden nicht verändert. Andere Spielmodi werden in ihren eigenen Paketen überarbeitet.
