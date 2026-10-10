/*
 * Q3Rally scripted-object physics bridge.
 * Kept C-compatible so both native game modules and QVMs can use it.
 */
#ifndef RALLY_PHYSICS_H
#define RALLY_PHYSICS_H

#include "q_shared.h"

/* Collision shape requested by the game module. AUTO keeps the historic
 * behaviour: convex MD3 hull when vertices are supplied, else a box. */
typedef enum {
	RALLY_PHYSICS_SHAPE_AUTO = 0,
	RALLY_PHYSICS_SHAPE_BOX,
	RALLY_PHYSICS_SHAPE_SPHERE
} rallyPhysicsShape_t;

typedef struct {
	vec3_t origin;
	vec3_t angles;
	vec3_t mins;
	vec3_t maxs;
	float mass;
	float restitution;
	float friction;
	float rollingFriction;
	float spinningFriction;
	float linearDamping;
	float angularDamping;
	int contents;
	/* Appended for Autoball; zero-initialised descs keep the old behaviour. */
	int shapeType;			/* rallyPhysicsShape_t */
	float radius;			/* sphere radius; <= 0 derives it from mins/maxs */
	int disableSleep;		/* never deactivate (game balls) */
	float maxLinearSpeed;	/* units/s, <= 0 means unlimited */
} rallyPhysicsBodyDesc_t;

typedef struct {
	vec3_t origin;
	vec3_t angles;
	vec3_t linearVelocity;
	vec3_t angularVelocity;
	qboolean sleeping;
} rallyPhysicsBodyState_t;

#endif
