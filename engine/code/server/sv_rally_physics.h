#ifndef SV_RALLY_PHYSICS_H
#define SV_RALLY_PHYSICS_H

#include "../qcommon/rally_physics.h"

#ifdef __cplusplus
extern "C" {
#endif

void SV_RallyPhysics_Init( float gravity );
void SV_RallyPhysics_Shutdown( void );
void SV_RallyPhysics_Step( float frameSeconds );
qboolean SV_RallyPhysics_CreateBody( int entityNum,
	const rallyPhysicsBodyDesc_t *desc, const vec3_t *vertices, int numVertices );
void SV_RallyPhysics_RemoveBody( int entityNum );
qboolean SV_RallyPhysics_GetBodyState( int entityNum, rallyPhysicsBodyState_t *state );
void SV_RallyPhysics_ResetBody( int entityNum, const vec3_t origin,
	const vec3_t angles, const vec3_t linearVelocity );
qboolean SV_RallyPhysics_HasBody( int entityNum );
qboolean SV_RallyPhysics_Trace( const vec3_t start, const vec3_t mins,
	const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask,
	trace_t *trace );
void SV_RallyPhysics_ApplyImpulse( int entityNum, const vec3_t point,
	const vec3_t impulse );
void SV_RallyPhysics_ApplyVehicleContact( int entityNum, const vec3_t point,
	const vec3_t normal, const vec3_t vehicleVelocity, float vehicleMass,
	float impactScale, vec3_t objectImpulse );

#ifdef __cplusplus
}
#endif

#endif
