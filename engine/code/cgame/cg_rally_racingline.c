/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
===========================================================================
*/
// cg_rally_racingline.c -- racing line drawn on the road (cg_racingLine)
//
// The line follows the best known ghost lap of this track variant: the
// personal ghost (ghosts/<map>_tl<n>_rev<n>_*.ghost, own car) or the server
// base route, whichever lap is faster. It is resampled to evenly spaced
// points, dropped onto the road with a world trace and coloured:
//   green  = on the throttle, yellow = off the throttle, red = braking.
// Personal ghosts carry the driver's inputs (forwardmove, handbrake); the
// server route only has positions and times, so there the colour comes from
// the speed change along the line.
//
// Only the stretch ahead of the car is drawn (cg_racingLineDistance metres),
// faded in under the car and out towards the far end.
//
// Outside Ghost Race the server does not send its route on its own; the
// client asks for it with "ghostroutereq" (CG_GhostRoute_Request).

#include "cg_local.h"

#define RL_UNITS_PER_METER      35.66f
#define RL_SPACING              54.0f   // ~1.5 m between line points
#define RL_MAX_POINTS           8192
#define RL_TELEPORT_GAP         1200.0f // ghost samples further apart: a reset
#define RL_HALF_WIDTH           17.0f   // ~1 m wide band
#define RL_LIFT                 1.5f    // above the road (plus polygonOffset)
#define RL_TRACE_UP             24.0f
#define RL_TRACE_DOWN           160.0f
#define RL_LOOP_GAP             1200.0f // start and end this close: a circuit
#define RL_BEHIND_METERS        4.0f
#define RL_FADE_IN_METERS       6.0f
#define RL_ALPHA                0.62f
#define RL_SEARCH_BACK          30
#define RL_SEARCH_AHEAD         120
#define RL_SEARCH_RESET         700.0f  // nearest point further away: full search
#define RL_INPUT_WINDOW_MS      100
#define RL_ACCEL_SPAN           3       // points on each side for the speed change
#define RL_LIFT_ACCEL           -100.0f // units/s^2, measured on recorded laps
#define RL_BRAKE_ACCEL          -400.0f
#define RL_MIN_RUN              3       // shorter colour runs are smoothed away
#define RL_MAX_DRAW_POLYS       512

enum {
	RL_THROTTLE,
	RL_LIFT_OFF,
	RL_BRAKE
};

enum {
	RL_SOURCE_NONE,
	RL_SOURCE_PERSONAL,
	RL_SOURCE_BASE
};

typedef struct {
	vec3_t	origin;		// on the road
	vec3_t	right;		// half width, across the line
	float	dist;		// along the line from the first point
	float	time;		// ghost time in ms
	byte	kind;		// RL_THROTTLE / RL_LIFT_OFF / RL_BRAKE
	byte	breakBefore;	// a reset in the ghost: no segment from the previous point
} racingLinePoint_t;

typedef struct {
	racingLinePoint_t	points[RL_MAX_POINTS];
	int		count;
	float	length;
	qboolean	loop;
	qboolean	valid;
	// what the line was built from
	int		source;
	int		sourceTime;
	int		sourceFrames;
	int		sourceDuration;
	vec3_t	sourceStart;
	// nearest point to the car
	int		nearest;
	qboolean	nearestValid;
} racingLine_t;

static racingLine_t rl;
static qboolean rl_routeRequested;
static qhandle_t rl_shader;

static const float rl_colors[3][3] = {
	{ 0.15f, 0.90f, 0.25f },	// throttle
	{ 1.00f, 0.80f, 0.10f },	// off the throttle
	{ 1.00f, 0.15f, 0.10f }		// braking
};

void CG_RacingLine_Register( void ) {
	rl_shader = trap_R_RegisterShader( "gfx/misc/racingline" );
}

void CG_RacingLine_Reset( void ) {
	rl.valid = qfalse;
	rl.count = 0;
	rl.source = RL_SOURCE_NONE;
	rl.nearestValid = qfalse;
	rl_routeRequested = qfalse;
}

static const ghostFrame_t *RL_Frame( const ghostRecording_t *rec, int i ) {
	return &rec->frames[( rec->startIndex + i ) % MAX_GHOST_FRAMES];
}

/* Recording has driver inputs (personal ghosts); the server route has none. */
static qboolean RL_HasInputs( const ghostRecording_t *rec ) {
	int i;

	for ( i = 0; i < rec->frameCount; i++ ) {
		if ( RL_Frame( rec, i )->forwardmove != 0 ) {
			return qtrue;
		}
	}
	return qfalse;
}

static int RL_AddPoint( const vec3_t origin, float time, float dist, qboolean breakBefore ) {
	racingLinePoint_t *p;

	if ( rl.count >= RL_MAX_POINTS ) {
		return qfalse;
	}
	p = &rl.points[rl.count++];
	VectorCopy( origin, p->origin );
	VectorClear( p->right );
	p->time = time;
	p->dist = dist;
	p->kind = RL_THROTTLE;
	p->breakBefore = breakBefore ? 1 : 0;
	return qtrue;
}

/* Evenly spaced points along the ghost path; a reset starts a new piece. */
static void RL_Resample( const ghostRecording_t *rec, float spacing ) {
	int i;
	float dist = 0.0f;
	float sinceLast = 0.0f;

	rl.count = 0;
	RL_AddPoint( RL_Frame( rec, 0 )->origin, (float)RL_Frame( rec, 0 )->timeOffset, 0.0f, qfalse );

	for ( i = 1; i < rec->frameCount; i++ ) {
		const ghostFrame_t *a = RL_Frame( rec, i - 1 );
		const ghostFrame_t *b = RL_Frame( rec, i );
		vec3_t delta;
		float segment;
		float pos;

		VectorSubtract( b->origin, a->origin, delta );
		segment = VectorLength( delta );
		if ( segment < 0.001f ) {
			continue;
		}
		if ( segment > RL_TELEPORT_GAP ) {
			if ( !RL_AddPoint( b->origin, (float)b->timeOffset, dist, qtrue ) ) {
				return;
			}
			sinceLast = 0.0f;
			continue;
		}

		pos = spacing - sinceLast;
		while ( pos <= segment ) {
			vec3_t origin;
			float f = pos / segment;

			VectorMA( a->origin, f, delta, origin );
			if ( !RL_AddPoint( origin, a->timeOffset + f * ( b->timeOffset - a->timeOffset ),
					dist + pos, qfalse ) ) {
				return;
			}
			pos += spacing;
		}
		sinceLast = segment - ( pos - spacing );
		dist += segment;
	}

	/* keep the very end of the lap */
	if ( sinceLast > spacing * 0.25f && rl.count < RL_MAX_POINTS ) {
		const ghostFrame_t *last = RL_Frame( rec, rec->frameCount - 1 );
		RL_AddPoint( last->origin, (float)last->timeOffset, dist, qfalse );
	}
	rl.length = dist;
}

/* Drops the points onto the road and sets up the band across the line. */
static void RL_Ground( void ) {
	int i;

	for ( i = 0; i < rl.count; i++ ) {
		racingLinePoint_t *p = &rl.points[i];
		vec3_t start, end, normal, dir, right;
		trace_t tr;
		int prev = i > 0 && !p->breakBefore ? i - 1 : i;
		int next = i + 1 < rl.count && !rl.points[i + 1].breakBefore ? i + 1 : i;

		VectorSubtract( rl.points[next].origin, rl.points[prev].origin, dir );
		dir[2] = 0.0f;
		if ( VectorNormalize( dir ) < 0.001f ) {
			dir[0] = 1.0f;
		}

		VectorCopy( p->origin, start );
		start[2] += RL_TRACE_UP;
		VectorCopy( p->origin, end );
		end[2] -= RL_TRACE_DOWN;
		trap_CM_BoxTrace( &tr, start, end, NULL, NULL, 0, MASK_SOLID );
		if ( tr.fraction < 1.0f && !tr.startsolid && tr.plane.normal[2] > 0.3f ) {
			VectorCopy( tr.plane.normal, normal );
			VectorMA( tr.endpos, RL_LIFT, normal, p->origin );
		} else {
			VectorSet( normal, 0.0f, 0.0f, 1.0f );
		}

		CrossProduct( dir, normal, right );
		if ( VectorNormalize( right ) < 0.001f ) {
			VectorSet( right, -dir[1], dir[0], 0.0f );
		}
		VectorScale( right, RL_HALF_WIDTH, p->right );
	}
}

/* Colours from the driver's inputs around each point. */
static void RL_ClassifyInputs( const ghostRecording_t *rec ) {
	int i;
	int frame = 0;

	for ( i = 0; i < rl.count; i++ ) {
		racingLinePoint_t *p = &rl.points[i];
		int throttle = 0, coast = 0, brake = 0;
		int j;

		while ( frame + 1 < rec->frameCount && RL_Frame( rec, frame )->timeOffset < p->time - RL_INPUT_WINDOW_MS ) {
			frame++;
		}
		for ( j = frame; j < rec->frameCount; j++ ) {
			const ghostFrame_t *f = RL_Frame( rec, j );

			if ( f->timeOffset > p->time + RL_INPUT_WINDOW_MS && j > frame ) {
				break;
			}
			if ( f->forwardmove < 0 || ( f->buttons & BUTTON_HANDBRAKE ) ) {
				brake++;
			} else if ( f->forwardmove > 0 ) {
				throttle++;
			} else {
				coast++;
			}
		}
		if ( brake ) {
			p->kind = RL_BRAKE;
		} else if ( coast > throttle ) {
			p->kind = RL_LIFT_OFF;
		} else {
			p->kind = RL_THROTTLE;
		}
	}
}

/* Point index span [first,last] of the piece (no reset in between) holding i. */
static void RL_PieceBounds( int i, int *first, int *last ) {
	int a = i, b = i;

	while ( a > 0 && !rl.points[a].breakBefore ) {
		a--;
	}
	while ( b + 1 < rl.count && !rl.points[b + 1].breakBefore ) {
		b++;
	}
	*first = a;
	*last = b;
}

static float RL_SpeedAt( int i, int first, int last ) {
	int a = i > first ? i - 1 : i;
	int b = i < last ? i + 1 : i;
	float dt = ( rl.points[b].time - rl.points[a].time ) * 0.001f;

	if ( b == a || dt <= 0.0f ) {
		return 0.0f;
	}
	return ( rl.points[b].dist - rl.points[a].dist ) / dt;
}

/* Colours from the speed change (server route: positions and times only). */
static void RL_ClassifySpeed( void ) {
	static float speed[RL_MAX_POINTS];
	int i;
	int first = 0, last = -1;

	for ( i = 0; i < rl.count; i++ ) {
		if ( i > last ) {
			RL_PieceBounds( i, &first, &last );
		}
		speed[i] = RL_SpeedAt( i, first, last );
	}

	last = -1;
	for ( i = 0; i < rl.count; i++ ) {
		int a, b;
		float dt, accel;

		if ( i > last ) {
			RL_PieceBounds( i, &first, &last );
		}
		a = i - RL_ACCEL_SPAN < first ? first : i - RL_ACCEL_SPAN;
		b = i + RL_ACCEL_SPAN > last ? last : i + RL_ACCEL_SPAN;
		dt = ( rl.points[b].time - rl.points[a].time ) * 0.001f;
		if ( b == a || dt <= 0.0f ) {
			rl.points[i].kind = RL_THROTTLE;
			continue;
		}
		accel = ( speed[b] - speed[a] ) / dt;
		if ( accel < RL_BRAKE_ACCEL ) {
			rl.points[i].kind = RL_BRAKE;
		} else if ( accel < RL_LIFT_ACCEL ) {
			rl.points[i].kind = RL_LIFT_OFF;
		} else {
			rl.points[i].kind = RL_THROTTLE;
		}
	}
}

