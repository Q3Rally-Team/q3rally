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
cg_rally_fx.c  --  client-side map effects placed by mappers

rally_fx_spray
--------------
Spray and mist where a waterfall hits the water (also usable for rapids,
fountains or steam). The entity only lives in the map's entity string:
the server ignores it (g_spawn.c maps it to SP_info_null) and cgame reads
it once at map load with trap_GetEntityToken. All particles are simulated
and drawn locally, so the effect costs no entity slots, no snapshot
bandwidth and works the same in demos.

Keys (all optional except origin):
  origin    impact point, put it on the water surface
  radius    horizontal spread of the spray              (default 64)
  height    how high the mist rises                     (default 160)
  size      size of a mist puff                         (default 96)
  rate      mist puffs per second                       (default 30)
  droplets  droplets per second                         (default 50)
  angle     yaw the spray is thrown towards; -1 = all
            directions                                  (default -1)
  push      speed of the throw in that direction        (default 60)
  _color    tint "r g b" 0..1                           (default .92 .96 1)
  alpha     opacity of the mist                         (default 0.4)
  range     no new particles beyond this view distance  (default 3000)

The tint is multiplied with the light grid at the origin, so the spray is
not glowing white in a dark gorge or at night.

cg_mapEffects scales the density (0 = off, 1 = default, 2 = double).

Tyre smoke and dust
-------------------
CG_FX_TireSmoke puts the smoke of skidding tyres, the dust thrown up on
loose ground and the rear weapon smoke screen into the same particle pool: soft billowing puffs that grow,
turn, slow down (drag), rise or settle, take a part of the car's speed and
are tinted with the light grid where they start (no glowing smoke at
night). With r_softParticles (OpenGL2) they fade out where they cut into
the ground instead of showing a hard edge. cg_tireSmoke scales them
(0 = off, 1 = default, 2 = double).
===========================================================================
*/

#include "cg_local.h"

#define FX_MAX_EMITTERS         32
#define FX_MAX_PARTICLES        1024
#define FX_MAX_SPAWN_PER_FRAME  24
#define FX_MAX_KEYS             32

typedef enum {
	FXP_MIST,
	FXP_DROPLET,
	FXP_SMOKE			// tyre smoke and dust
} fxParticleType_t;

// tyre smoke and dust, see CG_FX_TireSmoke
typedef struct {
	vec3_t	color;
	float	alpha;
	float	size0, size1;		// start and end radius
	int		life, lifeRand;		// msec
	float	gravity;			// < 0 rises
	float	drag;
	float	spread;				// sideways speed
	float	upSpeed;
	float	carShare;			// share of the car's velocity
} fxSmokeKind_t;

static const fxSmokeKind_t fxSmokeKinds[FX_SMOKE_NUM_KINDS] = {
	// colour                alpha size0 size1 life lifeRand grav  drag spread up  car
	{ { 0.92f, 0.92f, 0.93f }, 0.42f, 12, 78, 2200, 900,  -14, 1.4f, 30,  22, 0.15f },	// FX_SMOKE_TIRE
	{ { 0.66f, 0.55f, 0.40f }, 0.55f, 16, 105, 2600, 1000,  -4, 1.0f, 50,  45, 0.30f },	// FX_SMOKE_DUST
	{ { 0.50f, 0.41f, 0.30f }, 0.45f, 12, 80, 1800, 600,    8, 1.6f, 40,  40, 0.25f },	// FX_SMOKE_DIRT
	{ { 0.60f, 0.57f, 0.52f }, 0.32f, 10, 62, 1400, 500,   10, 1.8f, 35,  30, 0.20f },	// FX_SMOKE_GRAVEL
	{ { 0.80f, 0.70f, 0.50f }, 0.50f, 14, 92, 2000, 800,    4, 1.3f, 45,  40, 0.25f },	// FX_SMOKE_SAND
	{ { 0.40f, 0.38f, 0.24f }, 0.30f, 10, 55, 1200, 400,   12, 2.0f, 35,  30, 0.20f },	// FX_SMOKE_GRASS
	{ { 0.70f, 0.70f, 0.72f }, 0.92f, 40, 230, 7000, 3000,  -5, 1.5f, 90,  30, 0.00f },	// FX_SMOKE_SCREEN
	{ { 0.24f, 0.23f, 0.22f }, 0.85f, 30, 150, 3500, 1500, -45, 1.8f, 220, 170, 0.00f },	// FX_SMOKE_BLAST
	{ { 0.86f, 0.89f, 0.93f }, 0.55f, 10, 66,  950,  350,  35, 2.2f, 30,  18, 0.00f },	// FX_SPRAY_WET
	{ { 0.93f, 0.95f, 0.99f }, 0.62f, 10, 70, 1200,  400,  20, 1.9f, 32,  18, 0.00f }	// FX_SPRAY_SNOW
};

