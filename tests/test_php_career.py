"""Per-mode career counters on the ladder (index.php profile upsert).

The game records the match outcome into the local profile before it builds
the payload, so the local player's snapshot already contains the match. The
ladder must take a snapshot that is ahead as-is and only add the match delta
for players without a snapshot. Regression: max(existing, snapshot) + delta
let wins/completed run one match ahead for the local player.
"""
import json
import shutil
import socket
import subprocess
import time
import urllib.error
import urllib.request
import pathlib

import pytest

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
PHP_DIR = REPO_ROOT / "ladder_service" / "php_webservice"
SERVER_KEY = "d" * 64
LOCAL = "0a1b2c3d-1111-4222-8333-0000000000a1"
REMOTE = "0a1b2c3d-1111-4222-8333-0000000000b2"

pytestmark = pytest.mark.skipif(shutil.which("php") is None, reason="php not installed")


def _free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


@pytest.fixture()
def ladder(tmp_path):
    root = tmp_path / "ladder"
    shutil.copytree(PHP_DIR, root, ignore=shutil.ignore_patterns("data"))
    (root / "data" / "private").mkdir(parents=True)
    (root / "data" / "private" / "server_keys.json").write_text(json.dumps([
        {"key": SERVER_KEY, "serverName": "Online Server", "type": "server", "status": "active",
         "createdAt": "2026-01-01T00:00:00Z", "approvedAt": "2026-01-01T00:00:00Z"},
    ]))
    port = _free_port()
    proc = subprocess.Popen(["php", "-S", f"127.0.0.1:{port}", "-t", str(root)],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    base = f"http://127.0.0.1:{port}/index.php/api/v1"
    for _ in range(50):
        try:
            urllib.request.urlopen(base + "/maps/levelshots", timeout=1)
            break
        except Exception:
            time.sleep(0.1)
    yield base
    proc.terminate()
    proc.wait(timeout=5)


def _request(url, payload=None):
    data = None if payload is None else json.dumps(payload).encode()
    req = urllib.request.Request(url, data=data, method="POST" if data else "GET")
    req.add_header("Content-Type", "application/json")
    req.add_header("Authorization", "Bearer " + SERVER_KEY)
    try:
        with urllib.request.urlopen(req, timeout=10) as resp:
            return resp.status, resp.read().decode()
    except urllib.error.HTTPError as err:
        return err.code, err.read().decode()


def _autoball_match(match_id, seq, red_goals, blue_goals, local_snapshot, local_goals, remote_goals):
    return {
        "matchId": match_id, "serverMatchSeq": seq, "mode": "GT_AUTOBALL", "map": "q3r_autoball_test",
        "server": {"name": "Online Server", "dedicated": True},
        "teams": [{"team": "red", "score": red_goals}, {"team": "blue", "score": blue_goals}],
        "players": [
            {"name": "Per", "playerId": LOCAL, "team": 1, "score": 300, "clientNum": 0,
             "autoballGoals": local_goals,
             "profile": dict({"valid": True}, **local_snapshot)},
            {"name": "Friend", "playerId": REMOTE, "team": 2, "score": 200, "clientNum": 1,
             "autoballGoals": remote_goals},
        ],
    }


def _profile(base, player_id):
    status, body = _request(base + "/players/" + player_id)
    assert status == 200, body
    return json.loads(body)


def test_local_snapshot_is_not_counted_twice(ladder):
    base = ladder
    # Match 1: red (local player) wins 3:1. The local profile already counts it.
    status, body = _request(base + "/matches", _autoball_match(
        "m-ab-1", 1, 3, 1,
        {"autoballWins": 1, "autoballCompleted": 1, "autoballGoals": 2, "wins": 1}, 2, 1))
    assert status == 201, body
    local = _profile(base, LOCAL)
    assert local["autoballWins"] == 1
    assert local["autoballCompleted"] == 1
    assert local["autoballGoals"] == 2
    remote = _profile(base, REMOTE)
    assert remote["autoballWins"] == 0
    assert remote["autoballCompleted"] == 1
    assert remote["autoballGoals"] == 1

    # Match 2: blue wins 0:2, the local player loses.
    status, body = _request(base + "/matches", _autoball_match(
        "m-ab-2", 2, 0, 2,
        {"autoballWins": 1, "autoballCompleted": 2, "autoballGoals": 2, "wins": 1}, 0, 2))
    assert status == 201, body
    local = _profile(base, LOCAL)
    assert local["autoballWins"] == 1
    assert local["autoballCompleted"] == 2
    assert local["autoballGoals"] == 2
    remote = _profile(base, REMOTE)
    assert remote["autoballWins"] == 1
    assert remote["autoballCompleted"] == 2
    assert remote["autoballGoals"] == 3


def test_stale_snapshot_falls_back_to_delta(ladder):
    base = ladder
    _request(base + "/matches", _autoball_match(
        "m-ab-3", 3, 2, 0, {"autoballWins": 1, "autoballCompleted": 1}, 1, 0))
    # A snapshot that is not ahead (other machine, reset profile) adds the delta.
    _request(base + "/matches", _autoball_match(
        "m-ab-4", 4, 2, 0, {"autoballWins": 0, "autoballCompleted": 0}, 1, 0))
    local = _profile(base, LOCAL)
    assert local["autoballWins"] == 2
    assert local["autoballCompleted"] == 2
