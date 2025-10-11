/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "client.h"

#include "../botlib/botlib.h"
#include "../qcommon/json.h"

#include <ctype.h>

extern	botlib_export_t	*botlib_export;

vm_t *uivm;

static uiLadderStatus_t cl_ladderStatus;
static void CL_LadderResetStatus( void );
static void CL_LadderBeginRequest( void );
#ifdef USE_CURL
static cvar_t *cl_ladderEndpoint = NULL;

typedef struct {
        char    *data;
        size_t  length;
        size_t  capacity;
} ladderDownloadBuffer_t;

static CURLM                  *cl_ladderCurlMulti = NULL;
static CURL                   *cl_ladderCurlEasy = NULL;
static ladderDownloadBuffer_t cl_ladderBuffer;
static qboolean               cl_ladderRequestQueued = qfalse;

static void CL_LadderCleanupRequest( qboolean releaseResources );
static void CL_LadderCancelRequest( void );
static void CL_LadderSetError( const char *message );


static qboolean CL_LadderBufferEnsureCapacity( ladderDownloadBuffer_t *buffer, size_t required ) {
        char    *newData;
        size_t  newCapacity;

        if ( required <= buffer->capacity ) {
                return qtrue;
        }

        newCapacity = buffer->capacity ? buffer->capacity : 1024;
        while ( newCapacity < required ) {
                newCapacity *= 2;
        }

        newData = (char *)Z_Malloc( newCapacity );
        if ( !newData ) {
                return qfalse;
        }

        if ( buffer->data && buffer->length > 0 ) {
                Com_Memcpy( newData, buffer->data, buffer->length );
        }

        if ( buffer->data ) {
                Z_Free( buffer->data );
        }

        buffer->data = newData;
        buffer->capacity = newCapacity;
        return qtrue;
}

static size_t CL_LadderCurlWrite( void *contents, size_t size, size_t nmemb, void *userp ) {
        ladderDownloadBuffer_t  *buffer;
        size_t                  bytes;

        buffer = (ladderDownloadBuffer_t *)userp;
        bytes = size * nmemb;

        if ( !buffer || !bytes ) {
                return bytes;
        }

        if ( !CL_LadderBufferEnsureCapacity( buffer, buffer->length + bytes + 1 ) ) {
                return 0;
        }

        Com_Memcpy( buffer->data + buffer->length, contents, bytes );
        buffer->length += bytes;
        buffer->data[buffer->length] = '\0';

        return bytes;
}

static const char *CL_LadderResolveEntriesNode( const char *json, const char *jsonEnd, const char **errorNode ) {
        const char      *entries;
        const char      *dataNode;

        entries = JSON_ObjectGetNamedValue( json, jsonEnd, "entries" );
        if ( entries ) {
                if ( errorNode ) {
                        *errorNode = JSON_ObjectGetNamedValue( json, jsonEnd, "error" );
                }
                return entries;
        }

        dataNode = JSON_ObjectGetNamedValue( json, jsonEnd, "data" );
        if ( dataNode ) {
                entries = JSON_ObjectGetNamedValue( dataNode, jsonEnd, "entries" );
                if ( entries && errorNode ) {
                        *errorNode = JSON_ObjectGetNamedValue( dataNode, jsonEnd, "error" );
                }
                return entries;
        }

        if ( errorNode ) {
                *errorNode = JSON_ObjectGetNamedValue( json, jsonEnd, "error" );
        }

        return NULL;
}

static void CL_LadderParseResponse( const char *json, size_t length ) {
        const char      *jsonEnd;
        const char      *entriesNode;
        const char      *errorNode;
        const char      *cursor;
        int                     count;

        if ( !json || !length ) {
                CL_LadderSetError( "Empty ladder response." );
                return;
        }

        jsonEnd = json + length;
        errorNode = NULL;
        entriesNode = CL_LadderResolveEntriesNode( json, jsonEnd, &errorNode );

        if ( !entriesNode ) {
                char errorMessage[MAX_STRING_CHARS];

                if ( errorNode && JSON_ValueGetString( errorNode, jsonEnd, errorMessage, sizeof( errorMessage ) ) ) {
                        CL_LadderSetError( errorMessage );
                } else {
                        CL_LadderSetError( "Ladder service returned an unexpected response." );
                }
                return;
        }

        cursor = JSON_ArrayGetFirstValue( entriesNode, jsonEnd );
        count = 0;

        while ( cursor && count < UI_MAX_LADDER_ENTRIES ) {
                uiLadderEntry_t        *entry;
                const char              *value;

                entry = &cl_ladderStatus.entries[count];
                Com_Memset( entry, 0, sizeof( *entry ) );

                value = JSON_ObjectGetNamedValue( cursor, jsonEnd, "rank" );
                if ( value ) {
                        entry->rank = JSON_ValueGetInt( value, jsonEnd );
                }

                value = JSON_ObjectGetNamedValue( cursor, jsonEnd, "player" );
                if ( value ) {
                        JSON_ValueGetString( value, jsonEnd, entry->player, sizeof( entry->player ) );
                }

                value = JSON_ObjectGetNamedValue( cursor, jsonEnd, "mode" );
                if ( value ) {
                        JSON_ValueGetString( value, jsonEnd, entry->mode, sizeof( entry->mode ) );
                }

                value = JSON_ObjectGetNamedValue( cursor, jsonEnd, "vehicle" );
                if ( value ) {
                        JSON_ValueGetString( value, jsonEnd, entry->vehicle, sizeof( entry->vehicle ) );
                }

                value = JSON_ObjectGetNamedValue( cursor, jsonEnd, "region" );
                if ( value ) {
                        JSON_ValueGetString( value, jsonEnd, entry->region, sizeof( entry->region ) );
                }

                value = JSON_ObjectGetNamedValue( cursor, jsonEnd, "metric" );
                if ( value ) {
                        JSON_ValueGetString( value, jsonEnd, entry->metric, sizeof( entry->metric ) );
                }

                count++;
                cursor = JSON_ArrayGetNextValue( cursor, jsonEnd );
        }

        cl_ladderStatus.entryCount = count;
        cl_ladderStatus.status = UI_LADDER_STATUS_READY;
        cl_ladderStatus.errorMessage[0] = '\0';
}

static void CL_LadderCleanupRequest( qboolean releaseResources ) {
        if ( cl_ladderCurlMulti && cl_ladderCurlEasy && cl_ladderRequestQueued ) {
                qcurl_multi_remove_handle( cl_ladderCurlMulti, cl_ladderCurlEasy );
        }

        cl_ladderRequestQueued = qfalse;

        if ( cl_ladderCurlEasy ) {
                if ( releaseResources ) {
                        qcurl_easy_cleanup( cl_ladderCurlEasy );
                        cl_ladderCurlEasy = NULL;
                } else {
#ifdef USE_CURL_DLOPEN
                        if ( qcurl_easy_reset ) {
                                qcurl_easy_reset( cl_ladderCurlEasy );
                        } else {
                                qcurl_easy_cleanup( cl_ladderCurlEasy );
                                cl_ladderCurlEasy = NULL;
                        }
#else
                        qcurl_easy_reset( cl_ladderCurlEasy );
#endif
                }
        }

        cl_ladderBuffer.length = 0;
        if ( cl_ladderBuffer.data ) {
                cl_ladderBuffer.data[0] = '\0';
        }

        if ( releaseResources ) {
                if ( cl_ladderCurlMulti ) {
                        qcurl_multi_cleanup( cl_ladderCurlMulti );
                        cl_ladderCurlMulti = NULL;
                }

                if ( cl_ladderBuffer.data ) {
                        Z_Free( cl_ladderBuffer.data );
                        cl_ladderBuffer.data = NULL;
                        cl_ladderBuffer.capacity = 0;
                }
        }
}

static void CL_LadderCancelRequest( void ) {
        CL_LadderCleanupRequest( qfalse );
	CL_LadderResetStatus();
}
#else
static void CL_LadderCancelRequest( void ) {
	CL_LadderCleanupRequest( qfalse );
	CL_LadderResetStatus();
}
#endif // USE_CURL

