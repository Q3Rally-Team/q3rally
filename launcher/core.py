"""Signed release catalog, resumable downloads and isolated installations."""
from __future__ import annotations

import base64
from contextlib import contextmanager
from datetime import datetime, timezone
import hashlib
import json
import logging
import os
from pathlib import Path, PurePosixPath
import platform
import re
import shutil
import stat
import ssl
import subprocess
import sys
import tempfile
import threading
import unicodedata
import urllib.error
import urllib.parse
import urllib.request
import zipfile

from nacl.signing import VerifyKey
import psutil
from . import __version__

LOG = logging.getLogger("q3rally.launcher")
MAX_CATALOG = 4 * 1024 * 1024
VERSION = re.compile(r"(\d+)\.(\d+)(?:\.(\d+))?([a-z])?\Z")
TARGET = re.compile(r"(?:windows|linux|macos)-(?:x86|x86_64|arm64|universal)\Z")


class LauncherError(Exception):
    pass


def version_key(version):
    match = VERSION.fullmatch(version)
    if not match:
        raise LauncherError(f"Invalid version number: {version}")
    major, minor, patch, suffix = match.groups()
    return int(major), int(minor), int(patch or 0), ord(suffix) if suffix else 0


def default_root():
    if sys.platform == "win32":
        return Path(os.environ.get("LOCALAPPDATA", Path.home() / "AppData/Local")) / "Q3RallyLauncher"
    if sys.platform == "darwin":
        return Path.home() / "Library/Application Support/Q3RallyLauncher"
    return Path(os.environ.get("XDG_DATA_HOME", Path.home() / ".local/share")) / "q3rally-launcher"


def native_targets():
    system = {"Windows": "windows", "Linux": "linux", "Darwin": "macos"}.get(platform.system())
    arch = {"AMD64": "x86_64", "x86_64": "x86_64", "aarch64": "arm64", "arm64": "arm64",
            "i386": "x86", "i686": "x86", "x86": "x86"}.get(platform.machine())
    if arch == "x86_64" and sys.maxsize <= 2**32:
        arch = "x86"
    if not system or not arch:
        raise LauncherError("This operating system is not supported by the launcher.")
    return [f"{system}-{arch}"] + (["macos-universal"] if system == "macos" else [])


def atomic_json(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix=".write-", dir=path.parent)
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as stream:
            json.dump(value, stream, ensure_ascii=False, indent=2)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def digest_file(path):
    with Path(path).open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def safe_path(name):
    if not isinstance(name, str) or not name or "\\" in name or "\x00" in name:
        raise LauncherError("Unsafe file path in package.")
    path = PurePosixPath(name)
    if path.is_absolute() or any(p in ("", ".", "..") for p in name.split("/")):
        raise LauncherError(f"Unsafe file path: {name}")
    for part in path.parts:
        if ":" in part or part.endswith((".", " ")) or re.fullmatch(r"(?i:con|prn|aux|nul|com[1-9]|lpt[1-9])(?:\..*)?", part):
            raise LauncherError(f"Non-portable file path: {name}")
    return path


def within(root, relative):
    path = Path(root).joinpath(*safe_path(relative).parts)
    if not path.resolve().is_relative_to(Path(root).resolve()):
        raise LauncherError("Path leaves the installation directory.")
    return path


def check_url(url, development=False):
    parsed = urllib.parse.urlsplit(url)
    if parsed.username or parsed.password or parsed.fragment:
        raise LauncherError("Invalid download URL.")
    local = parsed.scheme == "http" and parsed.hostname in ("127.0.0.1", "localhost", "::1")
    if parsed.scheme != "https" and not (development and local):
        raise LauncherError("Downloads require HTTPS.")
    if not parsed.hostname:
        raise LauncherError("Download URL has no host.")


class SafeRedirect(urllib.request.HTTPRedirectHandler):
    def __init__(self, development=False):
        self.development = development

    def redirect_request(self, request, fp, code, message, headers, newurl):
        check_url(newurl, self.development)
        return super().redirect_request(request, fp, code, message, headers, newurl)


