"""A sprint finish gate may award each driver only once."""
import pathlib,shutil,subprocess,tempfile,unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class SprintFinishTests(unittest.TestCase):
 def test_repeated_finish_contacts_do_not_award_again(self):
  s=(ROOT/'engine/code/game/g_rally_mapents.c').read_text();start=s.index('void Touch_Finish (');end=s.index('{',start)+1;depth=1
  while depth:depth+=(s[end]=='{')-(s[end]=='}');end+=1
  c='#include "'+(ROOT/'engine/code/game/g_local.h').as_posix()+'"\n'
  c+=r'''
#include <assert.h>
level_locals_t level;
vmCvar_t g_gametype,g_developer;
static int laps,placements,ladder,sends,warnings;
static int recalculations,expectedPosition;
static gentity_t *finisher;
char *QDECL va(char *format,...) { static char b[1024];va_list a;va_start(a,format);vsnprintf(b,sizeof(b),format,a);va_end(a);return b; }
void trap_SendServerCommand(int client,const char *s) { (void)client;(void)s;sends++; }
void QDECL G_Printf(const char *fmt,...) { (void)fmt; }
void QDECL Com_Printf(const char *fmt,...) { (void)fmt; }
static void G_RallyWarnMissedCheckpoint(gentity_t *gate,gentity_t *car) { (void)gate;(void)car;warnings++; }
static void G_RallyCompleteCleanSegment(gentity_t *car) { (void)car; }
static void G_RallyRecordSplitTime(gentity_t *car,int time) { (void)car;(void)time; }
static void G_RallyCompleteLap(gentity_t *car,int time,qboolean progress) { (void)car;(void)time;(void)progress;laps++; }
static void G_RallyUpdateTeamTime(gentity_t *car) { (void)car; }
static const char *G_RallyPlaceString(int place) { (void)place;return "second"; }
void G_GhostLadder_ClientFinished(gentity_t *car) { (void)car;ladder++; }
void CalculatePlayerPositions(void) { assert(finisher->client->finishRaceTime);finisher->client->ps.stats[STAT_POSITION]=expectedPosition;recalculations++; }
void G_Profile_RecordRacePlacement(gclient_t *client,int place) { (void)client;assert(place==expectedPosition);placements++; }
'''+s[start:end]+r'''
int main(void) {
 static gentity_t gate,first,second,empty;
 static gclient_t c1,c2;
 int before;
 g_gametype.integer=GT_SPRINT;level.time=1000;
 gate.number=7;first.number=7;first.s.clientNum=3;first.client=&c1;
 finisher=&first;expectedPosition=1;
 Touch_Finish(&gate,&first,NULL);
 assert(c1.finishRaceTime==1000 && level.finishRaceTime==1000 && level.winnerNumber==3);
 assert(laps==1 && placements==1 && ladder==1);before=sends;
 level.time=1400;Touch_Finish(&gate,&first,NULL);
 level.time=3000;Touch_Finish(&gate,&first,NULL);
 assert(c1.finishRaceTime==1000 && laps==1 && placements==1 && ladder==1 && sends==before && recalculations==1);
 second.number=7;second.s.clientNum=4;second.client=&c2;c2.ps.stats[STAT_POSITION]=3;
 finisher=&second;expectedPosition=2;
 Touch_Finish(&gate,&second,NULL);
 assert(c2.finishRaceTime==3000 && laps==2 && placements==2 && ladder==2 && level.winnerNumber==3 && c2.ps.stats[STAT_POSITION]==2);
 before=sends;level.time=4000;Touch_Finish(&gate,&second,NULL);Touch_Finish(&gate,&empty,NULL);
 assert(sends==before && laps==2 && warnings==0);
 return 0;
}
'''
  compiler=shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
  with tempfile.TemporaryDirectory(dir=ROOT/'tests') as directory:
   p=pathlib.Path(directory);(p/'finish.c').write_text(c);subprocess.run([compiler,'-DARCH_STRING="test"',str(p/'finish.c'),'-o',str(p/'finish.exe'),'-lm'],check=True);subprocess.run([str(p/'finish.exe')],check=True,timeout=5)
