"""Persist spectator intent and rejoin only queued race bots after restart."""
import pathlib,shutil,subprocess,tempfile,unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
def extract(s,m):
 a=s.index(m);b=s.index('{',a)+1;d=1
 while d:d+=(s[b]=='{')-(s[b]=='}');b+=1
 return s[a:b]
class QueuedRaceBotTests(unittest.TestCase):
 def test_session_intent_and_restart_guards(self):
  session=(ROOT/'engine/code/game/g_session.c').read_text();client=(ROOT/'engine/code/game/g_client.c').read_text()
  c='#include "'+(ROOT/'engine/code/game/g_local.h').as_posix()+'"\n'+r'''
#include <assert.h>
level_locals_t level;
vmCvar_t g_gametype,g_rallyIgnoreBots;
static char stored[1024];
char *QDECL va(char *format,...) { static char b[4][1024];static int n;va_list a;char *p=b[(n++)%4];va_start(a,format);vsnprintf(p,1024,format,a);va_end(a);return p; }
void trap_Cvar_Set(const char *name,const char *value) { (void)name;snprintf(stored,sizeof(stored),"%s",value); }
void trap_Cvar_VariableStringBuffer(const char *name,char *buffer,int size) { (void)name;snprintf(buffer,size,"%s",stored); }
qboolean isRallyRace(void) { return g_gametype.integer==GT_RACING || g_gametype.integer==GT_SPRINT || g_gametype.integer==GT_TEAM_RACING; }
'''
  c+=extract(session,'void G_WriteClientSessionData(')+'\n'+extract(session,'void G_ReadSessionData(')+'\n'+extract(client,'static void G_RestoreWaitingRaceBot(')+'\n'
  c+=r'''
int main(void) {
 static gclient_t client;
 level.clients=&client;g_gametype.integer=GT_SPRINT;
 client.sess.sessionTeam=TEAM_SPECTATOR;client.sess.spectatorState=SPECTATOR_FREE;
 client.sess.headlights=qtrue;client.sess.spectatorWilling=qfalse;
 G_WriteClientSessionData(&client);memset(&client,0,sizeof(client));G_ReadSessionData(&client);
 assert(client.sess.sessionTeam==TEAM_SPECTATOR && !client.sess.spectatorWilling && client.sess.headlights);
 level.startRaceTime=100;G_RestoreWaitingRaceBot(&client,qfalse,qtrue);assert(client.sess.sessionTeam==TEAM_SPECTATOR);
 level.startRaceTime=0;G_RestoreWaitingRaceBot(&client,qfalse,qfalse);assert(client.sess.sessionTeam==TEAM_SPECTATOR);
 G_RestoreWaitingRaceBot(&client,qtrue,qtrue);assert(client.sess.sessionTeam==TEAM_SPECTATOR);
 G_RestoreWaitingRaceBot(&client,qfalse,qtrue);assert(client.sess.sessionTeam==TEAM_FREE && client.sess.spectatorState==SPECTATOR_NOT);
 client.sess.sessionTeam=TEAM_SPECTATOR;client.sess.spectatorWilling=qtrue;
 G_WriteClientSessionData(&client);memset(&client,0,sizeof(client));G_ReadSessionData(&client);
 G_RestoreWaitingRaceBot(&client,qfalse,qtrue);assert(client.sess.sessionTeam==TEAM_SPECTATOR && client.sess.spectatorWilling);
 snprintf(stored,sizeof(stored),"%d 0 %d 0 0 0 0 1",TEAM_SPECTATOR,SPECTATOR_FREE);
 G_ReadSessionData(&client);assert(client.sess.spectatorWilling); // conservative legacy session
 client.sess.spectatorWilling=qfalse;g_rallyIgnoreBots.integer=1;
 G_RestoreWaitingRaceBot(&client,qfalse,qtrue);assert(client.sess.sessionTeam==TEAM_SPECTATOR);
 g_rallyIgnoreBots.integer=0;g_gametype.integer=GT_TEAM_RACING;
 G_RestoreWaitingRaceBot(&client,qfalse,qtrue);assert(client.sess.sessionTeam==TEAM_SPECTATOR);
 return 0;
}
'''
  compiler=shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
  with tempfile.TemporaryDirectory(dir=ROOT/'tests') as directory:
   p=pathlib.Path(directory);(p/'queue.c').write_text(c);subprocess.run([compiler,'-DARCH_STRING="test"',str(p/'queue.c'),'-o',str(p/'queue.exe'),'-lm'],check=True);subprocess.run([str(p/'queue.exe')],check=True,timeout=5)
