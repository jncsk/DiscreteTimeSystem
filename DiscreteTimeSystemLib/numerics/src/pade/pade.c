
#include "pade.h"
#include "pade_scaling.h"
#include "pade_exp_coeffs.h"
#include "core_matrix.h"
#include "core_error.h"
#include "matrix_norm.h"
#include "matrix_ops.h"
#include "matrix_solve.h"

typedef struct {
    Matrix* A2;   // may be NULL if not requested
    Matrix* A4;
    Matrix* A6;
    Matrix* A8;
    Matrix* A10;
    Matrix* A12;
} EvenPowers;

/**
 * @brief Precompute and store even powers of matrix A up to max_power.
 *
 * This function allocates and fills matrices for A^2, A^4, ..., A^{max_power}.
 * The computed powers are stored in the EvenPowers structure `P` for later
 * reuse when building the U and V matrices in the Padé approximation.
 *
 * @param[in]  A          Pointer to the input matrix A (n x n).
 * @param[in]  max_power  Maximum even exponent to compute (mustatusbe positive and even).
 * @param[out] P          Pointer to an EvenPowers struct to store computed powers.
 *                        Any unneeded members may remain NULL.
 *
 * @return CORE_ERROR_SUCCESS on success, or an error code on failure
 *         (e.g., allocation failure, invalid argument).
 *
 * @note
 * - The function will allocate matrices inside `P` only for the powers
 *   up to `max_power`.
 * - Caller mustatuscall `free_even_powers(&P)` after use to release memory.
 * - This is an internal helper and should not be declared in a header file.
 */
