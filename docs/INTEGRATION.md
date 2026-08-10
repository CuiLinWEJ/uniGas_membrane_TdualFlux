# Integration with uniGasFoam

## Why the Full Solver Is Not Included

This repository intentionally contains only the modified membrane-boundary source, integration helper, research cases, tools, and documentation. It does not vendor a full copy of uniGasFoam/OpenFOAM.

## Pinned Upstream Base

The verified development tree is based on:

```text
Repository: NVasileiadis93/uniGasFoam
Commit:     b81469f06dd6e70c4ec223d1e849765b297a4915
Subject:    ISSUE #1: force ASCII write dicts
OpenFOAM:   v2412
```

Use this exact commit for reproducibility work. Newer upstream revisions should be treated as unverified until the membrane integration has been rebuilt and regression-tested against them.

See [`REPRODUCIBILITY.md`](REPRODUCIBILITY.md) for the clean-clone procedure and acceptance checklist.

## Source Files

The modified membrane implementation is located in:

```text
source/uniGasReflectiveParticleMembranePatch/
├── uniGasReflectiveParticleMembranePatch.C
└── uniGasReflectiveParticleMembranePatch.H
```

The current cyclic-reflection rollback also requires a small public wrapper in upstream `uniGasParcel.H/.C`. The preferred installer is:

```text
patches/apply_uniGasParcel_returnAcrossCyclic.py
```

A traditional unified patch is also provided at:

```text
patches/uniGasParcel-returnAcrossCyclic.patch
```

## Case Configuration

The revised two-sided membrane entry uses physical front/back names:

```text
uniGasReflectiveParticleMembranePatchProperties
{
    reflectionProbability   0.50;
    velocity                (0 0 0);
    temperatureFront        1866.7;
    temperatureBack         200;
    forceWriteInterval      1e-3;
}
```

Convention:

```text
front = configured/control cyclic patch
back  = cyclic neighbour patch
```

The terms front/back describe physical membrane sides. They should not be confused with OpenFOAM cyclic patch implementation terminology.

## Required `uniGasParcel` Wrapper

For a local cyclic crossing, OpenFOAM has already mapped the parcel to the receiving side before the membrane model is called. A reflected parcel therefore has to be mapped back to its incident side immediately. The membrane model calls:

```cpp
p.returnAcrossCyclic(cloud_, td);
```

The helper exposes the existing protected `particle::hitCyclicPatch()` behavior without duplicating OpenFOAM's topology/transform logic.

From the repository root, apply the wrapper to a compatible uniGasFoam source tree:

```bash
python3 patches/apply_uniGasParcel_returnAcrossCyclic.py ~/OpenFOAM/uniGasFoam
```

The installer is idempotent: if the declaration or implementation is already present, it reports `[skip]` instead of adding a duplicate.

The traditional patch can be used instead when the target source matches its context exactly:

```bash
cd ~/OpenFOAM/uniGasFoam
patch -p1 < ~/OpenFOAM/uniGas_membrane_TdualFlux-github/patches/uniGasParcel-returnAcrossCyclic.patch
```

Do not apply both methods to an already modified tree. Prefer the Python installer when source formatting differs from the stored patch context.

## Membrane Source Integration

After the wrapper is present, copy the membrane source files into the corresponding uniGasFoam boundary directory. In the development layout used for verification:

```bash
SRC=~/OpenFOAM/uniGas_membrane_TdualFlux-github/source/uniGasReflectiveParticleMembranePatch
DST=~/OpenFOAM/uniGasFoam/src/lagrangian/uniGas/boundaries/derived/cyclicBoundaries/uniGasReflectiveParticleMembranePatch

cp "$SRC/uniGasReflectiveParticleMembranePatch.C" "$DST/"
cp "$SRC/uniGasReflectiveParticleMembranePatch.H" "$DST/"
```

Because uniGasFoam layouts can differ by version, locate the existing membrane files first when the path differs:

```bash
find /path/to/uniGasFoam -name 'uniGasReflectiveParticleMembranePatch.[CH]' -print
```

## Rebuild

The minimum library rebuild used in the verified development environment was:

```bash
source /usr/lib/openfoam/openfoam2412/etc/bashrc
cd ~/OpenFOAM/uniGasFoam
wmake libso src/lagrangian/uniGas
```

If the upstream uniGasFoam tree provides a project-level `Allwmake`, using that canonical build script is also appropriate for a clean rebuild.

After rebuilding, confirm that `uniGasFoam` resolves the intended custom library, for example:

```bash
ldd "$(command -v uniGasFoam)" | grep -E 'libUniGas|libOpenFOAM'
```

## Recommended Regression Check

Before production use, run the no-transmission `Pr100` case and at least one transmitting case. The v0.2.0 regression set additionally checks reversed flow to verify that the physical transmission direction reverses correctly.

See [`VALIDATION.md`](VALIDATION.md), [`REPRODUCIBILITY.md`](REPRODUCIBILITY.md), and [`../patches/APPLY_cyclic_direction_fix.md`](../patches/APPLY_cyclic_direction_fix.md) for the recorded regression results and clean integration procedure.

## Dictionary Notes

Preferred side-dependent temperature entries:

```text
temperatureFront
temperatureBack
```

A single `temperature` entry may be used as a general fallback. Legacy physical-side key names are no longer supported.

## Compatibility Status

The current integration has been compiled and exercised against upstream uniGasFoam commit `b81469f06dd6e70c4ec223d1e849765b297a4915` in an OpenFOAM v2412 development environment. Clean-clone reproduction against this pinned base has been completed for the v0.3.0 development branch, including clean integration, build verification, membrane parameter checks, reporting verification, and the three-case regression matrix. Comprehensive physical validation remains a separate task.