typedef struct {
	vec3_t		origin;
	float		radius;
	float		height;
	float		size;
	float		rate;
	float		droplets;
	float		alpha;
	float		range;
	float		push;
	vec3_t		pushDir;		// zero when the spray goes everywhere
	vec3_t		color;
	vec3_t		litColor;
	qboolean	lit;
	float		mistAccum;
	float		dropAccum;
} fxEmitter_t;

typedef struct {
	qboolean	active;
	int			type;
	vec3_t		org;
	vec3_t		vel;
	float		gravity;
	float		drag;
	int			startTime;
	int			endTime;
	float		size0;
	float		size1;
	float		alpha;
	vec3_t		color;
	float		rotation;
	float		rotSpeed;
	float		floorZ;
} fxParticle_t;

static fxEmitter_t	fxEmitters[FX_MAX_EMITTERS];
static int			fxNumEmitters;
static fxParticle_t	fxParticles[FX_MAX_PARTICLES];
static int			fxNextParticle;
static qhandle_t	fxMistShader;
static qhandle_t	fxDropShader;
static qhandle_t	fxSmokeShader;

// kept static, the QVM stack is small
static polyVert_t	fxMistVerts[FX_MAX_PARTICLES * 4];
static polyVert_t	fxDropVerts[FX_MAX_PARTICLES * 4];
static polyVert_t	fxSmokeVerts[FX_MAX_PARTICLES * 4];

static char			fxKeys[FX_MAX_KEYS][2][MAX_TOKEN_CHARS];
static int			fxNumKeys;

/*
=================
CG_FX_Value
=================
*/
static const char *CG_FX_Value( const char *key, const char *defaultValue ) {
	int i;

	for ( i = 0; i < fxNumKeys; i++ ) {
		if ( !Q_stricmp( fxKeys[i][0], key ) ) {
			return fxKeys[i][1];
		}
	}
	return defaultValue;
}

static float CG_FX_Float( const char *key, float defaultValue ) {
	const char *v = CG_FX_Value( key, NULL );

	return v ? atof( v ) : defaultValue;
}

static void CG_FX_Vector( const char *key, const char *defaultValue, vec3_t out ) {
	char	buf[MAX_TOKEN_CHARS];
	char	*p;
	int		i;

	Q_strncpyz( buf, CG_FX_Value( key, defaultValue ), sizeof( buf ) );
	p = buf;
	for ( i = 0; i < 3; i++ ) {
		out[i] = atof( COM_Parse( &p ) );
	}
}

