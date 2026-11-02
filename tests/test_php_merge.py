"""PHP ladder webservice: merge_player.php moves matches, ghosts, key binding
and profile of an old player id to the new one (the client got a new UUID).

Runs the real ladder_service/php_webservice with PHP's built-in web server.
Skipped when no php binary is available.
"""
import json
import math
import pathlib
import shutil
import socket
import subprocess
import time
import urllib.error
import urllib.request

import pytest

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
PHP_DIR = REPO_ROOT / "ladder_service" / "php_webservice"
SERVER_KEY = "b" * 64
OLD = "0a1b2c3d-1111-4222-8333-000000000001"
NEW = "0a1b2c3d-1111-4222-8333-000000000002"
FRIEND = "0a1b2c3d-1111-4222-8333-000000000003"

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
        {"key": "c" * 64, "serverName": "Per_OFFLINE", "type": "offline", "status": "active",
         "createdAt": "2026-01-01T00:00:00Z", "approvedAt": "2026-01-01T00:00:00Z", "playerId": OLD},
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
    yield base, root
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


def _match(match_id, player_id, wins, seq):
    return {
        "matchId": match_id, "serverMatchSeq": seq, "mode": "GT_RACING", "map": "q3r_testtrack",
        "server": {"name": "Online Server", "dedicated": True},
        "players": [
            {"name": "Per", "playerId": player_id, "score": 10, "position": 1, "clientNum": 0,
             "raceTimeMs": 180000, "bestLapMs": 59000, "checkpoints": 12, "lapCount": 3,
             "lapTimes": [61000, 60000, 59000],
             "profile": {"valid": True, "wins": wins, "playerScore": 100 + wins}},
            {"name": "Friend", "playerId": FRIEND, "score": 5, "position": 2, "clientNum": 1,
             "raceTimeMs": 190000, "bestLapMs": 61000, "checkpoints": 12, "lapCount": 3,
             "lapTimes": [63000, 62000, 61000]},
        ],
    }


def _ghost(player_id, lap_ms):
    radius = 3000.0
    lines = []
    for i in range(lap_ms // 100 + 1):
        t = i * 100
        a = 2 * math.pi * t / lap_ms
        lines.append(f"{t} {math.cos(a) * radius:.1f} {math.sin(a) * radius:.1f} 64.0 0 0 0 0 0 0 0 127 0")
    header = ["map q3r_testtrack", "vehicle evo", "track_length 0", "track_reversed 0",
              f"best_time_ms {lap_ms}", f"frames {len(lines)}"]
    return {
        "server": {"name": "Online Server", "dedicated": True},
        "player": {"id": player_id, "name": "Per"},
        "map": "q3r_testtrack", "vehicle": "evo", "trackLength": 0, "trackReversed": 0,
        "lapMs": lap_ms, "frames": len(lines), "gametype": 8, "physicsVersion": 1,
        "mapChecksum": 1, "courseLengthUnits": int(2 * math.pi * radius), "sprintTrack": False,
        "data": "\n".join(header + lines) + "\n",
    }


def _merge(root, *extra):
    return subprocess.run(["php", "merge_player.php", OLD, NEW, *extra], cwd=root,
                          capture_output=True, text=True, check=True).stdout


def test_merge_old_player_id_into_new(ladder):
    base, root = ladder
    for i in range(3):
        assert _request(base + "/matches", _match(f"m-old-{i}", OLD, 10 + i, i + 1))[0] == 201
    for i in range(2):
        assert _request(base + "/matches", _match(f"m-new-{i}", NEW, 20 + i, i + 4))[0] == 201
    assert _request(base + "/ghosts", _ghost(OLD, 58000))[0] == 201
    assert _request(base + "/ghosts", _ghost(NEW, 59000))[0] == 201
    friend_before = json.loads((root / "data" / "profiles" / (FRIEND + ".json")).read_text())

    out = _merge(root)
    assert "Matches: 3 with the old id, 5 of the player in total" in out
    assert "Dry run" in out
    assert (root / "data" / "profiles" / (OLD + ".json")).exists()

    out = _merge(root, "--apply")
    assert "Rebuilt profile: 5 games" in out
    assert not (root / "data" / "profiles" / (OLD + ".json")).exists()
    profile = json.loads(_request(base + "/players/" + NEW)[1])
    assert profile["gamesPlayed"] == 5
    assert profile["wins"] >= 21
    # Other players' profiles are untouched.
    friend_after = json.loads((root / "data" / "profiles" / (FRIEND + ".json")).read_text())
    assert friend_after["gamesPlayed"] == friend_before["gamesPlayed"] == 5

    # Matches and index only know the new id.
    for path in (root / "data").glob("m-*.json"):
        assert OLD not in path.read_text()
    assert OLD not in (root / "data" / "match_index.json").read_text()

    # The faster ghost (old id) now belongs to the new id.
    listing = json.loads(_request(base + "/ghosts?map=q3r_testtrack&tl=0&rev=0")[1])
    assert [(g["playerId"], g["lapMs"]) for g in listing["ghosts"]] == [(NEW, 58000)]
    raw = _request(base + "/ghosts/" + listing["ghosts"][0]["ghostId"] + "?format=raw")
    assert raw[0] == 200

    # Offline key binding follows.
    keys = json.loads((root / "data" / "private" / "server_keys.json").read_text())
    assert keys[1]["playerId"] == NEW
    assert any(p.name.startswith("merge-") for p in (root / "data" / "private").iterdir())
