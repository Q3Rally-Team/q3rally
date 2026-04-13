#include "profile_shared.h"
#include "g_local.h"
#include "g_profile.h"
#include "bg_ladder.h"
#include "bg_achievements.h"

#ifdef Q3_VM
#include "bg_lib.h"
#endif

#define PROFILE_AUTOSAVE_INTERVAL 30000
#define PROFILE_DISPLAY_L_PER_100KM 9.0f
#define PROFILE_SCORE_FRAG 2
#define PROFILE_SCORE_DEATH -2
#define PROFILE_SCORE_SUICIDE -5
#define PROFILE_SCORE_FLAG_CAPTURE 5
#define PROFILE_SCORE_FLAG_ASSIST 2
#define PROFILE_SCORE_DOMINATION_CAPTURE 5
#define PROFILE_SCORE_LAP 2
#define PROFILE_SCORE_LEAD_LAP 2
#define PROFILE_SCORE_RACE_WIN 10
#define PROFILE_SCORE_ELIMINATION_WIN 10
#define PROFILE_SCORE_ACHIEVEMENT_TIER 20

#define PROFILE_RANK_ENTRY( name, threshold ) { name, threshold },
static const profile_rank_def_t s_profileRankTable[] = {
    PROFILE_RANK_TABLE( PROFILE_RANK_ENTRY )
};
#undef PROFILE_RANK_ENTRY

#define PROFILE_RANK_COUNT ( sizeof( s_profileRankTable ) / sizeof( s_profileRankTable[0] ) )

#define PROFILE_VEHICLE_JSON_ENTRY_SIZE ( PROFILE_MAX_VEHICLE + 32 )
#define PROFILE_VEHICLE_JSON_BUFFER_SIZE ( PROFILE_MAX_TRACKED_VEHICLES * PROFILE_VEHICLE_JSON_ENTRY_SIZE + 32 )
#define PROFILE_FILE_BUFFER_SIZE 32768

static void QDECL G_PROFILE_LOG( const char *fmt, ... ) {
    (void)fmt;
}

static struct {
    qboolean        loaded;
    qboolean        dirty;
    qboolean        isOnlineSession;  /* qtrue wenn aktiver Client über dedizierten Server verbunden */
    char            name[PROFILE_MAX_NAME];
    profile_stats_t stats;
    profile_info_t  info;
    int             nextAutosaveTime;
    int             achievementsUnlocked[BG_ACHIEVEMENT_CATEGORY_COUNT];
} s_profileState;

static void G_Profile_ParseString( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue );
static void G_Profile_RecomputeAchievementState( void );
static void G_Profile_WriteToDisk( void );
static void G_Profile_MaybeAutosave( void );
static void G_Profile_UpdateRankState( void );

static qboolean G_Profile_ShouldTrackClient( const gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return qfalse;
    }

    if ( !client ) {
        return qfalse;
    }

    /* Offline / listen-server: immer tracken */
    if ( client->pers.localClient ) {
        return qtrue;
    }

    /* Online / dedizierter Server: tracken wenn die UUID des Clients
     * mit der UUID des geladenen Profils übereinstimmt.
     * So werden auf einem dedizierten Server nur die eigenen Stats
     * getracked, auch wenn mehrere Spieler verbunden sind. */
    if ( client->pers.uuid[0] && s_profileState.info.uuid[0] ) {
        if ( Q_stricmp( client->pers.uuid, s_profileState.info.uuid ) == 0 ) {
            return qtrue;
        }
    }

    return qfalse;
}

/* Gibt qtrue zurück wenn der Spieler über einen dedizierten Server
 * verbunden ist (nicht localClient). Wird für den Online-Multiplikator genutzt. */
static qboolean G_Profile_IsOnlineSession( const gclient_t *client ) {
    if ( !client ) {
        return qfalse;
    }
    return ( !client->pers.localClient && client->pers.uuid[0] ) ? qtrue : qfalse;
}

static qboolean G_Profile_IsRacingGametype( void ) {
    switch ( g_gametype.integer ) {
    case GT_RACING:
    case GT_RACING_DM:
    case GT_SPRINT:
    case GT_TEAM_RACING:
    case GT_TEAM_RACING_DM:
        return qtrue;
    default:
        break;
    }

    return qfalse;
}

typedef struct {
    char name[PROFILE_MAX_VEHICLE];
    int  timeMs;
} profile_vehicle_usage_t;

#define PROFILE_MAX_TRACKED_VEHICLES 64
static profile_vehicle_usage_t s_profileVehicleUsage[PROFILE_MAX_TRACKED_VEHICLES];
static const char *G_Profile_FindSectionEnd( const char *start, char endChar ) {
    const char *end = strchr( start, endChar );
    return end ? end : start;
}

static char G_Profile_ToLowerASCII( char c ) {
    return ( c >= 'A' && c <= 'Z' ) ? ( c + ( 'a' - 'A' ) ) : c;
}

static void G_Profile_NormalizeVehicleName( const char *vehicle, char *out, int outSize ) {
    int i;

    if ( !out || outSize <= 0 ) {
        return;
    }

    out[0] = '\0';

    if ( !vehicle ) {
        return;
    }

    for ( i = 0; i < outSize - 1 && vehicle[i]; ++i ) {
        if ( vehicle[i] == '/' ) {
            break;
        }

        out[i] = G_Profile_ToLowerASCII( vehicle[i] );
    }

    out[i] = '\0';
}

/*
 * G_Profile_GenerateUUID
 *
 * Erzeugt eine UUID v4 gemäß RFC 4122.
 *
 * Com_RandomBytes ist im game-Modul (qagame DLL) nicht verfügbar —
 * es lebt in qcommon und ist nicht über den Syscall-Mechanismus
 * erreichbar. Wir verwenden rand() mit level.time als Seed-Beitrag.
 *
 * Qualitätshinweis: diese UUID wird nur noch als Fallback generiert
 * wenn ein Legacy-Profil ohne UUID geladen wird. Neue Profile bekommen
 * ihre UUID bereits vom UI-Wizard (UI_Profile_WriteDefaultFile), der
 * trap_Milliseconds() + Profilname-Hash als Seed nutzt.
 *
 * out muss mindestens PROFILE_MAX_UUID (37) Bytes groß sein.
 */
static void G_Profile_GenerateUUID( char *out, int outSize ) {
    static const char hex[] = "0123456789abcdef";
    unsigned char b[16];
    int i;

    if ( !out || outSize < PROFILE_MAX_UUID ) {
        return;
    }

    /* Seed mit level.time XOR aktueller rand()-State für etwas mehr Varianz */
    srand( (unsigned int)( level.time ^ rand() ) );

    for ( i = 0; i < 16; ++i ) {
        b[i] = (unsigned char)( ( rand() ^ ( rand() << 8 ) ) & 0xFF );
    }

    /* RFC 4122 §4.4: Version = 4 */
    b[6] = ( b[6] & 0x0F ) | 0x40;

    /* RFC 4122 §4.4: Variant = 10xx */
    b[8] = ( b[8] & 0x3F ) | 0x80;

    Com_sprintf( out, outSize,
        "%c%c%c%c%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c%c%c%c%c%c%c%c%c",
        hex[b[ 0]>>4], hex[b[ 0]&0xF],
        hex[b[ 1]>>4], hex[b[ 1]&0xF],
        hex[b[ 2]>>4], hex[b[ 2]&0xF],
        hex[b[ 3]>>4], hex[b[ 3]&0xF],
        hex[b[ 4]>>4], hex[b[ 4]&0xF],
        hex[b[ 5]>>4], hex[b[ 5]&0xF],
        hex[b[ 6]>>4], hex[b[ 6]&0xF],
        hex[b[ 7]>>4], hex[b[ 7]&0xF],
        hex[b[ 8]>>4], hex[b[ 8]&0xF],
        hex[b[ 9]>>4], hex[b[ 9]&0xF],
        hex[b[10]>>4], hex[b[10]&0xF],
        hex[b[11]>>4], hex[b[11]&0xF],
        hex[b[12]>>4], hex[b[12]&0xF],
        hex[b[13]>>4], hex[b[13]&0xF],
        hex[b[14]>>4], hex[b[14]&0xF],
        hex[b[15]>>4], hex[b[15]&0xF]
    );
}

/*
 * G_Profile_IsValidUUID
 *
 * Prüft ob ein String das erwartete UUID-Format hat:
 * "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx" (36 Zeichen).
 * Lehnt leere Strings und offensichtlich korrupte Werte ab.
 * Keine vollständige RFC-Validierung — ausreichend als Ladezeit-Sanity-Check.
 */
static qboolean G_Profile_IsValidUUID( const char *s ) {
    int i;

    if ( !s ) {
        return qfalse;
    }

    if ( strlen( s ) != 36 ) {
        return qfalse;
    }

    /* Erwartetes Muster: 8-4-4-4-12 Hex-Gruppen, getrennt durch '-' */
    for ( i = 0; i < 36; ++i ) {
        char c = s[i];
        if ( i == 8 || i == 13 || i == 18 || i == 23 ) {
            if ( c != '-' ) return qfalse;
        } else {
            if ( !( ( c >= '0' && c <= '9' ) ||
                    ( c >= 'a' && c <= 'f' ) ||
                    ( c >= 'A' && c <= 'F' ) ) ) {
                return qfalse;
            }
        }
    }

    return qtrue;
}

qboolean Profile_GetRankForScore( const profile_stats_t *stats,
                                 const profile_rank_def_t *rankDefs,
                                 int rankDefCount,
                                 profile_rank_t *outRank ) {
    int i;
    const profile_rank_def_t *current;
    const profile_rank_def_t *next;
    int currentIndex;

    if ( !stats || !rankDefs || rankDefCount <= 0 || !outRank ) {
        return qfalse;
    }

    current = &rankDefs[0];
    next = NULL;
    currentIndex = 0;

    for ( i = 0; i < rankDefCount; ++i ) {
        if ( stats->playerScore >= rankDefs[i].minimumScore ) {
            current = &rankDefs[i];
            currentIndex = i;
        } else {
            next = &rankDefs[i];
            break;
        }
    }

    outRank->index = currentIndex;
    outRank->current = current;
    outRank->next = next;

    return qtrue;
}