/*
=================
CG_FX_AddSpray
=================
*/
static void CG_FX_AddSpray( void ) {
	fxEmitter_t	*e;
	float		yaw;
	int			lift;

	if ( fxNumEmitters >= FX_MAX_EMITTERS ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: more than %i rally_fx_spray entities\n", FX_MAX_EMITTERS );
		return;
	}

	e = &fxEmitters[fxNumEmitters++];
	memset( e, 0, sizeof( *e ) );

	CG_FX_Vector( "origin", "0 0 0", e->origin );
	e->radius   = Com_Clamp( 4.0f, 1024.0f, CG_FX_Float( "radius", 64.0f ) );
	e->height   = Com_Clamp( 0.0f, 1024.0f, CG_FX_Float( "height", 160.0f ) );
	e->size     = Com_Clamp( 4.0f, 512.0f, CG_FX_Float( "size", 96.0f ) );
	e->rate     = Com_Clamp( 0.0f, 200.0f, CG_FX_Float( "rate", 30.0f ) );
	e->droplets = Com_Clamp( 0.0f, 400.0f, CG_FX_Float( "droplets", 50.0f ) );
	e->alpha    = Com_Clamp( 0.0f, 1.0f, CG_FX_Float( "alpha", 0.4f ) );
	e->range    = Com_Clamp( 256.0f, 16384.0f, CG_FX_Float( "range", 3000.0f ) );
	e->push     = CG_FX_Float( "push", 60.0f );

	yaw = CG_FX_Float( "angle", -1.0f );
	if ( yaw >= 0.0f ) {
		e->pushDir[0] = cos( DEG2RAD( yaw ) );
		e->pushDir[1] = sin( DEG2RAD( yaw ) );
	}

	// an origin inside a rock would break the PVS test and hide the spray
	for ( lift = 0; lift < 128 && ( trap_CM_PointContents( e->origin, 0 ) & CONTENTS_SOLID ); lift += 4 ) {
		e->origin[2] += 4.0f;
	}
	if ( lift ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: rally_fx_spray at %s is inside solid, moved up %i units\n",
			CG_FX_Value( "origin", "" ), lift );
	}

	CG_FX_Vector( "_color", CG_FX_Value( "color", "0.92 0.96 1" ), e->color );
	if ( e->color[0] > 1.0f || e->color[1] > 1.0f || e->color[2] > 1.0f ) {
		// 0..255 notation
		VectorScale( e->color, 1.0f / 255.0f, e->color );
	}
}

/*
=================
CG_FX_ParseEntities

Walks the map's entity string once. Returns when it is exhausted.
=================
*/
static void CG_FX_ParseEntities( void ) {
	char	token[MAX_TOKEN_CHARS];

	fxNumEmitters = 0;

	while ( trap_GetEntityToken( token, sizeof( token ) ) ) {
		if ( token[0] != '{' ) {
			continue;
		}

		fxNumKeys = 0;
		while ( 1 ) {
			if ( !trap_GetEntityToken( token, sizeof( token ) ) || token[0] == '}' ) {
				break;
			}
			if ( fxNumKeys < FX_MAX_KEYS ) {
				Q_strncpyz( fxKeys[fxNumKeys][0], token, sizeof( fxKeys[0][0] ) );
			}
			if ( !trap_GetEntityToken( token, sizeof( token ) ) ) {
				break;
			}
			if ( fxNumKeys < FX_MAX_KEYS ) {
				Q_strncpyz( fxKeys[fxNumKeys][1], token, sizeof( fxKeys[0][1] ) );
				fxNumKeys++;
			}
		}

		if ( !Q_stricmp( CG_FX_Value( "classname", "" ), "rally_fx_spray" ) ) {
			CG_FX_AddSpray();
		} else if ( !Q_stricmp( CG_FX_Value( "classname", "" ), "worldspawn" ) ) {
			// Q3Rally: wet ground without rain (0 = dry, 1 = soaked)
			cgs.mapWetness = Com_Clamp( 0.0f, 1.0f, CG_FX_Float( "wetness", 0.0f ) );
		}
	}
}

/*
=================
CG_InitRallyFX

Called once the world map is loaded.
=================
*/
void CG_InitRallyFX( void ) {
	memset( fxParticles, 0, sizeof( fxParticles ) );
	fxNextParticle = 0;

	CG_FX_ParseEntities();

	fxSmokeShader = trap_R_RegisterShader( "gfx/misc/q3r_smoke" );

	if ( fxNumEmitters ) {
		fxMistShader = trap_R_RegisterShader( "gfx/misc/q3r_mist" );
		fxDropShader = trap_R_RegisterShader( "gfx/misc/q3r_droplet" );
		CG_Printf( "%i map effect emitter%s\n", fxNumEmitters, fxNumEmitters == 1 ? "" : "s" );
	}
}

