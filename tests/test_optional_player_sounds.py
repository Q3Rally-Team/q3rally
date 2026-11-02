"""Exercise the production custom-sound loader with VFS/codec fixtures."""
import pathlib
import platform
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class OptionalPlayerSoundTests(unittest.TestCase):
    def test_optional_files_codecs_and_default_vehicle_fallback(self):
        source = (ROOT / 'engine/code/cgame/cg_players.c').read_text()
        names = source[source.index('char\t*cg_customSoundNames'):]
        names = names[:names.index('};') + 2]
        helper = source.split('static sfxHandle_t CG_RegisterOptionalPlayerSound(', 1)[1]
        helper = 'static sfxHandle_t CG_RegisterOptionalPlayerSound(' + helper.split('static void CG_LoadClientInfo(', 1)[0]
        loader = source.split('// Optional vehicle sounds,', 1)[1]
        loader = loader[loader.index('\tdir = ci->modelName;'):]
        loader = loader.split('// Q3Rally Code Start\n\t// engine sound:', 1)[0]
        harness = '#include "' + (ROOT / 'engine/code/cgame/cg_local.h').as_posix() + '"\n' + r'''
#include <assert.h>
#include <ctype.h>
static int calls,probes;
static const struct { const char *path; int len,handle; } files[] = {
    {"sound/player/alpine/horn.ogg",20,101},
    {"sound/player/alpine/pain25_1.mp3",20,102},
    {"sound/player/sidepipe/horn.wav",20,201},
    {"sound/player/sidepipe/falling1.wav",20,202},
    {"sound/player/mixed/horn.wav",20,0},
    {"sound/player/mixed/horn.ogg",20,301},
    {"sound/player/opusonly/horn.opus",20,401},
    {"sound/player/empty/horn.wav",0,999}
};
int trap_FS_FOpenFile(const char *path,fileHandle_t *file,fsMode_t mode) {
    int i;assert(file==NULL && mode==FS_READ);assert(!strstr(path,"sarge"));probes++;
    for(i=0;i<ARRAY_LEN(files);i++) if(!strcmp(path,files[i].path)) return files[i].len;
    return -1;
}
sfxHandle_t trap_S_RegisterSound(const char *path,qboolean compressed) {
    int i;assert(!compressed);calls++;
    for(i=0;i<ARRAY_LEN(files);i++) if(!strcmp(path,files[i].path)) {
        assert(files[i].len>0);return files[i].handle;
    }
    assert(!"Missing optional sound must never be registered");return 0;
}
int QDECL Com_sprintf(char *dest,int size,const char *fmt,...) {
    int n;va_list ap;va_start(ap,fmt);n=vsnprintf(dest,size,fmt,ap);va_end(ap);return n;
}
void COM_StripExtension(const char *in,char *out,int size) {
    const char *dot=strrchr(in,'.');int n=dot?(int)(dot-in):(int)strlen(in);
    if(n>=size)n=size-1;memcpy(out,in,n);out[n]=0;
}
int Q_stricmp(const char *a,const char *b) {
    while(*a && tolower((unsigned char)*a)==tolower((unsigned char)*b)){a++;b++;}
    return tolower((unsigned char)*a)-tolower((unsigned char)*b);
}
''' + names + '\n' + helper + r'''
static void Load(clientInfo_t *ci,int modelloaded) {
    const char *dir,*s;int i;
''' + loader + r'''
}
static int slot(const char *name) {
    int i;for(i=0;cg_customSoundNames[i];i++)if(!strcmp(name,cg_customSoundNames[i]))return i;
    assert(0);return 0;
}
int main(void) {
    static clientInfo_t ci;
    strcpy(ci.modelName,"alpine");Load(&ci,1);
    assert(ci.sounds[slot("*horn.wav")]==101);
    assert(ci.sounds[slot("*falling1.wav")]==202);
    assert(ci.sounds[slot("*pain25_1.wav")]==102);
    assert(ci.sounds[slot("*death1.wav")]==0 && calls==3);
    calls=0;strcpy(ci.modelName,"missing");Load(&ci,0);
    assert(ci.sounds[slot("*horn.wav")]==201 && ci.sounds[slot("*falling1.wav")]==202 && calls==2);
    calls=probes=0;strcpy(ci.modelName,DEFAULT_MODEL);Load(&ci,1);
    assert(calls==2 && probes<65);
    calls=0;assert(CG_RegisterOptionalPlayerSound("mixed","horn.wav")==301 && calls==2);
    calls=0;assert(CG_RegisterOptionalPlayerSound("opusonly","horn.wav")==401 && calls==1);
    calls=0;assert(CG_RegisterOptionalPlayerSound("empty","horn.wav")==0 && calls==0);
    return 0;
}
'''
        compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
        with tempfile.TemporaryDirectory(dir=ROOT / 'tests') as directory:
            folder = pathlib.Path(directory)
            (folder / 'sounds.c').write_text(harness)
            flags = [] if platform.system() == 'Windows' else [f'-DARCH_STRING="{platform.machine()}"']
            subprocess.run([compiler, *flags, str(folder / 'sounds.c'), '-o', str(folder / 'sounds.exe')], check=True)
            subprocess.run([str(folder / 'sounds.exe')], check=True, timeout=5)
