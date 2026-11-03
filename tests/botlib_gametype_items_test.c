/* Exercise actual botlib item lookups and both goal selectors. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../engine/code/botlib/be_ai_goal.c"

botlib_import_t botimport;

float AAS_Time(void) { return 1.0f; }
int BotReachabilityArea(vec3_t origin, int client) { return 1; }
int AAS_AreaReachability(int area) { return 1; }
int AAS_AreaTravelTimeToGoalArea(int area, vec3_t origin, int goalarea, int flags) {
    return 10;
}
float FuzzyWeight(int *inventory, weightconfig_t *config, int number) {
    return 1.0f;
}
float FuzzyWeightUndecided(int *inventory, weightconfig_t *config, int number) {
    return 1.0f;
}
void QDECL Log_Write(char *fmt, ...) {}
void QDECL Com_Error(int level, const char *fmt, ...) {
    assert(0 && "unexpected Com_Error");
}
void QDECL Com_Printf(const char *fmt, ...) {}

static void TestItemFlags(int gametype, int flags, int available) {
    iteminfo_t info;
    itemconfig_t config;
    levelitem_t item;
    bot_goalstate_t state;
    weightconfig_t weights;
    int weightindex = 0;
    int inventory[1] = {0};
    vec3_t origin = {0, 0, 0};
    bot_goal_t goal;

    memset(&info, 0, sizeof(info));
    memset(&config, 0, sizeof(config));
    memset(&item, 0, sizeof(item));
    memset(&state, 0, sizeof(state));
    memset(&weights, 0, sizeof(weights));
    strcpy(info.name, "test weapon");
    config.numiteminfo = 1;
    config.iteminfo = &info;
    itemconfig = &config;
    item.number = 1;
    item.entitynum = 100;
    item.goalareanum = 2;
    item.flags = flags;
    levelitems = &item;
    g_gametype = gametype;
    state.itemweightconfig = &weights;
    state.itemweightindex = &weightindex;
    botgoalstates[1] = &state;

    assert((BotGetLevelItemGoal(-1, info.name, &goal) == 1) == available);
    assert(BotChooseLTGItem(1, origin, inventory, 0) == available);
    if (available) {
        assert(state.goalstacktop == 1);
        assert(state.goalstack[1].entitynum == 100);
    }
    BotResetAvoidGoals(1);
    state.goalstacktop = 0;
    assert(BotChooseNBGItem(1, origin, inventory, 0, NULL, 100) == available);
    if (available) {
        assert(state.goalstacktop == 1);
        assert(state.goalstack[1].entitynum == 100);
    }
}

int main(void) {
    static const int nonteam[] = {
        GT_RACING, GT_RACING_DM, GT_DERBY, GT_LCS, GT_ELIMINATION,
        GT_DEATHMATCH, GT_SPRINT, GT_GHOST
    };
    int i;

    assert(GT_DEATHMATCH == 6 && GT_TEAM == 16 && GT_SINGLE_PLAYER == 2);
    for (i = 0; i < sizeof(nonteam) / sizeof(nonteam[0]); i++) {
        TestItemFlags(nonteam[i], 0, 1);
        TestItemFlags(nonteam[i], IFL_NOTTEAM, 1);
        TestItemFlags(nonteam[i], IFL_NOTSINGLE, 1);
        TestItemFlags(nonteam[i], IFL_NOTFREE, 0);
        TestItemFlags(nonteam[i], IFL_NOTBOT, 0);
    }
    for (i = GT_TEAM; i < GT_MAX_GAME_TYPE; i++) {
        TestItemFlags(i, 0, 1);
        TestItemFlags(i, IFL_NOTFREE, 1);
        TestItemFlags(i, IFL_NOTSINGLE, 1);
        TestItemFlags(i, IFL_NOTTEAM, 0);
        TestItemFlags(i, IFL_NOTBOT, 0);
    }
    TestItemFlags(GT_SINGLE_PLAYER, IFL_NOTFREE | IFL_NOTTEAM, 1);
    TestItemFlags(GT_SINGLE_PLAYER, IFL_NOTSINGLE, 0);
    TestItemFlags(GT_SINGLE_PLAYER, IFL_NOTBOT, 0);
    puts("ok");
    return 0;
}
