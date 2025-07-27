#include <stdio.h>
#include "matrix.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "state_space.h"

int main()
{
	StateSpaceModel model;

	model = create_state_space(2, 1, 1);
	matrix_set_identity(&(model.A));

	printf("model.C.cols: %f\n", model.A.data[2]);
}

