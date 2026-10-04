"""The QVM starts at the first function of the first object file (g_main,
cg_main, ui_main). vmMain has to be that function: any function defined above
it becomes the entry point and the module silently does nothing (seen as
"Client/Server game mismatch" because G_InitGame never ran)."""
import pathlib
import re

import pytest

CODE = pathlib.Path(__file__).resolve().parents[1] / "engine" / "code"

# A function definition: a line starting at column 0 that ends with ")" or ") {"
# and whose next non-blank line (or the line itself) opens the body.
DEFINITION = re.compile(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;]*\)\s*\{?\s*$")


def first_function(path: pathlib.Path) -> str:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    in_comment = False
    for i, line in enumerate(lines):
        stripped = line.strip()
        if in_comment:
            if "*/" in stripped:
                in_comment = False
            continue
        if stripped.startswith("/*") and "*/" not in stripped:
            in_comment = True
            continue
        if line.startswith(("#", " ", "\t", "//")) or not stripped:
            continue
        match = DEFINITION.match(line)
        if not match or match.group(1) in ("if", "while", "for", "switch", "return", "sizeof"):
            continue
        if line.rstrip().endswith("{"):
            return match.group(1)
        following = next((l.strip() for l in lines[i + 1:] if l.strip()), "")
        if following.startswith("{"):
            return match.group(1)
    return ""


@pytest.mark.parametrize("source", ["game/g_main.c", "cgame/cg_main.c", "q3_ui/ui_main.c"])
def test_vmmain_is_the_first_function(source):
    assert first_function(CODE / source) == "vmMain"
