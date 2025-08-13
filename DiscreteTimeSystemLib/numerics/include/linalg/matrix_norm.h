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

// 1ノルム（列の絶対値和の最大）
CoreErrorStatus matrix_norm_1(const Matrix* mat, double* result);

// ∞ノルム（行の絶対値和の最大）
CoreErrorStatus matrix_norm_inf(const Matrix* mat, double* result);

// Frobeniusノルム
CoreErrorStatus matrix_norm_fro(const Matrix* mat, double* result);


