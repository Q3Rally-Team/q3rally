# Racing-Bots Runde 2: Startaufstellung und Rückwärtsfahrt

## Befund

Valley hat acht nummerierte `info_player_start`-Positionen. Im ersten Test standen alle vier Bots dort korrekt. Die beiden Autos aus Reihe 2 wurden anschließend **während des Countdowns** versetzt: Die Ghost-Recovery sah 308 beziehungsweise 327 Units Abstand zur Aufnahmelinie, löste `off_route_rejoin` aus und setzte beide nach einem Timeout bei 6,85 Sekunden auf Positionen weit vor dem Grid zurück. Das erklärt die vom Spieler beobachteten zwei ungewöhnlich weit vorn stehenden Autos; die Map-Starts fehlen nicht.

Die Rückwärtsbewegung hat zwei Ursachen. Der Recovery-Zweig wählte teils eine Blickrichtung *vom* Streckenziel weg und befahl dann Rückwärtsfahrt; damit fuhr er wieder auf das Ziel beziehungsweise Hindernis zu. Zusätzlich wandelte die alte Hindernisprüfung auch bei Schrittgeschwindigkeit eine blockierte Vorwärtsspur unmittelbar in einen Rückwärtsbefehl um. Die Telemetrie unterscheidet jetzt Fahrbefehl, signierte Geschwindigkeit und Recovery-Zustand.

## Änderung

- Während Intro und Countdown bleiben alle Racing-Bots auf dem vom Spiel zugewiesenen Gridplatz. Der Navigationsknoten erteilt keine Fahrbefehle und darf keine Recovery oder Notfall-Rücksetzung auslösen.
- In den ersten drei Sekunden nach Rennstart löst der größere, reguläre Abstand der hinteren Startreihe zur Ghost-Linie keine Off-Route-Recovery aus. Hindernis- und Feststeckprüfung bleiben anschließend aktiv.
- Gezieltes Rückwärtsfahren richtet das Auto zur Route aus und bewegt es rückwärts von ihr weg. Die maximale Rückwärtsphase sinkt von 1,35 auf 0,8 Sekunden.
- Bei weniger als 80 Units/s fordert eine blockierte Vorwärtsspur zunächst keinen Rückwärtsgang an. Anhaltende fehlende Bewegung kann weiterhin die ausdrückliche Recovery aktivieren.
- Die Feststeckprüfung fordert nun unter 25 Units Ortsfortschritt in 0,55 Sekunden und unter 50 Units/s Momentangeschwindigkeit. Die bisherige Schwelle von 70 Units in diesem Zeitfenster kennzeichnete auch regulär langsam fahrende Autos als blockiert.

## Prüfung

Vier Nightmare-Bots auf `q3r_valley`, Streckenlänge 1, normale Richtung, `timescale 1`. Die Läufe verwenden dieselben lokalen Ghost-Dateien und eine feste libc-RNG-Initialisierung. Fahrzeugkontakte und Scheduling bleiben variabel. Die JSON-Messwerte im Testpaket enthalten sämtliche Bots einzeln.

Im Lauf mit allein korrigiertem Grid blieben alle vier während des Countdowns auf ihren zugewiesenen Startpositionen. Im finalen Lauf betrug die maximale XY-Abweichung je Bot 0,0 bis 0,1 Units. Der erste Lauf aus Runde 1 zeigte dagegen zwei Notfall-Rücksetzungen während des Countdowns um mehrere hundert Units nach vorn.

Die Messung trennt Rückwärtsbefehl, tatsächliche signierte Rückwärtsgeschwindigkeit unter −35 Units/s und explizite Recovery. Ein Bremsbefehl bei Vorwärtsfahrt ist noch keine Rückwärtsbewegung. Die Werte summieren die vier Bots über jeweils 180 Spielsekunden:

| Stand | Rückwärtsbewegung | Rückwärtsbefehl | Resets im Rennen | Beste vollständige Bot-Runde |
| --- | ---: | ---: | ---: | ---: |
| Nur Grid-Fix | 62,3 Bot-Sekunden | 105,9 Bot-Sekunden | 2 | 34,3 s |
| Dazu Rückwärtslenkung und kurze Recovery | 51,3 Bot-Sekunden | 85,6 Bot-Sekunden | 0 | 37,3 s |
| Final: engere Feststeckprüfung | 27,5 Bot-Sekunden | 73,9 Bot-Sekunden | 6 | 35,3 s |

Der finale Stand reduzierte die gemessene Rückwärtsbewegung gegenüber dem Grid-Fix-Lauf um 56 %. Die sechs Resets im Rennen sind weiterhin ein Problem. Bei mehreren davon stand der Bot vor einer blockierten Vorwärtsspur und konnte die Route nicht mehr selbst erreichen. Ein weiterer Versuch mit sofortigem kurzem Rückwärtsfahren an dieser Stelle senkte die Resets auf einen, erhöhte aber die Rückwärtsbewegung auf 50,0 Bot-Sekunden und verschlechterte die beste Runde auf 38,1 Sekunden. Er ist daher nicht im Build. Einzelne Läufe mit Fahrzeugkontakten erlauben keine belastbare Aussage über einen allgemeinen Tempofortschritt; gegenüber den 34,3 Sekunden im Grid-Fix-Lauf ist die beste finale Runde langsamer.

Die genauen Botwerte stehen in `bot-racing-round2-results.json`.

Windows-DLL und QVM werden in einem headless Server auf `Race_Bot_Test` geladen. Die vorhandenen Ghost-/Racing-/DM-/Physikregressionen umfassen 15 Python-Testfunktionen mit ihren C-Testfällen.

Die Rückwärtsfahrt und Geschwindigkeit hängen weiterhin von Fahrzeug, Verkehr und Streckenabschnitt ab. Der vom Spieler beobachtete Rückstand von 7,5 Sekunden ist mit diesem Stand nicht nachweislich behoben. Für das nächste Racing-Tuning sind die blockierten Kurvenabschnitte und die Brems-/Lenkregelung wichtiger als ein höherer pauschaler Zielgeschwindigkeitswert.
