/*
 * Ladder ghosts in Ghost Race (g_ghost_ladder.c): list fetch with offline
 * cache, list transfer to clients, pick, download and streaming of a ghost.
 * The engine side (trap_LadderFetchGhosts, files, cvars) is simulated here.
 */
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../engine/code/game/g_ghost_ladder.c"

/* Shared ghost text buffer, defined in g_ghost.c in the game module. */
char g_ghostTextBuffer[G_GHOST_TEXT_BUFFER_SIZE];

level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
static gclient_t s_testClients[2];
vmCvar_t g_ghostDownload;
vmCvar_t g_gametype;
vmCvar_t g_trackLength;
vmCvar_t g_trackReversed;

void QDECL Com_Error( int level, const char *fmt, ... ) { (void)level; (void)fmt; assert( 0 ); }
void QDECL Com_Printf( const char *fmt, ... ) { (void)fmt; }
void QDECL G_Printf( const char *fmt, ... ) { (void)fmt; }
void QDECL G_LogPrintf( const char *fmt, ... ) { (void)fmt; }

/* ---- in-memory files ---------------------------------------------------- */
#define TEST_FILES 8
static char s_filePath[TEST_FILES][MAX_QPATH];
static char *s_fileData[TEST_FILES];
static const char *s_openData;
static int s_openPos;

static void WriteTestFile( const char *path, const char *data ) {
	int i;
	for ( i = 0; i < TEST_FILES; i++ ) {
		if ( !s_filePath[i][0] || !strcmp( s_filePath[i], path ) ) {
			Q_strncpyz( s_filePath[i], path, sizeof( s_filePath[i] ) );
			free( s_fileData[i] );
			s_fileData[i] = strdup( data );
			return;
		}
	}
	assert( 0 );
}

int trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
	int i;
	(void)mode;
	for ( i = 0; i < TEST_FILES; i++ ) {
		if ( s_filePath[i][0] && !strcmp( s_filePath[i], qpath ) ) {
			s_openData = s_fileData[i];
			s_openPos = 0;
			*f = 1;
			return (int)strlen( s_fileData[i] );
		}
	}
	*f = 0;
	return -1;
}
void trap_FS_Read( void *buffer, int len, fileHandle_t f ) {
	(void)f;
	memcpy( buffer, s_openData + s_openPos, (size_t)len );
	s_openPos += len;
}
void trap_FS_FCloseFile( fileHandle_t f ) { (void)f; }

/* ---- cvars and engine requests ------------------------------------------ */
static char s_listStatus[64];
static char s_fileStatus[64];
static ladderGhostFetch_t s_requests[16];
static int s_requestCount;
static char s_argv1[32];
static char s_argv2[64];
static char s_argv3[16];
static int s_argc = 3;

void trap_Cvar_VariableStringBuffer( const char *name, char *buffer, int size ) {
	if ( !strcmp( name, "mapname" ) ) {
		Q_strncpyz( buffer, "Q3R_TestTrack", size );
	} else if ( !strcmp( name, "sv_ladderGhostList" ) ) {
		Q_strncpyz( buffer, s_listStatus, size );
	} else if ( !strcmp( name, "sv_ladderGhostFile" ) ) {
		Q_strncpyz( buffer, s_fileStatus, size );
	} else {
		buffer[0] = '\0';
	}
}
int trap_Cvar_VariableIntegerValue( const char *name ) {
	return !strcmp( name, "sv_mapChecksum" ) ? -4242 : 0;
}
void trap_Cvar_Set( const char *name, const char *value ) {
	if ( !strcmp( name, "sv_ladderGhostList" ) ) {
		Q_strncpyz( s_listStatus, value, sizeof( s_listStatus ) );
	}
}
int trap_Milliseconds( void ) { return 12345; }
void trap_LadderFetchGhosts( const ladderGhostFetch_t *request ) {
	assert( s_requestCount < 16 );
	s_requests[s_requestCount++] = *request;
}
void trap_Argv( int n, char *buffer, int bufferLength ) {
	Q_strncpyz( buffer, n == 1 ? s_argv1 : ( n == 2 ? s_argv2 : ( n == 3 ? s_argv3 : "" ) ), bufferLength );
}
int trap_Argc( void ) { return s_argc; }

/* ---- server commands ----------------------------------------------------- */
static char s_commands[512][MAX_STRING_CHARS];
static int s_commandCount;

