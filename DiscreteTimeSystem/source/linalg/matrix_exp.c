#pragma once

#include <stdio.h>
#include "matrix.h"
#include "matrix_exp.h"
#include "matrix_ops.h"
#include <math.h>

void matrix_exp_exponential(const Matrix* A, double t, Matrix* result)
{
	if (A == NULL || result == NULL)
	{
		MATRIX_CORE_ERR_MESSAGE(MATRIX_CORE_ERR_NULL);
		return;
	}
	if (A->rows != A->cols) {
		MATRIX_CORE_ERR_MESSAGE(MATRIX_CORE_ERR_DIMENSION);
		return;
	}

	int k = 0;

	int status = 0;
	Matrix* At = matrix_create(3, 3, &status);
	status = matrix_ops_set_zero(At);
	if (status != MATRIX_OPS_SUCCESS)
	{
		MATRIX_CORE_ERR_MESSAGE(status);
	}

	for (int i = 0; i < A->rows; i++)
		for (int j = 0; j < A->cols; j++) {
			{
				printf("%d\n", i);
				matrix_ops_set(At, i, j, matrix_ops_get(A, i, j, &status) * t);
				if (status != MATRIX_OPS_SUCCESS) {
					MATRIX_CORE_ERR_MESSAGE(status);
				}
			}

			matrix_ops_print(A);
			matrix_ops_print(At);

		}
	return;

}


