#ifndef AI_DMNET_PATHSELECT_H
#define AI_DMNET_PATHSELECT_H

static ID_INLINE int Bot_SelectRouteIdWithFallbackByAvailability( const qboolean available[], int routeCount, int preferredId, int baseId ) {
	int order[8];
	int cursor = 0;
	int i;

	if ( routeCount <= 0 || routeCount > 8 ) {
		return -1;
	}
	if ( preferredId < 0 || preferredId >= routeCount ) {
		preferredId = baseId;
	}
	if ( baseId < 0 || baseId >= routeCount ) {
		baseId = 0;
	}

	order[cursor++] = preferredId;
	if ( preferredId != baseId ) {
		order[cursor++] = baseId;
	}
	for ( i = 0; i < routeCount; ++i ) {
		int j;
		qboolean alreadyAdded = qfalse;
		for ( j = 0; j < cursor; ++j ) {
			if ( order[j] == i ) {
				alreadyAdded = qtrue;
				break;
			}
		}
		if ( !alreadyAdded ) {
			order[cursor++] = i;
		}
	}

	for ( i = 0; i < cursor; ++i ) {
		int routeId = order[i];
		if ( available[routeId] ) {
			return routeId;
		}
	}

	return -1;
}

#endif
