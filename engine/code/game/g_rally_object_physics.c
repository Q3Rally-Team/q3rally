/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

#define RALLY_OBJECT_CONTACT_EPSILON 0.125f
static qboolean G_RallyObject_InverseWorldInertia( gentity_t *self,
		vec3_t axis[3], float inverseWorld[3][3] ) {
	float size[3];
	float inverseBody[3];
	float radius;
	float height;
	float rotationScaled[3][3];
	float rotationTranspose[3][3];
	int i;

	if ( !self || self->mass <= 0 )
		return qfalse;

	for ( i = 0; i < 3; i++ ) {
		size[i] = self->r.maxs[i] - self->r.mins[i];
		if ( size[i] <= 0.01f )
			return qfalse;
	}

	if ( self->inertiaShape == RALLY_OBJECT_INERTIA_SPHERE ) {
		radius = 0.5f * ( size[0] < size[1] ? size[0] : size[1] );
		if ( size[2] < 2.0f * radius ) radius = 0.5f * size[2];
		inverseBody[0] = inverseBody[1] = inverseBody[2] =
			2.5f / ( self->mass * radius * radius );
	} else if ( self->inertiaShape >= RALLY_OBJECT_INERTIA_CYLINDER_X &&
		self->inertiaShape <= RALLY_OBJECT_INERTIA_CYLINDER_Z ) {
		int longitudinalAxis;
		int axisA;
		int axisB;

		longitudinalAxis = self->inertiaShape - RALLY_OBJECT_INERTIA_CYLINDER_X;
		axisA = ( longitudinalAxis + 1 ) % 3;
		axisB = ( longitudinalAxis + 2 ) % 3;
		radius = 0.5f * ( size[axisA] < size[axisB] ? size[axisA] : size[axisB] );
		height = size[longitudinalAxis];
		inverseBody[longitudinalAxis] = 2.0f / ( self->mass * radius * radius );
		inverseBody[axisA] = inverseBody[axisB] =
			12.0f / ( self->mass * ( 3.0f * radius * radius + height * height ) );
	} else {
		/* Solid cuboid inertia about its centre: Ixx = m/12 * (y^2 + z^2). */
		inverseBody[0] = 12.0f / ( self->mass * ( size[1] * size[1] + size[2] * size[2] ) );
		inverseBody[1] = 12.0f / ( self->mass * ( size[0] * size[0] + size[2] * size[2] ) );
		inverseBody[2] = 12.0f / ( self->mass * ( size[0] * size[0] + size[1] * size[1] ) );
	}

	AnglesToOrientation( self->s.apos.trBase, axis );
	for ( i = 0; i < 3; i++ ) {
		rotationScaled[0][i] = inverseBody[i] * axis[0][i];
		rotationScaled[1][i] = inverseBody[i] * axis[1][i];
		rotationScaled[2][i] = inverseBody[i] * axis[2][i];
	}
	MatrixTranspose( axis, rotationTranspose );
	MatrixMultiply( rotationScaled, rotationTranspose, inverseWorld );
	return qtrue;
}

static void G_RallyObject_UpdateAngularVelocity( gentity_t *self ) {
	vec3_t axis[3];
	float inverseWorld[3][3];

	if ( G_RallyObject_InverseWorldInertia( self, axis, inverseWorld ) )
		VectorRotate( self->angularMomentum, inverseWorld, self->s.apos.trDelta );
	else
		VectorClear( self->s.apos.trDelta );
}

static void G_RallyObject_ContactVelocity( gentity_t *self, vec3_t at, vec3_t velocity ) {
	vec3_t arm;
	vec3_t spinVelocity;

	VectorSubtract( at, self->s.pos.trBase, arm );
	CrossProduct( self->s.apos.trDelta, arm, spinVelocity );
	VectorAdd( self->s.pos.trDelta, spinVelocity, velocity );
}

