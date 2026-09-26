/*
 * Native Bullet backend for Q3Rally's map-authored scripted objects.
 * Bullet 3.25 is distributed under the zlib license; see thirdparty/bullet3-3.25/LICENSE.txt.
 * The MD3-hull and BSP-brush conversion approach is adapted, with permission,
 * from Noire's ioquake3 Bullet integration.
 */
extern "C" {
#include "server.h"
#include "../qcommon/cm_local.h"
#include "../qcommon/cm_patch.h"
#include "../qcommon/qfiles.h"
}
#include "sv_rally_physics.h"
#include "../thirdparty/bullet3-3.25/src/btBulletDynamicsCommon.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

static const int RALLY_PHYSICS_MAX_HULL_VERTS = 4096;
static const float RALLY_PHYSICS_FIXED_STEP = 1.0f / 120.0f;
static const float RALLY_PHYSICS_MAX_FRAME = 0.1f;

struct rallyBody_t {
	btRigidBody *body;
	btCollisionShape *shape;
	btDefaultMotionState *motionState;
	btVector3 localOriginOffset;
	float mass;
	float friction;
	float restitution;
	int contents;

	rallyBody_t() : body(NULL), shape(NULL), motionState(NULL),
		localOriginOffset(0, 0, 0), mass(0.0f), friction(0.5f), restitution(0.0f),
		contents(0) {}
};

struct rallyWorld_t {
	btDefaultCollisionConfiguration *configuration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDiscreteDynamicsWorld *world;
	btCompoundShape *brushes;
	btRigidBody *brushBody;
	btRigidBody *patchBody;
	btBvhTriangleMeshShape *patchShape;
	btTriangleMesh *patchMesh;
	std::vector<btCollisionShape *> brushChildren;
	float accumulator;

	rallyWorld_t() : configuration(NULL), dispatcher(NULL), broadphase(NULL),
		solver(NULL), world(NULL), brushes(NULL), brushBody(NULL), patchBody(NULL),
		patchShape(NULL), patchMesh(NULL), accumulator(0.0f) {}
};

static rallyWorld_t *rallyWorld;
static rallyBody_t rallyBodies[MAX_GENTITIES];

static float ClampFloat( float value, float minValue, float maxValue ) {
	return value < minValue ? minValue : ( value > maxValue ? maxValue : value );
}

static void AnglesToQuaternion( const vec3_t angles, btQuaternion &rotation ) {
	const float toRadians = 0.008726646259971648f;
	float p = angles[PITCH] * toRadians;
	float y = angles[YAW] * toRadians;
	float r = angles[ROLL] * toRadians;
	float cp = cosf(p), sp = sinf(p);
	float cy = cosf(y), sy = sinf(y);
	float cr = cosf(r), sr = sinf(r);
	rotation.setValue(
		cy * sp * cr + sy * cp * sr,
		sy * cp * cr - cy * sp * sr,
		cy * cp * sr - sy * sp * cr,
		cy * cp * cr + sy * sp * sr );
	rotation.normalize();
}

static void QuaternionToAngles( const btQuaternion &rotation, vec3_t angles ) {
	const float toDegrees = 57.29577951308232f;
	float x = rotation.x(), y = rotation.y(), z = rotation.z(), w = rotation.w();
	float sinPitch = 2.0f * ( w * y - z * x );

	if ( fabsf( sinPitch ) >= 1.0f - 1e-6f ) {
		float sign = sinPitch < 0.0f ? -1.0f : 1.0f;
		angles[ROLL] = 0.0f;
		angles[YAW] = -2.0f * sign * atan2f( x, w ) * toDegrees;
		angles[PITCH] = sign * 90.0f;
		return;
	}

	angles[ROLL] = atan2f( 2.0f * ( w * x + y * z ),
		1.0f - 2.0f * ( x * x + y * y ) ) * toDegrees;
	angles[PITCH] = asinf( sinPitch ) * toDegrees;
	angles[YAW] = atan2f( 2.0f * ( w * z + x * y ),
		1.0f - 2.0f * ( y * y + z * z ) ) * toDegrees;
}

static btVector3 VectorToBullet( const vec3_t value ) {
	return btVector3( value[0], value[1], value[2] );
}

static void VectorFromBullet( const btVector3 &value, vec3_t out ) {
	out[0] = value.x();
	out[1] = value.y();
	out[2] = value.z();
}

