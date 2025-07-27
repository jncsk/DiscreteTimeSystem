#include <stdio.h>
#include "matrix.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "state_space.h"

int main()
{
	//Define a structure of StateSpaceModel
	StateSpaceModel model;

	StateSpaceModel model2;
	model2 = state_space_create(2, 1, 1);


	//Initialize the structure
	model = state_space_create(2, 1, 1);

	//Set values in the matrices
	matrix_set_identity(&(model.A));

	//Practice using matrix_multiply
	{
		matrix_set(&model.A, 0, 0, 1);
		matrix_set(&model.A, 0, 1, 2);
		matrix_set(&model.A, 1, 0, 3);
		matrix_set(&model.A, 1, 1, 4);

		matrix_set(&model.B, 0, 0, 2);
		matrix_set(&model.B, 1, 0, 3);

		matrix_set_zero(&model.C);

		matrix_multiply(&model.A, &model.B, &model2.B);

		for (int i = 0; i < (model.A.cols * model.A.rows); i++)
		{
			printf("model.A.data[%d]: %f\n", i, model.A.data[i]);
		}
		for (int i = 0; i < (model.B.cols * model.B.rows); i++)
		{
			printf("model.B.data[%d]: %f\n", i, model2.B.data[i]);
		}
	}
}