static CoreErrorStatus build_even_powers(const Matrix* A, int max_power, EvenPowers* P) {
    if (!A || !P) {
        return CORE_ERROR_INVALID_ARG;
    }

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // Allocate all elements of EvenPowers
#define MAKE(name) \
    do { \
        if (max_power >= name) { \
            P->A##name = matrix_core_create(A->rows, A->cols, &status); \
            if (status) return status; \
        } \
    } while (0)

    P->A2 = P->A4 = P->A6 = P->A8 = P->A10 = P->A12 = NULL;
    MAKE(2); MAKE(4); MAKE(6); MAKE(8); MAKE(10); MAKE(12);
#undef MAKE

    // A2 = A*A
    if (max_power >= 2) {
        status = matrix_ops_multiply(A, A, P->A2); if (status) return status;
    }
    // A4 = A2*A2
    if (max_power >= 4) {
        status = matrix_ops_multiply(P->A2, P->A2, P->A4); if (status) return status;
    }
    // A6 = A4*A2
    if (max_power >= 6) {
        status = matrix_ops_multiply(P->A4, P->A2, P->A6); if (status) return status;
    }
    // A8 = A4*A4
    if (max_power >= 8) {
        status = matrix_ops_multiply(P->A4, P->A4, P->A8); if (status) return status;
    }
    // A10 = A8*A2
    if (max_power >= 10) {
        status = matrix_ops_multiply(P->A8, P->A2, P->A10); if (status) return status;
    }
    // A12 = A6*A6
    if (max_power >= 12) {
        status = matrix_ops_multiply(P->A6, P->A6, P->A12); if (status) return status;
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

/**
 * @brief Free all allocated matrices in an EvenPowers structure.
 *
 * This function releases memory for each non-NULL matrix pointer in `P`
 * (e.g., A2, A4, A6, ...) and sets the pointers to NULL.
 *
 * @param[in,out] P  Pointer to an EvenPowers structure whose members will be freed.
 *                   If P is NULL, the function does nothing.
 *
 * @return CORE_ERROR_SUCCESS always.
 *
 * @note
 * - This function does not free the EvenPowers struct itself, only the matrices it holds.
 * - Safe to call multiple times; calling on already-freed members has no effect.
 * - Intended for internal use (should be declared static in the `.c` file).
 */
static CoreErrorStatus free_even_powers(EvenPowers* P) {
    if (!P) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (P->A2)  matrix_core_free(P->A2);
    if (P->A4)  matrix_core_free(P->A4);
    if (P->A6)  matrix_core_free(P->A6);
    if (P->A8)  matrix_core_free(P->A8);
    if (P->A10) matrix_core_free(P->A10);
    if (P->A12) matrix_core_free(P->A12);
    *P = (EvenPowers){ 0 };
}

/**
 * @brief Build the U and V matrices for the [m/m] Padé approximation.
 *
 * This function constructs the matrices:
 *   - V = c0 * I + c2 * A^2 + c4 * A^4 + ...
 *   - U = c1 * A + c3 * A^3 + c5 * A^5 + ...
 * using the given Padé coefficients and precomputed even powers of A.
 *
 * @param[in]  A         Pointer to the base matrix A.
 * @param[in]  b_even    Array of coefficients for even powers (length = even_len).
 * @param[in]  even_len  Number of coefficients in b_even.
 * @param[in]  b_odd     Array of coefficients for odd powers (length = odd_len).
 * @param[in]  odd_len   Number of coefficients in b_odd.
 * @param[in]  P         Pointer to EvenPowers containing precomputed A^2, A^4, ... up to required max.
 * @param[out] U         Output matrix to hold odd-term sum.
 * @param[out] V         Output matrix to hold even-term sum.
 * @param[out] tmpS      Temporary scratch matrix for intermediate multiplications.
 * @param[in]  I         Identity matrix of same dimension as A.
 *
 * @return CORE_ERROR_SUCCESS if successful, otherwise an error code.
 * 
 * **Steps:**
 * 1. Build V = c0 * I + c2 * A^2 + c4 * A^4 + ...
 *     1.1. Calculate V = c0 * I
 *     1.2. Calculate V = c0 * I + c2 * A^2 + c4 * A^4 +  ...
 * 2. Build U = c1 * A^1 + c3 * A^3 + c5 * A^5 + ...
 *     2.1. Initialize U = c1 * A.
 *     2.2. V = c1 * I + c3 * A^2 + c5 * A^4 + ... 
 *     2.3. V = A (c1 * I + c3 * A^2 + c5 * A^4 + ...) = c1 * A^1 + c3 * A^3 + c5 * A^5 + ...
 * 
 * @note
 * - Assumes all matrices are correctly allocated and sized before calling.
 * - Does not allocate new matrices; it only writes into provided ones.
 * - Intended for internal use within Padé exponential calculation.
 */
static CoreErrorStatus build_UV_with_powers(
    const Matrix* A,
    const double* b_even, int even_len,
    const double* b_odd, int odd_len,
    const EvenPowers* P,
    Matrix* U, Matrix* V, Matrix* tmpS, Matrix* I)
{
    if (!A || !b_even || !b_odd || !P || !U || !V || !tmpS || !I) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (even_len <= 0 || odd_len < 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    CoreErrorStatus status;

    // Map index j -> precomputed matrix A^(2j)
    // Index 0 is unused (NULL), since j=0 corresponds to I (identity matrix),
    // which is handled separately before this loop.
    // The restatusof the entries are pointers to precomputed even powers of A
    // stored in the EvenPowers struct (P). 
    Matrix* A2k_list[] = {
    NULL,        // j=0 : corresponds to A^0 = I (already handled)
    P->A2,       // j=1 : A^2
    P->A4,       // j=2 : A^4
    P->A6,       // j=3 : A^6
    P->A8,       // j=4 : A^8
    P->A10,      // j=5 : A^10
    P->A12       // j=6 : A^12
    };
    // Maximum valid index for accessing A2k_list[]
    // (subtract 1 because index 0 is always NULL)
    const int max_precomputed_power_index =
        (int)(sizeof(A2k_list) / sizeof(A2k_list[0])) - 1;

    // --- Step 1.1 : V = c0 * I ---
    status = matrix_ops_set_identity(I);
    if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

    status = matrix_ops_copy(I, V);
    if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

    status = matrix_ops_scale(V, b_even[0]);     
    if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

    // ---Step 1.2: V = c0 * I + c2 * A^2 + c4 * A^4 + ... ---
    for (int j = 1; j < even_len; ++j) {
        if (j > max_precomputed_power_index || !A2k_list[j]) {
            CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG); // not enough precomputed powers
        }
        status = matrix_ops_axpy(V, b_even[j], A2k_list[j]);  // V += b_even[j] * A^{2j}
        if (status != CORE_ERROR_SUCCESS) {
            CORE_ERROR_RETURN(status);
        }
    }

    // --- Step 2: build inner odd polynomial S = b1*I + b3*A^2 + b5*A^4 + ... ---
    if (odd_len > 0) {
        // --- Step 2.1 : tmpS = c1 * I ---
        status = matrix_ops_copy(I, tmpS);
        if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

        status = matrix_ops_scale(tmpS, b_odd[0]);
        if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

        // ---Step 2.2: U = c1 * I + c3 * A^2 + c5 * A^4 + ... ---
        for (int j = 1; j < odd_len; ++j) {
            if (j > max_precomputed_power_index || !A2k_list[j]) {
                CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
            }
            status = matrix_ops_axpy(tmpS, b_odd[j], A2k_list[j]); // tmpS += b_odd[j] * A^{2j}
            if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);
        }
    }
    else {
        // No odd coefficients: tmpS = 0
        status = matrix_ops_set_zero(tmpS);
        if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);
    }

    // ---Step 2.3: V = A (c1 * I + c3 * A^2 + c5 * A^4 + ...) ---
    status = matrix_ops_multiply(A, tmpS, U);
    if (status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

/**
 * @brief Get the maximum even power of A required for the given Pade order.
 *
 * This function returns the highestatuseven exponent needed when building
 * the polynomial terms \( U \) and \( V \) for a given [m/m] Pade approximation.
 * The value corresponds to the largestatus\( A^{2k} \) that mustatusbe precomputed
 * in `build_even_powers()` before constructing the approximation.
 *
 * @param m   Pade order (mustatusbe one of 3, 5, 7, 9, or 13)
 * @return    The maximum even power required (2, 4, 6, 8, or 12),
 *            or -1 if the order is unsupported.
 *
 * @note This is a `static` internal helper; it should not be placed in a header.
 */
static int max_even_power_for_m(int m) {
    switch (m) {
    case 3:  return 2;
    case 5:  return 4;
    case 7:  return 6;
    case 9:  return 8;
    case 13: return 12;
    default: return -1;
    }
}

/**
 * @brief Build the U and V matrices for [m/m] Padé approximation of exp(A).
 * 1. Computes the required even powers of A (A^2, A^4, ..., A^maxp).
 * 2. Uses the given Padé coefficients ( {k even} b, {k odd} b ) to construct:
 *    - U = sum_{k odd} b_k * A^k
 *    - V = sum_{k even} b_k * A^k
 *    where k ≤ 2m.
 *
 * @param[in]  A         Pointer to the input matrix A.
 * @param[in]  m         Padé approximation order (supported: 3, 5, 7, 9, 13).
 * @param[in]  b_even    Coefficient array for even powers (c_0, c_2, ..., c_{2m}).
 * @param[in]  even_len  Number of elements in b_even (should be m+1).
 * @param[in]  b_odd     Coefficient array for odd powers (c_1, c_3, ..., c_{2m-1}).
 * @param[in]  odd_len   Number of elements in b_odd (should be m).
 * @param[out] U         Output matrix to store U part of the Padé approximation.
 * @param[out] V         Output matrix to store V part of the Padé approximation.
 * @param[in,out] tmpS   Scratch matrix (same size as A) for temporary calculations.
 * @param[in]  I         Identity matrix of the same size as A (can be reused).
 *
 * @return CORE_ERROR_SUCCESS on success, otherwise an appropriate error code.
 *
 * @note The EvenPowers struct is created internally and freed before returning.
 */
static CoreErrorStatus build_UV_for_m(
    const Matrix* A, int m,
    const double* b_even, int even_len,
    const double* b_odd, int odd_len,
    Matrix* U, Matrix* V,
    Matrix* tmpS, Matrix* I)
{
    CoreErrorStatus status;
    EvenPowers P = { 0 };

    // Determine the maximum even power (A^(2k)) required for the given Pade order `m`.
    // This tells us up to which power of A we need to precompute (e.g., m=13 → A^12).
    // If `m` is unsupported, max_even_power_for_m() returns a negative value, so we treat it as invalid.
    const int maxp = max_even_power_for_m(m);
    if (maxp < 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    // Precompute the required even powers of matrix A up to A^maxp.
    // Example: if maxp = 4, this generates A^2 and A^4.
    // The results are stored in the EvenPowers struct `P` for reuse
    // in building the Padé U and V matrices (avoids repeated multiplications).
    status = build_even_powers(A, maxp, &P);
    if (status != CORE_ERROR_SUCCESS) {
        free_even_powers(&P); 
        CORE_ERROR_RETURN(status);
    }

    // Construct U and V using 
    // 1. The precomputed even powers of A stored in `P` (A^2, A^4, ...)
    // 2. The Padé coefficients defined in pade_exp_coeffs.h (`b_even`, `b_odd`)
    // This step combines the coefficients with the precomputed powers
    // to form the final U and V matrices for the [m/m] Padé approximation.
    status = build_UV_with_powers(A, b_even, even_len, b_odd, odd_len, &P, U, V, tmpS, I);

    free_even_powers(&P);
    return status;
}

CoreErrorStatus pade_expm(const Matrix* A, Matrix* result) {
    if (!A || !result) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }

    Matrix* As = NULL;
    Matrix* U = NULL;
    Matrix* V = NULL;
    Matrix* tempS = NULL;
    Matrix* I = NULL;
    Matrix* VminusU = NULL;
    Matrix* VplusU = NULL;
    Matrix* Tmp = NULL;
    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // Calculate norm
    double anorm = 0;
    status = matrix_norm_1(A, &anorm);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    // Determine the order of pade and the number of scaling
    int order = 0;
    int scale = 0;
    pade_choose_scaling_and_order(anorm, &scale, &order);

    // Scale A with the scaling value.
    As = matrix_core_create(A->rows, A->cols, &status);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    status = matrix_scale_down_pow2(A, scale, As);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    // Get coefficients of each, od
    const PadeExpTable* PadeCoeffs = pade_exp_get_table(order);

    U = matrix_core_create(As->rows, As->cols, &status);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    V = matrix_core_create(As->rows, As->cols, &status);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    tempS = matrix_core_create(As->rows, As->cols, &status);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    I = matrix_core_create(As->rows, As->cols, &status);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    status = build_UV_for_m(As, order,
        PadeCoeffs->even, PadeCoeffs->even_len,
        PadeCoeffs->odd, PadeCoeffs->odd_len,
        U, V, tempS, I);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    /* ---------- 3) Form (V - U) and (V + U) ---------- */
    VminusU = matrix_core_create(As->rows, As->cols, &status);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    VplusU = matrix_core_create(As->rows, As->cols, &status); 
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    /* VminusU = V - U */
    status = matrix_ops_copy(V, VminusU); 
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }
    status = matrix_ops_axpy(VminusU, -1.0, U);
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    /* VplusU = V + U */
    status = matrix_ops_copy(V, VplusU); 
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }
    status = matrix_ops_axpy(VplusU, 1.0, U);  
    if (status) { CORE_ERROR_SET(status); goto CLEANUP_EARLY; }

    /* ---------- 4) Solve (V - U) * X = (V + U)  via LU (no inverse) ---------- */
    /* result: caller-provided buffer (n x n). We solve into it directly. */
    status = matrix_solve_LU(VminusU, result, VplusU);
    if (status) {
        CORE_ERROR_SET(status);
        goto CLEANUP_EARLY;
    }

    /* ---------- 5) Undo scaling by repeated squaring: X <- X^(2^s) ----------
       Scaling-and-squaring: exp(A) = (exp(As))^(2^s)
       Do s times: result = result * result
    ------------------------------------------------------------------------ */
    if (scale > 0) {
        Matrix* Tmp = matrix_core_create(result->rows, result->cols, &status);
        if (!Tmp || status) {
            status = status ? status : CORE_ERROR_NULL;
            goto CLEANUP_EARLY;
        }

        for (int i = 0; i < scale; ++i) {
            /* Tmp = result * result  */
            status = matrix_ops_multiply(result, result, Tmp);
            if (status) { matrix_core_free(Tmp); goto CLEANUP_EARLY; }

            /* result <- Tmp */
            status = matrix_ops_copy(Tmp, result);
            if (status) { matrix_core_free(Tmp); goto CLEANUP_EARLY; }
        }

        matrix_core_free(Tmp);
    }

    /* ---------- 6) Success / cleanup ---------- */
    matrix_core_free(As);
    matrix_core_free(U);
    matrix_core_free(V);
    matrix_core_free(tempS);
    matrix_core_free(I);
    matrix_core_free(VminusU);
    matrix_core_free(VplusU);
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);

CLEANUP_EARLY:
    if (As)      matrix_core_free(As);
    if (U)       matrix_core_free(U);
    if (V)       matrix_core_free(V);
    if (tempS)    matrix_core_free(tempS);
    if (I)       matrix_core_free(I);
    if (VminusU) matrix_core_free(VminusU);
    if (VplusU)  matrix_core_free(VplusU);
    return status;
}

