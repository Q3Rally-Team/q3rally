"""Prepare isolated PK3-only data for native dedicated-server runtime checks."""
import argparse
import json
from pathlib import Path
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tools.release_assets import inventory


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--assets', type=Path, default=ROOT / 'baseq3r')
    parser.add_argument('--vm-dir', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    policy = json.loads((ROOT / 'tools/release_policy.json').read_text())
    assets, _ = inventory(args.assets, policy)
    vm_names = ('cgame.qvm', 'qagame.qvm', 'ui.qvm')
    for name in vm_names:
        if not (args.vm_dir / name).is_file():
            parser.error(f'Missing {args.vm_dir / name}')
    data = args.output / 'baseq3r'
    data.mkdir(parents=True, exist_ok=True)
    # Exclusive creation prevents replacing a working installation.
    with zipfile.ZipFile(data / 'pak0.pk3', 'x', allowZip64=False) as archive:
        for asset in sorted(assets.values(), key=lambda a: a.name):
            archive.writestr(asset.name, asset.read())
    with zipfile.ZipFile(data / 'qvms.pk3', 'x', zipfile.ZIP_DEFLATED, allowZip64=False) as archive:
        for name in vm_names:
            archive.write(args.vm_dir / name, 'vm/' + name)
    print(json.dumps({'assets': len(assets), 'basepath': str(args.output.resolve())}))


if __name__ == '__main__':
    main()
