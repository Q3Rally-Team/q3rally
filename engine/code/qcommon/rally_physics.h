/*
 * Q3Rally scripted-object physics bridge.
 * Kept C-compatible so both native game modules and QVMs can use it.
 */
#ifndef RALLY_PHYSICS_H
#define RALLY_PHYSICS_H

#include "q_shared.h"

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
} rallyPhysicsBodyDesc_t;

typedef struct {
	vec3_t origin;
	vec3_t angles;
	vec3_t linearVelocity;
	vec3_t angularVelocity;
	qboolean sleeping;
} rallyPhysicsBodyState_t;

#endif
