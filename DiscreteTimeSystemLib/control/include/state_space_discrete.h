#pragma once

#include "core_matrix.h"
#include "core_error.h"
#include "state_space.h"
#include "state_space_c2d.h"

/*
 * =============================================================================
 *  ss_discrete.h
 * =============================================================================
 *
 *  Description:
 *      ZOH-discretized state-space model (model only; no internal state).
 *      Provides constructors from a continuous-time model (c2d) or from
 *      already-discretized matrices, and utilities for one-step state update
 *      and output computation.
 *
 *  Notes:
 *      - This module stores (Ad, Bd, C, D) and sampling time Ts.
 *      - The state vector x and the input vector u are NOT stored; callers
 *        pass them to the step functions. This keeps the model reusable.
 *      - For performance-critical loops, use the workspace variant
 *        ss_discrete_step_ws() to avoid per-step allocations.
 *
 * =============================================================================
 */

 //------------------------------------------------
 //  Macro definitions
 //------------------------------------------------
 /* None */

 //------------------------------------------------
 //  Type definitions
 //------------------------------------------------

 /**
  * @brief ZOH-discretized state-space model (model only; no internal state).
  *
  * Members:
  *   n, m, p : dimensions (A: nÅ~n, B: nÅ~m, C: pÅ~n, D: pÅ~m)
  *   Ts      : sampling time (seconds)
  *   Ad, Bd  : discrete-time state and input matrices
  *   C, D    : output matrices (optional). If D is NULL, it is treated as zero.
  */
typedef struct {
    int n, m, p;
    double Ts;
    Matrix* Ad;  ///< n x n
    Matrix* Bd;  ///< n x m
    Matrix* C;   ///< p x n (optional; may be NULL)
    Matrix* D;   ///< p x m (optional; may be NULL; treated as zero if NULL)
} SSDiscrete;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Build a discrete model from a continuous-time system via ZOH c2d.
 *
 * Given a continuous-time model (A,B,C,D) and sampling time Ts, computes
 * (Ad,Bd) using state_space_c2d(), and deep-copies C,D if present (else NULL).
 *
 * @param[out] out  Destination SSDiscrete (will allocate Ad,Bd and optionally C,D).
 * @param[in]  sys  Continuous-time system (A: nÅ~n, B: nÅ~m). C,D may be NULL.
 * @param[in]  Ts   Sampling period (seconds). Ts >= 0.
 *
 * @return CORE_ERROR_SUCCESS on success
 * @return CORE_ERROR_NULL if pointers are NULL
 * @return CORE_ERROR_INVALID_ARG if Ts < 0
 * @return Error codes propagated from underlying routines
 */
CoreErrorStatus ss_discrete_init_from_csys(SSDiscrete* out,
    const StateSpaceModel* sys,
    double Ts);

/**
 * @brief Build a discrete model from given matrices (deep copy). C, D may be NULL.
 *
 * @param[out] out  Destination SSDiscrete (allocates and copies Ad,Bd,C,D).
 * @param[in]  Ts   Sampling period (seconds). Ts >= 0.
 * @param[in]  Ad   nÅ~n
 * @param[in]  Bd   nÅ~m
 * @param[in]  C    pÅ~n (nullable)
 * @param[in]  D    pÅ~m (nullable)
 *
 * @return CORE_ERROR_SUCCESS on success
 * @return CORE_ERROR_NULL / CORE_ERROR_DIMENSION on invalid inputs
 */
CoreErrorStatus ss_discrete_init_from_mats(SSDiscrete* out,
    double Ts,
    const Matrix* Ad,
    const Matrix* Bd,
    const Matrix* C,   /* nullable */
    const Matrix* D);  /* nullable */

/**
 * @brief Free internal matrices (Ad,Bd,C,D) and zero-out the struct.
 *
 * @param[in,out] out  Model to be cleared.
 * @return CORE_ERROR_SUCCESS on success, CORE_ERROR_NULL if out is NULL.
 */
CoreErrorStatus ss_discrete_free(SSDiscrete* out);

/**
 * @brief One-step update: x_next = Ad * x_now + Bd * u_now.
 *
 * There are two variants:
 *
 * - ss_discrete_step():
 *     Allocates temporary matrices (Ax, Bu) internally on each call.
 *     Easy to use, but less efficient in long simulation loops
 *     because allocation/free occurs at every step.
 *
 * - ss_discrete_step_ws():
 *     Workspace-based version. The caller provides pre-allocated
 *     scratch matrices (Ax_ws, Bu_ws). This avoids dynamic allocations
 *     inside the loop and is recommended for performance-critical
 *     or real-time simulations.
 *
 * @param[in]  dsys    Discrete model (Ad,Bd,Ts).
 * @param[in]  x_now   nÅ~1 current state.
 * @param[in]  u_now   mÅ~1 current input.
 * @param[out] x_next  nÅ~1 next state.
 *
 * @return CORE_ERROR_SUCCESS on success, or an error code.
 */
CoreErrorStatus ss_discrete_step(const SSDiscrete* dsys,
    const Matrix* x_now,
    const Matrix* u_now,
    Matrix* x_next);

/**
 * @brief One-step update with caller-provided workspaces (no per-call allocation).
 *
 * @see ss_discrete_step() for the semantics. This variant avoids dynamic
 *      allocations by reusing caller-provided Ax_ws and Bu_ws.
 *
 * @param[in]  dsys    Discrete model (Ad,Bd,Ts).
 * @param[in]  x_now   nÅ~1 current state.
 * @param[in]  u_now   mÅ~1 current input.
 * @param[out] x_next  nÅ~1 next state.
 * @param[in]  Ax_ws   nÅ~1 workspace for Ad*x_now.
 * @param[in]  Bu_ws   nÅ~1 workspace for Bd*u_now.
 *
 * @return CORE_ERROR_SUCCESS on success, or an error code.
 */
CoreErrorStatus ss_discrete_step_ws(const SSDiscrete* dsys,
    const Matrix* x_now,
    const Matrix* u_now,
    Matrix* x_next,
    Matrix* Ax_ws,
    Matrix* Bu_ws);

/**
 * @brief Convenience for scalar input (m == 1):
 *        x_next = Ad * x_now + u_now * Bd(:,0)
 *
 * @param[in]  dsys    Discrete model with m == 1.
 * @param[in]  x_now   nÅ~1 current state.
 * @param[in]  u_now   scalar input value.
 * @param[out] x_next  nÅ~1 next state.
 *
 * @return CORE_ERROR_SUCCESS on success,
 *         CORE_ERROR_INVALID_ARG if m != 1,
 *         or other error codes from underlying ops.
 */
CoreErrorStatus ss_discrete_step_scalar_u(const SSDiscrete* dsys,
    const Matrix* x_now,
    double u_now,
    Matrix* x_next);

/**
 * @brief Output equation: y = C x + D u.
 *
 * If D is NULL, it is treated as the zero matrix (i.e., y = C x).
 *
 * @param[in]  dsys   Discrete model (C required; D optional).
 * @param[in]  x_now  nÅ~1 state.
 * @param[in]  u_now  mÅ~1 input.
 * @param[out] y_out  pÅ~1 output.
 *
 * @return CORE_ERROR_SUCCESS on success
 * @return CORE_ERROR_NULL if pointers are NULL
 * @return CORE_ERROR_INVALID_ARG if C is missing
 * @return CORE_ERROR_DIMENSION on size mismatch
 */
CoreErrorStatus ss_discrete_output(const SSDiscrete* dsys,
    const Matrix* x_now,
    const Matrix* u_now,
    Matrix* y_out);
