# Reproducibility

## Scope

This document records the exact upstream uniGasFoam base used for the verified development tree and defines the clean-integration procedure targeted for v0.3.0.

The goal is to separate three things clearly:

1. the untouched upstream uniGasFoam revision;
2. the membrane modifications distributed by this repository; and
3. local build products, test outputs, backups, and debugging files that are not part of the release source.

## Pinned Upstream Base

The verified development tree was based on:

```text
Repository: NVasileiadis93/uniGasFoam
Commit:     b81469f06dd6e70c4ec223d1e849765b297a4915
Subject:    ISSUE #1: force ASCII write dicts
```

The clean reproducibility build used:

```text
OpenFOAM:   v2412
Compiler:   gcc (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0
MPI:        Open MPI 4.1.2
```

This commit is the reproducibility base for the current membrane integration work. Newer upstream uniGasFoam commits may exist, but they are not assumed to be equivalent until the membrane patch and parcel-tracking integration have been re-tested against them.

## Development Tree State

The maintainer's working uniGasFoam tree is intentionally **dirty** because it contains the membrane integration under test. The tracked modifications are limited to the four source files required by the current implementation:

```text
src/lagrangian/uniGas/boundaries/derived/cyclicBoundaries/
  uniGasReflectiveParticleMembranePatch/
    uniGasReflectiveParticleMembranePatch.C
    uniGasReflectiveParticleMembranePatch.H

src/lagrangian/uniGas/parcels/
  uniGasParcel.C
  uniGasParcel.H
```

The working tree also contains untracked build logs, source backups, and generated tutorial output. These files are development artifacts and must not be interpreted as additional release dependencies.

For reproducibility testing, do **not** clean, reset, or reuse the existing development tree. Use a fresh clone instead.

## Clean Integration Procedure

Create a separate clean test tree so the verified development installation remains untouched:

```bash
cd ~/OpenFOAM

git clone https://github.com/NVasileiadis93/uniGasFoam.git uniGasFoam-v030-clean
cd uniGasFoam-v030-clean
git checkout b81469f06dd6e70c4ec223d1e849765b297a4915

git status --short
git rev-parse HEAD
```

Expected state before integration:

```text
HEAD = b81469f06dd6e70c4ec223d1e849765b297a4915
working tree clean
```

From the membrane repository, apply the `uniGasParcel` wrapper:

```bash
cd ~/OpenFOAM/uniGas_membrane_TdualFlux-github
python3 patches/apply_uniGasParcel_returnAcrossCyclic.py \
    ~/OpenFOAM/uniGasFoam-v030-clean
```

Then copy the membrane source:

```bash
SRC=~/OpenFOAM/uniGas_membrane_TdualFlux-github/source/uniGasReflectiveParticleMembranePatch
DST=~/OpenFOAM/uniGasFoam-v030-clean/src/lagrangian/uniGas/boundaries/derived/cyclicBoundaries/uniGasReflectiveParticleMembranePatch

cp "$SRC/uniGasReflectiveParticleMembranePatch.C" "$DST/"
cp "$SRC/uniGasReflectiveParticleMembranePatch.H" "$DST/"
```

After integration, the clean test tree should show exactly these four tracked source files as modified:

```bash
cd ~/OpenFOAM/uniGasFoam-v030-clean
git status --short
```

Expected tracked modifications:

```text
M src/lagrangian/uniGas/boundaries/derived/cyclicBoundaries/uniGasReflectiveParticleMembranePatch/uniGasReflectiveParticleMembranePatch.C
M src/lagrangian/uniGas/boundaries/derived/cyclicBoundaries/uniGasReflectiveParticleMembranePatch/uniGasReflectiveParticleMembranePatch.H
M src/lagrangian/uniGas/parcels/uniGasParcel.C
M src/lagrangian/uniGas/parcels/uniGasParcel.H
```

The fresh-clone integration check on 2026-08-10 produced exactly these four tracked modifications. The wrapper installer reported one declaration and one implementation added successfully, with no extra tracked files changed.

## Build Procedure

Load OpenFOAM v2412 and build the modified uniGas library/solver using the upstream project's normal build procedure:

```bash
source /usr/lib/openfoam/openfoam2412/etc/bashrc
cd ~/OpenFOAM/uniGasFoam-v030-clean
./Allwmake
```

The clean-tree build completed successfully with `Allwmake exit = 0`.

The resulting solver resolved:

