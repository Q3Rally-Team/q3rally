# Jungle Opening (q3r_david4)

Capture the Flag für zwei Teams auf einer Dschungellichtung bei Nacht. Überarbeitung
der Map von |CE & P3rlE: gleiches Konzept, gleiche Waffen, neu gebaut mit Blick auf die
Erreichbarkeit der Flaggen. Erzeugt von `tools/maptools/make_david4.py`
(Python 3 + Pillow).

## Aufbau

Spiegelsymmetrisch zum Bach in der Mitte (Rot im Westen, Blau im Osten), die Flaggen
liegen 5.120 Units auseinander.

- **Basen:** Die Flagge steht auf einem flachen achteckigen Hügel (40 Units hoch). Das
  Rampenkreuz in Teamfarbe aus dem Original ist geblieben, die Ecken dazwischen sind
  jetzt Steinrampen. Die Flagge ist dadurch aus jeder Richtung erreichbar. Alle
  Steigungen sind sanft und an den Kanten gerundet (keine Absprungkanten), ein Auto
  kann quer über die Kuppe fahren und auf der anderen Seite weiter. Das Flaggenpad
  leuchtet in Teamfarbe. Zwei Flutlichtmasten mit Strahlern in Teamfarbe (Modell,
  `docs/lanterns.md`) und farbige Lampen markieren die Basis.
- **Außenposten:** Hinter jeder Flagge eine Halle zum Durchfahren (vorne und hinten
  offen) mit BFG (Südseite), Granatwerfer (Nordseite), Munition und Gesundheit.
- **Keine Leitplanken mehr:** Die roten und blauen Leitplanken um die alten Basen waren
  Sackgassen. Die Basisgrenze ist jetzt eine aufgemalte Pfeillinie am Boden.
- **Mitte:** Ein flacher Bach läuft entlang der Mittellinie. Im Norden kommt er als
  Kaskade in einen Teich; das Quad liegt auf einer flachen Steinplatte im Bach im
  Eichenhain. In der Mitte eine breite Kiesfurt mit dem Flammenwerfer. Im Süden fließt
  der Bach unter dem Wasserturm "Save Ferris" durch (Unsichtbarkeit unter dem Tank) und
  verschwindet in einem Durchlass. Der Bach ist überall befahrbar.
- **Deckung und Orientierung:** Felsgruppen, vor jeder Basis ein altes Steintor auf der
  Mittellinie, kleine Hügel, Laternen an den ausgefahrenen Erdwegen.
- **Rand:** Hügel mit Bäumen rund um die Lichtung, dahinter die Waldwand des Originals.
  Eine unsichtbare Wand hält die Autos auf der Lichtung.

## Items

| Item | Ort |
|---|---|
| BFG, Granatwerfer | Außenposten hinter jeder Flagge |
| Raketenwerfer | Nordflanke jeder Basis |
| Lightning Gun | Südflanke jeder Basis |
| Flammenwerfer | Furt in der Mitte |
| Quad Damage | Steinplatte im Bach (Norden) |
| Unsichtbarkeit | unter dem Wasserturm (Süden) |
| Gesundheit | Außenposten, beide Flanken, Hügel in der Mitte |
| Treibstoff | vier Kanister an den Flanken der Mitte |

16 Startpunkte pro Team (8 `team_CTF_<team>player` für den Matchstart, 8
`team_CTF_<team>spawn` für Respawns), alle mit Blick aufs Feld.

## Bauen

```text
python tools/maptools/make_david4.py
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -meta -custinfoparms -keeplights <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -vis -saveprt <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -light -custinfoparms -fast -shade -filter -super 2 -bounce 1 <map>
```

Die AAS entsteht wie bei Canyon Rush aus der Ersatzkarte (siehe
`docs/maps/aas_proxy.md`): deutlich weniger und größere Bodenflächen als bspc aus den
12.000 Geländedreiecken macht, und nur ein Viertel der Reachabilities.

```text
python tools/maptools/make_aas_proxy.py --above 6 --below 10 baseq3r/maps/q3r_david4.map
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -bsp -custinfoparms baseq3r/maps/q3r_david4_aasproxy.map
bspc -forcesidesvisible -optimize -bsp2aas baseq3r/maps/q3r_david4_aasproxy.bsp
python tools/maptools/make_aas_proxy.py --checksum baseq3r/maps/q3r_david4_aasproxy.aas baseq3r/maps/q3r_david4.bsp
```

Der Generator schreibt die Map und die Indexmap `maps/q3r_david4_index.pcx`,
`scripts/q3r_david4.shader` und `.arena`, `textures/q3r_david4/macro.jpg` sowie
`docs/maps/david4/david4_layout.png` und `validation.json`. `shaderlist.txt` muss
`q3r_david4` enthalten.

`validation.json` prüft unter anderem die Erreichbarkeit der Flaggen: 64 gerade
Anfahrten aus 900 Units Entfernung, jeweils autobreit, mit der steilsten Stelle und
jedem festen Brush im Weg (erwartet: keiner; die steilste Stelle ist die 2 Units hohe
Kante des Pads).

Das Bachwasser hat keinen Flüssigkeitsinhalt (es ist nur 12 Units tief): Autos und Bots
behandeln das Bachbett als Boden.

## Bots

Von jedem Teamspawn gibt es eine AAS-Route zur gegnerischen Flagge und von jeder Flagge
zurück zur eigenen Basis (an der fertigen AAS geprüft). Hinweis: Das Fahrverhalten der
Bots (`ai_vehicle.c`) ist derzeit nur im Deathmatch aktiv. In CTF nutzen die Bots die
normale Q3-Bewegung und kommen auf keiner Map weit.