def opener(development=False):
    handlers = [SafeRedirect(development)]
    if sys.platform == "darwin":
        # Frozen macOS apps cannot rely on a separately installed Python's CA file.
        context = ssl.create_default_context()
        context.set_alpn_protocols(["http/1.1"])
        if hasattr(context, "post_handshake_auth"):
            context.post_handshake_auth = True
        context.load_verify_locations(cafile=Path(__file__).with_name("assets") / "certificates/cacert.pem")
        handlers.append(urllib.request.HTTPSHandler(context=context))
    client = urllib.request.build_opener(*handlers)
    client.addheaders = [("User-Agent", f"Q3RallyLauncher/{__version__}")]
    return client


def decode_catalog(envelope, keys, *, fresh=True):
    try:
        payload = base64.b64decode(envelope["payload"], validate=True)
        signature = base64.b64decode(envelope["signature"], validate=True)
        public = base64.b64decode(keys[envelope["key_id"]], validate=True)
        if len(payload) > MAX_CATALOG:
            raise ValueError("catalog too large")
        VerifyKey(public).verify(payload, signature)
        catalog = json.loads(payload)
        if catalog["schema"] != 1 or catalog["product"] != "Q3Rally":
            raise ValueError("schema/product")
        if type(catalog["sequence"]) is not int or catalog["sequence"] < 1:
            raise ValueError("sequence")
        expiry = datetime.fromisoformat(catalog["expires"].replace("Z", "+00:00"))
        if expiry.tzinfo is None or (fresh and expiry <= datetime.now(timezone.utc)):
            raise ValueError("expired")
        for component in ("releases", "launchers"):
            seen = set()
            for release in catalog.get(component, []):
                version_key(release["version"])
                if release["version"] in seen:
                    raise ValueError("duplicate release")
                seen.add(release["version"])
                if not isinstance(release.get("notes", ""), str):
                    raise ValueError("release notes")
                for target, package in release["packages"].items():
                    if not TARGET.fullmatch(target):
                        raise ValueError("target")
                    safe_path(package["entrypoint"])
                    for field in ("sha256", "manifest_sha256"):
                        if not re.fullmatch(r"[a-f0-9]{64}", package[field]):
                            raise ValueError("digest")
                    if type(package["size"]) is not int or not 0 < package["size"] <= 20 * 1024**3:
                        raise ValueError("size")
                    if not isinstance(package["url"], str):
                        raise ValueError("url")
        return catalog
    except Exception as error:
        raise LauncherError("The version catalog is invalid, expired or not signed by a trusted key.") from error


