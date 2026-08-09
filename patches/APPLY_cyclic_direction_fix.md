# cyclic-direction-rollback integration

## Purpose

Fix the membrane boundary direction handling when a cyclic membrane is decomposed or crossed through processor boundaries.

## Files

1. `source/uniGasReflectiveParticleMembranePatch/uniGasReflectiveParticleMembranePatch.C`

   - separates processor cyclic and local cyclic direction logic
   - keeps physical incident side classification
   - avoids local cyclic reflected particle repeatedly interacting with membrane

2. `patches/uniGasParcel-returnAcrossCyclic.patch`

   Apply to:

```
~/OpenFOAM/uniGasFoam/src/lagrangian/uniGas/parcels/
```

Command:

```bash
cd ~/OpenFOAM/uniGasFoam
patch -p0 < ~/OpenFOAM/uniGas_membrane_TdualFlux-github/patches/uniGasParcel-returnAcrossCyclic.patch
```

## Recompile

```bash
cd ~/OpenFOAM/uniGasFoam
wmake libso src/lagrangian/uniGas
```

## Validation

Run Pr=0.50 first.

Expected:

- Front/Back transmission should no longer be strongly biased by processor partition.
- `rawTrans*` counters should remain physically consistent.
- Pr=1 should not enter reflection ping-pong.
