# Q3RallyRadiant map settings

Repository: https://github.com/P3rlE/Q3RallyRadiant
Local checkout: D:/Q3RallyDev/Q3RallyRadiant

## Agreed workflow

- New Q3Rally map: show worldspawn settings before creating/replacing the map.
- Accept: create the map and write the configured worldspawn values.
- Cancel: retain the existing map; do not clear its contents or create a replacement.
- Existing map: reopen the same editor through Map Settings.
- Apply existing-map changes as one undoable edit; Cancel makes no changes.
- Preserve unknown keys. Distinguish omitted/default keys from explicit values.
- Scope the automatic prompt to the Q3Rally gamepack.

## Integration point found

radiant/map.cpp: NewMap currently calls ConfirmModified, Map_Free, Map_New.
Collect settings before Map_Free so cancellation cannot destroy the open map.
Do not put the automatic dialog in Map_New itself: the startup path also calls
Map_New (radiant/main.cpp), and unrelated internal callers should not gain a prompt.
Register a separate Map Settings command and menu item for existing maps.

## Inventory work

Derive fields, types, defaults and ranges from Q3Rally runtime readers, gamepack
entity definitions and compiler keys. Initial runtime keys include message,
music, gravity, enableDust, enableSnow, enableBreath, allowShortTrack,
allowMediumTrack, allowLongTrack, reflectionImage and reversable. This is an
initial list, not the full schema; weather, lighting and compiler keys still
need to be inventoried. New maps should offer sensible defaults rather than
requiring users to manually fill every optional key.

## Implementation status

Implemented in the Radiant checkout: radiant/q3rally_mapsettings.h and
q3rally_mapsettings_fields.inc, with New Map and File > Map Settings integration.
50 predefined runtime/gamepack keys plus freely editable additional keys.
Windows build and Qt dialog regressions pass. The offscreen UI preview was
visually inspected. The user confirmed the feature works in the editor. Full inventory and test instructions are in
D:/Q3RallyDev/Q3RallyRadiant/docs/q3rally-map-settings.md.