/*
=================
CG_FX_Alloc
=================
*/
static fxParticle_t *CG_FX_Alloc( void ) {
	int i;

	for ( i = 0; i < FX_MAX_PARTICLES; i++ ) {
		fxParticle_t *p = &fxParticles[( fxNextParticle + i ) % FX_MAX_PARTICLES];

		if ( !p->active ) {
			fxNextParticle = ( fxNextParticle + i + 1 ) % FX_MAX_PARTICLES;
			memset( p, 0, sizeof( *p ) );
			p->active = qtrue;
			return p;
		}
	}
	return NULL;
}

/*
=================
CG_FX_Light

Tints the emitter colour with the light grid once.
=================
*/
static void CG_FX_Light( fxEmitter_t *e ) {
	vec3_t	ambient, directed, dir, probe;
	int		i;

	VectorCopy( e->color, e->litColor );
	VectorCopy( e->origin, probe );
	probe[2] += 16.0f;

	if ( trap_R_LightForPoint( probe, ambient, directed, dir ) ) {
		for ( i = 0; i < 3; i++ ) {
			// spray scatters light, keep it a bit brighter than the rock
			float l = ( ambient[i] + directed[i] * 0.6f ) / 160.0f;

			e->litColor[i] = e->color[i] * Com_Clamp( 0.5f, 1.0f, l );
		}
	}
	e->lit = qtrue;
}

/*
=================
CG_FX_SpawnMist
=================
*/
static void CG_FX_SpawnMist( fxEmitter_t *e ) {
	fxParticle_t	*p;
	float			ang, dist, out, life;
	qboolean		low;

	p = CG_FX_Alloc();
	if ( !p ) {
		return;
	}

	ang  = random() * 2.0f * M_PI;
	dist = sqrt( random() ) * e->radius;
	out  = 15.0f + random() * 35.0f;
	low  = random() < 0.35f;	// a part of the mist hugs the water

	p->type      = FXP_MIST;
	p->size0     = e->size * 0.35f;
	p->size1     = e->size * ( low ? 1.3f : 0.9f ) * ( 0.85f + random() * 0.5f );
	p->org[0] = e->origin[0] + cos( ang ) * dist;
	p->org[1] = e->origin[1] + sin( ang ) * dist;
	// start high enough that the grown puff does not cut into the water
	p->org[2] = e->origin[2] + p->size1 * 0.25f + random() * 16.0f;

	// with drag d the puff travels about v0 / d before it stops
	p->drag = 1.2f;
	p->vel[0] = cos( ang ) * out + e->pushDir[0] * e->push * 0.6f;
	p->vel[1] = sin( ang ) * out + e->pushDir[1] * e->push * 0.6f;
	if ( low ) {
		p->vel[2] = ( 0.05f + random() * 0.15f ) * e->height * p->drag;
	} else {
		p->vel[2] = ( 0.35f + random() * 0.65f ) * e->height * p->drag;
	}

	life = low ? 2600.0f + random() * 1800.0f : 1600.0f + random() * 1400.0f;
	p->startTime = cg.time;
	p->endTime   = cg.time + (int)life;
	p->alpha     = e->alpha * ( 0.6f + random() * 0.4f );
	p->rotation  = random() * 360.0f;
	p->rotSpeed  = crandom() * 30.0f;
	p->floorZ    = -1.0e9f;	// mist never hits a floor
	VectorCopy( e->litColor, p->color );
}

