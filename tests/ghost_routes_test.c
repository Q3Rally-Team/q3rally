#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "../engine/code/game/g_ghost.c"

level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
vmCvar_t g_trackLength;
vmCvar_t g_gametype;   /* base ghost transfer: GT_GHOST, otherwise on request (racing line) */
vmCvar_t g_trackReversed;
float CP_CURRENT_GRAVITY = 800.0f;

/* The harness models checkpoint brushes as the boxes declared below. */
qboolean trap_EntityContact(const vec3_t mins, const vec3_t maxs, const gentity_t *ent) {
    int axis;
    for (axis = 0; axis < 3; axis++) {
        if (mins[axis] > ent->r.absmax[axis] || maxs[axis] < ent->r.absmin[axis]) return qfalse;
    }
    return qtrue;
}

vec_t VectorNormalize( vec3_t vector ) {
    float length = sqrtf( VectorLengthSquared( vector ) );

    if ( length > 0.0f ) {
        VectorScale( vector, 1.0f / length, vector );
    }
    return length;
}

void vectoangles( const vec3_t vector, vec3_t angles ) {
    angles[PITCH] = atan2f( -vector[2], sqrtf( vector[0] * vector[0] + vector[1] * vector[1] ) ) * ( 180.0f / (float)M_PI );
    angles[YAW] = atan2f( vector[1], vector[0] ) * ( 180.0f / (float)M_PI );
    angles[ROLL] = 0.0f;
}

float AngleSubtract( float a1, float a2 ) {
    float delta = a1 - a2;

    while ( delta > 180.0f ) {
        delta -= 360.0f;
    }
    while ( delta < -180.0f ) {
        delta += 360.0f;
    }
    return delta;
}

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

int Q_stricmp( const char *s1, const char *s2 ) {
    return strcasecmp(s1 ? s1 : "", s2 ? s2 : "");
}

int Q_stricmpn( const char *s1, const char *s2, int n ) {
    return strncasecmp(s1 ? s1 : "", s2 ? s2 : "", (size_t)n);
}

void Q_strncpyz( char *dest, const char *src, int destsize ) {
    if (!dest || destsize <= 0) {
        return;
    }
    if (!src) {
        src = "";
    }
    strncpy(dest, src, (size_t)destsize - 1);
    dest[destsize - 1] = '\0';
}

int Com_sprintf(char *dest, int size, const char *fmt, ...) {
    va_list args;
    int written;
    va_start(args, fmt);
    written = vsnprintf(dest, (size_t)size, fmt, args);
    va_end(args);
    return written;
}

void COM_StripExtension( const char *in, char *out, int destsize ) {
    int len;
    Q_strncpyz(out, in, destsize);
    len = (int)strlen(out);
    while (len > 0) {
        if (out[len] == '.') {
            out[len] = '\0';
            break;
        }
        --len;
    }
}

void G_Printf(const char *fmt, ...) {
    (void)fmt;
}

static botPathRoute_t s_testMapPath;
const botPathRoute_t *G_BotPath_GetRouteByIndex( int routeIndex ) {
    return routeIndex == 0 && s_testMapPath.valid ? &s_testMapPath : NULL;
}
int G_BotPath_GetRouteCount( void ) { return s_testMapPath.valid ? 1 : 0; }
int trap_Cvar_VariableIntegerValue( const char *name ) { (void)name; return -123456; }

static char s_lastServerCommand[256];
static int s_lastServerCommandClient = -1;

void trap_SendServerCommand( int clientNum, const char *text ) {
    s_lastServerCommandClient = clientNum;
    snprintf( s_lastServerCommand, sizeof( s_lastServerCommand ), "%s", text );
}

qboolean BG_GametypeIsRace( int gametype ) {
    return gametype == GT_RACING || gametype == GT_GHOST;
}

typedef struct {
    const char *path;
    const char *content;
} mock_file_t;

static mock_file_t s_files[128];
static int s_fileCount;

