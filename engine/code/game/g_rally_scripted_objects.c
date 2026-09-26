/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
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

#include "g_local.h"
#include "../qcommon/qfiles.h"

#define		MAX_SCRIPT_TEXT		8192
#define SCRIPTED_OBJECT_PHYSICS_STEP_MSEC	10
#define SCRIPTED_OBJECT_MAX_SUBSTEPS		10
#define SCRIPTED_OBJECT_MAX_FRAME_MSEC		100
#define RALLY_MAX_HULL_MODELS			32
#define RALLY_MAX_HULL_VERTS			2048
#define RALLY_MAX_HULL_CACHE_VERTS		8192
#define MD3_HEADER_BYTES				108
#define MD3_SURFACE_HEADER_BYTES		108
#define MD3_VERTEX_BYTES				8

/* collision types */
#define		CT_BOX			0
#define		CT_CONE			1
#define		CT_CYLINDER		2

/* Global temporary force vector for testing */
vec3_t		tempForce;

typedef struct {
	char model[MAX_QPATH * 2];
	vec3_t *vertices;
	int numVertices;
	vec3_t mins;
	vec3_t maxs;
} rallyObjectHullCache_t;

static rallyObjectHullCache_t rallyObjectHullCache[RALLY_MAX_HULL_MODELS];
static vec3_t rallyObjectHullVertexPool[RALLY_MAX_HULL_CACHE_VERTS];
static int rallyObjectHullCacheCount;
static int rallyObjectHullVertexCount;
static byte rallyMD3SurfaceVertexData[MD3_MAX_VERTS * MD3_VERTEX_BYTES];
static vmCvar_t g_scriptedObjectBullet;
static qboolean rallyBulletPhysicsActive;

qboolean G_RallyPhysics_Enabled( void ) {
	return rallyBulletPhysicsActive;
}

void G_RallyPhysics_Init( void ) {
	trap_Cvar_Register( &g_scriptedObjectBullet, "g_scriptedObjectBullet", "1",
		CVAR_ARCHIVE | CVAR_LATCH );
	trap_Cvar_Update( &g_scriptedObjectBullet );
	rallyBulletPhysicsActive = g_scriptedObjectBullet.integer ? qtrue : qfalse;
	if ( !rallyBulletPhysicsActive ) {
		Com_Printf( "rally_scripted_object: using legacy physics solver\n" );
		return;
	}
	trap_RallyPhysicsInit( g_gravity.value );
	Com_Printf( "rally_scripted_object: Bullet backend enabled; set g_scriptedObjectBullet 0 and restart the map to compare\n" );
}

void G_RallyPhysics_Shutdown( void ) {
	if ( rallyBulletPhysicsActive )
		trap_RallyPhysicsShutdown();
	rallyBulletPhysicsActive = qfalse;
}

static qboolean G_RallyPhysics_CreateEntity( gentity_t *ent ) {
	rallyPhysicsBodyDesc_t desc;
	const vec3_t *vertices = NULL;
	int numVertices = 0;
	if ( !rallyBulletPhysicsActive || !ent )
		return qfalse;
	memset( &desc, 0, sizeof( desc ) );
	VectorCopy( ent->s.pos.trBase, desc.origin );
	VectorCopy( ent->s.apos.trBase, desc.angles );
	if ( ent->collisionHullVerts && ent->collisionHullVertCount >= 4 ) {
		VectorCopy( ent->collisionHullMins, desc.mins );
		VectorCopy( ent->collisionHullMaxs, desc.maxs );
		vertices = (const vec3_t *)ent->collisionHullVerts;
		numVertices = ent->collisionHullVertCount;
	} else {
		VectorCopy( ent->r.mins, desc.mins );
		VectorCopy( ent->r.maxs, desc.maxs );
	}
	desc.mass = ent->moveable ? (float)ent->mass : 0.0f;
	desc.restitution = ent->elasticity;
	desc.friction = ent->friction;
	desc.rollingFriction = ent->rollingFriction;
	desc.spinningFriction = ent->spinningFriction;
	desc.linearDamping = 0.025f;
	desc.angularDamping = 0.08f;
	desc.contents = ent->r.contents;
	return trap_RallyPhysicsCreateBody( ent->s.number, &desc, vertices, numVertices );
}

void G_RallyPhysics_RunFrame( void ) {
	int i, elapsedMsec;
	if ( !rallyBulletPhysicsActive )
		return;
	elapsedMsec = level.time - level.previousTime;
	if ( elapsedMsec < 0 ) elapsedMsec = 0;
	if ( elapsedMsec > 100 ) elapsedMsec = 100;
	trap_RallyPhysicsStep( elapsedMsec * 0.001f );
	for ( i = MAX_CLIENTS; i < level.num_entities; i++ ) {
		gentity_t *ent = &g_entities[i];
		rallyPhysicsBodyState_t state;
		if ( !ent->inuse || ent->s.eType != ET_SCRIPTED || !ent->moveable )
			continue;
		if ( !trap_RallyPhysicsGetBodyState( ent->s.number, &state ) )
			continue;
		VectorCopy( state.origin, ent->s.pos.trBase );
		VectorCopy( state.origin, ent->s.origin );
		VectorCopy( state.origin, ent->r.currentOrigin );
		VectorCopy( state.angles, ent->s.apos.trBase );
		VectorCopy( state.angles, ent->s.angles );
		VectorCopy( state.angles, ent->r.currentAngles );
		VectorCopy( state.linearVelocity, ent->s.pos.trDelta );
		VectorScale( state.angularVelocity, 57.2957795f, ent->s.apos.trDelta );
		ent->s.pos.trTime = level.time;
		ent->s.apos.trTime = level.time;
		ent->physicsSleeping = state.sleeping;
		trap_LinkEntity( ent );
	}
}

static int G_ReadMD3Int( const byte *data ) {
	return (int)( (unsigned int)data[0] |
		( (unsigned int)data[1] << 8 ) |
		( (unsigned int)data[2] << 16 ) |
		( (unsigned int)data[3] << 24 ) );
}

static int G_ReadMD3Short( const byte *data ) {
	int value;

	value = (int)data[0] | ( (int)data[1] << 8 );
	return ( value & 0x8000 ) ? value - 0x10000 : value;
}

