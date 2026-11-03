"""Virtual-file asset inventory and static dependency checks for release packages."""
from __future__ import annotations

import contextlib
import fnmatch
import hashlib
import json
import re
import struct
import zipfile
from dataclasses import dataclass
from pathlib import Path, PurePosixPath

IMAGES = (".tga", ".jpg", ".jpeg", ".png", ".pcx")
SOUNDS = (".wav", ".ogg", ".mp3")
SOURCE_SUFFIXES = {".map", ".blend", ".fbx", ".obj", ".mtl", ".py", ".ps1", ".bat", ".sh", ".o", ".asm", ".bak", ".log"}


def safe_name(name: str) -> str:
    name = name.replace("\\", "/")
    path = PurePosixPath(name)
    if not name or name.startswith("/") or ":" in name or any(p in ("", ".", "..") for p in name.split("/")):
        raise ValueError(f"Unsafe archive path: {name!r}")
    return path.as_posix()


def selected(name: str, policy: dict) -> bool:
    name = safe_name(name)
    parts = PurePosixPath(name).parts
    if any(p.startswith(".") for p in parts) or any(fnmatch.fnmatchcase(name.lower(), x.lower()) for x in policy["exclude"]):
        return False
    suffix = PurePosixPath(name).suffix.lower()
    if suffix in SOURCE_SUFFIXES or (suffix in (".c", ".h") and parts[0] != "botfiles"):
        return False
    if len(parts) == 1:
        return any(fnmatch.fnmatchcase(name, pattern) for pattern in policy["root_files"])
    return parts[0] in policy["roots"] and (suffix in policy["extensions"] or parts[-1].lower() in ("readme", "credits", "license"))


@dataclass(frozen=True)
class Asset:
    name: str
    source: Path
    member: str | None = None

    @contextlib.contextmanager
    def open(self):
        if self.member is None:
            with self.source.open("rb") as stream:
                yield stream
        else:
            with zipfile.ZipFile(self.source) as archive, archive.open(self.member) as stream:
                yield stream

    def read(self) -> bytes:
        with self.open() as stream:
            return stream.read()

    def digest(self) -> str:
        with self.open() as stream:
            return hashlib.file_digest(stream, "sha256").hexdigest()


def inventory(base: Path, policy: dict) -> tuple[dict[str, Asset], list[dict]]:
    if not base.is_dir():
        raise ValueError(f"Asset directory not found: {base}")
    result: dict[str, Asset] = {}
    findings = []
    # Normal VFS layering: later PK3 names override earlier ones; loose assets win.
    for package in sorted(base.glob("*.pk3")):
        with zipfile.ZipFile(package) as archive:
            seen = set()
            for info in archive.infolist():
                if info.is_dir():
                    continue
                name = safe_name(info.filename)
                if (info.external_attr >> 16) & 0o170000 == 0o120000:
                    raise ValueError(f"Symlink inside asset PK3: {package.name}:{name}")
                if name.lower() in seen:
                    raise ValueError(f"Duplicate/case-colliding path in {package.name}: {name}")
                seen.add(name.lower())
                if selected(name, policy):
                    result[name.lower()] = Asset(name, package, info.filename)
    loose = {}
    for path in sorted(base.rglob("*")):
        if path.is_symlink():
            raise ValueError(f"Asset symlinks must be materialized before release: {path}")
        if not path.is_file():
            continue
        name = path.relative_to(base).as_posix()
        if selected(name, policy):
            key = name.lower()
            if key in loose:
                raise ValueError(f"Case-colliding loose assets: {name} / {loose[key].name}")
            loose[key] = Asset(name, path)
    result.update(loose)
    return result, findings


def strip_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\n]*", " ", text, flags=re.S)


