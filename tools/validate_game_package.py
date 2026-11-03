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
    subprocess.run([sys.executable, str(ROOT/'tools/validate_dedicated_server.py'),
        '--executable', str(server), '--basepath', str(client.parent), '--output', str(output/'dedicated'),
        '--case', 'racing', '--case', 'sprint', '--http-probe'], check=True, timeout=240)
    subprocess.run([sys.executable, str(ROOT/'tools/validate_offline_registration.py'),
        '--executable', str(server), '--basepath', str(client.parent),
        '--output', str(output/'offline-registration')], check=True, timeout=120)
    atomic_json(output/'result.json', {'version': version, 'target': target,
        'signed_install_verified': True, 'executable_architecture_verified': True,
        'native_client_version_verified': True, 'packaged_lap_and_sprint_bots_http_verified': True,
        'offline_registration_pending_active_verified': True,
        'redirected_windows_cold_starts': cold_starts})
    print(target + ': package installation, architecture, client version and dedicated checks PASS', flush=True)


if __name__ == '__main__':
    main()
