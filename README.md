# DiscreteTimeSystem

A C++ library and sample application for exploring discrete-time system models. The project is organized with a layered architecture to keep modules focused and loosely coupled.

## Project Structure

- **[`DiscreteTimeSystemLib/`](DiscreteTimeSystemLib)** – reusable library broken into Core, Numerics, Control, and Application layers. See [Project Architecture documentation](docs/Project%20Architecture/ProjectArchitecture.md) for a detailed explanation of the layering.
- **[`DiscreteTimeSystemApp/`](DiscreteTimeSystemApp)** – example application that links against the library.
- **[`UnitTest/`](UnitTest)** – tests for the library and application.
- **[`docs/`](docs)** – additional documentation, including notes on the [matrix exponential algorithm](docs/MatrixExponential).

```
/DiscreteTimeSystem
├── DiscreteTimeSystemLib/
├── DiscreteTimeSystemApp/
├── UnitTest/
└── docs/
```

## Building

1. Install Visual Studio 2022 with the **Desktop development with C++** workload.
2. Open `DiscreteTimeSystem.sln`.
3. Select a build configuration (e.g., `Debug|x64`).
4. **Build → Build Solution** to compile the library and application.
5. Use **Test → Run All Tests** to build and execute the `UnitTest` project.

## Usage

1. In Visual Studio's Solution Explorer, select the `DiscreteTimeSystemApp` project and set it as the startup project if needed.
2. Press **Ctrl+F5** (Run Without Debugging) to launch the application.
3. A console window appears showing sample output from the discrete-time system and then closes when execution completes.

## Prerequisites

- Windows with Visual Studio 2022
- C++17-compatible compiler

## Limitations and Future Plans

- Only basic algorithms are implemented; more control and numerical features are planned.
- Additional unit tests and documentation are in progress.

