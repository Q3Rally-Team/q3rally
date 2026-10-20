import pathlib
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_sustained_profile_speed():
    with tempfile.TemporaryDirectory() as tmp:
        binary = pathlib.Path(tmp) / "profile_speed_test"
        subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra",
                        str(ROOT / "tests/profile_speed_test.c"),
                        str(ROOT / "engine/code/qcommon/q_shared.c"),
                        str(ROOT / "engine/code/qcommon/q_math.c"),
                        '-DARCH_STRING="test"', '-DOS_STRING="linux"',
                        '-DID_INLINE=inline', "-DPATH_SEP='/'", '-DDLL_EXT=".so"',
                        "-o", str(binary), "-lm"], check=True)
        subprocess.run([str(binary)], check=True)
