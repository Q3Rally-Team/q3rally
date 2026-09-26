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
vmCvar_t g_trackReversed;

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

const botPathRoute_t *G_BotPath_GetRouteByIndex( int routeIndex ) {
    (void)routeIndex;
    return NULL;
}

void trap_SendServerCommand( int clientNum, const char *text ) {
    (void)clientNum;
    (void)text;
}

typedef struct {
    const char *path;
    const char *content;
} mock_file_t;

static mock_file_t s_files[32];
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
    g_trackLength.integer = 2;
    g_trackReversed.integer = 1;

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

int main(void) {
    test_shared_route_ignores_legacy_vehicle_metadata();
    test_header_keys_require_delimiter();
    test_top5_retention_per_track_variant();
    test_legacy_ghosts_can_supply_the_shared_track_route();
    test_best_usable_ghost_supplies_shared_route();
    test_stable_navigation_at_overlap();
    puts("ok");
    return 0;
}