void trap_SendServerCommand( int clientNum, const char *text ) {
	assert( clientNum == 0 || clientNum == -1 );
	assert( s_commandCount < 512 );
	assert( strlen( text ) < MAX_STRING_CHARS - 1 );
	Q_strncpyz( s_commands[s_commandCount++], text, MAX_STRING_CHARS );
}

static int CountPrefix( const char *prefix ) {
	int i, n = 0;
	for ( i = 0; i < s_commandCount; i++ ) {
		if ( !strncmp( s_commands[i], prefix, strlen( prefix ) ) ) {
			n++;
		}
	}
	return n;
}
static const char *FindPrefix( const char *prefix ) {
	int i;
	for ( i = 0; i < s_commandCount; i++ ) {
		if ( !strncmp( s_commands[i], prefix, strlen( prefix ) ) ) {
			return s_commands[i];
		}
	}
	return NULL;
}

static void RunFrames( int count ) {
	while ( count-- > 0 ) {
		level.time += 50;
		G_GhostLadder_Frame();
	}
}

static const char *LIST_PATH = "ghosts/ladder/q3r_testtrack_tl1_rev0_p1_c-4242.list";

static char *MakeGhost( int lapMs, int frames ) {
	size_t size = (size_t)frames * 128 + 512;
	char *text = malloc( size );
	size_t used;
	int i;

	used = (size_t)snprintf( text, size, "# Q3Rally server ghost\nmap q3r_testtrack\nvehicle evo\n"
		"track_length 1\ntrack_reversed 0\nbest_time_ms %d\nframes %d\n", lapMs, frames );
	for ( i = 0; i < frames; i++ ) {
		int t = (int)( (long long)lapMs * i / ( frames - 1 ) );
		float a = 2.0f * (float)M_PI * i / ( frames - 1 );
		used += (size_t)snprintf( text + used, size - used,
			"%d %.1f %.1f 64.0 1.0 %.1f -2.0 10.0 0.0 0.0 0 127 0\n",
			t, cos( a ) * 3000.0f, sin( a ) * 3000.0f, a * 180.0f / (float)M_PI );
	}
	return text;
}

