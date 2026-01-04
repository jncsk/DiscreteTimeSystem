#include <math.h>

#include "pade_scaling.h"
#include "core_matrix.h"
#include "matrix_norm.h"
#include "matrix_ops.h"
#include <math.h>
#include <float.h>

/* Higham-style theta_m thresholds (double precision) for m = 3,5,7,9,13.
   These are widely used in expm implementations. */
typedef struct {
    int         m;           /* Pade order */
    double  theta;      /* threshold theta_m */
    int         mulcost;  /* rough mul cost to build powers for [m/m] Pade */
} PadeTheta;

static const PadeTheta PADE_THETA[] = {
    /* m      theta_m                                mulcost (approx) */
    {   3,     1.495585217958292e-02,      1                                 },   /* A^2 */
    {   5,     2.539398330063230e-01,      2                                 },   /* A^2, A^4 */
    {   7,     9.504178996162932e-01,      3                                 },   /* A^2, A^4, A^6 */
    {   9,     2.097847961257068e+00,     4                                 },   /* A^2..A^8 */
    {  13,    5.371920351148152e+00,     6                                 },   /* A^2..A^12 (reuse) */
};

static CoreErrorStatus ceil_log2_pos(double x, int* out_result) {
    if (!out_result) {
        return CORE_ERROR_INVALID_ARG;
    }
    if (!(x > 0.0)) {
        return CORE_ERROR_OUT_OF_BOUNDS;
    }

    if (x <= 1.0) {
        *out_result = 0;
        CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
    }

    double v = log(x) / log(2.0);
    int iv = (int)v;
    *out_result = (v == (double)iv) ? iv : iv + 1;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}
CoreErrorStatus pade_choose_scaling_and_order(double anorm, int* out_s, int* out_m) {
    if (!out_s || !out_m) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (isnan(anorm) || anorm < 0.0) {
        return CORE_ERROR_INVALID_ARG; 
    }

    if (anorm == 0.0) {
        *out_s = 0;
        *out_m = 3;  /* any small order works when A == 0 */
        CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
    }

    // Initialize with worst-case (max) values so that the first candidate
    // in the loop will always replace them.
    // This avoids the case where a low initial value prevents updates.
    int best_m = 13;               // Highest supported Pade order
    int best_s = 0x7fffffff;       // Very large scaling count (INT_MAX)
    int best_cost = 0x7fffffff;  // Very large cost (INT_MAX)

    // Get PADE_THETA element count
    const int N = (int)(sizeof(PADE_THETA) / sizeof(PADE_THETA[0]));

    for (int i = 0; i < N; ++i) {
        const int m = PADE_THETA[i].m;
        const double th = PADE_THETA[i].theta;
        const int mc = PADE_THETA[i].mulcost;

        /* minimal s to push anorm/2^s <= theta_m */
        int s = 0;
        if (anorm > th) {
            CoreErrorStatus status = ceil_log2_pos(anorm / th, &s);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
        }
        if (s < 0) s = 0;

        /* rough cost heuristic: more squarings and higher m cost more */
        const int cost = mc + s;

        if (cost < best_cost || (cost == best_cost && (m < best_m))) {
            best_cost = cost;
            best_s = s;
            best_m = m;
        }
    }

    *out_s = best_s;
    *out_m = best_m;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_scale_down_pow2(const Matrix* src, int s, Matrix* dst) {
    if (!src || !dst) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (s < 0) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (src->rows != dst->rows || src->cols != dst->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }
    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // Fast path: no scaling
    if (s == 0) {
        if (dst == src) {
            CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);  // no-op
        }
        status = matrix_ops_copy(dst, src);
        if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);
        CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
    }

    // Prepare destination
    if (dst != src) {
        status = matrix_ops_copy(dst, src);
        if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);
    }

    // Multiply by 2^{-s} using ldexp
    const double factor = ldexp(1.0, -s);  // equals 1.0 * 2^{-s}
    status = matrix_ops_scale(dst, factor);
    if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}
