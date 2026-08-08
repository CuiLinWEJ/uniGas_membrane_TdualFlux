# Integration with uniGasFoam

## Why the Full Solver Is Not Included

This repository intentionally contains only the modified membrane-boundary source and research cases. It does not vendor a full copy of uniGasFoam/OpenFOAM.

## Source Files

The modified implementation is located in:

```text
source/uniGasReflectiveParticleMembranePatch/
├── uniGasReflectiveParticleMembranePatch.C
└── uniGasReflectiveParticleMembranePatch.H
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

## Integration Procedure

Because uniGasFoam layouts can differ by version:

1. locate the existing `uniGasReflectiveParticleMembranePatch.C` and `.H`;
2. compare the source with this repository version;
3. apply the corresponding source modifications;
4. rebuild the required uniGasFoam library and solver;
5. run a no-transmission (`Pr100`) case before transmitting cases.

The source can be located with:

```bash
find /path/to/uniGasFoam -name 'uniGasReflectiveParticleMembranePatch.[CH]' -print
```

## Dictionary Notes

Preferred side-dependent temperature entries:

```text
temperatureFront
temperatureBack
```

A single `temperature` entry may be used as a general fallback. Legacy physical-side key names are no longer supported.
