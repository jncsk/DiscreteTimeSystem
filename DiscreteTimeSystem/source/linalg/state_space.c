
#include <stdlib.h>
#include "state_space.h"

StateSpaceModel create_state_space(int n, int m, int p) {
    StateSpaceModel model;
    model.A = matrix_create(n, n);
    model.B = matrix_create(n, m);
    model.C = matrix_create(p, n);
    return model;
}

void free_state_space(StateSpaceModel* model) {
    matrix_free(&model->A);
    matrix_free(&model->B);
    matrix_free(&model->C);
}