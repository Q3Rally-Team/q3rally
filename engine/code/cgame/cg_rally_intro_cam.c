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
"<N> px py pz ax ay az durMs blend fov hasLookAt [lx ly lz]  ..."
All values are space-separated ASCII on a single line, repeated N times.
hasLookAt is 0 or 1; if 1, three lookat floats follow immediately.

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

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static cgIntroCamNode_t		s_nodes[CG_MAX_INTRO_CAM_NODES];
static int					s_nodeCount       = 0;
static int					s_totalDurationMs = 0;
static int					s_startTime       = 0;
static qboolean				s_hasSequence     = qfalse;

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
	if (cg_developer.integer) CG_Printf( "CG_IntroCam: startTime=%d cg.time=%d\n", s_startTime, cg.time );
}

/* ------------------------------------------------------------------ */
/* Active query                                                        */
/* ------------------------------------------------------------------ */

qboolean CG_IntroCam_IsActive( void ) {
	int elapsed;
	if ( !s_hasSequence || s_nodeCount <= 0 || s_startTime <= 0 ) return qfalse;
	elapsed = cg.time - s_startTime;
	return ( elapsed >= 0 && elapsed < s_totalDurationMs ) ? qtrue : qfalse;
}

/* ------------------------------------------------------------------ */
/* Main evaluator                                                      */
/* ------------------------------------------------------------------ */

qboolean CG_IntroCam_CalcView( vec3_t originOut, vec3_t anglesOut, float *fovOut ) {
	int		elapsed, segStart, ni, nNext;
	float	t, blend;
	const cgIntroCamNode_t *node, *next;
	vec3_t	origin, angles;

	if ( !s_hasSequence || s_nodeCount <= 0 || s_startTime <= 0 ) return qfalse;

	elapsed = cg.time - s_startTime;
	if ( elapsed < 0 )                  elapsed = 0;
	if ( elapsed >= s_totalDurationMs ) return qfalse;

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
