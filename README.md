# uniGas_membrane_TdualFlux

## Overview

This repository contains the author's **post-thesis research implementation** of a transient dual-flux permeable membrane boundary model for **uniGasFoam**.

The objective of this work is to extend the original reflective membrane boundary treatment by introducing a permeable membrane formulation capable of evaluating transmitted particle flux and aerodynamic force under rarefied gas conditions.

The implementation presented here was developed after completion of the master's thesis to address issues identified during subsequent investigation and to improve the numerical treatment of permeable membrane boundaries.

---

## Features

Current implementation includes:

* Permeable membrane boundary model
* Transient dual-flux treatment
* Particle transmission handling
* Aerodynamic force evaluation
* Example simulation cases
* Preliminary verification cases

---

## Repository Structure

```text
.
├── README.md
├── src/
│   └── Modified source code
├── cases/
│   ├── Validation cases
│   └── Example cases
├── docs/
│   └── Documentation (to be expanded)
└── figures/
```

---

## Case Description

| Case                         | Description                                                                       |
| ---------------------------- | --------------------------------------------------------------------------------- |
| PlaneV1                      | Original flat-plate reference case                                                |
| PlaneV1_membrane_Pr100       | Impermeable membrane (solid-wall limit)                                           |
| PlaneV1_membrane_Pr050_Tdual | Permeable membrane with transmission probability = 0.50 using the dual-flux model |

Additional cases are provided for numerical comparison and model development.

---

## Validation Status

This repository should currently be regarded as a **research prototype**.

Completed:

* Source code implementation
* Successful compilation
* Numerical execution
* Preliminary verification

Ongoing:

* Comprehensive physical validation
* Additional benchmark comparisons
* Extended parameter studies
* Documentation improvement

The implementation is under continuous development, and validation results will be expanded in future releases.

---

## Relationship to uniGasFoam

This repository **does not redistribute the complete uniGasFoam source code**.

Only the author's modifications, additional source files, and demonstration cases are included.

Users should obtain the original **uniGasFoam/OpenFOAM** source code separately and integrate the modifications provided in this repository.

---

## AI Assistance

Generative AI (ChatGPT) was used as a programming assistance tool during post-thesis development for:

* technical discussion
* debugging suggestions
* code review
* documentation drafting

The repository maintainer independently performed:

* implementation design
* source-code modification
* code integration
* compilation
* numerical simulations
* result evaluation
* verification
* final technical decisions

---

## Citation

If you use this implementation in academic research, please cite the corresponding publication (when available) and acknowledge the original uniGasFoam project.

---

## License

This repository contains modifications based on the uniGasFoam/OpenFOAM framework.

The copyrights and licenses of the original projects remain with their respective authors.

This repository only claims copyright over the author's original modifications and accompanying documentation.
