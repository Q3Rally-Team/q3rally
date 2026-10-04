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
5. `data/` darf nicht direkt per HTTP erreichbar sein. Für Apache liegt
   `data/.htaccess` bei, die Server-Keys liegen zusätzlich in `data/private/`
   (eigene `.htaccess`). Unter nginx stattdessen:

   ```nginx
   location ^~ /data/ { deny all; return 404; }
   ```

   Prüfen: `curl -I https://example.com/data/match_index.json` muss 403 oder 404 liefern.

### Server-Keys

Die Keys liegen ab 1.0.13 in `data/private/server_keys.json`. Eine vorhandene
`data/server_keys.json` wird beim ersten Request automatisch dorthin
verschoben. Jede Änderung läuft unter einer exklusiven Sperre und ersetzt die
Datei atomar. Neue Keys (Formular, In-Game-Assistent, `POST /api/v1/register`)
sind immer Anträge im Status `pending`, bis sie in `admin.php` freigegeben werden;
pro IP sind 10 Anträge pro Stunde erlaubt.

Offline-Keys (Typ `offline` oder Servername mit `_OFFLINE`) melden Matches und
Ghosts immer als `offline`, unabhängig von `server.dedicated`.

Ein Offline-Key gehört einem Spieler (dem Profil, das ihn im Spiel registriert
hat). Der erste Upload mit genau einem menschlichen Spieler bindet den Key an
dessen Spieler-ID; eine ID kann nur an einen Offline-Key gebunden sein. Danach
zählen Matches dieses Keys nur für diesen Spieler (andere Menschen bleiben ohne
Profil-Gutschrift im Match), Ghosts anderer Spieler werden mit 403
`GHOST_PLAYER_MISMATCH` abgelehnt. In `admin.php` steht die Bindung beim Key,
"Unbind player" löst sie (z. B. nach Neuinstallation).

### Spieler-IDs zusammenführen

Bekommt ein Spieler im Spiel eine neue Profil-ID, führt die Ladder zwei Profile.
`php merge_player.php <alteId> <neueId>` zeigt, was sich ändern würde; mit
`--apply` werden Matches, Ghosts (pro Bucket bleibt der schnellere), die
Offline-Key-Bindung und das Profil auf die neue ID umgestellt und Profil und
Index neu aufgebaut. Vorher wird alles Geänderte nach `data/private/merge-<Zeit>/`
kopiert. Kommandozeilen-Tools als Webserver-User starten
(`sudo -u www-data php …`), sonst gehören neu geschriebene Dateien root.

### Schreibzugriffe und Limits

Uploads und Löschungen von Matches laufen unter einer gemeinsamen Sperre
(`data/ladder_write.lock`); Match-Dateien, `match_index.json`, Profile und
Ghost-Dateien werden atomar geschrieben (temporäre Datei + rename).
POST-Limit: 30 pro Minute und IP für Spieler und unbekannte Keys, 120 pro
Minute je freigegebenem Server-Key. Die Zähler liegen in `data/private/rl_*`;
alte `data/rl_*.json` aus früheren Versionen können gelöscht werden.

### Admin-Oberfläche

`admin.php` braucht `LADDER_ADMIN_PASSWORD` (Umgebungsvariable). Formulare
tragen ein Sitzungs-Token (CSRF), die Sitzung bekommt nach dem Login eine neue
ID, das Cookie ist HttpOnly und SameSite=Strict. Fehlversuche beim Login: 5 pro
IP und 30 insgesamt in 15 Minuten. Die Seite zeigt und sendet nur eine kurze
Key-ID, nie den Key selbst.

Hinter Cloudflare sieht PHP ohne `mod_remoteip` nur Cloudflare-Adressen
(`REMOTE_ADDR`). Dann gelten die IP-Grenzen pro Cloudflare-Knoten, und
`lastUsedIp` zeigt Cloudflare. Mit `mod_remoteip` und
`RemoteIPHeader CF-Connecting-IP` (nur für die Cloudflare-Bereiche als
`RemoteIPTrustedProxy`) bekommt PHP die echte Adresse.

Nach dem Upload ist die Oberfläche unter der Basis-URL erreichbar, zum Beispiel
`https://example.com/ladder/index.php`.

> Viele Hoster setzen `index.php` automatisch als Startdatei. Liegt der Ordner
> direkt im Document-Root, kann die Basis-URL auch `https://example.com/` sein.

## API-Übersicht

Die aktuellen Beispiele verwenden `/api/v1`. Der Router akzeptiert auch die
kurzen Pfade ohne Prefix, `/api/v1` ist aber die empfohlene Form.