static float PlaneDistance( const cplane_t *plane, const btVector3 &point ) {
	return plane->normal[0] * point.x() + plane->normal[1] * point.y() +
		plane->normal[2] * point.z() - plane->dist;
}

static qboolean BrushHullPoints( const cbrush_t *brush, std::vector<btVector3> &points,
	btVector3 &center ) {
	int i, j, k, l;
	qboolean outwardNormals = qtrue;
	float epsilon = 0.1f;

	center.setValue(
		( brush->bounds[0][0] + brush->bounds[1][0] ) * 0.5f,
		( brush->bounds[0][1] + brush->bounds[1][1] ) * 0.5f,
		( brush->bounds[0][2] + brush->bounds[1][2] ) * 0.5f );
	if ( brush->numsides < 4 || !brush->sides )
		return qfalse;

	for ( i = 0; i < brush->numsides; i++ ) {
		const cplane_t *plane = brush->sides[i].plane;
		float centerDistance;
		if ( !plane )
			continue;
		centerDistance = PlaneDistance( plane, center );
		if ( centerDistance > epsilon ) {
			outwardNormals = qfalse;
			break;
		}
	}

	for ( i = 0; i < brush->numsides; i++ ) {
		for ( j = i + 1; j < brush->numsides; j++ ) {
			for ( k = j + 1; k < brush->numsides; k++ ) {
				const cplane_t *p1 = brush->sides[i].plane;
				const cplane_t *p2 = brush->sides[j].plane;
				const cplane_t *p3 = brush->sides[k].plane;
				btVector3 n1, n2, n3, n2n3, point;
				float determinant;
				qboolean valid = qtrue;
				if ( !p1 || !p2 || !p3 )
					continue;
				n1 = VectorToBullet( p1->normal );
				n2 = VectorToBullet( p2->normal );
				n3 = VectorToBullet( p3->normal );
				n2n3 = n2.cross( n3 );
				determinant = n1.dot( n2n3 );
				if ( fabsf( determinant ) < 1e-4f )
					continue;
				point = ( n2n3 * p1->dist + n3.cross( n1 ) * p2->dist +
					n1.cross( n2 ) * p3->dist ) / determinant;

				for ( l = 0; l < brush->numsides; l++ ) {
					const cplane_t *plane = brush->sides[l].plane;
					float distance;
					if ( l == i || l == j || l == k || !plane )
						continue;
					distance = PlaneDistance( plane, point );
					if ( ( outwardNormals && distance > epsilon ) ||
						( !outwardNormals && distance < -epsilon ) ) {
						valid = qfalse;
						break;
					}
				}
				if ( !valid )
					continue;
				for ( size_t p = 0; p < points.size(); p++ ) {
					if ( ( points[p] - point ).length2() < 0.01f ) {
						valid = qfalse;
						break;
					}
				}
				if ( valid )
					points.push_back( point - center );
			}
		}
	}
	return points.size() >= 4 ? qtrue : qfalse;
}

static void AddBrushWorld( void ) {
	int i, j, validBrushes = 0, worldBrushCount = 0;
	std::vector<unsigned char> isWorldBrush;
	if ( !rallyWorld || !rallyWorld->world || cm.numBrushes <= 0 ||
		!cm.leafs || !cm.leafbrushes )
		return;

	/* cm.brushes also contains inline brush models (doors, triggers, etc.).
	 * Only brushes referenced by leaves in the world BSP belong in Bullet's
	 * permanent static world; linked entities are handled separately. */
	isWorldBrush.assign( cm.numBrushes, 0 );
	for ( i = 0; i < cm.numLeafs; i++ ) {
		const cLeaf_t *leaf = &cm.leafs[i];
		for ( j = 0; j < leaf->numLeafBrushes; j++ ) {
			int brushNum = cm.leafbrushes[leaf->firstLeafBrush + j];
			if ( brushNum >= 0 && brushNum < cm.numBrushes &&
				!isWorldBrush[brushNum] ) {
				isWorldBrush[brushNum] = 1;
				worldBrushCount++;
			}
		}
	}

	rallyWorld->brushes = new btCompoundShape( true, cm.numBrushes );
	for ( i = 0; i < cm.numBrushes; i++ ) {
		const cbrush_t *brush = &cm.brushes[i];
		std::vector<btVector3> points;
		btVector3 center;
		btConvexHullShape *shape;
		btTransform childTransform;
		int p;

		if ( !isWorldBrush[i] || !( brush->contents & CONTENTS_SOLID ) ||
			( brush->contents & CONTENTS_TRIGGER ) )
			continue;
		if ( !BrushHullPoints( brush, points, center ) )
			continue;

		shape = new btConvexHullShape();
		for ( p = 0; p < (int)points.size(); p++ )
			shape->addPoint( points[p], false );
		shape->recalcLocalAabb();
		shape->setMargin( 0.04f );
		childTransform.setIdentity();
		childTransform.setOrigin( center );
		rallyWorld->brushes->addChildShape( childTransform, shape );
		rallyWorld->brushChildren.push_back( shape );
		validBrushes++;
	}

	if ( validBrushes ) {
		btRigidBody::btRigidBodyConstructionInfo info( 0.0f, NULL, rallyWorld->brushes );
		rallyWorld->brushBody = new btRigidBody( info );
		rallyWorld->brushBody->setUserIndex( -1 );
		rallyWorld->world->addRigidBody( rallyWorld->brushBody );
	}
	Com_Printf( "rally_bullet: world BSP brush hulls=%d of %d referenced brushes\n",
		validBrushes, worldBrushCount );
}

