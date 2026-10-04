/*
 * Server-side lap ghost recording (g_ghost_record.c): sampling, session-best
 * gating, opt-out, invalidation by teleports and the uploaded ghost text.
 */
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../engine/code/game/g_ghost_record.c"

/* Shared ghost text buffer, defined in g_ghost.c in the game module. */
char g_ghostTextBuffer[G_GHOST_TEXT_BUFFER_SIZE];

level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
static gclient_t s_clients[2];
vmCvar_t g_ghostUpload;
vmCvar_t g_gametype;
vmCvar_t g_trackLength;
vmCvar_t g_trackReversed;

static int s_ladderEnabled = 1;
static char s_userinfo[2][MAX_INFO_STRING];
static int s_submitCount;
static ladderGhostMeta_t s_lastMeta;
static char s_lastData[LADDER_GHOST_MAX_DATA];

void QDECL Com_Error( int level, const char *fmt, ... ) { (void)level; (void)fmt; assert( 0 ); }
void QDECL Com_Printf( const char *fmt, ... ) { (void)fmt; }
void QDECL G_Printf( const char *fmt, ... ) { (void)fmt; }

qboolean BG_GametypeIsTimedRace( int gametype ) {
	return ( gametype == GT_RACING || gametype == GT_SPRINT || gametype == GT_GHOST ) ? qtrue : qfalse;
}
qboolean G_IsSprintTrack( void ) { return qfalse; }
static const char *s_rulesViolation;
const char *G_LadderRulesViolation( void ) { return s_rulesViolation; }

void trap_Cvar_VariableStringBuffer( const char *name, char *buffer, int size ) {
	if ( !strcmp( name, "mapname" ) ) {
		Q_strncpyz( buffer, "q3r_testtrack", size );
	} else {
		buffer[0] = '\0';
	}
}
int trap_Cvar_VariableIntegerValue( const char *name ) {
	if ( !strcmp( name, "sv_ladderEnabled" ) ) return s_ladderEnabled;
	if ( !strcmp( name, "sv_mapChecksum" ) ) return 4242;
	return 0;
}
void trap_GetUserinfo( int num, char *buffer, int size ) {
	Q_strncpyz( buffer, s_userinfo[num], size );
}
void trap_LadderSubmitGhost( const ladderGhostMeta_t *meta, const char *data ) {
	s_submitCount++;
	s_lastMeta = *meta;
	assert( (int)strlen( data ) == meta->dataLength );
	memcpy( s_lastData, data, meta->dataLength + 1 );
}

/* Drive one lap of "duration" ms around a circle, 50 ms server frames. */
static void DriveLap( int clientNum, int lapStart, int duration, int teleportAt ) {
	gentity_t *ent = &g_entities[clientNum];
	gclient_t *client = ent->client;
	int t;

	client->lapStartTime = lapStart;
	for ( t = lapStart; t < lapStart + duration; t += 50 ) {
		float a = 2.0f * (float)M_PI * (float)( t - lapStart ) / (float)duration;
		level.time = t;
		client->ps.origin[0] = cos( a ) * 3000.0f;
		client->ps.origin[1] = sin( a ) * 3000.0f;
		client->ps.origin[2] = 64.0f;
		if ( teleportAt && t - lapStart >= teleportAt && t - lapStart < teleportAt + 50 ) {
			client->ps.origin[0] += 5000.0f;
		}
		client->ps.viewangles[YAW] = a * 180.0f / (float)M_PI + 90.0f;
		client->ps.velocity[0] = -sin( a ) * 300.0f;
		client->ps.velocity[1] = cos( a ) * 300.0f;
		client->ps.stats[STAT_NEXT_CHECKPOINT] = 1 + (int)( a / ( (float)M_PI / 2.0f ) );
		G_GhostRecord_ClientFrame( ent );
	}
	level.time = lapStart + duration;
	G_GhostRecord_LapComplete( ent, lapStart, lapStart + duration );
}

static int CountFrameLines( const char *text, int *lastTime ) {
	const char *line = text;
	int count = 0;

	while ( line && *line ) {
		if ( ( *line >= '0' && *line <= '9' ) || *line == '-' ) {
			int t;
			float f[9];
			int i3[3];
			int n = sscanf( line, "%d %f %f %f %f %f %f %f %f %f %d %d %d", &t,
				&f[0], &f[1], &f[2], &f[3], &f[4], &f[5], &f[6], &f[7], &f[8], &i3[0], &i3[1], &i3[2] );
			assert( n == 13 );
			*lastTime = t;
			count++;
		}
		line = strchr( line, '\n' );
		if ( line ) {
			line++;
		}
	}
	return count;
}

static void ResetClient( int clientNum, qboolean bot, const char *userinfo ) {
	gentity_t *ent = &g_entities[clientNum];

	memset( &s_clients[clientNum], 0, sizeof( s_clients[clientNum] ) );
	ent->client = &s_clients[clientNum];
	ent->r.svFlags = bot ? SVF_BOT : 0;
	s_clients[clientNum].pers.connected = CON_CONNECTED;
	s_clients[clientNum].sess.sessionTeam = TEAM_FREE;
	Q_strncpyz( s_clients[clientNum].pers.uuid, "0a1b2c3d-1111-4222-8333-444455556666", sizeof( s_clients[clientNum].pers.uuid ) );
	Q_strncpyz( s_clients[clientNum].pers.netname, "^1Fast^7Driver", sizeof( s_clients[clientNum].pers.netname ) );
	Q_strncpyz( s_userinfo[clientNum], userinfo, sizeof( s_userinfo[clientNum] ) );
}

