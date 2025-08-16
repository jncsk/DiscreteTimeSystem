#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "matrix_solve.h"
#include "matrix_ops.h"
#include "core_error.h"

/* ---------- Internal helpers ---------- */

static inline double* row_ptr(Matrix* M, int r) {
    return M->data + (size_t)r * (size_t)M->cols;
}
static inline const double* row_ptr_c(const Matrix* M, int r) {
    return M->data + (size_t)r * (size_t)M->cols;
}

static void swap_rows(Matrix* M, int r1, int r2) {
    if (r1 == r2) return;
    double* a = row_ptr(M, r1);
    double* b = row_ptr(M, r2);
    const int n = M->cols;
    for (int j = 0; j < n; ++j) {
        double t = a[j]; a[j] = b[j]; b[j] = t;
    }
}

/**
 * @brief In-place LU factorization with partial pivoting (A = P * L * U).
 * @param[in,out] A   (n x n) On entry: A. On exit: L (unit diag) & U stored in A.
 * @param[out]    piv (n)     Pivot indices; row i swapped with piv[i].
 */
static CoreErrorStatus lu_decompose_inplace(Matrix* A, int* piv) {
    if (!A || !A->data || !piv) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (A->rows <= 0 || A->cols <= 0 || A->rows != A->cols) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);

    const int n = A->rows;
    for (int i = 0; i < n; ++i) piv[i] = i;

    for (int k = 0; k < n; ++k) {
        /* pivot selection */
        int p = k;
        double amax = fabs(row_ptr(A, k)[k]);
        for (int r = k + 1; r < n; ++r) {
            double v = fabs(row_ptr(A, r)[k]);
            if (v > amax) { amax = v; p = r; }
        }
        if (amax == 0.0) {
            CORE_ERROR_RETURN(CORE_ERROR_NUMERIC); /* singular/near-singular */
        }
        if (p != k) {
            swap_rows(A, p, k);
            int tmp = piv[k]; piv[k] = piv[p]; piv[p] = tmp;
        }

        double* Ak = row_ptr(A, k);
        const double Akk = Ak[k];
        for (int i = k + 1; i < n; ++i) {
            double* Ai = row_ptr(A, i);
            Ai[k] /= Akk;               /* L(i,k) */
            const double Lik = Ai[k];
            for (int j = k + 1; j < n; ++j) {
                Ai[j] -= Lik * Ak[j];   /* U update */
            }
        }
    }
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

static void apply_pivots_to_rhs(Matrix* B, const int* piv) {
    const int n = B->rows;
    for (int k = 0; k < n; ++k) {
        if (piv[k] != k) swap_rows(B, k, piv[k]);
    }
}

static void forward_subst_L(const Matrix* LU, Matrix* B /* becomes Y */) {
    const int n = LU->rows;
    const int nrhs = B->cols;
    for (int i = 0; i < n; ++i) {
        /* subtract L(i,0..i-1)*Y(0..i-1,:) ; L has unit diagonal */
        for (int j = 0; j < i; ++j) {
            const double Lij = row_ptr_c(LU, i)[j];
            if (Lij == 0.0) continue;
            double* Bi = row_ptr(B, i);
            const double* Bj = row_ptr_c(B, j);
            for (int c = 0; c < nrhs; ++c) Bi[c] -= Lij * Bj[c];
        }
    }
}

static CoreErrorStatus back_subst_U(const Matrix* LU, Matrix* Y /* becomes X */) {
    const int n = LU->rows;
    const int nrhs = Y->cols;
    for (int i = n - 1; i >= 0; --i) {
        const double Uii = row_ptr_c(LU, i)[i];
        if (Uii == 0.0) CORE_ERROR_RETURN(CORE_ERROR_NUMERIC);
        double* Xi = row_ptr(Y, i);
        for (int c = 0; c < nrhs; ++c) Xi[c] /= Uii;
        for (int r = 0; r < i; ++r) {
            const double Uri = row_ptr_c(LU, r)[i];
            if (Uri == 0.0) continue;
            double* Xr = row_ptr(Y, r);
            for (int c = 0; c < nrhs; ++c) Xr[c] -= Uri * Xi[c];
        }
    }
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

/* ---------- Public API ---------- */
/**
 * @brief Build the U and V matrices for [m/m] Padé approximation of exp(A).
 * 1. Computes the required even powers of A (A^2, A^4, ..., A^maxp).
 * 2. Uses the given Padé coefficients ( {k even} b, {k odd} b ) to construct:
 */
CoreErrorStatus matrix_solve_LU(const Matrix* A, Matrix* X, const Matrix* B) {
    if (!A || !B || !X || !A->data || !B->data || !X->data) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (A->rows <= 0 || A->cols <= 0) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (A->rows != A->cols) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (B->rows != A->rows || X->rows != A->rows) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (B->cols != X->cols) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);

    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    const int n = A->rows;
    const int nrhs = B->cols; (void)nrhs; /* only to silence unused if not referenced later */

    /* 1) Working copies: LU <- A, X <- B  (matrix_ops_copy(src, dst)) */
    Matrix* LU = matrix_core_create(n, n, &status);
    if (status != CORE_ERROR_SUCCESS) {
        matrix_core_free(LU);
        CORE_ERROR_RETURN(status);
    }
    status = matrix_ops_copy(A, LU);
    if (status != CORE_ERROR_SUCCESS) {
        matrix_core_free(LU); 
        CORE_ERROR_RETURN(status); 
    }
    status = matrix_ops_copy(B, X);
    if (status != CORE_ERROR_SUCCESS) {
        matrix_core_free(LU);
        CORE_ERROR_RETURN(status); 
    }

    /* 2) LU factorization with partial pivoting */
    int* piv = (int*)malloc((size_t)n * sizeof(int));
    if (!piv) { 
        matrix_core_free(LU); 
        CORE_ERROR_RETURN(CORE_ERROR_NOMEM); 
    }

    status = lu_decompose_inplace(LU, piv);
    if (status != CORE_ERROR_SUCCESS) {
        free(piv);
        matrix_core_free(LU); 
        CORE_ERROR_RETURN(status); 
    }

    /* 3) Apply row permutations to RHS: X = P * B */
    apply_pivots_to_rhs(X, piv);

    /* 4) Solve L*Y = X (forward), then U*X = Y (backward) */
    forward_subst_L(LU, X);
    status = back_subst_U(LU, X);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    free(piv);
    status = matrix_core_free(LU);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}
