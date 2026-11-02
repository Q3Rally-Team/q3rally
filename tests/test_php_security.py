"""PHP ladder webservice: key storage, registration, match ids and DELETE rights.

Runs the real ladder_service/php_webservice with PHP's built-in web server.
Skipped when no php binary is available.
"""
import json
import os
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
ONLINE_KEY = "b" * 64
OTHER_ONLINE_KEY = "c" * 64
OFFLINE_KEY = "d" * 64
PLAYER = "0a1b2c3d-1111-4222-8333-444455556666"

pytestmark = pytest.mark.skipif(shutil.which("php") is None, reason="php not installed")


def _key(key, name, key_type="server"):
    return {
        "key": key, "serverName": name, "ownerName": "Test", "ownerEmail": "",
        "type": key_type, "status": "active", "createdAt": "2026-01-01T00:00:00Z",
        "approvedAt": "2026-01-01T00:00:00Z", "lastUsedAt": None, "lastUsedIp": None,
        "matchCount": 0,
    }


def _free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


@pytest.fixture()
def ladder(tmp_path):
    root = tmp_path / "ladder"
    shutil.copytree(PHP_DIR, root, ignore=shutil.ignore_patterns("data"))
    (root / "data").mkdir()
    # Old location: the service has to move it into data/private/.
    (root / "data" / "server_keys.json").write_text(json.dumps([
        _key(ONLINE_KEY, "Online Server"),
        _key(OTHER_ONLINE_KEY, "Other Server"),
        _key(OFFLINE_KEY, "Tester_OFFLINE", "offline"),
    ]))
    port = _free_port()
    # Several workers, so parallel requests really run in parallel.
    env = dict(os.environ, PHP_CLI_SERVER_WORKERS="8")
    proc = subprocess.Popen(["php", "-S", f"127.0.0.1:{port}", "-t", str(root)],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, env=env)
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


def _request(url, payload=None, key=None, method=None):
    data = None if payload is None else json.dumps(payload).encode()
    req = urllib.request.Request(url, data=data, method=method or ("POST" if data else "GET"))
    req.add_header("Content-Type", "application/json")
    req.add_header("Accept", "application/json")
    if key:
        req.add_header("Authorization", "Bearer " + key)
    try:
        with urllib.request.urlopen(req, timeout=10) as resp:
            return resp.status, resp.read().decode()
    except urllib.error.HTTPError as err:
        return err.code, err.read().decode()


def _match(match_id, server_name, dedicated=True):
    return {
        "matchId": match_id,
        "mode": "GT_RACING",
        "map": "q3r_testtrack",
        "server": {"name": server_name, "dedicated": dedicated},
        "players": [{
            "name": "Driver", "playerId": PLAYER, "score": 10, "position": 1,
            "raceTimeMs": 180000, "bestLapMs": 59000, "checkpoints": 12,
            "lapCount": 3, "lapTimes": [61000, 60000, 59000],
        }],
    }


def test_key_file_moves_to_private_and_is_not_served(ladder):
    base, root = ladder

    status, _ = _request(base + "/matches/server_keys")
    assert status == 404
    assert not (root / "data" / "server_keys.json").exists()
    keys = json.loads((root / "data" / "private" / "server_keys.json").read_text())
    assert {k["key"] for k in keys} == {ONLINE_KEY, OTHER_ONLINE_KEY, OFFLINE_KEY}
    assert (root / "data" / "private" / ".htaccess").is_file()

    for reserved in ("server_keys", "match_index", "version", "rl_127.0.0.1", "SERVER_KEYS"):
        status, _ = _request(base + "/matches/" + reserved)
        assert status == 404, reserved


def test_reserved_match_ids_are_rejected(ladder):
    base, root = ladder

    for reserved in ("server_keys", "match_index", "rl_x", ".hidden"):
        status, body = _request(base + "/matches", _match(reserved, "Online Server"), ONLINE_KEY)
        assert status == 422, (reserved, body)
        assert json.loads(body)["error"]["code"] == "MATCH_ID_INVALID"