static void AddPatchWorld( void ) {
	int i, j, patchCount = 0, triangleCount = 0;
	std::vector<unsigned char> isWorldSurface;
	if ( !rallyWorld || !rallyWorld->world || !cm.surfaces ||
		!cm.leafs || !cm.leafsurfaces )
		return;

	/* As with brushes, only world-leaf patch surfaces are static world
	 * geometry; patches owned by inline brush models are not. */
	isWorldSurface.assign( cm.numSurfaces, 0 );
	for ( i = 0; i < cm.numLeafs; i++ ) {
		const cLeaf_t *leaf = &cm.leafs[i];
		for ( j = 0; j < leaf->numLeafSurfaces; j++ ) {
			int surfaceNum = cm.leafsurfaces[leaf->firstLeafSurface + j];
			if ( surfaceNum >= 0 && surfaceNum < cm.numSurfaces )
				isWorldSurface[surfaceNum] = 1;
		}
	}

	rallyWorld->patchMesh = new btTriangleMesh( true, false );
	for ( i = 0; i < cm.numSurfaces; i++ ) {
		cPatch_t *patch = cm.surfaces[i];
		int f;
		if ( !isWorldSurface[i] || !patch || !patch->pc ||
			!( patch->contents & CONTENTS_SOLID ) ||
			( patch->contents & CONTENTS_TRIGGER ) ||
			( patch->surfaceFlags & SURF_NONSOLID ) )
			continue;

		for ( f = 0; f < patch->pc->numFacets; f++ ) {
			facet_t *facet = &patch->pc->facets[f];
			patchPlane_t *surfacePlane;
			std::vector<btVector3> facetPoints;
			int b;

			if ( facet->surfacePlane < 0 || facet->surfacePlane >= patch->pc->numPlanes )
				continue;
			surfacePlane = &patch->pc->planes[facet->surfacePlane];
			for ( b = 0; b < facet->numBorders; b++ ) {
				int borderIndex = facet->borderPlanes[b];
				int nextIndex = facet->borderPlanes[( b + 1 ) % facet->numBorders];
				patchPlane_t *border, *nextBorder;
				btVector3 sn, bn, nn, bnXnn, point;
				float determinant;
				if ( borderIndex < 0 || borderIndex >= patch->pc->numPlanes ||
					nextIndex < 0 || nextIndex >= patch->pc->numPlanes )
					continue;
				border = &patch->pc->planes[borderIndex];
				nextBorder = &patch->pc->planes[nextIndex];
				sn.setValue( surfacePlane->plane[0], surfacePlane->plane[1], surfacePlane->plane[2] );
				bn.setValue( border->plane[0], border->plane[1], border->plane[2] );
				nn.setValue( nextBorder->plane[0], nextBorder->plane[1], nextBorder->plane[2] );
				bnXnn = bn.cross( nn );
				determinant = sn.dot( bnXnn );
				if ( fabsf( determinant ) < 1e-4f )
					continue;
				point = ( bnXnn * surfacePlane->plane[3] +
					nn.cross( sn ) * border->plane[3] +
					sn.cross( bn ) * nextBorder->plane[3] ) / determinant;
				/* A facet is bounded by all of its border planes, not just
				 * the two that generated this vertex. Reject intersections of
				 * bevel planes that lie outside the actual patch polygon. */
				{
					int k;
					qboolean inside = qtrue;
					for ( k = 0; k < facet->numBorders; k++ ) {
						int clipIndex = facet->borderPlanes[k];
						patchPlane_t *clip;
						float distance;
						if ( clipIndex == -1 )
							continue;
						if ( clipIndex < 0 || clipIndex >= patch->pc->numPlanes ) {
							inside = qfalse;
							break;
						}
						clip = &patch->pc->planes[clipIndex];
						distance = clip->plane[0] * point.x() +
							clip->plane[1] * point.y() + clip->plane[2] * point.z() -
							clip->plane[3];
						if ( facet->borderInward[k] ? distance < -0.5f : distance > 0.5f ) {
							inside = qfalse;
							break;
						}
					}
					if ( !inside )
						continue;
				}
				facetPoints.push_back( point );
			}

			if ( facetPoints.size() >= 3 ) {
				size_t p;
				for ( p = 1; p + 1 < facetPoints.size(); p++ ) {
					rallyWorld->patchMesh->addTriangle( facetPoints[0], facetPoints[p],
						facetPoints[p + 1], true );
					triangleCount++;
				}
			}
		}
		patchCount++;
	}

	if ( triangleCount > 0 ) {
		rallyWorld->patchShape = new btBvhTriangleMeshShape( rallyWorld->patchMesh, true, true );
		btRigidBody::btRigidBodyConstructionInfo info( 0.0f, NULL, rallyWorld->patchShape );
		rallyWorld->patchBody = new btRigidBody( info );
		rallyWorld->patchBody->setUserIndex( -1 );
		rallyWorld->world->addRigidBody( rallyWorld->patchBody );
		Com_Printf( "rally_bullet: BSP patch collision facets=%d triangles=%d\n",
			patchCount, triangleCount );
	} else {
		delete rallyWorld->patchMesh;
		rallyWorld->patchMesh = NULL;
	}
}