static void add_file(const char *path, const char *content) {
    s_files[s_fileCount].path = path;
    s_files[s_fileCount].content = content;
    s_fileCount++;
}

static int find_file_index(const char *path) {
    for (int i = 0; i < s_fileCount; ++i) {
        if (strcmp(s_files[i].path, path) == 0) {
            return i;
        }
    }
    return -1;
}

int trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize ) {
    int count = 0;
    int offset = 0;
    size_t extLen = strlen(extension);
    size_t dirLen = strlen(path);

    for (int i = 0; i < s_fileCount; ++i) {
        const char *full = s_files[i].path;
        const char *slash = strchr(full, '/');
        const char *name;
        size_t nameLen;

        if (!slash) {
            continue;
        }
        if ((size_t)(slash - full) != dirLen || strncmp(full, path, dirLen) != 0) {
            continue;
        }

        name = slash + 1;
        nameLen = strlen(name);
        if (nameLen < extLen || strcmp(name + nameLen - extLen, extension) != 0) {
            continue;
        }

        if (offset + (int)nameLen + 1 >= bufsize) {
            break;
        }

        memcpy(listbuf + offset, name, nameLen + 1);
        offset += (int)nameLen + 1;
        count++;
    }

    return count;
}

int trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
    int idx;
    (void)mode;
    idx = find_file_index(qpath);
    if (idx < 0) {
        if (f) *f = 0;
        return -1;
    }
    if (f) *f = idx + 1;
    return (int)strlen(s_files[idx].content);
}

void trap_FS_Read( void *buffer, int len, fileHandle_t f ) {
    int idx = f - 1;
    memcpy(buffer, s_files[idx].content, (size_t)len);
}

void trap_FS_FCloseFile( fileHandle_t f ) {
    (void)f;
}

static const char *build_ghost(const char *map, const char *vehicle, int best, int withVariant, int tl, int rev) {
    static char buf[16][1024];
    static int bi;
    bi = (bi + 1) % 16;
    if (withVariant) {
        snprintf(buf[bi], sizeof(buf[bi]),
            "map %s\nvehicle %s\nbest_time_ms %d\ntrack_length %d\ntrack_reversed %d\nframes\n0 0 0 0\n100 50 0 0\n200 100 0 0\n",
            map, vehicle, best, tl, rev);
    } else {
        snprintf(buf[bi], sizeof(buf[bi]),
            "map %s\nvehicle %s\nbest_time_ms %d\nframes\n0 0 0 0\n100 50 0 0\n200 100 0 0\n",
            map, vehicle, best);
    }
    return buf[bi];
}

static const char *build_ghost_with_vehicle_path(const char *map, int best, int tl, int rev) {
    static char buf[1024];
    snprintf(buf, sizeof(buf),
        "map %s\nvehicle_path ghosts/%s_tl%d_rev%d_fake.ghost\nbest_time_ms %d\ntrack_length %d\ntrack_reversed %d\nframes\n0 0 0 0\n100 50 0 0\n200 100 0 0\n",
        map, map, tl, rev, best, tl, rev);
    return buf;
}

static void reset_fs(void) {
    memset(s_files, 0, sizeof(s_files));
    s_fileCount = 0;
}

static void test_shared_route_ignores_legacy_vehicle_metadata(void) {
    const ghostBotRoute_t *route = NULL;
    const ghostBotRoute_t *sameRoute = NULL;
    const ghostRecord_t *record;

    reset_fs();
    g_trackLength.integer = 1;
    g_trackReversed.integer = 0;

    add_file("ghosts/mymap_tl1_rev0_a.ghost", build_ghost("mymap", "truck", 900, 1, 1, 0));
    add_file("ghosts/mymap_tl1_rev0_b.ghost", build_ghost("mymap", "sport", 1100, 1, 1, 0));

    G_Ghost_InitForMap("mymap");
    assert(G_Ghost_Test_GetLevelGhostCount() == 2);

    record = G_Ghost_Test_GetLevelGhost(0);
    assert(record != NULL);
    assert(strcmp(record->vehicleClass, "truck") == 0);
    record = G_Ghost_Test_GetLevelGhost(1);
    assert(record != NULL);
    assert(strcmp(record->vehicleClass, "sport") == 0);

    G_Ghost_BuildBotRoutes();
    assert(G_Ghost_GetBotRoute(&route) == qtrue);
    assert(route->bestTimeMs == 900);
    assert(G_Ghost_GetBotRoute(&sameRoute) == qtrue);
    assert(sameRoute == route);
    assert(route->bestTimeMs == 900);
}

