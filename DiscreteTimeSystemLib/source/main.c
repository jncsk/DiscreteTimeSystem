#include <stdio.h>
#include "matrix_core.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "state_space.h"
#include "bit_utils.h"

int main()
{
	//Define a structure of StateSpaceModel
	StateSpaceModel* model;

	MatrixCoreStatus status = 0;
	StateSpaceModel* model2;
	model2 = state_space_create(2, 1, 1, &status);


	//Initialize the structure
	model = state_space_create(2, 1, 1, &status);

	//Set values in the matrices
	matrix_ops_set_identity(&(model->A));

	// Practice using bit_utils_to_binary_lsb
	{
		int bits[32];
		int count = bit_utils_to_binary_lsb(19, bits, 32);

		printf("19 in binary: ");
		for (int i = 0; i < count; i++) {
			printf("%d", bits[i]);
		}
		printf("\n");
	}

	// Practice using matrix_ops_power
	{
		MatrixCoreStatus status = 0;
		Matrix* A = matrix_core_create(3, 3, &status);
		Matrix* result = matrix_core_create(3, 3, &status);

		matrix_ops_set(A, 0, 0, 1);
		matrix_ops_set(A, 0, 1, 2);
		matrix_ops_set(A, 0, 2, 2);

		matrix_ops_set(A, 1, 0, 3);
		matrix_ops_set(A, 1, 1, 4);
		matrix_ops_set(A, 1, 2, 3);

		matrix_ops_set(A, 2, 0, 2);
		matrix_ops_set(A, 2, 1, 3);
		matrix_ops_set(A, 2, 2, 5);

		matrix_ops_power(A, 4, result);
		matrix_ops_print(result);
	}

	// Practice using matrix_exp_exponential
	{
		MatrixCoreStatus status = 0;

		Matrix* A = matrix_core_create(3, 100, &status);
		MATRIX_CORE_HANDLE_ERR(status);

		Matrix* result = matrix_core_create(3, 3, &status);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 0, 0, 1);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 0, 1, 2);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 0, 2, 2);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 1, 0, 3);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 1, 1, 4);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 1, 2, 3);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 2, 0, 2);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 2, 1, 3);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(A, 2, 2, 5);
		MATRIX_CORE_HANDLE_ERR(status);

		matrix_exp_exponential(A, 3, result);
		MATRIX_CORE_HANDLE_ERR(status);
	}

	{
		Matrix* B = matrix_core_create_square(2, &status);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(B, 0, 0, 3);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(B, 0, 1, 2);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(B, 1, 0, 2);
		MATRIX_CORE_HANDLE_ERR(status);

		status = matrix_ops_set(B, 1, 1, 4);
		MATRIX_CORE_HANDLE_ERR(status);

		matrix_ops_print(B);
	}
}

