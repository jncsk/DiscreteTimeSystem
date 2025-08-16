#pragma once

#include "core_matrix.h"
#include <math.h>

#pragma once

/*
 * =============================================================================
 *  pade_scaling.h
 * =============================================================================
 *
 *
 * =============================================================================
 */

#include "core_error.h"

 //------------------------------------------------
//  Macro definitions
//------------------------------------------------
/* None */

//------------------------------------------------
//  Type definitions
//------------------------------------------------



//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Choose scaling s and Pade order m for exp(A*t) via scaling & squaring.
 *
 * Given anorm = ||A||_1 * t (already multiplied by the step t),
 * this picks (s, m) so that anorm / 2^s <= theta_m.
 *
 * Strategy:
 *  - For each candidate m in {3,5,7,9,13}, compute the minimal s_m = ceil(log2(anorm/theta_m)).
 *  - s_m is clipped at 0 (no scaling if already small).
 *  - Choose (m, s_m) that roughly minimizes cost = mult_cost(m) + s_m,
 *    where mult_cost(m) is an approximate number of matrix multiplications
 *    to build the required powers for the [m/m] Pade.
 *
 * @param anorm   L1 norm scaled by step: anorm = ||A||_1 * t  (nonnegative)
 * @param out_s   [out] chosen number of squarings s (>=0)
 * @param out_m   [out] chosen Pade order m in {3,5,7,9,13}
 */
CoreErrorStatus pade_choose_scaling_and_order(double anorm, int* out_s, int* out_m);

/**
 * @brief Convenience wrapper when you have ||A||_1 and step t separately.
 */
static inline void choose_scale_and_order_with_step(double normA1, double t,
    int* out_s, int* out_m) {
    const double anorm = fmax(0.0, normA1) * fmax(0.0, t);
    choose_scale_and_order(anorm, out_s, out_m);
}

CoreErrorStatus matrix_scale_down_pow2(const Matrix* src, int s, Matrix* dst);