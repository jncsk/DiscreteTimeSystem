#pragma once

#include <stdio.h>
#include <string.h>
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

    return CORE_ERROR_SUCCESS;
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

    return CORE_ERROR_SUCCESS;
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

    return CORE_ERROR_SUCCESS;
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

    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus matrix_ops_add(const Matrix* a, const Matrix* b, Matrix* result) {
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

    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus matrix_ops_multiply(const Matrix* a, const Matrix* b, Matrix* result) {
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

    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus matrix_ops_copy(const Matrix* src, Matrix* dest)
{
    if (!src || !dest) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (src->rows != dest->rows || src->cols != dest->cols)
    {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }
   if (src == dest) return CORE_ERROR_SUCCESS; 

    size_t n = (size_t)src->rows * (size_t)src->cols;
    memcpy(dest->data, src->data, n * sizeof(double));

    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus matrix_ops_power(const Matrix* mat, int n, Matrix* result)
{
    if (mat == NULL || result == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (mat->rows != mat->cols) {
        CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    }

    int size = mat->rows;
    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    // Handle special cases
    if (n == 0) {
        status = matrix_ops_set_identity(result);
        if (status != CORE_ERROR_SUCCESS) {
            CORE_ERROR_RETURN(status);
        }
        else {
            return CORE_ERROR_SUCCESS;
        }
    }
    if (n == 1) {
        status =matrix_ops_copy(mat, result);
        if (status != CORE_ERROR_SUCCESS) {
            CORE_ERROR_RETURN(status);
        }
        else {
            return CORE_ERROR_SUCCESS;
        }
    }

    // Allocate memory
    int bits[32];
    Matrix* base = matrix_core_create(size, size, &status);
    if (status != CORE_ERROR_SUCCESS){
        CORE_ERROR_RETURN(status);
    }
    Matrix* temp_result = matrix_core_create(size, size, &status);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    // Convert exponent to binary representation
    int bitsNum = 0;
    status = bit_utils_to_binary_lsb(n, bits, 32, &bitsNum);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    // Initialize base as mat
    status = matrix_ops_copy(mat, base);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    // Initialize result as identity matrix
    status = matrix_ops_set_identity(result);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    // Binary exponentiation
    for (int exp = 0; exp < bitsNum; exp++) {
        // If the current bit is 1, multiply result by base
        if (bits[exp] == 1) {
            status = matrix_ops_multiply(result, base, temp_result);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
            status = matrix_ops_copy(temp_result, result);
            if (status != CORE_ERROR_SUCCESS) {
                CORE_ERROR_RETURN(status);
            }
        }

        // Square the base for the next bit
        status = matrix_ops_multiply(base, base, temp_result);
        if (status != CORE_ERROR_SUCCESS) {
            CORE_ERROR_RETURN(status);
        }
        status = matrix_ops_copy(temp_result, base);
        if (status != CORE_ERROR_SUCCESS) {
            CORE_ERROR_RETURN(status);
        }
    }

    status = matrix_core_free(base);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    status = matrix_core_free(temp_result);
    if (status != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(status);
    }

    return CORE_ERROR_SUCCESS;
};

CoreErrorStatus matrix_ops_print(const Matrix* mat)
{
    if (mat == NULL) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }

    CoreErrorStatus status = 0;

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

    return CORE_ERROR_SUCCESS;
}

