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
    assert result.stdout.strip() == "team_racing 9"

    list_result = subprocess.run([
        "python3",
        str(script),
        "--list",
    ], check=True, capture_output=True, text=True)
    lines = [line.strip() for line in list_result.stdout.splitlines() if line.strip()]
    assert lines[0].startswith("0 - racing")
    assert any(line.startswith("2 - sprint") for line in lines)
    assert lines[-1].endswith("domination")
