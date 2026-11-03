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

/* Race intro camera generated from the active map's BPD spline. */

#include "cg_local.h"

/* ------------------------------------------------------------------ */
/* Constants                                                           */
/* ------------------------------------------------------------------ */

#define CG_MAX_INTRO_BPD_ROUTE_FRAMES 4096
#define CG_MAX_INTRO_BPD_POINTS 128
#define CG_MAX_INTRO_BPD_FILE_SIZE ( 64 * 1024 )

/* ------------------------------------------------------------------ */
/* Local types                                                         */
/* ------------------------------------------------------------------ */

typedef struct {
	int		timeMs;
	float		distance;
	vec3_t	origin;
	vec3_t	angles;
} cgIntroBpdFrame_t;

typedef struct {
	int		id;
	int		order;
	int		entityNum;
	vec3_t	origin;
	vec3_t	handle;
} cgIntroBpdPoint_t;

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static int					s_previewDurationMs = 0;
static int					s_startTime       = 0;
static qboolean				s_hasRoute          = qfalse;
static qboolean				s_waitingForStart    = qfalse;
static qboolean				s_skipped         = qfalse;
static qboolean				s_routeBuildPending = qfalse;
static qboolean				s_effectiveReversed = qfalse;
static qboolean				s_bpdLoaded = qfalse;
static cgIntroBpdFrame_t s_routeFrames[CG_MAX_INTRO_BPD_ROUTE_FRAMES];
static int					s_routeFrameCount = 0;
static int					s_routeDurationMs = 0;
static cgIntroBpdPoint_t s_bpdPoints[CG_MAX_INTRO_BPD_POINTS];
static int					s_bpdPointCount = 0;
static int					s_routeBuildAttemptTime = -1000;

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

static qboolean CG_IntroCam_ReadBpd( const char *path ) {
	fileHandle_t file;
	int fileLength, pointCount = 0;
	static char buffer[CG_MAX_INTRO_BPD_FILE_SIZE + 1];
	const char *cursor;
	char token[64];
	char expectedPath[MAX_QPATH], mapName[MAX_QPATH];

	s_routeFrameCount = 0;
	s_routeDurationMs = 0;
	s_bpdPointCount = 0;
	s_bpdLoaded = qfalse;
	if ( !path || Q_stricmpn( path, "maps/", 5 ) ) return qfalse;
	COM_StripExtension( COM_SkipPath( cgs.mapname ), mapName, sizeof( mapName ) );
	if ( !mapName[0] || Com_sprintf( expectedPath, sizeof( expectedPath ), "maps/%s.bpd", mapName ) >= sizeof( expectedPath )
		|| Q_stricmp( path, expectedPath ) ) return qfalse;

	fileLength = trap_FS_FOpenFile( path, &file, FS_READ );
	if ( fileLength <= 0 ) {
		if ( fileLength == 0 ) trap_FS_FCloseFile( file );
		CG_Printf( "^3CG_IntroCam: could not read BPD %s\n", path );
		return qfalse;
	}
	if ( fileLength > CG_MAX_INTRO_BPD_FILE_SIZE ) {
		trap_FS_FCloseFile( file );
		CG_Printf( "^3CG_IntroCam: BPD %s exceeds the %d byte limit\n", path, CG_MAX_INTRO_BPD_FILE_SIZE );
		return qfalse;
	}
	trap_FS_Read( buffer, fileLength, file );
	buffer[fileLength] = '\0';
	trap_FS_FCloseFile( file );
	cursor = buffer;
	while ( CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) {
		cgIntroBpdPoint_t *point;
		int axis;
		if ( pointCount >= CG_MAX_INTRO_BPD_POINTS ) return qfalse;
		point = &s_bpdPoints[pointCount];
		point->id = atoi( token );
		if ( point->id != pointCount + 1
			|| !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, ":" ) ) return qfalse;
		for ( axis = 0; axis < 3; axis++ ) {
			if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) return qfalse;
			point->origin[axis] = (float)atof( token );
			if ( IS_NAN( point->origin[axis] ) || point->origin[axis] < -131072.0f || point->origin[axis] > 131072.0f ) return qfalse;
		}
		if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) || Q_stricmp( token, ":" ) ) return qfalse;
		for ( axis = 0; axis < 3; axis++ ) {
			if ( !CG_IntroCam_NextToken( &cursor, token, sizeof( token ) ) ) return qfalse;
			point->handle[axis] = (float)atof( token );
			if ( IS_NAN( point->handle[axis] ) || point->handle[axis] < -16384.0f || point->handle[axis] > 16384.0f ) return qfalse;
		}
		pointCount++;
	}
	if ( pointCount < ( cgs.sprintTrack ? 1 : 2 ) ) return qfalse;
	s_bpdPointCount = pointCount;
	s_bpdLoaded = qtrue;
	return qtrue;
}

static void CG_IntroCam_EvaluateBezier( const vec3_t p0, const vec3_t h0,
	const vec3_t p3, const vec3_t h3, qboolean reversed, float t,
	vec3_t point, vec3_t derivative ) {
	vec3_t c1, c2;
	float u = 1.0f - t;
	if ( reversed ) {
		VectorMA( p0, -1.0f, h0, c1 );
		VectorAdd( p3, h3, c2 );
	} else {
		VectorAdd( p0, h0, c1 );
		VectorMA( p3, -1.0f, h3, c2 );
	}
	VectorScale( p0, u * u * u, point );
	VectorMA( point, 3.0f * t * u * u, c1, point );
	VectorMA( point, 3.0f * t * t * u, c2, point );
	VectorMA( point, t * t * t, p3, point );
	VectorScale( p0, -3.0f * u * u, derivative );
	VectorMA( derivative, 3.0f * u * u - 6.0f * t * u, c1, derivative );
	VectorMA( derivative, 6.0f * t * u - 3.0f * t * t, c2, derivative );
	VectorMA( derivative, 3.0f * t * t, p3, derivative );
}

static qboolean CG_IntroCam_AppendRoutePoint( const vec3_t point, const vec3_t tangent ) {
	cgIntroBpdFrame_t *frame;
	vec3_t direction;
	if ( s_routeFrameCount >= CG_MAX_INTRO_BPD_ROUTE_FRAMES ) return qfalse;
	frame = &s_routeFrames[s_routeFrameCount];
	VectorCopy( point, frame->origin );
	if ( s_routeFrameCount > 0 ) {
		vec3_t delta;
		VectorSubtract( point, s_routeFrames[s_routeFrameCount - 1].origin, delta );
		frame->distance = s_routeFrames[s_routeFrameCount - 1].distance + VectorLength( delta );
	} else {
		frame->distance = 0.0f;
	}
	VectorCopy( tangent, direction );
	if ( VectorNormalize( direction ) < 0.001f && s_routeFrameCount > 0 ) {
		VectorSubtract( point, s_routeFrames[s_routeFrameCount - 1].origin, direction );
	}
	vectoangles( direction, frame->angles );
	frame->angles[ROLL] = 0.0f;
	s_routeFrameCount++;
	return qtrue;
}

static qboolean CG_IntroCam_AppendBezierSegment( const vec3_t p0, const vec3_t h0,
	const vec3_t p3, const vec3_t h3, qboolean reversed ) {
	vec3_t previous, point, tangent, delta;
	float length = 0.0f;
	int i, steps;
	CG_IntroCam_EvaluateBezier( p0, h0, p3, h3, reversed, 0.0f, previous, tangent );
	for ( i = 1; i <= 16; i++ ) {
		CG_IntroCam_EvaluateBezier( p0, h0, p3, h3, reversed, i / 16.0f, point, tangent );
		VectorSubtract( point, previous, delta );
		length += VectorLength( delta );
		VectorCopy( point, previous );
	}
	steps = (int)( length / 72.0f ) + 1;
	if ( steps < 8 ) steps = 8;
	if ( steps > 128 ) steps = 128;
	for ( i = 0; i < steps; i++ ) {
		CG_IntroCam_EvaluateBezier( p0, h0, p3, h3, reversed, (float)i / steps, point, tangent );
		if ( !CG_IntroCam_AppendRoutePoint( point, tangent ) ) return qfalse;
	}
	return qtrue;
}

static qboolean CG_IntroCam_BuildRoute( void ) {
	cgIntroBpdPoint_t ordered[CG_MAX_INTRO_BPD_POINTS];
	qboolean matched[MAX_GENTITIES];
	vec3_t startPoint, finishPoint, lastPoint, tangent;
	int i, j, count = s_bpdPointCount;
	int startEntity = -1, finishEntity = -1;
	float totalLength;
	if ( !s_bpdLoaded || count < ( cgs.sprintTrack ? 1 : 2 ) ) return qfalse;
	Com_Memset( matched, 0, sizeof( matched ) );
	for ( i = 0; i < count; i++ ) {
		int bestEntity = -1;
		float bestDistance = 16.0f;
		for ( j = 0; j < MAX_GENTITIES; j++ ) {
			centity_t *cent = &cg_entities[j];
			vec3_t delta;
			float distance;
			if ( matched[j] || cent->currentState.eType != ET_CHECKPOINT ) continue;
			VectorSubtract( cent->currentState.origin2, s_bpdPoints[i].origin, delta );
			distance = VectorLengthSquared( delta );
			if ( distance < bestDistance ) {
				bestDistance = distance;
				bestEntity = j;
			}
		}
		if ( bestEntity < 0 ) return qfalse;
		matched[bestEntity] = qtrue;
		ordered[i] = s_bpdPoints[i];
		ordered[i].order = cg_entities[bestEntity].currentState.weapon;
		ordered[i].entityNum = bestEntity;
	}
	for ( i = 0; i < count - 1; i++ ) {
		for ( j = i + 1; j < count; j++ ) {
			if ( ordered[j].order < ordered[i].order ) {
				cgIntroBpdPoint_t swap = ordered[i];
				ordered[i] = ordered[j];
				ordered[j] = swap;
			}
		}
	}
	if ( cgs.sprintTrack ) {
		for ( j = 0; j < MAX_GENTITIES; j++ ) {
			centity_t *cent = &cg_entities[j];
			if ( matched[j] || cent->currentState.eType != ET_CHECKPOINT ) continue;
			if ( cent->currentState.weapon == count ) startEntity = j;
			if ( cent->currentState.weapon == count + 1 ) finishEntity = j;
		}
		if ( startEntity < 0 || finishEntity < 0 ) return qfalse;
		if ( s_effectiveReversed ) {
			VectorCopy( cg_entities[finishEntity].currentState.origin2, startPoint );
			VectorCopy( cg_entities[startEntity].currentState.origin2, finishPoint );
		} else {
			VectorCopy( cg_entities[startEntity].currentState.origin2, startPoint );
			VectorCopy( cg_entities[finishEntity].currentState.origin2, finishPoint );
		}
	}

	s_routeFrameCount = 0;
	if ( cgs.sprintTrack ) {
		vec3_t startHandle;
		VectorSubtract( ordered[0].origin, startPoint, startHandle );
		VectorScale( startHandle, ( s_effectiveReversed ? -1.0f : 1.0f ) / 3.0f, startHandle );
		if ( !CG_IntroCam_AppendBezierSegment( startPoint, startHandle,
			ordered[0].origin, ordered[0].handle, s_effectiveReversed ) ) return qfalse;
	}
	for ( i = 0; i < count - ( cgs.sprintTrack ? 1 : 0 ); i++ ) {
		int next = ( i + 1 ) % count;
		if ( !CG_IntroCam_AppendBezierSegment( ordered[i].origin, ordered[i].handle,
			ordered[next].origin, ordered[next].handle, s_effectiveReversed ) ) return qfalse;
	}
	if ( cgs.sprintTrack ) {
		vec3_t finishHandle;
		VectorSubtract( finishPoint, ordered[count - 1].origin, finishHandle );
		VectorScale( finishHandle, ( s_effectiveReversed ? -1.0f : 1.0f ) / 3.0f, finishHandle );
		if ( !CG_IntroCam_AppendBezierSegment( ordered[count - 1].origin,
			ordered[count - 1].handle, finishPoint, finishHandle, s_effectiveReversed ) ) return qfalse;
		CG_IntroCam_EvaluateBezier( ordered[count - 1].origin, ordered[count - 1].handle,
			finishPoint, finishHandle, s_effectiveReversed, 1.0f, lastPoint, tangent );
	} else {
		CG_IntroCam_EvaluateBezier( ordered[count - 1].origin, ordered[count - 1].handle,
			ordered[0].origin, ordered[0].handle, s_effectiveReversed, 1.0f, lastPoint, tangent );
	}
	if ( !CG_IntroCam_AppendRoutePoint( lastPoint, tangent ) || s_routeFrameCount < 2 ) return qfalse;
	totalLength = s_routeFrames[s_routeFrameCount - 1].distance;
	if ( totalLength < 1.0f ) return qfalse;
	for ( i = 0; i < s_routeFrameCount; i++ ) {
		s_routeFrames[i].timeMs = (int)( s_routeFrames[i].distance / totalLength * s_previewDurationMs );
		if ( i > 0 && s_routeFrames[i].timeMs <= s_routeFrames[i - 1].timeMs ) {
			s_routeFrames[i].timeMs = s_routeFrames[i - 1].timeMs + 1;
		}
	}
	s_routeFrames[s_routeFrameCount - 1].timeMs = s_previewDurationMs;
	s_routeDurationMs = s_previewDurationMs;
	s_routeBuildPending = qfalse;
	s_hasRoute = qtrue;
	if ( cg_developer.integer ) {
		CG_Printf( "CG_IntroCam: built %d intro samples from %d BPD points\n", s_routeFrameCount, count );
	}
	return qtrue;
}

