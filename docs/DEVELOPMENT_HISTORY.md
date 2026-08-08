# Development History

## Background

The membrane boundary implementation originated during master's-level research on rarefied-flow aerodynamic modelling. After thesis completion, the maintainer continued development to investigate and improve the two-sided membrane treatment.

The code in this repository represents a **post-thesis development branch**, not an archival copy of the exact source used for thesis results.

## Post-thesis Development Goals

The revision focused on:

- improving the handling of the coupled membrane interaction;
- applying side-dependent temperatures to the two physical membrane sides;
- recording transmitted-particle transport separately in both directions;
- accumulating membrane impulse for force evaluation;
- making post-processing and development cases reproducible enough for continued validation.

## Implemented Changes

The current source includes:

- probabilistic reflection using `reflectionProbability`;
- transmission probability `Pt = 1 - Pr`;
- diffuse reflection with separate `temperatureFront` and `temperatureBack`;
- front-to-back and back-to-front transmission counters;
- represented-particle, mass, and momentum transport accumulation;
- local membrane-force and membrane-flux output;
- post-processing utilities for processor-local output.

## Repository Cleanup for Public Development

The public-facing repository was reorganized so that:

- compiled binaries are not distributed;
- generated `polyMesh` data are not tracked because cases rebuild with `blockMesh`;
- duplicate development snapshots are removed in favor of Git history;
- case patch names use `plate_front`, `plate_back`, and `farField`;
- `Pr` is documented consistently as reflection probability;
- historical case-directory names are retained but fully described in `cases/README.md`; and
- validation claims are limited to preliminary verification.

## AI-assisted Development

ChatGPT was used interactively during the post-thesis development process for technical discussion, debugging suggestions, code review, and documentation assistance.

## Version Relationship

| Stage | Meaning |
|---|---|
| Thesis implementation | Earlier implementation associated with submitted thesis results |
| Post-thesis development | Continued revision including the source published here |
| Future validated release | Reserved for later documented validation |
