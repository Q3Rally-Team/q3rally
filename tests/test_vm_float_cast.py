"""The interpreter must implement C conversion, independently of host rounding."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class VMFloatCastTests(unittest.TestCase):
    def test_bytecode_cast_truncates_in_all_rounding_modes(self):
        source = (ROOT / 'engine/code/qcommon/vm_interpreted.c').read_text()
        body = source.split('case OP_CVFI:', 1)[1].split('goto nextInstruction;', 1)[0]
        harness = r'''
#include <assert.h>
#include <fenv.h>
#include <math.h>
#define Q_ftol lrintf
static int convert(float value) {
    int opStack[1], opStackOfs = 0;
    ((float *)opStack)[0] = value;
''' + body + r'''
    return opStack[0];
}
int main(void) {
    float values[] = {0.0f, 0.9f, -0.9f, 1.5f, -1.5f, 2.75f, -2.75f, 99.999f, -99.999f};
    int expected[] = {0, 0, 0, 1, -1, 2, -2, 99, -99};
    int modes[] = {FE_TONEAREST, FE_UPWARD, FE_DOWNWARD, FE_TOWARDZERO};
    int i, j;
    for (i = 0; i < 4; i++) {
        assert(fesetround(modes[i]) == 0);
        for (j = 0; j < 9; j++) assert(convert(values[j]) == expected[j]);
    }
    return 0;
}
'''
        compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
        with tempfile.TemporaryDirectory(dir=ROOT / 'tests') as directory:
            folder = pathlib.Path(directory)
            (folder / 'cast.c').write_text(harness)
            subprocess.run([compiler, '-fno-strict-aliasing', str(folder / 'cast.c'),
                            '-lm', '-o', str(folder / 'cast.exe')], check=True)
            subprocess.run([str(folder / 'cast.exe')], check=True, timeout=5)
