#pragma once
#include "core_error.h"
#include "core_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief Solve A * X = B using LU with partial pivoting (no inverse formed).
	 *
	 * @param[in]  A  Coefficient square matrix (n x n). Not modified.
	 * @param[out] X  Solution matrix (n x nrhs). Overwritten with the solution.
	 * @param[in]  B  Right-hand side matrix (n x nrhs). Not modified.
	 *
	 * @return CORE_ERROR_SUCCESS on success, otherwise an error code.
	 *
	 * @note Internally creates a working copy LU <- A and factorizes it in-place.
	 *       Supports multiple RHS: nrhs = B->cols = X->cols.
	 */
	CoreErrorStatus matrix_solve_LU(const Matrix* A, Matrix* X, const Matrix* B);

#ifdef __cplusplus
}
#endif
