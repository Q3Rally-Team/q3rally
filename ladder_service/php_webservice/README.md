# PHP Ladder Webservice

Dieses Verzeichnis enthält einen minimalen Ladder-Endpunkt, der sich auf typischem Webspace mit PHP-Unterstützung betreiben lässt. Der Service benötigt keine zusätzlichen Bibliotheken und speichert eingehende Matches als JSON-Dateien im Unterordner `data/`.

## Deployment
1. Den gesamten Inhalt dieses Ordners (`index.php` und den leeren Ordner `data/`) auf den gewünschten Webspace hochladen.
2. Sicherstellen, dass PHP 8.0 oder neuer aktiviert ist und der Webserver Schreibrechte für den Ordner `data/` besitzt. Bei Bedarf dem Ordner über das Hosting-Panel Schreibrechte gewähren.
3. Nach dem Upload ist die API unter der URL des Webspaces erreichbar, zum Beispiel `https://example.com/ladder/index.php`.

> Hinweis: Viele Hoster setzen `index.php` automatisch als Startdatei. Liegt der Ordner direkt im Document-Root, kann die Basis-URL z. B. `https://example.com/` sein. Sonst ggf. den Unterordner an die URL anhängen.

## API-Übersicht

* **POST `/matches`** – Speichert ein Match. Erwartet das JSON, das der Q3Rally-Server erzeugt (inklusive `matchId`). Bereits vorhandene IDs werden ignoriert und mit HTTP 200 quittiert.
* **GET `/matches`** – Liefert eine Liste aller gespeicherten Matches (neueste zuerst). Optional können `mode`, `limit` und `offset` als Query-Parameter gesetzt werden.
* **GET `/matches/{matchId}`** – Gibt das vollständige JSON zu einer Match-ID zurück.
* **DELETE `/matches/{matchId}`** – Löscht ein Match dauerhaft.

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

## Backup & Wartung
* Regelmäßig den Ordner `data/` sichern.
* Bei sehr vielen Matches kann die Dateibasis unübersichtlich werden; für große Installationen empfiehlt sich langfristig dennoch eine vollwertige Datenbank.
* Bei Upgrades der Profilstruktur kann `php migrate_profiles.php` ausgeführt werden, um bestehende `data/profiles/*.json` auf das neue Match-Dedupe-Tracking (`_processedMatchIds`, `_lastProcessedMatch`) zu migrieren.

## Fehlerbehandlung
Fehlerhafte Anfragen werden als strukturiertes JSON beantwortet:

```json
{
  "error": {
    "code": "MATCH_ID_REQUIRED",
    "message": "matchId is required.",
    "details": {}
  }
}
```

Ältere Payload-Varianten werden weiterhin akzeptiert (degraded mode), die neue Payload-Semantik wird jedoch bevorzugt normalisiert verarbeitet.


## Contract-Release v1.0.8 (Mode-aware Felder)

### Verbindliche Felder pro Modus

Die API erwartet jetzt pro Modus verpflichtende, kanonische Felder pro Spielerobjekt:

- **Racing-Modi** (`GT_RACING`, `GT_RACING_DM`, `GT_SPRINT`, `GT_TEAM_RACING`, `GT_TEAM_RACING_DM`):
  - `raceTimeMs`, `bestLapMs`, `checkpoints`
- **Deathmatch-Modi** (`GT_DEATHMATCH`, `GT_TEAM`, `GT_DERBY`, `GT_LCS`):
  - `kills`, `deaths`
- **Objective-Modi** (`GT_CTF`, `GT_CTF4`, `GT_DOMINATION`, `GT_KOTH`):
  - `objectiveScore`, `objectiveTimeMs`
- **Elimination** (`GT_ELIMINATION`):
  - `objectiveScore`, `objectiveTimeMs`, `eliminationRound`, `eliminationState`

Fehlen diese Felder, wird das Match als **invalid contract payload** abgewiesen.

### Deprecated / neu interpretierte Felder

Folgende Alt-Felder werden weiterhin angenommen, aber intern auf kanonische Keys umgeschrieben:

- `lapTime` -> `bestLapMs`
- `frags` -> `kills`
- `captures` -> `objectiveScore`
- `holdTime` -> `objectiveTimeMs`
- `roundState` -> `eliminationState`

`score` bleibt als Anzeige-/Sortierwert erhalten, ersetzt aber nicht mehr die modus-spezifischen Pflichtfelder.

### Breaking vs. Non-breaking

- **Breaking:** Score-only-Payloads ohne modusabhängige Pflichtfelder werden nicht mehr stillschweigend akzeptiert.
- **Non-breaking:** Legacy-Aliasse bleiben in einer Übergangsphase kompatibel und werden normalisiert verarbeitet.

### Migrationshinweise für Consumer

1. Payload-Producer (Game-Server/Adapter) auf kanonische Felder pro Modus umstellen.
2. Dashboard/Analytics nicht mehr auf universelles `score` verlassen, sondern modusabhängige Felder lesen.
3. Fehlerbehandlung auf strukturierte API-Errors (`error.code`, `error.message`, `error.details`) ausrichten.
4. Vor Go-Live E2E mit mindestens je einem Match pro Modus fahren.

### Release-Checkliste

- [ ] **Game-Server**: Exportiert pro Modus die neuen Pflichtfelder; Legacy-Aliasse nur noch fallback.
- [ ] **Python-Service**: Validierung + Normalisierung auf identische Feldregeln wie PHP-Webservice geprüft.
- [ ] **PHP-Webservice**: `version.txt`, `version.php`, UI-Changelog und README auf v1.0.8 aktualisiert.
- [ ] **Dashboard/Consumer**: Queries, KPI-Berechnung und UI-Felder auf kanonische Keys migriert.
- [ ] **E2E**: Upload/List/Detail/Delete für Racing, DM, Objective, Elimination erfolgreich getestet.