void G_RallyObject_SupportContact( gentity_t *self, vec3_t origin,
		vec3_t normal, vec3_t contact ) {
	int i;

	if ( self->collisionHullVerts && self->collisionHullVertCount >= 4 ) {
		vec3_t axis[3];
		vec3_t support;
		vec3_t candidate;
		float minimumProjection;
		float projection;
		int supportCount;
		int vertex;

		AnglesToAxis( self->s.apos.trBase, axis );
		minimumProjection = 1e30f;
		for ( vertex = 0; vertex < self->collisionHullVertCount; vertex++ ) {
			VectorScale( axis[0], self->collisionHullVerts[vertex][0], candidate );
			VectorMA( candidate, self->collisionHullVerts[vertex][1], axis[1], candidate );
			VectorMA( candidate, self->collisionHullVerts[vertex][2], axis[2], candidate );
			projection = DotProduct( candidate, normal );
			if ( projection < minimumProjection )
				minimumProjection = projection;
		}

		/* A supporting face/edge contains multiple equally-low vertices.
		 * Averaging that manifold avoids inventing torque from whichever MD3
		 * vertex happened to be first in the file (notably a barrel's base rim). */
		VectorClear( support );
		supportCount = 0;
		for ( vertex = 0; vertex < self->collisionHullVertCount; vertex++ ) {
			VectorScale( axis[0], self->collisionHullVerts[vertex][0], candidate );
			VectorMA( candidate, self->collisionHullVerts[vertex][1], axis[1], candidate );
			VectorMA( candidate, self->collisionHullVerts[vertex][2], axis[2], candidate );
			projection = DotProduct( candidate, normal );
			if ( projection <= minimumProjection + 0.25f ) {
				VectorAdd( support, candidate, support );
				supportCount++;
			}
		}
		if ( supportCount > 0 )
			VectorScale( support, 1.0f / supportCount, support );
		VectorAdd( origin, support, contact );
		return;
	}

	for ( i = 0; i < 3; i++ ) {
		float center;
		float halfSize;
		float direction;

		center = 0.5f * ( self->r.mins[i] + self->r.maxs[i] );
		halfSize = 0.5f * ( self->r.maxs[i] - self->r.mins[i] );
		direction = normal[i];
		/* Keep a face-centred support point for axis-aligned contacts; for
		 * oblique impacts, use the appropriate box corner. */
		if ( Q_fabs( direction ) < 0.1f )
			direction = 0.0f;
		else
			direction = direction > 0.0f ? 1.0f : -1.0f;
		contact[i] = origin[i] + center - direction * halfSize;
	}
}

static float G_RallyObject_ContactInverseMass( gentity_t *self, vec3_t at, vec3_t direction ) {
	vec3_t arm;
	vec3_t torque;
	vec3_t angularDelta;
	vec3_t velocityDelta;
	vec3_t axis[3];
	float inverseWorld[3][3];
	float result;

	if ( !self || self->mass <= 0 )
		return 0.0f;

	result = 1.0f / self->mass;
	VectorSubtract( at, self->s.pos.trBase, arm );
	if ( G_RallyObject_InverseWorldInertia( self, axis, inverseWorld ) ) {
		CrossProduct( arm, direction, torque );
		VectorRotate( torque, inverseWorld, angularDelta );
		CrossProduct( angularDelta, arm, velocityDelta );
		result += DotProduct( velocityDelta, direction );
	}
	return result;
}

static void G_RallyObject_ApplyImpulse( gentity_t *self, vec3_t at, vec3_t impulse ) {
	vec3_t arm;
	vec3_t moment;

	if ( !self || self->mass <= 0 )
		return;

	VectorMA( self->s.pos.trDelta, 1.0f / self->mass, impulse, self->s.pos.trDelta );
	VectorSubtract( at, self->s.pos.trBase, arm );
	CrossProduct( arm, impulse, moment );
	VectorAdd( self->angularMomentum, moment, self->angularMomentum );
	G_RallyObject_UpdateAngularVelocity( self );
}

void G_RallyObject_ApplyForce( gentity_t *self, vec3_t force, vec3_t at ) {
	vec3_t arm;
	vec3_t moment;

	if ( !self )
		return;

	VectorSubtract( at, self->s.pos.trBase, arm );
	VectorAdd( self->netForce, force, self->netForce );
	CrossProduct( arm, force, moment );
	VectorAdd( self->netMoment, moment, self->netMoment );
}

qboolean G_RallyObject_ApplyCollisionScaled( gentity_t *self, vec3_t at, vec3_t normal,
		float elasticity, float impulseScale, float *normalImpulseMagnitude ) {
	return G_RallyObject_ApplyCollisionWithOtherInverseMass( self, at, normal,
		elasticity, impulseScale, 0.0f, normalImpulseMagnitude );
}

qboolean G_RallyObject_ApplyCollisionWithOtherInverseMass( gentity_t *self, vec3_t at,
		vec3_t normal, float elasticity, float impulseScale, float otherInverseMass,
		float *normalImpulseMagnitude ) {
	vec3_t contactVelocity;
	vec3_t impulse;
	float denominator;
	float normalSpeed;
	float impulseMagnitude;

	if ( normalImpulseMagnitude )
		*normalImpulseMagnitude = 0.0f;

	if ( !self || self->mass <= 0 )
		return qfalse;

	G_RallyObject_ContactVelocity( self, at, contactVelocity );
	normalSpeed = DotProduct( normal, contactVelocity );
	if ( normalSpeed >= -0.01f )
		return qfalse;

	/* Include both translation and rotation about the actual contact point. */
	denominator = G_RallyObject_ContactInverseMass( self, at, normal ) +
		( otherInverseMass > 0.0f ? otherInverseMass : 0.0f );

	if ( denominator <= 0.000001f )
		return qfalse;

	elasticity = Com_Clamp( 0.0f, 1.0f, elasticity );
	if ( normalSpeed > -8.0f )
		elasticity = 0.0f; /* suppress low-speed resting-contact jitter */
	impulseMagnitude = -( 1.0f + elasticity ) * normalSpeed / denominator;
	impulseScale = Com_Clamp( 0.0f, 1.0f, impulseScale );
	impulseMagnitude *= impulseScale;
	if ( impulseMagnitude <= 0.0f )
		return qfalse;
	VectorScale( normal, impulseMagnitude, impulse );
	G_RallyObject_ApplyImpulse( self, at, impulse );
	if ( normalImpulseMagnitude )
		*normalImpulseMagnitude = impulseMagnitude;

	return qtrue;
}

