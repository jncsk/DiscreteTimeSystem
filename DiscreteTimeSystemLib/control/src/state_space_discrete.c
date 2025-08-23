#include "state_space_discrete.h"
#include "matrix_ops.h"
#include <string.h>

static CoreErrorStatus deep_copy_mat_opt(Matrix** dst, const Matrix* src) {
    if (!dst) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (!src) { *dst = NULL; CORE_ERROR_RETURN(CORE_ERROR_SUCCESS); }

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    Matrix* M = matrix_core_create(src->rows, src->cols, &status);
    if (status) CORE_ERROR_RETURN(status);

    status = matrix_ops_copy(M, src);
    if (status) { matrix_core_free(M); CORE_ERROR_RETURN(status); }
    *dst = M;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus ss_discrete_init_from_csys(SSDiscrete* out,
    const StateSpaceModel* sys,
    double Ts)
{
    if (!out || !sys || !sys->A || !sys->B) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (Ts < 0.0) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);

    memset(out, 0, sizeof(*out));
    out->n = sys->A->rows;
    out->m = sys->B->cols;
    out->p = (sys->C ? sys->C->rows : 0);
    out->Ts = Ts;

    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    out->Ad = matrix_core_create(out->n, out->n, &status); if (status) goto FAIL;
    out->Bd = matrix_core_create(out->n, out->m, &status); if (status) goto FAIL;

    status = state_space_c2d(sys, Ts, out->Ad, out->Bd); if (status) goto FAIL;

    status = deep_copy_mat_opt(&out->C, sys->C); if (status) goto FAIL;
    status = deep_copy_mat_opt(&out->D, sys->D); if (status) goto FAIL;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);

FAIL:
    ss_discrete_free(out);
    CORE_ERROR_RETURN(status);
}

CoreErrorStatus ss_discrete_init_from_mats(SSDiscrete* out,
    double Ts,
    const Matrix* Ad,
    const Matrix* Bd,
    const Matrix* C,
    const Matrix* D)
{
    if (!out || !Ad || !Bd) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (Ts < 0.0) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    if (Ad->rows != Ad->cols) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (Bd->rows != Ad->rows) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (C && C->cols != Ad->rows) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (D && (D->rows != (C ? C->rows : 0) || D->cols != Bd->cols)) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);

    memset(out, 0, sizeof(*out));
    out->n = Ad->rows;
    out->m = Bd->cols;
    out->p = C ? C->rows : 0;
    out->Ts = Ts;

    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    status = deep_copy_mat_opt(&out->Ad, Ad); if (status) goto FAIL;
    status = deep_copy_mat_opt(&out->Bd, Bd); if (status) goto FAIL;
    status = deep_copy_mat_opt(&out->C, C);  if (status) goto FAIL;
    status = deep_copy_mat_opt(&out->D, D);  if (status) goto FAIL;

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);

FAIL:
    ss_discrete_free(out);
    CORE_ERROR_RETURN(status);
}