/* Removes flicker: runs shorter than RL_MIN_RUN points take the colour
 * before them. Braking runs of two points stay. */
static void RL_SmoothKinds( void ) {
	int i = 0;

	while ( i < rl.count ) {
		int j = i;
		int run;

		while ( j + 1 < rl.count && rl.points[j + 1].kind == rl.points[i].kind && !rl.points[j + 1].breakBefore ) {
			j++;
		}
		run = j - i + 1;
		if ( i > 0 && !rl.points[i].breakBefore && run < RL_MIN_RUN &&
		     !( rl.points[i].kind == RL_BRAKE && run >= 2 ) ) {
			int k;
			for ( k = i; k <= j; k++ ) {
				rl.points[k].kind = rl.points[i - 1].kind;
			}
		}
		i = j + 1;
	}
}

static qboolean RL_Build( const ghostRecording_t *rec ) {
	float spacing = RL_SPACING;
	float pathLength = 0.0f;
	int i;
	vec3_t gap;

	rl.valid = qfalse;
	rl.nearestValid = qfalse;
	rl.count = 0;
	if ( !rec || rec->frameCount < 2 ) {
		return qfalse;
	}

	for ( i = 1; i < rec->frameCount; i++ ) {
		vec3_t delta;
		VectorSubtract( RL_Frame( rec, i )->origin, RL_Frame( rec, i - 1 )->origin, delta );
		pathLength += VectorLength( delta );
	}
	if ( pathLength / spacing > RL_MAX_POINTS - 16 ) {
		spacing = pathLength / ( RL_MAX_POINTS - 16 );
	}

	RL_Resample( rec, spacing );
	if ( rl.count < 2 ) {
		rl.count = 0;
		return qfalse;
	}
	RL_Ground();
	if ( RL_HasInputs( rec ) ) {
		RL_ClassifyInputs( rec );
	} else {
		RL_ClassifySpeed();
	}
	RL_SmoothKinds();

	VectorSubtract( rl.points[rl.count - 1].origin, rl.points[0].origin, gap );
	rl.loop = !CG_IsSprintTrack() && VectorLength( gap ) < RL_LOOP_GAP;
	rl.valid = qtrue;
	return qtrue;
}

/* The faster of the personal ghost and the server route. */
static const ghostRecording_t *RL_PickSource( int *sourceOut, int *timeOut ) {
	qboolean personal = cg.personalGhostAvailable && cg.ghostPlayback.valid && cg.ghostPlayback.frameCount > 1;
	qboolean base = cg.baseGhostAvailable && cg.baseGhost.valid && cg.baseGhost.frameCount > 1;
	int personalTime = cg.personalGhostBestTime > 0 ? cg.personalGhostBestTime : 0x7fffffff;
	int baseTime = cg.baseGhostBestTime > 0 ? cg.baseGhostBestTime : 0x7fffffff;

	if ( personal && ( !base || personalTime <= baseTime ) ) {
		*sourceOut = RL_SOURCE_PERSONAL;
		*timeOut = cg.personalGhostBestTime;
		return &cg.ghostPlayback;
	}
	if ( base ) {
		*sourceOut = RL_SOURCE_BASE;
		*timeOut = cg.baseGhostBestTime;
		return &cg.baseGhost;
	}
	*sourceOut = RL_SOURCE_NONE;
	*timeOut = 0;
	return NULL;
}