static void test_header_keys_require_delimiter(void) {
    const ghostRecord_t *record;

    reset_fs();
    g_trackLength.integer = 1;
    g_trackReversed.integer = 0;

    add_file("ghosts/mymap_tl1_rev0_a.ghost", build_ghost_with_vehicle_path("mymap", 900, 1, 0));

    G_Ghost_InitForMap("mymap");

    assert(G_Ghost_Test_GetLevelGhostCount() == 1);
    record = G_Ghost_Test_GetLevelGhost(0);
    assert(record != NULL);
    assert(record->vehicleClass[0] == '\0');
}

static void test_top5_retention_per_track_variant(void) {
    int recordCount;
    int worstBestTime = 0;

    reset_fs();
    g_trackLength.integer = 1;
    g_trackReversed.integer = 0;

    for (int i = 0; i < 7; ++i) {
        char path[128];
        snprintf(path, sizeof(path), "ghosts/mymap_tl1_rev0_%d.ghost", i);
        add_file(strdup(path), build_ghost("mymap", i % 2 ? "sport" : "truck", 1000 + i * 100, 1, 1, 0));
    }

    G_Ghost_InitForMap("mymap");

    recordCount = G_Ghost_Test_GetLevelGhostCount();
    for (int i = 0; i < recordCount; ++i) {
        const ghostRecord_t *record = G_Ghost_Test_GetLevelGhost(i);
        if (record && record->bestTimeMs > worstBestTime) {
            worstBestTime = record->bestTimeMs;
        }
    }

    assert(recordCount == 5);
    assert(worstBestTime == 1400);
}

static void test_legacy_ghosts_can_supply_the_shared_track_route(void) {
    const ghostBotRoute_t *route = NULL;

    reset_fs();
    g_trackLength.integer = 0;
    g_trackReversed.integer = 0;

    add_file("ghosts/mymap_old_a.ghost", build_ghost("mymap", "sport", 1500, 0, 0, 0));
    add_file("ghosts/mymap_old_b.ghost", build_ghost("mymap", "sport", 1600, 0, 0, 0));

    G_Ghost_InitForMap("mymap");

    assert(G_Ghost_Test_GetLevelGhostCount() == 2);
    for (int i = 0; i < 2; ++i) {
        const ghostRecord_t *record = G_Ghost_Test_GetLevelGhost(i);
        assert(record->ambiguousLegacy == qfalse);
    }

    G_Ghost_BuildBotRoutes();
    assert(G_Ghost_GetBotRoute(&route) == qtrue);
    assert(route->bestTimeMs == 1500);

    g_trackLength.integer = 2;
    g_trackReversed.integer = 1;
    G_Ghost_InitForMap("mymap");
    G_Ghost_BuildBotRoutes();
    assert(G_Ghost_GetBotRoute(&route) == qfalse);
}

