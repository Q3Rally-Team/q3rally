#!/usr/bin/env python3
"""Package the compiled Crossroads map and its image/model dependencies."""
import argparse
import hashlib
import json
import re
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BASE = ROOT / "baseq3r"
NAME = "q3r_crossroads"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-only", action="store_true", help="Refresh sources/docs while keeping the tested PK3 byte-identical")
    args = parser.parse_args()
    shader = BASE / "scripts" / f"{NAME}.shader"
    text = shader.read_text(encoding="utf-8")
    files = {f"maps/{NAME}.bsp", f"maps/{NAME}.map", f"maps/{NAME}_index.pcx", f"scripts/{NAME}.shader",
             f"scripts/{NAME}.arena", f"levelshots/{NAME}.jpg",
             "models/mapobjects/GR_trees/nadelbaum.md3",
             "models/mapobjects/GR_trees/nadel.tga", "models/mapobjects/GR_trees/stamm.jpg"}
    files.update(re.findall(r"^\s*map\s+(textures/\S+)", text, re.M))
    files.update(re.findall(r"^\s*q3r_water\s+(textures/\S+)", text, re.M))
    for sky in re.findall(r"^\s*skyparms\s+(textures/\S+)", text, re.M):
        files.update(f"{sky}_{side}.tga" for side in ("bk", "dn", "ft", "lf", "rt", "up"))
    # Standard Q3Rally textures and models ship with the game; only the
    # map's own files go into the PK3.
    files = {f for f in files if not f.startswith(("textures/", "models/"))
             or f.startswith(f"textures/{NAME}/")}
    for name in sorted(files):
        if not (BASE / name).is_file():
            raise FileNotFoundError(BASE / name)
    dist = ROOT / "dist"
    dist.mkdir(exist_ok=True)
    package = dist / f"{NAME}_v0.2.pk3"
    if not args.source_only:
        with zipfile.ZipFile(package, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
            for name in sorted(files):
                archive.write(BASE / name, name)
            archive.write(ROOT / "docs/maps/crossroads/README.md", "docs/crossroads/README.md")
    with zipfile.ZipFile(package) as archive:
        assert archive.testzip() is None
        assert len(archive.namelist()) == len(set(archive.namelist()))
    sources = dist / f"{NAME}_v0.2_source.zip"
    source_files = [ROOT / "tools/maptools" / f for f in
                    ("make_crossroads.py", "crossroads_textures.py", "validate_crossroads.py",
                     "package_crossroads.py", "build_crossroads.ps1")]
    source_files += [BASE / f for f in (f"maps/{NAME}.map", f"scripts/{NAME}.shader", f"scripts/{NAME}.arena")]
    source_files += list((ROOT / "docs/maps/crossroads").glob("*"))
    with zipfile.ZipFile(sources, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for path in sorted(source_files):
            if path.is_file():
                archive.write(path, path.relative_to(ROOT).as_posix())
    report = {"package": package.name, "sha256": hashlib.sha256(package.read_bytes()).hexdigest(),
              "bytes": package.stat().st_size, "files": sorted(files),
              "requires": "Q3Rally with CTF4 (g_gametype 20); base-game flags, vehicles and tree shaders"}
    (dist / "crossroads-work/package-manifest.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PK3: {package} ({package.stat().st_size:,} bytes)")
    print(f"Source: {sources} ({sources.stat().st_size:,} bytes)")
    print(f"SHA256: {report['sha256']}")


if __name__ == "__main__":
    main()