CoreErrorStatus ss_discrete_free(SSDiscrete* out) {
    if (!out) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (out->Ad) matrix_core_free(out->Ad);
    if (out->Bd) matrix_core_free(out->Bd);
    if (out->C)  matrix_core_free(out->C);
    if (out->D)  matrix_core_free(out->D);
    memset(out, 0, sizeof(*out));
    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus ss_discrete_step_ws(const SSDiscrete* dsys,
    const Matrix* x_now,
    const Matrix* u_now,
    Matrix* x_next,
    Matrix* Ax_ws,
    Matrix* Bu_ws)
{
    if (!dsys || !dsys->Ad || !dsys->Bd || !x_now || !u_now || !x_next || !Ax_ws || !Bu_ws)
        CORE_ERROR_RETURN(CORE_ERROR_NULL);

    const int n = dsys->n, m = dsys->m;
    if (x_now->rows != n || x_now->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (u_now->rows != m || u_now->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (x_next->rows != n || x_next->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (Ax_ws->rows != n || Ax_ws->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (Bu_ws->rows != n || Bu_ws->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // Ax_ws = Ad * x_now
    status = matrix_ops_multiply(Ax_ws, dsys->Ad, x_now); if (status) CORE_ERROR_RETURN(status);

    // Bu_ws = Bd * u_now   (n~m) * (m~1) = (n~1)
    status = matrix_ops_multiply(Bu_ws, dsys->Bd, u_now); if (status) CORE_ERROR_RETURN(status);

    // x_next = Ax_ws + Bu_ws
    status = matrix_ops_add(x_next, Ax_ws, Bu_ws); if (status) CORE_ERROR_RETURN(status);

    CORE_ERROR_RETURN(CORE_ERROR_SUCCESS);
}

CoreErrorStatus ss_discrete_step(const SSDiscrete* dsys,
    const Matrix* x_now,
    const Matrix* u_now,
    Matrix* x_next)
{
    if (!dsys || !dsys->Ad || !dsys->Bd || !x_now || !u_now || !x_next)
        CORE_ERROR_RETURN(CORE_ERROR_NULL);

    CoreErrorStatus status = CORE_ERROR_SUCCESS;
    Matrix* Ax = NULL, * Bu = NULL;

    Ax = matrix_core_create(dsys->n, 1, &status); if (status) goto DONE;
    Bu = matrix_core_create(dsys->n, 1, &status); if (status) goto DONE;
    status = ss_discrete_step_ws(dsys, x_now, u_now, x_next, Ax, Bu);

DONE:
    if (Bu) matrix_core_free(Bu);
    if (Ax) matrix_core_free(Ax);
    CORE_ERROR_RETURN(status);
}

CoreErrorStatus ss_discrete_step_scalar_u(const SSDiscrete* dsys,
    const Matrix* x_now,
    double u_now,
    Matrix* x_next)
{
    if (!dsys || !dsys->Ad || !dsys->Bd || !x_now || !x_next)
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (dsys->m != 1) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);

    const int n = dsys->n;
    if (x_now->rows != n || x_now->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (x_next->rows != n || x_next->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // x_next = Ad * x_now
    status = matrix_ops_multiply(x_next, dsys->Ad, x_now); if (status) CORE_ERROR_RETURN(status);

    // x_next += u * Bd(:,0)
    // Use temporary copy of Bd(:,0)
    Matrix* Bu = matrix_core_create(n, 1, &status); if (status) CORE_ERROR_RETURN(status);

    // Bu = Bd(:,0)
    for (int r = 0; r < n; ++r) Bu->data[r] = dsys->Bd->data[r * dsys->m + 0];

    status = matrix_ops_scale(Bu, u_now); if (status) { matrix_core_free(Bu); CORE_ERROR_RETURN(status); }
    status = matrix_ops_add(x_next, x_next, Bu);
    matrix_core_free(Bu);
    CORE_ERROR_RETURN(status);
}

CoreErrorStatus ss_discrete_output(const SSDiscrete* dsys,
    const Matrix* x_now,
    const Matrix* u_now,
    Matrix* y_out)
{
    if (!dsys || !x_now || !u_now || !y_out) CORE_ERROR_RETURN(CORE_ERROR_NULL);
    if (!dsys->C || !dsys->D) CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);

    const int p = dsys->p, n = dsys->n, m = dsys->m;
    if (x_now->rows != n || x_now->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (u_now->rows != m || u_now->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);
    if (y_out->rows != p || y_out->cols != 1) CORE_ERROR_RETURN(CORE_ERROR_DIMENSION);

    CoreErrorStatus status = CORE_ERROR_SUCCESS;

    // y = C x + D u
    status = matrix_ops_multiply(y_out, dsys->C, x_now); if (status) CORE_ERROR_RETURN(status);

    Matrix* Du = matrix_core_create(p, 1, &status); if (status) CORE_ERROR_RETURN(status);
    status = matrix_ops_multiply(Du, dsys->D, u_now); if (status) { matrix_core_free(Du); CORE_ERROR_RETURN(status); }

    status = matrix_ops_add(y_out, y_out, Du);
    matrix_core_free(Du);
    CORE_ERROR_RETURN(status);
}
