# Changelog

All notable repository changes are documented here.

## [0.3.0] - 2026-08-10

### Validation

- Pinned the reproducibility base to `NVasileiadis93/uniGasFoam` commit `b81469f06dd6e70c4ec223d1e849765b297a4915` with OpenFOAM v2412.
- Reproduced the integration from a fresh upstream clone and confirmed exactly four tracked source modifications.
- Completed a clean `Allwmake` build with GCC 11.4.0 and Open MPI 4.1.2.
- Re-ran the clean regression matrix to `t = 0.001 s`: `Pr100`, forward `Pr050`, and reverse `Pr050` all completed normally with the expected limiting/statistical behavior and transmission direction.
- Reconfirmed the production 17-column `membraneFlux.dat` schema.
- Added membrane input validation for `reflectionProbability`, `temperatureFront`, `temperatureBack`, and `forceWriteInterval`.
- Verified runtime `boundariesDict` reload through `updateProperties()` by changing `reflectionProbability` from `0.50` to invalid `1.10` during execution and observing the expected fatal validation error.
- Verified partial-interval membrane force/flux flushing with `endTime = 1e-5 s` and `forceWriteInterval = 1e-3 s`; both output files were preserved and the 17-column flux schema remained unchanged.
- Re-ran `Pr100`, forward `Pr050`, and reverse `Pr050` after the reporting revision; all regression acceptance criteria remained satisfied.
- Comprehensive physical validation remains in progress.

### Added

- Added `docs/REPRODUCIBILITY.md` with the pinned upstream base, clean-integration procedure, build environment, regression matrix, and acceptance checklist.

### Fixed

- Added range validation for membrane-specific input properties.
- Refreshed membrane-specific cached properties in `updateProperties()`.
- Preserved partial membrane force/flux reporting intervals at OpenFOAM write events.

### Planned
- Add automated upstream patch/integration workflow.
- Add reproducible benchmark and uncertainty results.
- Add numerical sensitivity, uncertainty, and conservation studies before a future `v1.0.0` release.

## [0.2.0] - 2026-08-10

### Fixed

- Corrected physical front/back incident-direction classification for local cyclic and processor-cyclic membrane crossings.
- Corrected local cyclic reflection handling by rolling reflected parcels back to the incident side immediately.
- Eliminated the cyclic reflection ping-pong behavior that could stall the `Pr100` no-transmission case.

### Added

- Added the `uniGasParcel::returnAcrossCyclic()` integration helper around OpenFOAM's existing cyclic topology/transform handling.
- Added an idempotent Python installer for the required `uniGasParcel.H/.C` wrapper.
- Added a traditional unified patch as an alternative integration method.
- Documented the production 17-column membrane-flux output after removal of temporary raw-direction diagnostics.

### Regression verification

The cyclic-direction revision was exercised with 14 MPI ranks to `t = 0.001 s`:

- `+X`, `Pr = 0.50`: reflection fraction `0.502565`; front-to-back transmission dominated (`45140` vs `1208`).
- `+X`, `Pr = 1.00`: `76138` reflections, `0` transmissions; the run completed without the previous reflection hang.
- `-X`, `Pr = 0.50`: reflection fraction `0.499242`; back-to-front transmission dominated (`72381` vs `2964`).

A cleanup smoke test also confirmed that the production membrane-flux record contains 17 columns and that temporary raw-direction diagnostic fields are absent.

### Documentation

- Updated the integration instructions to include the required `uniGasParcel` cyclic rollback wrapper.
- Updated citation metadata to version `0.2.0`.

### Status

This version is a **verified research prototype**, not a claim of comprehensive physical validation. Independent/reference validation, numerical sensitivity, repeated-run uncertainty, conservation checks, and clean-upstream reproducibility remain future work.

## [0.1.0] - 2026-08-07

### Added

- Post-thesis membrane-boundary source implementation.
- Side-dependent front/back membrane temperatures.
- Bidirectional transmitted-particle statistics.
- Membrane impulse/force output.
- Force and flux post-processing utilities.
- Eight development/comparison cases.
- Theory, integration, validation, development-history, and AI-assistance documentation.
- GitHub `CITATION.cff` metadata.
- GPL-3.0 license text and upstream notice.

### Changed

- Standardized probability documentation: `Pr` is reflection probability and `Pt = 1 - Pr`.
- Renamed example-case patches from `statGrid_master/statGrid_slave` to physical `plate_front/plate_back` naming.
- Renamed external case patch from `flow` to `farField`.
- Corrected `temperatureFront` value to `1866.7` K in revised two-sided cases.
- Replaced ambiguous public descriptions with implemented features: side-dependent membrane temperatures and bidirectional transmitted-particle flux accounting.
- Renamed user-facing transmission counters to `FrontToBack` and `BackToFront`.
- Made case `run.sh` scripts configurable via `OPENFOAM_BASHRC` and `NP`.

### Removed

- Legacy public terminology based on coupled-patch side naming.
- Compiled `uniGasFoam` executable and `.so` binary.
- Generated `constant/polyMesh/` directories.
- Duplicate source snapshots and temporary development dictionary files.

### Status

This version is a **research prototype with preliminary verification**, not a fully validated release.
