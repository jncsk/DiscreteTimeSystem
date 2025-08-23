#include <stdio.h>
#include "core_matrix.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "matrix_norm.h"
#include "state_space.h"
#include "state_space_c2d.h"
#include "bit_utils.h"
#include "pade.h"
#include "pade_scaling.h"
#include "app_motor.h"
#include "state_space_discrete.h"


#define DEBUG_STATE_SPACE 0
#define DEBUG_MATRIX_NORM 0
#define DEBUG_PADE 0
#define DEBUG_MATRIX_SET_BLOCK 0
#define DEBUG_APP_MOTOR 1

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

		status = matrix_ops_print(matrix->A, NULL);
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

		status = matrix_ops_print(matrix->B, NULL);
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
		status = matrix_ops_print(matrix->C, NULL);
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

		matrix_ops_print(res, NULL);
	}
	if (DEBUG_MATRIX_SET_BLOCK)
	{
		CoreErrorStatus status;
		StateSpaceModel* sys = state_space_create(3, 1, 1, &status);

		status = matrix_ops_set(sys->A, 0, 0, 0);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 0, 1, 2);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 0, 2, 0);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 1, 0, 2);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 1, 1, 0);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 1, 2, 1);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 2, 0, 0);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 2, 1, 1);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->A, 2, 2, -1);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->B, 0, 0, 0.5);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->B, 1, 0, -1);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set(sys->B, 2, 0, 0);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(sys->A, NULL);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(sys->B, NULL);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		Matrix* Ad = matrix_core_create(sys->A->rows, sys->A->cols, &status);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		Matrix* Bd = matrix_core_create(sys->B->rows, sys->B->cols, &status);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
				
		status = state_space_c2d(sys, 1, Ad, Bd);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(Ad, NULL);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(Bd, NULL);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
	}
	if (DEBUG_APP_MOTOR) {
		CoreErrorStatus st = CORE_ERROR_SUCCESS;

		// パラメータとモデル生成
		DCMotorParams* p = motor_set_params(1, 0, 1, 1, 1);
		StateSpaceModel* sys = motor_create(p, &st);
		if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP; }

		// A(1,1) = -1 を明示セット（0-based）
		st = matrix_ops_set(sys->A, 1, 1, -1.0);
		if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP; }

		// 離散モデル作成
		SSDiscrete d = { 0 };
		st = ss_discrete_init_from_csys(&d, sys, 1.0);
		if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP; }

		// 確認用に Ad, Bd を出力
		matrix_ops_print(d.Ad, "Ad");
		matrix_ops_print(d.Bd, "Bd");

		// 状態と入力
		Matrix* x = matrix_core_create(d.n, 1, &st); if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP2; }
		Matrix* xn = matrix_core_create(d.n, 1, &st); if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP3; }
		Matrix* u = matrix_core_create(d.m, 1, &st); if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP4; }
		st = matrix_ops_set_zero(x);                   if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP5; }

		for (int k = 0; k < 8; ++k) {
			double uk = (k == 0) ? 1.0 : (k == 1) ? 2.0 : 0.0;
			u->data[0] = uk;

			st = ss_discrete_step(&d, x, u, xn);
			if (st) { CORE_ERROR_PRINT_CALL_AND_LAST(st); goto CLEANUP5; }

			printf("step: %d, u=%.6f\n", k, uk);
			matrix_ops_print(xn, "x_next");

			// 次のステップへ
			Matrix* tmp = x; x = xn; xn = tmp;
		}

	CLEANUP5:
		matrix_core_free(u);
	CLEANUP4:
		matrix_core_free(xn);
	CLEANUP3:
		matrix_core_free(x);
	CLEANUP2:
		ss_discrete_free(&d);
	CLEANUP:
		state_space_free(sys);
		// motor_set_params() の解放が必要ならここで
	}
	printf("hello world!\n");

}

