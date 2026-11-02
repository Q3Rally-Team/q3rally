"""Production profile format and UI read/write paths with an in-memory VFS."""
import json
import pathlib
import platform
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
UUID = "12345678-1234-4234-8234-123456789abc"


def current():
    return {"profileVersion": 2,
        "identity": {"uuid": UUID, "name": "Driver", "info": {
            "gender": "", "birthDate": "", "avatar": "gfx/avatars/custom",
            "country": "DE", "favoriteCars": [{"model": "sidepipe", "skin": "blue", "rim": "wheel", "head": "driver"}]}},
        "webLink": {"state": "unlinked", "accountId": None, "profileId": None},
        "progress": {"epoch": "v0.8-local-1", "authority": "local", "currentRank": 2, "highestRank": 3,
            "stats": {"playerScore": 200, "kills": 5, "vehicles": [{"name": "sidepipe", "timeMs": 2500}]}}}


class ProfileFormatTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(dir=ROOT / "tests")
        cls.folder = pathlib.Path(cls.temp.name)
        source = (ROOT / "engine/code/q3_ui/ui_rally_profiles.c").read_text()
        names = ["UI_Profile_IsValidUUID", "UI_Profile_ParseDouble", "UI_Profile_ParseInt",
                 "UI_Profile_ParseString", "UI_Profile_FormatJsonString", "UI_Profile_ParseFavoriteCars",
                 "UI_Profile_ReadData", "UI_Profile_WriteFile"]
        functions = []
        for name in names:
            import re
            match = re.search(r"^(?:static )?[^\n;]+\b" + name + r"\([^;]*?\) \{", source, re.M)
            assert match, name
            end = source.index("\n}", match.end()) + 2
            functions.append(source[match.start():end])
        harness = r'''
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "profile_format.h"
static char disk[65536]; static int diskLength, cursor, writes;
int QDECL Com_sprintf(char *out,int size,const char *fmt,...) {
    int n; va_list args; assert(size>0);va_start(args,fmt);n=vsnprintf(out,size,fmt,args);va_end(args);return n;
}
void Q_strncpyz(char *out,const char *text,int size) { assert(size>0);snprintf(out,size,"%s",text); }
char *QDECL va(char *fmt,...) { static char out[1024];va_list args;va_start(args,fmt);vsnprintf(out,sizeof(out),fmt,args);va_end(args);return out; }
int trap_FS_FOpenFile(const char *path,fileHandle_t *file,fsMode_t mode) {
    (void)path;*file=1;cursor=0;if(mode==FS_WRITE){diskLength=0;disk[0]=0;writes++;}return diskLength;
}
void trap_FS_Read(void *out,int length,fileHandle_t file){(void)file;memcpy(out,disk+cursor,length);cursor+=length;}
void trap_FS_Write(const void *data,int length,fileHandle_t file){(void)file;assert(length<sizeof(disk));memcpy(disk,data,length);disk[length]=0;diskLength=length;}
void trap_FS_FCloseFile(fileHandle_t file){(void)file;}
void trap_Print(const char *text){(void)text;}
static qboolean UI_Profile_GetArchivedUUID(char *out,int size){(void)out;(void)size;return qfalse;}
static void UI_Profile_MigrateAvatar(char *out,int size){(void)out;(void)size;}
qboolean UI_Profile_WriteFile(const char *,const profile_info_t *,const profile_stats_t *);
''' + "\n\n".join(functions) + r'''
int main(int argc,char **argv) {
    FILE *file; profile_info_t info;profile_stats_t stats;int ok;
    assert(argc==3);file=fopen(argv[2],"rb");assert(file);
    diskLength=fread(disk,1,sizeof(disk)-1,file);disk[diskLength]=0;fclose(file);
    if(!strcmp(argv[1],"classify")){printf("%d\n",Profile_FormatClassify(disk));return 0;}
    ok=UI_Profile_ReadData("Driver",&info,&stats);
    if(ok)ok=UI_Profile_WriteFile("Driver",&info,&stats);
    printf("%d %d\n%s",ok,writes,disk);return 0;
}
'''
        code = cls.folder / "profile.c"
        cls.binary = cls.folder / "profile.exe"
        code.write_text(harness)
        flags = [] if platform.system() == "Windows" else ['-DARCH_STRING="test"']
        compiler = shutil.which("gcc") or "C:/msys64/mingw64/bin/gcc.exe"
        subprocess.run([compiler, *flags, "-I" + str(ROOT / "engine/code/qcommon"),
            str(code), str(ROOT / "engine/code/qcommon/profile_format.c"), "-o", str(cls.binary)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_profile(self, value, mode="ui"):
        text = value if isinstance(value, str) else json.dumps(value, ensure_ascii=False)
        path = self.folder / "input.json"
        path.write_text(text, encoding="utf-8")
        return subprocess.check_output([str(self.binary), mode, str(path)], encoding="utf-8", timeout=5)

    def test_legacy_identity_is_preserved_but_old_progress_is_reset(self):
        old = {"uuid": UUID, "name": "Driver", "info": {"country": "DE", "gender": "other",
            "birthDate": "2000-01-01", "avatar": "gfx/avatars/custom", "currentRank": 14, "highestRank": 14,
            "favoriteCars": current()["identity"]["info"]["favoriteCars"]},
            "stats": {"playerScore": 99999, "wins": 50, "kills": 1000}}
        status, text = self.run_profile(old).split("\n", 1)
        self.assertEqual(status, "1 2")
        result = json.loads(text)
        self.assertEqual(result["identity"]["uuid"], UUID)
        for field in ("country", "gender", "birthDate", "avatar", "favoriteCars"):
            expected = old["info"][field]
            actual = result["identity"]["info"][field]
            self.assertEqual(actual[:1] if field == "favoriteCars" else actual, expected)
        self.assertEqual(result["progress"]["stats"]["playerScore"], 0)
        self.assertEqual(result["progress"]["highestRank"], 0)
        self.assertEqual(result["webLink"], {"state": "unlinked", "accountId": None, "profileId": None})
        # The reset happens once; current profiles retain newly earned points.
        result["progress"]["stats"]["playerScore"] = 42
        _, text = self.run_profile(result).split("\n", 1)
        self.assertEqual(json.loads(text)["progress"]["stats"]["playerScore"], 42)

    def test_current_profile_roundtrip_keeps_progress_identity_and_vehicle_history(self):
        value = current()
        value["identity"]["name"] = 'Driver "Q" \\ test'
        value["progress"]["stats"]["playerScore"] = -5
        status, text = self.run_profile(value).split("\n", 1)
        self.assertEqual(status, "1 1")
        result = json.loads(text)
        self.assertEqual(result["identity"]["name"], value["identity"]["name"])
        self.assertEqual(result["progress"]["stats"]["playerScore"], -5)
        self.assertEqual(result["progress"]["stats"]["vehicles"], value["progress"]["stats"]["vehicles"])

    def test_all_statistic_fields_survive_menu_save(self):
        import re
        header = (ROOT / "engine/code/game/profile_shared.h").read_text()
        fields = header.split("typedef struct profile_stats_s {", 1)[1].split("} profile_stats_t;", 1)[0]
        value = current()
        stats = value["progress"]["stats"]
        for index, (kind, name) in enumerate(re.findall(r"^\s*(int|double|char)\s+(\w+)", fields, re.M)):
            stats[name] = "sidepipe" if kind == "char" else index + .25 if kind == "double" else index + 1
        stats["statsRevision"] = 1
        status, text = self.run_profile(value).split("\n", 1)
        self.assertEqual(status, "1 1")
        self.assertEqual(json.loads(text)["progress"]["stats"], stats)

    def test_four_long_vehicle_favorites_are_not_truncated(self):
        value = current()
        favorites = [{field: chr(97+i) * 63 for field in ("model", "skin", "rim", "head")} for i in range(4)]
        value["identity"]["info"]["favoriteCars"] = favorites
        status, text = self.run_profile(value).split("\n", 1)
        self.assertEqual(status, "1 1")
        self.assertEqual(json.loads(text)["identity"]["info"]["favoriteCars"], favorites)

    def test_future_linked_invalid_and_oversized_profiles_are_never_overwritten(self):
        cases = []
        for field, value in (("profileVersion", 3), ("profileVersion", "2")):
            item = current();item[field] = value;cases.append(json.dumps(item))
        item = current();item["webLink"]["state"] = "linked";cases.append(json.dumps(item))
        item = current();item["progress"]["authority"] = "central";cases.append(json.dumps(item))
        item = current();item["progress"]["epoch"] = "v0.9";cases.append(json.dumps(item))
        cases.append(json.dumps(current())[:-1] + ',"profileVersion":3}')
        cases.append('{"uuid":"' + UUID + '","profileVersion":3,"profileVersion":2}')
        cases += ['{"uuid":"x",}', json.dumps(current())[:-1], '{"uuid":"x","info":{"nested":' + '['*20 + '0' + ']'*20 + '}}',
                  json.dumps({"uuid": UUID, "padding": "x" * 33000})]
        for text in cases:
            with self.subTest(text=text[:50]):
                status, preserved = self.run_profile(text).split("\n", 1)
                self.assertEqual(status, "0 0")
                self.assertEqual(preserved, text)

    def test_expanding_a_large_profile_fails_before_opening_it_for_write(self):
        value = current()
        value["progress"]["stats"]["vehicles"] = [{"name": "x" * 31500, "timeMs": 10}]
        # It fits on input, but expanding all statistics exceeds the write buffer.
        text = json.dumps(value)
        status, preserved = self.run_profile(text).split("\n", 1)
        self.assertEqual(status, "0 0")
        self.assertEqual(preserved, text)
