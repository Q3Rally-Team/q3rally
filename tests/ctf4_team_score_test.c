#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Provide minimal prototypes required before including game headers
char *va(char *format, ...);
int Com_sprintf(char *dest, int size, const char *fmt, ...);
void G_Error(const char *fmt, ...);
void trap_SendServerCommand( int clientNum, const char *text );
void CenterPrint_All( const char *s );
void trap_Printf( const char *fmt, ... );

#include "../engine/code/game/g_team.c"

level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
gclient_t levelClients[MAX_CLIENTS];
vmCvar_t g_gametype;
vmCvar_t g_redteam;
vmCvar_t g_blueteam;
vmCvar_t g_maxclients;
vmCvar_t g_dominationScoreInterval;
vmCvar_t g_dominationCaptureDelay;
vec3_t vec3_origin = { 0.0f, 0.0f, 0.0f };
gitem_t bg_itemlist[1];

static gentity_t tempEntity;
static int lastServerCommandClient = 0;
static char lastServerCommand[1024];
static char lastCenterPrint[1024];

void reset_environment(void) {
    memset(&level, 0, sizeof(level));
    memset(g_entities, 0, sizeof(g_entities));
    memset(&tempEntity, 0, sizeof(tempEntity));
    memset(lastServerCommand, 0, sizeof(lastServerCommand));
    memset(lastCenterPrint, 0, sizeof(lastCenterPrint));
    memset(&teamgame, 0, sizeof(teamgame));
    level.gentities = g_entities;
    level.maxclients = MAX_CLIENTS;
    level.clients = levelClients;
    g_maxclients.integer = MAX_CLIENTS;
}

gentity_t *G_TempEntity(vec3_t origin, int event) {
    (void)origin;
    memset(&tempEntity, 0, sizeof(tempEntity));
    tempEntity.s.event = event;
    return &tempEntity;
}

void trap_SendServerCommand( int clientNum, const char *text ) {
    lastServerCommandClient = clientNum;
    strncpy(lastServerCommand, text, sizeof(lastServerCommand) - 1);
    lastServerCommand[sizeof(lastServerCommand) - 1] = '\0';
}

void CenterPrint_All( const char *s ) {
    strncpy(lastCenterPrint, s, sizeof(lastCenterPrint) - 1);
    lastCenterPrint[sizeof(lastCenterPrint) - 1] = '\0';
}

void G_Error(const char *fmt, ...) {
    (void)fmt;
    assert(!"G_Error called");
}

char *va(char *format, ... ) {
    static char string[2][32000];
    static int index;
    char *buf;
    va_list args;
    buf = string[index & 1];
    index++;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    return buf;
}

int Com_sprintf(char *dest, int size, const char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(dest, size, fmt, args);
    va_end(args);
    if (written < 0) {
        dest[0] = '\0';
        return 0;
    }
    if (written >= size) {
        dest[size - 1] = '\0';
        return size - 1;
    }
    return written;
}

int Q_vsnprintf( char *str, size_t size, const char *format, va_list ap );

int Q_vsnprintf( char *str, size_t size, const char *format, va_list ap ) {
    (void)size;
    return vsprintf(str, format, ap);
}

int Q_stricmpn( const char *s1, const char *s2, int n );
int Q_stricmpn( const char *s1, const char *s2, int n ) {
    return strncasecmp(s1, s2, (size_t)n);
}

void Q_strncpyz( char *dest, const char *src, int destsize );
void Q_strncpyz( char *dest, const char *src, int destsize ) {
    if (destsize <= 0) {
        return;
    }
    strncpy(dest, src, (size_t)destsize - 1);
    dest[destsize - 1] = '\0';
}

int Q_stricmp( const char *s1, const char *s2 );
int Q_stricmp( const char *s1, const char *s2 ) {
    return strcasecmp(s1, s2);
}

void trap_SetConfigstring( int num, const char *string ) {
    (void)num;
    (void)string;
}

int trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount ) {
    (void)mins;
    (void)maxs;
    (void)entityList;
    (void)maxcount;
    return 0;
}

gitem_t *BG_FindItemForPowerup( powerup_t pw ) {
    (void)pw;
    return &bg_itemlist[0];
}

void G_FreeEntity( gentity_t *ent ) {
    if (ent) {
        ent->inuse = qfalse;
    }
}

void AddScore( gentity_t *ent, vec3_t origin, int score ) {
    (void)ent;
    (void)origin;
    (void)score;
}

gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match) {
    (void)from;
    (void)fieldofs;
    (void)match;
    return NULL;
}

qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 ) {
    (void)p1;
    (void)p2;
    return qfalse;
}

void RespawnItem( gentity_t *ent ) {
    (void)ent;
}

void G_Printf( const char *fmt, ... ) {
    (void)fmt;
}

void trap_Printf( const char *fmt, ... ) {
    (void)fmt;
}

void G_SpawnItem (gentity_t *ent, gitem_t *item) {
    (void)ent;
    (void)item;
}

void G_Profile_RecordFlagCapture(gclient_t *client) {
    (void)client;
}

void G_Profile_RecordFlagAssist(gclient_t *client) {
    (void)client;
}

void CalculateRanks( void ) {}

