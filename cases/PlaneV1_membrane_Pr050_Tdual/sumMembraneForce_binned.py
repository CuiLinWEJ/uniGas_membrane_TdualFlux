from pathlib import Path
from collections import defaultdict

# Must match forceWriteInterval in boundariesDict
dt_bin = 1.0e-3

root = Path("postProcessing/membraneForce")
files = sorted(root.glob("*/processor*/membraneForce.dat"))

if not files:
    raise SystemExit("No membraneForce.dat files found.")

data = defaultdict(lambda: {
    "nFiles": 0,
    "nRef": 0.0,
    "nRej": 0.0,
    "impulse": [0.0, 0.0, 0.0],
})

for f in files:
    with f.open() as fp:
        for line in fp:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            parts = line.split()

            t = float(parts[0])
            k = int(round(t / dt_bin))
            t_bin = round(k * dt_bin, 12)

            nRef = float(parts[3])
            nRej = float(parts[4])
            impulse = [float(parts[5]), float(parts[6]), float(parts[7])]

            data[t_bin]["nFiles"] += 1
            data[t_bin]["nRef"] += nRef
            data[t_bin]["nRej"] += nRej

            for i in range(3):
                data[t_bin]["impulse"][i] += impulse[i]

out_file = root / "totalMembraneForce_binned.dat"

with out_file.open("w") as out:
    out.write(
        "# time nFiles nReflections nRejections "
        "totalImpulseX totalImpulseY totalImpulseZ "
        "totalForceX totalForceY totalForceZ\n"
    )

    for t in sorted(data):
        d = data[t]
        impulse = d["impulse"]
        force = [impulse[i] / dt_bin for i in range(3)]

        out.write(
            f"{t:.12g} "
            f"{int(d['nFiles'])} "
            f"{d['nRef']:.12g} "
            f"{d['nRej']:.12g} "
            f"{impulse[0]:.12e} "
            f"{impulse[1]:.12e} "
            f"{impulse[2]:.12e} "
            f"{force[0]:.12e} "
            f"{force[1]:.12e} "
            f"{force[2]:.12e}\n"
        )

print(f"Wrote {out_file}")
print(f"Included {len(files)} local membraneForce.dat files.")
