#!/usr/bin/env pwsh
# pre-commit hook: Build and run unit tests before committing
# Exit non-zero to block the commit if tests fail

# 設定
$Config       = if ($env:GIT_TEST_CONFIG)   { $env:GIT_TEST_CONFIG }   else { "Debug" }
$Platform     = if ($env:GIT_TEST_PLATFORM) { $env:GIT_TEST_PLATFORM } else { "x64" }
$SolutionName = if ($env:GIT_TEST_SOLUTION) { $env:GIT_TEST_SOLUTION } else { "DiscreteTimeSystem.sln" }
$TimeoutSec   = 300

# スクリプトディレクトリからソリューションルートを解決
if ($PSScriptRoot) {
    $scriptDir = $PSScriptRoot
} else {
    $scriptDir = Split-Path -Path $MyInvocation.MyCommand.Path -Parent
}
$solutionDir = (Resolve-Path (Join-Path $scriptDir "..")).Path

# Solution パスの確認
$solutionPath = Join-Path $solutionDir $SolutionName
if (-not (Test-Path -LiteralPath $solutionPath)) {
    Write-Error "Solution not found: $solutionPath"
    exit 1
}

# MSBuild の場所（環境に応じて変更）
$msbuildCandidates = @(
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)
$msbuildPath = $msbuildCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $msbuildPath) {
    Write-Error "MSBuild.exe not found. Please update msbuildCandidates."
    exit 1
}

# デバッグ出力
Write-Host "[pre-commit] SolutionDir = $solutionDir"
Write-Host "[pre-commit] Configuration = $Config, Platform = $Platform"
Write-Host "[pre-commit] MSBuild: $msbuildPath"
Write-Host "[pre-commit] Solution: $solutionPath"

# MSBuild 引数を文字列配列で作成
$msbuildArgs = @(
    "$solutionPath",
    "/m",
    "/v:m",
    "/p:Configuration=$Config",
    "/p:Platform=$Platform"
)

# null / 空文字 チェック
if (-not $msbuildArgs -or ($msbuildArgs | Where-Object { $_ -eq $null -or "$_".Trim() -eq "" }).Count) {
    Write-Error "msbuild args are empty or contain null: $($msbuildArgs -join ' | ')"
    exit 1
}

# ビルド実行
Write-Host "[pre-commit] Building solution..."
$build = Start-Process -FilePath $msbuildPath -ArgumentList $msbuildArgs -Wait -PassThru
if ($build.ExitCode -ne 0) {
    Write-Error "[pre-commit] Build failed with exit code $($build.ExitCode)"
    exit 1
}

# UnitTest 実行ファイルパス
$unitTestExe = Join-Path $solutionDir "x64\$Config\UnitTest.exe"
if (-not (Test-Path -LiteralPath $unitTestExe)) {
    Write-Error "[pre-commit] UnitTest.exe not found: $unitTestExe"
    exit 1
}

# UnitTest 実行
Write-Host "[pre-commit] Running unit tests..."
$test = Start-Process -FilePath $unitTestExe -ArgumentList "--gtest_color=yes" -Wait -PassThru -NoNewWindow
if ($test.ExitCode -ne 0) {
    Write-Error "[pre-commit] Unit tests failed with exit code $($test.ExitCode)"
    exit 1
}

Write-Host "[pre-commit] OK - All tests passed."
exit 0