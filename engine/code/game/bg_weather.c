/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// bg_weather.c -- Q3Rally weather shared by the server and the client
//
// The server decides the weather (g_weather.c) and sends it as an anchor in
// CS_WEATHER: the precipitation type, an intensity ramp from startTime to
// endTime and how wet and snowed in the ground was at startTime. Both sides
// integrate the ground from there in the same fixed steps, so the physics
// on the server, the prediction on the client and what the client draws all
// agree, also for a client that joins later.
//
// The anchor also carries the wind (direction and speed, turning over the
// same ramp); the gusts on top of it are a fixed function of the time.
//
// Where weather reaches the ground is in a grid built when the map loads
// (CS_WEATHERGRID gives its placement): for every cell the height of the
// highest solid surface seen from the sky. A point is out in the weather if
// it is not far below that height; under a bridge or a roof it is not.

#include "../qcommon/q_shared.h"
#include "bg_public.h"

/*
=======================================================================

GROUND STATE

=======================================================================
*/

#define WEATHER_STEP		100			// ms per integration step
#define WEATHER_WET_TIME	20.0f		// seconds to get wet in heavy rain
#define WEATHER_DRY_TIME	120.0f		// seconds to dry
#define WEATHER_SNOW_TIME	90.0f		// seconds to snow in during heavy snow
#define WEATHER_MELT_TIME	240.0f		// seconds for the snow to melt

float BG_WeatherIntensity( const weatherAnchor_t *a, int time ) {
	float f;

	if ( a->type == WEATHER_NONE ) {
		return 0.0f;
	}
	if ( time >= a->endTime || a->endTime <= a->startTime ) {
		return a->intensity1;
	}
	if ( time <= a->startTime ) {
		return a->intensity0;
	}

	f = (float)( time - a->startTime ) / (float)( a->endTime - a->startTime );
	return a->intensity0 + ( a->intensity1 - a->intensity0 ) * f;
}

static float BG_WeatherApproach( float value, float target, float up, float down ) {
	if ( value < target ) {
		value += up;
		if ( value > target ) {
			value = target;
		}
	} else if ( value > target ) {
		value -= down;
		if ( value < target ) {
			value = target;
		}
	}
	return value;
}

/*
=================
BG_WeatherStep

One step of the ground: rain wets it (more and faster the harder it rains),
snow covers it, without either it dries and the snow melts, and melting
snow keeps the ground wet.
=================
*/
static void BG_WeatherStep( weatherState_t *st, int time ) {
	const weatherAnchor_t *a = &st->anchor;
	float dt = WEATHER_STEP * 0.001f;
	float in = BG_WeatherIntensity( a, time );
	float wetTarget = 0.0f;

	if ( a->type == WEATHER_RAIN && in > 0.01f ) {
		wetTarget = 0.35f + 0.65f * in;
		st->wetness = BG_WeatherApproach( st->wetness, wetTarget,
			dt * ( 0.3f + in ) / WEATHER_WET_TIME, dt / WEATHER_DRY_TIME );
	} else {
		st->wetness = BG_WeatherApproach( st->wetness, 0.0f, 0.0f, dt / WEATHER_DRY_TIME );
	}

	if ( a->type == WEATHER_SNOW && in > 0.01f ) {
		st->snow = BG_WeatherApproach( st->snow, 1.0f, dt * in / WEATHER_SNOW_TIME, 0.0f );
	} else if ( st->snow > 0.0f ) {
		st->snow = BG_WeatherApproach( st->snow, 0.0f, 0.0f, dt / WEATHER_MELT_TIME );
		if ( st->wetness < st->snow ) {
			st->wetness = st->snow;		// melt water
		}
	}
}

