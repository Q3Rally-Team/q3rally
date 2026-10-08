/*
 * Unit test for the slipstream state in bg_misc.c (STAT_SLIPSTREAM).
 * The server sets a target, pmove ramps the current value with the command
 * msec. Client prediction must reach the same value with any split of the
 * same time into commands, and setting a new target must keep the ramp.
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

qboolean isRaceObserver( int clientNum ) {
	(void)clientNum;
	return qfalse;
}

static void Run( playerState_t *ps, int totalMsec, int commandMsec ) {
	while ( totalMsec > 0 ) {
		int msec = totalMsec < commandMsec ? totalMsec : commandMsec;
		BG_SlipstreamStep( ps, msec );
		totalMsec -= msec;
	}
}

int main( void ) {
	playerState_t ps;
	int current;

	/* wire layout: the stat must still fit the network array and a short */
	assert( STAT_SLIPSTREAM < MAX_STATS );
	assert( ( SLIPSTREAM_TARGET_MAX << SLIPSTREAM_TARGET_SHIFT | SLIPSTREAM_CURRENT_MAX ) <= 32767 );

	memset( &ps, 0, sizeof( ps ) );
	assert( BG_SlipstreamFactor( &ps ) == 0.0f );

	/* setting the target alone changes nothing yet */
	BG_SlipstreamSetTarget( &ps, 0.25f );
	assert( BG_SlipstreamCurrent( &ps ) == 0 );

	/* ramps up, never overshoots, reaches the target within the build time */
	Run( &ps, SLIPSTREAM_BUILD_MSEC / 10, 8 );
	current = BG_SlipstreamCurrent( &ps );
	assert( current > 0 && current < 64 );
	Run( &ps, SLIPSTREAM_BUILD_MSEC, 8 );
	assert( BG_SlipstreamCurrent( &ps ) == 64 );   /* 0.25 -> 32/127 -> 64/255 */
	assert( BG_SlipstreamFactor( &ps ) > 0.24f && BG_SlipstreamFactor( &ps ) < 0.26f );

	/* a new target from the server keeps the current value */
	BG_SlipstreamSetTarget( &ps, 0.0f );
	assert( BG_SlipstreamCurrent( &ps ) == 64 );
	Run( &ps, SLIPSTREAM_DECAY_MSEC, 8 );
	assert( BG_SlipstreamCurrent( &ps ) == 0 );

	/* full range decays within the decay time */
	BG_SlipstreamSetTarget( &ps, 1.0f );
	Run( &ps, SLIPSTREAM_BUILD_MSEC + 50, 16 );
	assert( BG_SlipstreamCurrent( &ps ) == SLIPSTREAM_CURRENT_MAX );
	BG_SlipstreamSetTarget( &ps, 0.0f );
	Run( &ps, SLIPSTREAM_DECAY_MSEC + 50, 16 );
	assert( BG_SlipstreamCurrent( &ps ) == 0 );

	/* out of range targets are clamped */
	BG_SlipstreamSetTarget( &ps, 5.0f );
	assert( ( ps.stats[STAT_SLIPSTREAM] >> SLIPSTREAM_TARGET_SHIFT ) == SLIPSTREAM_TARGET_MAX );
	BG_SlipstreamSetTarget( &ps, -1.0f );
	assert( ( ps.stats[STAT_SLIPSTREAM] >> SLIPSTREAM_TARGET_SHIFT ) == 0 );

	/* tiny commands still make progress (prediction with high fps) */
	BG_SlipstreamSetTarget( &ps, 0.25f );
	BG_SlipstreamStep( &ps, 1 );
	assert( BG_SlipstreamCurrent( &ps ) == 1 );

	/* other stats are untouched */
	memset( &ps, 0, sizeof( ps ) );
	ps.stats[STAT_FUEL] = 77;
	BG_SlipstreamSetTarget( &ps, 0.5f );
	Run( &ps, 1000, 8 );
	assert( ps.stats[STAT_FUEL] == 77 );

	printf( "ok\n" );
	return 0;
}
