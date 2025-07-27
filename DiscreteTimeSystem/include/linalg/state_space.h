#pragma once

/*
 * =============================================================================
 *  state_space.h
 * =============================================================================
 *
 *  Description:
 *      This header provides helper functions to initialize and free
 *      state-space system models (A, B, C matrices).
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
typedef struct {
    Matrix A;  // n x n
    Matrix B;  // n x m
    Matrix C;  // p x n
} StateSpaceModel;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------
StateSpaceModel create_state_space(int n, int m, int p);
void free_state_space(StateSpaceModel* model);
