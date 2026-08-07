# Integration with uniGasFoam

## Why the Full Solver Is Not Included

This repository intentionally contains only the modified membrane-boundary source and research cases. It does not vendor a full copy of uniGasFoam/OpenFOAM. This makes the post-thesis changes easier to identify and avoids presenting upstream source as original work.

## Dependency

Obtain a compatible upstream uniGasFoam source tree first. The exact upstream commit used for the development environment should be recorded before a stable release; the current repository does not yet freeze that dependency.

## Source Files

The modified implementation is located in:

```text
source/uniGasReflectiveParticleMembranePatch/
├── uniGasReflectiveParticleMembranePatch.C
└── uniGasReflectiveParticleMembranePatch.H
```

These files retain the OpenFOAM GPL license notice.

## Integration Procedure

Because uniGasFoam layouts can differ by version, do not blindly copy files into an arbitrary path. In the upstream checkout:

1. locate the existing `uniGasReflectiveParticleMembranePatch.C` and `.H`;
2. record the upstream commit before modification;
3. compare the upstream files with the files in this repository;
4. replace/apply the modifications in the corresponding boundary-model source directory;
5. ensure the class remains listed in the appropriate library `Make/files`;
6. rebuild the relevant uniGasFoam library and solver using the upstream build procedure; and
7. run a `Pr100` limiting case before using transmitting cases.

A useful way to locate the upstream files is:

```bash
find /path/to/uniGasFoam -name 'uniGasReflectiveParticleMembranePatch.[CH]' -print
```

## Case Configuration

A revised two-sided membrane entry uses:

```text
generalBoundaryProperties
{
    patch       plate_front;
}

boundaryModel   uniGasReflectiveParticleMembranePatch;

uniGasReflectiveParticleMembranePatchProperties
{
    reflectionProbability   0.50;
    velocity                (0 0 0);
    temperatureMaster       1866.7;
    temperatureSlave        200;
    forceWriteInterval      1e-3;
}
```

`reflectionProbability` is `Pr`; transmission probability is `Pt = 1 - Pr`.

## Build/Reproducibility Limitation

The repository currently does not include a standalone `Make/` directory because the modified class is intended to replace/integrate with the corresponding upstream class. A future stable release should pin the upstream commit and provide an automated patch or integration script.
