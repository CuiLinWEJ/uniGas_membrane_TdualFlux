# cyclic-direction-rollback integration

## Purpose

Fix membrane direction handling when a cyclic membrane is crossed locally or through a processor-cyclic boundary.

The key distinction is:

- processor-cyclic callback: `controlMol()` still sees the sending side and `td.switchProcessor == true`
- local cyclic callback: OpenFOAM has already mapped the parcel to the receiving side and `td.switchProcessor == false`

Physical front/back direction must therefore use both the current face and the crossing type.

## Membrane source

`source/uniGasReflectiveParticleMembranePatch/uniGasReflectiveParticleMembranePatch.C`

The corrected physical mapping is:

```text
processor crossing:
    front + d > 0  -> front -> back
    back  + d > 0  -> back  -> front

local cyclic crossing:
    back  + d < 0  -> front -> back
    front + d < 0  -> back  -> front
```

For reflection:

- processor crossing: cancel the pending processor transfer with `td.switchProcessor = false`
- local cyclic crossing: call `p.returnAcrossCyclic(cloud_, td)` to map topology back to the incident side immediately

For transmission:

- processor crossing keeps the normal processor transfer
- local cyclic crossing is already on the receiving side and requires no rollback

The temporary raw face/sign validation counters have been removed. `membraneFlux.dat` is back to the production 17-column format.

## uniGasParcel integration

The membrane model needs a small public wrapper around the protected `particle::hitCyclicPatch()` implementation.

Preferred installation method from this repository root:

```bash
python3 patches/apply_uniGasParcel_returnAcrossCyclic.py ~/OpenFOAM/uniGasFoam
```

The installer is idempotent and adds:

```cpp
void returnAcrossCyclic
(
    uniGasCloud& cloud,
    trackingData& td
);
```

to `uniGasParcel.H`, with this implementation in `uniGasParcel.C`:

```cpp
void Foam::uniGasParcel::returnAcrossCyclic
(
    uniGasCloud& cloud,
    trackingData& td
)
{
    particle::hitCyclicPatch(cloud, td);
}
```

A traditional patch is also provided at:

```text
patches/uniGasParcel-returnAcrossCyclic.patch
```

Use `-p1` when applying the git-style patch.

Do not apply both methods to an already modified tree.

## Runtime validation

Validation was performed with 14 MPI ranks to `t = 0.001 s`.

### +X, Pr = 0.50

```text
Reflection fraction = 0.502565
FrontToBack = 45140
BackToFront = 1208
```

### +X, Pr = 1.00

```text
Reflections = 76138
Transmissions = 0
```

The previous cyclic reflection hang did not recur.

### -X, Pr = 0.50

```text
Reflection fraction = 0.499242
FrontToBack = 2964
BackToFront = 72381
```

The reversed free stream correctly reverses the dominant physical transmission direction.

## Validation conclusion

The tests verify:

- reflection probability consistency with configured `Pr`;
- `Pr = 1` zero-transmission behavior without cyclic reflection ping-pong;
- correct physical transmission direction under reversed flow;
- consistent handling of local cyclic and processor-cyclic crossings;
- immediate rollback of local cyclic reflections to the incident side.
