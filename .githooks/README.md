# Git Hooks for This Project

This directory contains custom Git hooks used in this repository.  
They are designed to **build the solution and run unit tests before committing**, preventing commits if the build or tests fail.

## Setup

1. **Configure Git to use this directory for hooks**
```bash
git config core.hooksPath .githooks
```
2. Ensure required tools are installed
- PowerShell Code
- Microsoft Build Tools / Visual Studio 2022

