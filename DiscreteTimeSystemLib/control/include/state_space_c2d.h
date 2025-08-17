#pragma once

#include "core_matrix.h"
#include "core_error.h"
#include "state_space.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Continuous-to-Discrete (ZOH) exact discretization via block matrix exponential.
     *
     * Given a continuous-time state-space model:
     *   x_dot = A x + B u,
     *   y     = C x + D u,
     * this computes discrete-time (A_d, B_d) under Zero-Order Hold with sample time Ts:
     *   A_d = exp(A * Ts),
     *   B_d = ÅÁ_0^Ts exp(A É—) B dÉ—
     *
     * Implementation detail:
     *   Build M = [[A, B], [0, 0]] of size (n+m)Å~(n+m), scale by Ts, compute E = exp(M),
     *   then extract:
     *     A_d = E(0:n-1, 0:n-1),
     *     B_d = E(0:n-1, n:n+m-1).
     *
     * @param[in]  sys  Continuous-time system (A: nÅ~n, B: nÅ~m). C,DÇÕñ¢égópÅB
     * @param[in]  Ts   Sampling period. If Ts == 0, returns A_d=I, B_d=0.
     * @param[out] Ad   Pre-allocated nÅ~n matrix to receive A_d.
     * @param[out] Bd   Pre-allocated nÅ~m matrix to receive B_d.
     *
     * @return CORE_ERROR_SUCCESS on success
     * @return CORE_ERROR_NULL if any pointer is NULL
     * @return CORE_ERROR_DIMENSION if matrix sizes are inconsistent
     * @return CORE_ERROR_INVALID_ARG if Ts < 0
     * @return Error propagated from underlying routines (e.g., matrix ops, pade_expm)
     *
     * @note
     * - This routine allocates temporaries internally and frees them before return.
     * - Requires a working pade_expm(M, E). For Ts==0, no exponential is required.
     */
    CoreErrorStatus state_space_c2d(const StateSpaceModel* sys,
        double Ts,
        Matrix* Ad,
        Matrix* Bd);

#ifdef __cplusplus
}
#endif