void G_RallyObject_ResetCollisionCache( void ) {
	Com_Memset( rallyObjectHullCache, 0, sizeof( rallyObjectHullCache ) );
	rallyObjectHullCacheCount = 0;
	rallyObjectHullVertexCount = 0;
}

static qboolean G_LoadMD3CollisionHull( const char *model, rallyObjectHullCache_t *cache ) {
	byte header[MD3_HEADER_BYTES];
	byte surfaceHeader[MD3_SURFACE_HEADER_BYTES];
	int surfaceOffsets[MD3_MAX_SURFACES];
	int surfaceVertexOffsets[MD3_MAX_SURFACES];
	int surfaceVertexCounts[MD3_MAX_SURFACES];
	int fileLength;
	int fileEnd;
	int surfaceOffset;
	int numSurfaces;
	int numFrames;
	int rawVertexCount;
	int i, j;
	int numVertices;
	fileHandle_t file;
	vec3_t *vertices;
	vec3_t point;
	qboolean unique;

	fileLength = trap_FS_FOpenFile( model, &file, FS_READ );
	if ( !file )
		return qfalse;
	if ( fileLength < MD3_HEADER_BYTES ) {
		trap_FS_FCloseFile( file );
		return qfalse;
	}

	trap_FS_Read( header, sizeof( header ), file );
	if ( G_ReadMD3Int( header ) != MD3_IDENT || G_ReadMD3Int( header + 4 ) != MD3_VERSION ) {
		trap_FS_FCloseFile( file );
		return qfalse;
	}

	numFrames = G_ReadMD3Int( header + 76 );
	numSurfaces = G_ReadMD3Int( header + 84 );
	surfaceOffset = G_ReadMD3Int( header + 100 );
	fileEnd = G_ReadMD3Int( header + 104 );
	if ( numFrames < 1 || numFrames > MD3_MAX_FRAMES ||
		numSurfaces < 1 || numSurfaces > MD3_MAX_SURFACES ||
		fileEnd < MD3_HEADER_BYTES || fileEnd > fileLength ||
		surfaceOffset < MD3_HEADER_BYTES || surfaceOffset > fileEnd - MD3_SURFACE_HEADER_BYTES ) {
		trap_FS_FCloseFile( file );
		return qfalse;
	}

	rawVertexCount = 0;
	for ( i = 0; i < numSurfaces; i++ ) {
		int surfaceEnd;
		int surfaceFrames;
		int vertexCount;
		int vertexOffset;
		int xyzBytes;

		if ( surfaceOffset < 0 || surfaceOffset > fileEnd - MD3_SURFACE_HEADER_BYTES ||
			trap_FS_Seek( file, surfaceOffset, FS_SEEK_SET ) != 0 ) {
			trap_FS_FCloseFile( file );
			return qfalse;
		}
		trap_FS_Read( surfaceHeader, sizeof( surfaceHeader ), file );
		if ( G_ReadMD3Int( surfaceHeader ) != MD3_IDENT ) {
			trap_FS_FCloseFile( file );
			return qfalse;
		}

		surfaceFrames = G_ReadMD3Int( surfaceHeader + 72 );
		vertexCount = G_ReadMD3Int( surfaceHeader + 80 );
		vertexOffset = G_ReadMD3Int( surfaceHeader + 100 );
		surfaceEnd = G_ReadMD3Int( surfaceHeader + 104 );
		if ( surfaceFrames != numFrames || surfaceFrames < 1 || surfaceFrames > MD3_MAX_FRAMES ||
			vertexCount < 0 || vertexCount > MD3_MAX_VERTS ||
			vertexOffset < MD3_SURFACE_HEADER_BYTES || surfaceEnd < MD3_SURFACE_HEADER_BYTES ||
		surfaceOffset > fileEnd - surfaceEnd ) {
			trap_FS_FCloseFile( file );
			return qfalse;
		}
		xyzBytes = vertexCount * surfaceFrames * MD3_VERTEX_BYTES;
		if ( xyzBytes < 0 || vertexOffset > surfaceEnd || xyzBytes > surfaceEnd - vertexOffset ||
			rawVertexCount > RALLY_MAX_HULL_VERTS - vertexCount ) {
			trap_FS_FCloseFile( file );
			return qfalse;
		}

		surfaceOffsets[i] = surfaceOffset;
		surfaceVertexOffsets[i] = vertexOffset;
		surfaceVertexCounts[i] = vertexCount;
		rawVertexCount += vertexCount;
		surfaceOffset += surfaceEnd;
	}

	if ( rawVertexCount < 4 ) {
		trap_FS_FCloseFile( file );
		return qfalse;
	}

	if ( rawVertexCount < 1 || rallyObjectHullVertexCount >
		RALLY_MAX_HULL_CACHE_VERTS - rawVertexCount ) {
		trap_FS_FCloseFile( file );
		return qfalse;
	}
	vertices = rallyObjectHullVertexPool + rallyObjectHullVertexCount;
	numVertices = 0;
	ClearBounds( cache->mins, cache->maxs );

	for ( i = 0; i < numSurfaces; i++ ) {
		int count;

		count = surfaceVertexCounts[i];
		if ( !count )
			continue;
		if ( trap_FS_Seek( file, surfaceOffsets[i] + surfaceVertexOffsets[i], FS_SEEK_SET ) != 0 ) {
			trap_FS_FCloseFile( file );
			return qfalse;
		}
		trap_FS_Read( rallyMD3SurfaceVertexData, count * MD3_VERTEX_BYTES, file );
		for ( j = 0; j < count; j++ ) {
			int k;

			VectorSet( point,
				G_ReadMD3Short( rallyMD3SurfaceVertexData + j * MD3_VERTEX_BYTES ) * MD3_XYZ_SCALE,
				G_ReadMD3Short( rallyMD3SurfaceVertexData + j * MD3_VERTEX_BYTES + 2 ) * MD3_XYZ_SCALE,
				G_ReadMD3Short( rallyMD3SurfaceVertexData + j * MD3_VERTEX_BYTES + 4 ) * MD3_XYZ_SCALE );
			unique = qtrue;
			for ( k = 0; k < numVertices; k++ ) {
				if ( point[0] == vertices[k][0] && point[1] == vertices[k][1] && point[2] == vertices[k][2] ) {
					unique = qfalse;
					break;
				}
			}
			if ( !unique )
				continue;
			VectorCopy( point, vertices[numVertices++] );
			AddPointToBounds( point, cache->mins, cache->maxs );
		}
	}
	trap_FS_FCloseFile( file );

	if ( numVertices < 4 )
		return qfalse;
	cache->vertices = vertices;
	cache->numVertices = numVertices;
	rallyObjectHullVertexCount += rawVertexCount;
	return qtrue;
}

