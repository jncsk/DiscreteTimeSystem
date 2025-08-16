#pragma once
#include <stdio.h>
#include "core_matrix.h"

Matrix* matrix_core_create(int rows, int cols, CoreErrorStatus* err)
{
    if(rows <= 0 || cols <= 0)
    {
        *err = CORE_ERROR_OUT_OF_BOUNDS;
        CORE_ERROR_SET(CORE_ERROR_OUT_OF_BOUNDS);
        return NULL;
    }
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    if (mat == NULL)
    {
        *err = CORE_ERROR_ALLOCATION_FAILED;
        CORE_ERROR_SET(CORE_ERROR_ALLOCATION_FAILED);
        return NULL;
    }

    mat->rows = rows;
    mat->cols = cols;
    mat->data = (double*) malloc(rows * cols * sizeof(double));
    if (mat->data == NULL)
    {
        free(mat);
        *err = CORE_ERROR_ALLOCATION_FAILED;
        CORE_ERROR_SET(CORE_ERROR_ALLOCATION_FAILED);
        return NULL;
    }

    *err = CORE_ERROR_SUCCESS;
    return mat;
}

Matrix* matrix_core_create_square(int size, CoreErrorStatus* err)
{
    // size must be positive to form a valid square matrix
    if (size <= 0)
    {
        *err = CORE_ERROR_OUT_OF_BOUNDS;
        CORE_ERROR_SET(CORE_ERROR_OUT_OF_BOUNDS);
        return NULL;
    }

    return matrix_core_create(size, size, err);
}

CoreErrorStatus matrix_core_free(Matrix* mat)
{
    if (mat == NULL)
    {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    free(mat->data);
    mat->data = NULL;
    free(mat);
    mat = NULL;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

