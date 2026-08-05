#!/bin/bash

# =========================
# Local uniGasFoam run script
# Start from blockMesh
# No initialisation command
# =========================

source /usr/lib/openfoam/openfoam2412/etc/bashrc

# 本機 nproc = 28，先用 14 比較穩
NP=14

echo "======================================"
echo "OpenFOAM version:"
foamVersion

echo "Using uniGasFoam:"
which uniGasFoam

echo "Available logical cores:"
nproc

echo "Run with NP = $NP"
echo "Case directory:"
pwd
echo "======================================"

# =========================
# Clean old results
# =========================

echo "Cleaning old results..."

rm -rf processor*
rm -rf postProcessing
rm -rf log.*
rm -rf constant/polyMesh

# 刪除舊時間資料，但保留 0 資料夾
foamListTimes -rm 2>/dev/null

# =========================
# Mesh generation
# =========================

echo "Running blockMesh..."
blockMesh | tee log.blockMesh

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo "blockMesh failed. Stop."
    exit 1
fi

echo "Running checkMesh..."
checkMesh | tee log.checkMesh

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo "checkMesh failed. Stop."
    exit 1
fi

# =========================
# Decomposition
# =========================

echo "Setting numberOfSubdomains = $NP"

if [ -f system/decomposeParDict ]; then
    foamDictionary system/decomposeParDict -entry numberOfSubdomains -set "$NP"
else
    echo "system/decomposeParDict not found. Stop."
    exit 1
fi

echo "Running decomposePar..."
decomposePar -force | tee log.decomposePar

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo "decomposePar failed. Stop."
    exit 1
fi

# =========================
# Run solver
# =========================

echo "Running uniGasFoam in parallel..."

mpirun -np $NP uniGasFoam -parallel | tee log.uniGasFoam

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo "uniGasFoam failed. Stop."
    exit 1
fi

# =========================
# Reconstruct latest time
# =========================

echo "Reconstructing latest time..."

reconstructPar -latestTime | tee log.reconstructPar

echo "======================================"
echo "Run finished."
echo "======================================"