static qboolean G_RallyObject_LoadCollisionHull( gentity_t *ent ) {
	char model[MAX_QPATH * 2];
	char *extension;
	rallyObjectHullCache_t *cache;
	int i;

	if ( !ent || !ent->model || !ent->model[0] )
		return qfalse;
	Q_strncpyz( model, ent->model, sizeof( model ) );
	extension = strrchr( model, '.' );
	if ( !extension )
		Q_strcat( model, sizeof( model ), ".md3" );
	else if ( Q_stricmp( extension, ".md3" ) )
		return qfalse;

	for ( i = 0; i < rallyObjectHullCacheCount; i++ ) {
		if ( !Q_stricmp( rallyObjectHullCache[i].model, model ) ) {
			cache = &rallyObjectHullCache[i];
			goto assignHull;
		}
	}
	if ( rallyObjectHullCacheCount >= RALLY_MAX_HULL_MODELS )
		return qfalse;

	cache = &rallyObjectHullCache[rallyObjectHullCacheCount++];
	Q_strncpyz( cache->model, model, sizeof( cache->model ) );
	if ( !G_LoadMD3CollisionHull( model, cache ) )
		return qfalse;
	if ( g_developer.integer ) {
		Com_Printf( "rally_scripted_object: loaded %d convex collision vertices from %s\n",
			cache->numVertices, model );
	}

assignHull:
	if ( !cache->vertices || cache->numVertices < 4 )
		return qfalse;
	ent->collisionHullVerts = cache->vertices;
	ent->collisionHullVertCount = cache->numVertices;
	VectorCopy( cache->mins, ent->collisionHullMins );
	VectorCopy( cache->maxs, ent->collisionHullMaxs );
	return qtrue;
}

static qboolean G_ParseScriptVector( char **text_p, vec3_t value ) {
	char *token;
	int parsed;
	int i;

	token = COM_Parse( text_p );
	if ( !token || !token[0] )
		return qfalse;

	parsed = sscanf( token, "%f %f %f", &value[0], &value[1], &value[2] );
	if ( parsed == 3 )
		return qtrue;

	value[0] = atof( token );
	for ( i = 1; i < 3; i++ ) {
		token = COM_Parse( text_p );
		if ( !token || !token[0] )
			return qfalse;
		value[i] = atof( token );
	}
	return qtrue;
}

qboolean SeekToSection( char **pointer, char *str ){
	char		*token;

	/* UPDATE: using strstr instead? */
	/* UPDATE: check if end of file is inside of a bracket (ie bad brackets in script file) */

	/* seek to 'str {' */
	while ( 1 ) {
		token = COM_Parse( pointer );

		if( !token || token[0] == 0 )
			return qfalse;

		if ( !Q_stricmp( token, "{" ) ){
			/* loop through this */
			while ( 1 ) {
				token = COM_Parse( pointer );

				if( !token || token[0] == 0 )
					return qfalse;

				if ( !Q_stricmp( token, "}" ) )
					break;
			}
		}

		if ( !Q_stricmp( token, str ) )
			break;
	}

	if( !token || token[0] == 0 ) /* model not found */
		return qfalse;

	return qtrue;
}

qboolean G_ParseScriptedObject( gentity_t *ent ){
	char		*text_p;
	int			len;
	char		*token;
	char		text[MAX_SCRIPT_TEXT];
	char		filename[MAX_QPATH];
	fileHandle_t	f;

	/* setup defaults */
	ent->takedamage = qfalse;
	VectorSet( ent->r.mins, -16.0f, -16.0f, -16.0f );
	VectorSet( ent->r.maxs,  16.0f,  16.0f,  16.0f );
	ent->elasticity = 0.1f;
	ent->friction = 0.6f;
	ent->rollingFriction = 0.1f;
	ent->spinningFriction = 0.05f;
	ent->vehicleImpactScale = 0.5f;
	ent->weaponImpactScale = 1.0f;
	ent->mass = 100;
	ent->inertiaShape = RALLY_OBJECT_INERTIA_BOX;
	ent->moveable = qfalse;
	ent->number = 0;
	ent->health = 0;
	ent->maxHealth = 0;
	ent->s.modelindex = 0;

	if (!ent->script || ent->script[0] == 0){
		/* A direct model key is enough for a basic map-authored physics prop. */
		return ( ent->model && ent->model[0] ) ? qtrue : qfalse;
	}

	/* for debugging only load one object */
	/* if( Q_stricmp( ent->script, "models/mapobjects/barrels/barrel01" ) ) */
	/*	return qfalse; */

	Q_strncpyz(filename, ent->script, sizeof(filename));
	token = strchr(filename, '.');
	if (!token)
		Q_strcat(filename, sizeof(filename), ".script");

	if (g_developer.integer)
		Com_Printf("Attempting to load script %s\n", filename);

	/* load the file */
	len = trap_FS_FOpenFile( filename, &f, FS_READ );

	if ( !f ){
		Com_Printf("Could not find script %s\n", filename);
		return qfalse;
	}

	if ( len >= MAX_SCRIPT_TEXT ) {
		len = MAX_SCRIPT_TEXT - 1;
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;

	trap_FS_FCloseFile( f );

	/* parse the text */
	text_p = text;

	/* seek to "rally_scripted_object {" */
	if ( !SeekToSection( &text_p, "rally_scripted_object" ) ){
		Com_Printf( "Script file '%s' did not contain rally_scripted_object\n", filename );
		return qfalse;
	}

	/* send script file name in CS so we dont need */
	/* to do all of the drawing and stuff server side. */
	ent->s.modelindex = G_ScriptIndex( ent->script );

	/* read optional parameters */
	while ( 1 ) {
		token = COM_Parse( &text_p );

		if( !token || token[0] == 0 || !Q_stricmp( token, "}" ) ) {
			break;
		}

		if ( !Q_stricmp( token, "{" ) )
			continue;

		if (g_developer.integer)
			Com_Printf("Found token: %s\n", token);

		if ( !Q_stricmp( token, "type" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			ent->s.weapon = atoi(token);

			continue;
		}
		else if ( !Q_stricmp( token, "model" ) ){
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "deadmodel" ) ){
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "moveable" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			ent->moveable = atoi(token);

			continue;
		}
		else if ( !Q_stricmp( token, "elasticity" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			ent->elasticity = atof(token);

			continue;
		}
		else if ( !Q_stricmp( token, "mass" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			ent->mass = atoi(token);
			if (ent->mass <= 0)
				ent->mass = 100;

			continue;
		}
		else if ( !Q_stricmp( token, "frames" ) ){
			COM_Parse( &text_p );
			COM_Parse( &text_p );
			COM_Parse( &text_p );
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "health" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			ent->maxHealth = ent->health = atoi(token);
			ent->takedamage = ( ent->health > 0 ) ? qtrue : qfalse;

			continue;
		}
		else if ( !Q_stricmp( token, "mins" ) ){
			if ( !G_ParseScriptVector( &text_p, ent->r.mins ) )
				return qfalse;

			continue;
		}
		else if ( !Q_stricmp( token, "maxs" ) ){
			if ( !G_ParseScriptVector( &text_p, ent->r.maxs ) )
				return qfalse;

			continue;
		}
		else if ( !Q_stricmp( token, "friction" ) ){
			token = COM_Parse( &text_p );
			if ( !token )
				break;
			ent->friction = atof( token );
			continue;
		}
		else if ( !Q_stricmp( token, "hitsound" ) ){
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "presound" ) ){
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "postsound" ) ){
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "destroysound" ) ){
			COM_Parse( &text_p );
		}
		else if ( !Q_stricmp( token, "gibs" ) ) {
			/* skip gibs part of script (it is only used client side) */
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			if ( !Q_stricmp( token, "{" ) ){
				/* loop through this */
				while ( 1 ) {
					token = COM_Parse( &text_p );

					if( !token || token[0] == 0 )
						return qfalse;

					if ( !Q_stricmp( token, "}" ) )
						break;
				}
			}

			continue;
		}
		else {
			Com_Printf("Warning: Skipping unknown token %s in %s\n", token, filename);
			continue;
		}
	}

	if (g_developer.integer)
		Com_Printf("Successfully parsed script file\n");

	return qtrue;
}

