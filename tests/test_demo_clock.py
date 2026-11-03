import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_demo_playback_clock():
    source = (ROOT / "engine/code/client/cl_main.c").read_text()
    start = source.index("static void CL_UpdateDemoPlaybackTime(void)")
    end = source.index("\n}\n", start) + 3
    harness = r'''#include <assert.h>
#include <string.h>
#define CA_ACTIVE 8
static struct {int demoPlayer, state, demoStartTime, demoplaying;} clc;
static struct {int realtime;} cls;
static struct {int integer;} freeze;
#define cl_freezeDemo (&freeze)
static struct {int serverTime, serverTimeDelta; struct {int valid;} snap;} cl;
static int seconds;
static void Cvar_SetValue(const char *name,float value) {assert(!strcmp(name,"cl_demoTime")); seconds=(int)value;}
'''
    harness += source[start:end]
    timing = (ROOT / "engine/code/client/cl_cgame.c").read_text()
    pause_start = timing.index("if ( clc.demoplaying && cl_freezeDemo->integer ) {")
    pause_end = timing.index("} else {", pause_start)
    harness += "\nstatic void pause_tick(void) {" + timing[pause_start:pause_end] + "}}\n"
    harness += r'''
int main(void) {
    clc.demoPlayer=1; clc.state=CA_ACTIVE; clc.demoStartTime=50000; cl.snap.valid=1;
    cl.serverTime=50000; CL_UpdateDemoPlaybackTime(); assert(seconds==0);
    cl.serverTime=62345; CL_UpdateDemoPlaybackTime(); assert(seconds==12);
    CL_UpdateDemoPlaybackTime(); assert(seconds==12); // paused: same demo time
    cl.serverTime=72345; CL_UpdateDemoPlaybackTime(); assert(seconds==22); // forward
    cl.serverTime=52345; CL_UpdateDemoPlaybackTime(); assert(seconds==2); // backward
    cl.serverTime=50000; CL_UpdateDemoPlaybackTime(); assert(seconds==0); // restart
    cl.serverTime=49999; CL_UpdateDemoPlaybackTime(); assert(seconds==0);
    cl.serverTime=3650000; CL_UpdateDemoPlaybackTime(); assert(seconds==3600);
    clc.demoPlayer=0; CL_UpdateDemoPlaybackTime(); assert(seconds==0); // stop
    clc.demoPlayer=1; clc.state=0; CL_UpdateDemoPlaybackTime(); assert(seconds==0);
    clc.demoplaying=1; freeze.integer=1; cl.serverTime=62000;
    cls.realtime=1000; pause_tick();
    cls.realtime+=60000; pause_tick();
    assert(cls.realtime + cl.serverTimeDelta==62000);
    cls.realtime+=16;
    assert(cls.realtime + cl.serverTimeDelta==62016); // resume without a 60s jump
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "clock.c"
        binary = pathlib.Path(directory) / "clock.exe"
        c.write_text(harness)
        subprocess.run([compiler, str(c), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
