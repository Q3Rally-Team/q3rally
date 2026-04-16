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


def _post_php_match(php_env: dict[str, Any], payload: dict[str, Any]) -> tuple[int, dict[str, Any]]:
    return _http_json(
        "POST",
        f"{php_env['base_url']}/api/v1/matches",
        payload,
        headers=php_env["auth"],
    )


def _get_php_profile(php_env: dict[str, Any], player_id: str) -> dict[str, Any]:
    status, data = _http_json(
        "GET",
        f"{php_env['base_url']}/api/v1/players/{player_id}",
        headers=php_env["auth"],
    )
    assert status == 200, data
    return data


def _assert_profile_fields(profile: dict[str, Any], expected: dict[str, int]) -> None:
    for key, value in expected.items():
        assert profile[key] == value, f"{key}: expected {value}, got {profile[key]}"


def _profile_payload(match_id: str, mode: str, player_id: str) -> dict[str, Any]:
    payload = generate_golden_payload(mode)
    payload["matchId"] = match_id
    payload["winnerClientNum"] = 1
    payload["settings"]["winnerClientNum"] = 1
    payload["players"][0].update(
        {
            "playerId": player_id,
            "clientNum": 1,
            "displayName": "ProfileTest",
            "kills": 7,
            "deaths": 2,
            "position": 1,
            "totalRaceMs": 77777,
            "team": None,
        }
    )
    payload["teams"] = None
    return payload


def test_php_profile_dm_single_match_winner_sets_win_loss(php_env: dict[str, Any]) -> None:
    winner_id = "48b8e564-3e47-40a8-95ee-9611f88e6df0"
    loser_id = "bb005b6a-c280-4dbf-b64e-8c67e058af00"
    payload = _profile_payload("profile-dm-win-loss-1", "GT_DEATHMATCH", winner_id)
    payload["players"] = [
        {
            "playerId": winner_id,
            "clientNum": 1,
            "displayName": "DMWinner",
            "team": None,
            "kills": 12,
            "deaths": 4,
            "position": 1,
            "totalRaceMs": 0,
        },
        {
            "playerId": loser_id,
            "clientNum": 2,
            "displayName": "DMLoser",
            "team": None,
            "kills": 4,
            "deaths": 12,
            "position": 2,
            "totalRaceMs": 0,
        },
    ]

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    winner_profile = _get_php_profile(php_env, winner_id)
    loser_profile = _get_php_profile(php_env, loser_id)

    _assert_profile_fields(winner_profile, {"wins": 1, "losses": 0, "dmWins": 1, "dmCompleted": 1})
    _assert_profile_fields(loser_profile, {"wins": 0, "losses": 1, "dmWins": 0, "dmCompleted": 1})


def test_php_profile_racing_p1_updates_racing_counters(php_env: dict[str, Any]) -> None:
    player_id = "d6d92cb6-b889-4d89-b672-a38f23a9746c"
    payload = _profile_payload("profile-racing-p1-1", "GT_RACING", player_id)
    payload["players"][0].pop("profile", None)
    payload["players"][0]["position"] = 1
    payload["players"][0]["kills"] = 0
    payload["players"][0]["deaths"] = 0

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    _assert_profile_fields(profile, {"racingWins": 1, "racingCompleted": 1})