static void CL_UpdateEnsureCvars( void );
static void CL_UpdateResetStatus( void );
static void CL_UpdateSetStatus( const char *status, const char *latest, const char *url, const char *message );
static void CL_UpdateHandleResponse( void );
static qboolean CL_UpdateParseResponse( const char *data, size_t length, char *latestOut, size_t latestSize, char *urlOut, size_t urlSize, char *messageOut, size_t messageSize );
static int CL_UpdateCompareVersions( const char *localVersion, const char *remoteVersion );
static void CL_UpdateStripWrappingQuotes( char *text );
static qboolean CL_UpdateIsValidVersionString( const char *text );
static const char *CL_UpdateSkipVersionPrefix( const char *text );

static const char *const CL_UPDATE_ENDPOINT_DEFAULT = "https://ladder.q3rally.com/version.txt";
static const char *const CL_UPDATE_ENDPOINT_LEGACY = "https://ladder.q3rally.com/index.php/version";

static cvar_t *cl_updateEndpoint = NULL;
static cvar_t *cl_updateCheck = NULL;
static cvar_t *cl_updateStatusCvar = NULL;
static cvar_t *cl_updateLatestCvar = NULL;
static cvar_t *cl_updateUrlCvar = NULL;
static cvar_t *cl_updateMessageCvar = NULL;

#ifdef USE_CURL
typedef struct {
        char    *data;
        size_t  length;
        size_t  capacity;
} updateDownloadBuffer_t;

static CURLM                  *cl_updateCurlMulti = NULL;
static CURL                   *cl_updateCurlEasy = NULL;
static updateDownloadBuffer_t cl_updateBuffer;
static qboolean               cl_updateRequestQueued = qfalse;

static qboolean CL_UpdateBufferEnsureCapacity( updateDownloadBuffer_t *buffer, size_t required );
static void CL_UpdateCleanupRequest( qboolean releaseResources );
static size_t CL_UpdateCurlWrite( void *contents, size_t size, size_t nmemb, void *userp );
#else
static void CL_UpdateCleanupRequest( qboolean releaseResources ) { (void)releaseResources; }
#endif

void CL_LadderPumpRequest( void ) {
#ifdef USE_CURL
        CURLMcode       multiCode;
        CURLMsg         *msg;
        int             queued;
        int             running;

        if ( !cl_ladderCurlMulti || !cl_ladderCurlEasy || !cl_ladderRequestQueued ) {
                return;
        }

        running = 0;
        do {
                multiCode = qcurl_multi_perform( cl_ladderCurlMulti, &running );
        } while ( multiCode == CURLM_CALL_MULTI_PERFORM );

        if ( multiCode != CURLM_OK ) {
                const char *message;

#ifdef USE_CURL_DLOPEN
                message = qcurl_multi_strerror ? qcurl_multi_strerror( multiCode ) : NULL;
#else
                message = qcurl_multi_strerror( multiCode );
#endif
                CL_LadderSetError( message ? message : "Unable to fetch ladder data." );
                CL_LadderCleanupRequest( qfalse );
                return;
        }

        while ( ( msg = qcurl_multi_info_read( cl_ladderCurlMulti, &queued ) ) != NULL ) {
                long            responseCode;

                if ( msg->easy_handle != cl_ladderCurlEasy || msg->msg != CURLMSG_DONE ) {
                        continue;
                }

                if ( msg->data.result != CURLE_OK ) {
                        const char *message;

#ifdef USE_CURL_DLOPEN
                        message = qcurl_easy_strerror ? qcurl_easy_strerror( msg->data.result ) : NULL;
#else
                        message = qcurl_easy_strerror( msg->data.result );
#endif
                        CL_LadderSetError( message ? message : "Unable to fetch ladder data." );
                        CL_LadderCleanupRequest( qfalse );
                        return;
                }

                responseCode = 0;
                qcurl_easy_getinfo( cl_ladderCurlEasy, CURLINFO_RESPONSE_CODE, &responseCode );

                if ( responseCode != 200 ) {
                        CL_LadderSetError( va( "Ladder service returned %ld", responseCode ) );
                        CL_LadderCleanupRequest( qfalse );
                        return;
                }

                CL_LadderParseResponse( cl_ladderBuffer.data, cl_ladderBuffer.length );
                CL_LadderCleanupRequest( qfalse );

                if ( cl_ladderStatus.status != UI_LADDER_STATUS_READY ) {
                        if ( cl_ladderStatus.status != UI_LADDER_STATUS_ERROR ) {
                                CL_LadderSetError( "Failed to parse ladder response." );
                        }
                }

                break;
        }
#else
        (void)0;
#endif
}

static void CL_LadderResetStatus( void ) {
	Com_Memset( &cl_ladderStatus, 0, sizeof( cl_ladderStatus ) );
	cl_ladderStatus.status = UI_LADDER_STATUS_EMPTY;
}

static void CL_LadderBeginRequest( void ) {
	CL_LadderResetStatus();
	cl_ladderStatus.status = UI_LADDER_STATUS_LOADING;
}

static void CL_LadderSetError( const char *message ) {
        cl_ladderStatus.status = UI_LADDER_STATUS_ERROR;
        cl_ladderStatus.entryCount = 0;
        Q_strncpyz( cl_ladderStatus.errorMessage, ( message && message[0] ) ? message : "Ladder service unavailable.", sizeof( cl_ladderStatus.errorMessage ) );
}

static void CL_LadderRequestData( const char *mode, const char *timeframe, const char *region ) {
#ifdef USE_CURL
        CURLMcode               multiResult;
        char                    url[MAX_STRING_CHARS];
        const char              *endpoint;
        qboolean                hasQuery;

        CL_LadderBeginRequest();

        if ( !cl_ladderEndpoint ) {
                cl_ladderEndpoint = Cvar_Get( "cl_ladderEndpoint", "https://ladder.q3rally.com/api/v1/leaderboard", CVAR_ARCHIVE );
        }

        if ( !CL_cURL_Init() ) {
                CL_LadderSetError( "cURL support is not available." );
                return;
        }

        CL_LadderCleanupRequest( qfalse );

        if ( !cl_ladderCurlMulti ) {
                cl_ladderCurlMulti = qcurl_multi_init();
                if ( !cl_ladderCurlMulti ) {
                        CL_LadderSetError( "Failed to create HTTP transfer context." );
                        CL_LadderCleanupRequest( qfalse );
                        return;
                }
        }

        endpoint = cl_ladderEndpoint ? cl_ladderEndpoint->string : "https://ladder.q3rally.com/api/v1/leaderboard";
        hasQuery = (strchr( endpoint, '?' ) != NULL);

        Com_sprintf( url, sizeof( url ), "%s%c%s=%s&%s=%s&%s=%s",
                endpoint,
                hasQuery ? '&' : '?',
                "mode", mode && mode[0] ? mode : "all",
                "timeframe", timeframe && timeframe[0] ? timeframe : "all_time",
                "region", region && region[0] ? region : "global" );

        if ( !cl_ladderCurlEasy ) {
                cl_ladderCurlEasy = qcurl_easy_init();
        }

        if ( !cl_ladderCurlEasy ) {
                CL_LadderSetError( "Failed to initialize HTTP client." );
                return;
        }

        cl_ladderBuffer.length = 0;
        if ( cl_ladderBuffer.data ) {
                cl_ladderBuffer.data[0] = '\0';
        }

        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_URL, url );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_FOLLOWLOCATION, 1L );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_FAILONERROR, 1L );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_WRITEFUNCTION, CL_LadderCurlWrite );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_WRITEDATA, &cl_ladderBuffer );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_USERAGENT, va( "%s %s", Q3_VERSION, qcurl_version() ) );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_NOSIGNAL, 1L );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_TIMEOUT, 10L );
        qcurl_easy_setopt( cl_ladderCurlEasy, CURLOPT_CONNECTTIMEOUT, 5L );

        multiResult = qcurl_multi_add_handle( cl_ladderCurlMulti, cl_ladderCurlEasy );
        if ( multiResult != CURLM_OK ) {
                const char      *message;

#ifdef USE_CURL_DLOPEN
                message = qcurl_multi_strerror ? qcurl_multi_strerror( multiResult ) : NULL;
#else
                message = qcurl_multi_strerror( multiResult );
#endif
                CL_LadderSetError( message ? message : "Unable to queue ladder transfer." );
                CL_LadderCleanupRequest( qfalse );
                return;
        }

        cl_ladderRequestQueued = qtrue;
#else
        (void)mode;
        (void)timeframe;
        (void)region;

        CL_LadderBeginRequest();
        CL_LadderSetError( "Ladder data requires a build with cURL support." );