static int G_Profile_ParseFavoriteCars( const char *buffer, profile_info_t *info ) {
    const char *favoritesStart;
    const char *cursor;
    int parsedFavorites = 0;

    if ( !buffer || !info ) {
        return 0;
    }

    favoritesStart = strstr( buffer, "\"favoriteCars\"" );
    if ( !favoritesStart ) {
        return 0;
    }

    favoritesStart = strchr( favoritesStart, '[' );
    if ( !favoritesStart ) {
        return 0;
    }

    cursor = favoritesStart + 1;

    while ( *cursor && *cursor != ']' && parsedFavorites < PROFILE_MAX_FAVORITE_CARS ) {
        const char *objectStart = strchr( cursor, '{' );
        const char *objectEnd;
        char objectBuffer[512];
        int objectLength;
        profile_info_t tempInfo;

        if ( !objectStart ) {
            break;
        }

        objectEnd = G_Profile_FindSectionEnd( objectStart, '}' );
        if ( !objectEnd || objectEnd <= objectStart ) {
            break;
        }
        objectLength = objectEnd - objectStart + 1;
        if ( objectLength >= (int)sizeof( objectBuffer ) ) {
            objectLength = sizeof( objectBuffer ) - 1;
        }

        Com_Memcpy( objectBuffer, objectStart, objectLength );
        objectBuffer[objectLength] = '\0';

        Com_Memset( &tempInfo.favoriteCars[parsedFavorites], 0, sizeof( tempInfo.favoriteCars[parsedFavorites] ) );
        G_Profile_ParseString( objectBuffer, "model", tempInfo.favoriteCars[parsedFavorites].model, sizeof( tempInfo.favoriteCars[parsedFavorites].model ), "" );
        G_Profile_ParseString( objectBuffer, "skin", tempInfo.favoriteCars[parsedFavorites].skin, sizeof( tempInfo.favoriteCars[parsedFavorites].skin ), "" );
        G_Profile_ParseString( objectBuffer, "rim", tempInfo.favoriteCars[parsedFavorites].rim, sizeof( tempInfo.favoriteCars[parsedFavorites].rim ), "" );
        G_Profile_ParseString( objectBuffer, "head", tempInfo.favoriteCars[parsedFavorites].head, sizeof( tempInfo.favoriteCars[parsedFavorites].head ), "" );

        info->favoriteCars[parsedFavorites] = tempInfo.favoriteCars[parsedFavorites];
        parsedFavorites++;

        cursor = objectEnd + 1;
    }

    return parsedFavorites;
}

static void G_Profile_MaybeAutosave( void ) {
    if ( s_profileState.dirty && level.time >= s_profileState.nextAutosaveTime ) {
        G_Profile_WriteToDisk();
    }
}

void G_Profile_FlushIfDirty( void ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( s_profileState.dirty ) {
        G_Profile_WriteToDisk();
    } else {
        s_profileState.nextAutosaveTime = level.time + PROFILE_AUTOSAVE_INTERVAL;
    }
}

qboolean G_Profile_IsDirty( void ) {
    return s_profileState.loaded && s_profileState.dirty;
}

qboolean G_Profile_GetRank( const profile_stats_t *stats, profile_rank_t *outRank ) {
    return Profile_GetRankForScore( stats, s_profileRankTable, PROFILE_RANK_COUNT, outRank );
}

int G_Profile_GetPlayerScore( void ) {
    if ( !s_profileState.loaded ) {
        return 0;
    }

    return s_profileState.stats.playerScore;
}

qboolean G_Profile_GetUUID( char *out, int outSize ) {
    if ( !out || outSize < PROFILE_MAX_UUID ) {
        return qfalse;
    }

    out[0] = '\0';

    if ( !s_profileState.loaded ) {
        return qfalse;
    }

    if ( !G_Profile_IsValidUUID( s_profileState.info.uuid ) ) {
        return qfalse;
    }

    Q_strncpyz( out, s_profileState.info.uuid, outSize );
    return qtrue;
}

static int G_Profile_ComputeSnapshotRevision( void ) {
    int revision = 0;

    revision ^= s_profileState.stats.playerScore;
    revision ^= ( s_profileState.stats.wins << 1 );
    revision ^= ( s_profileState.stats.losses << 2 );
    revision ^= ( s_profileState.stats.kills << 3 );
    revision ^= ( s_profileState.stats.deaths << 4 );
    revision ^= ( s_profileState.stats.gamesPlayed << 5 );
    revision ^= ( s_profileState.info.currentRank << 6 );
    revision ^= ( s_profileState.info.highestRank << 7 );
    revision ^= ( s_profileState.stats.bestLapMs << 8 );
    return revision;
}

qboolean G_Profile_GetLadderSnapshot( ladderProfileSnapshot_t *outSnapshot,
                                      int *outSnapshotRevision,
                                      int *outSnapshotEpoch ) {
    int i;
    qtime_t now;

    if ( !outSnapshot ) {
        return qfalse;
    }

    Com_Memset( outSnapshot, 0, sizeof( *outSnapshot ) );
    if ( outSnapshotRevision ) {
        *outSnapshotRevision = 0;
    }
    if ( outSnapshotEpoch ) {
        *outSnapshotEpoch = 0;
    }

    if ( !s_profileState.loaded ) {
        return qfalse;
    }

    outSnapshot->valid = qtrue;
    outSnapshot->snapshotRevision = G_Profile_ComputeSnapshotRevision();
    outSnapshot->snapshotEpoch = trap_RealTime( &now );

    outSnapshot->playerScore = s_profileState.stats.playerScore;
    outSnapshot->currentRank = s_profileState.info.currentRank;
    outSnapshot->highestRank = s_profileState.info.highestRank;
    outSnapshot->wins = s_profileState.stats.wins;
    outSnapshot->losses = s_profileState.stats.losses;
    outSnapshot->kills = s_profileState.stats.kills;
    outSnapshot->deaths = s_profileState.stats.deaths;
    outSnapshot->flagCaptures = s_profileState.stats.flagCaptures;
    outSnapshot->flagAssists = s_profileState.stats.flagAssists;
    outSnapshot->bestLapMs = s_profileState.stats.bestLapMs;
    outSnapshot->accuracyAwards = s_profileState.stats.accuracyAwards;
    outSnapshot->excellentAwards = s_profileState.stats.excellentAwards;
    outSnapshot->impressiveAwards = s_profileState.stats.impressiveAwards;
    outSnapshot->perfectAwards = s_profileState.stats.perfectAwards;
    outSnapshot->damageDealt = s_profileState.stats.damageDealt;
    outSnapshot->damageTaken = s_profileState.stats.damageTaken;
    outSnapshot->distanceKm = (float)s_profileState.stats.distanceKm;
    outSnapshot->topSpeedKph = (float)s_profileState.stats.topSpeedKph;
    outSnapshot->fuelUsed = (float)s_profileState.stats.fuelUsed;
    Q_strncpyz( outSnapshot->mostUsedVehicle, s_profileState.stats.mostUsedVehicle, sizeof( outSnapshot->mostUsedVehicle ) );
    outSnapshot->gamesPlayed = s_profileState.stats.gamesPlayed;

    outSnapshot->racingWins = s_profileState.stats.racingWins;
    outSnapshot->racingPodiums = s_profileState.stats.racingPodiums;
    outSnapshot->racingCompleted = s_profileState.stats.racingCompleted;
    outSnapshot->racingTotalMs = s_profileState.stats.racingTotalMs;
    outSnapshot->racingDmWins = s_profileState.stats.racingDmWins;
    outSnapshot->racingDmPodiums = s_profileState.stats.racingDmPodiums;
    outSnapshot->racingDmCompleted = s_profileState.stats.racingDmCompleted;
    outSnapshot->racingDmTotalMs = s_profileState.stats.racingDmTotalMs;
    outSnapshot->sprintWins = s_profileState.stats.sprintWins;
    outSnapshot->sprintCompleted = s_profileState.stats.sprintCompleted;
    outSnapshot->sprintBestMs = s_profileState.stats.sprintBestMs;
    outSnapshot->eliminationWins = s_profileState.stats.eliminationWins;
    outSnapshot->eliminationCompleted = s_profileState.stats.eliminationCompleted;
    outSnapshot->eliminationTotalRoundsLasted = s_profileState.stats.eliminationTotalRoundsLasted;
    outSnapshot->lcsWins = s_profileState.stats.lcsWins;
    outSnapshot->lcsCompleted = s_profileState.stats.lcsCompleted;
    outSnapshot->lcsTotalSurvivalMs = s_profileState.stats.lcsTotalSurvivalMs;
    outSnapshot->derbyWins = s_profileState.stats.derbyWins;
    outSnapshot->derbyCompleted = s_profileState.stats.derbyCompleted;
    outSnapshot->derbyKills = s_profileState.stats.derbyKills;
    outSnapshot->dmWins = s_profileState.stats.dmWins;
    outSnapshot->dmCompleted = s_profileState.stats.dmCompleted;
    outSnapshot->dmKills = s_profileState.stats.dmKills;
    outSnapshot->ctfWins = s_profileState.stats.ctfWins;
    outSnapshot->ctfCompleted = s_profileState.stats.ctfCompleted;
    outSnapshot->ctfCaptures = s_profileState.stats.ctfCaptures;
    outSnapshot->ctf4Wins = s_profileState.stats.ctf4Wins;
    outSnapshot->ctf4Completed = s_profileState.stats.ctf4Completed;
    outSnapshot->ctf4Captures = s_profileState.stats.ctf4Captures;
    outSnapshot->teamWins = s_profileState.stats.teamWins;
    outSnapshot->teamCompleted = s_profileState.stats.teamCompleted;
    outSnapshot->teamKills = s_profileState.stats.teamKills;
    outSnapshot->teamRacingWins = s_profileState.stats.teamRacingWins;
    outSnapshot->teamRacingCompleted = s_profileState.stats.teamRacingCompleted;
    outSnapshot->teamRacingPodiums = s_profileState.stats.teamRacingPodiums;
    outSnapshot->teamRacingDmWins = s_profileState.stats.teamRacingDmWins;
    outSnapshot->teamRacingDmCompleted = s_profileState.stats.teamRacingDmCompleted;
    outSnapshot->teamRacingDmPodiums = s_profileState.stats.teamRacingDmPodiums;
    outSnapshot->dominationWins = s_profileState.stats.dominationWins;
    outSnapshot->dominationCompleted = s_profileState.stats.dominationCompleted;
    outSnapshot->dominationZoneHoldMs = s_profileState.stats.dominationZoneHoldMs;
    outSnapshot->kothWins = s_profileState.stats.kothWins;
    outSnapshot->kothCompleted = s_profileState.stats.kothCompleted;
    outSnapshot->kothZoneHoldMs = s_profileState.stats.kothZoneHoldMs;

    for ( i = 0; i < BG_ACHIEVEMENT_CATEGORY_COUNT; ++i ) {
        outSnapshot->achievementTiers[i] = s_profileState.achievementsUnlocked[i];
    }

    if ( outSnapshotRevision ) {
        *outSnapshotRevision = outSnapshot->snapshotRevision;
    }
    if ( outSnapshotEpoch ) {
        *outSnapshotEpoch = outSnapshot->snapshotEpoch;
    }

    return qtrue;
}

