/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2015 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

#define		MAX_SCRIPT_TEXT		8192


// collision types
#define		CT_BOX			0
#define		CT_CONE			1
#define		CT_CYLINDER		2


qboolean SeekToSection( char **pointer, char *str ){
	char		*token;

	// UPDATE: using strstr instead?
	// UPDATE: check if end of file is inside of a bracket (ie bad brackets in script file)

	// seek to 'str {'
	while ( 1 ) {
		token = COM_Parse( pointer );

		if( !token || token[0] == 0 )
			return qfalse;

		if ( !Q_stricmp( token, "{" ) ){
			// loop through this
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

	if( !token || token[0] == 0 ) // model not found 
		return qfalse;

	return qtrue;
}


qboolean G_ParseScriptedObject( gentity_t *ent ){
	char		*text_p;
	int			len, i;
	char		*token;
	char		text[MAX_SCRIPT_TEXT];
	char		filename[MAX_QPATH];
//	char		model[MAX_QPATH];
//	char		deadmodel[MAX_QPATH];
	fileHandle_t	f;

	// setup defaults
	ent->takedamage = qfalse;
	VectorLength(ent->r.mins);
	VectorLength(ent->r.maxs);
	ent->elasticity = 0.1f;
	ent->mass = 100;
	ent->moveable = qfalse;
	ent->number = 0;

	if (!ent->script || ent->script[0] == 0){
		Com_Printf("No Script file specified\n");
		return qfalse;
	}

	// for debugging only load one object
//	if( Q_stricmp( ent->script, "models/mapobjects/barrels/barrel01" ) )
//		return qfalse;

	Q_strncpyz(filename, ent->script, sizeof(filename));
	token = strchr(filename, '.');
	if (!token)
		Q_strcat(filename, sizeof(filename), ".script");

	if (g_developer.integer)
		Com_Printf("Attempting to load script %s\n", filename);

	// load the file
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

	// parse the text
	text_p = text;

	// seek to "rally_scripted_object {"
	if ( !SeekToSection( &text_p, "rally_scripted_object" ) ){
		Com_Printf( "Script file '%s' did not contain rally_scripted_object\n", filename );
		return qfalse;
	}

	// send script file name in CS so we dont need
	// to do all of the drawing and stuff server side.
	ent->s.modelindex = G_ScriptIndex( ent->script );

	// read optional parameters
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
			if (ent->health >= 0)
				ent->takedamage = qtrue;

			continue;
		}
		else if ( !Q_stricmp( token, "mins" ) ){
			for (i = 0; i < 3; i++){
				token = COM_Parse( &text_p );
				if ( !token ) break;

				ent->r.mins[i] = atof(token);
			}

			continue;
		}
		else if ( !Q_stricmp( token, "maxs" ) ){
			for (i = 0; i < 3; i++){
				token = COM_Parse( &text_p );
				if ( !token ) break;

				ent->r.maxs[i] = atof(token);
			}

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
			// skip gibs part of script (it is only used client side)
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			if ( !Q_stricmp( token, "{" ) ){
				// loop through this
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


void G_ScriptedObject_ApplyForce( gentity_t *self, vec3_t force, vec3_t at ){
	vec3_t	arm, moment;

	VectorSubtract( at, self->s.pos.trBase, arm );

	VectorAdd( self->netForce, force, self->netForce );

	CrossProduct( arm, force, moment );
	VectorAdd( self->netMoment, moment, self->netMoment );
}


qboolean G_ScriptedObject_ApplyCollision( gentity_t *self, vec3_t at, vec3_t normal, float elasticity ){
	vec3_t	arm;
	vec3_t	vP1;
	vec3_t	impulse, impulseMoment;
	vec3_t	cross, cross2;
	float	impulseNum, impulseDen, dot;
//	float	oppositeImpulseNum;

	// temp for inverseWorldInertiaTensor
	vec3_t	axis[3];
	vec3_t	inverseBodyInertiaTensor;
	vec3_t	inverseWorldInertiaTensor[3];

	VectorSubtract( at, self->s.pos.trBase, arm );

//	if (pm->pDebug){
//		Com_Printf("PM_ApplyCollision: arm %0.3f, %0.3f, %0.3f\n", arm[0], arm[1], arm[2]);
//		Com_Printf("PM_ApplyCollision: normal %0.3f, %0.3f, %0.3f\n", normal[0], normal[1], normal[2]);
//	}

	CrossProduct( self->s.apos.trDelta, arm, cross );
	VectorAdd( self->s.pos.trDelta, cross, vP1 );

	{
		float	m[3][3], m2[3][3];
		AnglesToOrientation( self->s.apos.trBase, axis );

		inverseBodyInertiaTensor[0] = 1.0f * 3.0f / (self->mass * (self->r.maxs[1] * self->r.maxs[1] + self->r.maxs[2] * self->r.maxs[2]));
		inverseBodyInertiaTensor[1] = 1.0f * 3.0f / (self->mass * (self->r.maxs[0] * self->r.maxs[0] + self->r.maxs[2] * self->r.maxs[2]));
		inverseBodyInertiaTensor[2] = 1.0f * 3.0f / (self->mass * (self->r.maxs[0] * self->r.maxs[0] + self->r.maxs[1] * self->r.maxs[1]));

		m[0][0] = inverseBodyInertiaTensor[0] * axis[0][0];
		m[1][0] = inverseBodyInertiaTensor[0] * axis[1][0];
		m[2][0] = inverseBodyInertiaTensor[0] * axis[2][0];

		m[0][1] = inverseBodyInertiaTensor[1] * axis[0][1];
		m[1][1] = inverseBodyInertiaTensor[1] * axis[1][1];
		m[2][1] = inverseBodyInertiaTensor[1] * axis[2][1];

		m[0][2] = inverseBodyInertiaTensor[2] * axis[0][2];
		m[1][2] = inverseBodyInertiaTensor[2] * axis[1][2];
		m[2][2] = inverseBodyInertiaTensor[2] * axis[2][2];

		MatrixTranspose( axis, m2 );
		MatrixMultiply( m, m2, inverseWorldInertiaTensor );
	}

	// added from collision
	VectorClear(impulse);
	dot = DotProduct(normal, vP1);
	if ( dot < -8 ){
		impulseNum = -(1.0f + elasticity) * DotProduct(normal, vP1);
//		oppositeImpulseNum = -(1.0f - elasticity) * DotProduct(normal, vP1);

		CrossProduct( arm, normal, cross );
		VectorRotate( cross, inverseWorldInertiaTensor, cross2 );
		CrossProduct( cross2, arm, cross );
//		Com_Printf( "oneOverMass %f, cross.normal %f\n", 1.0f / body->mass, DotProduct(cross, normal) );
		impulseDen = 1.0f / self->mass + DotProduct( cross, normal );

		VectorScale( normal, impulseNum / impulseDen, impulse );
	}
	else {
		// not hitting surface
//		Com_Printf( "PM_ApplyCollision: not hitting surface, %f\n", dot );
		return qfalse;
	}

	// apply impulse to primary quantities
	VectorMA( self->s.pos.trDelta, 1.0 / self->mass, impulse, self->s.pos.trDelta );
	CrossProduct( arm, impulse, impulseMoment );
	VectorAdd( self->angularMomentum, impulseMoment, self->angularMomentum );
    
    // compute affected auxiliary quantities
//	VectorRotate( self->angularMomentum, inverseWorldInertiaTensor, self->s.apos.trDelta );

	return qtrue;
}

/*

qboolean G_TraceIntersect( trace_t *tr, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, vec3_t intersect )
{
	vec3_t	dir;
	vec3_t	n, u, p, line;
	vec3_t	lineP;
	int		firstAxis, axis, lineAxis, next, next2;
	float	len, len2, d, denom;

	trap_Trace( tr, start, mins, maxs, end, passEntityNum, contentmask );

	if( tr->fraction == 1.0f || tr->startsolid || tr->allsolid )
	{
		VectorCopy( tr->endpos, intersect );
		return qfalse;
	}

	Com_Printf( "Trace: fraction %f, normal (%f %f %f), dist %f\n", tr->fraction, tr->plane.normal[0], tr->plane.normal[1], tr->plane.normal[2], tr->plane.dist );

	VectorSubtract( end, start, dir );
	len = VectorNormalize( dir );

	if( len == 0 )
	{
		VectorCopy( tr->endpos, intersect );
		return qfalse;
	}

	axis = 0;
	if( fabs(dir[1]) > fabs(dir[0]) && fabs(dir[1]) > fabs(dir[2]) )
		axis = 1;
	else if( fabs(dir[2]) > fabs(dir[0]) && fabs(dir[2]) > fabs(dir[1]) )
		axis = 2;

	firstAxis = axis;

	Com_Printf( "Axis %i, plane type %i\n", axis, tr->plane.type );

	// create plane normal and point
	VectorClear( n );
	VectorCopy( tr->endpos, p );
	if( dir[axis] > 0.0f )
	{
		p[axis] += maxs[axis];
		n[axis] = 1.0f;
	}
	else
	{
		p[axis] += mins[axis];
		n[axis] = -1.0f;
	}

	if( tr->plane.type == axis )
	{
		// FIXME: need to check other plane now? or check distances to make sure we are right.
		Com_Printf( "Coplaniar\n" );
		VectorCopy( p, intersect );

		Com_Printf( "1: Point at (%f %f %f)\n", intersect[0], intersect[1], intersect[2] );

		return qtrue;
	}
	d = p[axis] * n[axis];

	Com_Printf( "Plane point (%f %f %f), normal (%f %f %f), dist %f\n", p[0], p[1], p[2], n[0], n[1], n[2], d );

	CrossProduct( n, tr->plane.normal, u );
	// FIXME: why is u not normalized already since n and plane.normal are normalized
	len = VectorNormalize( u );
//	if( len == 0 )
//	{
		// planes are parallel, Should already be handled previously
//	}

	lineAxis = 0;
	if( fabs(u[1]) > fabs(u[0]) && fabs(u[1]) > fabs(u[2]) )
		lineAxis = 1;
	else if( fabs(u[2]) > fabs(u[0]) && fabs(u[2]) > fabs(u[1]) )
		lineAxis = 2;

	Com_Printf( "Line Axis %i\n", lineAxis );
	next = (lineAxis + 1) % 3;
	next2 = (next + 1) % 3;

	// trace plane is 1, bbox plane is 2

	Com_Printf( "Next %i, next2 %i\n", next, next2 );
	denom = ( n[next] * tr->plane.normal[next2] - n[next2] * tr->plane.normal[next] );

	Com_Printf( "Denom %f\n", denom );

	lineP[axis] = 0;
	lineP[next] = ( d * tr->plane.normal[next2] - n[next2] * tr->plane.dist ) / denom;
	lineP[next2] = ( tr->plane.dist * n[next] - tr->plane.normal[next] * d ) / denom;

	Com_Printf( "Line at (%f %f %f) in direction (%f %f %f)\n", lineP[0], lineP[1], lineP[2], u[0], u[1], u[2] );

	axis = 0;
	if( fabs(dir[1]) <= fabs(dir[0]) && fabs(dir[1]) > fabs(dir[2]) )
		axis = 1;
	else if( fabs(dir[1]) > fabs(dir[0]) && fabs(dir[1]) <= fabs(dir[2]) )
		axis = 1;
	else if( fabs(dir[2]) <= fabs(dir[0]) && fabs(dir[2]) > fabs(dir[1]) )
		axis = 2;
	else if( fabs(dir[2]) > fabs(dir[0]) && fabs(dir[2]) <= fabs(dir[1]) )
		axis = 2;

	if( axis == firstAxis )
		axis = (axis + 1) % 3;

	Com_Printf( "Axis %i\n", axis );

	// create plane normal and point
	VectorClear( n );
	VectorCopy( tr->endpos, p );
	if( dir[axis] > 0.0f )
	{
		p[axis] += maxs[axis];
		n[axis] = 1.0f;
	}
	else
	{
		p[axis] += mins[axis];
		n[axis] = -1.0f;
	}

	VectorSubtract( lineP, p, line );
	len = DotProduct( line, n );

	len2 = DotProduct( u, n );

	Com_Printf( "len %f, len2 %f\n", len, len2 );

	if( len2 == 0.0f )
	{
		// intersection line of two previous planes is parallel to the plane
		VectorCopy( lineP, intersect );

//		intersect[lineAxis] = tr->endpos[lineAxis];
		next = (firstAxis + 1) % 3;
		next2 = (next + 1) % 3;
		if( next == axis )
			intersect[next2] = tr->endpos[next2];
		else
			intersect[next] = tr->endpos[next];
		
		intersect[axis] = tr->endpos[axis];

//		intersect[1] = -intersect[1];
		Com_Printf( "2: Point at (%f %f %f)\n", intersect[0], intersect[1], intersect[2] );

		return qtrue;
	}

	VectorMA( p, -len / len2, u, intersect );

	Com_Printf( "3: Point at (%f %f %f)\n", intersect[0], intersect[1], intersect[2] );

	return qtrue;
}


void G_ScriptedObject_TracePhysics( gentity_t *self, float time )
{
	trace_t		tr;
	vec3_t		dir;
	vec3_t		start, end, intersect;
	float		dot;
	float		moveDist, dist;

	if( VectorLengthSquared( self->s.pos.trDelta ) == 0.0f )
	{
		dist = VectorNormalize2( self->lastNonZeroVelocity, dir );
	}
	else
	{
		dist = VectorNormalize2( self->s.pos.trDelta, dir );
	}

	moveDist = 0.5f > dist ? dist * 0.5f : 0.5f;
	VectorMA( self->s.pos.trBase, -1, dir, start );
	VectorMA( self->s.pos.trBase, time * ( dist + moveDist ), dir, end );

//	trap_Trace( &tr, start, self->r.mins, self->r.maxs, end, self->s.clientNum, CONTENTS_SOLID|CONTENTS_BODY );
	if( G_TraceIntersect( &tr, start, self->r.mins, self->r.maxs, end, self->s.clientNum, CONTENTS_SOLID|CONTENTS_BODY, intersect ) )
	{
		G_SetOrigin( &g_entities[level.testModelID], intersect );
		trap_LinkEntity( &g_entities[level.testModelID] );
	}

	if( tr.startsolid || tr.allsolid )
	{
		float	moveAhead = dist < 1.0f ? dist - 0.01f : 1.0f;

		VectorMA( self->s.pos.trBase, moveAhead, dir, start );
//		trap_Trace( &tr, start, self->r.mins, self->r.maxs, end, self->s.clientNum, CONTENTS_SOLID|CONTENTS_BODY );
		if( G_TraceIntersect( &tr, start, self->r.mins, self->r.maxs, end, self->s.clientNum, CONTENTS_SOLID|CONTENTS_BODY, intersect ) )
		{
			G_SetOrigin( &g_entities[level.testModelID], intersect );
			trap_LinkEntity( &g_entities[level.testModelID] );
		}

		if( tr.startsolid || tr.allsolid )
		{
			Com_Printf( "in solid twice, vel %f %f %f, allsolid %i\n", self->s.pos.trDelta[0], self->s.pos.trDelta[1], self->s.pos.trDelta[2], tr.allsolid );
		}
	}

//	if( tr.fraction == 1.0f )
//		return;

	if( tr.plane.normal[0] == 0 &&
		tr.plane.normal[1] == 0 &&
		tr.plane.normal[2] == 0 )
		return;

	dot = DotProduct( tr.plane.normal, self->s.pos.trDelta );
	if( dot < -16.0f )
	{
		VectorMA( self->s.pos.trDelta, -dot * ( 1.00f + self->elasticity ), tr.plane.normal, self->s.pos.trDelta );
	}
	else if( dot < 0.0f )
	{
		// stop the object once it is slow enough
//		VectorClear( self->s.pos.trDelta );
		VectorMA( self->s.pos.trDelta, -dot, tr.plane.normal, self->s.pos.trDelta );
	}
#if 0
	if( !G_ScriptedObject_ApplyCollision( self, intersect, tr.plane.normal, self->elasticity ) )
	{
		// stop the object once it is slow enough
		VectorClear( self->s.pos.trDelta );
//		VectorMA( self->s.pos.trDelta, -dot, tr.plane.normal, self->s.pos.trDelta );

		// HACK for the angular velocity if we are stopped on the ground
		// kind of the same thing as friction
		VectorScale( self->angularMomentum, 0.99f, self->angularMomentum );
	}
#endif

	dot = DotProduct( tr.plane.normal, self->netForce );
	if( dot < 0.0f )
	{
//		vec3_t	force;
//		VectorScale( tr.plane.normal, -dot, force );
//		G_ScriptedObject_ApplyForce( self, force, intersect );
		VectorMA( self->netForce, -dot, tr.plane.normal, self->netForce );
	}
}

*/

// Traces a cone at the current position and angles.
int G_TraceCone( trace_t *tr, vec3_t origin, vec3_t angles, vec3_t mins, vec3_t maxs, int passEntityNum, int contentmask )
{
	vec3_t		forward, right, up;
	vec3_t		start, end, bottom, top;
	float		radius;

	vec3_t		normal, intersect;
	float		firstHit;
	int			numHits;

	VectorClear( normal );
	VectorClear( intersect );
	firstHit = 1.0f;
	numHits = 0;

	AngleVectors( angles, forward, right, up );

	radius = ( maxs[0] - mins[0] ) / 2.0f;
	VectorMA( origin, mins[2], up, bottom );
	VectorMA( origin, maxs[2], up, top );

	VectorCopy( origin, start );

	// right
	VectorMA( bottom, radius, right, end );
	trap_Trace( tr, start, NULL, NULL, end, passEntityNum, contentmask );
	if( tr->fraction < 1.0f )
	{
		VectorAdd( intersect, tr->endpos, intersect );
		VectorAdd( normal, tr->plane.normal, normal );
		if( tr->fraction < firstHit )
			firstHit = tr->fraction;
		numHits++;
	}
	else
	{
		trap_Trace( tr, end, NULL, NULL, top, passEntityNum, contentmask );
		if( tr->fraction < 1.0f && !tr->startsolid && !tr->allsolid )
		{
			VectorAdd( intersect, tr->endpos, intersect );
			VectorAdd( normal, tr->plane.normal, normal );
			if( tr->fraction < firstHit )
				firstHit = tr->fraction;
			numHits++;
		}
	}

	// left
	VectorMA( bottom, -radius, right, end );
	trap_Trace( tr, start, NULL, NULL, end, passEntityNum, contentmask );
	if( tr->fraction < 1.0f )
	{
		VectorAdd( intersect, tr->endpos, intersect );
		VectorAdd( normal, tr->plane.normal, normal );
		if( tr->fraction < firstHit )
			firstHit = tr->fraction;
		numHits++;
	}
	else
	{
		trap_Trace( tr, end, NULL, NULL, top, passEntityNum, contentmask );
		if( tr->fraction < 1.0f && !tr->startsolid && !tr->allsolid )
		{
			VectorAdd( intersect, tr->endpos, intersect );
			VectorAdd( normal, tr->plane.normal, normal );
			if( tr->fraction < firstHit )
				firstHit = tr->fraction;
			numHits++;
		}
	}

	// front
	VectorMA( bottom, radius, forward, end );
	trap_Trace( tr, start, NULL, NULL, end, passEntityNum, contentmask );
	if( tr->fraction < 1.0f )
	{
		VectorAdd( intersect, tr->endpos, intersect );
		VectorAdd( normal, tr->plane.normal, normal );
		if( tr->fraction < firstHit )
			firstHit = tr->fraction;
		numHits++;
	}
	else
	{
		trap_Trace( tr, end, NULL, NULL, top, passEntityNum, contentmask );
		if( tr->fraction < 1.0f && !tr->startsolid && !tr->allsolid )
		{
			VectorAdd( intersect, tr->endpos, intersect );
			VectorAdd( normal, tr->plane.normal, normal );
			if( tr->fraction < firstHit )
				firstHit = tr->fraction;
			numHits++;
		}
	}

	// back
	VectorMA( bottom, -radius, forward, end );
	trap_Trace( tr, start, NULL, NULL, end, passEntityNum, contentmask );
	if( tr->fraction < 1.0f )
	{
		VectorAdd( intersect, tr->endpos, intersect );
		VectorAdd( normal, tr->plane.normal, normal );
		if( tr->fraction < firstHit )
			firstHit = tr->fraction;
		numHits++;
	}
	else
	{
		trap_Trace( tr, end, NULL, NULL, top, passEntityNum, contentmask );
		if( tr->fraction < 1.0f && !tr->startsolid && !tr->allsolid )
		{
			VectorAdd( intersect, tr->endpos, intersect );
			VectorAdd( normal, tr->plane.normal, normal );
			if( tr->fraction < firstHit )
				firstHit = tr->fraction;
			numHits++;
		}
	}

	if( numHits )
	{
//		VectorScale( normal, 1.0f / numHits, tr->plane.normal );
		VectorNormalize2( normal, tr->plane.normal );
		VectorScale( intersect, 1.0f / numHits, tr->endpos );
		tr->fraction = firstHit;
		return numHits;
	}

	return 0;
}


void G_ScriptedObject_TracePhysics( gentity_t *self, float time )
{
	trace_t		tr;
	vec3_t		dir;
	vec3_t		start, end;
	float		dot;
	float		moveDist, dist;

	if( VectorLengthSquared( self->s.pos.trDelta ) == 0.0f )
	{
		dist = VectorNormalize2( self->lastNonZeroVelocity, dir );
	}
	else
	{
		dist = VectorNormalize2( self->s.pos.trDelta, dir );
	}

	moveDist = 0.5f > dist ? dist * 0.5f : 0.5f;
	VectorMA( self->s.pos.trBase, -1, dir, start );
	VectorMA( self->s.pos.trBase, time * ( dist + moveDist ), dir, end );

	trap_Trace( &tr, start, NULL, NULL, end, self->s.number, MASK_PLAYERSOLID );
	if( tr.startsolid || tr.allsolid )
	{
		trap_Trace( &tr, start, NULL, NULL, end, self->s.number, MASK_PLAYERSOLID & ~CONTENTS_BODY );
		if( tr.fraction < 1.0f || G_TraceCone( &tr, end, self->s.apos.trBase, self->r.mins, self->r.maxs, self->s.number, MASK_PLAYERSOLID & ~CONTENTS_BODY ) )
		{
//			G_SetOrigin( &g_entities[level.testModelID], tr.endpos );
//			trap_LinkEntity( &g_entities[level.testModelID] );
		}
	}
	else if( tr.fraction < 1.0f || G_TraceCone( &tr, end, self->s.apos.trBase, self->r.mins, self->r.maxs, self->s.number, MASK_PLAYERSOLID ) )
	{
//		G_SetOrigin( &g_entities[level.testModelID], tr.endpos );
//		trap_LinkEntity( &g_entities[level.testModelID] );
	}

	if( tr.plane.normal[0] == 0 &&
		tr.plane.normal[1] == 0 &&
		tr.plane.normal[2] == 0 )
		return;

	dot = DotProduct( tr.plane.normal, self->s.pos.trDelta );
	if( dot < -16.0f )
	{
		VectorMA( self->s.pos.trDelta, -dot * ( 1.0f + self->elasticity ), tr.plane.normal, self->s.pos.trDelta );
	}
	else if( dot < 0.00f )
	{
		// stop the object once it is slow enough
//		VectorClear( self->s.pos.trDelta );
		VectorMA( self->s.pos.trDelta, -dot * 1.00f, tr.plane.normal, self->s.pos.trDelta );
	}

/*
	if( !G_ScriptedObject_ApplyCollision( self, tr.endpos, tr.plane.normal, self->elasticity ) )
	{
		// stop the object once it is slow enough
//		VectorClear( self->s.pos.trDelta );
		VectorMA( self->s.pos.trDelta, -dot, tr.plane.normal, self->s.pos.trDelta );

		// HACK for the angular velocity if we are stopped on the ground
		// kind of the same thing as friction
//		VectorScale( self->angularMomentum, 0.99f, self->angularMomentum );
	}
*/

	// friction
	VectorMA( self->s.pos.trDelta, -dot, tr.plane.normal, dir );
//	VectorScale( dir, -1.0f * self->mass, dir );
//	G_ScriptedObject_ApplyForce( self, dir, tr.endpos );
	VectorMA( self->netForce, -1.0f * self->mass, dir, self->netForce );

	dot = DotProduct( tr.plane.normal, self->netForce );
	if( dot < 0.00f )
	{
//		vec3_t	force;
//		VectorScale( tr.plane.normal, -dot, force );
//		G_ScriptedObject_ApplyForce( self, force, intersect );
		VectorMA( self->netForce, -dot * 1.00f, tr.plane.normal, self->netForce );
	}
}


void G_ScriptedObject_IntegratePhysics( gentity_t *self, float time )
{
	float		inverseWorldInertiaTensor[3][3], m[3][3];
	float		m2[3][3];
	vec3_t		axis[3];
	vec3_t		inverseBodyInertiaTensor;
	qboolean	doAngular = qfalse;

	if( self->netMoment[0] != 0 || 
		self->netMoment[1] != 0 || 
		self->netMoment[2] != 0 || 
		self->angularMomentum[0] != 0 || 
		self->angularMomentum[1] != 0 || 
		self->angularMomentum[2] != 0 )
	{
		doAngular = qtrue;
	}

	if( doAngular )
	{
		AnglesToOrientation( self->s.apos.trBase, axis );

		inverseBodyInertiaTensor[0] = 1.0f * 3.0f / (self->mass * (self->r.maxs[1] * self->r.maxs[1] + self->r.maxs[2] * self->r.maxs[2]));
		inverseBodyInertiaTensor[1] = 1.0f * 3.0f / (self->mass * (self->r.maxs[0] * self->r.maxs[0] + self->r.maxs[2] * self->r.maxs[2]));
		inverseBodyInertiaTensor[2] = 1.0f * 3.0f / (self->mass * (self->r.maxs[0] * self->r.maxs[0] + self->r.maxs[1] * self->r.maxs[1]));

		m[0][0] = inverseBodyInertiaTensor[0] * axis[0][0];
		m[1][0] = inverseBodyInertiaTensor[0] * axis[1][0];
		m[2][0] = inverseBodyInertiaTensor[0] * axis[2][0];

		m[0][1] = inverseBodyInertiaTensor[1] * axis[0][1];
		m[1][1] = inverseBodyInertiaTensor[1] * axis[1][1];
		m[2][1] = inverseBodyInertiaTensor[1] * axis[2][1];

		m[0][2] = inverseBodyInertiaTensor[2] * axis[0][2];
		m[1][2] = inverseBodyInertiaTensor[2] * axis[1][2];
		m[2][2] = inverseBodyInertiaTensor[2] * axis[2][2];

		MatrixTranspose( axis, m2 );
		MatrixMultiply( m, m2, inverseWorldInertiaTensor );
	}

	//linear
	VectorMA( self->s.pos.trDelta, time / self->mass, self->netForce, self->s.pos.trDelta );
	VectorMA( self->s.pos.trBase, time, self->s.pos.trDelta, self->s.pos.trBase );
	self->s.pos.trTime = level.time;

	// angular
	if( doAngular )
	{
//		vec3_t	oldAngles;

//		VectorCopy( self->s.apos.trBase, oldAngles );

		VectorMA( self->angularMomentum, time, self->netMoment, self->angularMomentum );
		VectorRotate( self->angularMomentum, inverseWorldInertiaTensor, self->s.apos.trDelta );

		m[0][0] = 0;									m[0][1] = (time) * -self->s.apos.trDelta[2];	m[0][2] = (time) * self->s.apos.trDelta[1];
		m[1][0] = (time) * self->s.apos.trDelta[2];		m[1][1] = 0;									m[1][2] = (time) * -self->s.apos.trDelta[0];
		m[2][0] = (time) * -self->s.apos.trDelta[1];	m[2][1] = (time) * self->s.apos.trDelta[0];		m[2][2] = 0;

		MatrixMultiply( m, axis, m2 );
		MatrixAdd( axis, m2, axis );
		OrthonormalizeOrientation( axis );
		OrientationToAngles( axis, self->s.apos.trBase );

//		VectorSubtract( self->s.apos.trBase, oldAngles, self->s.apos.trDelta );
//		if( time != 0 )
//			VectorScale( self->s.apos.trDelta, 1.0f / time, self->s.apos.trDelta );
	}

	if( self->s.pos.trDelta[0] != 0 || 
		self->s.pos.trDelta[1] != 0 || 
		self->s.pos.trDelta[2] != 0 )
	{
		VectorCopy( self->s.pos.trDelta, self->lastNonZeroVelocity );
	}
}


void G_ScriptedObject_Destroy( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ){
	Com_Printf("Destroying scripted map object %s\n", self->classname);

	self->s.eFlags |= EF_DEAD;
}

void G_ScriptedObject_Touch ( gentity_t *self, gentity_t *other, trace_t *trace ){
	vec3_t		dir;
//	float		angle, min;
	float		dot;
/*	
	vec3_t		mins, maxs;

	// FIXME: use width, height and depth instead of mins and maxs
	mins[0] = min(-self->r.mins[0], self->r.maxs[0]);
	mins[1] = min(-self->r.mins[1], self->r.maxs[1]);

	min = min(mins[0], mins[1]);

	VectorSet(mins, -min, -min, -min);
	VectorSet(maxs, min, min, min);
*/
	// hit sound
//	if (ent->hitSound)
//		FIXME: play hit sound

//	VectorSubtract( self->s.pos.trBase, other->s.pos.trBase, trace->plane.normal );
//	VectorNormalize( trace->plane.normal );
	VectorInverse( trace->plane.normal );

	VectorMA( self->s.pos.trDelta, -1, other->s.pos.trDelta, dir );

	dot = DotProduct( trace->plane.normal, dir );
//	if( dot < -16.0f )
//	{
		VectorMA( dir, -dot * ( 1.0f + self->elasticity ), trace->plane.normal, dir );
		VectorAdd( other->s.pos.trDelta, dir, self->s.pos.trDelta );
//	}
//	else if( dot < 0.00f )
//	{
		// stop the object once it is slow enough
//		VectorClear( self->s.pos.trDelta );
//		VectorMA( dir, -dot * 1.00f, trace->plane.normal, self->s.pos.trDelta );
//	}

//	G_ScriptedObject_ApplyCollision( self, trace->endpos, trace->plane.normal, self->elasticity );
}


vec3_t		tempForce;
void G_ScriptedObject_Think ( gentity_t *self ){
/*
	float	time = ( level.time - self->updateTime ) / 1000.0f;

	VectorSet( self->netForce, 0, 0, -CP_CURRENT_GRAVITY * self->mass );
	VectorClear( self->netMoment );

	if( level.time % 5000 < 1000 )
	{
		if( tempForce[0] == 0 && tempForce[1] == 0 && tempForce[2] == 0 )
		{
			tempForce[0] = crandom();
			tempForce[1] = crandom();
			tempForce[2] = random() * 1.5f;
		}
		VectorMA( self->netForce, CP_CURRENT_GRAVITY * self->mass, tempForce, self->netForce );
//		VectorSet( self->netForce, -CP_CURRENT_GRAVITY * self->mass / 4.0f, 0, 0 );
//		self->netForce[0] = -1;
	}
	else
	{
		VectorClear( tempForce );
	}

	if( self->moveable )
	{
		G_ScriptedObject_TracePhysics( self, 0.050f );
		G_ScriptedObject_IntegratePhysics( self, 0.050f );
	}
*/
	VectorCopy( self->s.pos.trBase, self->r.currentOrigin );
	VectorCopy( self->s.pos.trBase, self->s.origin );
	VectorCopy( self->s.apos.trBase, self->r.currentAngles );
	VectorCopy( self->s.apos.trBase, self->s.angles );
	trap_LinkEntity( self );

	self->nextthink = level.time + 50;
	self->updateTime = level.time;
}


void G_ScriptedObject_Pain ( gentity_t *self, gentity_t *attacker, int damage ){
/*
	// hit sound
//	if (ent->hitSound)
//		FIXME: play hit sound

	if (self->number > 0){
		self->s.frame = self->number - ((self->maxHealth / (float)self->number) * self->health);
	}
*/
//	Com_Printf("Scripted map object %s was hit\n", self->classname);
}


void SP_rally_scripted_object( gentity_t *ent ){
//	G_LogPrintf("Spawning a rally_scripted_object\n");

	// FIXME: check if one of these types of objects have
	// already been loaded and use that one instead.

	if ( !G_ParseScriptedObject( ent ) ){
		// if there was a problem loading the script just get rid of this ent
		G_FreeEntity(ent);
		return;
	}

	ent->s.eType = ET_SCRIPTED;

	if (!ent->moveable)
		ent->r.contents = CONTENTS_BODY;

	ent->die = G_ScriptedObject_Destroy;
//	ent->touch = G_ScriptedObject_Touch;
	ent->pain = G_ScriptedObject_Pain;
	ent->think = G_ScriptedObject_Think;
	ent->nextthink = level.time + 1000;
	ent->updateTime = ent->nextthink;

	VectorClear( ent->netForce );
	VectorClear( ent->netMoment );
	VectorClear( ent->angularMomentum );

	ent->s.pos.trType = TR_LINEAR;
	ent->s.apos.trType = TR_INTERPOLATE;

	VectorSet( ent->lastNonZeroVelocity, 0, 0, 0 );

//	if (ent->preSoundLoop)
//		ent->s.loopSound = ent->preSoundLoop;

	DropToFloor(ent);

	trap_LinkEntity (ent);
}

