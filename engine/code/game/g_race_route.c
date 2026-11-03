/* Shared racing route adapter and geometric guidance. GPL-2.0-or-later. */
#include "g_local.h"

static botRaceRoute_t s_mapRoute;
static qboolean s_bezierRouteBuilt;
static qboolean s_selectionLogged;
static int s_routeRotateDurations[MAX_GHOST_BOT_WAYPOINTS];

// Conservative longitudinal deceleration estimate for braking-distance planning.
#define RACE_ROUTE_BRAKE_ACCEL 500.0f

static float RaceClamp( float value, float low, float high ) {
	return value < low ? low : ( value > high ? high : value );
}

void G_RaceRoute_Reset( void ) {
	Com_Memset( &s_mapRoute, 0, sizeof( s_mapRoute ) );
	s_bezierRouteBuilt = qfalse;
	s_selectionLogged = qfalse;
}

// Use bounds only to cull candidates; confirm the sweep against the actual trigger brush.
static qboolean RaceCrossesGate( const vec3_t a, const vec3_t b,
	const gentity_t *gate, float minimumFraction, float *fraction ) {
	vec3_t edge, localMins, localMaxs, testMins, testMaxs, point;
	float enter = minimumFraction, leave = 1.0f;
	float length;
	int steps, sample;
	int axis;
	if ( !gate || !gate->inuse ) return qfalse;
	VectorSubtract( b, a, edge );
	for ( axis = 0; axis < 3; axis++ ) {
		float extent = ( axis == 2 ) ? CAR_HEIGHT * 0.5f : CAR_LENGTH * 0.5f;
		float delta = edge[axis];
		float low = gate->r.absmin[axis] - extent;
		float high = gate->r.absmax[axis] + extent;
		float first, last;
		if ( fabs( delta ) < 0.001f ) {
			if ( a[axis] < low || a[axis] > high ) return qfalse;
			continue;
		}
		first = ( low - a[axis] ) / delta;
		last = ( high - a[axis] ) / delta;
		if ( first > last ) { float swap = first; first = last; last = swap; }
		if ( first > enter ) enter = first;
		if ( last < leave ) leave = last;
		if ( enter > leave ) return qfalse;
	}

	// A checkpoint's AABB can be very loose for angled brushes. Match the game
	// trigger check using the car bounds against the actual brush model.
	VectorSet( localMins, -CAR_LENGTH * 0.5f, -CAR_LENGTH * 0.5f, -CAR_HEIGHT * 0.5f );
	VectorSet( localMaxs, CAR_LENGTH * 0.5f, CAR_LENGTH * 0.5f, CAR_HEIGHT * 0.5f );
	length = Distance( a, b );
	steps = (int)( length / 24.0f ) + 1;
	if ( steps < 1 ) steps = 1;
	for ( sample = 0; sample <= steps; sample++ ) {
		float testFraction = enter + ( 1.0f - enter ) * ( (float)sample / steps );
		VectorMA( a, testFraction, edge, point );
		VectorAdd( point, localMins, testMins );
		VectorAdd( point, localMaxs, testMaxs );
		if ( trap_EntityContact( testMins, testMaxs, gate ) ) {
			*fraction = testFraction;
			return qtrue;
		}
	}
	return qfalse;
}

static void RaceReverseWaypoints( ghostWaypoint_t *values, int first, int last ) {
	while ( first < last ) {
		ghostWaypoint_t swap = values[first];
		values[first] = values[last];
		values[last] = swap;
		first++;
		last--;
	}
}

static void RaceReverseSegments( ghostRouteSegment_t *values, int first, int last ) {
	while ( first < last ) {
		ghostRouteSegment_t swap = values[first];
		values[first] = values[last];
		values[last] = swap;
		first++;
		last--;
	}
}

static void RaceReverseInts( int *values, int first, int last ) {
	while ( first < last ) {
		int swap = values[first];
		values[first] = values[last];
		values[last] = swap;
		first++;
		last--;
	}
}

static void RaceRotateWaypointsLeft( ghostWaypoint_t *values, int count, int offset ) {
	if ( offset <= 0 || offset >= count ) return;
	RaceReverseWaypoints( values, 0, offset - 1 );
	RaceReverseWaypoints( values, offset, count - 1 );
	RaceReverseWaypoints( values, 0, count - 1 );
}

