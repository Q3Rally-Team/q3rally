from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
import sys

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


@pytest.fixture(scope="module", autouse=True)
def override_db(tmp_path_factory: pytest.TempPathFactory) -> None:
    db_path = tmp_path_factory.mktemp("db") / "test.db"
    engine = create_engine(
        f"sqlite:///{db_path}", connect_args={"check_same_thread": False}
    )
    TestingSession = sessionmaker(autocommit=False, autoflush=False, bind=engine)
    Base.metadata.create_all(bind=engine)

    def _override_get_session():
        with session_scope(TestingSession) as session:
            yield session

    main.app.dependency_overrides[main.get_session] = _override_get_session
    yield
    main.app.dependency_overrides.pop(main.get_session, None)


client = TestClient(main.app)


MATCH_TEMPLATE = {
    "matchId": "srv-20240405-183011-42",
    "mode": "GT_RACING",
    "startTime": datetime(2024, 4, 5, 18, 30, 11, tzinfo=timezone.utc).isoformat(),
    "endTime": datetime(2024, 4, 5, 18, 42, 39, tzinfo=timezone.utc).isoformat(),
    "duration": "PT12M28S",
    "startEpoch": 1712332211,
    "map": "q3r_country01",
    "server": {"name": "Q3Rally EU #1", "host": "203.0.113.10:27960", "build": "1.3.0"},
    "settings": {"g_gametype": 141},
    "players": [
        {
            "playerId": "sha256:abc",
            "displayName": "PlayerOne",
            "team": "red",
            "score": 123,
        }
    ],
}


def _build_match(match_id: str, mode: str) -> dict[str, object]:
    payload = {
        **MATCH_TEMPLATE,
        "matchId": match_id,
        "mode": mode,
    }
    if mode in _TEAM_MODES:
        payload["teams"] = [{"team": "red", "rawScore": 10}, {"team": "blue", "rawScore": 8}]
        payload["players"] = [{**MATCH_TEMPLATE["players"][0], "team": "red"}]
    else:
        payload["teams"] = None
        payload["players"] = [{**MATCH_TEMPLATE["players"][0], "team": None}]
    return payload


def test_create_match() -> None:
    response = client.post("/api/v1/matches", json=MATCH_TEMPLATE)
    assert response.status_code == 201, response.text
    assert response.json() == {"matchId": MATCH_TEMPLATE["matchId"]}


