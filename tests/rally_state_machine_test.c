#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../engine/code/game/g_rally_racetools.c"

// Globals required by included code
level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
gclient_t levelClients[MAX_CLIENTS];
vmCvar_t g_gametype;
vmCvar_t g_forceEngineStart;

// Additional globals referenced elsewhere
vmCvar_t g_eliminationStartDelay;
vmCvar_t g_eliminationInterval;
vmCvar_t g_eliminationWarning;

static char lastCvarName[64];
static char lastCvarValue[64];
static char commandLog[16][128];
static int commandLogCount;
static int lastSoundIndex;
static qboolean rallyMode = qtrue;

static const char *centerPrintLog[16];
static int centerPrintCount;

static char vaBuffer[4][256];
static int vaIndex;

char *va(char *format, ... ) {
    va_list args;
    vaIndex = (vaIndex + 1) % 4;
    va_start(args, format);
    vsnprintf(vaBuffer[vaIndex], sizeof(vaBuffer[vaIndex]), format, args);
    va_end(args);
    return vaBuffer[vaIndex];
}

int Com_sprintf(char *dest, int size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(dest, size, fmt, args);
    va_end(args);
    return written;
}

int TeamCount(int ignoreClientNum, team_t team) {
    (void)ignoreClientNum;
    (void)team;
    return 0;
}

qboolean isRallyRace(void) {
    return rallyMode;
}

void trap_Cvar_Set( const char *var_name, const char *value ) {
    strncpy(lastCvarName, var_name, sizeof(lastCvarName) - 1);
    lastCvarName[sizeof(lastCvarName) - 1] = '\0';
    strncpy(lastCvarValue, value, sizeof(lastCvarValue) - 1);
    lastCvarValue[sizeof(lastCvarValue) - 1] = '\0';
}

void trap_SendServerCommand( int clientNum, const char *text ) {
    if (commandLogCount < 16) {
        snprintf(commandLog[commandLogCount], sizeof(commandLog[commandLogCount]), "%d:%s", clientNum, text);
        commandLogCount++;
    }
}

void G_LogPrintf(const char *fmt, ...) {
    (void)fmt;
}

void CenterPrint_All(const char *message) {
    if (centerPrintCount < 16) {
        centerPrintLog[centerPrintCount++] = message;
    }
}

void G_FreeEntity(gentity_t *ent) {
    if (ent) {
        ent->inuse = qfalse;
    }
}

gentity_t *SelectSpawnPoint(vec3_t avoidPoint, vec3_t origin, vec3_t angles, qboolean isbot) {
    (void)avoidPoint;
    (void)origin;
    (void)angles;
    (void)isbot;
    return NULL;
}

qboolean SpotWouldTelefrag(gentity_t *spot) {
    (void)spot;
    return qfalse;
}

void G_Printf(const char *fmt, ...) {
    (void)fmt;
}

void Rally_Sound( gentity_t *ent, int event, int channel, int soundIndex ) {
    (void)ent;
    (void)event;
    (void)channel;
    lastSoundIndex = soundIndex;
}

int G_SoundIndex( char *name ) {
    (void)name;
    static int next = 1;
    return next++;
}

static gentity_t checkpoint;

gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match) {
    (void)fieldofs;
    if (match && strcmp(match, "rally_checkpoint") == 0) {
        return &checkpoint;
    }
    if (match && strcmp(match, "player") == 0) {
        int start = 0;
        if (from) {
            start = (int)(from - g_entities) + 1;
        }
        for (int i = start; i < MAX_CLIENTS; ++i) {
            if (g_entities[i].inuse && g_entities[i].classname && strcmp(g_entities[i].classname, "player") == 0) {
                return &g_entities[i];
            }
        }
    }
    return NULL;
}

void reset_environment(void) {
    memset(&level, 0, sizeof(level));
    memset(g_entities, 0, sizeof(g_entities));
    memset(levelClients, 0, sizeof(levelClients));
    memset(lastCvarName, 0, sizeof(lastCvarName));
    memset(lastCvarValue, 0, sizeof(lastCvarValue));
    commandLogCount = 0;
    centerPrintCount = 0;
    lastSoundIndex = 0;
    rallyMode = qtrue;
    checkpoint.number = 1;
    level.clients = levelClients;
    level.maxclients = MAX_CLIENTS;
    level.gentities = g_entities;
}

void test_elimination_configuration_initial_setup(void) {
    reset_environment();
    g_gametype.integer = GT_ELIMINATION;
    g_forceEngineStart.integer = 10;

    G_RallyConfigureElimination(4);

    assert(level.eliminationSetupComplete == qtrue);
    assert(level.eliminationInitialPlayers == 4);
    assert(level.eliminationPlayersRemaining == 4);
    assert(level.eliminationRound == 0);
    assert(strcmp(lastCvarName, "laplimit") == 0);
    assert(strcmp(lastCvarValue, "3") == 0);
}

void test_elimination_configuration_minimum_laps(void) {
    reset_environment();
    g_gametype.integer = GT_ELIMINATION;

    G_RallyConfigureElimination(0);
    assert(level.numberOfLaps == 1);
    assert(strcmp(lastCvarValue, "1") == 0);
}

void test_race_countdown_sequence(void) {
    reset_environment();
    g_gametype.integer = GT_RACING;
    g_forceEngineStart.integer = 30;

    gentity_t starter = {0};
    starter.think = RallyStarter_Think;
    starter.number = 0;
    starter.classname = "rally_starter";

    g_entities[0].inuse = qtrue;
    g_entities[0].client = &levelClients[0];
    g_entities[0].classname = "player";
    g_entities[0].ready = qtrue;
    g_entities[0].client->sess.sessionTeam = TEAM_RED;

    level.clients = levelClients;
    level.maxclients = MAX_CLIENTS;
    level.gentities = g_entities;
    level.time = 8000;
    level.startTime = 0;

    RallyStarter_Think(&starter);
    assert(starter.number == 3);
    assert(starter.pain_debounce_time == level.time);
    assert(commandLogCount == 0);

    int base_time = starter.pain_debounce_time;

    level.time = base_time + 2001;
    RallyStarter_Think(&starter);
    assert(commandLogCount >= 1);
    assert(strstr(commandLog[0], "rc \"3\" 3") != NULL);

    level.time = base_time + 3005;
    RallyStarter_Think(&starter);
    assert(strstr(commandLog[1], "rc \"2\" 2") != NULL);

    level.time = base_time + 4005;
    RallyStarter_Think(&starter);
    assert(strstr(commandLog[2], "rc \"1\" 1") != NULL);

    level.time = base_time + 5005;
    RallyStarter_Think(&starter);
    assert(level.startRaceTime == level.time);
    assert(starter.think == RallyRace_Think);
    assert(strstr(commandLog[3], "raceTime") != NULL);
    assert(strstr(commandLog[4], "rc \"GO!\" 0") != NULL);
    assert(lastSoundIndex != 0);
}

int main(void) {
    test_elimination_configuration_initial_setup();
    test_elimination_configuration_minimum_laps();
    test_race_countdown_sequence();
    printf("ok\n");
    return 0;
}
vec3_t vec3_origin = {0, 0, 0};

qboolean isRallyNonDMRace(void) {
    return rallyMode;
}

void CalculateRanks(void) {}

gentity_t *G_Spawn(void) {
    static gentity_t dummy;
    memset(&dummy, 0, sizeof(dummy));
    return &dummy;
}
