import pathlib
import subprocess
import tempfile

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
CODE = REPO_ROOT / "engine" / "code"


def test_racing_line() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        binary = pathlib.Path(tmp) / "racing_line_test"
        subprocess.run([
            "gcc",
            str(REPO_ROOT / "tests" / "racing_line_test.c"),
            str(CODE / "qcommon" / "q_shared.c"),
            str(CODE / "qcommon" / "q_math.c"),
            "-I" + str(CODE / "cgame"),
            "-I" + str(CODE),
            "-I" + str(CODE / "qcommon"),
            "-DARCH_STRING=\"test\"",
            "-DOS_STRING=\"linux\"",
            "-DID_INLINE=inline",
            "-DPATH_SEP='/'",
            "-DDLL_EXT=\".so\"",
            "-DQ3_LITTLE_ENDIAN",
            "-DUNIT_TEST",
            "-o",
            str(binary),
            "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout
