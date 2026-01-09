#!/usr/bin/env pwsh
# Script pentru build si run pe Windows (similar cu run.sh)
# Foloseste: .\run.ps1

$ErrorActionPreference = "Stop"

Set-Location -Path "$PSScriptRoot\PXRacer"

# Creaza build directory pentru Windows
if (-not (Test-Path "build-windows")) {
    New-Item -ItemType Directory -Path "build-windows" | Out-Null
}
Set-Location -Path "build-windows"

# Build
Write-Host "Building PXRacer for Windows..." -ForegroundColor Cyan
cmake ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

cmake --build . --config Debug
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Run daca build-ul a reusit
Write-Host "Starting PXRacer..." -ForegroundColor Green
# Run from PXRacer/PXRacer/ directory (where assets/ is located)
Set-Location -Path "..\PXRacer"
& "..\build-windows\PXRacer\Debug\PXRacer.exe"
