#pragma once

#include <stdio.h>
#include "matrix_ops.h"

void matrix_fill(Matrix* mat, double value) {
    int size = mat->rows * mat->cols;
    for (int i = 0; i < size; ++i) {
        mat->data[i] = value;
    }
}

void matrix_set_zero(Matrix* mat) {
    matrix_fill(mat, 0.0);
}

void matrix_set_identity(Matrix* mat) {
    if (mat->rows != mat->cols) {
        printf("Error: matrix is not square.\n");
        return;
    }

    matrix_set_zero(mat);

    for (int i = 0; i < mat->rows; ++i) {
        mat->data[i * mat->cols + i] = 1.0;
    }
}

void matrix_set(Matrix* mat, int i, int j, double value) {
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols) {
        printf("Error: index (%d, %d) out of bounds for matrix %dx%d\n",
            i, j, mat->rows, mat->cols);
        return;
    }
    mat->data[i * mat->cols + j] = value;
}

double matrix_get(const Matrix* mat, int i, int j) {
    if (i < 0 || i >= mat->rows || j < 0 || j >= mat->cols) {
        printf("Error: index (%d, %d) out of bounds for matrix %dx%d\n",
            i, j, mat->rows, mat->cols);
        return 0.0;
    }
    return mat->data[i * mat->cols + j];
}

void matrix_add(const Matrix* a, const Matrix* b, Matrix* result) {
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

void matrix_multiply(const Matrix* a, const Matrix* b, Matrix* result) {
    if (a->cols != b->rows ||
        a->rows != result->rows || b->cols != result->cols) {
        printf("Error: Dimension mismatch in matrix_multiply.\n");
        return;
    }

    for (int i = 0; i < a->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) {
            double sum = 0.0;
            for (int k = 0; k < a->cols; ++k) {
                sum += matrix_get(a, i, k) * matrix_get(b, k, j);
            }
            matrix_set(result, i, j, sum);
        }
    }
}