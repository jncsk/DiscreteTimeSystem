#pragma once

#include <stdio.h>
#include "matrix_core.h"
#include "matrix_exp.h"
#include "matrix_ops.h"
#include <math.h>

void matrix_exp_exponential(const Matrix* A, double t, Matrix* result)
{
	if (A == NULL || result == NULL) {
		CORE_ERROR_SET(CORE_ERROR_NULL);
		return CORE_ERROR_NULL;
	}
	if (A->rows != A->cols) {
		CORE_ERROR_SET(CORE_ERROR_DIMENSION);
		return CORE_ERROR_DIMENSION;
	}

	int k = 0;

	int status = 0;
	Matrix* At = matrix_core_create(3, 3, &status);
	status = matrix_ops_set_zero(At);
	if (status != CORE_ERROR_SUCCESS)
	{
		CORE_ERROR_SET(status);
		return status;
	}

	for (int i = 0; i < A->rows; i++)
		for (int j = 0; j < A->cols; j++) {
			{
				printf("%d\n", i);
				matrix_ops_set(At, i, j, matrix_ops_get(A, i, j, &status) * t);
				if (status != CORE_ERROR_SUCCESS) {
					CORE_ERROR_SET(status);
					return status;
				}
			}

			matrix_ops_print(A);
			matrix_ops_print(At);

		}
	return;

}