static void G_Profile_UpdateRankState( void ) {
    profile_rank_t rank;

    if ( !G_Profile_GetRank( &s_profileState.stats, &rank ) ) {
        return;
    }

    s_profileState.info.currentRank = rank.index;
    if ( rank.index > s_profileState.info.highestRank ) {
        s_profileState.info.highestRank = rank.index;
        s_profileState.dirty = qtrue;
    }
}

static double G_Profile_GetAchievementProgressForCategory( bgAchievementCategory_t category ) {
    switch ( category ) {
    case BG_ACHIEVEMENT_DISTANCE:
        return s_profileState.stats.distanceKm;
    case BG_ACHIEVEMENT_KILLS:
        return s_profileState.stats.kills;
    case BG_ACHIEVEMENT_WINS:
        return s_profileState.stats.wins;
    case BG_ACHIEVEMENT_SPRINT_WINS:
        return s_profileState.stats.sprintWins;
    case BG_ACHIEVEMENT_FLAG_CAPTURES:
        return s_profileState.stats.flagCaptures;
    case BG_ACHIEVEMENT_FLAG_ASSISTS:
        return s_profileState.stats.flagAssists;
    case BG_ACHIEVEMENT_FUEL:
        return s_profileState.stats.fuelUsed;
    case BG_ACHIEVEMENT_ACCURACY:
        return s_profileState.stats.accuracyAwards;
    case BG_ACHIEVEMENT_EXCELLENT:
        return s_profileState.stats.excellentAwards;
    case BG_ACHIEVEMENT_IMPRESSIVE:
        return s_profileState.stats.impressiveAwards;
    case BG_ACHIEVEMENT_PERFECT:
        return s_profileState.stats.perfectAwards;
    default:
        break;
    }

    return 0.0;
}

static void G_Profile_SendAchievementUnlock( gclient_t *client, bgAchievementCategory_t category, int tierIndex ) {
    int clientNum;
    const bgAchievementCategoryDef_t *categoryDef;

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    categoryDef = BG_AchievementGetCategory( category );
    if ( !categoryDef || tierIndex < 0 || tierIndex >= categoryDef->tierCount ) {
        return;
    }

    clientNum = client - level.clients;
    trap_SendServerCommand( clientNum, va( "achv %d %d", category, tierIndex ) );
}

static void G_Profile_CheckAchievementProgress( gclient_t *client, bgAchievementCategory_t category ) {
    const bgAchievementCategoryDef_t *categoryDef;
    int unlocked;

    if ( !s_profileState.loaded || category < 0 || category >= BG_ACHIEVEMENT_CATEGORY_COUNT ) {
        return;
    }

    categoryDef = BG_AchievementGetCategory( category );
    unlocked = BG_AchievementUnlockedTiers( categoryDef, G_Profile_GetAchievementProgressForCategory( category ) );

    if ( unlocked > s_profileState.achievementsUnlocked[category] ) {
        int tier;
        int newlyUnlocked = unlocked - s_profileState.achievementsUnlocked[category];

        for ( tier = s_profileState.achievementsUnlocked[category]; tier < unlocked; ++tier ) {
            G_Profile_SendAchievementUnlock( client, category, tier );
        }

        s_profileState.achievementsUnlocked[category] = unlocked;

        if ( G_Profile_ShouldTrackClient( client ) && newlyUnlocked > 0 ) {
            G_Profile_AddScore( newlyUnlocked * PROFILE_SCORE_ACHIEVEMENT_TIER );
        }
    }
}

static void G_Profile_RecomputeAchievementState( void ) {
    int i;

    for ( i = 0; i < BG_ACHIEVEMENT_CATEGORY_COUNT; ++i ) {
        const bgAchievementCategoryDef_t *category = BG_AchievementGetCategory( i );
        s_profileState.achievementsUnlocked[i] = BG_AchievementUnlockedTiers( category, G_Profile_GetAchievementProgressForCategory( i ) );
    }
}

static qboolean G_Profile_IsValidName( const char *name ) {
    int i;
    int len;

    if ( !name ) {
        return qfalse;
    }

    len = strlen( name );
    if ( len <= 0 || len >= PROFILE_MAX_NAME ) {
        return qfalse;
    }

    for ( i = 0; i < len; ++i ) {
        char c = name[i];
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) ) {
            continue;
        }
        if ( c == '_' || c == '-' ) {
            continue;
        }
        return qfalse;
    }

    return qtrue;
}

static void G_Profile_BuildPath( const char *name, char *buffer, int bufferSize ) {
    Com_sprintf( buffer, bufferSize, "profiles/%s.json", name );
}

static double G_Profile_ParseDouble( const char *buffer, const char *key, double defaultValue ) {
    char pattern[64];
    const char *cursor;

    Com_sprintf( pattern, sizeof( pattern ), "\"%s\"", key );
    cursor = strstr( buffer, pattern );
    if ( !cursor ) {
        return defaultValue;
    }

    cursor = strchr( cursor, ':' );
    if ( !cursor ) {
        return defaultValue;
    }
    cursor++;

    while ( *cursor == ' ' || *cursor == '\t' ) {
        cursor++;
    }

    return atof( cursor );
}

static int G_Profile_ParseInt( const char *buffer, const char *key, int defaultValue ) {
    char pattern[64];
    const char *cursor;

    Com_sprintf( pattern, sizeof( pattern ), "\"%s\"", key );
    cursor = strstr( buffer, pattern );
    if ( !cursor ) {
        return defaultValue;
    }

    cursor = strchr( cursor, ':' );
    if ( !cursor ) {
        return defaultValue;
    }
    cursor++;

    while ( *cursor == ' ' || *cursor == '\t' ) {
        cursor++;
    }

    return atoi( cursor );
}

static void G_Profile_ParseString( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue ) {
    char pattern[64];
    const char *cursor;
    const char *start;
    int length;

    if ( !out || outSize <= 0 ) {
        return;
    }

    out[0] = '\0';
    if ( defaultValue ) {
        Q_strncpyz( out, defaultValue, outSize );
    }

    if ( !buffer || !key ) {
        return;
    }

    Com_sprintf( pattern, sizeof( pattern ), "\"%s\"", key );
    cursor = strstr( buffer, pattern );
    if ( !cursor ) {
        return;
    }

    cursor = strchr( cursor, ':' );
    if ( !cursor ) {
        return;
    }
    cursor++;

    while ( *cursor == ' ' || *cursor == '\t' ) {
        cursor++;
    }

    if ( *cursor != '"' ) {
        return;
    }
    cursor++;
    start = cursor;

    while ( *cursor && *cursor != '"' ) {
        if ( *cursor == '\\' && cursor[1] != '\0' ) {
            cursor += 2;
            continue;
        }
        cursor++;
    }

    length = cursor - start;
    if ( length >= outSize ) {
        length = outSize - 1;
    }

    Com_Memcpy( out, start, length );
    out[length] = '\0';

    // unescape quotes and backslashes in-place
    {
        char *dst = out;
        char *src = out;

        while ( *src ) {
            if ( *src == '\\' && src[1] ) {
                src++;
            }
            *dst++ = *src++;
        }
        *dst = '\0';
    }
}

static void G_Profile_FormatJsonString( char *out, int outSize, const char *value ) {
    const char *src;
    char *dst;

    if ( !out || outSize <= 0 ) {
        return;
    }

    if ( !value ) {
        value = "";
    }

    dst = out;
    src = value;

    while ( *src && ( dst - out ) < outSize - 1 ) {
        if ( (*src == '"' || *src == '\\') && ( dst - out ) < outSize - 2 ) {
            *dst++ = '\\';
        }

        if ( ( dst - out ) >= outSize - 1 ) {
            break;
        }

        *dst++ = *src++;
    }

    *dst = '\0';
}

static profile_vehicle_usage_t *G_Profile_FindVehicleUsage( const char *vehicle, qboolean allowCreate ) {
    int i;
    profile_vehicle_usage_t *empty = NULL;

    if ( !vehicle || !vehicle[0] ) {
        return NULL;
    }

    for ( i = 0; i < PROFILE_MAX_TRACKED_VEHICLES; ++i ) {
        if ( !s_profileVehicleUsage[i].name[0] ) {
            if ( !empty ) {
                empty = &s_profileVehicleUsage[i];
            }
            continue;
        }

        if ( !Q_stricmp( s_profileVehicleUsage[i].name, vehicle ) ) {
            return &s_profileVehicleUsage[i];
        }
    }

    if ( allowCreate && empty ) {
        Q_strncpyz( empty->name, vehicle, sizeof( empty->name ) );
        empty->timeMs = 0;
        return empty;
    }

    if ( allowCreate ) {
        G_PROFILE_LOG( "G_Profile: Vehicle list is full (%d entries), ignoring '%s'\n",
                   PROFILE_MAX_TRACKED_VEHICLES,
                   vehicle );
    }

    return NULL;
}

static void G_Profile_AddVehicleTime( const char *vehicle, int timeMs ) {
    char normalized[PROFILE_MAX_VEHICLE];
    profile_vehicle_usage_t *usage;
    qboolean isNewVehicle;

    if ( timeMs <= 0 ) {
        return;
    }

    G_Profile_NormalizeVehicleName( vehicle, normalized, sizeof( normalized ) );
    if ( !normalized[0] ) {
        return;
    }

    usage = G_Profile_FindVehicleUsage( normalized, qtrue );
    if ( !usage ) {
        G_PROFILE_LOG( "G_Profile: Failed to find/create vehicle usage for '%s'\n", vehicle );
        return;
    }

    isNewVehicle = ( usage->timeMs == 0 );
    usage->timeMs += timeMs;
    s_profileState.dirty = qtrue;

    G_PROFILE_LOG( "G_Profile: Vehicle '%s' now has %d ms (added %d ms)\n",
               usage->name, usage->timeMs, timeMs );

    if ( usage->timeMs > s_profileState.stats.mostUsedVehicleTimeMs ) {
        s_profileState.stats.mostUsedVehicleTimeMs = usage->timeMs;
        Q_strncpyz( s_profileState.stats.mostUsedVehicle, usage->name, sizeof( s_profileState.stats.mostUsedVehicle ) );

        G_PROFILE_LOG( "G_Profile: NEW most used vehicle: '%s' with %d ms\n",
                   s_profileState.stats.mostUsedVehicle,
                   s_profileState.stats.mostUsedVehicleTimeMs );
    }

    if ( isNewVehicle ) {
        s_profileState.nextAutosaveTime = level.time;
        G_Profile_FlushIfDirty();
    } else {
        G_Profile_MaybeAutosave();
    }
}

