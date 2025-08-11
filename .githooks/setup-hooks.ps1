# setup-hooks.ps1
# Configure Git to use .githooks and set executable permissions (Windows)

Write-Host "[setup-hooks] Setting core.hooksPath to .githooks..."
git config core.hooksPath .githooks

if ($LASTEXITCODE -ne 0) {
    Write-Error "[setup-hooks] Failed to set hooksPath. Make sure you are in a Git repository."
    exit 1
}

Write-Host "[setup-hooks] Verifying hooksPath..."
$hooksPath = git config core.hooksPath
Write-Host "[setup-hooks] hooksPath is set to: $hooksPath"

Write-Host "[setup-hooks] Setup complete."

