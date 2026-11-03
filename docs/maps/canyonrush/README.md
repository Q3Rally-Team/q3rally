# Canyon Rush

A2B-Sprint (`q3r_sprint`, auch Ghost Race) durch einen roten Wuestencanyon.
Erzeugt von `tools/maptools/make_canyonrush.py` (Python 3 + Pillow).

## Strecke

- Eine Mittellinie von Start (A) nach Ziel (B), rund 34.000 Units lang. Grob
  geschaetzte Fahrzeit 60-70 Sekunden, im Spiel noch zu messen.
- Abfahrt vom Startportal in den Canyon, enge Schlucht mit Felsbogen, offener
  Talkessel mit Felsnadeln und trockenem Kiesflussbett, Kuppen, ueberhoehte
  Kurven, Ausfahrt bergauf zum Zielportal. Sechs Checkpoints (`rally_checkpoint`
  1-6 mit `info_notnull`-Respawnpunkten), `rally_start`, `rally_finish`,
  vier `info_player_start` und ein `info_player_deathmatch`.
- Die Canyonwaende sind auf jedem Querschnitt steil genug, dass man nicht
  hinausfahren oder abkuerzen kann; der Generator prueft das.

## Aufbau

- Hochebene als Hoehenfeld, entlang der Strecke ausgefraest: ebene, leicht
  ueberhoehte Fahrbahn, steile Sandsteinwaende bis zur Kante. Liegen zwei
  Abschnitte nebeneinander, bleibt der Grat dazwischen stehen.
- Kollision: geschlossene Dreiecksprismen, 64 Units an Fahrbahn und Wandfuss,
  128 an den Waenden, 256 auf der Hochebene (rund 37.700 Brushes).
- Boden: Indexmap `maps/q3r_canyonrush_index.pcx` mit fuenf Ebenen (Sand,
  Fahrspur, Kies, Fels, Sandstein-Schichtung), Reibung ueber die Eltern-Shader
  `t_sand`, `t_dirt`, `t_gravel`, `t_plain`. Die Schichtung (`strata.jpg`) wird
  an den Waenden senkrecht projiziert.
- Himmel `env/ame_desert` mit Sonne, Himmelslicht und warmem Dunst.

## Bauen

```text
python tools/maptools/make_canyonrush.py
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -meta -custinfoparms <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -vis -fast -saveprt <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -light -custinfoparms -fast -shade -filter -super 2 -bounce 1 <map>
```

Der Generator schreibt Map, Indexmap, `scripts/q3r_canyonrush.shader`,
`.arena`, `textures/q3r_canyonrush/strata.jpg` und `macro.jpg` sowie
`docs/maps/canyonrush/canyonrush_layout.png` und `validation.json`.
Die Bodenshader und Texturen nutzt auch `q3r_mesaking`.

Bots: `maps/q3r_canyonrush.aas` wird aus einer vereinfachten Ersatzkarte gebaut, siehe
`docs/maps/aas_proxy.md`. Der Bot faehrt rund 90 Prozent der Strecke und kehrt nach
Checkpoint 6 um.

Offen: Fahrzeit und Fahrgefuehl im Spiel pruefen, Bot-Fahrt vor Checkpoint 6.
