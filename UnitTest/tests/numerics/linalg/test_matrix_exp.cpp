#include <gtest/gtest.h>
#include <cmath>

extern "C" {
#include "matrix_exp.h"
#include "matrix_ops.h"
#include "core_matrix.h"
#include "core_error.h"
}

// Helper to check if matrix is identity
static void ExpectIdentity(const Matrix* m) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            double v = matrix_ops_get(m, i, j, &err);
            ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            if (i == j) EXPECT_DOUBLE_EQ(v, 1.0);
            else        EXPECT_DOUBLE_EQ(v, 0.0);
        }
    }
}

// A zero matrix scaled by any t returns the identity matrix.
TEST(MatrixExp, ZeroMatrixYieldsIdentity) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(3, 3, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(3, 3, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(matrix_ops_set(A, i, j, 0.0), CORE_ERROR_SUCCESS);
        }
    }

    err = matrix_exp_exponential(A, 5.0, R);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);
    ExpectIdentity(R);

    matrix_core_free(A);
    matrix_core_free(R);
}

// A simple 2x2 rotation matrix whose exponential has a known closed form.
TEST(MatrixExp, RotationMatrix) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(2, 2, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A = [[0, -1],[1, 0]]
    EXPECT_EQ(matrix_ops_set(A, 0, 0, 0.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, -1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 0.0), CORE_ERROR_SUCCESS);

    const double theta = 3.14159265358979323846 / 4.0; // 45 degrees
    err = matrix_exp_exponential(A, theta, R);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);

    Matrix* E = matrix_core_create(2, 2, &err);
    ASSERT_NE(E, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(E, 0, 0, std::cos(theta)), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(E, 0, 1, -std::sin(theta)), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(E, 1, 0, std::sin(theta)), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(E, 1, 1, std::cos(theta)), CORE_ERROR_SUCCESS);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            double rv = matrix_ops_get(R, i, j, &err);
            ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            double ev = matrix_ops_get(E, i, j, &err);
            ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            EXPECT_NEAR(rv, ev, 1e-12);
        }
    }

    matrix_core_free(A);
    matrix_core_free(R);
    matrix_core_free(E);
}

// Error handling for null arguments and mismatched dimensions.
TEST(MatrixExp, NullArguments) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(1, 1, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(1, 1, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_exp_exponential(nullptr, 1.0, R), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_exp_exponential(A, 1.0, nullptr), CORE_ERROR_NULL);

    matrix_core_free(A);
    matrix_core_free(R);
}

TEST(MatrixExp, DimensionMismatch) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(3, 3, &err);
    ASSERT_NE(R, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_exp_exponential(A, 1.0, R), CORE_ERROR_DIMENSION);

    matrix_core_free(A);
    matrix_core_free(R);
}

