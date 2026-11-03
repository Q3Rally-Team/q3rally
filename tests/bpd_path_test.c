#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define ID_INLINE inline
#define MAX_QPATH 64

typedef int qboolean;
#define qtrue 1
#define qfalse 0

static int Com_sprintf(char *dest, int size, const char *fmt, ...) {
    va_list args;
    int len;
    va_start(args, fmt);
    len = vsnprintf(dest, size, fmt, args);
    va_end(args);
    return len;
}

static int Q_stricmp(const char *s1, const char *s2) {
    return strcasecmp(s1 ? s1 : "", s2 ? s2 : "");
}

#include "../engine/code/game/bg_bpd_path.h"

static void test_paths(void) {
    char path[MAX_QPATH];

    assert(BG_BpdGenericPath(path, sizeof(path), "q3r_dirtyfun"));
    assert(!strcmp(path, "maps/q3r_dirtyfun.bpd"));
    assert(BG_BpdLengthPath(path, sizeof(path), "q3r_dirtyfun", 0));
    assert(!strcmp(path, "maps/q3r_dirtyfun_tl0.bpd"));
    assert(BG_BpdLengthPath(path, sizeof(path), "q3r_dirtyfun", 2));
    assert(!strcmp(path, "maps/q3r_dirtyfun_tl2.bpd"));

    assert(!BG_BpdLengthPath(path, sizeof(path), "q3r_dirtyfun", -1));
    assert(!BG_BpdLengthPath(path, sizeof(path), "q3r_dirtyfun", 3));
    assert(!BG_BpdGenericPath(path, sizeof(path), ""));
    assert(!BG_BpdGenericPath(path, 12, "q3r_dirtyfun"));
}

static void test_restriction(void) {
    assert(!BG_BpdIsLengthRestricted("012"));
    assert(!BG_BpdIsLengthRestricted("210"));
    assert(BG_BpdIsLengthRestricted("0"));
    assert(BG_BpdIsLengthRestricted("12"));
    assert(BG_BpdIsLengthRestricted(""));
    assert(!BG_BpdIsLengthRestricted(NULL));
}

static void test_selection(void) {
    /* The file for the active length always wins. */
    assert(BG_BpdSelectSource(qtrue, qtrue, qtrue) == BPD_SOURCE_LENGTH);
    assert(BG_BpdSelectSource(qtrue, qfalse, qfalse) == BPD_SOURCE_LENGTH);
    /* The generic file serves maps whose checkpoints are shared by all lengths. */
    assert(BG_BpdSelectSource(qfalse, qtrue, qfalse) == BPD_SOURCE_GENERIC);
    /* On a map with length variants it would hit the wrong checkpoints. */
    assert(BG_BpdSelectSource(qfalse, qtrue, qtrue) == BPD_SOURCE_NONE);
    assert(BG_BpdSelectSource(qfalse, qfalse, qfalse) == BPD_SOURCE_NONE);
}

static void test_path_matches_map(void) {
    assert(BG_BpdPathMatchesMap("maps/q3r_sk1.bpd", "q3r_sk1"));
    assert(BG_BpdPathMatchesMap("maps/q3r_sk1_tl1.bpd", "q3r_sk1"));
    assert(BG_BpdPathMatchesMap("MAPS/Q3R_SK1_TL0.BPD", "q3r_sk1"));
    assert(!BG_BpdPathMatchesMap("maps/q3r_sk1_tl3.bpd", "q3r_sk1"));
    assert(!BG_BpdPathMatchesMap("maps/q3r_nightcity_tl0.bpd", "q3r_sk1"));
    assert(!BG_BpdPathMatchesMap("maps/../q3r_sk1.bpd", "q3r_sk1"));
    assert(!BG_BpdPathMatchesMap("", "q3r_sk1"));
    assert(!BG_BpdPathMatchesMap(NULL, "q3r_sk1"));
    assert(!BG_BpdPathMatchesMap("maps/.bpd", ""));
}

int main(void) {
    test_paths();
    test_restriction();
    test_selection();
    test_path_matches_map();
    puts("ok");
    return 0;
}