static void RaceRotateSegmentsLeft( ghostRouteSegment_t *values, int count, int offset ) {
	if ( offset <= 0 || offset >= count ) return;
	RaceReverseSegments( values, 0, offset - 1 );
	RaceReverseSegments( values, offset, count - 1 );
	RaceReverseSegments( values, 0, count - 1 );
}

static void RaceRotateIntsLeft( int *values, int count, int offset ) {
	if ( offset <= 0 || offset >= count ) return;
	RaceReverseInts( values, 0, offset - 1 );
	RaceReverseInts( values, offset, count - 1 );
	RaceReverseInts( values, 0, count - 1 );
}

/* The game treats a circuit's combined start/finish brush as a gate, while
   the Radiant BPD contains only numbered rally_checkpoint control points.
   Rotate the sampled closed spline to just after that gate so both the gate
   validator and the bot's first-checkpoint guidance follow the same order. */
static qboolean RaceRotateClosedRouteToGate( gentity_t *gate ) {
	int segmentCount = s_mapRoute.numSegments;
	int gateSegment = -1;
	int rotate, i, elapsed = 0;
	float fraction = 0.0f;
	if ( !gate || segmentCount < 1 ) return qfalse;
	for ( i = 0; i < segmentCount; i++ ) {
		if ( RaceCrossesGate( s_mapRoute.waypoints[i].origin,
			s_mapRoute.waypoints[i + 1].origin, gate, 0.0f, &fraction ) ) {
			gateSegment = i;
			break;
		}
	}
	if ( gateSegment < 0 ) return qfalse;
	rotate = ( gateSegment + 1 ) % segmentCount;
	if ( !rotate ) return qtrue;
	for ( i = 0; i < segmentCount; i++ ) {
		s_routeRotateDurations[i] = s_mapRoute.waypoints[i + 1].timeOffset -
			s_mapRoute.waypoints[i].timeOffset;
	}
	RaceRotateWaypointsLeft( s_mapRoute.waypoints, segmentCount, rotate );
	RaceRotateSegmentsLeft( s_mapRoute.segments, segmentCount, rotate );
	RaceRotateIntsLeft( s_routeRotateDurations, segmentCount, rotate );
	for ( i = 0; i < segmentCount; i++ ) {
		s_mapRoute.waypoints[i].timeOffset = elapsed;
		elapsed += s_routeRotateDurations[i];
	}
	s_mapRoute.waypoints[segmentCount] = s_mapRoute.waypoints[0];
	s_mapRoute.waypoints[segmentCount].timeOffset = elapsed;
	s_mapRoute.waypoints[0].required = qtrue;
	s_mapRoute.waypoints[segmentCount].required = qtrue;
	return qtrue;
}

qboolean G_RaceRoute_ValidateGhost( ghostBotRoute_t *route ) {
    int gateIndex, segment = 0, labelStart = 0, i;
	float fraction = 0.0f;
	if ( !route || !route->valid || route->numWaypoints < 2 ) return qfalse;
	// Gate order comes from the active length/direction, not the file's label.
	for ( gateIndex = 0; gateIndex < level.numCheckpoints; gateIndex++ ) {
		qboolean found = qfalse;
		for ( ; segment < route->numWaypoints - 1; segment++, fraction = 0.0f ) {
			if ( RaceCrossesGate( route->waypoints[segment].origin,
				route->waypoints[segment + 1].origin, level.checkpoints[gateIndex], fraction, &fraction ) ) {
				found = qtrue;
				break;
			}
		}
		if ( !found ) return qfalse;
		for ( i = labelStart; i <= segment; i++ ) route->segments[i].nextCheckpoint = gateIndex + 1;
		labelStart = segment + 1;
	}
	for ( i = labelStart; i < route->numWaypoints - 1; i++ )
		route->segments[i].nextCheckpoint = level.numCheckpoints + ( level.sprintTrack ? 1 : 0 );
	// Sprint has a separate finish beyond the last ordinary checkpoint.
	if ( level.sprintTrack && level.finishEnt ) {
		for ( ; segment < route->numWaypoints - 1; segment++, fraction = 0.0f ) {
			if ( RaceCrossesGate( route->waypoints[segment].origin,
				route->waypoints[segment + 1].origin, level.finishEnt, fraction, &fraction ) ) return qtrue;
		}
		return qfalse;
	}
	return qtrue;
}

