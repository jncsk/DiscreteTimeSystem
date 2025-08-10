
#include <stdlib.h>
#include "matrix_core.h"
#include "state_space.h"


StateSpaceModel* state_space_create(int n, int m, int p, MatrixCoreStatus* err)
{
    StateSpaceModel* model = (StateSpaceModel*)malloc(sizeof(StateSpaceModel));

    if (model == NULL) {
        *err = MATRIX_CORE_ERR_ALLOCATION_FAILED;
        return NULL;
    }

    MatrixCoreStatus status = 0;
    model->A = *matrix_core_create(n, n, &status);
    if (status != MATRIX_CORE_SUCCESS) {
        free(model);
        *err = status;
        return NULL;
    }

    model->B = *matrix_core_create(n, m, &status);
    if (status != MATRIX_CORE_SUCCESS) {
        matrix_core_free(&model->A);
        free(model);
        *err = status;
        return NULL;
    }

    model->C = *matrix_core_create(p, n, &status);
    if (status != MATRIX_CORE_SUCCESS) {
        matrix_core_free(&model->A);
        matrix_core_free(&model->B);
        free(model);
        *err = status;
        return NULL;
    }

    *err = MATRIX_CORE_SUCCESS;
    return model;
}

void state_space_free(StateSpaceModel* model) {
    matrix_core_free(&model->A);
    matrix_core_free(&model->B);
    matrix_core_free(&model->C);
}