static void G_Profile_RecomputeMostUsedVehicle( void ) {
    int i;
    int bestTimeMs = 0;
    char bestVehicle[PROFILE_MAX_VEHICLE];

    bestVehicle[0] = '\0';

    for ( i = 0; i < PROFILE_MAX_TRACKED_VEHICLES; ++i ) {
        if ( !s_profileVehicleUsage[i].name[0] ) {
            continue;
        }

        if ( s_profileVehicleUsage[i].timeMs > bestTimeMs ) {
            bestTimeMs = s_profileVehicleUsage[i].timeMs;
            Q_strncpyz( bestVehicle, s_profileVehicleUsage[i].name, sizeof( bestVehicle ) );
        }
    }

    if ( bestTimeMs != s_profileState.stats.mostUsedVehicleTimeMs ||
         Q_stricmp( s_profileState.stats.mostUsedVehicle, bestVehicle ) ) {
        s_profileState.stats.mostUsedVehicleTimeMs = bestTimeMs;
        Q_strncpyz( s_profileState.stats.mostUsedVehicle, bestVehicle, sizeof( s_profileState.stats.mostUsedVehicle ) );
        s_profileState.dirty = qtrue;
    }
}

static void G_Profile_UpdateVehicleUsage( gentity_t *ent, int frameMsec ) {
    char userinfo[MAX_INFO_STRING];
    const char *vehicle;

    if ( frameMsec <= 0 ) {
        return;
    }

    trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
    
    // Versuche zuerst "chassis", dann "vehicle", dann "model"
    vehicle = Info_ValueForKey( userinfo, "chassis" );
    if ( !vehicle || !vehicle[0] ) {
        vehicle = Info_ValueForKey( userinfo, "vehicle" );
    }
    if ( !vehicle || !vehicle[0] ) {
        vehicle = Info_ValueForKey( userinfo, "model" );
    }

    if ( vehicle && vehicle[0] ) {
        G_Profile_AddVehicleTime( vehicle, frameMsec );
    }
}

static qboolean G_Profile_LoadFromDisk( void ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    static char buffer[PROFILE_FILE_BUFFER_SIZE];
    int length;
    const char *vehiclesStart;
    const char *cursor;

    if ( !s_profileState.name[0] ) {
        return qfalse;
    }

    G_Profile_BuildPath( s_profileState.name, path, sizeof( path ) );

    length = trap_FS_FOpenFile( path, &file, FS_READ );
    if ( length <= 0 ) {
        if ( file ) {
            trap_FS_FCloseFile( file );
        }
        return qfalse;
    }

    if ( length >= (int)sizeof( buffer ) ) {
        G_PROFILE_LOG( "G_Profile: Truncating profile read to %d bytes (file is %d bytes)\n",
                    (int)sizeof( buffer ) - 1,
                    length );
        length = sizeof( buffer ) - 1;
    }

    trap_FS_Read( buffer, length, file );
    buffer[length] = '\0';
    trap_FS_FCloseFile( file );

    Com_Memset( &s_profileState.stats, 0, sizeof( s_profileState.stats ) );
    Com_Memset( &s_profileState.info, 0, sizeof( s_profileState.info ) );
    Com_Memset( &s_profileVehicleUsage, 0, sizeof( s_profileVehicleUsage ) );

    s_profileState.stats.distanceKm = G_Profile_ParseDouble( buffer, "distanceKm", 0.0 );
    s_profileState.stats.fuelUsed = G_Profile_ParseDouble( buffer, "fuelUsed", 0.0 );
    s_profileState.stats.bestLapMs = G_Profile_ParseInt( buffer, "bestLapMs", 0 );
    s_profileState.stats.kills = G_Profile_ParseInt( buffer, "kills", 0 );
    s_profileState.stats.deaths = G_Profile_ParseInt( buffer, "deaths", 0 );
    s_profileState.stats.wins = G_Profile_ParseInt( buffer, "wins", 0 );
    s_profileState.stats.playerScore = G_Profile_ParseInt( buffer, "playerScore", 0 );
    s_profileState.stats.sprintWins = G_Profile_ParseInt( buffer, "sprintWins", 0 );
    s_profileState.stats.losses = G_Profile_ParseInt( buffer, "losses", 0 );
    s_profileState.stats.flagCaptures = G_Profile_ParseInt( buffer, "flagCaptures", 0 );
    s_profileState.stats.flagAssists = G_Profile_ParseInt( buffer, "flagAssists", 0 );
    s_profileState.stats.accuracyAwards = G_Profile_ParseInt( buffer, "accuracyAwards", 0 );
    s_profileState.stats.excellentAwards = G_Profile_ParseInt( buffer, "excellentAwards", 0 );
    s_profileState.stats.impressiveAwards = G_Profile_ParseInt( buffer, "impressiveAwards", 0 );
    s_profileState.stats.perfectAwards = G_Profile_ParseInt( buffer, "perfectAwards", 0 );
    s_profileState.stats.topSpeedKph = G_Profile_ParseDouble( buffer, "topSpeedKph", 0.0 );
    s_profileState.stats.damageDealt = G_Profile_ParseInt( buffer, "damageDealt", 0 );
    s_profileState.stats.damageTaken = G_Profile_ParseInt( buffer, "damageTaken", 0 );
    G_Profile_ParseString( buffer, "mostUsedVehicle", s_profileState.stats.mostUsedVehicle, sizeof( s_profileState.stats.mostUsedVehicle ), "" );
    s_profileState.stats.mostUsedVehicleTimeMs = G_Profile_ParseInt( buffer, "mostUsedVehicleTimeMs", 0 );
    s_profileState.stats.gamesPlayed           = G_Profile_ParseInt( buffer, "gamesPlayed",           0 );

    /* GT_RACING */
    s_profileState.stats.racingWins      = G_Profile_ParseInt( buffer, "racingWins",      0 );
    s_profileState.stats.racingPodiums   = G_Profile_ParseInt( buffer, "racingPodiums",   0 );
    s_profileState.stats.racingCompleted = G_Profile_ParseInt( buffer, "racingCompleted", 0 );
    s_profileState.stats.racingTotalMs   = G_Profile_ParseInt( buffer, "racingTotalMs",   0 );

    /* GT_RACING_DM */
    s_profileState.stats.racingDmWins      = G_Profile_ParseInt( buffer, "racingDmWins",      0 );
    s_profileState.stats.racingDmPodiums   = G_Profile_ParseInt( buffer, "racingDmPodiums",   0 );
    s_profileState.stats.racingDmCompleted = G_Profile_ParseInt( buffer, "racingDmCompleted", 0 );
    s_profileState.stats.racingDmTotalMs   = G_Profile_ParseInt( buffer, "racingDmTotalMs",   0 );

    /* GT_SPRINT */
    s_profileState.stats.sprintCompleted = G_Profile_ParseInt( buffer, "sprintCompleted", 0 );
    s_profileState.stats.sprintBestMs    = G_Profile_ParseInt( buffer, "sprintBestMs",    0 );

    /* GT_ELIMINATION */
    s_profileState.stats.eliminationWins              = G_Profile_ParseInt( buffer, "eliminationWins",              0 );
    s_profileState.stats.eliminationCompleted         = G_Profile_ParseInt( buffer, "eliminationCompleted",         0 );
    s_profileState.stats.eliminationTotalRoundsLasted = G_Profile_ParseInt( buffer, "eliminationTotalRoundsLasted", 0 );

    /* GT_LCS */
    s_profileState.stats.lcsWins            = G_Profile_ParseInt( buffer, "lcsWins",            0 );
    s_profileState.stats.lcsCompleted       = G_Profile_ParseInt( buffer, "lcsCompleted",       0 );
    s_profileState.stats.lcsTotalSurvivalMs = G_Profile_ParseInt( buffer, "lcsTotalSurvivalMs", 0 );

    /* GT_DERBY */
    s_profileState.stats.derbyWins      = G_Profile_ParseInt( buffer, "derbyWins",      0 );
    s_profileState.stats.derbyCompleted = G_Profile_ParseInt( buffer, "derbyCompleted", 0 );
    s_profileState.stats.derbyKills     = G_Profile_ParseInt( buffer, "derbyKills",     0 );

    /* GT_DEATHMATCH */
    s_profileState.stats.dmWins      = G_Profile_ParseInt( buffer, "dmWins",      0 );
    s_profileState.stats.dmCompleted = G_Profile_ParseInt( buffer, "dmCompleted", 0 );
    s_profileState.stats.dmKills     = G_Profile_ParseInt( buffer, "dmKills",     0 );

    /* GT_CTF */
    s_profileState.stats.ctfWins      = G_Profile_ParseInt( buffer, "ctfWins",      0 );
    s_profileState.stats.ctfCompleted = G_Profile_ParseInt( buffer, "ctfCompleted", 0 );
    s_profileState.stats.ctfCaptures  = G_Profile_ParseInt( buffer, "ctfCaptures",  0 );

    /* GT_CTF4 */
    s_profileState.stats.ctf4Wins      = G_Profile_ParseInt( buffer, "ctf4Wins",      0 );
    s_profileState.stats.ctf4Completed = G_Profile_ParseInt( buffer, "ctf4Completed", 0 );
    s_profileState.stats.ctf4Captures  = G_Profile_ParseInt( buffer, "ctf4Captures",  0 );

    /* GT_TEAM */
    s_profileState.stats.teamWins      = G_Profile_ParseInt( buffer, "teamWins",      0 );
    s_profileState.stats.teamCompleted = G_Profile_ParseInt( buffer, "teamCompleted", 0 );
    s_profileState.stats.teamKills     = G_Profile_ParseInt( buffer, "teamKills",     0 );

    /* GT_TEAM_RACING */
    s_profileState.stats.teamRacingWins      = G_Profile_ParseInt( buffer, "teamRacingWins",      0 );
    s_profileState.stats.teamRacingCompleted = G_Profile_ParseInt( buffer, "teamRacingCompleted", 0 );
    s_profileState.stats.teamRacingPodiums   = G_Profile_ParseInt( buffer, "teamRacingPodiums",   0 );

    /* GT_TEAM_RACING_DM */
    s_profileState.stats.teamRacingDmWins      = G_Profile_ParseInt( buffer, "teamRacingDmWins",      0 );
    s_profileState.stats.teamRacingDmCompleted = G_Profile_ParseInt( buffer, "teamRacingDmCompleted", 0 );
    s_profileState.stats.teamRacingDmPodiums   = G_Profile_ParseInt( buffer, "teamRacingDmPodiums",   0 );

    /* GT_DOMINATION */
    s_profileState.stats.dominationWins       = G_Profile_ParseInt( buffer, "dominationWins",       0 );
    s_profileState.stats.dominationCompleted  = G_Profile_ParseInt( buffer, "dominationCompleted",  0 );
    s_profileState.stats.dominationZoneHoldMs = G_Profile_ParseInt( buffer, "dominationZoneHoldMs", 0 );

    /* GT_KOTH */
    s_profileState.stats.kothWins       = G_Profile_ParseInt( buffer, "kothWins",       0 );
    s_profileState.stats.kothCompleted  = G_Profile_ParseInt( buffer, "kothCompleted",  0 );
    s_profileState.stats.kothZoneHoldMs = G_Profile_ParseInt( buffer, "kothZoneHoldMs", 0 );

    G_Profile_ParseString( buffer, "gender", s_profileState.info.gender, sizeof( s_profileState.info.gender ), "" );
    G_Profile_ParseString( buffer, "birthDate", s_profileState.info.birthDate, sizeof( s_profileState.info.birthDate ), "" );
    G_Profile_ParseString( buffer, "avatar", s_profileState.info.avatar, sizeof( s_profileState.info.avatar ), "" );
    G_Profile_ParseString( buffer, "country", s_profileState.info.country, sizeof( s_profileState.info.country ), "" );
    s_profileState.info.currentRank = G_Profile_ParseInt( buffer, "currentRank", 0 );
    s_profileState.info.highestRank = G_Profile_ParseInt( buffer, "highestRank", 0 );
    G_Profile_ParseFavoriteCars( buffer, &s_profileState.info );

    /* UUID lesen — Top-Level-Feld. Existiert nicht in Legacy-Profilen;
     * G_Profile_Init erkennt das leere Feld und generiert dann eine neue UUID. */
    G_Profile_ParseString( buffer, "uuid", s_profileState.info.uuid, sizeof( s_profileState.info.uuid ), "" );

    // Lade Vehicle-Array aus JSON
    vehiclesStart = strstr( buffer, "\"vehicles\"" );
    if ( vehiclesStart ) {
        vehiclesStart = strchr( vehiclesStart, '[' );
        if ( vehiclesStart ) {
            int vehicleIndex = 0;
            cursor = vehiclesStart + 1;

            while ( *cursor && *cursor != ']' ) {
                const char *nameStart;
                const char *nameEnd;
                const char *timeStr;
                char vehicleName[PROFILE_MAX_VEHICLE];
                char normalizedName[PROFILE_MAX_VEHICLE];
                int vehicleTime;
                int nameLength;
                profile_vehicle_usage_t *usage;

                if ( vehicleIndex >= PROFILE_MAX_TRACKED_VEHICLES ) {
                    G_PROFILE_LOG( "G_Profile: Skipping vehicle entries beyond limit of %d\n",
                               PROFILE_MAX_TRACKED_VEHICLES );
                    break;
                }

                // Finde "name"
                cursor = strstr( cursor, "\"name\"" );
                if ( !cursor ) break;
                
                cursor = strchr( cursor, ':' );
                if ( !cursor ) break;
                cursor++;
                
                while ( *cursor == ' ' || *cursor == '\t' ) cursor++;
                if ( *cursor != '"' ) break;
                cursor++;
                
                nameStart = cursor;
                nameEnd = strchr( cursor, '"' );
                if ( !nameEnd ) break;
                
                nameLength = nameEnd - nameStart;
                if ( nameLength >= PROFILE_MAX_VEHICLE ) nameLength = PROFILE_MAX_VEHICLE - 1;
                Com_Memcpy( vehicleName, nameStart, nameLength );
                vehicleName[nameLength] = '\0';
                
                // Finde "timeMs"
                cursor = strstr( nameEnd, "\"timeMs\"" );
                if ( !cursor ) break;
                
                cursor = strchr( cursor, ':' );
                if ( !cursor ) break;
                cursor++;

                while ( *cursor == ' ' || *cursor == '\t' ) cursor++;
                timeStr = cursor;
                vehicleTime = atoi( timeStr );

                G_Profile_NormalizeVehicleName( vehicleName, normalizedName, sizeof( normalizedName ) );
                if ( !normalizedName[0] ) {
                    // Nächstes Objekt
                    cursor = strchr( cursor, '}' );
                    if ( !cursor ) break;
                    cursor++;
                    vehicleIndex++;
                    continue;
                }

                usage = G_Profile_FindVehicleUsage( normalizedName, qtrue );
                if ( usage ) {
                    usage->timeMs += vehicleTime;
                }

                // Nächstes Objekt
                cursor = strchr( cursor, '}' );
                if ( !cursor ) break;
                cursor++;

                vehicleIndex++;
            }
        }
    }

    // Falls keine Vehicle-Liste vorhanden (Legacy), lade das mostUsedVehicle-Feld
    if ( s_profileState.stats.mostUsedVehicle[0] && s_profileVehicleUsage[0].name[0] == '\0' ) {
        char normalizedName[PROFILE_MAX_VEHICLE];
        profile_vehicle_usage_t *usage;

        G_Profile_NormalizeVehicleName( s_profileState.stats.mostUsedVehicle, normalizedName, sizeof( normalizedName ) );
        if ( normalizedName[0] ) {
            usage = G_Profile_FindVehicleUsage( normalizedName, qtrue );
            if ( usage ) {
                usage->timeMs = s_profileState.stats.mostUsedVehicleTimeMs;
            }
        }
    }

    G_Profile_RecomputeMostUsedVehicle();

    G_Profile_UpdateRankState();

    return qtrue;
}