static void RaceBezierControls( const vec3_t p0, const vec3_t h0,
	const vec3_t p3, const vec3_t h3, qboolean reversed, vec3_t c1, vec3_t c2 ) {
	if ( reversed ) {
		VectorMA( p0, -1.0f, h0, c1 );
		VectorAdd( p3, h3, c2 );
	} else {
		VectorAdd( p0, h0, c1 );
		VectorMA( p3, -1.0f, h3, c2 );
	}
}

static void RaceBezierEvaluate( const vec3_t p0, const vec3_t h0,
	const vec3_t p3, const vec3_t h3, qboolean reversed, float t,
	vec3_t point, vec3_t derivative, vec3_t secondDerivative ) {
	vec3_t c1, c2;
	float u = 1.0f - t;
	RaceBezierControls( p0, h0, p3, h3, reversed, c1, c2 );
	if ( point ) {
		VectorScale( p0, u * u * u, point );
		VectorMA( point, 3.0f * t * u * u, c1, point );
		VectorMA( point, 3.0f * t * t * u, c2, point );
		VectorMA( point, t * t * t, p3, point );
	}
	if ( derivative ) {
		VectorScale( p0, -3.0f * u * u, derivative );
		VectorMA( derivative, 3.0f * u * u - 6.0f * t * u, c1, derivative );
		VectorMA( derivative, 6.0f * t * u - 3.0f * t * t, c2, derivative );
		VectorMA( derivative, 3.0f * t * t, p3, derivative );
	}
	if ( secondDerivative ) {
		VectorScale( p0, 6.0f * u, secondDerivative );
		VectorMA( secondDerivative, -12.0f * u + 6.0f * t, c1, secondDerivative );
		VectorMA( secondDerivative, 6.0f * u - 12.0f * t, c2, secondDerivative );
		VectorMA( secondDerivative, 6.0f * t, p3, secondDerivative );
	}
}

