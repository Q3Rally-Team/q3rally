/*
==========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2021 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
==========================================================================
*/
// sv_ladder.c -- ladder upload interface

#include "server.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <float.h>
#endif

#define LADDER_SPOOL_LIMIT_DEFAULT       8
#define LADDER_SPOOL_LIMIT_MIN           1
#define LADDER_SPOOL_LIMIT_MAX           64
#define LADDER_JSON_INITIAL_CAPACITY    (16 * 1024)
#define LADDER_BACKOFF_BASE_MS          5000
#define LADDER_BACKOFF_MAX_MS           300000
#define LADDER_RESPONSE_LIMIT           512
#define LADDER_SPOOL_MAX_SIZE           (512 * 1024)
#define LADDER_MATCH_ID_FALLBACK        "pending"

#ifdef USE_CURL
#ifdef USE_LOCAL_HEADERS
#include "../curl-7.54.0/include/curl/curl.h"
#else
#include <curl/curl.h>
#endif
#ifdef USE_CURL_DLOPEN
#include "../sys/sys_loadlib.h"
#endif
#endif

typedef struct ladderJsonBuilder_s {
        char            *data;
        size_t  length;
        size_t  capacity;
} ladderJsonBuilder_t;

typedef struct ladderRequest_s {
        struct ladderRequest_s *next;
        ladderMatchPayload_t    payload;
        char            *json;
        size_t  jsonLength;
        char            matchId[LADDER_MAX_MATCH_ID];
        char            spoolPath[MAX_OSPATH];
        char            spoolName[64];
        int             attempt;
        int             nextAttemptTime;
#ifdef USE_CURL
        CURL            *easy;
        struct curl_slist *headers;
        char            errorBuffer[CURL_ERROR_SIZE];
        char            response[LADDER_RESPONSE_LIMIT];
        size_t  responseLength;
#endif
} ladderRequest_t;

typedef struct {
        qboolean        initialized;
        qboolean        spoolReady;
        qboolean        warnedNoCurl;
        qboolean        warnedNoUrl;
        int             maxQueue;
        int             queueSize;
        int             nextFileId;
        char            spoolDirectory[MAX_OSPATH];
        ladderRequest_t *head;
        ladderRequest_t *tail;
        ladderRequest_t *active;
#ifdef USE_CURL
        qboolean        curlLoaded;
        CURLM           *multi;
#endif
} svLadderState_t;

static svLadderState_t sv_ladder;

#ifdef USE_CURL
#ifdef USE_CURL_DLOPEN
static void *sv_curlLib;
static CURL* (*sv_curl_easy_init)(void);
static CURLcode (*sv_curl_easy_setopt)(CURL *curl, CURLoption option, ...);
static CURLcode (*sv_curl_easy_getinfo)(CURL *curl, CURLINFO info, ...);
static void (*sv_curl_easy_cleanup)(CURL *curl);
static const char *(*sv_curl_easy_strerror)(CURLcode errornum);
static CURLM* (*sv_curl_multi_init)(void);
static CURLMcode (*sv_curl_multi_add_handle)(CURLM *multi_handle, CURL *curl_handle);
static CURLMcode (*sv_curl_multi_remove_handle)(CURLM *multi_handle, CURL *curl_handle);
static CURLMcode (*sv_curl_multi_perform)(CURLM *multi_handle, int *running_handles);
static CURLMcode (*sv_curl_multi_cleanup)(CURLM *multi_handle);
static CURLMsg *(*sv_curl_multi_info_read)(CURLM *multi_handle, int *msgs_in_queue);
static const char *(*sv_curl_multi_strerror)(CURLMcode errornum);
static struct curl_slist *(*sv_curl_slist_append)(struct curl_slist *list, const char *string);
static void (*sv_curl_slist_free_all)(struct curl_slist *list);
#else
#define sv_curl_easy_init curl_easy_init
#define sv_curl_easy_setopt curl_easy_setopt
#define sv_curl_easy_getinfo curl_easy_getinfo
#define sv_curl_easy_cleanup curl_easy_cleanup
#define sv_curl_easy_strerror curl_easy_strerror
#define sv_curl_multi_init curl_multi_init
#define sv_curl_multi_add_handle curl_multi_add_handle
#define sv_curl_multi_remove_handle curl_multi_remove_handle
#define sv_curl_multi_perform curl_multi_perform
#define sv_curl_multi_cleanup curl_multi_cleanup
#define sv_curl_multi_info_read curl_multi_info_read
#define sv_curl_multi_strerror curl_multi_strerror
#define sv_curl_slist_append curl_slist_append
#define sv_curl_slist_free_all curl_slist_free_all
#endif
#endif

static int SV_LadderClampQueueLimit( int value ) {
        if ( value < LADDER_SPOOL_LIMIT_MIN ) {
                return LADDER_SPOOL_LIMIT_MIN;
        }
        if ( value > LADDER_SPOOL_LIMIT_MAX ) {
                return LADDER_SPOOL_LIMIT_MAX;
        }
        return value;
}

static void SV_LadderRefreshQueueLimit( void ) {
        int requested = LADDER_SPOOL_LIMIT_DEFAULT;

        if ( sv_telemetryMaxBatch ) {
                requested = sv_telemetryMaxBatch->integer;
        }

        sv_ladder.maxQueue = SV_LadderClampQueueLimit( requested );
}

static qboolean SV_LadderJsonInit( ladderJsonBuilder_t *builder, size_t capacity ) {
        builder->length = 0;
        builder->capacity = capacity;
        builder->data = Z_Malloc( builder->capacity );
        if ( !builder->data ) {
                builder->capacity = 0;
                return qfalse;
        }
        builder->data[0] = '\0';
        return qtrue;
}

static qboolean SV_LadderJsonEnsure( ladderJsonBuilder_t *builder, size_t extra ) {
        size_t required;
        char *newData;

        if ( !builder->data ) {
                return qfalse;
        }

        required = builder->length + extra + 1;
        if ( required <= builder->capacity ) {
                return qtrue;
        }

        while ( builder->capacity < required ) {
                builder->capacity *= 2;
        }

        newData = Z_Malloc( builder->capacity );
        if ( !newData ) {
                return qfalse;
        }

        Com_Memcpy( newData, builder->data, builder->length + 1 );
        Z_Free( builder->data );
        builder->data = newData;
        return qtrue;
}

