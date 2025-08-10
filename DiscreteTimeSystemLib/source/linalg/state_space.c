
#include "matrix_core.h"
#include "matrix_ops.h"
#include <stdlib.h>
#include "state_space.h"

StateSpaceModel* state_space_create(int n, int m, int p, MatrixCoreStatus* err)
{
    MatrixCoreStatus local = MATRIX_CORE_SUCCESS;
    if (!err) err = &local;

    if (n <= 0 || m <= 0 || p <= 0) {
        *err = MATRIX_CORE_ERR_OUT_OF_BOUNDS;
        MATRIX_CORE_SET_ERROR(*err);
        return NULL;
    }

    StateSpaceModel* model = (StateSpaceModel*)calloc(1, sizeof(StateSpaceModel));
    if (!model) {
        *err = MATRIX_CORE_ERR_ALLOCATION_FAILED;
        MATRIX_CORE_SET_ERROR(*err);
        return NULL;
    }

    // A: n x n
    model->A = matrix_core_create(n, n, err);
    if (*err != MATRIX_CORE_SUCCESS || !model->A) {
        state_space_free(model);  // safe: B,C ‚Í NULL
        return NULL;
    }

    // B: n x m
    model->B = matrix_core_create(n, m, err);
    if (*err != MATRIX_CORE_SUCCESS || !model->B) {
        state_space_free(model);  // A ‚ÍŠm•ÛÏ‚Ý
        return NULL;
    }

    // C: p x n
    model->C = matrix_core_create(p, n, err);
    if (*err != MATRIX_CORE_SUCCESS || !model->C) {
        state_space_free(model);  // A,B ‚ÍŠm•ÛÏ‚Ý
        return NULL;
    }

    *err = MATRIX_CORE_SUCCESS;
    return model;
}

MatrixCoreStatus state_space_free(StateSpaceModel* model)
{
    if (!model) {
        RETURN_ERROR(MATRIX_CORE_ERR_NULL);
    }

    MatrixCoreStatus first_err = MATRIX_CORE_SUCCESS;

    if (model->A) {
        MatrixCoreStatus s = matrix_core_free(model->A);
        if (first_err == MATRIX_CORE_SUCCESS && s != MATRIX_CORE_SUCCESS) first_err = s;
        model->A = NULL;
    }
    if (model->B) {
        MatrixCoreStatus s = matrix_core_free(model->B);
        if (first_err == MATRIX_CORE_SUCCESS && s != MATRIX_CORE_SUCCESS) first_err = s;
        model->B = NULL;
    }
    if (model->C) {
        MatrixCoreStatus s = matrix_core_free(model->C);
        if (first_err == MATRIX_CORE_SUCCESS && s != MATRIX_CORE_SUCCESS) first_err = s;
        model->C = NULL;
    }

    free(model);

    if (first_err != MATRIX_CORE_SUCCESS) {
        RETURN_ERROR(first_err);
    }
    return MATRIX_CORE_SUCCESS;
}