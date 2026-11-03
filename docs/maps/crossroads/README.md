# Crossroads V0.2

Offene Terrain-Map fuer Q3Rally Vier-Team-CTF, ausgelegt fuer 4-8 Spieler.
Rekonstruiert nach dem Konzept-PDF. Die dort erwaehnte originale Heightmap wurde
nicht mitgeliefert; das Hoehenmodell ist eine eigene Umsetzung der Skizze und der
Koordinaten, keine pixelgenaue Kopie der Vorlage.

## Installation und Start

Die Karte gehoert zu den Spieldaten unter `baseq3r`: `maps/q3r_crossroads.bsp`,
`scripts/q3r_crossroads.shader` und `.arena`, `textures/q3r_crossroads/macro.jpg`
und `levelshots/q3r_crossroads.jpg` (die `.map` und die Indexmap braucht nur der
Compiler). Fuer eine Einzelverteilung baut `package_crossroads.py` zusaetzlich eine
PK3 nach `dist/`. Die aktuelle Q3Rally-Spielversion mit CTF4 wird benoetigt. In der Konsole:

```text
/g_gametype 20
/map q3r_crossroads
```

Die Karte braucht `com_hunkMegs 256` (Standard ab dieser Engine-Version; aeltere
Konfigurationen mit `seta com_hunkMegs "128"` einmal anpassen und das Spiel neu
starten), sonst bricht das Laden mit `Hunk_AllocateTempMemory` ab.

Die Map erscheint auch in der Kartenauswahl fuer Vier-Team-CTF. Im Repository
liegen BSP und Quelldateien bereits unter `baseq3r`.

## Neu in V0.2

- **Groesser:** 5376 x 5376 Units Spielflaeche (V0.1: 4096). Basen bei (+-1664, +-1664),
  flache Hoefe von 640 x 640 Units auf Z=176. Rot (-1664,1664), Blau (1664,1664),
  Gruen (-1664,-1664), Gelb (1664,-1664).
- **Weiche Bodenuebergaenge:** Wiese, trockenes Gras, Feldweg, Kies, Bachkiesel und
  Beton gehen per Vertex-Alpha ineinander ueber (q3map2-Indexmap
  `maps/q3r_crossroads_index.pcx`, sechs Ebenen). Eine grossflaechige Helligkeitstextur
  bricht die Kachelwiederholung. Weich schattierte Normalen (`q3map_shadeAngle`).
- **Fahrbarkeit:** Kollisionsraster 32 statt 128 Units. Die Hoehen werden auf einem
  16-Unit-Raster geglaettet, jede Zelle bekommt die passendere Diagonale, auf den
  Diagonalstrassen laufen die Dreiecke entlang der Fahrtrichtung, und die Wege sind
  quer zur Fahrtrichtung eingeebnet. Gemessen entlang der Strassenmitten sinkt die
  groesste Neigungsaenderung zwischen zwei Dreiecken um den Faktor 4 bis 5
  (Basis -> Mitte 0.214 -> 0.046, Seitenstrasse 0.099 -> 0.019).
- **Bach:** echtes Bachbett mit Ufern, Kieselboden und klarem, spiegelndem Wasser
  (OpenGL2 `q3r_water`, OpenGL1 einfache Wasserstufen). Der Bach schlaengelt sich
  leicht, ist unterschiedlich breit, hat zwei Kiesinseln (je eine Schrotflinte) und
  drei breite Kiesfurten: Mitte und beide Seitenstrassen, Wassertiefe dort etwa 7-8 Units.
  Die tiefen Abschnitte (bis 22 Units) bremsen durch den normalen Wasserwiderstand.
  Das Bachtal laeuft im Osten und Westen in die Huegel hinaus.
- Findlinge an den Ufern, in der Kiesgrube und an den Wegen, Kieshaufen in der Grube,
  mehr Baeume im Spielfeld, dichter Wald mit Lichtungen auf den umgebenden Huegeln
  und leichter Sommerdunst (OpenGL2 `q3r_haze`).
- **Eigene Basen:** gleiche Kollision fuer alle Teams (Pfeiler, Nebengebaeude,
  Poller, ein 128 x 128 Sockel hinter dem Dach), Unterschiede nur ueber Fahrzeughoehe
  oder innerhalb dieser Grundflaechen:
  Rot Holzfaellercamp (Ziegeldaecher, Scheunentor, Holzkran),
  Blau Tankstelle (flaches Dach mit Leuchtfeldern, Schaufenster, Preismast),
  Gruen Forststation (steile gruene Daecher, Blockhuette, Feuerwachturm),
  Gelb Kieswerk (gelbe Pultdaecher, Wellblechhalle, Rolltor, Silo mit Foerderband).
- **Gemischtes Raster:** 64-Unit-Zellen in flachen, einheitlichen Wiesen, 32 Units an
  Wegen, Bach, Kieshaufen und Bodenuebergaengen. 44925 statt 63559 Brushes.
- Die farbigen Poller am Flaggenhof stehen nicht mehr auf der Achse der Ost-West-Strasse,
  sondern rahmen die Flagge quer zur Diagonalstrasse. Alle drei Zufahrten sind frei.
