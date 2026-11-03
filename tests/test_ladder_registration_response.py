"""The registration error key must not match the status value 'error'."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class RegistrationResponseTests(unittest.TestCase):
 def test_property_names_and_response_whitespace(self):
  source=(ROOT/'engine/code/server/sv_ladder.c').read_text()
  start=source.index('static void SV_LadderExtractJsonString(')
  end=source.index('\n}\n',start)+3
  code='#include <assert.h>\n#include <string.h>\n#include <stddef.h>\n'+source[start:end]+r'''
int main(void) {
 char out[128],small[4];
 const char *error="{\"status\":\"error\",\"error\":\"LOCAL_CURL_PROBE_OK\"}";
 SV_LadderExtractJsonString(error,"status",out,sizeof(out));assert(!strcmp(out,"error"));
 SV_LadderExtractJsonString(error,"error",out,sizeof(out));assert(!strcmp(out,"LOCAL_CURL_PROBE_OK"));
 SV_LadderExtractJsonString("{\"status\":\"pending\",\"key\":\"123abc\"}","key",out,sizeof(out));assert(!strcmp(out,"123abc"));
 SV_LadderExtractJsonString("{\"error\"\r\n :\t\n\"message\"}","error",out,sizeof(out));assert(!strcmp(out,"message"));
 SV_LadderExtractJsonString("{\"note\":\"quoted \\\"error\\\" text\",\"error\":\"correct\"}","error",out,sizeof(out));assert(!strcmp(out,"correct"));
 SV_LadderExtractJsonString(error,"missing",out,sizeof(out));assert(!out[0]);
 SV_LadderExtractJsonString(error,"error",small,sizeof(small));assert(!strcmp(small,"LOC"));
 SV_LadderExtractJsonString(error,"error",NULL,4);
 out[0]='x';SV_LadderExtractJsonString(error,"error",out,0);assert(out[0]=='x');
 return 0;
}
'''
  compiler=shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
  with tempfile.TemporaryDirectory(dir=ROOT/'tests') as directory:
   p=pathlib.Path(directory);(p/'registration.c').write_text(code)
   subprocess.run([compiler,str(p/'registration.c'),'-o',str(p/'registration.exe')],check=True)
   subprocess.run([str(p/'registration.exe')],check=True,timeout=5)
