/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - perle@q3rally.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
===========================================================================
*/

/*
===========================================================================
cg_rally_intro_cam.c  --  client-side intro camera sequence evaluator

WHY THIS EXISTS
---------------
The server evaluates G_ApplyIntroCamSequence() once per game frame
(~20 Hz at sv_fps 20) and writes the result into ps.origin /
ps.viewangles.  The Q3 snapshot system then ships those values to the
client, which interpolates between two consecutive snapshots.

That gives two problems:
  1. The camera only updates at 20 Hz regardless of client framerate,
     causing visible stutter on fast movements.
  2. EASE_IN_OUT blending is computed on the server; the client then
     linearly interpolates between already-eased samples, distorting
     the curve and producing small jumps where the ease gradient changes
     quickly.

SOLUTION
--------
The server serialises the full sequence into CS_INTRO_CAM once at
map load (G_ObserverCamSequence_WriteConfigstring).  When the intro
starts the server broadcasts "introCamStart <level.time>" to all clients.

This module reads the configstring, stores the nodes locally, and
evaluates the correct camera position using cg.time every render frame.
The result is written directly into cg.refdef, bypassing ps.origin
entirely.  The camera now runs at whatever framerate the client achieves
and blend curves are mathematically exact.

CONFIGSTRING FORMAT  (CS_INTRO_CAM)
------------------------------------
Mapper-authored sequence:
  "<N> px py pz ax ay az durMs blend fov hasLookAt [lx ly lz] ..."

Packaged Ghost route:
  "ghost <route-file> <preview-duration-ms> <effective-reversed>"

The route reference keeps long paths out of configstrings; cgame loads and
evaluates the matching camera-only route from the packaged intro_routes folder.

PUBLIC API
----------
  CG_IntroCam_ParseConfigstring()    call from CG_SetConfigValues and
                                     whenever CS_INTRO_CAM changes
  CG_IntroCam_SetStartTime(t)        call when "introCamStart" cmd arrives
  CG_IntroCam_IsActive()             qtrue while sequence is running
  CG_IntroCam_CalcView(o,a,fov)      fills output; returns qtrue when
                                     active -- caller must return
                                     CG_CalcFov() immediately after
===========================================================================
*/

#include "cg_local.h"

/* ------------------------------------------------------------------ */
/* Constants                                                           */
/* ------------------------------------------------------------------ */

#define CG_MAX_INTRO_CAM_NODES  64
#define CG_MAX_INTRO_GHOST_ROUTE_FRAMES 256
#define CG_MAX_INTRO_GHOST_ROUTE_FILE_SIZE ( 64 * 1024 )

/* ------------------------------------------------------------------ */
/* Local types                                                         */
/* ------------------------------------------------------------------ */

typedef struct {
	vec3_t		position;
	vec3_t		angles;
	int			durationMs;
	int			blendType;      /* 0=cut, 1=linear, 2=ease-in-out */
	float		fov;
	qboolean	hasLookAt;
	vec3_t		lookAt;
} cgIntroCamNode_t;

typedef struct {
	int		timeMs;
	vec3_t	origin;
	vec3_t	angles;
} cgIntroGhostRouteFrame_t;

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static cgIntroCamNode_t		s_nodes[CG_MAX_INTRO_CAM_NODES];
static int					s_nodeCount       = 0;
static int					s_totalDurationMs = 0;
static int					s_startTime       = 0;
static qboolean				s_hasSequence     = qfalse;
static qboolean				s_useGhostRoute   = qfalse;
static qboolean				s_skipped         = qfalse;
static cgIntroGhostRouteFrame_t s_ghostFrames[CG_MAX_INTRO_GHOST_ROUTE_FRAMES];
static int					s_ghostFrameCount = 0;
static int					s_ghostRouteDurationMs = 0;

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static float CG_IntroCam_Ease( int blendType, float t ) {
	if ( t <= 0.0f ) return 0.0f;
	if ( t >= 1.0f ) return 1.0f;
	switch ( blendType ) {
	case 1:  return t;
	case 2:  return t * t * ( 3.0f - 2.0f * t );
	default: return 0.0f;   /* cut: snap to start of segment */
	}
}

