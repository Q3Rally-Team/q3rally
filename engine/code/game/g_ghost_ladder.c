/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
===========================================================================
*/
// g_ghost_ladder.c -- ladder ghosts as opponents in Ghost Race
//
// In GT_GHOST the server fetches the ghost ranking of the current map and
// track variant from the ladder (via the engine, trap_LadderFetchGhosts) and
// sends it to every client. A client picks one entry ("lghostpick <idx>"),
// the server downloads that ghost into the ghosts/ladder/ cache if needed,
// parses it and streams it to that client with position and angles.
//
// The engine keeps the list and every downloaded ghost in ghosts/ladder/, so
// a server without network access still offers the ghosts it saw before.
//
// Wire protocol (server -> client, a few commands per client and frame):
//   lghostlist <total> <fromCache>
//   lghostents <first> <count> ( <lapMs> <vehicle> "<name>" <cacheFile> )*
//   lghostlistdone
//   lghostmeta <idx> <lapMs> <samples>
//   lghostdata <first> <count> ( <t> <x> <y> <z> <pitch> <yaw> <roll> )*  (whole units/degrees)
//   lghostdone <idx>
//   lghostfail <idx> <reason>
//   lghostpickok <idx>      pick taken (meta/data or fail follow)
//   lghostoppok <lapMs>     "ghostopp" taken (echoes the client's lap time)
// client -> server (the client repeats them until the answer arrives,
// a dedicated server drops commands that come within a second):
//   lghostlistreq           send the list again (cgame restart), answer: lghostlist
//   lghostpick <idx>        stream this ghost
//   ghostopp <lapMs> "<name>" [<idx>]  the ghost this driver races (any ghost
//                           type); idx >= 0: ladder entry, the server uses the
//                           lap time and name of its own list
// Result at the finish (server -> all clients):
//   lghostresult <client> <won 0|1> <bestLapMs> <ghostLapMs> "<name>"

#include "g_local.h"

#define LGHOST_MAX_ENTRIES              64
#define LGHOST_POOL_SIZE                4
#define LGHOST_MAX_FRAMES               2048
#define LGHOST_ENTRIES_PER_COMMAND      4
/* 20 samples stay below the 1022 character command limit even with 7 digit
 * times and 6 character coordinates. One command per client and frame keeps
 * the reliable command window (64) safe even when a client stalls briefly. */
#define LGHOST_SAMPLES_PER_COMMAND      20
#define LGHOST_COMMANDS_PER_FRAME       1
#define LGHOST_POLL_INTERVAL            250
#define LGHOST_LIST_TIMEOUT             15000
#define LGHOST_DOWNLOAD_TIMEOUT         20000
#define LGHOST_MAX_LIST_FILE            ( 64 * 1024 )

typedef enum {
	LGHOST_LIST_OFF,                // not GT_GHOST or g_ghostDownload 0
	LGHOST_LIST_PENDING,            // waiting for the first frame
	LGHOST_LIST_FETCHING,           // engine is downloading the ranking
	LGHOST_LIST_READY
} lghostListState_t;

typedef struct {
	char            cacheFile[MAX_QPATH];
	char            ghostId[LADDER_FETCH_MAX_ID];
	int             lapMs;
	char            vehicle[32];
	char            name[40];
	int             downloadRequestId;      // 0 = no download running
	int             downloadStartedAt;
	qboolean        downloadFailed;
} lghostEntry_t;

typedef struct {
	int             timeOffset;
	vec3_t          origin;
	vec3_t          angles;
} lghostFrame_t;

typedef struct {
	int             entry;                  // -1 = free
	int             lastUsed;
	int             frameCount;
	lghostFrame_t   frames[LGHOST_MAX_FRAMES];
} lghostParsed_t;

typedef enum {
	LGHOST_PICK_IDLE,
	LGHOST_PICK_WAITING,            // ghost not parsed yet (download running)
	LGHOST_PICK_STREAMING
} lghostPickState_t;

typedef struct {
	int             opponentLapMs;          // ghost lap the driver races, 0 = none
	char            opponentName[40];
	qboolean        listWanted;             // send the list when it is ready
	int             listNext;               // -1 = header not sent yet
	lghostPickState_t pickState;
	int             pickEntry;
	int             pickQueuedAt;
	int             pool;
	int             sendNext;               // -1 = meta not sent yet
} lghostClient_t;