qboolean G_RallyObject_ApplyCollision( gentity_t *self, vec3_t at, vec3_t normal,
		float elasticity, float *normalImpulseMagnitude ) {
	return G_RallyObject_ApplyCollisionScaled( self, at, normal, elasticity, 1.0f,
		normalImpulseMagnitude );
}

void G_RallyObject_ApplyContactFriction( gentity_t *self, vec3_t at, vec3_t normal,
		float normalImpulseMagnitude, float friction ) {
	vec3_t contactVelocity;
	vec3_t tangentVelocity;
	vec3_t tangentImpulse;
	float tangentSpeed;
	float denominator;
	float frictionImpulse;

	if ( !self || normalImpulseMagnitude <= 0.0f || friction <= 0.0f )
		return;

	G_RallyObject_ContactVelocity( self, at, contactVelocity );
	VectorMA( contactVelocity, -DotProduct( contactVelocity, normal ), normal, tangentVelocity );
	tangentSpeed = VectorNormalize( tangentVelocity );
	if ( tangentSpeed <= 0.001f )
		return;

	denominator = G_RallyObject_ContactInverseMass( self, at, tangentVelocity );
	if ( denominator <= 0.000001f )
		return;
	friction = Com_Clamp( 0.0f, 4.0f, friction );
	frictionImpulse = Com_Clamp( 0.0f, tangentSpeed / denominator,
		friction * normalImpulseMagnitude );
	VectorScale( tangentVelocity, -frictionImpulse, tangentImpulse );
	G_RallyObject_ApplyImpulse( self, at, tangentImpulse );
}

static void G_RallyObject_ApplyAngularResistanceAxis( gentity_t *self, vec3_t axis,
		float angularSpeed, float coefficient, float maximumImpulse ) {
	vec3_t inverseAxis[3];
	vec3_t angularImpulse;
	vec3_t deltaVelocity;
	float inverseWorld[3][3];
	float inverseInertia;
	float impulseMagnitude;

	if ( angularSpeed <= 0.001f || coefficient <= 0.0f || maximumImpulse <= 0.0f ||
		!G_RallyObject_InverseWorldInertia( self, inverseAxis, inverseWorld ) )
		return;

	VectorRotate( axis, inverseWorld, deltaVelocity );
	inverseInertia = DotProduct( axis, deltaVelocity );
	if ( inverseInertia <= 0.000001f )
		return;
	impulseMagnitude = Com_Clamp( 0.0f, angularSpeed / inverseInertia,
		coefficient * maximumImpulse );
	VectorScale( axis, -impulseMagnitude, angularImpulse );
	VectorAdd( self->angularMomentum, angularImpulse, self->angularMomentum );
	G_RallyObject_UpdateAngularVelocity( self );
}

void G_RallyObject_ApplyAngularContactResistance( gentity_t *self, vec3_t normal,
		float normalImpulseMagnitude ) {
	vec3_t omega;
	vec3_t rollingAxis;
	vec3_t spinAxis;
	float size[3];
	float radius;
	float spinSpeed;
	float rollingSpeed;
	float spinImpulseLimit;
	float rollingImpulseLimit;
	int i;

	if ( !self || normalImpulseMagnitude <= 0.0f ||
		( self->rollingFriction <= 0.0f && self->spinningFriction <= 0.0f ) )
		return;

	G_RallyObject_UpdateAngularVelocity( self );
	VectorCopy( self->s.apos.trDelta, omega );
	spinSpeed = DotProduct( omega, normal );
	VectorScale( normal, spinSpeed, spinAxis );
	spinSpeed = VectorNormalize( spinAxis );
	VectorSubtract( omega, spinAxis, rollingAxis );
	rollingSpeed = VectorNormalize( rollingAxis );

	radius = 4096.0f;
	for ( i = 0; i < 3; i++ ) {
		size[i] = 0.5f * ( self->r.maxs[i] - self->r.mins[i] );
		if ( size[i] < radius )
			radius = size[i];
	}
	if ( radius < 0.1f )
		radius = 0.1f;
	rollingImpulseLimit = normalImpulseMagnitude * radius;
	spinImpulseLimit = rollingImpulseLimit;

	G_RallyObject_ApplyAngularResistanceAxis( self, rollingAxis, rollingSpeed,
		self->rollingFriction, rollingImpulseLimit );
	G_RallyObject_ApplyAngularResistanceAxis( self, spinAxis, spinSpeed,
		self->spinningFriction, spinImpulseLimit );
}