static btCollisionShape *CreateBodyShape( const rallyPhysicsBodyDesc_t *desc,
	const vec3_t *vertices, int numVertices, btVector3 &localOffset ) {
	int i;
	btConvexHullShape *hull = NULL;
	btVector3 minPoint( BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT );
	btVector3 maxPoint( -BT_LARGE_FLOAT, -BT_LARGE_FLOAT, -BT_LARGE_FLOAT );
	localOffset.setZero();

	if ( vertices && numVertices >= 4 && numVertices <= RALLY_PHYSICS_MAX_HULL_VERTS ) {
		for ( i = 0; i < numVertices; i++ ) {
			btVector3 point = VectorToBullet( vertices[i] );
			minPoint.setMin( point );
			maxPoint.setMax( point );
		}
		localOffset = ( minPoint + maxPoint ) * 0.5f;
		hull = new btConvexHullShape();
		for ( i = 0; i < numVertices; i++ )
			hull->addPoint( VectorToBullet( vertices[i] ) - localOffset, false );
		hull->recalcLocalAabb();
		hull->setMargin( 0.04f );
		return hull;
	}

	{
		btVector3 mins = VectorToBullet( desc->mins );
		btVector3 maxs = VectorToBullet( desc->maxs );
		btVector3 halfExtents = ( maxs - mins ) * 0.5f;
		if ( halfExtents.x() < 0.5f || halfExtents.y() < 0.5f || halfExtents.z() < 0.5f ) {
			halfExtents.setValue( 16.0f, 16.0f, 16.0f );
			mins = -halfExtents;
		}
		localOffset = mins + halfExtents;
		return new btBoxShape( halfExtents );
	}
}

static void DestroyBody( rallyBody_t &entry ) {
	if ( rallyWorld && rallyWorld->world && entry.body )
		rallyWorld->world->removeRigidBody( entry.body );
	delete entry.motionState;
	delete entry.body;
	delete entry.shape;
	entry = rallyBody_t();
}

} // namespace

