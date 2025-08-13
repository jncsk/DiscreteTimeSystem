#include <gtest/gtest.h>

extern "C" {
#include "matrix_norm.h"
#include "core_matrix.h"
#include "matrix_ops.h"
#include "core_error.h"
}

// ========== matrix_norm_1 ==========
TEST(MatrixNorm_1, GivenValidMatrix_WhenNorm1_ThenReturnsSuccess) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    ASSERT_NE(m->data, nullptr);


    // Fill A with arbitrary (sequential) values
    double v = 1.0;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            EXPECT_EQ(matrix_ops_set(m, i, j, v++), CORE_ERROR_SUCCESS);
        }
    }

    double r = 100;
    EXPECT_EQ(matrix_norm_1(m, &r) , CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(r, 9.0);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixNorm_1, GivenMatrixWithNegatives_WhenNorm1_ThenUsesAbsoluteValues) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A = [[ 1, -2,  3],
    //         [-4,  5, -6]]
    // [|1|+|4|=5, |2|+|5|=7, |3|+|6|=9] Å® 9
    double vals[] = { 1, -2, 3, -4, 5, -6 };
    for (int k = 0; k < 6; ++k) m->data[k] = vals[k];

    double r = 0.0;
    EXPECT_EQ(matrix_norm_1(m, &r), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(r, 9.0);

    matrix_core_free(m);
}
TEST(MatrixNorm_1, GivenNullArguments_WhenNorm1_ThenReturnsErrNull) {
    double r = 0.0;

    EXPECT_EQ(matrix_norm_1(nullptr, &r), CORE_ERROR_NULL);

    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(1, 1, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_norm_1(m, nullptr), CORE_ERROR_NULL);

    matrix_core_free(m);
}

TEST(MatrixNorm_1, GivenInvalidArg_WhenNorm1_ThenReturnsInvalidArg) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    ASSERT_NE(m->data, nullptr);

    double r = 0;

    m->rows = 0;
    m->cols = 3;
    EXPECT_EQ(matrix_norm_1(m, &r), CORE_ERROR_INVALID_ARG);

    m->rows = 2;
    m->cols = 0;
    EXPECT_EQ(matrix_norm_1(m, &r), CORE_ERROR_INVALID_ARG);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ------------- 1) Vector shapes (1xN / Nx1) -------------
TEST(MatrixNorm_VectorShapes, RowVector_1xN) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(1, 4, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // m = [ 1, -2, 3, -4 ]
    double vals[] = { 1, -2, 3, -4 };
    for (int j = 0; j < 4; ++j) 
        EXPECT_EQ(matrix_ops_set(m, 0, j, vals[j]), CORE_ERROR_SUCCESS);

    double n1 = 0, ninf = 0, nfro = 0, nmax = 0;

    // 1-norm (induced): max absolute column sum = max(|1|,|2|,|3|,|4|) = 4
    EXPECT_EQ(matrix_norm_1(m, &n1), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(n1, 4.0);

    // infinity-norm (induced): max absolute row sum = |1|+|2|+|3|+|4| = 10
    EXPECT_EQ(matrix_norm_inf(m, &ninf), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(ninf, 10.0);

    // Frobenius norm: sqrt(1^2+(-2)^2+3^2+(-4)^2) = sqrt(30)
    EXPECT_EQ(matrix_norm_fro(m, &nfro), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(nfro, std::sqrt(30.0), 1e-12);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixNorm_VectorShapes, ColVector_Nx1) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(3, 1, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // m = [ 1; -2; 3 ]
    double vals[] = { 1, -2, 3 };
    for (int i = 0; i < 3; ++i) 
        EXPECT_EQ(matrix_ops_set(m, i, 0, vals[i]), CORE_ERROR_SUCCESS);

    double n1 = 0, ninf = 0, nfro = 0, nmax = 0;

    // 1-norm (induced): only one column, sum = |1|+|2|+|3| = 6
    EXPECT_EQ(matrix_norm_1(m, &n1), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(n1, 6.0);

    // infinity-norm (induced): max absolute row sum = max(|1|,|2|,|3|) = 3
    EXPECT_EQ(matrix_norm_inf(m, &ninf), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(ninf, 3.0);

    // Frobenius norm: sqrt(1^2+(-2)^2+3^2) = sqrt(14)
    EXPECT_EQ(matrix_norm_fro(m, &nfro), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(nfro, std::sqrt(14.0), 1e-12);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ------------- 2) Large values stability (overflow safe) -------------

TEST(MatrixNorm_LargeValues, HugeButSafe) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 2, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // 1e150 is safe in double: squaring gives 1e300, still below DBL_MAX
    EXPECT_EQ(matrix_ops_set(m, 0, 0, 1e150), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(m, 0, 1, -1e150), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(m, 1, 0, 1e150), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(m, 1, 1, 1e150), CORE_ERROR_SUCCESS);

    double n1 = 0, ninf = 0, nfro = 0, nmax = 0;

    // 1-norm: each column sum = 2e150
    EXPECT_EQ(matrix_norm_1(m, &n1), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(n1, 2e150);

    // infinity-norm: each row sum = 2e150
    EXPECT_EQ(matrix_norm_inf(m, &ninf), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(ninf, 2e150);

    // Frobenius norm: sqrt(4*(1e150)^2) = 2e150
    EXPECT_EQ(matrix_norm_fro(m, &nfro), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(nfro, 2e150);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ------------- 3) Ensure result is not modified on failure -------------

TEST(MatrixNorm_ResultNotModifiedOnFailure, NullArgsAndInvalidDims) {
    const double S = 123.456; // sentinel value
    double r;

    // (a) null matrix
    r = S;
    EXPECT_EQ(matrix_norm_1(nullptr, &r), CORE_ERROR_NULL);
    EXPECT_DOUBLE_EQ(r, S);

    // (b) null result
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(1, 1, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_norm_inf(m, nullptr), CORE_ERROR_NULL);

    // (c) invalid dimensions (rows = 0)
    r = S;
    m->rows = 0;
    EXPECT_EQ(matrix_norm_fro(m, &r), CORE_ERROR_INVALID_ARG);
    EXPECT_DOUBLE_EQ(r, S);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ------------- 4) Representative example for all norms -------------

TEST(MatrixNorm_AllKinds, MixedSigns2x3) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // m = [ [ 1, -2,  3 ],
    //       [ -4, 5, -6 ] ]
    double vals[] = { 1, -2, 3, -4, 5, -6 };
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(matrix_ops_set(m, i, j, vals[i * 3 + j]), CORE_ERROR_SUCCESS);
        }
    }

    double n1 = 0, ninf = 0, nfro = 0, nmax = 0;

    // 1-norm: column absolute sums = [5,7,9] -> max = 9
    EXPECT_EQ(matrix_norm_1(m, &n1), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(n1, 9.0);

    // infinity-norm: row absolute sums = [6,15] -> max = 15
    EXPECT_EQ(matrix_norm_inf(m, &ninf), CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(ninf, 15.0);

    // Frobenius norm: sqrt(1+4+9+16+25+36) = sqrt(91)
    EXPECT_EQ(matrix_norm_fro(m, &nfro), CORE_ERROR_SUCCESS);
    EXPECT_NEAR(nfro, std::sqrt(91.0), 1e-12);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}