from pathlib import Path
from collections import defaultdict

root = Path("postProcessing/membraneForce")
files = sorted(root.glob("*/processor*/membraneForce.dat"))

if not files:
    raise SystemExit("No membraneForce.dat files found.")

data = defaultdict(lambda: {
    "nFiles": 0,
    "nRef": 0.0,
    "nRej": 0.0,
    "impulse": [0.0, 0.0, 0.0],
    "force": [0.0, 0.0, 0.0],
})

for f in files:
    patch_name = f.parts[-3]  # statGrid_master or statGrid_slave

    with f.open() as fp:
        for line in fp:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            parts = line.split()

            t = round(float(parts[0]), 12)
            nRef = float(parts[3])
            nRej = float(parts[4])

            impulse = [
                float(parts[5]),
                float(parts[6]),
                float(parts[7]),
            ]

            force = [
                float(parts[8]),
                float(parts[9]),
                float(parts[10]),
            ]

            data[t]["nFiles"] += 1
            data[t]["nRef"] += nRef
            data[t]["nRej"] += nRej

            for i in range(3):
                data[t]["impulse"][i] += impulse[i]
                data[t]["force"][i] += force[i]

out_file = root / "totalMembraneForce.dat"

with out_file.open("w") as out:
    out.write(
        "# time nFiles nReflections nRejections "
        "totalImpulseX totalImpulseY totalImpulseZ "
        "totalForceX totalForceY totalForceZ\n"
    )

    for t in sorted(data):
        d = data[t]
        out.write(
            f"{t:.12g} "
            f"{int(d['nFiles'])} "
            f"{d['nRef']:.12g} "
            f"{d['nRej']:.12g} "
            f"{d['impulse'][0]:.12e} "
            f"{d['impulse'][1]:.12e} "
            f"{d['impulse'][2]:.12e} "
            f"{d['force'][0]:.12e} "
            f"{d['force'][1]:.12e} "
            f"{d['force'][2]:.12e}\n"
        )

print(f"Wrote {out_file}")
print(f"Included {len(files)} local membraneForce.dat files.")
