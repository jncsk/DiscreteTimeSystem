#include "matrix_norm.h"
#include "matrix_ops.h"

CoreErrorStatus matrix_norm_1(const Matrix* mat, double* result) {
    if (mat == NULL || result == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows <= 0 || mat->cols <= 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    CoreErrorStatus status;
    double max_col_sum = 0.0;

    for (int j = 0; j < mat->cols; j++) {
        double col_sum = 0.0;
        for (int i = 0; i < mat->rows; i++) {
            double val = matrix_ops_get(mat, i, j, &status);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
            col_sum += fabs(val);
        }
        if (j == 0 || col_sum > max_col_sum) {
            max_col_sum = col_sum;
        }
    }

    *result = max_col_sum;
    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus matrix_norm_inf(const Matrix* mat, double* result) {
    if (!mat || !result) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows <= 0 || mat->cols <= 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    double max_row_sum = 0.0;
    for (int i = 0; i < mat->rows; i++) {
        double row_sum = 0.0;
        for (int j = 0; j < mat->cols; j++) {
            CoreErrorStatus status;
            double val = matrix_ops_get(mat, i, j, &status);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
            row_sum += fabs(val);
        }
        if (i == 0 || row_sum > max_row_sum) {
            max_row_sum = row_sum;
        }
    }
    *result = max_row_sum;
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_norm_fro(const Matrix* mat, double* result) {
    if (!mat || !result) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows <= 0 || mat->cols <= 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    double sum_sq = 0.0;
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            CoreErrorStatus status;
            double val = matrix_ops_get(mat, i, j, &status);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
            sum_sq += val * val;
        }
    }
    *result = sqrt(sum_sq);
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}