#endif
}

static void CL_UpdateEnsureCvars( void ) {
        if ( !cl_updateStatusCvar ) {
                cl_updateStatusCvar = Cvar_Get( "cl_updateStatus", "idle", CVAR_ROM | CVAR_NORESTART );
        }

        if ( !cl_updateLatestCvar ) {
                cl_updateLatestCvar = Cvar_Get( "cl_updateLatest", "", CVAR_ROM | CVAR_NORESTART );
        }

        if ( !cl_updateUrlCvar ) {
                cl_updateUrlCvar = Cvar_Get( "cl_updateUrl", "", CVAR_ROM | CVAR_NORESTART );
        }

        if ( !cl_updateMessageCvar ) {
                cl_updateMessageCvar = Cvar_Get( "cl_updateMessage", "", CVAR_ROM | CVAR_NORESTART );
        }

        if ( !cl_updateEndpoint ) {
                cl_updateEndpoint = Cvar_Get( "cl_updateEndpoint", CL_UPDATE_ENDPOINT_DEFAULT, CVAR_ARCHIVE );

                if ( cl_updateEndpoint && !Q_stricmp( cl_updateEndpoint->string, CL_UPDATE_ENDPOINT_LEGACY ) ) {
                        Cvar_Set( cl_updateEndpoint->name, CL_UPDATE_ENDPOINT_DEFAULT );
                }
        }

        if ( !cl_updateCheck ) {
                cl_updateCheck = Cvar_Get( "cl_updateCheck", "1", CVAR_ARCHIVE );
        }
}

static void CL_UpdateSetStatus( const char *status, const char *latest, const char *url, const char *message ) {
        CL_UpdateEnsureCvars();

        Cvar_Set( cl_updateStatusCvar->name, status ? status : "" );
        Cvar_Set( cl_updateLatestCvar->name, latest ? latest : "" );
        Cvar_Set( cl_updateUrlCvar->name, url ? url : "" );
        Cvar_Set( cl_updateMessageCvar->name, message ? message : "" );
}

static void CL_UpdateTrim( char *text ) {
        char    *start;
        char    *end;
        size_t  length;

        if ( !text ) {
                return;
        }

        start = text;
        while ( *start && (unsigned char)*start <= ' ' ) {
                start++;
        }

        end = start + strlen( start );
        while ( end > start && (unsigned char)end[-1] <= ' ' ) {
                end--;
        }

        length = end - start;
        if ( start != text && length > 0 ) {
                memmove( text, start, length );
        }
        text[length] = '\0';
}

static void CL_UpdateStripWrappingQuotes( char *text ) {
        size_t length;

        if ( !text ) {
                return;
        }

        length = strlen( text );

        if ( length >= 2 ) {
                char first = text[0];
                char last = text[length - 1];

                if ( ( first == '"' && last == '"' ) || ( first == '\'' && last == '\'' ) ) {
                        memmove( text, text + 1, length - 1 );
                        text[length - 2] = '\0';
                }
        }

        if ( text[0] == '"' || text[0] == '\'' ) {
                memmove( text, text + 1, strlen( text ) );
        }

        length = strlen( text );
        if ( length > 0 && ( text[length - 1] == '"' || text[length - 1] == '\'' ) ) {
                text[length - 1] = '\0';
        }
}

static qboolean CL_UpdateIsValidVersionString( const char *text ) {
        if ( !text ) {
                return qfalse;
        }

	while ( *text ) {
		if ( isdigit( (unsigned char)*text ) ) {
			return qtrue;
		}
		text++;
	}

	return qfalse;
}

static const char *CL_UpdateSkipVersionPrefix( const char *text ) {
        if ( !text ) {
                return "";
        }

        while ( *text && (unsigned char)*text <= ' ' ) {
                text++;
        }

        if ( ( text[0] == 'v' || text[0] == 'V' ) && ( isdigit( (unsigned char)text[1] ) || text[1] == '.' ) ) {
                text++;
        }

        return text;
}

static void CL_UpdateResetStatus( void ) {
        CL_UpdateSetStatus( "idle", "", "", "" );
}

static int CL_UpdateCompareVersions( const char *localVersion, const char *remoteVersion ) {
        int localParts[6] = {0};
        int remoteParts[6] = {0};
        int i;

        Com_Printf("=== CL_UpdateCompareVersions ===\n");
        Com_Printf("Local version: '%s'\n", localVersion ? localVersion : "NULL");
        Com_Printf("Remote version: '%s'\n", remoteVersion ? remoteVersion : "NULL");

        if ( !remoteVersion || !remoteVersion[0] ) {
                Com_Printf("Remote version is empty, returning 0\n");
                return 0;
        }

        if ( !localVersion || !localVersion[0] ) {
                Com_Printf("Local version is empty, returning -1\n");
                return -1;
        }

        {
                const char *cursor;
                int         index;

                cursor = localVersion;
                index = 0;
                while ( *cursor && index < 6 ) {
                        while ( *cursor && !isdigit( (unsigned char)*cursor ) ) {
                                cursor++;
                        }

                        if ( !*cursor ) {
                                break;
                        }

                        while ( *cursor && isdigit( (unsigned char)*cursor ) ) {
                                localParts[index] = localParts[index] * 10 + ( *cursor - '0' );
                                cursor++;
                        }

                        index++;
                }

                cursor = remoteVersion;
                index = 0;
                while ( *cursor && index < 6 ) {
                        while ( *cursor && !isdigit( (unsigned char)*cursor ) ) {
                                cursor++;
                        }

                        if ( !*cursor ) {
                                break;
                        }

                        while ( *cursor && isdigit( (unsigned char)*cursor ) ) {
                                remoteParts[index] = remoteParts[index] * 10 + ( *cursor - '0' );
                                cursor++;
                        }

                        index++;
                }
        }

        Com_Printf("Local parts: [%d, %d, %d, %d, %d, %d]\n",
                   localParts[0], localParts[1], localParts[2], localParts[3], localParts[4], localParts[5]);
        Com_Printf("Remote parts: [%d, %d, %d, %d, %d, %d]\n",
                   remoteParts[0], remoteParts[1], remoteParts[2], remoteParts[3], remoteParts[4], remoteParts[5]);

        for ( i = 0; i < 6; i++ ) {
                if ( localParts[i] < remoteParts[i] ) {
                        Com_Printf("Local < Remote at index %d, returning -1\n", i);
                        return -1;
                }

                if ( localParts[i] > remoteParts[i] ) {
                        Com_Printf("Local > Remote at index %d, returning 1\n", i);
                        return 1;
                }
        }

        Com_Printf("Numeric parts are equal, doing string comparison\n");

        {
                const char *localComparable;
                const char *remoteComparable;
                size_t      localLen;
                size_t      remoteLen;
                int         cmpResult;  /* HIER DEKLARIERT */

                localComparable = CL_UpdateSkipVersionPrefix( localVersion );
                remoteComparable = CL_UpdateSkipVersionPrefix( remoteVersion );

                localLen = strlen( localComparable );
                remoteLen = strlen( remoteComparable );

                Com_Printf("After prefix skip:\n");
                Com_Printf("  Local: '%s' (length %d)\n", localComparable, (int)localLen);
                Com_Printf("  Remote: '%s' (length %d)\n", remoteComparable, (int)remoteLen);

                if ( remoteLen > localLen ) {
                        cmpResult = Q_stricmpn( localComparable, remoteComparable, localLen );
                        Com_Printf("Remote is longer. Q_stricmpn(first %d chars) = %d\n", (int)localLen, cmpResult);
                        if ( cmpResult == 0 ) {
                                Com_Printf("Remote starts with local and is longer -> Remote is newer, returning -1\n");
                                return -1;
                        }
                }

                if ( localLen > remoteLen ) {
                        cmpResult = Q_stricmpn( localComparable, remoteComparable, remoteLen );
                        Com_Printf("Local is longer. Q_stricmpn(first %d chars) = %d\n", (int)remoteLen, cmpResult);
                        if ( cmpResult == 0 ) {
                                Com_Printf("Local starts with remote and is longer -> Local is newer, returning 1\n");
                                return 1;
                        }
                }

                cmpResult = Q_stricmp( localComparable, remoteComparable );
                Com_Printf("Final Q_stricmp result: %d\n", cmpResult);
                Com_Printf("================================\n");
                return cmpResult;
        }
}

