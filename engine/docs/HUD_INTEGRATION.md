# HUD Refactor – Integration Guide

## Neue Dateistruktur

| Datei | Inhalt | Ersetzt |
|---|---|---|
| `cg_hud_elements.h` | Alle extern-Deklarationen, Toggle-CVars, Funktionsprototypen | — (neu) |
| `cg_hud_core.c` | `CG_DrawHUD()`, Upper/Lower HUD, Options-Menü, CVar-Registrierung | `cg_rally_hud2.c` (unterer Teil) + `cg_rally_hud.c` (Dispatcher) |
| `cg_hud_racing.c` | Arrow, Times, Ghost, Laps, Distance, Position, Elim.-Timeline, Car-Ahead, Panel-Funktionen | `cg_rally_hud.c` + `cg_rally_hud2.c` (oberer Teil) |
| `cg_hud_derby.c` | DerbyHitImpact, DerbyVehicleState, DerbyList (Duplikat aufgelöst!) | `cg_rally_hud2.c` |
| `cg_hud_vehicle.c` | AddObjectsToScene, RearviewMirror, MMap, FuelGauge, Speed/RPM | `cg_rally_hud.c` |

## Schritt 1: Makefile anpassen

In der `CGAME_OBJS`-Liste (und `CGAME_QVM_OBJ` falls vorhanden):

```makefile
# ALT (entfernen):
  $(B)/$(BASEGAME)/cgame/cg_rally_hud.o \
  $(B)/$(BASEGAME)/cgame/cg_rally_hud2.o \

# NEU (ersetzen durch):
  $(B)/$(BASEGAME)/cgame/cg_hud_core.o \
  $(B)/$(BASEGAME)/cgame/cg_hud_racing.o \
  $(B)/$(BASEGAME)/cgame/cg_hud_derby.o \
  $(B)/$(BASEGAME)/cgame/cg_hud_vehicle.o \
```

## Schritt 2: CVar-Registrierung in cg_main.c eintragen

In `CG_RegisterCvars()` (oder wo die anderen cvars registriert werden):

```c
#include "cg_hud_elements.h"

// Am Ende der Funktion:
CG_HUD_RegisterCvars();
```

## Schritt 3: HUD-Options-Menü an Taste binden

In `CG_InitConsoleCommands()` (cg_consolecmds.c):

```c
trap_AddCommand( "cg_hudToggle" );
```

In `CG_ConsoleCommand()`:

```c
if ( Q_stricmp( cmd, "cg_hudToggle" ) == 0 ) {
    CG_ToggleHUDOptions();
    return qtrue;
}
```

In der Spieler-Config (`q3config.cfg` oder `autoexec.cfg`):

```
bind F9 "cg_hudToggle"
```

## Schritt 4: Maus-Events weiterleiten (optional, für Click-Toggle)

Falls das Projekt `CG_MouseEvent()` implementiert:

```c
// In CG_MouseEvent():
if ( CG_HUDOptions_MouseEvent( cx, cy, clicked ) ) {
    return;
}
```

## Toggle-CVars Übersicht

Alle CVars sind `CVAR_ARCHIVE` – Einstellungen bleiben nach Neustart erhalten.

### Racing
| CVar | Standard | Beschreibung |
|---|---|---|
| `cg_hudShowTimes` | 1 | Zeiten-Panel (Lap/Total/Best) |
| `cg_hudShowLaps` | 1 | Runden-Zähler |
| `cg_hudShowPosition` | 1 | Rennposition |
| `cg_hudShowDistToFinish` | 1 | Distanz zum Ziel |
| `cg_hudShowGhostDelta` | 1 | Ghost-Split-Delta |
| `cg_hudShowArrow` | 1 | Checkpoint-Pfeil |
| `cg_hudShowCarAheadBehind` | 1 | Gegner-Liste (vor/hinter) |
| `cg_hudShowElimTimeline` | 1 | Elimination-Timeline |
| `cg_hudShowOpponentList` | 1 | Opponent-Liste (Elim/LCS) |
| `cg_hudShowScores` | 1 | DM/Team-Scores |

### Fahrzeug
| CVar | Standard | Beschreibung |
|---|---|---|
| `cg_hudShowSpeed` | 1 | Tachometer/RPM |
| `cg_hudShowFuelGauge` | 1 | Tankanzeige |
| `cg_hudShowRearView` | 1 | Rückspiegel |
| `cg_hudShowMiniMap` | 1 | Mini-Karte |

### Derby
| CVar | Standard | Beschreibung |
|---|---|---|
| `cg_hudShowDerbyVehicle` | 1 | Fahrzeugzustand-Panel |
| `cg_hudShowDerbyList` | 1 | Derby-Scoreboard |
| `cg_hudShowDerbyHitImpact` | 1 | Treffer-Flash |

## Aufgelöste Probleme

- **Duplikat `CG_DrawHUD_DerbyList`**: Die alte Version in `cg_rally_hud.c` war bereits mit `#if 0` / `*/` auskommentiert.
  Die aktive Version aus `cg_rally_hud2.c` lebt jetzt in `cg_hud_derby.c`.
- **`static`-Modifier**: Alle Funktionen die vom Core aufgerufen werden sind jetzt nicht mehr `static`
  (Deklaration über Header, kein Linker-Problem mehr).
- **Farb-Array**: `colors[4][4]` umbenannt zu `hudColors[4][4]` in `cg_hud_core.c` um Kollisionen zu vermeiden.
