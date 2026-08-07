# uniGas_membrane_TdualFlux

> Post-thesis research prototype for a probabilistic permeable-membrane boundary in **uniGasFoam**, with side-dependent thermal treatment, bidirectional transmission statistics, and membrane-force output.

## Overview

This repository contains the maintainer's post-thesis modifications to the `uniGasReflectiveParticleMembranePatch` boundary model used with **uniGasFoam/OpenFOAM**.

The current implementation treats an incident simulation particle as either:

- **reflected**, with probability `Pr`; or
- **transmitted**, with probability `Pt = 1 - Pr`.

For the revised two-sided implementation, the front/master and back/slave sides may use different membrane temperatures. The code also records transmitted-particle statistics in both directions and accumulates membrane impulse for aerodynamic-force evaluation.

This repository is a **research prototype**. It has been compiled and exercised with the included development/comparison cases, but comprehensive physical validation is still in progress.

## What Is Included

```text
.
├── source/                 Modified membrane boundary source files
├── cases/                  Development and comparison cases
├── tools/                  Force/flux post-processing utilities
├── docs/                   Theory, validation, integration, and development notes
├── CHANGELOG.md            Project change history
├── CITATION.cff            GitHub citation metadata
├── LICENSE                 GNU GPL v3 license text
└── README.md
```

Generated `constant/polyMesh/` directories and compiled binaries are intentionally not tracked. Every included case contains a `blockMeshDict`, and `run.sh` regenerates the mesh before execution.

## Boundary Naming

The example cases use descriptive patch names:

| Patch | Meaning |
|---|---|
| `plate_front` | Front/master side of the flat plate or membrane |
| `plate_back` | Back/slave side of the flat plate or membrane |
| `farField` | External inflow/outflow boundary |

Internally, the uniGasFoam coupled-boundary implementation still uses the terms **master** and **slave**; these correspond to `plate_front` and `plate_back` in the included cases.

## Probability Convention

The case parameter `reflectionProbability` is denoted by `Pr` in directory names and documentation:

```text
Pr = reflection probability
Pt = transmission probability = 1 - Pr
```

Examples:

| Label | Reflection `Pr` | Transmission `Pt` |
|---|---:|---:|
| `Pr000` | 0.00 | 1.00 |
| `Pr050` | 0.50 | 0.50 |
| `Pr100` | 1.00 | 0.00 |

Accordingly, `Pr100` is the **no-transmission (impermeable) limit** of the membrane model.

## Included Cases

Eight flat-plate cases are currently included. They are retained with their existing directory names to preserve development traceability; their exact settings and intended comparison roles are documented in [`cases/README.md`](cases/README.md).

The cases are development/comparison cases, **not a complete validation suite**.

## Relationship to uniGasFoam

This repository does **not** redistribute the complete uniGasFoam solver. It contains only the modified boundary source, cases, tools, and documentation needed to document this post-thesis development.

Obtain upstream uniGasFoam separately, then integrate the files in `source/` into a compatible uniGasFoam source tree. See [`docs/INTEGRATION.md`](docs/INTEGRATION.md).

Upstream project: `NVasileiadis93/uniGasFoam` on GitHub.

## Running an Example Case

The included case scripts currently default to an OpenFOAM v2412 installation path. Override the environment and MPI process count when necessary:

```bash
cd cases/PlaneV1_membrane_Pr050_Tdual
OPENFOAM_BASHRC=/path/to/OpenFOAM/etc/bashrc NP=8 ./run.sh
```

The script performs:

1. cleanup of generated data;
2. `blockMesh`;
3. `checkMesh`;
4. `decomposePar`;
5. parallel `uniGasFoam`; and
6. `reconstructPar -latestTime`.

## Post-processing

Utilities are centralized in `tools/`:

- `sumMembraneForce.py` — sums per-processor force output by reporting time;
- `sumMembraneForce_binned.py` — bins and sums membrane impulse/force;
- `sumMembraneFlux_binned.py` — bins bidirectional transmission statistics and computes global number, mass, and momentum fluxes.

Run them from the case directory, for example:

```bash
python ../../tools/sumMembraneForce_binned.py
python ../../tools/sumMembraneFlux_binned.py
```

## Validation Status

**Status: Research prototype / preliminary verification**

Completed so far:

- source implementation;
- successful compilation in the development environment;
- successful numerical execution of development cases;
- preliminary limiting/comparison checks.

Still required before calling the model fully validated:

- a documented benchmark matrix;
- conservation and statistical-uncertainty checks;
- independent/reference comparisons over a wider parameter range;
- reproducibility checks in a clean upstream environment.

See [`docs/VALIDATION.md`](docs/VALIDATION.md).

## Documentation

| Document | Purpose |
|---|---|
| [`docs/THEORY.md`](docs/THEORY.md) | Physical and numerical interpretation of `Pr`, reflection, transmission, force, and two-sided flux accounting |
| [`docs/INTEGRATION.md`](docs/INTEGRATION.md) | How to integrate the modified source into upstream uniGasFoam |
| [`docs/VALIDATION.md`](docs/VALIDATION.md) | What has and has not been validated |
| [`docs/DEVELOPMENT_HISTORY.md`](docs/DEVELOPMENT_HISTORY.md) | Post-thesis development history and scope |
| [`docs/AI_ASSISTANCE.md`](docs/AI_ASSISTANCE.md) | Disclosure of generative-AI assistance |
| [`CHANGELOG.md`](CHANGELOG.md) | Repository changes |

## AI Assistance

ChatGPT was used as an interactive programming-assistance tool during post-thesis development, including technical discussion, debugging suggestions, and documentation support. The maintainer performed the source integration, compilation, simulation execution, result inspection, and final technical decisions.

See [`docs/AI_ASSISTANCE.md`](docs/AI_ASSISTANCE.md) for the full statement.

## Citation

GitHub-compatible citation metadata are provided in [`CITATION.cff`](CITATION.cff). Because this is still a research prototype, users should cite the exact repository version or commit used and also acknowledge/cite upstream uniGasFoam as appropriate.

## License and Upstream Attribution

uniGasFoam and OpenFOAM are distributed under the GNU General Public License. The modified source files in this repository retain the upstream OpenFOAM license notices and are distributed under **GPL-3.0-or-later**.

The repository does not claim ownership of upstream OpenFOAM/uniGasFoam code. Any copyright claim by the repository maintainer applies only to original human-authored modifications and accompanying original documentation, subject to the upstream GPL terms.
