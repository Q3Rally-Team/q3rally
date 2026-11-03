"""Desktop UI and command line entry point for the Q3Rally launcher."""
import argparse
import json
import logging
import os
from pathlib import Path
import queue
import subprocess
import sys
import threading

from . import __version__
from .core import Manager, LauncherError, atomic_json, default_root, version_key
from . import theme


class MenuAction:
    """Keep the existing action state handling for commands in the tools menu."""
    def __init__(self, menu, label, command):
        self.menu = menu
        menu.add_command(label=label, command=command)
        self.index = menu.index("end")

    def configure(self, **options):
        self.menu.entryconfigure(self.index, **options)

    def __getitem__(self, key):
        return self.menu.entrycget(self.index, key)


def open_folder(path):
    path.mkdir(parents=True, exist_ok=True)
    if sys.platform == "win32":
        os.startfile(path)
    else:
        subprocess.Popen(["open" if sys.platform == "darwin" else "xdg-open", str(path)],
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def start_launcher(executable, root, config=None):
    command = [str(executable), "--data-dir", str(root)]
    if config:
        command += ["--config", str(config)]
    options = {"creationflags": subprocess.CREATE_NO_WINDOW} if os.name == "nt" else {"start_new_session": True}
    return subprocess.Popen(command, cwd=executable.parent, **options)


class Window:
    def __init__(self, manager, config, config_path=None):
        import tkinter as tk
        from tkinter import ttk, messagebox, filedialog
        self.tk, self.ttk, self.messages, self.filedialog = tk, ttk, messagebox, filedialog
        self.manager, self.config, self.config_path = manager, config, config_path
        self.envelope = self.catalog = None
        self.fresh = False
        self.items, self.local = {}, set()
        self.events = queue.Queue()
        self.cancel = threading.Event()
        self.busy = False
        self.cancellable = False
        theme.load_fonts()
        self.root = tk.Tk()
        self.root.title(f"Q3Rally Launcher {__version__}")
        width = min(1180, max(1000, self.root.winfo_screenwidth() - 80))
        height = min(800, max(700, self.root.winfo_screenheight() - 100))
        self.root.geometry(f"{width}x{height}"); self.root.minsize(1000, 700)
        self.root.configure(background=theme.BACKGROUND)
        body, heading, mono = theme.configure(self.root, ttk)
        self.root.columnconfigure(0, weight=1); self.root.rowconfigure(3, weight=1)
        top = ttk.Frame(self.root)
        top.grid(row=0, column=0, sticky="ew", padx=32, pady=(16, 12))
        top.columnconfigure(0, weight=1)
        ttk.Label(top, text="FREE / OPEN SOURCE / COMMUNITY BUILT", style="Eyebrow.TLabel").grid(row=0, column=0, sticky="w")
        ttk.Label(top, text=f"LAUNCHER {__version__}", style="Eyebrow.TLabel").grid(row=0, column=1, sticky="e")
        header = tk.Canvas(self.root, height=114, background=theme.PANEL, highlightthickness=0)
        header.grid(row=1, column=0, sticky="ew")
        def paint_header(event):
            header.delete("all")
            for x in range(max(460, event.width - 480), event.width + 140, 18):
                header.create_line(x, 114, x + 114, 0, fill="#1c1c1c")
            header.create_rectangle(0, 0, 5, 114, fill=theme.ACCENT, outline="")
            header.create_text(32, 38, anchor="w", text="Q3RALLY", fill=theme.TEXT, font=(heading, 37, "bold"))
            header.create_text(34, 83, anchor="w", text="IT'S DAMN FAST, BABY.", fill=theme.ACCENT, font=(mono, 11))
            header.create_text(event.width - 32, 57, anchor="e", text="YOUR NEXT RACE.", fill=theme.MUTED, font=(heading, 20, "bold"))
        header.bind("<Configure>", paint_header)
        intro = ttk.Frame(self.root)
        intro.grid(row=2, column=0, sticky="ew", padx=32, pady=(25, 18))
        intro.columnconfigure(0, weight=1)
        ttk.Label(intro, text="PLAY", font=(heading, 25, "bold")).grid(row=0, column=0, sticky="w")
        self.recommendation = tk.StringVar(value="Choose a version and play")
        ttk.Label(intro, textvariable=self.recommendation, style="Muted.TLabel").grid(row=1, column=0, sticky="w", pady=(4, 0))
        self.connection = tk.StringVar(value="CONNECTING")
        ttk.Label(intro, textvariable=self.connection, style="Eyebrow.TLabel").grid(row=1, column=1, sticky="e")
        content = ttk.Frame(self.root); content.grid(row=3, column=0, sticky="nsew", padx=32)
        content.columnconfigure(0, minsize=330); content.columnconfigure(1, weight=1); content.rowconfigure(0, weight=1)
        versions = ttk.Frame(content, style="Card.TFrame", padding=18)
        versions.grid(row=0, column=0, sticky="nsew", padx=(0, 24))
        versions.columnconfigure(0, weight=1); versions.rowconfigure(1, weight=1)
        ttk.Label(versions, text="01 / GAME VERSIONS", background=theme.PANEL, foreground=theme.QUIET,
                  font=(mono, 10)).grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 14))
        self.list = ttk.Treeview(versions, columns=("version", "status"), show="headings", selectmode="browse", height=4)
        self.list.heading("version", text="VERSION"); self.list.heading("status", text="STATUS")
        self.list.column("version", width=82, stretch=False); self.list.column("status", width=190)
        self.list.grid(row=1, column=0, sticky="nsew")
        version_scroll = ttk.Scrollbar(versions, command=self.list.yview)
        version_scroll.grid(row=1, column=1, sticky="ns")
        self.list.configure(yscrollcommand=version_scroll.set)
        ttk.Label(versions, text="Each version has its own installation\nand saved games.", background=theme.PANEL,
                  foreground=theme.QUIET, font=(body, 10)).grid(row=2, column=0, columnspan=2, sticky="w", pady=(18, 0))
        self.list.bind("<<TreeviewSelect>>", self.selected)
        notes_frame = ttk.Frame(content); notes_frame.grid(row=0, column=1, sticky="nsew")
        notes_frame.rowconfigure(4, weight=1); notes_frame.columnconfigure(0, weight=1)
        ttk.Label(notes_frame, text="02 / YOUR SELECTION", style="Eyebrow.TLabel").grid(row=0, column=0, sticky="w")
        self.selection_title = tk.StringVar(value="Ready for the next round?")
        ttk.Label(notes_frame, textvariable=self.selection_title, style="Title.TLabel").grid(row=1, column=0, sticky="w", pady=(8, 3))
        self.selection_meta = tk.StringVar(value="Select a game version on the left.")
        ttk.Label(notes_frame, textvariable=self.selection_meta, style="Muted.TLabel").grid(row=2, column=0, sticky="w", pady=(0, 20))
        ttk.Label(notes_frame, text="RELEASE NOTES", style="Eyebrow.TLabel").grid(row=3, column=0, sticky="w", pady=(0, 10))
        self.notes = tk.Text(notes_frame, wrap="word", state="disabled", background=theme.FIELD, foreground=theme.MUTED,
                             borderwidth=0, highlightthickness=1, highlightbackground=theme.BORDER,
                             highlightcolor=theme.ACCENT, padx=20, pady=18, font=(body, 12), spacing1=3, spacing3=5, width=40, height=6)
        self.notes.grid(row=4, column=0, sticky="nsew")
        scroll = ttk.Scrollbar(notes_frame, command=self.notes.yview); scroll.grid(row=4, column=1, sticky="ns")
        self.notes.configure(yscrollcommand=scroll.set)
        actions = ttk.Frame(self.root, style="Card.TFrame", padding=(20, 18))
        actions.grid(row=4, column=0, sticky="ew", padx=32, pady=(24, 0))
        actions.columnconfigure(0, weight=1)
        self.action_title = tk.StringVar(value="Choose your game version")
        ttk.Label(actions, textvariable=self.action_title, background=theme.PANEL, font=(heading, 18, "bold")).grid(row=0, column=0, sticky="w")
        self.manage_button = ttk.Button(actions, text="Manage version  ▾", command=self.show_management)
        self.manage_button.grid(row=0, column=1, rowspan=2, padx=(20, 14))
        self.play = ttk.Button(actions, text="PLAY  →", style="Primary.TButton", command=lambda: self.install_or_play(True))
        self.play.grid(row=0, column=2, rowspan=2, sticky="e")
        self.status = tk.StringVar(value="Checking available versions …")
        self.status_label = ttk.Label(actions, textvariable=self.status, background=theme.PANEL, foreground=theme.QUIET, wraplength=480, width=1)
        self.status_label.grid(row=1, column=0, sticky="ew", pady=(6, 0))
        self.status_label.bind("<Configure>", lambda event: self.status_label.configure(wraplength=max(180, event.width)))
        self.progress = ttk.Progressbar(actions, maximum=100)
        self.progress.grid(row=2, column=0, columnspan=3, sticky="ew", pady=(16, 0), ipady=0)
        self.progress.configure(length=100)
        self.progress.grid_remove()
        self.pause = ttk.Button(actions, text="Pause download", command=self.cancel.set)
        self.pause.grid(row=3, column=0, columnspan=3, sticky="e", pady=(12, 0)); self.pause.grid_remove()
        self.management = tk.Menu(self.root, tearoff=False, background=theme.FIELD, foreground=theme.TEXT,
                                  activebackground=theme.ACCENT_DIM, activeforeground=theme.TEXT, font=(body, 12),
                                  borderwidth=0, activeborderwidth=0, relief="flat")
        self.install = MenuAction(self.management, "Install only / reinstall", lambda: self.install_or_play(False))
        self.repair = MenuAction(self.management, "Verify game files", self.verify)
        self.import_button = MenuAction(self.management, "Import profile …", self.import_profile)
        self.management.add_separator()
        self.remove = MenuAction(self.management, "Remove game version …", self.uninstall)
        footer = ttk.Frame(self.root); footer.grid(row=5, column=0, sticky="ew", padx=22, pady=(12, 16))
        footer.columnconfigure(1, weight=1)
        self.refresh_button = ttk.Button(footer, text="Refresh versions", style="Quiet.TButton", command=self.refresh)
        self.refresh_button.grid(row=0, column=0, sticky="w")
        self.update_button = ttk.Button(footer, text="Update launcher", command=self.update_launcher)
        self.update_button.grid(row=0, column=2, padx=12); self.update_button.grid_remove()
        self.logs_button = ttk.Button(footer, text="Logs", style="Quiet.TButton", command=lambda: open_folder(manager.root / "logs"))
        self.logs_button.grid(row=0, column=3, sticky="e")
        self.preferences = manager.root / "preferences.json"
        try:
            self.preferred = json.loads(self.preferences.read_text()).get("selected")
        except (OSError, ValueError):
            self.preferred = None
        self.envelope, self.catalog = manager.cached_catalog()
        self.fill()
        self.root.protocol("WM_DELETE_WINDOW", self.close)
        self.root.after(80, self.poll)
        self.root.after(100, self.refresh)

    def show_management(self):
        try:
            self.management.tk_popup(self.manage_button.winfo_rootx(),
                                     self.manage_button.winfo_rooty() + self.manage_button.winfo_height() + 6)
        finally:
            self.management.grab_release()

    def fill(self):
        current = self.list.selection()
        wanted = current[0] if current else self.preferred
        self.items = {}
        installed = self.manager.installed()
        self.local = {f"{item[0]['version']}|{item[1]}" for item in installed}
        for item in installed + self.manager.available(self.catalog or {}):
            self.items[f"{item[0]['version']}|{item[1]}"] = item
        for key in self.list.get_children():
            self.list.delete(key)
        available = self.manager.available(self.catalog or {})
        newest = available[0][0]["version"] if available else None
        if newest:
            self.recommendation.set(f"Recommended: Q3Rally v{newest}")
        else:
            self.recommendation.set("Choose a version and play")
        for key, item in sorted(self.items.items(), key=lambda pair: version_key(pair[1][0]["version"]), reverse=True):
            status = "Installed" if key in self.local else "Available"
            if item[0]["version"] == newest:
                status += " · Recommended"
            self.list.insert("", "end", iid=key, values=("v" + item[0]["version"], status))
        if self.items:
            key = wanted if wanted in self.items else next(iter(self.list.get_children()))
            self.list.selection_set(key)
        else:
            self.notes.configure(state="normal"); self.notes.delete("1.0", "end")
            self.notes.insert("1.0", "No compatible version is available yet. Installed versions can also be played offline.")
            self.notes.configure(state="disabled")
        self.selected()

    def selected(self, event=None):
        selected = self.list.selection()
        valid = bool(selected)
        key = selected[0] if valid else None
        local = key in self.local
        for button in (self.play, self.install, self.repair, self.remove, self.import_button):
            button.configure(state="disabled")
        if valid:
            item = self.items[key]
            self.selection_title.set(f"Q3Rally v{item[0]['version']}")
            size = item[2].get("size", 0)
            self.selection_meta.set(("Installed · Ready to play" if local else "Available to install") +
                                    (f" · {size / 1048576:.0f} MB" if size else ""))
            self.action_title.set(f"Q3Rally v{item[0]['version']}")
            self.notes.configure(state="normal"); self.notes.delete("1.0", "end")
            self.notes.insert("1.0", item[0].get("notes", "No release notes available."))
            self.notes.configure(state="disabled")
            self.play.configure(text="PLAY  →" if local else "INSTALL & PLAY  →")
            self.install.configure(label="Reinstall" if local else "Install only")
            self.preferred = key
            atomic_json(self.preferences, {"selected": key})
            if not self.busy:
                self.play.configure(state="normal" if local or self.fresh else "disabled")
                self.install.configure(state="normal" if self.fresh else "disabled")
                self.repair.configure(state="normal" if local else "disabled")
                self.remove.configure(state="normal" if local else "disabled")
                self.import_button.configure(state="normal")
        else:
            self.selection_title.set("Ready for the next round?")
            self.selection_meta.set("Select a game version on the left.")
            self.action_title.set("Choose your game version")
        self.manage_button.configure(state="normal" if valid and not self.busy else "disabled")
        self.refresh_button.configure(state="disabled" if self.busy else "normal")
        if self.busy and self.cancellable:
            self.pause.grid(); self.pause.configure(state="normal")
        else:
            self.pause.grid_remove()
        launcher = Manager(self.manager.root, self.manager.keys, development=self.manager.development, product="Q3RallyLauncher")
        updates = launcher.available(self.catalog or {})
        has_update = self.fresh and updates and version_key(updates[0][0]["version"]) > version_key(__version__)
        if has_update:
            self.update_button.grid(); self.update_button.configure(state="disabled" if self.busy else "normal")
        else:
            self.update_button.grid_remove()

    def work(self, label, operation, *, cancellable=False):
        if self.busy:
            return
        self.busy = True; self.cancellable = cancellable
        self.cancel.clear(); self.progress["value"] = 0; self.status.set(label); self.selected()
        self.progress.configure(mode="determinate" if cancellable else "indeterminate")
        self.progress.grid()
        if not cancellable:
            self.progress.start(15)
        def run():
            try:
                result = operation()
                self.events.put(("done", result))
            except Exception as error:
                logging.exception("Launcher operation failed")
                self.events.put(("error", str(error)))
        # Keep filesystem transactions and game locks alive if the UI closes.
        threading.Thread(target=run, daemon=False).start()

    def poll(self):
        try:
            while True:
                kind, result = self.events.get_nowait()
                if kind == "progress":
                    done, total = result
                    self.progress.stop(); self.progress.configure(mode="determinate")
                    self.progress["value"] = 100 * done / total
                    self.status.set(f"Download: {done / 1048576:.1f} / {total / 1048576:.1f} MiB")
                elif kind == "status":
                    self.status.set(result)
                    self.cancellable = False
                    self.pause.grid_remove()
                    self.progress.stop(); self.progress.grid_remove()
                else:
                    self.busy = False
                    self.cancellable = False
                    self.progress.stop(); self.progress.grid_remove()
                    if kind == "error":
                        self.status.set(result)
                    elif isinstance(result, tuple) and result[0] == "catalog":
                        self.envelope, self.catalog = result[1:]; self.fresh = True
                        self.connection.set("ONLINE / CATALOG UP TO DATE")
                        self.status.set("Versions are up to date. Choose a version and start your race.")
                    elif result == "restart":
                        self.root.destroy(); return
                    else:
                        self.status.set(result or "Done.")
                    self.fill()
        except queue.Empty:
            pass
        self.root.after(80, self.poll)

    def refresh(self):
        self.work("Checking available versions …", lambda: ("catalog", *self.manager.load_catalog(self.config["catalog_url"])))

    def install_or_play(self, play):
        release, target, _ = self.items[self.list.selection()[0]]
        version = release["version"]
        key = f"{version}|{target}"
        if not play and key in self.local and not self.messages.askyesno("Reinstall", "Reinstall this game version? Your profiles and demos will be kept."):
            return
        def operation():
            if not play or key not in self.local:
                self.manager.install(self.envelope, version, target, cancel=self.cancel,
                    progress=lambda done, total: self.events.put(("progress", (done, total))))
            if play:
                self.events.put(("status", "Game running. Have fun!"))
                code = self.manager.run(version, target)
                return "Game closed." if code == 0 else f"Game exited (code {code}). See the log for details."
            return "Installation complete."
        self.work("Preparing game …", operation, cancellable=not play or key not in self.local)

    def verify(self):
        release, target, _ = self.items[self.list.selection()[0]]
        def operation():
            self.manager.verify(release["version"], target)
            return "File verification complete."
        self.work("Verifying installed files …", operation)

    def uninstall(self):
        release, target, _ = self.items[self.list.selection()[0]]
        if self.messages.askyesno("Remove version", "Remove this game installation? Your profiles and demos will be kept."):
            def operation():
                self.manager.uninstall(release["version"], target)
                return "Game installation removed."
            self.work("Removing game installation …", operation)

    def import_profile(self):
        from .profiles import import_profile
        release, _, _ = self.items[self.list.selection()[0]]
        name = self.filedialog.askopenfilename(title="Import an existing profile", filetypes=[("Q3Rally profile", "*.json")])
        if not name:
            return
        def operation():
            imported = import_profile(Path(name), self.manager.root, release["version"])
            return f"Profile {imported.stem} imported. Select it under Profiles in the game."
        self.work("Importing profile …", operation)

    def update_launcher(self):
        manager = Manager(self.manager.root, self.manager.keys, development=self.manager.development, product="Q3RallyLauncher")
        release, target, _ = manager.available(self.catalog)[0]
        def operation():
            manager.install(self.envelope, release["version"], target, cancel=self.cancel,
                progress=lambda done, total: self.events.put(("progress", (done, total))))
            executable = manager.verify(release["version"], target)
            atomic_json(manager.root / "launcher-selection.json", {"version": release["version"], "target": target})
            start_launcher(executable, manager.root, self.config_path)
            return "restart"
        self.work("Updating launcher …", operation, cancellable=True)

    def close(self):
        self.cancel.set()
        self.root.destroy()


