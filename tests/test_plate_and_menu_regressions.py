"""Exercise production plate registration, custom spinners and shipped map data."""
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CODE = ROOT / 'engine/code'


def function(source, name):
    match = re.search(r'(?m)^(?:static\s+)?[\w* \t]+\b' + name + r'\s*\([^;]*?\)\s*\{', source)
    start = match.start()
    opening = source.index('{', start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


def compile_run(source, shared=False):
    compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
    with tempfile.TemporaryDirectory(dir=ROOT/'tests') as temporary:
        folder = Path(temporary)
        c = folder/'probe.c'; exe = folder/'probe.exe'
        c.write_text(source)
        command = [compiler, '-I'+str(CODE), str(c), '-o', str(exe), '-lm']
        if sys.platform.startswith('linux'):
            command.insert(1, '-DARCH_STRING="test"')
        if shared:
            command.insert(-3, str(CODE/'qcommon/q_shared.c'))
        result = subprocess.run(command, capture_output=True, text=True)
        assert result.returncode == 0, result.stdout + result.stderr
        subprocess.run([str(exe)], check=True, timeout=10)


def test_custom_spin_controls_keep_all_choices_and_hit_boxes():
    menu = (CODE/'q3_ui/ui_qmenu.c').read_text()
    options = (CODE/'q3_ui/ui_rally_startserver.c').read_text()
    begin = options.index('        n = 0;\n\ttrackLengthUiCount = 0;')
    end = options.index('\n\ty += BIGCHAR_HEIGHT+2;', begin)
    source = r'''
#include "q3_ui/ui_local.h"
#include <assert.h>
sfxHandle_t menu_move_sound = 1;
void trap_Error(const char *text) { abort(); }
char *QDECL va(char *text, ...) { return "unexpected widget"; }
float Com_Clamp(float a,float b,float x) { return x<a?a:x>b?b:x; }
static void Action_Init(menuaction_s *x) {}
void MenuField_Init(menufield_s *x) {}
static void RadioButton_Init(menuradiobutton_s *x) {}
static void Slider_Init(menuslider_s *x) {}
void Bitmap_Init(menubitmap_s *x) {}
static void Text_Init(menutext_s *x) {}
static void ScrollList_Init(menulist_s *x) {}
static void PText_Init(menutext_s *x) {}
static void BText_Init(menutext_s *x) {}
static void ListBox_Init(menulist_s *x) {}
static int allowLength[3],trackLengthValueByUiIndex[3],trackLengthUiCount;
static char *track_length_list[4];
'''
    source += function(menu, 'SpinControl_Init') + '\n'
    source += function(menu, 'SpinControl_Key') + '\n'
    source += function(menu, 'Menu_AddItem') + '\n'
    source += function(options, 'ServerOptions_TrackLengthValueFromIndex') + '\n'
    source += function(options, 'ServerOptions_TrackLengthIndexFromValue') + '\n'
    source += 'static void build_lengths(void) { int n;\n' + options[begin:end] + '\n}\n'
    source += r'''
int main(void) {
    const char *skills[] = {"I Can Win","Bring It On","Hurt Me Plenty","Hardcore","Nightmare!",NULL};
    const char *teams[] = {"Red","Blue","Green","Yellow",NULL};
    menuframework_s menu = {0}; menulist_s list = {0}; int i;
    list.generic.type = MTYPE_SPINCONTROL; list.generic.flags = QMF_NODEFAULTINIT;
    list.generic.left=100; list.generic.right=300; list.generic.top=140; list.generic.bottom=170;
    list.itemnames=skills; list.curvalue=1;
    Menu_AddItem(&menu,&list); assert(list.numitems==5);
    assert(list.generic.left==100 && list.generic.right==300 && list.generic.top==140 && list.generic.bottom==170);
    for(i=0;i<10;i++) { SpinControl_Key(&list,K_MOUSE1); assert(list.curvalue==(i+2)%5); }
    list.curvalue=0; SpinControl_Key(&list,K_LEFTARROW); assert(list.curvalue==4);
    SpinControl_Key(&list,K_RIGHTARROW); assert(list.curvalue==0);
    list.itemnames=teams; Menu_AddItem(&menu,&list); assert(list.numitems==4);
    list.generic.flags=0; list.itemnames=skills; list.generic.name="Skill";
    Menu_AddItem(&menu,&list); assert(list.numitems==5 && list.generic.right>list.generic.left);
    allowLength[0]=1; allowLength[1]=1; allowLength[2]=0; build_lengths();
    list.itemnames=(const char **)track_length_list; Menu_AddItem(&menu,&list);
    assert(list.numitems==2 && ServerOptions_TrackLengthValueFromIndex(0)==0 && ServerOptions_TrackLengthValueFromIndex(1)==1);
    list.curvalue=0; SpinControl_Key(&list,K_MOUSE1); assert(ServerOptions_TrackLengthValueFromIndex(list.curvalue)==1);
    SpinControl_Key(&list,K_MOUSE1); assert(ServerOptions_TrackLengthValueFromIndex(list.curvalue)==0);
    allowLength[0]=0; build_lengths(); assert(trackLengthUiCount==1 && ServerOptions_TrackLengthValueFromIndex(0)==1);
    allowLength[1]=0; allowLength[2]=1; build_lengths(); assert(ServerOptions_TrackLengthValueFromIndex(0)==2);
    assert(ServerOptions_TrackLengthIndexFromValue(2)==0);
    return 0;
}
'''
    compile_run(source)


def test_plate_registration_is_personal_and_generated_paths_are_narrow():
    players = (CODE/'cgame/cg_players.c').read_text()
    source = r'''
#include "cgame/cg_local.h"
#include "qcommon/rally_plate_path.h"
#include <assert.h>
static char generated[MAX_QPATH], generatedName[MAX_NAME_LENGTH], sourceName[MAX_QPATH];
static int chars, generatedCount;
void QDECL Com_Printf(const char *fmt, ...) {}
void QDECL Com_Error(int code, const char *fmt, ...) { abort(); }
float AngleNormalize180(float angle) { return angle; }
void CreateLicensePlateImage(const char *input,const char *output,const char *name,int count) {
    Q_strncpyz(sourceName,input,sizeof(sourceName)); Q_strncpyz(generated,output,sizeof(generated));
    Q_strncpyz(generatedName,name,sizeof(generatedName)); chars=count; generatedCount++;
}
qhandle_t trap_R_RegisterShader(const char *name) { return 200; }
qhandle_t trap_R_RegisterModel(const char *name) { return 300; }
'''
    source += function(players, 'CG_RegisterClientPlate') + '\n'
    source += function(players, 'CG_CopyClientInfoModel') + '\n'
    source += r'''
int main(void) {
    clientInfo_t a={0}, b={0}; char old[MAX_QPATH]; int i;
    const char *bad[]={"models/players/plates/player64.tga","models/players/plates/player01.tga",
        "models/players/plates/player0.tga/../x","models/players/plates/default.tga",
        "models/players/plates/player0.jpg","models/players/plates/player0_deadbee.tga",
        "models/players/plates/player0_deadbeef0.tga","models/players/plates/player0_deadbeez.tga",
        "models/players/plates/player99999999999999.tga",NULL};
    assert(!Rally_IsGeneratedPlateImage(NULL,qtrue));
    assert(Rally_IsGeneratedPlateImage("models/players/plates/player0.tga",qtrue));
    assert(Rally_IsGeneratedPlateImage("models/players/plates/player63_deadbeef.tga",qtrue));
    assert(Rally_IsGeneratedPlateImage("models/players/plates/player0_deadbeef",qfalse));
    assert(!Rally_IsGeneratedPlateImage("models/players/plates/player0_deadbeef",qtrue));
    for(i=0;bad[i];i++) assert(!Rally_IsGeneratedPlateImage(bad[i],qtrue));
    a.clientNum=0; a.bodyModel=55; Q_strncpyz(a.name,"Alice",sizeof(a.name));
    CG_RegisterClientPlate(&a); assert(chars==10 && a.plateShader==200 && a.plateModel==300);
    assert(!strcmp(generatedName,"Alice") && !strcmp(a.plateName,"plate_usa"));
    assert(Rally_IsGeneratedPlateImage(generated,qtrue)); strcpy(old,generated);
    b.clientNum=1; Q_strncpyz(b.name,"Bob",sizeof(b.name));
    CG_CopyClientInfoModel(&a,&b); assert(b.bodyModel==55 && generatedCount==2);
    assert(strcmp(old,generated) && !strcmp(generatedName,"Bob"));
    strcpy(old,generated); Q_strncpyz(b.name,"Renamed",sizeof(b.name));
    CG_RegisterClientPlate(&b); assert(strcmp(old,generated)); strcpy(old,generated);
    CG_RegisterClientPlate(&b); assert(!strcmp(old,generated));
    Q_strncpyz(b.plateSkinName,"eu_germany",sizeof(b.plateSkinName));
    CG_RegisterClientPlate(&b); assert(chars==20 && !strcmp(b.plateName,"plate_eu"));
    assert(strcmp(old,generated) && strstr(sourceName,"eu_germany.tga"));
    return 0;
}
'''
    compile_run(source, shared=True)


def test_canyon_cruise_menu_lengths_match_shipped_bsp_routes():
    data = (ROOT/'baseq3r/maps/q3r_sk1.bsp').read_bytes()
    offset, size = struct.unpack_from('<ii', data, 8)
    blocks = re.findall(r'\{([^{}]*)\}', data[offset:offset+size].decode('latin1'))
    entities = [{k.lower():v for k,v in re.findall(r'"([^"]*)"\s+"([^"]*)"',block)} for block in blocks]
    world = entities[0]
    allowed = {str(i) for i, label in enumerate(['short','medium','long']) if world.get('allow'+label+'track','1') != '0'}
    arena = (ROOT/'baseq3r/scripts/q3r_sk1.arena').read_text()
    advertised = set(re.search(r'tracklengths\s+"([^"]*)"',arena).group(1).split())
    assert advertised == allowed == {'0','1'}
    barriers = [e for e in entities if e.get('classname')=='func_static' and 'onlytracklength' in e]
    assert {e['onlytracklength'] for e in barriers} == allowed
    checkpoints = [e for e in entities if e.get('classname')=='rally_checkpoint']
    for length in allowed:
        active = [int(e['number']) for e in checkpoints if length in e.get('onlytracklength','012')]
        assert sorted(active) == list(range(1, 18))
        route = ROOT / f'baseq3r/maps/q3r_sk1_tl{length}.bpd'
        route_checkpoints = [int(line.split(':', 1)[0]) for line in route.read_text().splitlines() if line.strip()]
        assert route_checkpoints == sorted(active)