void G_ScriptedObject_Destroy( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ){
	if (g_developer.integer) Com_Printf("Destroying scripted map object %s\n", self->classname);
	if ( G_RallyPhysics_Enabled() )
		trap_RallyPhysicsRemoveBody( self->s.number );

	self->s.eFlags |= EF_DEAD;
	self->takedamage = qfalse;
	self->moveable = qfalse;
	self->r.contents = 0;
	self->think = NULL;
	self->nextthink = 0;
	trap_LinkEntity( self );
}

static float G_ScriptedObject_VehicleContactInverseMass( gentity_t *vehicle ) {
	carBody_t *body;

	if ( !vehicle || !vehicle->client )
		return 0.0f;

	body = &vehicle->client->car.sBody;
	if ( body->mass <= 0.0f )
		return 0.0f;

	/* Pmove has already resolved the car's point contact. Use only the
	 * translational mass here; applying a second rotational response from the
	 * post-Pmove chassis state can turn the prop reaction into a lethal kick. */
	return 1.0f / body->mass;
}

static void G_ScriptedObject_ApplyVehicleCounterImpulse( gentity_t *vehicle,
		const vec3_t objectImpulse, vec3_t appliedDeltaVelocity ) {
	car_t *car;
	carBody_t *body;
	carBody_t *targetBody;
	int i;

	VectorClear( appliedDeltaVelocity );
	if ( !vehicle || !vehicle->client || VectorLengthSquared( objectImpulse ) <= 1e-6f )
		return;

	car = &vehicle->client->car;
	body = &car->sBody;
	targetBody = &car->tBody;
	if ( body->mass <= 0.0f )
		return;

	/* The chassis has already completed Pmove's contact solve. Apply only the
	 * small equal-and-opposite linear impulse; do not reconstruct center speed
	 * from a swept point or inject a second angular impulse after the solve. */
	VectorScale( objectImpulse, -1.0f / body->mass, appliedDeltaVelocity );
	VectorAdd( body->v, appliedDeltaVelocity, body->v );
	VectorAdd( targetBody->v, appliedDeltaVelocity, targetBody->v );
	for ( i = 0; i < NUM_CAR_POINTS; i++ ) {
		VectorAdd( car->sPoints[i].v, appliedDeltaVelocity, car->sPoints[i].v );
		VectorAdd( car->tPoints[i].v, appliedDeltaVelocity, car->tPoints[i].v );
	}
	VectorAdd( vehicle->client->ps.velocity, appliedDeltaVelocity,
		vehicle->client->ps.velocity );
	VectorCopy( body->v, vehicle->s.pos.trDelta );
}