static qboolean RaceImportBezier( void ) {
	int checkpointCount = level.numCheckpoints;
	gentity_t *routePoints[MAX_GHOST_BOT_WAYPOINTS / 8];
	gentity_t *startFinishGate = NULL;
	int routePointCount = 0;
	int segmentIndex, segmentCount, sampleCount = 0, time = 0;
	int handleCount = 0;
	qboolean reversed = ( g_trackReversed.integer && level.trackIsReversable ) ? qtrue : qfalse;
	if ( checkpointCount < ( level.sprintTrack ? 1 : 2 ) || checkpointCount > MAX_GHOST_BOT_WAYPOINTS / 8 ) {
		G_Printf( "G_RaceRoute: BPD unavailable (sprint=%d checkpoints=%d)\n", level.sprintTrack, checkpointCount );
		return qfalse;
	}
	if ( level.sprintTrack && ( !level.startEnt || !level.finishEnt ) ) {
		G_Printf( "G_RaceRoute: BPD unavailable (sprint endpoints not initialized)\n" );
		return qfalse;
	}
	for ( segmentIndex = 0; segmentIndex < checkpointCount; segmentIndex++ ) {
		if ( !level.checkpoints[segmentIndex] || !level.checkpoints[segmentIndex]->inuse ) {
			G_Printf( "G_RaceRoute: BPD unavailable (checkpoint slot %d missing)\n", segmentIndex + 1 );
			return qfalse;
		}
		/* A lap's combined start/finish is a race gate, not a BPD control point.
		   Sprint start and finish are separate endpoint entities and are handled
		   below in the same way. */
		if ( !level.sprintTrack && level.checkpoints[segmentIndex] == level.finishEnt ) {
			startFinishGate = level.checkpoints[segmentIndex];
			continue;
		}
		if ( !level.checkpoints[segmentIndex]->bpdLoaded ) {
			G_Printf( "G_RaceRoute: BPD unavailable (checkpoint %d has no BPD entry)\n",
				level.checkpoints[segmentIndex]->bpdIndex );
			return qfalse;
		}
		if ( routePointCount >= ARRAY_LEN( routePoints ) ) return qfalse;
		routePoints[routePointCount++] = level.checkpoints[segmentIndex];
		if ( VectorLengthSquared( level.checkpoints[segmentIndex]->s.angles2 ) > 0.01f ) handleCount++;
	}
	if ( routePointCount < ( level.sprintTrack ? 1 : 2 ) ) {
		G_Printf( "G_RaceRoute: BPD unavailable (route control points=%d)\n", routePointCount );
		return qfalse;
	}
	G_Printf( "G_RaceRoute: importing BPD handles=%d/%d reversed=%d\n", handleCount, routePointCount, reversed );

	Com_Memset( &s_mapRoute, 0, sizeof( s_mapRoute ) );
	s_mapRoute.closed = level.sprintTrack ? qfalse : qtrue;
	s_mapRoute.fromBezierPath = qtrue;
	Q_strncpyz( s_mapRoute.path, "checkpoint_bpd", sizeof( s_mapRoute.path ) );
	segmentCount = level.sprintTrack ? routePointCount + 1 : routePointCount;
	for ( segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++ ) {
		gentity_t *start, *end;
		vec3_t p0, h0, p3, h3;
		vec3_t previous, point, nextPoint;
		float approximateLength = 0.0f;
		int step, steps;
		if ( level.sprintTrack ) {
			if ( segmentIndex == 0 ) {
				start = reversed ? level.finishEnt : level.startEnt;
				end = routePoints[0];
				VectorCopy( start->s.origin2, p0 );
				VectorCopy( end->s.origin2, p3 );
				VectorSubtract( p3, p0, h0 );
				VectorScale( h0, ( reversed ? -1.0f : 1.0f ) / 3.0f, h0 );
				VectorCopy( end->s.angles2, h3 );
			} else if ( segmentIndex == routePointCount ) {
				start = routePoints[routePointCount - 1];
				end = reversed ? level.startEnt : level.finishEnt;
				VectorCopy( start->s.origin2, p0 );
				VectorCopy( end->s.origin2, p3 );
				VectorCopy( start->s.angles2, h0 );
				VectorSubtract( p3, p0, h3 );
				VectorScale( h3, ( reversed ? -1.0f : 1.0f ) / 3.0f, h3 );
			} else {
				start = routePoints[segmentIndex - 1];
				end = routePoints[segmentIndex];
				VectorCopy( start->s.origin2, p0 );
				VectorCopy( start->s.angles2, h0 );
				VectorCopy( end->s.origin2, p3 );
				VectorCopy( end->s.angles2, h3 );
			}
		} else {
			int startIndex = ( routePointCount - 1 + segmentIndex ) % routePointCount;
			int endIndex = ( startIndex + 1 ) % routePointCount;
			start = routePoints[startIndex];
			end = routePoints[endIndex];
			VectorCopy( start->s.origin2, p0 );
			VectorCopy( start->s.angles2, h0 );
			VectorCopy( end->s.origin2, p3 );
			VectorCopy( end->s.angles2, h3 );
		}
		if ( reversed ) {
			VectorScale( h0, -1.0f, h0 );
			VectorScale( h3, -1.0f, h3 );
		}
		RaceBezierEvaluate( p0, h0, p3, h3, qfalse, 0.0f, previous, NULL, NULL );
		for ( step = 0; step <= 12; step++ ) {
			RaceBezierEvaluate( p0, h0, p3, h3, qfalse, step / 12.0f, point, NULL, NULL );
			if ( step > 0 ) {
				VectorSubtract( point, previous, nextPoint );
				approximateLength += VectorLength( nextPoint );
			}
			VectorCopy( point, previous );
		}
		steps = (int)( approximateLength / 72.0f ) + 1;
		if ( steps < 8 ) steps = 8;
		if ( steps > 128 ) steps = 128;
		if ( sampleCount + steps + 1 > MAX_GHOST_BOT_WAYPOINTS ) return qfalse;
		for ( step = 0; step < steps; step++ ) {
			float fraction = (float)step / steps;
			float midpoint = ( (float)step + 0.5f ) / steps;
			vec3_t alpha, delta, cross, chord;
			float chordLength, derivativeLength, curvature, recommendedSpeed;
			float gravity = CP_CURRENT_GRAVITY > 1.0f ? CP_CURRENT_GRAVITY : 800.0f;
			int family;
			ghostWaypoint_t *waypoint = &s_mapRoute.waypoints[sampleCount];
			ghostRouteSegment_t *segment = &s_mapRoute.segments[sampleCount];

			RaceBezierEvaluate( p0, h0, p3, h3, qfalse, fraction, waypoint->origin, NULL, NULL );
			waypoint->timeOffset = time;
			waypoint->required = ( sampleCount == 0 ) ? qtrue : qfalse;
			RaceBezierEvaluate( p0, h0, p3, h3, qfalse, (float)( step + 1 ) / steps, nextPoint, NULL, NULL );
			VectorSubtract( nextPoint, waypoint->origin, chord );
			chordLength = VectorLength( chord );
			RaceBezierEvaluate( p0, h0, p3, h3, qfalse, midpoint, NULL, delta, alpha );
			CrossProduct( delta, alpha, cross );
			derivativeLength = VectorLength( delta );
			curvature = derivativeLength > 0.001f ? VectorLength( cross ) /
				( derivativeLength * derivativeLength * derivativeLength ) : 0.0f;
			recommendedSpeed = curvature > 0.000001f ? sqrt( gravity / curvature ) : 2600.0f;
			segment->recommendedSpeed = RaceClamp( recommendedSpeed, 300.0f, 2600.0f );
			segment->curvature = curvature;
			for ( family = 0; family < GHOST_LINE_FAMILY_COUNT; family++ ) segment->lines[family].speedScale = 1.0f;
			time += (int)( 1000.0f * chordLength / segment->recommendedSpeed ) + 1;
			sampleCount++;
		}
	}
	if ( level.sprintTrack ) {
		gentity_t *finish = reversed ? level.startEnt : level.finishEnt;
		VectorCopy( finish->s.origin2, s_mapRoute.waypoints[sampleCount].origin );
	} else {
		VectorCopy( s_mapRoute.waypoints[0].origin, s_mapRoute.waypoints[sampleCount].origin );
	}
	s_mapRoute.waypoints[sampleCount].timeOffset = time;
	s_mapRoute.waypoints[sampleCount].required = qtrue;
	s_mapRoute.numWaypoints = sampleCount + 1;
	s_mapRoute.numSegments = sampleCount;
	s_mapRoute.valid = qtrue;
	if ( !level.sprintTrack && startFinishGate &&
		!RaceRotateClosedRouteToGate( startFinishGate ) ) {
		s_mapRoute.valid = qfalse;
		G_Printf( "G_RaceRoute: BPD rejected (route misses start/finish gate)\n" );
		return qfalse;
	}
	if ( !G_RaceRoute_ValidateGhost( &s_mapRoute ) ) {
		s_mapRoute.valid = qfalse;
		G_Printf( "G_RaceRoute: BPD rejected (route misses checkpoint order)\n" );
		return qfalse;
	}
	G_Printf( "G_RaceRoute: BPD route ready waypoints=%d\n", s_mapRoute.numWaypoints );
	return qtrue;
}

