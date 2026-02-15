# Atmospheric / Wetter – Mapper Checklist (DE/EN)

## Deutsch

### 1) Wetterzonen als Entities setzen
- Nutze `rally_weather_rain` für Regenzonen und `rally_weather_snow` für Schneezonen in der Map.
- Diese Entities werden serverseitig als `ET_WEATHER` angelegt.
- Die Brush-Geometrie der Entity ist relevant, weil clientseitig geprüft wird, ob Partikelpositionen innerhalb dieser Zone liegen.

### 2) Typ und Basisparameter verstehen
- `rally_weather_rain` setzt `weapon = 0`, `rally_weather_snow` setzt `weapon = 1`; darüber erkennt der Client den Wettertyp.
- Die Basis-Partikelanzahl pro Typ wird in den Client-State geschrieben und beim Setup übernommen.

### 3) Spawnflags (Regen) bewusst einsetzen
- Bei Regen wird `spawnflags` übertragen und clientseitig als Schalter für Splash-Verhalten ausgewertet.
- Effekt: `waterSplash`/`landSplash` werden abhängig davon aktiviert/deaktiviert.
- Bei Schnee sind Splashes standardmäßig deaktiviert.

### 4) Automatisches Qualitäts-/Budgetsystem berücksichtigen
- Das System ist automatisch adaptiv: Profilwahl über `cg_atmosphericLevel` (Low/Medium/High).
- Pro Frame wird das Budget aus Profil + FPS + FOV + Basisdrops berechnet.
- Die sichtbare Partikelmenge kann sich daher je nach Client/HW unterscheiden (gewolltes Performance-Verhalten).

### 5) Sichtkegel-/Distanz-Culling beachten
- Partikel außerhalb Distanz oder Sichtkegel werden deaktiviert/neu verteilt.
- Sehr große oder ungünstig platzierte Wettervolumes können dadurch „dünner“ wirken.
- Empfehlung: Wetterzonen entlang typischer Fahr-/Sichtlinien platzieren statt globale Monster-Volumes.

### 6) Konsistenz durch deterministische Seeds
- Spawn-Variation (Position/Farbe/Flake-Auswahl) wird deterministisch pro Zone + Partikel erzeugt.
- Ergebnis: konsistenteres Erscheinungsbild statt vollständig zufälliger Verteilung pro Frame.

### 7) QA / Abnahme
- Relevante Client-Cvar: `cg_atmosphericLevel` (Default: `2`).
- Beim Testen mindestens Low/Medium/High prüfen.

---

## English

### 1) Place weather entities correctly
- Use `rally_weather_rain` for rain volumes and `rally_weather_snow` for snow volumes.
- These entities are created as `ET_WEATHER` server-side.
- Entity brush bounds matter because client-side particle checks test whether points are inside the weather zone.

### 2) Understand type wiring and base inputs
- `rally_weather_rain` uses `weapon = 0`, `rally_weather_snow` uses `weapon = 1`; client code uses this as weather type.
- Baseline particle count is carried in entity state and consumed during atmospheric setup.

### 3) Use rain spawnflags intentionally
- For rain, `spawnflags` are passed and interpreted client-side as splash toggle input.
- This controls whether water/land splash effects are enabled.
- Snow splash effects are disabled by design.

### 4) Expect automatic adaptive behavior
- The system is automatically adaptive: profile selection via `cg_atmosphericLevel` (Low/Medium/High).
- Per-frame budget is computed from profile + FPS + FOV + base drops.
- Visible density may differ by client performance/settings (intentional for performance).

### 5) Account for cone/distance culling
- Particles outside distance/view cone are culled or repositioned.
- Very large or poorly placed weather volumes may look thinner in practice.
- Recommendation: place weather zones around expected racing/sight corridors rather than one giant global volume.

### 6) Deterministic consistency
- Spawn variation (position/color/flake choice) is deterministic per zone + particle.
- Result: more stable visual appearance than fully random every frame.

### 7) QA / validation
- Relevant client cvar: `cg_atmosphericLevel` (default: `2`).
- Validate maps at least on low/medium/high.