static lghostListState_t s_listState;
static qboolean         s_listFromCache;
static int              s_listRequestId;
static int              s_listRequestedAt;
static int              s_lastPoll;
static char             s_mapName[MAX_QPATH];
static char             s_listPath[MAX_QPATH];
static lghostEntry_t    s_entries[LGHOST_MAX_ENTRIES];
static int              s_entryCount;
static lghostParsed_t   s_pool[LGHOST_POOL_SIZE];
static lghostClient_t   s_clients[MAX_CLIENTS];
static int              s_nextRequestId;
#define s_fileBuffer     g_ghostTextBuffer   /* shared, see g_local.h */

static qboolean G_GhostLadder_Enabled( void ) {
	return g_gametype.integer == GT_GHOST && g_ghostDownload.integer != 0;
}

static int G_GhostLadder_NextRequestId( void ) {
	s_nextRequestId++;
	if ( s_nextRequestId <= 0 || s_nextRequestId > 1000000 ) {
		s_nextRequestId = 1;
	}
	return ( ( trap_Milliseconds() & 0x3ff ) << 20 ) | s_nextRequestId;
}

/* Reads a whole file into s_fileBuffer; returns its length or -1. */
static int G_GhostLadder_ReadFile( const char *path, int maxLength ) {
	fileHandle_t f;
	int length;

	length = trap_FS_FOpenFile( path, &f, FS_READ );
	if ( length <= 0 || !f ) {
		if ( f ) {
			trap_FS_FCloseFile( f );
		}
		return -1;
	}
	if ( length > maxLength ) {
		trap_FS_FCloseFile( f );
		return -1;
	}
	trap_FS_Read( s_fileBuffer, length, f );
	trap_FS_FCloseFile( f );
	s_fileBuffer[length] = '\0';
	return length;
}

static qboolean G_GhostLadder_FileExists( const char *path ) {
	fileHandle_t f;
	int length;

	length = trap_FS_FOpenFile( path, &f, FS_READ );
	if ( f ) {
		trap_FS_FCloseFile( f );
	}
	return length > 0;
}

/* Copies one tab separated field and returns the start of the next one. */
static char *G_GhostLadder_Field( char *cursor, char *out, int outSize ) {
	int n = 0;

	while ( *cursor && *cursor != '\t' && *cursor != '\n' && *cursor != '\r' ) {
		char c = *cursor++;
		if ( c == '"' || c == '\\' || c == ';' || (unsigned char)c < 0x20 ) {
			continue;
		}
		if ( n < outSize - 1 ) {
			out[n++] = c;
		}
	}
	out[n] = '\0';
	if ( *cursor == '\t' ) {
		cursor++;
	}
	return cursor;
}

static qboolean G_GhostLadder_SafeToken( const char *text ) {
	if ( !text[0] ) {
		return qfalse;
	}
	for ( ; *text; text++ ) {
		if ( *text <= ' ' || *text == '"' ) {
			return qfalse;
		}
	}
	return qtrue;
}

/* Parses the list file written by the engine (see sv_ladder.c). */
static void G_GhostLadder_LoadList( void ) {
	char *cursor;
	int i;

	s_entryCount = 0;
	if ( G_GhostLadder_ReadFile( s_listPath, LGHOST_MAX_LIST_FILE ) < 0 ) {
		return;
	}

	cursor = s_fileBuffer;
	while ( *cursor && s_entryCount < LGHOST_MAX_ENTRIES ) {
		lghostEntry_t entry;
		char lapText[16];

		memset( &entry, 0, sizeof( entry ) );
		cursor = G_GhostLadder_Field( cursor, entry.cacheFile, sizeof( entry.cacheFile ) );
		cursor = G_GhostLadder_Field( cursor, entry.ghostId, sizeof( entry.ghostId ) );
		cursor = G_GhostLadder_Field( cursor, lapText, sizeof( lapText ) );
		cursor = G_GhostLadder_Field( cursor, entry.vehicle, sizeof( entry.vehicle ) );
		cursor = G_GhostLadder_Field( cursor, entry.name, sizeof( entry.name ) );
		while ( *cursor && *cursor != '\n' ) {
			cursor++;
		}
		if ( *cursor == '\n' ) {
			cursor++;
		}

		entry.lapMs = atoi( lapText );
		if ( entry.lapMs <= 0 || Q_stricmpn( entry.cacheFile, "ghosts/ladder/", 14 ) ||
		     strstr( entry.cacheFile, ".." ) || !G_GhostLadder_SafeToken( entry.cacheFile ) ||
		     !G_GhostLadder_SafeToken( entry.ghostId ) || !G_GhostLadder_SafeToken( entry.vehicle ) ) {
			continue;
		}
		if ( !entry.name[0] ) {
			Q_strncpyz( entry.name, "Player", sizeof( entry.name ) );
		}
		s_entries[s_entryCount++] = entry;
	}

	/* The pool refers to entry indices of the old list. */
	for ( i = 0; i < LGHOST_POOL_SIZE; i++ ) {
		s_pool[i].entry = -1;
		s_pool[i].frameCount = 0;
	}
}

