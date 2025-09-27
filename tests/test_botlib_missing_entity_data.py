import pathlib
import subprocess

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
TEST_BINARY = REPO_ROOT / "tests" / "botlib_missing_entity_data_test"

COMPILE_ARGS = [
    "gcc",
    str(REPO_ROOT / "tests" / "botlib_missing_entity_data_test.c"),
    str(REPO_ROOT / "engine" / "code" / "botlib" / "be_aas_bspq3.c"),
    f"-I{REPO_ROOT / 'engine' / 'code' / 'botlib'}",
    f"-I{REPO_ROOT / 'engine' / 'code' / 'qcommon'}",
    f"-I{REPO_ROOT / 'engine' / 'code'}",
    "-DARCH_STRING=\"test\"",
    "-DOS_STRING=\"linux\"",
    "-DID_INLINE=inline",
    "-DPATH_SEP='/'",
    "-DDLL_EXT=\".so\"",
    "-DQ3_LITTLE_ENDIAN",
    "-o",
    str(TEST_BINARY),
]


def test_missing_entity_data_regression():
    subprocess.run(COMPILE_ARGS, check=True)
    try:
        result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    finally:
        TEST_BINARY.unlink(missing_ok=True)
    assert "ok" in result.stdout
