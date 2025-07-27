#pragma once

/*
 * =============================================================================
 *  matrix.h
 * =============================================================================
 *
 *  Description:
 *      This header defines fundamental structures and core functions
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

#include <stdlib.h>

//------------------------------------------------
//  Macro definitions
//------------------------------------------------
/* None */

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
 * @return A Matrix structure with allocated memory.
 */
Matrix matrix_create(int rows, int cols);

/**
 * @brief Free the memory associated with a matrix.
 *
 * @param mat Pointer to the matrix to free.
 */
void matrix_free(Matrix* mat);