extern "C" void SV_RallyPhysics_Init( float gravity ) {
	if ( rallyWorld )
		SV_RallyPhysics_Shutdown();

	rallyWorld = new rallyWorld_t();
	rallyWorld->configuration = new btDefaultCollisionConfiguration();
	rallyWorld->dispatcher = new btCollisionDispatcher( rallyWorld->configuration );
	rallyWorld->broadphase = new btDbvtBroadphase();
	rallyWorld->solver = new btSequentialImpulseConstraintSolver();
	rallyWorld->world = new btDiscreteDynamicsWorld( rallyWorld->dispatcher,
		rallyWorld->broadphase, rallyWorld->solver, rallyWorld->configuration );
	rallyWorld->world->setGravity( btVector3( 0.0f, 0.0f, -fabsf( gravity ) ) );
	rallyWorld->world->getSolverInfo().m_numIterations = 12;
	rallyWorld->world->getSolverInfo().m_splitImpulse = true;
	rallyWorld->world->getSolverInfo().m_splitImpulsePenetrationThreshold = -0.04f;
	rallyWorld->world->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;

	for ( int i = 0; i < MAX_GENTITIES; i++ )
		rallyBodies[i] = rallyBody_t();
	AddBrushWorld();
	AddPatchWorld();
	Com_Printf( "rally_bullet: initialized (Bullet 3.25, fixed %.2f Hz, gravity %.1f)\n",
		1.0f / RALLY_PHYSICS_FIXED_STEP, -fabsf( gravity ) );
}

extern "C" void SV_RallyPhysics_Shutdown( void ) {
	int i;
	if ( !rallyWorld )
		return;
	for ( i = 0; i < MAX_GENTITIES; i++ )
		DestroyBody( rallyBodies[i] );
	if ( rallyWorld->brushBody && rallyWorld->world )
		rallyWorld->world->removeRigidBody( rallyWorld->brushBody );
	delete rallyWorld->brushBody;
	if ( rallyWorld->patchBody && rallyWorld->world )
		rallyWorld->world->removeRigidBody( rallyWorld->patchBody );
	delete rallyWorld->patchBody;
	delete rallyWorld->brushes;
	for ( i = 0; i < (int)rallyWorld->brushChildren.size(); i++ )
		delete rallyWorld->brushChildren[i];
	delete rallyWorld->patchShape;
	delete rallyWorld->patchMesh;
	delete rallyWorld->world;
	delete rallyWorld->solver;
	delete rallyWorld->broadphase;
	delete rallyWorld->dispatcher;
	delete rallyWorld->configuration;
	delete rallyWorld;
	rallyWorld = NULL;
}

extern "C" void SV_RallyPhysics_Step( float frameSeconds ) {
	int steps = 0;
	if ( !rallyWorld || !rallyWorld->world || frameSeconds <= 0.0f )
		return;
	rallyWorld->accumulator += std::min( frameSeconds, RALLY_PHYSICS_MAX_FRAME );
	while ( rallyWorld->accumulator >= RALLY_PHYSICS_FIXED_STEP && steps < 12 ) {
		rallyWorld->world->stepSimulation( RALLY_PHYSICS_FIXED_STEP, 0,
			RALLY_PHYSICS_FIXED_STEP );
		rallyWorld->accumulator -= RALLY_PHYSICS_FIXED_STEP;
		steps++;
	}
}