static void G_RallyObject_ResolvePairCollision( gentity_t *self, gentity_t *other,
		vec3_t normal, vec3_t contact ) {
	vec3_t selfVelocity;
	vec3_t otherVelocity;
	vec3_t relativeVelocity;
	vec3_t tangentVelocity;
	vec3_t normalImpulse;
	vec3_t frictionImpulseVector;
	float normalSpeed;
	float denominator;
	float impulseMagnitude;
	float tangentSpeed;
	float frictionImpulse;
	float elasticity;
	float friction;

	if ( !other || !other->moveable || other->s.eType != ET_SCRIPTED || other->mass <= 0 )
		return;
	G_RallyObject_Wake( self );

	G_RallyObject_ContactVelocity( self, contact, selfVelocity );
	G_RallyObject_ContactVelocity( other, contact, otherVelocity );
	VectorSubtract( selfVelocity, otherVelocity, relativeVelocity );
	normalSpeed = DotProduct( relativeVelocity, normal );
	if ( normalSpeed >= -0.01f )
		return;

	denominator = G_RallyObject_ContactInverseMass( self, contact, normal ) +
		G_RallyObject_ContactInverseMass( other, contact, normal );
	if ( denominator <= 0.000001f )
		return;

	elasticity = Com_Clamp( 0.0f, 1.0f, ( self->elasticity + other->elasticity ) * 0.5f );
	if ( normalSpeed > -8.0f )
		elasticity = 0.0f;
	impulseMagnitude = -( 1.0f + elasticity ) * normalSpeed / denominator;
	VectorScale( normal, impulseMagnitude, normalImpulse );
	G_RallyObject_ApplyImpulse( self, contact, normalImpulse );
	VectorScale( normalImpulse, -1.0f, frictionImpulseVector );
	G_RallyObject_ApplyImpulse( other, contact, frictionImpulseVector );

	/* Friction acts on the relative velocity at contact and transfers spin
	 * and translation together, instead of independently damping either one. */
	G_RallyObject_ContactVelocity( self, contact, selfVelocity );
	G_RallyObject_ContactVelocity( other, contact, otherVelocity );
	VectorSubtract( selfVelocity, otherVelocity, relativeVelocity );
	VectorMA( relativeVelocity, -DotProduct( relativeVelocity, normal ), normal, tangentVelocity );
	tangentSpeed = VectorNormalize( tangentVelocity );
	friction = Com_Clamp( 0.0f, 4.0f, ( self->friction + other->friction ) * 0.5f );
	if ( tangentSpeed > 0.001f && friction > 0.0f ) {
		denominator = G_RallyObject_ContactInverseMass( self, contact, tangentVelocity ) +
			G_RallyObject_ContactInverseMass( other, contact, tangentVelocity );
		if ( denominator > 0.000001f ) {
			frictionImpulse = Com_Clamp( 0.0f, tangentSpeed / denominator,
				friction * impulseMagnitude );
			VectorScale( tangentVelocity, -frictionImpulse, frictionImpulseVector );
			G_RallyObject_ApplyImpulse( self, contact, frictionImpulseVector );
			VectorScale( frictionImpulseVector, -1.0f, frictionImpulseVector );
			G_RallyObject_ApplyImpulse( other, contact, frictionImpulseVector );
		}
	}
	G_RallyObject_ApplyAngularContactResistance( self, normal, impulseMagnitude );
	G_RallyObject_ApplyAngularContactResistance( other, normal, impulseMagnitude );
}

static void G_RallyObject_TraceShape( gentity_t *self, trace_t *trace,
		const vec3_t start, const vec3_t end ) {
	if ( self->collisionHullVerts && self->collisionHullVertCount >= 4 ) {
		trap_TraceConvex( trace, start, end, self->collisionHullMins, self->collisionHullMaxs,
			self->collisionHullVerts, self->collisionHullVertCount, self->s.apos.trBase,
			self->s.number, MASK_PLAYERSOLID );
	} else {
		trap_Trace( trace, start, self->r.mins, self->r.maxs, end,
			self->s.number, MASK_PLAYERSOLID );
	}
}