static void G_Profile_WriteToDisk( void ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    static char buffer[PROFILE_FILE_BUFFER_SIZE];
    static char vehicleJson[PROFILE_VEHICLE_JSON_BUFFER_SIZE];
    static char favoriteCarsJson[1024];
    static char favoriteField[PROFILE_MAX_FAVORITE_FIELD * 2];
    static char readBuffer[PROFILE_FILE_BUFFER_SIZE];
    char gender[PROFILE_MAX_GENDER * 2];
    char birthDate[PROFILE_MAX_BIRTHDATE * 2];
    char avatar[PROFILE_MAX_AVATAR * 2];
    char country[PROFILE_MAX_COUNTRY * 2];
    fileHandle_t readFile;
    int readLength;
    int length;
    int i;
    int vehicleJsonPos;

    if ( !s_profileState.loaded || !s_profileState.name[0] ) {
        return;
    }

    G_Profile_BuildPath( s_profileState.name, path, sizeof( path ) );

    // Lese bestehende Info-Felder aus der Datei
    readLength = trap_FS_FOpenFile( path, &readFile, FS_READ );
    if ( readLength > 0 ) {
        if ( readLength >= (int)sizeof( readBuffer ) ) {
            readLength = sizeof( readBuffer ) - 1;
        }

        trap_FS_Read( readBuffer, readLength, readFile );
        readBuffer[readLength] = '\0';
        trap_FS_FCloseFile( readFile );

        G_Profile_ParseString( readBuffer, "gender", s_profileState.info.gender, sizeof( s_profileState.info.gender ), s_profileState.info.gender );
        G_Profile_ParseString( readBuffer, "birthDate", s_profileState.info.birthDate, sizeof( s_profileState.info.birthDate ), s_profileState.info.birthDate );
        G_Profile_ParseString( readBuffer, "avatar", s_profileState.info.avatar, sizeof( s_profileState.info.avatar ), s_profileState.info.avatar );
        G_Profile_ParseString( readBuffer, "country", s_profileState.info.country, sizeof( s_profileState.info.country ), s_profileState.info.country );
        s_profileState.info.currentRank = G_Profile_ParseInt( readBuffer, "currentRank", s_profileState.info.currentRank );
        s_profileState.info.highestRank = G_Profile_ParseInt( readBuffer, "highestRank", s_profileState.info.highestRank );

        /* UUID aus Datei nur übernehmen wenn im RAM noch keine valide UUID sitzt.
         * Schreibrichtung ist immer RAM → Disk, nie Disk → RAM bei der UUID,
         * ausser beim ersten Laden. So wird eine einmal generierte UUID nie
         * versehentlich durch einen Re-Read-Zyklus überschrieben. */
        if ( !G_Profile_IsValidUUID( s_profileState.info.uuid ) ) {
            char uuidOnDisk[PROFILE_MAX_UUID];
            G_Profile_ParseString( readBuffer, "uuid", uuidOnDisk, sizeof( uuidOnDisk ), "" );
            if ( G_Profile_IsValidUUID( uuidOnDisk ) ) {
                Q_strncpyz( s_profileState.info.uuid, uuidOnDisk, sizeof( s_profileState.info.uuid ) );
            }
        }
    } else if ( readFile ) {
        trap_FS_FCloseFile( readFile );
    }

    // Escape Sonderzeichen für JSON
    G_Profile_FormatJsonString( gender, sizeof( gender ), s_profileState.info.gender );
    G_Profile_FormatJsonString( birthDate, sizeof( birthDate ), s_profileState.info.birthDate );
    G_Profile_FormatJsonString( avatar, sizeof( avatar ), s_profileState.info.avatar );
    G_Profile_FormatJsonString( country, sizeof( country ), s_profileState.info.country );

    // Build favorite cars JSON
    vehicleJsonPos = 0;
    vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos, "[\n" );

    for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
        if ( i > 0 ) {
            vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos, ",\n" );
        }

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].model );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\t\t\t{\"model\": \"%s\", ", favoriteField );

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].skin );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\"skin\": \"%s\", ", favoriteField );

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].rim );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\"rim\": \"%s\", ", favoriteField );

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].head );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\"head\": \"%s\"}", favoriteField );
    }

    Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos, "\n\t\t]" );

    // Baue Fahrzeug-Array für JSON - manuell ohne Q_strcat
    vehicleJsonPos = Com_sprintf( vehicleJson, sizeof( vehicleJson ), "[\n" );
    if ( vehicleJsonPos < 0 ) {
        vehicleJsonPos = 0;
    }

    for ( i = 0; i < PROFILE_MAX_TRACKED_VEHICLES; ++i ) {
        int available;
        int written;

        if ( !s_profileVehicleUsage[i].name[0] ) {
            continue;
        }

        if ( vehicleJsonPos >= (int)sizeof( vehicleJson ) - 1 ) {
            G_PROFILE_LOG( "G_Profile: Vehicle JSON buffer full, truncating list at %d entries\n", i );
            break;
        }

        // Füge Komma hinzu, wenn nicht das erste Element
        if ( vehicleJsonPos > 2 ) {  // mehr als nur "[\n"
            available = sizeof( vehicleJson ) - vehicleJsonPos;
            written = Com_sprintf( vehicleJson + vehicleJsonPos, available, ",\n" );
            if ( written < 0 || written >= available ) {
                G_PROFILE_LOG( "G_Profile: Failed to append vehicle separator, truncating output\n" );
                vehicleJsonPos = sizeof( vehicleJson ) - 1;
                break;
            }
            vehicleJsonPos += written;
        }

        available = sizeof( vehicleJson ) - vehicleJsonPos;
        written = Com_sprintf( vehicleJson + vehicleJsonPos,
                               available,
                               "\t\t\t{\"name\": \"%s\", \"timeMs\": %d}",
                               s_profileVehicleUsage[i].name,
                               s_profileVehicleUsage[i].timeMs );
        if ( written < 0 || written >= available ) {
            G_PROFILE_LOG( "G_Profile: Truncated vehicle entry for '%s'\n", s_profileVehicleUsage[i].name );
            vehicleJsonPos = sizeof( vehicleJson ) - 1;
            break;
        }

        vehicleJsonPos += written;
    }

    Com_sprintf( vehicleJson + vehicleJsonPos, sizeof( vehicleJson ) - vehicleJsonPos, "\n\t\t]" );

    // Debug-Ausgabe
    G_PROFILE_LOG( "G_Profile: Writing vehicles array:\n%s\n", vehicleJson );
    G_PROFILE_LOG( "G_Profile: mostUsedVehicle='%s', timeMs=%d\n", 
               s_profileState.stats.mostUsedVehicle, 
               s_profileState.stats.mostUsedVehicleTimeMs );

    /* Write JSON in multiple Com_sprintf calls to stay within QVM argument limits.
     * Part 1: header + info section */
    length = Com_sprintf( buffer, sizeof( buffer ),
        "{\n"
        "\t\"uuid\": \"%s\",\n"
        "\t\"name\": \"%s\",\n"
        "\t\"info\": {\n"
        "\t\t\"gender\": \"%s\",\n"
        "\t\t\"birthDate\": \"%s\",\n"
        "\t\t\"avatar\": \"%s\",\n"
        "\t\t\"country\": \"%s\",\n"
        "\t\t\"currentRank\": %d,\n"
        "\t\t\"highestRank\": %d,\n"
        "\t\t\"favoriteCars\": %s\n"
        "\t},\n"
        "\t\"stats\": {\n",
        s_profileState.info.uuid,
        s_profileState.name,
        gender,
        birthDate,
        avatar,
        country,
        s_profileState.info.currentRank,
        s_profileState.info.highestRank,
        favoriteCarsJson );

    if ( length < 0 || length >= (int)sizeof( buffer ) ) {
        G_PROFILE_LOG( "G_Profile: Com_sprintf part1 failed\n" );
        return;
    }

    /* Part 2: base stats */
    {
        int len2 = Com_sprintf( buffer + length, sizeof( buffer ) - length,
            "\t\t\"distanceKm\": %.6f,\n"
            "\t\t\"fuelUsed\": %.3f,\n"
            "\t\t\"bestLapMs\": %d,\n"
            "\t\t\"kills\": %d,\n"
            "\t\t\"deaths\": %d,\n"
            "\t\t\"wins\": %d,\n"
            "\t\t\"playerScore\": %d,\n"
            "\t\t\"sprintWins\": %d,\n"
            "\t\t\"losses\": %d,\n"
            "\t\t\"flagCaptures\": %d,\n"
            "\t\t\"flagAssists\": %d,\n"
            "\t\t\"accuracyAwards\": %d,\n"
            "\t\t\"excellentAwards\": %d,\n"
            "\t\t\"impressiveAwards\": %d,\n"
            "\t\t\"perfectAwards\": %d,\n"
            "\t\t\"topSpeedKph\": %.2f,\n"
            "\t\t\"damageDealt\": %d,\n"
            "\t\t\"damageTaken\": %d,\n"
            "\t\t\"mostUsedVehicle\": \"%s\",\n"
            "\t\t\"mostUsedVehicleTimeMs\": %d,\n"
            "\t\t\"gamesPlayed\": %d,\n",
            s_profileState.stats.distanceKm,
            s_profileState.stats.fuelUsed,
            s_profileState.stats.bestLapMs,
            s_profileState.stats.kills,
            s_profileState.stats.deaths,
            s_profileState.stats.wins,
            s_profileState.stats.playerScore,
            s_profileState.stats.sprintWins,
            s_profileState.stats.losses,
            s_profileState.stats.flagCaptures,
            s_profileState.stats.flagAssists,
            s_profileState.stats.accuracyAwards,
            s_profileState.stats.excellentAwards,
            s_profileState.stats.impressiveAwards,
            s_profileState.stats.perfectAwards,
            s_profileState.stats.topSpeedKph,
            s_profileState.stats.damageDealt,
            s_profileState.stats.damageTaken,
            s_profileState.stats.mostUsedVehicle,
            s_profileState.stats.mostUsedVehicleTimeMs,
            s_profileState.stats.gamesPlayed );
        if ( len2 < 0 || length + len2 >= (int)sizeof( buffer ) ) {
            G_PROFILE_LOG( "G_Profile: Com_sprintf part2 failed\n" );
            return;
        }
        length += len2;
    }

    /* Part 3: mode-specific stats */
    {
        int len3 = Com_sprintf( buffer + length, sizeof( buffer ) - length,
            "\t\t\"racingWins\": %d,\n"
            "\t\t\"racingPodiums\": %d,\n"
            "\t\t\"racingCompleted\": %d,\n"
            "\t\t\"racingTotalMs\": %d,\n"
            "\t\t\"racingDmWins\": %d,\n"
            "\t\t\"racingDmPodiums\": %d,\n"
            "\t\t\"racingDmCompleted\": %d,\n"
            "\t\t\"racingDmTotalMs\": %d,\n"
            "\t\t\"sprintCompleted\": %d,\n"
            "\t\t\"sprintBestMs\": %d,\n"
            "\t\t\"eliminationWins\": %d,\n"
            "\t\t\"eliminationCompleted\": %d,\n"
            "\t\t\"eliminationTotalRoundsLasted\": %d,\n"
            "\t\t\"lcsWins\": %d,\n"
            "\t\t\"lcsCompleted\": %d,\n"
            "\t\t\"lcsTotalSurvivalMs\": %d,\n"
            "\t\t\"derbyWins\": %d,\n"
            "\t\t\"derbyCompleted\": %d,\n"
            "\t\t\"derbyKills\": %d,\n"
            "\t\t\"dmWins\": %d,\n"
            "\t\t\"dmCompleted\": %d,\n"
            "\t\t\"dmKills\": %d,\n",
            s_profileState.stats.racingWins,
            s_profileState.stats.racingPodiums,
            s_profileState.stats.racingCompleted,
            s_profileState.stats.racingTotalMs,
            s_profileState.stats.racingDmWins,
            s_profileState.stats.racingDmPodiums,
            s_profileState.stats.racingDmCompleted,
            s_profileState.stats.racingDmTotalMs,
            s_profileState.stats.sprintCompleted,
            s_profileState.stats.sprintBestMs,
            s_profileState.stats.eliminationWins,
            s_profileState.stats.eliminationCompleted,
            s_profileState.stats.eliminationTotalRoundsLasted,
            s_profileState.stats.lcsWins,
            s_profileState.stats.lcsCompleted,
            s_profileState.stats.lcsTotalSurvivalMs,
            s_profileState.stats.derbyWins,
            s_profileState.stats.derbyCompleted,
            s_profileState.stats.derbyKills,
            s_profileState.stats.dmWins,
            s_profileState.stats.dmCompleted,
            s_profileState.stats.dmKills );
        if ( len3 < 0 || length + len3 >= (int)sizeof( buffer ) ) {
            G_PROFILE_LOG( "G_Profile: Com_sprintf part3 failed\n" );
            return;
        }
        length += len3;
    }

    /* Part 4: team/zone stats + footer */
    {
        int len4 = Com_sprintf( buffer + length, sizeof( buffer ) - length,
            "\t\t\"ctfWins\": %d,\n"
            "\t\t\"ctfCompleted\": %d,\n"
            "\t\t\"ctfCaptures\": %d,\n"
            "\t\t\"ctf4Wins\": %d,\n"
            "\t\t\"ctf4Completed\": %d,\n"
            "\t\t\"ctf4Captures\": %d,\n"
            "\t\t\"teamWins\": %d,\n"
            "\t\t\"teamCompleted\": %d,\n"
            "\t\t\"teamKills\": %d,\n"
            "\t\t\"teamRacingWins\": %d,\n"
            "\t\t\"teamRacingCompleted\": %d,\n"
            "\t\t\"teamRacingPodiums\": %d,\n"
            "\t\t\"teamRacingDmWins\": %d,\n"
            "\t\t\"teamRacingDmCompleted\": %d,\n"
            "\t\t\"teamRacingDmPodiums\": %d,\n"
            "\t\t\"dominationWins\": %d,\n"
            "\t\t\"dominationCompleted\": %d,\n"
            "\t\t\"dominationZoneHoldMs\": %d,\n"
            "\t\t\"kothWins\": %d,\n"
            "\t\t\"kothCompleted\": %d,\n"
            "\t\t\"kothZoneHoldMs\": %d,\n"
            "\t\t\"vehicles\": %s\n"
            "\t}\n"
            "}\n",
            s_profileState.stats.ctfWins,
            s_profileState.stats.ctfCompleted,
            s_profileState.stats.ctfCaptures,
            s_profileState.stats.ctf4Wins,
            s_profileState.stats.ctf4Completed,
            s_profileState.stats.ctf4Captures,
            s_profileState.stats.teamWins,
            s_profileState.stats.teamCompleted,
            s_profileState.stats.teamKills,
            s_profileState.stats.teamRacingWins,
            s_profileState.stats.teamRacingCompleted,
            s_profileState.stats.teamRacingPodiums,
            s_profileState.stats.teamRacingDmWins,
            s_profileState.stats.teamRacingDmCompleted,
            s_profileState.stats.teamRacingDmPodiums,
            s_profileState.stats.dominationWins,
            s_profileState.stats.dominationCompleted,
            s_profileState.stats.dominationZoneHoldMs,
            s_profileState.stats.kothWins,
            s_profileState.stats.kothCompleted,
            s_profileState.stats.kothZoneHoldMs,
            vehicleJson );
        if ( len4 < 0 || length + len4 >= (int)sizeof( buffer ) ) {
            G_PROFILE_LOG( "G_Profile: Com_sprintf part4 failed\n" );
            return;
        }
        length += len4;
    }

    if ( length < 0 ) {
        G_PROFILE_LOG( "G_Profile: Com_sprintf failed\n" );
        return;
    }

    if ( length >= (int)sizeof( buffer ) ) {
        G_PROFILE_LOG( "G_Profile: Profile JSON truncated to %d bytes (needed %d)\n",
                    (int)sizeof( buffer ) - 1,
                    length );
        length = sizeof( buffer ) - 1;
    }

    // Debug-Ausgabe des gesamten Buffers
    G_PROFILE_LOG( "G_Profile: Writing profile file:\n%s\n", buffer );

    trap_FS_FOpenFile( path, &file, FS_WRITE );
    if ( file <= 0 ) {
        G_PROFILE_LOG( "G_Profile: Failed to open %s for writing\n", path );
        return;
    }

    trap_FS_Write( buffer, length, file );
    trap_FS_FCloseFile( file );

    G_PROFILE_LOG( "G_Profile: Successfully wrote %d bytes to %s\n", length, path );

    s_profileState.dirty = qfalse;
    s_profileState.nextAutosaveTime = level.time + PROFILE_AUTOSAVE_INTERVAL;
}

