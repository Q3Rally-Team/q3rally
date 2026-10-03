import subprocess
from pathlib import Path
import sys

REPO_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO_ROOT))

from tools.startserver import cli


def test_canonicalise_normalises_aliases():
    assert cli.canonicalise(" Race ") == "racing"
    assert cli.canonicalise("TEAM-RACING-DM") == "team_racing_dm"
    assert cli.canonicalise("LastCarStanding") == "lcs"


def test_resolve_gametype_known_and_unknown():
    assert cli.resolve_gametype("ctf")[1] == cli.GAMETYPES["ctf"]
    assert cli.resolve_gametype("capture_the_flag") == ("ctf", cli.GAMETYPES["ctf"])
    fallback = cli.resolve_gametype("unsupported-mode")
    assert fallback == ("elimination", cli.GAMETYPES["elimination"])


def test_iter_gametypes_are_sorted_by_numeric_value():
    numbers = [number for _, number in cli.iter_gametypes()]
    assert numbers == sorted(numbers)


def test_cli_outputs_expected_lines():
    script = REPO_ROOT / "tools" / "startserver" / "cli.py"
    result = subprocess.run([
        "python3",
        str(script),
        "team-racing"
    ], check=True, capture_output=True, text=True)
    assert result.stdout.strip() == "team_racing 17"

    list_result = subprocess.run([
        "python3",
        str(script),
        "--list",
    ], check=True, capture_output=True, text=True)
    lines = [line.strip() for line in list_result.stdout.splitlines() if line.strip()]
    assert lines[0].startswith("0 - racing")
    assert any(line.startswith("7 - sprint") for line in lines)
    assert any(line.startswith("8 - ghost") for line in lines)
    assert lines[-1].endswith("koth")


def test_gametype_values_match_bg_public():
    """cli.GAMETYPES must mirror the explicit gametype_t values in bg_public.h."""
    import re

    header = (REPO_ROOT / "engine" / "code" / "game" / "bg_public.h").read_text()
    engine_values = {
        name: int(value)
        for name, value in re.findall(r"^\s*GT_([A-Z0-9_]+)\s*=\s*(\d+)\s*,", header, re.MULTILINE)
    }
    engine_to_cli = {
        "RACING": "racing",
        "RACING_DM": "racing_dm",
        "SINGLE_PLAYER": "single_player",
        "DERBY": "derby",
        "LCS": "lcs",
        "ELIMINATION": "elimination",
        "DEATHMATCH": "deathmatch",
        "SPRINT": "sprint",
        "GHOST": "ghost",
        "TEAM": "team",
        "TEAM_RACING": "team_racing",
        "TEAM_RACING_DM": "team_racing_dm",
        "CTF": "ctf",
        "CTF4": "ctf4",
        "DOMINATION": "domination",
        "KOTH": "koth",
    }
    assert set(engine_values) == set(engine_to_cli)
    for engine_name, cli_name in engine_to_cli.items():
        assert cli.GAMETYPES[cli_name] == engine_values[engine_name], engine_name


def test_ghost_aliases_resolve():
    assert cli.resolve_gametype("ghost") == ("ghost", 8)
    assert cli.resolve_gametype("Ghost-Race") == ("ghost", 8)
    assert cli.resolve_gametype("ghost_only") == ("ghost", 8)
