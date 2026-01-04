#pragma once

#include "pade_exp_coeffs.h"
#include "core_error.h"
#include "core_matrix.h"
#include <stdio.h>

/*-------------------------------------------
 *  Coefficient base arrays (b0..b_{2m})
 *  NOTE: keep as static const to avoid ODR.
 *------------------------------------------*/

 /* m=3 coefficients (b0..b3) */
static const double PADE3_B[] = {
    120.0, 
    60.0, 
    12.0, 
    1.0
};

/* m=5 coefficients (b0..b5) */
static const double PADE5_B[] = {
    30240.0, 
    15120.0, 
    3360.0,
    420.0,
    30.0,
    1.0
};

/* m=7 coefficients (b0..b7) */
static const double PADE7_B[] = {
    17297280.0, 
    8648640.0, 
    1995840.0, 
    277200.0, 
    25200.0, 
    1512.0, 
    56.0, 
    1.0
};

/* m=9 coefficients (b0..b9) */
static const double PADE9_B[] = {
    17643225600.0, 
    8821612800.0, 
    2075673600.0, 
    302702400.0, 
    30270240.0,
    2162160.0, 
    110880.0, 
    3960.0, 
    90.0, 
    1.0
};

/* m=13 coefficients (b0..b13) */
static const double PADE13_B[] = {
    64764752532480000.0, 
    32382376266240000.0, 
    7771770303897600.0,
    1187353796428800.0, 
    129060195264000.0, 
    10559470521600.0,
    670442572800.0, 
    33522128640.0, 
    1323241920.0, 
    40840800.0,
    960960.0, 
    16380.0, 
    182.0, 
    1.0
};

/*-------------------------------------------
 *  Even / Odd split (use literals; no [] refs)
 *------------------------------------------*/

 /* m=3 split */
static const double PADE3_EVEN[] = { 120.0, 12.0 };   /* b0, b2 */
static const double PADE3_ODD[] = { 60.0,  1.0 };   /* b1, b3 */
const PadeExpTable PADE_EXP_M3 = { 3, PADE3_EVEN, PADE3_ODD, 2, 2 };

/* m=5 split */
static const double PADE5_EVEN[] = { 30240.0, 3360.0, 30.0 };   /* b0,b2,b4 */
static const double PADE5_ODD[] = { 15120.0, 420.0, 1.0 };     /* b1,b3,b5 */
const PadeExpTable PADE_EXP_M5 = { 5, PADE5_EVEN, PADE5_ODD, 3, 3 };

/* m=7 split */
static const double PADE7_EVEN[] = { 17297280.0, 1995840.0, 25200.0, 56.0 };
static const double PADE7_ODD[] = { 8648640.0,  277200.0,  1512.0, 1.0 };
const PadeExpTable PADE_EXP_M7 = { 7, PADE7_EVEN, PADE7_ODD, 4, 4 };

/* m=9 split */
static const double PADE9_EVEN[] = { 17643225600.0, 2075673600.0, 30270240.0, 110880.0, 90.0 };
static const double PADE9_ODD[] = { 8821612800.0,  302702400.0,   2162160.0,   3960.0,  1.0 };
const PadeExpTable PADE_EXP_M9 = { 9, PADE9_EVEN, PADE9_ODD, 5, 5 };

/* m=13 split */
static const double PADE13_EVEN[] = {
    64764752532480000.0, 7771770303897600.0, 129060195264000.0,
    670442572800.0, 1323241920.0, 960960.0, 182.0
};
static const double PADE13_ODD[] = {
    32382376266240000.0, 1187353796428800.0, 10559470521600.0,
    33522128640.0, 40840800.0, 16380.0, 1.0
};
const PadeExpTable PADE_EXP_M13 = { 13, PADE13_EVEN, PADE13_ODD, 7, 7 };

/*-------------------------------------------
 *  Selector
 *------------------------------------------*/
const PadeExpTable* pade_exp_get_table(int m) {
    switch (m) {
    case 3:  return &PADE_EXP_M3;
    case 5:  return &PADE_EXP_M5;
    case 7:  return &PADE_EXP_M7;
    case 9:  return &PADE_EXP_M9;
    case 13: return &PADE_EXP_M13;
    default: return NULL;
    }
}