static qboolean CL_UpdateParseResponse( const char *data, size_t length, char *latestOut, size_t latestSize, char *urlOut, size_t urlSize, char *messageOut, size_t messageSize ) {
        const char      *cursor;
        const char      *end;
        size_t          index;

        if ( !data || !latestOut || !latestSize ) {
                return qfalse;
        }

        if ( !length ) {
                return qfalse;
        }

        cursor = data;
        end = data + length;

        while ( cursor < end && ( *cursor == '\n' || *cursor == '\r' || *cursor == '\t' || *cursor == ' ' ) ) {
                cursor++;
        }

        index = 0;
        while ( cursor < end && *cursor != '\n' && *cursor != '\r' && index + 1 < latestSize ) {
                latestOut[index++] = *cursor++;
        }
        latestOut[index] = '\0';

        CL_UpdateTrim( latestOut );
        CL_UpdateStripWrappingQuotes( latestOut );
        CL_UpdateTrim( latestOut );

        if ( !CL_UpdateIsValidVersionString( latestOut ) ) {
                latestOut[0] = '\0';
                return qfalse;
        }

        if ( urlOut && urlSize > 0 ) {
                urlOut[0] = '\0';
        }

        if ( messageOut && messageSize > 0 ) {
                messageOut[0] = '\0';
        }

        return qtrue;
}

static void CL_UpdateHandleResponse( void ) {
#ifdef USE_CURL
        char    latest[64];
        char    cleanVersion[64];
        char    *underscore;
        int     comparison;

        if ( !cl_updateBuffer.data || cl_updateBuffer.length == 0 ) {
                CL_UpdateSetStatus( "error", "", "", "Empty response from the update server." );
                return;
        }

        if ( !CL_UpdateParseResponse( cl_updateBuffer.data, cl_updateBuffer.length, latest, sizeof( latest ), NULL, 0, NULL, 0 ) ) {
                CL_UpdateSetStatus( "error", "", "", "Invalid response from the update server." );
                return;
        }

        /* NEU: Bereinige PRODUCT_VERSION - entferne alles ab dem ersten Unterstrich */
        Q_strncpyz( cleanVersion, PRODUCT_VERSION, sizeof( cleanVersion ) );
        underscore = strchr( cleanVersion, '_' );
        if ( underscore ) {
                *underscore = '\0';
        }

        comparison = CL_UpdateCompareVersions( cleanVersion, latest );

        if ( comparison < 0 ) {
                const char *updateUrl = "https://www.q3rally.com";
                const char *updateMessage = "Visit www.q3rally.com to download the latest version.";

                CL_UpdateSetStatus( "outdated", latest, updateUrl, updateMessage );
                Com_Printf( S_COLOR_YELLOW "Update available:" S_COLOR_WHITE " Installed %s, latest %s\n", PRODUCT_VERSION, latest );
        } else {
                CL_UpdateSetStatus( "up_to_date", latest, "", "" );
        }
#else
        CL_UpdateSetStatus( "error", "", "", "Update check requires a build with cURL support." );
#endif
}

#ifdef USE_CURL
static qboolean CL_UpdateBufferEnsureCapacity( updateDownloadBuffer_t *buffer, size_t required ) {
        char    *newData;
        size_t  newCapacity;

        if ( required <= buffer->capacity ) {
                return qtrue;
        }

        newCapacity = buffer->capacity ? buffer->capacity : 1024;
        while ( newCapacity < required ) {
                newCapacity *= 2;
        }

        newData = (char *)Z_Malloc( newCapacity );
        if ( !newData ) {
                return qfalse;
        }

        if ( buffer->data && buffer->length > 0 ) {
                Com_Memcpy( newData, buffer->data, buffer->length );
        }

        if ( buffer->data ) {
                Z_Free( buffer->data );
        }

        buffer->data = newData;
        buffer->capacity = newCapacity;
        return qtrue;
}

static size_t CL_UpdateCurlWrite( void *contents, size_t size, size_t nmemb, void *userp ) {
        updateDownloadBuffer_t  *buffer;
        size_t                  bytes;

        buffer = (updateDownloadBuffer_t *)userp;
        bytes = size * nmemb;

        if ( !buffer || !bytes ) {
                return bytes;
        }

        if ( !CL_UpdateBufferEnsureCapacity( buffer, buffer->length + bytes + 1 ) ) {
                return 0;
        }

        Com_Memcpy( buffer->data + buffer->length, contents, bytes );
        buffer->length += bytes;
        buffer->data[buffer->length] = '\0';

        return bytes;
}

static void CL_UpdateCleanupRequest( qboolean releaseResources ) {
        if ( cl_updateCurlMulti && cl_updateCurlEasy && cl_updateRequestQueued ) {
                qcurl_multi_remove_handle( cl_updateCurlMulti, cl_updateCurlEasy );
        }

        cl_updateRequestQueued = qfalse;

        if ( cl_updateCurlEasy ) {
                if ( releaseResources ) {
                        qcurl_easy_cleanup( cl_updateCurlEasy );
                        cl_updateCurlEasy = NULL;
                } else {
#ifdef USE_CURL_DLOPEN
                        if ( qcurl_easy_reset ) {
                                qcurl_easy_reset( cl_updateCurlEasy );
                        } else {
                                qcurl_easy_cleanup( cl_updateCurlEasy );
                                cl_updateCurlEasy = NULL;
                        }
#else
                        qcurl_easy_reset( cl_updateCurlEasy );
#endif
                }
        }

        cl_updateBuffer.length = 0;
        if ( cl_updateBuffer.data ) {
                cl_updateBuffer.data[0] = '\0';
        }

        if ( releaseResources ) {
                if ( cl_updateCurlMulti ) {
                        qcurl_multi_cleanup( cl_updateCurlMulti );
                        cl_updateCurlMulti = NULL;
                }

                if ( cl_updateBuffer.data ) {
                        Z_Free( cl_updateBuffer.data );
                        cl_updateBuffer.data = NULL;
                        cl_updateBuffer.capacity = 0;
                }
        }
}

void CL_UpdatePumpRequest( void ) {
        CURLMcode       multiCode;
        CURLMsg         *msg;
        int             queued;
        int             running;

        if ( !cl_updateCurlMulti || !cl_updateCurlEasy || !cl_updateRequestQueued ) {
                return;
        }

        running = 0;
        do {
                multiCode = qcurl_multi_perform( cl_updateCurlMulti, &running );
        } while ( multiCode == CURLM_CALL_MULTI_PERFORM );

        if ( multiCode != CURLM_OK ) {
                const char *message;

#ifdef USE_CURL_DLOPEN
                message = qcurl_multi_strerror ? qcurl_multi_strerror( multiCode ) : NULL;
#else
                message = qcurl_multi_strerror( multiCode );
#endif
                CL_UpdateSetStatus( "error", "", "", message ? message : "Update service is not reachable." );
                CL_UpdateCleanupRequest( qfalse );
                return;
        }

        while ( ( msg = qcurl_multi_info_read( cl_updateCurlMulti, &queued ) ) != NULL ) {
                long responseCode = 0;

                if ( msg->easy_handle != cl_updateCurlEasy || msg->msg != CURLMSG_DONE ) {
                        continue;
                }

                if ( msg->data.result != CURLE_OK ) {
                        const char *message;

#ifdef USE_CURL_DLOPEN
                        message = qcurl_easy_strerror ? qcurl_easy_strerror( msg->data.result ) : NULL;
#else
                        message = qcurl_easy_strerror( msg->data.result );
#endif
                        CL_UpdateSetStatus( "error", "", "", message ? message : "Update service is not reachable." );
                        CL_UpdateCleanupRequest( qfalse );
                        return;
                }

                qcurl_easy_getinfo( cl_updateCurlEasy, CURLINFO_RESPONSE_CODE, &responseCode );

                if ( responseCode != 200 ) {
                        CL_UpdateSetStatus( "error", "", "", va( "Update server responded with HTTP %ld", responseCode ) );
                        CL_UpdateCleanupRequest( qfalse );
                        return;
                }

                CL_UpdateHandleResponse();
                CL_UpdateCleanupRequest( qfalse );
                break;
        }
}

