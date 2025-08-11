@echo off
setlocal
REM Wrapper for Windows to run the PowerShell-based pre-commit hook.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0pre-commit.ps1"
set EXITCODE=%ERRORLEVEL%
endlocal & exit /b %EXITCODE%
