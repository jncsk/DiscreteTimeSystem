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