/*
=================
CG_FX_SpawnDroplet
=================
*/
static void CG_FX_SpawnDroplet( fxEmitter_t *e ) {
	fxParticle_t	*p;
	float			ang, dist, out, up;

	p = CG_FX_Alloc();
	if ( !p ) {
		return;
	}

	ang  = random() * 2.0f * M_PI;
	dist = random() * e->radius * 0.6f;
	out  = 40.0f + random() * 120.0f;
	up   = ( 120.0f + random() * 220.0f ) * ( 0.4f + e->height / 266.0f );

	p->type = FXP_DROPLET;
	p->org[0] = e->origin[0] + cos( ang ) * dist;
	p->org[1] = e->origin[1] + sin( ang ) * dist;
	p->org[2] = e->origin[2] + 4.0f;
	p->vel[0] = cos( ang ) * out + e->pushDir[0] * e->push;
	p->vel[1] = sin( ang ) * out + e->pushDir[1] * e->push;
	p->vel[2] = up;
	p->gravity   = 600.0f;
	p->drag      = 0.3f;
	p->startTime = cg.time;
	p->endTime   = cg.time + 900 + (int)( random() * 600.0f );
	p->size0     = 2.0f + random() * 3.0f;
	p->size1     = p->size0;
	p->alpha     = 0.9f;
	p->floorZ    = e->origin[2] - 4.0f;
	VectorMA( e->litColor, 0.25f, colorWhite, p->color );
}

/*
=================
CG_FX_TireSmoke

One puff of tyre smoke or dust (kind FX_SMOKE_*) at origin, the contact
point of the tyre; up is the ground normal, carVel the car's velocity.
strength scales the size and opacity (1 = normal).
=================
*/
static void CG_FX_SpawnPuff( const vec3_t origin, const vec3_t up, const vec3_t carVel, const vec3_t throwVel, int kind, float strength );

void CG_FX_TireSmoke( const vec3_t origin, const vec3_t up, const vec3_t carVel, int kind, float strength ) {
	CG_FX_SpawnPuff( origin, up, carVel, vec3_origin, kind, strength );
}

/*
=================
CG_FX_Spray

Like CG_FX_TireSmoke, but the puff is thrown with velocity throwVel (tyre
spray, snow dust).
=================
*/
void CG_FX_Spray( const vec3_t origin, const vec3_t up, const vec3_t throwVel, int kind, float strength ) {
	CG_FX_SpawnPuff( origin, up, vec3_origin, throwVel, kind, strength );
}

static void CG_FX_SpawnPuff( const vec3_t origin, const vec3_t up, const vec3_t carVel, const vec3_t throwVel, int kind, float strength ) {
	const fxSmokeKind_t	*k;
	fxParticle_t		*p;
	vec3_t				ambient, directed, dir, probe;
	float				density, scale, ang;
	int					i;

	if ( kind < 0 || kind >= FX_SMOKE_NUM_KINDS || !fxSmokeShader ) {
		return;
	}
	// the smoke screen is a weapon, it must not depend on cg_tireSmoke
	density = ( kind == FX_SMOKE_SCREEN || kind == FX_SMOKE_BLAST ) ? 1.0f : Com_Clamp( 0.0f, 2.0f, cg_tireSmoke.value );
	if ( density <= 0.0f ) {
		return;
	}
	// skip far away puffs
	if ( DistanceSquared( origin, cg.refdef.vieworg ) > ( kind == FX_SMOKE_SCREEN ? 8000.0f * 8000.0f : 4000.0f * 4000.0f ) ) {
		return;
	}
	if ( density < 1.0f && random() > density ) {
		return;
	}

	p = CG_FX_Alloc();
	if ( !p ) {
		return;
	}
	k = &fxSmokeKinds[kind];
	scale = Com_Clamp( 0.3f, 2.0f, strength ) * ( density > 1.0f ? 1.0f + ( density - 1.0f ) * 0.4f : 1.0f );

	p->type = FXP_SMOKE;
	VectorMA( origin, k->size0 * 0.5f + 4.0f, up, p->org );

	ang = random() * 2.0f * M_PI;
	for ( i = 0; i < 3; i++ ) {
		p->vel[i] = carVel[i] * k->carShare + throwVel[i] * ( 0.8f + random() * 0.4f ) + up[i] * k->upSpeed * ( 0.6f + random() * 0.8f );
	}
	p->vel[0] += cos( ang ) * k->spread * random();
	p->vel[1] += sin( ang ) * k->spread * random();

	p->gravity   = k->gravity;
	p->drag      = k->drag;
	p->startTime = cg.time;
	p->endTime   = cg.time + k->life + (int)( random() * k->lifeRand );
	p->size0     = k->size0 * scale;
	p->size1     = k->size1 * scale * ( 0.8f + random() * 0.4f );
	p->alpha     = k->alpha * Com_Clamp( 0.3f, 1.0f, strength ) * ( 0.75f + random() * 0.25f );
	p->rotation  = random() * 360.0f;
	p->rotSpeed  = crandom() * 40.0f;
	p->floorZ    = -1.0e9f;

	// light grid at the start, so the smoke is not glowing in the dark
	VectorCopy( k->color, p->color );
	VectorMA( origin, 24.0f, up, probe );
	if ( trap_R_LightForPoint( probe, ambient, directed, dir ) ) {
		// spray and snow dust scatter the light, they stay brighter
		float minLight = ( kind == FX_SPRAY_WET || kind == FX_SPRAY_SNOW ) ? 0.65f : 0.3f;

		for ( i = 0; i < 3; i++ ) {
			float l = ( ambient[i] + directed[i] * 0.6f ) / 170.0f;

			p->color[i] = k->color[i] * Com_Clamp( minLight, 1.1f, l );
		}
	}
}

