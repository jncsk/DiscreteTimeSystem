#include <gtest/gtest.h>

extern "C" {
#include "core_matrix.h"
#include "matrix_ops.h"
#include "core_error.h"
#include "pade.h"
}

#include <cmath>

TEST(PadeExpm, ZeroMatrixReturnsIdentity) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create_square(2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set_zero(A), CORE_ERROR_SUCCESS);

    Matrix* result = matrix_core_create_square(2, &err);
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(pade_expm(A, result), CORE_ERROR_SUCCESS);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            CoreErrorStatus st = CORE_ERROR_SUCCESS;
            double val = matrix_ops_get(result, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            if (i == j) {
                EXPECT_NEAR(val, 1.0, 1e-12);
            } else {
                EXPECT_NEAR(val, 0.0, 1e-12);
            }
        }
    }

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(result), CORE_ERROR_SUCCESS);
}

TEST(PadeExpm, SimpleTwoByTwoMatrixMatchesReference) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create_square(2, &err);
    ASSERT_NE(A, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set(A, 0, 0, 0.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, -1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 0.0), CORE_ERROR_SUCCESS);

    Matrix* result = matrix_core_create_square(2, &err);
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(pade_expm(A, result), CORE_ERROR_SUCCESS);

    const double c = std::cos(1.0);
    const double s = std::sin(1.0);

    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    double val = 0.0;

    val = matrix_ops_get(result, 0, 0, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    EXPECT_NEAR(val, c, 1e-9);

    val = matrix_ops_get(result, 0, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    EXPECT_NEAR(val, s, 1e-9);

    val = matrix_ops_get(result, 1, 0, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    EXPECT_NEAR(val, -s, 1e-9);

    val = matrix_ops_get(result, 1, 1, &st);
    ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    EXPECT_NEAR(val, c, 1e-9);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(result), CORE_ERROR_SUCCESS);
}

TEST(PadeExpm, NullPointersReturnError) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create_square(2, &err);
    ASSERT_NE(A, nullptr);
    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr);

    EXPECT_EQ(pade_expm(nullptr, R), CORE_ERROR_NULL);
    EXPECT_EQ(pade_expm(A, nullptr), CORE_ERROR_NULL);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(PadeExpm, NonSquareMatrixReturnsDimensionError) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(2, 3, &err);
    ASSERT_NE(A, nullptr);
    Matrix* R = matrix_core_create(2, 3, &err);
    ASSERT_NE(R, nullptr);

    EXPECT_EQ(pade_expm(A, R), CORE_ERROR_DIMENSION);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