/* Read one whitespace-delimited token from *p, advance *p. */
static qboolean CG_IntroCam_NextToken( const char **p, char *buf, int bufSize ) {
	const char *s = *p;
	int i = 0;

	while ( *s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' ) s++;
	if ( !*s ) return qfalse;

	while ( *s && *s != ' ' && *s != '\t' && *s != '\r' && *s != '\n' ) {
		if ( i < bufSize - 1 ) buf[i++] = *s;
		s++;
	}
	buf[i] = '\0';
	*p = s;
	return ( i > 0 );
}

static qboolean CG_IntroCam_LoadGhostRoute( const char *path, int previewDurationMs, int expectedTrackReversed ) {
	fileHandle_t file;
	int fileLength;
	static char buffer[CG_MAX_INTRO_GHOST_ROUTE_FILE_SIZE + 1];
	const char *cursor;
	char token[64];
	char routeMap[MAX_QPATH];
	char serverInfo[MAX_INFO_STRING];
	char expectedMap[MAX_QPATH];
	int routeTrackLength, routeTrackReversed, expectedTrackLength;
	int frameCount, i, previousTime;

	s_ghostFrameCount = 0;
	s_ghostRouteDurationMs = 0;
	s_useGhostRoute = qfalse;

	if ( !path || Q_stricmpn( path, "intro_routes/", 13 ) ) {
		CG_Printf( "^3CG_IntroCam: invalid Ghost route path\n" );
		return qfalse;
	}
	if ( previewDurationMs < 1000 || previewDurationMs > 60000 ) {
		CG_Printf( "^3CG_IntroCam: invalid Ghost preview duration %d ms\n", previewDurationMs );
		return qfalse;
	}

	fileLength = trap_FS_FOpenFile( path, &file, FS_READ );
	if ( fileLength < 0 ) {
		CG_Printf( "^3CG_IntroCam: could not open packaged route %s\n", path );
		return qfalse;
	}
	if ( fileLength == 0 ) {
		trap_FS_FCloseFile( file );
		CG_Printf( "^3CG_IntroCam: packaged route %s is empty\n", path );
		return qfalse;
	}
	if ( fileLength > CG_MAX_INTRO_GHOST_ROUTE_FILE_SIZE ) {
		trap_FS_FCloseFile( file );
		CG_Printf( "^3CG_IntroCam: route %s exceeds the %d byte limit\n",
			path, CG_MAX_INTRO_GHOST_ROUTE_FILE_SIZE );
		return qfalse;
	}
	trap_FS_Read( buffer, fileLength, file );
	buffer[fileLength] = '\0';
	trap_FS_FCloseFile( file );

	cursor = buffer;
	if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, "Q3RALLY_INTRO_ROUTE" )
		|| !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || atoi( token ) != 1 ) {
		CG_Printf( "^3CG_IntroCam: unsupported route header in %s\n", path );
		return qfalse;
	}
	if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, "map" )
		|| !CG_IntroCam_NextToken( &cursor, routeMap, sizeof( routeMap ) )
		|| !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, "track_length" )
		|| !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: invalid route metadata in %s\n", path );
		return qfalse;
	}
	routeTrackLength = atoi( token );
	if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, "track_reversed" )
		|| !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: invalid route variant in %s\n", path );
		return qfalse;
	}
	routeTrackReversed = atoi( token ) ? 1 : 0;
	if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, "frames" )
		|| !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: missing frame count in %s\n", path );
		return qfalse;
	}
	frameCount = atoi( token );
	if ( frameCount < 2 || frameCount > CG_MAX_INTRO_GHOST_ROUTE_FRAMES ) {
		CG_Printf( "^3CG_IntroCam: invalid frame count %d in %s\n", frameCount, path );
		return qfalse;
	}

	Q_strncpyz( serverInfo, CG_ConfigString( CS_SERVERINFO ), sizeof( serverInfo ) );
	Q_strncpyz( expectedMap, Info_ValueForKey( serverInfo, "mapname" ), sizeof( expectedMap ) );
	expectedTrackLength = atoi( Info_ValueForKey( serverInfo, "g_trackLength" ) );
	if ( Q_stricmp( routeMap, expectedMap ) || routeTrackLength != expectedTrackLength
		|| routeTrackReversed != expectedTrackReversed ) {
		CG_Printf( "^3CG_IntroCam: route variant does not match this server (%s tl%d rev%d)\n",
			routeMap, routeTrackLength, routeTrackReversed );
		return qfalse;
	}

	previousTime = -1;
	for ( i = 0; i < frameCount; i++ ) {
		cgIntroGhostRouteFrame_t *frame = &s_ghostFrames[i];
		int axis;

		if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) {
			CG_Printf( "^3CG_IntroCam: route %s ends before frame %d\n", path, i );
			return qfalse;
		}
		frame->timeMs = atoi( token );
		if ( frame->timeMs < 0 || frame->timeMs <= previousTime || frame->timeMs > 60000 ) {
			CG_Printf( "^3CG_IntroCam: invalid route time at frame %d in %s\n", i, path );
			return qfalse;
		}
		previousTime = frame->timeMs;
		for ( axis = 0; axis < 3; axis++ ) {
			if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) return qfalse;
			frame->origin[axis] = (float)atof( token );
			if ( IS_NAN( frame->origin[axis] ) || frame->origin[axis] < -131072.0f || frame->origin[axis] > 131072.0f ) return qfalse;
		}
		for ( axis = 0; axis < 3; axis++ ) {
			if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) return qfalse;
			frame->angles[axis] = (float)atof( token );
			if ( IS_NAN( frame->angles[axis] ) || frame->angles[axis] < -3600.0f || frame->angles[axis] > 3600.0f ) return qfalse;
		}
	}

	s_ghostFrameCount = frameCount;
	s_ghostRouteDurationMs = s_ghostFrames[frameCount - 1].timeMs;
	if ( s_ghostRouteDurationMs <= 0 ) {
		s_ghostFrameCount = 0;
		return qfalse;
	}
	s_totalDurationMs = previewDurationMs;
	s_useGhostRoute = qtrue;
	s_hasSequence = qtrue;
	if ( cg_developer.integer ) {
		CG_Printf( "CG_IntroCam: loaded %d route points from %s (%d ms preview)\n",
			s_ghostFrameCount, path, s_totalDurationMs );
	}
	return qtrue;
}

