# Racing Bot Hybrid Waypoint Plan (Record + Flexible Corridor)

## Ziel

Das aktuelle Bot-Racing-System soll für Mapper deutlich einfacher werden, ohne die bestehende Checkpoint-/Lap-Logik zu brechen.

**Hybrid-Ansatz:**

1. **Ghost-Converter (bevorzugt):** Eine vorhandene `.ghost`-Runde wird automatisiert in eine `maps/<map>.bpd`-Linie umgewandelt.
2. **Flexible Ingame-Navigation:** Bots nutzen die Linie als Primärziel, dürfen aber lokal ausweichen (Hindernisse, andere Fahrer, blockierte Linie).
3. **Fallback Record & Bake:** Falls keine Ghost-Datei vorliegt, kann die Linie weiterhin direkt im Spiel aufgezeichnet und als `.bpd` gespeichert werden.

---

## Warum dieser Mix sinnvoll ist

### Vorteile von "Ghost-Converter"

- Noch weniger Aufwand für Mapper: bestehende Bestzeiten/Trainingsrunden können direkt genutzt werden.
- Reproduzierbare Qualität: dieselbe `.ghost` erzeugt deterministisch dieselbe Bot-Linie.
- Ideal für Community-Workflow: Ghost teilen → Converter laufen lassen → sofort botfähig testen.

### Vorteile von "Record & Bake" (Fallback)

- Sehr niedrige Einstiegshürde für Mapper (keine manuelle Bezier-Feinarbeit als Pflicht).
- Linie repräsentiert real fahrbare Route statt theoretischer Kurve.
- Schneller Iterationszyklus: Runde aufzeichnen → testen → bei Bedarf neu aufzeichnen.

### Vorteile der "flexiblen Abweichung"

- Weniger Bot-Staus bei Kollisionen.
- Stabileres Verhalten in engen Kurven/Bottlenecks.
- Bots wirken weniger "auf Schienen" und natürlicher im Rennen.

---

## Technischer Entwurf

## 1) Datenquelle pro Map

- Primäre Racing-Line aus `maps/<map>.bpd`.
- Checkpoints (`rally_checkpoint`) bleiben **autoritativ** für Rundenfortschritt, Platzierung, Splits.
- `.bpd` liefert die Fahrgeometrie (Zielpunkte/Handles) zwischen den Checkpoints.

**Regel:** Falls `.bpd` fehlt, fallback auf bestehendes Verhalten mit `bezierPos`/`bezierDir` aus den Map-Entities.

## 2) Autoring-Workflow für Mapper

**Bevorzugter Weg (Ghost → BPD):**

- Eingabe: `ghost/<map>/<run>.ghost` (oder bestehender Ghost-Pfad)
- Tool: `ghost2bpd <input.ghost> <output.bpd>`
- Ausgabe: `maps/<map>.bpd`

Beim Konvertieren:

- Ghost-Samples werden resampled/geglättet.
- Samples werden auf Checkpoint-Segmente projiziert.
- Bézier-Handles werden automatisch aus Tangenten/Krümmung geschätzt.

**Fallback im Spiel (Record & Bake):**

Neuer Dev-Workflow (Konsole):

- `recordRacingLine start`
- Mapper fährt eine Referenzrunde
- `recordRacingLine stop`
- `recordRacingLine save`

Beim Save:

- Sample-Punkte werden geglättet.
- Punkte werden Checkpoint-Segmenten zugeordnet.
- Ausgabe als `maps/<map>.bpd` im bestehenden parserfreundlichen Format.

## 3) Bot-Fahrlogik im Rennen

Bots bekommen pro Segment einen **Korridor** um die ideale Linie:

- Ideale Linie = Punkt auf Kurve bei `f` (bestehender Ansatz).
- Korridorbreite dynamisch nach Strecke/Tempo (z. B. enger in Schikanen, breiter auf Geraden).
- Lokales Ziel wird innerhalb des Korridors gewählt, nicht nur exakt auf der Linie.

Prioritäten für Zielwahl:

1. Kollision vermeiden (Fahrzeuge/Hindernisse).
2. Rennfortschritt halten (Richtung nächster Checkpoint).
3. Zur Ideallinie zurückkehren, wenn frei.

## 4) Rückkehrlogik (Recovery)

Wenn Bot stark von der Linie abweicht:

- "Rejoin Target" vor dem Bot auf der idealen Linie bestimmen.
- Lenkung auf Rejoin statt hart auf nächsten Checkpoint "einschnappen".
- Anti-Oszillation durch kurze Cooldown-Zeit für Spurwechselentscheidungen.

---

## Konkrete Implementierungsschritte

1. **Ghost-Converter ergänzen** (neues Tool oder Script):
   - `.ghost` einlesen, glätten, segmentieren, `.bpd` exportieren.
2. **Dev Commands ergänzen** in `g_cmds.c`:
   - Aufnahme starten/stoppen/speichern.
3. **Recorder/Exporter** in `g_rally_tools.c`:
   - Sample sammeln, glätten, als `.bpd` schreiben.
4. **Lader robust machen** in `g_rally_tools.c`/`g_spawn.c`:
   - saubere Fallback-Regeln und klare Warnungen im Log.
5. **Bot-Lenkung erweitern** in `ai_dmnet.c`:
   - Korridorziel statt Punktziel, plus Recovery-Logik.
6. **Debug-Overlay/Logs**:
   - aktuelle Ideallinie, Korridor und gewähltes Bot-Ziel sichtbar machen.

---

## Mapper-UX (kurz)

- Strecke mit normalen `rally_checkpoint`-Volumes bauen.
- Bestehende Ghost-Runde per `ghost2bpd` in `.bpd` umwandeln.
- Bot-Testlauf starten.
- Nur falls nötig: Ingame aufnehmen und kritische Segmente nachjustieren.
- Bei Problemen nur einzelne Segmente nachjustieren statt komplette Strecke neu zu verdrahten.

---

## Akzeptanzkriterien

- Mapper kann für neue Racing-Map aus einer vorhandenen `.ghost` ohne manuelle Handle-Feinarbeit eine nutzbare Bot-Linie erzeugen.
- Bots bleiben bei Verkehr/Hindernissen nicht dauerhaft auf der Ideallinie hängen.
- Nach Ausweichmanövern kehren Bots stabil auf Race-Fortschrittskurs zurück.
- Ohne `.bpd` bleibt Legacy-Verhalten vollständig funktionsfähig.
