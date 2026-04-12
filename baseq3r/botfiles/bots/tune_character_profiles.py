#!/usr/bin/env python3
"""Apply consistent bot personality tuning to all *_c.c files.

Archetype targets (skill 4 baseline):
- aggression
- self_preservation
- camper
- reaction_time (lower = faster)
- fire_throttle (lower = more continuous fire)

Skill scaling keeps identity stable:
- aggression/self_preservation/camper remain fixed by archetype.
- reaction_time and fire_throttle improve with skill while preserving archetype ordering.
"""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent

ARCHETYPES = {
    "berserker": dict(aggression=0.92, self_preservation=0.28, camper=0.05, reaction=0.45, fire=0.16),
    "hunter": dict(aggression=0.68, self_preservation=0.52, camper=0.18, reaction=0.58, fire=0.22),
    "tactician": dict(aggression=0.56, self_preservation=0.78, camper=0.26, reaction=0.66, fire=0.30),
    "ambusher": dict(aggression=0.34, self_preservation=0.84, camper=0.72, reaction=0.76, fire=0.38),
    "juggernaut": dict(aggression=0.86, self_preservation=0.74, camper=0.30, reaction=0.60, fire=0.20),
    "trickster": dict(aggression=0.62, self_preservation=0.40, camper=0.34, reaction=0.54, fire=0.25),
    "balanced": dict(aggression=0.60, self_preservation=0.60, camper=0.20, reaction=0.62, fire=0.24),
}

BOT_ARCHETYPE = {
    "angelyss": "hunter",
    "arachna": "ambusher",
    "ayumi": "tactician",
    "beret": "hunter",
    "dark": "berserker",
    "default": "balanced",
    "gargoyle": "juggernaut",
    "grism": "juggernaut",
    "grunt": "trickster",
    "jenna": "tactician",
    "kyonshi": "ambusher",
    "liz": "hunter",
    "major": "juggernaut",
    "merman": "hunter",
    "neko": "tactician",
    "penguin": "trickster",
    "rai": "tactician",
    "sarge": "berserker",
    "sergei": "berserker",
    "skelebot": "juggernaut",
    "sly": "ambusher",
    "tanisha": "tactician",
    "tony": "berserker",
    "widowe": "ambusher",
}

SKILL_SCALE = {
    1: dict(reaction=1.45, fire=1.25),
    4: dict(reaction=1.00, fire=1.00),
    5: dict(reaction=0.55, fire=0.80),
}


def clamp(value: float, low: float, high: float) -> float:
    return max(low, min(high, value))


def fmt(value: float) -> str:
    return f"{value:.2f}".rstrip("0").rstrip(".")


def replace_value(block: str, key: str, value: float) -> str:
    pattern = rf"({re.escape(key)}\s+)([0-9]+(?:\.[0-9]+)?)"
    updated, count = re.subn(pattern, lambda m: f"{m.group(1)}{fmt(value)}", block, count=1)
    if count != 1:
        raise ValueError(f"Could not update {key}")
    return updated


def tune_block(block: str, profile: dict[str, float], skill: int) -> str:
    scale = SKILL_SCALE[skill]
    reaction = clamp(profile["reaction"] * scale["reaction"], 0.0, 3.0)
    fire = clamp(profile["fire"] * scale["fire"], 0.01, 1.0)

    out = block
    out = replace_value(out, "CHARACTERISTIC_AGGRESSION", profile["aggression"])
    out = replace_value(out, "CHARACTERISTIC_SELFPRESERVATION", profile["self_preservation"])
    out = replace_value(out, "CHARACTERISTIC_CAMPER", profile["camper"])
    out = replace_value(out, "CHARACTERISTIC_REACTIONTIME", reaction)
    out = replace_value(out, "CHARACTERISTIC_FIRETHROTTLE", fire)
    return out


def tune_file(path: Path, profile: dict[str, float]) -> None:
    text = path.read_text()
    for skill in (1, 4, 5):
        patt = re.compile(rf"(skill {skill}\s*\{{)(.*?)(\n\}})", re.S)
        match = patt.search(text)
        if not match:
            raise ValueError(f"{path.name}: missing skill {skill}")
        tuned = tune_block(match.group(2), profile, skill)
        text = text[: match.start(2)] + tuned + text[match.end(2) :]

    if path.name == "default_c.c":
        text = text.replace('"bots/gargoyle_w.c"', '"bots/default_w.c"')
        text = text.replace('"bots/gargoyle_i.c"', '"bots/default_i.c"')
        text = text.replace('"bots/gargoyle_t.c"', '"bots/default_t.c"')

    path.write_text(text)


def main() -> None:
    for path in sorted(ROOT.glob("*_c.c")):
        stem = path.stem[:-2]  # trim _c
        archetype = BOT_ARCHETYPE.get(stem)
        if archetype is None:
            continue
        tune_file(path, ARCHETYPES[archetype])


if __name__ == "__main__":
    main()