/*
=================
CG_FX_Quad
=================
*/
static void CG_FX_Quad( polyVert_t *v, const vec3_t org, float size, float rotation,
						const vec3_t color, float alpha ) {
	vec3_t	right, up;
	float	s, c;
	byte	rgba[4];
	int		i;

	c = cos( DEG2RAD( rotation ) ) * size;
	s = sin( DEG2RAD( rotation ) ) * size;
	for ( i = 0; i < 3; i++ ) {
		right[i] = cg.refdef.viewaxis[1][i] * c + cg.refdef.viewaxis[2][i] * s;
		up[i]    = cg.refdef.viewaxis[2][i] * c - cg.refdef.viewaxis[1][i] * s;
	}

	rgba[0] = (byte)( Com_Clamp( 0.0f, 1.0f, color[0] ) * 255.0f );
	rgba[1] = (byte)( Com_Clamp( 0.0f, 1.0f, color[1] ) * 255.0f );
	rgba[2] = (byte)( Com_Clamp( 0.0f, 1.0f, color[2] ) * 255.0f );
	rgba[3] = (byte)( Com_Clamp( 0.0f, 1.0f, alpha ) * 255.0f );

	for ( i = 0; i < 3; i++ ) {
		v[0].xyz[i] = org[i] + right[i] + up[i];
		v[1].xyz[i] = org[i] - right[i] + up[i];
		v[2].xyz[i] = org[i] - right[i] - up[i];
		v[3].xyz[i] = org[i] + right[i] - up[i];
	}
	v[0].st[0] = 0; v[0].st[1] = 0;
	v[1].st[0] = 1; v[1].st[1] = 0;
	v[2].st[0] = 1; v[2].st[1] = 1;
	v[3].st[0] = 0; v[3].st[1] = 1;
	for ( i = 0; i < 4; i++ ) {
		v[i].modulate[0] = rgba[0];
		v[i].modulate[1] = rgba[1];
		v[i].modulate[2] = rgba[2];
		v[i].modulate[3] = rgba[3];
	}
}