static void G_GhostLadder_ResetClient( int clientNum ) {
	lghostClient_t *client = &s_clients[clientNum];

	memset( client, 0, sizeof( *client ) );
	client->listNext = -1;
	client->pickEntry = -1;
	client->pool = -1;
	client->sendNext = -1;
}

static void G_GhostLadder_SendFail( int clientNum, int entry, const char *reason ) {
	trap_SendServerCommand( clientNum, va( "lghostfail %d %s", entry, reason ) );
	s_clients[clientNum].pickState = LGHOST_PICK_IDLE;
	s_clients[clientNum].pickEntry = -1;
	s_clients[clientNum].pool = -1;
}

/*
=================
G_GhostLadder_ParseGhost

Parses a .ghost file into a pool slot, at most LGHOST_MAX_FRAMES frames
(evenly thinned out). Frame lines: t ox oy oz ax ay az [vx vy vz buttons fwd up].
=================
*/
static qboolean G_GhostLadder_ParseGhost( const char *path, lghostParsed_t *out ) {
	static int      offsets[8192];
	static float    values[8192][6];
	char            *cursor;
	char            mapName[MAX_QPATH];
	int             total = 0;
	int             i;

	if ( G_GhostLadder_ReadFile( path, LADDER_GHOST_MAX_DATA ) < 0 ) {
		return qfalse;
	}

	mapName[0] = '\0';
	cursor = s_fileBuffer;
	while ( *cursor ) {
		char *line = cursor;
		char *end = cursor;

		while ( *end && *end != '\n' ) {
			end++;
		}
		cursor = *end ? end + 1 : end;
		*end = '\0';

		while ( *line == ' ' || *line == '\t' ) {
			line++;
		}
		if ( !Q_stricmpn( line, "map ", 4 ) ) {
			Q_strncpyz( mapName, line + 4, sizeof( mapName ) );
			for ( i = 0; mapName[i]; i++ ) {
				if ( mapName[i] == '\r' || mapName[i] == ' ' ) {
					mapName[i] = '\0';
					break;
				}
			}
			continue;
		}
		if ( ( *line >= '0' && *line <= '9' ) && total < 8192 ) {
			char *token = line;
			int t;
			int field;
			float parsed[6];

			t = atoi( token );
			for ( field = 0; field < 6; field++ ) {
				while ( *token && *token != ' ' && *token != '\t' ) {
					token++;
				}
				while ( *token == ' ' || *token == '\t' ) {
					token++;
				}
				if ( !*token ) {
					break;
				}
				parsed[field] = atof( token );
			}
			if ( field < 6 || ( total > 0 && t < offsets[total - 1] ) ) {
				continue;
			}
			offsets[total] = t;
			for ( field = 0; field < 6; field++ ) {
				values[total][field] = parsed[field];
			}
			total++;
		}
	}

	if ( total < 2 || ( mapName[0] && Q_stricmp( mapName, s_mapName ) ) ) {
		return qfalse;
	}

	out->frameCount = total > LGHOST_MAX_FRAMES ? LGHOST_MAX_FRAMES : total;
	for ( i = 0; i < out->frameCount; i++ ) {
		int src = ( out->frameCount == total ) ? i :
			(int)( (float)i * (float)( total - 1 ) / (float)( out->frameCount - 1 ) + 0.5f );
		lghostFrame_t *frame = &out->frames[i];

		if ( src >= total ) {
			src = total - 1;
		}
		frame->timeOffset = offsets[src] - offsets[0];
		VectorSet( frame->origin, values[src][0], values[src][1], values[src][2] );
		VectorSet( frame->angles, AngleNormalize180( values[src][3] ),
			AngleNormalize180( values[src][4] ), AngleNormalize180( values[src][5] ) );
	}
	return qtrue;
}

