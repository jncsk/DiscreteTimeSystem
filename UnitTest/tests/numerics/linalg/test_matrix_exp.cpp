#include <gtest/gtest.h>
#include <cmath>

extern "C" {
#include "matrix_exp.h"
#include "core_matrix.h"
#include "matrix_ops.h"
#include "core_error.h"
}

// --- NULL argument handling and non-square matrices ---

TEST(MatrixExp, NullArguments) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* r = matrix_core_create(1, 1, &err);
    ASSERT_NE(r, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_exp_exponential(nullptr, 1.0, r), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_exp_exponential(r, 1.0, nullptr), CORE_ERROR_NULL);

    matrix_core_free(r);
}

TEST(MatrixExp, NonSquareMatrix) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 3, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(2, 3, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_exp_exponential(A, 1.0, R), CORE_ERROR_DIMENSION);

    matrix_core_free(A);
    matrix_core_free(R);
}

// --- Exponential of zero matrix yields identity ---

TEST(MatrixExp, ZeroMatrixIsIdentity) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create_square(3, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create_square(3, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    ASSERT_EQ(matrix_ops_set_zero(A), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_exp_exponential(A, 5.0, R), CORE_ERROR_SUCCESS);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double v = matrix_ops_get(R, i, j, &err);
            EXPECT_EQ(err, CORE_ERROR_SUCCESS);
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_DOUBLE_EQ(v, expected);
        }
    }

    matrix_core_free(A);
    matrix_core_free(R);
}

// --- Analytic 2x2 exponential ---

TEST(MatrixExp, Analytic2x2) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create_square(2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A = [[0, -1], [1, 0]]
    ASSERT_EQ(matrix_ops_set(A, 0, 0, 0.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(A, 0, 1, -1.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(A, 1, 0, 1.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(A, 1, 1, 0.0), CORE_ERROR_SUCCESS);

    double t = 1.0; // radians
    ASSERT_EQ(matrix_exp_exponential(A, t, R), CORE_ERROR_SUCCESS);

    double c = std::cos(t);
    double s = std::sin(t);
    EXPECT_NEAR(matrix_ops_get(R, 0, 0, &err), c, 1e-9);
    EXPECT_NEAR(matrix_ops_get(R, 0, 1, &err), -s, 1e-9);
    EXPECT_NEAR(matrix_ops_get(R, 1, 0, &err), s, 1e-9);
    EXPECT_NEAR(matrix_ops_get(R, 1, 1, &err), c, 1e-9);

    matrix_core_free(A);
    matrix_core_free(R);
}

