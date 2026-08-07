# Theory

## Background

Very Low Earth Orbit (VLEO) satellites operate in a rarefied gas environment where the mean free path of atmospheric particles becomes comparable to the characteristic length of spacecraft components. Under these conditions, continuum-based fluid dynamics is no longer applicable, and kinetic approaches such as the Direct Simulation Monte Carlo (DSMC) method are required.

The **uniGasFoam** solver provides a DSMC framework for simulating rarefied gas flows. However, the original membrane boundary treatment is primarily designed for reflective surfaces and does not explicitly account for particle transmission through permeable membranes.

This repository extends the original implementation by introducing a permeable membrane boundary model with a transient dual-flux formulation.

---

# Physical Concept

A permeable membrane differs from an impermeable solid wall because incident particles may either:

* be reflected by the membrane; or
* pass through the membrane.

Therefore, both reflected and transmitted particles contribute to the overall particle transport and momentum exchange.

The transmission behaviour is represented using a transmission probability.

```
Reflection Probability
        +
Transmission Probability
        = 1
```

The present implementation determines the particle behaviour probabilistically during particle-wall interaction.

---

# Dual-Flux Concept

The dual-flux model considers two particle populations during membrane interaction:

1. Reflected particles
2. Transmitted particles

Both particle populations are tracked independently during the interaction process.

This treatment allows:

* particle transmission through the membrane;
* momentum exchange on both sides of the membrane;
* aerodynamic force evaluation.

Compared with a purely reflective boundary, the dual-flux formulation provides a more realistic representation of permeable membrane behaviour.

---

# Aerodynamic Force Evaluation

The aerodynamic force acting on the membrane is evaluated from the momentum exchange generated during particle-wall interactions.

The force contribution includes momentum changes associated with:

* reflected particles;
* transmitted particles.

The total aerodynamic force is accumulated throughout the simulation.

---

# Numerical Framework

The implementation is developed within the **uniGasFoam** DSMC framework.

The current repository contains only the modified source code required for the permeable membrane boundary model.

The original uniGasFoam framework is not redistributed.

---

# Assumptions

The present implementation assumes:

* rarefied gas flow;
* DSMC particle representation;
* probabilistic membrane transmission;
* particle interaction treated at the membrane boundary.

Additional physical models may be incorporated in future versions.

---

# Current Scope

This implementation focuses on:

* permeable membrane boundary treatment;
* transmitted particle handling;
* dual-flux formulation;
* aerodynamic force calculation.

The repository is intended as a research implementation and remains under continuous development.

---

# Future Development

Planned improvements include:

* additional physical validation;
* extended benchmark comparisons;
* more membrane models;
* improved numerical robustness;
* expanded documentation.