- Lightgrid 64 x 64 x 128 nur ueber der Spielflaeche.

Reibung bleibt exakt: Die Kollisionsseiten der Gelaendedreiecke tragen je nach
ueberwiegendem Boden einen der Eltern-Shader `t_grass`, `t_dirt`, `t_gravel` oder
`t_plain` mit dem passenden Oberflaechenflag. Gezeichnet werden die von q3map2
gewaehlten Mischshader `ter_<ebene>` bzw. `ter_<a>to<b>`.

## Symmetrie und Fairness

Relief, Strassen, Basen und Deko sind wie in V0.1 an beiden Achsen gespiegelt. Der
geschwungene Bach ist punktsymmetrisch zur Mitte (Rot <-> Gelb, Blau <-> Gruen). Er
kreuzt alle drei Strassen genau auf der Achse, deshalb sind Strassenlaengen, Furten
und Steigungen fuer alle vier Teams gleich. Nur das freie Gelaende neben dem Bach
unterscheidet sich zwischen Nord und Sued leicht.

## Bearbeiten und neu bauen

`baseq3r/maps/q3r_crossroads.map` laesst sich in NetRadiant Custom oeffnen. **Der
Generator ueberschreibt diese Datei.** Fuer reproduzierbare Aenderungen
`tools/maptools/make_crossroads.py` bearbeiten. Er schreibt Map, Indexmap,
Uebersicht und Heightmap. `tools/maptools/crossroads_textures.py` schreibt die
Bodenshader am Ende von `scripts/q3r_crossroads.shader` und die Makrotextur.

Beide Skripte brauchen Python 3 und Pillow. Die Kollision besteht aus 37764
geschlossenen Dreiecksprismen (32/64-Unit-Raster) und 6720 groeberen
Hintergrunddreiecken. Verdeckte Seiten verwenden `common/caulk`, da `common/nodraw`
in diesem Repository **nonsolid** ist.

Kompletter Neubau:

```powershell
./tools/maptools/build_crossroads.ps1 -Python <python.exe> -Q3Map2 <q3map2.exe>
```

Die Compiler-Aufrufe (Pfade entsprechend einsetzen):

```text
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -meta -custinfoparms <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -vis -fast -saveprt <map>
q3map2 -game quake3 -fs_basegame baseq3r -fs_basepath <repo> -light -custinfoparms -fast -filter -super 2 -bounce 1 <map>
```

Alles ausser der Huelle ist Detail, daher genuegt `-vis -fast`. `quake3` plus
`custinfoparms` ist beabsichtigt: Das eingebaute `q3rally`-Profil aelterer Compiler
hat veraltete Materialbits. Die Shaderliste muss `q3r_crossroads` enthalten.

`crossroads_heightmap_16bit.png` ist eine 513 x 513 grosse 16-Bit-Heightmap der
triangulierten Oberflaeche (Schwarz Z=0, Weiss Z=512, oben ist Norden).
`crossroads_layout.png` ist eine aus der Geometrie abgeleitete Uebersicht.
`crossroads_ingame.jpg` und die Menue-Vorschau sind echte Aufnahmen aus dem
Q3Rally-Client (OpenGL2, fertige Lichtdaten). `server_validation.json` stammt
noch vom Pakettest der V0.1.

## Pruefstand und offene Punkte

Der Generator prueft Symmetrie, ebene Hoefe, Wasser nur im Bachbett, Furttiefen,
Startpunkte, Pickups und Deko abseits der Strassen und die maximale Neigung
(knapp 26 Grad). `validate_crossroads.py` prueft danach die kompilierte
BSP-Kollision (Boden, Grenze, Starts, Pickups).

Fahrtest mit Vollgas geradeaus (Standardauto, `fixedtime`, ohne Lenken, also
untere Grenze der echten Fahrzeiten):

| Strecke | V0.1 | V0.2 |
|---|---|---|
| Basis -> Mitte (Diagonalstrasse, durch die Furt) | 5.9 s | 6.7 s |
| Basis -> Nachbarbasis ueber die Seitenfurt | 6.7 s | 7.9 s |
| Basis -> Nachbarbasis Ost-West | 8.7 s (Poller getroffen) | 7.4 s |

Hoechstgeschwindigkeit auf den Strassen 416-453 (V0.1) bzw. 539-572 (V0.2). Das
Konzept nennt 8-12, 12-18 und 25-40 Sekunden; mit Kurven, Gegnern und Flagge
liegen echte Laeufe darueber, das muss sich im Spiel zeigen.

Bots: `maps/q3r_crossroads.aas` wird aus einer vereinfachten Ersatzkarte gebaut, siehe
`docs/maps/aas_proxy.md`.

Offen: Spiel mit vier menschlichen Teams und Balancing.

Die PK3 enthaelt nur die kartenspezifischen Dateien (Karte, Quelle, Indexmap,
Shader, Makrotextur, Vorschau). Alle Standardtexturen, Baummodelle, Flaggen und
Fahrzeuge kommen aus Q3Rally. Die Quelldaten und
Materialien des Spiels unterliegen den im Haupt-README genannten Datenlizenzen.