static void G_RallyObject_VerticalSupportOffset( gentity_t *self, vec3_t offset ) {
	vec3_t axis[3];
	vec3_t candidate;
	vec3_t local;
	float minimumZ;
	int supportCount;
	int vertexCount;
	int vertex;
	int i;

	AnglesToAxis( self->s.apos.trBase, axis );
	vertexCount = ( self->collisionHullVerts && self->collisionHullVertCount >= 4 )
		? self->collisionHullVertCount : 8;
	minimumZ = 1e30f;
	for ( vertex = 0; vertex < vertexCount; vertex++ ) {
		if ( self->collisionHullVerts && self->collisionHullVertCount >= 4 ) {
			VectorCopy( self->collisionHullVerts[vertex], local );
		} else {
			for ( i = 0; i < 3; i++ )
				local[i] = ( vertex & ( 1 << i ) ) ? self->r.maxs[i] : self->r.mins[i];
		}
		VectorScale( axis[0], local[0], candidate );
		VectorMA( candidate, local[1], axis[1], candidate );
		VectorMA( candidate, local[2], axis[2], candidate );
		if ( candidate[2] < minimumZ )
			minimumZ = candidate[2];
	}

	VectorClear( offset );
	supportCount = 0;
	for ( vertex = 0; vertex < vertexCount; vertex++ ) {
		if ( self->collisionHullVerts && self->collisionHullVertCount >= 4 ) {
			VectorCopy( self->collisionHullVerts[vertex], local );
		} else {
			for ( i = 0; i < 3; i++ )
				local[i] = ( vertex & ( 1 << i ) ) ? self->r.maxs[i] : self->r.mins[i];
		}
		VectorScale( axis[0], local[0], candidate );
		VectorMA( candidate, local[1], axis[1], candidate );
		VectorMA( candidate, local[2], axis[2], candidate );
		if ( candidate[2] <= minimumZ + 0.25f ) {
			VectorAdd( offset, candidate, offset );
			supportCount++;
		}
	}
	if ( supportCount > 0 )
		VectorScale( offset, 1.0f / supportCount, offset );
}

static qboolean G_RallyObject_ResolveGroundSupport( gentity_t *self, float time,
		qboolean applyFriction ) {
	static const float rayStartOffsets[] = { 4.0f, 8.0f, 16.0f, 32.0f };
	trace_t trace;
	vec3_t zero;
	vec3_t supportOffset;
	vec3_t supportPoint;
	vec3_t rayStart;
	vec3_t rayEnd;
	vec3_t normal;
	vec3_t contactVelocity;
	float gap;
	float correction;
	float normalSpeed;
	float normalImpulse;
	float collisionImpulse;
	gentity_t *support;
	qboolean wasGrounded;
	int i;

	if ( !self || !self->moveable || self->mass <= 0 || time <= 0.0f )
		return qfalse;

	wasGrounded = self->physicsGrounded;
	G_RallyObject_VerticalSupportOffset( self, supportOffset );
	VectorAdd( self->s.pos.trBase, supportOffset, supportPoint );
	VectorClear( zero );

	for ( i = 0; i < ARRAY_LEN( rayStartOffsets ); i++ ) {
		VectorCopy( supportPoint, rayStart );
		VectorCopy( supportPoint, rayEnd );
		rayStart[2] += rayStartOffsets[i];
		rayEnd[2] -= 8.0f;
		trap_Trace( &trace, rayStart, zero, zero, rayEnd,
			self->s.number, MASK_PLAYERSOLID );
		if ( trace.startsolid || trace.allsolid || trace.fraction >= 1.0f ||
			trace.plane.normal[2] < 0.55f )
			continue;

		support = NULL;
		if ( trace.entityNum >= 0 && trace.entityNum < ENTITYNUM_MAX_NORMAL )
			support = &g_entities[trace.entityNum];
		if ( trace.entityNum != ENTITYNUM_WORLD &&
			( !support || !support->inuse || support->client || support->moveable ||
				support->s.eType == ET_MOVER ) )
			continue;

		gap = trace.endpos[2] - supportPoint[2];
		/* Only repair a shallow penetration or hold a true near-contact. Do not
		 * magnetize a prop to the floor while it is still airborne. */
		if ( gap < -0.75f || gap > 8.0f )
			continue;

		VectorCopy( trace.plane.normal, normal );
		if ( VectorNormalize( normal ) == 0.0f )
			continue;
		correction = 0.0f;
		if ( gap > 0.0f || gap < -RALLY_OBJECT_CONTACT_EPSILON ) {
			correction = gap + RALLY_OBJECT_CONTACT_EPSILON;
			self->s.pos.trBase[2] += correction;
			self->s.pos.trTime = level.time;
		}

		VectorCopy( trace.endpos, supportPoint );
		G_RallyObject_ContactVelocity( self, supportPoint, contactVelocity );
		normalSpeed = DotProduct( contactVelocity, normal );
		if ( normalSpeed > 2.0f ) {
			self->physicsGrounded = qfalse;
			return qfalse;
		}

		normalImpulse = self->mass * CP_CURRENT_GRAVITY * time /
			Com_Clamp( 0.55f, 1.0f, normal[2] );
		collisionImpulse = 0.0f;
		G_RallyObject_ApplyCollision( self, supportPoint, normal, 0.0f,
			&collisionImpulse );
		if ( collisionImpulse > normalImpulse )
			normalImpulse = collisionImpulse;
		if ( applyFriction ) {
			G_RallyObject_ApplyContactFriction( self, supportPoint, normal,
				normalImpulse, self->friction );
			G_RallyObject_ApplyAngularContactResistance( self, normal, normalImpulse );
		}

		self->physicsGrounded = normal[2] >= 0.55f &&
			Q_fabs( self->s.pos.trDelta[2] ) <= 2.0f;
		if ( g_developer.integer &&
			( !wasGrounded || Q_fabs( correction ) > 0.25f ) &&
			level.time >= self->scriptedDebugGroundLogTime ) {
			Com_Printf( "rally_scripted_object: ground support ent=%d mode=%s gap=%.2f correction=%.2f normal=(%.2f %.2f %.2f) impulse=%.1f grounded=%d v=(%.1f %.1f %.1f) omega=(%.2f %.2f %.2f)\n",
				self->s.number, applyFriction ? "friction" : "recover", gap,
				correction, normal[0], normal[1], normal[2], normalImpulse,
				self->physicsGrounded, self->s.pos.trDelta[0], self->s.pos.trDelta[1],
				self->s.pos.trDelta[2], self->s.apos.trDelta[0],
				self->s.apos.trDelta[1], self->s.apos.trDelta[2] );
			self->scriptedDebugGroundLogTime = level.time + 500;
		}
		return qtrue;
	}

	self->physicsGrounded = qfalse;
	return qfalse;
}

