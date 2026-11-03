"""BPD file selection for maps with track length variants."""
import atexit
import pathlib
import shutil
import subprocess
import tempfile

REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
CODE = REPO_ROOT / "engine" / "code"
BUILD_DIR = pathlib.Path(tempfile.mkdtemp(prefix="q3rally-test-"))
atexit.register(shutil.rmtree, BUILD_DIR, True)
TEST_BINARY = BUILD_DIR / "bpd_path_test"


def test_bpd_path_helpers() -> None:
    subprocess.run(
        ["gcc", "-Wall", "-Werror", str(REPO_ROOT / "tests" / "bpd_path_test.c"), "-o", str(TEST_BINARY)],
        check=True,
    )
    result = subprocess.run([str(TEST_BINARY)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout


def test_game_and_cgame_share_the_bpd_rule() -> None:
    spawn = (CODE / "game" / "g_spawn.c").read_text()
    loader = spawn.split("void LoadBezierPathFile_Think(gentity_t *ent) {", 1)[1].split("\n}\n", 1)[0]
    assert "BG_BpdSelectSource(" in loader
    assert "level.trackHasLengthVariants" in loader
    assert "level.bpdPath" in loader
    assert '"maps/%s.bpd"' not in loader

    # Checkpoints are inspected before any spawn filter can free them.
    spawn_vars = spawn.split("void G_SpawnGEntityFromSpawnVars( void ) {", 1)[1]
    assert spawn_vars.index("BG_BpdIsLengthRestricted(") < spawn_vars.index('"notsingle"')

    intro = (CODE / "game" / "g_rally_observer_cams.c").read_text()
    assert "level.bpdPath" in intro
    assert '"maps/%s.bpd"' not in intro

    intro_cam = (CODE / "cgame" / "cg_rally_intro_cam.c").read_text()
    assert "BG_BpdPathMatchesMap( path, mapName )" in intro_cam

    save = (CODE / "cgame" / "cg_consolecmds.c").read_text()
    save = save.split("static void CG_SaveBezierPoints_f( void )", 1)[1]
    assert "CG_IntroCam_BezierPointsPath(" in save
    assert '"maps/%s.bpd"' not in save