static void CG_IntroCam_EnsureRoute( void ) {
	if ( !s_hasRoute && s_routeBuildPending && s_bpdLoaded
		&& cg.time >= s_routeBuildAttemptTime + 100 ) {
		s_routeBuildAttemptTime = cg.time;
		CG_IntroCam_BuildRoute();
	}
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
	s_routeBuildPending = qfalse;
	s_routeBuildAttemptTime = -1000;
	s_bpdLoaded = qfalse;
	s_bpdPointCount = 0;
	/* A late-joining client may receive the route config after the start command. */

	cs = CG_ConfigString( CS_INTRO_ROUTE );
	if ( !cs || !cs[0] ) {
		return;
	}

	p = cs;
	if ( !CG_IntroCam_NextToken( &p, token, sizeof( token ) ) || Q_stricmp( token, "bpd" )
		|| !CG_IntroCam_NextToken( &p, routePath, sizeof( routePath ) )
		|| !CG_IntroCam_NextToken( &p, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: malformed BPD configstring\n" );
		return;
	}
	previewDurationMs = atoi( token );
	if ( !CG_IntroCam_NextToken( &p, token, sizeof( token ) ) ) {
		CG_Printf( "^3CG_IntroCam: missing BPD route direction\n" );
		return;
	}
	expectedTrackReversed = atoi( token ) ? 1 : 0;
	waitingForStart = qfalse;
	if ( CG_IntroCam_NextToken( &p, token, sizeof( token ) ) ) {
		waitingForStart = atoi( token ) ? qtrue : qfalse;
	}
	if ( previewDurationMs < 1000 || previewDurationMs > 60000 ) return;
	s_previewDurationMs = previewDurationMs;
	s_effectiveReversed = expectedTrackReversed ? qtrue : qfalse;
	if ( !CG_IntroCam_ReadBpd( routePath ) ) {
		CG_Printf( "^3CG_IntroCam: no usable BPD route in %s\n", routePath );
		return;
	}
	s_routeBuildPending = qtrue;
	CG_IntroCam_EnsureRoute();
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
	CG_IntroCam_EnsureRoute();
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

static void CG_IntroCam_SampleBpdRoute( int targetTime, vec3_t origin, vec3_t angles ) {
	int i;
	const cgIntroBpdFrame_t *from, *to;
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

static qboolean CG_IntroCam_CalcBpdRouteView( int elapsed, vec3_t originOut, vec3_t anglesOut, float *fovOut ) {
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
	CG_IntroCam_SampleBpdRoute( routeTime, routeOrigin, routeAngles );

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

	CG_IntroCam_EnsureRoute();
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

	return CG_IntroCam_CalcBpdRouteView( elapsed, originOut, anglesOut, fovOut );
}

float CG_IntroCam_FadeAlpha( void ) {
	const int fadeDurationMs = 2500;
	const int revealDurationMs = 2500;
	int elapsed;
	float fadeIn, fadeOut, alpha, transition;

	CG_IntroCam_EnsureRoute();
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
