import pathlib
import subprocess
import tempfile

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE_FILE = REPO_ROOT / "engine" / "code" / "server" / "sv_ladder.c"


def test_ladder_ghost_serialisation_and_endpoint() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = pathlib.Path(tmp)
        code = REPO_ROOT / "engine" / "code"
        patched = []
        for line in SOURCE_FILE.read_text().splitlines():
            if line.startswith('#include "../'):
                patched.append(line.replace('#include "../', '#include "' + str(code) + '/'))
            else:
                patched.append(line)
        (tmp_path / "sv_ladder_ghost_for_test.c").write_text("\n".join(patched) + "\n")
        binary = tmp_path / "ladder_ghost_test"
        subprocess.run([
            "gcc",
            "-I" + str(tmp_path),
            "-I" + str(REPO_ROOT / "tests"),
            "-I" + str(code / "server"),
            "-I" + str(code / "qcommon"),
            "-I" + str(code / "game"),
            "-DARCH_STRING=\"test\"",
            "-DOS_STRING=\"linux\"",
            "-DID_INLINE=inline",
            "-DPATH_SEP='/'",
            "-DDLL_EXT=\".so\"",
            "-DQ3_LITTLE_ENDIAN",
            "-DUNIT_TEST",
            str(REPO_ROOT / "tests" / "ladder_ghost_test.c"),
            "-o",
            str(binary),
        ], check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout
