#include <assert.h>
#include <stdio.h>

#include "g_local.h"
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "ai_main.h"
#include "ai_dmq3.h"

gentity_t g_entities[MAX_GENTITIES];
level_locals_t level;
int gametype;

/* These driving scenarios use navigable ground, not CTF off-mesh recovery. */
int BotPointAreaNum(vec3_t origin) { (void)origin; return 1; }
int trap_AAS_AreaTravelTimeToGoalArea(int area, vec3_t origin, int goal, int flags) {
    (void)area; (void)origin; (void)goal; (void)flags; return 1;
}

typedef struct {
    float minx, maxx, miny, maxy;
} rectangle_t;

static rectangle_t walls[8];
static int wallCount;
static int wallEntity[8];
static int wallContents[8];
static qboolean leftCliff, rightCliff, leftLava;
static qboolean farCliff, middlePit;
static qboolean overlappingCar;
static qboolean sideDrop, turnCliff;
static float floorSlope;
static int routeResets, routeMoves;
static bot_moveresult_t routeResult;
static qboolean routeViewAvailable;
static gclient_t testClient;
static vec3_t routeView;

/* A swept box against real rectangles catches pillars a center ray misses. */
static qboolean intersect(float start, float change, float low, float high,
        float *enter, float *leave) {
    float first, last;
    if (fabs(change) < 0.0001f) return start >= low && start <= high;
    first = (low - start) / change;
    last = (high - start) / change;
    if (first > last) {
        float swap = first;
        first = last;
        last = swap;
    }
    if (first > *enter) *enter = first;
    if (last < *leave) *leave = last;
    return *enter <= *leave;
}

void trap_Trace(trace_t *trace, const vec3_t start, const vec3_t mins,
        const vec3_t maxs, const vec3_t end, int passEntityNum, int contents) {
    int i;
    vec3_t delta;
    memset(trace, 0, sizeof(*trace));
    trace->fraction = 1;
    trace->entityNum = ENTITYNUM_NONE;
    VectorCopy(end, trace->endpos);
    VectorSubtract(end, start, delta);
    if (!mins && fabs(delta[0]) < 0.001f && fabs(delta[1]) < 0.001f && delta[2] < 0) {
        if (overlappingCar && (contents & CONTENTS_BODY)) {
            trace->fraction = 0;
            trace->startsolid = qtrue;
            trace->entityNum = 1;
            return;
        }
        if ((sideDrop && start[0] > 20 && start[1] < -20) ||
                (turnCliff && start[0] > 0 && start[1] > 1.1f * start[0] &&
                start[0]*start[0] + start[1]*start[1] > 10000) ||
                (farCliff && start[0] > 250) ||
                (middlePit && start[0] > 150 && start[0] < 220)) return;
        if (start[0] > 20 && ((leftCliff && start[1] > 55) ||
                (rightCliff && start[1] < -55))) return;
        if (start[2] >= floorSlope * start[0] && end[2] < floorSlope * start[0]) {
            trace->fraction = (start[2] - floorSlope * start[0]) / -delta[2];
            VectorMA(start, trace->fraction, delta, trace->endpos);
            VectorSet(trace->plane.normal, -floorSlope, 0, 1);
            VectorNormalize(trace->plane.normal);
            trace->entityNum = ENTITYNUM_WORLD;
        }
        return;
    }
    for (i = 0; i < wallCount; i++) {
        if (!(contents & wallContents[i])) continue;
        float enter = 0, leave = 1;
        float minx = walls[i].minx - (maxs ? maxs[0] : 0);
        float maxx = walls[i].maxx - (mins ? mins[0] : 0);
        float miny = walls[i].miny - (maxs ? maxs[1] : 0);
        float maxy = walls[i].maxy - (mins ? mins[1] : 0);
        if (intersect(start[0], delta[0], minx, maxx, &enter, &leave) &&
                intersect(start[1], delta[1], miny, maxy, &enter, &leave) &&
                enter >= 0 && enter < trace->fraction && leave >= 0) {
            trace->fraction = enter;
            trace->startsolid = enter == 0;
            trace->allsolid = enter == 0 && leave >= 1;
            if (enter == 0 && leave < 1) trace->fraction = 1; /* Q3 trace exits an initial overlap. */
            trace->contents = wallContents[i];
            trace->entityNum = wallEntity[i];
            VectorMA(start, enter, delta, trace->endpos);
            VectorSet(trace->plane.normal, -1, 0, 0);
        }
    }
}

int trap_PointContents(const vec3_t point, int passEntityNum) {
    return leftLava && point[0] > 20 && point[1] > 55 ? CONTENTS_LAVA : 0;
}

void trap_BotResetAvoidReach(int movestate) {
    routeResets++;
}

void trap_BotMoveToGoal(void *result, int movestate, void *goal, int travelflags) {
    *(bot_moveresult_t *)result = routeResult;
    routeMoves++;
}

int trap_BotMovementViewTarget(int movestate, void *goal, int travelflags,
        float lookahead, vec3_t target) {
    if (!routeViewAvailable) return qfalse;
    VectorCopy(routeView, target);
    return qtrue;
}

int trap_Cvar_VariableIntegerValue(const char *name) {
    return 0;
}

void QDECL G_LogPrintf(const char *fmt, ...) {
}

static void setup(bot_state_t *bs) {
    memset(bs, 0, sizeof(*bs));
    memset(g_entities, 0, sizeof(g_entities));
    memset(&level, 0, sizeof(level));
    memset(&testClient, 0, sizeof(testClient));
    memset(&routeResult, 0, sizeof(routeResult));
    level.time = 1000;
    level.maxclients = 8;
    gametype = GT_DEATHMATCH;
    bs->client = 0;
    bs->enemy = -1;
    bs->settings.skill = 3;
    bs->cur_ps.pm_type = PM_NORMAL;
    bs->cur_ps.stats[STAT_HEALTH] = 100;
    VectorSet(bs->origin, 0, 0, 64);
    VectorCopy(bs->origin, bs->cur_ps.origin);
    testClient.ps = bs->cur_ps;
    testClient.ps.stats[STAT_GEAR] = 1;
    testClient.pers.controlMode = CT_MOUSE;
    g_entities[0].client = &testClient;
    wallCount = routeMoves = routeResets = 0;
    { int i; for (i = 0; i < 8; i++) { wallEntity[i] = ENTITYNUM_WORLD; wallContents[i] = CONTENTS_SOLID; } }
    leftCliff = rightCliff = leftLava = qfalse;
    farCliff = middlePit = qfalse;
    overlappingCar = qfalse;
    sideDrop = turnCliff = qfalse;
    floorSlope = 0;
    routeViewAvailable = qtrue;
    VectorSet(routeView, 1000, 0, 64);
    VectorSet(routeResult.movedir, 1, 0, 0);
}

static float commandYaw(const usercmd_t *cmd) {
    return SHORT2ANGLE((cmd->angles[YAW] + testClient.ps.delta_angles[YAW]) & 65535);
}

static void test_steering_survives_aim_and_angle_deltas(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {600, 600, 64};
    setup(&bs);
    bs.cur_ps.viewangles[YAW] = testClient.ps.viewangles[YAW] = 90;
    testClient.ps.delta_angles[PITCH] = 1010;
    testClient.ps.delta_angles[YAW] = -23456;
    testClient.ps.delta_angles[ROLL] = 700;
    VectorSet(testClient.ps.velocity, 0, 900, 0);
    BotVehicleDrive(&bs, target, 600, NULL);
    /* Enemy aiming runs after planning, but must not replace the road target. */
    VectorSet(bs.ideal_viewangles, 20, 270, 0);
    VectorSet(bs.viewangles, 20, 270, 0);
    cmd.angles[YAW] = ANGLE2SHORT(270);
    cmd.buttons = BUTTON_ATTACK | BUTTON_HANDBRAKE;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(fabs(AngleSubtract(commandYaw(&cmd), 45)) < 0.02f);
    assert(((cmd.angles[PITCH] + testClient.ps.delta_angles[PITCH]) & 65535) == 0);
    assert(((cmd.angles[ROLL] + testClient.ps.delta_angles[ROLL]) & 65535) == 0);
    assert(cmd.forwardmove < 0); /* 900 forward exceeds the desired speed. */
    assert(cmd.rightmove == 0);
    assert(cmd.buttons & BUTTON_ATTACK);
    assert(!(cmd.buttons & BUTTON_HANDBRAKE));
}

static void test_behind_target_uses_a_turning_arc(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {-600, 0, 64};
    setup(&bs);
    BotVehicleDrive(&bs, target, 900, NULL);
    BotVehicleUpdateInput(&bs, &cmd);
    assert(fabs(WheelAngle(testClient.ps.viewangles[YAW], commandYaw(&cmd))) > 10);
    assert(bs.vehicle.targetSpeed > 0 && bs.vehicle.targetSpeed < 900);
}

static void test_keyboard_steering_and_reverse_sign(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {600, 160, 64};
    int normal;
    setup(&bs);
    testClient.pers.controlMode = CT_JOYSTICK;
    BotVehicleDrive(&bs, target, 600, NULL);
    BotVehicleUpdateInput(&bs, &cmd);
    normal = cmd.rightmove;
    assert(normal < 0); /* A target to the left needs the engine's left wheel sign. */
    testClient.ps.stats[STAT_GEAR] = -1;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.rightmove == -normal);
}

static void test_speed_control_brakes_and_holds(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {600, 0, 64};
    setup(&bs);
    BotVehicleDrive(&bs, target, 600, NULL);
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove > 0);
    testClient.ps.velocity[0] = 1000;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove < 0);

    BotVehicleDrive(&bs, target, 0, NULL);
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove < 0);
    assert(!(cmd.buttons & BUTTON_HANDBRAKE));
    testClient.ps.velocity[0] = 10;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove == 0);
    assert(cmd.buttons & BUTTON_HANDBRAKE);
}

static void test_vehicle_width_detects_off_center_pillar(void) {
    bot_state_t bs;
    vec3_t target = {600, 0, 64};
    trace_t centerRay;
    setup(&bs);
    walls[wallCount++] = (rectangle_t){100, 120, 22, 40};
    trap_Trace(&centerRay, bs.origin, NULL, NULL, target, 0, MASK_PLAYERSOLID);
    assert(centerRay.fraction == 1);
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked);
    assert(fabs(AngleSubtract(bs.vehicle.desiredYaw, 0)) > 10);
    assert(bs.vehicle.targetSpeed < 900);
}

static void test_avoidance_does_not_choose_a_cliff_or_lava(void) {
    bot_state_t bs;
    vec3_t target = {600, 0, 64};
    setup(&bs);
    walls[wallCount++] = (rectangle_t){80, 100, -10, 10};
    leftCliff = qtrue;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) < -10);
    setup(&bs);
    walls[wallCount++] = (rectangle_t){80, 100, -10, 10};
    leftLava = qtrue;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) < -10);
}

static void test_unsafe_diagonal_target_with_clear_front(void) {
    bot_state_t bs;
    vec3_t target = {600, 600, 64};
    setup(&bs);
    leftCliff = qtrue;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.clearance > 0.99f); /* Straight ahead remains safe. */
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) < -10);
    assert(bs.vehicle.targetSpeed < 900);

    setup(&bs);
    leftLava = qtrue;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.clearance > 0.99f);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) < -10);

    /* With drops on both sides, keep the safe straight corridor at low speed. */
    setup(&bs);
    leftCliff = rightCliff = qtrue;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.clearance > 0.99f);
    assert(fabs(AngleSubtract(bs.vehicle.desiredYaw, 0)) < 0.01f);
    assert(bs.vehicle.targetSpeed > 0 && bs.vehicle.targetSpeed <= 340);
}

static void test_passing_side_persists_but_rejects_new_danger(void) {
    bot_state_t bs;
    vec3_t target = {600, 0, 64};
    setup(&bs);
    walls[wallCount++] = (rectangle_t){120, 140, -8, 8};
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.avoidSign == -1);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) < -10);
    /* A small goal/obstacle change must not immediately reverse the detour. */
    target[1] = 250;
    level.time += 100;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.avoidSign == -1);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) < -10);
    /* Safety is re-evaluated during the commitment, never cached. */
    rightCliff = qtrue;
    level.time += 100;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.avoidSign == 1);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) > 10);
    BotVehicleDrive(&bs, target, 0, NULL);
    assert(bs.vehicle.targetSpeed == 0);
}

static void test_firing_lane_keeps_distant_enemy_but_avoids_contact(void) {
    bot_state_t bs;
    vec3_t target = {600, 0, 64};
    setup(&bs);
    bs.enemy = 1;
    g_entities[1].client = &testClient;
    bs.cur_ps.velocity[0] = 600;
    walls[wallCount++] = (rectangle_t){240, 270, -25, 25};
    wallEntity[0] = 1;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(!bs.vehicle.blocked);
    assert(fabs(AngleSubtract(bs.vehicle.desiredYaw, 0)) < 0.01f);
    assert(bs.vehicle.targetSpeed > 0 && bs.vehicle.targetSpeed < 900);
    /* The opponent's additional vehicle bounds identify the same target. */
    wallEntity[0] = 100;
    g_entities[100].flags = FL_EXTRA_BBOX;
    g_entities[100].r.ownerNum = 1;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(!bs.vehicle.blocked);
    /* A nearer opponent still requires a safe passing manoeuvre. */
    walls[0].minx = 120; walls[0].maxx = 140;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked);
    assert(fabs(AngleSubtract(bs.vehicle.desiredYaw, 0)) > 10);
    /* Neither a different car nor a wall receives combat-lane treatment. */
    walls[0].minx = 240; walls[0].maxx = 270;
    wallEntity[0] = 2;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked);
    wallEntity[0] = ENTITYNUM_WORLD;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked);
}

static void test_full_braking_corridor_detects_far_edge_and_pit(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {900, 0, 64};
    setup(&bs);
    bs.cur_ps.velocity[0] = 800;
    testClient.ps.velocity[0] = 800;
    farCliff = qtrue;
    BotVehicleDrive(&bs, target, 1000, NULL);
    assert(bs.vehicle.blocked);
    assert(bs.vehicle.targetSpeed < 800); /* Brake early; 180 units would miss the edge. */
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove < 0);
    assert(fabs(AngleSubtract(bs.vehicle.desiredYaw, 0)) < 0.01f); /* No unsafe side. */
    setup(&bs);
    bs.cur_ps.velocity[0] = 800;
    middlePit = qtrue;
    BotVehicleDrive(&bs, target, 1000, NULL);
    assert(bs.vehicle.blocked);
    assert(fabs(AngleSubtract(bs.vehicle.desiredYaw, 0)) > 10);
    assert(bs.vehicle.targetSpeed < 1000); /* Far bank does not hide the pit. */
}

static void test_overlapping_car_does_not_disable_ground_recovery(void) {
    bot_state_t bs;
    vec3_t target = {600, 0, 64};
    setup(&bs);
    overlappingCar = qtrue;
    for (level.time = 1000; level.time <= 2400; level.time += 100) {
        BotVehicleDrive(&bs, target, 600, NULL);
    }
    assert(bs.vehicle.recovery == BOT_VEHICLE_REVERSE);
    assert(bs.vehicle.targetSpeed < 0);
}

static void test_rear_blocked_recovery_does_not_back_into_wall(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {600, 0, 64};
    setup(&bs);
    walls[wallCount++] = (rectangle_t){-100, -70, -40, 40};
    for (level.time = 1000; level.time <= 2400; level.time += 100) {
        BotVehicleDrive(&bs, target, 600, NULL);
    }
    BotVehicleUpdateInput(&bs, &cmd);
    assert(bs.vehicle.recovery != BOT_VEHICLE_REVERSE);
    assert(bs.vehicle.targetSpeed >= 0);
    assert(cmd.forwardmove >= 0);

    /* An obstacle inside the stopping margin must stop reversal immediately. */
    setup(&bs);
    for (level.time = 1000; level.time <= 2400; level.time += 100) {
        BotVehicleDrive(&bs, target, 600, NULL);
    }
    assert(bs.vehicle.recovery == BOT_VEHICLE_REVERSE);
    assert(bs.vehicle.targetSpeed < 0);
    walls[wallCount++] = (rectangle_t){-55, -35, -40, 40};
    BotVehicleDrive(&bs, target, 600, NULL);
    BotVehicleUpdateInput(&bs, &cmd);
    assert(bs.vehicle.targetSpeed == 0);
    assert(cmd.forwardmove == 0);
}

static void test_stuck_recovery_and_route_replan_without_teleport(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {600, 0, 64};
    qboolean reversed = qfalse, turned = qfalse, resumed = qfalse;
    setup(&bs);
    bs.ltg_time = bs.nbg_time = 100;
    for (level.time = 1000; level.time < 18000; level.time += 100) {
        BotVehicleDrive(&bs, target, 600, NULL);
        BotVehicleUpdateInput(&bs, &cmd);
        if (bs.vehicle.recovery == BOT_VEHICLE_REVERSE) {
            reversed = qtrue;
            assert(cmd.forwardmove < 0);
        }
        if (reversed && bs.vehicle.recovery == BOT_VEHICLE_TURN) turned = qtrue;
        if (turned && bs.vehicle.recovery == BOT_VEHICLE_DRIVE) resumed = qtrue;
        assert(bs.cur_ps.origin[0] == 0 && bs.cur_ps.origin[1] == 0 && bs.cur_ps.origin[2] == 64);
        assert(testClient.ps.origin[0] == 0 && testClient.ps.origin[1] == 0 && testClient.ps.origin[2] == 64);
    }
    assert(reversed && turned && resumed);
    assert(routeResets > 0);
    assert(bs.ltg_time == 0 && bs.nbg_time == 0);
}

static void test_no_stale_dead_or_foreign_mode_commands(void) {
    bot_state_t bs;
    usercmd_t cmd = {0};
    vec3_t target = {600, 0, 64};
    setup(&bs);
    BotVehicleDrive(&bs, target, 600, NULL);
    cmd.forwardmove = cmd.rightmove = 100;
    cmd.buttons = BUTTON_ATTACK;
    level.time += 400;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove == 0 && cmd.rightmove == 0);
    assert(!bs.vehicle.active);
    assert(cmd.buttons & BUTTON_ATTACK); /* Respawn input remains usable. */

    setup(&bs);
    BotVehicleDrive(&bs, target, 600, NULL);
    testClient.ps.stats[STAT_HEALTH] = 0;
    cmd.forwardmove = cmd.rightmove = 100;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove == 0 && cmd.rightmove == 0);
    assert(!bs.vehicle.active);

    setup(&bs);
    cmd.forwardmove = 80;
    cmd.rightmove = 25;
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove == 80 && cmd.rightmove == 25);
    BotVehicleDrive(&bs, target, 600, NULL);
    assert(bs.vehicle.active);
    gametype = GT_TEAM;
    BotVehicleDrive(&bs, target, 600, NULL);
    BotVehicleUpdateInput(&bs, &cmd);
    assert(cmd.forwardmove == 80 && cmd.rightmove == 25);
}

static void test_route_waits_and_fallback_direction(void) {
    bot_state_t bs;
    bot_goal_t goal = {0};
    bot_moveresult_t result;
    setup(&bs);
    VectorSet(goal.origin, 50, 0, 64);
    routeResult.flags = MOVERESULT_WAITING;
    BotVehicleMoveToGoal(&bs, &result, &goal, 0);
    assert(routeMoves == 1);
    assert(bs.vehicle.targetSpeed == 0); /* Even when near the goal. */
    setup(&bs);
    VectorSet(goal.origin, 0, 800, 64);
    routeViewAvailable = qfalse;
    VectorSet(routeResult.movedir, 0, 1, 0);
    BotVehicleMoveToGoal(&bs, &result, &goal, 0);
    assert(bs.vehicle.targetSpeed > 0);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) > 10);
}

static void test_wheel_support_and_sloping_road(void) {
    bot_state_t bs;
    vec3_t target = {900, 0, 64};
    setup(&bs);
    sideDrop = qtrue;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked);
    assert(AngleSubtract(bs.vehicle.desiredYaw, 0) > 10); /* Center supported, right wheels over edge. */
    setup(&bs);
    floorSlope = 0.5f;
    bs.cur_ps.velocity[0] = 800;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(!bs.vehicle.blocked);
    assert(bs.vehicle.targetSpeed == 900); /* Follow the slope, rather than seeing a false cliff. */
    setup(&bs);
    walls[wallCount++] = (rectangle_t){100, 120, -20, 20};
    wallContents[0] = CONTENTS_BOTCLIP;
    BotVehicleDrive(&bs, target, 900, NULL);
    assert(bs.vehicle.blocked); /* Match the mask used by bot Pmove. */
}

static void test_overlap_escape_rejects_world_wall(void) {
    bot_state_t bs;
    vec3_t target = {900, 0, 64};
    setup(&bs);
    g_entities[1].inuse = qtrue;
    g_entities[1].client = &testClient;
    g_entities[1].r.contents = CONTENTS_BODY;
    VectorSet(g_entities[1].r.currentOrigin, 40, 0, 64);
    VectorSet(g_entities[1].r.absmin, 15, -25, 42);
    VectorSet(g_entities[1].r.absmax, 65, 25, 86);
    walls[wallCount++] = (rectangle_t){15, 65, -25, 25};
    wallEntity[0] = 1; wallContents[0] = CONTENTS_BODY;
    for (level.time = 1000; level.time <= 2400; level.time += 100) BotVehicleDrive(&bs, target, 600, NULL);
    assert(bs.vehicle.recovery == BOT_VEHICLE_REVERSE && bs.vehicle.targetSpeed < 0);
    walls[wallCount++] = (rectangle_t){-55, -35, -40, 40};
    BotVehicleDrive(&bs, target, 600, NULL);
    assert(bs.vehicle.targetSpeed == 0); /* The escape permission cannot bypass a rear wall. */
}

static void test_recovery_checks_actual_turn_corridor(void) {
    bot_state_t bs;
    vec3_t target = {900, 0, 64};
    setup(&bs);
    BotVehicleDrive(&bs, target, 600, NULL);
    bs.vehicle.recovery = BOT_VEHICLE_TURN;
    bs.vehicle.recoveryUntil = 1800;
    bs.vehicle.turnSign = 1;
    turnCliff = qtrue;
    BotVehicleDrive(&bs, target, 600, NULL);
    assert(bs.vehicle.targetSpeed == 0); /* Reject a cliff in the commanded 50-degree turn corridor. */
}

int main(void) {
    test_steering_survives_aim_and_angle_deltas();
    test_behind_target_uses_a_turning_arc();
    test_keyboard_steering_and_reverse_sign();
    test_speed_control_brakes_and_holds();
    test_vehicle_width_detects_off_center_pillar();
    test_avoidance_does_not_choose_a_cliff_or_lava();
    test_unsafe_diagonal_target_with_clear_front();
    test_passing_side_persists_but_rejects_new_danger();
    test_firing_lane_keeps_distant_enemy_but_avoids_contact();
    test_full_braking_corridor_detects_far_edge_and_pit();
    test_overlapping_car_does_not_disable_ground_recovery();
    test_rear_blocked_recovery_does_not_back_into_wall();
    test_stuck_recovery_and_route_replan_without_teleport();
    test_no_stale_dead_or_foreign_mode_commands();
    test_route_waits_and_fallback_direction();
    test_wheel_support_and_sloping_road();
    test_overlap_escape_rejects_world_wall();
    test_recovery_checks_actual_turn_corridor();
    puts("ok");
    return 0;
}
