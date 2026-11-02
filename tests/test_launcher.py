"""Exercise the real signed catalog/download/install path with a loopback server."""
import base64
from datetime import datetime, timedelta, timezone
import hashlib
import http.server
import io
import json
import os
from pathlib import Path
import shutil
import stat
import subprocess
import tempfile
import threading
import unittest
from unittest.mock import patch
import zipfile

from nacl.signing import SigningKey
from launcher.core import Manager, LauncherError, decode_catalog, file_lock, safe_path, version_key
from launcher.profiles import import_profile
from tools.publish_launcher_catalog import sign


class LauncherTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.key = SigningKey.generate()
        self.keys = {"test": base64.b64encode(bytes(self.key.verify_key)).decode()}
        self.requests = []
        self.agents = []
        owner = self
        class Handler(http.server.BaseHTTPRequestHandler):
            def do_GET(self):
                owner.agents.append(self.headers.get("User-Agent"))
                data = owner.archive if self.path == "/game.zip" else json.dumps(owner.envelope).encode()
                offset = int(self.headers.get("Range", "bytes=0-").split("=")[1].split("-")[0])
                owner.requests.append((self.path, offset))
                self.send_response(206 if offset else 200)
                if offset:
                    self.send_header("Content-Range", f"bytes {offset}-{len(data)-1}/{len(data)}")
                self.send_header("Content-Length", str(len(data) - offset))
                self.end_headers()
                self.wfile.write(data[offset:])
            def log_message(self, *args):
                pass
        self.server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
        threading.Thread(target=self.server.serve_forever, daemon=True).start()
        self.addCleanup(self.server.server_close)
        self.addCleanup(self.server.shutdown)
        self.url = f"http://127.0.0.1:{self.server.server_port}"
        self.manager = Manager(self.root / "data", self.keys, targets=["windows-x86_64"], development=True)
        self.prepare()

    def prepare(self, *, version="0.8", extra=None, expires=None, sequence=1, product="Q3Rally", executable=b"example executable\n"):
        manifest = {"schema": 1, "product": product, "version": version, "target": "windows-x86_64",
                    "files": [{"path": "q3rally.x86_64.exe", "size": len(executable),
                               "sha256": hashlib.sha256(executable).hexdigest()}]}
        raw = json.dumps(manifest).encode()
        stream = io.BytesIO()
        with zipfile.ZipFile(stream, "w") as archive:
            prefix = f"{product}-v{version}-windows-x86_64/"
            entry = zipfile.ZipInfo(prefix + "q3rally.x86_64.exe")
            entry.create_system = 3
            entry.external_attr = (stat.S_IFREG | 0o755) << 16
            archive.writestr(entry, executable)
            archive.writestr(prefix + "release-manifest.json", raw)
            if extra:
                archive.writestr(prefix + extra, "unexpected")
        self.archive = stream.getvalue()
        package = {"url": self.url + "/game.zip", "size": len(self.archive),
                   "sha256": hashlib.sha256(self.archive).hexdigest(),
                   "manifest_sha256": hashlib.sha256(raw).hexdigest(), "entrypoint": "q3rally.x86_64.exe"}
        self.payload = {"schema": 1, "product": "Q3Rally", "sequence": sequence,
                        "expires": (expires or datetime.now(timezone.utc) + timedelta(days=1)).isoformat(),
                        "releases": [], "launchers": []}
        self.payload["releases" if product == "Q3Rally" else "launchers"] = [{"version": version, "packages": {"windows-x86_64": package}}]
        self.envelope = sign(self.payload, self.key, "test")
        self.package = package

    def install(self):
        return self.manager.install(self.envelope, "0.8", "windows-x86_64")

    def test_install_verify_tamper_repair_and_user_data_survives_removal(self):
        directory = self.install()
        self.assertEqual(len(self.manager.installed()), 1)
        executable = self.manager.verify("0.8", "windows-x86_64")
        executable.write_bytes(b"tampered")
        with self.assertRaises(LauncherError):
            self.manager.verify("0.8", "windows-x86_64")
        self.install()
        self.manager.verify("0.8", "windows-x86_64")
        profile = self.manager.root / "user-data/0.8/baseq3r/profiles/keep.json"
        profile.parent.mkdir(parents=True)
        profile.write_text("keep")
        self.manager.uninstall("0.8", "windows-x86_64")
        self.assertFalse(directory.exists())
        self.assertEqual(profile.read_text(), "keep")

    def test_partial_download_resumes(self):
        part = self.manager.cache / (self.package["sha256"] + ".part")
        part.write_bytes(self.archive[:101])
        self.install()
        self.assertIn(("/game.zip", 101), self.requests)

    def test_bad_download_hash_rejected_before_installation(self):
        self.archive = self.archive[:-1] + bytes([self.archive[-1] ^ 1])
        with self.assertRaises(LauncherError):
            self.install()
        self.assertEqual(self.manager.installed(), [])

    def test_cancel_preserves_partial_and_working_installation(self):
        self.install()
        part = self.manager.cache / (self.package["sha256"] + ".part")
        part.write_bytes(self.archive[:111])
        cancel = threading.Event(); cancel.set()
        with self.assertRaises(LauncherError):
            self.manager.install(self.envelope, "0.8", "windows-x86_64", cancel=cancel)
        self.assertEqual(part.stat().st_size, 111)
        self.manager.verify("0.8", "windows-x86_64")

    def test_production_refuses_plain_http(self):
        production = Manager(self.root / "production", self.keys)
        with self.assertRaises(LauncherError):
            production.load_catalog(self.url + "/catalog.json")

    def test_signature_expiry_and_unknown_key_rejected(self):
        wrong = dict(self.envelope, signature=base64.b64encode(b"0" * 64).decode())
        for envelope, keys in [(wrong, self.keys), (self.envelope, {})]:
            with self.assertRaises(LauncherError):
                decode_catalog(envelope, keys)
        self.prepare(expires=datetime.now(timezone.utc) - timedelta(days=1))
        with self.assertRaises(LauncherError):
            self.install()
        self.assertEqual(decode_catalog(self.envelope, self.keys, fresh=False)["sequence"], 1)

    def test_catalog_rollback_rejected(self):
        self.prepare(sequence=2)
        self.manager.load_catalog(self.url + "/catalog.json")
        self.prepare(sequence=1)
        with self.assertRaises(LauncherError):
            self.manager.load_catalog(self.url + "/catalog.json")

    def test_catalog_and_download_identify_the_launcher(self):
        from launcher import __version__
        self.manager.load_catalog(self.url + "/catalog.json")
        self.install()
        self.assertGreaterEqual(len(self.agents), 2)
        self.assertEqual(set(self.agents), {f"Q3RallyLauncher/{__version__}"})

    def test_bad_repair_preserves_working_installation(self):
        directory = self.install()
        for extra in ("../outside", ".launcher-install.json", "Q3Rally.x86_64.exe"):
            with self.subTest(extra=extra):
                self.prepare(extra=extra)
                with self.assertRaises(LauncherError):
                    self.install()
                self.manager.verify("0.8", "windows-x86_64")
        self.assertTrue(directory.exists())

    def test_interrupted_swap_recovers_verified_previous_installation(self):
        directory = self.install()
        directory.rename(directory.parent / (".previous-" + directory.name))
        recovered = Manager(self.manager.root, self.keys, targets=["windows-x86_64"], development=True)
        self.assertTrue(recovered.verify("0.8", "windows-x86_64").is_file())

    def test_swap_failure_restores_previous_installation(self):
        directory = self.install()
        rename = Path.rename
        def fail_stage(path, destination):
            if path.name.startswith(".stage-"):
                raise OSError("simulated interrupted swap")
            return rename(path, destination)
        with patch.object(Path, "rename", fail_stage):
            with self.assertRaises(OSError):
                self.install()
        self.assertTrue(self.manager.verify("0.8", "windows-x86_64").is_file())
        self.assertFalse((directory.parent / (".previous-" + directory.name)).exists())

    def test_busy_version_cannot_be_reinstalled_or_removed(self):
        directory = self.install()
        with file_lock(self.manager.locks / ("Q3Rally-" + directory.name + ".lock")):
            with self.assertRaises(LauncherError):
                self.install()
        with patch("launcher.core.processes_using", return_value=True):
            with self.assertRaises(LauncherError):
                self.manager.uninstall("0.8", "windows-x86_64")

    def test_offline_launch_uses_own_home_and_checks_files(self):
        self.install()
        with patch("launcher.core.subprocess.Popen") as popen:
            popen.return_value.wait.return_value = 0
            self.assertEqual(self.manager.run("0.8", "windows-x86_64"), 0)
            argv = popen.call_args.args[0]
            self.assertEqual(argv[-3:], ["+set", "fs_homepath", str(self.manager.root / "user-data/0.8")])

    def test_real_downloaded_program_starts_and_logs_arguments(self):
        if os.name == "nt":
            if not shutil.which("gcc"):
                self.skipTest("gcc required to build a Windows launch probe")
            source = self.root / "probe.c"
            binary = self.root / "probe.exe"
            source.write_text('#include <stdio.h>\nint main(int argc,char **argv){for(int i=1;i<argc;i++)puts(argv[i]);return 0;}\n')
            subprocess.run(["gcc", str(source), "-o", str(binary)], check=True, capture_output=True)
            executable = binary.read_bytes()
        else:
            executable = b'#!/bin/sh\nprintf "%s\\n" "$@"\n'
        self.prepare(executable=executable)
        self.install()
        self.assertEqual(self.manager.run("0.8", "windows-x86_64"), 0)
        log = next((self.manager.root / "logs").glob("0.8-*.log")).read_text()
        self.assertIn("fs_homepath", log)
        self.assertIn(str(self.manager.root / "user-data/0.8"), log)

    def test_catalog_publisher_roundtrip_and_development_rejection(self):
        try:
            from cryptography.hazmat.primitives import serialization
            from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
        except ImportError:
            self.skipTest("PEM publishing is tested separately on the 64-bit build host")
        from tools.publish_launcher_catalog import main
        archive = self.root / "game.zip"; archive.write_bytes(self.archive)
        index = self.root / "release.json"
        index.write_text(json.dumps({"product": "Q3Rally", "version": "0.8", "artifacts": [
            {"filename": "game.zip", "target": "windows-x86_64", "size": len(self.archive), "sha256": self.package["sha256"]}]}))
        private = self.root / "temporary-test-key.pem"
        private.write_bytes(Ed25519PrivateKey.from_private_bytes(bytes(self.key)).private_bytes(
            serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
        public = self.root / "config.json"; public.write_text(json.dumps({"public_keys": self.keys}))
        output = self.root / "catalog.json"
        arguments = ["catalog", "--private-key", str(private), "--public-config", str(public), "--key-id", "test",
                     "--release-index", str(index), "--base-url", "https://github.com/example/releases/download/v0.8",
                     "--sequence", "1", "--output", str(output)]
        with patch.dict(os.environ, {"Q3RALLY_SIGNING_PASSWORD": ""}):
            self.assertEqual(main(arguments), 0)
        decoded = decode_catalog(json.loads(output.read_text()), self.keys)
        self.assertTrue(decoded["releases"][0]["packages"]["windows-x86_64"]["url"].startswith("https://github.com/"))
        urls = self.root / "urls.json"
        exact = "https://cdn.sanity.io/files/project/production/asset-hash.zip"
        urls.write_text(json.dumps({"game.zip": exact}))
        with patch.dict(os.environ, {"Q3RALLY_SIGNING_PASSWORD": ""}):
            self.assertEqual(main(arguments + ["--download-urls", str(urls)]), 0)
        decoded = decode_catalog(json.loads(output.read_text()), self.keys)
        self.assertEqual(decoded["releases"][0]["packages"]["windows-x86_64"]["url"], exact)
        data = json.loads(index.read_text()); data["development"] = True; index.write_text(json.dumps(data))
        with patch.dict(os.environ, {"Q3RALLY_SIGNING_PASSWORD": ""}), self.assertRaises(ValueError):
            main(arguments)

    def test_launcher_package_uses_separate_installation_root(self):
        self.prepare(product="Q3RallyLauncher")
        updater = Manager(self.manager.root, self.keys, targets=["windows-x86_64"], development=True, product="Q3RallyLauncher")
        path = updater.install(self.envelope, "0.8", "windows-x86_64")
        self.assertEqual(path.parent.name, "launcher-versions")
        self.assertEqual(self.manager.installed(), [])

    def test_identity_import_never_copies_progress_or_overwrites(self):
        source = self.root / "driver.json"
        source.write_text(json.dumps({"uuid": "12345678-1234-4234-8234-123456789abc", "name": "Driver", "points": 9999}))
        imported = import_profile(source, self.manager.root, "0.8")
        data = json.loads(imported.read_text())
        self.assertEqual(data["identity"]["name"], "Driver")
        self.assertEqual(data["progress"]["currentRank"], 0)
        self.assertNotIn("points", data["progress"]["stats"])
        with self.assertRaises(LauncherError):
            import_profile(source, self.manager.root, "0.8")
        with self.assertRaises(LauncherError):
            import_profile(source, self.manager.root, "0.7d")

    def test_portable_paths_and_version_order(self):
        for path in ("../file", "/file", "C:/file", "CON.txt", "a\\b", "a./b"):
            with self.subTest(path=path), self.assertRaises(LauncherError):
                safe_path(path)
        self.assertEqual(sorted(["0.8", "0.7d", "0.7c"], key=version_key), ["0.7c", "0.7d", "0.8"])

    def test_previously_published_signature_remains_compatible(self):
        fixture = json.loads((Path(__file__).parent / 'fixtures/launcher-legacy-catalog.json').read_text())
        catalog = decode_catalog(fixture['envelope'], fixture['public_keys'], fresh=False)
        self.assertEqual(catalog['sequence'], 4)
        self.assertEqual(catalog['launchers'][0]['version'], '0.2.0')


if __name__ == "__main__":
    unittest.main()
