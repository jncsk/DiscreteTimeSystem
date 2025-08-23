#pragma once
#include "core_error.h"
#include "core_matrix.h"
#include "state_space.h"      // continuous model (A,B,C,D)
#include "state_space_c2d.h"  // c2d (ZOH)

#ifdef __cplusplus
extern "C" {
#endif

    /** ZOH-discretized state-space model */
    typedef struct {
        int n, m, p;
        double Ts;
        Matrix* Ad;   // n x n
        Matrix* Bd;   // n x m
        Matrix* C;    // p x n (optional; may be NULL)
        Matrix* D;    // p x m (optional; may be NULL)
    } SSDiscrete;

    /** Build from continuous model (ZOH c2d). Deep-copies C,D if present (else NULL). */
    CoreErrorStatus ss_discrete_init_from_csys(SSDiscrete* out,
        const StateSpaceModel* sys,
        double Ts);

    /** Build from matrices (deep copy). C,D may be NULL. */
    CoreErrorStatus ss_discrete_init_from_mats(SSDiscrete* out,
        double Ts,
        const Matrix* Ad,
        const Matrix* Bd,
        const Matrix* C,   /* nullable */
        const Matrix* D);  /* nullable */

    /** Free internal matrices and zero-out the struct. */
    CoreErrorStatus ss_discrete_free(SSDiscrete* out);

    /** One-step update: x_next = Ad x_now + Bd u_now  (allocates small temporaries internally) */
    CoreErrorStatus ss_discrete_step(const SSDiscrete* dsys,
        const Matrix* x_now,   // (n x 1)
        const Matrix* u_now,   // (m x 1)
        Matrix* x_next);       // (n x 1)

    /** Faster version with caller-provided workspaces (no allocation in the loop). */
    CoreErrorStatus ss_discrete_step_ws(const SSDiscrete* dsys,
        const Matrix* x_now,     // (n x 1)
        const Matrix* u_now,     // (m x 1)
        Matrix* x_next,          // (n x 1)
        Matrix* Ax_ws,           // (n x 1)
        Matrix* Bu_ws);          // (n x 1)

    /** Convenience for scalar input (m==1): x_next = Ad x_now + u*Bd(:,0) */
    CoreErrorStatus ss_discrete_step_scalar_u(const SSDiscrete* dsys,
        const Matrix* x_now,
        double u_now,
        Matrix* x_next);

    /** Output: y = C x + D u  (requires C and D; returns INVALID_ARG if missing) */
    CoreErrorStatus ss_discrete_output(const SSDiscrete* dsys,
        const Matrix* x_now,   // (n x 1)
        const Matrix* u_now,   // (m x 1)
        Matrix* y_out);        // (p x 1)

#ifdef __cplusplus
}
#endif