extern "C" qboolean SV_RallyPhysics_CreateBody( int entityNum,
	const rallyPhysicsBodyDesc_t *desc, const vec3_t *vertices, int numVertices ) {
	rallyBody_t *entry;
	btTransform transform;
	btQuaternion rotation;
	btVector3 inertia( 0.0f, 0.0f, 0.0f );
	btRigidBody::btRigidBodyConstructionInfo info( 0.0f, NULL, NULL );
	float mass;
	if ( !rallyWorld || !rallyWorld->world || !desc || entityNum < 0 || entityNum >= MAX_GENTITIES )
		return qfalse;
	if ( numVertices < 0 || numVertices > RALLY_PHYSICS_MAX_HULL_VERTS )
		return qfalse;
	entry = &rallyBodies[entityNum];
	if ( entry->body )
		DestroyBody( *entry );

	entry->shape = CreateBodyShape( desc, vertices, numVertices, entry->localOriginOffset );
	if ( !entry->shape ) {
		*entry = rallyBody_t();
		return qfalse;
	}
	mass = std::max( 0.0f, desc->mass );
	if ( mass > 0.0f )
		entry->shape->calculateLocalInertia( mass, inertia );
	AnglesToQuaternion( desc->angles, rotation );
	transform.setIdentity();
	transform.setRotation( rotation );
	transform.setOrigin( VectorToBullet( desc->origin ) + quatRotate( rotation, entry->localOriginOffset ) );
	entry->motionState = new btDefaultMotionState( transform );
	info = btRigidBody::btRigidBodyConstructionInfo( mass, entry->motionState,
		entry->shape, inertia );
	info.m_restitution = ClampFloat( desc->restitution, 0.0f, 1.0f );
	info.m_friction = ClampFloat( desc->friction, 0.0f, 2.0f );
	info.m_rollingFriction = ClampFloat( desc->rollingFriction, 0.0f, 2.0f );
	info.m_spinningFriction = ClampFloat( desc->spinningFriction, 0.0f, 2.0f );
	info.m_linearDamping = ClampFloat( desc->linearDamping, 0.0f, 1.0f );
	info.m_angularDamping = ClampFloat( desc->angularDamping, 0.0f, 1.0f );
	entry->body = new btRigidBody( info );
	entry->body->setUserIndex( entityNum );
	entry->body->setSleepingThresholds( 1.0f, 0.15f );
	entry->body->setDeactivationTime( 0.5f );
	entry->mass = mass;
	entry->friction = info.m_friction;
	entry->restitution = info.m_restitution;
	entry->contents = desc->contents;
	if ( mass > 0.0f ) {
		btVector3 aabbMin, aabbMax;
		entry->shape->getAabb( btTransform::getIdentity(), aabbMin, aabbMax );
		float minDimension = std::min( aabbMax.x() - aabbMin.x(),
			std::min( aabbMax.y() - aabbMin.y(), aabbMax.z() - aabbMin.z() ) );
		float ccdRadius = std::max( 0.05f, minDimension * 0.2f );
		entry->body->setCcdSweptSphereRadius( ccdRadius );
		entry->body->setCcdMotionThreshold( std::max( 0.1f, ccdRadius * 0.5f ) );
	}
	rallyWorld->world->addRigidBody( entry->body );
	return qtrue;
}

extern "C" void SV_RallyPhysics_RemoveBody( int entityNum ) {
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES )
		return;
	DestroyBody( rallyBodies[entityNum] );
}

extern "C" qboolean SV_RallyPhysics_GetBodyState( int entityNum,
	rallyPhysicsBodyState_t *state ) {
	rallyBody_t *entry;
	btTransform transform;
	if ( !rallyWorld || entityNum < 0 || entityNum >= MAX_GENTITIES ||
		!state || !rallyBodies[entityNum].body )
		return qfalse;
	entry = &rallyBodies[entityNum];
	transform = entry->body->getWorldTransform();
	VectorFromBullet( transform.getOrigin() - quatRotate( transform.getRotation(),
		entry->localOriginOffset ), state->origin );
	QuaternionToAngles( transform.getRotation(), state->angles );
	VectorFromBullet( entry->body->getLinearVelocity(), state->linearVelocity );
	VectorFromBullet( entry->body->getAngularVelocity(), state->angularVelocity );
	state->sleeping = entry->body->getActivationState() == ISLAND_SLEEPING ? qtrue : qfalse;
	return qtrue;
}

extern "C" qboolean SV_RallyPhysics_HasBody( int entityNum ) {
	return ( rallyWorld && entityNum >= 0 && entityNum < MAX_GENTITIES &&
		rallyBodies[entityNum].body ) ? qtrue : qfalse;
}

namespace {

static qboolean TraceBodyAllowed( const btCollisionObject *object,
	int passEntityNum, int contentMask, int *entityNumOut ) {
	int entityNum;
	if ( !object )
		return qfalse;
	entityNum = object->getUserIndex();
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES ||
		entityNum == passEntityNum || rallyBodies[entityNum].body != object ||
		!( rallyBodies[entityNum].contents & contentMask ) )
		return qfalse;
	if ( entityNumOut )
		*entityNumOut = entityNum;
	return qtrue;
}

class rallyRayTraceCallback_t : public btCollisionWorld::ClosestRayResultCallback {
public:
	int passEntityNum;
	int contentMask;