/*
=================
BG_WeatherWindBase

The wind of the anchor at a time, without the gusts: turns the short way
round from windYaw0 to windYaw1 while the speed changes.
=================
*/
void BG_WeatherWindBase( const weatherAnchor_t *a, int time, float *yaw, float *speed ) {
	float f, d;

	if ( time >= a->endTime || a->endTime <= a->startTime ) {
		f = 1.0f;
	} else if ( time <= a->startTime ) {
		f = 0.0f;
	} else {
		f = (float)( time - a->startTime ) / (float)( a->endTime - a->startTime );
	}

	d = AngleNormalize180( a->windYaw1 - a->windYaw0 );
	*yaw = AngleNormalize360( a->windYaw0 + d * f );
	*speed = a->windSpeed0 + ( a->windSpeed1 - a->windSpeed0 ) * f;
}

/*
=================
BG_WeatherWindNow

The wind with gusts: a few slow waves of different length, so it never
repeats in a way one would notice; stronger gusts while it rains or snows.
The direction swings a little with it.
=================
*/
static void BG_WeatherWindNow( weatherState_t *st, int time ) {
	float t = ( time & 0xffffff ) * 0.001f;		// seconds, kept small for the sines
	float gustiness = 0.3f + 0.3f * st->intensity;
	float gust, swing, yaw, speed;

	BG_WeatherWindBase( &st->anchor, time, &yaw, &speed );
	st->windYaw = yaw;
	st->windSpeed = speed;

	gust = 0.55f * sin( t * 0.31f ) + 0.3f * sin( t * 0.83f + 1.7f ) + 0.15f * sin( t * 2.3f + 4.1f );
	gust = 1.0f + gustiness * gust;
	swing = 10.0f * sin( t * 0.13f + 0.5f ) + 5.0f * sin( t * 0.47f + 2.2f );

	yaw = DEG2RAD( yaw + swing );
	st->wind[0] = cos( yaw ) * speed * gust;
	st->wind[1] = sin( yaw ) * speed * gust;
	st->wind[2] = 0.0f;
}

void BG_WeatherSetAnchor( weatherState_t *st, const weatherAnchor_t *a ) {
	st->anchor = *a;
	st->steps = 0;
	st->wetness = a->wetness0;
	st->snow = a->snow0;
}

/*
=================
BG_WeatherAdvance

Brings the ground up to time. Only the steps since the last call are done;
going back in time (a demo restarting) starts again from the anchor.
=================
*/
void BG_WeatherAdvance( weatherState_t *st, int time ) {
	int target;

	if ( time < st->anchor.startTime ) {
		target = 0;
	} else {
		target = ( time - st->anchor.startTime ) / WEATHER_STEP;
	}

	if ( target < st->steps ) {
		BG_WeatherSetAnchor( st, &st->anchor );
	}

	// a client that joins long after the anchor catches up here once
	while ( st->steps < target ) {
		BG_WeatherStep( st, st->anchor.startTime + st->steps * WEATHER_STEP + WEATHER_STEP / 2 );
		st->steps++;
	}

	st->intensity = BG_WeatherIntensity( &st->anchor, time );
	BG_WeatherWindNow( st, time );
}

void BG_WeatherAnchorString( const weatherAnchor_t *a, char *buf, int size ) {
	Com_sprintf( buf, size, "%i %i %i %.3f %.3f %.3f %.3f %.1f %.1f %.1f %.1f", a->type, a->startTime, a->endTime,
		a->intensity0, a->intensity1, a->wetness0, a->snow0, a->windYaw0, a->windSpeed0, a->windYaw1, a->windSpeed1 );
}

void BG_WeatherParseAnchor( const char *s, weatherAnchor_t *a ) {
	char buf[MAX_INFO_STRING];
	char *p, *tok;
	int i;
	float f[11];

	memset( f, 0, sizeof( f ) );
	Q_strncpyz( buf, s, sizeof( buf ) );
	p = buf;
	for ( i = 0; i < 11; i++ ) {
		tok = COM_Parse( &p );
		if ( !tok[0] ) {
			break;
		}
		f[i] = atof( tok );
	}

	a->type = (int)f[0];
	if ( a->type < WEATHER_NONE || a->type > WEATHER_SNOW ) {
		a->type = WEATHER_NONE;
	}
	a->startTime = (int)f[1];
	a->endTime = (int)f[2];
	a->intensity0 = f[3];
	a->intensity1 = f[4];
	a->wetness0 = f[5];
	a->snow0 = f[6];
	a->windYaw0 = f[7];
	a->windSpeed0 = f[8];
	a->windYaw1 = f[9];
	a->windSpeed1 = f[10];
}

