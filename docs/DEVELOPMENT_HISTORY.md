# Development History

## Project Background

This repository documents the post-thesis development of a permeable membrane boundary model for **uniGasFoam**.

The initial implementation was developed during the author's master's research on aerodynamic performance analysis of permeable drag sails for CubeSats operating in rarefied gas environments.

After completion of the thesis, additional investigation identified several implementation issues that could affect the numerical treatment of permeable membrane interactions. To improve the robustness and extensibility of the model, a revised implementation was developed.

---

# Development Objectives

The primary objectives of this development are:

* improve the numerical treatment of permeable membrane boundaries;
* introduce a transient dual-flux formulation;
* support transmitted particle handling;
* improve aerodynamic force evaluation;
* establish a cleaner and more maintainable implementation.

---

# Major Improvements

Compared with the earlier implementation, the current version includes:

* redesign of the permeable membrane interaction procedure;
* implementation of a transient dual-flux framework;
* improved handling of transmitted particles;
* revised aerodynamic force calculation;
* source-code refactoring for improved readability and maintainability;
* additional demonstration and validation cases.

The repository represents an ongoing research implementation rather than a frozen software release.

---

# Development Process

The development followed an iterative workflow:

1. Identify implementation issues through numerical investigation.
2. Analyse the underlying physical and numerical behaviour.
3. Design possible improvements.
4. Implement the modified boundary model.
5. Compile and execute numerical simulations.
6. Evaluate simulation results.
7. Refine the implementation based on verification results.
8. Repeat the process until stable behaviour is achieved.

This iterative process was repeated throughout development to improve numerical consistency and implementation quality.

---

# AI-assisted Development

Generative AI (ChatGPT) was used as a software development assistance tool during the post-thesis development stage.

Typical uses included:

* technical discussion;
* debugging suggestions;
* code review;
* implementation alternatives;
* documentation assistance.

The repository maintainer independently performed:

* identification of implementation issues;
* technical decision-making;
* source-code modification;
* software integration;
* compilation;
* numerical simulations;
* result evaluation;
* verification;
* final implementation selection.

---

# Current Status

Current development stage:

**Research Prototype**

Completed:

* implementation of the revised boundary model;
* successful compilation;
* preliminary verification;
* example simulation cases.

Ongoing:

* comprehensive physical validation;
* additional benchmark comparisons;
* extended documentation;
* further optimisation.

---

# Future Development

Future work will focus on:

* expanding validation cases;
* improving numerical robustness;
* supporting additional membrane models;
* extending documentation;
* preparing a fully validated public release.

---

# Version History

| Version                 | Description                                                                                                     |
| ----------------------- | --------------------------------------------------------------------------------------------------------------- |
| Thesis Version          | Initial implementation developed during the master's research.                                                  |
| Post-thesis Development | Revised implementation with transient dual-flux formulation, improved force evaluation, and ongoing validation. |
