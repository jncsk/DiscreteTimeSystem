#include <stdio.h>
#include "core_matrix.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "matrix_norm.h"
#include "state_space.h"
#include "bit_utils.h"
#include "pade.h"
#include "pade_scaling.h"

#define DEBUG_STATE_SPACE 0
#define DEBUG_MATRIX_NORM 0
#define DEBUG_PADE 1

int main()
{	
	if (DEBUG_STATE_SPACE)
	{
		CoreErrorStatus status = CORE_ERROR_SUCCESS;
		StateSpaceModel* matrix = state_space_create(3, 1, 1, &status);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		printf("=======matrix->A=========\n");
		status = matrix_ops_set(matrix->A, 0, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 0, 1, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 0, 2, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 1, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 1, 1, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 1, 2, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 2, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 2, 1, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->A, 2, 2, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		status = matrix_ops_print(matrix->A);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		printf("=======matrix->B=========\n");
		status = matrix_ops_set(matrix->B, 0, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->B, 1, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->B, 2, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		status = matrix_ops_print(matrix->B);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		printf("=======matrix->C=========\n");
		status = matrix_ops_set(matrix->C, 0, 0, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->C, 0, 1, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(matrix->C, 0, 2, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_print(matrix->C);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		printf("=======matrix_exp_exponential=========\n");
		Matrix* result = matrix_core_create_square(3, &status);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		status = matrix_ops_set_zero(result);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		status = matrix_exp_exponential(matrix->A, 2, result);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
	}
	if (DEBUG_MATRIX_NORM) 
	{
		printf("=======matrix_norm=========\n");
		CoreErrorStatus status = CORE_ERROR_SUCCESS;
		Matrix* mat = matrix_core_create_square(2, &status);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		status = matrix_ops_set(mat, 0, 0, 2);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(mat, 0, 1, 2);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(mat, 1, 0, 2);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set(mat, 1, 1, 3);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		double res = 0;
		status = matrix_norm_1(mat, &res);
		status = matrix_norm_inf(mat, &res);
		status = matrix_norm_fro(mat, &res);
		printf("res; %f\n", res);
	}

	if (DEBUG_PADE)
	{
		printf("=======matrix_norm=========\n");
		CoreErrorStatus status = CORE_ERROR_SUCCESS;
		Matrix* mat = matrix_core_create_square(3, &status);
		status = matrix_ops_fill_sequential(mat, 1, 1);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		Matrix* res = matrix_core_create_square(3, &status);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}
		status = matrix_ops_set_zero(res);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		double anorm = 0;

		status = pade_expm(mat, res);
		if (status != CORE_ERROR_SUCCESS) {
			CORE_ERROR_PRINT_CALL_AND_LAST(status);
		}

		matrix_ops_print(res);
	}
	printf("hello world!\n");
}

