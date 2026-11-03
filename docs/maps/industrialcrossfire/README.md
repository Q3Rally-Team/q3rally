# Industrial Crossfire

Vier-Team-CTF (`q3r_ctf4`, Datei `q3r_ctf4_03`) auf einer quadratischen Hafenmole in der
Abenddämmerung. Erzeugt von `tools/maptools/make_industrialcrossfire.py`
(Python 3 + Pillow). Die frühere Testversion der Map war nur ein Texturkasten ohne
Quelldatei; sie wurde ersetzt.

## Aufbau

- Mole 5600 × 5600 Units, ringsum Hafenbecken, festgemachte Schiffe und eine
  Industriesilhouette am anderen Ufer. Eine unsichtbare Wand an den Kaikanten hält die
  Autos auf der Mole.
- **Fairness:** Das Layout ist um 90 Grad drehsymmetrisch. Jede Basis hat dieselben
  Wege, dieselbe Deckung und dieselben Pickups; nur das Aussehen unterscheidet sich.
  Flagge zu Nachbarflagge 4500 Units, zur gegenüberliegenden 6364.
- **Basen** in den Ecken zwischen zwei Kais: offener Hof, L-förmige Halle um die
  Flaggennische, Vordach in Teamfarbe, zwei Flutlichtmasten mit Leiter, Plattform und sechs
  Strahlern auf die Flagge gerichtet (Modell, `docs/lanterns.md`). Wahrzeichen auf dem Hallendach:
  - Blau (Nordost): Containerterminal, Containerturm mit Mast.
  - Rot (Nordwest): Stahlwerk, zwei Schornsteine.
  - Grün (Südwest): Bahnbetriebswerk, Wasserturm.
  - Gelb (Südost): Schüttgutterminal, zwei Silos mit Förderband.
- **Randstraßen** entlang der Kais verbinden die Nachbarbasen. Auf den Kais ein
  Portalkran über der Straße mit Containerstapeln, auf der anderen Kaihälfte ein
  Abstellgleis mit Güterwagen. Gleise, gedeckte Wagen, Kessel- und Flachwagen sind
  Modelle (`docs/rail.md`) mit unsichtbarer Kollision, die Schienen liegen auf einem
  Schotterbett.
- **Zwischen Rand und Mitte:** Lagerhalle mit Rampe aufs Dach (Railgun oben, Sprung auf
  die Randstraße), Tanklager mit Rohrbrücke, Container-Labyrinth, kleiner Rangierbahnhof.
- **Das Kreuz in der Mitte („Crossfire“):** Von jeder Randstraße führt eine Rampe auf
  eine Hochstraße (224 Units hoch). Die vier Arme treffen sich auf einer Plattform über
  dem zentralen Platz. Oben liegt Quad Damage, unten rote Panzerung. Diagonalstraßen
  verbinden jede Basis mit dem Platz.
- Licht: tiefe Sonne aus Osten (Skybox `env/ely_sunset`), Natriumlampen an den Straßen,
  Flutlicht an den Basen. Regenwetter mit 45 % Wahrscheinlichkeit.

## Pickups (je Viertel gleich)

Raketenwerfer und Munition, Gesundheit, Treibstoff und NOS in jeder Basis; Railgun auf
dem Hallendach, Schrotflinte im Tanklager, Granatwerfer im Labyrinth, Plasma im
Rangierbahnhof, gelbe Panzerung auf jedem Arm, Heckwaffen (Öl, Mine, Rauch, Bio) an den
Straßen. Mitte: Quad Damage oben, rote Panzerung unten.

## Bauen

```text
python tools/maptools/make_industrialcrossfire.py
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -meta -custinfoparms -keeplights <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -vis -saveprt <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -light -custinfoparms -fast -shade -filter -super 2 -bounce 1 <map>
bspc -forcesidesvisible -optimize -bsp2aas <bsp>
```

Brushes und Modelle, keine Gelände-Prismen: die AAS baut bspc direkt in wenigen Sekunden
(die Modelle zählen dafür nicht, nur ihre unsichtbaren Kollisions-Brushes).
`shaderlist.txt` muss `q3r_ctf4_03`, `q3r_lanterns` und `q3r_rail` enthalten.

Offen: Spieltest mit vier Teams, Balancing der Pickups. Bots fahren, bleiben aber
gelegentlich am Fuß einer Hochstraßen-Rampe hängen (90-Grad-Einfahrt von der Randstraße).