static int G_GhostLadder_FindPool( int entry ) {
	int i;

	for ( i = 0; i < LGHOST_POOL_SIZE; i++ ) {
		if ( s_pool[i].entry == entry && s_pool[i].frameCount > 1 ) {
			return i;
		}
	}
	return -1;
}

/* Free or least recently used slot that nobody is streaming right now. */
static int G_GhostLadder_AllocPool( void ) {
	int best = -1;
	int i;
	int c;

	for ( i = 0; i < LGHOST_POOL_SIZE; i++ ) {
		qboolean busy = qfalse;

		for ( c = 0; c < MAX_CLIENTS; c++ ) {
			if ( s_clients[c].pickState == LGHOST_PICK_STREAMING && s_clients[c].pool == i ) {
				busy = qtrue;
				break;
			}
		}
		if ( busy ) {
			continue;
		}
		if ( s_pool[i].entry < 0 ) {
			return i;
		}
		if ( best < 0 || s_pool[i].lastUsed < s_pool[best].lastUsed ) {
			best = i;
		}
	}
	return best;
}

/* Returns a pool slot holding the entry, or -1 while it is not available. */
static int G_GhostLadder_Acquire( int entryIndex ) {
	lghostEntry_t *entry = &s_entries[entryIndex];
	int pool = G_GhostLadder_FindPool( entryIndex );

	if ( pool >= 0 ) {
		s_pool[pool].lastUsed = level.time;
		return pool;
	}
	if ( !G_GhostLadder_FileExists( entry->cacheFile ) ) {
		return -1;
	}
	pool = G_GhostLadder_AllocPool();
	if ( pool < 0 ) {
		return -1;
	}
	if ( !G_GhostLadder_ParseGhost( entry->cacheFile, &s_pool[pool] ) ) {
		s_pool[pool].entry = -1;
		s_pool[pool].frameCount = 0;
		entry->downloadFailed = qtrue;
		G_Printf( "Ladder ghost %s is not usable\n", entry->ghostId );
		return -1;
	}
	s_pool[pool].entry = entryIndex;
	s_pool[pool].lastUsed = level.time;
	return pool;
}

static void G_GhostLadder_StartDownload( int entryIndex ) {
	lghostEntry_t *entry = &s_entries[entryIndex];
	ladderGhostFetch_t request;

	if ( entry->downloadRequestId ) {
		return;
	}
	memset( &request, 0, sizeof( request ) );
	request.kind = LADDER_FETCH_GHOST;
	request.requestId = G_GhostLadder_NextRequestId();
	Q_strncpyz( request.map, s_mapName, sizeof( request.map ) );
	Q_strncpyz( request.ghostId, entry->ghostId, sizeof( request.ghostId ) );
	Q_strncpyz( request.target, entry->cacheFile, sizeof( request.target ) );
	entry->downloadRequestId = request.requestId;
	entry->downloadStartedAt = level.time;
	entry->downloadFailed = qfalse;
	trap_LadderFetchGhosts( &request );
}

static void G_GhostLadder_RequestList( void ) {
	ladderGhostFetch_t request;

	memset( &request, 0, sizeof( request ) );
	request.kind = LADDER_FETCH_LIST;
	request.requestId = G_GhostLadder_NextRequestId();
	Q_strncpyz( request.map, s_mapName, sizeof( request.map ) );
	request.trackLength = ( g_trackLength.integer >= 0 && g_trackLength.integer <= 2 ) ? g_trackLength.integer : 0;
	request.trackReversed = g_trackReversed.integer ? 1 : 0;
	request.physicsVersion = BG_PHYSICS_VERSION;
	request.mapChecksum = trap_Cvar_VariableIntegerValue( "sv_mapChecksum" );
	Q_strncpyz( request.target, s_listPath, sizeof( request.target ) );

	s_listRequestId = request.requestId;
	s_listRequestedAt = level.time;
	s_listState = LGHOST_LIST_FETCHING;
	trap_Cvar_Set( "sv_ladderGhostList", "" );
	trap_LadderFetchGhosts( &request );
}

/* Status from the engine: "<requestId> ok|fail <count>" entries separated by
 * ';' (newest first; ghost downloads keep the last few results).
 * Returns 1 ok, -1 fail, 0 pending. */
