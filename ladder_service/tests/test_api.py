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
    match = {
        **MATCH_TEMPLATE,
        "matchId": "srv-20240405-183011-44",
        "mode": "GT_TEAM_RACING_DM",
    }
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


def test_delete_match() -> None:
    response = client.delete(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert response.status_code == 204
    follow_up = client.get(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert follow_up.status_code == 404
