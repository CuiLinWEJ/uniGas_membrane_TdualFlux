#!/usr/bin/env python3
from pathlib import Path
from collections import defaultdict
import argparse
import math
import sys


def parse_args():
    parser = argparse.ArgumentParser(
        description="Sum membraneFlux.dat files and compute global binned flux."
    )

    parser.add_argument(
        "--root",
        default="postProcessing/membraneFlux",
        help="Root directory containing membraneFlux outputs."
    )

    parser.add_argument(
        "--bin-width",
        type=float,
        default=1.0e-3,
        help="Time-bin width. Default: 1e-3 s."
    )

    parser.add_argument(
        "--output",
        default="postProcessing/membraneFlux/totalMembraneFlux_binned.dat",
        help="Output file."
    )

    return parser.parse_args()


def bin_end_time(t, bin_width):
    eps = 1.0e-12
    return math.ceil((t - eps)/bin_width)*bin_width


def main():
    args = parse_args()

    root = Path(args.root)
    out_file = Path(args.output)
    bin_width = args.bin_width

    files = sorted(root.glob("*/processor*/membraneFlux.dat"))

    if not files:
        print(f"No membraneFlux.dat files found under {root}", file=sys.stderr)
        sys.exit(1)

    bins = defaultdict(lambda: {
        "nRecords": 0,
        "areaTime": 0.0,
        "areaByProc": {},
        "nTransAtoB": 0.0,
        "nTransBtoA": 0.0,
        "npAtoB": 0.0,
        "npBtoA": 0.0,
        "massAtoB": 0.0,
        "massBtoA": 0.0,
        "momentumX_areaTime": 0.0,
        "momentumY_areaTime": 0.0,
        "momentumZ_areaTime": 0.0,
    })

    n_rows = 0

    for f in files:
        with f.open("r", errors="ignore") as fh:
            for line in fh:
                line = line.strip()

                if not line or line.startswith("#"):
                    continue

                parts = line.split()

                if len(parts) < 17:
                    print(f"Warning: skipped short row in {f}: {line}", file=sys.stderr)
                    continue

                try:
                    time = float(parts[0])
                    dt = float(parts[1])
                    proc = int(float(parts[2]))
                    local_area = float(parts[3])

                    n_trans_AtoB = float(parts[4])
                    n_trans_BtoA = float(parts[5])

                    np_AtoB = float(parts[6])
                    np_BtoA = float(parts[7])

                    mass_AtoB = float(parts[8])
                    mass_BtoA = float(parts[9])

                    momentum_flux_x = float(parts[14])
                    momentum_flux_y = float(parts[15])
                    momentum_flux_z = float(parts[16])

                except ValueError:
                    print(f"Warning: skipped unparsable row in {f}: {line}", file=sys.stderr)
                    continue

                btime = bin_end_time(time, bin_width)
                rec = bins[btime]

                area_dt = local_area*dt

                rec["nRecords"] += 1
                rec["areaTime"] += area_dt

                # processor-local area is constant; keep one value per processor
                rec["areaByProc"][proc] = max(
                    rec["areaByProc"].get(proc, 0.0),
                    local_area
                )

                rec["nTransAtoB"] += n_trans_AtoB
                rec["nTransBtoA"] += n_trans_BtoA

                rec["npAtoB"] += np_AtoB
                rec["npBtoA"] += np_BtoA

                rec["massAtoB"] += mass_AtoB
                rec["massBtoA"] += mass_BtoA

                # Convert local flux back to momentum contribution,
                # then recompute global flux after summation.
                rec["momentumX_areaTime"] += momentum_flux_x*area_dt
                rec["momentumY_areaTime"] += momentum_flux_y*area_dt
                rec["momentumZ_areaTime"] += momentum_flux_z*area_dt

                n_rows += 1

    out_file.parent.mkdir(parents=True, exist_ok=True)

    with out_file.open("w") as out:
        out.write(
            "# time nRecords totalArea effectiveDuration areaTime "
            "nTransAtoB nTransBtoA "
            "transmittedNpAtoB transmittedNpBtoA "
            "transmittedMassAtoB transmittedMassBtoA "
            "absNumberFlux netNumberFlux "
            "absMassFlux netMassFlux "
            "momentumFluxX momentumFluxY momentumFluxZ\n"
        )

        for t in sorted(bins):
            rec = bins[t]

            area_time = rec["areaTime"]
            total_area = sum(rec["areaByProc"].values())

            if area_time <= 0.0:
                continue

            effective_duration = area_time/total_area if total_area > 0 else 0.0

            abs_np = rec["npAtoB"] + rec["npBtoA"]
            net_np = rec["npAtoB"] - rec["npBtoA"]

            abs_mass = rec["massAtoB"] + rec["massBtoA"]
            net_mass = rec["massAtoB"] - rec["massBtoA"]

            abs_number_flux = abs_np/area_time
            net_number_flux = net_np/area_time

            abs_mass_flux = abs_mass/area_time
            net_mass_flux = net_mass/area_time

            momentum_flux_x = rec["momentumX_areaTime"]/area_time
            momentum_flux_y = rec["momentumY_areaTime"]/area_time
            momentum_flux_z = rec["momentumZ_areaTime"]/area_time

            out.write(
                f"{t:.12g} "
                f"{rec['nRecords']} "
                f"{total_area:.12e} "
                f"{effective_duration:.12e} "
                f"{area_time:.12e} "
                f"{int(round(rec['nTransAtoB']))} "
                f"{int(round(rec['nTransBtoA']))} "
                f"{rec['npAtoB']:.12e} "
                f"{rec['npBtoA']:.12e} "
                f"{rec['massAtoB']:.12e} "
                f"{rec['massBtoA']:.12e} "
                f"{abs_number_flux:.12e} "
                f"{net_number_flux:.12e} "
                f"{abs_mass_flux:.12e} "
                f"{net_mass_flux:.12e} "
                f"{momentum_flux_x:.12e} "
                f"{momentum_flux_y:.12e} "
                f"{momentum_flux_z:.12e}\n"
            )

    print(f"Wrote {out_file}")
    print(f"Included {len(files)} local membraneFlux.dat files.")
    print(f"Read {n_rows} data rows.")


if __name__ == "__main__":
    main()
