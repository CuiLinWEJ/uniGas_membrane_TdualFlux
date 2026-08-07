# Theory

## Scope

This document describes the physical interpretation of the modified `uniGasReflectiveParticleMembranePatch` implementation. It is intentionally limited to what is implemented in the current source and does not claim complete physical validation.

## Rarefied-Gas Context

uniGasFoam is a particle-based framework for rarefied-gas simulation. In a permeable membrane model, an incident simulation particle may interact with the surface or cross the coupled patch. The present implementation uses a probabilistic reflection/transmission decision at the membrane.

## Probability Convention

The dictionary entry

```text
reflectionProbability  Pr;
```

defines the probability of reflection:

```text
Pr = P(reflection)
Pt = P(transmission) = 1 - Pr
```

Thus:

- `Pr = 0`: every incident particle is transmitted;
- `Pr = 0.5`: reflection and transmission have equal probability;
- `Pr = 1`: every incident particle is reflected and the membrane reaches the no-transmission limit.

## Reflected Particles

When a particle is selected for reflection, the current implementation samples a diffuse reflected velocity using the membrane-side temperature. The reflected velocity is constructed in a local basis consisting of the face normal and two tangential directions, then the specified membrane velocity is added.

The revised implementation supports separate temperatures for the two coupled sides:

```text
temperatureMaster
temperatureSlave
```

In the example cases:

```text
master -> plate_front
slave  -> plate_back
```

## Transmitted Particles

When a particle is not selected for reflection, it is transmitted through the coupled membrane. The code records transmission statistics separately for the two directions:

- front/master to back/slave (`A -> B`);
- back/slave to front/master (`B -> A`).

The recorded quantities include transmitted parcel counts, represented-particle counts, mass, and signed momentum contributions.

## Bidirectional Flux Accounting

The current source accumulates transmitted quantities independently in both directions and writes local membrane-flux records. The post-processing utility `tools/sumMembraneFlux_binned.py` combines processor-local output and reports both absolute and net flux quantities.

This repository historically used the label `Tdual`. In the public documentation, the implemented feature is described more explicitly as **side-dependent membrane temperatures with bidirectional transmitted-particle flux accounting** rather than assigning a broader formal model name that has not yet been independently validated.

## Membrane Force

For reflected particles, the membrane impulse is accumulated from the particle momentum change. If `U_pre` and `U_post` are the particle velocities before and after reflection, the membrane receives the opposite particle momentum change:

```text
Delta p_membrane = Np * m * (U_pre - U_post)
```

where `Np` is the represented-particle weighting and `m` is the molecular mass. The accumulated impulse is divided by the reporting interval to obtain the reported membrane force.

For particles that are simply transmitted without a velocity change at the membrane, the present implementation does not add a reflection impulse; their transport is instead tracked in the transmission/flux statistics.

## Current Limits of the Theory Documentation

The code-level interpretation above describes the implemented algorithm. It should not be read as proof that the model is physically complete for every membrane material or rarefied-flow regime. Benchmark validation, conservation checks, statistical uncertainty, and sensitivity to numerical parameters remain part of the validation work.