def test_php_profile_single_mode_regression_keeps_racing_keys_stable(php_env: dict[str, Any]) -> None:
    dm_only_player = "fea4bdaa-4d9d-42ca-b7f1-0e0b3fc9a2aa"
    race_only_player = "54d0f81f-0be8-463d-bc42-164c728ad623"

    dm_payload = _profile_payload("profile-ui-single-mode-dm", "GT_DEATHMATCH", dm_only_player)
    dm_payload["players"][0].pop("profile", None)
    dm_status, dm_created = _post_php_match(php_env, dm_payload)
    assert dm_status == 201, dm_created

    dm_profile = _get_php_profile(php_env, dm_only_player)
    for racing_key in (
        "racingWins",
        "racingPodiums",
        "racingCompleted",
        "racingTotalMs",
        "racingDmWins",
        "racingDmPodiums",
        "racingDmCompleted",
        "teamRacingWins",
        "teamRacingPodiums",
        "teamRacingCompleted",
    ):
        assert racing_key in dm_profile, f"missing profile key: {racing_key}"
        assert dm_profile[racing_key] == 0, f"{racing_key} should remain zero for DM-only profile"

    race_payload = _profile_payload("profile-ui-single-mode-racing", "GT_RACING", race_only_player)
    race_payload["players"][0].pop("profile", None)
    race_payload["players"][0]["position"] = 1
    race_payload["players"][0]["kills"] = 0
    race_payload["players"][0]["deaths"] = 0
    race_status, race_created = _post_php_match(php_env, race_payload)
    assert race_status == 201, race_created

    race_profile = _get_php_profile(php_env, race_only_player)
    assert race_profile["racingCompleted"] == 1
    assert race_profile["racingWins"] == 1
    assert race_profile["racingPodiums"] == 1
    assert race_profile["dmCompleted"] == 0


def test_php_profile_combined_dm_and_racing_stats_are_separated(php_env: dict[str, Any]) -> None:
    player_id = "7f6e7277-d5fe-4a36-bfa6-e5a5b2f3cf09"

    dm_payload = _profile_payload("profile-combined-dm-race-dm", "GT_DEATHMATCH", player_id)
    dm_payload["players"][0].pop("profile", None)
    dm_status, dm_created = _post_php_match(php_env, dm_payload)
    assert dm_status == 201, dm_created

    race_payload = _profile_payload("profile-combined-dm-race-race", "GT_RACING", player_id)
    race_payload["players"][0].pop("profile", None)
    race_payload["players"][0]["position"] = 1
    race_payload["players"][0]["kills"] = 0
    race_payload["players"][0]["deaths"] = 0
    race_status, race_created = _post_php_match(php_env, race_payload)
    assert race_status == 201, race_created

    profile = _get_php_profile(php_env, player_id)
    _assert_profile_fields(
        profile,
        {
            "wins": 2,
            "losses": 0,
            "dmWins": 1,
            "dmCompleted": 1,
            "racingWins": 1,
            "racingCompleted": 1,
        },
    )


def test_php_profile_reupload_same_match_id_does_not_double_count(php_env: dict[str, Any]) -> None:
    player_id = "2ad63bf6-9a30-46bb-a6d6-a2693f5e9b8f"
    payload = _profile_payload("profile-dedupe-match-id-1", "GT_DEATHMATCH", player_id)
    payload["players"][0].pop("profile", None)

    first_status, first_created = _post_php_match(php_env, payload)
    assert first_status == 201, first_created
    second_status, second_created = _post_php_match(php_env, payload)
    assert second_status == 200, second_created

    profile = _get_php_profile(php_env, player_id)
    expected_golden_profile = {
        "wins": 1,
        "losses": 0,
        "gamesPlayed": 1,
        "dmWins": 1,
        "dmCompleted": 1,
        "racingWins": 0,
        "racingCompleted": 0,
    }
    _assert_profile_fields(profile, expected_golden_profile)


def test_php_profile_dm_snapshot_delta_kills_count_once_per_match(php_env: dict[str, Any]) -> None:
    player_id = "6b1f2505-c27c-4f19-b6df-8a92f049a5e3"

    first_payload = _profile_payload("profile-dm-snapshot-delta-1", "GT_DEATHMATCH", player_id)
    first_payload["players"][0]["profile"] = {"valid": True, "kills": 100, "wins": 10, "losses": 1}
    first_payload["players"][0]["kills"] = 5
    first_status, first_created = _post_php_match(php_env, first_payload)
    assert first_status == 201, first_created

    second_payload = _profile_payload("profile-dm-snapshot-delta-2", "GT_DEATHMATCH", player_id)
    second_payload["players"][0]["profile"] = {"valid": True, "kills": 105, "wins": 11, "losses": 1}
    second_payload["players"][0]["kills"] = 4
    second_status, second_created = _post_php_match(php_env, second_payload)
    assert second_status == 201, second_created

    profile = _get_php_profile(php_env, player_id)
    _assert_profile_fields(
        profile,
        {
            "kills": 109,
            "wins": 12,
            "losses": 1,
            "gamesPlayed": 2,
            "dmWins": 2,
            "dmCompleted": 2,
        },
    )
    assert profile["_processedMatchIds"] == ["profile-dm-snapshot-delta-1", "profile-dm-snapshot-delta-2"]


