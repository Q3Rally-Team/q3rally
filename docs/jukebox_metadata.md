# Jukebox OGG-Metadaten

Die Jukebox liest optional Vorbis-Comments aus `.ogg`-Dateien in `music/jukebox`.

## Unterstützte Tags

- `TITLE`
- `ARTIST`
- `ALBUM` (wird aktuell gelesen und gecacht, aber nicht im Overlay angezeigt)

## Anzeige-/Fallback-Logik

Beim Scan (`CG_JukeboxScan`) werden Dauer und Metadaten pro Track gecacht.

Overlay-Name beim Abspielen (`CG_JukeboxPlayIndex`):

1. Wenn `ARTIST` **und** `TITLE` vorhanden sind: `Artist - Title`
2. Sonst, wenn nur `TITLE` vorhanden ist: `Title`
3. Sonst Fallback auf den bisherigen Dateinamen-basierten Anzeigenamen

Damit bleibt das bisherige Verhalten für Dateien ohne Tags vollständig erhalten.
