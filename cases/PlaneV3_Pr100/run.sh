#!/usr/bin/env bash
set -euo pipefail

# Run from the case directory regardless of where the script is invoked.
CASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$CASE_DIR"

# Override these from the shell when needed, for example:
#   OPENFOAM_BASHRC=/path/to/OpenFOAM/etc/bashrc NP=8 ./run.sh
OPENFOAM_BASHRC="${OPENFOAM_BASHRC:-/usr/lib/openfoam/openfoam2412/etc/bashrc}"
NP="${NP:-14}"

if [[ ! -f "$OPENFOAM_BASHRC" ]]; then
    echo "OpenFOAM bashrc not found: $OPENFOAM_BASHRC" >&2
    echo "Set OPENFOAM_BASHRC to the correct OpenFOAM environment file." >&2
    exit 1
fi

# shellcheck disable=SC1090
source "$OPENFOAM_BASHRC"

echo "======================================"
echo "OpenFOAM version: $(foamVersion)"
echo "uniGasFoam executable: $(command -v uniGasFoam || echo 'not found')"
echo "Available logical cores: $(nproc)"
echo "MPI processes: $NP"
echo "Case directory: $CASE_DIR"
echo "======================================"

if ! command -v uniGasFoam >/dev/null 2>&1; then
    echo "uniGasFoam was not found in PATH. Build/install upstream uniGasFoam first." >&2
    exit 1
fi

# Clean generated data. The mesh is intentionally regenerated from blockMeshDict.
rm -rf processor* postProcessing log.* constant/polyMesh
foamListTimes -rm 2>/dev/null || true

echo "Running blockMesh..."
blockMesh | tee log.blockMesh

echo "Running checkMesh..."
checkMesh | tee log.checkMesh

echo "Setting numberOfSubdomains = $NP"
foamDictionary system/decomposeParDict -entry numberOfSubdomains -set "$NP"

echo "Running decomposePar..."
decomposePar -force | tee log.decomposePar

echo "Running uniGasFoam in parallel..."
mpirun -np "$NP" uniGasFoam -parallel | tee log.uniGasFoam

echo "Reconstructing latest time..."
reconstructPar -latestTime | tee log.reconstructPar

echo "======================================"
echo "Run finished."
echo "======================================"
