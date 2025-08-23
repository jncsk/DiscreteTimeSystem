#pragma once

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "matrix_ops.h"
#include "bit_utils.h"
#include "core_matrix.h"

CoreErrorStatus matrix_ops_fill(Matrix* mat, double value) {
    if (mat == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }

    int size = mat->rows * mat->cols;

    for (int i = 0; i < size; ++i) {
        mat->data[i] = value;
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_set(Matrix* mat, int i, int j, double value) {
    if (mat == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (i < 0 || i >= mat->rows) {
        CORE_ERROR_RETURN(CORE_ERROR_OUT_OF_BOUNDS);
    }
    if (j < 0 || j >= mat->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_OUT_OF_BOUNDS);
    }

    mat->data[i * mat->cols + j] = value;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

double matrix_ops_get(const Matrix* mat, int i, int j, CoreErrorStatus* err)
{
    if (mat == NULL) {
        if (err) *err = CORE_ERROR_NULL;
        CORE_ERROR_SET(CORE_ERROR_NULL);
        return 0.0; // default value
    }
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols) {
        if (err) *err = CORE_ERROR_OUT_OF_BOUNDS;
        CORE_ERROR_SET(CORE_ERROR_OUT_OF_BOUNDS);
        return 0.0;
    }

    if (err) *err = CORE_ERROR_SUCCESS;
    return mat->data[i * mat->cols + j];
}

CoreErrorStatus matrix_ops_set_zero(Matrix* mat)
{
    if (mat == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }

    CoreErrorStatus status = matrix_ops_fill(mat, 0.0);
    if (status != CORE_ERROR_SUCCESS) {
        // Propagate the actual error code instead of always returning NULL
        // so that callers receive precise diagnostics when filling fails.
        CORE_ERROR_RETURN(status);
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_set_identity(Matrix* mat) {
    if (mat == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows != mat->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }
        
    CoreErrorStatus status = matrix_ops_set_zero(mat);
    if (status != CORE_ERROR_SUCCESS) {
        // Preserve the specific error from matrix_ops_set_zero instead of
        // obscuring it with CORE_ERROR_NULL.
        CORE_ERROR_RETURN(status);
    }

    for (int i = 0; i < mat->rows; ++i)  {
        mat->data[i * mat->cols + i] = 1.0;
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_add(Matrix* result, const Matrix* a, const Matrix* b) {
    if (a == NULL || b == NULL || result == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (a->rows != b->rows || a->cols != b->cols ||
        a->rows != result->rows || a->cols != result->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }

    int size = a->rows * a->cols;

    for (int i = 0; i < size; ++i) {
        result->data[i] = a->data[i] + b->data[i];
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_multiply(Matrix* result, const Matrix* a, const Matrix* b) {
    if (a == NULL || b == NULL || result == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (a->cols     !=  b->rows        ||
        a->rows    !=  result->rows || 
        b->cols     !=  result->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }

    CoreErrorStatus status;

    for (int i = 0; i < a->rows; ++i) 
    {
        for (int j = 0; j < b->cols; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < a->cols; ++k) {
                sum += matrix_ops_get(a, i, k, &status) * matrix_ops_get(b, k, j, &status);
                if (status != CORE_ERROR_SUCCESS) {
                    CORE_ERROR_RETURN(status);
                }
            }
            status =matrix_ops_set(result, i, j, sum);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
        }
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_copy(Matrix* dest, const Matrix* src)
{
    if (!src || !dest) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (src->rows != dest->rows || src->cols != dest->cols)
    {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }
   if (src == dest) CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);

    size_t n = (size_t)src->rows * (size_t)src->cols;
    memcpy(dest->data, src->data, n * sizeof(double));

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_power(const Matrix* mat, int n, Matrix* result)
{
    if (mat == NULL || result == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows != mat->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }
    if (n < 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    int size = mat->rows;
    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    Matrix* base = NULL;
    Matrix* temp_result = NULL;
    int bits[32];
    int bitsNum = 0;

    // Handle special cases
    if (n == 0) {
        status = matrix_ops_set_identity(result);
        goto cleanup;
    }
    if (n == 1) {
        status = matrix_ops_copy(result, mat);
        goto cleanup;
    }

    // Allocate memory
    base = matrix_core_create(size, size, &status);
    if (status != CORE_ERROR_SUCCESS){
        goto cleanup;
    }
    temp_result = matrix_core_create(size, size, &status);
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    // Convert exponent to binary representation
    status = bit_utils_to_binary_lsb(n, bits, 32, &bitsNum);
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    // Initialize base as mat
    status = matrix_ops_copy(base, mat);
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    // Initialize result as identity matrix
    status = matrix_ops_set_identity(result);
    if (status != CORE_ERROR_SUCCESS) {
        goto cleanup;
    }

    // Binary exponentiation
    for (int exp = 0; exp < bitsNum; exp++) {
        // If the current bit is 1, multiply result by base
        if (bits[exp] == 1) {
            status = matrix_ops_multiply(temp_result, result, base);
            if (status != CORE_ERROR_SUCCESS) {
                goto cleanup;
            }
            status = matrix_ops_copy(result, temp_result);
            if (status != CORE_ERROR_SUCCESS) {
                goto cleanup;
            }
        }

        // Square the base for the next bit
        status = matrix_ops_multiply(temp_result, base, base);
        if (status != CORE_ERROR_SUCCESS) {
            goto cleanup;
        }
        status = matrix_ops_copy(base, temp_result);
        if (status != CORE_ERROR_SUCCESS) {
            goto cleanup;
        }
    }

cleanup:
    if (base) {
        CoreErrorStatus tmp = matrix_core_free(base);
        if (status == CORE_ERROR_SUCCESS && tmp != CORE_ERROR_SUCCESS) {
            status = tmp;
        }
    }
    if (temp_result) {
        CoreErrorStatus tmp = matrix_core_free(temp_result);
        if (status == CORE_ERROR_SUCCESS && tmp != CORE_ERROR_SUCCESS) {
            status = tmp;
        }
    }

    CORE_ERROR_RETURN(status);
}

CoreErrorStatus matrix_ops_print(const Matrix* mat, const char* label)
{
    if (mat == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // ƒ‰ƒxƒ‹‚ª‚ ‚ê‚Îæ“ª‚Éo—Í
    if (label && label[0] != '\0') {
        printf("%s\n", label);
    }

    printf("Matrix (%d x %d):\n", mat->rows, mat->cols);
    for (int i = 0; i < mat->rows; i++) {
        printf("[ ");
        for (int j = 0; j < mat->cols; j++) {
            printf("%10.6f ", matrix_ops_get(mat, i, j, &status));
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
        }
        printf("]\n");
    }
    printf("\n");

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_scale(Matrix* mat, double factor) {
    if (!mat || !mat->data) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows <= 0 || mat->cols <= 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }
    if (isnan(factor)) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    /* Fast paths */
    if (factor == 1.0) {
        CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
    }
    if (factor == 0.0) {
        return matrix_ops_set_zero(mat);
    }

    const int n = mat->rows * mat->cols;

    // General case
    double* __restrict d = mat->data;
    for (int i = 0; i < n; ++i) {
        d[i] *= factor;
    }
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_axpy(Matrix* Y, double alpha, const Matrix* X)
{
    if (!Y || !X)
    {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (!Y->data || !X->data)
    {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (Y->rows != X->rows || Y->cols != X->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    const int nrows = Y->rows;
    const int ncols = Y->cols;
    double* ydata = Y->data;
    const double* xdata = X->data;

    for (int i = 0; i < nrows * ncols; ++i) {
        ydata[i] += alpha * xdata[i];
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_fill_sequential(Matrix* mat, double start, double step) {
    if(!mat) CORE_ERROR_RETURN(CORE_ERROR_NULL);

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    double v = start;
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            status = matrix_ops_set(mat, i, j, v);
            if(status != CORE_ERROR_SUCCESS) CORE_ERROR_RETURN(status);
            v += step;
        }
    }
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_set_block(Matrix* dst, int offset_row, int offset_col, const Matrix* src) {
    if (!dst || !src) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (offset_row < 0 || offset_col < 0) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (src->rows + offset_row > dst->rows) CORE_ERROR_RETURN(CORE_ERROR_OUT_OF_BOUNDS);
    if (src->cols + offset_col > dst->cols) CORE_ERROR_RETURN(CORE_ERROR_OUT_OF_BOUNDS);
  
    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    for (int r = 0; r < src->rows; ++r) {
        double* dst_row = &dst->data[(offset_row + r) * dst->cols + offset_col];
        const double* src_row = &src->data[r * src->cols];

        memcpy(dst_row, src_row, sizeof(double) * src->cols);
    }
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus matrix_ops_get_block(const Matrix* src, int offset_row, int offset_col, Matrix* out)
{
    if (!src || !out)                          CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (offset_row < 0 || offset_col < 0)      CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (out->rows <= 0 || out->cols <= 0)      CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (offset_row + out->rows > src->rows ||
        offset_col + out->cols > src->cols)    CORE_ERROR_RETURN(CORE_ERROR_OUT_OF_BOUNDS);

    for (int r = 0; r < out->rows; ++r) {
        const double* src_row = &src->data[(offset_row + r) * src->cols + offset_col];
        double* out_row = &out->data[r * out->cols];
        memcpy(out_row, src_row, sizeof(double) * (size_t)out->cols);
    }

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}