/* ------------------------------------------------------------------ */
/* Parse configstring                                                  */
/* ------------------------------------------------------------------ */

void CG_IntroCam_ParseConfigstring( void ) {
	const char	*cs;
	const char	*p;
	char		tok[64];
	int			i, count, totalMs;

	s_nodeCount       = 0;
	s_totalDurationMs = 0;
	s_hasSequence     = qfalse;
	s_useGhostRoute   = qfalse;
	s_ghostFrameCount = 0;
	s_ghostRouteDurationMs = 0;
	/* Do NOT reset s_startTime here: a late-joining client may receive
	   the configstring after the start command already arrived. */

	cs = CG_ConfigString( CS_INTRO_CAM );
	if ( !cs || !cs[0] ) {
		return;
	}

	p = cs;

	if ( !CG_IntroCam_NextToken( &p, tok, sizeof( tok ) ) ) {
		CG_Printf( "^3CG_IntroCam: empty configstring\n" );
		return;
	}
	if ( !Q_stricmp( tok, "ghost" ) ) {
		char routePath[MAX_QPATH];
		int previewDurationMs;
		int expectedTrackReversed;

		if ( !CG_IntroCam_NextToken( &p, routePath, sizeof( routePath ) )
			|| !CG_IntroCam_NextToken( &p, tok, sizeof( tok ) ) ) {
			CG_Printf( "^3CG_IntroCam: malformed Ghost route configstring\n" );
			return;
		}
		previewDurationMs = atoi( tok );
		if ( !CG_IntroCam_NextToken( &p, tok, sizeof( tok ) ) ) {
			CG_Printf( "^3CG_IntroCam: missing Ghost route direction\n" );
			return;
		}
		expectedTrackReversed = atoi( tok ) ? 1 : 0;
		CG_IntroCam_LoadGhostRoute( routePath, previewDurationMs, expectedTrackReversed );
		return;
	}
	count = atoi( tok );
	if ( count <= 0 || count > CG_MAX_INTRO_CAM_NODES ) {
		CG_Printf( "^3CG_IntroCam: invalid node count %d\n", count );
		return;
	}

	totalMs = 0;

	for ( i = 0; i < count; i++ ) {
		cgIntroCamNode_t *n = &s_nodes[i];
		int hl;

#define RDF( field ) \
		if ( !CG_IntroCam_NextToken( &p, tok, sizeof(tok) ) ) { \
			CG_Printf( "^3CG_IntroCam: parse error node %d\n", i ); \
			s_nodeCount = 0; return; \
		} (field) = (float)atof( tok );

#define RDI( field ) \
		if ( !CG_IntroCam_NextToken( &p, tok, sizeof(tok) ) ) { \
			CG_Printf( "^3CG_IntroCam: parse error node %d\n", i ); \
			s_nodeCount = 0; return; \
		} (field) = atoi( tok );

		RDF( n->position[0] )  RDF( n->position[1] )  RDF( n->position[2] )
		RDF( n->angles[0]   )  RDF( n->angles[1]   )  RDF( n->angles[2]   )
		RDI( n->durationMs  )
		RDI( n->blendType   )
		RDF( n->fov         )
		RDI( hl             )
		n->hasLookAt = hl ? qtrue : qfalse;

		if ( n->hasLookAt ) {
			RDF( n->lookAt[0] )  RDF( n->lookAt[1] )  RDF( n->lookAt[2] )
		} else {
			VectorClear( n->lookAt );
		}

#undef RDF
#undef RDI

		if ( n->durationMs <= 0 ) {
			CG_Printf( "^3CG_IntroCam: node %d zero duration\n", i );
			s_nodeCount = 0;
			return;
		}
		totalMs += n->durationMs;
	}

	if ( totalMs <= 0 ) {
		CG_Printf( "^3CG_IntroCam: zero total duration\n" );
		return;
	}

	s_nodeCount       = count;
	s_totalDurationMs = totalMs;
	s_hasSequence     = qtrue;

	if (cg_developer.integer) CG_Printf( "CG_IntroCam: %d nodes, %d ms\n", s_nodeCount, s_totalDurationMs );
}

