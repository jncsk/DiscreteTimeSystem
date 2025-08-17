#include <gtest/gtest.h>
extern "C" {
#include "core_matrix.h"
#include "core_error.h"
#include "matrix_ops.h"
#include "state_space.h"     // StateSpaceModel ÇÃê∂ê¨APIÇâºíË
#include "state_space_c2d.h" // ç°âÒÇÃëŒè€
#include "pade.h"            // pade_expm ÇÃñﬂÇËílå^Ç»Ç«
}

static StateSpaceModel* make_ss1(CoreErrorStatus* err) {
    // n=1, m=1, p=1 ÇÃã…è¨ånÅBA=[-1], B=[2]
    StateSpaceModel* sys = state_space_create(1, 1, 1, err);
    if (!sys || *err) return sys;
    sys->A->data[0] = -1.0;
    sys->B->data[0] = 2.0;
    // C,D ÇÕç°âÒñ¢égóp
    return sys;
}

TEST(StateSpaceC2D, TsZeroReturnsIdentityAndZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_ss1(&err);
    ASSERT_NE(sys, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    Matrix* Ad = matrix_core_create(1, 1, &err);
    Matrix* Bd = matrix_core_create(1, 1, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    err = state_space_c2d(sys, 0.0, Ad, Bd);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);
    ASSERT_NE(Ad, nullptr);
    ASSERT_NE(Bd, nullptr);

    EXPECT_DOUBLE_EQ(Ad->data[0], 1.0);  // I
    EXPECT_DOUBLE_EQ(Bd->data[0], 0.0);  // 0

    matrix_core_free(Ad);
    matrix_core_free(Bd);
    state_space_free(sys);
}

TEST(StateSpaceC2D, ScalarSystemMatchesAnalyticWhenExpmAvailable) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_ss1(&err);
    ASSERT_NE(sys, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    const double Ts = 0.1;
    Matrix* Ad = matrix_core_create(1, 1, &err);
    Matrix* Bd = matrix_core_create(1, 1, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // é¿çs
    err = state_space_c2d(sys, Ts, Ad, Bd);

    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // âêÕâ:
    // A = [-1], B=[2]
    // Ad = exp(-1 * Ts)
    // Bd = ÅÁ_0^Ts exp(-1 * É—) * 2 dÉ— = 2*(1 - exp(-Ts))
    const double Ad_ref = std::exp(-Ts);
    const double Bd_ref = 2.0 * (1.0 - std::exp(-Ts));
    const double tol = 1e-10;

    EXPECT_NEAR(Ad->data[0], Ad_ref, tol);
    EXPECT_NEAR(Bd->data[0], Bd_ref, tol);

    matrix_core_free(Ad);
    matrix_core_free(Bd);
    state_space_free(sys);
}

TEST(StateSpaceC2D, DimensionChecks) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = state_space_create(2, 1, 1, &err); // n=2, m=1
    ASSERT_NE(sys, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A,B ÇÃìKìñÇ»íl
    sys->A->data[0] = 0.0; sys->A->data[1] = 1.0;
    sys->A->data[2] = -2.0; sys->A->data[3] = -3.0;
    sys->B->data[0] = 1.0; sys->B->data[1] = 0.0;

    Matrix* Ad_bad = matrix_core_create(1, 1, &err); // nÅ~n Ç≈ÇÕÇ»Ç¢
    Matrix* Bd_bad = matrix_core_create(1, 1, &err); // nÅ~m Ç≈ÇÕÇ»Ç¢
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    err = state_space_c2d(sys, 0.1, Ad_bad, Bd_bad);
    EXPECT_EQ(err, CORE_ERROR_DIMENSION);

    matrix_core_free(Ad_bad);
    matrix_core_free(Bd_bad);
    state_space_free(sys);
}

TEST(StateSpaceC2D, NullArgs) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    StateSpaceModel* sys = make_ss1(&err);
    ASSERT_NE(sys, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    Matrix* Ad = matrix_core_create(1, 1, &err);
    Matrix* Bd = matrix_core_create(1, 1, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(state_space_c2d(NULL, 0.1, Ad, Bd), CORE_ERROR_NULL);
    EXPECT_EQ(state_space_c2d(sys, 0.1, NULL, Bd), CORE_ERROR_NULL);
    EXPECT_EQ(state_space_c2d(sys, 0.1, Ad, NULL), CORE_ERROR_NULL);

    matrix_core_free(Ad);
    matrix_core_free(Bd);
    state_space_free(sys);
}
