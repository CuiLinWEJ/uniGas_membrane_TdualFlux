#!/usr/bin/env python3
from pathlib import Path
import sys

root = Path(sys.argv[1]).expanduser() if len(sys.argv) > 1 else Path('~/OpenFOAM/uniGasFoam').expanduser()
parcel_dir = root / 'src/lagrangian/uniGas/parcels'
h_file = parcel_dir / 'uniGasParcel.H'
c_file = parcel_dir / 'uniGasParcel.C'

if not h_file.is_file() or not c_file.is_file():
    raise SystemExit(f'uniGasParcel files not found under: {parcel_dir}')

h_text = h_file.read_text()
c_text = c_file.read_text()

h_decl = '''        //- Map a locally reflected parcel back across the cyclic interface.\n        //  This exposes particle::hitCyclicPatch() to cyclic boundary models\n        //  without duplicating OpenFOAM topology/transform logic.\n        void returnAcrossCyclic\n        (\n            uniGasCloud& cloud,\n            trackingData& td\n        );\n'''

h_anchor = '''        void hitWallPatch\n        (\n            uniGasCloud& cloud,\n            trackingData& td\n        );\n\n\n        //- Transform the physical properties of the particle\n'''

if 'void returnAcrossCyclic' in h_text:
    print(f'[skip] declaration already present: {h_file}')
else:
    if h_text.count(h_anchor) != 1:
        raise SystemExit('Could not find a unique uniGasParcel.H insertion anchor; no H changes made.')
    h_replacement = '''        void hitWallPatch\n        (\n            uniGasCloud& cloud,\n            trackingData& td\n        );\n\n\n''' + h_decl + '''\n        //- Transform the physical properties of the particle\n'''
    h_file.write_text(h_text.replace(h_anchor, h_replacement, 1))
    print(f'[ok] added declaration: {h_file}')

c_impl = '''void Foam::uniGasParcel::returnAcrossCyclic\n(\n    uniGasCloud& cloud,\n    trackingData& td\n)\n{\n    particle::hitCyclicPatch(cloud, td);\n}\n'''

c_anchor = '''void Foam::uniGasParcel::transformProperties\n(\n    const tensor& T\n)\n'''

if 'void Foam::uniGasParcel::returnAcrossCyclic' in c_text:
    print(f'[skip] implementation already present: {c_file}')
else:
    if c_text.count(c_anchor) != 1:
        raise SystemExit('Could not find a unique uniGasParcel.C insertion anchor; no C changes made.')
    c_replacement = c_impl + '\n\n' + c_anchor
    c_file.write_text(c_text.replace(c_anchor, c_replacement, 1))
    print(f'[ok] added implementation: {c_file}')

print('\nVerification:')
for p in (h_file, c_file):
    for n, line in enumerate(p.read_text().splitlines(), 1):
        if 'returnAcrossCyclic' in line:
            print(f'{p}:{n}:{line.strip()}')