void CL_UpdateRequestLatest( void ) {
        const char      *endpoint;
        CURLMcode       multiResult;

        CL_UpdateEnsureCvars();

        if ( cl_updateCheck && !cl_updateCheck->integer ) {
                CL_UpdateSetStatus( "disabled", "", "", "" );
                return;
        }

        if ( cl_updateRequestQueued ) {
                return;
        }

        if ( !CL_cURL_Init() ) {
                CL_UpdateSetStatus( "error", "", "", "cURL support is not available." );
                return;
        }

        CL_UpdateCleanupRequest( qfalse );

        if ( !cl_updateCurlMulti ) {
                cl_updateCurlMulti = qcurl_multi_init();
                if ( !cl_updateCurlMulti ) {
                        CL_UpdateSetStatus( "error", "", "", "Failed to create HTTP context." );
                        return;
                }
        }

        if ( !cl_updateCurlEasy ) {
                cl_updateCurlEasy = qcurl_easy_init();
        }

        if ( !cl_updateCurlEasy ) {
                CL_UpdateSetStatus( "error", "", "", "Failed to initialise HTTP client." );
                return;
        }

        endpoint = cl_updateEndpoint ? cl_updateEndpoint->string : "";
        if ( !endpoint || !endpoint[0] ) {
                CL_UpdateSetStatus( "error", "", "", "cl_updateEndpoint is not set." );
                return;
        }

        cl_updateBuffer.length = 0;
        if ( cl_updateBuffer.data ) {
                cl_updateBuffer.data[0] = '\0';
        }

        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_URL, endpoint );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_FOLLOWLOCATION, 1L );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_FAILONERROR, 0L );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_WRITEFUNCTION, CL_UpdateCurlWrite );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_WRITEDATA, &cl_updateBuffer );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_USERAGENT, Q3_VERSION );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_NOSIGNAL, 1L );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_TIMEOUT, 10L );
        qcurl_easy_setopt( cl_updateCurlEasy, CURLOPT_CONNECTTIMEOUT, 5L );

        multiResult = qcurl_multi_add_handle( cl_updateCurlMulti, cl_updateCurlEasy );
        if ( multiResult != CURLM_OK ) {
                const char *message;

#ifdef USE_CURL_DLOPEN
                message = qcurl_multi_strerror ? qcurl_multi_strerror( multiResult ) : NULL;
#else
                message = qcurl_multi_strerror( multiResult );
#endif
                CL_UpdateSetStatus( "error", "", "", message ? message : "Failed to start update request." );
                CL_UpdateCleanupRequest( qfalse );
                return;
        }

        cl_updateRequestQueued = qtrue;
        CL_UpdateSetStatus( "checking", "", "", "" );
}

void CL_UpdateShutdown( void ) {
        CL_UpdateCleanupRequest( qtrue );
        CL_UpdateResetStatus();
}
#else
void CL_UpdatePumpRequest( void ) {
        (void)0;
}

void CL_UpdateRequestLatest( void ) {
        CL_UpdateEnsureCvars();
        CL_UpdateSetStatus( "error", "", "", "Update check requires a build with cURL support." );
}

void CL_UpdateShutdown( void ) {
        CL_UpdateResetStatus();
}
#endif

/*
====================
GetClientState
====================
*/
static void GetClientState( uiClientState_t *state ) {
	state->connectPacketCount = clc.connectPacketCount;
	state->connState = clc.state;
	Q_strncpyz( state->servername, clc.servername, sizeof( state->servername ) );
	Q_strncpyz( state->updateInfoString, cls.updateInfoString, sizeof( state->updateInfoString ) );
	Q_strncpyz( state->messageString, clc.serverMessage, sizeof( state->messageString ) );
	state->clientNum = cl.snap.ps.clientNum;
}

/*
====================
LAN_LoadCachedServers
====================
*/
void LAN_LoadCachedServers( void ) {
	int size;
	fileHandle_t fileIn;
	cls.numglobalservers = cls.numfavoriteservers = 0;
	cls.numGlobalServerAddresses = 0;
	if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
		FS_Read(&cls.numglobalservers, sizeof(int), fileIn);
		FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn);
		FS_Read(&size, sizeof(int), fileIn);
		if (size == sizeof(cls.globalServers) + sizeof(cls.favoriteServers)) {
			FS_Read(&cls.globalServers, sizeof(cls.globalServers), fileIn);
			FS_Read(&cls.favoriteServers, sizeof(cls.favoriteServers), fileIn);
		} else {
			cls.numglobalservers = cls.numfavoriteservers = 0;
			cls.numGlobalServerAddresses = 0;
		}
		FS_FCloseFile(fileIn);
	}
}

/*
====================
LAN_SaveServersToCache
====================
*/
void LAN_SaveServersToCache( void ) {
	int size;
	fileHandle_t fileOut = FS_SV_FOpenFileWrite("servercache.dat");
	FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
	FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);
	size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers);
	FS_Write(&size, sizeof(int), fileOut);
	FS_Write(&cls.globalServers, sizeof(cls.globalServers), fileOut);
	FS_Write(&cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
	FS_FCloseFile(fileOut);
}


/*
====================
LAN_ResetPings
====================
*/
static void LAN_ResetPings(int source) {
	int count,i;
	serverInfo_t *servers = NULL;
	count = 0;

	switch (source) {
		case AS_LOCAL :
			servers = &cls.localServers[0];
			count = MAX_OTHER_SERVERS;
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			servers = &cls.globalServers[0];
			count = MAX_GLOBAL_SERVERS;
			break;
		case AS_FAVORITES :
			servers = &cls.favoriteServers[0];
			count = MAX_OTHER_SERVERS;
			break;
	}
	if (servers) {
		for (i = 0; i < count; i++) {
			servers[i].ping = -1;
		}
	}
}

/*
====================
LAN_AddServer
====================
*/
static int LAN_AddServer(int source, const char *name, const char *address) {
	int max, *count, i;
	netadr_t adr;
	serverInfo_t *servers = NULL;
	max = MAX_OTHER_SERVERS;
	count = NULL;

	switch (source) {
		case AS_LOCAL :
			count = &cls.numlocalservers;
			servers = &cls.localServers[0];
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			max = MAX_GLOBAL_SERVERS;
			count = &cls.numglobalservers;
			servers = &cls.globalServers[0];
			break;
		case AS_FAVORITES :
			count = &cls.numfavoriteservers;
			servers = &cls.favoriteServers[0];
			break;
	}
	if (servers && *count < max) {
		NET_StringToAdr( address, &adr, NA_UNSPEC );
		for ( i = 0; i < *count; i++ ) {
			if (NET_CompareAdr(servers[i].adr, adr)) {
				break;
			}
		}
		if (i >= *count) {
			servers[*count].adr = adr;
			Q_strncpyz(servers[*count].hostName, name, sizeof(servers[*count].hostName));
			servers[*count].visible = qtrue;
			(*count)++;
			return 1;
		}
		return 0;
	}
	return -1;
}

/*
====================
LAN_RemoveServer
====================
*/
static void LAN_RemoveServer(int source, const char *addr) {
	int *count, i;
	serverInfo_t *servers = NULL;
	count = NULL;
	switch (source) {
		case AS_LOCAL :
			count = &cls.numlocalservers;
			servers = &cls.localServers[0];
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			count = &cls.numglobalservers;
			servers = &cls.globalServers[0];
			break;
		case AS_FAVORITES :
			count = &cls.numfavoriteservers;
			servers = &cls.favoriteServers[0];
			break;
	}
	if (servers) {
		netadr_t comp;
		NET_StringToAdr( addr, &comp, NA_UNSPEC );
		for (i = 0; i < *count; i++) {
			if (NET_CompareAdr( comp, servers[i].adr)) {
				int j = i;
				while (j < *count - 1) {
					Com_Memcpy(&servers[j], &servers[j+1], sizeof(servers[j]));
					j++;
				}
				(*count)--;
				break;
			}
		}
	}
}


/*
====================
LAN_GetServerCount
====================
*/
static int LAN_GetServerCount( int source ) {
	switch (source) {
		case AS_LOCAL :
			return cls.numlocalservers;
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			return cls.numglobalservers;
			break;
		case AS_FAVORITES :
			return cls.numfavoriteservers;
			break;
	}
	return 0;
}

