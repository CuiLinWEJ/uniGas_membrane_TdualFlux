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

Preferred installation method:

```bash
cd ~/OpenFOAM/uniGas_membrane_TdualFlux-github
git switch fix/cyclic-direction-rollback
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

A traditional patch is also provided at `patches/uniGasParcel-returnAcrossCyclic.patch`. Because it uses git-style `a/src/...` and `b/src/...` paths, use `-p1`, not `-p0`:

```bash
cd ~/OpenFOAM/uniGasFoam
patch -p1 < ~/OpenFOAM/uniGas_membrane_TdualFlux-github/patches/uniGasParcel-returnAcrossCyclic.patch
```

Do not re-apply the patch if one of the two files has already been modified; use the Python installer instead.

## Sync membrane source

```bash
SRC=~/OpenFOAM/uniGas_membrane_TdualFlux-github/source/uniGasReflectiveParticleMembranePatch
DST=~/OpenFOAM/uniGasFoam/src/lagrangian/uniGas/boundaries/derived/cyclicBoundaries/uniGasReflectiveParticleMembranePatch

cp "$SRC/uniGasReflectiveParticleMembranePatch.C" "$DST/"
cp "$SRC/uniGasReflectiveParticleMembranePatch.H" "$DST/"
```

## Recompile

```bash
source /usr/lib/openfoam/openfoam2412/etc/bashrc
cd ~/OpenFOAM/uniGasFoam
wmake libso src/lagrangian/uniGas
```

## Runtime validation

Validation was performed with 14 MPI ranks to `t = 0.001 s`.

### +X, Pr = 0.50

```text
Reflections = 46826
Transmissions = 46348
Total = 93174
Reflection fraction = 0.502565

FrontToBack = 45140
BackToFront = 1208
```

The +X free stream is correctly dominated by physical front-to-back transmission.

### +X, Pr = 1.00

```text
Reflections = 76138
Transmissions = 0
Total = 76138
Reflection fraction = 1
```

The run completed normally to `t = 0.001 s`; the previous reflection ping-pong/hang did not recur.

### -X, Pr = 0.50

```text
Reflections = 75117
Transmissions = 75345
Total = 150462
Reflection fraction = 0.499242

FrontToBack = 2964
BackToFront = 72381
```

The reversed free stream is correctly dominated by physical back-to-front transmission.

## Validation conclusion

The tests jointly verify:

- reflection probability remains statistically consistent with the configured `Pr`
- `Pr = 1` produces zero transmission without cyclic reflection ping-pong
- physical transmission direction reverses correctly with the free-stream direction
- local cyclic and processor-cyclic crossings are classified consistently
- local reflections return immediately to the incident side instead of generating a secondary membrane interaction
