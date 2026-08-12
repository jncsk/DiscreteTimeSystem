# Project Architecture Overview

This document explains the layered architecture design philosophy used in this project.
The aim is to maintain **high cohesion** within modules and **low coupling** between layers,
ensuring maintainability, clarity, and scalability.

---

## Layered Architecture

```
Application Layer
       ↓
Control Layer
       ↓
Numerics Layer
       ↓
Core Layer
```

### 1. Application Layer
- **Responsibility**: Provides the entry points and user-facing interface of the project.
- **Examples**:
  - CLI / GUI main programs
  - Configuration file parsing
- **Dependencies**:
  - Calls into the Control Layer.
  - Should not directly depend on Core Layer internals.

---

### 2. Control Layer
- **Responsibility**: Coordinates domain-specific logic, orchestrating the use of numerics and core utilities to perform high-level tasks.
- **Examples**:
  - Control simulations
  - State-space model integration
- **Dependencies**:
  - Calls into the Numerics Layer for calculations.
  - Should not directly depend on Core Layer internals.

---

### 3. Numerics Layer
- **Responsibility**: Implements reusable numerical algorithms and data processing logic.
- **Examples**:
  - `matrix_ops.*` — matrix arithmetic (add, multiply, inverse)
  - `math_utils.*` — mathematical helper functions (factorial, power functions)
  - `bit_utils.*` — bit-level data manipulation
- **Notes**:
  - Does not manage low-level memory allocation for data structures — relies on Core Layer for that.
  - Contains the actual *algorithms* and computation logic.

---

### 4. Core Layer
- **Responsibility**: Provides low-level building blocks and essential data structures.
- **Examples**:
  - `matrix_core.*` — defines the `Matrix` structure and functions for creation, destruction, and size validation.
  - `core_error.*` — error handling system.
- **Notes**:
  - Independent from any higher-level algorithms.
  - Should be minimal and stable.
  - Serves as the "foundation" that all other layers build on.

---

## Example File Placement

```
/DiscreteTimeSystem
├── DiscreteTimeSystemLib/
│   ├── app/                                           # Application Layer
│   │   └── src/
│   │       └── main.c
│   ├── control/                                       # Control Layer
│   │   ├── include/
│   │   │   └── state_space.h
│   │   └── src/
│   │       └── state_space.c
│   ├── numerics/                                      # Numerics Layer
│   │   ├── include/
│   │   │   ├── bit_utils.h
│   │   │   ├── linalg/
│   │   │   ├── math_utils.h
│   │   │   └── pade/
│   │   └── src/
│   │       └── ...
│   └── core/                                          # Core Layer
│       ├── include/
│       │   ├── core_error.h
│       │   └── core_matrix.h
│       └── src/
│           └── ...
├── DiscreteTimeSystemApp/
│   └── main.c                                         # Example application
└── UnitTest/
    └── tests/                                         # Unit tests
        ├── core/
        ├── control/
        ├── numerics/
        └── main.cpp
```

---

## Benefits of This Structure

1. **Maintainability** — Clear separation of responsibilities.
2. **Reusability** — Core and Numerics layers can be reused by multiple applications.
3. **Testability** — Each layer can be tested independently.
4. **Scalability** — Adding new algorithms or applications requires minimal changes to other layers.

---

## Guidelines
- Keep Core Layer free from business logic.
- Avoid cyclic dependencies between layers.
- Prefer adding new algorithms to the Computation / Numerics Layer rather than Core.