```text
uniGasFoam:
/home/aero506/OpenFOAM/aero506-v2412/platforms/linux64GccDPInt32Opt/bin/uniGasFoam

libUniGas.so:
/home/aero506/OpenFOAM/aero506-v2412/platforms/linux64GccDPInt32Opt/lib/libUniGas.so

libOpenFOAM.so:
/usr/lib/openfoam/openfoam2412/platforms/linux64GccDPInt32Opt/lib/libOpenFOAM.so
```

If a full `Allwmake` build is unnecessarily expensive during iteration, the modified library can also be rebuilt directly:

```bash
wmake libso src/lagrangian/uniGas
```

## Minimum Regression Matrix

A clean integration is not considered verified solely because it compiles. At minimum, the following regression behavior must be reproduced:

```text
+X, Pr = 0.50
    reflection fraction statistically near 0.50
    FrontToBack > BackToFront

+X, Pr = 1.00
    Transmissions = 0
    run completes without cyclic reflection ping-pong

-X, Pr = 0.50
    reflection fraction statistically near 0.50
    BackToFront > FrontToBack

membraneFlux.dat
    production format contains exactly 17 columns
```

For short statistical regression tests, the exact historical particle counts are not required to match because DSMC sampling is stochastic. Directionality, limiting behavior, output schema, and statistically consistent reflection probability are the primary acceptance criteria.

### Clean `Pr100` result

The clean-build `Pr100` case produced:

```text
Reflections = 76093
Transmissions = 0
Total = 76093
Reflection fraction = 1
membraneFlux columns = 17
Final simulation time = 0.001 s
Solver termination = End / Finalising parallel run
```

The solver log reached `Time = 0.001` and then terminated normally with `End` and `Finalising parallel run`. This confirms the exact no-transmission limiting behavior, absence of the previous cyclic reflection hang for this clean build, and the production 17-column flux schema.

### Clean `Pr050` forward (`+X`) result

The clean-build forward-flow `Pr050` case produced:

```text
Reflections = 46606
Transmissions = 46354
Total = 92960
Reflection fraction = 0.501355
FrontToBack = 45212
BackToFront = 1142
Final simulation time = 0.001 s
Solver termination = End / Finalising parallel run
```

The reflection fraction is statistically consistent with `Pr = 0.50`, and the transmitted-particle direction is strongly dominated by `FrontToBack`, as expected for the forward-flow regression configuration.

### Clean `Pr050` reverse (`-X`) result

The clean-build reverse-flow `Pr050` case produced:

```text
Reflections = 75037
Transmissions = 75538
Total = 150575
Reflection fraction = 0.498336
FrontToBack = 2928
BackToFront = 72610
Final simulation time = 0.001 s
Solver termination = End / Finalising parallel run
```

The reflection fraction is statistically consistent with `Pr = 0.50`, and the transmitted-particle direction reverses as expected: `BackToFront` strongly dominates `FrontToBack`.

## v0.3.0 Reproducibility Checklist

The following clean-room reproducibility items are complete:

- [x] exact upstream repository identified;
- [x] exact upstream base commit identified;
- [x] OpenFOAM environment identified (`v2412`);
- [x] compiler version recorded (`GCC 11.4.0`);
- [x] MPI implementation/version recorded (`Open MPI 4.1.2`);
- [x] fresh upstream clone created at the pinned commit;
- [x] wrapper installer applied successfully to the fresh clone;
- [x] exactly four tracked source modifications confirmed;
- [x] clean-tree build completed successfully;
- [x] `Pr100` no-transmission regression passed (`T = 0`, reflection fraction `1`, normal completion to `t = 0.001 s`);
- [x] `Pr050` forward-direction regression passed (reflection fraction `0.501355`, `FrontToBack > BackToFront`, normal completion to `t = 0.001 s`);
- [x] `Pr050` reverse-direction regression passed (reflection fraction `0.498336`, `BackToFront > FrontToBack`, normal completion to `t = 0.001 s`);
- [x] production 17-column flux schema confirmed.

## Interpretation

The clean-room integration/regression matrix is complete for the current v0.3.0 development branch. Pinning the upstream commit and reproducing the build and directional regressions makes the software integration substantially more reproducible, but it does not by itself constitute physical validation. Numerical sensitivity, repeated-run uncertainty, conservation accounting, parameter validation, reporting-finalisation checks, and independent/reference comparisons remain separate tasks.
