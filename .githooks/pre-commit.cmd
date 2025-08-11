REM Wrapper script (Windows) to run the PowerShell-based pre-commit hook.
REM This batch file is triggered by Git when a commit is made, and delegates execution to a PowerShell script.


@echo off
REM Suppress the display of commands as they are executed (cleaner output).


setlocal
REM Limit any environment variable changes to this batch file's scope only.
REM All changes will be discarded at the endlocal command.


powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0pre-commit.ps1"
REM Run pre-commit.ps1 located in the same directory as this pre-commit.cmd file.
REM -NoProfile: Prevent loading the user's PowerShell profile (avoids environment differences).
REM -ExecutionPolicy Bypass: Temporarily disable PowerShell's script execution policy.
REM %~dp0: Expands to the drive letter and path of the currently running batch file.

set EXITCODE=%ERRORLEVEL%
REM Store the exit code from the previous command (PowerShell execution).


endlocal & exit /b %EXITCODE%
REM End the local environment changes and return the stored exit code to the caller (Git).
REM Git uses this exit code to decide whether to continue (0) or abort (>0) the commit.