static qboolean SV_LadderJsonAppendRaw( ladderJsonBuilder_t *builder, const char *text, size_t length ) {
        if ( !SV_LadderJsonEnsure( builder, length ) ) {
                return qfalse;
        }

        Com_Memcpy( builder->data + builder->length, text, length );
        builder->length += length;
        builder->data[builder->length] = '\0';
        return qtrue;
}

static qboolean SV_LadderJsonAppendChar( ladderJsonBuilder_t *builder, char ch ) {
        if ( !SV_LadderJsonEnsure( builder, 1 ) ) {
                return qfalse;
        }

        builder->data[builder->length++] = ch;
        builder->data[builder->length] = '\0';
        return qtrue;
}

static qboolean SV_LadderJsonAppendString( ladderJsonBuilder_t *builder, const char *text ) {
        const unsigned char *cursor;

        if ( !SV_LadderJsonAppendChar( builder, '\"' ) ) {
                return qfalse;
        }

        if ( !text ) {
                text = "";
        }

        cursor = (const unsigned char *)text;
        while ( *cursor ) {
                unsigned char c = *cursor++;

                switch ( c ) {
                case '\\':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\\\", 2 ) ) {
                                return qfalse;
                        }
                        break;
                case '\"':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\\"", 2 ) ) {
                                return qfalse;
                        }
                        break;
                case '\b':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\b", 2 ) ) {
                                return qfalse;
                        }
                        break;
                case '\f':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\f", 2 ) ) {
                                return qfalse;
                        }
                        break;
                case '\n':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\n", 2 ) ) {
                                return qfalse;
                        }
                        break;
                case '\r':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\r", 2 ) ) {
                                return qfalse;
                        }
                        break;
                case '\t':
                        if ( !SV_LadderJsonAppendRaw( builder, "\\t", 2 ) ) {
                                return qfalse;
                        }
                        break;
                default:
                        if ( c < 0x20 ) {
                                char buffer[7];
                                Com_sprintf( buffer, sizeof( buffer ), "\\u%04x", c );
                                if ( !SV_LadderJsonAppendRaw( builder, buffer, strlen( buffer ) ) ) {
                                        return qfalse;
                                }
                        } else {
                                if ( !SV_LadderJsonEnsure( builder, 1 ) ) {
                                        return qfalse;
                                }
                                builder->data[builder->length++] = (char)c;
                        }
                        break;
                }
        }

        if ( !SV_LadderJsonAppendChar( builder, '\"' ) ) {
                return qfalse;
        }

        return qtrue;
}

static qboolean SV_LadderJsonAppendInt( ladderJsonBuilder_t *builder, int value ) {
        char buffer[32];

        Com_sprintf( buffer, sizeof( buffer ), "%d", value );
        return SV_LadderJsonAppendRaw( builder, buffer, strlen( buffer ) );
}

static qboolean SV_LadderIsFinite( float value ) {
#if defined(_MSC_VER)
        return _finite( value ) != 0;
#else
        return isfinite( value );
#endif
}

static qboolean SV_LadderJsonAppendFloat( ladderJsonBuilder_t *builder, float value ) {
        char buffer[48];

        if ( Q_isnan( value ) || !SV_LadderIsFinite( value ) ) {
                value = 0.0f;
        }

        Com_sprintf( buffer, sizeof( buffer ), "%.3f", value );
        return SV_LadderJsonAppendRaw( builder, buffer, strlen( buffer ) );
}

static qboolean SV_LadderJsonAppendBoolean( ladderJsonBuilder_t *builder, qboolean value ) {
        if ( value ) {
                return SV_LadderJsonAppendRaw( builder, "true", 4 );
        }
        return SV_LadderJsonAppendRaw( builder, "false", 5 );
}

static const char *SV_LadderTeamName( int team ) {
        switch ( team ) {
        case TEAM_RED:
                return "red";
        case TEAM_BLUE:
                return "blue";
        case TEAM_SPECTATOR:
                return "spectator";
        default:
                break;
        }

        return "free";
}

