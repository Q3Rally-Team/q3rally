"""Exercise the production console renderer with redirected and resized handles."""
from pathlib import Path
import shutil
import subprocess
import tempfile


def test_redirected_console_and_bounded_input_drawing():
    root = Path(__file__).resolve().parents[1]
    source = (root/'engine/code/sys/con_win32.c').read_text()
    start = source.index('static void CON_Show( void )')
    end = source.index('\n}', start) + 2
    harness = r'''
#include <assert.h>
#include <string.h>
typedef int qboolean;
typedef unsigned short WORD;
typedef struct { short X, Y; } COORD;
typedef struct { short Left, Top, Right, Bottom; } SMALL_RECT;
typedef struct { union { char AsciiChar; } Char; WORD Attributes; } CHAR_INFO;
typedef struct { COORD dwSize, dwCursorPosition; SMALL_RECT srWindow; } CONSOLE_SCREEN_BUFFER_INFO;
#define MAX_EDIT_LINE 128
#define COLOR_WHITE '7'
static int qconsole_hout, qconsole_linelen, qconsole_cursor;
static qboolean qconsole_initialized, qconsole_drawinput = 1;
static char qconsole_line[MAX_EDIT_LINE];
static int available, screenCalls, writes, cursorCalls, expectedOffset;
static CONSOLE_SCREEN_BUFFER_INFO screen;
static int GetConsoleScreenBufferInfo(int handle, CONSOLE_SCREEN_BUFFER_INFO *info) {
    (void)handle; screenCalls++;
    if (!available) return 0; /* Leaves info untouched, as the real failed API may. */
    *info = screen; return 1;
}
static WORD CON_ColorCharToAttrib(char color) { (void)color; return 7; }
static int Q_IsColorString(const char *text) { (void)text; return 0; }
static void WriteConsoleOutput(int handle, const CHAR_INFO *line, COORD size, COORD origin, SMALL_RECT *area) {
    int width = area->Right - area->Left + 1;
    (void)handle;
    assert(size.X == MAX_EDIT_LINE && size.Y == 1);
    assert(origin.X == expectedOffset && origin.X >= 0 && origin.X + width <= size.X);
    assert(width > 0 && width <= screen.dwSize.X);
    assert(area->Top == screen.dwCursorPosition.Y && area->Bottom == area->Top);
    assert(line[origin.X].Attributes == 7);
    writes++;
}
static void SetConsoleCursorPosition(int handle, COORD position) {
    (void)handle; assert(position.X >= 0 && position.X < screen.dwSize.X); cursorCalls++;
}
'''
    harness += source[start:end]
    harness += r'''
int main(void) {
    CON_Show(); assert(!screenCalls && !writes); /* No interactive console. */
    qconsole_initialized = 1;
    CON_Show(); assert(screenCalls == 1 && !writes); /* Redirected/closed output. */
    available = 1;
    CON_Show(); assert(!writes); /* Invalid zero-sized screen. */
    screen.dwSize.X = 80; screen.dwSize.Y = 25; screen.dwCursorPosition.Y = 25;
    CON_Show(); assert(!writes); /* Invalid cursor after a resize. */
    screen.dwCursorPosition.Y = 3;
    CON_Show(); assert(writes == 1 && cursorCalls == 1);
    qconsole_linelen = qconsole_cursor = MAX_EDIT_LINE - 1;
    memset(qconsole_line, 'a', MAX_EDIT_LINE - 1);
    expectedOffset = MAX_EDIT_LINE - 80;
    CON_Show(); assert(writes == 2);
    screen.dwSize.X = 1; expectedOffset = MAX_EDIT_LINE - 1;
    CON_Show(); assert(writes == 3);
    qconsole_cursor = 0; CON_Show(); assert(writes == 4);
    screen.dwSize.X = 200; expectedOffset = 0;
    CON_Show(); assert(writes == 5);
    qconsole_drawinput = 0; CON_Show(); assert(writes == 5);
    return 0;
}
'''
    compiler = shutil.which('gcc') or 'C:/msys64/mingw64/bin/gcc.exe'
    with tempfile.TemporaryDirectory(dir=root/'tests') as directory:
        c = Path(directory)/'console.c'
        binary = Path(directory)/'console.exe'
        c.write_text(harness)
        subprocess.run([compiler, str(c), '-o', str(binary)], check=True)
        subprocess.run([str(binary)], check=True, timeout=5)
