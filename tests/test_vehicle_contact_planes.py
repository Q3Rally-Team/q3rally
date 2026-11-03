"""Sliding contacts must not retain velocity into a wall, floor, or third plane."""
import pathlib
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_vehicle_contact_plane_velocity() -> None:
    code = ROOT / "engine" / "code"
    source = (code / "game" / "bg_physics.c").read_text()
    start = source.index("static void PM_ClipCarPointVelocity(")
    function = source[start:source.index("\n}\n", start) + 3]
    assert "PM_ClipCarPointVelocity( vel, clipPlanes, numplanes );" in source
    with tempfile.TemporaryDirectory(prefix="q3rally-contact-planes-") as temp:
        build = pathlib.Path(temp)
        (build / "contact_planes_under_test.h").write_text(function)
        binary = build / "contact_planes_test"
        subprocess.run([
            "gcc", str(ROOT / "tests" / "vehicle_contact_planes_test.c"),
            str(code / "qcommon" / "q_math.c"),
            "-I" + str(build), "-I" + str(code / "qcommon"),
            '-DARCH_STRING="test"', '-DOS_STRING="linux"',
            "-DID_INLINE=inline", "-o", str(binary), "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], capture_output=True, text=True)
        assert result.returncode == 0, result.stderr

