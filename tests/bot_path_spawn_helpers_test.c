#include <assert.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#define ID_INLINE inline
#define MAX_BOT_PATH_ROUTES 3
#define MAX_BOT_PATH_NODES 8

typedef int qboolean;
#define qtrue 1
#define qfalse 0

typedef struct {
    int order;
} botPathNodeSpawn_t;

int Q_stricmp(const char *s1, const char *s2) {
    return strcasecmp(s1 ? s1 : "", s2 ? s2 : "");
}

#include "../engine/code/game/g_botpath_spawn_helpers.h"

static int order_cmp(const void *a, const void *b) {
    const botPathNodeSpawn_t *left = (const botPathNodeSpawn_t *)a;
    const botPathNodeSpawn_t *right = (const botPathNodeSpawn_t *)b;
    return (left->order > right->order) - (left->order < right->order);
}

static void test_entity_parsing_defaults_and_clamps(void) {
    qboolean valid;

    assert(G_BotPath_ClampPathId(-5) == 0);
    assert(G_BotPath_ClampPathId(99) == MAX_BOT_PATH_ROUTES - 1);
    assert(G_BotPath_ClampOrder(-1) == 0);
    assert(G_BotPath_ClampOrder(999) == MAX_BOT_PATH_NODES - 1);

    assert(G_BotPath_ClampTargetSpeed(-50.0f) == -1.0f);
    assert(G_BotPath_ClampTargetSpeed(9999.0f) == 5000.0f);
    assert(G_BotPath_ClampWidth(0.25f) == 1.0f);
    assert(G_BotPath_ClampWidth(12000.0f) == 8192.0f);

    assert(G_BotPath_NormalizeTrackLengthMask(0, &valid) == 7);
    assert(valid == qfalse);
    assert(G_BotPath_NormalizeTrackLengthMask(5, &valid) == 5);
    assert(valid == qtrue);

    assert(G_BotPath_ParseReversedMode("both", &valid) == BOT_PATH_REVERSED_BOTH && valid == qtrue);
    assert(G_BotPath_ParseReversedMode("reversed", &valid) == BOT_PATH_REVERSED_REVERSED && valid == qtrue);
    assert(G_BotPath_ParseReversedMode("forward", &valid) == BOT_PATH_REVERSED_FORWARD && valid == qtrue);
    assert(G_BotPath_ParseReversedMode("invalid-token", &valid) == BOT_PATH_REVERSED_FORWARD && valid == qfalse);
}

static void test_filtering_tracklength_and_reversed(void) {
    assert(G_BotPath_NodeMatchesTrackLengthMask(1, 0) == qtrue);
    assert(G_BotPath_NodeMatchesTrackLengthMask(1, 1) == qfalse);
    assert(G_BotPath_NodeMatchesTrackLengthMask(2, 1) == qtrue);
    assert(G_BotPath_NodeMatchesTrackLengthMask(4, 2) == qtrue);
    assert(G_BotPath_NodeMatchesTrackLengthMask(1, 9) == qtrue);

    assert(G_BotPath_NodeMatchesReversedMode(BOT_PATH_REVERSED_BOTH, 0, qtrue) == qtrue);
    assert(G_BotPath_NodeMatchesReversedMode(BOT_PATH_REVERSED_FORWARD, 0, qtrue) == qtrue);
    assert(G_BotPath_NodeMatchesReversedMode(BOT_PATH_REVERSED_REVERSED, 1, qtrue) == qtrue);
    assert(G_BotPath_NodeMatchesReversedMode(BOT_PATH_REVERSED_REVERSED, 1, qfalse) == qfalse);
}

static void test_sorting_order_and_duplicate_cases(void) {
    botPathNodeSpawn_t nodes[] = {
        {.order = 7}, {.order = 2}, {.order = 2}, {.order = 0}, {.order = 5}, {.order = 5}
    };
    int count = (int)(sizeof(nodes) / sizeof(nodes[0]));

    qsort(nodes, count, sizeof(nodes[0]), order_cmp);
    count = G_BotPath_CompressUniqueOrdersInPlace(nodes, count);

    assert(count == 4);
    assert(nodes[0].order == 0);
    assert(nodes[1].order == 2);
    assert(nodes[2].order == 5);
    assert(nodes[3].order == 7);
}

int main(void) {
    test_entity_parsing_defaults_and_clamps();
    test_filtering_tracklength_and_reversed();
    test_sorting_order_and_duplicate_cases();
    puts("ok");
    return 0;
}
