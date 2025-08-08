#pragma once

#include <stdio.h>
#include "matrix_ops.h"
#include "bit_utils.h"
#include "matrix_core.h"

int matrix_ops_fill(Matrix* mat, double value) {
    if (mat == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }

    int size = mat->rows * mat->cols;

    for (int i = 0; i < size; ++i) 
    {
        mat->data[i] = value;
    }

    return MATRIX_OPS_SUCCESS;
}

int matrix_ops_set_zero(Matrix* mat)
{
    if (mat == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }

    matrix_ops_fill(mat, 0.0);

    return MATRIX_OPS_SUCCESS;
}

int matrix_ops_set_identity(Matrix* mat) {
    if (mat == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }
    if (mat->rows != mat->cols)
    {
        return MATRIX_OPS_ERR_DIMENSION;
    }

    matrix_ops_set_zero(mat);

    for (int i = 0; i < mat->rows; ++i) 
    {
        mat->data[i * mat->cols + i] = 1.0;
    }

    return MATRIX_OPS_SUCCESS;
}

int matrix_ops_set(Matrix* mat, int i, int j, double value) {
    if (mat == NULL)
    {
        MATRIX_CORE_ERR_MESSAGE(MATRIX_OPS_ERR_NULL);
        return MATRIX_OPS_ERR_NULL;
    }
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols)
    {
        MATRIX_CORE_ERR_MESSAGE(MATRIX_OPS_ERR_OUT_OF_BOUNDS);
        return MATRIX_OPS_ERR_OUT_OF_BOUNDS;
    }

    mat->data[i * mat->cols + j] = value;

    return MATRIX_OPS_SUCCESS;
}

double matrix_ops_get(const Matrix* mat, int i, int j, int* err)
{
    if (mat == NULL)
    {
        if (err) *err = MATRIX_OPS_ERR_NULL;
        return 0.0; // default value
    }
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols)
    {
        if (err) *err = MATRIX_OPS_ERR_OUT_OF_BOUNDS;
        return 0.0;
    }

    if (err) *err = MATRIX_OPS_SUCCESS;
    return mat->data[i * mat->cols + j];
}

int matrix_ops_add(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a == NULL || b == NULL || result == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols ||
        a->rows != result->rows || a->cols != result->cols)
    {
        return MATRIX_OPS_ERR_DIMENSION;
    }

    int size = a->rows * a->cols;

    for (int i = 0; i < size; ++i) 
    {
        result->data[i] = a->data[i] + b->data[i];
    }

    return MATRIX_OPS_SUCCESS;
}

int matrix_ops_multiply(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a == NULL || b == NULL || result == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }
    if (a->cols     !=  b->rows        ||
        a->rows    !=  result->rows || 
        b->cols     !=  result->cols) 
    {
        return MATRIX_OPS_ERR_DIMENSION;
    }

    int status;

    for (int i = 0; i < a->rows; ++i) 
    {
        for (int j = 0; j < b->cols; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < a->cols; ++k) {
                sum += matrix_ops_get(a, i, k, &status) * matrix_ops_get(b, k, j, &status);
                if (status != MATRIX_OPS_SUCCESS) {
                    MATRIX_CORE_ERR_MESSAGE(status);
                }
            }
            matrix_ops_set(result, i, j, sum);
        }
    }

    return MATRIX_OPS_SUCCESS;
}

int matrix_ops_copy(const Matrix* src, Matrix* dest)
{
    if (src == NULL || dest == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }
    if (src->rows != dest->rows || src->cols != dest->cols) 
    {
        return MATRIX_OPS_ERR_DIMENSION;
    }

    int status;

    for (int i = 0; i < src->rows; i++) 
    {
        for (int j = 0; j < src->cols; j++) {
            matrix_ops_set(dest, i, j, matrix_ops_get(src, i, j, &status));
            if (status != MATRIX_OPS_SUCCESS) {
                MATRIX_CORE_ERR_MESSAGE(status);
            }

        }
    }

    return MATRIX_OPS_SUCCESS;
}

int matrix_ops_power(const Matrix* mat, int n, Matrix* result)
{
    if (mat == NULL || result == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }
    if (mat->rows != mat->cols)
    {
        return MATRIX_OPS_ERR_DIMENSION;
    }

    int size = mat->rows;

    // Handle special cases
    if (n == 0) {
        matrix_ops_set_identity(result);
        return MATRIX_OPS_SUCCESS;
    }
    if (n == 1) {
        matrix_ops_copy(mat, result);
        return MATRIX_OPS_SUCCESS;
    }

    // Allocate memory
    int bits[32];
    int status;
    Matrix* base = matrix_create(size, size, &status);
    if (status != MATRIX_CORE_SUCCESS)
    {
        MATRIX_CORE_ERR_MESSAGE(status);
    }
    Matrix* temp_result = matrix_create(size, size, &status);
    {
        MATRIX_CORE_ERR_MESSAGE(status);
    }

    // Convert exponent to binary representation
    int bitsNum = bit_utils_to_binary_lsb(n, bits, 32);

    // Initialize base as mat
    matrix_ops_copy(mat, base);

    // Initialize result as identity matrix
    matrix_ops_set_identity(result);

    // Binary exponentiation
    for (int exp = 0; exp < bitsNum; exp++) {
        // If the current bit is 1, multiply result by base
        if (bits[exp] == 1) {
            matrix_ops_multiply(result, base, temp_result);
            matrix_ops_copy(temp_result, result);
        }

        // Square the base for the next bit
        matrix_ops_multiply(base, base, temp_result);
        matrix_ops_copy(temp_result, base);
    }

    matrix_free(base);
    matrix_free(temp_result);

    return MATRIX_OPS_SUCCESS;
};

int matrix_ops_print(const Matrix* mat)
{
    if (mat == NULL)
    {
        return MATRIX_OPS_ERR_NULL;
    }

    int status = 0;

    printf("Matrix (%d x %d):\n", mat->rows, mat->cols);
    for (int i = 0; i < mat->rows; i++) {
        printf("[ ");
        for (int j = 0; j < mat->cols; j++) {
            printf("%10.6f ", matrix_ops_get(mat, i, j, &status));
        }
        if (status != MATRIX_OPS_SUCCESS) {
            MATRIX_CORE_ERR_MESSAGE(status);
        }

        printf("]\n");
    }
    printf("\n");

    return MATRIX_OPS_SUCCESS;
}

