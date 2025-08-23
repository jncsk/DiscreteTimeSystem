#pragma once

#include "matrix_exp.h"
#include "matrix_ops.h"
#include "pade.h"

CoreErrorStatus matrix_exp_exponential(const Matrix* A, double t, Matrix* result)
{
    if (!A || !result) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (A->rows != A->cols || result->rows != A->rows || result->cols != A->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }

    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    Matrix* At = matrix_core_create(A->rows, A->cols, &status);
    if (status) CORE_ERROR_RETURN(status);

    status = matrix_ops_copy(At, A);  if (status) goto cleanup;
    status = matrix_ops_scale(At, t);   if (status) goto cleanup;
    status = pade_expm(At, result);    if (status) goto cleanup;

cleanup:
    if (At) matrix_core_free(At);
    CORE_ERROR_RETURN(status);
}


