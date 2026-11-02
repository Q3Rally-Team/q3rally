"""Website palette and process-local fonts; never installs system fonts."""
import ctypes
import ctypes.util
import logging
from pathlib import Path
import sys

BACKGROUND = "#0a0a0a"
PANEL = "#111111"
FIELD = "#181818"
BORDER = "#2a2a2a"
ACCENT = "#ff5500"
ACCENT_DIM = "#cc4400"
TEXT = "#ffffff"
MUTED = "#bbbbbb"
QUIET = "#888888"


def load_fonts():
    paths = sorted(Path(__file__).with_name("assets").joinpath("fonts").glob("*.ttf"))
    try:
        if sys.platform == "win32":
            add = ctypes.windll.gdi32.AddFontResourceExW
            add.argtypes = [ctypes.c_wchar_p, ctypes.c_uint, ctypes.c_void_p]
            for path in paths:
                add(str(path), 0x10, None)  # FR_PRIVATE: visible to this process only.
        elif sys.platform == "darwin":
            foundation = ctypes.CDLL("/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation")
            text = ctypes.CDLL("/System/Library/Frameworks/CoreText.framework/CoreText")
            foundation.CFURLCreateFromFileSystemRepresentation.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_long, ctypes.c_bool]
            foundation.CFURLCreateFromFileSystemRepresentation.restype = ctypes.c_void_p
            foundation.CFRelease.argtypes = [ctypes.c_void_p]
            text.CTFontManagerRegisterFontsForURL.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_void_p]
            for path in paths:
                raw = str(path).encode()
                url = foundation.CFURLCreateFromFileSystemRepresentation(None, raw, len(raw), False)
                if url:
                    text.CTFontManagerRegisterFontsForURL(url, 1, None)
                    foundation.CFRelease(url)
        else:
            config = ctypes.CDLL(ctypes.util.find_library("fontconfig") or "libfontconfig.so.1")
            config.FcConfigGetCurrent.restype = ctypes.c_void_p
            config.FcConfigAppFontAddFile.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
            config.FcConfigBuildFonts.argtypes = [ctypes.c_void_p]
            current = config.FcConfigGetCurrent()
            for path in paths:
                config.FcConfigAppFontAddFile(current, str(path).encode())
            config.FcConfigBuildFonts(current)
    except (OSError, AttributeError):
        logging.exception("Bundled fonts unavailable; using platform fallbacks")


def configure(root, ttk):
    import tkinter.font as font
    families = set(font.families(root))
    body = "Barlow" if "Barlow" in families else "Arial"
    heading = "Barlow Condensed" if "Barlow Condensed" in families else body
    mono = "Share Tech Mono" if "Share Tech Mono" in families else "Courier"
    style = ttk.Style(root)
    style.theme_use("clam")
    style.configure(".", background=BACKGROUND, foreground=TEXT, font=(body, 12))
    style.configure("Card.TFrame", background=PANEL, bordercolor=BORDER, borderwidth=1, relief="solid")
    style.configure("Card.TLabel", background=PANEL)
    style.configure("Muted.TLabel", foreground=MUTED)
    style.configure("Eyebrow.TLabel", foreground=QUIET, font=(mono, 10))
    style.configure("Title.TLabel", font=(heading, 28, "bold"))
    style.configure("TButton", background=FIELD, foreground=TEXT, bordercolor=BORDER,
                    lightcolor=FIELD, darkcolor=FIELD, borderwidth=1, relief="flat", padding=(18, 11))
    style.map("TButton", background=[("active", "#252525"), ("disabled", PANEL)],
              foreground=[("disabled", "#686868")], bordercolor=[("focus", ACCENT)])
    style.configure("Primary.TButton", background=ACCENT, foreground=TEXT, bordercolor=ACCENT,
                    lightcolor=ACCENT, darkcolor=ACCENT, font=(heading, 16, "bold"), padding=(30, 15))
    style.map("Primary.TButton", background=[("disabled", "#332219"), ("active", ACCENT_DIM)],
              foreground=[("disabled", QUIET)], bordercolor=[("disabled", "#443126"), ("!disabled", ACCENT)])
    style.configure("Quiet.TButton", background=BACKGROUND, borderwidth=0, padding=(10, 8), foreground=MUTED)
    style.configure("Treeview", background=PANEL, fieldbackground=PANEL, foreground=TEXT,
                    rowheight=58, borderwidth=0, bordercolor=PANEL, lightcolor=PANEL,
                    darkcolor=PANEL, relief="flat", font=(body, 12))
    style.map("Treeview", background=[("selected", "#422213")], foreground=[("selected", TEXT)],
              bordercolor=[("focus", BORDER)])
    style.configure("Treeview.Heading", background=PANEL, foreground=QUIET, borderwidth=0,
                    relief="flat", font=(mono, 10), padding=(8, 12))
    style.map("Treeview.Heading", background=[("active", PANEL)])
    style.configure("Horizontal.TProgressbar", background=ACCENT, troughcolor=BORDER,
                    borderwidth=0, bordercolor=BORDER, lightcolor=ACCENT, darkcolor=ACCENT)
    style.configure("Vertical.TScrollbar", background=BORDER, troughcolor=PANEL, borderwidth=0,
                    bordercolor=PANEL, arrowcolor=QUIET, lightcolor=BORDER, darkcolor=BORDER, arrowsize=12)
    style.map("Vertical.TScrollbar", background=[("active", "#444444"), ("!active", BORDER)])
    return body, heading, mono
