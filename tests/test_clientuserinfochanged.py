import pathlib
import subprocess

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
TEST_BINARY = REPO_ROOT / "tests" / "clientuserinfochanged_test"

COMPILE_ARGS = [
    "gcc",
    str(REPO_ROOT / "tests" / "clientuserinfochanged_test.c"),
    str(REPO_ROOT / "engine" / "code" / "game" / "g_client_userinfo.c"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "game"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "qcommon"),
    "-I" + str(REPO_ROOT / "engine" / "code"),
    "-DARCH_STRING=\"test\"",
    "-DOS_STRING=\"linux\"",
    "-DID_INLINE=inline",
    "-DPATH_SEP='/'",
    "-DDLL_EXT=\".so\"",
    "-DQ3_LITTLE_ENDIAN",
    "-o",
    str(TEST_BINARY),
]


def test_clientuserinfochanged_configstring_updates_only_on_changes():
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
    assert "ok" in result.stdout
