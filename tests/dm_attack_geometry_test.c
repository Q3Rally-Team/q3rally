#include <assert.h>
#include <stdio.h>

#include "g_local.h"
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ai_char.h"
#include "../botlib/be_ai_chat.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "../botlib/be_ai_weap.h"
#include "ai_main.h"
#include "ai_dmq3.h"
#include "chars.h"

gentity_t g_entities[MAX_GENTITIES];
float floattime;
int gametype;

static int attacks, traces;
static int interceptedBy;
static qboolean muzzleBlocked;
static float reactionTime;
static float throttleCharacteristic;
static weaponinfo_t weapon;
static vec3_t visibilityStart, shotStart, shotEnd;

void BotEntityInfo(int entnum, aas_entityinfo_t *info) {
    memset(info, 0, sizeof(*info));
    info->valid = qtrue;
    info->number = entnum;
}

float trap_Characteristic_BFloat(int character, int index, float min, float max) {
    return index == CHARACTERISTIC_REACTIONTIME ? reactionTime :
        index == CHARACTERISTIC_FIRETHROTTLE ? throttleCharacteristic : 1.0f;
}

void trap_BotGetWeaponInfo(int weaponstate, int weaponnum, void *info) {
    *(weaponinfo_t *)info = weapon;
}

int BotSameTeam(bot_state_t *bs, int entnum) {
    return entnum == 2;
}

void trap_EA_Attack(int client) {
    ++attacks;
}

void BotAI_Trace(bsp_trace_t *trace, vec3_t start, vec3_t mins, vec3_t maxs,
        vec3_t end, int passent, int contents) {
    ++traces;
    memset(trace, 0, sizeof(*trace));
    trace->fraction = 1.0f;
    trace->ent = ENTITYNUM_NONE;
    if (!mins) {
        VectorCopy(start, visibilityStart);
        /* Cover reaches the muzzle but leaves the old camera/eye ray clear. */
        if (muzzleBlocked && start[2] < 30.0f) {
            trace->fraction = 0.1f;
            trace->ent = ENTITYNUM_WORLD;
        }
    } else {
        VectorCopy(start, shotStart);
        VectorCopy(end, shotEnd);
        trace->ent = interceptedBy;
        trace->fraction = interceptedBy == ENTITYNUM_WORLD ? 0.01f : 0.5f;
    }
}

#include "dm_attack_under_test.h"

static void setup(bot_state_t *bs) {
    memset(bs, 0, sizeof(*bs));
    memset(g_entities, 0, sizeof(g_entities));
    memset(&weapon, 0, sizeof(weapon));
    gametype = GT_DEATHMATCH;
    floattime = 100.0f;
    bs->enemy = 1;
    bs->weaponnum = WP_MACHINEGUN;
    bs->settings.skill = 5; /* Geometry cases use uninterrupted fire. */
    bs->cur_ps.viewheight = 40;
    VectorSet(bs->eye, 0, 0, 40);
    VectorSet(bs->aimtarget, 600, 0, 22);
    attacks = traces = 0;
    interceptedBy = 1;
    muzzleBlocked = qfalse;
    reactionTime = 0.25f;
    throttleCharacteristic = 1;
}

static void test_body_direction_controls_release(void) {
    bot_state_t bs;
    setup(&bs);
    bs.cur_ps.viewangles[YAW] = 90.0f;
    BotCheckAttack(&bs);
    assert(attacks == 0);
    assert(traces == 0);

    setup(&bs);
    bs.viewangles[YAW] = 90.0f; /* Turning input must not turn the gun. */
    BotCheckAttack(&bs);
    assert(attacks == 1);
    assert(fabs(shotStart[0] - 14.0f) < 0.01f);
    assert(fabs(shotStart[1]) < 0.01f);
    assert(fabs(shotStart[2] - 22.0f) < 0.01f);
    assert(fabs(shotEnd[0] - 1014.0f) < 0.01f);
    assert(fabs(shotEnd[1]) < 0.01f);
}

static void test_tilted_body_and_entity_muzzle(void) {
    bot_state_t bs;
    vec3_t forward, right, up, expected, delta;
    setup(&bs);
    VectorSet(bs.cur_ps.viewangles, -12, 45, 17);
    VectorSet(g_entities[0].s.pos.trBase, 100.8f, -30.2f, 18.7f);
    /* bs->origin may lag the authoritative trajectory origin used to fire. */
    VectorSet(bs.origin, 90, -25, 17);
    AngleVectors(bs.cur_ps.viewangles, forward, right, up);
    VectorCopy(g_entities[0].s.pos.trBase, expected);
    VectorMA(expected, 22, up, expected);
    VectorMA(expected, 14, forward, expected);
    SnapVector(expected);
    VectorMA(expected, 600, forward, bs.aimtarget);
    BotCheckAttack(&bs);
    assert(attacks == 1);
    VectorSubtract(shotStart, expected, delta);
    assert(VectorLengthSquared(delta) < 0.0001f);
    VectorSubtract(visibilityStart, expected, delta);
    assert(VectorLengthSquared(delta) < 0.0001f);
    VectorMA(expected, 1000, forward, expected);
    VectorSubtract(shotEnd, expected, delta);
    assert(VectorLengthSquared(delta) < 0.0001f);
}