qboolean SpotWouldTelefrag( gentity_t *spot ) {
    (void)spot;
    return qfalse;
}

gentity_t *SelectSpawnPoint( vec3_t avoidPoint, vec3_t origin, vec3_t angles, qboolean isbot ) {
    (void)avoidPoint;
    (void)origin;
    (void)angles;
    (void)isbot;
    return NULL;
}

void test_green_scoring_uses_new_event(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    level.teamScores[TEAM_RED] = 1;
    level.teamScores[TEAM_BLUE] = 0;
    level.teamScores[TEAM_GREEN] = 3;
    level.teamScores[TEAM_YELLOW] = 0;

    AddTeamScore(vec3_origin, TEAM_GREEN, 1);

    assert(level.teamScores[TEAM_GREEN] == 4);
    assert(tempEntity.s.eventParm == GTS_GREENTEAM_SCORED);
    assert(strstr(lastCenterPrint, "GREEN") != NULL);
    assert(strstr(lastServerCommand, "scores!") != NULL);
}

void test_green_lead_change_emits_text(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    level.teamScores[TEAM_RED] = 2;
    level.teamScores[TEAM_BLUE] = 3;
    level.teamScores[TEAM_GREEN] = 3;
    level.teamScores[TEAM_YELLOW] = 1;

    AddTeamScore(vec3_origin, TEAM_GREEN, 1);

    assert(level.teamScores[TEAM_GREEN] == 4);
    assert(tempEntity.s.eventParm == GTS_GREENTEAM_TOOK_LEAD);
    assert(strstr(lastCenterPrint, "takes the lead") != NULL);
}

void test_yellow_tie_triggers_tied_sound(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    level.teamScores[TEAM_RED] = 5;
    level.teamScores[TEAM_BLUE] = 3;
    level.teamScores[TEAM_GREEN] = 2;
    level.teamScores[TEAM_YELLOW] = 4;

    AddTeamScore(vec3_origin, TEAM_YELLOW, 1);

    assert(level.teamScores[TEAM_YELLOW] == 5);
    assert(tempEntity.s.eventParm == GTS_TEAMS_ARE_TIED);
    assert(strstr(lastServerCommand, "tie for the lead") != NULL);
}

void test_yellow_lead_event_and_audio(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    level.teamScores[TEAM_RED] = 4;
    level.teamScores[TEAM_BLUE] = 4;
    level.teamScores[TEAM_GREEN] = 2;
    level.teamScores[TEAM_YELLOW] = 4;

    AddTeamScore(vec3_origin, TEAM_YELLOW, 1);

    assert(level.teamScores[TEAM_YELLOW] == 5);
    assert(tempEntity.s.eventParm == GTS_YELLOWTEAM_TOOK_LEAD);
    assert(strstr(lastCenterPrint, "YELLOW") != NULL);
}

void test_green_flag_drop_sets_status(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    Team_InitGame();

    gentity_t dropped;
    memset(&dropped, 0, sizeof(dropped));
    gitem_t item;
    memset(&item, 0, sizeof(item));
    item.giTag = PW_GREENFLAG;
    dropped.item = &item;

    Team_CheckDroppedItem(&dropped);

    assert(teamgame.flagStatus[TEAM_GREEN] == FLAG_DROPPED);
}

void test_yellow_flag_drop_sets_status(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    Team_InitGame();

    gentity_t dropped;
    memset(&dropped, 0, sizeof(dropped));
    gitem_t item;
    memset(&item, 0, sizeof(item));
    item.giTag = PW_YELLOWFLAG;
    dropped.item = &item;

    Team_CheckDroppedItem(&dropped);

    assert(teamgame.flagStatus[TEAM_YELLOW] == FLAG_DROPPED);
}

void test_green_return_event_emits_new_id(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;

    gentity_t base;
    memset(&base, 0, sizeof(base));

    Team_ReturnFlagSound(&base, TEAM_GREEN);

    assert(tempEntity.s.eventParm == GTS_GREEN_RETURN);
}

void test_yellow_capture_event_emits_new_id(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;

    gentity_t base;
    memset(&base, 0, sizeof(base));

    Team_CaptureFlagSound(&base, TEAM_YELLOW);

    assert(tempEntity.s.eventParm == GTS_YELLOW_CAPTURE);
}

void test_green_taken_event_emits_new_id(void) {
    reset_environment();
    g_gametype.integer = GT_CTF4;
    Team_InitGame();

    gentity_t base;
    memset(&base, 0, sizeof(base));

    Team_SetFlagStatus(TEAM_GREEN, FLAG_ATBASE);
    teamgame.flagTakenTime[TEAM_GREEN] = 0;
    level.time = 0;

    Team_TakeFlagSound(&base, TEAM_GREEN);

    assert(tempEntity.s.eventParm == GTS_GREEN_TAKEN);
}

int main(void) {
    test_green_scoring_uses_new_event();
    test_green_lead_change_emits_text();
    test_yellow_tie_triggers_tied_sound();
    test_yellow_lead_event_and_audio();
    test_green_flag_drop_sets_status();
    test_yellow_flag_drop_sets_status();
    test_green_return_event_emits_new_id();
    test_yellow_capture_event_emits_new_id();
    test_green_taken_event_emits_new_id();
    printf("ok\n");
    return 0;
}
