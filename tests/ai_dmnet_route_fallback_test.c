#include <assert.h>
#include <stdio.h>

#define ID_INLINE inline

typedef int qboolean;
#define qtrue 1
#define qfalse 0

#include "../engine/code/game/ai_dmnet_pathselect.h"

static void test_route_fallback_when_preferred_missing(void) {
    qboolean available[3] = { qtrue, qfalse, qtrue };

    assert(Bot_SelectRouteIdWithFallbackByAvailability(available, 3, 1, 0) == 0);
    assert(Bot_SelectRouteIdWithFallbackByAvailability(available, 3, 2, 0) == 2);

    available[0] = qfalse;
    assert(Bot_SelectRouteIdWithFallbackByAvailability(available, 3, 1, 0) == 2);

    available[2] = qfalse;
    assert(Bot_SelectRouteIdWithFallbackByAvailability(available, 3, 1, 0) == -1);
}

static void test_deterministic_selection_with_multiple_path_lines(void) {
    qboolean available[3] = { qfalse, qtrue, qtrue };
    int first = Bot_SelectRouteIdWithFallbackByAvailability(available, 3, 99, 0);

    for (int i = 0; i < 8; ++i) {
        int next = Bot_SelectRouteIdWithFallbackByAvailability(available, 3, 99, 0);
        assert(next == first);
    }

    assert(first == 1);
}

int main(void) {
    test_route_fallback_when_preferred_missing();
    test_deterministic_selection_with_multiple_path_lines();
    puts("ok");
    return 0;
}
