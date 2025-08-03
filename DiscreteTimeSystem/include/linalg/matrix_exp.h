#pragma once

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
void matrix_exp_exponential(const Matrix* A, double t, Matrix* result);


