from __future__ import annotations

import json
import shutil
import socket
import subprocess
import sys
import time
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from urllib import error as urlerror
from urllib import request

import pytest
from fastapi.testclient import TestClient
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

REPO_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO_ROOT))

from ladder_service.ladder_service import main
from ladder_service.ladder_service.db import session_scope
from ladder_service.ladder_service.models import Base
from ladder_service.ladder_service.schemas import _TEAM_MODES, _VALID_GAMETYPES


@dataclass(frozen=True)
class ModeSemantics:
    team_mode: bool
    race_mode: bool
    elimination_mode: bool
    zone_mode: bool


SEMANTIC_ORACLE: dict[str, ModeSemantics] = {
    "GT_RACING": ModeSemantics(team_mode=False, race_mode=True, elimination_mode=False, zone_mode=False),
    "GT_RACING_DM": ModeSemantics(team_mode=False, race_mode=True, elimination_mode=False, zone_mode=False),
    "GT_SPRINT": ModeSemantics(team_mode=False, race_mode=True, elimination_mode=False, zone_mode=False),
    "GT_TEAM_RACING": ModeSemantics(team_mode=True, race_mode=True, elimination_mode=False, zone_mode=False),
    "GT_TEAM_RACING_DM": ModeSemantics(team_mode=True, race_mode=True, elimination_mode=False, zone_mode=False),
    "GT_ELIMINATION": ModeSemantics(team_mode=False, race_mode=False, elimination_mode=True, zone_mode=False),
    "GT_LCS": ModeSemantics(team_mode=False, race_mode=False, elimination_mode=False, zone_mode=False),
    "GT_DERBY": ModeSemantics(team_mode=False, race_mode=False, elimination_mode=False, zone_mode=False),
    "GT_DEATHMATCH": ModeSemantics(team_mode=False, race_mode=False, elimination_mode=False, zone_mode=False),
    "GT_TEAM": ModeSemantics(team_mode=True, race_mode=False, elimination_mode=False, zone_mode=False),
    "GT_CTF": ModeSemantics(team_mode=True, race_mode=False, elimination_mode=False, zone_mode=False),
    "GT_CTF4": ModeSemantics(team_mode=True, race_mode=False, elimination_mode=False, zone_mode=False),
    "GT_DOMINATION": ModeSemantics(team_mode=True, race_mode=False, elimination_mode=False, zone_mode=True),
    "GT_SINGLE_PLAYER": ModeSemantics(team_mode=False, race_mode=False, elimination_mode=False, zone_mode=False),
}


@pytest.fixture(scope="module", autouse=True)
def override_db(tmp_path_factory: pytest.TempPathFactory) -> None:
    db_path = tmp_path_factory.mktemp("db") / "matrix.db"
    engine = create_engine(
        f"sqlite:///{db_path}", connect_args={"check_same_thread": False}
    )
    testing_session = sessionmaker(autocommit=False, autoflush=False, bind=engine)
    Base.metadata.create_all(bind=engine)

    def _override_get_session():
        with session_scope(testing_session) as session:
            yield session

    main.app.dependency_overrides[main.get_session] = _override_get_session
    yield
    main.app.dependency_overrides.pop(main.get_session, None)


client = TestClient(main.app)


def _find_free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def _http_json(method: str, url: str, payload: dict[str, Any] | None = None, headers: dict[str, str] | None = None) -> tuple[int, dict[str, Any]]:
    req_headers = {"Accept": "application/json"}
    body = None
    if payload is not None:
        body = json.dumps(payload).encode("utf-8")
        req_headers["Content-Type"] = "application/json"
    if headers:
        req_headers.update(headers)

    req = request.Request(url, data=body, headers=req_headers, method=method)
    try:
        with request.urlopen(req, timeout=5) as resp:
            data = resp.read().decode("utf-8")
            return resp.status, json.loads(data) if data else {}
    except urlerror.HTTPError as exc:
        data = exc.read().decode("utf-8")
        parsed = json.loads(data) if data else {}
        return exc.code, parsed


