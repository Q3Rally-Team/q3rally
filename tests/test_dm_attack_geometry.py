"""Exercise the real BotCheckAttack function with deterministic game services."""

import pathlib
import subprocess
import tempfile


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]


def _function(source: str, signature: str) -> str:
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 1
    cursor = opening + 1
    while depth:
        if source[cursor] == "{":
            depth += 1
        elif source[cursor] == "}":
            depth -= 1
        cursor += 1
    return source[start:cursor]


def test_dm_attack_geometry_behavior() -> None:
    game = REPO_ROOT / "engine" / "code" / "game"
    ai_source = (game / "ai_dmq3.c").read_text()
    weapon_source = (game / "g_weapon.c").read_text()
    # Compile the production implementations, rather than a copied test version.
    functions = [
        _function(ai_source, "qboolean InFieldOfVision("),
        _function(weapon_source, "void CalcMuzzlePoint ("),
        _function(ai_source, "static float BotDeathmatchSkill("),
        _function(ai_source, "void BotCheckAttack("),
    ]
    with tempfile.TemporaryDirectory(prefix="q3rally-dm-attack-") as temporary:
        build = pathlib.Path(temporary)
        (build / "dm_attack_under_test.h").write_text("\n\n".join(functions))
        binary = build / "dm_attack_geometry_test"
        subprocess.run(
            [
                "gcc",
                str(REPO_ROOT / "tests" / "dm_attack_geometry_test.c"),
                str(REPO_ROOT / "engine" / "code" / "qcommon" / "q_math.c"),
                str(REPO_ROOT / "engine" / "code" / "qcommon" / "q_shared.c"),
                "-I" + str(game),
                "-I" + str(build),
                '-DARCH_STRING="test"',
                '-DOS_STRING="linux"',
                "-DID_INLINE=inline",
                "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                "-o",
                str(binary),
                "-lm",
            ],
            check=True,
        )
        result = subprocess.run([str(binary)], capture_output=True, text=True)
        assert result.returncode == 0, result.stderr
        assert "ok" in result.stdout