/*
====================
LAN_GetLocalServerAddressString
====================
*/
static void LAN_GetServerAddressString( int source, int n, char *buf, int buflen ) {
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToStringwPort( cls.localServers[n].adr) , buflen );
				return;
			}
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				Q_strncpyz(buf, NET_AdrToStringwPort( cls.globalServers[n].adr) , buflen );
				return;
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToStringwPort( cls.favoriteServers[n].adr) , buflen );
				return;
			}
			break;
	}
	buf[0] = '\0';
}

/*
====================
LAN_GetServerInfo
====================
*/
static void LAN_GetServerInfo( int source, int n, char *buf, int buflen ) {
	char info[MAX_STRING_CHARS];
	serverInfo_t *server = NULL;
	info[0] = '\0';
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.localServers[n];
			}
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				server = &cls.globalServers[n];
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.favoriteServers[n];
			}
			break;
	}
	if (server && buf) {
		buf[0] = '\0';
		Info_SetValueForKey( info, "hostname", server->hostName);
		Info_SetValueForKey( info, "mapname", server->mapName);
		Info_SetValueForKey( info, "clients", va("%i",server->clients));
		Info_SetValueForKey( info, "sv_maxclients", va("%i",server->maxClients));
		Info_SetValueForKey( info, "ping", va("%i",server->ping));
		Info_SetValueForKey( info, "minping", va("%i",server->minPing));
		Info_SetValueForKey( info, "maxping", va("%i",server->maxPing));
		Info_SetValueForKey( info, "game", server->game);
		Info_SetValueForKey( info, "gametype", va("%i",server->gameType));
		Info_SetValueForKey( info, "nettype", va("%i",server->netType));
		Info_SetValueForKey( info, "addr", NET_AdrToStringwPort(server->adr));
		Info_SetValueForKey( info, "punkbuster", va("%i", server->punkbuster));
		Info_SetValueForKey( info, "g_needpass", va("%i", server->g_needpass));
		Info_SetValueForKey( info, "g_humanplayers", va("%i", server->g_humanplayers));
		Q_strncpyz(buf, info, buflen);
	} else {
		if (buf) {
			buf[0] = '\0';
		}
	}
}

/*
====================
LAN_GetServerPing
====================
*/
static int LAN_GetServerPing( int source, int n ) {
	serverInfo_t *server = NULL;
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.localServers[n];
			}
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				server = &cls.globalServers[n];
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.favoriteServers[n];
			}
			break;
	}
	if (server) {
		return server->ping;
	}
	return -1;
}

/*
====================
LAN_GetServerPtr
====================
*/
static serverInfo_t *LAN_GetServerPtr( int source, int n ) {
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.localServers[n];
			}
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				return &cls.globalServers[n];
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.favoriteServers[n];
			}
			break;
	}
	return NULL;
}

/*
====================
LAN_CompareServers
====================
*/
static int LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 ) {
	int res;
	serverInfo_t *server1, *server2;
	int clients1, clients2;

	server1 = LAN_GetServerPtr(source, s1);
	server2 = LAN_GetServerPtr(source, s2);
	if (!server1 || !server2) {
		return 0;
	}

	res = 0;
	switch( sortKey ) {
		case SORT_HOST:
			res = Q_stricmp( server1->hostName, server2->hostName );
			break;

		case SORT_MAP:
			res = Q_stricmp( server1->mapName, server2->mapName );
			break;
		case SORT_CLIENTS:
			// sub sort by max clients
			if ( server1->clients == server2->clients ) {
				clients1 = server1->maxClients;
				clients2 = server2->maxClients;
			} else {
				clients1 = server1->clients;
				clients2 = server2->clients;
			}

			if (clients1 < clients2) {
				res = -1;
			}
			else if (clients1 > clients2) {
				res = 1;
			}
			else {
				res = 0;
			}
			break;
		case SORT_GAME:
			if (server1->gameType < server2->gameType) {
				res = -1;
			}
			else if (server1->gameType > server2->gameType) {
				res = 1;
			}
			else {
				res = 0;
			}
			break;
		case SORT_PING:
			if (server1->ping < server2->ping) {
				res = -1;
			}
			else if (server1->ping > server2->ping) {
				res = 1;
			}
			else {
				res = 0;
			}
			break;
	}

	if (sortDir) {
		if (res < 0)
			return 1;
		if (res > 0)
			return -1;
		return 0;
	}
	return res;
}

/*
====================
LAN_GetPingQueueCount
====================
*/
static int LAN_GetPingQueueCount( void ) {
	return (CL_GetPingQueueCount());
}

/*
====================
LAN_ClearPing
====================
*/
static void LAN_ClearPing( int n ) {
	CL_ClearPing( n );
}

/*
====================
LAN_GetPing
====================
*/
static void LAN_GetPing( int n, char *buf, int buflen, int *pingtime ) {
	CL_GetPing( n, buf, buflen, pingtime );
}

/*
====================
LAN_GetPingInfo
====================
*/
static void LAN_GetPingInfo( int n, char *buf, int buflen ) {
	CL_GetPingInfo( n, buf, buflen );
}

/*
====================
LAN_MarkServerVisible
====================
*/
static void LAN_MarkServerVisible(int source, int n, qboolean visible ) {
	if (n == -1) {
		int count = MAX_OTHER_SERVERS;
		serverInfo_t *server = NULL;
		switch (source) {
			case AS_LOCAL :
				server = &cls.localServers[0];
				break;
			case AS_MPLAYER:
			case AS_GLOBAL :
				server = &cls.globalServers[0];
				count = MAX_GLOBAL_SERVERS;
				break;
			case AS_FAVORITES :
				server = &cls.favoriteServers[0];
				break;
		}
		if (server) {
			for (n = 0; n < count; n++) {
				server[n].visible = visible;
			}
		}

	} else {
		switch (source) {
			case AS_LOCAL :
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.localServers[n].visible = visible;
				}
				break;
			case AS_MPLAYER:
			case AS_GLOBAL :
				if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
					cls.globalServers[n].visible = visible;
				}
				break;
			case AS_FAVORITES :
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.favoriteServers[n].visible = visible;
				}
				break;
		}
	}
}


/*
=======================
LAN_ServerIsVisible
=======================
*/
static int LAN_ServerIsVisible(int source, int n ) {
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.localServers[n].visible;
			}
			break;
		case AS_MPLAYER:
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				return cls.globalServers[n].visible;
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.favoriteServers[n].visible;
			}
			break;
	}
	return qfalse;
}

/*
=======================
LAN_UpdateVisiblePings
=======================
*/
qboolean LAN_UpdateVisiblePings(int source ) {
	return CL_UpdateVisiblePings_f(source);
}

/*
====================
LAN_GetServerStatus
====================
*/
int LAN_GetServerStatus( char *serverAddress, char *serverStatus, int maxLen ) {
	return CL_ServerStatus( serverAddress, serverStatus, maxLen );
}

/*
====================
CL_GetGlConfig
====================
*/
static void CL_GetGlconfig( glconfig_t *config ) {
	*config = cls.glconfig;
}

/*
====================
CL_GetClipboardData
====================
*/
static void CL_GetClipboardData( char *buf, int buflen ) {
	char	*cbd;

	cbd = Sys_GetClipboardData();

	if ( !cbd ) {
		*buf = 0;
		return;
	}

	Q_strncpyz( buf, cbd, buflen );

	Z_Free( cbd );
}

/*
====================
Key_KeynumToStringBuf
====================
*/
static void Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) {
	Q_strncpyz( buf, Key_KeynumToString( keynum ), buflen );
}

/*
====================
Key_GetBindingBuf
====================
*/
static void Key_GetBindingBuf( int keynum, char *buf, int buflen ) {
	char	*value;

	value = Key_GetBinding( keynum );
	if ( value ) {
		Q_strncpyz( buf, value, buflen );
	}
	else {
		*buf = 0;
	}
}

/*
====================
CLUI_GetCDKey
====================
*/
static void CLUI_GetCDKey( char *buf, int buflen ) {
#ifndef STANDALONE
	const char *gamedir;
	gamedir = Cvar_VariableString( "fs_game" );
	if (UI_usesUniqueCDKey() && gamedir[0] != 0) {
		Com_Memcpy( buf, &cl_cdkey[16], 16);
		buf[16] = 0;
	} else {
		Com_Memcpy( buf, cl_cdkey, 16);
		buf[16] = 0;
	}
#else
	*buf = 0;
#endif
}