static void G_Profile_ClearState( void ) {
    Com_Memset( &s_profileState, 0, sizeof( s_profileState ) );
    Com_Memset( &s_profileVehicleUsage, 0, sizeof( s_profileVehicleUsage ) );
}

void G_Profile_Init( void ) {
    char activeName[PROFILE_MAX_NAME];

    G_Profile_ClearState();

    trap_Cvar_VariableStringBuffer( "profile_active", activeName, sizeof( activeName ) );
    if ( !activeName[0] ) {
        return;
    }

    if ( !G_Profile_IsValidName( activeName ) ) {
        G_PROFILE_LOG( "G_Profile: Ignoring invalid profile name '%s'\n", activeName );
        return;
    }

    Q_strncpyz( s_profileState.name, activeName, sizeof( s_profileState.name ) );
    if ( !G_Profile_LoadFromDisk() ) {
        Com_Memset( &s_profileState.stats, 0, sizeof( s_profileState.stats ) );
        Com_Memset( &s_profileState.info, 0, sizeof( s_profileState.info ) );
        s_profileState.dirty = qtrue;
        G_Profile_WriteToDisk();
    }

    /* UUID sicherstellen: falls das Profil geladen wurde aber noch keine
     * gültige UUID hat (Legacy-Profil vor dieser Änderung), wird jetzt
     * eine generiert. Neue Profile erhalten ihre UUID bereits vom Wizard. */
    if ( !G_Profile_IsValidUUID( s_profileState.info.uuid ) ) {
        G_Profile_GenerateUUID( s_profileState.info.uuid, sizeof( s_profileState.info.uuid ) );
        Com_Printf( "Q3Rally Profile: generated UUID %s for '%s'\n",
                    s_profileState.info.uuid, s_profileState.name );
        s_profileState.dirty = qtrue;
        G_Profile_WriteToDisk();
    } else {
        Com_Printf( "Q3Rally Profile: loaded UUID %s for '%s'\n",
                    s_profileState.info.uuid, s_profileState.name );
    }

    G_Profile_RecomputeAchievementState();

    s_profileState.loaded = qtrue;
    s_profileState.nextAutosaveTime = level.time + PROFILE_AUTOSAVE_INTERVAL;
}