/* ------------------------------------------------------------------ */
/* Start time                                                          */
/* ------------------------------------------------------------------ */

/*
Called when the server sends "introCamStart <serverTime>".
serverTime == level.time at the moment RACE_STATE_INTRO_CAM was entered.
cg.time and level.time share the same epoch (ms since level start),
so serverTime is used directly as the elapsed-time base.
*/
void CG_IntroCam_SetStartTime( int serverTime ) {
	s_startTime = serverTime;
	s_skipped = qfalse;
	if (cg_developer.integer) CG_Printf( "CG_IntroCam: startTime=%d cg.time=%d\n", s_startTime, cg.time );
}

void CG_IntroCam_Skip( void ) {
	if ( s_startTime > 0 ) {
		s_skipped = qtrue;
		if ( cg_developer.integer ) {
			CG_Printf( "CG_IntroCam: skipped at cg.time=%d\n", cg.time );
		}
	}
}

/* ------------------------------------------------------------------ */
/* Active query                                                        */
/* ------------------------------------------------------------------ */

qboolean CG_IntroCam_IsRaceIntroPending( void ) {
	int elapsed;
	if ( !s_hasSequence || ( !s_useGhostRoute && s_nodeCount <= 0 ) || s_startTime <= 0 ) return qfalse;
	elapsed = cg.time - s_startTime;
	return ( elapsed >= 0 && elapsed < s_totalDurationMs ) ? qtrue : qfalse;
}

