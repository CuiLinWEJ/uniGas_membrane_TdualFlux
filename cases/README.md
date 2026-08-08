# Case Guide

This directory contains flat-plate development and comparison cases used while revising the permeable-membrane boundary implementation. These cases are **not yet a complete physical-validation suite**.

## Boundary Names

| Patch | Meaning |
|---|---|
| `plate_front` | Physical front side |
| `plate_back` | Physical back side |
| `farField` | External flow boundary |

For coupled membrane patches, `plate_front` is the configured/control cyclic patch and `plate_back` is its cyclic neighbour patch. The physical direction convention is always front/back.

## Probability Convention

`Pr` always means **reflection probability**:

```text
Pr = P(reflection)
Pt = P(transmission) = 1 - Pr
```

## Current Case Matrix

The existing directory names are retained for traceability. Use the table below rather than inferring physics from the historical `V1`, `V3`, or `Tdual` labels alone.

| Directory | Boundary model / purpose | Pr | Front temperature (K) | Back temperature (K) | Notes |
|---|---|---:|---:|---:|---|
| `PlaneV1_membrane_Pr050_Tdual` | Revised two-sided membrane development case | 0.50 | 1866.7 | 200 | Side-dependent temperatures; bidirectional transmission/flux accounting |
| `PlaneV1_membrane_Pr100_Tdual` | Revised two-sided no-transmission regression case | 1.00 | 1866.7 | 200 | Exercises revised code with `Pt = 0` |

Other comparison cases follow the same front/back naming convention.

## Naming for New Cases

For new cases, prefer descriptive names:

```text
FlatPlate_<BoundaryModel>_Pr<NNN>_Tf<frontK>_Tb<backK>
```

Examples:

```text
FlatPlate_Membrane_Pr050_Tf1867_Tb200
FlatPlate_Membrane_Pr100_Tf200
FlatPlate_Solid_Tf200_Tb200
```
