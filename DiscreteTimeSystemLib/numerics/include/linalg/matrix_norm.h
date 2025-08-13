#pragma once
#include <math.h>
/*
 * =============================================================================
 *  matrix_norm.h
 * =============================================================================
 *
 *  Description:
 *      This header provides functions for computing matrix norms in C.
 *
 *  Features:
 *      - Compute 1-norm                  : Maximum absolute column sum.
 *      - Compute infinity-norm         : Maximum absolute row sum.
 *      - Compute Frobenius-norm    : Square root of sum of squares of all elements.
 *
 * =============================================================================
 */

#include "core_error.h"
#include "core_matrix.h"

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
 * @brief Calculate 1 norm.
 *
 * @param mat   Pointer to the matrix (must not be NULL).
 * @param result Pointer to where the computed norm will be stored (must not be NULL).
 * @return CORE_ERROR_SUCCESS if succeeds, otherwise an error code.
 */
CoreErrorStatus matrix_norm_1(const Matrix* mat, double* result);

/**
 * @brief Calculate 1 norm.
 *
 * @param mat   Pointer to the matrix (must not be NULL).
 * @param result Pointer to where the computed norm will be stored (must not be NULL).
 * @return CORE_ERROR_SUCCESS if succeeds, otherwise an error code.
 */
CoreErrorStatus matrix_norm_inf(const Matrix* mat, double* result);

/**
 * @brief Calculate 1 norm.
 *
 * @param mat   Pointer to the matrix (must not be NULL).
 * @param result Pointer to where the computed norm will be stored (must not be NULL).
 * @return CORE_ERROR_SUCCESS if succeeds, otherwise an error code.
 */
CoreErrorStatus matrix_norm_fro(const Matrix* mat, double* result);