def test_get_match() -> None:
    response = client.get(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert response.status_code == 200
    data = response.json()
    assert data["matchId"] == MATCH_TEMPLATE["matchId"]
    assert "createdAt" in data
    assert data["startEpoch"] == MATCH_TEMPLATE["startEpoch"]
    assert data["players"][0]["rawScore"] == MATCH_TEMPLATE["players"][0]["score"]
    assert data["players"][0]["score"] == MATCH_TEMPLATE["players"][0]["score"]


def test_list_matches() -> None:
    response = client.get("/api/v1/matches?limit=10")
    assert response.status_code == 200
    data = response.json()
    assert len(data["matches"]) >= 1


def test_list_matches_filter_mode() -> None:
    alt_match = {
        **MATCH_TEMPLATE,
        "matchId": "srv-20240405-183011-43",
        "mode": "ARCADE_RACING",
    }
    created = client.post("/api/v1/matches", json=alt_match)
    assert created.status_code == 201, created.text

    response = client.get("/api/v1/matches?mode=GT_RACING")
    assert response.status_code == 200, response.text
    data = response.json()
    assert data["matches"], "Expected at least one GT_RACING match in response"
    for match in data["matches"]:
        assert match["mode"] == "GT_RACING"
        assert match["matchId"] != alt_match["matchId"]

    cleanup = client.delete(f"/api/v1/matches/{alt_match['matchId']}")
    assert cleanup.status_code == 204


def test_list_matches_supports_team_race_dm_mode() -> None:
    match = _build_match("srv-20240405-183011-44", "GT_TEAM_RACING_DM")
    created = client.post("/api/v1/matches", json=match)
    assert created.status_code == 201, created.text

    response = client.get("/api/v1/matches?mode=GT_TEAM_RACING_DM")
    assert response.status_code == 200, response.text
    data = response.json()
    assert any(entry["matchId"] == match["matchId"] for entry in data["matches"])

    cleanup = client.delete(f"/api/v1/matches/{match['matchId']}")
    assert cleanup.status_code == 204


def test_create_match_accepts_sprint_mode() -> None:
    match = {
        **MATCH_TEMPLATE,
        "matchId": "srv-20240405-183011-45",
        "mode": "sprint",
        "settings": {"g_gametype": 145},
    }

    response = client.post("/api/v1/matches", json=match)
    assert response.status_code == 201, response.text

    stored = client.get(f"/api/v1/matches/{match['matchId']}")
    assert stored.status_code == 200, stored.text
    payload = stored.json()
    assert payload["mode"] == "GT_SPRINT"

    cleanup = client.delete(f"/api/v1/matches/{match['matchId']}")
    assert cleanup.status_code == 204


def test_mode_forbids_teams_with_validation_codes() -> None:
    match = _build_match("srv-20240405-183011-46", "GT_RACING")
    match["teams"] = [{"team": "red", "rawScore": 1}]

    response = client.post("/api/v1/matches", json=match)
    assert response.status_code == 422, response.text
    data = response.json()
    assert "MODE_FORBIDS_TEAMS" in data["validation"]["errorCodes"]
    assert any(item["errorCode"] == "MODE_FORBIDS_TEAMS" for item in data["detail"])


def test_team_mode_requires_teams() -> None:
    match = _build_match("srv-20240405-183011-47", "GT_TEAM")
    match["teams"] = None

    response = client.post("/api/v1/matches", json=match)
    assert response.status_code == 422, response.text
    assert "MODE_REQUIRES_TEAMS" in response.json()["validation"]["errorCodes"]


def test_team_mode_rejects_unknown_player_team() -> None:
    match = _build_match("srv-20240405-183011-48", "GT_CTF")
    match["players"][0]["team"] = "spectator"

    response = client.post("/api/v1/matches", json=match)
    assert response.status_code == 422, response.text
    assert "PLAYER_TEAM_UNKNOWN" in response.json()["validation"]["errorCodes"]


def test_non_team_mode_sanitizes_player_team() -> None:
    match = _build_match("srv-20240405-183011-49", "GT_ELIMINATION")
    match["players"][0]["team"] = "red"

    response = client.post("/api/v1/matches", json=match)
    assert response.status_code == 201, response.text

    stored = client.get(f"/api/v1/matches/{match['matchId']}")
    assert stored.status_code == 200, stored.text
    data = stored.json()
    assert data["players"][0]["team"] is None
    assert data["teams"] is None

    cleanup = client.delete(f"/api/v1/matches/{match['matchId']}")
    assert cleanup.status_code == 204


@pytest.mark.parametrize("mode", sorted(_VALID_GAMETYPES))
def test_all_modes_accept_mode_specific_payload(mode: str) -> None:
    match_id = f"srv-20240405-183011-{mode.lower()}"
    match = _build_match(match_id, mode)

    created = client.post("/api/v1/matches", json=match)
    assert created.status_code == 201, f"{mode}: {created.text}"

    stored = client.get(f"/api/v1/matches/{match_id}")
    assert stored.status_code == 200, f"{mode}: {stored.text}"
    payload = stored.json()
    if mode in _TEAM_MODES:
        assert payload["teams"] is not None
        assert payload["players"][0]["team"] == "red"
    else:
        assert payload["teams"] is None
        assert payload["players"][0]["team"] is None

    cleanup = client.delete(f"/api/v1/matches/{match_id}")
    assert cleanup.status_code == 204


def test_delete_match() -> None:
    response = client.delete(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert response.status_code == 204
    follow_up = client.get(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert follow_up.status_code == 404