qboolean CG_IntroCam_IsActive( void ) {
	return ( !s_skipped && CG_IntroCam_IsRaceIntroPending() ) ? qtrue : qfalse;
}

int CG_IntroCam_RemainingSeconds( void ) {
	int remainingMs;

	if ( !CG_IntroCam_IsActive() ) {
		return 0;
	}

	remainingMs = s_totalDurationMs - ( cg.time - s_startTime );
	return ( remainingMs + 999 ) / 1000;
}

static void CG_IntroCam_SampleGhostRoute( int targetTime, vec3_t origin, vec3_t angles ) {
	int i;
	const cgIntroGhostRouteFrame_t *from, *to;
	float fraction;

	if ( targetTime <= s_ghostFrames[0].timeMs ) {
		VectorCopy( s_ghostFrames[0].origin, origin );
		VectorCopy( s_ghostFrames[0].angles, angles );
		return;
	}
	if ( targetTime >= s_ghostFrames[s_ghostFrameCount - 1].timeMs ) {
		VectorCopy( s_ghostFrames[s_ghostFrameCount - 1].origin, origin );
		VectorCopy( s_ghostFrames[s_ghostFrameCount - 1].angles, angles );
		return;
	}

	for ( i = 0; i < s_ghostFrameCount - 1; i++ ) {
		if ( targetTime > s_ghostFrames[i + 1].timeMs ) {
			continue;
		}
		from = &s_ghostFrames[i];
		to = &s_ghostFrames[i + 1];
		fraction = (float)( targetTime - from->timeMs ) / (float)( to->timeMs - from->timeMs );
		origin[0] = from->origin[0] + ( to->origin[0] - from->origin[0] ) * fraction;
		origin[1] = from->origin[1] + ( to->origin[1] - from->origin[1] ) * fraction;
		origin[2] = from->origin[2] + ( to->origin[2] - from->origin[2] ) * fraction;
		angles[PITCH] = LerpAngle( from->angles[PITCH], to->angles[PITCH], fraction );
		angles[YAW] = LerpAngle( from->angles[YAW], to->angles[YAW], fraction );
		angles[ROLL] = LerpAngle( from->angles[ROLL], to->angles[ROLL], fraction );
		return;
	}

	VectorCopy( s_ghostFrames[s_ghostFrameCount - 1].origin, origin );
	VectorCopy( s_ghostFrames[s_ghostFrameCount - 1].angles, angles );
}

