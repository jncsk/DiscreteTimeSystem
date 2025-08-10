#pragma once

#include <stdio.h>
#include <string.h>
#include "matrix_ops.h"
#include "bit_utils.h"
#include "matrix_core.h"

MatrixCoreStatus matrix_ops_fill(Matrix* mat, double value) {
    if (mat == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }

    int size = mat->rows * mat->cols;

    for (int i = 0; i < size; ++i) 
    {
        mat->data[i] = value;
    }

    return MATRIX_CORE_SUCCESS;
}

MatrixCoreStatus matrix_ops_set_zero(Matrix* mat)
{
    if (mat == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }

    MatrixCoreStatus status =matrix_ops_fill(mat, 0.0);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }

    return MATRIX_CORE_SUCCESS;
}

MatrixCoreStatus matrix_ops_set_identity(Matrix* mat) {
    if (mat == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }
    if (mat->rows != mat->cols)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_DIMENSION);
    }

    
    MatrixCoreStatus status = matrix_ops_set_zero(mat);;
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }

    for (int i = 0; i < mat->rows; ++i) 
    {
        mat->data[i * mat->cols + i] = 1.0;
    }

    return MATRIX_CORE_SUCCESS;
}

MatrixCoreStatus matrix_ops_set(Matrix* mat, int i, int j, double value) {
    if (mat == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }
    if (i < 0 || i >= mat->rows)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_OUT_OF_BOUNDS);
    }    
    if (j < 0 || j >= mat->cols)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_OUT_OF_BOUNDS);
    }

    mat->data[i * mat->cols + j] = value;

    return MATRIX_CORE_SUCCESS;
}

double matrix_ops_get(const Matrix* mat, int i, int j, MatrixCoreStatus* err)
{
    if (mat == NULL)
    {
        if (err) *err = MATRIX_CORE_ERR_NULL;
        MATRIX_CORE_SET_ERROR(MATRIX_CORE_ERR_NULL);
        return 0.0; // default value
    }
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols)
    {
        if (err) *err = MATRIX_CORE_ERR_OUT_OF_BOUNDS;
        MATRIX_CORE_SET_ERROR(MATRIX_CORE_ERR_OUT_OF_BOUNDS);
        return 0.0;
    }

    if (err) *err = MATRIX_CORE_SUCCESS;
    return mat->data[i * mat->cols + j];
}

MatrixCoreStatus matrix_ops_add(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a == NULL || b == NULL || result == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }
    if (a->rows != b->rows || a->cols != b->cols ||
        a->rows != result->rows || a->cols != result->cols)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_DIMENSION);
    }

    int size = a->rows * a->cols;

    for (int i = 0; i < size; ++i) 
    {
        result->data[i] = a->data[i] + b->data[i];
    }

    return MATRIX_CORE_SUCCESS;
}

MatrixCoreStatus matrix_ops_multiply(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a == NULL || b == NULL || result == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }
    if (a->cols     !=  b->rows        ||
        a->rows    !=  result->rows || 
        b->cols     !=  result->cols) 
    {
        RETURN_ERROR(MATRIX_CORE_ERR_DIMENSION);
    }

    MatrixCoreStatus status;

    for (int i = 0; i < a->rows; ++i) 
    {
        for (int j = 0; j < b->cols; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < a->cols; ++k) {
                sum += matrix_ops_get(a, i, k, &status) * matrix_ops_get(b, k, j, &status);
                if (status != MATRIX_CORE_SUCCESS) 
                {
                    RETURN_ERROR(status);
                }
            }
            status =matrix_ops_set(result, i, j, sum);
            if (status != MATRIX_CORE_SUCCESS) 
            {
                RETURN_ERROR(status);
            }
        }
    }

    return MATRIX_CORE_SUCCESS;
}

MatrixCoreStatus matrix_ops_copy(const Matrix* src, Matrix* dest)
{
    if (!src || !dest) {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }
    if (src->rows != dest->rows || src->cols != dest->cols)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_DIMENSION);
    }
   if (src == dest) return MATRIX_CORE_SUCCESS; 

    size_t n = (size_t)src->rows * (size_t)src->cols;
    memcpy(dest->data, src->data, n * sizeof(double));

    return MATRIX_CORE_SUCCESS;
}

MatrixCoreStatus matrix_ops_power(const Matrix* mat, int n, Matrix* result)
{
    if (mat == NULL || result == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }
    if (mat->rows != mat->cols)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_DIMENSION);
    }

    int size = mat->rows;
    MatrixCoreStatus status = MATRIX_CORE_SUCCESS;
    // Handle special cases
    if (n == 0) {
        status = matrix_ops_set_identity(result);
        if (status != MATRIX_CORE_SUCCESS)
        {
            RETURN_ERROR(status);
        }
        else
        {
            return MATRIX_CORE_SUCCESS;
        }
    }
    if (n == 1) {
        status =matrix_ops_copy(mat, result);
        if (status != MATRIX_CORE_SUCCESS)
        {
            RETURN_ERROR(status);
        }
        else
        {
            return MATRIX_CORE_SUCCESS;
        }
    }

    // Allocate memory
    int bits[32];
    Matrix* base = matrix_core_create(size, size, &status);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(status);
    }
    Matrix* temp_result = matrix_core_create(size, size, &status);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(status);
    }

    // Convert exponent to binary representation
    int bitsNum = bit_utils_to_binary_lsb(n, bits, 32);

    // Initialize base as mat
    status = matrix_ops_copy(mat, base);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(status);
    }

    // Initialize result as identity matrix
    status = matrix_ops_set_identity(result);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(status);
    }

    // Binary exponentiation
    for (int exp = 0; exp < bitsNum; exp++) {
        // If the current bit is 1, multiply result by base
        if (bits[exp] == 1) {
            status = matrix_ops_multiply(result, base, temp_result);
            if (status != MATRIX_CORE_SUCCESS)
            {
                RETURN_ERROR(status);
            }
            status = matrix_ops_copy(temp_result, result);
            if (status != MATRIX_CORE_SUCCESS)
            {
                RETURN_ERROR(status);
            }
        }

        // Square the base for the next bit
        status = matrix_ops_multiply(base, base, temp_result);
        if (status != MATRIX_CORE_SUCCESS)
        {
            RETURN_ERROR(status);
        }
        status = matrix_ops_copy(temp_result, base);
        if (status != MATRIX_CORE_SUCCESS)
        {
            RETURN_ERROR(status);
        }
    }

    status = matrix_core_free(base);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(status);
    }

    status = matrix_core_free(temp_result);
    if (status != MATRIX_CORE_SUCCESS)
    {
        RETURN_ERROR(status);
    }

    return MATRIX_CORE_SUCCESS;
};

MatrixCoreStatus matrix_ops_print(const Matrix* mat)
{
    if (mat == NULL)
    {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }

    MatrixCoreStatus status = 0;

    printf("Matrix (%d x %d):\n", mat->rows, mat->cols);
    for (int i = 0; i < mat->rows; i++) {
        printf("[ ");
        for (int j = 0; j < mat->cols; j++) {
            printf("%10.6f ", matrix_ops_get(mat, i, j, &status));
            if (status != MATRIX_CORE_SUCCESS)
            {
                RETURN_ERROR(status);
            }
        }

        printf("]\n");
    }
    printf("\n");

    return MATRIX_CORE_SUCCESS;
}

