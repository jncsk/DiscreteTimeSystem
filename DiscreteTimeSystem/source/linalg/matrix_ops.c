#pragma once

#include <stdio.h>
#include "matrix_ops.h"
#include "bit_utils.h"

void matrix_ops_fill(Matrix* mat, double value) {
    int size = mat->rows * mat->cols;
    for (int i = 0; i < size; ++i) {
        mat->data[i] = value;
    }
}

void matrix_ops_set_zero(Matrix* mat) {
    matrix_ops_fill(mat, 0.0);
}

void matrix_ops_set_identity(Matrix* mat) {
    if (mat->rows != mat->cols) {
        printf("Error: matrix is not square.\n");
        return;
    }

    matrix_ops_set_zero(mat);

    for (int i = 0; i < mat->rows; ++i) {
        mat->data[i * mat->cols + i] = 1.0;
    }
}

void matrix_ops_set(Matrix* mat, int i, int j, double value) {
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols) {
        printf("Error: index (%d, %d) out of bounds for matrix %dx%d\n",
            i, j, mat->rows, mat->cols);
        return;
    }
    mat->data[i * mat->cols + j] = value;
}

double matrix_ops_get(const Matrix* mat, int i, int j) {
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols) {
        printf("Error: index (%d, %d) out of bounds for matrix %dx%d\n",
            i, j, mat->rows, mat->cols);
        return 0.0;
    }
    return mat->data[i * mat->cols + j];
}

void matrix_ops_add(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a->rows != b->rows || a->cols != b->cols ||
        a->rows != result->rows || a->cols != result->cols) {
        printf("Error: Dimension mismatch in matrix_add.\n");
        return;
    }

    int size = a->rows * a->cols;
    for (int i = 0; i < size; ++i) {
        result->data[i] = a->data[i] + b->data[i];
    }
}

void matrix_ops_multiply(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a->cols     !=  b->rows        ||
        a->rows    !=  result->rows || 
        b->cols     !=  result->cols) 
    {
        printf("Error: Dimension mismatch in matrix_multiply.\n");
        return;
    }

    for (int i = 0; i < a->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) {
            double sum = 0.0;
            for (int k = 0; k < a->cols; ++k) {
                sum += matrix_ops_get(a, i, k) * matrix_ops_get(b, k, j);
            }
            matrix_ops_set(result, i, j, sum);
        }
    }
}

void matrix_ops_copy(const Matrix* src, Matrix* dest)
{
    if (src->rows != dest->rows || src->cols != dest->cols) {
        printf("Error: matrix dimensions do not match for copy.\n");
        return;
    }

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            matrix_ops_set(dest, i, j, matrix_ops_get(src, i, j));
        }
    }
}

void matrix_ops_power(const Matrix* mat, int n, Matrix* result)
{
    if (mat->rows != mat->cols) {
        printf("Error: matrix is not square.\n");
        return;
    }

    int size = mat->rows;

    // Handle special cases
    if (n == 0) {
        matrix_ops_set_identity(result);
        return;
    }
    if (n == 1) {
        matrix_ops_copy(mat, result);
        return;
    }

    // Allocate memory
    int bits[32];
    Matrix base = matrix_create(size, size);
    Matrix temp_result = matrix_create(size, size);

    // Convert exponent to binary representation
    int bitsNum = bit_utils_to_binary_lsb(n, bits, 32);

    // Initialize base as mat
    matrix_ops_copy(mat, &base);

    // Initialize result as identity matrix
    matrix_ops_set_identity(result);

    // Binary exponentiation
    for (int exp = 0; exp < bitsNum; exp++) {
        // If the current bit is 1, multiply result by base
        if (bits[exp] == 1) {
            matrix_ops_multiply(result, &base, &temp_result);
            matrix_ops_copy(&temp_result, result);
        }

        // Square the base for the next bit
        matrix_ops_multiply(&base, &base, &temp_result);
        matrix_ops_copy(&temp_result, &base);
    }

    matrix_free(&base);
    matrix_free(&temp_result);
};

void matrix_ops_print(const Matrix* mat)
{
    if (mat == NULL || mat->data == NULL) {
        printf("Error: Null matrix pointer.\n");
        return;
    }

    printf("Matrix (%d x %d):\n", mat->rows, mat->cols);
    for (int i = 0; i < mat->rows; i++) {
        printf("[ ");
        for (int j = 0; j < mat->cols; j++) {
            printf("%10.6f ", matrix_ops_get(mat, i, j));
        }
        printf("]\n");
    }
    printf("\n");
}