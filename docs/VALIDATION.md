# Validation Status

## Overview

This document summarizes the current validation status of the transient dual-flux permeable membrane boundary model implemented in this repository.

The objective of the validation process is to verify numerical correctness, physical consistency, and computational stability before the implementation is considered fully validated.

---

# Current Status

**Development Stage:** Research Prototype

The implementation is functional and capable of performing numerical simulations. However, validation is still ongoing and should not yet be regarded as complete.

---

# Completed

## Source Code

* Source code implementation completed.
* Successful compilation under the target uniGasFoam/OpenFOAM environment.
* Basic functionality verified.

## Numerical Execution

The modified boundary model can:

* execute simulation cases successfully;
* calculate transmitted particle behaviour;
* evaluate aerodynamic force;
* produce stable numerical results for the current demonstration cases.

## Preliminary Verification

Initial verification has been carried out using the example cases included in this repository.

These cases confirm that:

* the implementation executes correctly;
* the modified algorithm behaves as expected;
* no obvious numerical instability has been observed under the tested conditions.

---

# Ongoing Validation

The following work is still in progress.

## Physical Validation

* Comparison with additional reference cases.
* Comparison with published literature.
* Evaluation under different flow conditions.

## Numerical Assessment

* Sensitivity studies.
* Additional convergence analysis.
* Robustness evaluation.

## Extended Testing

* Additional membrane transmission probabilities.
* Different geometries.
* Wider operating conditions.

---

# Known Limitations

At the current stage, users should be aware that:

* validation is still under active development;
* only the included demonstration cases have been examined;
* additional benchmark comparisons are planned for future releases.

---

# Future Work

The following tasks are planned:

* complete benchmark validation;
* expand verification cases;
* improve documentation;
* provide additional example cases;
* publish a stable validated release.

---

# Citation

If this implementation is used in research, please cite the corresponding publication (when available) and acknowledge the original uniGasFoam project.

---

# Revision History

| Version | Status                                                   |
| ------- | -------------------------------------------------------- |
| v0.1    | Initial research prototype with preliminary verification |
