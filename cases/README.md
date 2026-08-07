# Case Description

This directory contains the numerical cases used for implementation development and preliminary verification.

## Naming Convention

Case names follow the format

```text
<Geometry>_<BoundaryType>_<TransmissionProbability>_<Model>
```

where

* **Geometry** describes the computational geometry.
* **BoundaryType** indicates the membrane or solid boundary.
* **TransmissionProbability** specifies the membrane transmission probability.
* **Model** identifies the implementation.

---

## Current Cases

| Directory                    | Description                                                    |
| ---------------------------- | -------------------------------------------------------------- |
| PlaneV1                      | Original flat-plate reference case.                            |
| PlaneV1_membrane_Pr100       | Membrane boundary with transmission probability = 1.00.        |
| PlaneV1_membrane_Pr050_Tdual | Dual-flux membrane model with transmission probability = 0.50. |

---

## Transmission Probability

| Name  | Description                     |
| ----- | ------------------------------- |
| Pr000 | No transmission                 |
| Pr025 | Transmission probability = 0.25 |
| Pr050 | Transmission probability = 0.50 |
| Pr075 | Transmission probability = 0.75 |
| Pr100 | Transmission probability = 1.00 |

---

## Notes

These cases are intended for implementation development and preliminary verification.

Additional benchmark and validation cases will be included in future releases.
