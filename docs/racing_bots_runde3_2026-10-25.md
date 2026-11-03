# Racing-Bots Runde 3: Linienführung, Checkpoints und Turbo

## Ausgangspunkt

Im Valley-Screenshot fuhren fünf Nightmare-Bots 36,000 bis 45,050 Sekunden; der Spieler erreichte 26,350 Sekunden ohne NOS. Die Fahrzeuge haben dieselbe Performance. Alle Bots kamen an, aber der Abstand des schnellsten Bots betrug 9,650 Sekunden.

Die Runden-2-Telemetrie zeigte wiederholte harte Geschwindigkeitsverluste nach Wandkontakten. Ein Beispiel vor Checkpoint 7: 427 auf 73 Units/s in einem Tick. Die Racing-Führung hatte eine Ghost-Zielrichtung berechnet. `Bot_CheckForObstacles` ersetzte diese Richtung jedoch schon bei Treffern seiner seitlichen Sonden durch einen pauschalen 15-Grad-Lenkwinkel. In Valley sieht diese lange Sonde in Kurven die Streckenbegrenzung; die Korrektur kann gegen die aufgezeichnete Linie arbeiten.

## Aenderung

- Im normalen Racing-Fahrzustand bleiben die Richtung zur Ghost-Linie und die Bremsanforderung des Hindernisfühlers getrennt. Bei einer erkannten Fahrzeugkollision darf die Hindernisreaktion weiterhin die Richtung ändern. Recovery behält ihre eigene Lenkung.
- Die Debug-Telemetrie enthält Fahrzeug- und Ziel-Yaw sowie seitliche Geschwindigkeit relativ zur Route.
- Kurz vor einem noch nicht registrierten Checkpoint kann der festgehaltene Zielpunkt hinter das Fahrzeug geraten. Dann verlangte die Lenkung innerhalb von 0,2 Sekunden erst 93 und anschließend 170 Grad zurück. Liegt das Fahrzeug noch auf der Route und zeigt deren Tangente weiter nach vorn, fährt es nun durch das Tor, bis dessen Durchfahrt registriert wird. Bei größerer Abweichung bleibt die Checkpoint-Sperre erhalten.
- Racing-Bots aktivieren gespeicherten Turbo auf freien, schnellen Abschnitten. Dazu zählt auch der vom Spiel für saubere Checkpoint-Segmente verliehene Turbo. In Kurven, bei Kollisionen, größerer Linienabweichung und in Recovery wird er nicht angefordert. `g_botRaceUseTurbo` steuert die Funktion; Standard ist `1`, für den Vergleich ohne NOS ist der Wert `0`.
- Das isolierte Benchmark-Szenario kann jetzt die fünf Botnamen aus dem Screenshot fahren und deaktiviert NOS standardmäßig. `--bot-turbo` aktiviert es für einen Vergleich.

## Messung

Alle automatischen Läufe verwendeten `q3r_valley`, normale Richtung, Streckenlänge 1, Skill 5, lokale Ghost-Dateien und `timescale 1`. Fahrzeugkontakte und Server-Scheduling bleiben variabel; die Werte sind einzelne Läufe und keine statistische Siegquote.

| Vier Bots, je 180 Spielsekunden | Durchschnittstempo über Bots | Resets im Rennen | Abgeschlossene Runden je Bot | Beste Bot-Runde |
| --- | ---: | ---: | ---: | ---: |
| Runde 2 | 445 Units/s | 6 | 3 | 35,3 s |
| Ghost-Richtung beibehalten, ohne Turbo | 505 Units/s | 0 | 4 | 34,9 s |
| Checkpoint-Ziel korrigiert und Turbo aktiviert | 544 Units/s | 0 | 4 | 32,2 s |
| Checkpoint-Ziel korrigiert, NOS aus | 531 Units/s | 0 | 4 | 32,9 s |

Der Vergleich ohne NOS ist fair zu deiner Runde: Der Checkpoint-Fix senkte die beste gemessene Vier-Bot-Runde gegenüber Runde 2 von 35,3 auf 32,9 Sekunden; alle Bots beendeten vier Runden ohne Reset. Mit NOS lag die beste Vier-Bot-Runde bei 32,2 Sekunden. Im gleich großen Fünf-Bot-Test ohne NOS lagen die erste Runde des schnellsten Bots bei 34,8 und die nächste bei 32,9 Sekunden; alle fünf schafften zwei Runden mit sämtlichen Checkpoints und ohne Reset. Mit fünf Bots und NOS lag die beste erste Runde bei 35,2 und die beste spätere Runde bei 33,2 Sekunden. Die Unterschiede sind klein und variablem Verkehr ausgesetzt. NOS erklärt den Rückstand daher nicht.

Weitere isolierte Versuche wurden verworfen: Ein Dämpfungsversatz für seitliche Geschwindigkeit, eine pauschal höhere Kurvengeschwindigkeit, aggressiveres Gas und eine kürzere Zielpunkt-Vorschau verschlechterten Fahrfluss oder Rundenzeiten. Eine vorhandene 26,350-s-Roadster-Ghostlinie war für die fünf Bots trotz geringerer Streckenabweichung langsamer als die zuvor gewählte 22,600-s-Raptor-Linie.

Der Abstand zur 26,350-s-Spielerrunde ist weiterhin deutlich. Selbst die 32,9 Sekunden ohne NOS stammen aus Bot-Rennen ohne Spieler und sind kein direkter Duellvergleich. Der nächste Schritt braucht eine fahrzeugdynamische Kurvenregelung und eine Bewertung von Ghost-Linien nach tatsächlicher Bot-Fahrbarkeit. Ein höherer Geschwindigkeits-Sollwert allein hat die Bots in den Messläufen nicht schneller gemacht.

Die Einzelwerte stehen in `bot-racing-round3-results.json` im Testpaket.