void G_ScriptedObject_TouchWithVelocity ( gentity_t *self, gentity_t *other, trace_t *trace,
		vec3_t vehicleVelocity ) {
	vec3_t		relativeVelocity;
	vec3_t		objectVelocityBefore;
	vec3_t		objectAngularBefore;
	vec3_t	outwardNormal;
	vec3_t	contactPoint;
	float		closingSpeed;
	float		normalImpulseMagnitude;
	float		vehicleInverseMass;
	vec3_t	carVelocityAfter;
	vec3_t	carDeltaVelocity;
	vec3_t	objectImpulse;

	if ( !self->moveable || !other || !trace )
		return;
	G_RallyObject_Wake( self );

	/* ClientImpacts builds this from contact toward the touched prop; it is the
	 * outward direction in which the car should impart momentum. */
	VectorCopy( trace->plane.normal, outwardNormal );
	if ( VectorNormalize( outwardNormal ) == 0.0f )
		return;
	G_RallyObject_SupportContact( self, self->s.pos.trBase, outwardNormal, contactPoint );
	vehicleInverseMass = G_ScriptedObject_VehicleContactInverseMass( other );
	VectorSubtract( self->s.pos.trDelta, vehicleVelocity, relativeVelocity );
	closingSpeed = DotProduct( relativeVelocity, outwardNormal );
	if ( G_RallyPhysics_Enabled() ) {
		VectorClear( objectImpulse );
		if ( vehicleInverseMass > 0.0f ) {
			trap_RallyPhysicsVehicleContact( self->s.number, contactPoint, outwardNormal,
				vehicleVelocity, 1.0f / vehicleInverseMass, self->vehicleImpactScale,
				objectImpulse );
		}
		G_ScriptedObject_ApplyVehicleCounterImpulse( other, objectImpulse,
			carDeltaVelocity );
		if ( other->client )
			VectorCopy( other->client->car.sBody.v, carVelocityAfter );
		else
			VectorCopy( vehicleVelocity, carVelocityAfter );
		if ( g_developer.integer && level.time >= self->scriptedDebugImpactLogTime ) {
			Com_Printf( "rally_bullet: impact ent=%d car=%d close=%.2f J=(%.2f %.2f %.2f) carDv=(%.2f %.2f %.2f) point=(%.1f %.1f %.1f) normal=(%.2f %.2f %.2f)\n",
				self->s.number, other->s.number, closingSpeed,
				objectImpulse[0], objectImpulse[1], objectImpulse[2],
				carDeltaVelocity[0], carDeltaVelocity[1], carDeltaVelocity[2],
				contactPoint[0], contactPoint[1], contactPoint[2],
				outwardNormal[0], outwardNormal[1], outwardNormal[2] );
			self->scriptedDebugImpactLogTime = level.time + 500;
		}
		return;
	}

	VectorCopy( self->s.pos.trDelta, objectVelocityBefore );
	VectorCopy( self->s.apos.trDelta, objectAngularBefore );
	if ( closingSpeed >= -0.01f ) {
		if ( g_developer.integer && level.time >= self->scriptedDebugImpactLogTime ) {
			Com_Printf( "rally_scripted_object: impact ignored ent=%d car=%d close=%.2f normal=(%.2f %.2f %.2f) objectV=(%.1f %.1f %.1f) carV=(%.1f %.1f %.1f)\n",
				self->s.number, other->s.number, closingSpeed,
				outwardNormal[0], outwardNormal[1], outwardNormal[2],
				objectVelocityBefore[0], objectVelocityBefore[1], objectVelocityBefore[2],
				vehicleVelocity[0], vehicleVelocity[1], vehicleVelocity[2] );
			self->scriptedDebugImpactLogTime = level.time + 500;
		}
		return;
	}

	/* Resolve prop translation and rotation against the car's translational
	 * mass. Pmove already handles the car's swept-point contact response. */
	VectorCopy( relativeVelocity, self->s.pos.trDelta );
	normalImpulseMagnitude = 0.0f;
	G_RallyObject_ApplyCollisionWithOtherInverseMass( self, contactPoint, outwardNormal,
		self->elasticity, self->vehicleImpactScale, vehicleInverseMass,
		&normalImpulseMagnitude );
	G_RallyObject_ApplyContactFriction( self, contactPoint, outwardNormal,
		normalImpulseMagnitude, self->friction );
	G_RallyObject_ApplyAngularContactResistance( self, outwardNormal, normalImpulseMagnitude );
	VectorAdd( self->s.pos.trDelta, vehicleVelocity, self->s.pos.trDelta );
	VectorCopy( self->s.pos.trDelta, self->lastNonZeroVelocity );
	VectorScale( outwardNormal, normalImpulseMagnitude, objectImpulse );
	G_ScriptedObject_ApplyVehicleCounterImpulse( other, objectImpulse, carDeltaVelocity );
	if ( other->client )
		VectorCopy( other->client->car.sBody.v, carVelocityAfter );
	else
		VectorCopy( vehicleVelocity, carVelocityAfter );
	if ( g_developer.integer && level.time >= self->scriptedDebugImpactLogTime ) {
		Com_Printf( "rally_scripted_object: impact ent=%d car=%d close=%.2f scale=%.2f carInv=%.5f J=%.2f point=(%.1f %.1f %.1f) normal=(%.2f %.2f %.2f) v0=(%.1f %.1f %.1f) carV=(%.1f %.1f %.1f) carDv=(%.2f %.2f %.2f) carV1=(%.1f %.1f %.1f) v1=(%.1f %.1f %.1f) omega0=(%.2f %.2f %.2f) omega1=(%.2f %.2f %.2f)\n",
			self->s.number, other->s.number, closingSpeed, self->vehicleImpactScale,
			vehicleInverseMass,
			normalImpulseMagnitude, contactPoint[0], contactPoint[1], contactPoint[2],
			outwardNormal[0], outwardNormal[1], outwardNormal[2],
			objectVelocityBefore[0], objectVelocityBefore[1], objectVelocityBefore[2],
			vehicleVelocity[0], vehicleVelocity[1], vehicleVelocity[2],
			carDeltaVelocity[0], carDeltaVelocity[1], carDeltaVelocity[2],
			carVelocityAfter[0], carVelocityAfter[1], carVelocityAfter[2],
			self->s.pos.trDelta[0], self->s.pos.trDelta[1], self->s.pos.trDelta[2],
			objectAngularBefore[0], objectAngularBefore[1], objectAngularBefore[2],
			self->s.apos.trDelta[0], self->s.apos.trDelta[1], self->s.apos.trDelta[2] );
		self->scriptedDebugImpactLogTime = level.time + 500;
	}
}

void G_RallyObject_Wake( gentity_t *self ) {
	if ( !self || !self->moveable )
		return;

	self->physicsQuietSince = -1;
	self->physicsGrounded = qfalse;
	if ( G_RallyPhysics_Enabled() ) {
		self->physicsSleeping = qfalse;
		return;
	}
	if ( !self->physicsSleeping )
		return;

	self->physicsSleeping = qfalse;
	self->updateTime = level.time;
	self->physicsAccumulatorMsec = 0;
	self->nextthink = level.time + SCRIPTED_OBJECT_PHYSICS_STEP_MSEC;
}