/*
=======================================================================

SKY GRID

=======================================================================
*/

#define WEATHER_GRID_CELL		64.0f
#define WEATHER_GRID_BELOW		40.0f		// how far below the surface still counts as out in the weather

void BG_WeatherGridPlace( weatherGrid_t *g, const vec3_t mins, const vec3_t maxs ) {
	float cell = WEATHER_GRID_CELL;

	while ( ( maxs[0] - mins[0] ) / cell > WEATHER_GRID_MAX || ( maxs[1] - mins[1] ) / cell > WEATHER_GRID_MAX ) {
		cell *= 2.0f;
	}

	g->origin[0] = mins[0];
	g->origin[1] = mins[1];
	g->cell = cell;
	g->width = (int)( ( maxs[0] - mins[0] ) / cell ) + 1;
	g->height = (int)( ( maxs[1] - mins[1] ) / cell ) + 1;
	if ( g->width > WEATHER_GRID_MAX ) {
		g->width = WEATHER_GRID_MAX;
	}
	if ( g->height > WEATHER_GRID_MAX ) {
		g->height = WEATHER_GRID_MAX;
	}
	g->top = maxs[2];
	g->bottom = mins[2];
	g->valid = qfalse;
}

void BG_WeatherGridString( const weatherGrid_t *g, char *buf, int size ) {
	Com_sprintf( buf, size, "%i %i %i %i %i %i %i", (int)g->origin[0], (int)g->origin[1], (int)g->cell,
		g->width, g->height, (int)g->top, (int)g->bottom );
}

qboolean BG_WeatherGridParse( const char *s, weatherGrid_t *g ) {
	char buf[MAX_INFO_STRING];
	char *p, *tok;
	int i, v[7];

	Q_strncpyz( buf, s, sizeof( buf ) );
	p = buf;
	for ( i = 0; i < 7; i++ ) {
		tok = COM_Parse( &p );
		if ( !tok[0] ) {
			g->width = g->height = 0;
			g->valid = qfalse;
			return qfalse;
		}
		v[i] = atoi( tok );
	}

	g->origin[0] = v[0];
	g->origin[1] = v[1];
	g->cell = v[2];
	g->width = v[3];
	g->height = v[4];
	g->top = v[5];
	g->bottom = v[6];
	g->valid = qfalse;

	return g->cell > 0.0f && g->width > 0 && g->height > 0
		&& g->width <= WEATHER_GRID_MAX && g->height <= WEATHER_GRID_MAX;
}

/*
=================
BG_WeatherIsSky

A trace that went up ended at the sky: a sky face, or for maps whose sky
shader was compiled without surfaceparm sky (no SURF_SKY in the BSP) a
ceiling face at the top of the map.
=================
*/
#define WEATHER_SKY_TOP		160.0f

qboolean BG_WeatherIsSky( const weatherGrid_t *g, const trace_t *tr ) {
	if ( tr->startsolid || tr->fraction >= 1.0f ) {
		return qfalse;
	}
	if ( tr->surfaceFlags & SURF_SKY ) {
		return qtrue;
	}
	return tr->endpos[2] >= g->top - WEATHER_SKY_TOP && tr->plane.normal[2] < -0.7f
		&& !( tr->surfaceFlags & SURF_NODRAW );
}

