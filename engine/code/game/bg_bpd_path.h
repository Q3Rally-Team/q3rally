#ifndef BG_BPD_PATH_H
#define BG_BPD_PATH_H

/*
Bezier path (.bpd) file names, shared by game and cgame.

The BPD files are the authoritative race route. A map whose checkpoints differ
between track lengths (rally_checkpoint with a restricted onlyTrackLength)
needs one file per length:

	maps/<map>_tl<n>.bpd	n = g_trackLength (0 short, 1 medium, 2 long)

The generic maps/<map>.bpd is used only when no file for the active length
exists and every checkpoint is shared by all lengths. On a map with length
variants its records would land on the wrong checkpoints, so it is ignored.
*/

#define BPD_TRACK_LENGTHS	3

typedef enum {
	BPD_SOURCE_NONE,
	BPD_SOURCE_LENGTH,
	BPD_SOURCE_GENERIC
} bpdSource_t;

static ID_INLINE qboolean BG_BpdGenericPath( char *out, int size, const char *mapName ) {
	if ( !out || size <= 0 || !mapName || !mapName[0] ) {
		return qfalse;
	}
	return Com_sprintf( out, size, "maps/%s.bpd", mapName ) < size ? qtrue : qfalse;
}

static ID_INLINE qboolean BG_BpdLengthPath( char *out, int size, const char *mapName, int trackLength ) {
	if ( !out || size <= 0 || !mapName || !mapName[0] ) {
		return qfalse;
	}
	if ( trackLength < 0 || trackLength >= BPD_TRACK_LENGTHS ) {
		return qfalse;
	}
	return Com_sprintf( out, size, "maps/%s_tl%d.bpd", mapName, trackLength ) < size ? qtrue : qfalse;
}

/* An onlyTrackLength value that excludes at least one length. */
static ID_INLINE qboolean BG_BpdIsLengthRestricted( const char *onlyTrackLength ) {
	if ( !onlyTrackLength ) {
		return qfalse;
	}
	return ( !strchr( onlyTrackLength, '0' ) || !strchr( onlyTrackLength, '1' )
		|| !strchr( onlyTrackLength, '2' ) ) ? qtrue : qfalse;
}

static ID_INLINE bpdSource_t BG_BpdSelectSource( qboolean lengthFileExists,
	qboolean genericFileExists, qboolean mapHasLengthVariants ) {
	if ( lengthFileExists ) {
		return BPD_SOURCE_LENGTH;
	}
	if ( genericFileExists && !mapHasLengthVariants ) {
		return BPD_SOURCE_GENERIC;
	}
	return BPD_SOURCE_NONE;
}

/* True for maps/<map>.bpd and maps/<map>_tl0..2.bpd (case-insensitive). */
static ID_INLINE qboolean BG_BpdPathMatchesMap( const char *path, const char *mapName ) {
	char expected[MAX_QPATH];
	int i;

	if ( !path || !path[0] ) {
		return qfalse;
	}
	if ( BG_BpdGenericPath( expected, sizeof( expected ), mapName ) && !Q_stricmp( path, expected ) ) {
		return qtrue;
	}
	for ( i = 0; i < BPD_TRACK_LENGTHS; i++ ) {
		if ( BG_BpdLengthPath( expected, sizeof( expected ), mapName, i ) && !Q_stricmp( path, expected ) ) {
			return qtrue;
		}
	}
	return qfalse;
}

#endif
