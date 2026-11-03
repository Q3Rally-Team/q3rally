# Mesa King

King of the Hill (`q3r_koth`) in einer roten Felsarena.
Erzeugt von `tools/maptools/make_mesaking.py` (Python 3 + Pillow).

- Runde Sandarena (Radius 2150 Units) in einem Ring steiler Sandsteinwaende.
- In der Mitte ein Tafelberg mit flacher Kuppe (Radius 400, die Zone ist ein
  `trigger_koth_hill` ueber der Kuppe), erreichbar ueber vier Rampen auf den
  Diagonalen (14 Grad); sonst sind die Flanken Steilwaende. Ueber der Kuppe
  schwebt die Krone, am Rand der Zone stehen Leuchtpfosten; beides zeichnet das
  Spiel selbst (siehe `docs/koth_crown.md`), die Map braucht dafuer nichts.
- Rot startet im Osten, Blau im Westen, je vier `team_KOTH_*player` und vier
  `team_KOTH_*spawn`. Die Map ist an beiden Achsen gespiegelt, alle Rampen sind
  fuer beide Teams gleich weit.
- Deckungsfelsen, Fahrspuren von den Startzonen zu den Rampen, Pickups paarweise
  gespiegelt (Raketenwerfer, Munition, Schrotflinte, Panzerung, Gesundheit,
  Treibstoff, NOS).
- Man kann bis auf den oberen Rand des Kessels fahren; dort haelt ein unsichtbarer
  Ring die Autos. Dahinter steigen Duenen bis zum Horizont an, es gibt keine harte
  Kante zur Skybox.
- Nutzt Bodenshader und Texturen von `q3r_canyonrush`; zuerst
  `make_canyonrush.py` laufen lassen. Kompilieren wie Canyon Rush.

Bots: Die AAS laesst sich direkt aus der Map bauen. Schneller geht es mit der
Ersatzkarte, siehe `docs/maps/aas_proxy.md`.

Offen: Balancing im Spiel. In den Tests fahren die Bots nicht auf den Huegel.