static void RL_Update( void ) {
	const ghostRecording_t *rec;
	int source, time;

	rec = RL_PickSource( &source, &time );
	if ( !rec ) {
		rl.valid = qfalse;
		rl.source = RL_SOURCE_NONE;
		return;
	}
	if ( rl.source == source && rl.sourceTime == time && rl.sourceFrames == rec->frameCount &&
	     rl.sourceDuration == rec->duration && VectorCompare( rl.sourceStart, RL_Frame( rec, 0 )->origin ) ) {
		return;
	}
	rl.source = source;
	rl.sourceTime = time;
	rl.sourceFrames = rec->frameCount;
	rl.sourceDuration = rec->duration;
	VectorCopy( RL_Frame( rec, 0 )->origin, rl.sourceStart );
	RL_Build( rec );
	if ( cg_developer.integer ) {
		CG_Printf( "Racing line: %d points, %.0f m, %s ghost (%d ms)%s\n", rl.count,
			rl.length / RL_UNITS_PER_METER, source == RL_SOURCE_PERSONAL ? "personal" : "server",
			time, rl.loop ? ", circuit" : "" );
	}
}

static float RL_DistSq( int i, const vec3_t origin ) {
	vec3_t d;
	VectorSubtract( rl.points[i].origin, origin, d );
	return DotProduct( d, d );
}

static int RL_Wrap( int i ) {
	if ( rl.loop ) {
		i %= rl.count;
		if ( i < 0 ) {
			i += rl.count;
		}
	}
	return i;
}

/* Nearest line point to the car; searched around the last one so crossings
 * and bridges do not make it jump. */
static int RL_FindNearest( const vec3_t origin ) {
	int best = -1;
	float bestDist = 0.0f;
	int i;

	if ( rl.nearestValid ) {
		for ( i = rl.nearest - RL_SEARCH_BACK; i <= rl.nearest + RL_SEARCH_AHEAD; i++ ) {
			int k = RL_Wrap( i );
			float d;

			if ( k < 0 || k >= rl.count ) {
				continue;
			}
			d = RL_DistSq( k, origin );
			if ( best < 0 || d < bestDist ) {
				best = k;
				bestDist = d;
			}
		}
	}
	if ( best < 0 || bestDist > RL_SEARCH_RESET * RL_SEARCH_RESET ) {
		best = -1;
		for ( i = 0; i < rl.count; i++ ) {
			float d = RL_DistSq( i, origin );
			if ( best < 0 || d < bestDist ) {
				best = i;
				bestDist = d;
			}
		}
	}
	rl.nearest = best;
	rl.nearestValid = best >= 0;
	return best;
}

/* Distance along the line from point "from" forward to point k. */
static float RL_Ahead( int from, int k, int steps ) {
	float d = rl.points[k].dist - rl.points[from].dist;

	if ( rl.loop && steps > 0 && d < 0.0f ) {
		d += rl.length;
	} else if ( rl.loop && steps < 0 && d > 0.0f ) {
		d -= rl.length;
	}
	return d;
}

static float RL_Alpha( float ahead, float range ) {
	float fadeIn = ( ahead + RL_BEHIND_METERS * RL_UNITS_PER_METER ) / ( RL_FADE_IN_METERS * RL_UNITS_PER_METER );
	float fadeOut = ( range - ahead ) / ( range * 0.35f );
	float a = RL_ALPHA;

	if ( fadeIn < 1.0f ) {
		a *= fadeIn < 0.0f ? 0.0f : fadeIn;
	}
	if ( fadeOut < 1.0f ) {
		a *= fadeOut < 0.0f ? 0.0f : fadeOut;
	}
	return a;
}

static void RL_SetVert( polyVert_t *v, const racingLinePoint_t *p, float side, float alpha ) {
	const float *c = rl_colors[p->kind];

	VectorMA( p->origin, side, p->right, v->xyz );
	v->st[0] = side < 0.0f ? 0.0f : 1.0f;
	v->st[1] = 0.5f;
	v->modulate[0] = (byte)( c[0] * 255.0f );
	v->modulate[1] = (byte)( c[1] * 255.0f );
	v->modulate[2] = (byte)( c[2] * 255.0f );
	v->modulate[3] = (byte)( alpha * 255.0f );
}

