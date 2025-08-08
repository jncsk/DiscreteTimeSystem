#pragma once
#include <stdio.h>
#include "matrix_core.h"

THREAD_LOCAL MatrixError g_matrix_last_error = { 0, NULL, 0 };

Matrix* matrix_create(int rows, int cols, int* err)
{
    if(rows < 0 || cols < 0)
    {
        *err = MATRIX_CORE_ERR_OUT_OF_BOUNDS;
        MATRIX_CORE_SET_ERROR(MATRIX_CORE_ERR_OUT_OF_BOUNDS);
        return NULL;
    }
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    if (mat == NULL)
    {
        *err = MATRIX_CORE_ERR_ALLOCATION_FAILED;
        MATRIX_CORE_SET_ERROR(MATRIX_CORE_ERR_ALLOCATION_FAILED);
        return NULL;
    }

    mat->rows = rows;
    mat->cols = cols;
    mat->data = (double*) malloc(rows * cols * sizeof(double));
    if (mat->data == NULL)
    {
        free(mat);
        *err = MATRIX_CORE_ERR_ALLOCATION_FAILED;
        MATRIX_CORE_SET_ERROR(MATRIX_CORE_ERR_ALLOCATION_FAILED);
        return NULL;
    }

    *err = MATRIX_CORE_SUCCESS;
    return mat;
}


int matrix_free(Matrix* mat) 
{
    if (mat == NULL)
    {
        MATRIX_CORE_SET_ERROR(MATRIX_CORE_ERR_NULL);
        return MATRIX_CORE_ERR_NULL;
    }
    free(mat->data);
    mat->data = NULL;

    return MATRIX_CORE_SUCCESS;
}

MatrixError matrix_core_get_last_error(void) {
    return g_matrix_last_error;
}