/*
=================
CG_AddRallyFX

Spawns, moves and draws the map effect particles. Called every frame.
=================
*/
void CG_AddRallyFX( void ) {
	float	dt, density;
	int		i, n, numMist, numDrop, numSmoke;

	density = Com_Clamp( 0.0f, 2.0f, cg_mapEffects.value );
	dt = Com_Clamp( 0.0f, 0.1f, cg.frametime * 0.001f );

	// spawn
	for ( i = 0; i < fxNumEmitters && density > 0.0f; i++ ) {
		fxEmitter_t	*e = &fxEmitters[i];
		float		dist, lod;

		dist = Distance( cg.refdef.vieworg, e->origin );
		if ( dist > e->range || !trap_R_inPVS( cg.refdef.vieworg, e->origin ) ) {
			e->mistAccum = e->dropAccum = 0.0f;
			continue;
		}
		if ( !e->lit ) {
			CG_FX_Light( e );
		}

		// thinner when far away, nobody sees single droplets at 2000 units
		lod = 1.0f;
		if ( dist > e->range * 0.4f ) {
			lod = 1.0f - 0.7f * ( dist - e->range * 0.4f ) / ( e->range * 0.6f );
		}

		e->mistAccum += e->rate * density * lod * dt;
		e->dropAccum += e->droplets * density * lod * lod * dt;
		for ( n = 0; e->mistAccum >= 1.0f && n < FX_MAX_SPAWN_PER_FRAME; n++ ) {
			CG_FX_SpawnMist( e );
			e->mistAccum -= 1.0f;
		}
		for ( n = 0; e->dropAccum >= 1.0f && n < FX_MAX_SPAWN_PER_FRAME; n++ ) {
			CG_FX_SpawnDroplet( e );
			e->dropAccum -= 1.0f;
		}
		// do not save up particles when the frame budget is exceeded
		if ( e->mistAccum > 1.0f ) e->mistAccum = 1.0f;
		if ( e->dropAccum > 1.0f ) e->dropAccum = 1.0f;
	}

	// move and draw
	numMist = numDrop = numSmoke = 0;
	for ( i = 0; i < FX_MAX_PARTICLES; i++ ) {
		fxParticle_t	*p = &fxParticles[i];
		float			t, a, size, drag;

		if ( !p->active ) {
			continue;
		}
		if ( cg.time >= p->endTime || cg.time < p->startTime ) {
			p->active = qfalse;
			continue;
		}

		drag = 1.0f - p->drag * dt;
		if ( drag < 0.0f ) {
			drag = 0.0f;
		}
		VectorScale( p->vel, drag, p->vel );
		p->vel[2] -= p->gravity * dt;
		VectorMA( p->org, dt, p->vel, p->org );

		if ( p->org[2] < p->floorZ && p->vel[2] < 0.0f ) {
			p->active = qfalse;		// droplet fell back into the water
			continue;
		}

		t = (float)( cg.time - p->startTime ) / (float)( p->endTime - p->startTime );
		a = p->alpha * ( 1.0f - t );
		if ( p->type == FXP_SMOKE ) {
			// quick to appear, slow to thin out
			a = p->alpha * ( 1.0f - t ) * ( 1.0f - t );
			if ( t < 0.06f ) {
				a *= t / 0.06f;
			}
		} else if ( t < 0.15f ) {
			a *= t / 0.15f;
		}
		size = p->size0 + ( p->size1 - p->size0 ) * ( 1.0f - ( 1.0f - t ) * ( 1.0f - t ) );
		p->rotation += p->rotSpeed * dt;

		if ( p->type == FXP_SMOKE ) {
			// slows down as it grows
			p->rotSpeed *= 1.0f - 0.5f * dt;
			CG_FX_Quad( &fxSmokeVerts[numSmoke * 4], p->org, size, p->rotation, p->color, a );
			numSmoke++;
		} else if ( p->type == FXP_MIST ) {
			CG_FX_Quad( &fxMistVerts[numMist * 4], p->org, size, p->rotation, p->color, a );
			numMist++;
		} else {
			CG_FX_Quad( &fxDropVerts[numDrop * 4], p->org, size, 0.0f, p->color, a );
			numDrop++;
		}
	}

	if ( numMist ) {
		trap_R_AddPolysToScene( fxMistShader, 4, fxMistVerts, numMist );
	}
	if ( numDrop ) {
		trap_R_AddPolysToScene( fxDropShader, 4, fxDropVerts, numDrop );
	}
	if ( numSmoke ) {
		trap_R_AddPolysToScene( fxSmokeShader, 4, fxSmokeVerts, numSmoke );
	}
}
