# Q3Rally Launcher

Current published launcher: **v0.2.1**, English UI, Python/Tk with a bundled
PyInstaller runtime. Users extract the complete ZIP and do not install Python.

Candidate v0.3.0 adds a persistent game-installation folder selector, a wider
release list and maximized startup. It does not change the public catalog.

## Distribution and supported targets

- Primary player download: https://www.q3rally.com/
- Launcher ZIPs: `https://www.q3rally.com/files/Q3RallyLauncher-v0.2.1-<target>.zip`
- Signed catalog: `https://www.q3rally.com/launcher/catalog.json`
- Test catalog: `https://www.q3rally.com/launcher/catalog-test.json`
- Game releases: https://github.com/Q3Rally-Team/q3rally/releases

Targets: `windows-x86`, `windows-x86_64`, `linux-x86`, `linux-x86_64`,
`macos-x86_64`, `macos-arm64`. Linux x86 launcher builds use Debian 12
(glibc 2.36); game packages require system SDL2/libcurl. Native CI tests cover
all six targets. Physical Mac graphics/audio/Gatekeeper checks remain open;
apps are not Developer-ID signed or notarized.

## Behavior and data

The launcher recommends the newest compatible stable game while allowing older
published versions side by side. Installation verifies signed catalog metadata,
SHA-256 and package manifests. Downloads can pause/resume; staged replacements
roll back on failure. An intact installed game can launch offline. File checks,
repair, running-game protection, diagnostic logs and launcher self-update are
included. Workflows create test artifacts and never publish releases automatically.

Writable data is separate from installed files:

- Windows: `%LOCALAPPDATA%/Q3RallyLauncher`
- macOS: `~/Library/Application Support/Q3RallyLauncher`
- Linux: `${XDG_DATA_HOME:-~/.local/share}/q3rally-launcher`

Each game version has `versions/<version>-<target>` and `user-data/<version>`.
Removing a game retains user data. Profile import copies identity/favorites,
not complete settings, progress or remote credentials. A full manual migration
must happen with the game closed and must not overwrite an existing destination.

The installation-folder selector below the version list chooses where new game
versions are installed. The choice and previously used folders are saved in
`game-storage.json` under the launcher data directory. Existing versions remain
visible in their original folders; verify, reinstall and remove continue to use
the correct folder. Profiles, download cache, logs and managed launcher updates
remain in the standard launcher data directory. A disconnected game drive does
not remove its saved folder preference. The selector is disabled during operations.

The launcher starts maximized and the release list has additional horizontal space.

Graphics selection belongs to the actual installed game executable. On Windows,
Settings > System > Display > Graphics > add the game EXE > Options > High
performance. The executable path changes with each separately installed version.

## Development and validation

Use Python 3.13 with Tk:

```text
python -m pip install -r launcher/requirements.txt
python run_launcher.py
python -m unittest discover -s tests -p test_launcher.py -v
python tools/build_launcher.py --output output/launcher-build
```

`--data-dir` isolates test installations. CLI actions include
`--action list|install|play|verify|remove --version 0.8.1`.
`--development` permits HTTP only on loopback; signatures stay mandatory.
`launcher.yml` builds all six native packages and validates the frozen runtime.
Use `launcher/requirements-publishing.txt` only for signing/publishing tools.

`launcher/config.json` contains the production endpoint and public Ed25519 key.
The private key stays outside Git under
`%LOCALAPPDATA%/Q3RallyRelease/Keys/release-1.pem`. Never include it in a package,
log, source commit or web upload. Catalogs require increasing sequence numbers
and an expiry; refresh them before expiry even if no new game is released.
Mac HTTPS uses the bundled CA file and configured HTTP/1.1 ALPN; retain these
settings and the launcher User-Agent when changing network code.

The catalog lives in `/var/www/q3rally-launcher`, outside the Astro build.
The game version feed is `https://ladder.q3rally.com/version.txt`.
See [release procedure](releasing.md). Historical build investigations and CI
evidence are retained in [the development archive](history/launcher-development.md).