static int G_GhostLadder_FetchStatus( const char *cvarName, int requestId ) {
	char status[MAX_CVAR_VALUE_STRING];
	char *cursor;

	trap_Cvar_VariableStringBuffer( cvarName, status, sizeof( status ) );
	cursor = status;
	while ( *cursor ) {
		char *next = cursor;

		while ( *next && *next != ';' ) {
			next++;
		}
		if ( *next ) {
			*next++ = '\0';
		}
		if ( atoi( cursor ) == requestId ) {
			while ( *cursor && *cursor != ' ' ) {
				cursor++;
			}
			while ( *cursor == ' ' ) {
				cursor++;
			}
			return !Q_stricmpn( cursor, "ok", 2 ) ? 1 : -1;
		}
		cursor = next;
	}
	return 0;
}

static void G_GhostLadder_ListReady( qboolean fromCache ) {
	int i;

	s_listState = LGHOST_LIST_READY;
	s_listFromCache = fromCache;
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		s_clients[i].listNext = -1;
		if ( s_clients[i].pickState != LGHOST_PICK_IDLE ) {
			/* Entry indices changed with the new list. */
			G_GhostLadder_SendFail( i, s_clients[i].pickEntry, "list_changed" );
		}
	}
	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED &&
		     !( g_entities[i].r.svFlags & SVF_BOT ) ) {
			s_clients[i].listWanted = qtrue;
		}
	}
	G_Printf( "Ladder ghosts: %d for %s%s\n", s_entryCount, s_mapName, fromCache ? " (cached)" : "" );
}

/*
 * Map name as a path component of at most maxLen characters. Longer names
 * become "<prefix>~<8 hex FNV-1a of the full name>", so the list path never
 * exceeds MAX_QPATH (it was cut off silently for map names > ~17 chars).
 * Short names stay unchanged, existing cache files keep their names.
 */
static void G_GhostLadder_MapComponent( const char *map, char *out, int outSize, int maxLen ) {
	unsigned int hash = 2166136261u;
	const char *p;
	int keep;

	if ( (int)strlen( map ) <= maxLen ) {
		Q_strncpyz( out, map, outSize );
		return;
	}
	for ( p = map; *p; p++ ) {
		hash ^= (unsigned char)*p;
		hash *= 16777619u;
	}
	keep = maxLen - 9;
	if ( keep >= outSize ) {
		keep = outSize - 1;
	}
	Q_strncpyz( out, map, keep + 1 );
	Com_sprintf( out + keep, outSize - keep, "~%08x", hash );
}

/*
=================
G_GhostLadder_Init

Called from G_InitGame after the ghost/bot route setup.
=================
*/
void G_GhostLadder_Init( void ) {
	int i;
	int trackLength;
	int trackReversed;

	s_listState = LGHOST_LIST_OFF;
	s_entryCount = 0;
	s_listFromCache = qfalse;
	s_lastPoll = 0;
	for ( i = 0; i < LGHOST_POOL_SIZE; i++ ) {
		s_pool[i].entry = -1;
		s_pool[i].frameCount = 0;
		s_pool[i].lastUsed = 0;
	}
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		G_GhostLadder_ResetClient( i );
	}

	if ( !G_GhostLadder_Enabled() ) {
		return;
	}

	trap_Cvar_VariableStringBuffer( "mapname", s_mapName, sizeof( s_mapName ) );
	Q_strlwr( s_mapName );
	trackLength = ( g_trackLength.integer >= 0 && g_trackLength.integer <= 2 ) ? g_trackLength.integer : 0;
	trackReversed = g_trackReversed.integer ? 1 : 0;
	{
		char mapPart[MAX_QPATH];

		/* "ghosts/ladder/" + map + "_tl0_rev0_p<3>_c-2147483648.list" */
		G_GhostLadder_MapComponent( s_mapName, mapPart, sizeof( mapPart ), 16 );
		Com_sprintf( s_listPath, sizeof( s_listPath ), "ghosts/ladder/%s_tl%d_rev%d_p%d_c%d.list",
			mapPart, trackLength, trackReversed, BG_PHYSICS_VERSION,
			trap_Cvar_VariableIntegerValue( "sv_mapChecksum" ) );
	}

	/* Offer what the cache holds right away; the download refreshes it. */
	G_GhostLadder_LoadList();
	s_listState = LGHOST_LIST_PENDING;
}