def test_php_profile_ctf_captures_snapshot_delta_counted_once(php_env: dict[str, Any]) -> None:
    player_id = "de6fc0cf-9db9-4f9a-8578-4862dca5f7cc"
    payload = generate_golden_payload("GT_CTF")
    payload["matchId"] = "profile-ctf-captures-snapshot-delta-1"
    payload["winnerClientNum"] = 1
    payload["settings"]["winnerClientNum"] = 1
    payload["players"] = [
        {
            "playerId": player_id,
            "clientNum": 1,
            "displayName": "CTFCarrier",
            "team": 1,
            "captures": 3,
            "kills": 2,
            "deaths": 1,
            "profile": {"valid": True, "flagCaptures": 10, "ctfCaptures": 10, "wins": 5, "losses": 2},
        }
    ]
    payload["teams"] = [{"team": 1, "rawScore": 5}, {"team": 2, "rawScore": 1}]

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    _assert_profile_fields(
        profile,
        {
            "flagCaptures": 13,
            "ctfCaptures": 13,
            "wins": 6,
            "losses": 2,
            "ctfWins": 1,
            "ctfCompleted": 1,
            "gamesPlayed": 1,
        },
    )
    assert profile["_processedMatchIds"] == ["profile-ctf-captures-snapshot-delta-1"]


def test_php_profile_ctf_winner_client_num_with_numeric_team_ids_sets_win_loss(php_env: dict[str, Any]) -> None:
    winner_id = "4f4ab6bd-88c1-45f3-a145-4779a2f5bf2e"
    loser_id = "a2ff3f08-a789-4419-896f-8d03196d3574"
    payload = generate_golden_payload("GT_CTF")
    payload["matchId"] = "profile-ctf-winner-client-num-int-team-1"
    payload["winnerClientNum"] = 8
    payload["settings"]["winnerClientNum"] = 8
    payload["players"] = [
        {"playerId": winner_id, "clientNum": 8, "displayName": "Winner", "team": 1, "captures": 1, "profile": {"valid": True}},
        {"playerId": loser_id, "clientNum": 9, "displayName": "Loser", "team": 2, "captures": 0, "profile": {"valid": True}},
    ]
    payload["teams"] = [{"team": 1, "rawScore": 3}, {"team": 2, "rawScore": 99}]

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    winner = _get_php_profile(php_env, winner_id)
    loser = _get_php_profile(php_env, loser_id)
    _assert_profile_fields(winner, {"wins": 1, "losses": 0, "ctfWins": 1, "gamesPlayed": 1})
    _assert_profile_fields(loser, {"wins": 0, "losses": 1, "ctfWins": 0, "gamesPlayed": 1})
    assert winner["_processedMatchIds"] == ["profile-ctf-winner-client-num-int-team-1"]
    assert loser["_processedMatchIds"] == ["profile-ctf-winner-client-num-int-team-1"]


def test_php_profile_retry_same_match_id_keeps_processed_match_ids_and_totals_stable(php_env: dict[str, Any]) -> None:
    player_id = "47b3ef88-ee16-4446-9323-a89ad42d5311"
    payload = _profile_payload("profile-retry-same-match-id-1", "GT_DEATHMATCH", player_id)
    payload["players"][0].pop("profile", None)
    payload["players"][0]["kills"] = 6

    first_status, first_created = _post_php_match(php_env, payload)
    assert first_status == 201, first_created

    retry_status, retry_created = _post_php_match(php_env, payload)
    assert retry_status == 200, retry_created

    profile = _get_php_profile(php_env, player_id)
    _assert_profile_fields(
        profile,
        {
            "wins": 1,
            "losses": 0,
            "kills": 6,
            "gamesPlayed": 1,
            "dmWins": 1,
            "dmCompleted": 1,
        },
    )
    assert profile["_processedMatchIds"] == ["profile-retry-same-match-id-1"]

