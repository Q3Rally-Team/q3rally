# Q3Rally Ladder Service

Ein leichtgewichtiger Webservice, der die in `docs/ladder-service.md` beschriebene REST-Schnittstelle bereitstellt. Er sammelt Matches über `POST /api/v1/matches`, speichert die Nutzdaten in einer SQLite-Datenbank und stellt Abfragen über `GET /api/v1/matches` sowie `GET /api/v1/matches/{matchId}` bereit.

## Anforderungen

* Python 3.11
* Abhängigkeiten aus `requirements.txt`

Installation (am besten in einem virtuellen Umfeld):

```bash
cd ladder_service
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## Entwicklung starten

```bash
uvicorn ladder_service.main:app --reload
```

Standardmäßig wird eine Datei `ladder.db` im Arbeitsverzeichnis genutzt. Über die Umgebungsvariable `LADDER_DB_PATH` kann ein alternativer Pfad vorgegeben werden.

## API-/Contract-Version

Aktueller Contract-Stand: **v1.0.8** (Release-Datum **2026-04-13**). Der Stand beschreibt mode-aware Pflichtfelder pro Matchmodus inklusive Legacy-Alias-Normalisierung und strukturierter Fehlerantworten.

## Endpunkte

| Methode | Pfad | Beschreibung |
| --- | --- | --- |
| `POST` | `/api/v1/matches` | Legt ein neues Match ab. Gibt bei Erfolg `{ "matchId": "…" }` zurück. |
| `GET` | `/api/v1/matches` | Listet Matches, optional gefiltert nach `mode`, mit Pagination über `limit`/`offset`. |
| `GET` | `/api/v1/matches/{matchId}` | Liefert die ursprünglich gesendeten Daten plus `createdAt`-Zeitstempel. |
| `DELETE` | `/api/v1/matches/{matchId}` | Entfernt ein Match. |

## Tests

```bash
pytest
```

## E2E-Modusmatrix-Tests

```bash
pytest tests/test_mode_e2e_matrix.py -q
```

Der Testlauf deckt alle unterstützten Modi mit Golden-Payloads, Negativfällen und Python/PHP-End-to-End-Prüfung ab.

