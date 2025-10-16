/*
==========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2025 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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
==========================================================================
*/

#include "g_local.h"

/*
 * Generic physics routines for moveable rally objects.
 * These routines originally lived in g_rally_scripted_objects.c but are
 * separated here so they can be reused by other game code.
 */

void G_RallyObject_ApplyForce( gentity_t *self, vec3_t force, vec3_t at ) {
    vec3_t arm, moment;

    VectorSubtract( at, self->s.pos.trBase, arm );

    VectorAdd( self->netForce, force, self->netForce );

    CrossProduct( arm, force, moment );
    VectorAdd( self->netMoment, moment, self->netMoment );
}

qboolean G_RallyObject_ApplyCollision( gentity_t *self, vec3_t at, vec3_t normal, float elasticity ) {
    vec3_t arm;
    vec3_t vP1;
    vec3_t impulse, impulseMoment;
    vec3_t cross, cross2;
    float   impulseNum, impulseDen, dot;

    /* temp for inverseWorldInertiaTensor */
    vec3_t  axis[3];
    vec3_t  inverseBodyInertiaTensor;
    vec3_t  inverseWorldInertiaTensor[3];

    VectorSubtract( at, self->s.pos.trBase, arm );

    CrossProduct( self->s.apos.trDelta, arm, cross );
    VectorAdd( self->s.pos.trDelta, cross, vP1 );

    {
        float   m[3][3], m2[3][3];
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

    /* added from collision */
    VectorClear(impulse);
    dot = DotProduct(normal, vP1);
    if ( dot < -8 ){
        impulseNum = -(1.0f + elasticity) * DotProduct(normal, vP1);

        CrossProduct( arm, normal, cross );
        VectorRotate( cross, inverseWorldInertiaTensor, cross2 );
        CrossProduct( cross2, arm, cross );
        impulseDen = 1.0f / self->mass + DotProduct( cross, normal );

        VectorScale( normal, impulseNum / impulseDen, impulse );
    }
    else {
        /* not hitting surface */
        return qfalse;
    }

    /* apply impulse to primary quantities */
    VectorMA( self->s.pos.trDelta, 1.0 / self->mass, impulse, self->s.pos.trDelta );
    CrossProduct( arm, impulse, impulseMoment );
    VectorAdd( self->angularMomentum, impulseMoment, self->angularMomentum );

    /* compute affected auxiliary quantities */
    /* VectorRotate( self->angularMomentum, inverseWorldInertiaTensor, self->s.apos.trDelta ); */

    return qtrue;
}

/* Traces a cone at the current position and angles. */
static int G_RallyObject_TraceCone( trace_t *tr, vec3_t origin, vec3_t angles, vec3_t mins, vec3_t maxs, int passEntityNum, int contentmask )
{
    vec3_t          forward, right, up;
    vec3_t          start, end, bottom, top;
    float           radius;

    vec3_t          normal, intersect;
    float           firstHit;
    int             numHits;

    VectorClear( normal );
    VectorClear( intersect );
    firstHit = 1.0f;
    numHits = 0;

    AngleVectors( angles, forward, right, up );

    radius = ( maxs[0] - mins[0] ) / 2.0f;
    VectorMA( origin, mins[2], up, bottom );
    VectorMA( origin, maxs[2], up, top );

    VectorCopy( origin, start );

    /* right */
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

    /* left */
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

    /* front */
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

    /* back */
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
        VectorNormalize2( normal, tr->plane.normal );
        VectorScale( intersect, 1.0f / numHits, tr->endpos );
        tr->fraction = firstHit;
        return numHits;
    }

    return 0;
}

