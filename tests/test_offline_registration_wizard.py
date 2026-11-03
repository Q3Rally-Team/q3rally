"""Exercise real wizard transitions and existing-profile registration guards."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

def function(source, signature):
    start = re.search(re.escape(signature) + r'[^;{]*\{', source).start()
    return source[start:source.index('\n}', start) + 2]

def run(code):
    compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
    with tempfile.TemporaryDirectory(dir=ROOT/'tests') as directory:
        c=Path(directory)/'wizard.c'; binary=Path(directory)/'wizard.exe'
        c.write_text(code)
        subprocess.run([compiler,str(c),'-o',str(binary)],check=True)
        subprocess.run([str(binary)],check=True,timeout=5)

def test_new_profile_next_and_skip_reach_optional_offline_registration():
    source=(ROOT/'engine/code/q3_ui/ui_rally_profile_wizard.c').read_text()
    constants='\n'.join(re.findall(r'^#define (?:ID_PW_\w+|PW_PAGES)\s+[^\n]+',source,re.M))
    run('#include <assert.h>\n'+constants+r'''
#define QM_ACTIVATED 1
#define PW_PAGE_NAME 0
#define PW_PAGE_DETAILS 1
#define PW_PAGE_LADDER 2
#define PW_PAGE_DONE 3
#define PW_RESULT_NONE 0
#define PW_RESULT_OK 1
#define PW_RESULT_ERROR 2
typedef struct {int id;} menucommon_s;
static struct {int page,ladderResult,offlineKeyResult;struct {int curvalue;} avatarSpin;} s_pw;
static int committed,requests,popped;
static void PW_LoadAvatarShader(int n) {(void)n;}
static int PW_CommitName(void) {return 1;}
static void PW_CommitDetails(void) {committed++;}
static void PW_SetPage(int n) {s_pw.page=n;}
static void PW_StartLadderRegister(void) {requests++;}
static void UI_PopMenu(void) {popped++;}
'''+function(source,'static void PW_MenuEvent(')+r'''
int main(void) {
 menucommon_s next={ID_PW_NEXT},skip={ID_PW_SKIP},back={ID_PW_BACK};
 assert(PW_PAGES==3);
 s_pw.page=PW_PAGE_DETAILS; PW_MenuEvent(&next,QM_ACTIVATED);
 assert(committed==1 && s_pw.page==PW_PAGE_LADDER && !requests && !popped);
 PW_MenuEvent(&next,QM_ACTIVATED); assert(requests==1 && s_pw.page==PW_PAGE_LADDER);
 s_pw.offlineKeyResult=PW_RESULT_ERROR; PW_MenuEvent(&next,QM_ACTIVATED);
 assert(requests==2 && s_pw.offlineKeyResult==PW_RESULT_NONE);
 s_pw.ladderResult=s_pw.offlineKeyResult=PW_RESULT_OK;
 PW_MenuEvent(&next,QM_ACTIVATED); assert(s_pw.page==PW_PAGE_DONE && !popped);
 PW_MenuEvent(&next,QM_ACTIVATED); assert(popped==1);
 s_pw.page=PW_PAGE_DETAILS; PW_MenuEvent(&skip,QM_ACTIVATED);
 assert(committed==2 && s_pw.page==PW_PAGE_LADDER);
 PW_MenuEvent(&back,QM_ACTIVATED); assert(s_pw.page==PW_PAGE_DETAILS);
 PW_MenuEvent(&skip,QM_ACTIVATED); PW_MenuEvent(&skip,QM_ACTIVATED);
 assert(s_pw.page==PW_PAGE_DONE && requests==2);
}
''')

def test_existing_profile_is_offered_registration_without_cross_profile_flags():
    source=(ROOT/'engine/code/q3_ui/ui_rally_ladder_wizard.c').read_text()
    run(r'''
#include <assert.h>
#include <string.h>
#define PROFILE_MAX_NAME 128
static int ui_ladderWizardDismissed,ui_ladderWizardDismissedProfiles;
static int ready,creating,shown;
static char active[128],dismissed[1024];
static void LadderWizard_RegisterCvars(void) {}
static void trap_Cvar_Update(int *v) {(void)v;}
static void LadderWizard_GetActiveProfile(char *out,int n) {snprintf(out,n,"%s",active);}
static int UI_ProfileWizard_IsActive(void) {return creating;}
static int trap_Cvar_VariableValue(const char *n) {(void)n;return ready;}
static void trap_Cvar_VariableStringBuffer(const char *n,char *out,int size) {(void)n;snprintf(out,size,"%s",dismissed);}
static int LadderWizard_ProfileListContains(const char *list,const char *name) {return !strcmp(list,name);}
static void UI_LadderWizardMenu(void) {shown++;}
'''.replace('#include <string.h>','#include <string.h>\n#include <stdio.h>')+function(source,'void UI_LadderWizard_MaybeShow(')+r'''
int main(void) {
 UI_LadderWizard_MaybeShow();assert(!shown);
 strcpy(active,"Existing08Profile");creating=1;UI_LadderWizard_MaybeShow();assert(!shown);
 creating=0;ready=1;UI_LadderWizard_MaybeShow();assert(!shown);
 ready=0;strcpy(dismissed,active);UI_LadderWizard_MaybeShow();assert(!shown);
 strcpy(dismissed,"AnotherProfile");UI_LadderWizard_MaybeShow();assert(shown==1);
}
''')

def test_engine_accepts_pending_and_active_but_not_failed_registration():
    source=(ROOT/'engine/code/server/sv_ladder.c').read_text()
    start=source.index('if ( Q_stricmp( statusField, "pending" ) != 0')+len('if ( ')
    end=source.index(' {',start)
    condition=source[start:end].rstrip()
    assert condition.endswith(')')
    condition=condition[:-1].rstrip()
    run(r'''
#include <assert.h>
#include <ctype.h>
static int Q_stricmp(const char *a,const char *b) {
 while (*a && tolower((unsigned char)*a)==tolower((unsigned char)*b)) {a++;b++;}
 return tolower((unsigned char)*a)-tolower((unsigned char)*b);
}
static int rejected(const char *statusField) {return '''+condition+r''';}
int main(void) {
 assert(!rejected("pending") && !rejected("active") && !rejected("ACTIVE"));
 assert(rejected("error") && rejected("revoked") && rejected("") && rejected("success"));
}
''')
