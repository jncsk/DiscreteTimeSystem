#pragma once

/*
 * =============================================================================
 *  bit_utils.h
 * =============================================================================
 *
 *  Description:
 *      This header provides utility functions for bit-level operations
 *      and binary representations of integers in C.
 *
 *  Features:
 *      - Convert an integer to its binary string representation
 *      - Print an integer in binary format
 *      - Decompose an integer's binary representation into an array of bits
 *
 * =============================================================================
 */

#include <stdio.h>
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
 * @brief Convert an unsigned integer to binary representation (LSB first).
 *
 * @param value     Unsigned integer to convert
 * @param bits      Output array for binary digits (0 or 1)
 * @param maxBits   Maximum number of bits to store
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus bit_utils_to_binary_lsb(uint32_t value, int* bits, int maxBits, int* out_len);

/**
 * @brief Convert an unsigned integer to binary representation (MSB first).
 *
 * @param value Unsigned integer to convert
 * @param bits Output array for binary digits (0 or 1)
 * @param maxBits Maximum number of bits to store
 * @param out_len the length of the bit of the given value
 * @return CORE_ERROR_SUCCESS if freed successfully, otherwise an error code.
 */
CoreErrorStatus bit_utils_to_binary_msb(uint32_t value, int* bits, int maxBits, int* out_len);

/**
 * @brief Print the binary representation of an unsigned integer to stdout.
 *
 * @param value            The unsigned integer to print.
 * @param bit_length    The number of bits to display in the output
 */
CoreErrorStatus bit_utils_print_binary(uint32_t value, int bit_length);

