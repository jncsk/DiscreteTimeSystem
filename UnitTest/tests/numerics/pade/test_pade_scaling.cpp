#include <gtest/gtest.h>
#include <cmath>
#include <limits>

extern "C" {
#include "core_matrix.h"
#include "matrix_ops.h"
#include "pade_scaling.h"
}

namespace {

// Utility to compare two matrices element-wise.
static void ExpectMatrixEq(const Matrix* A, const Matrix* B) {
    ASSERT_EQ(A->rows, B->rows);
    ASSERT_EQ(A->cols, B->cols);

    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    for (int i = 0; i < A->rows; ++i) {
        for (int j = 0; j < A->cols; ++j) {
            double a = matrix_ops_get(A, i, j, &err);
            ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            double b = matrix_ops_get(B, i, j, &err);
            ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(a, b);
        }
    }
}

}  // namespace

// ==============================
// Tests for pade_choose_scaling_and_order
// ==============================

TEST(PadeChooseScalingAndOrder, ReturnsExpectedPairsForKnownNorms) {
    struct TestCase { double anorm; int expect_s; int expect_m; };
    const TestCase cases[] = {
        {0.0, 0, 3},       // zero norm
        {0.1, 0, 5},       // small -> choose m=5
        {0.5, 1, 5},       // requires scaling with m=5
        {2.0, 0, 9},       // larger -> m=9 without scaling
        {2.5, 2, 7},       // even larger -> m=7 with scaling
    };

    for (const auto& tc : cases) {
        int s = -1, m = -1;
        CoreErrorStatus st = pade_choose_scaling_and_order(tc.anorm, &s, &m);
        EXPECT_EQ(st, CORE_ERROR_SUCCESS);
        EXPECT_EQ(s, tc.expect_s);
        EXPECT_EQ(m, tc.expect_m);
    }
}

TEST(PadeChooseScalingAndOrder, ReturnsErrorOnInvalidInputs) {
    int s = 0, m = 0;

    // Negative anorm
    EXPECT_EQ(pade_choose_scaling_and_order(-1.0, &s, &m), CORE_ERROR_INVALID_ARG);

    // NaN anorm
    double nanv = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(pade_choose_scaling_and_order(nanv, &s, &m), CORE_ERROR_INVALID_ARG);

    // Null pointers
    EXPECT_EQ(pade_choose_scaling_and_order(1.0, nullptr, &m), CORE_ERROR_NULL);
    EXPECT_EQ(pade_choose_scaling_and_order(1.0, &s, nullptr), CORE_ERROR_NULL);
}

// ==============================
// Tests for matrix_scale_down_pow2
// ==============================

TEST(MatrixScaleDownPow2, ScalingByZeroCopiesSource) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* src = matrix_core_create(2, 3, &err);
    ASSERT_NE(src, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* dst = matrix_core_create(2, 3, &err);
    ASSERT_NE(dst, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    ASSERT_EQ(matrix_ops_fill_sequential(src, 1.0, 1.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_fill(dst, 0.0), CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_scale_down_pow2(src, 0, dst), CORE_ERROR_SUCCESS);
    ExpectMatrixEq(src, dst);

    EXPECT_EQ(matrix_core_free(src), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(dst), CORE_ERROR_SUCCESS);
}

TEST(MatrixScaleDownPow2, PositiveScaleMatchesLdexp) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int rows = 2, cols = 2;

    Matrix* src = matrix_core_create(rows, cols, &err);
    ASSERT_NE(src, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* dst = matrix_core_create(rows, cols, &err);
    ASSERT_NE(dst, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    ASSERT_EQ(matrix_ops_fill_sequential(src, 1.0, 1.0), CORE_ERROR_SUCCESS);

    const int s = 3;  // scale by 2^{-3}
    EXPECT_EQ(matrix_scale_down_pow2(src, s, dst), CORE_ERROR_SUCCESS);

    for (int i = 0; i < rows * cols; ++i) {
        double expected = std::ldexp(src->data[i], -s);
        EXPECT_DOUBLE_EQ(dst->data[i], expected);
    }

    EXPECT_EQ(matrix_core_free(src), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(dst), CORE_ERROR_SUCCESS);
}

TEST(MatrixScaleDownPow2, ReturnsErrorOnNegativeScale) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* src = matrix_core_create(1, 1, &err);
    ASSERT_NE(src, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* dst = matrix_core_create(1, 1, &err);
    ASSERT_NE(dst, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_scale_down_pow2(src, -1, dst), CORE_ERROR_NULL);

    EXPECT_EQ(matrix_core_free(src), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(dst), CORE_ERROR_SUCCESS);
}

TEST(MatrixScaleDownPow2, ReturnsErrorOnDimensionMismatch) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* src = matrix_core_create(2, 2, &err);
    ASSERT_NE(src, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* dst = matrix_core_create(3, 3, &err);
    ASSERT_NE(dst, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_scale_down_pow2(src, 0, dst), CORE_ERROR_DIMENSION);

    EXPECT_EQ(matrix_core_free(src), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(dst), CORE_ERROR_SUCCESS);
}