/*
====================
CLUI_SetCDKey
====================
*/
#ifndef STANDALONE
static void CLUI_SetCDKey( char *buf ) {
	const char *gamedir;
	gamedir = Cvar_VariableString( "fs_game" );
	if (UI_usesUniqueCDKey() && gamedir[0] != 0) {
		Com_Memcpy( &cl_cdkey[16], buf, 16 );
		cl_cdkey[32] = 0;
		// set the flag so the fle will be written at the next opportunity
		cvar_modifiedFlags |= CVAR_ARCHIVE;
	} else {
		Com_Memcpy( cl_cdkey, buf, 16 );
		// set the flag so the fle will be written at the next opportunity
		cvar_modifiedFlags |= CVAR_ARCHIVE;
	}
}
#endif

/*
====================
GetConfigString
====================
*/
static int GetConfigString(int index, char *buf, int size)
{
	int		offset;

	if (index < 0 || index >= MAX_CONFIGSTRINGS)
		return qfalse;

	offset = cl.gameState.stringOffsets[index];
	if (!offset) {
		if( size ) {
			buf[0] = 0;
		}
		return qfalse;
	}

	Q_strncpyz( buf, cl.gameState.stringData+offset, size);
 
	return qtrue;
}

/*
====================
FloatAsInt
====================
*/
static int FloatAsInt( float f ) {
	floatint_t fi;
	fi.f = f;
	return fi.i;
}

/*
====================
CL_UISystemCalls

The ui module is making a system call
====================
*/
intptr_t CL_UISystemCalls( intptr_t *args ) {
	switch( args[0] ) {
	case UI_ERROR:
		Com_Error( ERR_DROP, "%s", (const char*)VMA(1) );
		return 0;

	case UI_PRINT:
		Com_Printf( "%s", (const char*)VMA(1) );
		return 0;

	case UI_MILLISECONDS:
		return Sys_Milliseconds();

	case UI_CVAR_REGISTER:
		Cvar_Register( VMA(1), VMA(2), VMA(3), args[4] ); 
		return 0;

	case UI_CVAR_UPDATE:
		Cvar_Update( VMA(1) );
		return 0;

	case UI_CVAR_SET:
		Cvar_SetSafe( VMA(1), VMA(2) );
		return 0;

	case UI_CVAR_VARIABLEVALUE:
		return FloatAsInt( Cvar_VariableValue( VMA(1) ) );

	case UI_CVAR_VARIABLESTRINGBUFFER:
		Cvar_VariableStringBuffer( VMA(1), VMA(2), args[3] );
		return 0;

	case UI_CVAR_SETVALUE:
		Cvar_SetValueSafe( VMA(1), VMF(2) );
		return 0;

	case UI_CVAR_RESET:
		Cvar_Reset( VMA(1) );
		return 0;

	case UI_CVAR_CREATE:
		Cvar_Register( NULL, VMA(1), VMA(2), args[3] );
		return 0;

	case UI_CVAR_INFOSTRINGBUFFER:
		Cvar_InfoStringBuffer( args[1], VMA(2), args[3] );
		return 0;

	case UI_ARGC:
		return Cmd_Argc();

	case UI_ARGV:
		Cmd_ArgvBuffer( args[1], VMA(2), args[3] );
		return 0;

	case UI_CMD_EXECUTETEXT:
		if(args[1] == EXEC_NOW
		&& (!strncmp(VMA(2), "snd_restart", 11)
		|| !strncmp(VMA(2), "vid_restart", 11)
		|| !strncmp(VMA(2), "quit", 5)))
		{
			Com_Printf (S_COLOR_YELLOW "turning EXEC_NOW '%.11s' into EXEC_INSERT\n", (const char*)VMA(2));
			args[1] = EXEC_INSERT;
		}
		Cbuf_ExecuteText( args[1], VMA(2) );
		return 0;

	case UI_FS_FOPENFILE:
		return FS_FOpenFileByMode( VMA(1), VMA(2), args[3] );

	case UI_FS_READ:
		FS_Read( VMA(1), args[2], args[3] );
		return 0;

	case UI_FS_WRITE:
		FS_Write( VMA(1), args[2], args[3] );
		return 0;

	case UI_FS_FCLOSEFILE:
		FS_FCloseFile( args[1] );
		return 0;

	case UI_FS_GETFILELIST:
		return FS_GetFileList( VMA(1), VMA(2), VMA(3), args[4] );

	case UI_FS_SEEK:
		return FS_Seek( args[1], args[2], args[3] );
	
	case UI_R_REGISTERMODEL:
		return re.RegisterModel( VMA(1) );

	case UI_R_REGISTERSKIN:
		return re.RegisterSkin( VMA(1) );

	case UI_R_REGISTERSHADERNOMIP:
		return re.RegisterShaderNoMip( VMA(1) );

	case UI_R_CLEARSCENE:
		re.ClearScene();
		return 0;

	case UI_R_ADDREFENTITYTOSCENE:
		re.AddRefEntityToScene( VMA(1) );
		return 0;

	case UI_R_ADDPOLYTOSCENE:
		re.AddPolyToScene( args[1], args[2], VMA(3), 1 );
		return 0;

	case UI_R_ADDLIGHTTOSCENE:
		re.AddLightToScene( VMA(1), VMF(2), VMF(3), VMF(4), VMF(5) );
		return 0;

	case UI_R_RENDERSCENE:
		re.RenderScene( VMA(1) );
		return 0;

	case UI_R_SETCOLOR:
		re.SetColor( VMA(1) );
		return 0;

	case UI_R_DRAWSTRETCHPIC:
		re.DrawStretchPic( VMF(1), VMF(2), VMF(3), VMF(4), VMF(5), VMF(6), VMF(7), VMF(8), args[9] );
		return 0;

  case UI_R_MODELBOUNDS:
		re.ModelBounds( args[1], VMA(2), VMA(3) );
		return 0;

	case UI_UPDATESCREEN:
		SCR_UpdateScreen();
		return 0;

	case UI_CM_LERPTAG:
		re.LerpTag( VMA(1), args[2], args[3], args[4], VMF(5), VMA(6) );
		return 0;

	case UI_S_REGISTERSOUND:
		return S_RegisterSound( VMA(1), args[2] );

	case UI_S_STARTLOCALSOUND:
		S_StartLocalSound( args[1], args[2] );
		return 0;

	case UI_KEY_KEYNUMTOSTRINGBUF:
		Key_KeynumToStringBuf( args[1], VMA(2), args[3] );
		return 0;

	case UI_KEY_GETBINDINGBUF:
		Key_GetBindingBuf( args[1], VMA(2), args[3] );
		return 0;

	case UI_KEY_SETBINDING:
		Key_SetBinding( args[1], VMA(2) );
		return 0;

	case UI_KEY_ISDOWN:
		return Key_IsDown( args[1] );

	case UI_KEY_GETOVERSTRIKEMODE:
		return Key_GetOverstrikeMode();

	case UI_KEY_SETOVERSTRIKEMODE:
		Key_SetOverstrikeMode( args[1] );
		return 0;

	case UI_KEY_CLEARSTATES:
		Key_ClearStates();
		return 0;

	case UI_KEY_GETCATCHER:
		return Key_GetCatcher();

	case UI_KEY_SETCATCHER:
		// Don't allow the ui module to close the console
		Key_SetCatcher( args[1] | ( Key_GetCatcher( ) & KEYCATCH_CONSOLE ) );
		return 0;

	case UI_GETCLIPBOARDDATA:
		CL_GetClipboardData( VMA(1), args[2] );
		return 0;

	case UI_GETCLIENTSTATE:
		GetClientState( VMA(1) );
		return 0;		

	case UI_GETGLCONFIG:
		CL_GetGlconfig( VMA(1) );
		return 0;

	case UI_GETCONFIGSTRING:
		return GetConfigString( args[1], VMA(2), args[3] );

	case UI_LAN_LOADCACHEDSERVERS:
		LAN_LoadCachedServers();
		return 0;

	case UI_LAN_SAVECACHEDSERVERS:
		LAN_SaveServersToCache();
		return 0;

	case UI_LAN_ADDSERVER:
		return LAN_AddServer(args[1], VMA(2), VMA(3));

	case UI_LAN_REMOVESERVER:
		LAN_RemoveServer(args[1], VMA(2));
		return 0;

	case UI_LAN_GETPINGQUEUECOUNT:
		return LAN_GetPingQueueCount();

	case UI_LAN_CLEARPING:
		LAN_ClearPing( args[1] );
		return 0;

	case UI_LAN_GETPING:
		LAN_GetPing( args[1], VMA(2), args[3], VMA(4) );
		return 0;

	case UI_LAN_GETPINGINFO:
		LAN_GetPingInfo( args[1], VMA(2), args[3] );
		return 0;

	case UI_LAN_GETSERVERCOUNT:
		return LAN_GetServerCount(args[1]);

	case UI_LAN_GETSERVERADDRESSSTRING:
		LAN_GetServerAddressString( args[1], args[2], VMA(3), args[4] );
		return 0;

	case UI_LAN_GETSERVERINFO:
		LAN_GetServerInfo( args[1], args[2], VMA(3), args[4] );
		return 0;

	case UI_LAN_GETSERVERPING:
		return LAN_GetServerPing( args[1], args[2] );

	case UI_LAN_MARKSERVERVISIBLE:
		LAN_MarkServerVisible( args[1], args[2], args[3] );
		return 0;

	case UI_LAN_SERVERISVISIBLE:
		return LAN_ServerIsVisible( args[1], args[2] );

	case UI_LAN_UPDATEVISIBLEPINGS:
		return LAN_UpdateVisiblePings( args[1] );

	case UI_LAN_RESETPINGS:
		LAN_ResetPings( args[1] );
		return 0;

	case UI_LAN_SERVERSTATUS:
		return LAN_GetServerStatus( VMA(1), VMA(2), args[3] );

	case UI_LAN_COMPARESERVERS:
		return LAN_CompareServers( args[1], args[2], args[3], args[4], args[5] );

	case UI_MEMORY_REMAINING:
		return Hunk_MemoryRemaining();

	case UI_GET_CDKEY:
		CLUI_GetCDKey( VMA(1), args[2] );
		return 0;

	case UI_SET_CDKEY:
#ifndef STANDALONE
		CLUI_SetCDKey( VMA(1) );
#endif
		return 0;
	
	case UI_SET_PBCLSTATUS:
		return 0;	

        case UI_REQUEST_LADDERDATA:
                CL_LadderRequestData( (const char *)VMA(1), (const char *)VMA(2), (const char *)VMA(3) );
                return 0;

        case UI_CANCEL_LADDERREQUEST:
                CL_LadderCancelRequest();
                return 0;

        case UI_GET_LADDERSTATUS:
                if ( VMA(1) ) {
                        Com_Memcpy( VMA(1), &cl_ladderStatus, sizeof( cl_ladderStatus ) );
                }
		return 0;

	case UI_R_REGISTERFONT:
		re.RegisterFont( VMA(1), args[2], VMA(3));
		return 0;

	case UI_MEMSET:
		Com_Memset( VMA(1), args[2], args[3] );
		return 0;

	case UI_MEMCPY:
		Com_Memcpy( VMA(1), VMA(2), args[3] );
		return 0;

	case UI_STRNCPY:
		strncpy( VMA(1), VMA(2), args[3] );
		return args[1];

	case UI_SIN:
		return FloatAsInt( sin( VMF(1) ) );

	case UI_COS:
		return FloatAsInt( cos( VMF(1) ) );

	case UI_ATAN2:
		return FloatAsInt( atan2( VMF(1), VMF(2) ) );

	case UI_SQRT:
		return FloatAsInt( sqrt( VMF(1) ) );

	case UI_FLOOR:
		return FloatAsInt( floor( VMF(1) ) );

	case UI_CEIL:
		return FloatAsInt( ceil( VMF(1) ) );

	case UI_PC_ADD_GLOBAL_DEFINE:
		return botlib_export->PC_AddGlobalDefine( VMA(1) );
	case UI_PC_LOAD_SOURCE:
		return botlib_export->PC_LoadSourceHandle( VMA(1) );
	case UI_PC_FREE_SOURCE:
		return botlib_export->PC_FreeSourceHandle( args[1] );
	case UI_PC_READ_TOKEN:
		return botlib_export->PC_ReadTokenHandle( args[1], VMA(2) );
	case UI_PC_SOURCE_FILE_AND_LINE:
		return botlib_export->PC_SourceFileAndLine( args[1], VMA(2), VMA(3) );

	case UI_S_STOPBACKGROUNDTRACK:
		S_StopBackgroundTrack();
		return 0;
	case UI_S_STARTBACKGROUNDTRACK:
		S_StartBackgroundTrack( VMA(1), VMA(2));
		return 0;

	case UI_REAL_TIME:
		return Com_RealTime( VMA(1) );

	case UI_CIN_PLAYCINEMATIC:
	  Com_DPrintf("UI_CIN_PlayCinematic\n");
	  return CIN_PlayCinematic(VMA(1), args[2], args[3], args[4], args[5], args[6]);

	case UI_CIN_STOPCINEMATIC:
	  return CIN_StopCinematic(args[1]);

	case UI_CIN_RUNCINEMATIC:
	  return CIN_RunCinematic(args[1]);

	case UI_CIN_DRAWCINEMATIC:
	  CIN_DrawCinematic(args[1]);
	  return 0;

	case UI_CIN_SETEXTENTS:
	  CIN_SetExtents(args[1], args[2], args[3], args[4], args[5]);
	  return 0;

	case UI_R_REMAP_SHADER:
		re.RemapShader( VMA(1), VMA(2), VMA(3) );
		return 0;

	case UI_VERIFY_CDKEY:
		return CL_CDKeyValidate(VMA(1), VMA(2));
		
	default:
		Com_Error( ERR_DROP, "Bad UI system trap: %ld", (long int) args[0] );

	}

	return 0;
}

