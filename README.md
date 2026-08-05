uniGasReflectiveParticleMembranePatch
Tdual + Force + Flux Validated Version

Main source:
source/uniGasReflectiveParticleMembranePatch

Compiled library:
lib/libUniGas_TdualFluxValidated.so

Post-processing scripts:
tools/sumMembraneForce_binned.py
tools/sumMembraneFlux_binned.py

Validated cases:
1. PlaneV1_membrane_Pr100_Tdual
   reflectionProbability = 1.00
   Result:
   - transmitted flux = 0
   - Mean Fx = 1.757990768865e-02 N
   - SD Fx   = 7.828147857726e-05 N
   - CV Fx   = 0.445289 %
   - agrees with dual-temperature solid-wall result

2. PlaneV1_membrane_Pr050_Tdual
   reflectionProbability = 0.50
   Result:
   - Reflections = 286092
   - Transmissions = 284648
   - Total incidents = 570740
   - Transmission fraction = 0.498735
   - Expected = 0.50

Membrane area check:
processor0 localArea = 0.001188 m2
processor3 localArea = 0.000612 m2
totalArea = 0.001800 m2

Conclusion:
The modified membrane boundary supports:
- temperatureMaster / temperatureSlave
- membrane force output
- transmitted-particle flux output
- Pr = 1.00 solid-wall limit
- Pr < 1.00 stochastic transmission behavior
