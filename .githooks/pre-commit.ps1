# ============================
#  Git pre-commit hook
#  Build & run unit tests
# ============================

$ErrorActionPreference = "Stop"

# ===== 設定 =====
if ($env:GIT_TEST_CONFIG) {
    $Config = $env:GIT_TEST_CONFIG
} else {
    $Config = "Debug"
}

if ($env:GIT_TEST_PLATFORM) {
    $Platform = $env:GIT_TEST_PLATFORM
} else {
    $Platform = "x64"
}

if ($env:GIT_TEST_SOLUTION) {
    $Solution = $env:GIT_TEST_SOLUTION
} else {
    $Solution = "DiscreteTimeSystem.sln"
}

$MsBuildPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
$TestExePath = ".\x64\$Config\UnitTest.exe"

# ===== ビルド =====
Write-Host "[pre-commit] Building unit tests..."
& "$MsBuildPath" $Solution /m /verbosity:minimal /p:Configuration=$Config /p:Platform=$Platform

# ===== テスト実行 =====
if (Test-Path $TestExePath) {
    Write-Host "[pre-commit] Running unit tests..."
    & $TestExePath --gtest_color=yes
} else {
    Write-Error "[pre-commit] UnitTest.exe not found: $TestExePath"
    exit 1
}

Write-Host "[pre-commit] OK - All tests passed"
exit 0