void G_ScriptedObject_Touch ( gentity_t *self, gentity_t *other, trace_t *trace ) {
	vec3_t vehicleVelocity;

	if ( !other )
		return;
	if ( other->client )
		VectorCopy( other->client->ps.velocity, vehicleVelocity );
	else
		VectorCopy( other->s.pos.trDelta, vehicleVelocity );
	G_ScriptedObject_TouchWithVelocity( self, other, trace, vehicleVelocity );
}

void G_ScriptedObject_Think ( gentity_t *self ){
	int elapsedMsec;
	int steps;

	if ( !self->moveable )
		return;
	if ( self->physicsSleeping ) {
		self->nextthink = 0;
		return;
	}

	elapsedMsec = level.time - self->updateTime;
	if ( elapsedMsec < 0 )
		elapsedMsec = 0;
	if ( elapsedMsec > SCRIPTED_OBJECT_MAX_FRAME_MSEC )
		elapsedMsec = SCRIPTED_OBJECT_MAX_FRAME_MSEC;
	self->updateTime = level.time;
	self->physicsAccumulatorMsec += elapsedMsec;

	steps = 0;
	while ( self->physicsAccumulatorMsec >= SCRIPTED_OBJECT_PHYSICS_STEP_MSEC &&
		steps < SCRIPTED_OBJECT_MAX_SUBSTEPS ) {
		float stepSeconds;

		stepSeconds = SCRIPTED_OBJECT_PHYSICS_STEP_MSEC * 0.001f;
		VectorClear( self->netForce );
		if ( !self->physicsGrounded || Q_fabs( self->s.pos.trDelta[2] ) > 2.0f )
			self->netForce[2] = -CP_CURRENT_GRAVITY * self->mass;
		VectorClear( self->netMoment );
		G_RallyObject_IntegratePhysics( self, stepSeconds );
		G_RallyObject_TracePhysics( self, stepSeconds );
		G_RallyObject_ApplyGroundSupport( self, stepSeconds );

		self->physicsAccumulatorMsec -= SCRIPTED_OBJECT_PHYSICS_STEP_MSEC;
		steps++;
	}

	if ( self->physicsGrounded &&
		VectorLengthSquared( self->s.pos.trDelta ) <= 4.0f &&
		VectorLengthSquared( self->s.apos.trDelta ) <= 0.0025f ) {
		if ( self->physicsQuietSince < 0 ) {
			self->physicsQuietSince = level.time;
		} else if ( level.time - self->physicsQuietSince >= 750 ) {
			self->physicsSleeping = qtrue;
			VectorClear( self->s.pos.trDelta );
			VectorClear( self->s.apos.trDelta );
			VectorClear( self->angularMomentum );
			VectorClear( self->netForce );
			VectorClear( self->netMoment );
			self->physicsAccumulatorMsec = 0;
			self->updateTime = level.time;
			self->nextthink = 0;
		}
	} else {
		self->physicsQuietSince = -1;
	}

	/* Update entity position and angles for rendering */
	VectorCopy( self->s.pos.trBase, self->r.currentOrigin );
	VectorCopy( self->s.pos.trBase, self->s.origin );
	VectorCopy( self->s.apos.trBase, self->r.currentAngles );
	VectorCopy( self->s.apos.trBase, self->s.angles );
	
	/* Link entity into world for collision detection */
	trap_LinkEntity( self );

	/* Accumulated fixed 10 ms steps make server frame jitter irrelevant. */
	if ( !self->physicsSleeping )
		self->nextthink = level.time + SCRIPTED_OBJECT_PHYSICS_STEP_MSEC;
}

void G_ScriptedObject_Pain ( gentity_t *self, gentity_t *attacker, int damage ){
	/* Optional: Hit sound and frame animation based on health */
	/*
	if (self->number > 0){
		self->s.frame = self->number - ((self->maxHealth / (float)self->number) * self->health);
	}
	*/

	/* Debug output for pain events */
	/* Com_Printf("Scripted map object %s was hit\n", self->classname); */
}