def main(argv=None):
    parser = argparse.ArgumentParser(description="Q3Rally Launcher")
    parser.add_argument("--data-dir", type=Path, default=default_root())
    parser.add_argument("--config", type=Path)
    parser.add_argument("--development", action="store_true", help="Allow loopback HTTP fixtures; signatures remain mandatory")
    parser.add_argument("--action", choices=("list", "install", "play", "verify", "remove"))
    parser.add_argument("--version")
    args = parser.parse_args(argv)
    args.data_dir.mkdir(parents=True, exist_ok=True)
    (args.data_dir / "logs").mkdir(exist_ok=True)
    logging.basicConfig(filename=args.data_dir / "logs/launcher.log", level=logging.INFO,
                        format="%(asctime)s %(levelname)s %(message)s")
    config_path = args.config or Path(__file__).with_name("config.json")
    try:
        config = json.loads(config_path.read_text(encoding="utf-8"))
        manager = Manager(args.data_dir, config["public_keys"], development=args.development)
        if not args.action:
            # Desktop shortcuts may keep pointing to the original web download;
            # a verified newer managed launcher takes over on subsequent starts.
            selection = manager.root / "launcher-selection.json"
            if selection.exists():
                try:
                    chosen = json.loads(selection.read_text())
                    if version_key(chosen["version"]) > version_key(__version__):
                        updater = Manager(manager.root, manager.keys, product="Q3RallyLauncher")
                        executable = updater.verify(chosen["version"], chosen["target"])
                        start_launcher(executable, manager.root, args.config)
                        return 0
                except Exception:
                    logging.exception("Managed launcher unavailable; falling back to original launcher")
            Window(manager, config, args.config).root.mainloop()
            return 0
        envelope, catalog = manager.cached_catalog()
        if args.action in ("list", "install"):
            try:
                envelope, catalog = manager.load_catalog(config["catalog_url"])
            except Exception:
                if args.action == "install":
                    raise
                logging.exception("Catalog unavailable; using installed releases")
        available = manager.available(catalog or {})
        local = manager.installed()
        if args.action == "list":
            print(json.dumps({"available": [{"version": r["version"], "target": t} for r, t, _ in available],
                              "installed": [{"version": r["version"], "target": t} for r, t, _ in local]}, indent=2))
            return 0
        candidates = available if args.action == "install" else local
        item = next((item for item in candidates if item[0]["version"] == args.version), None)
        if item is None:
            raise LauncherError("Version unavailable or not installed; specify --version.")
        version, target = item[0]["version"], item[1]
        if args.action == "install": manager.install(envelope, version, target)
        elif args.action == "verify": manager.verify(version, target)
        elif args.action == "remove": manager.uninstall(version, target)
        elif args.action == "play": return manager.run(version, target)
        return 0
    except Exception as error:
        logging.exception("Launcher failed")
        if args.action:
            print(str(error), file=sys.stderr)
        else:
            import tkinter.messagebox
            tkinter.messagebox.showerror("Q3Rally Launcher", str(error))
        return 1