	rallyRayTraceCallback_t( const btVector3 &from, const btVector3 &to,
		int pass, int mask ) : ClosestRayResultCallback( from, to ),
		passEntityNum( pass ), contentMask( mask ) {}

	bool needsCollision( btBroadphaseProxy *proxy ) const override {
		if ( !ClosestRayResultCallback::needsCollision( proxy ) )
			return false;
		return TraceBodyAllowed( static_cast<const btCollisionObject *>(
			proxy->m_clientObject ), passEntityNum, contentMask, NULL ) ? true : false;
	}
};

class rallyConvexTraceCallback_t : public btCollisionWorld::ClosestConvexResultCallback {
public:
	int passEntityNum;
	int contentMask;

	rallyConvexTraceCallback_t( const btVector3 &from, const btVector3 &to,
		int pass, int mask ) : ClosestConvexResultCallback( from, to ),
		passEntityNum( pass ), contentMask( mask ) {}

	bool needsCollision( btBroadphaseProxy *proxy ) const override {
		if ( !ClosestConvexResultCallback::needsCollision( proxy ) )
			return false;
		return TraceBodyAllowed( static_cast<const btCollisionObject *>(
			proxy->m_clientObject ), passEntityNum, contentMask, NULL ) ? true : false;
	}
};

static void StoreTraceResult( trace_t *trace, float fraction,
	const btVector3 &normal, const btVector3 &hitPoint, const vec3_t start,
	const vec3_t end, int entityNum ) {
	int i;
	vec3_t point;
	Com_Memset( trace, 0, sizeof( *trace ) );
	trace->fraction = fraction;
	for ( i = 0; i < 3; i++ )
		trace->endpos[i] = start[i] + fraction * ( end[i] - start[i] );
	VectorFromBullet( normal, trace->plane.normal );
	VectorFromBullet( hitPoint, point );
	trace->plane.dist = DotProduct( trace->plane.normal, point );
	trace->plane.type = PlaneTypeForNormal( trace->plane.normal );
	SetPlaneSignbits( &trace->plane );
	trace->contents = rallyBodies[entityNum].contents;
	trace->entityNum = entityNum;
	if ( fraction <= 0.0f )
		trace->startsolid = qtrue;
}

}

extern "C" qboolean SV_RallyPhysics_Trace( const vec3_t start,
	const vec3_t mins, const vec3_t maxs, const vec3_t end,
	int passEntityNum, int contentMask, trace_t *trace ) {
	btVector3 from, to, offset( 0.0f, 0.0f, 0.0f );
	int entityNum;
	qboolean pointTrace = qtrue;
	if ( !rallyWorld || !rallyWorld->world || !start || !end || !trace ||
		contentMask == 0 )
		return qfalse;
	for ( int i = 0; i < 3; i++ ) {
		if ( mins && maxs && ( mins[i] != 0.0f || maxs[i] != 0.0f ) )
			pointTrace = qfalse;
	}
	from = VectorToBullet( start );
	to = VectorToBullet( end );
	if ( pointTrace ) {
		rallyRayTraceCallback_t callback( from, to, passEntityNum, contentMask );
		rallyWorld->world->rayTest( from, to, callback );
		if ( !callback.hasHit() || !TraceBodyAllowed( callback.m_collisionObject,
			passEntityNum, contentMask, &entityNum ) )
			return qfalse;
		StoreTraceResult( trace, callback.m_closestHitFraction,
			callback.m_hitNormalWorld, callback.m_hitPointWorld, start, end,
			entityNum );
		return qtrue;
	}
	{
		btVector3 centerOffset;
		btBoxShape shape( btVector3(
			std::max( 0.001f, ( maxs[0] - mins[0] ) * 0.5f ),
			std::max( 0.001f, ( maxs[1] - mins[1] ) * 0.5f ),
			std::max( 0.001f, ( maxs[2] - mins[2] ) * 0.5f ) ) );
		btTransform fromTransform, toTransform;
		centerOffset.setValue( ( maxs[0] + mins[0] ) * 0.5f,
			( maxs[1] + mins[1] ) * 0.5f,
			( maxs[2] + mins[2] ) * 0.5f );
		offset = centerOffset;
		from += offset;
		to += offset;
		fromTransform.setIdentity();
		toTransform.setIdentity();
		fromTransform.setOrigin( from );
		toTransform.setOrigin( to );
		rallyConvexTraceCallback_t callback( from, to, passEntityNum, contentMask );
		rallyWorld->world->convexSweepTest( &shape, fromTransform,
			toTransform, callback, 0.0f );
		if ( !callback.hasHit() || !TraceBodyAllowed( callback.m_hitCollisionObject,
			passEntityNum, contentMask, &entityNum ) )
			return qfalse;
		StoreTraceResult( trace, callback.m_closestHitFraction,
			callback.m_hitNormalWorld, callback.m_hitPointWorld, start, end,
			entityNum );
		return qtrue;
	}
}

