"""PHP ladder webservice: lap ghost upload, ranking and plausibility checks.

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
API_KEY = "a" * 64
SERVER_NAME = "Ghost Test Server"
PLAYER_A = "0a1b2c3d-1111-4222-8333-444455556666"
PLAYER_B = "0a1b2c3d-1111-4222-8333-777788889999"

pytestmark = pytest.mark.skipif(shutil.which("php") is None, reason="php not installed")


def _free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


@pytest.fixture()
def ladder(tmp_path):
    root = tmp_path / "ladder"
    shutil.copytree(PHP_DIR, root, ignore=shutil.ignore_patterns("data"))
    (root / "data").mkdir()
    (root / "data" / "server_keys.json").write_text(json.dumps([{
        "key": API_KEY, "serverName": SERVER_NAME, "ownerName": "Test", "ownerEmail": "",
        "type": "offline", "status": "active", "createdAt": "2026-01-01T00:00:00Z",
        "approvedAt": "2026-01-01T00:00:00Z", "lastUsedAt": None, "lastUsedIp": None, "matchCount": 0,
    }]))
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


def _request(url, payload=None, key=API_KEY):
    data = None if payload is None else json.dumps(payload).encode()
    req = urllib.request.Request(url, data=data, method="POST" if data else "GET")
    req.add_header("Content-Type", "application/json")
    req.add_header("Accept", "application/json")
    if key:
        req.add_header("Authorization", "Bearer " + key)
    try:
        with urllib.request.urlopen(req, timeout=10) as resp:
            body = resp.read().decode()
            return resp.status, body
    except urllib.error.HTTPError as err:
        return err.code, err.read().decode()


def _ghost(player_id=PLAYER_A, lap_ms=60000, radius=3000.0, teleport=False, vehicle="evo",
           physics=1, checksum=4242, course=None):
    """Circle lap sampled every 100 ms, in the .ghost text format."""
    lines = []
    steps = lap_ms // 100
    for i in range(steps + 1):
        t = i * 100
        a = 2 * math.pi * t / lap_ms
        x, y = math.cos(a) * radius, math.sin(a) * radius
        if teleport and i == steps // 2:
            x += 9000
        lines.append(f"{t} {x:.1f} {y:.1f} 64.0 0.0 {math.degrees(a) + 90:.1f} 0.0 0.0 0.0 0.0 0 127 0")
    header = [
        "# Q3Rally server ghost", "map q3r_testtrack", f"vehicle {vehicle}", "track_length 1",
        "track_reversed 0", f"best_time_ms {lap_ms}", f"physics_version {physics}",
        f"map_checksum {checksum}", "player Tester", f"player_id {player_id}", f"frames {len(lines)}",
    ]
    data = "\n".join(header + lines) + "\n"
    if course is None:
        course = int(2 * math.pi * radius)
    return {
        "ghostId": f"q3r_testtrack-{player_id[:8]}-{lap_ms}",
        "server": {"name": SERVER_NAME, "dedicated": False, "build": "test"},
        "player": {"id": player_id, "name": "^1Fast^7Driver"},
        "map": "q3r_testtrack", "vehicle": vehicle, "trackLength": 1, "trackReversed": 0,
        "lapMs": lap_ms, "frames": len(lines), "gametype": 8, "physicsVersion": physics,
        "mapChecksum": checksum, "courseLengthUnits": course, "sprintTrack": False, "data": data,
    }


def test_ghost_upload_ranking_and_download(ladder):
    base, root = ladder

    status, body = _request(base + "/ghosts", _ghost(lap_ms=60000))
    assert status == 201, body
    stored = json.loads(body)
    assert stored["stored"] is True and stored["rank"] == 1
    ghost_id = stored["ghostId"]
    assert ghost_id == f"q3r_testtrack.tl1_rev0.evo.p1_c4242.{PLAYER_A}"

    # Slower lap of the same player is not stored.
    status, body = _request(base + "/ghosts", _ghost(lap_ms=61000))
    assert status == 200 and json.loads(body)["reason"] == "NOT_FASTER"

    # Faster lap replaces the old one; second player ranks behind.
    status, body = _request(base + "/ghosts", _ghost(lap_ms=58000))
    assert status == 201 and json.loads(body)["previousLapMs"] == 60000
    status, body = _request(base + "/ghosts", _ghost(player_id=PLAYER_B, lap_ms=59000))
    assert status == 201 and json.loads(body)["rank"] == 2

    status, body = _request(base + "/ghosts?map=q3r_testtrack&tl=1&rev=0&vehicle=evo", key=None)
    assert status == 200, body
    listing = json.loads(body)
    assert [g["lapMs"] for g in listing["ghosts"]] == [58000, 59000]
    assert listing["ghosts"][0]["playerName"] == "FastDriver"
    assert listing["ghosts"][0]["source"] == "offline"
    assert "data" not in listing["ghosts"][0]

    status, body = _request(base + "/ghosts/" + ghost_id, key=None)
    assert status == 200
    record = json.loads(body)
    assert record["lapMs"] == 58000 and record["data"].startswith("# Q3Rally server ghost\nmap q3r_testtrack\n")

    status, body = _request(base + "/ghosts/" + ghost_id + "?format=raw", key=None)
    assert status == 200 and body.startswith("# Q3Rally server ghost")

    # Download variant for the website: attachment with a readable file name.
    with urllib.request.urlopen(base + "/ghosts/" + ghost_id + "?format=raw&download=1", timeout=10) as resp:
        assert resp.headers["Content-Disposition"] == 'attachment; filename="q3r_testtrack_tl1_rev0_evo_58000.ghost"'
        assert resp.headers["Access-Control-Allow-Origin"] == "https://www.q3rally.com"

    # Other physics version / map build: separate bucket, filtered out.
    status, body = _request(base + "/ghosts", _ghost(lap_ms=50000, physics=2))
    assert status == 201
    status, body = _request(base + "/ghosts?map=q3r_testtrack&tl=1&rev=0&vehicle=evo&physics=1&checksum=4242", key=None)
    assert [g["lapMs"] for g in json.loads(body)["ghosts"]] == [58000, 59000]

    keys = json.loads((root / "data" / "server_keys.json").read_text())
    assert keys[0]["matchCount"] == 0 and keys[0]["ghostCount"] == 5


@pytest.mark.parametrize("mutate,code", [
    (lambda g: g.update(_ghost(teleport=True)), "GHOST_IMPLAUSIBLE"),
    (lambda g: g.update(_ghost(radius=60000.0)), "GHOST_IMPLAUSIBLE"),
    (lambda g: g.update(courseLengthUnits=500000), "GHOST_IMPLAUSIBLE"),
    (lambda g: g.update(lapMs=59000), "GHOST_HEADER_MISMATCH"),
    (lambda g: g.update(frames=3), "GHOST_HEADER_MISMATCH"),
    (lambda g: g["player"].update(id="not-a-uuid"), "GHOST_FIELD_INVALID"),
    (lambda g: g.update(data=g["data"].replace("\n0 ", "\n0 x ", 1)), "GHOST_DATA_INVALID"),
])
def test_ghost_upload_rejects_implausible(ladder, mutate, code):
    base, _ = ladder
    ghost = _ghost()
    mutate(ghost)
    status, body = _request(base + "/ghosts", ghost)
    assert status == 422, body
    assert json.loads(body)["error"]["code"] == code


def test_ghost_upload_requires_key(ladder):
    base, _ = ladder
    status, _ = _request(base + "/ghosts", _ghost(), key=None)
    assert status == 401
    status, _ = _request(base + "/ghosts", _ghost(), key="b" * 64)
    assert status == 401


PLAYER_C = "0a1b2c3d-1111-4222-8333-aaaabbbbcccc"


def test_ghost_list_text_format_per_vehicle(ladder):
    """The game engine reads the ranking as tab separated text, best K per vehicle."""
    base, _ = ladder
    for player, lap, vehicle in [(PLAYER_A, 58000, "evo"), (PLAYER_B, 59000, "evo"),
                                 (PLAYER_C, 60000, "evo"), (PLAYER_A, 61000, "sidepipe")]:
        status, body = _request(base + "/ghosts", _ghost(player_id=player, lap_ms=lap, vehicle=vehicle))
        assert status == 201, body

    query = "/ghosts?map=q3r_testtrack&tl=1&rev=0&physics=1&checksum=4242"
    status, body = _request(base + query + "&perVehicle=2&format=text", key=None)
    assert status == 200, body
    lines = [line.split("\t") for line in body.splitlines()]
    assert [(f[1], f[2]) for f in lines] == [("58000", "evo"), ("59000", "evo"), ("61000", "sidepipe")]
    assert lines[0][0] == f"q3r_testtrack.tl1_rev0.evo.p1_c4242.{PLAYER_A}"
    assert lines[0][3] == "FastDriver"
    assert all(len(f) == 4 for f in lines)

    status, body = _request(base + query + "&limit=2&format=text", key=None)
    assert [line.split("\t")[1] for line in body.splitlines()] == ["58000", "59000"]

    # JSON stays the default and honours perVehicle as well.
    status, body = _request(base + query + "&perVehicle=1", key=None)
    assert [g["lapMs"] for g in json.loads(body)["ghosts"]] == [58000, 61000]


def test_ghost_catalog_lists_maps_variants_builds_and_vehicles(ladder):
    """Overview for the ranking page on the ladder website."""
    base, _ = ladder
    status, body = _request(base + "/ghosts/catalog", key=None)
    assert status == 200 and json.loads(body) == {"maps": []}

    for player, lap, vehicle, checksum in [(PLAYER_A, 58000, "evo", 4242), (PLAYER_B, 59000, "evo", 4242),
                                           (PLAYER_A, 61000, "sidepipe", 4242)]:
        status, body = _request(base + "/ghosts", _ghost(player_id=player, lap_ms=lap, vehicle=vehicle, checksum=checksum))
        assert status == 201, body
    time.sleep(1.1)  # the newer map build appears later
    status, body = _request(base + "/ghosts", _ghost(player_id=PLAYER_C, lap_ms=57000, checksum=5555))
    assert status == 201, body

    status, body = _request(base + "/ghosts/catalog", key=None)
    assert status == 200, body
    maps = json.loads(body)["maps"]
    assert [m["map"] for m in maps] == ["q3r_testtrack"]
    variant = maps[0]["variants"][0]
    assert variant["variant"] == "tl1_rev0" and variant["trackLength"] == 1 and variant["trackReversed"] == 0
    assert variant["count"] == 4
    # Current build (first ghost newest) first, with vehicles per build.
    assert [b["bucket"] for b in variant["buckets"]] == ["p1_c5555", "p1_c4242"]
    old = variant["buckets"][1]
    assert old["physicsVersion"] == 1 and old["mapChecksum"] == 4242
    assert old["vehicles"] == {"evo": 2, "sidepipe": 1} and old["count"] == 3

    # The catalog path is not mistaken for a ghost id.
    status, body = _request(base + "/ghosts/catalog?format=raw", key=None)
    assert status == 200 and body.startswith("{")
