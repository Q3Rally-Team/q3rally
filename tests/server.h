#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef uint8_t byte;
typedef enum { qfalse = 0, qtrue = 1 } qboolean;

typedef int fileHandle_t;

typedef struct {
        int dummy;
} gentity_t;

typedef struct {
        int dummy;
} playerState_t;

typedef struct {
        int dummy;
} entityState_t;

typedef struct {
        int dummy;
} car_t;

#define MAX_CLIENTS 64
#define TEAM_NUM_TEAMS 4
#define MAX_QPATH 64
#define MAX_NAME_LENGTH 32
#define MAX_INFO_STRING 1024
#define MAX_OSPATH 256
#define MAX_STRING_CHARS 1024

#define TEAM_FREE 0
#define TEAM_RED 1
#define TEAM_BLUE 2
#define TEAM_SPECTATOR 3

#define LADDER_MAX_MATCH_ID             64
#define LADDER_MAX_MODE                 32
#define LADDER_MAX_TIME_STRING          32
#define LADDER_MAX_SERVER_NAME          64
#define LADDER_MAX_SERVER_HOST          96
#define LADDER_MAX_SERVER_BUILD         32
#define LADDER_MAX_PLAYER_ID            96
#define LADDER_MAX_GUID                 64
#define LADDER_MAX_MODEL                MAX_QPATH
#define LADDER_MAX_VEHICLE              MAX_QPATH
#define LADDER_MAX_LAP_TIMES            32

#ifndef RACE_MAX_RECORDED_LAPS
#define RACE_MAX_RECORDED_LAPS          LADDER_MAX_LAP_TIMES
#endif

typedef struct ladderPlayerPayload_s {
        int                     clientNum;
        char            playerId[LADDER_MAX_PLAYER_ID];
        char            guid[LADDER_MAX_GUID];
        char            name[MAX_NAME_LENGTH];
        char            cleanName[MAX_NAME_LENGTH];
        char            model[LADDER_MAX_MODEL];
        char            vehicle[LADDER_MAX_VEHICLE];
        int                     team;
        qboolean        isBot;
        int                     score;
        int                     ping;
        int                     time;
        int                     scoreFlags;
        int                     powerUps;
        int                     accuracy;
        int                     impressiveCount;
        int                     impressiveTelefragCount;
        int                     excellentCount;
        int                     gauntletCount;
        int                     defendCount;
        int                     assistCount;
        qboolean        perfect;
        int                     captures;
        int                     damageDealt;
        int                     damageTaken;
        int                     position;
        int                     bestLapMs;
        int                     totalRaceMs;
        int                     lapCount;
        int                     lapTimes[LADDER_MAX_LAP_TIMES];
        int                     kills;
        int                     deaths;
        int                     zoneHoldMs;
        int                     zoneActiveSigil;
        int                     survivalMs;
        int                     eliminationRound;
        int                     eliminationPlayersRemaining;
        float           eliminationMetric;
        int                     finishRaceTime;
        float           kdRatio;
} ladderPlayerPayload_t;

typedef struct ladderMatchPayload_s {
        qboolean        valid;
        char            matchId[LADDER_MAX_MATCH_ID];
        char            mode[LADDER_MAX_MODE];
        int                     gametype;
        char            mapName[MAX_QPATH];
        char            startTimeIso[LADDER_MAX_TIME_STRING];
        char            endTimeIso[LADDER_MAX_TIME_STRING];
        char            durationIso[LADDER_MAX_TIME_STRING];
        int                     startEpoch;
        int                     endEpoch;
        int                     durationSeconds;
        char            serverName[LADDER_MAX_SERVER_NAME];
        char            serverHost[LADDER_MAX_SERVER_HOST];
        char            serverBuild[LADDER_MAX_SERVER_BUILD];
        int                     levelStartTime;
        int                     levelEndTime;
        int                     raceStartTime;
        int                     raceEndTime;
        int                     finishRaceTime;
        int                     winnerClientNum;
        int                     numberOfLaps;
        qboolean        trackReversed;
        int                     eliminationStartDelay;
        int                     eliminationInterval;
        int                     eliminationWarning;
        int                     teamScores[TEAM_NUM_TEAMS];
        int                     teamTimes[TEAM_NUM_TEAMS];
        int                     playerCount;
        ladderPlayerPayload_t players[MAX_CLIENTS];
} ladderMatchPayload_t;

typedef struct cvar_s {
        int integer;
        char string[128];
} cvar_t, vmCvar_t;

extern cvar_t *sv_ladderUrl;
extern cvar_t *sv_ladderApiKey;
extern cvar_t *sv_ladderEnabled;
extern cvar_t *sv_telemetryMaxBatch;

static inline void *Z_Malloc( int size ) {
        return calloc( 1, (size_t)size );
}

static inline void Z_Free( void *ptr ) {
        free( ptr );
}

static inline void Com_Memcpy( void *dest, const void *src, size_t count ) {
        memcpy( dest, src, count );
}

static inline void Com_Memset( void *dest, int value, size_t count ) {
        memset( dest, value, count );
}

static inline int Com_sprintf( char *dest, int size, const char *fmt, ... ) {
        va_list args;
        int written;
        va_start( args, fmt );
        written = vsnprintf( dest, (size_t)size, fmt, args );
        va_end( args );
        return written;
}

static inline void Com_Printf( const char *fmt, ... ) {
        (void)fmt;
}

static inline void Com_DPrintf( const char *fmt, ... ) {
        (void)fmt;
}

static inline qboolean Sys_Mkdir( const char *path ) {
        (void)path;
        return qtrue;
}

static inline FILE *Sys_FOpen( const char *path, const char *mode ) {
        return fopen( path, mode );
}

static inline char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs ) {
        (void)directory;
        (void)extension;
        (void)filter;
        (void)wantsubs;
        if ( numfiles ) {
                *numfiles = 0;
        }
        return NULL;
}

static inline void Sys_FreeFileList( char **list ) {
        (void)list;
}

static inline void *Sys_LoadFunction( void *module, const char *name ) {
        (void)module;
        (void)name;
        return NULL;
}

static inline void *Sys_LoadDll( const char *name, qboolean useSystemLib ) {
        (void)name;
        (void)useSystemLib;
        return NULL;
}

static inline void Sys_UnloadLibrary( void *lib ) {
        (void)lib;
}

static inline int Sys_Milliseconds( void ) {
        return 0;
}

static inline int Q_isnan( float value ) {
        return isnan( value ) ? 1 : 0;
}

static inline const char *Cvar_VariableString( const char *var_name ) {
        (void)var_name;
        return "";
}

static inline qboolean FS_CreatePath( char *path ) {
        (void)path;
        return qtrue;
}

static inline void Q_strncpyz( char *dest, const char *src, int destsize ) {
        if ( !dest || destsize <= 0 ) {
                return;
        }
        if ( !src ) {
                src = "";
        }
        strncpy( dest, src, (size_t)destsize - 1 );
        dest[destsize - 1] = '\0';
}

static inline int Q_stricmp( const char *s1, const char *s2 ) {
        return strcasecmp( s1 ? s1 : "", s2 ? s2 : "" );
}

#endif /* TEST_SERVER_H */