extern "C" void SV_RallyPhysics_ApplyImpulse( int entityNum,
	const vec3_t point, const vec3_t impulse ) {
	rallyBody_t *entry;
	btRigidBody *body;
	btVector3 worldPoint, worldImpulse;
	if ( !rallyWorld || !rallyWorld->world || entityNum < 0 ||
		entityNum >= MAX_GENTITIES || !point || !impulse )
		return;
	entry = &rallyBodies[entityNum];
	body = entry->body;
	if ( !body || body->getInvMass() <= 0.0f )
		return;
	worldPoint = VectorToBullet( point );
	worldImpulse = VectorToBullet( impulse );
	body->activate( true );
	body->applyImpulse( worldImpulse,
		worldPoint - body->getCenterOfMassPosition() );
}

extern "C" void SV_RallyPhysics_ApplyVehicleContact( int entityNum,
	const vec3_t point, const vec3_t normal, const vec3_t vehicleVelocity,
	float vehicleMass, float impactScale, vec3_t objectImpulse ) {
	rallyBody_t *entry;
	btRigidBody *body;
	btVector3 n, contact, vehicleV, relative, impulse;
	float closingSpeed, inverseVehicleMass, inverseMass, normalImpulse, scale;
	VectorClear( objectImpulse );
	if ( !rallyWorld || !rallyWorld->world || entityNum < 0 ||
		entityNum >= MAX_GENTITIES || vehicleMass <= 0.0f )
		return;
	entry = &rallyBodies[entityNum];
	body = entry->body;
	if ( !body || body->getInvMass() <= 0.0f )
		return;
	n = VectorToBullet( normal );
	if ( n.length2() < 1e-6f )
		return;
	n.normalize();
	contact = VectorToBullet( point );
	vehicleV = VectorToBullet( vehicleVelocity );
	relative = body->getVelocityInLocalPoint( contact - body->getCenterOfMassPosition() ) - vehicleV;
	closingSpeed = -relative.dot( n );
	if ( closingSpeed <= 0.01f )
		return;
	inverseVehicleMass = 1.0f / vehicleMass;
	inverseMass = body->computeImpulseDenominator( contact, n ) + inverseVehicleMass;
	if ( inverseMass <= 1e-6f )
		return;
	scale = ClampFloat( impactScale, 0.0f, 2.0f );
	normalImpulse = ( 1.0f + entry->restitution ) * closingSpeed / inverseMass * scale;
	if ( normalImpulse <= 0.0f )
		return;
	impulse = n * normalImpulse;
	body->activate( true );
	body->applyImpulse( impulse, contact - body->getCenterOfMassPosition() );

	/* Coulomb-limited tangential impulse; the car receives the opposite vector
	 * through the game-side adapter so momentum is exchanged only once. */
	{
		btVector3 tangentVelocity = body->getVelocityInLocalPoint(
			contact - body->getCenterOfMassPosition() ) - vehicleV;
		tangentVelocity -= n * tangentVelocity.dot( n );
		if ( tangentVelocity.length2() > 1e-4f ) {
			btVector3 tangentDirection = -tangentVelocity.normalized();
			float tangentDenominator = body->computeImpulseDenominator( contact,
				tangentDirection ) + inverseVehicleMass;
			if ( tangentDenominator > 1e-6f ) {
				float frictionImpulse = tangentVelocity.length() / tangentDenominator;
				float maxFrictionImpulse = sqrtf( std::max( 0.0f, entry->friction * 0.5f ) ) * normalImpulse;
				frictionImpulse = std::min( frictionImpulse, maxFrictionImpulse );
				btVector3 friction = tangentDirection * frictionImpulse;
				body->applyImpulse( friction, contact - body->getCenterOfMassPosition() );
				impulse += friction;
			}
		}
	}
	VectorFromBullet( impulse, objectImpulse );
}
