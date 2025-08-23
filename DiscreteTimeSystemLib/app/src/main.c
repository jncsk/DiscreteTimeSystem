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


#define DEBUG_STATE_SPACE 0
#define DEBUG_MATRIX_NORM 0
#define DEBUG_PADE 0
#define DEBUG_MATRIX_SET_BLOCK 0
#define DEBUG_APP_MOTOR 0

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

		status = matrix_ops_print(sys->A);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(sys->B);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		Matrix* Ad = matrix_core_create(sys->A->rows, sys->A->cols, &status);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		Matrix* Bd = matrix_core_create(sys->B->rows, sys->B->cols, &status);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
				
		status = state_space_c2d(sys, 1, Ad, Bd);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(Ad);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_print(Bd);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
	}
	
	if (DEBUG_APP_MOTOR) {
		CoreErrorStatus status = CORE_ERROR_SUCCESS;

		// パラメータとモデル生成
		DCMotorParams* p = motor_set_params(1, 0, 1, 1, 1);
		StateSpaceModel* sys = motor_create(p, &status);
		if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		matrix_ops_set(sys->A, 1, 1, -1.0);
		matrix_ops_print(sys->A);
		matrix_ops_print(sys->B);
		matrix_ops_print(sys->C);

		// 状態と一時バッファ
		Matrix* x_now = matrix_core_create(2, 1, &status); if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
		Matrix* x_next = matrix_core_create(2, 1, &status); if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
		Matrix* Ax       = matrix_core_create(2, 1, &status); if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
		Matrix* Bu       = matrix_core_create(2, 1, &status); if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		status = matrix_ops_set_zero(x_now); if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);
		status = matrix_ops_set_zero(x_next); if (status) CORE_ERROR_PRINT_CALL_AND_LAST(status);

		// 離散化（Ts=1.0 はかなり大きいので注意。例としてそのまま）
		Matrix* Ad = matrix_core_create(sys->A->rows, sys->A->cols, &status);
		Matrix* Bd = matrix_core_create(sys->B->rows, sys->B->cols, &status);

		status = state_space_c2d(sys, 1, Ad, Bd);
		if (status) { CORE_ERROR_PRINT_CALL_AND_LAST(status); goto CLEANUP; }

		matrix_ops_print(Ad);
		matrix_ops_print(Bd);

		// 時間ステップ
		double u = 0.0;

		for (int k = 0; k < 50; k++) {
			// 入力シーケンス
			if (k == 0) u = 1.0;
			else if (k == 1) u = 2.0;
			else u = 0.0;

			// Ax = Ad * x_now
			status = matrix_ops_multiply(Ax, Ad, x_now);
			if (status) { CORE_ERROR_PRINT_CALL_AND_LAST(status); goto CLEANUP; }

			// Bu = Bd * u   （毎回 Bd をコピーしてからスケール）
			status = matrix_ops_copy(Bd, Bu);
			if (status) { CORE_ERROR_PRINT_CALL_AND_LAST(status); goto CLEANUP; }
			status = matrix_ops_scale(Bu, u);     // in-place: Bu ← u * Bu
			if (status) { CORE_ERROR_PRINT_CALL_AND_LAST(status); goto CLEANUP; }

			// x_next = Ax + Bu
			status = matrix_ops_add(Ax, Bu, x_next); // シグネチャが C=A+B の場合
			if (status) { CORE_ERROR_PRINT_CALL_AND_LAST(status); goto CLEANUP; }

			printf("time: %d\n", k);
			matrix_ops_print(x_now);

			// x_now ← x_next （ポインタスワップが速くて安全）
			Matrix* tmp = x_now;
			x_now = x_next;
			x_next = tmp;
		}

	CLEANUP:
		matrix_core_free(Bd);
		matrix_core_free(Ad);
		matrix_core_free(Bu);
		matrix_core_free(Ax);
		matrix_core_free(x_next);
		matrix_core_free(x_now);
		state_space_free(sys);

	CLEANUP_EARLY:
		; // no-op
	}
	printf("hello world!\n");
}

