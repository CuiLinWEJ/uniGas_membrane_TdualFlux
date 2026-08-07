# Post-processing Tools

Run these scripts from an individual case directory after `uniGasFoam` has produced `postProcessing/membraneForce` and/or `postProcessing/membraneFlux`.

## Force

```bash
python ../../tools/sumMembraneForce.py
python ../../tools/sumMembraneForce_binned.py
```

`sumMembraneForce_binned.py` assumes a default bin width of `1e-3 s`, matching the common example `forceWriteInterval`. Check the case dictionary before relying on that default.

## Flux

```bash
python ../../tools/sumMembraneFlux_binned.py --bin-width 1e-3
```

The flux utility aggregates front-to-back and back-to-front transmission output and reports absolute/net number, mass, and momentum flux quantities.
