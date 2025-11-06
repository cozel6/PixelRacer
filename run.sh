#!/bin/bash

# Script pentru build si run pe macOS/Linux
# Foloseste: ./run.sh

cd "$(dirname "$0")/PXRacer"

# Creează build directory pentru macOS
mkdir -p build-macos
cd build-macos

# Build
echo "Building PXRacer for macOS..."
cmake .. && cmake --build . --config Release

# Run dacă build-ul a reușit
if [ $? -eq 0 ]; then
    echo "Starting PXRacer..."
    # Run from PXRacer/PXRacer/ directory (where assets/ is located)
    cd ../PXRacer && ../build-macos/PXRacer/PXRacer
else
    echo "Build failed!"
    exit 1
fi
