#include <assert.h>
#include <stdio.h>

#include "q_shared.h"
#include "bg_koth.h"

int main( void ) {
	vec3_t mins = { -64.0f, -64.0f, 0.0f };
	vec3_t maxs = { 64.0f, 64.0f, 96.0f };
	vec3_t flatMaxs = { 64.0f, 64.0f, 0.0f };

	assert( BG_KOTH_MapHillValid( 1, mins, maxs ) == qtrue );
	assert( BG_KOTH_MapHillValid( 0, mins, maxs ) == qfalse );
	assert( BG_KOTH_MapHillValid( 2, mins, maxs ) == qfalse );
	assert( BG_KOTH_MapHillValid( 1, mins, flatMaxs ) == qfalse );

	assert( BG_KOTH_OvertimeHolder( TEAM_RED, qfalse, 1, 0 ) == TEAM_RED );
	assert( BG_KOTH_OvertimeHolder( TEAM_BLUE, qfalse, 0, 1 ) == TEAM_BLUE );
	assert( BG_KOTH_OvertimeHolder( TEAM_RED, qfalse, 0, 0 ) == TEAM_FREE );
	assert( BG_KOTH_OvertimeHolder( TEAM_RED, qtrue, 1, 1 ) == TEAM_FREE );
	assert( BG_KOTH_OvertimeHolder( TEAM_BLUE, qfalse, 1, 0 ) == TEAM_FREE );

	assert( BG_KOTH_OvertimeHoldComplete( TEAM_RED, 0, 9999, 10000 ) == qfalse );
	assert( BG_KOTH_OvertimeHoldComplete( TEAM_RED, 0, 10000, 10000 ) == qtrue );
	assert( BG_KOTH_OvertimeHoldComplete( TEAM_FREE, 0, 10000, 10000 ) == qfalse );
	assert( BG_KOTH_OvertimeHoldComplete( TEAM_BLUE, 100, 99, 10000 ) == qfalse );

	assert( BG_KOTH_AddElapsedMs( 2000, 16 ) == 2016 );
	assert( BG_KOTH_AddElapsedMs( 2000, -16 ) == 2000 );
	assert( BG_KOTH_AddElapsedMs( 0x7fffffff - 5, 16 ) == 0x7fffffff );

	puts( "ok" );
	return 0;
}
