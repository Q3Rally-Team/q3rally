import hashlib
import io
import json
import plistlib
import struct
import tempfile
import unittest
from unittest import mock
import zipfile
from pathlib import Path

import make_release as release
from tools.release_assets import audit, inventory

ROOT = Path(__file__).resolve().parents[1]


def bsp(material="textures/demo/material"):
    entities = b'{ "classname" "worldspawn" }\0'
    texture = material.encode().ljust(64, b"\0") + struct.pack("<ii", 0, 0)
    surface = struct.pack("<i", 0) + bytes(100)
    lumps, payload = [], bytearray()
    for i in range(17):
        data = entities if i == 0 else texture if i == 1 else surface if i == 13 else b""
        lumps.append((144 + len(payload), len(data))); payload.extend(data)
    return b"IBSP" + struct.pack("<i", 46) + b"".join(struct.pack("<ii", *lump) for lump in lumps) + payload


class ReleaseTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=ROOT / "tests")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.base = self.root / "baseq3r"
        self.policy = json.loads((ROOT / "tools/release_policy.json").read_text())
        self.put("default.cfg", b"seta name Player\n")
        self.put("textures/demo/plain.tga", b"image")
        self.put("scripts/demo.shader", b"textures/demo/material\n{\n {\n map textures/demo/plain.tga\n }\n}\n")
        self.put("models/demo.skin", b"body,textures/demo/material.tga\n") # engine strips image suffix for shader lookup
        self.put("maps/demo.bsp", bsp())
        self.put("scripts/demo.arena", b'{ map "demo" }')
        header = bytearray(108); header[:4] = b"IDP3"; struct.pack_into("<i", header, 100, 108)
        self.put("models/object.md3", header)
        self.put("models/object.script", b'rally_scripted_object {\n model "models/object.md3"\n hitsound "sound/hit.wav"\n}')
        self.put("sound/hit.wav", b"sound")
        self.put("sound/CREDITS", b"Attribution")
        self.put("video/idlogo.ogv", b"video")
        self.put("maps/demo.map", b"editor source")
        self.put("profiles/private.cfg", b"PASSWORD")
        self.put("q3config.cfg", b"PRIVATE SETTINGS")
        self.put("server_private.cfg", b"PRIVATE SERVER CONFIG")
        self.put("textures/demo/debug.py", b"developer source")
        self.vm = self.root / "vm"; self.vm.mkdir()
        for name in release.QVMS:
            (self.vm / name).write_bytes(b"QVM")

    def put(self, name, data):
        path = self.base / name; path.parent.mkdir(parents=True, exist_ok=True); path.write_bytes(data)
        return path

    def build(self, target, version="0.8"):
        path = self.root / target; path.mkdir()
        platform, arch = target.split("-", 1)
        if platform == "macos":
            bundle = path / "Q3Rally.app"; binary = bundle / "Contents/MacOS"; binary.mkdir(parents=True)
            (bundle / "Contents/Info.plist").write_bytes(plistlib.dumps({"CFBundleExecutable": "q3rally"}))
            (binary / "q3rally").write_bytes(f"Q3Rally v{version}\0".encode())
            for i in (1, 2): (binary / f"renderer_opengl{i}.dylib").write_bytes(b"renderer")
        else:
            extension = ".exe" if platform == "windows" else ""
            (path / f"q3rally.{arch}{extension}").write_bytes(f"Q3Rally v{version}\0".encode())
            (path / f"q3rally-server.{arch}{extension}").write_bytes(b"server")
            for i in (1, 2): (path / f"renderer_opengl{i}_{arch}{'.dll' if platform == 'windows' else '.so'}").write_bytes(b"renderer")
            if platform == "windows": (path / "SDL2.dll").write_bytes(b"SDL2")
        return path

    def test_three_platform_packages_and_reproducibility(self):
        targets = ["windows-x86_64", "linux-x86_64", "macos-arm64"]
        args = ["--assets", str(self.base), "--qvm-dir", str(self.vm)]
        for target in targets: args += ["--target", f"{target}={self.build(target)}"]
        outputs = [self.root / "out1", self.root / "out2"]
        for out in outputs:
            self.assertEqual(release.main(args + ["--output", str(out)]), 0)
        for target in targets:
            name = f"Q3Rally-v0.8-{target}.zip"
            self.assertEqual(release.sha256(outputs[0] / name), release.sha256(outputs[1] / name))
            with zipfile.ZipFile(outputs[0] / name) as archive:
                manifest_name = next(name for name in archive.namelist() if name.endswith("release-manifest.json"))
                manifest = json.loads(archive.read(manifest_name)); root = manifest_name.removesuffix("release-manifest.json")
                for record in manifest["files"]:
                    self.assertEqual(hashlib.sha256(archive.read(root + record["path"])).hexdigest(), record["sha256"])
                pk3 = next(name for name in archive.namelist() if name.endswith("pak0.pk3"))
                vm_pk3 = next(name for name in archive.namelist() if name.endswith("qvms.pk3"))
                with zipfile.ZipFile(io.BytesIO(archive.read(vm_pk3))) as vms:
                    self.assertEqual(sorted(vms.namelist()), sorted("vm/" + name for name in release.QVMS))
                    for info in vms.infolist():
                        self.assertEqual(vms.read(info), b"QVM")
                        self.assertLessEqual(info.extract_version, 20)
                self.assertFalse(any(name.endswith(".qvm") for name in archive.namelist()))
                with zipfile.ZipFile(io.BytesIO(archive.read(pk3))) as assets:
                    # Q3's legacy minizip must be able to read every local header.
                    pk3_bytes = archive.read(pk3)
                    for info in assets.infolist():
                        header = struct.unpack_from("<4s5H3I2H", pk3_bytes, info.header_offset)
                        self.assertEqual(header[0], b"PK\x03\x04")
                        self.assertLessEqual(header[1], 20)
                        self.assertNotEqual(header[7], 0xffffffff)
                        self.assertNotEqual(header[8], 0xffffffff)
                    self.assertIn("maps/demo.bsp", assets.namelist())
                    self.assertIn("models/object.script", assets.namelist())
                    self.assertIn("video/idlogo.ogv", assets.namelist())
                    self.assertIn("sound/CREDITS", assets.namelist())
                    self.assertNotIn("maps/demo.map", assets.namelist())
                    self.assertFalse(any("private" in name or name.endswith(".py") or "q3config" in name for name in assets.namelist()))
                if target.startswith("linux"):
                    info = archive.getinfo(root + "q3rally.x86_64")
                    self.assertTrue((info.external_attr >> 16) & 0o111)
        self.assertEqual(release.main(args + ["--output", str(outputs[0])]), 1)

    def test_missing_dynamic_curl_blocks_preflight(self):
        build = self.build("windows-x86_64")
        client = build / "q3rally.x86_64.exe"
        client.write_bytes(client.read_bytes() + b"cl_cURLLib\0libcurl-4.dll\0")
        with self.assertRaisesRegex(ValueError, "libcurl"):
            release.runtime_plan("windows-x86_64", build, "0.8", self.vm)
        (build / "libcurl-4.dll").write_bytes(b"curl")
        files, _ = release.runtime_plan("windows-x86_64", build, "0.8", self.vm)
        self.assertIn("libcurl-4.dll", [file.name for file in files])

    def test_missing_asset_blocks_publication(self):
        self.put("textures/demo/plain.tga", b"image").unlink()
        out = self.root / "output"
        args = ["--assets", str(self.base), "--qvm-dir", str(self.vm), "--target", f"windows-x86_64={self.build('windows-x86_64')}", "--output", str(out)]
        self.assertEqual(release.main(args), 1); self.assertFalse(out.exists())

    def test_version_and_required_binaries(self):
        build = self.build("windows-x86_64", "0.7d")
        with self.assertRaisesRegex(ValueError, "version"):
            release.runtime_plan("windows-x86_64", build, "0.8", self.vm)
        (build / "SDL2.dll").unlink()
        with self.assertRaisesRegex(ValueError, "SDL2"):
            release.runtime_plan("windows-x86_64", build, "0.8", self.vm)

    def test_pk3_layering_and_traversal(self):
        with zipfile.ZipFile(self.base / "pak0.pk3", "w") as z:
            z.writestr("textures/demo/plain.tga", b"older")
            z.writestr("textures/demo/packed.tga", b"packed")
            z.writestr("profiles/secret.cfg", b"never ship")
        files, _ = inventory(self.base, self.policy)
        self.assertEqual(files["textures/demo/plain.tga"].read(), b"image")
        self.assertEqual(files["textures/demo/packed.tga"].read(), b"packed")
        self.assertNotIn("profiles/secret.cfg", files)
        with zipfile.ZipFile(self.base / "bad.pk3", "w") as z: z.writestr("../escape.txt", b"bad")
        with self.assertRaisesRegex(ValueError, "Unsafe"):
            inventory(self.base, self.policy)

    def test_models_bsp_bounds_and_asset_formats(self):
        self.put("models/bad.md3", b"<html>not a model</html>")
        self.put("maps/bad.bsp", b"IBSP")
        files, _ = inventory(self.base, self.policy)
        report = audit(files)
        self.assertTrue(any(item["source"] == "models/bad.md3" and item["kind"] == "format" for item in report["errors"]))
        self.assertTrue(any(item["source"] == "maps/bad.bsp" and item["kind"] == "format" for item in report["errors"]))
        self.assertFalse(any(item["source"] == "models/demo.skin" for item in report["errors"]))

    def test_publish_failure_rolls_back_artifacts(self):
        out = self.root / "output"
        args = ["--assets", str(self.base), "--qvm-dir", str(self.vm), "--target", f"linux-x86_64={self.build('linux-x86_64')}", "--output", str(out)]
        operation = "rename" if release.os.name == "nt" else "link"
        original = getattr(release.os, operation)
        calls = 0
        def fail_second(*args):
            nonlocal calls
            calls += 1
            if calls == 2: raise OSError("simulated publication failure")
            return original(*args)
        with mock.patch.object(release.os, operation, side_effect=fail_second):
            self.assertEqual(release.main(args), 1)
        self.assertEqual(list(out.iterdir()), [])

    def test_dry_run_writes_report_but_no_release(self):
        out, report = self.root / "output", self.root / "report.json"
        args = ["--assets", str(self.base), "--qvm-dir", str(self.vm), "--target", f"linux-x86_64={self.build('linux-x86_64')}", "--output", str(out), "--report", str(report), "--dry-run"]
        self.assertEqual(release.main(args), 0); self.assertTrue(report.is_file()); self.assertFalse(out.exists())


if __name__ == "__main__":
    unittest.main()