static void test_invalid_recordings_and_compatibility(void) {
    const ghostBotRoute_t *route;
    ghostRecord_t record;
    char header[256];
    const char *badFrames[] = {
        "0 0 0 0\n100 50 0\n200 100 0 0\n",  /* missing coordinate */
        "0 0 0 0\n100 50 0 0\n50 100 0 0\n", /* backwards clock */
        "0 0 0 0\n100 5000 0 0\n200 5100 0 0\n", /* teleport */
        "0 0 0 0\n0 50 0 0\n200 100 0 0\n", /* moving at equal time */
        "0 0 0 0\n100 50 nan 0\n200 100 0 0\n",
        "0 0 0 0\n99999999999999999 50 0 0\n"
    };
    int i;
    g_trackLength.integer = 1;
    g_trackReversed.integer = 0;
    for (i = 0; i < (int)(sizeof(badFrames)/sizeof(badFrames[0])); i++) {
        char bad[1024];
        reset_fs();
        snprintf(bad, sizeof(bad), "map mymap\ntrack_length 1\ntrack_reversed 0\nbest_time_ms 10\nframes\n%s", badFrames[i]);
        add_file("ghosts/mymap_tl1_rev0_bad.ghost", bad);
        add_file("ghosts/mymap_tl1_rev0_good.ghost", build_ghost("mymap", "evo", 500, 1, 1, 0));
        G_Ghost_InitForMap("mymap");
        G_Ghost_BuildBotRoutes();
        assert(G_Ghost_GetBotRoute(&route));
        assert(strstr(route->path, "good.ghost"));
    }
    snprintf(header, sizeof(header), "physics_version %d\nmap_checksum -123456\n", BG_PHYSICS_VERSION);
    assert(G_Ghost_ParseHeader(header, NULL, -1, -1, qtrue, &record));
    snprintf(header, sizeof(header), "physics_version %d\n", BG_PHYSICS_VERSION + 1);
    assert(!G_Ghost_ParseHeader(header, NULL, -1, -1, qtrue, &record));
    strcpy(header, "map_checksum 123456\n");
    assert(!G_Ghost_ParseHeader(header, NULL, -1, -1, qtrue, &record));
}

static void make_gate(gentity_t *gate, float x) {
    memset(gate, 0, sizeof(*gate));
    gate->inuse = qtrue;
    VectorSet(gate->r.absmin, x - 10, -10, -10);
    VectorSet(gate->r.absmax, x + 10, 10, 10);
}

static void test_checkpoint_order_and_sprint_finish(void) {
    static ghostBotRoute_t route;
    static gentity_t gates[3];
    memset(&route, 0, sizeof(route));
    route.valid = qtrue;
    route.numWaypoints = 3;
    VectorSet(route.waypoints[1].origin, 200, 0, 0);
    VectorSet(route.waypoints[2].origin, 400, 0, 0);
    make_gate(&gates[0], 100); make_gate(&gates[1], 300); make_gate(&gates[2], 400);
    level.numCheckpoints = 2;
    level.checkpoints[0] = &gates[0]; level.checkpoints[1] = &gates[1];
    assert(G_RaceRoute_ValidateGhost(&route));
    {
        botRaceGuidance_t guidance;
        vec3_t missedGate = {330, 0, 0};
        route.numSegments = 2;
        route.segments[0].recommendedSpeed = route.segments[1].recommendedSpeed = 900;
        // Nearest geometry lies beyond gate 1. The open gate keeps us on its leg.
        assert(G_RaceRoute_Guidance(&route, missedGate, 1, qfalse, 1, 500, &guidance));
        assert(guidance.segment == 0 && guidance.target[0] <= 200);
        // Once the game confirms gate 1, the next leg becomes eligible.
        assert(G_RaceRoute_Guidance(&route, missedGate, 0, qfalse, 2, 500, &guidance));
        assert(guidance.segment == 1);
    }
    level.checkpoints[0] = &gates[1]; level.checkpoints[1] = &gates[0];
    assert(!G_RaceRoute_ValidateGhost(&route));
    level.checkpoints[0] = &gates[0]; level.checkpoints[1] = &gates[1];
    level.sprintTrack = qtrue; level.finishEnt = &gates[2];
    assert(G_RaceRoute_ValidateGhost(&route));
    VectorSet(route.waypoints[2].origin, 310, 0, 0);
    assert(!G_RaceRoute_ValidateGhost(&route));
    memset(&level, 0, sizeof(level));
}

