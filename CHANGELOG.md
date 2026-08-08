# Changelog

All notable repository changes are documented here.

## [Unreleased]

### Validation

- Comprehensive physical validation remains in progress.
- Exact upstream uniGasFoam commit is not yet pinned.

### Planned

- Add automated upstream patch/integration workflow.
- Add reproducible benchmark and uncertainty results.

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
