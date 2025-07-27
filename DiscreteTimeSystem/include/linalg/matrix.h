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
typedef struct {
    int rows;
    int cols;
    double* data;  // Row-major: data[i * cols + j]
} Matrix;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------
Matrix matrix_create(int rows, int cols);
void matrix_free(Matrix* mat);
