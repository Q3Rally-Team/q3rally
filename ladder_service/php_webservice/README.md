# PHP Ladder Webservice

Dieses Verzeichnis enthält den PHP-basierten Q3Rally-Ladder-Endpunkt für
klassischen Webspace mit PHP-Unterstützung. Der Service benötigt keine
zusätzlichen Bibliotheken und speichert eingehende Matches als JSON-Dateien im
Unterordner `data/`.

Die verbindliche Payload-Semantik liegt zentral in
[`../../docs/ladder_payload_semantics.md`](../../docs/ladder_payload_semantics.md).
Die aktuelle Contract-Version ist **v1.0.8** (Release-Datum **2026-04-13**).

## Deployment

1. Den gesamten Inhalt dieses Ordners auf den gewünschten Webspace hochladen.
2. Sicherstellen, dass PHP 8.0 oder neuer aktiviert ist.
3. Dem Webserver Schreibrechte für `data/` geben.
4. Optional `register.php` und `admin.php` für Server-Key-Registrierung und Administration konfigurieren.

Nach dem Upload ist die Oberfläche unter der Basis-URL erreichbar, zum Beispiel
`https://example.com/ladder/index.php`.

> Viele Hoster setzen `index.php` automatisch als Startdatei. Liegt der Ordner
> direkt im Document-Root, kann die Basis-URL auch `https://example.com/` sein.

## API-Übersicht

Die aktuellen Beispiele verwenden `/api/v1`. Der Router akzeptiert auch die
kurzen Pfade ohne Prefix, `/api/v1` ist aber die empfohlene Form.

| Methode | Pfad | Auth | Beschreibung |
| --- | --- | --- | --- |
| `POST` | `/api/v1/register` | nein | Registriert einen Server-Key-Antrag. |
| `POST` | `/api/v1/matches` | Bearer-Key | Speichert ein Match. Bereits vorhandene IDs werden idempotent mit HTTP 200 quittiert. |
| `GET` | `/api/v1/matches` | Bearer-Key | Liefert gespeicherte Matches, optional mit `mode`, `limit` und `offset`. |
| `GET` | `/api/v1/matches/{matchId}` | nein | Gibt das öffentliche Match-JSON zu einer Match-ID zurück. |
| `GET` | `/api/v1/matches/index` | nein | Liefert den kompakten Frontend-Index. |
| `GET` | `/api/v1/players/{playerId}` | nein | Liefert ein öffentliches Spielerprofil. |
| `DELETE` | `/api/v1/matches/{matchId}` | Bearer-Key | Löscht ein Match dauerhaft. |

## Beispiel-Aufrufe

```bash
# Match speichern
curl -X POST https://example.com/ladder/index.php/api/v1/matches \
     -H "Authorization: Bearer $LADDER_API_KEY" \
     -H "Content-Type: application/json" \
     -d @match.json

# Letzte Matches anzeigen
curl https://example.com/ladder/index.php/api/v1/matches?limit=10 \
     -H "Authorization: Bearer $LADDER_API_KEY"

# Einzelnes Match abrufen
curl https://example.com/ladder/index.php/api/v1/matches/srv-20240405-183011-42

# Match löschen
curl -X DELETE https://example.com/ladder/index.php/api/v1/matches/srv-20240405-183011-42 \
     -H "Authorization: Bearer $LADDER_API_KEY"
```

## Datenablage

Jedes Match wird als einzelne JSON-Datei unter `data/<matchId>.json` abgelegt.
Der Service ergänzt automatisch `receivedAt`, damit Listen sortiert werden
können. Der schnelle Frontend-Index und Spielerprofile werden ebenfalls unter
`data/` gepflegt.

## Backup & Wartung

* Regelmäßig den Ordner `data/` sichern.
* Nach manuellen Datenänderungen kann `php rebuild_index.php` den Match-Index neu aufbauen.
* Bei Upgrades der Profilstruktur kann `php migrate_profiles.php` bestehende Profile migrieren.
* Bei sehr vielen Matches empfiehlt sich langfristig eine Datenbank-basierte Ablage.

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

Ältere Payload-Varianten werden in einer Übergangsphase normalisiert
verarbeitet, sofern sie eindeutig auf die kanonische Semantik abbildbar sind.
Neue Producer sollten sich direkt an
[`../../docs/ladder_payload_semantics.md`](../../docs/ladder_payload_semantics.md)
orientieren.
