# Case Guide

This directory contains flat-plate development and comparison cases used while revising the permeable-membrane boundary implementation. These cases are **not yet a complete physical-validation suite**.

## Boundary Names

| Patch | Meaning |
|---|---|
| `plate_front` | Front/master side |
| `plate_back` | Back/slave side |
| `farField` | External flow boundary |

For coupled membrane patches, `plate_front` corresponds to the internal **master** side and `plate_back` to the **slave** side.

## Probability Convention

`Pr` always means **reflection probability**:

```text
Pr = P(reflection)
Pt = P(transmission) = 1 - Pr
```

Therefore:

| Label | Pr | Pt | Interpretation |
|---|---:|---:|---|
| `Pr000` | 0.00 | 1.00 | fully transmitting limit |
| `Pr050` | 0.50 | 0.50 | equal reflection/transmission probability |
| `Pr100` | 1.00 | 0.00 | no-transmission / impermeable membrane limit |

## Current Case Matrix

The existing directory names are retained for traceability. Use the table below rather than inferring physics from the historical `V1`, `V3`, or `Tdual` labels alone.

| Directory | Boundary model / purpose | Pr | Front temperature (K) | Back temperature (K) | Notes |
|---|---|---:|---:|---:|---|
| `PlaneV1` | Legacy two-patch membrane comparison | 0.50 | 1866.7 | 200 | Separate membrane boundary entries on front and back |
| `PlaneV1_membrane_Pr050_Tdual` | Revised two-sided membrane development case | 0.50 | 1866.7 | 200 | Side-dependent temperatures; bidirectional transmission/flux accounting |
| `PlaneV1_membrane_Pr100` | Single membrane no-transmission comparison | 1.00 | 200 | — | `Pr100` limiting case; no transmitted particles expected |
| `PlaneV1_membrane_Pr100_Tdual` | Revised two-sided no-transmission regression case | 1.00 | 1866.7 | 200 | Exercises revised code with `Pt = 0` |
| `PlaneV1_solid` | Diffuse solid-wall reference | — | 1866.7 | 200 | Uses `uniGasDiffuseWallPatch` on both plate sides |
| `PlaneV1_solid_T200` | Isothermal diffuse solid-wall reference | — | 200 | 200 | Temperature-control comparison |
| `PlaneV3` | Legacy membrane temperature-control comparison | 0.50 | 200 | 200 | Same flat-plate geometry with both sides at 200 K |
| `PlaneV3_Pr100` | Membrane no-transmission temperature-control case | 1.00 | 200 | — | `Pr100` comparison at 200 K |

`—` indicates that a separate back-side temperature is not explicitly configured in that case's membrane boundary entry.

## Meshes

Generated `constant/polyMesh/` directories are intentionally excluded from version control. Every case includes `system/blockMeshDict`, and `run.sh` removes any old mesh and rebuilds it with `blockMesh`.

## Running a Case

From a case directory:

```bash
OPENFOAM_BASHRC=/path/to/OpenFOAM/etc/bashrc NP=8 ./run.sh
```

The default environment path in `run.sh` targets OpenFOAM v2412 and can be overridden with `OPENFOAM_BASHRC`.

## Naming for New Cases

For new cases, prefer descriptive names rather than continuing the historical `V1/V3` sequence:

```text
FlatPlate_<BoundaryModel>_Pr<NNN>_Tf<frontK>_Tb<backK>
```

Examples:

```text
FlatPlate_Membrane_Pr050_Tf1867_Tb200
FlatPlate_Membrane_Pr100_Tf200
FlatPlate_Solid_Tf200_Tb200
```

Use three digits for probability labels (`Pr000`, `Pr025`, `Pr050`, `Pr075`, `Pr100`) so directory sorting remains consistent.