static void G_GhostLadder_SendList( int clientNum ) {
	lghostClient_t *client = &s_clients[clientNum];
	char command[MAX_STRING_CHARS];
	int offset;
	int count;
	int i;

	if ( client->listNext < 0 ) {
		trap_SendServerCommand( clientNum, va( "lghostlist %d %d", s_entryCount, s_listFromCache ? 1 : 0 ) );
		client->listNext = 0;
		return;
	}
	if ( client->listNext >= s_entryCount ) {
		trap_SendServerCommand( clientNum, "lghostlistdone" );
		client->listWanted = qfalse;
		client->listNext = -1;
		return;
	}

	count = s_entryCount - client->listNext;
	if ( count > LGHOST_ENTRIES_PER_COMMAND ) {
		count = LGHOST_ENTRIES_PER_COMMAND;
	}
	offset = Com_sprintf( command, sizeof( command ), "lghostents %d %d", client->listNext, count );
	for ( i = 0; i < count; i++ ) {
		const lghostEntry_t *entry = &s_entries[client->listNext + i];
		offset += Com_sprintf( command + offset, sizeof( command ) - offset, " %d %s \"%s\" %s",
			entry->lapMs, entry->vehicle, entry->name, entry->cacheFile );
	}
	trap_SendServerCommand( clientNum, command );
	client->listNext += count;
}

static void G_GhostLadder_SendGhostChunk( int clientNum ) {
	lghostClient_t *client = &s_clients[clientNum];
	lghostParsed_t *ghost = &s_pool[client->pool];
	char command[MAX_STRING_CHARS];
	int offset;
	int count;
	int i;

	if ( ghost->entry != client->pickEntry ) {
		G_GhostLadder_SendFail( clientNum, client->pickEntry, "evicted" );
		return;
	}
	if ( client->sendNext < 0 ) {
		trap_SendServerCommand( clientNum, va( "lghostmeta %d %d %d", client->pickEntry,
			s_entries[client->pickEntry].lapMs, ghost->frameCount ) );
		client->sendNext = 0;
		return;
	}
	if ( client->sendNext >= ghost->frameCount ) {
		trap_SendServerCommand( clientNum, va( "lghostdone %d", client->pickEntry ) );
		client->pickState = LGHOST_PICK_IDLE;
		client->pool = -1;
		return;
	}

	count = ghost->frameCount - client->sendNext;
	if ( count > LGHOST_SAMPLES_PER_COMMAND ) {
		count = LGHOST_SAMPLES_PER_COMMAND;
	}
	/* Samples first, the header gets the number that really fit. */
	{
		char samples[MAX_STRING_CHARS];
		const int limit = MAX_STRING_CHARS - 32;    /* room for the header */

		offset = 0;
		samples[0] = '\0';
		for ( i = 0; i < count; i++ ) {
			const lghostFrame_t *frame = &ghost->frames[client->sendNext + i];
			char sample[96];
			int length = Com_sprintf( sample, sizeof( sample ), " %d %.0f %.0f %.0f %.0f %.0f %.0f",
				frame->timeOffset, frame->origin[0], frame->origin[1], frame->origin[2],
				frame->angles[0], frame->angles[1], frame->angles[2] );

			if ( offset + length >= limit ) {
				break;
			}
			Q_strcat( samples, sizeof( samples ), sample );
			offset += length;
		}
		count = i;
		Com_sprintf( command, sizeof( command ), "lghostdata %d %d%s", client->sendNext, count, samples );
	}
	trap_SendServerCommand( clientNum, command );
	client->sendNext += count;
	ghost->lastUsed = level.time;
}

static void G_GhostLadder_UpdateDownloads( void ) {
	int i;

	for ( i = 0; i < s_entryCount; i++ ) {
		lghostEntry_t *entry = &s_entries[i];
		int status;

		if ( !entry->downloadRequestId ) {
			continue;
		}
		status = G_GhostLadder_FetchStatus( "sv_ladderGhostFile", entry->downloadRequestId );
		if ( status < 0 || ( status == 0 && level.time - entry->downloadStartedAt > LGHOST_DOWNLOAD_TIMEOUT ) ) {
			entry->downloadFailed = qtrue;
			entry->downloadRequestId = 0;
		} else if ( status > 0 || G_GhostLadder_FileExists( entry->cacheFile ) ) {
			entry->downloadRequestId = 0;
		}
	}
}