void G_Profile_Shutdown( void ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    G_Profile_FlushIfDirty();

    G_Profile_ClearState();
}

void G_Profile_RecordDamage( gclient_t *attacker, gclient_t *victim, int damage ) {
    if ( damage <= 0 || !s_profileState.loaded ) {
        return;
    }

    if ( attacker && G_Profile_ShouldTrackClient( attacker ) && attacker != victim ) {
        s_profileState.stats.damageDealt += damage;
        s_profileState.dirty = qtrue;
    }

    if ( victim && G_Profile_ShouldTrackClient( victim ) ) {
        s_profileState.stats.damageTaken += damage;
        s_profileState.dirty = qtrue;
    }
}

void G_Profile_TrackClientSpawn( gclient_t *client ) {
    if ( !client ) {
        return;
    }

    /* Erkenne ob diese Session online ist (dedizierter Server + UUID) */
    if ( G_Profile_IsOnlineSession( client ) && G_Profile_ShouldTrackClient( client ) ) {
        s_profileState.isOnlineSession = qtrue;
    }

    client->profileHasLastOrigin = qfalse;
    client->profileLastTime = 0;
}

void G_Profile_UpdateClientFrame( gentity_t *ent ) {
    gclient_t *client;
    vec3_t delta;
    double distanceQu;
    double distanceKm;
    double speedQu;
    double speedKph;
    int frameMsec;
    int currentTime;

    if ( !s_profileState.loaded || !s_profileState.name[0] ) {
        return;
    }

    if ( !ent || !( client = ent->client ) ) {
        return;
    }

    if ( ent->r.svFlags & SVF_BOT || client->pers.connected != CON_CONNECTED ) {
        return;
    }

    if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    // Zeit-Tracking: Messe tatsächlich vergangene Zeit seit letztem Frame
    currentTime = level.time;
    
    if ( client->profileLastTime == 0 ) {
        // Erster Frame nach Spawn - initialisiere nur, addiere keine Zeit
        client->profileLastTime = currentTime;
        frameMsec = 0;
    } else {
        frameMsec = currentTime - client->profileLastTime;
        
        // Sicherheitscheck: Verhindere negative oder unrealistisch große Werte
        if ( frameMsec < 0 || frameMsec > 1000 ) {
            frameMsec = 0;
        }
        
        client->profileLastTime = currentTime;
    }

    // Distanz-Tracking
    if ( !client->profileHasLastOrigin ) {
        VectorCopy( ent->r.currentOrigin, client->profileLastOrigin );
        client->profileHasLastOrigin = qtrue;
    } else {
        VectorSubtract( ent->r.currentOrigin, client->profileLastOrigin, delta );
        distanceQu = VectorLength( delta );
        if ( distanceQu > 0.0 ) {
            distanceKm = distanceQu / CP_M_2_QU / 1000.0;
            s_profileState.stats.distanceKm += distanceKm;
            s_profileState.stats.fuelUsed += distanceKm * ( PROFILE_DISPLAY_L_PER_100KM / 100.0f );
            s_profileState.dirty = qtrue;
        }
        VectorCopy( ent->r.currentOrigin, client->profileLastOrigin );
    }

    G_Profile_UpdateVehicleUsage( ent, frameMsec );

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_DISTANCE );
    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_FUEL );

    // Geschwindigkeits-Tracking
    speedQu = VectorLength( client->ps.velocity );
    speedKph = ( speedQu / CP_M_2_QU ) * 3.6;
    if ( speedKph > s_profileState.stats.topSpeedKph ) {
        s_profileState.stats.topSpeedKph = speedKph;
        s_profileState.dirty = qtrue;
    }

    // Auto-Save Check
    G_Profile_MaybeAutosave();
}

void G_Profile_AddScore( int delta ) {
    int previousRank;
    profile_rank_t rankInfo;
    int clientNum = -1;
    int i;

    if ( !s_profileState.loaded || delta == 0 ) {
        return;
    }

    /* Online-Bonus: Score-Punkte werden mit 1.25 multipliziert wenn
     * der Spieler über einen dedizierten Server verbunden ist.
     * Nur auf positive Deltas — Strafen bleiben unverändert. */
    if ( s_profileState.isOnlineSession && delta > 0 ) {
        delta = (int)( delta * 1.25f );
        if ( delta < 1 ) delta = 1;
    }

    previousRank = s_profileState.info.currentRank;

    s_profileState.stats.playerScore += delta;
    s_profileState.dirty = qtrue;

    G_Profile_UpdateRankState();

    if ( G_Profile_GetRank( &s_profileState.stats, &rankInfo ) && rankInfo.index != previousRank ) {
        for ( i = 0; i < level.maxclients; ++i ) {
            gclient_t *client = &level.clients[i];

            if ( client->pers.connected == CON_CONNECTED && client->pers.localClient ) {
                clientNum = i;
                break;
            }
        }

        if ( clientNum >= 0 && rankInfo.current && rankInfo.current->name ) {
            const char *nextName = ( rankInfo.next && rankInfo.next->name ) ? rankInfo.next->name : "";

            trap_SendServerCommand( clientNum,
                                    va( ( rankInfo.index > previousRank ) ? "rankup %d \"%s\" \"%s\"" :
                                         "rankdown %d \"%s\" \"%s\"",
                                         rankInfo.index, rankInfo.current->name, nextName ) );
        }
    }

    G_Profile_MaybeAutosave();
}

void G_Profile_RecordKill( gclient_t *attacker, gclient_t *victim ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( attacker ) ) {
        return;
    }

    if ( attacker == victim ) {
        G_Profile_AddScore( PROFILE_SCORE_SUICIDE );
        return;
    }

    s_profileState.stats.kills++;
    s_profileState.dirty = qtrue;

    /* Modi-spezifische Kill-Zähler */
    switch ( g_gametype.integer ) {
    case GT_DEATHMATCH:
        s_profileState.stats.dmKills++;
        break;
    case GT_DERBY:
        s_profileState.stats.derbyKills++;
        break;
    case GT_TEAM:
        s_profileState.stats.teamKills++;
        break;
    default:
        break;
    }

    G_Profile_AddScore( PROFILE_SCORE_FRAG );

    G_Profile_CheckAchievementProgress( attacker, BG_ACHIEVEMENT_KILLS );
}

void G_Profile_RecordDeath( gclient_t *victim ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( victim ) ) {
        return;
    }

    s_profileState.stats.deaths++;
    s_profileState.dirty = qtrue;

    G_Profile_AddScore( PROFILE_SCORE_DEATH );
}

void G_Profile_RecordFlagCapture( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.flagCaptures++;
    s_profileState.dirty = qtrue;

    /* Modi-spezifische Capture-Zähler */
    G_Profile_RecordCtfCapture( client );

    G_Profile_AddScore( PROFILE_SCORE_FLAG_CAPTURE );

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_FLAG_CAPTURES );
}

