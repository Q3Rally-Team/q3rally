import pathlib
import subprocess
import tempfile


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
CODE = REPO_ROOT / "engine" / "code"


def test_deathmatch_enemy_selection() -> None:
    source = (CODE / "game" / "ai_dmq3.c").read_text()

    def function(signature: str) -> str:
        start = source.index(signature)
        end = source.index("\n}\n", start) + 3
        return source[start:end]

    # Compile the production selector and perception helpers, with controlled
    # world/client stubs; no copy of the selection implementation lives here.
    signatures = [
        "qboolean EntityIsDead(",
        "qboolean EntityCarriesFlag(",
        "qboolean EntityIsInvisible(",
        "qboolean EntityIsShooting(",
        "qboolean EntityIsChatting(",
        "void BotUpdateBattleInventory(",
        "qboolean InFieldOfVision(",
        "int BotFindEnemy(",
    ]
    with tempfile.TemporaryDirectory(prefix="q3rally-dm-enemy-") as tmp:
        build = pathlib.Path(tmp)
        (build / "dm_enemy_selection_under_test.h").write_text(
            "\n".join(function(signature) for signature in signatures)
        )
        binary = build / "dm_enemy_selection_test"
        subprocess.run([
            "gcc", str(REPO_ROOT / "tests" / "dm_enemy_selection_test.c"),
            str(CODE / "qcommon" / "q_math.c"),
            "-I" + str(build), "-I" + str(CODE / "game"),
            "-I" + str(CODE / "qcommon"), "-I" + str(CODE),
            '-DARCH_STRING="test"', '-DOS_STRING="linux"',
            "-DID_INLINE=inline", "-DPATH_SEP='/'", '-DDLL_EXT=".so"',
            "-ffunction-sections", "-fdata-sections",
            "-Wl,--gc-sections", "-o", str(binary), "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout
