"""Build a native launcher bundle and a manifest-compatible update ZIP."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import stat
import subprocess
import sys
import zipfile
import tkinter

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from launcher import __version__
from launcher.core import atomic_json, digest_file, native_targets, safe_path


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "launcher/config.json")
    parser.add_argument("--development", action="store_true", help="Build without a production trust key; not for publishing")
    args = parser.parse_args(argv)
    config = json.loads(args.config.read_text())
    if not config.get("public_keys") and not args.development:
        raise ValueError("Configure the public signing key before building a production launcher")
    # Fail early instead of producing a package that only supports CLI commands.
    tkinter.Tcl().eval("info library")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    target = native_targets()[0]
    command = [sys.executable, "-m", "PyInstaller", "--noconfirm", "--clean", "--onedir", "--windowed",
        "--name", "Q3RallyLauncher", "--paths", str(ROOT), "--specpath", str(output / "spec"),
        "--distpath", str(output / "dist"), "--workpath", str(output / "build"),
        "--copy-metadata", "psutil", "--copy-metadata", "PyNaCl", "--copy-metadata", "cffi",
        "--hidden-import", "_cffi_backend",
        "--add-data", str(ROOT / "engine/docs/COPYING.txt") + os.pathsep + ".",
        "--add-data", str(ROOT / "launcher/assets") + os.pathsep + "launcher/assets",
        "--add-data", str(args.config.resolve()) + os.pathsep + "launcher", str(ROOT / "run_launcher.py")]
    if sys.platform == "darwin":
        command += ["--osx-bundle-identifier", "com.q3rally.launcher"]
    subprocess.run(command, cwd=ROOT, check=True)
    if sys.platform == "darwin":
        bundle = output / "dist/Q3RallyLauncher.app"
        entrypoint = "Q3RallyLauncher.app/Contents/MacOS/Q3RallyLauncher"
        files = [(p, "Q3RallyLauncher.app/" + p.relative_to(bundle).as_posix()) for p in bundle.rglob("*") if p.is_file() or p.is_symlink()]
    else:
        bundle = output / "dist/Q3RallyLauncher"
        entrypoint = "Q3RallyLauncher" + (".exe" if os.name == "nt" else "")
        files = [(p, p.relative_to(bundle).as_posix()) for p in bundle.rglob("*") if p.is_file() or p.is_symlink()]
    filename = f"Q3RallyLauncher-v{__version__}-{target}.zip"
    prefix = filename[:-4] + "/"
    records = []
    with zipfile.ZipFile(output / filename, "w", zipfile.ZIP_DEFLATED) as archive:
        for path, name in sorted(files):
            safe_path(name)
            symlink = path.is_symlink()
            if symlink and not path.resolve().is_relative_to(bundle.resolve()):
                raise ValueError("Launcher bundle symlink escapes its root")
            data = os.readlink(path).encode() if symlink else path.read_bytes()
            info = zipfile.ZipInfo(prefix + name, (1980, 1, 1, 0, 0, 0))
            info.create_system = 3
            mode = stat.S_IFLNK | 0o777 if symlink else stat.S_IFREG | (0o755 if path.stat().st_mode & 0o111 or name == entrypoint else 0o644)
            info.external_attr = mode << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, data)
            records.append({"path": name, "size": len(data), "sha256": hashlib.sha256(data).hexdigest(), "symlink": symlink})
        manifest = {"schema": 1, "product": "Q3RallyLauncher", "version": __version__, "target": target,
                    "entrypoint": entrypoint, "development": args.development, "files": records}
        archive.writestr(prefix + "release-manifest.json", json.dumps(manifest, sort_keys=True, indent=2) + "\n")
    package = output / filename
    atomic_json(output / "release.json", {"schema": 1, "product": "Q3RallyLauncher", "version": __version__,
        "development": args.development, "artifacts": [{"target": target, "filename": filename,
            "size": package.stat().st_size, "sha256": digest_file(package)}]})
    print(package)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
