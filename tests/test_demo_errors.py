import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_demo_map_and_error_menu():
    source = (ROOT / "engine/code/client/cl_main.c").read_text()
    start = source.index("void CL_InitDownloads(void) {")
    end = source.index("  if ( !(cl_allowDownload", start)
    check_map = source[start:end] + "}\n"
    start = source.index("void CL_DemoErrorMenu(")
    end = source.index("\n}\n", start) + 3
    harness = r'''#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#define CS_SERVERINFO 0
#define MAX_OSPATH 256
#define ERR_DROP 1
#define qfalse 0
#define UI_SET_ACTIVE_MENU 1
#define UIMENU_DEMOS 2
typedef int fileHandle_t;
static struct {int demoPlayer;} clc;
static struct {struct {char stringData[32]; int stringOffsets[1];} gameState;} cl;
static int present, opens, closed, loaded, menu, uivm=1;
static char mapname[64], demoError[128], generalError[128];
static jmp_buf error;
static const char *Info_ValueForKey(const char *info,const char *key) {return mapname;}
#define Com_sprintf snprintf
static void FS_FOpenFileRead(const char *path,int *file,int unique) {opens++; assert(!strcmp(path,"maps/test.bsp")); *file=present;}
static void FS_FCloseFile(int file) {assert(file); closed++;}
static void CL_DownloadsComplete(void) {loaded++;}
static void Com_Error(int code,const char *fmt,...) {assert(code==ERR_DROP); longjmp(error,1);}
static void Cvar_Set(const char *key,const char *value) {if(!strcmp(key,"cl_demoError")) strcpy(demoError,value); else if(!strcmp(key,"com_errorMessage")) strcpy(generalError,value); else assert(0);}
static void VM_Call(int vm,int command,int target) {assert(vm==1 && command==UI_SET_ACTIVE_MENU); menu=target;}
'''
    harness += check_map + source[start:end]
    harness += r'''
int main(void) {
    clc.demoPlayer=1; strcpy(mapname,"test");
    if(!setjmp(error)) {CL_InitDownloads(); assert(0);}
    assert(opens==1 && !closed && !loaded);
    present=1; CL_InitDownloads(); assert(opens==2 && closed==1 && loaded==1);
    mapname[0]=0;
    if(!setjmp(error)) {CL_InitDownloads(); assert(0);}
    assert(opens==2);
    strcpy(generalError,"old failure");
    CL_DemoErrorMenu("Damaged recording");
    assert(menu==UIMENU_DEMOS && !strcmp(demoError,"Damaged recording") && !generalError[0]);
    uivm=0; menu=0; CL_DemoErrorMenu("Missing map"); assert(!menu);
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "errors.c"
        binary = pathlib.Path(directory) / "errors.exe"
        c.write_text(harness)
        subprocess.run([compiler, str(c), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