static void test_shared_route_priority_and_map_adapter(void) {
    const botRaceRoute_t *route;
    static gentity_t gates[5], sprintFinish;
    const float corners[4][2] = {{1000,0},{1000,1000},{0,1000},{0,0}};
    int i;
    memset(&level, 0, sizeof(level));
    reset_fs();
    for (i = 0; i < 4; i++) {
        make_gate(&gates[i], corners[i][0]);
        gates[i].r.absmin[1] = corners[i][1] - 10;
        gates[i].r.absmax[1] = corners[i][1] + 10;
        VectorSet(gates[i].s.origin2, corners[i][0], corners[i][1], 0);
        gates[i].bpdLoaded = qtrue;
        gates[i].bpdIndex = i;
        level.checkpoints[i] = &gates[i];
    }
    make_gate(&gates[4], 500);
    VectorSet(gates[4].s.origin2, 500, 0, 0);
    level.checkpoints[4] = level.finishEnt = &gates[4];
    level.numCheckpoints = 5;
    G_RaceRoute_Reset();
    assert(G_RaceRoute_Get(&route));
    assert(route->fromBezierPath && route->closed && !strcmp(route->path, "checkpoint_bpd"));
    assert(Distance(route->waypoints[0].origin, route->waypoints[route->numWaypoints-1].origin) < .01f);
    assert(fabs(route->waypoints[0].origin[0] - 500) < 70);
    assert(route->segments[0].nextCheckpoint == 1);
    assert(route->segments[route->numSegments-1].nextCheckpoint == 5);

    /* Sprint imports its separate endpoint entities around the BPD checkpoint. */
    make_gate(&sprintFinish, 1000);
    sprintFinish.r.absmin[1] = 990; sprintFinish.r.absmax[1] = 1010;
    VectorSet(sprintFinish.s.origin2, 1000, 1000, 0);
    level.sprintTrack = qtrue;
    level.startEnt = &gates[4]; level.finishEnt = &sprintFinish;
    level.numCheckpoints = 1;
    G_RaceRoute_Reset();
    assert(G_RaceRoute_Get(&route));
    assert(!route->closed && route->waypoints[route->numWaypoints-1].origin[1] == 1000);
    add_file("ghosts/mymap_tl1_rev0_good.ghost", build_ghost("mymap", "evo", 500, 1, 1, 0));
    G_Ghost_InitForMap("mymap");
    G_Ghost_BuildBotRoutes();
    assert(G_RaceRoute_Get(&route) && route->fromBezierPath);
    /* Recorded ghosts do not replace missing BPD controls for AI routes. */
    gates[0].bpdLoaded = qfalse;
    G_RaceRoute_Reset();
    assert(!G_RaceRoute_Get(&route));
    memset(&level, 0, sizeof(level));
}

static void test_segment_guidance_and_braking(void) {
    static botRaceRoute_t route;
    botRaceGuidance_t result;
    vec3_t origin = {500, 30, 0};
    int i;
    memset(&route, 0, sizeof(route));
    route.valid = qtrue; route.numWaypoints = 5; route.numSegments = 4;
    VectorSet(route.waypoints[0].origin, 0, 0, 0);
    VectorSet(route.waypoints[1].origin, 1000, 0, 0);
    VectorSet(route.waypoints[2].origin, 1000, 1000, 0);
    VectorSet(route.waypoints[3].origin, 0, 1000, 0);
    VectorSet(route.waypoints[4].origin, 0, 0, 0);
    for (i = 0; i < 4; i++) route.segments[i].recommendedSpeed = 1000;
    assert(G_RaceRoute_Guidance(&route, origin, -1, qfalse, 0, 500, &result));
    assert(result.segment == 0 && fabs(result.distance - 30) < .01f);
    assert(result.target[0] > 500 && result.target[0] < 1000 && result.target[1] == 0);
    assert(result.tangent[0] == 1 && result.tangent[1] == 0);
    // Approaching a slow zone must brake before entering it.
    route.segments[1].recommendedSpeed = 150;
    origin[0] = 900; origin[1] = 0;
    assert(G_RaceRoute_Guidance(&route, origin, -1, qfalse, 0, 1000, &result));
    assert(result.targetSpeed < 500);
    // Loop guidance crosses the seam; an open Sprint ends there.
    VectorSet(origin, 0, 50, 0);
    route.closed = qtrue;
    assert(G_RaceRoute_Guidance(&route, origin, 3, qtrue, 0, 500, &result));
    assert(result.target[0] > 0 && result.target[1] == 0);
    route.closed = qfalse;
    assert(G_RaceRoute_Guidance(&route, origin, 3, qfalse, 0, 500, &result));
    assert(VectorLengthSquared(result.target) == 0);
    // Height separates bridges from the line directly underneath.
    VectorSet(route.waypoints[2].origin, 1000, 0, 500);
    VectorSet(route.waypoints[3].origin, 0, 0, 500);
    VectorSet(origin, 500, 0, 500);
    assert(G_RaceRoute_Guidance(&route, origin, -1, qfalse, 0, 500, &result));
    assert(result.segment == 2 && result.distance == 0);
}

