"""Run the production UI file-list code against PK3-only and loose VFS listings."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


def extract(source, marker):
    start = source.index(marker)
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


class UiFileListTests(unittest.TestCase):
    def test_pk3_root_files_and_empty_random_selection(self):
        source = (ROOT / 'engine/code/q3_ui/ui_rally_tools.c').read_text()
        c = r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#define MAX_QPATH 64
#define qtrue 1
#define qfalse 0
#define BL_EXCLUDE 1
#define BL_ONLY 2
typedef int qboolean;
#define Q_stricmp strcasecmp
#define Q_stricmpn strncasecmp
#define Com_sprintf snprintf
static int ui_randSeed, loose;
int Q_rand(int *seed) { return ++*seed * -12345; }
void Q_strncpyz(char *out,const char *in,int size) { snprintf(out,size,"%s",in); }
void COM_StripExtension(const char *in,char *out,int size) {
    char *dot; Q_strncpyz(out,in,size); dot=strrchr(out,'.'); if(dot)*dot=0;
}
int trap_FS_GetFileList(const char *dir,const char *ext,char *out,int size) {
    const char *names[3]; int count=0,i,offset=0;
    if(!strcmp(ext,"/")) {
        if(loose) names[count++]=".";
        names[count++]="extras/";
    } else if(!strcmp(dir,"gfx/2d")) names[count++]="menucar01.tga";
    else if(!strcmp(dir,"gfx/2d/extras")) names[count++]="menucar02.tga";
    for(i=0;i<count;i++) {
        int len=strlen(names[i])+1; assert(offset+len<=size);
        memcpy(out+offset,names[i],len); offset+=len;
    }
    return count;
}
'''
        for marker in ('int UI_RandomInt(', 'static qboolean UI_FileListHas(', 'int UI_BuildFileList('):
            c += extract(source, marker) + '\n'
        c += r'''
int main(void) {
    char list[256][64]; int i;
    assert(UI_RandomInt(0)==0 && UI_RandomInt(-1)==0);
    for(i=0;i<100;i++) { int n=UI_RandomInt(4); assert(n>=0 && n<4); }
    memset(list,0,sizeof(list));
    assert(UI_BuildFileList("gfx/2d","tga","menucar",1,0,0,0,list)==2);
    assert(!strcmp(list[0],"01") && !strcmp(list[1],"02"));
    loose=1; memset(list,0,sizeof(list));
    assert(UI_BuildFileList("gfx/2d","tga","menucar",1,0,0,0,list)==2);
    assert(!strcmp(list[0],"01") && !strcmp(list[1],"02"));
    memset(list,0,sizeof(list));
    assert(UI_BuildFileList("gfx/2d","tga","absent",1,0,0,0,list)==0);
    return 0;
}
'''
        compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
        with tempfile.TemporaryDirectory(dir=ROOT / 'tests') as directory:
            path = pathlib.Path(directory)
            (path/'filelist.c').write_text(c)
            subprocess.run([compiler,str(path/'filelist.c'),'-o',str(path/'filelist.exe')],check=True)
            subprocess.run([str(path/'filelist.exe')],check=True,timeout=5)
