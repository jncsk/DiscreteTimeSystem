#pragma once
#include <stdio.h>
#include "matrix.h"

Matrix matrix_create(int rows, int cols) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = malloc(rows * cols * sizeof(double));
    return mat;
}


void matrix_free(Matrix* mat) {
    free(mat->data);
    mat->data = NULL;
}