void G_Profile_RecordFlagAssist( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.flagAssists++;
    s_profileState.dirty = qtrue;

    G_Profile_AddScore( PROFILE_SCORE_FLAG_ASSIST );

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_FLAG_ASSISTS );
}

void G_Profile_RecordLapComplete( gclient_t *client, qboolean isLeader, qboolean allowRankProgress ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( !allowRankProgress ) {
        return;
    }

    G_Profile_AddScore( PROFILE_SCORE_LAP );

    if ( isLeader ) {
        G_Profile_AddScore( PROFILE_SCORE_LEAD_LAP );
    }
}

void G_Profile_RecordRacePlacement( gclient_t *client, int position ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( !G_Profile_IsRacingGametype() ) {
        return;
    }

    if ( position <= 0 ) {
        return;
    }

    /* Podium (1.-3. Platz) */
    if ( position <= 3 ) {
        switch ( g_gametype.integer ) {
        case GT_RACING:
            s_profileState.stats.racingPodiums++;
            break;
        case GT_RACING_DM:
            s_profileState.stats.racingDmPodiums++;
            break;
        case GT_TEAM_RACING:
            s_profileState.stats.teamRacingPodiums++;
            break;
        case GT_TEAM_RACING_DM:
            s_profileState.stats.teamRacingDmPodiums++;
            break;
        default:
            break;
        }
        s_profileState.dirty = qtrue;
        return;
    }

    /* Position schlechter als 3 — Strafpunkte */
    if ( client->pers.profileRacePlacementPenalized ) {
        return;
    }

    G_Profile_AddScore( -5 );
    client->pers.profileRacePlacementPenalized = qtrue;
}

/* Gesamte Rennzeit für Racing-Modi akkumulieren.
 * Wird am Ende eines Rennens mit der tatsächlichen Fahrzeit aufgerufen. */
void G_Profile_RecordRaceTime( gclient_t *client, int totalRaceMs ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( totalRaceMs <= 0 ) {
        return;
    }

    switch ( g_gametype.integer ) {
    case GT_RACING:
        s_profileState.stats.racingTotalMs += totalRaceMs;
        break;
    case GT_RACING_DM:
        s_profileState.stats.racingDmTotalMs += totalRaceMs;
        break;
    default:
        break;
    }
    s_profileState.dirty = qtrue;
}

/* Überlebenszeit für LCS akkumulieren. */
void G_Profile_RecordSurvivalTime( gclient_t *client, int survivalMs ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( survivalMs <= 0 ) {
        return;
    }

    if ( g_gametype.integer == GT_LCS ) {
        s_profileState.stats.lcsTotalSurvivalMs += survivalMs;
        s_profileState.dirty = qtrue;
    }
}

/* Elimination-Runden-Zähler: wie viele Runden der Spieler überlebt hat. */
void G_Profile_RecordEliminationRound( gclient_t *client, int roundsLasted ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( roundsLasted <= 0 ) {
        return;
    }

    if ( g_gametype.integer == GT_ELIMINATION ) {
        s_profileState.stats.eliminationTotalRoundsLasted += roundsLasted;
        s_profileState.dirty = qtrue;
    }
}

/* Zone-Haltezeit für Domination und KOTH akkumulieren. */
void G_Profile_RecordZoneHold( gclient_t *client, int zoneHoldMs ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( zoneHoldMs <= 0 ) {
        return;
    }

    switch ( g_gametype.integer ) {
    case GT_DOMINATION:
        s_profileState.stats.dominationZoneHoldMs += zoneHoldMs;
        break;
    case GT_KOTH:
        s_profileState.stats.kothZoneHoldMs += zoneHoldMs;
        break;
    default:
        break;
    }
    s_profileState.dirty = qtrue;
}

/* CTF-Captures für den jeweiligen Modus zählen. */
void G_Profile_RecordCtfCapture( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    switch ( g_gametype.integer ) {
    case GT_CTF:
        s_profileState.stats.ctfCaptures++;
        break;
    case GT_CTF4:
        s_profileState.stats.ctf4Captures++;
        break;
    default:
        break;
    }
    s_profileState.dirty = qtrue;
}

/* Sprint best time */
void G_Profile_RecordSprintTime( gclient_t *client, int totalMs ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( g_gametype.integer != GT_SPRINT || totalMs <= 0 ) {
        return;
    }

    if ( s_profileState.stats.sprintBestMs == 0 || totalMs < s_profileState.stats.sprintBestMs ) {
        s_profileState.stats.sprintBestMs = totalMs;
        s_profileState.dirty = qtrue;
    }
}

void G_Profile_RecordWin( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.wins++;
    s_profileState.dirty = qtrue;

    /* Modi-spezifische Win-Zähler */
    switch ( g_gametype.integer ) {
    case GT_RACING:
        s_profileState.stats.racingWins++;
        break;
    case GT_RACING_DM:
        s_profileState.stats.racingDmWins++;
        break;
    case GT_SPRINT:
        s_profileState.stats.sprintWins++;
        s_profileState.stats.sprintCompleted++;
        G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_SPRINT_WINS );
        break;
    case GT_ELIMINATION:
        s_profileState.stats.eliminationWins++;
        s_profileState.stats.eliminationCompleted++;
        break;
    case GT_LCS:
        s_profileState.stats.lcsWins++;
        s_profileState.stats.lcsCompleted++;
        break;
    case GT_DERBY:
        s_profileState.stats.derbyWins++;
        s_profileState.stats.derbyCompleted++;
        break;
    case GT_DEATHMATCH:
        s_profileState.stats.dmWins++;
        s_profileState.stats.dmCompleted++;
        break;
    case GT_CTF:
        s_profileState.stats.ctfWins++;
        s_profileState.stats.ctfCompleted++;
        break;
    case GT_CTF4:
        s_profileState.stats.ctf4Wins++;
        s_profileState.stats.ctf4Completed++;
        break;
    case GT_TEAM:
        s_profileState.stats.teamWins++;
        s_profileState.stats.teamCompleted++;
        break;
    case GT_TEAM_RACING:
        s_profileState.stats.teamRacingWins++;
        s_profileState.stats.teamRacingCompleted++;
        break;
    case GT_TEAM_RACING_DM:
        s_profileState.stats.teamRacingDmWins++;
        s_profileState.stats.teamRacingDmCompleted++;
        break;
    case GT_DOMINATION:
        s_profileState.stats.dominationWins++;
        s_profileState.stats.dominationCompleted++;
        break;
    case GT_KOTH:
        s_profileState.stats.kothWins++;
        s_profileState.stats.kothCompleted++;
        break;
    default:
        break;
    }

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_WINS );

    if ( G_Profile_ShouldTrackClient( client ) ) {
        if ( G_Profile_IsRacingGametype() ) {
            G_Profile_AddScore( PROFILE_SCORE_RACE_WIN );
        } else {
            G_Profile_AddScore( PROFILE_SCORE_ELIMINATION_WIN );
        }
    }
}

void G_Profile_RecordLoss( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.losses++;
    s_profileState.stats.gamesPlayed++;
    s_profileState.dirty = qtrue;

    /* Modi-spezifische Completed-Zähler (nur bei Verlierern —
     * Gewinner zählen ihr Completed bereits in RecordWin) */
    switch ( g_gametype.integer ) {
    case GT_RACING:
        s_profileState.stats.racingCompleted++;
        break;
    case GT_RACING_DM:
        s_profileState.stats.racingDmCompleted++;
        break;
    case GT_SPRINT:
        s_profileState.stats.sprintCompleted++;
        break;
    case GT_ELIMINATION:
        s_profileState.stats.eliminationCompleted++;
        break;
    case GT_LCS:
        s_profileState.stats.lcsCompleted++;
        break;
    case GT_DERBY:
        s_profileState.stats.derbyCompleted++;
        break;
    case GT_DEATHMATCH:
        s_profileState.stats.dmCompleted++;
        break;
    case GT_CTF:
        s_profileState.stats.ctfCompleted++;
        break;
    case GT_CTF4:
        s_profileState.stats.ctf4Completed++;
        break;
    case GT_TEAM:
        s_profileState.stats.teamCompleted++;
        break;
    case GT_TEAM_RACING:
        s_profileState.stats.teamRacingCompleted++;
        break;
    case GT_TEAM_RACING_DM:
        s_profileState.stats.teamRacingDmCompleted++;
        break;
    case GT_DOMINATION:
        s_profileState.stats.dominationCompleted++;
        break;
    case GT_KOTH:
        s_profileState.stats.kothCompleted++;
        break;
    default:
        break;
    }

    if ( G_Profile_ShouldTrackClient( client ) && !G_Profile_IsRacingGametype() ) {
        G_Profile_AddScore( -5 );
    }
}

void G_Profile_RecordDominationCapture( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    G_Profile_AddScore( PROFILE_SCORE_DOMINATION_CAPTURE );
}

void G_Profile_RecordBestLap( gclient_t *client, int lapTime ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( lapTime <= 0 ) {
        return;
    }

    if ( s_profileState.stats.bestLapMs == 0 || lapTime < s_profileState.stats.bestLapMs ) {
        s_profileState.stats.bestLapMs = lapTime;
        s_profileState.dirty = qtrue;
    }
}

void G_Profile_RecordExcellent( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.excellentAwards++;
    s_profileState.dirty = qtrue;

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_EXCELLENT );
}

void G_Profile_RecordImpressive( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.impressiveAwards++;
    s_profileState.dirty = qtrue;

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_IMPRESSIVE );
}

void G_Profile_RecordAccuracy( gclient_t *client, int accuracyPercent ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( accuracyPercent < 75 ) {
        return;
    }

    s_profileState.stats.accuracyAwards++;
    s_profileState.dirty = qtrue;

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_ACCURACY );
}

void G_Profile_RecordPerfect( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.perfectAwards++;
    s_profileState.dirty = qtrue;

    G_Profile_CheckAchievementProgress( client, BG_ACHIEVEMENT_PERFECT );
}

/* ── Public parse helpers used by g_main.c to read profile JSON ── */
int G_Profile_ParseIntPublic( const char *buffer, const char *key, int defaultValue ) {
    return G_Profile_ParseInt( buffer, key, defaultValue );
}

double G_Profile_ParseDoublePublic( const char *buffer, const char *key, double defaultValue ) {
    return G_Profile_ParseDouble( buffer, key, defaultValue );
}

void G_Profile_ParseStringPublic( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue ) {
    G_Profile_ParseString( buffer, key, out, outSize, defaultValue );
}