void G_RallyObject_TracePhysics( gentity_t *self, float time )
{
    trace_t         tr;
    vec3_t          dir;
    vec3_t          start, end;
    float           dot;
    float           moveDist, dist;

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

    if( tr.fraction < 1.0f && tr.entityNum < ENTITYNUM_MAX_NORMAL ) {
        gentity_t *hit = &g_entities[ tr.entityNum ];

        if( hit->client != NULL || ( hit->r.svFlags & SVF_BOT ) ) {
            vec3_t invNormal;
            float vSelf, vHit, closing, totalDamage, damageSelfF, damageHitF;
            int damageSelf, damageHit;

            /* store normal velocities before collision response */
            vSelf = DotProduct( self->s.pos.trDelta, tr.plane.normal );
            vHit = -DotProduct( hit->s.pos.trDelta, tr.plane.normal );

            if( G_RallyObject_ApplyCollision( self, tr.endpos, tr.plane.normal, self->elasticity ) ) {
                VectorScale( tr.plane.normal, -1.0f, invNormal );
                G_RallyObject_ApplyCollision( hit, tr.endpos, invNormal, self->elasticity );

                if( vSelf < 0.0f ) {
                    vSelf = 0.0f;
                }
                if( vHit < 0.0f ) {
                    vHit = 0.0f;
                }
                closing = vSelf + vHit;
                if( closing > 0.0f ) {
                    totalDamage = ( closing + g_vehicleDamageOffset.value ) * g_vehicleDamageScale.value;
                    totalDamage *= g_derbyDamageFactor.value;
                    damageSelfF = totalDamage * ( vHit / closing );
                    damageHitF  = totalDamage * ( vSelf / closing );
                    damageHitF  *= g_derbyRammerDamageRatio.value;
                    damageSelfF *= 2.0f - g_derbyRammerDamageRatio.value;
                    damageSelf = (int)max( 1.0f, damageSelfF );
                    damageHit = (int)max( 1.0f, damageHitF );

                    if( !self->takedamage ) {
                        self->takedamage = qtrue;
                        if( self->health <= 0 ) {
                            self->health = g_vehicleHealth.integer;
                        }
                    }
                    if( !hit->takedamage ) {
                        hit->takedamage = qtrue;
                        if( hit->health <= 0 ) {
                            hit->health = g_vehicleHealth.integer;
                        }
                    }

                    G_Damage( self, hit, hit, tr.plane.normal, tr.endpos, damageSelf, DAMAGE_NO_ARMOR, MOD_VEHICLE_COLLISION );
                    G_Damage( hit, self, self, invNormal, tr.endpos, damageHit, DAMAGE_NO_ARMOR, MOD_VEHICLE_COLLISION );
                }
            }

            return;
        }
    }

    if( tr.startsolid || tr.allsolid )
    {
        trap_Trace( &tr, start, NULL, NULL, end, self->s.number, MASK_PLAYERSOLID & ~CONTENTS_BODY );
        if( tr.fraction < 1.0f || G_RallyObject_TraceCone( &tr, end, self->s.apos.trBase, self->r.mins, self->r.maxs, self->s.number, MASK_PLAYERSOLID & ~CONTENTS_BODY ) )
        {
            /* Debug: Uncomment to visualize collision points */
            /* G_SetOrigin( &g_entities[level.testModelID], tr.endpos ); */
            /* trap_LinkEntity( &g_entities[level.testModelID] ); */
        }
    }
    else if( tr.fraction < 1.0f || G_RallyObject_TraceCone( &tr, end, self->s.apos.trBase, self->r.mins, self->r.maxs, self->s.number, MASK_PLAYERSOLID ) )
    {
        /* Debug: Uncomment to visualize collision points */
        /* G_SetOrigin( &g_entities[level.testModelID], tr.endpos ); */
        /* trap_LinkEntity( &g_entities[level.testModelID] ); */
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
        /* stop the object once it is slow enough */
        VectorMA( self->s.pos.trDelta, -dot * 1.00f, tr.plane.normal, self->s.pos.trDelta );
    }

    /* Apply friction */
    VectorMA( self->s.pos.trDelta, -dot, tr.plane.normal, dir );
    VectorMA( self->netForce, -1.0f * self->mass, dir, self->netForce );

    dot = DotProduct( tr.plane.normal, self->netForce );
    if( dot < 0.00f )
    {
        VectorMA( self->netForce, -dot * 1.00f, tr.plane.normal, self->netForce );
    }
}

void G_RallyObject_IntegratePhysics( gentity_t *self, float time )
{
    float           inverseWorldInertiaTensor[3][3], m[3][3];
    float           m2[3][3];
    vec3_t          axis[3];
    vec3_t          inverseBodyInertiaTensor;
    qboolean        doAngular = qfalse;

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

    /* linear physics */
    VectorMA( self->s.pos.trDelta, time / self->mass, self->netForce, self->s.pos.trDelta );
    VectorMA( self->s.pos.trBase, time, self->s.pos.trDelta, self->s.pos.trBase );
    self->s.pos.trTime = level.time;

    /* angular physics */
    if( doAngular )
    {
        VectorMA( self->angularMomentum, time, self->netMoment, self->angularMomentum );
        VectorRotate( self->angularMomentum, inverseWorldInertiaTensor, self->s.apos.trDelta );

        m[0][0] = 0;                                                                m[0][1] = (time) * -self->s.apos.trDelta[2];    m[0][2] = (time) * self->s.apos.trDelta[1];
        m[1][0] = (time) * self->s.apos.trDelta[2];         m[1][1] = 0;                                                   m[1][2] = (time) * -self->s.apos.trDelta[0];
        m[2][0] = (time) * -self->s.apos.trDelta[1];        m[2][1] = (time) * self->s.apos.trDelta[0];         m[2][2] = 0;

        MatrixMultiply( m, axis, m2 );
        MatrixAdd( axis, m2, axis );
        OrthonormalizeOrientation( axis );
        OrientationToAngles( axis, self->s.apos.trBase );
    }

    if( self->s.pos.trDelta[0] != 0 ||
        self->s.pos.trDelta[1] != 0 ||
        self->s.pos.trDelta[2] != 0 )
    {
        VectorCopy( self->s.pos.trDelta, self->lastNonZeroVelocity );
    }
}