def test_php_profile_upsert_prefers_valid_snapshot(php_env: dict[str, Any]) -> None:
    player_id = "f47ac10b-58cc-4372-a567-0e02b2c3d479"
    payload = _profile_payload("profile-snapshot-1", "GT_DEATHMATCH", player_id)
    payload["players"][0]["profile"] = {
        "valid": True,
        "wins": 11,
        "losses": 5,
        "kills": 81,
        "deaths": 44,
        "dmWins": 9,
        "racingWins": 4,
    }

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    assert profile["wins"] == 12
    assert profile["losses"] == 5
    assert profile["kills"] == 81
    assert profile["deaths"] == 44
    assert profile["dmWins"] == 9
    assert profile["racingWins"] == 4


def test_php_profile_upsert_derives_without_snapshot_and_is_idempotent(php_env: dict[str, Any]) -> None:
    player_id = "9f8c8e19-a6d9-4aa9-a2e3-644f0e2f532f"
    payload = _profile_payload("profile-derived-1", "GT_DEATHMATCH", player_id)
    payload["players"][0].pop("profile", None)

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    assert profile["wins"] == 1
    assert profile["losses"] == 0
    assert profile["kills"] == 7
    assert profile["deaths"] == 2
    assert profile["dmWins"] == 1
    assert profile["racingWins"] == 0
    assert profile["gamesPlayed"] == 1

    duplicate_status, duplicate = _post_php_match(php_env, payload)
    assert duplicate_status == 200, duplicate

    profile_after_duplicate = _get_php_profile(php_env, player_id)
    assert profile_after_duplicate["wins"] == 1
    assert profile_after_duplicate["kills"] == 7
    assert profile_after_duplicate["gamesPlayed"] == 1


def test_php_profile_race_and_dm_wins_increment_separately(php_env: dict[str, Any]) -> None:
    player_id = "1d3d02af-c23c-4f74-a20e-2f2f1cb2f60e"

    race_payload = _profile_payload("profile-race-dm-race", "GT_RACING", player_id)
    race_payload["winnerClientNum"] = -1
    race_payload["settings"]["winnerClientNum"] = -1
    race_payload["players"][0]["position"] = 1
    race_payload["players"][0]["kills"] = 0
    race_payload["players"][0]["deaths"] = 0
    race_status, race_created = _post_php_match(php_env, race_payload)
    assert race_status == 201, race_created

    dm_payload = _profile_payload("profile-race-dm-dm", "GT_DEATHMATCH", player_id)
    dm_payload["players"][0].pop("profile", None)
    dm_status, dm_created = _post_php_match(php_env, dm_payload)
    assert dm_status == 201, dm_created

    profile = _get_php_profile(php_env, player_id)
    assert profile["racingWins"] == 1
    assert profile["dmWins"] == 1


def test_php_profile_racing_dm_updates_racing_fields_not_dm_bucket(php_env: dict[str, Any]) -> None:
    player_id = "b40f7ebf-036f-4d3f-81a5-a0f3f8195d5d"

    payload = _profile_payload("profile-racing-dm-only-racing", "GT_RACING_DM", player_id)
    payload["players"][0].pop("profile", None)
    payload["players"][0]["position"] = 2
    payload["players"][0]["bestLapMs"] = 65432

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    assert profile["racingDmWins"] == 1
    assert profile["racingDmCompleted"] == 1
    assert profile["racingDmPodiums"] == 1
    assert profile["racingDmTotalMs"] == 77777
    assert profile["dmWins"] == 0
    assert profile["dmCompleted"] == 0
    assert profile["dmKills"] == 0


