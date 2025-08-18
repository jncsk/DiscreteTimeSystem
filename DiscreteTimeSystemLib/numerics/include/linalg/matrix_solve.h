#pragma once

#include "core_matrix.h"
#include "core_error.h"

/*
 * =============================================================================
 *  matrix_solve.h
 * =============================================================================
 *
 *  Description:
 *      Linear system solvers for dense matrices.
 *      This module provides a one-shot LU solve (with partial pivoting), and
 *      reusable factor/solve entry points to amortize cost over multiple RHS.
 *
 *  Features:
 *      - A * X = B solve via LU (no explicit inverse)
 *      - Partial pivoting for numerical stability
 *      - Multiple RHS support (nrhs = B->cols = X->cols)
 *      - Optional reusable factorization API (LU + pivots)
 *
 * =============================================================================
 */

 //------------------------------------------------
//  Macro definitions
//------------------------------------------------
/* None */

//------------------------------------------------
//  Type definitions
//------------------------------------------------
/* None */

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Solve A * X = B using LU with partial pivoting (no inverse formed).
 *
 * @param[in]  A  Coefficient square matrix (n x n). Not modified.
 * @param[out] X  Solution matrix (n x nrhs). Overwritten with the solution.
 * @param[in]  B  Right-hand side matrix (n x nrhs). Not modified.
 *
 * @return CORE_ERROR_SUCCESS on success, otherwise an error code.
 *
 * @note Internally creates a working copy LU <- A and factorizes it in-place.
 *       Supports multiple RHS: nrhs = B->cols = X->cols.
 */
CoreErrorStatus matrix_solve_LU(const Matrix* A, Matrix* X, const Matrix* B);

