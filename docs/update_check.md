# Update-Prüfung in Q3Rally

Diese Notiz beschreibt, wie die im Client integrierte Update-Prüfung funktioniert.

## Auslöser des Checks

Sobald die UI initialisiert wird (`CL_InitUI`), wird `CL_UpdateRequestLatest` aufgerufen. Dadurch startet die eigentliche Update-Prüfung immer beim Übergang ins Hauptmenü bzw. wenn die UI neu geladen wird.【F:engine/code/client/cl_ui.c†L2145-L2189】

## HTTP-Anfrage

Die Funktion `CL_UpdateRequestLatest` sorgt dafür, dass die benötigten Cvars existieren und prüft zunächst, ob `cl_updateCheck` aktiviert ist. Anschließend initialisiert sie (falls notwendig) die cURL-Handles, setzt das Ziel (`cl_updateEndpoint`) sowie weitere Optionen und reiht den Request in den Multi-Handle ein. Gleichzeitig wird der Status auf `checking` gesetzt.【F:engine/code/client/cl_ui.c†L480-L503】【F:engine/code/client/cl_ui.c†L956-L1032】

Standardmäßig verweist `cl_updateEndpoint` auf `https://ladder.q3rally.com/version.txt`. Der Wert lässt sich aber per Cvar oder über die `q3config.cfg` überschreiben, falls ein eigener Server genutzt werden soll.【F:engine/code/client/cl_ui.c†L501-L507】

Beim ersten Start mit der neuen Version prüft der Client, ob in der bestehenden Konfiguration noch der frühere Standardwert `https://ladder.q3rally.com/index.php/version` eingetragen ist, und aktualisiert ihn automatisch auf die Textdatei. Auf diese Weise schlagen bestehende Installationen nicht mehr mit HTTP 404 fehl, sobald der Server auf das neue Format umgestellt ist.【F:engine/code/client/cl_ui.c†L501-L507】

### Erwartete Serverantwort

Der Client erwartet eine Textdatei, deren erste Zeile die Versionsnummer enthält (z. B. `v0.7`). Vor- und nachgelagerte Leerzeichen sowie Leerzeilen werden entfernt; optional umschließende Anführungszeichen werden ebenfalls gestrichen, bevor die Version ausgewertet wird. Ein führendes `v`/`V` ist optional – die Vergleichslogik berücksichtigt beide Varianten automatisch. Weitere Inhalte der Datei werden ignoriert.【F:engine/code/client/cl_ui.c†L589-L818】

Damit der Check funktioniert, muss der Server einen per HTTPS erreichbaren GET-Endpunkt bereitstellen, der diese Textdatei ausliefert. Eine statische Datei reicht völlig aus – sie muss lediglich aktualisiert werden, sobald eine neue Version veröffentlicht wird.

## Verarbeitung der Antwort

In jedem Frame ruft `CL_Frame` die Pump-Funktion `CL_UpdatePumpRequest` auf. Diese kümmert sich darum, den cURL-Transfer voranzutreiben und reagiert auf Fehler (z. B. fehlende Verbindung oder HTTP-Status ungleich 200). Sobald der Download abgeschlossen ist, wird `CL_UpdateHandleResponse` ausgeführt.【F:engine/code/client/cl_main.c†L2974-L2995】【F:engine/code/client/cl_ui.c†L894-L952】

`CL_UpdateHandleResponse` wertet den Inhalt aus: `CL_UpdateParseResponse` liest die Versionszeichenkette aus der Textdatei und stellt sicher, dass sie mindestens eine Ziffer enthält. Danach vergleicht `CL_UpdateCompareVersions` die Remote-Version mit der lokalen `PRODUCT_VERSION`. Ist die entfernte Version neuer, wird der Status `outdated` gesetzt; andernfalls `up_to_date`. Fehlende oder ungültige Antworten führen zu `error` mitsamt Meldung.【F:engine/code/client/cl_ui.c†L653-L818】

Bei einem Mismatch setzt der Client eine Standardnachricht („Visit www.q3rally.com to download the latest version.“) sowie die Website-URL, damit das Hauptmenü einen Hinweis auf die Download-Seite anzeigen kann.【F:engine/code/client/cl_ui.c†L791-L808】【F:engine/code/q3_ui/ui_menu.c†L200-L233】

Die aktuelle lokale Versionsnummer ist in `q_shared.h` als `PRODUCT_VERSION` definiert; sie wird gegen den vom Server gelieferten Wert geprüft.【F:engine/code/qcommon/q_shared.h†L57-L74】

## Darstellung im UI

Das UI fragt regelmäßig die Status- und Detail-Cvars (`cl_updateStatus`, `cl_updateLatest`, `cl_updateUrl`, `cl_updateMessage`) ab. Sobald der Status `outdated` lautet, baut `UI_MaybeShowUpdateDialog` ein Popup mit Version, Nachricht und Download-Link und zeigt es im Hauptmenü an. Für den Status `error` wird nun ebenfalls ein Hinweis eingeblendet, der die Fehlermeldung aus `cl_updateMessage` übernimmt. Jeder Dialog erscheint nur einmal pro Statuswechsel.【F:engine/code/q3_ui/ui_menu.c†L175-L236】【F:engine/code/q3_ui/ui_menu.c†L500-L527】

## Zusammenfassung des Entscheidungsweges

1. UI-Initialisierung triggert `CL_UpdateRequestLatest`.
2. cURL lädt die Versionsinformationen vom konfigurierten Endpoint.
3. `CL_UpdateHandleResponse` analysiert die Antwort und vergleicht sie mit `PRODUCT_VERSION`.
4. Bei abweichender (neuere) Version setzt der Client den Status `outdated` und das Hauptmenü zeigt ein Popup an.
