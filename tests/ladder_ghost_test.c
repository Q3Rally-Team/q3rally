/*
 * Ghost upload path of sv_ladder.c: JSON body, endpoint URL and spool naming.
 */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "server.h"

cvar_t *sv_ladderUrl = NULL;
cvar_t *sv_ladderApiKey = NULL;
cvar_t *sv_ladderEnabled = NULL;
cvar_t *sv_telemetryMaxBatch = NULL;

#include "sv_ladder_ghost_for_test.c"

static cvar_t s_urlCvar;

static void CheckUrl( const char *base, qboolean ghost, const char *expected ) {
	ladderRequest_t request;
	char url[MAX_STRING_CHARS];

	Com_Memset( &request, 0, sizeof( request ) );
	request.isGhost = ghost;
	Q_strncpyz( s_urlCvar.string, base, sizeof( s_urlCvar.string ) );
	sv_ladderUrl = &s_urlCvar;
	SV_LadderBuildRequestUrl( &request, url, sizeof( url ) );
	if ( strcmp( url, expected ) ) {
		printf( "url mismatch: %s -> %s (expected %s)\n", base, url, expected );
	}
	assert( !strcmp( url, expected ) );
}

int main( void ) {
	ladderGhostMeta_t meta;
	ladderRequest_t request;
	const char *data = "# Q3Rally server ghost\nmap q3r_valley\nvehicle evo\nbest_time_ms 61234\n"
		"frames 2\n0 1.0 2.0 3.0 0.0 90.0 0.0 0.0 0.0 0.0 0 0 0\n"
		"61234 5.0 6.0 7.0 0.0 91.0 0.0 10.0 0.0 0.0 1 127 0\n";
	char *json;
	size_t length = 0;

	Com_Memset( &meta, 0, sizeof( meta ) );
	meta.valid = 1;
	Q_strncpyz( meta.ghostId, "q3r_valley-1234abcd-61234", sizeof( meta.ghostId ) );
	Q_strncpyz( meta.map, "q3r_valley", sizeof( meta.map ) );
	Q_strncpyz( meta.vehicle, "evo", sizeof( meta.vehicle ) );
	Q_strncpyz( meta.playerId, "1234abcd-0000-4000-8000-000000000001", sizeof( meta.playerId ) );
	Q_strncpyz( meta.playerName, "Driver \"Q\"", sizeof( meta.playerName ) );
	meta.trackLength = 1;
	meta.trackReversed = 0;
	meta.lapMs = 61234;
	meta.frameCount = 2;
	meta.gametype = 8;
	meta.physicsVersion = 1;
	meta.mapChecksum = -123456;
	meta.courseLengthUnits = 98765;
	meta.sprintTrack = 0;
	meta.dataLength = (int)strlen( data );

	json = SV_LadderSerializeGhost( &meta, data, "My Server", &length );
	assert( json != NULL );
	assert( length == strlen( json ) );
	{
		const char *prefix = "{\"ghostId\":\"q3r_valley-1234abcd-61234\",\"server\":{\"name\":\"My Server\",";
		assert( !strncmp( json, prefix, strlen( prefix ) ) );
	}
	assert( strstr( json, "\"dedicated\":false" ) != NULL );
	assert( strstr( json, "\"player\":{\"id\":\"1234abcd-0000-4000-8000-000000000001\",\"name\":\"Driver \\\"Q\\\"\"}" ) != NULL );
	assert( strstr( json, "\"map\":\"q3r_valley\",\"vehicle\":\"evo\",\"trackLength\":1,\"trackReversed\":0,\"lapMs\":61234,\"frames\":2" ) != NULL );
	assert( strstr( json, "\"physicsVersion\":1,\"mapChecksum\":-123456,\"courseLengthUnits\":98765,\"sprintTrack\":false" ) != NULL );
	assert( strstr( json, "\"data\":\"# Q3Rally server ghost\\nmap q3r_valley\\n" ) != NULL );
	assert( json[length - 1] == '}' );
	Z_Free( json );

	/* Ghosts go to the sibling endpoint of .../matches; matches are unchanged. */
	CheckUrl( "https://ladder.q3rally.com/index.php/matches", qtrue, "https://ladder.q3rally.com/index.php/ghosts" );
	CheckUrl( "https://ladder.q3rally.com/index.php/api/v1/matches?x=1", qtrue, "https://ladder.q3rally.com/index.php/api/v1/ghosts?x=1" );
	CheckUrl( "https://example.com/matches/index.php/matches", qtrue, "https://example.com/matches/index.php/ghosts" );
	CheckUrl( "https://ladder.q3rally.com/index.php/matches", qfalse, "https://ladder.q3rally.com/index.php/matches" );

	/* Spool files carry the request kind in their name. */
	Com_Memset( &request, 0, sizeof( request ) );
	request.isGhost = qtrue;
	Q_strncpyz( request.matchId, meta.ghostId, sizeof( request.matchId ) );
	SV_LadderBuildSpoolName( &request );
	assert( !strncmp( request.spoolName, "ghost-", 6 ) );
	request.isGhost = qfalse;
	SV_LadderBuildSpoolName( &request );
	assert( !strncmp( request.spoolName, "match-", 6 ) );

	/* Ghost download: URLs, list conversion, cache names and file writes. */
	{
		ladderGhostFetch_t fetch;
		char url[MAX_STRING_CHARS];
		char list[8192];
		char cacheA[MAX_QPATH];
		char cacheB[MAX_QPATH];
		const char *body =
			"q3r_valley.tl1_rev0.evo.p1_c-5.1234abcd-0000-4000-8000-000000000001\t61234\tevo\tDriver \"Q\"; x\n"
			"bad id!\t5000\tevo\tNope\n"
			"q3r_valley.tl1_rev0.sidepipe.p1_c-5.1234abcd-0000-4000-8000-000000000002\t0\tsidepipe\tZero\n"
			"q3r_valley.tl1_rev0.sidepipe.p1_c-5.1234abcd-0000-4000-8000-000000000003\t62000\tsidepipe\t\n";
		int count;

		Com_Memset( &fetch, 0, sizeof( fetch ) );
		fetch.kind = LADDER_FETCH_LIST;
		fetch.requestId = 7;
		Q_strncpyz( fetch.map, "q3r_valley", sizeof( fetch.map ) );
		fetch.trackLength = 1;
		fetch.physicsVersion = 1;
		fetch.mapChecksum = -5;
		Q_strncpyz( fetch.target, "ghosts/ladder/q3r_valley_tl1_rev0.list", sizeof( fetch.target ) );
		Q_strncpyz( s_urlCvar.string, "https://ladder.q3rally.com/index.php/api/v1/matches", sizeof( s_urlCvar.string ) );
		sv_ladderUrl = &s_urlCvar;
		SV_LadderFetchBuildUrl( &fetch, url, sizeof( url ) );
		assert( !strcmp( url, "https://ladder.q3rally.com/index.php/api/v1/ghosts?map=q3r_valley&tl=1&rev=0"
			"&physics=1&checksum=-5&perVehicle=5&limit=100&format=text" ) );

		fetch.kind = LADDER_FETCH_GHOST;
		Q_strncpyz( fetch.ghostId, "q3r_valley.tl1_rev0.evo.p1_c-5.abc", sizeof( fetch.ghostId ) );
		SV_LadderFetchBuildUrl( &fetch, url, sizeof( url ) );
		assert( !strcmp( url, "https://ladder.q3rally.com/index.php/api/v1/ghosts/q3r_valley.tl1_rev0.evo.p1_c-5.abc?format=raw" ) );

		Q_strncpyz( s_urlCvar.string, "https://example.com/other", sizeof( s_urlCvar.string ) );
		SV_LadderFetchBuildUrl( &fetch, url, sizeof( url ) );
		assert( url[0] == '\0' );

		SV_LadderFetchCacheName( "q3r_valley", "a.b", 1000, cacheA, sizeof( cacheA ) );
		SV_LadderFetchCacheName( "q3r_valley", "a.b", 999, cacheB, sizeof( cacheB ) );
		assert( !strncmp( cacheA, "ghosts/ladder/q3r_valley/", 25 ) );
		assert( strlen( cacheA ) == 25 + 16 + 6 );
		assert( strcmp( cacheA, cacheB ) );

		count = SV_LadderFetchBuildList( "q3r_valley", body, list, sizeof( list ) );
		assert( count == 2 );
		assert( strstr( list, "\tq3r_valley.tl1_rev0.evo.p1_c-5.1234abcd-0000-4000-8000-000000000001\t61234\tevo\tDriver Q x\n" ) != NULL );
		assert( strstr( list, "\t62000\tsidepipe\tPlayer\n" ) != NULL );
		assert( !strncmp( list, "ghosts/ladder/q3r_valley/", 25 ) );
		assert( SV_LadderFetchBuildList( "q3r_valley", body, list, 40 ) == -1 );
		assert( SV_LadderFetchBuildList( "q3r_valley", "", list, sizeof( list ) ) == 0 );

		/* Targets outside ghosts/ladder/ are refused. */
		assert( SV_LadderFetchTargetIsSafe( "ghosts/ladder/x.list" ) );
		assert( !SV_LadderFetchTargetIsSafe( "ghosts/ladder/../q3config.cfg" ) );
		assert( !SV_LadderFetchTargetIsSafe( "vm/qagame.qvm" ) );

		/* Finished list: written to the target, status cvar set. */
		fetch.kind = LADDER_FETCH_LIST;
		test_fsWriteCount = 0;
		SV_LadderFetchFinish( &fetch, body, 200 );
		assert( test_fsWriteCount == 1 );
		assert( !strcmp( test_fsLastPath, "ghosts/ladder/q3r_valley_tl1_rev0.list" ) );
		assert( !strcmp( test_cvarSetName, "sv_ladderGhostList" ) );
		assert( !strcmp( test_cvarSetValue, "7 ok 2" ) );

		SV_LadderFetchFinish( &fetch, NULL, 503 );
		assert( !strcmp( test_cvarSetValue, "7 fail 0" ) );
		assert( test_fsWriteCount == 1 );

		/* Finished ghost: only plausible .ghost text reaches the cache. */
		fetch.kind = LADDER_FETCH_GHOST;
		fetch.requestId = 9;
		Q_strncpyz( fetch.target, cacheA, sizeof( fetch.target ) );
		SV_LadderFetchFinish( &fetch, "{\"error\":1}", 200 );
		assert( test_fsWriteCount == 1 );
		assert( !strcmp( test_cvarSetName, "sv_ladderGhostFile" ) );
		assert( !strcmp( test_cvarSetValue, "9 fail 0" ) );
		SV_LadderFetchFinish( &fetch, data, 200 );
		assert( test_fsWriteCount == 2 );
		assert( !strcmp( test_fsLastPath, cacheA ) );
		assert( !strcmp( test_fsLastData, data ) );
		assert( !strcmp( test_cvarSetValue, "9 ok 1" ) );

		/* A queued download keeps the result of the one before it. */
		fetch.requestId = 10;
		SV_LadderFetchFinish( &fetch, NULL, 404 );
		assert( !strcmp( test_cvarSetValue, "10 fail 0;9 ok 1" ) );

		/* Long map names are shortened with a hash (MAX_QPATH). */
		{
			char longA[MAX_QPATH];
			char longB[MAX_QPATH];
			SV_LadderFetchCacheName( "q3r_an_extremely_long_map_name_v2", "a.b", 1000, longA, sizeof( longA ) );
			SV_LadderFetchCacheName( "q3r_an_extremely_long_map_name_v3", "a.b", 1000, longB, sizeof( longB ) );
			assert( strlen( longA ) < MAX_QPATH - 1 );
			assert( strchr( longA, '~' ) && strcmp( longA, longB ) );
			assert( !strncmp( longA, "ghosts/ladder/q3r_an_extremel~", 30 ) );
		}

		/* Ghost cache: pruned to the fresh list once it grows too large. */
		{
			static char names[70][32];
			char keepPath[MAX_QPATH];
			const char *keep = keepPath + 25;   /* file name of a listed ghost */
			int i;

			SV_LadderFetchCacheName( "q3r_valley",
				"q3r_valley.tl1_rev0.evo.p1_c-5.1234abcd-0000-4000-8000-000000000001", 61234,
				keepPath, sizeof( keepPath ) );

			for ( i = 0; i < 70; i++ ) {
				snprintf( names[i], sizeof( names[i] ), "%016d.ghost", i );
				test_fsListFiles[i] = names[i];
			}
			snprintf( names[0], sizeof( names[0] ), "%s", keep );
			test_fsListCount = 70;
			test_fsRemoveCount = 0;
			fetch.kind = LADDER_FETCH_LIST;
			fetch.requestId = 11;
			Q_strncpyz( fetch.target, "ghosts/ladder/q3r_valley_tl1_rev0.list", sizeof( fetch.target ) );
			SV_LadderFetchFinish( &fetch, body, 200 );
			assert( !strcmp( test_fsListDir, "ghosts/ladder/q3r_valley" ) );
			assert( test_fsRemoveCount == 69 );
			assert( strcmp( test_fsLastRemoved + 25, keep ) );

			test_fsListCount = 10;     /* small cache: nothing removed */
			test_fsRemoveCount = 0;
			SV_LadderFetchFinish( &fetch, body, 200 );
			assert( test_fsRemoveCount == 0 );
			test_fsListCount = 0;
		}
	}

	puts( "ok" );
	return 0;
}
