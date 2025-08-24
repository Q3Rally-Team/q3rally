#include "g_local.h"
#include <assert.h>
#include <string.h>

static int stubEntityList[MAX_GENTITIES];
static int stubEntityCount;

gentity_t g_entities[MAX_GENTITIES];

level_locals_t level;

void G_FreeEntity(gentity_t *e) {}
qboolean G_RadiusDamage_NoKnockBack(vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod) { return qfalse; }
gentity_t *G_Find(gentity_t *from, int fieldofs, const char *match) { return NULL; }
gentity_t *G_TempRallyEntity(vec3_t origin, int event) { return NULL; }
gentity_t *G_TempEntity(vec3_t origin, int event) { return NULL; }

int trap_EntitiesInBox(const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount) {
    int count = stubEntityCount < maxcount ? stubEntityCount : maxcount;
    for (int i = 0; i < count; ++i) {
        entityList[i] = stubEntityList[i];
    }
    return count;
}

static void test_no_hazard(void) {
    carPoint_t point;
    memset(&point, 0, sizeof(point));
    point.radius = 1.0f;
    point.r[0] = point.r[1] = point.r[2] = 0.0f;

    float sCOF = 0.5f;
    float kCOF = 0.4f;

    stubEntityCount = 0;
    qboolean result = G_FrictionCalc(&point, &sCOF, &kCOF);
    assert(result == qfalse);
    assert(sCOF == 0.5f);
    assert(kCOF == 0.4f);
}

static void test_oil_hazard(void) {
    carPoint_t point;
    memset(&point, 0, sizeof(point));
    point.radius = 1.0f;
    point.r[0] = point.r[1] = point.r[2] = 0.0f;

    float sCOF = 0.5f;
    float kCOF = 0.4f;

    memset(&g_entities[0], 0, sizeof(g_entities[0]));
    g_entities[0].s.eType = ET_EVENTS + EV_HAZARD;
    g_entities[0].s.weapon = HT_OIL;
    g_entities[0].splashRadius = 1;
    g_entities[0].r.currentOrigin[0] = g_entities[0].r.currentOrigin[1] = g_entities[0].r.currentOrigin[2] = 0.0f;

    stubEntityList[0] = 0;
    stubEntityCount = 1;

    qboolean result = G_FrictionCalc(&point, &sCOF, &kCOF);
    assert(result == qtrue);
    assert(sCOF == CP_OIL_SCOF);
    assert(kCOF == CP_OIL_KCOF);
}

int main(void) {
    test_no_hazard();
    test_oil_hazard();
    return 0;
}
