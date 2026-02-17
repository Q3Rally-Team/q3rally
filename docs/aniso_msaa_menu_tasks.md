# Aufgabenplan: Anisotropes Filtern & MSAA im Q3Rally-Menü

## Ziel
Spieler sollen **anisotropes Filtern** und **MSAA** direkt über das Grafikmenü konfigurieren können, statt nur per Konsole/Config.

---

## Epic 1: Menü-UX für Anisotropie und MSAA erweitern

### Task 1.1 – Neue UI-Elemente im Video-Menü ergänzen
- In `engine/code/q3_ui/ui_video.c` zwei zusätzliche Optionen einbauen:
  - **Anisotropic Filtering** (Off, 2x, 4x, 8x, 16x)
  - **MSAA** (Off, 2x, 4x)
- Positionierung im bestehenden Layout unter „Texture Filter“ bzw. im Quality-Block.

**Akzeptanzkriterien**
- Beide Optionen sind im Menü sichtbar und fokussierbar.
- Auswahlwerte werden beim Öffnen aus den aktuellen Cvars korrekt vorgeladen.

### Task 1.2 – Cvar-Mapping sauber definieren
- UI-Werte auf Cvars abbilden:
  - Anisotropie:
    - `r_ext_texture_filter_anisotropic` (0/1)
    - `r_ext_max_anisotropy` (2/4/8/16)
  - MSAA:
    - `r_ext_multisample` (0/2/4)
    - optional zusätzlich `r_ext_framebuffer_multisample` im opengl2-Renderer
- Beim Anwenden die gleichen Persistenz-/Latch-Muster wie bestehende Grafikoptionen verwenden.

**Akzeptanzkriterien**
- Änderung im Menü erzeugt konsistente Cvar-Werte.
- `Apply` + `vid_restart` setzt die Werte korrekt durch.

### Task 1.3 – Feature-abhängige Anzeige / Fallbacks
- Wenn Extension/Capability fehlt:
  - Option ausgrauen **oder** nur „Off“ erlauben.
- Sinnvolle Begrenzung der maximalen auswählbaren Anisotropie anhand `maxAnisotropy`.

**Akzeptanzkriterien**
- Auf Systemen ohne Support keine fehlerhaften/irreführenden Auswahlwerte.
- UI fällt robust auf „Off“ zurück.

---

## Epic 2: Renderer-Integration validieren (GL1 + GL2)

### Task 2.1 – GL1-Pfad verifizieren
- Prüfen, dass neue UI-Werte im GL1-Renderer korrekt greifen.
- Fokus: `r_ext_texture_filter_anisotropic`, `r_ext_max_anisotropy`, `r_ext_multisample`.

**Akzeptanzkriterien**
- GL1 startet mit gewählten Werten ohne Fehler.
- Texturniveau sichtbar anisotrop, sofern Hardware/Driver unterstützt.

### Task 2.2 – GL2-Pfad inkl. FBO-MSAA verifizieren
- Prüfen, ob bei GL2 zusätzlich `r_ext_framebuffer_multisample` gesetzt werden soll.
- Sicherstellen, dass Limitierung auf verfügbare Samples sauber funktioniert.

**Akzeptanzkriterien**
- GL2 verwendet gewählte MSAA-Stufe stabil.
- Bei fehlender Unterstützung automatische Absenkung ohne Crash.

### Task 2.3 – Renderer-spezifische Logik kapseln
- `cl_renderer` berücksichtigen (opengl1/opengl2).
- Mapping-Funktion(en) in UI-Code zentral halten, statt verstreuter Sonderfälle.

**Akzeptanzkriterien**
- Keine widersprüchlichen Cvar-Kombinationen bei Rendererwechsel.
- Code bleibt wartbar (klare Helper-Funktionen).

---

## Epic 3: UX-Details, Doku, Qualitätssicherung

### Task 3.1 – Hilfetexte/Labels ergänzen
- Kurze Hinweise im Menü:
  - „Aniso verbessert Schärfe in der Tiefe“
  - „MSAA glättet Kanten, kostet GPU-Leistung“
- Optional „Neustart erforderlich“ bei latched Cvars kenntlich machen.

**Akzeptanzkriterien**
- Nutzer versteht Wirkung und Performance-Kosten ohne externe Doku.

### Task 3.2 – Testmatrix definieren
- Testfälle:
  1. opengl2 + volle Unterstützung
  2. opengl2 + limitierte Samples
  3. opengl1 + Unterstützung
  4. fehlende Aniso-Extension
- Je Fall prüfen: Menüanzeige, Apply-Verhalten, Persistenz, visuelles Ergebnis.

**Akzeptanzkriterien**
- Für alle Fälle dokumentierte Soll-/Ist-Ergebnisse vorhanden.

### Task 3.3 – Dokumentation aktualisieren
- Nutzerdoku/Readme um neue Menüoptionen erweitern.
- Server-/Client-Troubleshooting: „Option nicht verfügbar“ erklären.

**Akzeptanzkriterien**
- Neue Optionen sind in der Doku auffindbar und verständlich beschrieben.

---

## Empfohlene Umsetzungsreihenfolge
1. UI-Elemente + Cvar-Mapping (Epic 1)
2. GL2-Validierung inkl. FBO-MSAA (Task 2.2)
3. GL1-Validierung (Task 2.1)
4. Fallback-/Renderer-Kapselung (Task 2.3)
5. Hilfetexte, Tests, Doku (Epic 3)

---

## Definition of Done (gesamt)
- Menü bietet Aniso + MSAA als auswählbare Optionen.
- Optionen sind renderer-/hardwareabhängig robust.
- Persistenz und `vid_restart`-Flow funktionieren fehlerfrei.
- Dokumentation und Testmatrix sind aktualisiert.
