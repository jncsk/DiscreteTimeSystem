#pragma once

#include "core_error.h"
#include "core_matrix.h"

/*
 * =============================================================================
 *  matrix_exp.h
 * =============================================================================
 *
 *  Description:
 *      This header provides functions for computing the matrix exponential
 *      and related operations in C.
 *
 *  Features:
 *      - Compute the matrix exponential using series expansion methods
 *        (e.g., Taylor or Padé approximations)
 *      - Diagonalization-based exponential computation (if applicable)
 *      - Matrix power functions for discrete-time system solutions
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

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Compute \f$e^{tA}\f$ using a scaling and squaring Padé approximation.
 *
 * The input matrix \p A is scaled by the scalar \p t and then the matrix
 * exponential is evaluated via the Padé algorithm.  The result is written to
 * the pre-allocated matrix \p result which must have the same dimensions as
 * \p A.
 *
 * @param A      Pointer to the square matrix to exponentiate.
 * @param t      Scalar multiplier applied to \p A prior to exponentiation.
 * @param result Output matrix receiving the value of \f$e^{tA}\f$.
 *
 * @return ::CORE_ERROR_SUCCESS on success or an appropriate error code on
 *         failure.
 */
CoreErrorStatus matrix_exp_exponential(const Matrix* A, double t, Matrix* result);