/*
=================
G_GhostLadder_Frame

Called every server frame from G_RunFrame.
=================
*/
void G_GhostLadder_Frame( void ) {
	int clientNum;

	if ( s_listState == LGHOST_LIST_OFF ) {
		return;
	}

	if ( s_listState == LGHOST_LIST_PENDING ) {
		G_GhostLadder_RequestList();
		return;
	}

	if ( level.time - s_lastPoll >= LGHOST_POLL_INTERVAL ) {
		s_lastPoll = level.time;

		if ( s_listState == LGHOST_LIST_FETCHING ) {
			int status = G_GhostLadder_FetchStatus( "sv_ladderGhostList", s_listRequestId );

			if ( status > 0 ) {
				G_GhostLadder_LoadList();
				G_GhostLadder_ListReady( qfalse );
			} else if ( status < 0 || level.time - s_listRequestedAt > LGHOST_LIST_TIMEOUT ) {
				/* Offline or ladder down: keep the cached list. */
				G_GhostLadder_ListReady( qtrue );
			}
		}

		if ( s_listState == LGHOST_LIST_READY ) {
			G_GhostLadder_UpdateDownloads();
			for ( clientNum = 0; clientNum < level.maxclients; clientNum++ ) {
				lghostClient_t *client = &s_clients[clientNum];
				int pool;

				if ( client->pickState != LGHOST_PICK_WAITING ) {
					continue;
				}
				pool = G_GhostLadder_Acquire( client->pickEntry );
				if ( pool >= 0 ) {
					client->pool = pool;
					client->sendNext = -1;
					client->pickState = LGHOST_PICK_STREAMING;
				} else if ( s_entries[client->pickEntry].downloadFailed ) {
					G_GhostLadder_SendFail( clientNum, client->pickEntry, "download" );
				} else if ( !s_entries[client->pickEntry].downloadRequestId ) {
					if ( G_GhostLadder_FileExists( s_entries[client->pickEntry].cacheFile ) ) {
						/* Cached but every pool slot is being streamed. */
						if ( level.time - client->pickQueuedAt > LGHOST_DOWNLOAD_TIMEOUT ) {
							G_GhostLadder_SendFail( clientNum, client->pickEntry, "busy" );
						}
					} else {
						G_GhostLadder_StartDownload( client->pickEntry );
					}
				}
			}
		}
	}

	if ( s_listState != LGHOST_LIST_READY ) {
		return;
	}

	for ( clientNum = 0; clientNum < level.maxclients; clientNum++ ) {
		lghostClient_t *client = &s_clients[clientNum];
		int sent;

		if ( level.clients[clientNum].pers.connected != CON_CONNECTED ) {
			continue;
		}
		for ( sent = 0; sent < LGHOST_COMMANDS_PER_FRAME; sent++ ) {
			if ( client->listWanted ) {
				G_GhostLadder_SendList( clientNum );
			} else if ( client->pickState == LGHOST_PICK_STREAMING ) {
				G_GhostLadder_SendGhostChunk( clientNum );
			} else {
				break;
			}
		}
	}
}

void G_GhostLadder_ClientBegin( int clientNum ) {
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || s_listState == LGHOST_LIST_OFF ) {
		return;
	}
	if ( g_entities[clientNum].r.svFlags & SVF_BOT ) {
		return;
	}
	if ( !s_clients[clientNum].listWanted ) {
		s_clients[clientNum].listWanted = qtrue;
		s_clients[clientNum].listNext = -1;
	}
}

void G_GhostLadder_ClientDisconnect( int clientNum ) {
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		return;
	}
	G_GhostLadder_ResetClient( clientNum );
}

/* Name for a server command argument: printable, no quotes or separators. */
static void G_GhostLadder_SafeName( const char *in, char *out, int outSize ) {
	int n = 0;

	for ( ; *in && n < outSize - 1; in++ ) {
		if ( (unsigned char)*in < 0x20 || (unsigned char)*in > 0x7e ||
		     *in == '"' || *in == '\\' || *in == ';' ) {
			continue;
		}
		out[n++] = *in;
	}
	out[n] = '\0';
	if ( !out[0] ) {
		Q_strncpyz( out, "Ghost", outSize );
	}
}

