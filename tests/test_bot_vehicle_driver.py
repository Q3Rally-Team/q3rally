"""Run the production vehicle driver against a small deterministic collision world."""

import pathlib
import subprocess
import tempfile


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_bot_vehicle_driver_behavior() -> None:
    code = REPO_ROOT / "engine" / "code"
    with tempfile.TemporaryDirectory(prefix="q3rally-vehicle-") as temporary:
        binary = pathlib.Path(temporary) / "bot_vehicle_driver_test"
        subprocess.run(
            [
                "gcc",
                str(REPO_ROOT / "tests" / "bot_vehicle_driver_test.c"),
                str(code / "game" / "ai_vehicle.c"),
                str(code / "qcommon" / "q_math.c"),
                str(code / "qcommon" / "q_shared.c"),
                "-I" + str(code / "game"),
                '-DARCH_STRING="test"',
                '-DOS_STRING="linux"',
                "-DID_INLINE=inline",
                "-ffunction-sections",
                "-fdata-sections",
                "-Wl,--gc-sections",
                "-o",
                str(binary),
                "-lm",
            ],
            check=True,
        )
        result = subprocess.run([str(binary)], capture_output=True, text=True)
        assert result.returncode == 0, result.stderr
        assert "ok" in result.stdout
