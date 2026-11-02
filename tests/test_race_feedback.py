"""Exercise production race warning decisions without a running renderer/server."""
import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def function(source, marker):
    start = source.index(marker)
    opening = source.index('{', start)
    depth, end = 1, opening + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


def test_race_feedback():
    server = (ROOT / 'engine/code/game/g_rally_mapents.c').read_text()
    hud = (ROOT / 'engine/code/cgame/cg_hud_racing.c').read_text()
    harness = '#include "' + (ROOT / 'engine/code/game/g_local.h').as_posix() + '"\n'
    harness += r'''
#include <assert.h>
level_locals_t level;
static int notices, recipient;
static char message[256];
char *QDECL va(char *format, ...) {
    static char result[1024]; va_list ap;
    va_start(ap,format); vsnprintf(result,sizeof(result),format,ap); va_end(ap);
    return result;
}
void trap_SendServerCommand(int client, const char *text) {
    notices++; recipient=client; snprintf(message,sizeof(message),"%s",text);
}
'''
    harness += function(server, 'static void G_RallyWarnMissedCheckpoint(') + '\n'
    harness += function(hud, 'static qboolean CG_IsMovingAwayFromCheckpoint(') + '\n'
    harness += r'''
int main(void) {
    static gentity_t car, gate;
    static gclient_t client;
    vec3_t velocity={0,0,0}, target={100,0,0};
    car.client=&client; car.s.number=7; car.number=2;
    client.ps.pm_type=PM_NORMAL; gate.number=4;
    level.time=10000;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    level.startRaceTime=11000;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    level.startRaceTime=1000;
    // The shared start/finish carries the last gate number, even at lap start.
    level.finishEnt=&gate; car.number=1; gate.number=20;
    client.lapStartTime=1000; client.lastCheckpointTime=1000;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    assert(client.checkpointWarningTime==0 && car.number==1);
    level.time+=5000; // repeated/slow start-line contact must remain quiet
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    // The same protection also covers the start of subsequent laps.
    car.currentLap=2; client.lapStartTime=client.lastCheckpointTime=level.time-500;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    // A separate sprint finish must still warn about missing checkpoint 1.
    level.sprintTrack=qtrue;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==1);
    assert(strstr(message,"checkpoint 1"));
    level.sprintTrack=qfalse; level.finishEnt=NULL;
    notices=0; client.checkpointWarningTime=0;
    client.lastCheckpointTime=0; car.number=2;
    gate.number=1; G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    gate.number=2; G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==0);
    gate.number=4; G_RallyWarnMissedCheckpoint(&gate,&car);
    assert(notices==1 && recipient==7 && strstr(message,"checkpoint 2"));
    assert(strchr(message,'\n') && !strstr(message,"\\n"));
    assert(car.number==2 && client.lastCheckpointTime==0);
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==1);
    level.time+=4000;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==2);
    level.time+=4000; client.finishRaceTime=level.time;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==2);
    client.finishRaceTime=0; car.r.svFlags=SVF_BOT;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==2);
    car.r.svFlags=0; client.sess.sessionTeam=TEAM_SPECTATOR;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==2);
    client.sess.sessionTeam=TEAM_FREE; client.ps.pm_type=PM_DEAD;
    G_RallyWarnMissedCheckpoint(&gate,&car); assert(notices==2);
    assert(!CG_IsMovingAwayFromCheckpoint(velocity,target,180));
    velocity[2]=200; assert(!CG_IsMovingAwayFromCheckpoint(velocity,target,180));
    velocity[0]=40; assert(!CG_IsMovingAwayFromCheckpoint(velocity,target,180));
    velocity[0]=-100;
    assert(CG_IsMovingAwayFromCheckpoint(velocity,target,180));
    assert(CG_IsMovingAwayFromCheckpoint(velocity,target,-180));
    assert(!CG_IsMovingAwayFromCheckpoint(velocity,target,100));
    assert(!CG_IsMovingAwayFromCheckpoint(velocity,target,0));
    target[0]=0; assert(!CG_IsMovingAwayFromCheckpoint(velocity,target,180));
    return 0;
}
'''
    compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
    with tempfile.TemporaryDirectory(dir=ROOT / 'tests') as directory:
        c = pathlib.Path(directory) / 'feedback.c'
        binary = pathlib.Path(directory) / 'feedback.exe'
        c.write_text(harness)
        subprocess.run([compiler,str(c),'-o',str(binary)],check=True)
        subprocess.run([str(binary)],check=True,timeout=5)
