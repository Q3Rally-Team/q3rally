import pathlib
import subprocess
import tempfile


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
CODE = REPO_ROOT / "engine" / "code"


def test_gametype_classification() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        binary = pathlib.Path(tmp) / "gametype_classification_test"
        subprocess.run([
            "gcc",
            str(REPO_ROOT / "tests" / "gametype_classification_test.c"),
            str(CODE / "qcommon" / "q_shared.c"),
            str(CODE / "qcommon" / "q_math.c"),
            "-I" + str(CODE / "game"),
            "-I" + str(CODE),
            "-I" + str(CODE / "qcommon"),
            "-DARCH_STRING=\"test\"",
            "-DOS_STRING=\"linux\"",
            "-DID_INLINE=inline",
            "-DPATH_SEP='/'",
            "-DDLL_EXT=\".so\"",
            "-o",
            str(binary),
            "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout
