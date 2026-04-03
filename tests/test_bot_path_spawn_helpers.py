import pathlib
import subprocess

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
TEST_BINARY = REPO_ROOT / "tests" / "bot_path_spawn_helpers_test"

COMPILE_ARGS = [
    "gcc",
    str(REPO_ROOT / "tests" / "bot_path_spawn_helpers_test.c"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "game"),
    "-I" + str(REPO_ROOT / "engine" / "code"),
    "-DARCH_STRING=\"test\"",
    "-DOS_STRING=\"linux\"",
    "-DID_INLINE=inline",
    "-DQ3_LITTLE_ENDIAN",
    "-o",
    str(TEST_BINARY),
]


def test_bot_path_spawn_helpers_behavior() -> None:
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
    assert "ok" in result.stdout