/*
=================
G_GhostLadder_ClientFinished

Ghost Race result at the finish line: the driver's best lap (A2B: the whole
run) against the lap of the ghost the driver reported with "ghostopp".
Sent to every client so the scoreboard shows all results.
=================
*/
void G_GhostLadder_ClientFinished( gentity_t *ent ) {
	int clientNum;
	lghostClient_t *state;
	int bestLapMs;
	qboolean won;

	if ( g_gametype.integer != GT_GHOST || !ent || !ent->client ) {
		return;
	}
	clientNum = ent - g_entities;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		return;
	}
	state = &s_clients[clientNum];
	bestLapMs = ent->client->bestLapMs;
	if ( state->opponentLapMs <= 0 || bestLapMs <= 0 ) {
		return;
	}

	won = bestLapMs < state->opponentLapMs ? qtrue : qfalse;
	trap_SendServerCommand( -1, va( "lghostresult %d %d %d %d \"%s\"", clientNum, won ? 1 : 0,
		bestLapMs, state->opponentLapMs, state->opponentName ) );
	G_LogPrintf( "GhostRace: %d %s %s: %d vs %d\n", clientNum,
		won ? "beat" : "lost to", state->opponentName, bestLapMs, state->opponentLapMs );
}

/*
=================
G_GhostLadder_ClientCommand

Returns qtrue when the command was a ladder ghost command.
=================
*/
qboolean G_GhostLadder_ClientCommand( gentity_t *ent, const char *cmd ) {
	int clientNum = ent - g_entities;
	lghostClient_t *client;
	char arg[16];
	int entry;

	if ( !Q_stricmp( cmd, "ghostopp" ) ) {
		char name[64];
		int reportedMs;
		int lapMs;
		int ladderEntry = -1;

		if ( clientNum < 0 || clientNum >= MAX_CLIENTS || g_gametype.integer != GT_GHOST ) {
			return qtrue;
		}
		trap_Argv( 1, arg, sizeof( arg ) );
		trap_Argv( 2, name, sizeof( name ) );
		reportedMs = atoi( arg );
		lapMs = reportedMs;
		if ( trap_Argc() > 3 ) {
			trap_Argv( 3, arg, sizeof( arg ) );
			ladderEntry = atoi( arg );
		}
		if ( lapMs < 1000 || lapMs > 3600000 ) {
			lapMs = 0;
		}
		if ( lapMs > 0 && ladderEntry >= 0 && ladderEntry < s_entryCount && s_listState == LGHOST_LIST_READY ) {
			/* Ladder ghost: lap time and name from the server's list, not
			 * from the client. */
			lapMs = s_entries[ladderEntry].lapMs;
			Q_strncpyz( name, s_entries[ladderEntry].name, sizeof( name ) );
		}
		s_clients[clientNum].opponentLapMs = lapMs;
		G_GhostLadder_SafeName( name, s_clients[clientNum].opponentName,
			sizeof( s_clients[clientNum].opponentName ) );
		trap_SendServerCommand( clientNum, va( "lghostoppok %d", reportedMs ) );
		return qtrue;
	}

	if ( Q_stricmp( cmd, "lghostlistreq" ) && Q_stricmp( cmd, "lghostpick" ) ) {
		return qfalse;
	}
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		return qtrue;
	}
	client = &s_clients[clientNum];

	if ( s_listState == LGHOST_LIST_OFF ) {
		trap_SendServerCommand( clientNum, "lghostlist 0 0" );
		trap_SendServerCommand( clientNum, "lghostlistdone" );
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostlistreq" ) ) {
		client->listWanted = qtrue;
		client->listNext = -1;
		return qtrue;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	entry = atoi( arg );
	if ( s_listState != LGHOST_LIST_READY || entry < 0 || entry >= s_entryCount ) {
		G_GhostLadder_SendFail( clientNum, entry, "invalid" );
		return qtrue;
	}

	trap_SendServerCommand( clientNum, va( "lghostpickok %d", entry ) );
	if ( client->pickState != LGHOST_PICK_IDLE && client->pickEntry == entry ) {
		/* The client repeated the pick: the running one goes on. */
		return qtrue;
	}

	client->pickState = LGHOST_PICK_WAITING;
	client->pickEntry = entry;
	client->pickQueuedAt = level.time;
	client->pool = -1;
	client->sendNext = -1;
	s_entries[entry].downloadFailed = qfalse;
	s_lastPoll = 0;     /* look at it in this frame */
	return qtrue;
}
