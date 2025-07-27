#pragma once
/*
 * =============================================================================
 *  matrix_ops.h
 * =============================================================================
 *
 *  Description:
 *      This header defines general-purpose matrix operations for C programs.
 *
 *  Features:
 *      - Matrix addition and subtraction
 *      - Matrix multiplication
 *      - Scalar multiplication
 *      - Matrix transposition
 *      - Identity and zero matrix generation
 *
 * =============================================================================
 */

#include "matrix.h"

 //------------------------------------------------
//  Macro definitions
//------------------------------------------------
/* None */

//------------------------------------------------
//  Type definitions
//------------------------------------------------

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Fill all elements of a matrix with a given scalar value.
 *
 * @param mat   Pointer to the matrix.
 * @param value Scalar value to assign to each element.
 */
void matrix_fill(Matrix* mat, double value);

/**
 * @brief Set all elements of a matrix to zero.
 *
 * @param mat Pointer to the matrix.
 */
void matrix_set_zero(Matrix* mat);

/**
 * @brief Initialize a square matrix as an identity matrix.
 *
 * @param mat Pointer to the square matrix.
 * @note This function does nothing if the matrix is not square.
 */
void matrix_set_identity(Matrix* mat);

/**
 * @brief Set a specific element of a matrix.
 *
 * @param mat   Pointer to the matrix.
 * @param i     Row index (0-based).
 * @param j     Column index (0-based).
 * @param value Value to set at (i, j).
 */
void matrix_set(Matrix* mat, int i, int j, double value);

/**
 * @brief Get the value of a specific element in a matrix.
 *
 * @param mat Pointer to the matrix.
 * @param i   Row index (0-based).
 * @param j   Column index (0-based).
 * @return Value at position (i, j). Returns 0.0 if out of bounds.
 */
double matrix_get(const Matrix* mat, int i, int j);

/**
 * @brief Add two matrices: result = a + b
 *
 * @param a First input matrix (must match dimensions of b)
 * @param b Second input matrix
 * @param result Output matrix (preallocated with same dimensions as a/b)
 */
void matrix_add(const Matrix* a, const Matrix* b, Matrix* result);

/**
 * @brief Multiply two matrices: result = a * b
 *
 * @param a Left matrix (size m x n)
 * @param b Right matrix (size n x p)
 * @param result Output matrix (must be preallocated with size m x p)
 */
void matrix_multiply(const Matrix* a, const Matrix* b, Matrix* result);