def test_php_profile_best_lap_updates_only_in_race_career_modes(php_env: dict[str, Any]) -> None:
    player_id = "6164e8ce-cc2e-4cc0-b5a5-65f69f8f7f38"

    race_payload = _profile_payload("profile-best-lap-race", "GT_RACING", player_id)
    race_payload["players"][0].pop("profile", None)
    race_payload["players"][0]["bestLapMs"] = 11111
    race_status, race_created = _post_php_match(php_env, race_payload)
    assert race_status == 201, race_created

    dm_payload = _profile_payload("profile-best-lap-dm", "GT_DEATHMATCH", player_id)
    dm_payload["players"][0].pop("profile", None)
    dm_payload["players"][0]["bestLapMs"] = 999
    dm_status, dm_created = _post_php_match(php_env, dm_payload)
    assert dm_status == 201, dm_created

    profile = _get_php_profile(php_env, player_id)
    assert profile["bestLapMs"] == 11111


def test_php_profile_dm_uses_top_level_winner_client_num(php_env: dict[str, Any]) -> None:
    player_id = "8ef82988-d418-443f-bb09-00143874ece1"
    payload = _profile_payload("profile-dm-top-level-winner", "GT_DEATHMATCH", player_id)
    payload["players"][0].pop("profile", None)
    payload["winnerClientNum"] = 1
    payload["settings"]["winnerClientNum"] = 0

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    assert profile["wins"] == 1
    assert profile["dmWins"] == 1


def test_php_profile_racing_uses_top_level_winner_without_position_fallback(php_env: dict[str, Any]) -> None:
    player_id = "01214b8a-e25f-4d26-bf77-160dca8a9dac"
    payload = _profile_payload("profile-racing-top-level-winner", "GT_RACING", player_id)
    payload["players"][0].pop("profile", None)
    payload["winnerClientNum"] = 1
    payload["settings"]["winnerClientNum"] = 0
    payload["players"][0]["position"] = 2

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    assert profile["wins"] == 1
    assert profile["racingWins"] == 1
    assert profile["racingPodiums"] == 1


def test_php_profile_missing_winner_field_is_graceful(php_env: dict[str, Any]) -> None:
    player_id = "f2cbf5d4-b8f9-4548-b3d8-fc1dbec8f8f5"
    payload = _profile_payload("profile-missing-winner", "GT_DEATHMATCH", player_id)
    payload["players"][0].pop("profile", None)
    payload.pop("winnerClientNum", None)
    payload["settings"].pop("winnerClientNum", None)

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    profile = _get_php_profile(php_env, player_id)
    assert profile["wins"] == 0
    assert profile["losses"] == 0
    assert profile["dmWins"] == 0
    assert profile["dmCompleted"] == 1


def test_php_profile_team_mode_applies_team_win_loss_delta(php_env: dict[str, Any]) -> None:
    winner_id = "fdde3dae-c8c7-4c36-a60d-145b7ad95c25"
    loser_id = "0dd4d0a4-80ef-4d7d-93bd-c4e9c4e8ab16"
    payload = generate_golden_payload("GT_TEAM")
    payload["matchId"] = "profile-team-delta-1"
    payload["winnerClientNum"] = 1
    payload["settings"]["winnerClientNum"] = 1
    payload["players"] = [
        {
            "playerId": winner_id,
            "clientNum": 1,
            "displayName": "Winner",
            "team": "red",
            "kills": 3,
            "deaths": 1,
            "position": 1,
            "profile": {"valid": True, "wins": 4, "losses": 2},
        },
        {
            "playerId": loser_id,
            "clientNum": 2,
            "displayName": "Loser",
            "team": "blue",
            "kills": 1,
            "deaths": 3,
            "position": 2,
            "profile": {"valid": True, "wins": 9, "losses": 1},
        },
    ]
    payload["teams"] = [{"team": "red", "rawScore": 10}, {"team": "blue", "rawScore": 5}]

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    winner = _get_php_profile(php_env, winner_id)
    loser = _get_php_profile(php_env, loser_id)
    assert winner["wins"] == 5
    assert winner["losses"] == 2
    assert loser["wins"] == 9
    assert loser["losses"] == 2


