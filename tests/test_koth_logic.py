import atexit
import pathlib
import shutil
import tempfile
import subprocess


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
# Build output goes to a temporary directory, never into tests/.
BUILD_DIR = pathlib.Path(tempfile.mkdtemp(prefix="q3rally-test-"))
atexit.register(shutil.rmtree, BUILD_DIR, True)
TEST_BINARY = BUILD_DIR / "koth_logic_test"

COMPILE_ARGS = [
    "gcc",
    str(REPO_ROOT / "tests" / "koth_logic_test.c"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "game"),
    "-I" + str(REPO_ROOT / "engine" / "code"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "qcommon"),
    "-DARCH_STRING=\"test\"",
    "-DOS_STRING=\"linux\"",
    "-DID_INLINE=inline",
    "-DPATH_SEP='/'",
    "-DDLL_EXT=\".so\"",
    "-o",
    str(TEST_BINARY),
]


def test_koth_objective_and_overtime_rules() -> None:
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
    assert "ok" in result.stdout
