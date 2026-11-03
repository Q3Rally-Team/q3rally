"""A proposed rigid-body step must not tunnel through a thin world brush."""
import pathlib
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_vehicle_world_guard() -> None:
    code = ROOT / "engine" / "code"
    source = (code / "game" / "bg_physics.c").read_text()
    start = source.index("static void PM_ConstrainCarBodyToWorld(")
    function = source[start:source.index("\n}\n", start) + 3]
    assert source.count("PM_ConstrainCarBodyToWorld( car );") == 3
    with tempfile.TemporaryDirectory(prefix="q3rally-world-guard-") as temp:
        build = pathlib.Path(temp)
        (build / "world_guard_under_test.h").write_text(function)
        binary = build / "world_guard_test"
        subprocess.run([
            "gcc", str(ROOT / "tests" / "vehicle_world_guard_test.c"),
            str(code / "qcommon" / "q_math.c"),
            "-I" + str(build), "-I" + str(code / "game"),
            '-DARCH_STRING="test"', '-DOS_STRING="linux"',
            "-DID_INLINE=inline", "-o", str(binary), "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], capture_output=True, text=True)
        assert result.returncode == 0, result.stderr

