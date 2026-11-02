"""Production renderer fallback must balance nested filesystem temp buffers."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT=pathlib.Path(__file__).resolve().parents[1]


def extract(source, marker):
    start=source.index(marker);end=source.index('{',start)+1;depth=1
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[start:end]


class RendererFileFallbackTests(unittest.TestCase):
    def test_nested_loads_empty_files_and_missing_files(self):
        source=(ROOT/'engine/code/qcommon/files.c').read_text()
        c=r'''
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef unsigned char byte;
typedef int fileHandle_t;
#define MAX_OSPATH 256
#define BASEGAME "baseq3r"
#define ERR_FATAL 0
#define Com_sprintf snprintf
static int fs_loadStack,fs_loadCount,clears,closes,opens;
static void *fs_searchpaths=(void*)1;
static long normal=-1,fallback=4;
void Com_Error(int code,const char *message) { (void)code;(void)message; abort(); }
void *Hunk_AllocateTempMemory(int n) { return malloc(n); }
void Hunk_FreeTempMemory(void *p) { free(p); }
void Hunk_ClearTempMemory(void) { clears++; }
long FS_ReadFile(const char *path,void **buffer) {
    (void)path;
    if(buffer) *buffer=NULL;
    if(normal<0)return -1;
    if(buffer) { *buffer=malloc(normal+1);fs_loadStack++;fs_loadCount++; }
    return normal;
}
long FS_SV_FOpenFileRead(const char *path,fileHandle_t *handle) {
    assert(!strcmp(path,"baseq3r/test.tga"));opens++;*handle=fallback>=0?1:0;return fallback;
}
void FS_FCloseFile(fileHandle_t handle) { assert(handle==1);closes++; }
int FS_Read(void *buffer,int n,fileHandle_t handle) { assert(handle==1);memset(buffer,42,n);return n; }
'''
        c+=extract(source,'long FS_ReadFileWithHomepathFallback(')+'\n'
        c+=extract(source,'void FS_FreeFile( void *buffer )')+'\n'
        c+=r'''
int main(void) {
    void *outer,*image;int old;
    normal=8;assert(FS_ReadFile("model.md3",&outer)==8 && fs_loadStack==1);
    normal=-1;assert(FS_ReadFileWithHomepathFallback("test.tga",&image)==4);
    assert(fs_loadStack==2 && ((byte*)image)[4]==0);
    FS_FreeFile(image);assert(fs_loadStack==1 && clears==0);
    FS_FreeFile(outer);assert(fs_loadStack==0 && clears==1);
    normal=0;old=opens;
    assert(FS_ReadFileWithHomepathFallback("test.tga",&image)==0 && image);
    assert(opens==old && fs_loadStack==1);FS_FreeFile(image);
    normal=-1;old=closes;
    assert(FS_ReadFileWithHomepathFallback("test.tga",NULL)==4);
    assert(fs_loadStack==0 && closes==old+1);
    fallback=0;old=closes;image=(void*)1;
    assert(FS_ReadFileWithHomepathFallback("test.tga",&image)==-1);
    assert(!image && closes==old+1 && fs_loadStack==0);
    fallback=-1;image=(void*)1;
    assert(FS_ReadFileWithHomepathFallback("test.tga",&image)==-1);
    assert(!image && fs_loadStack==0);
    return 0;
}
'''
        compiler=shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
        with tempfile.TemporaryDirectory(dir=ROOT/'tests') as directory:
            p=pathlib.Path(directory);(p/'fallback.c').write_text(c)
            subprocess.run([compiler,str(p/'fallback.c'),'-o',str(p/'fallback.exe')],check=True)
            subprocess.run([str(p/'fallback.exe')],check=True,timeout=5)