qboolean G_RallyObject_ApplyGroundSupport( gentity_t *self, float time ) {
	return G_RallyObject_ResolveGroundSupport( self, time, qtrue );
}

static qboolean G_RallyObject_RecoverStartSolid( gentity_t *self,
		float *recoveryHeight, qboolean *grounded ) {
	static const float liftSteps[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
	trace_t test;
	vec3_t candidate;
	vec3_t end;
	qboolean stableSupport;
	gentity_t *support;
	int i;

	for ( i = 0; i < ARRAY_LEN( liftSteps ); i++ ) {
		VectorCopy( self->s.pos.trBase, candidate );
		candidate[2] += liftSteps[i];
		G_RallyObject_TraceShape( self, &test, candidate, candidate );
		if ( test.startsolid || test.allsolid || test.fraction < 1.0f )
			continue;

		/* First escape the overlap. Then probe down so a floor contact gets a
		 * real normal instead of repeatedly clearing all linear velocity. */
		VectorCopy( candidate, end );
		end[2] -= 2.0f;
		G_RallyObject_TraceShape( self, &test, candidate, end );
		stableSupport = qfalse;
		if ( !test.startsolid && !test.allsolid && test.fraction < 1.0f &&
			test.plane.normal[2] >= 0.55f ) {
			if ( test.entityNum == ENTITYNUM_WORLD ) {
				stableSupport = qtrue;
			} else if ( test.entityNum >= 0 && test.entityNum < ENTITYNUM_MAX_NORMAL ) {
				support = &g_entities[test.entityNum];
				stableSupport = support->inuse && !support->client && !support->moveable &&
					support->s.eType != ET_MOVER;
			}
		}

		if ( stableSupport ) {
			VectorMA( test.endpos, RALLY_OBJECT_CONTACT_EPSILON,
				test.plane.normal, self->s.pos.trBase );
			if ( self->s.pos.trDelta[2] < 0.0f )
				self->s.pos.trDelta[2] = 0.0f;
			self->physicsGrounded = qtrue;
		} else {
			VectorCopy( candidate, self->s.pos.trBase );
			self->physicsGrounded = qfalse;
		}
		self->s.pos.trTime = level.time;
		*recoveryHeight = liftSteps[i];
		*grounded = stableSupport;
		return qtrue;
	}

	return qfalse;
}

void G_RallyObject_TracePhysics( gentity_t *self, float time ) {
	trace_t trace;
	vec3_t start;
	vec3_t end;
	vec3_t normal;
	vec3_t contactVelocity;
	vec3_t contactPoint;
	vec3_t obstacleVelocity;
	vec3_t velocityBefore;
	float incomingNormalSpeed;
	float normalImpulseMagnitude;
	float contactInverseMass;
	float recoveryHeight;
	qboolean debugTrace;
	qboolean recoveredGrounded;
	gentity_t *hit;

	if ( !self || !self->moveable || time <= 0.0f )
		return;
	if ( self->s.pos.trDelta[2] > 2.0f || self->s.pos.trDelta[2] < -2.0f )
		self->physicsGrounded = qfalse;

	VectorCopy( self->s.pos.trBase, start );
	VectorMA( start, time, self->s.pos.trDelta, end );
	G_RallyObject_TraceShape( self, &trace, start, end );
	debugTrace = g_developer.integer && level.time >= self->scriptedDebugTraceLogTime &&
		( trace.fraction < 1.0f || trace.startsolid || trace.allsolid );
	if ( debugTrace ) {
		VectorCopy( self->s.pos.trDelta, velocityBefore );
		self->scriptedDebugTraceLogTime = level.time + 500;
		Com_Printf( "rally_scripted_object: trace ent=%d fraction=%.3f startsolid=%d allsolid=%d hit=%d normal=(%.2f %.2f %.2f) pos=(%.1f %.1f %.1f) vIn=(%.1f %.1f %.1f)\n",
			self->s.number, trace.fraction, trace.startsolid, trace.allsolid,
			trace.entityNum, trace.plane.normal[0], trace.plane.normal[1],
			trace.plane.normal[2], self->s.pos.trBase[0], self->s.pos.trBase[1],
			self->s.pos.trBase[2], velocityBefore[0], velocityBefore[1], velocityBefore[2] );
	}

	if ( trace.fraction >= 1.0f && !trace.startsolid && !trace.allsolid ) {
		VectorCopy( end, self->s.pos.trBase );
		self->s.pos.trTime = level.time;
		return;
	}

	VectorCopy( trace.plane.normal, normal );
	if ( VectorNormalize( normal ) == 0.0f ) {
		/* A zero normal on an allsolid start is common at resting contact. Try
		 * a small upward depenetration and floor probe before stopping motion. */
		if ( G_RallyObject_RecoverStartSolid( self, &recoveryHeight,
			&recoveredGrounded ) ) {
			if ( debugTrace )
				Com_Printf( "rally_scripted_object: trace response ent=%d recovered upward %.2f grounded=%d; vOut=(%.1f %.1f %.1f)\n",
					self->s.number, recoveryHeight, recoveredGrounded,
					self->s.pos.trDelta[0], self->s.pos.trDelta[1],
					self->s.pos.trDelta[2] );
			return;
		}

		/* If no free position can be found within the recovery limit, stop to
		 * avoid tunnelling through an unresolved wall or embedded prop. A point
		 * support query may still recover a shallow floor overlap and preserve
		 * the motion while restoring ground friction. */
		if ( G_RallyObject_ResolveGroundSupport( self, time, qfalse ) ) {
			if ( debugTrace )
				Com_Printf( "rally_scripted_object: trace response ent=%d recovered via ground support; vOut=(%.1f %.1f %.1f) omega=(%.2f %.2f %.2f)\n",
					self->s.number, self->s.pos.trDelta[0], self->s.pos.trDelta[1],
					self->s.pos.trDelta[2], self->s.apos.trDelta[0],
					self->s.apos.trDelta[1], self->s.apos.trDelta[2] );
			return;
		}

		VectorClear( self->s.pos.trDelta );
		if ( debugTrace )
			Com_Printf( "rally_scripted_object: trace response ent=%d recovery failed; stopped linear velocity; omega=(%.2f %.2f %.2f)\n",
				self->s.number, self->s.apos.trDelta[0], self->s.apos.trDelta[1],
				self->s.apos.trDelta[2] );
		return;
	}

	VectorMA( trace.endpos, RALLY_OBJECT_CONTACT_EPSILON, normal, self->s.pos.trBase );
	self->s.pos.trTime = level.time;
	hit = ( trace.entityNum >= 0 && trace.entityNum < ENTITYNUM_MAX_NORMAL )
		? &g_entities[trace.entityNum] : NULL;
	if ( hit && ( hit->flags & FL_EXTRA_BBOX ) &&
		hit->r.ownerNum >= 0 && hit->r.ownerNum < level.num_entities ) {
		hit = &g_entities[hit->r.ownerNum];
	}

	if ( hit && hit->inuse && hit != self && hit->s.eType == ET_SCRIPTED &&
		hit->moveable && !( hit->s.eFlags & EF_DEAD ) ) {
		vec3_t selfContact;
		vec3_t otherContact;
		vec3_t oppositeNormal;
		vec3_t pairContact;

		G_RallyObject_Wake( hit );
		VectorScale( normal, -1.0f, oppositeNormal );
		G_RallyObject_SupportContact( self, trace.endpos, normal, selfContact );
		G_RallyObject_SupportContact( hit, hit->s.pos.trBase, oppositeNormal, otherContact );
		VectorAdd( selfContact, otherContact, pairContact );
		VectorScale( pairContact, 0.5f, pairContact );
		G_RallyObject_ResolvePairCollision( self, hit, normal, pairContact );
		if ( debugTrace )
			Com_Printf( "rally_scripted_object: pair response ent=%d hit=%d vOut=(%.1f %.1f %.1f) omega=(%.2f %.2f %.2f)\n",
				self->s.number, hit->s.number, self->s.pos.trDelta[0],
				self->s.pos.trDelta[1], self->s.pos.trDelta[2],
				self->s.apos.trDelta[0], self->s.apos.trDelta[1], self->s.apos.trDelta[2] );
		return;
	}

	VectorClear( obstacleVelocity );
	if ( hit && hit->client )
		VectorCopy( hit->s.pos.trDelta, obstacleVelocity );
	VectorSubtract( self->s.pos.trDelta, obstacleVelocity, self->s.pos.trDelta );
	G_RallyObject_SupportContact( self, trace.endpos, normal, contactPoint );
	G_RallyObject_ContactVelocity( self, contactPoint, contactVelocity );
	incomingNormalSpeed = DotProduct( contactVelocity, normal );
	normalImpulseMagnitude = 0.0f;
	G_RallyObject_ApplyCollision( self, contactPoint, normal, self->elasticity,
		&normalImpulseMagnitude );
	if ( incomingNormalSpeed < 0.0f && normalImpulseMagnitude <= 0.0f ) {
		/* Resolve even a very slow inward contact without injecting bounce. */
		contactInverseMass = G_RallyObject_ContactInverseMass( self, contactPoint, normal );
		if ( contactInverseMass > 0.000001f ) {
			normalImpulseMagnitude = -incomingNormalSpeed / contactInverseMass;
			VectorScale( normal, normalImpulseMagnitude, contactVelocity );
			G_RallyObject_ApplyImpulse( self, contactPoint, contactVelocity );
		}
	}
	G_RallyObject_ApplyContactFriction( self, contactPoint, normal,
		normalImpulseMagnitude, self->friction );
	G_RallyObject_ApplyAngularContactResistance( self, normal, normalImpulseMagnitude );
	VectorAdd( self->s.pos.trDelta, obstacleVelocity, self->s.pos.trDelta );
	if ( normal[2] >= 0.55f && ( !hit || ( !hit->client && hit->s.eType != ET_MOVER ) ) ) {
		if ( self->s.pos.trDelta[2] < 0.0f )
			self->s.pos.trDelta[2] = 0.0f;
		if ( Q_fabs( self->s.pos.trDelta[2] ) <= 2.0f )
			self->physicsGrounded = qtrue;
	}
	if ( debugTrace )
		Com_Printf( "rally_scripted_object: world response ent=%d hit=%d vOut=(%.1f %.1f %.1f) omega=(%.2f %.2f %.2f) impulse=%.2f\n",
			self->s.number, hit ? hit->s.number : ENTITYNUM_WORLD,
			self->s.pos.trDelta[0], self->s.pos.trDelta[1], self->s.pos.trDelta[2],
			self->s.apos.trDelta[0], self->s.apos.trDelta[1], self->s.apos.trDelta[2],
			normalImpulseMagnitude );
}

void G_RallyObject_IntegratePhysics( gentity_t *self, float time ) {
	vec3_t axis[3];
	float inverseWorld[3][3];
	float rotationStep[3][3];
	float rotationResult[3][3];
	int i;

	if ( !self || !self->moveable || self->mass <= 0 || time <= 0.0f )
		return;

	VectorMA( self->s.pos.trDelta, time / self->mass, self->netForce, self->s.pos.trDelta );

	if ( VectorLengthSquared( self->netMoment ) > 0.000001f ||
		VectorLengthSquared( self->angularMomentum ) > 0.000001f ) {
		if ( G_RallyObject_InverseWorldInertia( self, axis, inverseWorld ) ) {
			VectorMA( self->angularMomentum, time, self->netMoment, self->angularMomentum );
			VectorRotate( self->angularMomentum, inverseWorld, self->s.apos.trDelta );

			rotationStep[0][0] = 0.0f;
			rotationStep[0][1] = -time * self->s.apos.trDelta[2];
			rotationStep[0][2] =  time * self->s.apos.trDelta[1];
			rotationStep[1][0] =  time * self->s.apos.trDelta[2];
			rotationStep[1][1] = 0.0f;
			rotationStep[1][2] = -time * self->s.apos.trDelta[0];
			rotationStep[2][0] = -time * self->s.apos.trDelta[1];
			rotationStep[2][1] =  time * self->s.apos.trDelta[0];
			rotationStep[2][2] = 0.0f;
			MatrixMultiply( rotationStep, axis, rotationResult );
			MatrixAdd( axis, rotationResult, axis );
			OrthonormalizeOrientation( axis );
			OrientationToAngles( axis, self->s.apos.trBase );
		}
	}

	for ( i = 0; i < 3; i++ ) {
		if ( self->s.pos.trDelta[i] != 0.0f ) {
			VectorCopy( self->s.pos.trDelta, self->lastNonZeroVelocity );
			break;
		}
	}
}
