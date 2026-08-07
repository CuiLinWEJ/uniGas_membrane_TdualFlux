# Development History

## Background

The membrane boundary implementation originated during master's-level research on rarefied-flow aerodynamic modelling. After thesis completion, the maintainer identified a directional/implementation issue that warranted further investigation and continued development outside the submitted thesis version.

The code in this repository therefore represents a **post-thesis development branch**, not an archival copy of the exact source used for the thesis results.

## Post-thesis Development Goals

The revision focused on:

- correcting the handling of the coupled membrane interaction identified during later investigation;
- applying side-dependent temperatures to the two sides of the coupled membrane;
- recording transmitted-particle transport separately in both directions;
- accumulating membrane impulse for force evaluation;
- making post-processing and development cases reproducible enough for continued validation.

## Implemented Changes

The current source includes:

- probabilistic reflection using `reflectionProbability`;
- transmission probability `Pt = 1 - Pr`;
- diffuse reflection with separate `temperatureMaster` and `temperatureSlave`;
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

ChatGPT was used interactively during the post-thesis development process for technical discussion, debugging suggestions, code review, and documentation assistance. The maintainer supplied code and run results, evaluated suggestions, modified/integrated the implementation, executed simulations, and made the final technical decisions.

See [`AI_ASSISTANCE.md`](AI_ASSISTANCE.md) for the detailed disclosure.

## Version Relationship

| Stage | Meaning |
|---|---|
| Thesis implementation | Earlier implementation associated with the submitted thesis |
| Post-thesis development | Continued revision after the thesis, including the source published here |
| Future validated release | Reserved for a later version after documented independent/numerical validation |

The repository should not be described as the exact thesis-source archive unless a separate historical tag or source snapshot is explicitly created for that purpose.