static void test_best_usable_ghost_supplies_shared_route(void) {
    static const char unusableGhost[] =
        "map mymap\nvehicle racecar\nbest_time_ms 800\n"
        "track_length 1\ntrack_reversed 0\nframes\n";
    const ghostBotRoute_t *route = NULL;

    reset_fs();
    g_trackLength.integer = 1;
    g_trackReversed.integer = 0;

    add_file("ghosts/mymap_tl1_rev0_fast.ghost", unusableGhost);
    add_file("ghosts/mymap_tl1_rev0_valid.ghost", build_ghost("mymap", "truck", 1000, 1, 1, 0));

    G_Ghost_InitForMap("mymap");
    G_Ghost_BuildBotRoutes();

    assert(G_Ghost_GetBotRoute(&route) == qtrue);
    assert(route->bestTimeMs == 1000);
    assert(strstr(route->path, "valid.ghost") != NULL);
}

static void test_clean_ghost_race_lap_is_preferred(void) {
    static const char cleanGhost[] =
        "map mymap\nvehicle evo\nbest_time_ms 1300\ntrack_length 1\ntrack_reversed 0\n"
        "gametype 8\nclean 1\nframes\n0 0 0 0\n100 50 0 0\n200 100 0 0\n";
    const ghostBotRoute_t *route = NULL;
    const ghostRecord_t *best;
    int i, cleanKept = 0;

    reset_fs();
    g_trackLength.integer = 1;
    g_trackReversed.integer = 0;

    add_file("ghosts/mymap_tl1_rev0_clean.ghost", cleanGhost);
    for (i = 0; i < 5; i++) {
        char name[64];
        snprintf(name, sizeof(name), "ghosts/mymap_tl1_rev0_fast%d.ghost", i);
        add_file(name, build_ghost("mymap", "evo", 900 + i * 10, 1, 1, 0));
    }

    G_Ghost_InitForMap("mymap");
    /* The slower clean lap survives the top 5 against faster laps ... */
    assert(G_Ghost_Test_GetLevelGhostCount() == 5);
    for (i = 0; i < G_Ghost_Test_GetLevelGhostCount(); i++) {
        if (G_Ghost_Test_GetLevelGhost(i)->clean) {
            cleanKept++;
        }
    }
    assert(cleanKept == 1);

    /* ... and becomes the best record and the bot route. */
    best = G_Ghost_FindBestRecord();
    assert(best && best->clean && best->bestTimeMs == 1300);
    G_Ghost_BuildBotRoutes();
    assert(G_Ghost_GetBotRoute(&route) == qtrue);
    assert(route->clean && strstr(route->path, "clean.ghost") != NULL);
}

