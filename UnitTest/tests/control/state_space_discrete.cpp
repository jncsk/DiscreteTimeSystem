#include <gtest/gtest.h>
#include <cmath>

extern "C" {
#include "core_matrix.h"
#include "core_error.h"
#include "matrix_ops.h"
#include "state_space.h"
#include "state_space_c2d.h"
#include "state_space_discrete.h"
}

// 連続系: A=[[0,1],[0,-1]], B=[[0],[1]], y=theta (C=[0,1], D=[0])
static StateSpaceModel* make_csys_A01(CoreErrorStatus* err) {
    StateSpaceModel* sys = state_space_create(2, 1, 1, err);
    EXPECT_EQ(*err, CORE_ERROR_SUCCESS);

    // A
    matrix_ops_set(sys->A, 0, 0, 0.0);
    matrix_ops_set(sys->A, 0, 1, 1.0);
    matrix_ops_set(sys->A, 1, 0, 0.0);
    matrix_ops_set(sys->A, 1, 1, -1.0);

    // B
    matrix_ops_set(sys->B, 0, 0, 0.0);
    matrix_ops_set(sys->B, 1, 0, 1.0);

    // C,D
    matrix_ops_set(sys->C, 0, 0, 0.0);
    matrix_ops_set(sys->C, 0, 1, 1.0);
    matrix_ops_set(sys->D, 0, 0, 0.0);
    return sys;
}

TEST(SSDiscrete, InitFromCsys_AdBdMatchExpected_T1)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_csys_A01(&st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    SSDiscrete d = { 0 };
    st = ss_discrete_init_from_csys(&d, sys, 1.0);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    const double a = std::exp(-1.0), b = 1.0 - a;

    // Ad = [[1, b], [0, a]]
    EXPECT_NEAR(d.Ad->data[0], 1.0, 1e-12);
    EXPECT_NEAR(d.Ad->data[1], b, 1e-12);
    EXPECT_NEAR(d.Ad->data[2], 0.0, 1e-12);
    EXPECT_NEAR(d.Ad->data[3], a, 1e-12);

    // Bd = [[a], [b]]
    EXPECT_NEAR(d.Bd->data[0], a, 1e-12);
    EXPECT_NEAR(d.Bd->data[1], b, 1e-12);

    ss_discrete_free(&d);
    state_space_free(sys);
}

