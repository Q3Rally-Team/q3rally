# AAS fuer die Terrain-Maps

Crossroads, Canyon Rush und Mesa King bestehen aus zehntausenden Dreiecksprismen,
jedes Dreieck mit eigener Ebene. bspc braucht dafuer Stunden (Canyon Rush war nach
50 Minuten noch im Brush-BSP) oder ueberschreitet `AAS_MAX_REACHABILITYSIZE`
(65536 in einem normalen bspc). Bots brauchen aber nur die befahrbare Flaeche.
Deshalb wird die AAS aus einer vereinfachten Ersatzkarte gebaut.

`tools/maptools/make_aas_proxy.py` liest die erzeugte `.map`, uebernimmt Entities,
Gebaeude, Felsen, Baumstaemme, Trigger und die unsichtbare Grenze unveraendert und
vernetzt nur das Gelaende neu:

- Befahrbar ist, was ein Auto von Spawns und Pickups aus erreicht: Schritte nicht
  steiler als etwa 31 Grad, nicht durch Brushes mit `boundary` im Shadernamen.
- Dort liegt die Ersatzflaeche hoechstens 8 Units ueber und 24 Units unter dem
  echten Boden (an den kleinsten Zellen von 64 Units etwas mehr).
- Waende bis 256 Units neben der Fahrflaeche bleiben steiler als 45 Grad, damit
  bspc sie nicht als begehbar einstuft und Bots nicht darueber routen.
- Alles andere (Huegel hinter der Grenze, Hochebene ueber dem Canyon) wird grob
  vernetzt, bis 1024 Units pro Zelle. Deko-Brushes weit ausserhalb fallen weg.

Die AAS dieser Ersatzkarte bekommt danach die Pruefsumme der echten BSP und
heisst wie die echte Map. Ohne diesen Schritt meldet das Spiel
"aas file ... is out of date".

| Map | Gelaende-dreiecke | Ersatz-dreiecke | AAS-Areas | Reachability | bspc (Cloud) |
|---|---|---|---|---|---|
| q3r_crossroads | 44484 | 1664 | 9898 | 27884 | 31 s |
| q3r_canyonrush | 37680 | 7992 | 19142 | 47466 | 7 min |
| q3r_mesaking | 12920 | 3272 | 7458 | 20468 | 39 s |
| q3r_david4 | 11284 | 2236 | 10617 | 30854 | 27 s |

## Ablauf (Beispiel Canyon Rush)

```text
python tools/maptools/make_aas_proxy.py baseq3r/maps/q3r_canyonrush.map
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -bsp -custinfoparms baseq3r/maps/q3r_canyonrush_aasproxy.map
bspc -forcesidesvisible -optimize -bsp2aas baseq3r/maps/q3r_canyonrush_aasproxy.bsp
python tools/maptools/make_aas_proxy.py --checksum baseq3r/maps/q3r_canyonrush_aasproxy.aas baseq3r/maps/q3r_canyonrush.bsp
```

Der letzte Schritt schreibt `baseq3r/maps/q3r_canyonrush.aas`. Fuer den Ersatz-BSP
genuegt `-bsp`, ohne `-meta`, Vis und Licht. Die `*_aasproxy.*`-Dateien sind
Zwischenstaende (per `.gitignore` ausgeschlossen) und gehoeren nicht ins Spiel.
`--above` und `--below` aendern die Toleranzen.

**Nach jedem Neukompilieren der echten BSP** aendert sich deren Pruefsumme. Dann
mindestens den letzten Schritt wiederholen; hat sich das Gelaende geaendert, die
ganze Kette.

## Pruefung

In der Cloud geladen ("loaded maps/....aas") und Bots verfolgt:

- Crossroads (CTF4): Bots fahren zwischen den Basen hin und her, einer bleibt als
  Verteidiger an der eigenen Flagge stehen.
- Mesa King (KOTH): Bots fahren durch die ganze Arena zu den Pickups. Auf den
  Huegel fahren sie in den Tests nicht; das ist die KOTH-Zielwahl der Bot-KI, nicht
  die AAS.
- Canyon Rush (Sprint): ein Bot faehrt rund 90 Prozent der Strecke und kehrt nach
  Checkpoint 6 um. In der Kurve vor Checkpoint 6 kommt er auf die Aussenboeschung.
  Eine aufgezeichnete Ghost-Runde (`ghosts/`) duerfte den Bots helfen.