/*
====================
CL_ShutdownUI
====================
*/
void CL_ShutdownUI( void ) {
        Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_UI );
        cls.uiStarted = qfalse;
#ifdef USE_CURL
        CL_LadderCleanupRequest( qtrue );
#endif
        CL_UpdateShutdown();
        CL_LadderResetStatus();
        if ( !uivm ) {
                return;
        }
	VM_Call( uivm, UI_SHUTDOWN );
	VM_Free( uivm );
	uivm = NULL;
}

/*
====================
CL_InitUI
====================
*/
#define UI_OLD_API_VERSION	4

void CL_InitUI( void ) {
	int		v;
	vmInterpret_t		interpret;
    
    CL_UpdateEnsureCvars();
	CL_UpdateResetStatus();
	CL_LadderResetStatus();

	// load the dll or bytecode
	interpret = Cvar_VariableValue("vm_ui");
	if(cl_connectedToPureServer)
	{
		// if sv_pure is set we only allow qvms to be loaded
		if(interpret != VMI_COMPILED && interpret != VMI_BYTECODE)
			interpret = VMI_COMPILED;
	}

	uivm = VM_Create( "ui", CL_UISystemCalls, interpret );
	if ( !uivm ) {
		Com_Error( ERR_FATAL, "VM_Create on UI failed" );
	}

	// sanity check
	v = VM_Call( uivm, UI_GETAPIVERSION );
	if (v == UI_OLD_API_VERSION) {
//		Com_Printf(S_COLOR_YELLOW "WARNING: loading old Quake III Arena User Interface version %d\n", v );
		// init for this gamestate
		VM_Call( uivm, UI_INIT, (clc.state >= CA_AUTHORIZING && clc.state < CA_ACTIVE));
	}
	else if (v != UI_API_VERSION) {
		// Free uivm now, so UI_SHUTDOWN doesn't get called later.
		VM_Free( uivm );
		uivm = NULL;

		Com_Error( ERR_DROP, "User Interface is version %d, expected %d", v, UI_API_VERSION );
		cls.uiStarted = qfalse;
	}
	else {
		// init for this gamestate
		VM_Call( uivm, UI_INIT, (clc.state >= CA_AUTHORIZING && clc.state < CA_ACTIVE) );
	}

	CL_UpdateRequestLatest();
}

#ifndef STANDALONE
qboolean UI_usesUniqueCDKey( void ) {
	if (uivm) {
		return (VM_Call( uivm, UI_HASUNIQUECDKEY) == qtrue);
	} else {
		return qfalse;
	}
}
#endif

/*
====================
UI_GameCommand

See if the current console command is claimed by the ui
====================
*/
qboolean UI_GameCommand( void ) {
	if ( !uivm ) {
		return qfalse;
	}

	return VM_Call( uivm, UI_CONSOLE_COMMAND, cls.realtime );
}