static qboolean RL_ShouldDraw( void ) {
	const playerState_t *ps;

	if ( !cg_racingLine.integer || !isRallyRace() || !cg.snap ) {
		return qfalse;
	}
	ps = &cg.predictedPlayerState;
	if ( ps->pm_type == PM_INTERMISSION || cg.intermissionStarted ) {
		return qfalse;
	}
	if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR && !( ps->pm_flags & PMF_FOLLOW ) ) {
		return qfalse;
	}
	return qtrue;
}

/*
=================
CG_AddRacingLine

Adds the racing line ahead of the car to the scene.
=================
*/
void CG_AddRacingLine( void ) {
	static polyVert_t verts[RL_MAX_DRAW_POLYS * 4];
	float range;
	int steps, maxSteps;
	int center;
	int polys = 0;
	int behind;
	int s;

	if ( !RL_ShouldDraw() ) {
		return;
	}

	/* sources: personal ghost (looked up once per map/variant/car) and the
	 * server route (asked for outside Ghost Race) */
	if ( !cg.personalGhostSearchValid ) {
		CG_LoadPersonalGhost();
	}
	if ( cgs.gametype != GT_GHOST && !rl_routeRequested && !cg.baseGhostStatusKnown && !cg.baseGhostTransferPending ) {
		rl_routeRequested = qtrue;
		CG_GhostRoute_Request();
	}

	RL_Update();
	if ( !rl.valid || !rl_shader ) {
		return;
	}

	center = RL_FindNearest( cg.predictedPlayerState.origin );
	if ( center < 0 ) {
		return;
	}

	range = cg_racingLineDistance.value;
	if ( range < 30.0f ) {
		range = 30.0f;
	} else if ( range > 500.0f ) {
		range = 500.0f;
	}
	range *= RL_UNITS_PER_METER;

	behind = (int)( RL_BEHIND_METERS * RL_UNITS_PER_METER / RL_SPACING ) + 1;
	maxSteps = rl.loop ? rl.count - behind - 1 : rl.count;
	if ( maxSteps > RL_MAX_DRAW_POLYS + behind ) {
		maxSteps = RL_MAX_DRAW_POLYS + behind;
	}

	for ( steps = -behind; steps < maxSteps && polys < RL_MAX_DRAW_POLYS; steps++ ) {
		int a = RL_Wrap( center + steps );
		int b = RL_Wrap( center + steps + 1 );
		float aheadA, aheadB, alphaA, alphaB;

		if ( a < 0 || b < 0 || a >= rl.count || b >= rl.count ) {
			continue;
		}
		if ( rl.points[b].breakBefore ) {
			continue;
		}
		aheadA = RL_Ahead( center, a, steps );
		aheadB = RL_Ahead( center, b, steps + 1 );
		if ( aheadA > range ) {
			break;
		}
		alphaA = RL_Alpha( aheadA, range );
		alphaB = RL_Alpha( aheadB, range );
		if ( alphaA <= 0.0f && alphaB <= 0.0f ) {
			continue;
		}

		s = polys * 4;
		RL_SetVert( &verts[s + 0], &rl.points[a], -1.0f, alphaA );
		RL_SetVert( &verts[s + 1], &rl.points[a], 1.0f, alphaA );
		RL_SetVert( &verts[s + 2], &rl.points[b], 1.0f, alphaB );
		RL_SetVert( &verts[s + 3], &rl.points[b], -1.0f, alphaB );
		polys++;
	}

	if ( polys > 0 ) {
		trap_R_AddPolysToScene( rl_shader, 4, verts, polys );
	}
}