def audit(assets: dict[str, Asset]) -> dict:
    errors, warnings, shaders = [], [], set()
    shader_texts = {}
    for key, asset in assets.items():
        if key.endswith(".shader"):
            text = strip_comments(asset.read().decode("utf-8", "replace"))
            shader_texts[key] = text
            tokens = re.findall(r'"[^"\n]*"|[^\s{}]+|[{}]', text)
            depth, previous = 0, ""
            for token in tokens:
                if token == "{":
                    if depth == 0:
                        shaders.add(previous.strip('"').lower())
                    depth += 1
                elif token == "}":
                    depth -= 1
                    if depth < 0:
                        errors.append({"source": asset.name, "kind": "shader-syntax", "reference": "unbalanced braces"})
                        depth = 0
                else:
                    previous = token
            if depth:
                errors.append({"source": asset.name, "kind": "shader-syntax", "reference": "unbalanced braces"})

    def exists(ref, kind):
        ref = ref.replace("\\", "/").lstrip("/").lower()
        if ref.startswith(("$", "*")) or ref in ("-", "none", "full", "_white", "_black", "noshader"):
            return True
        if kind == "material" and ref in shaders:
            return True
        if ref in assets:
            return True
        suffixes = SOUNDS if kind == "sound" else IMAGES if kind in ("image", "material") else ()
        stem = str(PurePosixPath(ref).with_suffix("")) if PurePosixPath(ref).suffix.lower() in suffixes else ref
        return (kind == "material" and stem in shaders) or any(stem + ext in assets for ext in suffixes)

    def check(source, ref, kind="file", optional=False):
        ref = ref.strip().strip('"')
        if ref and not exists(ref, kind):
            (warnings if optional else errors).append({"source": source, "kind": kind, "reference": ref})

    for key, asset in sorted(assets.items()):
        try:
            if key.endswith(".shader"):
                for line in shader_texts[key].splitlines():
                    tokens = re.findall(r'"[^"\n]*"|[^\s{}]+', line)
                    if not tokens:
                        continue
                    op = tokens[0].lower()
                    if op in ("map", "clampmap", "normalmap", "specularmap", "diffusemap", "q3r_water") and len(tokens) >= 2:
                        check(asset.name, tokens[1], "image")
                    elif op == "animmap":
                        for token in tokens[2:]:
                            check(asset.name, token, "image")
                    elif op in ("qer_editorimage", "q3map_lightimage") and len(tokens) >= 2:
                        check(asset.name, tokens[1], "image", optional=True)
                    elif op == "skyparms" and len(tokens) >= 2:
                        if tokens[1] != "-":
                            for face in ("bk", "dn", "ft", "lf", "rt", "up"):
                                check(asset.name, tokens[1].strip('"') + "_" + face, "image")
            elif key.endswith(".skin"):
                for line in strip_comments(asset.read().decode("utf-8", "replace")).splitlines():
                    if "," in line:
                        surface, material = line.split(",", 1)
                        if not surface.strip().startswith("tag_"):
                            check(asset.name, material, "material")
            elif key.endswith(".arena"):
                text = strip_comments(asset.read().decode("utf-8", "replace"))
                for match in re.finditer(r'\bmap\s+"?([^\s"{}]+)', text, re.I):
                    check(asset.name, f"maps/{match[1]}.bsp")
            elif key.endswith(".bsp"):
                data = asset.read()
                if len(data) < 144 or data[:4] != b"IBSP" or struct.unpack_from("<i", data, 4)[0] != 46:
                    raise ValueError("unsupported or truncated IBSP header")
                lumps = [struct.unpack_from("<ii", data, 8 + i * 8) for i in range(17)]
                for offset, length in lumps:
                    if offset < 0 or length < 0 or offset + length > len(data):
                        raise ValueError("invalid BSP lump bounds")
                offset, length = lumps[1]
                if length % 72:
                    raise ValueError("invalid texture lump size")
                texture_offset, texture_length = offset, length
                face_offset, face_length = lumps[13]
                if face_length % 104:
                    raise ValueError("invalid draw-surface lump size")
                used = {struct.unpack_from("<i", data, i)[0] for i in range(face_offset, face_offset + face_length, 104)}
                if any(index < 0 or index >= texture_length // 72 for index in used):
                    raise ValueError("invalid draw-surface material index")
                for index, i in enumerate(range(texture_offset, texture_offset + texture_length, 72)):
                    check(asset.name, data[i:i + 64].split(b"\0")[0].decode("ascii", "replace"), "material", optional=index not in used)
                offset, length = lumps[0]
                entities = data[offset:offset + length].decode("utf-8", "replace")
                for entity in re.findall(r"\{([^{}]*)\}", entities):
                    values = dict(re.findall(r'"([^"\n]+)"\s*"([^"\n]*)"', entity))
                    for name, value in values.items():
                        if name.lower() in ("model", "model2"):
                            check(asset.name, value, optional=values.get("classname") == "misc_model")
                        elif name.lower() in ("music", "scoreboardmusic", "noise", "sound"):
                            for sound in value.split():
                                check(asset.name, sound, "sound")
                        elif name.lower() in ("reflectionimage", "colorlut", "_colorlut"):
                            check(asset.name, value, "image")
            elif key.endswith(".script"):
                text = strip_comments(asset.read().decode("utf-8", "replace"))
                for name, value in re.findall(r'\b(model|deadmodel|presound|hitsound|destroysound|postsound)\s+"([^"\n]+)"', text, re.I):
                    if name.lower().endswith("sound"):
                        check(asset.name, value, "sound")
                    elif value.lower().startswith("models/"):
                        check(asset.name, value if PurePosixPath(value).suffix else value + ".md3")
            elif key.endswith(".cfg"):
                text = strip_comments(asset.read().decode("utf-8", "replace"))
                for ref in re.findall(r'\bexec\s+"?([^\s";]+)', text, re.I):
                    check(asset.name, ref if ref.lower().endswith(".cfg") else ref + ".cfg")
            elif key.endswith(".md3"):
                data = asset.read()
                if len(data) < 108 or data[:4] != b"IDP3":
                    raise ValueError("invalid MD3 header")
                count, offset = struct.unpack_from("<i", data, 84)[0], struct.unpack_from("<i", data, 100)[0]
                if count < 0 or count > 1024:
                    raise ValueError("invalid MD3 surface count")
                for _ in range(count):
                    if offset < 108 or offset + 108 > len(data):
                        raise ValueError("invalid MD3 surface bounds")
                    values = struct.unpack_from("<10i", data, offset + 68)
                    n, shaders_offset, end = values[2], values[6], values[9]
                    if n < 0 or n > 4096 or end < 108 or offset + end > len(data) or shaders_offset < 0 or shaders_offset + n * 68 > end:
                        raise ValueError("invalid MD3 shader bounds")
                    for i in range(n):
                        pos = offset + shaders_offset + i * 68
                        ref = data[pos:pos + 64].split(b"\0")[0].decode("ascii", "replace")
                        # Skins can override embedded material names, so these are warnings.
                        check(asset.name, ref, "material", optional=True)
                    offset += end
        except (ValueError, struct.error) as error:
            errors.append({"source": asset.name, "kind": "format", "reference": str(error)})
    def unique(items):
        return [dict(source=s, kind=k, reference=r) for s,k,r in sorted({(i["source"],i["kind"],i["reference"]) for i in items})]
    return {"asset_count": len(assets), "shader_count": len(shaders), "errors": unique(errors), "warnings": unique(warnings),
            "scope": "Static shader/skybox, skin, arena, BSP texture/entity, MD3 material, scripted-object and cfg exec references; dynamic code/menu/bot references require runtime validation."}