/*
=================
BG_WeatherGridBuild

For every cell the highest surface from which the sky can be seen: going
down the column from the top of the map, each surface hit is tested with a
trace back up. The first one that sees a sky face is where the weather
lands. Surfaces that see nothing above them are the outside of the map
(the caulked top of the sky brushes); the column goes on below them. Cells
where nothing is found get the bottom of the map, so everything there
counts as out in the weather.

The trace function must only see the world (no entities), so that the
server and the client build the same grid.
=================
*/
void BG_WeatherGridBuild( weatherGrid_t *g, weatherTrace_t traceFunc ) {
	trace_t tr, up;
	vec3_t start, end, from, top;
	int x, y, tries;
	float h;

	for ( y = 0; y < g->height; y++ ) {
		for ( x = 0; x < g->width; x++ ) {
			start[0] = end[0] = g->origin[0] + ( x + 0.5f ) * g->cell;
			start[1] = end[1] = g->origin[1] + ( y + 0.5f ) * g->cell;
			start[2] = g->top - 1.0f;
			end[2] = g->bottom - 1.0f;
			h = g->bottom;

			for ( tries = 0; tries < 8; tries++ ) {
				traceFunc( &tr, start, NULL, NULL, end, ENTITYNUM_NONE, MASK_SOLID );
				if ( tr.allsolid || tr.fraction >= 1.0f ) {
					break;
				}

				if ( !( tr.surfaceFlags & SURF_SKY ) ) {
					// does this surface see the sky?
					from[0] = top[0] = start[0];
					from[1] = top[1] = start[1];
					from[2] = tr.endpos[2] + 2.0f;
					top[2] = g->top + 1.0f;
					traceFunc( &up, from, NULL, NULL, top, ENTITYNUM_NONE, MASK_SOLID );
					if ( BG_WeatherIsSky( g, &up ) ) {
						h = tr.endpos[2];
						break;
					}
				}

				// on through the solid below it
				start[2] = tr.endpos[2] - 1.0f;
			}

			if ( h < -32000.0f ) {
				h = -32000.0f;
			} else if ( h > 32000.0f ) {
				h = 32000.0f;
			}
			g->heights[y * g->width + x] = (short)h;
		}
	}

	g->valid = qtrue;
}

/*
=================
BG_WeatherExposed

Whether weather reaches the point. The lowest of the four nearest cells
counts, so slopes and the edges of the open sky are inside and the weather
blows a little way under the edge of a roof.
=================
*/
qboolean BG_WeatherExposed( const weatherGrid_t *g, const vec3_t point ) {
	float fx, fy, h, low;
	int x0, y0, x1, y1;

	if ( !g->valid ) {
		return qfalse;
	}

	fx = ( point[0] - g->origin[0] ) / g->cell - 0.5f;
	fy = ( point[1] - g->origin[1] ) / g->cell - 0.5f;
	x0 = (int)floor( fx );
	y0 = (int)floor( fy );
	x1 = x0 + 1;
	y1 = y0 + 1;
	if ( x0 < 0 ) x0 = 0;
	if ( y0 < 0 ) y0 = 0;
	if ( x1 > g->width - 1 ) x1 = g->width - 1;
	if ( y1 > g->height - 1 ) y1 = g->height - 1;
	if ( x0 > g->width - 1 ) x0 = g->width - 1;
	if ( y0 > g->height - 1 ) y0 = g->height - 1;
	if ( x1 < 0 ) x1 = 0;
	if ( y1 < 0 ) y1 = 0;

	low = g->heights[y0 * g->width + x0];
	h = g->heights[y0 * g->width + x1];
	if ( h < low ) low = h;
	h = g->heights[y1 * g->width + x0];
	if ( h < low ) low = h;
	h = g->heights[y1 * g->width + x1];
	if ( h < low ) low = h;

	return point[2] >= low - WEATHER_GRID_BELOW;
}

/*
=================
BG_WeatherWetness / BG_WeatherSnow

How wet or snowed in the ground at a point is, 0 to 1.
=================
*/
float BG_WeatherWetness( const weatherState_t *st, const weatherGrid_t *g, const vec3_t point ) {
	if ( st->wetness <= 0.0f || !BG_WeatherExposed( g, point ) ) {
		return 0.0f;
	}
	return st->wetness;
}

float BG_WeatherSnow( const weatherState_t *st, const weatherGrid_t *g, const vec3_t point ) {
	if ( st->snow <= 0.0f || !BG_WeatherExposed( g, point ) ) {
		return 0.0f;
	}
	return st->snow;
}
