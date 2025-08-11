# Git Hooks for This Project

This directory contains custom Git hooks used in this repository.  
They are designed to **build the solution and run unit tests before committing**, preventing commits if the build or tests fail.

## Setup

1. Configure Git to use this directory for hooks
```bash
git config core.hooksPath .githooks
```
2. Ensure required tools are installed
- PowerShell Code
- Microsoft Build Tools / Visual Studio 2022
3. Make scripts executable (Linux/macOS only)
```bash
chmod +x .githooks/pre-commit
```
## Hooks in This Directory

### pre-commit.cmd
- **Windows** entry point for the pre-commit hook.
- Invokes `pre-commit.ps1` using PowerShell.
- Ensures the hook runs even if PowerShell script execution policy is restricted by using `-ExecutionPolicy Bypass`.

### pre-commit.ps1
- Builds the configured solution using MSBuild.
- Runs the unit tests (`UnitTest.exe`) with Google Test.
- Aborts the commit if:
  - The build fails
  - Unit tests fail

### pre-commit (optional, Bash version)
- Can be created for Linux/macOS environments to run similar checks.
- Not included by default in this project.
