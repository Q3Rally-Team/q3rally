import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_demo_seek_reconstruction():
    source = (ROOT / "engine/code/client/cl_cgame.c").read_text()
    start = source.index("    if (clc.demoPlayer && clc.demoSeeking) {")
    end = source.index("\n\t// if we have gotten to this point", start)
    harness = r'''#include <assert.h>
#include <string.h>
#define CA_ACTIVE 8
#define STEREO_CENTER 0
#define qfalse 0
static struct { int demoPlayer, demoSeeking, demoStartTime, demoSeekTarget, demoSeekPaused, state; } clc;
static struct { int serverTime, oldServerTime, serverTimeDelta, oldFrameServerTime; struct {int serverTime;} snap; } cl;
static struct {int realtime;} cls;
static int now, rendered, last, muted, frozen, seekingFlag;
static int Sys_Milliseconds(void) { return now++; }
static void CL_CGameRendering(int stereo) { assert(cl.serverTime >= last); assert(cl.serverTime-last<=50); last=cl.serverTime; rendered++; }
static void CL_ReadDemoMessage(void) { cl.snap.serverTime+=50; if(cl.snap.serverTime>2000) { clc.demoPlayer=0; clc.state=0; } }
static void S_ClearSoundBuffer(void) { muted++; }
static void Cvar_Set(const char *name,const char *value) { if(!strcmp(name,"cl_freezeDemo")) frozen=value[0]=='1'; else seekingFlag=value[0]=='1'; }
static void seek(void) {
'''
    harness += source[start:end] + "\n}\n"
    harness += r'''
int main(void) {
    clc.demoPlayer=clc.demoSeeking=clc.demoSeekPaused=1; clc.state=CA_ACTIVE;
    clc.demoStartTime=1000; clc.demoSeekTarget=500;
    cl.snap.serverTime=1000; last=950; cls.realtime=3000;
    seek(); assert(clc.demoSeeking); // work is split across UI frames
    while(clc.demoSeeking) seek();
    assert(rendered==11 && cl.serverTime==1500 && frozen && !seekingFlag);
    assert(cl.serverTimeDelta==-1500 && cl.oldFrameServerTime==1500 && muted);
    clc.demoSeeking=1; clc.demoSeekTarget=5000;
    while(clc.demoPlayer) seek(); // target beyond EOF must terminate
    assert(clc.state==0);
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "seek.c"
        binary = pathlib.Path(directory) / "seek.exe"
        c.write_text(harness)
        subprocess.run([compiler, str(c), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
