# King of the Hill: Krone und Leuchtpfosten

Seit Oktober 2026 zeichnet cgame den Hügel in King of the Hill mit zwei Modellen statt
mit Bodenring, Lichtsäule und Sprite.

## Was man sieht

- **Krone:** Über der Mitte der Zone schwebt eine goldene Krone
  (`models/mapobjects/koth/crown.md3`). Sie dreht sich langsam, wippt und neigt sich
  leicht. Edelsteine und Samtkappe leuchten in der Farbe des Besitzers (Rot, Blau,
  hellgrau wenn der Hügel niemandem gehört). Ein weiches Leuchten hinter der Krone und
  ein dynamisches Licht färben den Hügel mit ein.
- **Eroberung:** Solange ein Team den Hügel einnimmt, pulsieren die Steine zur Farbe
  dieses Teams hin, schneller je weiter die Eroberung ist.
- **Umkämpft:** Beide Teams im Hügel: die Krone ruckelt, die Steine flackern gelb.
- **Übernahme:** Wechselt der Besitzer, springt die Krone hoch, dreht sich zweimal und
  wird kurz größer.
- **Leuchtpfosten:** Auf dem Rand der Zone stehen Poller (`post.md3`), etwa alle 110
  Units, mindestens 12 und höchstens 32. Während der Eroberung gehen sie nacheinander
  in der Farbe des erobernden Teams an; der Ring zeigt so den Fortschritt. Gehört der
  Hügel einem Team, läuft eine langsame Lichtwelle in dessen Farbe um. Umkämpft:
  abwechselnd gelb und Besitzerfarbe.
- **Absenken:** Kommt ein Auto näher als 100 Units, versinkt der Poller im Boden.
  Autos fahren also nie sichtbar durch einen Pfosten.
- **Minimap:** Krone (3,5-fach) und Pfosten (doppelt so groß) werden auch dort gezeichnet.

Die Zone ist ein Quader (`trigger_koth_hill`). Die Pfosten stehen auf seinem Rand und
werden per Trace auf den Boden gesetzt. Pfosten, deren Boden mehr als 72 Units von der
Hügelmitte abweicht (der Rand läuft z. B. über eine Klippe), werden weggelassen.

## Daten vom Server

`CS_KOTHSTATUS` hat drei zusätzliche Felder am Ende:

```text
<owner> <contested> <capture_pct> <x> <y> <z> <radius> <capturer> <half_x> <half_y>
```

`capturer` ist das Team, das gerade erobert (0 = keins), `half_x`/`half_y` sind die
halben Ausmaße der Zone. Ältere Leser (Bot-Code, alte Clients) lesen nur die ersten
sieben Felder und funktionieren weiter. Fehlen die neuen Felder, nimmt der Client den
Radius für beide Seiten.

## Dateien

- Code: `engine/code/cgame/cg_hud_vehicle.c` (`CG_AddKOTHHillIndicatorToScene`),
  Status in `cg_servercmds.c`, Server in `game/g_team.c` (`KOTH_SetHillStatus`).
- Modelle und Texturen: `baseq3r/models/mapobjects/koth/`, `baseq3r/gfx/misc/koth_glow.tga`,
  erzeugt von `tools/koth/make_koth_models.py` (Python 3 + numpy).
- Shader: `baseq3r/scripts/q3r_koth.shader`. Edelsteine, Samt und Lampen nutzen
  `rgbGen entity`; die Farbe setzt cgame pro Modell.

Entfernt wurden das alte System und seine Cvars (`cg_kothRingScale`,
`cg_kothBeamHalfWidth`, `cg_kothRingAlphaBase`, `cg_kothRingAlphaPulse`,
`cg_kothBeamAlphaBase`, `cg_kothBeamAlphaPulse`) sowie `scripts/koth_beam.shader`,
`textures/fx/koth_beam.png`, `textures/fx/koth_ring.png` und `gfx/2d/hillmarker*.png`.

Maps brauchen nichts weiter als den `trigger_koth_hill`. Der Schlüssel
`indicatorHeight` hat für die Krone keine Bedeutung mehr (sie schwebt in fester Höhe
über dem Boden der Hügelmitte).
