import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_preview_mouse_coordinates():
    source = (ROOT / "engine/code/client/cl_input.c").read_text()
    start = source.index("void CL_MousePositionEvent(")
    end = source.index("void CL_MouseEvent(", start)
    harness = r'''#include <assert.h>
#include <math.h>
#define KEYCATCH_UI 2
#define UI_MOUSE_POSITION 99
static int uivm=1, catcher=2, calls, lastX, lastY;
static struct { struct { int vidWidth, vidHeight; } glconfig; } cls;
static struct { int demoPlayer; } clc;
static int Key_GetCatcher(void) { return catcher; }
static void VM_Call(int vm,int command,int x,int y) { assert(command==99); calls++; lastX=x; lastY=y; }
'''
    harness += source[start:end]
    harness += r'''
int main(void) {
    clc.demoPlayer=1;
    cls.glconfig.vidWidth=1920; cls.glconfig.vidHeight=1080;
    CL_MousePositionEvent(960,540); assert(lastX==320 && lastY==240);
    CL_MousePositionEvent(1320,855); assert(lastX==480 && lastY==380);
    cls.glconfig.vidWidth=1280; cls.glconfig.vidHeight=1024;
    CL_MousePositionEvent(640,512); assert(lastX==320 && lastY==240);
    clc.demoPlayer=0; CL_MousePositionEvent(0,0); assert(calls==3);
    clc.demoPlayer=1; catcher=0; CL_MousePositionEvent(0,0); assert(calls==3);
    catcher=2; uivm=0; CL_MousePositionEvent(0,0); assert(calls==3);
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        sourcefile = pathlib.Path(directory) / "mouse.c"
        binary = pathlib.Path(directory) / "mouse.exe"
        sourcefile.write_text(harness)
        subprocess.run([compiler, str(sourcefile), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
