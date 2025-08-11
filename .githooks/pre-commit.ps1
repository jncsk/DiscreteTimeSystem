#!/usr/bin/env pwsh
# pre-commit hook: Build and run unit tests before committing
# Exit non-zero to block the commit if tests fail

# 1
# Initialize build/test variables with defaults (can be overridden via environment variables)
$Config       = if ($env:GIT_TEST_CONFIG)   { $env:GIT_TEST_CONFIG }   else { "Debug" }
$Platform     = if ($env:GIT_TEST_PLATFORM) { $env:GIT_TEST_PLATFORM } else { "x64" }
$SolutionName = if ($env:GIT_TEST_SOLUTION) { $env:GIT_TEST_SOLUTION } else { "DiscreteTimeSystem.sln" }
$TimeoutSec   = 300

# 2
# Determine the parent directory of the script’s location and store it as an absolute path in $solutionDir
if ($PSScriptRoot) {
    $scriptDir = $PSScriptRoot
} else {
    $scriptDir = Split-Path -Path $MyInvocation.MyCommand.Path -Parent
}
$solutionDir = (Resolve-Path (Join-Path $scriptDir "..")).Path

# 3
# Verify that the solution file (.sln) exists; if not, print error and abort commit
$solutionPath = Join-Path $solutionDir $SolutionName
if (-not (Test-Path -LiteralPath $solutionPath)) {
    Write-Error "Solution not found: $solutionPath"
    exit 1
}

# 4
# Locate MSBuild (adjust paths if Visual Studio is installed elsewhere)
$msbuildCandidates = @(
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)
$msbuildPath = $msbuildCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $msbuildPath) {
    Write-Error "MSBuild.exe not found. Please update msbuildCandidates."
    exit 1
}

# 5
# Print debug information
Write-Host "[pre-commit] SolutionDir = $solutionDir"
Write-Host "[pre-commit] Configuration = $Config, Platform = $Platform"
Write-Host "[pre-commit] MSBuild: $msbuildPath"
Write-Host "[pre-commit] Solution: $solutionPath"

# 6
# Set MSBuild arguments
$msbuildArgs = @(
    "$solutionPath",
    "/m",
    "/v:m",
    "/p:Configuration=$Config",
    "/p:Platform=$Platform"
)
# Check for null or empty arguments
if (-not $msbuildArgs -or ($msbuildArgs | Where-Object { $_ -eq $null -or "$_".Trim() -eq "" }).Count) {
    Write-Error "msbuild args are empty or contain null: $($msbuildArgs -join ' | ')"
    exit 1
}

# 7
Write-Host "[pre-commit] Building solution..."
Push-Location $solutionDir
try {
    & $msbuildPath @msbuildArgs
    $buildExit = $LASTEXITCODE
} finally {
    Pop-Location
}
if ($buildExit -ne 0) {
    Write-Error "[pre-commit] Build failed with exit code $buildExit"
    exit 1
}

# 8
# Check for unit test executable
$unitTestExe = Join-Path $solutionDir "x64\$Config\UnitTest.exe"
if (-not (Test-Path -LiteralPath $unitTestExe)) {
    Write-Error "[pre-commit] UnitTest.exe not found: $unitTestExe"
    exit 1
}

# 9
# Run unit tests
Write-Host "[pre-commit] Running unit tests..."
$test = Start-Process -FilePath $unitTestExe -ArgumentList "--gtest_color=yes" -Wait -PassThru -NoNewWindow
if ($test.ExitCode -ne 0) {
    Write-Error "[pre-commit] Unit tests failed with exit code $($test.ExitCode)"
    exit 1
}

# All tests passed
Write-Host "[pre-commit] OK - All tests passed."
exit 0
