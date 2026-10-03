/*
 * Unit test for the central gametype classification in bg_misc.c.
 * GT_GHOST must behave like a race everywhere (ghost recording/playback,
 * intro camera, bot route source) while being the only human-only mode.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../engine/code/game/bg_misc.c"

void QDECL Com_Error( int level, const char *fmt, ... ) {
	(void)level;
	(void)fmt;
	assert( 0 && "Com_Error called" );
}

void QDECL Com_Printf( const char *fmt, ... ) {
	(void)fmt;
}

/* bg_misc.c references this game/cgame helper; not needed here */
qboolean isRaceObserver( int clientNum ) {
	(void)clientNum;
	return qfalse;
}

static const int s_validGametypes[] = {
	GT_RACING, GT_RACING_DM, GT_SINGLE_PLAYER, GT_DERBY, GT_LCS, GT_ELIMINATION,
	GT_DEATHMATCH, GT_SPRINT, GT_GHOST, GT_TEAM, GT_TEAM_RACING, GT_TEAM_RACING_DM,
	GT_CTF, GT_CTF4, GT_DOMINATION, GT_KOTH
};

static qboolean IsValidGametype( int gametype ) {
	size_t i;
	for ( i = 0; i < sizeof( s_validGametypes ) / sizeof( s_validGametypes[0] ); i++ ) {
		if ( s_validGametypes[i] == gametype ) {
			return qtrue;
		}
	}
	return qfalse;
}

int main( void ) {
	int gt;

	/* GT_GHOST is a non-team mode with a stable value */
	assert( GT_GHOST == 8 );
	assert( GT_GHOST < GT_TEAM );

	/* Ghost Race is a plain race: ghosts are recorded and played back */
	assert( BG_GametypeIsRace( GT_GHOST ) );
	assert( BG_GametypeIsTimedRace( GT_GHOST ) );
	assert( BG_GametypeIsNonDMRace( GT_GHOST, qfalse ) );
	assert( BG_GametypeIsNonDMRace( GT_GHOST, qtrue ) );
	assert( BG_GametypeHasRaceFinish( GT_GHOST ) );

	/* ...but the only mode without bots */
	assert( !BG_GametypeAllowsBots( GT_GHOST ) );
	for ( gt = 0; gt < GT_MAX_GAME_TYPE; gt++ ) {
		if ( gt != GT_GHOST ) {
			assert( BG_GametypeAllowsBots( gt ) );
		}
	}

	/* Ghost Race uses the racing entity set of a map */
	assert( !strcmp( BG_GametypeEntityName( GT_GHOST ), "racing" ) );
	assert( !strcmp( BG_GametypeDisplayName( GT_GHOST ), "Ghost Race" ) );

	/* Unchanged classification of the existing modes */
	assert( BG_GametypeIsRace( GT_RACING ) && BG_GametypeIsRace( GT_RACING_DM ) );
	assert( BG_GametypeIsRace( GT_SPRINT ) && BG_GametypeIsRace( GT_ELIMINATION ) );
	assert( BG_GametypeIsRace( GT_TEAM_RACING ) && BG_GametypeIsRace( GT_TEAM_RACING_DM ) );
	assert( BG_GametypeIsRace( GT_SINGLE_PLAYER ) );
	assert( !BG_GametypeIsRace( GT_DERBY ) && !BG_GametypeIsRace( GT_LCS ) );
	assert( !BG_GametypeIsRace( GT_DEATHMATCH ) && !BG_GametypeIsRace( GT_CTF ) );
	assert( !BG_GametypeIsTimedRace( GT_RACING_DM ) && !BG_GametypeIsTimedRace( GT_ELIMINATION ) );
	assert( BG_GametypeIsNonDMRace( GT_ELIMINATION, qfalse ) );
	assert( !BG_GametypeIsNonDMRace( GT_ELIMINATION, qtrue ) );
	assert( !BG_GametypeHasRaceFinish( GT_ELIMINATION ) );

	/* Entity names follow the GT_* values (regression: the old table in
	   g_spawn.c was indexed by g_gametype and out of sync) */
	assert( !strcmp( BG_GametypeEntityName( GT_SPRINT ), "sprint" ) );
	assert( !strcmp( BG_GametypeEntityName( GT_SINGLE_PLAYER ), "single" ) );
	assert( !strcmp( BG_GametypeEntityName( GT_TEAM ), "team" ) );
	assert( !strcmp( BG_GametypeEntityName( GT_KOTH ), "koth" ) );
	for ( gt = -1; gt <= GT_MAX_GAME_TYPE; gt++ ) {
		if ( IsValidGametype( gt ) ) {
			assert( BG_GametypeEntityName( gt ) != NULL );
			assert( strcmp( BG_GametypeDisplayName( gt ), "Unknown" ) );
		} else {
			assert( BG_GametypeEntityName( gt ) == NULL );
			assert( !strcmp( BG_GametypeDisplayName( gt ), "Unknown" ) );
		}
	}

	printf( "ok\n" );
	return 0;
}
