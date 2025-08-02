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
 *      - Matrix copy
 *      - Print the contents of a matrix
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
void matrix_ops_fill(Matrix* mat, double value);

/**
 * @brief Set all elements of a matrix to zero.
 *
 * @param mat Pointer to the matrix.
 */
void matrix_ops_set_zero(Matrix* mat);

/**
 * @brief Initialize a square matrix as an identity matrix.
 *
 * @param mat Pointer to the square matrix.
 * @note This function does nothing if the matrix is not square.
 */
void matrix_ops_set_identity(Matrix* mat);

/**
 * @brief Set a specific element of a matrix.
 *
 * @param mat   Pointer to the matrix.
 * @param i     Row index (0-based).
 * @param j     Column index (0-based).
 * @param value Value to set at (i, j).
 */
void matrix_ops_set(Matrix* mat, int i, int j, double value);

/**
 * @brief Get the value of a specific element in a matrix.
 *
 * @param mat Pointer to the matrix.
 * @param i   Row index (0-based).
 * @param j   Column index (0-based).
 * @return Value at position (i, j). Returns 0.0 if out of bounds.
 */
double matrix_ops_get(const Matrix* mat, int i, int j);

/**
 * @brief Add two matrices: result = a + b
 *
 * @param a First input matrix (must match dimensions of b)
 * @param b Second input matrix
 * @param result Output matrix (preallocated with same dimensions as a/b)
 */
void matrix_ops_add(const Matrix* a, const Matrix* b, Matrix* result);

/**
 * @brief Multiply two matrices: result = a * b
 *
 * @param a            Left matrix (size m x n)
 * @param b           Right matrix (size n x p)
 * @param result    Output matrix (must be preallocated with size m x p)
 */
void matrix_ops_multiply(const Matrix* a, const Matrix* b, Matrix* result);

/**
 * @brief Compute the integer power of a square matrix (A^n)
 *
 * @param mat        The input square matrix (size N x N)
 * @param n            Non-negative integer exponent
 * @param result	    Output matrix (must be preallocated with size N x N)
 */
void matrix_ops_power(const Matrix* mat, int n, Matrix* result);

/**
 * @brief Copy all elements from one matrix to another.
 *
 * @param src   Source matrix
 * @param dest  Destination matrix (must have same dimensions as src)
 */
void matrix_ops_copy(const Matrix* src, Matrix* dest);


/**
 * @brief Print the contents of a matrix to stdout in a readable format.
 *
 * @param mat Pointer to the matrix to be printed.
 */
void matrix_ops_print(const Matrix* mat);