@contextmanager
def file_lock(path):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a+b") as stream:
        try:
            if os.name == "nt":
                import msvcrt
                stream.seek(0)
                if not stream.read(1):
                    stream.write(b"0"); stream.flush()
                stream.seek(0)
                msvcrt.locking(stream.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as error:
            raise LauncherError("This version is already in use or being modified.") from error
        try:
            yield
        finally:
            if os.name == "nt":
                stream.seek(0); msvcrt.locking(stream.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                fcntl.flock(stream, fcntl.LOCK_UN)


def processes_using(directory):
    directory = Path(directory).resolve()
    for process in psutil.process_iter(["name", "exe"]):
        try:
            executable = process.info["exe"]
            if executable and Path(executable).resolve().is_relative_to(directory):
                return True
            if not executable and (process.info["name"] or "").lower().startswith("q3rally"):
                return True  # Cannot establish which installation owns the process.
        except (psutil.NoSuchProcess, psutil.AccessDenied, OSError):
            continue
    return False


class Manager:
    def __init__(self, root, keys, *, targets=None, development=False, product="Q3Rally"):
        self.root = Path(root).resolve()
        self.keys = keys
        self.targets = targets or native_targets()
        self.development = development
        self.product = product
        self.component = "releases" if product == "Q3Rally" else "launchers"
        self.versions = self.root / ("versions" if product == "Q3Rally" else "launcher-versions")
        self.cache = self.root / "downloads"
        self.locks = self.root / "locks"
        for directory in (self.versions, self.cache, self.locks):
            directory.mkdir(parents=True, exist_ok=True)
        self._recover()

    def _recover(self):
        """Restore an interrupted directory swap using only verified receipts."""
        for backup in self.versions.glob(".previous-*"):
            if backup.is_symlink() or not backup.is_dir():
                continue
            try:
                receipt = json.loads((backup / ".launcher-install.json").read_text())
                version, target = receipt["version"], receipt["target"]
                destination = self.path(version, target)
                if backup.name != ".previous-" + destination.name:
                    continue
                with file_lock(self.locks / (self.product + "-" + destination.name + ".lock")):
                    catalog = decode_catalog(receipt["envelope"], self.keys, fresh=False)
                    package = next(p for r, t, p in self.available(catalog) if r["version"] == version and t == target)
                    if destination.exists():
                        self.verify(version, target)
                        if not processes_using(backup):
                            shutil.rmtree(backup)
                    else:
                        self._verify(backup, version, target, package)
                        backup.rename(destination)
            except (OSError, ValueError, KeyError, StopIteration, LauncherError):
                LOG.warning("Recovery deferred for %s", backup, exc_info=True)

    def load_catalog(self, url):
        if not self.keys:
            raise LauncherError("The public release key has not been configured.")
        check_url(url, self.development)
        with opener(self.development).open(url, timeout=20) as response:
            raw = response.read(MAX_CATALOG * 2 + 1)
        if len(raw) > MAX_CATALOG * 2:
            raise LauncherError("The version catalog is too large.")
        envelope = json.loads(raw)
        catalog = decode_catalog(envelope, self.keys)
        cachefile = self.root / "catalog-cache.json"
        with file_lock(self.locks / "catalog.lock"):
            if cachefile.exists() and catalog["sequence"] < json.loads(cachefile.read_text())["sequence"]:
                raise LauncherError("An older version catalog was rejected.")
            atomic_json(cachefile, {"sequence": catalog["sequence"], "envelope": envelope})
        return envelope, catalog

    def cached_catalog(self):
        try:
            envelope = json.loads((self.root / "catalog-cache.json").read_text())["envelope"]
            return envelope, decode_catalog(envelope, self.keys, fresh=False)
        except (OSError, ValueError, KeyError, LauncherError):
            return None, None

    def available(self, catalog):
        releases = []
        for release in catalog.get(self.component, []):
            for target in self.targets:
                if target in release["packages"]:
                    releases.append((release, target, release["packages"][target]))
                    break
        return sorted(releases, key=lambda item: version_key(item[0]["version"]), reverse=True)

    def path(self, version, target):
        version_key(version)
        if not TARGET.fullmatch(target):
            raise LauncherError("Invalid platform target.")
        path = within(self.versions, f"{version}-{target}")
        if path.is_symlink():
            raise LauncherError("The installation directory must not be a symbolic link.")
        return path

    def installed(self):
        result = []
        for directory in sorted(self.versions.iterdir()):
            if directory.name.startswith(".") or not directory.is_dir() or directory.is_symlink():
                continue
            try:
                receipt = json.loads((directory / ".launcher-install.json").read_text())
                catalog = decode_catalog(receipt["envelope"], self.keys, fresh=False)
                item = next(item for item in self.available(catalog)
                            if item[0]["version"] == receipt["version"] and item[1] == receipt["target"])
                if directory == self.path(receipt["version"], receipt["target"]):
                    result.append(item)
            except (OSError, ValueError, KeyError, StopIteration, LauncherError):
                LOG.warning("Ignoring invalid install receipt: %s", directory)
        return sorted(result, key=lambda item: version_key(item[0]["version"]), reverse=True)

    def _download(self, package, cancel, progress):
        check_url(package["url"], self.development)
        partial = self.cache / (package["sha256"] + ".part")
        offset = partial.stat().st_size if partial.exists() else 0
        if offset > package["size"]:
            partial.unlink(); offset = 0
        if offset == package["size"]:
            if digest_file(partial) == package["sha256"]:
                return partial
            partial.unlink(); offset = 0
        request = urllib.request.Request(package["url"], headers={"Accept-Encoding": "identity"})
        if offset:
            request.add_header("Range", f"bytes={offset}-")
        with opener(self.development).open(request, timeout=20) as response:
            if response.status == 206:
                expected = f"bytes {offset}-{package['size']-1}/{package['size']}"
                if response.headers.get("Content-Range") != expected:
                    raise LauncherError("Invalid response while resuming the download.")
            elif response.status == 200:
                offset = 0
            else:
                raise LauncherError(f"Download failed: HTTP {response.status}")
            with partial.open("ab" if offset else "wb") as stream:
                while True:
                    if cancel.is_set():
                        raise LauncherError("Download paused. You can resume it later.")
                    chunk = response.read(256 * 1024)
                    if not chunk:
                        break
                    offset += len(chunk)
                    if offset > package["size"]:
                        raise LauncherError("The download is larger than expected.")
                    stream.write(chunk)
                    progress(offset, package["size"])
        if offset != package["size"]:
            raise LauncherError("Download incomplete. Please try again.")
        if digest_file(partial) != package["sha256"]:
            partial.unlink()
            raise LauncherError("The download checksum does not match. Please download it again.")
        return partial

    def _extract(self, archive_path, stage, version, target, package):
        prefix = f"{self.product}-v{version}-{target}/"
        with zipfile.ZipFile(archive_path) as archive:
            entries = archive.infolist()
            if len(entries) > 100000 or sum(item.file_size for item in entries) > 20 * 1024**3:
                raise LauncherError("The installation package exceeds the size limit.")
            paths, links = set(), []
            for item in entries:
                if not item.filename.startswith(prefix):
                    raise LauncherError("Unexpected package directory.")
                name = item.filename[len(prefix):].rstrip("/")
                if not name:
                    continue
                safe_path(name)
                if name.casefold() == ".launcher-install.json":
                    raise LauncherError("The package contains a reserved installation receipt.")
                key = unicodedata.normalize("NFC", name).casefold()
                if key in paths:
                    raise LauncherError("Duplicate filenames in package.")
                paths.add(key)
                path = within(stage, name)
                mode = item.external_attr >> 16
                if item.is_dir():
                    path.mkdir(parents=True, exist_ok=True)
                elif stat.S_ISLNK(mode):
                    if os.name == "nt":
                        raise LauncherError("Windows packages must not contain symbolic links.")
                    destination = archive.read(item).decode("utf-8")
                    if "\\" in destination or PurePosixPath(destination).is_absolute():
                        raise LauncherError("Unsafe link target.")
                    links.append((path, destination))
                elif stat.S_IFMT(mode) not in (0, stat.S_IFREG):
                    raise LauncherError("Unsupported file type in package.")
                else:
                    path.parent.mkdir(parents=True, exist_ok=True)
                    with archive.open(item) as source, path.open("xb") as destination:
                        shutil.copyfileobj(source, destination, 256 * 1024)
                    path.chmod(0o755 if mode & 0o111 else 0o644)
            # Files are extracted before links, so no archive-created link can
            # redirect a later file write outside the staging directory.
            for path, destination in links:
                path.parent.mkdir(parents=True, exist_ok=True)
                if not (path.parent / destination).resolve().is_relative_to(stage.resolve()):
                    raise LauncherError("Link leaves the installation directory.")
                path.symlink_to(destination)
            self._verify(stage, version, target, package)

    def _verify(self, directory, version, target, package):
        manifest_path = within(directory, "release-manifest.json")
        if manifest_path.is_symlink() or manifest_path.stat().st_size > 16 * 1024**2 or digest_file(manifest_path) != package["manifest_sha256"]:
            raise LauncherError("The installation manifest was modified.")
        manifest = json.loads(manifest_path.read_text())
        if (manifest.get("product"), manifest.get("version"), manifest.get("target")) != (self.product, version, target):
            raise LauncherError("The installation manifest belongs to another version.")
        expected = {"release-manifest.json", ".launcher-install.json"}
        for record in manifest["files"]:
            name = record["path"]
            if name in expected:
                raise LauncherError("Duplicate files in manifest.")
            expected.add(name)
            path = within(directory, name)
            if record.get("symlink"):
                if not path.is_symlink():
                    raise LauncherError(f"Missing link: {name}")
                data = os.readlink(path).encode()
                size, digest = len(data), hashlib.sha256(data).hexdigest()
            else:
                if path.is_symlink() or not path.is_file():
                    raise LauncherError(f"Missing file: {name}")
                size, digest = path.stat().st_size, digest_file(path)
            if size != record["size"] or digest != record["sha256"]:
                raise LauncherError(f"File verification failed: {name}")
        actual = {p.relative_to(directory).as_posix() for p in directory.rglob("*") if p.is_file() or p.is_symlink()}
        if actual - expected or package["entrypoint"] not in expected - {"release-manifest.json", ".launcher-install.json"}:
            raise LauncherError("Unexpected files in the installation.")
        executable = within(directory, package["entrypoint"])
        if not executable.is_file():
            raise LauncherError("The game executable is missing.")
        return executable

    def _receipt(self, version, target):
        directory = self.path(version, target)
        receipt = json.loads((directory / ".launcher-install.json").read_text())
        catalog = decode_catalog(receipt["envelope"], self.keys, fresh=False)
        release, _, package = next(item for item in self.available(catalog)
                                  if item[0]["version"] == version and item[1] == target)
        return directory, release, package

    def verify(self, version, target):
        directory, _, package = self._receipt(version, target)
        return self._verify(directory, version, target, package)

    def install(self, envelope, version, target, *, cancel=None, progress=lambda done, total: None):
        catalog = decode_catalog(envelope, self.keys)
        release, _, package = next(item for item in self.available(catalog)
                                  if item[0]["version"] == version and item[1] == target)
        directory = self.path(version, target)
        with file_lock(self.locks / (self.product + "-" + directory.name + ".lock")):
            if processes_using(directory):
                raise LauncherError("The game is still running. Please install it later.")
            with file_lock(self.locks / (package["sha256"] + ".download.lock")):
                archive = self._download(package, cancel or threading.Event(), progress)
            stage = Path(tempfile.mkdtemp(prefix=".stage-", dir=self.versions))
            backup = self.versions / (".previous-" + directory.name)
            try:
                self._extract(archive, stage, version, target, package)
                if cancel is not None and cancel.is_set():
                    raise LauncherError("Installation paused. Please start it again.")
                atomic_json(stage / ".launcher-install.json", {"version": version, "target": target, "envelope": envelope})
                if backup.exists():
                    raise LauncherError("A previous recovery must be checked first.")
                if directory.exists():
                    directory.rename(backup)
                try:
                    stage.rename(directory)
                except Exception:
                    if backup.exists():
                        backup.rename(directory)
                    raise
                if backup.exists():
                    shutil.rmtree(backup)
            finally:
                if stage.exists():
                    shutil.rmtree(stage)
        return directory

    def uninstall(self, version, target):
        directory = self.path(version, target)
        with file_lock(self.locks / (self.product + "-" + directory.name + ".lock")):
            if processes_using(directory):
                raise LauncherError("This version is still running.")
            # User data deliberately lives elsewhere and survives removal.
            if directory.exists():
                shutil.rmtree(directory)

    def run(self, version, target):
        directory = self.path(version, target)
        with file_lock(self.locks / (self.product + "-" + directory.name + ".lock")):
            if processes_using(directory):
                raise LauncherError("This version is already running.")
            executable = self.verify(version, target)
            home = within(self.root / "user-data", version)
            home.mkdir(parents=True, exist_ok=True)
            logs = self.root / "logs"; logs.mkdir(exist_ok=True)
            command = [str(executable), "+set", "fs_basepath", str(executable.parent), "+set", "fs_homepath", str(home)]
            logpath = logs / (version + "-" + datetime.now(timezone.utc).strftime("%Y%m%d-%H%M%S-%f") + ".log")
            options = {"creationflags": subprocess.CREATE_NO_WINDOW} if os.name == "nt" else {}
            with logpath.open("wb") as log:
                process = subprocess.Popen(command, cwd=executable.parent, stdout=log, stderr=subprocess.STDOUT, **options)
                return process.wait()
