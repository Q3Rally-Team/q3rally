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
cg_rally_intro_cam.c  --  client-side packaged Ghost route camera

The server selects a route matching the active map, track length, and
travel direction, then broadcasts the synchronized intro start time. Each
client loads that packaged camera-only route and evaluates its view from
cg.time every render frame. This keeps the camera smooth at any client
framerate while the server owns the shared race-start countdown.

CONFIGSTRING FORMAT (CS_INTRO_ROUTE)
------------------------------------
  "ghost <route-file> <preview-duration-ms> <effective-reversed> <pending-start>"

Routes live in the packaged intro_routes folder. The route file contains
only timed positions and angles; it contains no player identity, vehicle,
lap time, or control input.

PUBLIC API
----------
  CG_IntroCam_ParseConfigstring()    called when CS_INTRO_ROUTE changes
  CG_IntroCam_SetStartTime(t)        called for the synchronized server time
  CG_IntroCam_IsActive()             true while this client's preview runs
  CG_IntroCam_CalcView(o,a,fov)      evaluates the current camera view
  CG_IntroCam_FadeAlpha()            returns the black transition alpha; holds black while a race intro is pending
===========================================================================
*/

#include "cg_local.h"

/* ------------------------------------------------------------------ */
/* Constants                                                           */
/* ------------------------------------------------------------------ */

#define CG_MAX_INTRO_GHOST_ROUTE_FRAMES 256
#define CG_MAX_INTRO_GHOST_ROUTE_FILE_SIZE ( 64 * 1024 )

/* ------------------------------------------------------------------ */
/* Local types                                                         */
/* ------------------------------------------------------------------ */

typedef struct {
	int		timeMs;
	vec3_t	origin;
	vec3_t	angles;
} cgIntroGhostRouteFrame_t;

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static int					s_previewDurationMs = 0;
static int					s_startTime       = 0;
static qboolean				s_hasRoute          = qfalse;
static qboolean				s_waitingForStart    = qfalse;
static qboolean				s_skipped         = qfalse;
static cgIntroGhostRouteFrame_t s_routeFrames[CG_MAX_INTRO_GHOST_ROUTE_FRAMES];
static int					s_routeFrameCount = 0;
static int					s_routeDurationMs = 0;

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

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

	s_routeFrameCount = 0;
	s_routeDurationMs = 0;

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
		cgIntroGhostRouteFrame_t *frame = &s_routeFrames[i];
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

	s_routeFrameCount = frameCount;
	s_routeDurationMs = s_routeFrames[frameCount - 1].timeMs;
	if ( s_routeDurationMs <= 0 ) {
		s_routeFrameCount = 0;
		return qfalse;
	}
	s_previewDurationMs = previewDurationMs;
	s_hasRoute = qtrue;
	if ( cg_developer.integer ) {
		CG_Printf( "CG_IntroCam: loaded %d route points from %s (%d ms preview)\n",
			s_routeFrameCount, path, s_previewDurationMs );
	}
	return qtrue;
}

/* ------------------------------------------------------------------ */
/* Parse configstring                                                  */
/* ------------------------------------------------------------------ */

