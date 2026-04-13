# E2E-Testmatrix pro Modus

Diese Matrix dient als Test-Orakel für `ladder_service/tests/test_mode_e2e_matrix.py`.
Sie ist auf der Soll-Semantik aus `docs/ladder_payload_semantics.md` aufgebaut.

## Modus-Matrix (Soll-Semantik)

| Modus | Team-Payload erlaubt/pflichtig | Race-Semantik aktiv | Elimination-Semantik aktiv | Zone-Hold-Semantik aktiv | Negativtest (absichtlich falsch) |
|---|---|---:|---:|---:|---|
| GT_RACING | verboten | ✅ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_RACING_DM | verboten | ✅ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_SPRINT | verboten | ✅ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_TEAM_RACING | pflichtig | ✅ | ❌ | ❌ | `teams=null` (muss Python-seitig reject) |
| GT_TEAM_RACING_DM | pflichtig | ✅ | ❌ | ❌ | `teams=null` (muss Python-seitig reject) |
| GT_ELIMINATION | verboten | ❌ | ✅ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_LCS | verboten | ❌ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_DERBY | verboten | ❌ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_DEATHMATCH | verboten | ❌ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |
| GT_TEAM | pflichtig | ❌ | ❌ | ❌ | `teams=null` (muss Python-seitig reject) |
| GT_CTF | pflichtig | ❌ | ❌ | ❌ | `teams=null` (muss Python-seitig reject) |
| GT_CTF4 | pflichtig | ❌ | ❌ | ❌ | `teams=null` (muss Python-seitig reject) |
| GT_DOMINATION | pflichtig | ❌ | ❌ | ✅ | `teams=null` (muss Python-seitig reject) |
| GT_SINGLE_PLAYER | verboten | ❌ | ❌ | ❌ | `teams` gesetzt (muss Python-seitig reject) |

## Golden-Payloads

Für **jeden Modus** erzeugt der Testlauf automatisch einen Golden-Payload über den Game-Generator
(`generate_golden_payload(mode)`) mit:

- kanonischem `mode`
- stabiler `matchId`
- mindestens einem Player
- absichtlich reichhaltigen Feldern (`kills`, `deaths`, `bestLapMs`, `lapTimes`, `zoneHoldMs`, `eliminationRound`),
  damit modusspezifische Neutralisierung/Übernahme serverseitig überprüft werden kann.

## Automatisierte Prüfungen pro Modus

1. **Game-Generator** erzeugt Golden-Payload.
2. **Python-Service** (`POST/GET /api/v1/matches`) akzeptiert valide Payloads und rejected den definierten Negativfall.
3. **PHP-Webservice** (`POST/GET /api/v1/matches`) verarbeitet und speichert den Golden-Payload, inklusive
   modusspezifischer Feld-Normalisierung.
4. **Orakel-Vergleich** gegen diese Matrix:
   - race: `kills/deaths` neutralisiert,
   - non-race+non-elimination: `bestLapMs/lapCount/lapTimes` neutralisiert,
   - non-elimination: `elimination*` neutralisiert,
   - non-zone: `zoneHoldMs/zoneActiveSigil` neutralisiert.

## CI-geeignete Ausführung

```bash
pytest ladder_service/tests/test_mode_e2e_matrix.py -q
```

Der Test ist vollständig automatisiert und gibt eine kompakte, human-readable Modus-Zusammenfassung aus.
