import pathlib
import subprocess

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
TEST_BINARY = REPO_ROOT / "tests" / "rally_state_machine_test"

COMPILE_ARGS = [
    "gcc",
    str(REPO_ROOT / "tests" / "rally_state_machine_test.c"),
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


def test_rally_state_machine_behaviour() -> None:
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
    assert "ok" in result.stdout
