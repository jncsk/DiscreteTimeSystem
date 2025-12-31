#pragma once

#include "core_matrix.h"
#include "core_error.h"
#include "state_space.h"

/*
 * =============================================================================
 *  rk.h
 * =============================================================================
 *
 *  Description:
 *      Fixed-step 4th-order Runge–Kutta (RK4) integrators for ODEs.
 *      - Generic RK4 with user-provided vector field callback f(t, x, u, dxdt).
 *      - Linear state-space specialization: x' = A x + B u (ZOH during step).
 *
 *  Notes:
 *      - All functions validate dimensions and NULLs.
 *      - Workspace variants avoid per-call allocations in tight loops.
 * =============================================================================
 */

 //------------------------------------------------
 //  Macro definitions
 //------------------------------------------------
 /* None */

 //------------------------------------------------
 //  Type definitions
 //------------------------------------------------

 /** Generic ODE RHS: dxdt = f(t, x, u). All matrices are column vectors except params. */
typedef CoreErrorStatus(*RkOdeFunc)(
    double t,
    const Matrix* x,   /* n×1 */
    const Matrix* u,   /* m×1 or NULL */
    void* params,      /* user data (nullable) */
    Matrix* dxdt       /* n×1 output */
    );

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief RK4 one-step for a generic ODE: x_{n+1} = RK4(f, t, x_n, u, h).
 *
 * @param[in]  f       ODE function pointer: dxdt = f(t, x, u, params, dxdt)
 * @param[in]  t       Current time
 * @param[in]  x_now   n×1 current state
 * @param[in]  u_now   m×1 input vector (nullable if unused by f)
 * @param[in]  h       Step size (>0)
 * @param[in]  params  Opaque user data passed to f (nullable)
 * @param[out] x_next  n×1 next state
 *
 * @return CORE_ERROR_SUCCESS on success, or error from f / ops.
 */
CoreErrorStatus rk4_step(RkOdeFunc f,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    void* params,
    Matrix* x_next);

/**
 * @brief RK4 one-step with caller-provided workspaces (avoid allocations).
 *
 * Workspaces (all n×1): k1, k2, k3, k4, tmp.
 */
CoreErrorStatus rk4_step_ws(RkOdeFunc f,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    void* params,
    Matrix* x_next,
    Matrix* k1, Matrix* k2, Matrix* k3, Matrix* k4,
    Matrix* tmp);

/**
 * @brief Linear state-space specialization (ZOH during the step):
 *        x' = A x + B u, with constant u on [t, t+h].
 *
 * @param[in]  A      n×n
 * @param[in]  B      n×m (nullable if m==0)
 * @param[in]  t      Current time (not used mathematically; for symmetry)
 * @param[in]  x_now  n×1
 * @param[in]  u_now  m×1 (nullable if m==0)
 * @param[in]  h      Step size (>0)
 * @param[out] x_next n×1
 */
CoreErrorStatus rk4_lin_step(const Matrix* A,
    const Matrix* B,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    Matrix* x_next);

/**
 * @brief Workspace variant for linear case (no allocations).
 *        Workspaces: k1..k4 (n×1), tmp (n×1), Ax (n×1), Bu (n×1).
 */
CoreErrorStatus rk4_lin_step_ws(const Matrix* A,
    const Matrix* B,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    Matrix* x_next,
    Matrix* k1, Matrix* k2, Matrix* k3, Matrix* k4,
    Matrix* tmp, Matrix* Ax, Matrix* Bu);
