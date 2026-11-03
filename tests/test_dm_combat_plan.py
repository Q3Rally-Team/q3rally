"""Exercise the production DM combat planner and fight-node command ordering."""

import pathlib
import subprocess
import tempfile


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
CODE = REPO_ROOT / "engine" / "code"


def _function(source: str, signature: str) -> str:
    start = source.index(signature)
    return source[start:source.index("\n}\n", start) + 3]


def test_dm_combat_plan_behavior() -> None:
    source = (CODE / "game" / "ai_dmq3.c").read_text()
    network = (CODE / "game" / "ai_dmnet.c").read_text()
    constants_start = source.index("#define RALLY_COMBAT_APPROACH")
    constants_end = source.index("static qboolean BotUseRallyCombatMovement", constants_start)
    # Exercise the real new dispatch before handing legacy movement to a stub.
    # This intentionally excludes the existing legacy movement implementation.
    dispatch = _function(source, "bot_moveresult_t BotAttackMove(")
    legacy_start = dispatch.index("\tif (bs->attackchase_time")
    dispatch = dispatch[:legacy_start] + "\treturn LegacyCombatMove( bs, tfl );\n}\n"
    aggression = _function(source, "float BotAggression(")
    aggression = aggression[:aggression.index("\t//if the bot has quad")] + "\treturn 37;\n}\n"
    fight = _function(network, "int AINode_Battle_Fight(")
    command_start = fight.index("\t//choose the best weapon to fight with")
    fight_tail = (
        "static int FightCommandsUnderTest( bot_state_t *bs ) {\n"
        "bot_moveresult_t moveresult;\n"
        "aas_entityinfo_t entinfo;\n"
        "BotEntityInfo( bs->enemy, &entinfo );\n"
        + fight[command_start:]
    )
    functions = [
        source[constants_start:constants_end],
        _function(source, "static float BotDeathmatchSkill("),
        _function(source, "static int BotFirstUsableFrontWeapon("),
        _function(source, "static float BotDeathmatchAggression("),
        aggression,
        _function(source, "static qboolean BotIsRallyCombatMode("),
        _function(source, "int BotNeedsCombatSupplies("),
        _function(source, "static void BotRallyCombatSetPhase("),
        _function(source, "static bot_moveresult_t BotDeathmatchCombatMove("),
        dispatch,
        fight_tail,
    ]
    with tempfile.TemporaryDirectory(prefix="q3rally-dm-plan-") as tmp:
        build = pathlib.Path(tmp)
        (build / "dm_combat_plan_under_test.h").write_text("\n".join(functions))
        binary = build / "dm_combat_plan_test"
        subprocess.run([
            "gcc", str(REPO_ROOT / "tests" / "dm_combat_plan_test.c"),
            str(CODE / "qcommon" / "q_math.c"),
            str(CODE / "qcommon" / "q_shared.c"),
            "-I" + str(build), "-I" + str(CODE / "game"),
            "-I" + str(REPO_ROOT / "baseq3r" / "botfiles"),
            '-DARCH_STRING="test"', '-DOS_STRING="linux"',
            "-DID_INLINE=inline", "-ffunction-sections", "-fdata-sections",
            "-Wl,--gc-sections", "-o", str(binary), "-lm",
        ], check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    assert "ok" in result.stdout
