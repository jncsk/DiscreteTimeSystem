#pragma once
#include "core_matrix.h"

/*
 * =============================================================================
 *  pade.h
 * =============================================================================
 *
 *  Description:
 *      Matrix exponential via Scaling & Squaring with Pade approximation
 *      (Higham-style implementation). This API computes exp(A) for a dense
 *      real matrix A without forming the power series explicitly.
 *
 *  Features:
 *      - Supports arbitrary n x n real matrices
 *      - Numerically stable using scaling & squaring and (m=3,5,7,9,13) Pade
 *      - Zero-allocation interface for the output (caller allocates result)
 *      - Propagates well-defined error codes on invalid inputs or singularities
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

CoreErrorStatus pade_expm(const Matrix* A, Matrix* result);
