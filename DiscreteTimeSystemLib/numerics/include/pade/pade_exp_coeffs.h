#pragma once

/*
 * =============================================================================
 *  pade_exp_coeffs.h
 * =============================================================================
 *
 *  Description:
 *      Precomputed Padé approximant coefficients for the matrix exponential.
 *
 *  Features:
 *      - Provides coefficient tables for [m/m] Padé approximants with m = 3,5,7,9,13
 *      - Coefficients split into even and odd parts for efficient Horner evaluation
 *      - Immutable global constants (extern declarations here, defined in .c file)
 *      - Access helper `pade_exp_get_table(m)` for lookup
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
/**
 * @brief Structure holding Padé approximant coefficients for exp(x).
 *
 * For exp(x), the [m/m] Padé approximant is:
 *   r_m(x) = φ_even(x) / φ_odd(x)
 *
 * where coefficients are separated into even and odd terms
 * (see Higham, "Functions of Matrices", SIAM 2008).
 */
typedef struct {
    int m;
    const double* even;
    const double* odd;
    int even_len;
    int odd_len;
} PadeExpTable;

//------------------------------------------------
//  Extern coefficient tables
//------------------------------------------------
extern const PadeExpTable PADE_EXP_M3;
extern const PadeExpTable PADE_EXP_M5;
extern const PadeExpTable PADE_EXP_M7;
extern const PadeExpTable PADE_EXP_M9;
extern const PadeExpTable PADE_EXP_M13;

//------------------------------------------------
//  Function Prototypes
//------------------------------------------------

/**
 * @brief Retrieve Padé coefficient table for given order m.
 *
 * @param[in] m  Order of Padé approximant (supported: 3, 5, 7, 9, 13).
 * @return Pointer to coefficient table if available, otherwise NULL.
 */
const PadeExpTable* pade_exp_get_table(int m);
