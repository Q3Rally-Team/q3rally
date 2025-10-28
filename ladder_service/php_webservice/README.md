# Q3Rally Ladder Service

Dieses Verzeichnis enthält einen kleinen PHP-Dienst, der Q3Rally-Matches entgegennimmt, speichert und im Browser als Ladder-Dashboard darstellt. Der Service benötigt keine zusätzlichen Bibliotheken und legt eingehende Matches als JSON-Dateien im Unterordner `data/` ab.

## Deployment
1. Den gesamten Inhalt dieses Ordners (`index.php` und den leeren Ordner `data/`) auf den gewünschten Webspace hochladen.
2. Sicherstellen, dass PHP 8.0 oder neuer aktiviert ist und der Webserver Schreibrechte für den Ordner `data/` besitzt. Bei Bedarf dem Ordner über das Hosting-Panel Schreibrechte gewähren.
3. Nach dem Upload ist die API unter der URL des Webspaces erreichbar, zum Beispiel `https://example.com/ladder/index.php`.

> Hinweis: Viele Hoster setzen `index.php` automatisch als Startdatei. Liegt der Ordner direkt im Document-Root, kann die Basis-URL z. B. `https://example.com/` sein. Sonst ggf. den Unterordner an die URL anhängen.

## API-Übersicht

* **POST `/matches`** – Speichert ein Match. Erwartet das JSON, das der Q3Rally-Server erzeugt (inklusive `matchId`). Bereits vorhandene IDs werden ignoriert und mit HTTP 200 quittiert.
* **GET `/matches`** – Liefert eine Liste aller gespeicherten Matches (neueste zuerst). Optional können `mode`, `limit` und `offset` als Query-Parameter gesetzt werden.
* **GET `/matches/{matchId}`** – Gibt das vollständige JSON zu einer Match-ID zurück.
* **DELETE `/matches/{matchId}`** – Löscht ein Match dauerhaft.
* **GET `/version`** – Liefert die aktuelle Spielversion für den im Client integrierten Update-Check.
* **POST `/version`** – Aktualisiert Version, Download-Link und Nachricht für den Update-Check. Erfordert ein Passwort.

### Beispiel-Aufrufe
```bash
# Match speichern
curl -X POST https://example.com/ladder/index.php/matches \
     -H "Content-Type: application/json" \
     -d @match.json

# Letzte Matches anzeigen
curl https://example.com/ladder/index.php/matches?limit=10

# Einzelnes Match abrufen
curl https://example.com/ladder/index.php/matches/srv-20240405-183011-42

# Match löschen
curl -X DELETE https://example.com/ladder/index.php/matches/srv-20240405-183011-42
```

## Datenablage
Jedes Match wird als einzelne JSON-Datei unter `data/<matchId>.json` abgelegt. So lässt sich der Ordner bei Bedarf sichern oder in andere Systeme importieren. Der Service fügt automatisch einen Zeitstempel `receivedAt` hinzu, um Listen sortieren zu können.

## Versionsverwaltung für den Update-Check

Der Q3Rally-Client fragt beim Start `https://ladder.q3rally.com/api/v1/version` ab, um neue Versionen zu erkennen. Das Ladder-Backend liefert dafür die passende Antwort über den Endpunkt `/version`.

### Passwort konfigurieren

Für Schreibzugriffe muss ein Passwort gesetzt werden. Dies geschieht bevorzugt über eine Umgebungsvariable oder Server-Konfiguration:

* `LADDER_VERSION_PASSWORD`
* alternativ `VERSION_PASSWORD`

Auf Shared-Hosting-Umgebungen lässt sich das beispielsweise über `.htaccess` erledigen:

```apacheconf
SetEnv LADDER_VERSION_PASSWORD "?Vc09061981!"
```

Alternativ kann ein Pfad zu einer Passwortdatei hinterlegt werden (`LADDER_VERSION_PASSWORD_FILE` bzw. `VERSION_PASSWORD_FILE`). Der Dienst erwartet darin den Klartext (ohne Zeilenumbrüche am Ende). Ohne weitere Konfiguration wird automatisch die Datei `version.password` im Projektordner gelesen – sie kann mit einem beliebigen Editor erstellt werden:

```bash
echo -n '?Vc09061981!' > /pfad/zur/ladder/version.password
```

Ist kein Passwort gesetzt, beantwortet der Dienst Schreibversuche mit HTTP 403.

### Version auslesen

```bash
curl https://example.com/ladder/index.php/version
```

Die Antwort erfolgt standardmäßig als JSON und enthält mindestens das Feld `latest` (bzw. `version`). Auf Wunsch liefert der Server auch Plaintext, wenn ausschließlich `text/plain` im `Accept`-Header steht.

### Version setzen

```bash
curl -X POST https://example.com/ladder/index.php/version \
     -H "Content-Type: application/json" \
     -d '{
           "password": "supersecret",
           "version": "v0.7",
           "downloadUrl": "https://downloads.example.com/q3rally-v0.7.zip",
           "message": "Bugfix-Release mit verbesserten Streckenzeiten."
         }'
```

Erfolgreiche Anfragen werden mit der gespeicherten Version beantwortet. Felder wie `downloadUrl` oder `message` sind optional und lassen sich später erneut überschreiben oder durch leere Werte entfernen.

## Web-Frontend
Die Startseite von `index.php` liefert ein Dashboard, das die gespeicherten Matches aus dem `data/`-Ordner lädt und verschiedene Ranglisten generiert:

* **Bestzeiten-Ranking** – Renn-Modi werden ausgewertet, Bestzeiten pro Spieler und Map erkannt und aufsteigend sortiert.
* **Deathmatch-Wertung** – Für klassische Deathmatch-Modi werden K/D-Werte, Kills und Deaths berechnet und nach Verhältnis sortiert.
* **Objective-Wertung** – CTF-, Domination-, Derby- und ähnliche Modi werden nach Zielwerten, Punkten oder Rundensiegen gerankt. Jede Tabelle zeigt die beste Map-Leistung pro Spieler.
* **Elimination-Sonderwertung** – Für Elimination-Matches wird zusätzlich eine separate Rangliste geführt, die sich auf Rundensiege stützt oder sie bei fehlenden Angaben herleitet.

### Domination-Punkte
Für Domination-Matches durchsucht die Auswertung jedes Scoreboard-Element nach typischen Feldern für Kontrollpunkte (`objectives`, `objectiveScore`, `objectivePoints`). Ist eines dieser Felder vorhanden, wird dieser Wert als Ranglisten-Metrik übernommen. Fehlen solche Angaben, greift die Ladder auf das allgemeine Punktesystem des Matches zurück (`score`, `points`, `value`).

Das bedeutet konkret: Die Spalte **Points** in der Domination-Tabelle zeigt entweder die tatsächlich übernommene Anzahl an gehaltenen Domination-Zielen oder – falls das Match-JSON diese Information nicht enthält – die regulären Score/Points des Spielers. Dieses Verhalten ist in `index.php` hinterlegt, siehe `OBJECTIVE_MODE_PRIORITY['gt_domination']` und die Definition der Score-Pfade.

## Backup & Wartung
* Regelmäßig den Ordner `data/` sichern.
* Bei sehr vielen Matches kann die Dateibasis unübersichtlich werden; für große Installationen empfiehlt sich langfristig dennoch eine vollwertige Datenbank.

## Fehlerbehandlung
Fehlerhafte Anfragen werden als JSON im Format `{ "error": "..." }` beantwortet. Stimmt etwas mit den Dateirechten nicht, liefert der Service HTTP-Status 500.
