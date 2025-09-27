from __future__ import annotations

from datetime import datetime, timezone

import pytest
from fastapi.testclient import TestClient
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

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
    "map": "q3r_country01",
    "server": {"name": "Q3Rally EU #1", "host": "203.0.113.10:27960", "build": "1.3.0"},
    "settings": {"g_gametype": 141},
    "players": [
        {
            "playerId": "sha256:abc",
            "displayName": "PlayerOne",
            "team": "red",
            "rawScore": 123,
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


def test_list_matches() -> None:
    response = client.get("/api/v1/matches?limit=10")
    assert response.status_code == 200
    data = response.json()
    assert len(data["matches"]) >= 1


def test_delete_match() -> None:
    response = client.delete(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert response.status_code == 204
    follow_up = client.get(f"/api/v1/matches/{MATCH_TEMPLATE['matchId']}")
    assert follow_up.status_code == 404