int main( void ) {
	char *ghost;
	const char *meta;
	int i;

	level.clients = s_testClients;
	level.maxclients = 1;
	s_testClients[0].pers.connected = CON_CONNECTED;
	g_entities[0].client = &s_testClients[0];
	g_gametype.integer = GT_GHOST;
	g_ghostDownload.integer = 1;
	g_trackLength.integer = 1;

	/* Cache from an earlier session: two ghosts. */
	WriteTestFile( LIST_PATH,
		"ghosts/ladder/q3r_testtrack/aaaa.ghost\tq3r_testtrack.tl1_rev0.evo.p1_c-4242.p1\t60000\tevo\tAlpha\n"
		"ghosts/ladder/q3r_testtrack/bbbb.ghost\tq3r_testtrack.tl1_rev0.evo.p1_c-4242.p2\t61000\tevo\tBeta\n" );

	G_GhostLadder_Init();
	assert( s_entryCount == 2 );
	assert( s_requestCount == 0 );
	G_GhostLadder_ClientBegin( 0 );

	/* First frame: the list is requested for map, variant and bucket. */
	RunFrames( 1 );
	assert( s_requestCount == 1 );
	assert( s_requests[0].kind == LADDER_FETCH_LIST );
	assert( !strcmp( s_requests[0].map, "q3r_testtrack" ) );
	assert( s_requests[0].trackLength == 1 && s_requests[0].trackReversed == 0 );
	assert( s_requests[0].physicsVersion == BG_PHYSICS_VERSION && s_requests[0].mapChecksum == -4242 );
	assert( !strcmp( s_requests[0].target, LIST_PATH ) );

	/* The engine stores the fresh ranking (three ghosts, one with a bad path). */
	WriteTestFile( LIST_PATH,
		"ghosts/ladder/q3r_testtrack/aaaa.ghost\tq3r_testtrack.tl1_rev0.evo.p1_c-4242.p1\t59000\tevo\tAlpha\n"
		"ghosts/ladder/q3r_testtrack/cccc.ghost\tq3r_testtrack.tl1_rev0.sidepipe.p1_c-4242.p3\t59500\tsidepipe\tGamma Ray\n"
		"maps/evil.ghost\tq3r_testtrack.tl1_rev0.evo.p1_c-4242.p4\t60000\tevo\tEvil\n"
		"ghosts/ladder/q3r_testtrack/bbbb.ghost\tq3r_testtrack.tl1_rev0.evo.p1_c-4242.p2\t61000\tevo\tBeta\n" );
	snprintf( s_listStatus, sizeof( s_listStatus ), "%d ok 4", s_requests[0].requestId );
	RunFrames( 10 );
	assert( s_entryCount == 3 );
	assert( !strcmp( s_commands[0], "lghostlist 3 0" ) );
	assert( !strcmp( s_commands[1], "lghostents 0 3 59000 evo \"Alpha\" ghosts/ladder/q3r_testtrack/aaaa.ghost"
		" 59500 sidepipe \"Gamma Ray\" ghosts/ladder/q3r_testtrack/cccc.ghost"
		" 61000 evo \"Beta\" ghosts/ladder/q3r_testtrack/bbbb.ghost" ) );
	assert( !strcmp( s_commands[2], "lghostlistdone" ) );
	assert( s_commandCount == 3 );

	/* Pick entry 1: not cached yet, so it is downloaded first. */
	s_commandCount = 0;
	strcpy( s_argv1, "1" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
	assert( s_commandCount == 1 && !strcmp( s_commands[0], "lghostpickok 1" ) );
	s_commandCount = 0;
	RunFrames( 1 );
	assert( s_requestCount == 2 );
	{
		/* The client repeats the pick (answer still on the way): only a
		 * new answer, the waiting pick and its download go on. */
		int queuedAt = s_clients[0].pickQueuedAt;
		RunFrames( 1 );
		assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
		assert( s_commandCount == 1 && !strcmp( s_commands[0], "lghostpickok 1" ) );
		assert( s_clients[0].pickState == LGHOST_PICK_WAITING && s_clients[0].pickQueuedAt == queuedAt );
		s_commandCount = 0;
		RunFrames( 1 );
		assert( s_requestCount == 2 );
	}
	assert( s_requests[1].kind == LADDER_FETCH_GHOST );
	assert( !strcmp( s_requests[1].ghostId, "q3r_testtrack.tl1_rev0.sidepipe.p1_c-4242.p3" ) );
	assert( !strcmp( s_requests[1].target, "ghosts/ladder/q3r_testtrack/cccc.ghost" ) );
	RunFrames( 1 );
	assert( s_commandCount == 0 );

	/* Download arrives: 3000 recorded samples are thinned out to 2048. */
	ghost = MakeGhost( 59500, 3000 );
	WriteTestFile( "ghosts/ladder/q3r_testtrack/cccc.ghost", ghost );
	snprintf( s_fileStatus, sizeof( s_fileStatus ), "%d ok 1", s_requests[1].requestId );
	RunFrames( 200 );
	assert( !strcmp( s_commands[0], "lghostmeta 1 59500 2048" ) );
	assert( !strncmp( s_commands[1], "lghostdata 0 20 0 3000 0 64 1 0 -2 ", 35 ) );
	assert( CountPrefix( "lghostdata " ) == ( 2048 + 19 ) / 20 );
	for ( i = 0; i < s_commandCount; i++ ) {
		assert( strlen( s_commands[i] ) < 1000 );
	}
	assert( !strcmp( s_commands[s_commandCount - 1], "lghostdone 1" ) );
	{
		const char *last = s_commands[s_commandCount - 2];
		assert( !strncmp( last, "lghostdata 2040 8 ", 18 ) );
		assert( strstr( last, " 59500 3000 " ) != NULL );
	}

	/* Second pick of the same ghost comes straight from the pool. */
	s_commandCount = 0;
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
	assert( !strcmp( s_commands[0], "lghostpickok 1" ) );
	s_commandCount = 0;
	RunFrames( 200 );
	assert( s_requestCount == 2 );
	assert( !strcmp( s_commands[0], "lghostmeta 1 59500 2048" ) );

	/* Failed download is reported to the client. */
	s_commandCount = 0;
	strcpy( s_argv1, "2" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
	RunFrames( 6 );
	assert( s_requestCount == 3 );
	snprintf( s_fileStatus, sizeof( s_fileStatus ), "%d fail 0", s_requests[2].requestId );
	RunFrames( 6 );
	assert( FindPrefix( "lghostfail 2 download" ) != NULL );

	/* Invalid index. */
	s_commandCount = 0;
	strcpy( s_argv1, "7" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
	assert( !strcmp( s_commands[0], "lghostfail 7 invalid" ) );
	assert( !G_GhostLadder_ClientCommand( &g_entities[0], "say" ) );

	/* Offline: the list request fails, the cached list is offered. */
	s_commandCount = 0;
	s_requestCount = 0;
	level.time = 0;
	G_GhostLadder_Init();
	G_GhostLadder_ClientBegin( 0 );
	RunFrames( 1 );
	assert( s_requestCount == 1 );
	snprintf( s_listStatus, sizeof( s_listStatus ), "%d fail 0", s_requests[0].requestId );
	RunFrames( 10 );
	assert( !strcmp( s_commands[0], "lghostlist 3 1" ) );
	assert( !strcmp( s_commands[s_commandCount - 1], "lghostlistdone" ) );

	/* Cached ghost is used without a download. */
	s_commandCount = 0;
	strcpy( s_argv1, "1" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
	assert( !strcmp( s_commands[0], "lghostpickok 1" ) );
	s_commandCount = 0;
	RunFrames( 200 );
	assert( s_requestCount == 1 );
	assert( !strcmp( s_commands[0], "lghostmeta 1 59500 2048" ) );

	/* Ghost of another map is refused. */
	WriteTestFile( "ghosts/ladder/q3r_testtrack/aaaa.ghost", "map other\nframes 2\n0 0 0 0 0 0 0\n100 1 1 1 0 0 0\n" );
	s_commandCount = 0;
	strcpy( s_argv1, "0" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "lghostpick" ) );
	RunFrames( 6 );
	assert( FindPrefix( "lghostfail 0 download" ) != NULL );

	/* Result at the finish: best lap against the reported ghost, to all. */
	s_commandCount = 0;
	strcpy( s_argv1, "59500" );
	strcpy( s_argv2, "Gamma \"Ray\";" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "ghostopp" ) );
	assert( s_clients[0].opponentLapMs == 59500 && !strcmp( s_clients[0].opponentName, "Gamma Ray" ) );
	assert( s_commandCount == 1 && !strcmp( s_commands[0], "lghostoppok 59500" ) );
	s_commandCount = 0;
	s_testClients[0].bestLapMs = 59000;
	G_GhostLadder_ClientFinished( &g_entities[0] );
	assert( !strcmp( s_commands[0], "lghostresult 0 1 59000 59500 \"Gamma Ray\"" ) );
	s_testClients[0].bestLapMs = 60000;
	G_GhostLadder_ClientFinished( &g_entities[0] );
	assert( !strcmp( s_commands[1], "lghostresult 0 0 60000 59500 \"Gamma Ray\"" ) );
	/* No ghost reported ("ghostopp 0"): no result. */
	strcpy( s_argv1, "0" );
	strcpy( s_argv2, "-" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "ghostopp" ) );
	assert( !strcmp( s_commands[2], "lghostoppok 0" ) );
	G_GhostLadder_ClientFinished( &g_entities[0] );
	assert( s_commandCount == 3 );

	/* Ladder ghost: lap time and name come from the server's list, a
	 * client claiming a slower ghost cannot win by it. */
	s_commandCount = 0;
	s_argc = 4;
	strcpy( s_argv1, "3599999" );
	strcpy( s_argv2, "Slow Fake" );
	strcpy( s_argv3, "1" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "ghostopp" ) );
	assert( s_clients[0].opponentLapMs == 59500 && !strcmp( s_clients[0].opponentName, "Gamma Ray" ) );
	assert( !strcmp( s_commands[0], "lghostoppok 3599999" ) );
	s_testClients[0].bestLapMs = 60000;
	G_GhostLadder_ClientFinished( &g_entities[0] );
	assert( !strcmp( s_commands[1], "lghostresult 0 0 60000 59500 \"Gamma Ray\"" ) );
	/* Unknown entry index: the client's values stay (personal ghost). */
	strcpy( s_argv1, "61000" );
	strcpy( s_argv2, "Personal Ghost" );
	strcpy( s_argv3, "-1" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "ghostopp" ) );
	assert( s_clients[0].opponentLapMs == 61000 && !strcmp( s_clients[0].opponentName, "Personal Ghost" ) );
	strcpy( s_argv3, "17" );
	assert( G_GhostLadder_ClientCommand( &g_entities[0], "ghostopp" ) );
	assert( s_clients[0].opponentLapMs == 61000 );
	s_argc = 3;

	/* Other gametypes do nothing. */
	s_commandCount = 0;
	s_requestCount = 0;
	g_gametype.integer = GT_RACING;
	G_GhostLadder_Init();
	G_GhostLadder_ClientBegin( 0 );
	RunFrames( 20 );
	assert( s_requestCount == 0 && s_commandCount == 0 );

	for ( i = 0; i < TEST_FILES; i++ ) {
		free( s_fileData[i] );
	}
	free( ghost );
	(void)meta;
	puts( "ok" );
	return 0;
}
