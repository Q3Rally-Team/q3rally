#!/usr/bin/env python3
"""Build deterministic, audited Q3Rally release archives from existing platform builds."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import plistlib
import re
import shutil
import stat
import sys
import tempfile
import zipfile
from dataclasses import dataclass
from pathlib import Path

from tools.release_assets import Asset, SOURCE_SUFFIXES, audit, inventory, safe_name

ROOT = Path(__file__).resolve().parent
QVMS = ("cgame.qvm", "qagame.qvm", "ui.qvm")
TARGET = re.compile(r"(windows|linux|macos)-(x86|x86_64|arm64|universal)\Z")
STAMP = (1980, 1, 1, 0, 0, 0)


@dataclass(frozen=True)
class File:
    name: str
    path: Path
    executable: bool = False
    symlink: bool = False

    def data(self) -> bytes:
        return os.readlink(self.path).encode() if self.symlink else self.path.read_bytes()


def sha256(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def runtime_plan(target: str, build: Path, version: str, qvm_dir: Path | None = None) -> tuple[list[File], str]:
    match = TARGET.fullmatch(target)
    if not match or (match[1] == "windows" and match[2] not in ("x86", "x86_64")) or (match[1] == "linux" and match[2] == "universal"):
        raise ValueError(f"Unsupported target: {target}")
    platform, arch = match.groups()
    build = build.resolve(strict=True)
    files = []
    if platform == "macos":
        bundle = build if build.suffix.lower() == ".app" else build / "Q3Rally.app"
        if not bundle.is_dir():
            raise ValueError("macOS requires a prepared Q3Rally.app (engine/make-macosx-app.sh)")
        if any(bundle.rglob("_CodeSignature")):
            raise ValueError("Use an unsigned macOS bundle; adding release data would invalidate its signature. Sign the assembled release separately.")
        metadata = plistlib.loads((bundle / "Contents/Info.plist").read_bytes())
        executable = safe_name(metadata["CFBundleExecutable"])
        if "/" in executable:
            raise ValueError("CFBundleExecutable must be a filename")
        bindir = bundle / "Contents/MacOS"
        client = bindir / executable
        for renderer in ("renderer_opengl1", "renderer_opengl2"):
            if not list(bindir.glob(renderer + "*.dylib")):
                raise ValueError(f"Missing macOS renderer: {renderer}")
        for path in sorted(bundle.rglob("*")):
            relative = path.relative_to(bundle)
            if "baseq3r" in relative.parts or any(part.startswith(".") for part in relative.parts):
                continue
            if path.is_symlink():
                if not path.resolve().is_relative_to(bundle.resolve()):
                    raise ValueError(f"Bundle symlink escapes bundle: {relative}")
                files.append(File(bundle.name + "/" + relative.as_posix(), path, symlink=True))
            elif path.is_file():
                if path.suffix.lower() in SOURCE_SUFFIXES:
                    raise ValueError(f"Developer file in prepared app: {relative}")
                files.append(File(bundle.name + "/" + relative.as_posix(), path, bool(path.stat().st_mode & 0o111) or path == client))
        asset_prefix = bundle.name + "/Contents/MacOS/baseq3r"
        vmroot = qvm_dir or (build / "baseq3r/vm" if build.suffix != ".app" else bindir / "baseq3r/vm")
    else:
        ext = ".exe" if platform == "windows" else ""
        library = ".dll" if platform == "windows" else ".so"
        client = build / f"q3rally.{arch}{ext}"
        required = [client, build / f"q3rally-server.{arch}{ext}"] + [build / f"renderer_opengl{i}_{arch}{library}" for i in (1, 2)]
        if platform == "windows":
            required.append(build / "SDL2.dll")
        for path in required:
            if not path.is_file():
                raise ValueError(f"Missing runtime file for {target}: {path.name}")
        chosen = set(required)
        chosen.update(build.glob("*.dll" if platform == "windows" else "*.so*"))
        if (build / "lib").exists():
            chosen.update(p for p in (build / "lib").rglob("*") if p.is_file() and (p.suffix.lower() == ".dll" if platform == "windows" else ".so" in p.name))
        for path in sorted(chosen):
            if not path.resolve().is_relative_to(build):
                raise ValueError(f"Runtime dependency escapes build directory: {path}")
            # Materialize library file links, retaining the loader's expected name.
            files.append(File(path.relative_to(build).as_posix(), path, platform == "linux"))
        asset_prefix = "baseq3r"
        vmroot = qvm_dir or build / "baseq3r/vm"
    if not client.is_file():
        raise ValueError(f"Missing client executable: {client.name}")
    marker = rb"Q3Rally v" + re.escape(version.encode()) + rb"(?=[\x00 +])"
    if not re.search(marker, client.read_bytes()):
        raise ValueError(f"Client version does not match v{version}: rebuild with VERSION=v{version}")
    for name in QVMS:
        path = vmroot / name
        if not path.is_file() or not path.stat().st_size:
            raise ValueError(f"Missing compiled VM: {path}")
        files.append(File(asset_prefix + "/vm/" + name, path))
    names = [safe_name(file.name).lower() for file in files]
    if len(names) != len(set(names)):
        raise ValueError(f"Duplicate runtime paths for {target}")
    return sorted(files, key=lambda file: file.name), asset_prefix


def zip_info(name: str, executable=False, symlink=False, stored=False) -> zipfile.ZipInfo:
    info = zipfile.ZipInfo(safe_name(name), STAMP)
    info.create_system = 3
    info.external_attr = ((stat.S_IFLNK | 0o777) if symlink else (stat.S_IFREG | (0o755 if executable else 0o644))) << 16
    info.compress_type = zipfile.ZIP_STORED if stored else zipfile.ZIP_DEFLATED
    return info


def write_stream(archive, info, stream):
    digest, size = hashlib.sha256(), 0
    with archive.open(info, "w", force_zip64=True) as output:
        while block := stream.read(1024 * 1024):
            output.write(block); digest.update(block); size += len(block)
    return digest.hexdigest(), size


def make_asset_pk3(path: Path, assets: dict[str, Asset]) -> list[dict]:
    records = []
    with zipfile.ZipFile(path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for asset in sorted(assets.values(), key=lambda value: value.name):
            digest = hashlib.sha256()
            size = 0
            with asset.open() as source, archive.open(zip_info(asset.name), "w", force_zip64=True) as output:
                while block := source.read(1024 * 1024):
                    output.write(block); digest.update(block); size += len(block)
            records.append({"path": asset.name, "size": size, "sha256": digest.hexdigest()})
    return records


def package(path: Path, target: str, version: str, files: list[File], prefix: str, pk3: Path, asset_records: list[dict]) -> dict:
    root = f"Q3Rally-v{version}-{target}/"
    records = []
    with zipfile.ZipFile(path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for file in files:
            info = zip_info(root + file.name, file.executable, file.symlink)
            if file.symlink:
                data = file.data(); archive.writestr(info, data)
                digest, size = hashlib.sha256(data).hexdigest(), len(data)
            else:
                with file.path.open("rb") as stream:
                    digest, size = write_stream(archive, info, stream)
            records.append({"path": file.name, "size": size, "sha256": digest, "symlink": file.symlink})
        documentation = [(ROOT / "engine/docs/COPYING.txt", "COPYING-engine.txt"), (ROOT / "changelog.txt", "changelog.txt")]
        for vendor in sorted((ROOT / "engine/code").iterdir()):
            if vendor.is_dir():
                for license_name in ("COPYING", "COPYING.txt", "LICENSE", "LICENSE.txt", "LICENSE.md"):
                    if (vendor / license_name).is_file():
                        documentation.append((vendor / license_name, f"licenses/{vendor.name}/{license_name}"))
        for source, name in [(pk3, prefix + "/pak0.pk3")] + documentation:
            with source.open("rb") as stream:
                digest, size = write_stream(archive, zip_info(root + name, stored=source == pk3), stream)
            records.append({"path": name, "size": size, "sha256": digest})
        readme = (f"Q3Rally v{version} - {target}\n\n"
                  "Extract the entire archive before starting the game.\n"
                  "Windows/Linux: run the q3rally executable in this directory.\n"
                  "macOS: open Q3Rally.app.\n"
                  "Keep the baseq3r data, renderer libraries and runtime dependencies together.\n"
                  "This package contains no personal settings, profiles or demos.\n"
                  "See changelog.txt, COPYING-engine.txt and bundled asset credits.\n"
                  "release-manifest.json lists the packaged files and SHA-256 checksums.\n").encode()
        archive.writestr(zip_info(root + "README.txt"), readme)
        records.append({"path": "README.txt", "size": len(readme), "sha256": hashlib.sha256(readme).hexdigest()})
        manifest = {"schema": 1, "product": "Q3Rally", "version": version, "target": target,
                    "client_version_verified": True, "files": records, "assets": asset_records,
                    "limitations": ["Platform dependency closure and launch tests must be verified on the target OS.", "QVM provenance must be verified by the build pipeline."]}
        archive.writestr(zip_info(root + "release-manifest.json"), json.dumps(manifest, sort_keys=True, indent=2) + "\n")
    return {"target": target, "filename": path.name, "sha256": sha256(path), "size": path.stat().st_size}


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", default="0.8")
    parser.add_argument("--assets", type=Path, default=ROOT / "baseq3r")
    parser.add_argument("--policy", type=Path, default=ROOT / "tools/release_policy.json")
    parser.add_argument("--target", action="append", default=[], metavar="PLATFORM-ARCH=BUILD_DIRECTORY")
    parser.add_argument("--qvm-dir", type=Path, help="Optional common directory of platform-independent QVMs")
    parser.add_argument("--output", type=Path, help="Default: dist/releases/vVERSION")
    parser.add_argument("--report", type=Path, help="Write JSON audit/preflight report (also on failure)")
    parser.add_argument("--audit-only", action="store_true")
    parser.add_argument("--dry-run", action="store_true", help="Run all preflight checks without writing release archives")
    args = parser.parse_args(argv)
    version = args.version.removeprefix("v")
    if not re.fullmatch(r"[0-9]+\.[0-9]+[A-Za-z0-9.-]*", version):
        parser.error("Invalid release version")
    if not args.audit_only and not args.target:
        parser.error("Provide at least one --target, or use --audit-only")
    try:
        policy = json.loads(args.policy.read_text(encoding="utf-8"))
        assets, _ = inventory(args.assets.resolve(), policy)
        report = audit(assets)
        if "default.cfg" not in assets:
            report["errors"].append({"source": "release", "kind": "required", "reference": "default.cfg"})
        report["version"], report["targets"] = version, []
        plans = {}
        for spec in args.target:
            name, separator, directory = spec.partition("=")
            try:
                if not separator or name in plans:
                    raise ValueError("Use unique PLATFORM-ARCH=BUILD_DIRECTORY targets")
                files, prefix = runtime_plan(name, Path(directory), version, args.qvm_dir)
                plans[name] = files, prefix
                report["targets"].append({"target": name, "file_count": len(files), "client_version_verified": True})
            except (ValueError, OSError, KeyError) as error:
                report["errors"].append({"source": name, "kind": "runtime", "reference": str(error)})
        if args.report:
            if args.report.name in ("release.json", "SHA256SUMS") or args.report.suffix.lower() != ".json":
                raise ValueError("Use a separate .json audit report path, not a release catalog")
            args.report.parent.mkdir(parents=True, exist_ok=True)
            args.report.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        print(f"Assets: {len(assets)}; errors: {len(report['errors'])}; warnings: {len(report['warnings'])}")
        for finding in report["errors"][:10]:
            print(f"  {finding['source']}: {finding['reference']}", file=sys.stderr)
        if report["errors"]:
            return 1
        if args.audit_only or args.dry_run:
            return 0
        output = (args.output or ROOT / "dist/releases" / f"v{version}").resolve()
        output.mkdir(parents=True, exist_ok=True)
        names = [f"Q3Rally-v{version}-{name}.zip" for name in plans] + ["SHA256SUMS", "release.json"]
        for name in names:
            if (output / name).exists():
                raise ValueError(f"Refusing to overwrite {output / name}; use a new output directory")
        with tempfile.TemporaryDirectory(prefix=".q3r-release-", dir=output) as temporary:
            stage = Path(temporary).resolve()
            if not stage.is_relative_to(output):
                raise ValueError("Staging directory escapes output directory")
            pk3 = stage / "pak0.pk3"
            records = make_asset_pk3(pk3, assets)
            artifacts = []
            for name, (files, prefix) in plans.items():
                artifacts.append(package(stage / f"Q3Rally-v{version}-{name}.zip", name, version, files, prefix, pk3, records))
            (stage / "release.json").write_text(json.dumps({"schema": 1, "product": "Q3Rally", "version": version, "artifacts": artifacts, "signed": False}, indent=2, sort_keys=True) + "\n", encoding="utf-8")
            (stage / "SHA256SUMS").write_text("".join(f"{a['sha256']}  {a['filename']}\n" for a in artifacts), encoding="utf-8")
            # Publish fully written files without overwriting a concurrent release.
            published = []
            try:
                for name in names:
                    if os.name == "nt":
                        os.rename(stage / name, output / name) # Windows rename refuses existing files
                    else:
                        os.link(stage / name, output / name)
                    published.append(output / name)
            except OSError:
                for path in published:
                    path.unlink()
                raise
        print(f"Release written to {output}")
        return 0
    except (OSError, ValueError, KeyError, zipfile.BadZipFile) as error:
        print(f"Release failed: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
