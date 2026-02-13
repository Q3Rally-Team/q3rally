# Remote-Menühintergrund (UI-Download)

Dieses Feature erlaubt es, ein optionales Hintergrundbild für das Hauptmenü von einer externen URL zu laden. Standardmäßig ist es aktiviert und nutzt die voreingestellte URL; der Download kann jederzeit deaktiviert werden.【F:engine/code/client/cl_bgasset.c†L97-L114】【F:engine/code/client/cl_bgasset.c†L957-L1002】

## Opt-in / Opt-out

- **Aktivieren:** Eine gültige `http://`- oder `https://`-URL per `ui_menuBackUrl` bzw. `cl_uiBackgroundURL` setzen und `ui_menuBackEnable` (UI) bzw. `cl_uiBackgroundEnable` aktivieren (Standard: `1`).【F:engine/code/client/cl_bgasset.c†L97-L114】【F:engine/code/client/cl_bgasset.c†L425-L432】【F:engine/code/client/cl_bgasset.c†L922-L931】
- **Deaktivieren (Opt-out):** `ui_menuBackEnable` oder `cl_uiBackgroundEnable` auf `0` setzen. In diesem Zustand wird kein Download angestoßen und der Status bleibt `disabled`.【F:engine/code/client/cl_bgasset.c†L107-L114】【F:engine/code/client/cl_bgasset.c†L957-L964】
- Ohne URL bleibt das Feature im `idle`-Zustand und startet keinen Request.【F:engine/code/client/cl_bgasset.c†L967-L972】

> **Datenschutz / Zustimmung:** Es erfolgen **keine automatischen Downloads**, wenn das Feature deaktiviert ist oder keine URL gesetzt wurde.【F:engine/code/client/cl_bgasset.c†L957-L1002】

## Zulässige Dateitypen

Die Datei wird nach dem Download validiert. Unterstützt werden:

- **PNG** (Signaturprüfung)
- **JPEG/JPG** (SOF-Marker-basiert)
- **TGA** (nur unkomprimiert/komprimiert mit 24 oder 32 Bit)【F:engine/code/client/cl_bgasset.c†L585-L605】【F:engine/code/client/cl_bgasset.c†L637-L645】

Andere Formate führen zu einem Fehlerzustand.【F:engine/code/client/cl_bgasset.c†L650-L652】

## Limits

- **Dateigröße:** Standardlimit 5 MiB (`CL_BGASSET_DEFAULT_MAX_SIZE`). Über `cl_uiBackgroundMaxSize` lässt sich der Maximalwert anpassen. Downloads werden abgebrochen, wenn `Content-Length` oder die tatsächliche Größe das Limit überschreiten.【F:engine/code/client/cl_bgasset.c†L18-L19】【F:engine/code/client/cl_bgasset.c†L627-L631】【F:engine/code/client/cl_bgasset.c†L669-L679】【F:engine/code/client/cl_bgasset.c†L735-L744】【F:engine/code/client/cl_bgasset.c†L773-L777】【F:engine/code/client/cl_bgasset.c†L929-L931】
- **Abmessungen:** Maximal 4096×4096 px. Größere Bilder werden abgelehnt.【F:engine/code/client/cl_bgasset.c†L19-L19】【F:engine/code/client/cl_bgasset.c†L660-L663】

## Cache-Ort

Das Bild wird im lokalen Cache unter `<BASEGAME>/ui_cache` gespeichert; die Cache-Tabelle liegt in `<BASEGAME>/ui_cache/cache_index.txt`.【F:engine/code/client/cl_bgasset.c†L8-L10】  
Zur Revalidierung nutzt der Client `ETag` und `Last-Modified` Header, wodurch bestehende Cache-Einträge aktualisiert werden können, ohne das Asset erneut zu laden.【F:engine/code/client/cl_bgasset.c†L820-L827】

## Datenschutz / Netzwerkverhalten

- Es werden nur **HTTP(S)-Requests** zur vom Nutzer konfigurierten URL durchgeführt.【F:engine/code/client/cl_bgasset.c†L425-L432】【F:engine/code/client/cl_bgasset.c†L829-L839】
- Der Request setzt einen **User-Agent** mit Produktname und Version sowie – falls vorhanden – `ETag`/`Last-Modified`-Header zur Cache-Revalidierung.【F:engine/code/client/cl_bgasset.c†L820-L840】
- Ohne aktiviertes Feature oder ohne URL findet **kein** Download statt.【F:engine/code/client/cl_bgasset.c†L957-L972】

## Offline-Fallback

Ist der Remote-Hintergrund deaktiviert, fehlgeschlagen (`failed`/`unavailable`) oder nicht verfügbar, greift die UI automatisch auf den **Standard-Hintergrund** zurück.【F:engine/code/q3_ui/ui_atoms.c†L192-L199】【F:engine/code/q3_ui/ui_atoms.c†L274-L289】  
Wenn cURL nicht verfügbar ist, markiert der Client den Zustand als `unavailable` und nutzt ebenfalls den Standard-Hintergrund.【F:engine/code/client/cl_bgasset.c†L939-L945】
