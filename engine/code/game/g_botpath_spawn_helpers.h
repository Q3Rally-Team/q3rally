#ifndef G_BOTPATH_SPAWN_HELPERS_H
#define G_BOTPATH_SPAWN_HELPERS_H

#define BOT_PATH_REVERSED_FORWARD	0
#define BOT_PATH_REVERSED_REVERSED	1
#define BOT_PATH_REVERSED_BOTH		2

static ID_INLINE int G_BotPath_ClampPathId( int pathId ) {
	if ( pathId < 0 ) {
		return 0;
	}
	if ( pathId >= MAX_BOT_PATH_ROUTES ) {
		return MAX_BOT_PATH_ROUTES - 1;
	}
	return pathId;
}

static ID_INLINE int G_BotPath_ClampOrder( int order ) {
	if ( order < 0 ) {
		return 0;
	}
	if ( order >= MAX_BOT_PATH_NODES ) {
		return MAX_BOT_PATH_NODES - 1;
	}
	return order;
}

static ID_INLINE float G_BotPath_ClampTargetSpeed( float targetSpeed ) {
	if ( targetSpeed < -1.0f ) {
		return -1.0f;
	}
	if ( targetSpeed > 5000.0f ) {
		return 5000.0f;
	}
	return targetSpeed;
}

static ID_INLINE float G_BotPath_ClampWidth( float width ) {
	if ( width < 1.0f ) {
		return 1.0f;
	}
	if ( width > 8192.0f ) {
		return 8192.0f;
	}
	return width;
}

static ID_INLINE int G_BotPath_NormalizeTrackLengthMask( int trackLengthMask, qboolean *isValid ) {
	if ( isValid ) {
		*isValid = ( trackLengthMask >= 1 && trackLengthMask <= 7 ) ? qtrue : qfalse;
	}
	if ( trackLengthMask < 1 || trackLengthMask > 7 ) {
		return 7;
	}
	return trackLengthMask;
}

static ID_INLINE int G_BotPath_ParseReversedMode( const char *value, qboolean *isValid ) {
	if ( isValid ) {
		*isValid = qtrue;
	}
	if ( !Q_stricmp( value, "both" ) ) {
		return BOT_PATH_REVERSED_BOTH;
	}
	if ( !Q_stricmp( value, "1" ) || !Q_stricmp( value, "true" ) || !Q_stricmp( value, "reversed" ) ) {
		return BOT_PATH_REVERSED_REVERSED;
	}
	if ( !Q_stricmp( value, "0" ) || !Q_stricmp( value, "false" ) || !Q_stricmp( value, "forward" ) ) {
		return BOT_PATH_REVERSED_FORWARD;
	}
	if ( isValid ) {
		*isValid = qfalse;
	}
	return BOT_PATH_REVERSED_FORWARD;
}

static ID_INLINE qboolean G_BotPath_NodeMatchesTrackLengthMask( int trackLengthMask, int trackLength ) {
	int trackLengthBit;

	if ( trackLength < 0 || trackLength > 2 ) {
		return qtrue;
	}
	trackLengthBit = 1 << trackLength;
	return ( trackLengthMask & trackLengthBit ) != 0;
}

static ID_INLINE qboolean G_BotPath_NodeMatchesReversedMode( int reversedMode, int trackReversed, qboolean trackIsReversable ) {
	int activeReverse;

	if ( reversedMode == BOT_PATH_REVERSED_BOTH ) {
		return qtrue;
	}
	activeReverse = ( trackReversed && trackIsReversable ) ? BOT_PATH_REVERSED_REVERSED : BOT_PATH_REVERSED_FORWARD;
	return reversedMode == activeReverse;
}

static ID_INLINE int G_BotPath_CompressUniqueOrdersInPlace( botPathNodeSpawn_t *nodes, int nodeCount ) {
	int i;
	int uniqueCount = 0;

	for ( i = 0; i < nodeCount; ++i ) {
		if ( uniqueCount > 0 && nodes[i].order == nodes[uniqueCount - 1].order ) {
			continue;
		}
		nodes[uniqueCount++] = nodes[i];
	}

	return uniqueCount;
}

#endif