void CG_IntroCam_ParseConfigstring( void ) {
	const char *p;
	const char *cs;
	char token[64];
	char routePath[MAX_QPATH];
	int previewDurationMs;
	int expectedTrackReversed;
	qboolean waitingForStart;

	s_previewDurationMs = 0;
	s_hasRoute = qfalse;
	s_waitingForStart = qfalse;
	s_routeFrameCount = 0;
	s_routeDurationMs = 0;
	/* A late-joining client may receive the route config after the start command. */

	cs = CG_ConfigString( CS_INTRO_ROUTE );
	if ( !cs || !cs[0] ) {
		return;
	}

	p = cs;
	if ( !CG_IntroCam_NextToken( &p, token, sizeof( token ) ) || Q_stricmp( token, "ghost" )
		|| !CG_IntroCam_NextToken( &p, routePath, sizeof( routePath ) )
		|| !CG_IntroCam_NextToken( &p, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: malformed Ghost route configstring\n" );
		return;
	}
	previewDurationMs = atoi( token );
	if ( !CG_IntroCam_NextToken( &p, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: missing Ghost route direction\n" );
		return;
	}
	expectedTrackReversed = atoi( token ) ? 1 : 0;
	waitingForStart = qfalse;
	if ( CG_IntroCam_NextToken( &p, token, sizeof( token ) ) ) {
		waitingForStart = atoi( token ) ? qtrue : qfalse;
	}
	CG_IntroCam_LoadGhostRoute( routePath, previewDurationMs, expectedTrackReversed );
	s_waitingForStart = waitingForStart;
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
	s_waitingForStart = qfalse;
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
	if ( !s_hasRoute || ( s_routeFrameCount < 2 ) || s_startTime <= 0 ) return qfalse;
	elapsed = cg.time - s_startTime;
	return ( elapsed >= 0 && elapsed < s_previewDurationMs ) ? qtrue : qfalse;
}

qboolean CG_IntroCam_IsActive( void ) {
	return ( !s_skipped && CG_IntroCam_IsRaceIntroPending() ) ? qtrue : qfalse;
}

int CG_IntroCam_RemainingSeconds( void ) {
	int remainingMs;

	if ( !CG_IntroCam_IsActive() ) {
		return 0;
	}

	remainingMs = s_previewDurationMs - ( cg.time - s_startTime );
	return ( remainingMs + 999 ) / 1000;
}

static void CG_IntroCam_SampleGhostRoute( int targetTime, vec3_t origin, vec3_t angles ) {
	int i;
	const cgIntroGhostRouteFrame_t *from, *to;
	float fraction;

	if ( targetTime <= s_routeFrames[0].timeMs ) {
		VectorCopy( s_routeFrames[0].origin, origin );
		VectorCopy( s_routeFrames[0].angles, angles );
		return;
	}
	if ( targetTime >= s_routeFrames[s_routeFrameCount - 1].timeMs ) {
		VectorCopy( s_routeFrames[s_routeFrameCount - 1].origin, origin );
		VectorCopy( s_routeFrames[s_routeFrameCount - 1].angles, angles );
		return;
	}

	for ( i = 0; i < s_routeFrameCount - 1; i++ ) {
		if ( targetTime > s_routeFrames[i + 1].timeMs ) {
			continue;
		}
		from = &s_routeFrames[i];
		to = &s_routeFrames[i + 1];
		fraction = (float)( targetTime - from->timeMs ) / (float)( to->timeMs - from->timeMs );
		origin[0] = from->origin[0] + ( to->origin[0] - from->origin[0] ) * fraction;
		origin[1] = from->origin[1] + ( to->origin[1] - from->origin[1] ) * fraction;
		origin[2] = from->origin[2] + ( to->origin[2] - from->origin[2] ) * fraction;
		angles[PITCH] = LerpAngle( from->angles[PITCH], to->angles[PITCH], fraction );
		angles[YAW] = LerpAngle( from->angles[YAW], to->angles[YAW], fraction );
		angles[ROLL] = LerpAngle( from->angles[ROLL], to->angles[ROLL], fraction );
		return;
	}

	VectorCopy( s_routeFrames[s_routeFrameCount - 1].origin, origin );
	VectorCopy( s_routeFrames[s_routeFrameCount - 1].angles, angles );
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

	if ( s_routeFrameCount < 2 || s_routeDurationMs <= 0 || s_previewDurationMs <= 0 ) {
		return qfalse;
	}

	routeTime = (int)( ( (float)elapsed / (float)s_previewDurationMs ) * (float)s_routeDurationMs );
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
	int elapsed;

	if ( s_skipped || !s_hasRoute || s_startTime <= 0 ) {
		return qfalse;
	}

	elapsed = cg.time - s_startTime;
	if ( elapsed < 0 ) {
		elapsed = 0;
	}
	if ( elapsed >= s_previewDurationMs ) {
		return qfalse;
	}

	return CG_IntroCam_CalcGhostRouteView( elapsed, originOut, anglesOut, fovOut );
}

float CG_IntroCam_FadeAlpha( void ) {
	const int fadeDurationMs = 2500;
	const int revealDurationMs = 2500;
	int elapsed;
	float fadeIn, fadeOut, alpha, transition;

	if ( !s_hasRoute || s_skipped ) {
		return 0.0f;
	}
	if ( s_waitingForStart ) {
		if ( cg.snap && cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
			return 0.0f;
		}
		return 1.0f;
	}
	if ( s_startTime <= 0 ) {
		return 0.0f;
	}

	elapsed = cg.time - s_startTime;
	if ( elapsed < 0 ) {
		return 1.0f;
	}
	if ( elapsed >= s_previewDurationMs ) {
		transition = (float)( elapsed - s_previewDurationMs ) / (float)revealDurationMs;
		if ( transition > 1.0f ) transition = 1.0f;
		return 1.0f - transition * transition * ( 3.0f - 2.0f * transition );
	}

	fadeIn = (float)elapsed / (float)fadeDurationMs;
	if ( fadeIn < 0.0f ) fadeIn = 0.0f;
	if ( fadeIn > 1.0f ) fadeIn = 1.0f;
	fadeIn = 1.0f - fadeIn * fadeIn * ( 3.0f - 2.0f * fadeIn );

	fadeOut = (float)( elapsed - ( s_previewDurationMs - fadeDurationMs ) ) / (float)fadeDurationMs;
	if ( fadeOut < 0.0f ) fadeOut = 0.0f;
	if ( fadeOut > 1.0f ) fadeOut = 1.0f;
	fadeOut = fadeOut * fadeOut * ( 3.0f - 2.0f * fadeOut );

	alpha = ( fadeIn > fadeOut ) ? fadeIn : fadeOut;
	return alpha;
}
