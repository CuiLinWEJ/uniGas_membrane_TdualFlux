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

The development environment used OpenFOAM v2412.

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

## Build Procedure

Load OpenFOAM v2412 and build the modified uniGas library/solver using the upstream project's normal build procedure. The development environment supports:

```bash
source /usr/lib/openfoam/openfoam2412/etc/bashrc
cd ~/OpenFOAM/uniGasFoam-v030-clean
./Allwmake
```

If a full `Allwmake` build is unnecessarily expensive during iteration, the modified library can be rebuilt directly:

```bash
wmake libso src/lagrangian/uniGas
```

After compilation, verify which library the solver resolves:

```bash
command -v uniGasFoam
ldd "$(command -v uniGasFoam)" | grep -E 'libUniGas|libOpenFOAM'
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

## v0.3.0 Reproducibility Checklist

The following items should be completed before a v0.3.0 release is marked reproducible:

- [x] exact upstream repository identified;
- [x] exact upstream base commit identified;
- [x] OpenFOAM major environment identified (`v2412`);
- [x] fresh upstream clone created at the pinned commit;
- [ ] wrapper installer applied successfully to the fresh clone;
- [ ] exactly four tracked source modifications confirmed;
- [ ] clean-tree build completed successfully;
- [ ] `Pr100` no-transmission regression passed;
- [ ] `Pr050` forward-direction regression passed;
- [ ] `Pr050` reverse-direction regression passed;
- [ ] production 17-column flux schema confirmed;
- [ ] compiler version recorded;
- [ ] MPI implementation/version recorded.

## Interpretation

Pinning the upstream commit makes the software integration reproducible, but it does not by itself constitute physical validation. Numerical sensitivity, repeated-run uncertainty, conservation accounting, and independent/reference comparisons remain separate validation tasks.
