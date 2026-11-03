"""Verify and exercise a native game release through the launcher's installer."""
import argparse
import base64
from datetime import datetime, timedelta, timezone
import json
from pathlib import Path
import shutil
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from nacl.signing import SigningKey
from launcher.core import Manager, atomic_json
from tools.publish_launcher_catalog import Index, package_record, sign


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--release-index', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--base-index', type=Path)
    parser.add_argument('--update-index', type=Path)
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    index = Index(args.release_index.resolve())
    artifact, = index.data['artifacts']
    target, version = artifact['target'], index.data['version']
    package = package_record(index, artifact, 'https://example.invalid', 'Q3Rally')
    key = SigningKey.generate()
    keys = {'package-test': base64.b64encode(bytes(key.verify_key)).decode()}
    envelope = sign({'schema': 1, 'product': 'Q3Rally', 'sequence': 1,
        'expires': (datetime.now(timezone.utc) + timedelta(days=1)).isoformat(),
        'releases': [{'version': version, 'packages': {target: package}}], 'launchers': []}, key, 'package-test')
    manager = Manager(output / 'data', keys, targets=[target])
    shutil.copyfile(index.parent / artifact['filename'], manager.cache / (package['sha256'] + '.part'))
    manager.install(envelope, version, target)
    client = manager.verify(version, target)
    raw = client.read_bytes()
    if target.startswith('windows-'):
        offset, = struct.unpack_from('<I', raw, 0x3c)
        magic, machine = struct.unpack_from('<4sH', raw, offset)
        assert magic == b'PE\0\0' and machine == (0x14c if target == 'windows-x86' else 0x8664)
    elif target.startswith('linux-'):
        assert raw[:4] == b'\x7fELF' and raw[4] == (1 if target == 'linux-x86' else 2)
    else:
        # The app assembler uses lipo, which may wrap one architecture in a
        # universal Mach-O container. Check its slices rather than a thin header.
        architectures = subprocess.check_output(['lipo', '-archs', str(client)], text=True).split()
        assert architectures == [target.split('-', 1)[1]], architectures
    result = subprocess.run([str(client), '--version'], capture_output=True, timeout=30)
    assert result.returncode == 0 and ('Q3Rally v' + version + ' ').encode() in result.stdout, (result.stdout, result.stderr)
    arch = target.split('-', 1)[1]
    server = client.parent / ('q3rally-server' if target.startswith('macos-') else 'q3rally-server.' + arch + ('.exe' if target.startswith('windows-') else ''))
    assert server.is_file(), server
    cold_starts = 0
    if target.startswith('windows-'):
        # No interactive console is available to launchers/services/CI. Repeated
        # redirected starts catch uninitialized Win32 console screen metadata.
        for attempt in range(25):
            cold = subprocess.run([str(server), '+set', 'fs_basepath', str(client.parent),
                '+set', 'fs_homepath', str(output/'cold-home'), '+set', 'net_enabled', '0', '+quit'],
                cwd=client.parent, stdin=subprocess.DEVNULL, capture_output=True,
                creationflags=subprocess.CREATE_NO_WINDOW, timeout=30)
            assert cold.returncode == 0, (attempt, cold.returncode, cold.stdout, cold.stderr)
            cold_starts += 1
    # The garage enumerates model directories, even when the asset PK3 has
    # only file entries. Check the packaged filesystem on the target engine.
    manifest = json.loads((manager.path(version, target) / 'release-manifest.json').read_text())
    vehicle_dirs = {item['path'].split('/')[2] for item in manifest['assets']
                    if item['path'].startswith('models/players/') and item['path'].endswith('/body.md3')}
    vehicle_home = output / 'vehicle-list-home'
    listing = subprocess.run([str(server), '+set', 'fs_basepath', str(client.parent),
        '+set', 'fs_homepath', str(vehicle_home), '+set', 'net_enabled', '0', '+set', 'logfile', '2',
        '+dir', 'models/players', '/', '+quit'], cwd=client.parent,
        stdin=subprocess.DEVNULL, capture_output=True, timeout=30,
        **({'creationflags': subprocess.CREATE_NO_WINDOW} if target.startswith('windows-') else {}))
    directories = set((vehicle_home / 'baseq3r/qconsole.log').read_text(errors='replace').splitlines())
    missing = {name for name in vehicle_dirs if name not in directories and name + '/' not in directories}
    assert listing.returncode == 0 and vehicle_dirs and not missing, (missing, listing.stdout, listing.stderr)
    subprocess.run([sys.executable, str(ROOT/'tools/validate_dedicated_server.py'),
        '--executable', str(server), '--basepath', str(client.parent), '--output', str(output/'dedicated'),
        '--case', 'racing', '--case', 'sprint', '--http-probe'], check=True, timeout=240)
    subprocess.run([sys.executable, str(ROOT/'tools/validate_offline_registration.py'),
        '--executable', str(server), '--basepath', str(client.parent),
        '--output', str(output/'offline-registration')], check=True, timeout=120)
    incremental=False
    if args.base_index or args.update_index:
        if not args.base_index or not args.update_index:
            raise ValueError('Both base and update indexes are required')
        from unittest.mock import patch
        old_index=Index(args.base_index.resolve())
        old_artifact,=[a for a in old_index.data['artifacts'] if a['target']==target]
        old_version=old_index.data['version']
        old_package=package_record(old_index,old_artifact,'https://example.invalid','Q3Rally')
        update_data=json.loads(args.update_index.read_text())
        update,=[u for u in update_data['updates'] if u['target']==target and u['version']==version]
        new_package=dict(package)
        new_package['updates']=[{**{k:update[k] for k in ('from_version','from_manifest_sha256','sha256','size')},
                                'url':'https://example.invalid/'+update['filename']}]
        delta_envelope=sign({'schema':1,'product':'Q3Rally','sequence':2,
            'expires':(datetime.now(timezone.utc)+timedelta(days=1)).isoformat(),
            'releases':[{'version':version,'packages':{target:new_package}},
                        {'version':old_version,'packages':{target:old_package}}],'launchers':[]},key,'package-test')
        incremental_manager=Manager(output/'incremental-data',keys,targets=[target])
        shutil.copyfile(old_index.parent/old_artifact['filename'],incremental_manager.cache/(old_package['sha256']+'.part'))
        incremental_manager.install(delta_envelope,old_version,target)
        shutil.copyfile(args.update_index.parent/update['filename'],incremental_manager.cache/(update['sha256']+'.part'))
        original_download=incremental_manager._download
        def delta_only(record,*positional,**named):
            if record['sha256']==package['sha256']:
                raise AssertionError('Incremental validation attempted a full download')
            return original_download(record,*positional,**named)
        with patch.object(incremental_manager,'_download',side_effect=delta_only):
            incremental_client=incremental_manager.install(delta_envelope,version,target)
        assert incremental_manager.last_install_method=='incremental'
        verified=incremental_manager.verify(version,target)
        assert verified.is_file()
        incremental_manager.verify(old_version,target)
        incremental=True
    atomic_json(output/'result.json', {'version': version, 'target': target,
        'signed_install_verified': True, 'executable_architecture_verified': True,
        'native_client_version_verified': True, 'packaged_lap_and_sprint_bots_http_verified': True,
        'offline_registration_pending_active_verified': True,
        'vehicle_directory_listing_verified': True,
        'incremental_install_and_unchanged_base_verified': incremental,
        'redirected_windows_cold_starts': cold_starts})
    print(target + ': package installation, architecture, client version and dedicated checks PASS', flush=True)


if __name__ == '__main__':
    main()
