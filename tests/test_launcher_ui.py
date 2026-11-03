"""Optional native Tk layout checks; enable Q3RALLY_TEST_GUI=1 on a desktop."""
import os
import subprocess
import sys
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from launcher.app import Window
from launcher.core import Manager
from launcher.core import native_targets


@unittest.skipUnless(os.environ.get("Q3RALLY_TEST_GUI") == "1", "native desktop check")
class LauncherLayoutTests(unittest.TestCase):
    @unittest.skipUnless(os.name == 'nt', 'Windows maximize state')
    def test_starts_maximized(self):
        # Use a fresh Tcl interpreter/process, as a real launcher start does.
        code = '''
import tempfile
from pathlib import Path
from unittest.mock import patch
from launcher.app import Window
from launcher.core import Manager
with tempfile.TemporaryDirectory() as temporary:
    manager = Manager(Path(temporary), {}, targets=['windows-x86_64'])
    with patch.object(Window, 'refresh'):
        window = Window(manager, {'catalog_url':'https://example.invalid/catalog.json'})
        try:
            window.root.update()
            assert window.root.state() == 'zoomed', window.root.state()
        finally:
            window.root.destroy()
'''
        result = subprocess.run([sys.executable, '-c', code], cwd=Path(__file__).resolve().parents[1],
                                capture_output=True, text=True, timeout=20,
                                creationflags=subprocess.CREATE_NO_WINDOW)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_selection_and_controls_fit_minimum_window(self):
        with tempfile.TemporaryDirectory() as temporary:
            manager = Manager(Path(temporary), {}, targets=["windows-x86_64"])
            # Check minimum-size layout independently of the maximize/restore
            # transitions on the Windows CI virtual desktop.
            with patch.object(Window, "refresh"), patch('launcher.app.maximize_window'):
                window = Window(manager, {"catalog_url": "https://example.invalid/catalog.json"})
                try:
                    window.root.update()
                    window.catalog = {"releases": [
                        {"version": "0.8", "notes": "Latest version", "packages": {"windows-x86_64": {}}},
                        {"version": "0.7d", "notes": "Earlier version", "packages": {"windows-x86_64": {}}}]}
                    window.fresh = True
                    window.fill()
                    minimum = window.root.minsize()
                    window.root.geometry(f"{minimum[0]}x{minimum[1]}")
                    window.root.update()
                    layout = [(str(w), w.winfo_geometry(), w.winfo_reqheight(), w.winfo_ismapped())
                              for w in window.root.winfo_children()]
                    self.assertGreaterEqual(window.list.winfo_width(), 350,
                                            (window.root.winfo_geometry(), layout))
                    self.assertEqual(window.directory_field.cget('state'), 'readonly')
                    self.assertEqual(window.directory_field.get(), str(manager.versions))
                    window.install_directory.set('C:/Games/' + 'a very long folder name/' * 20)
                    window.root.update()
                    self.assertTrue(window.list.winfo_ismapped())
                    self.assertGreaterEqual(window.list.winfo_height(), 45)
                    window.install_directory.set(str(manager.versions))
                    self.assertEqual(window.list.selection(), ("0.8|windows-x86_64",))
                    window.list.selection_set("0.7d|windows-x86_64")
                    window.root.update()
                    self.assertEqual(window.preferred, "0.7d|windows-x86_64")
                    self.assertIn("Earlier version", window.notes.get("1.0", "end"))
                    self.assertEqual(window.install['label'], "Install only")
                    self.assertEqual(window.repair['state'], "disabled")
                    self.assertEqual(window.import_button['state'], "normal")
                    self.assertEqual(window.remove['state'], "disabled")
                    self.assertFalse(window.pause.winfo_ismapped())
                    self.assertFalse(window.update_button.winfo_ismapped())
                    for control in (window.play, window.manage_button, window.refresh_button, window.logs_button, window.folder_button):
                        self.assertTrue(control.winfo_ismapped())
                        self.assertGreaterEqual(control.winfo_rootx(), window.root.winfo_rootx())
                        self.assertLessEqual(control.winfo_rootx() + control.winfo_width(),
                                             window.root.winfo_rootx() + window.root.winfo_width())
                    item = window.items['0.7d|windows-x86_64']
                    with patch.object(manager, 'installed', return_value=[item]):
                        window.fill(); window.root.update()
                        self.assertEqual(window.install['label'], "Reinstall")
                        self.assertEqual(window.repair['state'], "normal")
                        self.assertEqual(window.remove['state'], "normal")
                        self.assertEqual(window.play['text'], "PLAY  →")
                    window.busy = True; window.cancellable = True
                    window.selected(); window.root.update()
                    self.assertTrue(window.pause.winfo_ismapped())
                    self.assertTrue(window.manage_button.instate(['disabled']))
                    self.assertTrue(window.folder_button.instate(['disabled']))
                    self.assertLessEqual(window.pause.winfo_rooty() + window.pause.winfo_height(),
                                         window.root.winfo_rooty() + window.root.winfo_height())
                    window.busy = False; window.cancellable = False
                    window.catalog['launchers'] = [{'version': '0.5.0', 'packages': {native_targets()[0]: {}}}]
                    window.selected(); window.root.update()
                    self.assertFalse(window.pause.winfo_ismapped())
                    self.assertTrue(window.update_button.winfo_ismapped())
                    self.assertLessEqual(window.update_button.winfo_rootx() + window.update_button.winfo_width(),
                                         window.root.winfo_rootx() + window.root.winfo_width())
                    window.busy = False
                    with tempfile.TemporaryDirectory() as games:
                        with patch.object(window.filedialog, 'askdirectory', return_value=games):
                            window.choose_install_directory()
                        self.assertEqual(manager.versions, Path(games).resolve())
                        self.assertEqual(window.install_directory.get(), str(manager.versions))
                finally:
                    window.root.destroy()


if __name__ == "__main__":
    unittest.main()
