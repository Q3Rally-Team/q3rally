"""Run production camera/state transitions with deterministic engine stubs."""
import pathlib
import re
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_race_camera_transitions():
    active = (ROOT / "engine/code/game/g_active.c").read_text()
    main = (ROOT / "engine/code/game/g_main.c").read_text()
    client = (ROOT / "engine/code/game/g_client.c").read_text()
    def extract(source, marker):
        start = source.index(marker)
        masked = re.sub(r"/\*.*?\*/|//[^\n]*", lambda m: " " * len(m.group()), source, flags=re.S)
        opening = masked.index("{", start)
        depth, end = 1, opening + 1
        while depth:
            depth += (masked[end] == "{") - (masked[end] == "}")
            end += 1
        return source[start:end]
    observer = extract(active, "if ( ent->client->finishRaceTime && ent->client->finishRaceTime + RACE_OBSERVER_DELAY")
    end_frame_start = active.index("void ClientEndFrame( gentity_t *ent )")
    dispatch = active[end_frame_start:active.index("\t// turn off any expired powerups", end_frame_start)] + "}"
    header = (ROOT / "engine/code/game/g_local.h").as_posix()
    harness = '#include "' + header + '"\n'
    harness += r'''#include <assert.h>
level_locals_t level;
static int unlinks, observed, stops;
static gentity_t teleportEvent;
qboolean isRaceObserver(int n) { return qtrue; }
qboolean G_ClientShouldUseIntroCam(gentity_t *ent) { return qfalse; }
void SpectatorClientEndFrame(gentity_t *ent) { observed++; ent->client->ps.origin[0]=999; }
void StopFollowing(gentity_t *ent) { stops++; ent->client->sess.spectatorState=SPECTATOR_FREE; }
void FindIntermissionPoint(void) { VectorSet(level.intermission_origin,100,200,300); VectorSet(level.intermission_angle,10,90,0); }
void trap_UnlinkEntity(gentity_t *ent) { assert(ent); unlinks++; }
gentity_t *G_TempEntity(vec3_t origin,int event) { return &teleportEvent; }
gentity_t *SelectSpectatorSpawnPoint(vec3_t origin,vec3_t angles) { VectorSet(origin,400,500,600); VectorSet(angles,0,180,0); return &teleportEvent; }
'''
    harness += extract(client, "void SetClientViewAngle(") + "\n"
    harness += extract(main, "void MoveClientToIntermission(") + "\n"
    harness += dispatch + "\n"
    harness += "static void transition(gentity_t *ent) { gclient_t *client=ent->client; " + observer + "}\n"
    harness += r'''
int main(void) {
    static gentity_t ent, parts[6];
    static gclient_t client;
    int i;
    ent.client=&client; ent.frontBounds=&parts[0]; ent.rearBounds=&parts[1];
    for(i=0;i<4;i++) client.carPoints[i]=&parts[i+2];
    client.finishRaceTime=100; level.time=100+RACE_OBSERVER_DELAY+1;
    client.ps.pm_type=PM_NORMAL; client.ps.pm_flags=PMF_FOLLOW;
    VectorSet(client.ps.velocity,100,200,300);
    transition(&ent);
    assert(ent.raceObserver && ent.s.eType==ET_INVISIBLE && ent.r.contents==0);
    assert(client.ps.pm_type==PM_SPECTATOR && (client.ps.pm_flags & PMF_OBSERVE));
    assert(!(client.ps.pm_flags & PMF_FOLLOW));
    assert(client.ps.origin[0]==400 && client.ps.damageAngles[YAW]==180);
    assert(client.ps.velocity[0]==0 && unlinks==7);
    for(i=0;i<4;i++) assert(!client.carPoints[i]);
    // Already-finished observers must stay at the results camera during end frame.
    client.sess.spectatorState=SPECTATOR_OBSERVE;
    level.intermissiontime=level.time;
    MoveClientToIntermission(&ent);
    assert(stops==1 && client.ps.pm_type==PM_INTERMISSION);
    assert(!(client.ps.pm_flags & (PMF_FOLLOW|PMF_OBSERVE)));
    assert(client.ps.origin[0]==100 && client.ps.damageAngles[YAW]==90);
    ClientEndFrame(&ent);
    assert(observed==0 && client.ps.origin[0]==100);
    level.intermissiontime=0;
    ClientEndFrame(&ent);
    assert(observed==1); // normal race spectating is preserved
}
'''
    compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
    with tempfile.TemporaryDirectory(dir=ROOT / "tests") as directory:
        c = pathlib.Path(directory) / "transition.c"
        binary = pathlib.Path(directory) / "transition.exe"
        c.write_text(harness)
        subprocess.run([compiler, '-DARCH_STRING="test"', str(c), "-o", str(binary), '-lm'], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
