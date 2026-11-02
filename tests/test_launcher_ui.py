"""Optional native Tk layout checks; enable Q3RALLY_TEST_GUI=1 on a desktop."""
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from launcher.app import Window
from launcher.core import Manager
from launcher.core import native_targets


@unittest.skipUnless(os.environ.get("Q3RALLY_TEST_GUI") == "1", "native desktop check")
class LauncherLayoutTests(unittest.TestCase):
    def test_selection_and_controls_fit_minimum_window(self):
        with tempfile.TemporaryDirectory() as temporary:
            manager = Manager(Path(temporary), {}, targets=["windows-x86_64"])
            with patch.object(Window, "refresh"):
                window = Window(manager, {"catalog_url": "https://example.invalid/catalog.json"})
                try:
                    window.catalog = {"releases": [
                        {"version": "0.8", "notes": "Latest version", "packages": {"windows-x86_64": {}}},
                        {"version": "0.7d", "notes": "Earlier version", "packages": {"windows-x86_64": {}}}]}
                    window.fresh = True
                    window.fill()
                    minimum = window.root.minsize()
                    window.root.geometry(f"{minimum[0]}x{minimum[1]}")
                    window.root.update()
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
                    for control in (window.play, window.manage_button, window.refresh_button, window.logs_button):
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
                    self.assertLessEqual(window.pause.winfo_rooty() + window.pause.winfo_height(),
                                         window.root.winfo_rooty() + window.root.winfo_height())
                    window.busy = False; window.cancellable = False
                    window.catalog['launchers'] = [{'version': '0.3.0', 'packages': {native_targets()[0]: {}}}]
                    window.selected(); window.root.update()
                    self.assertFalse(window.pause.winfo_ismapped())
                    self.assertTrue(window.update_button.winfo_ismapped())
                    self.assertLessEqual(window.update_button.winfo_rootx() + window.update_button.winfo_width(),
                                         window.root.winfo_rootx() + window.root.winfo_width())
                finally:
                    window.root.destroy()


if __name__ == "__main__":
    unittest.main()