static void G_ApplyScriptedObjectMapProperties( gentity_t *ent ) {
	char *physics;
	char *mapValue;
	int value;
	float floatValue;
	vec3_t vectorValue;
	qboolean physicsSpecified;

	physicsSpecified = G_SpawnString( "physics", NULL, &physics );
	if ( physicsSpecified && physics && physics[0] ) {
		if ( !Q_stricmp( physics, "dynamic" ) || !Q_stricmp( physics, "movable" ) ) {
			ent->moveable = qtrue;
		} else if ( !Q_stricmp( physics, "static" ) ) {
			ent->moveable = qfalse;
		} else {
			Com_Printf( "rally_scripted_object: unknown physics mode '%s' (use static or dynamic)\n", physics );
		}
	} else if ( G_SpawnInt( "moveable", "0", &value ) ) {
		/* Legacy spelling remains supported; map keys override the archetype. */
		ent->moveable = value ? qtrue : qfalse;
	}

	if ( G_SpawnString( "mass", NULL, &mapValue ) && mapValue && mapValue[0] ) {
		value = atoi( mapValue );
		if ( value < 1 ) value = 1;
		if ( value > 100000 ) value = 100000;
		ent->mass = value;
	}
	if ( G_SpawnString( "inertia_shape", NULL, &mapValue ) && mapValue && mapValue[0] ) {
		if ( !Q_stricmp( mapValue, "box" ) )
			ent->inertiaShape = RALLY_OBJECT_INERTIA_BOX;
		else if ( !Q_stricmp( mapValue, "sphere" ) )
			ent->inertiaShape = RALLY_OBJECT_INERTIA_SPHERE;
		else if ( !Q_stricmp( mapValue, "cylinder_x" ) )
			ent->inertiaShape = RALLY_OBJECT_INERTIA_CYLINDER_X;
		else if ( !Q_stricmp( mapValue, "cylinder_y" ) )
			ent->inertiaShape = RALLY_OBJECT_INERTIA_CYLINDER_Y;
		else if ( !Q_stricmp( mapValue, "cylinder_z" ) )
			ent->inertiaShape = RALLY_OBJECT_INERTIA_CYLINDER_Z;
		else
			Com_Printf( "rally_scripted_object: unknown inertia_shape '%s' (using box)\n", mapValue );
	}
	if ( G_SpawnString( "elasticity", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnFloat( "elasticity", "0.1", &floatValue ) ) {
		ent->elasticity = Com_Clamp( 0.0f, 1.0f, floatValue );
	}
	if ( G_SpawnString( "friction", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnFloat( "friction", "0.6", &floatValue ) ) {
		ent->friction = Com_Clamp( 0.0f, 4.0f, floatValue );
	}
	if ( G_SpawnString( "rolling_friction", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnFloat( "rolling_friction", "0.1", &floatValue ) ) {
		ent->rollingFriction = Com_Clamp( 0.0f, 1.0f, floatValue );
	}
	if ( G_SpawnString( "spinning_friction", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnFloat( "spinning_friction", "0.05", &floatValue ) ) {
		ent->spinningFriction = Com_Clamp( 0.0f, 1.0f, floatValue );
	}
	if ( G_SpawnString( "vehicle_impact_scale", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnFloat( "vehicle_impact_scale", "0.5", &floatValue ) ) {
		ent->vehicleImpactScale = Com_Clamp( 0.0f, 1.0f, floatValue );
	}
	if ( G_SpawnString( "weapon_impact_scale", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnFloat( "weapon_impact_scale", "1.0", &floatValue ) ) {
		ent->weaponImpactScale = Com_Clamp( 0.0f, 5.0f, floatValue );
	}
	if ( G_SpawnString( "health", NULL, &mapValue ) && mapValue && mapValue[0] ) {
		value = atoi( mapValue );
		ent->health = value > 0 ? value : 0;
		ent->maxHealth = ent->health;
		ent->takedamage = ent->health > 0 ? qtrue : qfalse;
	} else if ( ent->health <= 0 ) {
		/* Both legacy -1 and documented 0 mean indestructible. */
		ent->health = 0;
		ent->maxHealth = 0;
		ent->takedamage = qfalse;
	}
	if ( G_SpawnString( "mins", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnVector( "mins", "0 0 0", vectorValue ) )
		VectorCopy( vectorValue, ent->r.mins );
	if ( G_SpawnString( "maxs", NULL, &mapValue ) && mapValue && mapValue[0] &&
		G_SpawnVector( "maxs", "0 0 0", vectorValue ) )
		VectorCopy( vectorValue, ent->r.maxs );
	if ( ent->mass < 1 ) ent->mass = 1;
	if ( ent->mass > 100000 ) ent->mass = 100000;
	ent->elasticity = Com_Clamp( 0.0f, 1.0f, ent->elasticity );
	ent->friction = Com_Clamp( 0.0f, 4.0f, ent->friction );
	ent->rollingFriction = Com_Clamp( 0.0f, 1.0f, ent->rollingFriction );
	ent->spinningFriction = Com_Clamp( 0.0f, 1.0f, ent->spinningFriction );
	ent->vehicleImpactScale = Com_Clamp( 0.0f, 1.0f, ent->vehicleImpactScale );
	ent->weaponImpactScale = Com_Clamp( 0.0f, 5.0f, ent->weaponImpactScale );

	if ( ent->model && ent->model[0] ) {
		/* Radiant's standard model key overrides the archetype model. */
		ent->s.modelindex2 = G_ModelIndex( ent->model );
	}

	for ( value = 0; value < 3; value++ ) {
		if ( ent->r.mins[value] >= ent->r.maxs[value] ||
			ent->r.mins[value] < -4096.0f || ent->r.mins[value] > 4096.0f ||
			ent->r.maxs[value] < -4096.0f || ent->r.maxs[value] > 4096.0f ) {
			Com_Printf( "rally_scripted_object: invalid collision bounds; using a 32-unit box\n" );
			VectorSet( ent->r.mins, -16.0f, -16.0f, -16.0f );
			VectorSet( ent->r.maxs,  16.0f,  16.0f,  16.0f );
			break;
		}
	}
}

void G_ScriptedObject_ApplyWeaponImpact( gentity_t *target, gentity_t *inflictor,
	gentity_t *attacker, const vec3_t direction, const vec3_t point, int damage ) {
	vec3_t impulseDirection, impulse, impactPoint;
	float impulseMagnitude, minImpulse, maxImpulse;
	if ( !target || target->s.eType != ET_SCRIPTED || !target->moveable ||
		!G_RallyPhysics_Enabled() || damage <= 0 ||
		target->weaponImpactScale <= 0.0f )
		return;
	/* Only player-fired weapons and their projectiles should impart this extra
	 * impulse; environmental damage and vehicle contacts have separate paths. */
	if ( ( !attacker || !attacker->client ) &&
		( !inflictor || inflictor->s.eType != ET_MISSILE ) )
		return;

	VectorClear( impulseDirection );
	if ( direction )
		VectorCopy( direction, impulseDirection );
	if ( VectorNormalize( impulseDirection ) == 0.0f && inflictor )
		VectorCopy( inflictor->s.pos.trDelta, impulseDirection );
	if ( VectorNormalize( impulseDirection ) == 0.0f && inflictor ) {
		VectorSubtract( target->r.currentOrigin, inflictor->r.currentOrigin,
			impulseDirection );
	}
	if ( VectorNormalize( impulseDirection ) == 0.0f )
		VectorSet( impulseDirection, 0.0f, 0.0f, 1.0f );
	if ( point )
		VectorCopy( point, impactPoint );
	else
		VectorCopy( target->r.currentOrigin, impactPoint );

	/* Damage scales the kick; the cap limits extreme modded damage values while
	 * keeping the response comparable across different prop masses. */
	impulseMagnitude = (float)damage * 20.0f * target->weaponImpactScale;
	/* A very weak hit can otherwise fall below the resting contact-friction
	 * threshold on a heavier prop and get cancelled completely in one Bullet
	 * step. Preserve mass response, but guarantee a modest knockback velocity. */
	minImpulse = (float)target->mass * 20.0f * target->weaponImpactScale;
	if ( impulseMagnitude < minImpulse )
		impulseMagnitude = minImpulse;
	maxImpulse = (float)target->mass * 250.0f;
	if ( impulseMagnitude > maxImpulse )
		impulseMagnitude = maxImpulse;
	VectorScale( impulseDirection, impulseMagnitude, impulse );
	if ( g_developer.integer && level.time >= target->scriptedDebugTraceLogTime ) {
		Com_Printf( "rally_scripted_object: weapon impact ent=%d damage=%d scale=%.2f impulse=%.1f point=(%.1f %.1f %.1f) dir=(%.2f %.2f %.2f)\n",
			target->s.number, damage, target->weaponImpactScale,
			impulseMagnitude, impactPoint[0], impactPoint[1], impactPoint[2],
			impulseDirection[0], impulseDirection[1], impulseDirection[2] );
		target->scriptedDebugTraceLogTime = level.time + 500;
	}
	trap_RallyPhysicsApplyImpulse( target->s.number, impactPoint, impulse );
}

static void G_ScriptedObject_DropToFloor( gentity_t *ent ) {
	trace_t trace;
	vec3_t end;

	VectorCopy( ent->s.origin, end );
	end[2] -= 4096.0f;
	if ( ent->collisionHullVerts && ent->collisionHullVertCount >= 4 ) {
		trap_TraceConvex( &trace, ent->s.origin, ent->collisionHullMins,
			ent->collisionHullMaxs, end, ent->collisionHullVerts,
			ent->collisionHullVertCount, ent->s.apos.trBase,
			ent->s.number, MASK_PLAYERSOLID );
	} else {
		trap_Trace( &trace, ent->s.origin, ent->r.mins, ent->r.maxs, end,
			ent->s.number, MASK_PLAYERSOLID );
	}

	/* Preserve the ordinary mover-ground relationship used by map props. */
	ent->s.groundEntityNum = trace.entityNum;
	G_SetOrigin( ent, trace.endpos );
}

void SP_rally_scripted_object( gentity_t *ent ){
	/* Check if script file can be loaded and parsed */
	if ( !G_ParseScriptedObject( ent ) ){
		/* If there was a problem loading the script, remove this entity */
		G_FreeEntity(ent);
		return;
	}
	G_ApplyScriptedObjectMapProperties( ent );
	/* Bullet props remain traceable by weapons even when they have no health.
	 * G_Damage treats healthless props as impulse-only targets. */
	if ( G_RallyPhysics_Enabled() )
		ent->takedamage = qtrue;

	/* Set entity type for client-side rendering */
	ent->s.eType = ET_SCRIPTED;

	/* Static and dynamic props both participate in vehicle body traces. */
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_PLAYERSOLID;
	if ( ( ent->moveable || G_RallyPhysics_Enabled() ) && ent->model && ent->model[0] &&
		!G_RallyObject_LoadCollisionHull( ent ) ) {
		Com_Printf( "rally_scripted_object: no usable MD3 collision hull for '%s'; using mins/maxs fallback\n",
			ent->model );
	}

	/* Set up entity callbacks */
	ent->die = G_ScriptedObject_Destroy;
	ent->touch = G_ScriptedObject_Touch;    /* Enable collision with vehicles */
	ent->pain = G_ScriptedObject_Pain;
	ent->think = ( ent->moveable && !G_RallyPhysics_Enabled() ) ? G_ScriptedObject_Think : NULL;
	
	/* Static props remain asleep; dynamic props start on the next simulation step. */
	ent->nextthink = ( ent->moveable && !G_RallyPhysics_Enabled() ) ?
		level.time + SCRIPTED_OBJECT_PHYSICS_STEP_MSEC : 0;
	ent->updateTime = level.time;
	ent->physicsAccumulatorMsec = 0;
	ent->physicsQuietSince = -1;
	ent->scriptedDebugImpactLogTime = 0;
	ent->scriptedDebugTraceLogTime = 0;
	ent->scriptedDebugGroundLogTime = 0;
	ent->physicsSleeping = qfalse;
	ent->physicsGrounded = qfalse;

	/* Initialize physics state */
	VectorClear( ent->netForce );
	VectorClear( ent->netMoment );
	VectorClear( ent->angularMomentum );

	/* Positions are authoritative on the server and interpolated from snapshots. */
	ent->s.pos.trType = TR_INTERPOLATE;
	ent->s.apos.trType = TR_INTERPOLATE;
	/* Spawnvars populate s.angles, while clients and the physics solver evaluate
	 * the angular trajectory. Seed it before the initial floor trace so authored
	 * pitch/roll is honored for both rendering and convex collision. */
	VectorCopy( ent->s.angles, ent->s.apos.trBase );
	VectorCopy( ent->s.angles, ent->r.currentAngles );

	/* Initialize velocity tracking */
	VectorSet( ent->lastNonZeroVelocity, 0, 0, 0 );

	/* Optional: Set looping sound if defined in script */
	/*
	if (ent->preSoundLoop)
		ent->s.loopSound = ent->preSoundLoop;
	*/

	/* Drop with the actual oriented model hull when available. The generic
	 * DropToFloor uses only the unrotated map bounds, which can leave a tilted
	 * dynamic prop embedded in the floor before its first physics step. */
	G_ScriptedObject_DropToFloor( ent );
	if ( G_RallyPhysics_Enabled() && !G_RallyPhysics_CreateEntity( ent ) ) {
		Com_Printf( "rally_scripted_object: Bullet body creation failed for ent=%d; keeping legacy collision\n",
			ent->s.number );
		if ( ent->moveable ) {
			ent->think = G_ScriptedObject_Think;
			ent->nextthink = level.time + SCRIPTED_OBJECT_PHYSICS_STEP_MSEC;
		}
	}
	if ( g_developer.integer ) {
		Com_Printf( "rally_scripted_object: spawn ent=%d model='%s' modelindex2=%d physics=%s mass=%d impactScale=%.2f hullVerts=%d origin=(%.1f %.1f %.1f) angles=(%.1f %.1f %.1f) mins=(%.1f %.1f %.1f) maxs=(%.1f %.1f %.1f)\n",
			ent->s.number, ent->model ? ent->model : "<script>", ent->s.modelindex2,
			ent->moveable ? "dynamic" : "static", ent->mass, ent->vehicleImpactScale,
			ent->collisionHullVertCount, ent->s.pos.trBase[0], ent->s.pos.trBase[1],
			ent->s.pos.trBase[2], ent->s.apos.trBase[0], ent->s.apos.trBase[1],
			ent->s.apos.trBase[2], ent->r.mins[0], ent->r.mins[1], ent->r.mins[2],
			ent->r.maxs[0], ent->r.maxs[1], ent->r.maxs[2] );
	}

	/* Link entity into world */
	trap_LinkEntity (ent);
}
