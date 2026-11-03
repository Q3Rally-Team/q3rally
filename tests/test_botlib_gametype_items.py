import pathlib
import subprocess
import tempfile


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
CODE = REPO_ROOT / "engine" / "code"


def test_botlib_gametype_item_filtering() -> None:
    with tempfile.TemporaryDirectory(prefix="q3rally-botlib-mode-") as tmp:
        binary = pathlib.Path(tmp) / "botlib_gametype_items_test"
        subprocess.run([
            "gcc",
            str(REPO_ROOT / "tests" / "botlib_gametype_items_test.c"),
            str(CODE / "qcommon" / "q_shared.c"),
            str(CODE / "qcommon" / "q_math.c"),
            "-I" + str(CODE),
            '-DARCH_STRING="test"',
            '-DOS_STRING="linux"',
            "-DID_INLINE=inline",
            "-DPATH_SEP='/'",
            '-DDLL_EXT=".so"',
            "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
            "-o", str(binary), "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout
