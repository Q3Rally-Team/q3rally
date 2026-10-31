# Laternen- und Flutlicht-Modelle

Modelle für Laternen und Flutlichtmasten in den Maps, als `misc_model` gesetzt (q3map2
backt sie in die BSP). Erzeugt von `tools/mapobjects/make_lanterns.py` (Python 3 + numpy,
gemeinsame Mesh-Werkzeuge in `tools/mapobjects/meshlib.py`), Shader in
`scripts/q3r_lanterns.shader`, Dateien unter `models/mapobjects/q3r_lanterns/`.

| Modell | Was | Ursprung | Maße (Units) |
|---|---|---|---|
| `lantern.md3` | Hängelaterne: Ring, Kaminhaube, überstehendes Dach, nach unten schmaler werdendes Glas mit Eisenrahmen, Boden mit Spitze, Flamme innen | Oberkante des Rings, hängt nach unten | 22 breit, 48 hoch |
| `lantern_post.md3` | Gusseiserner Laternenpfahl mit Sockel, Ringen, Kugelkopf, geschwungenem Arm mit Zierschnecke; die Laterne hängt an zwei Kettengliedern | Boden, Arm zeigt nach +x | 160 hoch, Laterne 34 vom Pfahl |
| `lantern_wall.md3` | Wandarm mit Platte, Strebe und Laterne | Wandfläche, Arm nach +x | Arm 22, Laterne hängt bis 51 unter den Ansatz |
| `streetlamp.md3` | Natriumdampf-Straßenlaterne: konischer Mast, Schwanenhals, flacher Leuchtenkopf mit glühender Unterseite | Boden, Arm nach +x | 328 hoch, Kopf 88 vom Mast, Glas bei z 316 |
| `floodmast.md3` | Flutlichtmast: konischer, verzinkter Mast mit Fußflansch, Muffen, Kabelkasten, Steigleiter mit Rückenschutz, runde Plattform mit Geländer, Rahmen mit sechs geneigten Strahlern | Boden, Strahler zeigen nach +x | 500 hoch, Plattform bei z 428, Strahler bei z 460 und 486 |
| `floodmast_red.md3`, `floodmast_blue.md3` | wie `floodmast.md3`, Gläser in Teamfarbe | | |

Die Ausrichtung setzt der Schlüssel `angle` (0 = Arm bzw. Strahler nach +x).

MD3-Koordinaten reichen nur bis ±512. Der Flutlichtmast ist deshalb 500 hoch gebaut; höhere
oder niedrigere Masten bekommen `modelscale` (Industrial Crossfire 1.3 = 650, Jungle Opening
0.86 = 430, Ridge Pass 1.92 = 960).

## Licht und Kollision

Die Modelle leuchten selbst (Glas und Flamme flackern leicht über `rgbGen wave noise`,
die Linse der Straßenlaterne glüht), werfen aber kein Licht. Daneben gehört eine
`light`-Entity:

- Laternenpfahl: bei (34, 0, 106) relativ zum Ursprung, warm (1 0.72 0.38), 190–220.
- Wandlaterne: bei (22, 0, -33) relativ zum Wandansatz.
- Straßenlaterne: unter dem Kopf, bei (88, 0, 290), Natriumgelb (1 0.72 0.38), 320.
- Flutlichtmast: vor den Strahlern und etwas darunter, Wert 700–900. Nicht zu nah an den
  Mast setzen, sonst überstrahlt das Licht den Mast (Jungle Opening: 170 vor dem Mast,
  z +360; Industrial Crossfire: 80 vor dem Mast, z 580).

`misc_model` kollidiert nicht. Wo Autos gegen den Pfahl fahren können, kommt ein
unsichtbarer, fester Brush um den Schaft (Laternenpfahl 10 × 10, Straßenlaterne 12 × 12,
Flutlichtmast etwa 26 × 26 mal `modelscale`). Arm, Laterne und Flutlichtkopf hängen über Autohöhe.

## Wo sie stehen

- `q3r_david4` (Jungle Opening): 12 Laternenpfähle an den Erdwegen, 2 an der
  Quad-Platte, 4 Wandlaternen an den Ruinentoren.
- `q3r_david4` (Jungle Opening): 4 Flutlichtmasten in Teamfarbe an den hinteren Ecken der
  Basen, auf den Flaggenhügel gerichtet.
- `q3r_ctf4_03` (Industrial Crossfire): 24 Straßenlaternen an den Kaistraßen, 8
  Flutlichtmasten an den Hofecken, auf die Flagge gerichtet.
- `q3r_ridgepass` (Ridge Pass): 2 Flutlichtmasten an den Enden der Tribüne, `modelscale`
  1.92 (rund 960 hoch), auf die Mitte der Start-Ziel-Geraden gerichtet.

`shaderlist.txt` muss `q3r_lanterns` enthalten, damit q3map2 die Shader kennt.
