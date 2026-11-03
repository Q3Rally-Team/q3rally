"""Explicit identity-only import; legacy releases never share current profiles."""
import json
from pathlib import Path
import re
import uuid

from .core import LauncherError, atomic_json, version_key, within


def import_profile(source, root, version):
    if version_key(version) < version_key("0.8"):
        raise LauncherError("Profiles can only be imported into v0.8 or newer.")
    source = Path(source)
    if not re.fullmatch(r"[A-Za-z0-9_-]{1,31}", source.stem) or source.stat().st_size >= 32768:
        raise LauncherError("Invalid profile filename or profile file too large.")
    data = json.loads(source.read_text(encoding="utf-8"))
    if data.get("profileVersion", 1) not in (1, 2):
        raise LauncherError("This profile version cannot be imported.")
    identity = data.get("identity", data)
    if not isinstance(identity, dict) or not isinstance(identity.get("uuid"), str):
        raise LauncherError("Invalid profile identity.")
    if str(uuid.UUID(identity["uuid"])) != identity["uuid"].lower():
        raise LauncherError("Invalid profile ID.")
    info = identity.get("info", {})
    # No career totals, credentials or unverified remote bindings are imported.
    copied = {"profileVersion": 2, "identity": {
        "uuid": identity["uuid"], "name": identity.get("name", source.stem),
        "info": {key: info.get(key, "") for key in ("gender", "birthDate", "avatar", "country")}},
        "progress": {"epoch": "v0.8-local-1", "authority": "local", "currentRank": 0, "highestRank": 0, "stats": {"statsRevision": 1}},
        "webLink": {"state": "unlinked", "accountId": None, "profileId": None}}
    favorites = info.get("favoriteCars", [])
    if not isinstance(favorites, list) or len(favorites) > 4:
        raise LauncherError("Invalid favorite vehicles.")
    copied["identity"]["info"]["favoriteCars"] = favorites
    name = copied["identity"]["name"]
    if not isinstance(name, str) or len(name.encode("utf-8")) >= 64:
        raise LauncherError("Invalid display name.")
    for favorite in favorites:
        if not isinstance(favorite, dict) or any(not isinstance(favorite.get(key, ""), str) or
                len(favorite.get(key, "").encode("utf-8")) >= 64 for key in ("model", "skin", "rim", "head")):
            raise LauncherError("Invalid favorite vehicles.")
    for key, value in copied["identity"]["info"].items():
        if key != "favoriteCars" and (not isinstance(value, str) or len(value.encode("utf-8")) >= 64):
            raise LauncherError("Invalid personal information.")
    destination = within(Path(root) / "user-data", f"{version}/baseq3r/profiles/{source.stem}.json")
    if destination.exists():
        raise LauncherError("A profile with this filename already exists. Nothing was overwritten.")
    atomic_json(destination, copied)
    return destination
