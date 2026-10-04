"""PHP ladder webservice: admin.php login, session, CSRF tokens and key ids.

Runs the real ladder_service/php_webservice with PHP's built-in web server.
Skipped when no php binary is available.
"""
import http.cookiejar
import json
import os
import pathlib
import re
import shutil
import socket
import subprocess
import time
import urllib.error
import urllib.parse
import urllib.request

import pytest

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
PHP_DIR = REPO_ROOT / "ladder_service" / "php_webservice"
PASSWORD = "correct horse battery staple"
PENDING_KEY = "e" * 64

pytestmark = pytest.mark.skipif(shutil.which("php") is None, reason="php not installed")


def _free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


@pytest.fixture()
def admin(tmp_path):
    root = tmp_path / "ladder"
    shutil.copytree(PHP_DIR, root, ignore=shutil.ignore_patterns("data"))
    (root / "data" / "private").mkdir(parents=True)
    (root / "data" / "private" / "server_keys.json").write_text(json.dumps([{
        "key": PENDING_KEY, "serverName": "New Server", "ownerName": "Owner", "ownerEmail": "o@example.com",
        "type": "server", "status": "pending", "createdAt": "2026-10-01T00:00:00Z",
        "approvedAt": None, "lastUsedAt": None, "lastUsedIp": None, "matchCount": 0,
    }]))
    port = _free_port()
    env = dict(os.environ, LADDER_ADMIN_PASSWORD=PASSWORD)
    proc = subprocess.Popen(["php", "-S", f"127.0.0.1:{port}", "-t", str(root)],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, env=env)
    url = f"http://127.0.0.1:{port}/admin.php"
    for _ in range(50):
        try:
            urllib.request.urlopen(url, timeout=1)
            break
        except urllib.error.HTTPError:
            break
        except Exception:
            time.sleep(0.1)
    yield url, root
    proc.terminate()
    proc.wait(timeout=5)


class Browser:
    def __init__(self):
        self.jar = http.cookiejar.CookieJar()
        self.opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(self.jar))
        self.headers = {}

    def request(self, url, form=None):
        data = None if form is None else urllib.parse.urlencode(form).encode()
        try:
            with self.opener.open(urllib.request.Request(url, data=data), timeout=10) as resp:
                self.headers = resp.headers
                return resp.read().decode()
        except urllib.error.HTTPError as err:
            self.headers = err.headers
            return err.read().decode()

    def cookie(self, name):
        return next((c for c in self.jar if c.name == name), None)


def _keys(root):
    return json.loads((root / "data" / "private" / "server_keys.json").read_text())


def _key_id(key):
    import hashlib
    return hashlib.sha256(key.encode()).hexdigest()[:16]


def test_login_session_and_csrf(admin):
    url, root = admin
    browser = Browser()

    page = browser.request(url)
    assert 'name="password"' in page
    assert browser.headers["X-Frame-Options"] == "DENY"
    before = browser.cookie("q3r_ladder_admin")
    assert before is not None

    page = browser.request(url, {"password": PASSWORD})
    assert "Ladder Admin" in page and 'name="password"' not in page
    after = browser.cookie("q3r_ladder_admin")
    assert after.value != before.value          # new session id after the login
    assert after.has_nonstandard_attr("HttpOnly")

    # The page never contains the key, only its id.
    page = browser.request(url + "?tab=pending")
    assert PENDING_KEY not in page and PENDING_KEY[:12] not in page
    assert _key_id(PENDING_KEY) in page
    token = re.search(r"name='csrf' value='([0-9a-f]{64})'", page).group(1)

    # Without or with a wrong token nothing happens (form from another site).
    page = browser.request(url, {"action": "approve", "keyId": _key_id(PENDING_KEY)})
    assert "The form has expired" in page
    page = browser.request(url, {"action": "approve", "keyId": _key_id(PENDING_KEY), "csrf": "0" * 64})
    assert "The form has expired" in page
    assert _keys(root)[0]["status"] == "pending"

    # With the token: approved. The full key in the form is not accepted.
    page = browser.request(url, {"action": "approve", "keyId": PENDING_KEY, "csrf": token})
    assert _keys(root)[0]["status"] == "pending"
    page = browser.request(url, {"action": "approve", "keyId": _key_id(PENDING_KEY), "csrf": token})
    assert "Key approved." in page
    assert _keys(root)[0]["status"] == "active"

    # Logout ends the session.
    browser.request(url, {"action": "logout", "csrf": token})
    assert 'name="password"' in browser.request(url)


def test_failed_logins_are_limited(admin):
    url, root = admin
    browser = Browser()
    for _ in range(5):
        page = browser.request(url, {"password": "wrong"})
        assert "Wrong password." in page
    # Locked, even with the right password.
    page = browser.request(url, {"password": PASSWORD})
    assert "Too many failed logins" in page
    assert 'name="password"' in page
