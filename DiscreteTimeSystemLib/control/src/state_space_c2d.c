
#include "state_space_c2d.h"
#include "matrix_ops.h"
#include "matrix_exp.h"
#include "pade.h"

CoreErrorStatus state_space_c2d(const StateSpaceModel* sys, double Ts, Matrix* Ad, Matrix* Bd) {
	if (!sys || !sys->A || !sys->B || !Ad || !Bd)
		CORE_ERROR_RETURN(CORE_ERROR_NULL);

	const int n = sys->A->rows;
	const int m = sys->B->cols;

	if (sys->A->cols != n || sys->B->rows != n) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
	if (Ad->rows != n || Ad->cols != n) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
	if (Bd->rows != n || Bd->cols != m) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);

	CoreErrorStatus status;

	if (Ts == 0.0) {
		status = matrix_ops_set_identity(Ad); if (status) CORE_ERROR_RETURN(status);
		status = matrix_ops_set_zero(Bd); if (status) CORE_ERROR_RETURN(status);
		CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
	}

	Matrix* M = NULL, * E = NULL;
	
	// M: (n+m) x (n+m)
	M = matrix_core_create(n + m, n + m, &status);
	if (status) CORE_ERROR_RETURN(status);

	status = matrix_ops_set_zero(M);                                            if (status) goto FAIL;
	status = matrix_ops_set_block(M, 0, 0, sys->A);                      if (status) goto FAIL;
	status = matrix_ops_set_block(M, 0, sys->A->cols, sys->B);   if (status) goto FAIL;

	// E = exp(M)
	E = matrix_core_create(M->rows, M->cols, &status);             if (status) goto FAIL;
	status = matrix_exp_exponential(M, Ts, E);                              if (status) goto FAIL;

	// Ad = E(0:n-1, 0:n-1)
	status = matrix_ops_get_block(E, 0, 0, Ad);                              if (status) goto FAIL;

	// Bd = E(0:n-1, n:n+m-1)
	status = matrix_ops_get_block(E, 0, sys->A->cols, Bd);           if (status) goto FAIL;

	status = matrix_core_free(M);
	status = matrix_core_free(E);
	CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);

FAIL:
	if (E) matrix_core_free(E);
	if (M) matrix_core_free(M);
	CORE_ERROR_RETURN(status);
}
