#include <gtest/gtest.h>

extern "C" {
#include "matrix_solve.h"
#include "matrix_ops.h"
#include "core_matrix.h"
#include "core_error.h"
}

// ========== matrix_solve_LU ==========
TEST(MatrixSolve_LU, GivenValidSystem_WhenSolve_ThenSolutionMatches) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* B = matrix_core_create(2, 1, &err);
    ASSERT_NE(B, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 1, &err);
    ASSERT_NE(X, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A = [ [3, 1],
    //       [1, 2] ]
    EXPECT_EQ(matrix_ops_set(A, 0, 0, 3.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 2.0), CORE_ERROR_SUCCESS);

    // B = [ [9],
    //       [8] ]
    EXPECT_EQ(matrix_ops_set(B, 0, 0, 9.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(B, 1, 0, 8.0), CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_solve_LU(A, X, B), CORE_ERROR_SUCCESS);

    double x0 = matrix_ops_get(X, 0, 0, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    double x1 = matrix_ops_get(X, 1, 0, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_DOUBLE_EQ(x0, 2.0);
    EXPECT_DOUBLE_EQ(x1, 3.0);

    EXPECT_EQ(matrix_core_free(X), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
}

TEST(MatrixSolve_LU, GivenNullArguments_WhenSolve_ThenErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* B = matrix_core_create(2, 1, &err);
    ASSERT_NE(B, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 1, &err);
    ASSERT_NE(X, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_solve_LU(nullptr, X, B), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_solve_LU(A, nullptr, B), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_solve_LU(A, X, nullptr), CORE_ERROR_NULL);

    EXPECT_EQ(matrix_core_free(X), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
}

TEST(MatrixSolve_LU, GivenDimensionMismatch_WhenSolve_ThenErrInvalidArg) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* B = matrix_core_create(3, 1, &err); // rows mismatch
    ASSERT_NE(B, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 1, &err);
    ASSERT_NE(X, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_solve_LU(A, X, B), CORE_ERROR_INVALID_ARG);

    EXPECT_EQ(matrix_core_free(X), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);

    err = CORE_ERROR_SUCCESS;
    A = matrix_core_create(2, 2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    B = matrix_core_create(2, 1, &err);
    ASSERT_NE(B, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    X = matrix_core_create(2, 2, &err); // cols mismatch
    ASSERT_NE(X, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_solve_LU(A, X, B), CORE_ERROR_INVALID_ARG);

    EXPECT_EQ(matrix_core_free(X), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
}

