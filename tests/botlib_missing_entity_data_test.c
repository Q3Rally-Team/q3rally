#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q_shared.h"
#include "botlib.h"
#include "l_script.h"

static int g_print_error_count;
static const char *g_entity_data;

static void TestPrint(int type, char *fmt, ...) {
    (void)type;
    (void)fmt;
    g_print_error_count++;
}

static char *TestBSPEntityData(void) {
    return (char *)g_entity_data;
}

void *GetClearedHunkMemory(int size) {
    return calloc(1, (size_t)size);
}

void *GetHunkMemory(int size) {
    return calloc(1, (size_t)size);
}

void FreeMemory(void *ptr) {
    free(ptr);
}

void StripDoubleQuotes(char *string) {
    (void)string;
}

int PS_ExpectTokenType(script_t *script, int type, int subtype, token_t *token) {
    (void)script;
    (void)type;
    (void)subtype;
    (void)token;
    return 0;
}

script_t *LoadScriptMemory(char *ptr, int length, char *name) {
    (void)ptr;
    (void)length;
    (void)name;
    return NULL;
}

void FreeScript(script_t *script) {
    (void)script;
}

void SetScriptFlags(script_t *script, int flags) {
    (void)script;
    (void)flags;
}

int PS_ReadToken(script_t *script, token_t *token) {
    (void)script;
    (void)token;
    return 0;
}

void ScriptError(script_t *script, char *str, ...) {
    (void)script;
    (void)str;
}

void ScriptWarning(script_t *script, char *str, ...) {
    (void)script;
    (void)str;
}

botlib_import_t botimport;

int AAS_LoadBSPFile(void);

int main(void) {
    memset(&botimport, 0, sizeof(botimport));
    botimport.Print = TestPrint;
    botimport.BSPEntityData = TestBSPEntityData;

    g_entity_data = NULL;
    g_print_error_count = 0;
    int result = AAS_LoadBSPFile();
    assert(result == BLERR_MISSINGENTITYDATA);
    assert(g_print_error_count > 0);

    static const char empty[] = "";
    g_entity_data = empty;
    g_print_error_count = 0;
    result = AAS_LoadBSPFile();
    assert(result == BLERR_MISSINGENTITYDATA);
    assert(g_print_error_count > 0);

    puts("ok");
    return 0;
}
