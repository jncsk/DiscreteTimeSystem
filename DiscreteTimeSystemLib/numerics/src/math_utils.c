#pragma once

#include "math_utils.h"

CoreErrorStatus math_utils_factorial(uint32_t n, uint64_t* result) {
    if (!result) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    // Factorials from 0! to 20! fit within uint64_t. Inputs resulting in overflow (21! and beyond) are not allowed.
    if (n > 20) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    uint64_t r = 1;
    for (uint32_t i = 2; i <= n; i++) {
        r *= i;
    }
    *result = r;
    return CORE_ERROR_SUCCESS;
}