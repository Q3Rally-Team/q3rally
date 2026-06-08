# Q3Rally Ladder Service

Ein leichtgewichtiger FastAPI-Webservice für Q3Rally-Matchberichte. Der Service
nimmt Matches über `POST /api/v1/matches` entgegen, speichert die Payloads in
SQLite und stellt sie über Listen- und Detail-Endpunkte wieder bereit.

Die verbindliche Payload-Semantik ist in
[`../docs/ladder_payload_semantics.md`](../docs/ladder_payload_semantics.md)
dokumentiert. Die automatisierte Modusmatrix steht in
[`../docs/e2e_mode_test_matrix.md`](../docs/e2e_mode_test_matrix.md).

## Anforderungen

* Python 3.11
* Abhängigkeiten aus `requirements.txt`

Installation, idealerweise in einem virtuellen Umfeld:

```bash
cd ladder_service
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

Unter Windows kann die virtuelle Umgebung mit `.venv\Scripts\activate`
aktiviert werden.

## Entwicklung starten

```bash
uvicorn ladder_service.main:app --reload
```

Standardmäßig wird `ladder.db` im Arbeitsverzeichnis genutzt. Über
`LADDER_DB_PATH` kann ein alternativer SQLite-Pfad gesetzt werden.

## API-Version

Aktueller Contract-Stand: **v1.0.8** (Release-Datum **2026-04-13**).

| Methode | Pfad | Beschreibung |
| --- | --- | --- |
| `POST` | `/api/v1/matches` | Legt ein neues Match ab. Gibt bei Erfolg `{ "matchId": "..." }` zurück. |
| `GET` | `/api/v1/matches` | Listet Matches, optional gefiltert nach `mode`, mit Pagination über `limit`/`offset`. |
| `GET` | `/api/v1/matches/{matchId}` | Liefert die ursprünglich gesendete Payload plus `createdAt`. |
| `DELETE` | `/api/v1/matches/{matchId}` | Entfernt ein Match. |

## Tests

```bash
pytest
```

Gezielte E2E-Modusmatrix:

```bash
pytest tests/test_mode_e2e_matrix.py -q
```

Der Testlauf deckt die unterstützten Modi mit Golden-Payloads, Negativfällen
und Python/PHP-End-to-End-Prüfung ab.
