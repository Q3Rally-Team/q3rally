import atexit
import pathlib
import shutil
import tempfile
import subprocess

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
# Build output goes to a temporary directory, never into tests/.
BUILD_DIR = pathlib.Path(tempfile.mkdtemp(prefix="q3rally-test-"))
atexit.register(shutil.rmtree, BUILD_DIR, True)
TEST_BINARY = BUILD_DIR / "ctf4_team_score_test"

COMPILE_ARGS = [
    "gcc",
    str(REPO_ROOT / "tests" / "ctf4_team_score_test.c"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "game"),
    "-I" + str(REPO_ROOT / "engine" / "code"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "qcommon"),
    "-DARCH_STRING=\"test\"",
    "-DOS_STRING=\"linux\"",
    "-DID_INLINE=inline",
    "-DPATH_SEP='/'",
    "-DDLL_EXT=\".so\"",
    "-DQ3_LITTLE_ENDIAN",
    "-DUNIT_TEST",
    "-o",
    str(TEST_BINARY),
    "-lm",
]


def test_ctf4_team_score_behaviour() -> None:
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
    assert "ok" in result.stdout
