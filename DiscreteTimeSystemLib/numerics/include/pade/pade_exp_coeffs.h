#pragma once

typedef struct {
    int m;
    const double* even;
    const double* odd;
    int even_len;
    int odd_len;
} PadeExpTable;

extern const PadeExpTable PADE_EXP_M3;
extern const PadeExpTable PADE_EXP_M5;
extern const PadeExpTable PADE_EXP_M7;
extern const PadeExpTable PADE_EXP_M9;
extern const PadeExpTable PADE_EXP_M13;

const PadeExpTable* pade_exp_get_table(int m);
