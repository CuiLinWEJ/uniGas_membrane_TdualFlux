#!/usr/bin/env bash
set -o pipefail

# Run from the case directory regardless of where the script is invoked.
CASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$CASE_DIR"

OPENFOAM_BASHRC="${OPENFOAM_BASHRC:-/usr/lib/openfoam/openfoam2412/etc/bashrc}"
NP="${NP:-14}"

if [[ ! -f "$OPENFOAM_BASHRC" ]]; then
    echo "OpenFOAM bashrc not found: $OPENFOAM_BASHRC" >&2
    exit 1
fi

source "$OPENFOAM_BASHRC"
set -e

echo "OpenFOAM version: ${WM_PROJECT_VERSION:-unknown}"
echo "uniGasFoam executable: $(command -v uniGasFoam || echo 'not found')"
echo "MPI processes: $NP"

if ! command -v uniGasFoam >/dev/null 2>&1; then
    echo "uniGasFoam was not found in PATH." >&2
    exit 1
fi

rm -rf processor* postProcessing log.* constant/polyMesh
foamListTimes -rm 2>/dev/null || true

blockMesh | tee log.blockMesh
checkMesh | tee log.checkMesh
foamDictionary system/decomposeParDict -entry numberOfSubdomains -set "$NP"
decomposePar -force | tee log.decomposePar
mpirun -np "$NP" uniGasFoam -parallel | tee log.uniGasFoam
reconstructPar -latestTime | tee log.reconstructPar
