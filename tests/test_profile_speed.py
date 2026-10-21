import pathlib
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_sustained_profile_speed():
    with tempfile.TemporaryDirectory() as tmp:
        binary = pathlib.Path(tmp) / "profile_speed_test"
        subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra",
                        str(ROOT / "tests/profile_speed_test.c"),
                        str(ROOT / "engine/code/game/g_profile_speed.c"),
                        str(ROOT / "engine/code/qcommon/q_shared.c"),
                        str(ROOT / "engine/code/qcommon/q_math.c"),
                        '-DARCH_STRING="test"', '-DOS_STRING="linux"',
                        '-DID_INLINE=inline', "-DPATH_SEP='/'", '-DDLL_EXT=".so"',
                        "-o", str(binary), "-lm"], check=True)
        subprocess.run([str(binary)], check=True)


def test_profile_frame_repeated_server_timestamps():
    source = (ROOT / "engine/code/game/g_profile.c").read_text()
    start = source.index("void G_Profile_UpdateClientFrame(")
    end = source.index("\nvoid G_Profile_AddScore(", start)
    with tempfile.TemporaryDirectory() as tmp:
        tmp = pathlib.Path(tmp)
        (tmp / "profile_update_under_test.h").write_text(source[start:end])
        binary = tmp / "profile_speed_frame_test"
        subprocess.run([
            "cc", "-std=c99", "-I" + str(tmp),
            str(ROOT / "tests/profile_speed_frame_test.c"),
            str(ROOT / "engine/code/game/g_profile_speed.c"),
            str(ROOT / "engine/code/qcommon/q_shared.c"),
            str(ROOT / "engine/code/qcommon/q_math.c"),
            '-DARCH_STRING="test"', '-DOS_STRING="linux"',
            '-DID_INLINE=inline', "-DPATH_SEP='/'", '-DDLL_EXT=".so"',
            "-o", str(binary), "-lm",
        ], check=True)
        subprocess.run([str(binary)], check=True)
