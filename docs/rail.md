# Gleis- und Waggon-Modelle

Gleise und Güterwagen als `misc_model` statt Brushes (q3map2 backt sie in die BSP).
Erzeugt von `tools/mapobjects/make_rail.py` (Python 3 + numpy, Mesh-Werkzeuge aus
`tools/mapobjects/meshlib.py`), Shader in `scripts/q3r_rail.shader`, Dateien unter
`models/mapobjects/q3r_rail/`.

| Modell | Was | Maße (Units) |
|---|---|---|
| `track.md3` | 256 Units Gleis: zwei Schienen mit Fuß, Steg und Kopf, acht Holzschwellen mit Rippenplatten | Spurweite 72 (Schienen bei y ±36), Schienenoberkante z 7, Schwellen 112 lang |
| `boxcar_brown.md3`, `boxcar_grey.md3`, `boxcar_red.md3` | Gedeckter Güterwagen: zwei Drehgestelle mit Radsätzen, Federn und Achslagern, Rahmen, Puffer und Kupplungshaken, Wagenkasten mit Rippen, Schiebetüren auf Laufschienen, Griffe, Leitern an allen Ecken, Handbremsrad am +x-Ende, gewölbtes Dach | 360 lang (mit Puffern 392), 104 breit, 195 hoch |
| `tankcar.md3` | Kesselwagen: Kessel r 54 mit gewölbten Böden und Spannbändern, Sättel, Dom mit Deckel und Ventilen, Laufsteg mit Geländer, Leitern, Endbühnen | 340 lang, Kesselachse z 104, 183 hoch |
| `flatcar.md3` | Flachwagen: Bohlenboden, Seitenträger mit Rungentaschen, Containerverriegelungen | 300 × 80, Ladefläche z 40 |

## Platzieren

- Ursprung jeweils auf dem Boden in der Mitte, Länge entlang +x. `angle` 90 legt das
  Modell entlang y.
- Gleise: Abschnitte von 256 Units aneinanderreihen. Passt die Länge nicht genau,
  streckt `modelscale_vec "<f> 1 1"` jeden Abschnitt etwas (Industrial Crossfire teilt
  die Strecke in gleich lange Stücke). Darunter ein Schotterbett als Boden-Decal.
- Waggons stehen auf dem Gleis (Räder auf z 7), Ursprung auf der Gleismitte.
  Ladung (Container) kommt auf den Flachwagen bei z 40.

## Kollision und Licht

`misc_model` kollidiert nicht. Jeder Waggon bekommt einen unsichtbaren, festen Brush
(Shader `propclip` in der Map: nodraw, trans, detail, `SURF_METAL`); er hält Autos und
Geschosse auf und lässt Licht durch, den Schatten wirft das Modell:

- Gedeckter Wagen: Drehgestell-Block 300 × 80, z 7–44, Wagenkasten 360 × 104, z 44–194.
- Kesselwagen: Drehgestell-Block 280 × 80, z 7–44, achteckiges Prisma r 54 um die
  Kesselachse (z 104), 340 lang.
- Flachwagen: 300 × 80, z 7–40.
- Gleis: zwei flache Boxen 8 breit, z 0–7, unter den Schienen.

Kleine Teile sind vertex-beleuchtet. Die großen Flächen (Wagenkasten, Dach, Kessel,
Ladefläche) nutzen `q3map_forceMeta` und bekommen eine Lightmap, damit Schatten
sauber darauf fallen.

## Wo sie stehen

- `q3r_ctf4_03` (Industrial Crossfire): Abstellgleis mit drei gedeckten Wagen und einem
  Kesselwagen auf jedem Kai, kleiner Rangierbahnhof mit einem gedeckten Wagen und zwei
  Flachwagen mit Containern.

`shaderlist.txt` muss `q3r_rail` enthalten.
