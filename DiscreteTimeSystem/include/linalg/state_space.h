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

/**
 * @brief Structure representing a discrete-time state-space model:
 *        x[k+1] = Ax[k] + Bu[k]
 *        y[k]   = Cx[k]
 *
 * A: n x n (system matrix)
 * B: n x m (input matrix)
 * C: p x n (output matrix)
 */
typedef struct {
    Matrix A;  // n x n
    Matrix B;  // n x m
    Matrix C;  // p x n
} StateSpaceModel;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Create a state-space model with specified dimensions.
 *
 * @param n Number of states.
 * @param m Number of inputs.
 * @param p Number of outputs.
 * @return Initialized StateSpaceModel structure.
 */
StateSpaceModel create_state_space(int n, int m, int p);

/**
 * @brief Free all dynamically allocated memory in a state-space model.
 *
 * @param model Pointer to the StateSpaceModel to free.
 */
void free_state_space(StateSpaceModel* model);
