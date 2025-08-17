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
 *      - Printing matrix contents
 *
 * =============================================================================
 */

#include "core_matrix.h"
#include "core_error.h"

 //------------------------------------------------
//  Macro definitions
//------------------------------------------------

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
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_fill(Matrix* mat, double value);

/**
 * @brief Fill a matrix with sequential values starting from a given number.
 *
 * Each element of the matrix is set to:
 *     value = start + (index * step)
 * where index runs in row-major order from 0 to (rows*cols - 1).
 *
 * @param mat   Pointer to the matrix to fill.
 * @param start Starting value for the first element.
 * @param step  Increment to apply between consecutive elements.
 * @return CORE_ERROR_SUCCESS if the operation completes successfully,
 *         otherwise an appropriate error code.
 */
CoreErrorStatus matrix_ops_fill_sequential(Matrix* mat, double start, double step);

/**
 * @brief Set a specific element of a matrix.
 *
 * @param mat   Pointer to the matrix.
 * @param i     Row index (0-based).
 * @param j     Column index (0-based).
 * @param value Value to set at (i, j).
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_set(Matrix* mat, int i, int j, double value);

/**
 * @brief Set all elements of a matrix to zero.
 *
 * @param mat Pointer to the matrix.
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_set_zero(Matrix* mat);

/**
 * @brief Initialize a square matrix as an identity matrix.
 *
 * @param mat Pointer to the square matrix.
 * @return CORE_ERROR_SUCCESS on success, otherwise an error code.
 */
CoreErrorStatus matrix_ops_set_identity(Matrix* mat);

/**
 * @brief Get the value of a specific element in a matrix.
 *
 * @param mat Pointer to the matrix.
 * @param i Row index (0-based).
 * @param j Column index (0-based).
 * @param err Error code
 * @return Value at position (i, j). Returns 0.0 if out of bounds.
 */
double matrix_ops_get(const Matrix* mat, int i, int j, CoreErrorStatus* err);

/**
 * @brief Add two matrices: result = a + b
 *
 * @param a First input matrix (must match dimensions of b)
 * @param b Second input matrix
 * @param result Output matrix (preallocated with same dimensions as a/b)
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_add(const Matrix* a, const Matrix* b, Matrix* result);

/**
 * @brief Multiply two matrices: result = a * b
 *
 * @param a            Left matrix (size m x n)
 * @param b           Right matrix (size n x p)
 * @param result    Output matrix (must be preallocated with size m x p)
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_multiply(const Matrix* a, const Matrix* b, Matrix* result);

/**
 * @brief Compute the integer power of a square matrix (A^n)
 *
 * @param mat        The input square matrix (size N x N)
 * @param n            Non-negative integer exponent
 * @param result	    Output matrix (must be preallocated with size N x N)
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_power(const Matrix* mat, int n, Matrix* result);

/**
 * @brief Copy all elements from one matrix to another.
 *
 * @param src   Source matrix
 * @param dest  Destination matrix (must have same dimensions as src)
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus matrix_ops_copy(const Matrix* src, Matrix* dest);

/**
 * @brief Print the contents of a matrix to stdout in a readable format.
 *
 * @param mat Pointer to the matrix to be printed.
 * @return CORE_ERROR_SUCCESS if success, otherwise an error code.
 */
CoreErrorStatus matrix_ops_print(const Matrix* mat);

/**
 * @brief Scale each element in the matrix using input value, factor.
 *
 * @param mat Pointer to the matrix to be scaled.
 * @param factor Scaling value
 * @return CORE_ERROR_SUCCESS if success, otherwise an error code.
 */
CoreErrorStatus matrix_ops_scale(Matrix* mat, double factor);

/**
 * @brief Y <- alpha * X + Y  (matrix A times X Plus Y operation)
 *
 * Adds a scaled matrix X into matrix Y.
 *
 * @param[out] Y      Destination matrix (will be updated in-place).
 * @param[in]  alpha  Scaling factor for X.
 * @param[in]  X      Source matrix to be scaled and added.
 * @return CORE_ERROR_SUCCESS if successful, otherwise an error code.
 *
 * @note
 * - Both matrices must have the same dimensions.
 * - Operation is performed element-wise.
 */
CoreErrorStatus matrix_ops_axpy(Matrix* Y, double alpha, const Matrix* X);

/**
 * @brief Copy a rectangular source matrix into a sub-block of the destination matrix.
 *
 * @param[out] dst         Destination matrix to receive the block copy.
 * @param[in]  offset_row  Top-left row index in @p dst where @p src is placed (0-based).
 * @param[in]  offset_col  Top-left column index in @p dst where @p src is placed (0-based).
 * @param[in]  src         Source matrix to copy from.
 *
 * @return CORE_ERROR_SUCCESS if success, otherwise an error code.
 *
 */
CoreErrorStatus matrix_ops_set_block(Matrix * dst, int offset_row, int offset_col, const Matrix * src);

/**
 * @brief Copy a sub-block (submatrix) from @p src into an already-allocated @p dst.
 *
 * Copies the rectangular region of @p src starting at (@p offset_row, @p offset_col)
 * with size (@p dst->rows, @p dst->cols) into @p dst. 
 *
 * @param[in]  src         Source matrix.
 * @param[in]  offset_row  Starting row index in @p src (0-based, >= 0).
 * @param[in]  offset_col  Starting column index in @p src (0-based, >= 0).
 * @param[out] dst         Destination matrix (pre-allocated, size defines copy area).
 *
 * @return CORE_ERROR_SUCCESS on success
 * @return CORE_ERROR_NULL if @p src or @p dst is NULL
 * @return CORE_ERROR_INVALID_ARG if offsets are negative or @p dst has non-positive dims
 * @return CORE_ERROR_OUT_OF_BOUNDS if the requested block exceeds @p src dimensions
 *
 */
CoreErrorStatus matrix_ops_get_block(const Matrix* src, int offset_row, int offset_col, Matrix* out);
