#include <assert.h>
#include <stdio.h>
#include "q_shared.h"
#define OVERCLIP 1.001f
#include "contact_planes_under_test.h"

static void check( const vec3_t initial, vec3_t *planes, int count, int freeAxis ) {
    vec3_t velocity;
    int i;
    VectorCopy( initial, velocity );
    PM_ClipCarPointVelocity( velocity, planes, count );
    for ( i = 0; i < count; i++ ) assert( DotProduct( velocity, planes[i] ) >= -0.011f );
    assert( VectorLengthSquared( velocity ) <= VectorLengthSquared( initial ) + 0.01f );
    if ( freeAxis >= 0 ) assert( fabs( velocity[freeAxis] - initial[freeAxis] ) < 0.01f );
}

int main( void ) {
    vec3_t wallFloor[] = {{1, 0, 0}, {0, 0, 1}};
    vec3_t floorWall[] = {{0, 0, 1}, {1, 0, 0}};
    vec3_t crease[] = {{1, 0, 0}, {0, 1, 0}};
    vec3_t corner[] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    vec3_t diagonal[] = {{0.70710678f, 0.70710678f, 0}, {0, 0, 1}};
    const vec3_t impact = {-300, -120, -200};
    const vec3_t away = {200, 150, 80};
    check( impact, wallFloor, 2, 1 );
    check( impact, floorWall, 2, 1 );
    check( impact, crease, 2, 2 );
    check( impact, corner, 3, -1 );
    check( impact, diagonal, 2, -1 );
    check( away, wallFloor, 2, 0 );
    puts( "ok" );
    return 0;
}
