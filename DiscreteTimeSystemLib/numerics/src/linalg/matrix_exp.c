#include <stdio.h>
#include <math.h>
#include "matrix_exp.h"
#include "matrix_ops.h"
#include "math_utils.h"

CoreErrorStatus matrix_exp_exponential(const Matrix* A, double t, Matrix* result)
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
	CoreErrorStatus status = CORE_ERROR_SUCCESS;

	return CORE_ERROR_SUCCESS;
}


