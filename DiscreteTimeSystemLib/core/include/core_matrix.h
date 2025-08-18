#pragma once

#include <stdlib.h>
#include <stdio.h> // For fprintf in macros
#include "core_error.h"

/*
 * =============================================================================
 *  core_matrix.h
 * =============================================================================
 *
 *  Description:
 *      This header defines fundamental structures, macros, and core functions
 *      for matrix operations in C.
 *
 *  Features:
 *      - Creation and deletion of matrices
 *      - Matrix addition, subtraction, and multiplication
 *      - Matrix transposition
 *      - Scalar operations on matrices
 *
 * =============================================================================
 */

//------------------------------------------------
//  Macro definitions
//------------------------------------------------

//------------------------------------------------
//  Type definitions
//------------------------------------------------

/**
 * @brief Structure representing a 2D matrix.
 *
 * The matrix is stored in row-major order as a 1D array:
 * element at (i, j) is located at data[i * cols + j].
 */
typedef struct {
    int rows;
    int cols;
    double* data;  // Row-major: data[i * cols + j]
} Matrix;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Create a new matrix with specified dimensions.
 *
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @param err Pointer to CoreErrorStatus for storing error code (can be NULL).
 * @return Pointer to allocated Matrix, or NULL on failure.
 */
Matrix* matrix_core_create(int rows, int cols, CoreErrorStatus* err);

/**
 * @brief Create a new square matrix with specified dimensions.
 *
 * @param size Number of rows/columns; must be greater than 0.
 * @param err Pointer to CoreErrorStatus for storing error code (can be NULL).
 * @return Pointer to allocated Matrix, or NULL on failure.
 */
Matrix* matrix_core_create_square(int size, CoreErrorStatus* err);

/**
 * @brief Free the memory associated with a matrix.
 *
 * @param mat Pointer to the matrix to free.
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_core_free(Matrix* mat);
