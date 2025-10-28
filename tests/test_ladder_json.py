import pathlib
import subprocess

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE_FILE = REPO_ROOT / "engine" / "code" / "server" / "sv_ladder.c"
GENERATED_SOURCE = REPO_ROOT / "tests" / "sv_ladder_for_test.c"
TEST_BINARY = REPO_ROOT / "tests" / "ladder_json_test"

COMPILE_ARGS = [
    "gcc",
    "-I" + str(REPO_ROOT / "tests"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "server"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "qcommon"),
    "-I" + str(REPO_ROOT / "engine" / "code" / "game"),
    "-DARCH_STRING=\"test\"",
    "-DOS_STRING=\"linux\"",
    "-DID_INLINE=inline",
    "-DPATH_SEP='/'",
    "-DDLL_EXT=\".so\"",
    "-DQ3_LITTLE_ENDIAN",
    "-DUNIT_TEST",
    str(REPO_ROOT / "tests" / "ladder_json_test.c"),
    "-o",
    str(TEST_BINARY),
]


def _prepare_source() -> None:
    patched_lines = []
    for line in SOURCE_FILE.read_text().splitlines():
        if line.startswith('#include "../'):
            patched_lines.append(line.replace('#include "../', '#include "../engine/code/'))
        else:
            patched_lines.append(line)
    GENERATED_SOURCE.write_text("\n".join(patched_lines) + "\n")


def test_ladder_json_includes_lap_data() -> None:
    _prepare_source()
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
        GENERATED_SOURCE.unlink(missing_ok=True)
    assert "ok" in result.stdout
