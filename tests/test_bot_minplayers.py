"""Exercise population control with active, waiting, and human race clients."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
def extract(s,marker):
 start=s.index(marker);end=s.index('{',start)+1;depth=1
 while depth:depth+=(s[end]=='{')-(s[end]=='}');end+=1
 return s[start:end]
class BotPopulationTests(unittest.TestCase):
 def test_waiting_bots_population_and_removal_order(self):
  source=(ROOT/'engine/code/game/g_bot.c').read_text()
  c='#include "'+(ROOT/'engine/code/game/g_local.h').as_posix()+'"\n'
  c+=r'''
#include <assert.h>
level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
vmCvar_t g_maxclients,g_gametype,bot_minplayers;
static gclient_t clients[MAX_CLIENTS];
static int added,removed,lastAddedTeam;
static char lastCommand[128];
char *QDECL va(char *format,...) { static char b[256];va_list a;va_start(a,format);vsnprintf(b,sizeof(b),format,a);va_end(a);return b; }
void trap_Cvar_Update(vmCvar_t *c) { (void)c; }
void trap_SendConsoleCommand(int when,const char *s) { (void)when;removed++;snprintf(lastCommand,sizeof(lastCommand),"%s",s); }
qboolean BG_GametypeAllowsBots(int type) { return type!=GT_GHOST; }
void G_AddRandomBot(int team) { added++;lastAddedTeam=team; }
static void setup(int n,int bot,int team) { clients[n].pers.connected=CON_CONNECTED;clients[n].sess.sessionTeam=team;g_entities[n].r.svFlags=bot?SVF_BOT:0; }
static void tick(void) { level.time+=10001;G_CheckMinimumPlayers(); }
'''
  # Forward declaration is needed by the harness helper above.
  c=c.replace('static void tick(void)', 'void G_CheckMinimumPlayers(void);\nstatic void tick(void)')
  for m in ('int G_RemoveRandomBot(', 'int G_CountHumanPlayers(', 'int G_CountBotPlayers(', 'void G_CheckMinimumPlayers('):c+=extract(source,m)+'\n'
  c+=r'''
int main(void) {
 level.clients=clients;g_maxclients.integer=4;g_gametype.integer=GT_RACING;
 setup(0,1,TEAM_FREE);setup(1,1,TEAM_FREE);setup(2,1,TEAM_SPECTATOR);
 bot_minplayers.integer=3;tick();assert(added==0 && removed==0);
 bot_minplayers.integer=2;tick();assert(removed==1 && !strcmp(lastCommand,"clientkick 2\n"));
 clients[2].pers.connected=CON_DISCONNECTED;
 setup(2,0,TEAM_FREE);bot_minplayers.integer=3;tick();assert(added==0 && removed==1);
 clients[2].pers.connected=CON_DISCONNECTED;
 tick();assert(added==1 && lastAddedTeam==TEAM_FREE);
 setup(2,1,TEAM_SPECTATOR);bot_minplayers.integer=4;tick();assert(added==1); // one human slot reserved
 g_gametype.integer=GT_GHOST;bot_minplayers.integer=3;tick();assert(added==1 && removed==1);
 g_gametype.integer=GT_RACING;level.intermissiontime=1;bot_minplayers.integer=1;tick();assert(removed==1);
 level.intermissiontime=0;
 memset(clients,0,sizeof(clients));memset(g_entities,0,sizeof(g_entities));
 g_maxclients.integer=8;g_gametype.integer=GT_TEAM_RACING;bot_minplayers.integer=2;
 setup(0,1,TEAM_RED);setup(1,1,TEAM_BLUE);tick();assert(added==3); // team fill unchanged
 g_gametype.integer=GT_CTF4;bot_minplayers.integer=1;
 tick();assert(added==5 && lastAddedTeam==TEAM_YELLOW); // all four teams fill
 return 0;
}
'''
  compiler=shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
  with tempfile.TemporaryDirectory(dir=ROOT/'tests') as directory:
   p=pathlib.Path(directory);(p/'population.c').write_text(c)
   subprocess.run([compiler,'-DARCH_STRING="test"',str(p/'population.c'),'-o',str(p/'population.exe'),'-lm'],check=True)
   subprocess.run([str(p/'population.exe')],check=True,timeout=5)
