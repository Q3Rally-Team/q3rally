"""A moving bot that makes no progress along its leg still needs recovery."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class BezierProgressTests(unittest.TestCase):
    def test_rejoin_requires_safe_ground_and_preserves_required_gate(self):
        source = (ROOT / 'engine/code/game/ai_dmnet.c').read_text()
        function = source.split('static qboolean Bot_BezierRecover(', 1)[1]
        function = 'static qboolean Bot_BezierRecover(' + function.split('/*\n==================\nAINode_MoveToNextCheckpoint', 1)[0]
        harness = r'''
#include <assert.h>
#include <math.h>
#include <string.h>
typedef int qboolean;
typedef float vec3_t[3];
#define qtrue 1
#define qfalse 0
#define CAR_LENGTH 100
#define CAR_HEIGHT 80
#define GHOST_RECOVERY_RESET_MAX_DISTANCE 1000
#define MASK_PLAYERSOLID 1
#define CONTENTS_BODY 2
#define PITCH 0
#define ROLL 2
#define STAT_FRAC_TO_NEXT_CHECKPOINT 0
#define FLOAT2SHORT(x) ((int)((x)*32767))
typedef struct { vec3_t origin; int stats[2]; } ps_t;
typedef struct { ps_t ps; } client_t;
typedef struct { client_t *client; int currentLap; } gentity_t;
typedef struct { int entitynum; float bezierProgressFraction; ps_t cur_ps; } bot_state_t;
typedef struct { int startsolid,allsolid,contents,entityNum; float fraction; struct { vec3_t normal; } plane; vec3_t endpos; } trace_t;
static struct { int maxclients; } level = {8};
static client_t client;
static gentity_t g_entities[1] = {{&client,2}};
static int noGround,occupied,ceiling,roof,resets,traces;
static float resetFraction;
#define VectorSet(v,a,b,c) ((v)[0]=(a),(v)[1]=(b),(v)[2]=(c))
#define VectorCopy(a,b) memcpy(b,a,sizeof(vec3_t))
static float Distance(const vec3_t a,const vec3_t b) { float x=a[0]-b[0],y=a[1]-b[1],z=a[2]-b[2];return sqrtf(x*x+y*y+z*z); }
static float VectorNormalize(vec3_t v) { float n=sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);if(n){v[0]/=n;v[1]/=n;v[2]/=n;}return n; }
static void vectoangles(const vec3_t v,vec3_t a) { (void)v;VectorSet(a,0,0,0); }
static void G_GetPointOnCurveBetweenCheckpoints(gentity_t *p,gentity_t *n,float f,vec3_t o) { (void)p;(void)n;VectorSet(o,f*1000,0,120); }
static void G_GetDervOnCurveBetweenCheckpoints(gentity_t *p,gentity_t *n,float f,vec3_t o) { (void)p;(void)n;(void)f;VectorSet(o,1,0,0); }
static void trap_Trace(trace_t *t,const vec3_t s,const vec3_t mi,const vec3_t ma,const vec3_t e,int who,int mask) {
    (void)who;(void)mask;traces++;assert(mi[0]==-50 && ma[0]==50);memset(t,0,sizeof(*t));t->entityNum=100;
    if(e[2]<s[2]) { t->startsolid=roof && s[2]+ma[2]>=256;t->fraction=noGround?1:.5f;t->plane.normal[2]=1;VectorSet(t->endpos,s[0],s[1],74); }
    else t->fraction=ceiling?0:1;
}
static int Bot_GhostResetSpotOccupied(gentity_t *e,const vec3_t p) { (void)e;(void)p;return occupied; }
static void Bot_ApplyRouteReset(bot_state_t *b,const vec3_t p,vec3_t a) { (void)b;(void)a;resets++;resetFraction=p[0]/1000; }
''' + function + r'''
int main(void) {
    bot_state_t b={0};gentity_t prev={0},next={0};
    b.bezierProgressFraction=.5f;b.cur_ps.origin[0]=300;
    b.cur_ps.stats[1]=12;client.ps.stats[1]=12;
    noGround=1;assert(!Bot_BezierRecover(&b,&prev,&next) && resets==0);
    noGround=0;occupied=1;assert(!Bot_BezierRecover(&b,&prev,&next) && resets==0);
    occupied=0;ceiling=1;assert(!Bot_BezierRecover(&b,&prev,&next) && resets==0);
    ceiling=0;b.cur_ps.origin[0]=10000;traces=0;
    assert(!Bot_BezierRecover(&b,&prev,&next) && resets==0 && traces==0);
    b.cur_ps.origin[0]=300;roof=1;
    assert(Bot_BezierRecover(&b,&prev,&next) && resets==1);
    assert(resetFraction>.4f && resetFraction<.5f);
    assert(b.cur_ps.stats[1]==12 && client.ps.stats[1]==12 && g_entities[0].currentLap==2);
    return 0;
}
'''
        compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
        with tempfile.TemporaryDirectory(dir=ROOT / 'tests') as directory:
            folder = pathlib.Path(directory)
            (folder / 'rejoin.c').write_text(harness)
            subprocess.run([compiler, str(folder / 'rejoin.c'), '-lm', '-o', str(folder / 'rejoin.exe')], check=True)
            subprocess.run([str(folder / 'rejoin.exe')], check=True, timeout=5)

    def test_progress_lap_restart_and_stall(self):
        source = (ROOT / 'engine/code/game/ai_dmnet.c').read_text()
        function = source.split('static qboolean Bot_BezierProgressStalled(', 1)[1]
        function = 'static qboolean Bot_BezierProgressStalled(' + function.split('static qboolean Bot_BezierRecover(', 1)[0]
        harness = r'''
#include <assert.h>
typedef int qboolean;
#define qfalse 0
#define qtrue 1
typedef struct {
    int bezierProgressCheckpoint, bezierProgressLap;
    float bezierProgressFraction, bezierProgressTime;
} bot_state_t;
''' + function + r'''
int main(void) {
    bot_state_t b = {0};
    assert(!Bot_BezierProgressStalled(&b, 12, 1, .3f, 10));
    assert(!Bot_BezierProgressStalled(&b, 12, 1, .31f, 17));
    assert(Bot_BezierProgressStalled(&b, 12, 1, .3f, 18));
    assert(!Bot_BezierProgressStalled(&b, 12, 1, .35f, 19));
    assert(!Bot_BezierProgressStalled(&b, 13, 1, .01f, 30));
    assert(!Bot_BezierProgressStalled(&b, 13, 2, .01f, 40));
    assert(!Bot_BezierProgressStalled(&b, 13, 2, .01f, 2));
    assert(Bot_BezierProgressStalled(&b, 13, 2, .01f, 10));
    return 0;
}
'''
        compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
        with tempfile.TemporaryDirectory(dir=ROOT / 'tests') as directory:
            folder = pathlib.Path(directory)
            (folder / 'progress.c').write_text(harness)
            subprocess.run([compiler, str(folder / 'progress.c'), '-o', str(folder / 'progress.exe')], check=True)
            subprocess.run([str(folder / 'progress.exe')], check=True, timeout=5)
