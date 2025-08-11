REM Wrapper script (Windows) to run the PowerShell-based setup-hook.
REM This batch file needs to be triggered manually when first clone of the repository.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup-hooks.ps1"
REM Run setup-hooks.ps1 located in the same directory as this cmd file.
REM -NoProfile: Prevent loading the user's PowerShell profile (avoids environment differences).

endlocal & exit /b %EXITCODE%
REM Git uses this exit code to decide whether to continue (0) or abort (>0) the commit.