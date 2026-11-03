"""Prepare signed catalogs for webspace hosting; never uploads or publishes."""
import argparse
import base64
from datetime import datetime, timedelta, timezone
import hashlib
import json
import os
from pathlib import Path
import plistlib
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from launcher.core import atomic_json, check_url, decode_catalog, digest_file, safe_path, version_key


def sign(payload, key, key_id):
    raw = json.dumps(payload, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode()
    signed = key.sign(raw)
    signature = getattr(signed, "signature", signed)
    return {"key_id": key_id, "payload": base64.b64encode(raw).decode(),
            "signature": base64.b64encode(signature).decode()}


def package_record(index, artifact, base_url, product, download_urls=None):
    check_url(base_url)
    if index.data.get("development"):
        raise ValueError("Development bundles cannot be published")
    filename = artifact["filename"]
    if Path(filename).name != filename or "/" in filename or "\\" in filename:
        raise ValueError("Artifact filename must be a single filename")
    archive_path = index.parent / filename
    if digest_file(archive_path) != artifact["sha256"] or archive_path.stat().st_size != artifact["size"]:
        raise ValueError(f"Release checksum mismatch: {filename}")
    version, target = index.data["version"], artifact["target"]
    prefix = f"{product}-v{version}-{target}/"
    with zipfile.ZipFile(archive_path) as archive:
        raw_manifest = archive.read(prefix + "release-manifest.json")
        manifest = json.loads(raw_manifest)
        if manifest.get("development"):
            raise ValueError("Development bundles cannot be published")
        if (manifest["product"], manifest["version"], manifest["target"]) != (product, version, target):
            raise ValueError("Manifest identity mismatch")
        if product == "Q3RallyLauncher":
            executable = manifest["entrypoint"]
        elif target.startswith("macos-"):
            plist = plistlib.loads(archive.read(prefix + "Q3Rally.app/Contents/Info.plist"))
            name = plist["CFBundleExecutable"]
            if safe_path(name).name != name:
                raise ValueError("Invalid macOS executable")
            executable = "Q3Rally.app/Contents/MacOS/" + name
        else:
            arch = target.split("-", 1)[1]
            executable = f"q3rally.{arch}" + (".exe" if target.startswith("windows-") else "")
        safe_path(executable)
        if executable not in {record["path"] for record in manifest["files"]}:
            raise ValueError("Entrypoint is not declared by the release manifest")
    from urllib.parse import quote
    url = (download_urls or {}).get(filename, base_url.rstrip("/") + "/" + quote(filename))
    check_url(url)
    return {"url": url, "size": artifact["size"],
            "sha256": artifact["sha256"], "manifest_sha256": hashlib.sha256(raw_manifest).hexdigest(), "entrypoint": executable}


class Index:
    def __init__(self, path):
        self.path = Path(path)
        self.parent = self.path.parent
        self.data = json.loads(self.path.read_text())


def main(argv=None):
    # Publishing is a build-host tool; the installed launcher does not need it.
    from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
    from cryptography.hazmat.primitives import serialization
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    keygen = sub.add_parser("keygen")
    keygen.add_argument("--private-key", type=Path, required=True)
    keygen.add_argument("--public-config", type=Path, required=True)
    keygen.add_argument("--key-id", default="q3rally-release-1")
    catalog = sub.add_parser("catalog")
    catalog.add_argument("--private-key", type=Path, required=True)
    catalog.add_argument("--key-id", default="q3rally-release-1")
    catalog.add_argument("--public-config", type=Path, required=True)
    catalog.add_argument("--previous", type=Path)
    catalog.add_argument("--release-index", type=Path, action="append", default=[])
    catalog.add_argument("--launcher-index", type=Path, action="append", default=[])
    catalog.add_argument("--base-url", required=True)
    catalog.add_argument("--launcher-base-url")
    catalog.add_argument("--download-urls", type=Path, help="JSON object mapping artifact filenames to exact HTTPS URLs (e.g. Sanity assets)")
    catalog.add_argument("--notes", type=Path)
    catalog.add_argument("--sequence", type=int, required=True)
    catalog.add_argument("--valid-days", type=int, default=90)
    catalog.add_argument("--output", type=Path, required=True)
    args = parser.parse_args(argv)
    password = os.environ.get("Q3RALLY_SIGNING_PASSWORD", "").encode() or None
    if args.command == "keygen":
        private = args.private_key.resolve()
        if private.is_relative_to(ROOT) or private.exists():
            raise ValueError("Private key must be a new file outside the repository")
        key = Ed25519PrivateKey.generate()
        private.parent.mkdir(parents=True, exist_ok=True)
        encryption = serialization.BestAvailableEncryption(password) if password else serialization.NoEncryption()
        raw = key.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, encryption)
        with os.fdopen(os.open(private, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600), "wb") as stream:
            stream.write(raw)
        config = json.loads(args.public_config.read_text()) if args.public_config.exists() else {
            "catalog_url": "https://www.q3rally.com/launcher/catalog.json", "public_keys": {}}
        config["public_keys"][args.key_id] = base64.b64encode(key.public_key().public_bytes_raw()).decode()
        atomic_json(args.public_config, config)
        print("Private key created outside repository; public launcher configuration updated.")
        return 0
    if args.sequence < 1 or not 1 <= args.valid_days <= 365:
        raise ValueError("Sequence/validity out of range")
    key = serialization.load_pem_private_key(args.private_key.read_bytes(), password)
    if not isinstance(key, Ed25519PrivateKey):
        raise ValueError("An Ed25519 signing key is required")
    keys = json.loads(args.public_config.read_text())["public_keys"]
    download_urls = json.loads(args.download_urls.read_text(encoding="utf-8")) if args.download_urls else {}
    if not isinstance(download_urls, dict) or any(not isinstance(k, str) or not isinstance(v, str) for k, v in download_urls.items()):
        raise ValueError("Download URLs must map filenames to HTTPS URL strings")
    for url in download_urls.values():
        check_url(url)
    data = {"schema": 1, "product": "Q3Rally", "sequence": args.sequence,
            "expires": (datetime.now(timezone.utc) + timedelta(days=args.valid_days)).isoformat(),
            "releases": [], "launchers": []}
    if args.previous:
        previous = decode_catalog(json.loads(args.previous.read_text()), keys, fresh=False)
        if args.sequence <= previous["sequence"]:
            raise ValueError("Catalog sequence must increase")
        for component in ("releases", "launchers"):
            data[component] = previous.get(component, [])
    for component, paths, product, base in [("releases", args.release_index, "Q3Rally", args.base_url),
            ("launchers", args.launcher_index, "Q3RallyLauncher", args.launcher_base_url or args.base_url)]:
        for path in paths:
            index = Index(path)
            version = index.data["version"]; version_key(version)
            if index.data.get("product", product) != product:
                raise ValueError("Wrong release product")
            record = {"version": version, "notes": args.notes.read_text(encoding="utf-8") if args.notes else "",
                      "packages": {artifact["target"]: package_record(index, artifact, base, product, download_urls) for artifact in index.data["artifacts"]}}
            old = next((item for item in data[component] if item["version"] == version), None)
            if old:
                # Preserve packages for other platforms when publishing incrementally.
                record["packages"] = {**old["packages"], **record["packages"]}
                data[component].remove(old)
            data[component].append(record)
        data[component].sort(key=lambda item: version_key(item["version"]), reverse=True)
    envelope = sign(data, key, args.key_id)
    decode_catalog(envelope, keys)
    atomic_json(args.output, envelope)
    print("Signed catalog prepared. Nothing has been uploaded.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