static qboolean SV_LadderJsonAppendKey( ladderJsonBuilder_t *builder, const char *key, qboolean *first ) {
        if ( !SV_LadderJsonEnsure( builder, 1 ) ) {
                return qfalse;
        }

        if ( !*first ) {
                if ( !SV_LadderJsonAppendChar( builder, ',' ) ) {
                        return qfalse;
                }
        } else {
                *first = qfalse;
        }

        if ( !SV_LadderJsonAppendString( builder, key ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendChar( builder, ':' ) ) {
                return qfalse;
        }
        return qtrue;
}

static qboolean SV_LadderJsonAppendTeamArray( ladderJsonBuilder_t *builder, const int *values, int count ) {
        int i;

        if ( !SV_LadderJsonAppendChar( builder, '[' ) ) {
                return qfalse;
        }
        for ( i = 0; i < count; ++i ) {
                if ( i > 0 ) {
                        if ( !SV_LadderJsonAppendChar( builder, ',' ) ) {
                                return qfalse;
                        }
                }
                if ( !SV_LadderJsonAppendInt( builder, values[i] ) ) {
                        return qfalse;
                }
        }
        if ( !SV_LadderJsonAppendChar( builder, ']' ) ) {
                return qfalse;
        }
        return qtrue;
}

static qboolean SV_LadderJsonAppendLapArray( ladderJsonBuilder_t *builder, const ladderPlayerPayload_t *player ) {
        int count = player->lapCount;
        int i;

        if ( count < 0 ) {
                count = 0;
        }
        if ( count > RACE_MAX_RECORDED_LAPS ) {
                count = RACE_MAX_RECORDED_LAPS;
        }

        if ( !SV_LadderJsonAppendChar( builder, '[' ) ) {
                return qfalse;
        }
        for ( i = 0; i < count; ++i ) {
                if ( i > 0 ) {
                        if ( !SV_LadderJsonAppendChar( builder, ',' ) ) {
                                return qfalse;
                        }
                }
                if ( !SV_LadderJsonAppendInt( builder, player->lapTimes[i] ) ) {
                        return qfalse;
                }
        }
        if ( !SV_LadderJsonAppendChar( builder, ']' ) ) {
                return qfalse;
        }
        return qtrue;
}

static qboolean SV_LadderJsonAppendPlayer( ladderJsonBuilder_t *builder, const ladderPlayerPayload_t *player ) {
        qboolean first = qtrue;
        const char *teamName = SV_LadderTeamName( player->team );

        if ( !SV_LadderJsonAppendChar( builder, '{' ) ) {
                return qfalse;
        }

        if ( !SV_LadderJsonAppendKey( builder, "clientNum", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->clientNum ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "playerId", &first ) ||
             !SV_LadderJsonAppendString( builder, player->playerId ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "guid", &first ) ||
             !SV_LadderJsonAppendString( builder, player->guid ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "name", &first ) ||
             !SV_LadderJsonAppendString( builder, player->name ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "cleanName", &first ) ||
             !SV_LadderJsonAppendString( builder, player->cleanName ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "displayName", &first ) ||
             !SV_LadderJsonAppendString( builder, player->cleanName ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "model", &first ) ||
             !SV_LadderJsonAppendString( builder, player->model ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "vehicle", &first ) ||
             !SV_LadderJsonAppendString( builder, player->vehicle ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "team", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->team ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "teamName", &first ) ||
             !SV_LadderJsonAppendString( builder, teamName ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "isBot", &first ) ||
             !SV_LadderJsonAppendBoolean( builder, player->isBot ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "score", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->score ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "rawScore", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->score ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "playerScore", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->score ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "ping", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->ping ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "time", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->time ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "scoreFlags", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->scoreFlags ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "powerUps", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->powerUps ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "accuracy", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->accuracy ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "impressiveCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->impressiveCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "impressiveTelefragCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->impressiveTelefragCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "excellentCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->excellentCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "gauntletCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->gauntletCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "defendCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->defendCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "assistCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->assistCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "perfect", &first ) ||
             !SV_LadderJsonAppendBoolean( builder, player->perfect ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "captures", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->captures ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "damageDealt", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->damageDealt ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "damageTaken", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->damageTaken ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "position", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->position ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "bestLapMs", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->bestLapMs ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "totalRaceMs", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->totalRaceMs ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "lapCount", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->lapCount ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "lapTimes", &first ) ||
             !SV_LadderJsonAppendLapArray( builder, player ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "kills", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->kills ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "deaths", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->deaths ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "zoneHoldMs", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->zoneHoldMs ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "zoneActiveSigil", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->zoneActiveSigil ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "survivalMs", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->survivalMs ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "eliminationRound", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->eliminationRound ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "eliminationPlayersRemaining", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->eliminationPlayersRemaining ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "eliminationMetric", &first ) ||
             !SV_LadderJsonAppendFloat( builder, player->eliminationMetric ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "finishRaceTime", &first ) ||
             !SV_LadderJsonAppendInt( builder, player->finishRaceTime ) ) {
                return qfalse;
        }
        if ( !SV_LadderJsonAppendKey( builder, "kdRatio", &first ) ||
             !SV_LadderJsonAppendFloat( builder, player->kdRatio ) ) {
                return qfalse;
        }

        if ( !SV_LadderJsonAppendChar( builder, '}' ) ) {
                return qfalse;
        }

        return qtrue;
}

static char *SV_LadderSerializeMatch( const ladderMatchPayload_t *payload, size_t *lengthOut ) {
        ladderJsonBuilder_t builder;
        qboolean first = qtrue;
        int i;

        if ( !payload || !payload->valid ) {
                return NULL;
        }

        if ( !SV_LadderJsonInit( &builder, LADDER_JSON_INITIAL_CAPACITY ) ) {
                return NULL;
        }

        if ( !SV_LadderJsonAppendChar( &builder, '{' ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "matchId", &first ) ||
             !SV_LadderJsonAppendString( &builder, payload->matchId ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "mode", &first ) ||
             !SV_LadderJsonAppendString( &builder, payload->mode ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "gametype", &first ) ||
             !SV_LadderJsonAppendInt( &builder, payload->gametype ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "map", &first ) ||
             !SV_LadderJsonAppendString( &builder, payload->mapName ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "startTime", &first ) ||
             !SV_LadderJsonAppendString( &builder, payload->startTimeIso ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendKey( &builder, "endTime", &first ) ||
             !SV_LadderJsonAppendString( &builder, payload->endTimeIso ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendKey( &builder, "duration", &first ) ||
             !SV_LadderJsonAppendString( &builder, payload->durationIso ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendKey( &builder, "startEpoch", &first ) ||
             !SV_LadderJsonAppendInt( &builder, payload->startEpoch ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendKey( &builder, "endEpoch", &first ) ||
             !SV_LadderJsonAppendInt( &builder, payload->endEpoch ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendKey( &builder, "durationSeconds", &first ) ||
             !SV_LadderJsonAppendInt( &builder, payload->durationSeconds ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "server", &first ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendChar( &builder, '{' ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        {
                qboolean serverFirst = qtrue;
                if ( !SV_LadderJsonAppendKey( &builder, "name", &serverFirst ) ||
                     !SV_LadderJsonAppendString( &builder, payload->serverName ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "host", &serverFirst ) ||
                     !SV_LadderJsonAppendString( &builder, payload->serverHost ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "build", &serverFirst ) ||
                     !SV_LadderJsonAppendString( &builder, payload->serverBuild ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
        }
        if ( !SV_LadderJsonAppendChar( &builder, '}' ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "settings", &first ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendChar( &builder, '{' ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        {
                qboolean settingsFirst = qtrue;
                if ( !SV_LadderJsonAppendKey( &builder, "g_gametype", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->gametype ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "levelStartTime", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->levelStartTime ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "levelEndTime", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->levelEndTime ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "raceStartTime", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->raceStartTime ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "raceEndTime", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->raceEndTime ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "finishRaceTime", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->finishRaceTime ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "winnerClientNum", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->winnerClientNum ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "numberOfLaps", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->numberOfLaps ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "trackReversed", &settingsFirst ) ||
                     !SV_LadderJsonAppendBoolean( &builder, payload->trackReversed ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "eliminationStartDelay", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->eliminationStartDelay ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "eliminationInterval", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->eliminationInterval ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "eliminationWarning", &settingsFirst ) ||
                     !SV_LadderJsonAppendInt( &builder, payload->eliminationWarning ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
        }
        if ( !SV_LadderJsonAppendChar( &builder, '}' ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( !SV_LadderJsonAppendKey( &builder, "teamScores", &first ) ||
             !SV_LadderJsonAppendTeamArray( &builder, payload->teamScores, TEAM_NUM_TEAMS ) ) {
                Z_Free( builder.data );
                return NULL;
        }
        if ( !SV_LadderJsonAppendKey( &builder, "teamTimes", &first ) ||
             !SV_LadderJsonAppendTeamArray( &builder, payload->teamTimes, TEAM_NUM_TEAMS ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        {
                int playerCount = payload->playerCount;
                if ( playerCount < 0 ) {
                        playerCount = 0;
                } else if ( playerCount > MAX_CLIENTS ) {
                        Com_Printf( "Ladder: clamping playerCount from %d to %d\n",
                                payload->playerCount, MAX_CLIENTS );
                        playerCount = MAX_CLIENTS;
                }
                if ( !SV_LadderJsonAppendKey( &builder, "playerCount", &first ) ||
                     !SV_LadderJsonAppendInt( &builder, playerCount ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }

                if ( !SV_LadderJsonAppendKey( &builder, "players", &first ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                if ( !SV_LadderJsonAppendChar( &builder, '[' ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
                for ( i = 0; i < playerCount; ++i ) {
                        if ( i > 0 ) {
                                if ( !SV_LadderJsonAppendChar( &builder, ',' ) ) {
                                        Z_Free( builder.data );
                                        return NULL;
                                }
                        }
                        if ( !SV_LadderJsonAppendPlayer( &builder, &payload->players[i] ) ) {
                                Z_Free( builder.data );
                                return NULL;
                        }
                }
                if ( !SV_LadderJsonAppendChar( &builder, ']' ) ) {
                        Z_Free( builder.data );
                        return NULL;
                }
        }

        if ( !SV_LadderJsonAppendChar( &builder, '}' ) ) {
                Z_Free( builder.data );
                return NULL;
        }

        if ( lengthOut ) {
                *lengthOut = builder.length;
        }
        return builder.data;
}

static void SV_LadderSanitizeComponent( const char *input, char *output, size_t outputSize ) {
        size_t written = 0;

        if ( outputSize == 0 ) {
                return;
        }

        if ( !input ) {
                input = LADDER_MATCH_ID_FALLBACK;
        }

        while ( *input && written + 1 < outputSize ) {
                char c = *input++;
                if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
                     ( c >= '0' && c <= '9' ) || c == '-' || c == '_' ) {
                        output[written++] = c;
                }
        }

        if ( written == 0 ) {
                Q_strncpyz( output, LADDER_MATCH_ID_FALLBACK, outputSize );
        } else {
                output[written] = '\0';
        }
}

static void SV_LadderBuildSpoolName( ladderRequest_t *request ) {
        char safeId[32];
        int id;

        SV_LadderSanitizeComponent( request->matchId, safeId, sizeof( safeId ) );
        id = sv_ladder.nextFileId++;
        Com_sprintf( request->spoolName, sizeof( request->spoolName ),
                "match-%08x-%02d-%s.json", Sys_Milliseconds(), id & 0xff, safeId );
}

static qboolean SV_LadderEnsureSpoolDirectory( void ) {
        char home[MAX_OSPATH];
        char path[MAX_OSPATH];
        const char *homePath;

        sv_ladder.spoolReady = qfalse;
        sv_ladder.spoolDirectory[0] = '\0';

        homePath = Cvar_VariableString( "fs_homepath" );
        if ( !homePath || !homePath[0] ) {
                        return qfalse;
        }

        Q_strncpyz( home, homePath, sizeof( home ) );

        Com_sprintf( path, sizeof( path ), "%s%ctelemetry", home, PATH_SEP );
        if ( !Sys_Mkdir( path ) ) {
                Com_Printf( "Ladder: failed to create directory '%s' (%s)\n", path, strerror( errno ) );
                return qfalse;
        }

        Com_sprintf( path, sizeof( path ), "%s%ctelemetry%coutbox", home, PATH_SEP, PATH_SEP );
        if ( !Sys_Mkdir( path ) ) {
                Com_Printf( "Ladder: failed to create directory '%s' (%s)\n", path, strerror( errno ) );
                return qfalse;
        }

        Q_strncpyz( sv_ladder.spoolDirectory, path, sizeof( sv_ladder.spoolDirectory ) );
        sv_ladder.spoolReady = qtrue;
        return qtrue;
}

static void SV_LadderExtractMatchIdFromJson( ladderRequest_t *request ) {
        const char *needle = "\"matchId\":\"";
        char *found;

        if ( !request->json ) {
                return;
        }

        found = strstr( request->json, needle );
        if ( found ) {
                char *cursor = found + strlen( needle );
                size_t length = 0;
                while ( *cursor && *cursor != '\"' && length + 1 < sizeof( request->matchId ) ) {
                        request->matchId[length++] = *cursor++;
                }
                request->matchId[length] = '\0';
        }
}

static qboolean SV_LadderWriteSpool( ladderRequest_t *request ) {
        char pathCopy[MAX_OSPATH];
        FILE *file;
        size_t written;

        if ( !sv_ladder.spoolReady || !request->json || !request->jsonLength ) {
                return qfalse;
        }

        if ( !request->spoolName[0] ) {
                SV_LadderBuildSpoolName( request );
        }

        Com_sprintf( request->spoolPath, sizeof( request->spoolPath ),
                "%s%c%s", sv_ladder.spoolDirectory, PATH_SEP, request->spoolName );

        Q_strncpyz( pathCopy, request->spoolPath, sizeof( pathCopy ) );
        if ( FS_CreatePath( pathCopy ) ) {
                Com_Printf( "Ladder: failed to prepare spool path '%s'\n", request->spoolPath );
                return qfalse;
        }

        file = Sys_FOpen( request->spoolPath, "wb" );
        if ( !file ) {
                Com_Printf( "Ladder: failed to open spool file '%s' (%s)\n", request->spoolPath, strerror( errno ) );
                return qfalse;
        }

        written = fwrite( request->json, 1, request->jsonLength, file );
        fclose( file );

        if ( written != request->jsonLength ) {
                Com_Printf( "Ladder: failed to write spool file '%s' (%s)\n", request->spoolPath, strerror( errno ) );
                return qfalse;
        }

        return qtrue;
}

static void SV_LadderQueuePush( ladderRequest_t *request ) {
        request->next = NULL;

        if ( !sv_ladder.tail ) {
                sv_ladder.head = request;
                sv_ladder.tail = request;
        } else {
                sv_ladder.tail->next = request;
                sv_ladder.tail = request;
        }
        sv_ladder.queueSize++;
}

static ladderRequest_t *SV_LadderQueuePop( void ) {
        ladderRequest_t *request = sv_ladder.head;

        if ( !request ) {
                return NULL;
        }

        sv_ladder.head = request->next;
        if ( !sv_ladder.head ) {
                sv_ladder.tail = NULL;
        }
        request->next = NULL;
        if ( sv_ladder.queueSize > 0 ) {
                sv_ladder.queueSize--;
        }
        return request;
}

static void SV_LadderFreeRequest( ladderRequest_t *request, qboolean keepSpool ) {
        if ( !request ) {
                return;
        }
#ifdef USE_CURL
        if ( request->headers ) {
                sv_curl_slist_free_all( request->headers );
                request->headers = NULL;
        }
        if ( request->easy ) {
                if ( sv_ladder.multi ) {
                        sv_curl_multi_remove_handle( sv_ladder.multi, request->easy );
                }
                sv_curl_easy_cleanup( request->easy );
                request->easy = NULL;
        }
#endif
        if ( request->json ) {
                Z_Free( request->json );
                request->json = NULL;
        }

        if ( request->spoolPath[0] && !keepSpool ) {
                if ( remove( request->spoolPath ) != 0 && errno != ENOENT ) {
                        Com_DPrintf( "Ladder: failed to remove spool file '%s' (%s)\n",
                                request->spoolPath, strerror( errno ) );
                }
        }

        Z_Free( request );
}

static ladderRequest_t *SV_LadderAllocRequest( void ) {
        ladderRequest_t *request = Z_Malloc( sizeof( ladderRequest_t ) );
        if ( request ) {
                Com_Memset( request, 0, sizeof( *request ) );
        }
        return request;
}

static void SV_LadderLoadSpool( void ) {
        char **files;
        int count;
        int loaded = 0;

        if ( !sv_ladder.spoolReady ) {
                return;
        }

        SV_LadderRefreshQueueLimit();

        files = Sys_ListFiles( sv_ladder.spoolDirectory, ".json", NULL, &count, qfalse );
        if ( !files || count <= 0 ) {
                if ( files ) {
                        Sys_FreeFileList( files );
                }
                return;
        }

        qsort( files, count, sizeof( char * ), (int (*)( const void *, const void * ))strcmp );

        for ( int i = 0; i < count; ++i ) {
                char path[MAX_OSPATH];
                FILE *file;
                long size;
                ladderRequest_t *request;

                if ( !files[i] || !files[i][0] ) {
                        continue;
                }

                if ( sv_ladder.queueSize + ( sv_ladder.active ? 1 : 0 ) >= sv_ladder.maxQueue ) {
                        break;
                }

                if ( files[i][0] == '.' ) {
                        continue;
                }

                request = SV_LadderAllocRequest();
                if ( !request ) {
                        break;
                }

                Q_strncpyz( request->spoolName, files[i], sizeof( request->spoolName ) );
                Com_sprintf( path, sizeof( path ), "%s%c%s", sv_ladder.spoolDirectory, PATH_SEP, files[i] );
                Q_strncpyz( request->spoolPath, path, sizeof( request->spoolPath ) );

                file = Sys_FOpen( path, "rb" );
                if ( !file ) {
                        Com_Printf( "Ladder: unable to open spool file '%s' (%s)\n", path, strerror( errno ) );
                        SV_LadderFreeRequest( request, qfalse );
                        continue;
                }

                if ( fseek( file, 0, SEEK_END ) != 0 ) {
                        fclose( file );
                        SV_LadderFreeRequest( request, qfalse );
                        continue;
                }

                size = ftell( file );
                if ( size <= 0 || size > LADDER_SPOOL_MAX_SIZE ) {
                        fclose( file );
                        Com_Printf( "Ladder: skipping spool file '%s' (invalid size)\n", path );
                        SV_LadderFreeRequest( request, qfalse );
                        continue;
                }

                if ( fseek( file, 0, SEEK_SET ) != 0 ) {
                        fclose( file );
                        SV_LadderFreeRequest( request, qfalse );
                        continue;
                }

                request->json = Z_Malloc( size + 1 );
                if ( !request->json ) {
                        fclose( file );
                        SV_LadderFreeRequest( request, qfalse );
                        break;
                }

                request->jsonLength = fread( request->json, 1, size, file );
                fclose( file );

                if ( request->jsonLength != (size_t)size ) {
                        SV_LadderFreeRequest( request, qfalse );
                        continue;
                }
                request->json[request->jsonLength] = '\0';

                SV_LadderExtractMatchIdFromJson( request );

                SV_LadderQueuePush( request );
                loaded++;
        }

        Sys_FreeFileList( files );

        sv_ladder.nextFileId += loaded;

        if ( loaded > 0 ) {
                Com_Printf( "Ladder: queued %d stored match%s from spool\n",
                        loaded, loaded == 1 ? "" : "es" );
        }
}

#ifdef USE_CURL
static int SV_LadderComputeBackoff( int attempt ) {
        int delay = LADDER_BACKOFF_BASE_MS;
        int i;

        if ( attempt <= 1 ) {
                return delay;
        }

        for ( i = 1; i < attempt; ++i ) {
                if ( delay >= LADDER_BACKOFF_MAX_MS / 2 ) {
                        delay = LADDER_BACKOFF_MAX_MS;
                        break;
                }
                delay *= 2;
        }

        if ( delay > LADDER_BACKOFF_MAX_MS ) {
                delay = LADDER_BACKOFF_MAX_MS;
        }
        return delay;
}

#endif

#ifdef USE_CURL
#ifdef USE_CURL_DLOPEN
#ifdef WIN32
#define LADDER_DEFAULT_CURL_LIB "libcurl-4.dll"
#define LADDER_ALTERNATE_CURL_LIB "libcurl-3.dll"
#elif defined(__APPLE__)
#define LADDER_DEFAULT_CURL_LIB "libcurl.dylib"
#else
#define LADDER_DEFAULT_CURL_LIB "libcurl.so.4"
#define LADDER_ALTERNATE_CURL_LIB "libcurl.so.3"
#endif

static void *SV_LadderGPA( const char *symbol ) {
        void *func = Sys_LoadFunction( sv_curlLib, symbol );
        if ( !func ) {
                Com_Printf( "Ladder: missing curl symbol %s\n", symbol );
                return NULL;
        }
        return func;
}

static qboolean SV_LadderLoadCurlLibrary( void ) {
        if ( sv_ladder.curlLoaded ) {
                return qtrue;
        }

        if ( !sv_curlLib ) {
                sv_curlLib = Sys_LoadDll( LADDER_DEFAULT_CURL_LIB, qtrue );
#ifdef LADDER_ALTERNATE_CURL_LIB
                if ( !sv_curlLib ) {
                        sv_curlLib = Sys_LoadDll( LADDER_ALTERNATE_CURL_LIB, qtrue );
                }
#endif
                if ( !sv_curlLib ) {
                        if ( !sv_ladder.warnedNoCurl ) {
                                Com_Printf( "Ladder: unable to load %s\n", LADDER_DEFAULT_CURL_LIB );
                                sv_ladder.warnedNoCurl = qtrue;
                        }
                        return qfalse;
                }
        }

        sv_curl_easy_init = SV_LadderGPA( "curl_easy_init" );
        sv_curl_easy_setopt = SV_LadderGPA( "curl_easy_setopt" );
        sv_curl_easy_getinfo = SV_LadderGPA( "curl_easy_getinfo" );
        sv_curl_easy_cleanup = SV_LadderGPA( "curl_easy_cleanup" );
        sv_curl_easy_strerror = SV_LadderGPA( "curl_easy_strerror" );
        sv_curl_multi_init = SV_LadderGPA( "curl_multi_init" );
        sv_curl_multi_add_handle = SV_LadderGPA( "curl_multi_add_handle" );
        sv_curl_multi_remove_handle = SV_LadderGPA( "curl_multi_remove_handle" );
        sv_curl_multi_perform = SV_LadderGPA( "curl_multi_perform" );
        sv_curl_multi_cleanup = SV_LadderGPA( "curl_multi_cleanup" );
        sv_curl_multi_info_read = SV_LadderGPA( "curl_multi_info_read" );
        sv_curl_multi_strerror = SV_LadderGPA( "curl_multi_strerror" );
        sv_curl_slist_append = SV_LadderGPA( "curl_slist_append" );
        sv_curl_slist_free_all = SV_LadderGPA( "curl_slist_free_all" );

        if ( !sv_curl_easy_init || !sv_curl_easy_setopt || !sv_curl_easy_getinfo ||
             !sv_curl_easy_cleanup || !sv_curl_easy_strerror || !sv_curl_multi_init ||
             !sv_curl_multi_add_handle || !sv_curl_multi_remove_handle ||
             !sv_curl_multi_perform || !sv_curl_multi_cleanup || !sv_curl_multi_info_read ||
             !sv_curl_multi_strerror || !sv_curl_slist_append || !sv_curl_slist_free_all ) {
                Com_Printf( "Ladder: failed to resolve required curl symbols\n" );
                sv_ladder.warnedNoCurl = qtrue;
                return qfalse;
        }

        sv_ladder.curlLoaded = qtrue;
        sv_ladder.warnedNoCurl = qfalse;
        return qtrue;
}
#else
static qboolean SV_LadderLoadCurlLibrary( void ) {
        sv_ladder.curlLoaded = qtrue;
        return qtrue;
}
#endif

static qboolean SV_LadderEnsureCurl( void ) {
        if ( !SV_LadderLoadCurlLibrary() ) {
                return qfalse;
        }

        if ( !sv_ladder.multi ) {
                sv_ladder.multi = sv_curl_multi_init();
                if ( !sv_ladder.multi ) {
                        Com_Printf( "Ladder: failed to initialize curl multi interface\n" );
                        return qfalse;
                }
        }

        return qtrue;
}

static void SV_LadderShutdownCurl( void ) {
        if ( sv_ladder.multi ) {
                sv_curl_multi_cleanup( sv_ladder.multi );
                sv_ladder.multi = NULL;
        }

#ifdef USE_CURL_DLOPEN
        if ( sv_curlLib ) {
                Sys_UnloadLibrary( sv_curlLib );
                sv_curlLib = NULL;
        }
        sv_curl_easy_init = NULL;
        sv_curl_easy_setopt = NULL;
        sv_curl_easy_getinfo = NULL;
        sv_curl_easy_cleanup = NULL;
        sv_curl_easy_strerror = NULL;
        sv_curl_multi_init = NULL;
        sv_curl_multi_add_handle = NULL;
        sv_curl_multi_remove_handle = NULL;
        sv_curl_multi_perform = NULL;
        sv_curl_multi_cleanup = NULL;
        sv_curl_multi_info_read = NULL;
        sv_curl_multi_strerror = NULL;
        sv_curl_slist_append = NULL;
        sv_curl_slist_free_all = NULL;
#endif

        sv_ladder.curlLoaded = qfalse;
}

static size_t SV_LadderCurlWriteCallback( void *buffer, size_t size, size_t nmemb, void *userdata ) {
        ladderRequest_t *request = (ladderRequest_t *)userdata;
        size_t bytes = size * nmemb;
        size_t remaining;
        size_t toCopy;

        if ( !request ) {
                return bytes;
        }

        if ( request->responseLength >= sizeof( request->response ) - 1 ) {
                return bytes;
        }

        remaining = sizeof( request->response ) - 1 - request->responseLength;
        toCopy = bytes;
        if ( toCopy > remaining ) {
                toCopy = remaining;
        }

        if ( toCopy > 0 ) {
                Com_Memcpy( request->response + request->responseLength, buffer, toCopy );
                request->responseLength += toCopy;
                request->response[request->responseLength] = '\0';
        }

        return bytes;
}

static qboolean SV_LadderStartRequest( ladderRequest_t *request ) {
        CURL *easy;
        CURLcode code;
        struct curl_slist *headers = NULL;
        char authHeader[256];
        const char *url = sv_ladderUrl ? sv_ladderUrl->string : "";
        const char *apiKey = sv_ladderApiKey ? sv_ladderApiKey->string : "";

        if ( !url || !url[0] ) {
                return qfalse;
        }

        if ( !SV_LadderEnsureCurl() ) {
                return qfalse;
        }

        easy = sv_curl_easy_init();
        if ( !easy ) {
                return qfalse;
        }

        request->easy = easy;
        request->errorBuffer[0] = '\0';
        request->response[0] = '\0';
        request->responseLength = 0;

        code = sv_curl_easy_setopt( easy, CURLOPT_URL, url );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_POST, 1L );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_POSTFIELDS, request->json );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_POSTFIELDSIZE, (long)request->jsonLength );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_ERRORBUFFER, request->errorBuffer );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_WRITEFUNCTION, SV_LadderCurlWriteCallback );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_WRITEDATA, request );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_FOLLOWLOCATION, 1L );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_MAXREDIRS, 5L );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_CONNECTTIMEOUT, 10L );
        if ( code != CURLE_OK ) {
                        return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_TIMEOUT, 30L );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_NOSIGNAL, 1L );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        code = sv_curl_easy_setopt( easy, CURLOPT_USERAGENT, Q3_VERSION );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        headers = sv_curl_slist_append( headers, "Content-Type: application/json" );
        headers = sv_curl_slist_append( headers, "Accept: application/json" );

        if ( apiKey && apiKey[0] ) {
                Com_sprintf( authHeader, sizeof( authHeader ), "Authorization: Bearer %s", apiKey );
                headers = sv_curl_slist_append( headers, authHeader );
        }

        if ( headers ) {
                code = sv_curl_easy_setopt( easy, CURLOPT_HTTPHEADER, headers );
                if ( code != CURLE_OK ) {
                        sv_curl_slist_free_all( headers );
                        return qfalse;
                }
        }

        request->headers = headers;

        code = sv_curl_easy_setopt( easy, CURLOPT_PRIVATE, request );
        if ( code != CURLE_OK ) {
                return qfalse;
        }

        if ( sv_curl_multi_add_handle( sv_ladder.multi, easy ) != CURLM_OK ) {
                return qfalse;
        }

        return qtrue;
}

static void SV_LadderHandleFailure( ladderRequest_t *request, const char *reason, long responseCode, qboolean retry ) {
        if ( retry ) {
                int delay = SV_LadderComputeBackoff( ++request->attempt );
                request->nextAttemptTime = Sys_Milliseconds() + delay;
                Com_Printf( "Ladder: upload failed for match %s (%s, HTTP %ld), retrying in %.1f s\n",
                        request->matchId[0] ? request->matchId : "<unknown>",
                        reason ? reason : "error", responseCode, delay / 1000.0f );
                SV_LadderQueuePush( request );
        } else {
                Com_Printf( "Ladder: upload dropped for match %s (%s, HTTP %ld)\n",
                        request->matchId[0] ? request->matchId : "<unknown>",
                        reason ? reason : "error", responseCode );
                SV_LadderFreeRequest( request, qfalse );
        }
}

static void SV_LadderCompleteRequest( ladderRequest_t *request, CURLcode result, long responseCode ) {
        const char *errorMsg = NULL;
        qboolean retry = qfalse;

        if ( result == CURLE_OK && responseCode >= 200 && responseCode < 300 ) {
                Com_Printf( "Ladder: uploaded match %s (HTTP %ld)\n",
                        request->matchId[0] ? request->matchId : "<unknown>", responseCode );
                SV_LadderFreeRequest( request, qfalse );
                return;
        }

        if ( result != CURLE_OK ) {
                errorMsg = request->errorBuffer[0] ? request->errorBuffer : sv_curl_easy_strerror( result );
                retry = qtrue;
        } else if ( responseCode >= 500 || responseCode == 0 ) {
                errorMsg = request->response[0] ? request->response : "server error";
                retry = qtrue;
        } else {
                errorMsg = request->response[0] ? request->response : "client error";
                retry = qfalse;
        }

        SV_LadderHandleFailure( request, errorMsg, responseCode, retry );
}

#endif

#ifdef USE_CURL
static void SV_LadderActivateNext( void ) {
        int pending;

        if ( sv_ladder.active ) {
                return;
        }

        if ( !sv_ladder.multi ) {
                if ( !SV_LadderEnsureCurl() ) {
                        return;
                }
        }

        pending = sv_ladder.queueSize;
        while ( pending-- > 0 ) {
                ladderRequest_t *request = SV_LadderQueuePop();
                if ( !request ) {
                        break;
                }

                if ( request->nextAttemptTime > Sys_Milliseconds() ) {
                        SV_LadderQueuePush( request );
                        continue;
                }

                if ( SV_LadderStartRequest( request ) ) {
                        sv_ladder.active = request;
                        return;
                }

                request->nextAttemptTime = Sys_Milliseconds() + SV_LadderComputeBackoff( ++request->attempt );
                SV_LadderQueuePush( request );
        }
}

static void SV_LadderPollActive( void ) {
        CURLMsg *msg;
        int messages;

        if ( !sv_ladder.multi ) {
                return;
        }

        if ( sv_ladder.active ) {
                int running = 0;
                CURLMcode mcode = sv_curl_multi_perform( sv_ladder.multi, &running );
                if ( mcode != CURLM_OK && !sv_ladder.warnedNoCurl ) {
                        Com_Printf( "Ladder: curl_multi_perform failed: %s\n", sv_curl_multi_strerror( mcode ) );
                        sv_ladder.warnedNoCurl = qtrue;
                }
        }

        while ( ( msg = sv_curl_multi_info_read( sv_ladder.multi, &messages ) ) != NULL ) {
                if ( msg->msg == CURLMSG_DONE ) {
                        ladderRequest_t *request = NULL;
                        long responseCode = 0;
                        CURL *easy = msg->easy_handle;

                        sv_curl_easy_getinfo( easy, CURLINFO_PRIVATE, (char **)&request );
                        sv_curl_easy_getinfo( easy, CURLINFO_RESPONSE_CODE, &responseCode );

                        sv_curl_multi_remove_handle( sv_ladder.multi, easy );
                        sv_curl_easy_cleanup( easy );

                        if ( request ) {
                                if ( sv_ladder.active == request ) {
                                        sv_ladder.active = NULL;
                                }
                                request->easy = NULL;
                                if ( request->headers ) {
                                        sv_curl_slist_free_all( request->headers );
                                        request->headers = NULL;
                                }
                                SV_LadderCompleteRequest( request, msg->data.result, responseCode );
                        }
                }
        }
}
#endif

static void SV_LadderClearQueue( qboolean keepSpool ) {
        ladderRequest_t *request;

        while ( ( request = SV_LadderQueuePop() ) != NULL ) {
                SV_LadderFreeRequest( request, keepSpool );
        }
}

static void SV_LadderResetState( void ) {
        sv_ladder.queueSize = 0;
        sv_ladder.head = NULL;
        sv_ladder.tail = NULL;
        sv_ladder.active = NULL;
        sv_ladder.warnedNoCurl = qfalse;
        sv_ladder.warnedNoUrl = qfalse;
        sv_ladder.nextFileId = 1;
        SV_LadderRefreshQueueLimit();
}

void SV_LadderInit( void ) {
        if ( sv_ladder.initialized ) {
                SV_LadderShutdown();
        }

        Com_Memset( &sv_ladder, 0, sizeof( sv_ladder ) );
        SV_LadderRefreshQueueLimit();
        sv_ladder.initialized = qtrue;

        if ( SV_LadderEnsureSpoolDirectory() ) {
                SV_LadderLoadSpool();
        }
}

void SV_LadderShutdown( void ) {
        if ( !sv_ladder.initialized ) {
                return;
        }

#ifdef USE_CURL
        if ( sv_ladder.active ) {
                SV_LadderFreeRequest( sv_ladder.active, qtrue );
                sv_ladder.active = NULL;
        }
#endif
        SV_LadderClearQueue( qtrue );
#ifdef USE_CURL
        SV_LadderShutdownCurl();
#endif
        sv_ladder.initialized = qfalse;
        SV_LadderResetState();
}

void SV_LadderSubmit( const ladderMatchPayload_t *payload ) {
        ladderRequest_t *request;
        char *json;
        size_t length = 0;
        int total;

        if ( !sv_ladder.initialized ) {
                SV_LadderInit();
        }

        if ( !sv_ladder.initialized ) {
                return;
        }

        if ( !sv_ladderEnabled || !sv_ladderEnabled->integer ) {
                return;
        }

        if ( !payload || !payload->valid ) {
                return;
        }

        if ( sv_ladderUrl && sv_ladderUrl->string[0] ) {
                sv_ladder.warnedNoUrl = qfalse;
        } else {
                if ( !sv_ladder.warnedNoUrl ) {
                        Com_Printf( "Ladder: sv_ladderUrl is not set, dropping payload\n" );
                        sv_ladder.warnedNoUrl = qtrue;
                }
                return;
        }

        SV_LadderRefreshQueueLimit();

        total = sv_ladder.queueSize + ( sv_ladder.active ? 1 : 0 );
        if ( total >= sv_ladder.maxQueue ) {
                Com_Printf( "Ladder: queue full, dropping match %s\n",
                        payload->matchId[0] ? payload->matchId : "<unknown>" );
                return;
        }

        json = SV_LadderSerializeMatch( payload, &length );
        if ( !json || !length ) {
                        Com_Printf( "Ladder: failed to serialize match payload\n" );
                if ( json ) {
                        Z_Free( json );
                }
                return;
        }

        request = SV_LadderAllocRequest();
        if ( !request ) {
                Z_Free( json );
                return;
        }

        Com_Memcpy( &request->payload, payload, sizeof( *payload ) );
        request->json = json;
        request->jsonLength = length;
        Q_strncpyz( request->matchId, payload->matchId, sizeof( request->matchId ) );
        request->nextAttemptTime = 0;
        request->attempt = 0;

        if ( sv_ladder.spoolReady ) {
                if ( !SV_LadderWriteSpool( request ) ) {
                        request->spoolName[0] = '\0';
                        request->spoolPath[0] = '\0';
                }
        }

#ifndef USE_CURL
        if ( !sv_ladder.warnedNoCurl ) {
                Com_Printf( "Ladder: built without libcurl support, dropping match %s\n",
                        payload->matchId[0] ? payload->matchId : "<unknown>" );
                sv_ladder.warnedNoCurl = qtrue;
        }
        SV_LadderFreeRequest( request, qfalse );
        return;
#else
        SV_LadderQueuePush( request );
        SV_LadderActivateNext();
#endif
}

void SV_LadderFrame( void ) {
        if ( !sv_ladderEnabled || !sv_ladderEnabled->integer ) {
                return;
        }

        if ( !sv_ladder.initialized ) {
                SV_LadderInit();
                if ( !sv_ladder.initialized ) {
                        return;
                }
        }

#ifdef USE_CURL
        SV_LadderActivateNext();
        SV_LadderPollActive();
        if ( !sv_ladder.active ) {
                SV_LadderActivateNext();
        }
#endif
}