TEST(SSDiscrete, StepSequence_u01u12ThenZero_ReachesExpectedStates)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_csys_A01(&st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    SSDiscrete d = { 0 };
    st = ss_discrete_init_from_csys(&d, sys, 1.0);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    const double a = std::exp(-1.0), b = 1.0 - a;

    Matrix* x = matrix_core_create(2, 1, &st);
    Matrix* xn = matrix_core_create(2, 1, &st);
    Matrix* u = matrix_core_create(1, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    matrix_ops_set_zero(x);

    // k=0: u0=1 → x1 = Bd
    u->data[0] = 1.0;
    ASSERT_EQ(ss_discrete_step(&d, x, u, xn), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(xn->data[0], a, 1e-10);
    EXPECT_NEAR(xn->data[1], b, 1e-10);
    std::swap(x, xn);

    // k=1: u1=2
    u->data[0] = 2.0;
    ASSERT_EQ(ss_discrete_step(&d, x, u, xn), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(xn->data[0], 3 * a + b * b, 1e-10);
    EXPECT_NEAR(xn->data[1], b * (a + 2.0), 1e-10);
    std::swap(x, xn);

    // k=2: u=0
    u->data[0] = 0.0;
    ASSERT_EQ(ss_discrete_step(&d, x, u, xn), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(xn->data[1], a * (b * (a + 2.0)), 1e-10);

    matrix_core_free(u);
    matrix_core_free(xn);
    matrix_core_free(x);
    ss_discrete_free(&d);
    state_space_free(sys);
}

TEST(SSDiscrete, StepWS_MatchesStep)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_csys_A01(&st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    SSDiscrete d = { 0 };
    ASSERT_EQ(ss_discrete_init_from_csys(&d, sys, 1.0), CORE_ERROR_SUCCESS);

    Matrix* x = matrix_core_create(2, 1, &st);
    Matrix* xn1 = matrix_core_create(2, 1, &st);
    Matrix* xn2 = matrix_core_create(2, 1, &st);
    Matrix* u = matrix_core_create(1, 1, &st);
    Matrix* Ax = matrix_core_create(2, 1, &st);
    Matrix* Bu = matrix_core_create(2, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    matrix_ops_set_zero(x);
    u->data[0] = 1.23;

    ASSERT_EQ(ss_discrete_step(&d, x, u, xn1), CORE_ERROR_SUCCESS);
    ASSERT_EQ(ss_discrete_step_ws(&d, x, u, xn2, Ax, Bu), CORE_ERROR_SUCCESS);

    EXPECT_NEAR(xn1->data[0], xn2->data[0], 1e-12);
    EXPECT_NEAR(xn1->data[1], xn2->data[1], 1e-12);

    matrix_core_free(Bu);
    matrix_core_free(Ax);
    matrix_core_free(u);
    matrix_core_free(xn2);
    matrix_core_free(xn1);
    matrix_core_free(x);
    ss_discrete_free(&d);
    state_space_free(sys);
}

TEST(SSDiscrete, ScalarUStep_EqualsVectorVersionWhenM1)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_csys_A01(&st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    SSDiscrete d = { 0 };
    ASSERT_EQ(ss_discrete_init_from_csys(&d, sys, 1.0), CORE_ERROR_SUCCESS);

    Matrix* x = matrix_core_create(2, 1, &st);
    Matrix* xn = matrix_core_create(2, 1, &st);
    Matrix* u = matrix_core_create(1, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    matrix_ops_set_zero(x);

    // u=2 ベクトル版
    u->data[0] = 2.0;
    ASSERT_EQ(ss_discrete_step(&d, x, u, xn), CORE_ERROR_SUCCESS);
    double x0_vec = xn->data[0];
    double x1_vec = xn->data[1];

    // スカラ版
    matrix_ops_set_zero(x);
    ASSERT_EQ(ss_discrete_step_scalar_u(&d, x, 2.0, xn), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(xn->data[0], x0_vec, 1e-12);
    EXPECT_NEAR(xn->data[1], x1_vec, 1e-12);

    matrix_core_free(u);
    matrix_core_free(xn);
    matrix_core_free(x);
    ss_discrete_free(&d);
    state_space_free(sys);
}

TEST(SSDiscrete, Output_YequalsCxPlusDu_WhenCDPresent)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_csys_A01(&st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    SSDiscrete d = { 0 };
    ASSERT_EQ(ss_discrete_init_from_csys(&d, sys, 1.0), CORE_ERROR_SUCCESS);

    // y = theta = [0 1]x + 0*u
    Matrix* x = matrix_core_create(2, 1, &st);
    Matrix* u = matrix_core_create(1, 1, &st);
    Matrix* y = matrix_core_create(1, 1, &st);

    x->data[0] = 0.3;  // omega
    x->data[1] = 1.2;  // theta
    u->data[0] = 5.0;

    ASSERT_EQ(ss_discrete_output(&d, x, u, y), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(y->data[0], 1.2, 1e-12);

    matrix_core_free(y);
    matrix_core_free(u);
    matrix_core_free(x);
    ss_discrete_free(&d);
    state_space_free(sys);
}

TEST(SSDiscrete, ErrorHandling_NullAndDimension)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_csys_A01(&st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    SSDiscrete d = { 0 };
    ASSERT_EQ(ss_discrete_init_from_csys(&d, sys, 1.0), CORE_ERROR_SUCCESS);

    Matrix* x = matrix_core_create(2, 1, &st);
    Matrix* u = matrix_core_create(1, 1, &st);
    Matrix* xn = matrix_core_create(2, 1, &st);

    // NULL
    EXPECT_EQ(ss_discrete_step(nullptr, x, u, xn), CORE_ERROR_NULL);
    EXPECT_EQ(ss_discrete_step(&d, nullptr, u, xn), CORE_ERROR_NULL);
    EXPECT_EQ(ss_discrete_step(&d, x, nullptr, xn), CORE_ERROR_NULL);
    EXPECT_EQ(ss_discrete_step(&d, x, u, nullptr), CORE_ERROR_NULL);

    // Dimension mismatch
    Matrix* x_bad = matrix_core_create(3, 1, &st);
    EXPECT_EQ(ss_discrete_step(&d, x_bad, u, xn), CORE_ERROR_DIMENSION);
    matrix_core_free(x_bad);

    matrix_core_free(xn);
    matrix_core_free(u);
    matrix_core_free(x);
    ss_discrete_free(&d);
    state_space_free(sys);
}

TEST(SSDiscrete, Output_WithoutD_TreatsDAsZero)
{
    CoreErrorStatus st = CORE_ERROR_SUCCESS;

    // 連続系を作るが D は NULL のままにする
    StateSpaceModel* sys = state_space_create(2, 1, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    matrix_ops_set(sys->A, 0, 0, 0.0); matrix_ops_set(sys->A, 0, 1, 1.0);
    matrix_ops_set(sys->A, 1, 0, 0.0); matrix_ops_set(sys->A, 1, 1, -1.0);
    matrix_ops_set(sys->B, 0, 0, 0.0); matrix_ops_set(sys->B, 1, 0, 1.0);
    // C は設定、D は触らない（NULL）
    matrix_ops_set(sys->C, 0, 0, 0.0);
    matrix_ops_set(sys->C, 0, 1, 1.0);

    SSDiscrete d = { 0 };
    ASSERT_EQ(ss_discrete_init_from_csys(&d, sys, 1.0), CORE_ERROR_SUCCESS);

    Matrix* x = matrix_core_create(2, 1, &st);
    Matrix* u = matrix_core_create(1, 1, &st);
    Matrix* y = matrix_core_create(1, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);

    x->data[0] = 0.3; // omega
    x->data[1] = 1.2; // theta
    u->data[0] = 5.0;

    ASSERT_EQ(ss_discrete_output(&d, x, u, y), CORE_ERROR_SUCCESS);
    // y = Cx + 0*u = theta
    EXPECT_NEAR(y->data[0], 1.2, 1e-12);

    matrix_core_free(y);
    matrix_core_free(u);
    matrix_core_free(x);
    ss_discrete_free(&d);
    state_space_free(sys);
}

