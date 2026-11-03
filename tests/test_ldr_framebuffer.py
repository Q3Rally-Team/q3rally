"""Regression: Low needs an LDR scene target even without HDR or MSAA."""
import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_ldr_framebuffer():
    fbo = (ROOT / "engine/code/renderergl2/tr_fbo.c").read_text()
    start = fbo.index("if (multisample && glRefConfig.framebufferMultisample)", fbo.index("void FBO_Init(void)"))
    end = fbo.index("// clear render buffer", start)
    backend = (ROOT / "engine/code/renderergl2/tr_backend.c").read_text()
    begin = backend.index("const void *RB_PostProcess(const void *data)")
    guard_end = backend.index("\n\tif (cmd)", begin)
    harness = r'''#include <assert.h>
#include <stddef.h>
#define GL_DEPTH_COMPONENT24 1
#define GL_COLOR_ATTACHMENT0 2
#define GL_DEPTH_ATTACHMENT 3
typedef struct {int width,height;} image_t;
typedef struct {int id;} FBO_t;
typedef int ivec4_t[4];
typedef int qboolean;
typedef struct {int unused;} postProcessCommand_t;
static struct {int framebufferObject,framebufferMultisample;} glRefConfig;
static struct {int integer;} hdr,post;
#define r_hdr (&hdr)
#define r_postProcess (&post)
static struct {FBO_t *renderFbo,*msaaResolveFbo; image_t *renderImage,*renderDepthImage;} tr;
static struct {int numIndexes;} tess;
static FBO_t targets[2];
static int created,ended;
static FBO_t *FBO_Create(const char *name,int w,int h) {assert(w==1920 && h==1080); return &targets[created++];}
static void FBO_CreateBuffer(FBO_t *f,int format,int attachment,int samples) {assert(f);}
static void FBO_AttachImage(FBO_t *f,image_t *image,int a,int level) {assert(f && image);}
static void R_CheckFBO(FBO_t *f) {assert(f);}
static void RB_EndSurface(void) {ended++;}
static void setup(int multisample) {int hdrFormat=0; created=0; tr.renderFbo=tr.msaaResolveFbo=NULL;
'''
    harness += fbo[start:end] + "}\n"
    harness += backend[begin:guard_end] + "return data; }\n"
    harness += r'''
int main(void) {
    static image_t image={1920,1080}; postProcessCommand_t command;
    tr.renderImage=tr.renderDepthImage=&image;
    glRefConfig.framebufferObject=glRefConfig.framebufferMultisample=1;
    hdr.integer=0; post.integer=1; setup(0);
    assert(created==1 && tr.renderFbo && !tr.msaaResolveFbo); // Low
    assert(RB_PostProcess(&command)==&command); // proceeds beyond early guard
    hdr.integer=1; setup(0); assert(created==1 && tr.renderFbo);
    setup(4); assert(created==2 && tr.renderFbo && tr.msaaResolveFbo);
    hdr.integer=post.integer=0; setup(0); assert(!created && !tr.renderFbo);
    post.integer=1; tess.numIndexes=1;
    assert(RB_PostProcess(&command)==&command+1 && ended==1); // safe direct-screen fallback
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "ldr.c"
        binary = pathlib.Path(directory) / "ldr.exe"
        c.write_text(harness)
        subprocess.run([compiler, str(c), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
