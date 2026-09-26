/* Shared, side-effect-free King of the Hill rules used by the game and tests. */
#ifndef BG_KOTH_H
#define BG_KOTH_H

#include "bg_public.h"

static qboolean BG_KOTH_HillBoundsValid( const vec3_t mins, const vec3_t maxs ) {
	int axis;

	for ( axis = 0; axis < 3; axis++ ) {
		if ( maxs[axis] <= mins[axis] ) {
			return qfalse;
		}
	}
	return qtrue;
}

static qboolean BG_KOTH_MapHillValid( int hillCount, const vec3_t mins, const vec3_t maxs ) {
	return ( hillCount == 1 && BG_KOTH_HillBoundsValid( mins, maxs ) ) ? qtrue : qfalse;
}

static int BG_KOTH_OvertimeHolder( int owner, qboolean contested, int redCount, int blueCount ) {
	if ( contested ) {
		return TEAM_FREE;
	}
	if ( owner == TEAM_RED && redCount > 0 && blueCount == 0 ) {
		return TEAM_RED;
	}
	if ( owner == TEAM_BLUE && blueCount > 0 && redCount == 0 ) {
		return TEAM_BLUE;
	}
	return TEAM_FREE;
}

static qboolean BG_KOTH_OvertimeHoldComplete( int owner, int ownerSince, int now, int holdMs ) {
	if ( owner != TEAM_RED && owner != TEAM_BLUE ) {
		return qfalse;
	}
	if ( holdMs <= 0 || now < ownerSince ) {
		return qfalse;
	}
	return ( now - ownerSince >= holdMs ) ? qtrue : qfalse;
}

static int BG_KOTH_AddElapsedMs( int totalMs, int elapsedMs ) {
	if ( totalMs < 0 ) {
		totalMs = 0;
	}
	if ( elapsedMs <= 0 ) {
		return totalMs;
	}
	if ( totalMs > 0x7fffffff - elapsedMs ) {
		return 0x7fffffff;
	}
	return totalMs + elapsedMs;
}

#endif /* BG_KOTH_H */
