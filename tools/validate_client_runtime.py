"""Bounded native desktop-client checks using an isolated home and staged data.

Requires a real display/OpenGL context and audio device. This verifies device
initialization, not perceived sound quality or interactive input usability.
"""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import time

UUID = '12345678-1234-4234-8234-123456789abc'


def preset_values(name, renderer):
    """Use the real UI preset table instead of maintaining a second definition."""
    source = (Path(__file__).resolve().parents[1] /
              'engine/code/q3_ui/ui_advanced_graphics.c').read_text()
    table = source.split('gfxPresetTable[] = {', 1)[1].split('\n};', 1)[0]
    rows = re.findall(r'\{\s*"([^"]+)"\s*,\s*\{\s*"([^"]*)"\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"\s*\}\s*\}', table)
    if not rows:
        raise ValueError('Graphics preset table could not be read')
    index = ('low', 'medium', 'high', 'ultra').index(name)
    values = {row[0]: row[index + 1] for row in rows}
    msaa = re.search(r'gfxPresetMsaa\[[^]]+\]\s*=\s*\{([^}]+)\}', source)
    samples = int(msaa[1].split(',')[index])
    values['r_ext_multisample'] = str(samples if renderer == 'opengl1' else 0)
    if renderer == 'opengl2':
        values['r_ext_framebuffer_multisample'] = str(samples)
    return values


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path, required=True)
    parser.add_argument('--basepath', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--renderer', action='append', choices=('opengl1', 'opengl2'))
    parser.add_argument('--timeout', type=int, default=180)
    parser.add_argument('--check-player-sounds', action='store_true')
    parser.add_argument('--preset', choices=('low', 'medium', 'high', 'ultra'))
    parser.add_argument('--legacy-profile', action='store_true', help='Verify identity-only migration and the one-time v0.8 progress reset')
    args = parser.parse_args()
    executable = args.executable.resolve(strict=True)
    basepath = args.basepath.resolve(strict=True)
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    home = output / 'home'
    game = home / 'baseq3r'
    profiles = game / 'profiles'
    profiles.mkdir(parents=True)
    profile = profiles / 'InstallProbe.json'
    seed = {'uuid': UUID, 'name': 'InstallProbe', 'info': {'country': 'DE'},
            'stats': {'kills': 3, 'deaths': 1, 'wins': 2, 'playerScore': 321, 'distanceKm': 2.5}}
    if not args.legacy_profile:
        seed = {'profileVersion': 2,
                'identity': {'uuid': UUID, 'name': 'InstallProbe', 'info': {'country': 'DE'}},
                'webLink': {'state': 'unlinked', 'accountId': None, 'profileId': None},
                'progress': {'epoch': 'v0.8-local-1', 'authority': 'local',
                             'currentRank': 0, 'highestRank': 0, 'stats': seed['stats']}}
    profile.write_text(json.dumps(seed))
    results = []
    for index, renderer in enumerate(args.renderer or ('opengl1', 'opengl2')):
        script = ['echo CLIENT_PROBE_BEGIN', 'wait 360', 'echo PROBE_SETTINGS_BEGIN',
                  'sensitivity', 'cg_drawFPS', 'bind F8', 'profile_active', 'cl_uuid',
                  'echo PROBE_SETTINGS_END', f'screenshotJPEG menu-{renderer}', 'wait 2']
        if index == 0:
            script += ['seta sensitivity 7.25', 'seta cg_drawFPS 1',
                       'bind F8 screenshotJPEG', 'writeconfig persisted.cfg']
        # The world is loaded synchronously before these frames. A short warmup
        # covers first snapshots without turning software rendering into a benchmark.
        script += ['map q3r_country01', 'wait 30', f'screenshotJPEG track-{renderer}',
                   'play sound/misc/menu1.ogg', 'wait 5', 'vid_restart', 'wait 30',
                   f'screenshotJPEG restarted-{renderer}', 'wait 2', 'sensitivity',
                   'cg_drawFPS', 'bind F8', 'profile_active', 'cl_uuid',
                   'echo CLIENT_PROBE_END', 'quit']
        if args.check_player_sounds:
            script[-2:-2] = ['s_list', 'testmodel models/flags/y_flag.md3', 'wait 5', f'screenshotJPEG yellow-flag-{renderer}', 'wait 2']
        (game / 'validation.cfg').write_text('\n'.join(script) + '\n')
        command = [str(executable), '+set', 'fs_basepath', str(basepath),
                   '+set', 'fs_homepath', str(home), '+set', 'r_fullscreen', '0',
                   '+set', 'r_mode', '-1', '+set', 'r_customwidth', '800',
                   '+set', 'r_customheight', '600', '+set', 'cl_renderer', renderer,
                   '+set', 'in_mouse', '0', '+set', 'in_nograb', '1',
                   '+set', 's_initsound', '1', '+set', 's_useOpenAL', '0',
                   '+set', 's_sdlCapture', '0', '+set', 'com_maxfps', '30',
                   '+set', 'com_maxfpsUnfocused', '30', '+set', 'com_maxfpsMinimized', '30',
                   '+set', 'cl_motd', '0', '+set', 'net_ip', '127.0.0.1',
                   '+set', 'net_enabled', '1', '+set', 'sv_ladderEnabled', '0',
                   '+set', 'profile_active', 'InstallProbe', '+set', 'logfile', '2',
                   '+exec', 'validation.cfg']
        values = {}
        if args.preset:
            values = preset_values(args.preset, renderer)
            # The engine limits the number of +commands at startup. Use its
            # normal config phase so all preset values precede renderer init.
            (game / 'autoexec.cfg').write_text(''.join(
                f'seta {key} "{value}"\n' for key, value in values.items()))
            (output / f'{renderer}.preset.json').write_text(json.dumps(values, indent=2) + '\n')
        kwargs = {}
        if os.name == 'nt':
            startup = subprocess.STARTUPINFO()
            startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            startup.wShowWindow = 0
            kwargs['startupinfo'] = startup
        print(f'START {renderer}', flush=True)
        with (output / f'{renderer}.stdout.log').open('w', encoding='utf-8') as log:
            proc = subprocess.Popen(command, cwd=basepath, stdout=log,
                                    stderr=subprocess.STDOUT, **kwargs)
            deadline = time.monotonic() + args.timeout
            code = None
            while proc.poll() is None and time.monotonic() < deadline:
                live = (output / f'{renderer}.stdout.log').read_text(encoding='utf-8', errors='replace')
                if "ERROR: Couldn't compile shader" in live or 'recursive error after:' in live:
                    code = 'fatal_shader_error'
                    break
                time.sleep(.25)
            if proc.poll() is None:
                code = code or 'timeout'
                proc.terminate()
                try:
                    proc.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    proc.wait(timeout=10)
            else:
                code = proc.returncode
        text = (output / f'{renderer}.stdout.log').read_text(encoding='utf-8', errors='replace')
        console = game / 'qconsole.log'
        if console.is_file():
            text = console.read_text(encoding='utf-8', errors='replace')
            (output / f'{renderer}.qconsole.log').write_text(text, encoding='utf-8')
        settings = text.split('PROBE_SETTINGS_BEGIN\n', 1)[-1].split('PROBE_SETTINGS_END', 1)[0]
        config = (game / 'q3config.cfg').read_text(errors='replace') if (game / 'q3config.cfg').is_file() else ''
        (output / f'{renderer}.q3config.cfg').write_text(config)
        preset_mismatches = {}
        for key, expected in values.items():
            matches = re.findall(r'^seta\s+' + re.escape(key) + r'\s+"([^"]*)"', config, re.M | re.I)
            actual = matches[-1] if matches else None
            if actual != expected:
                preset_mismatches[key] = {'expected': expected, 'actual': actual}
        images = {stage: (game / f'screenshots/{stage}-{renderer}.jpg').is_file()
                  for stage in ('menu', 'track', 'restarted')}
        stored_profile = json.loads(profile.read_text())
        identity = stored_profile.get('identity', {})
        progress = stored_profile.get('progress', {})
        checks = {'normal_exit': code == 0, 'end_marker': 'CLIENT_PROBE_END' in text,
                  'preset_applied': not preset_mismatches,
                  'audio_initialized': 'SDL audio initialized.' in text,
                  'renderer_initialized': bool(re.search(r'GL_RENDERER:', text)),
                  'requested_renderer': f'Trying to load "renderer_{renderer}_' in text and
                      f'Trying to load "renderer_{"opengl2" if renderer == "opengl1" else "opengl1"}_' not in text,
                  'renderer_restarted': text.count(f'Trying to load "renderer_{renderer}_') >= 2,
                  'screenshots': all(images.values()),
                  'profile_preserved': identity.get('uuid') == UUID and progress.get('stats', {}).get('kills') == (0 if args.legacy_profile else 3),
                  'profile_loaded': UUID in text,
                  'profile_format': stored_profile.get('profileVersion') == 2,
                  'profile_local_unlinked': stored_profile.get('webLink') == {'state': 'unlinked', 'accountId': None, 'profileId': None} and progress.get('authority') == 'local',
                  'profile_epoch': progress.get('epoch') == 'v0.8-local-1',
                  'profile_metadata_preserved': identity.get('info', {}).get('country') == 'DE',

                  'settings_saved': '7.25' in config and 'screenshotJPEG' in config,
                  'settings_reloaded': index == 0 or ('7.25' in settings and 'screenshotJPEG' in settings)}
        if args.check_player_sounds:
            checks['no_missing_player_sounds'] = not re.search(r'(?:Failed to load sound|could not find).*sound/player/', text, re.I)
            checks['horn_registered'] = bool(re.search(r'sound/player/[^\s]+/horn\.(?:wav|mp3|ogg|opus)', text))
            checks['yellow_flag_model'] = (game / f'screenshots/yellow-flag-{renderer}.jpg').is_file() and "Can't register model" not in text
        result = {'renderer': renderer, 'preset': args.preset or 'default', 'preset_mismatches': preset_mismatches,
                  'exit': code, 'checks': checks, 'images': images,
                  'gl_renderer': re.findall(r'GL_RENDERER:([^\r\n]*)', text),
                  'audio_driver': re.findall(r'SDL audio driver is ([^\r\n]*)', text),
                  'passed': all(checks.values())}
        results.append(result)
        (output / 'summary.json').write_text(json.dumps(results, indent=2) + '\n')
        print(json.dumps(result), flush=True)
    return 0 if all(result['passed'] for result in results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
