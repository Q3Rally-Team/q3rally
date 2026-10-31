# Ridge Pass

Rundkurs (Racing, Team Racing, Racing DM, Team Racing DM, Elimination) über einen
verschneiten Bergpass. Nachfolger von Ridge Racer (`q3r_ridgeracer`), aber eine
neue Strecke. Erzeugt von `tools/maptools/make_ridgepass.py` (Python 3 + Pillow).

## Strecke

- Liegende Acht, rund 27.000 Units pro Runde, grob geschätzt 48 Sekunden
  (im Spiel noch zu messen). 3 Runden, 12 Startplätze in versetzter Aufstellung.
- **Tal:** Start/Ziel-Gerade. Außen eine überdachte Tribüne mit Fangzaun, zwei
  Flutlichtmasten (Modell, `docs/lanterns.md`) und Plakatwänden; innen das Boxengebäude mit acht Garagen, verglastem Obergeschoss,
  Balkon, Sponsorenband und einem Race-Control-Turm. Startportal mit Q3Rally-Banner,
  Startampel und Zielflaggen-Pfosten, aufgemalte Start- und Startplatzlinien.
- **Aufstieg:** durch die Unterführung unter der Brücke, dann drei Serpentinen
  (Kehrenradius etwa 300 Units auf der Mittellinie) den Hang hinauf.
- **Grat:** schnelle Wechselkurven mit Kuppe, Schneewällen am Rand und Blick in beide
  Täler.
- **Abfahrt:** lange Rechtskurve, Lawinengalerie (Betondach, Talseite mit Pfeilern
  offen, Lampen), dann über die Brücke zurück ins Tal.
- Fahrbahn 430–560 Units breit, an den Kehren breiter. Höchste Steigung etwa 13 %.
- Ränder: Matschstreifen neben dem Asphalt, dahinter geräumte Schneewälle. In
  Einschnitten Fels, auf Dämmen Schneeböschungen. Unsichtbare Grenze weit
  außerhalb, damit niemand ins Gebirge fährt.

## Checkpoints

17 `rally_checkpoint` plus `rally_startfinish` (laps 3). Vor jeder Kurve liegt ein
Checkpoint, etwa 350 Units vor dem Kurveneingang. Lange Abschnitte ohne Kurve
bekommen zusätzliche Checkpoints, damit kein Abstand über etwa 2.600 Units liegt.
Jeder Checkpoint zielt auf ein `info_notnull` als Rücksetzpunkt (in Fahrtrichtung).

Die Trigger sind schmal (Fahrbahn plus Rand) und reichen auf den Serpentinen nicht in
die Nachbarstrecke. In der Nähe der Kreuzung liegt kein Checkpoint, weil ein Trigger
dort die andere Ebene erreichen würde. Die Liste mit Position und Kurvenradius steht
in `validation.json`.

## Pickups

Für Racing DM: Raketenwerfer (2×), Schrotflinte, Plasma, Granatwerfer,
Flammenwerfer, Heckwaffen (Mine, Öl, Rauch, Bio), passende Munition, gelbe und grüne
Panzerung, Gesundheit, NOS (3×), Treibstoff. In normalen Rennen entfernt das Spiel
Waffen und Munition selbst.

## Bauen

```text
python tools/maptools/make_ridgepass.py
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -meta -custinfoparms -keeplights <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -vis -fast -saveprt <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -light -custinfoparms -fast -shade -filter -super 2 -bounce 1 <map>
```

Der Generator schreibt:
- die Map und die Indexmap `maps/q3r_ridgepass_index.pcx`
- `scripts/q3r_ridgepass.shader` und `scripts/q3r_ridgepass.arena`
- `textures/q3r_ridgepass/macro.jpg`
- die Intro-Kamerafahrt `intro_routes/q3r_ridgepass_tl1_rev0.route`
- `docs/maps/ridgepass/ridgepass_layout.png` und `validation.json`

`shaderlist.txt` muss `q3r_ridgepass` und `q3r_lanterns` enthalten. Die Wegweiser sind
die blauen und roten Schilder aus Ridge Racer (`textures/ridgeracer/turns_01` und
`turns_02`); für Linkskurven wird das Bild gespiegelt.

Die AAS wird wie bei Canyon Rush aus einer Ersatzkarte gebaut, siehe
`docs/maps/aas_proxy.md`. Beim Tausch der Flutlichtmasten (Oktober 2026) blieb die
Kollision an derselben Stelle; die bestehende AAS bekam nur die neue Prüfsumme
(`make_aas_proxy.py --checksum`).

Offen: Bot-Route (`.bpd`), Rundenzeit und Fahrgefühl im Spiel prüfen, Balancing der
Pickups.