def test_registration_is_pending_and_throttled(ladder):
    base, root = ladder

    payload = {"serverName": "Someone_OFFLINE", "ownerName": "x", "type": "offline"}
    status, body = _request(base + "/register", payload)
    assert status == 201, body
    issued = json.loads(body)
    assert issued["status"] == "pending"

    # A pending key can neither report nor delete anything.
    status, _ = _request(base + "/matches", _match("m-pending", "Someone_OFFLINE"), issued["key"])
    assert status == 403
    status, _ = _request(base + "/matches/m-pending", key=issued["key"], method="DELETE")
    assert status == 403

    for _ in range(9):
        status, _ = _request(base + "/register", payload)
    assert status == 201
    status, body = _request(base + "/register", payload)
    assert status == 429, body


def test_offline_keys_report_offline_matches(ladder):
    base, root = ladder

    status, body = _request(base + "/matches", _match("m-offline", "Tester_OFFLINE", dedicated=True), OFFLINE_KEY)
    assert status == 201, body
    status, body = _request(base + "/matches/m-offline")
    assert status == 200
    stored = json.loads(body)
    assert stored["source"] == "offline"
    assert "ingestKeyId" not in stored

    status, body = _request(base + "/matches", _match("m-online", "Online Server"), ONLINE_KEY)
    assert status == 201, body
    assert json.loads(_request(base + "/matches/m-online")[1])["source"] == "online"


def test_only_the_reporting_server_deletes_a_match(ladder):
    base, root = ladder

    status, body = _request(base + "/matches", _match("m-del", "Online Server"), ONLINE_KEY)
    assert status == 201, body

    status, _ = _request(base + "/matches/m-del", key=OFFLINE_KEY, method="DELETE")
    assert status == 403
    status, _ = _request(base + "/matches/m-del", key=OTHER_ONLINE_KEY, method="DELETE")
    assert status == 403
    status, _ = _request(base + "/matches/server_keys", key=ONLINE_KEY, method="DELETE")
    assert status == 404
    assert (root / "data" / "private" / "server_keys.json").is_file()

    status, _ = _request(base + "/matches/m-del", key=ONLINE_KEY, method="DELETE")
    assert status == 204
    assert _request(base + "/matches/m-del")[0] == 404

    # Matches from before 1.0.13 have no reporter: only removable on the server.
    (root / "data" / "m-legacy.json").write_text(json.dumps(_match("m-legacy", "Online Server")))
    status, _ = _request(base + "/matches/m-legacy", key=ONLINE_KEY, method="DELETE")
    assert status == 403
    assert (root / "data" / "m-legacy.json").exists()


def test_key_usage_counts_survive_parallel_requests(ladder):
    base, root = ladder
    from concurrent.futures import ThreadPoolExecutor

    def report(i):
        return _request(base + "/matches", _match(f"m-par-{i}", "Online Server"), ONLINE_KEY)[0]

    with ThreadPoolExecutor(max_workers=8) as pool:
        statuses = list(pool.map(report, range(20)))
    assert statuses.count(201) == 20
    keys = json.loads((root / "data" / "private" / "server_keys.json").read_text())
    online = next(k for k in keys if k["key"] == ONLINE_KEY)
    assert online["matchCount"] == 20
    assert len(keys) == 3


def test_parallel_matches_keep_index_and_profiles_complete(ladder):
    """Match file, index and profile are written under one lock (no lost updates)."""
    base, root = ladder
    from concurrent.futures import ThreadPoolExecutor

    def report(i):
        return _request(base + "/matches", _match(f"m-idx-{i}", "Online Server"), ONLINE_KEY)[0]

    with ThreadPoolExecutor(max_workers=8) as pool:
        statuses = list(pool.map(report, range(24)))
    assert statuses.count(201) == 24
    index = json.loads((root / "data" / "match_index.json").read_text())
    assert sorted(e["matchId"] for e in index) == sorted(f"m-idx-{i}" for i in range(24))
    assert not list((root / "data").glob("*.tmp"))


def test_approved_servers_have_their_own_rate_limit(ladder):
    """30 POST/min per IP for players and unknown keys, 120 per approved server key."""
    base, _ = ladder
    for i in range(35):
        status, body = _request(base + "/matches", _match(f"m-rl-{i}", "Online Server"), ONLINE_KEY)
        assert status == 201, (i, body)

    statuses = [_request(base + "/matches", _match(f"m-bad-{i}", "x"), "f" * 64)[0] for i in range(31)]
    assert statuses[:30] == [401] * 30
    assert statuses[30] == 429
    # The approved server is not affected by the exhausted per-IP bucket.
    status, _ = _request(base + "/matches", _match("m-rl-after", "Online Server"), ONLINE_KEY)
    assert status == 201
