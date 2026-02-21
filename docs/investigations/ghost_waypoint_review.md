# Review: Waypoint-System aus `.ghost` für Racing-Bots

## Kurzfazit
Ja, hier steckt deutlich Potenzial. Der aktuelle Ansatz funktioniert bereits als robuste Basis (Datei laden, Waypoints extrahieren, Bot daran ausrichten), aber in der Linienwahl und Interpolation gibt es mehrere Hebel für spürbar bessere Bot-Performance.

## Was heute gut ist
- Der Server lädt Ghost-Dateien robust (Header/Frames, Dateigrößenlimit, Fallbacks).
- Es wird eine Bot-Route aus Waypoints aufgebaut und die schnellste passende Ghost-Datei als Quelle gewählt.
- Bots nutzen diese Route bereits aktiv zur Zielrichtung + grober Sollgeschwindigkeit.

## Konkretes Verbesserungspotenzial

### 1) Ambiguität bei Streckenkreuzungen
Aktuell wird pro Tick der geometrisch nächste Waypoint gesucht. Bei Track-Overlaps/Kreuzungen kann das auf den "falschen" Streckenabschnitt springen.

**Vorschlag:**
- Fortschrittsindex pro Bot cachen (nur vorwärts, mit kleinem Suchfenster).
- Optional Blend aus Checkpoint-Progress + Ghost-Progress verwenden.

### 2) Lookahead als fester Index-Offset
`bestIndex + 12` ist strecken- und sampling-abhängig. Bei dichter oder dünner Abtastung führt das zu zu kurzem/langem Vorausschauen.

**Vorschlag:**
- Lookahead über Zeit (z. B. +400..900 ms) oder Distanz (z. B. +20..50 m) statt über festen Index.
- Alternativ adaptive Skalierung basierend auf aktueller Geschwindigkeit.

### 3) Interpolation nur implizit, nicht entlang Segment
Die Zielführung nimmt aktuell einen zukünftigen Waypoint, aber keine explizite Projektion auf das aktuelle Segment. Das erzeugt unnötiges Pendeln.

**Vorschlag:**
- Position auf das Segment `(i -> i+1)` projizieren (`t`), daraus tangentielle Richtung ableiten.
- Optional Catmull-Rom/Spline-Smoothing auf den Waypoints für weicheres Lenken.

### 4) Sollgeschwindigkeit aus einem einzigen Segment
Die Sollgeschwindigkeit wird nur aus `bestIndex` und `bestIndex+1` geschätzt. Das ist anfällig für Ausreißer (Timing-Jitter in Ghost-Frames).

**Vorschlag:**
- Geschwindigkeit über Fenster mitteln (z. B. 5-15 Segmente).
- Kurvenvorschau nutzen: bei hoher Krümmung frühzeitig drosseln.

### 5) Keine Qualitätsmetrik der Ghost-Route
Jede ausreichend parsebare Route ist nutzbar, auch wenn sie verrauscht ist.

**Vorschlag:**
- Beim Laden Plausibilitätschecks: monotone Timestamps, Mindest-/Maximalabstände, Ausreißerfilter.
- Optionale Vorverarbeitung (resampling auf konstantes Zeitraster).

## Erwarteter Impact (priorisiert)
1. **Fortschrittsindex + Suchfenster** → größte Stabilitätsverbesserung sofort.
2. **Zeit-/Distanz-Lookahead** → bessere Linienwahl bei unterschiedlichen Strecken.
3. **Segmentprojektion + geglättete Sollgeschwindigkeit** → weniger Lenkzacken, natürlichere Bot-Bewegung.

## Minimal-invasive Umsetzungsstrategie
1. Pro Bot `ghostRouteIndexHint` einführen.
2. Nearest-Search nur in lokalem Fenster um den Hint (z. B. `[-8, +24]`).
3. Lookahead per Zeitoffset statt fixer Anzahl Waypoints.
4. Speed-Smoothing über mehrere Segmente.

Damit bleibt das System kompatibel zum bestehenden `.ghost`-Format, gewinnt aber deutlich an Robustheit und Pace-Konsistenz.