int main( void ) {
	int lastTime = -1;
	int frames;

	g_ghostUpload.integer = 1;
	g_gametype.integer = GT_GHOST;
	g_trackLength.integer = 1;
	g_trackReversed.integer = 1;
	level.startRaceTime = 1000;
	level.trackLength = 12345.0f;

	G_GhostRecord_Init();
	ResetClient( 0, qfalse, "\\model\\Evo/red\\cg_ghostShare\\1" );

	/* Clean first lap: uploaded with full metadata and a parsable ghost. */
	DriveLap( 0, 1000, 60000, 0 );
	assert( s_submitCount == 1 );
	assert( s_lastMeta.valid && s_lastMeta.lapMs == 60000 );
	assert( !strcmp( s_lastMeta.map, "q3r_testtrack" ) );
	assert( !strcmp( s_lastMeta.vehicle, "evo" ) );
	assert( !strcmp( s_lastMeta.playerName, "FastDriver" ) );
	assert( !strcmp( s_lastMeta.playerId, "0a1b2c3d-1111-4222-8333-444455556666" ) );
	assert( s_lastMeta.trackLength == 1 && s_lastMeta.trackReversed == 1 );
	assert( s_lastMeta.physicsVersion == BG_PHYSICS_VERSION && s_lastMeta.mapChecksum == 4242 );
	assert( s_lastMeta.courseLengthUnits == 12345 && s_lastMeta.gametype == GT_GHOST );
	assert( strstr( s_lastData, "map q3r_testtrack\n" ) );
	assert( strstr( s_lastData, "vehicle evo\n" ) );
	assert( strstr( s_lastData, "track_length 1\ntrack_reversed 1\nbest_time_ms 60000\n" ) );
	assert( strstr( s_lastData, "physics_version 1\nmap_checksum 4242\n" ) );
	frames = CountFrameLines( s_lastData, &lastTime );
	assert( frames == s_lastMeta.frameCount );
	assert( frames > 100 && frames < GHOST_REC_MAX_FRAMES );
	assert( lastTime == 60000 );
	assert( !strncmp( s_lastData + strlen( "# Q3Rally server ghost\n" ), "map ", 4 ) );

	/* Slower lap: not a session best, not uploaded. */
	DriveLap( 0, 61000, 65000, 0 );
	assert( s_submitCount == 1 );

	/* Faster lap with a reset/teleport in the middle: invalid, not uploaded. */
	DriveLap( 0, 126000, 55000, 20000 );
	assert( s_submitCount == 1 );

	/* Faster clean lap: uploaded. */
	DriveLap( 0, 181000, 55000, 0 );
	assert( s_submitCount == 2 && s_lastMeta.lapMs == 55000 );

	/* Opted out: nothing uploaded even for a new best. */
	Q_strncpyz( s_userinfo[0], "\\model\\evo\\cg_ghostShare\\0", sizeof( s_userinfo[0] ) );
	DriveLap( 0, 236000, 50000, 0 );
	assert( s_submitCount == 2 );

	/* Bots never upload, ladder disabled never uploads. */
	ResetClient( 1, qtrue, "\\model\\evo" );
	DriveLap( 1, 1000, 40000, 0 );
	assert( s_submitCount == 2 );
	ResetClient( 1, qfalse, "\\model\\evo" );
	s_ladderEnabled = 0;
	DriveLap( 1, 1000, 40000, 0 );
	assert( s_submitCount == 2 );
	s_ladderEnabled = 1;

	/* Non-timed race modes (e.g. Racing DM) do not record. */
	g_gametype.integer = GT_RACING_DM;
	DriveLap( 1, 50000, 40000, 0 );
	assert( s_submitCount == 2 );
	g_gametype.integer = GT_RACING;
	DriveLap( 1, 100000, 40000, 0 );
	assert( s_submitCount == 3 && s_lastMeta.lapMs == 40000 );

	/* Non-standard rules (cheats, timescale, physics): no upload. */
	s_rulesViolation = "timescale 0.5";
	DriveLap( 1, 150000, 30000, 0 );
	assert( s_submitCount == 3 );
	/* Rules back to standard in the middle of a lap: that lap is never
	 * uploaded (recording did not start at the lap start), the next one is. */
	{
		gentity_t *ent = &g_entities[1];
		int t;

		ent->client->lapStartTime = 190000;
		for ( t = 190000; t < 220000; t += 50 ) {
			float a = 2.0f * (float)M_PI * (float)( t - 190000 ) / 30000.0f;
			if ( t == 200000 ) {
				s_rulesViolation = NULL;
			}
			level.time = t;
			ent->client->ps.origin[0] = cos( a ) * 3000.0f;
			ent->client->ps.origin[1] = sin( a ) * 3000.0f;
			G_GhostRecord_ClientFrame( ent );
		}
		level.time = 220000;
		G_GhostRecord_LapComplete( ent, 190000, 220000 );
		assert( s_submitCount == 3 );
	}
	DriveLap( 1, 220000, 30000, 0 );
	assert( s_submitCount == 4 && s_lastMeta.lapMs == 30000 );

	puts( "ok" );
	return 0;
}
