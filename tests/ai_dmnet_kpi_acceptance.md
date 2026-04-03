# DMNet KPI-Akzeptanzkriterien

Dieses Dokument definiert die Auswertung der optionalen DMNet-Debug-Exports
(`g_aiDmnetDebugExport`) als Gate für zukünftige AI-Änderungen.

## Export aktivieren

```cfg
set g_aiDmnetDebugExport 1
set g_aiDmnetDebugExportPath "logs/ai_dmnet_debug.csv"
```

Alternativ JSONL:

```cfg
set g_aiDmnetDebugExport 2
set g_aiDmnetDebugExportPath "logs/ai_dmnet_debug.jsonl"
```

Pro Tick werden u. a. geschrieben:

- `routeIndex`
- `targetSpeed` / `actualSpeed`
- `decisionState` (Überholzustand)
- `collisionRisk`
- `recoveryEvent`
- `routeDeviation`

## KPI-Berechnung

```bash
python3 tools/ai_dmnet_kpi.py logs/ai_dmnet_debug.csv
```

Beispiel mit Akzeptanzgrenzen:

```bash
python3 tools/ai_dmnet_kpi.py logs/ai_dmnet_debug.csv \
  --threshold avg_ideal_line_deviation=140 \
  --threshold collision_rate=0.22 \
  --threshold recovery_frequency=0.08
```

## KPI-Definitionen

- `avg_ideal_line_deviation`: mittlere Abweichung von der Ideallinie (`routeDeviation`).
- `overtake_maneuvers`: Anzahl gestarteter Überholsequenzen.
- `overtake_aborts`: Anzahl Überhol-Abbrüche (`abort_overtake` während aktiver Überholsequenz).
- `collision_rate`: Anteil Ticks mit Kollisionsrisiko (`collisionRisk == 1`).
- `recovery_frequency`: Anteil Ticks mit Recovery-Ereignis.

## Nutzung als Akzeptanzkriterium

1. Vor einer AI-Änderung Referenzlauf fahren und KPIs speichern.
2. Nach der Änderung denselben Lauf wiederholen.
3. Änderung nur übernehmen, wenn definierte KPI-Schwellen eingehalten werden.
4. Bei Verschlechterung: Anpassung verwerfen oder neue Baseline bewusst dokumentieren.
