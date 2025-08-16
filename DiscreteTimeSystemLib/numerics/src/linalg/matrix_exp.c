#pragma once

#include <stdio.h>
#include <math.h>
#include "matrix_exp.h"
#include "matrix_ops.h"
#include "math_utils.h"

CoreErrorStatus matrix_exp_exponential(const Matrix* A, double t, Matrix* result)
{
    if (A == NULL || result == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (A->rows != A->cols ||
        result->rows != A->rows || result->cols != A->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }

    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    const int n = A->rows;

    Matrix* At = matrix_core_create(n, n, &status);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_SET(status);
        return status;
    }
    Matrix* term = matrix_core_create(n, n, &status);
    if (status != CORE_ERROR_SUCCESS) {
        matrix_core_free(At);
        CORE_ERROR_SET(status);
        return status;
    }
    Matrix* temp = matrix_core_create(n, n, &status);
    if (status != CORE_ERROR_SUCCESS) {
        matrix_core_free(At);
        matrix_core_free(term);
        CORE_ERROR_SET(status);
        return status;
    }

    // At = t * A
    status = matrix_ops_copy(A, At);
    if (status == CORE_ERROR_SUCCESS) {
        status = matrix_ops_scale(At, t);
    }
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    // result <- I
    status = matrix_ops_set_identity(result);
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    // term <- I (first term of the series)
    status = matrix_ops_set_identity(term);
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    const int MAX_ITER = 20;
    for (int k = 1; k <= MAX_ITER; ++k) {
        // temp = term * At  (produces A^k t^k / (k-1)!)
        status = matrix_ops_multiply(term, At, temp);
        if (status != CORE_ERROR_SUCCESS) {
            break;
        }

        // Divide by k to build (At)^k / k!
        status = matrix_ops_scale(temp, 1.0 / (double)k);
        if (status != CORE_ERROR_SUCCESS) {
            break;
        }

        // term = temp
        status = matrix_ops_copy(temp, term);
        if (status != CORE_ERROR_SUCCESS) {
            break;
        }

        // result += term
        status = matrix_ops_axpy(result, 1.0, term);
        if (status != CORE_ERROR_SUCCESS) {
            break;
        }

        // Check convergence using max absolute entry
        double maxval = 0.0;
        for (int i = 0; i < n * n; ++i) {
            double v = fabs(term->data[i]);
            if (v > maxval) maxval = v;
        }
        if (maxval < 1e-12) {
            break;
        }
    }

cleanup:
    matrix_core_free(At);
    matrix_core_free(term);
    matrix_core_free(temp);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_SET(status);
    }
    return status;
}