@pytest.mark.parametrize("mode,win_field", [("GT_CTF", "ctfWins"), ("GT_CTF4", "ctf4Wins")])
def test_php_profile_objective_team_int_with_teams_array(php_env: dict[str, Any], mode: str, win_field: str) -> None:
    suffix = "1" if mode == "GT_CTF" else "4"
    winner_id = f"11111111-1111-4111-8111-{suffix}00000000001"
    loser_id = f"22222222-2222-4222-8222-{suffix}00000000002"
    payload = generate_golden_payload(mode)
    payload["matchId"] = f"profile-{mode.lower()}-int-team-with-teams"
    payload.pop("winnerClientNum", None)
    payload["settings"].pop("winnerClientNum", None)
    payload["players"] = [
        {"playerId": winner_id, "clientNum": 1, "displayName": "Winner", "team": 1, "captures": 2, "profile": {"valid": True}},
        {"playerId": loser_id, "clientNum": 2, "displayName": "Loser", "team": 2, "captures": 0, "profile": {"valid": True}},
    ]
    payload["teams"] = [{"team": 1, "rawScore": 15}, {"team": 2, "rawScore": 9}]

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    winner = _get_php_profile(php_env, winner_id)
    loser = _get_php_profile(php_env, loser_id)
    assert winner["wins"] == 1
    assert loser["losses"] == 1
    assert winner[win_field] == 1


@pytest.mark.parametrize("mode,win_field", [("GT_CTF", "ctfWins"), ("GT_CTF4", "ctf4Wins")])
def test_php_profile_objective_team_string_without_teams_uses_team_scores(php_env: dict[str, Any], mode: str, win_field: str) -> None:
    suffix = "1" if mode == "GT_CTF" else "4"
    winner_id = f"33333333-3333-4333-8333-{suffix}00000000003"
    loser_id = f"44444444-4444-4444-8444-{suffix}00000000004"
    payload = generate_golden_payload(mode)
    payload["matchId"] = f"profile-{mode.lower()}-string-team-no-teams"
    payload.pop("winnerClientNum", None)
    payload["settings"].pop("winnerClientNum", None)
    payload["players"] = [
        {"playerId": winner_id, "clientNum": 1, "displayName": "Winner", "team": "RED", "captures": 1, "profile": {"valid": True}},
        {"playerId": loser_id, "clientNum": 2, "displayName": "Loser", "team": "blue", "captures": 0, "profile": {"valid": True}},
    ]
    payload["teams"] = None
    payload["teamScores"] = {"RED": 8, "blue": 4}

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    winner = _get_php_profile(php_env, winner_id)
    loser = _get_php_profile(php_env, loser_id)
    assert winner["wins"] == 1
    assert loser["losses"] == 1
    assert winner[win_field] == 1


@pytest.mark.parametrize("mode,win_field", [("GT_CTF", "ctfWins"), ("GT_CTF4", "ctf4Wins")])
def test_php_profile_objective_winner_client_num_has_priority_over_team_scores(php_env: dict[str, Any], mode: str, win_field: str) -> None:
    suffix = "1" if mode == "GT_CTF" else "4"
    winner_id = f"55555555-5555-4555-8555-{suffix}00000000005"
    loser_id = f"66666666-6666-4666-8666-{suffix}00000000006"
    payload = generate_golden_payload(mode)
    payload["matchId"] = f"profile-{mode.lower()}-winner-client-priority"
    payload["winnerClientNum"] = 2
    payload["settings"]["winnerClientNum"] = 1
    payload["players"] = [
        {"playerId": loser_id, "clientNum": 1, "displayName": "Loser", "team": "red", "captures": 0, "profile": {"valid": True}},
        {"playerId": winner_id, "clientNum": 2, "displayName": "Winner", "team": 2, "captures": 3, "profile": {"valid": True}},
    ]
    payload["teams"] = [{"team": "red", "rawScore": 99}, {"team": "blue", "rawScore": 1}]
    payload["teamScores"] = {"red": 99, "blue": 1}

    status, created = _post_php_match(php_env, payload)
    assert status == 201, created

    winner = _get_php_profile(php_env, winner_id)
    loser = _get_php_profile(php_env, loser_id)
    assert winner["wins"] == 1
    assert loser["losses"] == 1
    assert winner[win_field] == 1