| Methode | Pfad | Auth | Beschreibung |
| --- | --- | --- | --- |
| `POST` | `/api/v1/register` | nein | Registriert einen Server-Key-Antrag (immer `pending`, Freigabe in `admin.php`). |
| `POST` | `/api/v1/matches` | Bearer-Key | Speichert ein Match. Bereits vorhandene IDs werden idempotent mit HTTP 200 quittiert. |
| `GET` | `/api/v1/matches` | Bearer-Key | Liefert gespeicherte Matches, optional mit `mode`, `limit` und `offset`. |
| `GET` | `/api/v1/matches/{matchId}` | nein | Gibt das öffentliche Match-JSON zu einer Match-ID zurück. |
| `GET` | `/api/v1/matches/index` | nein | Liefert den kompakten Frontend-Index. |
| `GET` | `/api/v1/players/{playerId}` | nein | Liefert ein öffentliches Spielerprofil. |
| `POST` | `/api/v1/ghosts` | Bearer-Key | Speichert einen Runden-Ghost (nur wenn schneller als der bisherige Ghost des Spielers). |
| `GET` | `/api/v1/ghosts?map=&tl=&rev=&vehicle=&physics=&checksum=&limit=&perVehicle=&format=` | nein | Rangliste der Ghosts einer Strecken-Variante, ohne Ghost-Daten. `perVehicle=K`: beste K je Fahrzeug; `format=text`: eine Zeile je Ghost (`ghostId`, `lapMs`, Fahrzeug, Name, tab-getrennt) für die Spiel-Engine. |
| `GET` | `/api/v1/ghosts/catalog` | nein | Übersicht für die Ranglisten-Seite: Maps, Streckenvarianten, Map-Versionen (Physik + Prüfsumme, aktuelle zuerst) und Fahrzeuge mit Anzahl. |
| `GET` | `/api/v1/ghosts/{ghostId}` | nein | Ein Ghost inkl. Daten; `?format=raw` liefert die `.ghost`-Datei als Text. |
| `DELETE` | `/api/v1/matches/{matchId}` | Bearer-Key | Löscht ein Match dauerhaft. Nur mit dem Key, der das Match gemeldet hat; Offline-Keys nie. Matches vor 1.0.13 nur direkt auf dem Server. |

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

## Runden-Ghosts

Spielserver (dedizierte Server und registrierte Offline-Clients) zeichnen die Runden
ihrer Fahrer selbst auf und melden eine Runde, wenn sie die beste der Sitzung ist.
`ghosts.php` speichert pro Spieler den besten Ghost je

* Map und Strecken-Variante (`tl0..2`, `rev0/1`)
* Fahrzeug
* Physik-Version (`BG_PHYSICS_VERSION` im Spiel) und BSP-Prüfsumme der Map

Ghosts anderer Physik-Versionen oder Map-Stände landen in eigenen Buckets und werden
nicht gegeneinander gewertet. Ablage:
`data/ghosts/<map>/tl<n>_rev<r>/<fahrzeug>/p<physik>_c<prüfsumme>/` mit `index.json`
und einer Datei pro Spieler. Pro Bucket bleiben höchstens 200 Ghosts erhalten.

Plausibilitätsprüfungen beim Upload (HTTP 422 bei Verstoß):

* Kopfzeilen der Ghost-Daten passen zu den Metadaten (Map, Fahrzeug, Variante, Zeit, Frames)
* erster Messpunkt am Rundenstart, letzter bei der Rundenzeit
* keine Lücken über 5 s, keine Sprünge (Segmentgeschwindigkeit über 6000 Units/s)
* Durchschnittsgeschwindigkeit plausibel, gefahrene Strecke 0,6- bis 4-fach der Kurslänge

Uploads zählen beim Server-Key als `ghostCount`, nicht als `matchCount`.

Die Startseite hat einen Reiter **Ghosts** mit den schnellsten Runden-Ghosts je Map,
Streckenvariante (Kurz/Mittel/Lang, vorwärts/rückwärts), Fahrzeug und Map-Version,
inklusive Download der `.ghost`-Datei. Gibt es zu einer Variante mehrere Map-Versionen
oder Physik-Stände, ist die aktuelle vorausgewählt (höchste Physik-Version, dann die
Version, deren erster Ghost am jüngsten ist).

Im Spielmodus Ghost Race holt der Spielserver die Rangliste mit
`?map=…&tl=…&rev=…&physics=…&checksum=…&perVehicle=5&limit=100&format=text`
und einzelne Ghosts mit `/ghosts/{ghostId}?format=raw` (ohne Key). Beides legt er
unter `baseq3r/ghosts/ladder/` ab und nutzt den Cache, wenn die Ladder nicht
erreichbar ist.

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
