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
void matrix_fill(Matrix* mat, double value);
void matrix_set_zero(Matrix* mat);
void matrix_set_identity(Matrix* mat);
void matrix_set(Matrix* mat, int i, int j, double value);
double matrix_get(const Matrix* mat, int i, int j);
