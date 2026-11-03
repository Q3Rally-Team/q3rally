"""Exercise the production demo I/O functions against deterministic filesystem stubs."""
import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_demo_io():
    source = (ROOT / "engine/code/client/cl_main.c").read_text()
    def function(name):
        start = source.index(name)
        start = source.rfind("\n", 0, start) + 1
        opening = source.index("{", start)
        depth = 1
        end = opening + 1
        while depth:
            depth += (source[end] == "{") - (source[end] == "}")
            end += 1
        return source[start:end]
    harness = r'''#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#define LEGACY_PROTOCOL
#define MAX_OSPATH 256
#define MAX_MSGLEN 64
#define DEMOEXT "dm_"
#define ERR_DROP 1
#define S_COLOR_RED ""
#define qtrue 1
#define qfalse 0
typedef int qboolean;
typedef unsigned char byte;
typedef struct { int cursize, maxsize, readcount; byte *data; } msg_t;
static struct { int demofile, demorecording, spDemoRecording, demoPlayer, serverMessageSequence, lastPacketTime; } clc;
static struct { int realtime; } cls;
static struct { struct { int valid; } snap; } cl;
static struct { int integer; } current = {67}, legacy = {66};
#define com_protocol (&current)
#define com_legacyprotocol (&legacy)
static int demo_protocols[] = {67, 66, 68, 0};
static int length, reads, completed, parsed, closed, failWrite, opens, failRead;
static qboolean demoWriteFailed;
static jmp_buf error;
static int LittleLong(int n) { return n; }
static void MSG_Init(msg_t *m, byte *b, int n) { memset(m,0,sizeof(*m)); m->data=b; m->maxsize=n; }
static int FS_Read(void *b,int n,int f) { reads++; if(reads==failRead) return n-1; if(reads==1) { *(int*)b=1; return 4; } if(reads==2) { *(int*)b=length; return 4; } assert(n>=0 && n<=MAX_MSGLEN); return n; }
static int FS_Write(const void *b,int n,int f) { return failWrite ? 0 : n; }
static void FS_FCloseFile(int f) { closed++; }
static void CL_DemoCompleted(void) { completed++; }
static void CL_ParseServerMessage(msg_t *m) { parsed++; }
static void Com_Error(int n,const char *s,...) { longjmp(error,1); }
static void Com_Printf(const char *s,...) {}
#define Com_sprintf snprintf
static void FS_FOpenFileRead(const char *n,int *f,int unique) { assert(++opens<10); *f = strstr(n,"dm_68") ? 1 : 0; }
'''
    harness += "\n".join(function(name) for name in (
        "static qboolean CL_DemoWrite(", "void CL_ReadDemoMessage(", "static int CL_WalkDemoExt("))
    harness += r'''
int main(void) {
    char name[MAX_OSPATH]; int file;
    clc.demofile=1;
    length=-2;
    if (!setjmp(error)) { CL_ReadDemoMessage(); assert(0); }
    assert(reads==2 && parsed==0);
    reads=0; length=MAX_MSGLEN+1;
    if (!setjmp(error)) { CL_ReadDemoMessage(); assert(0); }
    assert(reads==2);
    reads=0; length=-1; CL_ReadDemoMessage(); assert(completed==1 && reads==2);
    reads=0; length=16; CL_ReadDemoMessage(); assert(parsed==1 && reads==3);
    assert(CL_WalkDemoExt("test", name, &file)==68 && file==1 && opens==3);
    failWrite=1; clc.demorecording=clc.spDemoRecording=1;
    assert(!CL_DemoWrite(name,4));
    assert(closed==1 && !clc.demofile && !clc.demorecording && !clc.spDemoRecording);
    clc.demofile=1; clc.demoPlayer=1; cl.snap.valid=1;
    length=16;
    for(failRead=1;failRead<=3;failRead++) {
        reads=0;
        if (!setjmp(error)) { CL_ReadDemoMessage(); assert(0); }
        assert(reads==failRead);
    }
    failRead=0; reads=0; length=0;
    if (!setjmp(error)) { CL_ReadDemoMessage(); assert(0); }
    assert(reads==2);
    reads=0; length=-1; cl.snap.valid=0;
    if (!setjmp(error)) { CL_ReadDemoMessage(); assert(0); }
    cl.snap.valid=1; reads=0; CL_ReadDemoMessage(); assert(completed==2);
    puts("demo I/O ok");
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "demo.c"
        binary = pathlib.Path(directory) / "demo.exe"
        c.write_text(harness)
        subprocess.run([compiler, str(c), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
