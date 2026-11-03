#include <assert.h>
#include <stdio.h>
#include "../qcommon/q_shared.h"
#include "bg_public.h"

static pmove_t movement;
static pmove_t *pm = &movement;
static playerState_t player;
static qboolean floorPresent, wallPresent;
static int lastMask;

static void traceWorld( trace_t *tr, const vec3_t start, const vec3_t mins,
    const vec3_t maxs, const vec3_t end, int pass, int mask ) {
    float fraction;
    int axis;
    memset( tr, 0, sizeof( *tr ) );
    tr->fraction = 1;
    VectorCopy( end, tr->endpos );
    lastMask = mask;
    for ( axis = 0; axis < 3; axis += 2 ) {
        if ( axis == 0 ? !wallPresent : !floorPresent ) continue;
        /* Solid slab occupies [-12, 0], with an open void underneath it. */
        if ( start[axis] + mins[axis] < 0 && start[axis] + maxs[axis] > -12 ) {
            tr->startsolid = qtrue;
            return;
        }
        if ( start[axis] + mins[axis] >= 0 && end[axis] + mins[axis] < 0 ) {
            fraction = (start[axis] + mins[axis] - 0.125f) / (start[axis] - end[axis]);
            if ( fraction < tr->fraction ) {
                tr->fraction = fraction;
                VectorClear( tr->plane.normal );
                tr->plane.normal[axis] = 1;
            }
        }
    }
    for ( axis = 0; axis < 3; axis++ ) tr->endpos[axis] =
        start[axis] + tr->fraction * (end[axis] - start[axis]);
}

#include "world_guard_under_test.h"

static void setup( car_t *car ) {
    int i;
    memset( car, 0, sizeof( *car ) );
    memset( &movement, 0, sizeof( movement ) );
    movement.ps = &player;
    movement.trace = traceWorld;
    movement.tracemask = MASK_PLAYERSOLID | CONTENTS_BOTCLIP;
    floorPresent = qtrue;
    wallPresent = qfalse;
    VectorSet( car->sBody.r, 20, 0, 20 );
    VectorSet( car->tBody.r, 25, 15, -30 );
    VectorSet( car->tBody.CoM, 25, 15, -35 );
    VectorSet( car->tBody.v, 100, 60, -300 );
    for ( i = 0; i < NUM_CAR_POINTS; i++ ) {
        VectorCopy( car->tBody.r, car->tPoints[i].r );
        car->tPoints[i].r[0] += i;
        VectorCopy( car->tBody.v, car->tPoints[i].v );
    }
}

int main( void ) {
    car_t car;
    int i;
    setup( &car );
    PM_ConstrainCarBodyToWorld( &car );
    assert( car.tBody.r[2] > 4 && car.tBody.r[2] < 5 );
    assert( car.tBody.r[0] == 25 && car.tBody.r[1] == 15 );
    assert( car.tBody.v[2] == 0 && car.tBody.v[0] == 100 );
    assert( fabs( car.tBody.CoM[2] - car.tBody.r[2] + 5 ) < 0.001f );
    assert( !(lastMask & CONTENTS_BODY) && (lastMask & CONTENTS_BOTCLIP) );
    for ( i = 0; i < NUM_CAR_POINTS; i++ ) {
        assert( fabs( car.tPoints[i].r[2] - car.tBody.r[2] ) < 0.001f );
        assert( car.tPoints[i].v[2] == 0 );
    }
    setup( &car ); floorPresent = qfalse;
    PM_ConstrainCarBodyToWorld( &car );
    assert( car.tBody.r[2] == -30 && car.tBody.v[2] == -300 ); /* Actual drop remains a drop. */
    setup( &car ); car.sBody.r[2] = -3;
    PM_ConstrainCarBodyToWorld( &car );
    assert( car.tBody.r[2] == -30 ); /* No teleport from an invalid start. */
    setup( &car ); car.tBody.r[2] = 20;
    PM_ConstrainCarBodyToWorld( &car );
    assert( car.tBody.r[0] == 25 && car.tBody.r[2] == 20 );
    setup( &car ); wallPresent = qtrue; car.tBody.r[0] = -30; car.tBody.v[0] = -200;
    PM_ConstrainCarBodyToWorld( &car );
    assert( car.tBody.r[0] > 4 && car.tBody.r[2] > 4 );
    assert( car.tBody.v[0] == 0 && car.tBody.v[2] == 0 );
    puts( "ok" );
    return 0;
}
