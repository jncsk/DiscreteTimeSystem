#include "integrators/rk4.h"
#include "matrix_ops.h"
#include <string.h>

static CoreErrorStatus _check_vec(const Matrix* v, int n) {
    if (!v) return CORE_ERROR_NULL;
    if (v->rows != n || v->cols != 1) return CORE_ERROR_DIMENSION;
    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus rk4_step_ws(RkOdeFunc f,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    void* params,
    Matrix* x_next,
    Matrix* k1, Matrix* k2, Matrix* k3, Matrix* k4,
    Matrix* tmp)
{
    if (!f || !x_now || !x_next || !k1 || !k2 || !k3 || !k4 || !tmp)
        return CORE_ERROR_NULL;
    if (h <= 0.0) return CORE_ERROR_INVALID_ARG;
    const int n = x_now->rows;
    if (x_now->cols != 1) return CORE_ERROR_DIMENSION;
    if (x_next->rows != n || x_next->cols != 1) return CORE_ERROR_DIMENSION;
    if (_check_vec(k1, n) || _check_vec(k2, n) || _check_vec(k3, n) || _check_vec(k4, n) || _check_vec(tmp, n))
        return CORE_ERROR_DIMENSION;

    CoreErrorStatus st = CORE_ERROR_SUCCESS;

    // k1 = f(t, x, u)
    st = f(t, x_now, u_now, params, k1);                  if (st) return st;

    // k2 = f(t + h/2, x + h/2 * k1, u)
    // tmp = x + (h/2) * k1
    st = matrix_ops_copy(tmp, x_now);                   if (st) return st;
    st = matrix_ops_scale(k1, h * 0.5);                     if (st) return st;
    st = matrix_ops_add(tmp, tmp, k1);                  if (st) return st;
    st = f(t + h * 0.5, tmp, u_now, params, k2);       if (st) return st;
    st = matrix_ops_scale(k1, 1.0 / (h * 0.5));           if (st) return st; // restore k1 if scale is in-place (optional)

    // k3 = f(t + h/2, x + h/2 * k2, u)
    st = matrix_ops_copy(tmp, x_now);                  if (st) return st;
    st = matrix_ops_scale(k2, h * 0.5);                    if (st) return st;
    st = matrix_ops_add(tmp, tmp, k2);                 if (st) return st;
    st = f(t + h * 0.5, tmp, u_now, params, k3);      if (st) return st;
    st = matrix_ops_scale(k2, 1.0 / (h * 0.5));          if (st) return st;

    // k4 = f(t + h, x + h*k3, u)
    st = matrix_ops_copy(tmp, x_now);                 if (st) return st;
    st = matrix_ops_scale(k3, h);                            if (st) return st;
    st = matrix_ops_add(tmp, tmp, k3);                 if (st) return st;
    st = f(t + h, tmp, u_now, params, k4);              if (st) return st;
    st = matrix_ops_scale(k3, 1.0 / h);                    if (st) return st;

    // x_next = x + h/6 * (k1 + 2*k2 + 2*k3 + k4)
    // tmp = k1 + 2*k2
    st = matrix_ops_scale(k2, 2.0);                         if (st) return st;
    st = matrix_ops_add(tmp, k1, k2);                   if (st) return st;

    // tmp = tmp + 2*k3
    st = matrix_ops_scale(k3, 2.0);                         if (st) return st;
    st = matrix_ops_add(tmp, tmp, k3);                 if (st) return st;

    // tmp = tmp + k4
    st = matrix_ops_add(tmp, tmp, k4);                 if (st) return st;

    // tmp = (h/6) * tmp
    st = matrix_ops_scale(tmp, h / 6.0);                 if (st) return st;

    // x_next = x + tmp
    st = matrix_ops_add(x_next, x_now, tmp);       if (st) return st;

    // （任意）スケールした k2/k3 を元に戻したい場合はここで戻す
    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus rk4_step(RkOdeFunc f,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    void* params,
    Matrix* x_next)
{
    if (!f || !x_now || !x_next) return CORE_ERROR_NULL;
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    const int n = x_now->rows;

    Matrix* k1 = NULL, * k2 = NULL, * k3 = NULL, * k4 = NULL, * tmp = NULL;
    k1 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    k2 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    k3 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    k4 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    tmp = matrix_core_create(n, 1, &st); if (st) goto DONE;

    st = rk4_step_ws(f, t, x_now, u_now, h, params, x_next, k1, k2, k3, k4, tmp);

DONE:
    if (tmp) matrix_core_destroy(tmp);
    if (k4)  matrix_core_destroy(k4);
    if (k3)  matrix_core_destroy(k3);
    if (k2)  matrix_core_destroy(k2);
    if (k1)  matrix_core_destroy(k1);
    return st;
}

/* ---------- Linear specialization: x' = A x + B u (ZOH) ---------- */

static CoreErrorStatus _lin_rhs(const Matrix* A, const Matrix* B,
    const Matrix* x, const Matrix* u,
    Matrix* Ax, Matrix* Bu, Matrix* out_dx)
{
    CoreErrorStatus st = matrix_ops_multiply(Ax, A, x); if (st) return st;
    if (B && u) {
        st = matrix_ops_multiply(Bu, B, u); if (st) return st;
        st = matrix_ops_add(out_dx, Ax, Bu);
    }
    else {
        st = matrix_ops_copy(out_dx, Ax);
    }
    return st;
}

CoreErrorStatus rk4_lin_step_ws(const Matrix* A,
    const Matrix* B,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    Matrix* x_next,
    Matrix* k1, Matrix* k2, Matrix* k3, Matrix* k4,
    Matrix* tmp, Matrix* Ax, Matrix* Bu)
{
    (void)t; // not used
    if (!A || !x_now || !x_next || !k1 || !k2 || !k3 || !k4 || !tmp || !Ax || !Bu)
        return CORE_ERROR_NULL;
    if (h <= 0.0) return CORE_ERROR_INVALID_ARG;
    const int n = A->rows;
    if (A->cols != n) return CORE_ERROR_DIMENSION;
    if (_check_vec(x_now, n) || _check_vec(x_next, n) || _check_vec(k1, n) ||
        _check_vec(k2, n) || _check_vec(k3, n) || _check_vec(k4, n) ||
        _check_vec(tmp, n) || _check_vec(Ax, n) || _check_vec(Bu, n))
        return CORE_ERROR_DIMENSION;
    if (B && u_now) {
        if (B->rows != n) return CORE_ERROR_DIMENSION;
        if (u_now->cols != 1 || B->cols != u_now->rows) return CORE_ERROR_DIMENSION;
    }

    CoreErrorStatus st = CORE_ERROR_SUCCESS;

    // k1 = f(x)
    st = _lin_rhs(A, B, x_now, u_now, Ax, Bu, k1); if (st) return st;

    // k2: tmp = x + h/2 * k1
    st = matrix_ops_copy(tmp, x_now);           if (st) return st;
    st = matrix_ops_scale(k1, h * 0.5);           if (st) return st;
    st = matrix_ops_add(tmp, tmp, k1);          if (st) return st;
    st = _lin_rhs(A, B, tmp, u_now, Ax, Bu, k2);      if (st) return st;
    st = matrix_ops_scale(k1, 1.0 / (h * 0.5));     if (st) return st;

    // k3
    st = matrix_ops_copy(tmp, x_now);           if (st) return st;
    st = matrix_ops_scale(k2, h * 0.5);           if (st) return st;
    st = matrix_ops_add(tmp, tmp, k2);          if (st) return st;
    st = _lin_rhs(A, B, tmp, u_now, Ax, Bu, k3);      if (st) return st;
    st = matrix_ops_scale(k2, 1.0 / (h * 0.5));     if (st) return st;

    // k4
    st = matrix_ops_copy(tmp, x_now);           if (st) return st;
    st = matrix_ops_scale(k3, h);               if (st) return st;
    st = matrix_ops_add(tmp, tmp, k3);          if (st) return st;
    st = _lin_rhs(A, B, tmp, u_now, Ax, Bu, k4);      if (st) return st;
    st = matrix_ops_scale(k3, 1.0 / h);           if (st) return st;

    // x_next = x + h/6 (k1 + 2k2 + 2k3 + k4)
    st = matrix_ops_scale(k2, 2.0);             if (st) return st;
    st = matrix_ops_add(tmp, k1, k2);           if (st) return st;

    st = matrix_ops_scale(k3, 2.0);             if (st) return st;
    st = matrix_ops_add(tmp, tmp, k3);          if (st) return st;

    st = matrix_ops_add(tmp, tmp, k4);          if (st) return st;

    st = matrix_ops_scale(tmp, h / 6.0);          if (st) return st;
    st = matrix_ops_add(x_next, x_now, tmp);    if (st) return st;

    return CORE_ERROR_SUCCESS;
}

CoreErrorStatus rk4_lin_step(const Matrix* A,
    const Matrix* B,
    double t,
    const Matrix* x_now,
    const Matrix* u_now,
    double h,
    Matrix* x_next)
{
    if (!A || !x_now || !x_next) return CORE_ERROR_NULL;
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    const int n = A->rows;

    Matrix* k1 = NULL, * k2 = NULL, * k3 = NULL, * k4 = NULL, * tmp = NULL, * Ax = NULL, * Bu = NULL;
    k1 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    k2 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    k3 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    k4 = matrix_core_create(n, 1, &st); if (st) goto DONE;
    tmp = matrix_core_create(n, 1, &st); if (st) goto DONE;
    Ax = matrix_core_create(n, 1, &st); if (st) goto DONE;
    Bu = matrix_core_create(n, 1, &st); if (st) goto DONE;

    st = rk4_lin_step_ws(A, B, t, x_now, u_now, h, x_next, k1, k2, k3, k4, tmp, Ax, Bu);

DONE:
    if (Bu)  matrix_core_destroy(Bu);
    if (Ax)  matrix_core_destroy(Ax);
    if (tmp) matrix_core_destroy(tmp);
    if (k4)  matrix_core_destroy(k4);
    if (k3)  matrix_core_destroy(k3);
    if (k2)  matrix_core_destroy(k2);
    if (k1)  matrix_core_destroy(k1);
    return st;
}
