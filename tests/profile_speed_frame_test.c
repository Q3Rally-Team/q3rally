/* Exercise the real UpdateClientFrame body with multiple user commands per tick. */
#include "../engine/code/game/g_local.h"
#include "../engine/code/game/bg_achievements.h"
#include <assert.h>
#include <stdio.h>

#define PROFILE_DISPLAY_L_PER_100KM 9.0f
static struct {
    qboolean loaded, dirty;
    char name[PROFILE_MAX_NAME];
    profile_stats_t stats;
} s_profileState;
level_locals_t level;
float CP_M_2_QU = 35.66f;
static qboolean G_Profile_ShouldTrackClient(const gclient_t *client) { return client != NULL; }
static void G_Profile_UpdateVehicleUsage(gentity_t *ent, int ms) { (void)ent; (void)ms; }
static void G_Profile_CheckAchievementProgress(gclient_t *client, int category) { (void)client; (void)category; }
static void G_Profile_MaybeAutosave(void) {}
void QDECL Com_Error(int level, const char *fmt, ...) { (void)level; (void)fmt; assert(0); }
void QDECL Com_Printf(const char *fmt, ...) { (void)fmt; }

#include "profile_update_under_test.h"

int main(void) {
    static gclient_t client;
    static gentity_t ent;
    int t, command;
    s_profileState.loaded = qtrue;
    s_profileState.name[0] = 'x';
    ent.client = &client;
    ent.health = 100;
    client.pers.connected = CON_CONNECTED;
    client.sess.sessionTeam = TEAM_FREE;
    client.ps.pm_type = PM_NORMAL;
    client.ps.velocity[0] = 248.0f / 3.6f * CP_M_2_QU;
    for (t = 100; t <= 350; t += 50) {
        level.time = t;
        for (command = 0; command < 4; command++) {
            G_Profile_UpdateClientFrame(&ent);
            if (t < 350) assert(s_profileState.stats.topSpeedKph == 0.0);
        }
    }
    assert(s_profileState.stats.topSpeedKph > 247.9);
    assert(s_profileState.stats.topSpeedKph < 248.1);
    assert(s_profileState.dirty);
    /* A collision peak still cannot replace the confirmed speed. */
    level.time = 400;
    client.ps.velocity[0] = 399.0f / 3.6f * CP_M_2_QU;
    G_Profile_UpdateClientFrame(&ent);
    client.ps.velocity[0] = 248.0f / 3.6f * CP_M_2_QU;
    for (t = 450; t <= 650; t += 50) {
        level.time = t;
        for (command = 0; command < 4; command++) G_Profile_UpdateClientFrame(&ent);
    }
    assert(s_profileState.stats.topSpeedKph < 248.1);
    puts("ok: repeated commands preserve the confirmation window");
    return 0;
}
