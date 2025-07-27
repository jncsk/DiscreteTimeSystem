#include <stdio.h>
#include "matrix.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "state_space.h"

int main()
{
	//Define a structure of StateSpaceModel
	StateSpaceModel model;

	//Initialize the structure
	model = state_space_create(4, 1, 1);

	//Set values in the matrices
	matrix_set_identity(&(model.A));
	matrix_set_zero(&model.B);
	matrix_set_zero(&model.C);

	for (int i = 0; i < (model.A.cols * model.A.rows); i++)
	{
		printf("model.A.data[%d]: %f\n",i, model.A.data[i]);
	}

}