static void test_cover_reaction_and_weapon_safety(void) {
    bot_state_t bs;
    setup(&bs);
    muzzleBlocked = qtrue;
    BotCheckAttack(&bs);
    assert(attacks == 0);
    assert(traces == 1);

    setup(&bs);
    bs.enemysight_time = floattime - 0.1f;
    BotCheckAttack(&bs);
    assert(attacks == 0 && traces == 0);

    setup(&bs);
    interceptedBy = 2; /* Existing teammate interception guard. */
    BotCheckAttack(&bs);
    assert(attacks == 0);

    setup(&bs);
    interceptedBy = ENTITYNUM_WORLD;
    weapon.proj.damagetype = DAMAGETYPE_RADIAL;
    weapon.proj.damage = 100;
    weapon.proj.radius = 120;
    BotCheckAttack(&bs);
    assert(attacks == 0);
}

static void test_legacy_modes_keep_their_geometry(void) {
    bot_state_t bs;
    setup(&bs);
    gametype = GT_TEAM;
    bs.cur_ps.viewangles[YAW] = 90.0f;
    BotCheckAttack(&bs);
    assert(attacks == 1);
    assert(fabs(visibilityStart[2] - 40.0f) < 0.01f);
    assert(fabs(shotStart[0] + 12.0f) < 0.01f);
    assert(fabs(shotStart[2] - 40.0f) < 0.01f);

    setup(&bs);
    gametype = GT_TEAM;
    bs.viewangles[YAW] = 90.0f;
    BotCheckAttack(&bs);
    assert(attacks == 0);
}

static void test_skill_reaction_and_fixed_gun_release(void) {
    bot_state_t bs;
    setup(&bs);
    reactionTime = 0.8f;
    bs.enemysight_time = floattime - 0.45f;
    bs.settings.skill = 1;
    BotCheckAttack(&bs);
    assert(attacks == 0);
    bs.settings.skill = 5;
    BotCheckAttack(&bs);
    assert(attacks == 1);

    setup(&bs);
    reactionTime = 0;
    bs.settings.skill = 5;
    bs.enemysight_time = floattime - 0.05f;
    BotCheckAttack(&bs);
    assert(attacks == 0); /* Even skill 5 keeps a real reaction delay. */

    setup(&bs);
    throttleCharacteristic = 0;
    bs.settings.skill = 1;
    srand(1);
    BotCheckAttack(&bs);
    assert(bs.firethrottlewait_time > floattime);
    assert(attacks == 0); /* A chosen pause applies to this tick as well. */
    setup(&bs);
    throttleCharacteristic = 0;
    bs.settings.skill = 5;
    srand(1);
    BotCheckAttack(&bs);
    assert(bs.firethrottlewait_time == 0);
    assert(bs.firethrottleshoot_time >= floattime + 0.99f && attacks == 1);
}

static void test_lower_skill_release_cadence(void) {
    bot_state_t bs;
    int skill, tick, previous = -1;
    for (skill = 1; skill <= 5; skill++) {
        setup(&bs);
        bs.settings.skill = skill;
        bs.enemysight_time = 100;
        throttleCharacteristic = 1; /* A bold personality must not erase the lower-skill cap. */
        srand(42);
        for (tick = 0; tick < 800; tick++) {
            floattime = 100 + tick * 0.1f;
            BotCheckAttack(&bs);
        }
        assert(attacks > previous);
        previous = attacks;
        if (skill == 1) assert(attacks < 400);
        if (skill == 5) assert(attacks > 780); /* Preserve the existing full Nightmare bursts. */
    }
}

static void test_nightmare_profile_values_remain_unchanged(void) {
    bot_state_t bs;
    int seed;
    setup(&bs);
    reactionTime = 0.8f;
    throttleCharacteristic = 0.6f;
    bs.enemysight_time = floattime - 0.4f;
    srand(1);
    BotCheckAttack(&bs);
    assert(attacks == 1 && fabs(bs.firethrottleshoot_time - floattime - 1) < 0.001f);
    for (seed = 1; seed < 10000; seed++) {
        srand(seed);
        if (random() > 0.95f) break;
    }
    assert(seed < 10000);
    setup(&bs);
    throttleCharacteristic = 0.6f;
    srand(seed);
    BotCheckAttack(&bs);
    assert(attacks == 0 && fabs(bs.firethrottlewait_time - floattime - 0.12f) < 0.001f);
}

int main(void) {
    test_body_direction_controls_release();
    test_tilted_body_and_entity_muzzle();
    test_cover_reaction_and_weapon_safety();
    test_legacy_modes_keep_their_geometry();
    test_skill_reaction_and_fixed_gun_release();
    test_lower_skill_release_cadence();
    test_nightmare_profile_values_remain_unchanged();
    puts("ok");
    return 0;
}
