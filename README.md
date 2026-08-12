# DiscreteTimeSystem

## Purpose of This Project
This project provides mathematical calculation algorithms implemented in C.
The algorithms are exposed through HTTP APIs hosted on Azure.

## Technical Highlights
- Implemented mathematical calculation algorithms for discrete-time systems in C. 
- Documented the underlying mathematical theory, algorithm design, and implementation details in [`02_Math/`](docs/02_Math/).
- Built and tested a multi-target C/C++ project on Linux using CMake.
- Added automated unit tests for the calculation library.
- Containerized the HTTP server in a Linux-based Docker image.
- Configured Nginx as a reverse proxy.
- Deployed the application to Azure as a Linux-based Docker container.

## System Architecture

```text
Client
  │ HTTP :80
  ▼
Nginx
  │ HTTP :8080
  ▼
DiscreteTimeSystemRunner
  │ Function calls
  ▼
DiscreteTimeSystemLib
```
Nginx receives client requests and forwards them to the HTTP server.
DiscreteTimeSystemRunner converts HTTP requests into calls to the calculation library.
DiscreteTimeSystemLib performs the mathematical calculations.

## Project Structure
This project is organized into three main areas:
1. Application Components
2. Build and Dependency Management
3. Deployment Infrastructure

### 1. Application Components
Contains the C calculation library, sample application, HTTP server, and unit tests.
- **[`DiscreteTimeSystemLib/`](DiscreteTimeSystemLib)** – reusable C library containing the mathematical calculation algorithms.
- **[`DiscreteTimeSystemApp/`](DiscreteTimeSystemApp)** – console application for running and verifying the library directly.
- **[`DiscreteTimeSystemRunner/`](DiscreteTimeSystemRunner)** – an HTTP server exposing functionality from DiscreteTimeSystemLib
- **[`UnitTest/`](UnitTest)** – tests for the library and application.

### 2. Build and Dependency Management
Contains the CMake configuration and third-party dependencies required to build and link the project.
- **[`CMakeLists.txt`](CMakeLists.txt)** – top-level CMake configuration.
- **[`external/`](external)** – third-party libraries and dependencies.

### 3. Deployment Infrastructure
Contains the Docker, Nginx, and Azure configurations used to package, expose, and host the HTTP API.
- **[`Dockerfile`](Dockerfile)** – container image definition for building and running the HTTP server.
- Nginx configuration – reverse-proxy configuration that forwards incoming requests to the HTTP server.
- Azure configuration – configuration used to host the containerized application.


## Build and Run
### 1. Linux with CMake
<!-- Add the actual configure, build, and run commands. -->

### 2. Windows with Visual Studio
1. Open the solution in Visual Studio 2022.
2. Select DiscreteTimeSystemApp as the startup project.
3. Press Ctrl+F5 to run the application without debugging.
### 3. Docker
<!-- Add the actual docker build and docker run commands. -->

## API Usage
<!-- Add one representative endpoint, request, and response example. -->

## Testing
<!-- Add the actual commands used to build and run the automated tests. -->

## Prerequisites
1. C and C++17-compatible compilers
2. CMake
3. Linux or Windows with Visual Studio 2022
4. Docker, when running the containerized HTTP server

## Limitations and Future Plans
- Only basic algorithms are implemented; more control and numerical features are planned.
- Additional unit tests and documentation are in progress.