@pytest.fixture(scope="module")
def php_env(tmp_path_factory: pytest.TempPathFactory) -> dict[str, Any]:
    php_bin = shutil.which("php")
    if not php_bin:
        pytest.skip("php executable not available in environment")

    src_dir = REPO_ROOT / "ladder_service" / "php_webservice"
    run_dir = tmp_path_factory.mktemp("php_webservice") / "php_webservice"
    shutil.copytree(src_dir, run_dir)

    data_dir = run_dir / "data"
    data_dir.mkdir(exist_ok=True)

    port = _find_free_port()
    process = subprocess.Popen(
        [php_bin, "-S", f"127.0.0.1:{port}", "-t", str(run_dir)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    base_url = f"http://127.0.0.1:{port}/index.php"
    started = False
    for _ in range(30):
        try:
            with request.urlopen(request.Request(base_url, headers={"Accept": "text/html"}), timeout=2) as resp:
                if resp.status == 200:
                    started = True
                    break
        except Exception:
            pass
        time.sleep(0.2)

    if not started:
        stderr = process.stderr.read() if process.stderr else ""
        process.terminate()
        raise RuntimeError(f"PHP test server did not start. stderr={stderr}")

    status, reg = _http_json(
        "POST",
        f"{base_url}/api/v1/register",
        {
            "serverName": "Q3Rally Matrix Test",
            "ownerName": "CI",
            "ownerEmail": "ci@example.org",
            "type": "offline",
        },
    )
    assert status == 201, reg

    yield {
        "base_url": base_url,
        "auth": {"Authorization": f"Bearer {reg['key']}"},
        "process": process,
        "run_dir": run_dir,
    }

    process.terminate()
    process.wait(timeout=5)


def generate_golden_payload(mode: str) -> dict[str, Any]:
    now = datetime(2026, 4, 13, 12, 0, 0, tzinfo=timezone.utc)
    payload: dict[str, Any] = {
        "matchId": f"matrix-{mode.lower()}",
        "mode": mode,
        "startTime": now.isoformat(),
        "endTime": now.isoformat(),
        "duration": "PT10M",
        "startEpoch": int(now.timestamp()),
        "map": "q3r_country01",
        "server": {
            "name": "Q3Rally Matrix Test",
            "host": "127.0.0.1:27960",
            "build": "test",
            "dedicated": 1,
        },
        "settings": {"g_gametype": 999},
        "players": [
            {
                "playerId": "p1",
                "displayName": "Golden",
                "team": "red",
                "score": 42,
                "rawScore": 42,
                "kills": 7,
                "deaths": 1,
                "bestLapMs": 1111,
                "lapCount": 2,
                "lapTimes": [1111, 1200],
                "position": 1,
                "zoneHoldMs": 444,
                "zoneActiveSigil": 2,
                "eliminationRound": 3,
                "eliminationPlayersRemaining": 2,
            }
        ],
    }

    if mode in _TEAM_MODES:
        payload["teams"] = [
            {"team": "red", "rawScore": 10},
            {"team": "blue", "rawScore": 5},
        ]
    else:
        payload["teams"] = None
        payload["players"][0]["team"] = None

    return payload


def _assert_semantics_in_php_stored_payload(mode: str, stored: dict[str, Any]) -> None:
    semantics = SEMANTIC_ORACLE[mode]
    player = stored["players"][0]

    if semantics.race_mode:
        assert player["kills"] == 0
        assert player["deaths"] == 0

    if not semantics.race_mode and not semantics.elimination_mode:
        assert player["bestLapMs"] == 0
        assert player["lapCount"] == 0
        assert player["lapTimes"] == []

    if not semantics.elimination_mode:
        assert player["eliminationRound"] == 0
        assert player["eliminationPlayersRemaining"] == 0

    if not semantics.zone_mode:
        assert player["zoneHoldMs"] == 0
        assert player["zoneActiveSigil"] == -1


def _make_negative_payload(mode: str) -> dict[str, Any]:
    payload = generate_golden_payload(mode)
    if SEMANTIC_ORACLE[mode].team_mode:
        payload["teams"] = None
    else:
        payload["teams"] = [{"team": "red", "rawScore": 1}]
    return payload


@pytest.mark.parametrize("mode", sorted(_VALID_GAMETYPES))
def test_mode_matrix_golden_payloads_and_negative_cases(mode: str, php_env: dict[str, Any]) -> None:
    assert mode in SEMANTIC_ORACLE, f"{mode} missing in semantic oracle"

    payload = generate_golden_payload(mode)

    # Game-generator produces a complete payload for every mode.
    assert payload["mode"] == mode
    assert payload["matchId"].startswith("matrix-")
    assert payload["players"]

    # Python service: accept valid payload, then semantic checks.
    py_create = client.post("/api/v1/matches", json=payload)
    assert py_create.status_code == 201, f"{mode}: {py_create.text}"

    py_stored = client.get(f"/api/v1/matches/{payload['matchId']}")
    assert py_stored.status_code == 200, f"{mode}: {py_stored.text}"
    py_data = py_stored.json()

    if SEMANTIC_ORACLE[mode].team_mode:
        assert py_data["teams"] is not None
        assert py_data["players"][0]["team"] == "red"
    else:
        assert py_data["teams"] is None
        assert py_data["players"][0]["team"] is None

    py_negative = client.post("/api/v1/matches", json={**_make_negative_payload(mode), "matchId": f"neg-{mode.lower()}"})
    if SEMANTIC_ORACLE[mode].team_mode:
        assert py_negative.status_code == 422
        assert "MODE_REQUIRES_TEAMS" in py_negative.json()["validation"]["errorCodes"]
    else:
        assert py_negative.status_code == 422
        assert "MODE_FORBIDS_TEAMS" in py_negative.json()["validation"]["errorCodes"]

    # PHP service: accept payload, store it, and normalize mode-specific semantics.
    php_status, php_create = _http_json(
        "POST",
        f"{php_env['base_url']}/api/v1/matches",
        payload,
        headers=php_env["auth"],
    )
    assert php_status == 201, f"{mode}: {php_create}"

    php_get_status, php_data = _http_json(
        "GET",
        f"{php_env['base_url']}/api/v1/matches/{payload['matchId']}",
        headers=php_env["auth"],
    )
    assert php_get_status == 200, f"{mode}: {php_data}"
    _assert_semantics_in_php_stored_payload(mode, php_data)


def test_mode_matrix_human_readable_report() -> None:
    rows = []
    for mode in sorted(_VALID_GAMETYPES):
        s = SEMANTIC_ORACLE[mode]
        rows.append(
            f"- {mode}: team={int(s.team_mode)} race={int(s.race_mode)} elimination={int(s.elimination_mode)} zone={int(s.zone_mode)}"
        )
    report = "\n".join(rows)
    assert "GT_RACING" in report
    assert "GT_TEAM" in report
