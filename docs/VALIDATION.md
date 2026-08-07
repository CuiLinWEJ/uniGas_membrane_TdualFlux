# Validation Status

## Current Status

**Development stage: Research prototype / preliminary verification**

The current implementation compiles and executes in the development environment, and the repository contains several limiting/comparison cases. These checks are useful for software development, but they do **not** yet constitute comprehensive physical validation.

## What Has Been Checked

### Software execution

- the modified source has compiled successfully in the development environment;
- the membrane boundary executes in the included development cases;
- membrane-force output is produced;
- bidirectional transmitted-particle statistics are produced in the revised two-sided cases.

### Probability limiting/comparison cases

The repository includes `Pr050` and `Pr100` membrane cases. Because `Pr` is reflection probability:

- `Pr050` exercises simultaneous reflection and transmission;
- `Pr100` is the no-transmission limit and is useful for checking that transmitted-particle counts/fluxes vanish.

### Solid-wall comparisons

Diffuse solid-wall cases are retained for comparison with the membrane model under corresponding temperature settings.

## Case Matrix

See [`../cases/README.md`](../cases/README.md) for the exact boundary models, reflection probabilities, and front/back temperatures of all eight cases.

## Not Yet Complete

Before describing the implementation as fully validated, the following work is still required:

### Physical/reference validation

- comparison against an independent analytical, benchmark, or published reference where applicable;
- comparison over more than one flow condition;
- documented acceptance criteria for force and transmitted flux.

### Numerical verification

- time-step sensitivity;
- particle-weight / particles-per-cell sensitivity;
- mesh/domain sensitivity where relevant;
- repeated-run statistical uncertainty;
- conservation checks for particle number, mass, and momentum accounting.

### Reproducibility

- clean integration into a fresh upstream uniGasFoam checkout;
- rebuild from source without repository-provided binaries;
- documented upstream commit/version and compiler/OpenFOAM environment.

## Important Interpretation

Successful execution is not equivalent to physical validation. Likewise, agreement between two implementations that share assumptions is not by itself an independent benchmark.

Until the items above are completed and documented, users should treat results from this repository as research/development results and independently verify them for their intended application.

## Planned Release Criterion

A future `v1.0.0` release should be reserved for a version for which:

1. the exact upstream dependency is frozen/documented;
2. a reproducible benchmark matrix is included;
3. limiting cases are passed;
4. numerical sensitivity/statistical uncertainty are reported; and
5. known limitations are explicitly documented.