static void test_stable_navigation_at_overlap(void) {
    ghostBotRoute_t route;
    vec3_t origin = {0.0f, 0.0f, 0.0f};
    int index;

    memset(&route, 0, sizeof(route));
    route.valid = qtrue;
    route.numWaypoints = 6;

    VectorSet(route.waypoints[0].origin, 0.0f, 0.0f, 0.0f);
    VectorSet(route.waypoints[1].origin, 10.0f, 0.0f, 0.0f);
    VectorSet(route.waypoints[2].origin, 20.0f, 0.0f, 0.0f);
    VectorSet(route.waypoints[3].origin, 30.0f, 0.0f, 0.0f);
    VectorSet(route.waypoints[4].origin, 5.0f, 0.0f, 0.0f);
    VectorSet(route.waypoints[5].origin, 40.0f, 0.0f, 0.0f);

    index = G_Ghost_SelectClosestWaypoint(&route, origin, 5, 5);
    assert(index >= 2);
}

/* Outside Ghost Race the route only goes to clients that ask for it. */
static void test_route_on_request_outside_ghost_race(void) {
    gentity_t *ent = &g_entities[3];
    static gclient_t client;

    memset( &client, 0, sizeof( client ) );
    client.pers.connected = CON_CONNECTED;
    ent->client = &client;
    s_clientGhostTransferPending[3] = qfalse;
    s_clientGhostTransferSent[3] = qfalse;

    g_gametype.integer = GT_RACING;
    G_Ghost_AnnounceForClient( ent );
    assert( !s_clientGhostTransferPending[3] );

    G_Ghost_RequestRouteForClient( ent );
    assert( s_clientGhostTransferPending[3] );
    s_clientGhostTransferNext[3] = 5;
    G_Ghost_RequestRouteForClient( ent );   /* repeat while sending: ignored */
    assert( s_clientGhostTransferNext[3] == 5 );

    /* already sent (cgame restart): sent again */
    s_clientGhostTransferPending[3] = qfalse;
    s_clientGhostTransferSent[3] = qtrue;
    G_Ghost_RequestRouteForClient( ent );
    assert( s_clientGhostTransferPending[3] && !s_clientGhostTransferSent[3] );
    assert( s_clientGhostTransferNext[3] == 0 );

    /* no race: answered with "none" */
    s_clientGhostTransferPending[3] = qfalse;
    g_gametype.integer = GT_DEATHMATCH;
    s_lastServerCommand[0] = '\0';
    G_Ghost_RequestRouteForClient( ent );
    assert( !s_clientGhostTransferPending[3] );
    assert( s_lastServerCommandClient == 3 && !strcmp( s_lastServerCommand, "ghostmeta none 0" ) );

    /* Ghost Race announces on its own */
    g_gametype.integer = GT_GHOST;
    s_clientGhostTransferSent[3] = qfalse;
    G_Ghost_AnnounceForClient( ent );
    assert( s_clientGhostTransferPending[3] );
    s_clientGhostTransferPending[3] = qfalse;
    ent->client = NULL;
}

int main(void) {
    test_shared_route_ignores_legacy_vehicle_metadata();
    test_header_keys_require_delimiter();
    test_top5_retention_per_track_variant();
    test_legacy_ghosts_can_supply_the_shared_track_route();
    test_best_usable_ghost_supplies_shared_route();
    test_clean_ghost_race_lap_is_preferred();
    test_stable_navigation_at_overlap();
    test_route_on_request_outside_ghost_race();
    test_invalid_recordings_and_compatibility();
    test_checkpoint_order_and_sprint_finish();
    test_shared_route_priority_and_map_adapter();
    test_segment_guidance_and_braking();
    {
        static char names[80][80];
        const ghostBotRoute_t *route;
        int i;
        reset_fs();
        g_trackLength.integer = 1; g_trackReversed.integer = 0;
        for (i = 0; i < 80; i++) {
            snprintf(names[i], sizeof(names[i]), "ghosts/othermap_tl1_rev0_20261025-%06d.ghost", i);
            add_file(names[i], "map othermap\n");
        }
        add_file("ghosts/mymap_tl1_rev0_good.ghost", build_ghost("mymap", "evo", 500, 1, 1, 0));
        G_Ghost_InitForMap("mymap"); G_Ghost_BuildBotRoutes();
        assert(G_Ghost_GetBotRoute(&route));
    }
    puts("ok");
    return 0;
}
