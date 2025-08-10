
#include "matrix_core.h"
#include "matrix_ops.h"
#include <stdlib.h>
#include "state_space.h"

StateSpaceModel* state_space_create(int n, int m, int p, CoreErrorStatus* err)
{
    CoreErrorStatus local = CORE_ERROR_SUCCESS;
    if (!err) err = &local;

    if (n <= 0 || m <= 0 || p <= 0) {
        *err = CORE_ERROR_OUT_OF_BOUNDS;
        CORE_ERROR_SET(*err);
        return NULL;
    }

    StateSpaceModel* model = (StateSpaceModel*)calloc(1, sizeof(StateSpaceModel));
    if (!model) {
        *err = CORE_ERROR_ALLOCATION_FAILED;
        CORE_ERROR_SET(*err);
        return NULL;
    }

    // A: n x n
    model->A = matrix_core_create(n, n, err);
    if (*err != CORE_ERROR_SUCCESS || !model->A) {
        state_space_free(model);  // safe: B,C ‚Í NULL
        return NULL;
    }

    // B: n x m
    model->B = matrix_core_create(n, m, err);
    if (*err != CORE_ERROR_SUCCESS || !model->B) {
        state_space_free(model);  // A ‚ÍŠm•ÛÏ‚Ý
        return NULL;
    }

    // C: p x n
    model->C = matrix_core_create(p, n, err);
    if (*err != CORE_ERROR_SUCCESS || !model->C) {
        state_space_free(model);  // A,B ‚ÍŠm•ÛÏ‚Ý
        return NULL;
    }

    *err = CORE_ERROR_SUCCESS;
    return model;
}

CoreErrorStatus state_space_free(StateSpaceModel* model)
{
    if (!model) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }

    CoreErrorStatus first_err = CORE_ERROR_SUCCESS;

    if (model->A) {
        CoreErrorStatus s = matrix_core_free(model->A);
        if (first_err == CORE_ERROR_SUCCESS && s != CORE_ERROR_SUCCESS) first_err = s;
        model->A = NULL;
    }
    if (model->B) {
        CoreErrorStatus s = matrix_core_free(model->B);
        if (first_err == CORE_ERROR_SUCCESS && s != CORE_ERROR_SUCCESS) first_err = s;
        model->B = NULL;
    }
    if (model->C) {
        CoreErrorStatus s = matrix_core_free(model->C);
        if (first_err == CORE_ERROR_SUCCESS && s != CORE_ERROR_SUCCESS) first_err = s;
        model->C = NULL;
    }

    free(model);

    if (first_err != CORE_ERROR_SUCCESS) {
        CORE_ERROR_RETURN(first_err);
    }
    return CORE_ERROR_SUCCESS;
}