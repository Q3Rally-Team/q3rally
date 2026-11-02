"""Exercise the actual preset table and pending-value application/detection."""
import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_graphics_presets():
    source = (ROOT / "engine/code/q3_ui/ui_advanced_graphics.c").read_text()
    table_start = source.index("static const gfxPresetCvar_t gfxPresetTable")
    table_end = source.index("/*", source.index("const char *gfx_preset_names", table_start))
    pending_start = source.index("#define GFX_MAX_PENDING")
    functions_end = source.index("\n}\n", source.index("void UI_GfxPreset_Apply")) + 3
    harness = r'''#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define GFX_NUM_PRESETS 4
#define MAX_CVAR_VALUE_STRING 256
#define qtrue 1
#define qfalse 0
#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))
#define Com_sprintf snprintf
#define Q_stricmp strcasecmp
typedef int qboolean;
typedef struct { const char *cvar; const char *value[4]; } gfxPresetCvar_t;
static qboolean gfxRestart;
static int gl2=1, latch;
static struct {char name[64],value[128];} cvars[128];
static int count;
static void Q_strncpyz(char *dst,const char *src,int size) {snprintf(dst,size,"%s",src);}
static int find(const char *name) {int i; for(i=0;i<count;i++) if(!strcmp(cvars[i].name,name)) return i; strcpy(cvars[count].name,name); return count++;}
static void trap_Cvar_Set(const char *name,const char *value) {if(!latch) strcpy(cvars[find(name)].value,value);}
static void trap_Cvar_VariableStringBuffer(const char *name,char *buf,int size) {Q_strncpyz(buf,cvars[find(name)].value,size);}
static qboolean Gfx_RendererIsGL2(void) {return gl2;}
'''
    harness += source[table_start:table_end] + source[pending_start:functions_end]
    harness += r'''
int main(void) {
    int renderer,level;
    for(renderer=0;renderer<2;renderer++) {
        gl2=renderer;
        for(level=0;level<4;level++) {
            gfxNumPending=0; latch=0;
            trap_Cvar_Set("r_colorLut","gfx/luts/warm");
            trap_Cvar_Set("r_sunShadowCasters","0");
            trap_Cvar_Set("cg_tireSmoke","0");
            latch=1; gfxRestart=0;
            UI_GfxPreset_Apply(level);
            assert(gfxRestart && UI_GfxPreset_Detect()==level);
            assert(Gfx_CvarIs("r_colorLut","") && Gfx_CvarIs("r_sunShadowCasters","1"));
            assert(Gfx_CvarIs("cg_tireSmoke","1"));
            assert(UI_GfxMsaa_Read()==gfxPresetMsaa[level]);
            UI_GfxSetCvar("r_colorLut","gfx/luts/cold");
            assert(UI_GfxPreset_Detect()==GFX_NUM_PRESETS);
            UI_GfxPreset_Apply(level);
            assert(UI_GfxPreset_Detect()==level);
        }
    }
    puts("Graphics presets passed for GL1 and GL2");
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "presets.c"
        binary = pathlib.Path(directory) / "presets.exe"
        c.write_text(harness)
        subprocess.run([compiler, str(c), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
