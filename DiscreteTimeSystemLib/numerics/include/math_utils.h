#pragma once

/*
 * =============================================================================
 *  math_utils.h
 * =============================================================================
 *
 *  Description:
 *      This header provides utility functions for general mat operations in C.
 *
 *  Features:
 *      - Calculate factorial for a given non-negative integer.
 * 
 * =============================================================================
 */

#include <stdint.h>
#include "core_error.h"

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
 * @brief Calculate the factorial of a non-negative integer (n!). 
 * @param n Non-negative Integer input (0 <= n <= 20 for unit64_t range)
 * @param result Pointer to store the calculation result
 * @return CORE_ERROR_SUCCESS if successful, otherwise an error code
 */
CoreErrorStatus math_utils_factorial(uint32_t n, uint64_t* result);