qboolean G_RaceRoute_Get( const botRaceRoute_t **outRoute ) {
	if ( !outRoute ) return qfalse;
	*outRoute = NULL;
	if ( !s_bezierRouteBuilt ) {
		s_bezierRouteBuilt = qtrue;
		RaceImportBezier();
	}
	if ( s_mapRoute.valid ) *outRoute = &s_mapRoute;
	if ( !s_selectionLogged ) {
		G_Printf( "G_RaceRoute: source=%s route=%s\n", *outRoute ? "bpd" : "checkpoint",
			*outRoute ? ( *outRoute )->path : "fallback" );
		s_selectionLogged = qtrue;
	}
	return *outRoute != NULL;
}

qboolean G_RaceRoute_Guidance( const botRaceRoute_t *route, const vec3_t origin,
	int hint, qboolean allowWrap, int nextCheckpoint, float actualSpeed, botRaceGuidance_t *out ) {
	int i, best = -1, count;
	float bestDistance = 1e30f, bestFraction = 0.0f;
	float lookAhead, travelled, preview, limit;
	qboolean targetSet = qfalse;
	if ( !route || !route->valid || !out || route->numSegments < 1 ) return qfalse;
	count = route->numSegments;
	if ( hint >= 0 && hint < count && nextCheckpoint > 0 &&
		route->segments[hint].nextCheckpoint > 0 && route->segments[hint].nextCheckpoint != nextCheckpoint ) hint = -1;
	Com_Memset( out, 0, sizeof( *out ) );
	for ( i = 0; i < count; i++ ) {
		vec3_t edge, delta, closest;
		float lengthSq, fraction, distance;
		if ( nextCheckpoint > 0 && route->segments[i].nextCheckpoint > 0 &&
			route->segments[i].nextCheckpoint != nextCheckpoint ) continue;
		if ( hint >= 0 && hint < count && ( i < hint - 3 || i > hint + 48 ) &&
			!( route->closed && allowWrap && hint >= count - 48 && i < 48 ) ) continue;
		VectorSubtract( route->waypoints[i + 1].origin, route->waypoints[i].origin, edge );
		lengthSq = VectorLengthSquared( edge );
		if ( lengthSq < 0.01f ) continue;
		VectorSubtract( origin, route->waypoints[i].origin, delta );
		fraction = RaceClamp( DotProduct( delta, edge ) / lengthSq, 0.0f, 1.0f );
		VectorMA( route->waypoints[i].origin, fraction, edge, closest );
		VectorSubtract( origin, closest, delta );
		distance = VectorLengthSquared( delta );
		if ( distance < bestDistance ) { best = i; bestDistance = distance; bestFraction = fraction; }
	}
	if ( best < 0 ) return qfalse;
	out->segment = best;
	out->distance = sqrt( bestDistance );
	VectorSubtract( route->waypoints[best + 1].origin, route->waypoints[best].origin, out->tangent );
	VectorNormalize( out->tangent );
	lookAhead = RaceClamp( 180.0f + actualSpeed * 0.25f, 180.0f, 600.0f );
	preview = RaceClamp( actualSpeed * actualSpeed / ( 2.0f * RACE_ROUTE_BRAKE_ACCEL ) + lookAhead,
		lookAhead, 4096.0f );
	travelled = 0.0f;
	limit = 2600.0f;
	for ( i = 0; i < count && travelled <= preview; i++ ) {
		int segment = ( best + i ) % count;
		int next = ( segment + 1 ) % count;
		vec3_t edge, nextEdge;
		float length, remaining, speed, dot = 1.0f;
		if ( best + i >= count && !route->closed ) break;
		VectorSubtract( route->waypoints[segment + 1].origin, route->waypoints[segment].origin, edge );
		length = VectorNormalize( edge );
		remaining = length * ( i == 0 ? 1.0f - bestFraction : 1.0f );
		speed = RaceClamp( route->segments[segment].recommendedSpeed, 150.0f, 2600.0f );
		// Incoming corner limit, applied with braking distance rather than an average.
		if ( segment + 1 < count || route->closed ) {
			float nextLength, cornerSpeed;
			VectorSubtract( route->waypoints[next + 1].origin, route->waypoints[next].origin, nextEdge );
			nextLength = VectorNormalize( nextEdge );
			dot = RaceClamp( DotProduct( edge, nextEdge ), -1.0f, 1.0f );
			cornerSpeed = sqrt( 600.0f * ( length + nextLength ) * 0.5f / ( 0.01f + sqrt( 2.0f * ( 1.0f - dot ) ) ) );
			if ( cornerSpeed < speed ) speed = cornerSpeed;
		}
		if ( sqrt( speed * speed + 2.0f * RACE_ROUTE_BRAKE_ACCEL * travelled ) < limit )
			limit = sqrt( speed * speed + 2.0f * RACE_ROUTE_BRAKE_ACCEL * travelled );
		if ( i == 0 ) out->curvature = 1.0f - dot;
		if ( !targetSet ) {
			float offset = RaceClamp( lookAhead - travelled, 0.0f, remaining );
			VectorMA( route->waypoints[segment].origin, ( i == 0 ? bestFraction * length : 0.0f ) + offset, edge, out->target );
			out->targetIndex = segment + 1;
			if ( travelled + remaining >= lookAhead ) targetSet = qtrue;
			// Hold the next gate as the target until the game confirms its crossing.
			if ( nextCheckpoint > 0 && route->segments[next].nextCheckpoint > 0 &&
				route->segments[next].nextCheckpoint != nextCheckpoint ) targetSet = qtrue;
		}
		travelled += remaining;
	}
	out->targetSpeed = limit;
	return qtrue;
}