static qboolean CG_IntroCam_CalcGhostRouteView( int elapsed, vec3_t originOut, vec3_t anglesOut, float *fovOut ) {
	vec3_t routeOrigin, routeAngles;
	vec3_t lookAt, desiredOrigin, direction;
	vec3_t traceMins = { -4.0f, -4.0f, -4.0f };
	vec3_t traceMaxs = {  4.0f,  4.0f,  4.0f };
	trace_t trace;
	float yawRadians;
	float forwardX, forwardY, rightX, rightY;
	int routeTime;

	if ( s_ghostFrameCount < 2 || s_ghostRouteDurationMs <= 0 || s_totalDurationMs <= 0 ) {
		return qfalse;
	}

	routeTime = (int)( ( (float)elapsed / (float)s_totalDurationMs ) * (float)s_ghostRouteDurationMs );
	CG_IntroCam_SampleGhostRoute( routeTime, routeOrigin, routeAngles );

	yawRadians = DEG2RAD( routeAngles[YAW] );
	forwardX = (float)cos( yawRadians );
	forwardY = (float)sin( yawRadians );
	rightX = forwardY;
	rightY = -forwardX;

	VectorSet( lookAt,
		routeOrigin[0] + forwardX * 220.0f,
		routeOrigin[1] + forwardY * 220.0f,
		routeOrigin[2] + 55.0f );
	VectorSet( desiredOrigin,
		routeOrigin[0] - forwardX * 340.0f + rightX * 45.0f,
		routeOrigin[1] - forwardY * 340.0f + rightY * 45.0f,
		routeOrigin[2] + 165.0f );

	CG_Trace( &trace, lookAt, traceMins, traceMaxs, desiredOrigin,
		cg.predictedPlayerState.clientNum, MASK_SOLID );
	VectorCopy( trace.endpos, originOut );
	VectorSubtract( lookAt, originOut, direction );
	if ( VectorLengthSquared( direction ) < 1.0f ) {
		VectorCopy( routeAngles, anglesOut );
	} else {
		vectoangles( direction, anglesOut );
		anglesOut[ROLL] = 0.0f;
	}

	if ( fovOut ) {
		*fovOut = 82.0f;
	}
	return qtrue;
}

/* ------------------------------------------------------------------ */
/* Main evaluator                                                      */
/* ------------------------------------------------------------------ */

qboolean CG_IntroCam_CalcView( vec3_t originOut, vec3_t anglesOut, float *fovOut ) {
	int		elapsed, segStart, ni, nNext;
	float	t, blend;
	const cgIntroCamNode_t *node, *next;
	vec3_t	origin, angles;

	if ( s_skipped || !s_hasSequence || ( !s_useGhostRoute && s_nodeCount <= 0 ) || s_startTime <= 0 ) return qfalse;

	elapsed = cg.time - s_startTime;
	if ( elapsed < 0 )                  elapsed = 0;
	if ( elapsed >= s_totalDurationMs ) return qfalse;
	if ( s_useGhostRoute ) {
		return CG_IntroCam_CalcGhostRouteView( elapsed, originOut, anglesOut, fovOut );
	}

	segStart = 0;
	ni       = 0;
	nNext    = 0;
	t        = 0.0f;

	for ( ni = 0; ni < s_nodeCount; ni++ ) {
		int dur = s_nodes[ni].durationMs;
		if ( ni == s_nodeCount - 1 || elapsed < segStart + dur ) {
			nNext = ( ni + 1 < s_nodeCount ) ? ni + 1 : ni;
			t     = ( dur > 0 ) ? (float)( elapsed - segStart ) / (float)dur : 0.0f;
			if ( t < 0.0f ) t = 0.0f;
			if ( t > 1.0f ) t = 1.0f;
			break;
		}
		segStart += dur;
	}

	node  = &s_nodes[ni];
	next  = &s_nodes[nNext];
	blend = CG_IntroCam_Ease( node->blendType, t );

	origin[0] = node->position[0] + ( next->position[0] - node->position[0] ) * blend;
	origin[1] = node->position[1] + ( next->position[1] - node->position[1] ) * blend;
	origin[2] = node->position[2] + ( next->position[2] - node->position[2] ) * blend;

	if ( node->hasLookAt ) {
		vec3_t delta;
		VectorSubtract( node->lookAt, origin, delta );
		if ( VectorLengthSquared( delta ) > 0.001f ) {
			vectoangles( delta, angles );
		} else {
			VectorCopy( node->angles, angles );
		}
	} else {
		angles[0] = LerpAngle( node->angles[0], next->angles[0], blend );
		angles[1] = LerpAngle( node->angles[1], next->angles[1], blend );
		angles[2] = LerpAngle( node->angles[2], next->angles[2], blend );
	}

	if ( fovOut ) {
		*fovOut = node->fov + ( next->fov - node->fov ) * blend;
	}

	VectorCopy( origin, originOut );
	VectorCopy( angles, anglesOut );
	return qtrue;
}
