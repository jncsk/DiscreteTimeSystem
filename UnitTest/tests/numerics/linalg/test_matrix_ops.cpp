#include <gtest/gtest.h>
#include <string>

extern "C" {
#include "core_matrix.h"
#include "matrix_ops.h"
#include "core_error.h"
}

// ========== Helpers ==========
static void ExpectMatrixEq(const Matrix* A, const Matrix* B) {
    ASSERT_EQ(A->rows, B->rows);
    ASSERT_EQ(A->cols, B->cols);
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    for (int i = 0; i < A->rows; ++i) {
        for (int j = 0; j < A->cols; ++j) {
            double a = matrix_ops_get(A, i, j, &err); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            double b = matrix_ops_get(B, i, j, &err); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(a, b);
        }
    }
}

// ========== matrix_ops_fill ==========
TEST(MatrixOps_Fill, GivenValidMatrix_WhenFillWithScalar_ThenReturnsSuccessAndAllElementsSet) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    ASSERT_NE(m->data, nullptr);

    err = matrix_ops_fill(m, 1.0);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);

    const int size = m->rows * m->cols;
    for (int i = 0; i < size; ++i) {
        EXPECT_DOUBLE_EQ(m->data[i], 1.0);
    }

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Fill, GivenNullMatrix_WhenFill_ThenReturnsErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    err = matrix_ops_fill(nullptr, 1.0);
    EXPECT_EQ(err, CORE_ERROR_NULL);
}

// ========== matrix_ops_set_zero ==========
TEST(MatrixOps_SetZero, GivenValidMatrix_WhenSetZero_ThenReturnsSuccessAndAllZeros) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    ASSERT_NE(m->data, nullptr);

    err = matrix_ops_set_zero(m);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);

    const int size = m->rows * m->cols;
    for (int i = 0; i < size; ++i) {
        EXPECT_DOUBLE_EQ(m->data[i], 0.0);
    }

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_SetZero, GivenNullMatrix_WhenSetZero_ThenReturnsErrNull) {
    EXPECT_EQ(matrix_ops_set_zero(nullptr), CORE_ERROR_NULL);
}

// ========== matrix_ops_set_identity ==========
TEST(MatrixOps_SetIdentity, GivenSquareMatrix_WhenSetIdentity_ThenReturnsSuccessAndIsIdentity) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create_square(3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set_identity(m), CORE_ERROR_SUCCESS);

    // Check both diagonal==1 and off-diagonal==0
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            if (i == j) EXPECT_DOUBLE_EQ(m->data[i * m->cols + j], 1.0);
            else        EXPECT_DOUBLE_EQ(m->data[i * m->cols + j], 0.0);
        }
    }

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_SetIdentity, GivenNullMatrix_WhenSetIdentity_ThenReturnsErrNull) {
    EXPECT_EQ(matrix_ops_set_identity(nullptr), CORE_ERROR_NULL);
}

TEST(MatrixOps_SetIdentity, GivenNonSquareMatrix_WhenSetIdentity_ThenReturnsErrDimension) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(1, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set_identity(m), CORE_ERROR_DIMENSION);
    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ========== matrix_ops_set ==========
TEST(MatrixOps_Set, GivenValidIndex_WhenSetValue_ThenReturnsSuccessAndValueIsStored) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(3, 5, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set(m, 1, 2, 100.0), CORE_ERROR_SUCCESS);
    double value = matrix_ops_get(m, 1, 2, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(value, 100.0);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Set, GivenNullMatrix_WhenSet_ThenReturnsErrNull) {
    EXPECT_EQ(matrix_ops_set(nullptr, 1, 2, 3.0), CORE_ERROR_NULL);
}

TEST(MatrixOps_Set, GivenOutOfBoundsIndex_WhenSet_ThenReturnsErrOutOfBoundsAndNoMutation) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(3, 5, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Set a known value for later verification unchanged
    EXPECT_EQ(matrix_ops_set(m, 0, 0, 7.0), CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set(m, 3, 0, 1.0), CORE_ERROR_OUT_OF_BOUNDS); // i == rows
    EXPECT_EQ(matrix_ops_set(m, 0, 5, 1.0), CORE_ERROR_OUT_OF_BOUNDS); // j == cols
    EXPECT_EQ(matrix_ops_set(m, -1, 0, 1.0), CORE_ERROR_OUT_OF_BOUNDS); // i < 0
    EXPECT_EQ(matrix_ops_set(m, 0, -1, 1.0), CORE_ERROR_OUT_OF_BOUNDS); // j < 0

    double v = matrix_ops_get(m, 0, 0, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(v, 7.0);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ========== matrix_ops_get ==========
TEST(MatrixOps_Get, GivenValidIndex_WhenGet_ThenReturnsSuccessAndCorrectValue) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(3, 5, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set(m, 1, 2, 100.0), CORE_ERROR_SUCCESS);
    double value = matrix_ops_get(m, 1, 2, &err);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_DOUBLE_EQ(value, 100.0);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Get, GivenNullMatrix_WhenGet_ThenSetsErrNullAndReturnsZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    double v = matrix_ops_get(nullptr, 1, 2, &err);
    EXPECT_EQ(err, CORE_ERROR_NULL);
    EXPECT_DOUBLE_EQ(v, 0.0);
}

TEST(MatrixOps_Get, GivenOutOfBoundsIndex_WhenGet_ThenSetsErrOutOfBoundsAndReturnsZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(3, 5, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set(m, 0, 0, 7.0), CORE_ERROR_SUCCESS);

    double v;
    v = matrix_ops_get(m, 3, 0, &err);  EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS); EXPECT_DOUBLE_EQ(v, 0.0);
    v = matrix_ops_get(m, 0, 5, &err);  EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS); EXPECT_DOUBLE_EQ(v, 0.0);
    v = matrix_ops_get(m, -1, 0, &err); EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS); EXPECT_DOUBLE_EQ(v, 0.0);
    v = matrix_ops_get(m, 0, -1, &err); EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS); EXPECT_DOUBLE_EQ(v, 0.0);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

// ========== matrix_ops_add ==========
TEST(MatrixOps_Add, GivenSameSizeMatrices_WhenAdd_ThenReturnsSuccessAndCorrectSum) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int rows = 2, cols = 3;

    Matrix* a = matrix_core_create(rows, cols, &err);
    ASSERT_NE(a, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* b = matrix_core_create(rows, cols, &err);
    ASSERT_NE(b, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* r = matrix_core_create(rows, cols, &err);
    ASSERT_NE(r, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Initialize values for test verification:
    // a[i][j] = i*cols + j
    // b[i][j] = 10 * (i*cols + j)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double val_a = static_cast<double>(i * cols + j);
            double val_b = 10.0 * static_cast<double>(i * cols + j);
            EXPECT_EQ(matrix_ops_set(a, i, j, val_a), CORE_ERROR_SUCCESS);
            EXPECT_EQ(matrix_ops_set(b, i, j, val_b), CORE_ERROR_SUCCESS);
        }
    }

    EXPECT_EQ(matrix_ops_add(a, b, r), CORE_ERROR_SUCCESS);

    // Expected: r[i][j] = 11 * (i*cols + j)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double expected = 11.0 * static_cast<double>(i * cols + j);
            double actual = matrix_ops_get(r, i, j, &err);
            EXPECT_EQ(err, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(actual, expected);
        }
    }

    EXPECT_EQ(matrix_core_free(a), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(b), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(r), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Add, GivenNullArguments_WhenAdd_ThenReturnsErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* a = matrix_core_create(1, 1, &err);
    Matrix* r = matrix_core_create(1, 1, &err);
    ASSERT_NE(a, nullptr); ASSERT_NE(r, nullptr);

    EXPECT_EQ(matrix_ops_add(a, nullptr, r), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_ops_add(nullptr, a, r), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_ops_add(a, a, nullptr), CORE_ERROR_NULL);

    EXPECT_EQ(matrix_core_free(a), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(r), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Add, GivenDifferentSizes_WhenAdd_ThenReturnsErrDimension) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* a = matrix_core_create(2, 3, &err);
    ASSERT_NE(a, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* b = matrix_core_create(3, 2, &err);
    ASSERT_NE(b, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* r = matrix_core_create(2, 3, &err);
    ASSERT_NE(r, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_add(a, b, r), CORE_ERROR_DIMENSION);

    EXPECT_EQ(matrix_core_free(a), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(b), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(r), CORE_ERROR_SUCCESS);
}

// ========== matrix_ops_multiply ==========
TEST(MatrixOps_Multiply, GivenRectangularMatrices_WhenMultiply_ThenReturnsSuccessAndCorrectResult) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create(2, 3, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* B = matrix_core_create(3, 2, &err);
    ASSERT_NE(B, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(2, 2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A = [[1,2,3],[4,5,6]]
    EXPECT_EQ(matrix_ops_set(A, 0, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, 2.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 2, 3.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, 4.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 5.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 2, 6.0), CORE_ERROR_SUCCESS);

    // B = [[7,8],[9,10],[11,12]]
    EXPECT_EQ(matrix_ops_set(B, 0, 0, 7.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(B, 0, 1, 8.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(B, 1, 0, 9.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(B, 1, 1, 10.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(B, 2, 0, 11.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(B, 2, 1, 12.0), CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_multiply(A, B, R), CORE_ERROR_SUCCESS);

    CoreErrorStatus s;
    double v;
    v = matrix_ops_get(R, 0, 0, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS); EXPECT_DOUBLE_EQ(v, 58.0);
    v = matrix_ops_get(R, 0, 1, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS); EXPECT_DOUBLE_EQ(v, 64.0);
    v = matrix_ops_get(R, 1, 0, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS); EXPECT_DOUBLE_EQ(v, 139.0);
    v = matrix_ops_get(R, 1, 1, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS); EXPECT_DOUBLE_EQ(v, 154.0);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Multiply, GivenIdentityMatrix_WhenMultiply_ThenReturnsSuccessAndPreservesMatrix) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int n = 3;

    Matrix* A = matrix_core_create(n, n, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* I = matrix_core_create(n, n, &err);
    ASSERT_NE(I, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(n, n, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Fill A with sequential values
    double val = 1.0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            EXPECT_EQ(matrix_ops_set(A, i, j, val++), CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set_identity(I), CORE_ERROR_SUCCESS);

    // A * I = A
    EXPECT_EQ(matrix_ops_multiply(A, I, R), CORE_ERROR_SUCCESS);
    ExpectMatrixEq(R, A);

    // I * A = A
    EXPECT_EQ(matrix_ops_multiply(I, A, R), CORE_ERROR_SUCCESS);
    ExpectMatrixEq(R, A);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(I), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Multiply, GivenZeroMatrix_WhenMultiply_ThenReturnsSuccessAndResultIsZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create(2, 3, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* Z = matrix_core_create(3, 4, &err);
    ASSERT_NE(Z, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(2, 4, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_fill(A, 5.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set_zero(Z), CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_multiply(A, Z, R), CORE_ERROR_SUCCESS);
    for (int i = 0; i < R->rows; ++i)
        for (int j = 0; j < R->cols; ++j) {
            CoreErrorStatus s;
            double r = matrix_ops_get(R, i, j, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(r, 0.0);
        }

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(Z), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Multiply, GivenDimensionMismatch_WhenMultiply_ThenReturnsErrDimension) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create(2, 3, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* B = matrix_core_create(2, 2, &err); // incompatible with A(2x3)
    ASSERT_NE(B, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create(2, 2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_multiply(A, B, R), CORE_ERROR_DIMENSION);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Multiply, GivenNullArguments_WhenMultiply_ThenReturnsErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* A = matrix_core_create(1, 1, &err);
    Matrix* B = matrix_core_create(1, 1, &err);
    Matrix* R = matrix_core_create(1, 1, &err);
    ASSERT_NE(A, nullptr); ASSERT_NE(B, nullptr); ASSERT_NE(R, nullptr);

    EXPECT_EQ(matrix_ops_multiply(nullptr, B, R), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_ops_multiply(A, nullptr, R), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_ops_multiply(A, B, nullptr), CORE_ERROR_NULL);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(B), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

// ========== matrix_ops_power ==========
TEST(MatrixOps_Power, GivenExponentZero_WhenPower_ThenReturnsIdentity) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int n = 3;

    Matrix* A = matrix_core_create_square(n, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Fill A with arbitrary (sequential) values
    double v = 1.0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            EXPECT_EQ(matrix_ops_set(A, i, j, v++), CORE_ERROR_SUCCESS);

    Matrix* R = matrix_core_create_square(n, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(A, 0, R), CORE_ERROR_SUCCESS);

    Matrix* I = matrix_core_create_square(n, &err);
    ASSERT_NE(I, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set_identity(I), CORE_ERROR_SUCCESS);

    ExpectMatrixEq(R, I);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(I), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenExponentOne_WhenPower_ThenReturnsSelf) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create_square(2, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // A = [[1,2],[3,4]]
    EXPECT_EQ(matrix_ops_set(A, 0, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, 2.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, 3.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 4.0), CORE_ERROR_SUCCESS);

    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(A, 1, R), CORE_ERROR_SUCCESS);
    ExpectMatrixEq(R, A);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenSmallMatrix_WhenPower2or3_ThenReturnsExpectedValues) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create_square(2, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    // A = [[1,2],[3,4]]
    EXPECT_EQ(matrix_ops_set(A, 0, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, 2.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, 3.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 4.0), CORE_ERROR_SUCCESS);

    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // n=2 -> [[7,10],[15,22]]
    EXPECT_EQ(matrix_ops_power(A, 2, R), CORE_ERROR_SUCCESS);
    {
        Matrix* E = matrix_core_create_square(2, &err);
        ASSERT_NE(E, nullptr);
        EXPECT_EQ(matrix_ops_set(E, 0, 0, 7.0), CORE_ERROR_SUCCESS);
        EXPECT_EQ(matrix_ops_set(E, 0, 1, 10.0), CORE_ERROR_SUCCESS);
        EXPECT_EQ(matrix_ops_set(E, 1, 0, 15.0), CORE_ERROR_SUCCESS);
        EXPECT_EQ(matrix_ops_set(E, 1, 1, 22.0), CORE_ERROR_SUCCESS);
        ExpectMatrixEq(R, E);
        EXPECT_EQ(matrix_core_free(E), CORE_ERROR_SUCCESS);
    }

    // n=3 -> [[37,54],[81,118]]
    EXPECT_EQ(matrix_ops_power(A, 3, R), CORE_ERROR_SUCCESS);
    {
        Matrix* E = matrix_core_create_square(2, &err);
        ASSERT_NE(E, nullptr);
        EXPECT_EQ(matrix_ops_set(E, 0, 0, 37.0), CORE_ERROR_SUCCESS);
        EXPECT_EQ(matrix_ops_set(E, 0, 1, 54.0), CORE_ERROR_SUCCESS);
        EXPECT_EQ(matrix_ops_set(E, 1, 0, 81.0), CORE_ERROR_SUCCESS);
        EXPECT_EQ(matrix_ops_set(E, 1, 1, 118.0), CORE_ERROR_SUCCESS);
        ExpectMatrixEq(R, E);
        EXPECT_EQ(matrix_core_free(E), CORE_ERROR_SUCCESS);
    }

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenIdentityMatrix_WhenPowerLargeExponent_ThenRemainsIdentity) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int n = 4;

    Matrix* I = matrix_core_create_square(n, &err);
    ASSERT_NE(I, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set_identity(I), CORE_ERROR_SUCCESS);

    Matrix* R = matrix_core_create_square(n, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(I, 10, R), CORE_ERROR_SUCCESS);
    ExpectMatrixEq(R, I);

    EXPECT_EQ(matrix_core_free(I), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenZeroMatrix_WhenPowerPositive_ThenRemainsZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int n = 3;

    Matrix* Z = matrix_core_create_square(n, &err);
    ASSERT_NE(Z, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set_zero(Z), CORE_ERROR_SUCCESS);

    Matrix* R = matrix_core_create_square(n, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(Z, 5, R), CORE_ERROR_SUCCESS);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            CoreErrorStatus s;
            double r = matrix_ops_get(R, i, j, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(r, 0.0);
        }

    EXPECT_EQ(matrix_core_free(Z), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenNonSquareMatrix_WhenPower_ThenReturnsErrDimension) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create(2, 3, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(A, 2, R), CORE_ERROR_DIMENSION);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenNegativeExponent_WhenPower_ThenReturnsErrInvalidArg) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;

    Matrix* A = matrix_core_create_square(2, &err);
    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(A, -1, R), CORE_ERROR_INVALID_ARG);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenNullArguments_WhenPower_ThenReturnsErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* R = matrix_core_create_square(2, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(nullptr, 2, R), CORE_ERROR_NULL);
    EXPECT_EQ(matrix_ops_power(R, 2, nullptr), CORE_ERROR_NULL);

    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Power, GivenFibonacciMatrix_WhenPowerLargeExponent_ThenSmokeTest) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int n = 2;

    Matrix* A = matrix_core_create_square(n, &err);
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Fibonacci Q-matrix: [[1,1],[1,0]]
    EXPECT_EQ(matrix_ops_set(A, 0, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 0, 1, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(A, 1, 1, 0.0), CORE_ERROR_SUCCESS);

    Matrix* R = matrix_core_create_square(n, &err);
    ASSERT_NE(R, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_power(A, 10, R), CORE_ERROR_SUCCESS);

    CoreErrorStatus s;
    double r00 = matrix_ops_get(R, 0, 0, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS);
    double r01 = matrix_ops_get(R, 0, 1, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS);
    double r10 = matrix_ops_get(R, 1, 0, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS);
    double r11 = matrix_ops_get(R, 1, 1, &s); ASSERT_EQ(s, CORE_ERROR_SUCCESS);
    EXPECT_GE(r00, 0.0);
    EXPECT_GE(r01, 0.0);
    EXPECT_GE(r10, 0.0);
    EXPECT_GE(r11, 0.0);

    EXPECT_EQ(matrix_core_free(A), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_core_free(R), CORE_ERROR_SUCCESS);
}

/* ===== Helpers ===== */
static void fill_sequential(Matrix* m, double start = 1.0, double step = 1.0) {
    ASSERT_NE(m, nullptr);
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    double v = start;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            ASSERT_EQ(matrix_ops_set(m, i, j, v), CORE_ERROR_SUCCESS);
            v += step;
        }
    }
}

static void expect_equal_matrix(const Matrix* a, const Matrix* b, double tol = 0.0) {
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    ASSERT_EQ(a->rows, b->rows);
    ASSERT_EQ(a->cols, b->cols);
    for (int i = 0; i < a->rows; ++i) {
        for (int j = 0; j < a->cols; ++j) {
            double va = matrix_ops_get(a, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double vb = matrix_ops_get(b, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            if (tol == 0.0) {
                EXPECT_DOUBLE_EQ(va, vb);
            }
            else {
                EXPECT_NEAR(va, vb, tol);
            }
        }
    }
}

// Helper: clone matrix (structure + data)
static Matrix* clone_matrix(const Matrix* src) {
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    Matrix* dst = matrix_core_create(src->rows, src->cols, &st);
    if (!dst || st != CORE_ERROR_SUCCESS) return nullptr;
    for (int i = 0; i < src->rows; ++i) {
        for (int j = 0; j < src->cols; ++j) {
            double v = matrix_ops_get(src, i, j, &st);
            if (st != CORE_ERROR_SUCCESS) return nullptr;
            if (matrix_ops_set(dst, i, j, v) != CORE_ERROR_SUCCESS) return nullptr;
        }
    }
    return dst;
}

/* =========================
 * Basic behavior
 * ========================= */

TEST(MatrixScale, GivenFactorOne_WhenScale_ThenMatrixUnchanged) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(m, 1.0);

    Matrix* before = clone_matrix(m);
    ASSERT_NE(before, nullptr);

    EXPECT_EQ(matrix_ops_scale(m, 1.0), CORE_ERROR_SUCCESS);
    expect_equal_matrix(m, before);

    matrix_core_free(before);
    matrix_core_free(m);
}

TEST(MatrixScale, GivenFactorZero_WhenScale_ThenAllZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 2, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(m, 3.0);

    EXPECT_EQ(matrix_ops_scale(m, 0.0), CORE_ERROR_SUCCESS);

    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            double v = matrix_ops_get(m, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(v, 0.0);
        }
    }
    matrix_core_free(m);
}

TEST(MatrixScale, GivenPositiveFactor_WhenScale_ThenValuesMultiplied) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(m, 1.0); // 1 2 3 / 4 5 6
    const double k = 2.5;

    EXPECT_EQ(matrix_ops_scale(m, k), CORE_ERROR_SUCCESS);

    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    double expected = 1.0;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            double v = matrix_ops_get(m, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(v, expected * k);
            expected += 1.0;
        }
    }
    matrix_core_free(m);
}

TEST(MatrixScale, GivenNegativeFactor_WhenScale_ThenValuesNegatedAndScaled) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 2, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(m, -1.0); // -1 -0 / 1 2 (depending on sequence)
    const double k = -3.0;

    EXPECT_EQ(matrix_ops_scale(m, k), CORE_ERROR_SUCCESS);

    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    double expected = -1.0;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            double v = matrix_ops_get(m, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(v, expected * k);
            expected += 1.0;
        }
    }
    matrix_core_free(m);
}

/* =========================
 * Error handling
 * ========================= */

TEST(MatrixScale, GivenNullMatrix_WhenScale_ThenErrNull) {
    EXPECT_EQ(matrix_ops_scale(nullptr, 2.0), CORE_ERROR_NULL);
}

TEST(MatrixScale, GivenNullData_WhenScale_ThenErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(1, 1, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Simulate broken matrix
    double* saved = m->data;
    m->data = nullptr;
    EXPECT_EQ(matrix_ops_scale(m, 2.0), CORE_ERROR_NULL);
    m->data = saved;

    matrix_core_free(m);
}

TEST(MatrixScale, GivenInvalidDims_WhenScale_ThenInvalidArg) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 2, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    m->rows = 0;  // invalidate
    EXPECT_EQ(matrix_ops_scale(m, 2.0), CORE_ERROR_INVALID_ARG);

    m->rows = 2;
    m->cols = 0;  // invalidate
    EXPECT_EQ(matrix_ops_scale(m, 2.0), CORE_ERROR_INVALID_ARG);

    matrix_core_free(m);
}

TEST(MatrixScale, GivenNaNFactor_WhenScale_ThenInvalidArgAndUnchanged) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 2, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(m, 1.0);
    Matrix* before = clone_matrix(m);
    ASSERT_NE(before, nullptr);

    const double nanv = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(matrix_ops_scale(m, nanv), CORE_ERROR_INVALID_ARG);

    expect_equal_matrix(m, before);
    matrix_core_free(before);
    matrix_core_free(m);
}

/* =========================
 * In-place behavior
 * ========================= */

TEST(MatrixScale, GivenInPlace_WhenScale_ThenWorks) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(3, 3, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(m, 1.0);
    Matrix* ref = clone_matrix(m);
    ASSERT_NE(ref, nullptr);

    const double k = 0.125; // 1/8
    EXPECT_EQ(matrix_ops_scale(m, k), CORE_ERROR_SUCCESS);

    // Compare with reference scaled offline
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < ref->rows; ++i) {
        for (int j = 0; j < ref->cols; ++j) {
            double v = matrix_ops_get(ref, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            ASSERT_EQ(matrix_ops_set(ref, i, j, v * k), CORE_ERROR_SUCCESS);
        }
    }
    expect_equal_matrix(m, ref);

    matrix_core_free(ref);
    matrix_core_free(m);
}


/* ===== Helpers ===== */


static Matrix* clone_by_copy(const Matrix* src) {
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    Matrix* dst = matrix_core_create(src->rows, src->cols, &st);
    if (!dst || st != CORE_ERROR_SUCCESS) return nullptr;
    EXPECT_EQ(matrix_ops_copy(src, dst), CORE_ERROR_SUCCESS);
    return dst;
}


/* ===== Normal cases ===== */

TEST(MatrixAxpy, GivenGeneralAlpha_WhenAxpy_ThenYEqualsY0PlusAlphaX) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(2, 3, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 3, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, 10.0, 1.0); // 10..15
    fill_sequential(X, 1.0, 1.0); // 1..6
    Matrix* Y0 = clone_by_copy(Y); ASSERT_NE(Y0, nullptr);

    const double alpha = 2.5;
    EXPECT_EQ(matrix_ops_axpy(Y, alpha, X), CORE_ERROR_SUCCESS);

    // Check Y == Y0 + alpha*X
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < Y->rows; ++i) {
        for (int j = 0; j < Y->cols; ++j) {
            double y0 = matrix_ops_get(Y0, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double x = matrix_ops_get(X, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double y = matrix_ops_get(Y, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(y, y0 + alpha * x);
        }
    }

    matrix_core_free(Y0);
    matrix_core_free(X);
    matrix_core_free(Y);
}

TEST(MatrixAxpy, GivenAlphaZero_WhenAxpy_ThenYUnchanged) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(3, 2, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(3, 2, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, -2.0, 0.5);
    fill_sequential(X, 5.0, 1.0);
    Matrix* Y0 = clone_by_copy(Y); ASSERT_NE(Y0, nullptr);

    EXPECT_EQ(matrix_ops_axpy(Y, 0.0, X), CORE_ERROR_SUCCESS);
    expect_equal_matrix(Y, Y0);

    matrix_core_free(Y0);
    matrix_core_free(X);
    matrix_core_free(Y);
}

TEST(MatrixAxpy, GivenAlphaOne_WhenAxpy_ThenYPlusX) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(2, 2, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 2, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, 1.0, 1.0); // 1,2;3,4
    fill_sequential(X, 4.0, -0.5);// 4,3.5;3,2.5
    Matrix* ref = clone_by_copy(Y); ASSERT_NE(ref, nullptr);

    // ref <- ref + X
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < ref->rows; ++i) {
        for (int j = 0; j < ref->cols; ++j) {
            double v = matrix_ops_get(ref, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double x = matrix_ops_get(X, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            ASSERT_EQ(matrix_ops_set(ref, i, j, v + x), CORE_ERROR_SUCCESS);
        }
    }

    EXPECT_EQ(matrix_ops_axpy(Y, 1.0, X), CORE_ERROR_SUCCESS);
    expect_equal_matrix(Y, ref);

    matrix_core_free(ref);
    matrix_core_free(X);
    matrix_core_free(Y);
}

TEST(MatrixAxpy, GivenNegativeAlpha_WhenAxpy_ThenSubtractScaledX) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(2, 3, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 3, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, 0.0, 2.0);
    fill_sequential(X, 1.0, 0.5);
    Matrix* Y0 = clone_by_copy(Y); ASSERT_NE(Y0, nullptr);

    const double alpha = -3.0;
    EXPECT_EQ(matrix_ops_axpy(Y, alpha, X), CORE_ERROR_SUCCESS);

    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < Y->rows; ++i) {
        for (int j = 0; j < Y->cols; ++j) {
            double y = matrix_ops_get(Y, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double y0 = matrix_ops_get(Y0, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double x = matrix_ops_get(X, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(y, y0 + alpha * x);
        }
    }

    matrix_core_free(Y0);
    matrix_core_free(X);
    matrix_core_free(Y);
}

/* ===== In-place (Y == X) ===== */

TEST(MatrixAxpy, GivenInPlace_WhenAxpy_ThenYUpdatedAsYPlusAlphaY) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(3, 3, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, 1.0, 1.0);   // 1..9
    Matrix* Y0 = clone_by_copy(Y);  // keep original
    ASSERT_NE(Y0, nullptr);

    const double alpha = 0.25;
    // In-place: X == Y
    EXPECT_EQ(matrix_ops_axpy(Y, alpha, Y), CORE_ERROR_SUCCESS);

    // Expect: Y = (1 + alpha) * Y0
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < Y->rows; ++i) {
        for (int j = 0; j < Y->cols; ++j) {
            double y = matrix_ops_get(Y, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double y0 = matrix_ops_get(Y0, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            EXPECT_DOUBLE_EQ(y, (1.0 + alpha) * y0);
        }
    }

    matrix_core_free(Y0);
    matrix_core_free(Y);
}

/* ===== Error handling ===== */

TEST(MatrixAxpy, GivenNullY_WhenAxpy_ThenErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* X = matrix_core_create(1, 1, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_axpy(nullptr, 2.0, X), CORE_ERROR_NULL);
    matrix_core_free(X);
}

TEST(MatrixAxpy, GivenNullX_WhenAxpy_ThenErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(1, 1, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_axpy(Y, 2.0, nullptr), CORE_ERROR_NULL);
    matrix_core_free(Y);
}

TEST(MatrixAxpy, GivenDimMismatch_WhenAxpy_ThenInvalidArg) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(2, 3, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(3, 2, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_axpy(Y, 1.0, X), CORE_ERROR_INVALID_ARG);

    matrix_core_free(X);
    matrix_core_free(Y);
}

TEST(MatrixAxpy, GivenNullData_WhenAxpy_ThenErrNull) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(2, 2, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 2, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Break destination data pointer
    double* saved = Y->data;
    Y->data = nullptr;
    EXPECT_EQ(matrix_ops_axpy(Y, 1.0, X), CORE_ERROR_NULL);
    Y->data = saved;

    // Break source data pointer
    saved = X->data;
    X->data = nullptr;
    EXPECT_EQ(matrix_ops_axpy(Y, 1.0, X), CORE_ERROR_NULL);
    X->data = saved;

    matrix_core_free(X);
    matrix_core_free(Y);
}

/* ===== Robustness / bigger size ===== */

TEST(MatrixAxpy, GivenLargeMatrix_WhenAxpy_ThenAllElementsMatchReference) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    const int R = 64, C = 96;
    Matrix* Y = matrix_core_create(R, C, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(R, C, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, 0.0, 0.1);
    fill_sequential(X, -5.0, 0.2);
    Matrix* Yref = clone_by_copy(Y); ASSERT_NE(Yref, nullptr);

    const double alpha = -0.75;

    // Compute reference: Yref = Yref + alpha * X
    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            double y0 = matrix_ops_get(Yref, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            double x = matrix_ops_get(X, i, j, &st);
            ASSERT_EQ(st, CORE_ERROR_SUCCESS);
            ASSERT_EQ(matrix_ops_set(Yref, i, j, y0 + alpha * x), CORE_ERROR_SUCCESS);
        }
    }

    EXPECT_EQ(matrix_ops_axpy(Y, alpha, X), CORE_ERROR_SUCCESS);
    expect_equal_matrix(Y, Yref, /*tol=*/0.0);

    matrix_core_free(Yref);
    matrix_core_free(X);
    matrix_core_free(Y);
}

// ========== matrix_ops_print ==========
TEST(MatrixOps_Print, GivenValidMatrix_WhenPrint_ThenOutputsExpectedString) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 2, &err);
    ASSERT_NE(m, nullptr);
    ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    EXPECT_EQ(matrix_ops_set(m, 0, 0, 1.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(m, 0, 1, 2.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(m, 1, 0, 3.0), CORE_ERROR_SUCCESS);
    EXPECT_EQ(matrix_ops_set(m, 1, 1, 4.0), CORE_ERROR_SUCCESS);

    testing::internal::CaptureStdout();
    EXPECT_EQ(matrix_ops_print(m), CORE_ERROR_SUCCESS);
    std::string output = testing::internal::GetCapturedStdout();

    std::string expected =
        "Matrix (2 x 2):\n"
        "[   1.000000   2.000000 ]\n"
        "[   3.000000   4.000000 ]\n\n";
    EXPECT_EQ(output, expected);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixOps_Print, GivenNullMatrix_WhenPrint_ThenReturnsErrNull) {
    EXPECT_EQ(matrix_ops_print(nullptr), CORE_ERROR_NULL);
}

/* ===== Optional: behavior with NaN alpha =====
   Enable one of the following depending on your contract.

TEST(MatrixAxpy, GivenNaNAlpha_WhenAxpy_ThenInvalidArgAndUnchanged) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(2, 2, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(2, 2, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    fill_sequential(Y, 1.0, 1.0);
    Matrix* Y0 = clone_by_copy(Y); ASSERT_NE(Y0, nullptr);

    const double nanv = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(matrix_ops_axpy(Y, nanv, X), CORE_ERROR_INVALID_ARG);
    expect_equal_matrix(Y, Y0);

    matrix_core_free(Y0);
    matrix_core_free(X);
    matrix_core_free(Y);
}

TEST(MatrixAxpy, GivenNaNAlpha_WhenAxpy_ThenYBecomesNaNWhereXNonZero) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* Y = matrix_core_create(1, 3, &err);
    ASSERT_NE(Y, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);
    Matrix* X = matrix_core_create(1, 3, &err);
    ASSERT_NE(X, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

    // Fill Y and X
    ASSERT_EQ(matrix_ops_set(Y, 0, 0, 1.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(Y, 0, 1, 2.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(Y, 0, 2, 3.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(X, 0, 0, 0.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(X, 0, 1, 5.0), CORE_ERROR_SUCCESS);
    ASSERT_EQ(matrix_ops_set(X, 0, 2, -7.0), CORE_ERROR_SUCCESS);

    const double nanv = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(matrix_ops_axpy(Y, nanv, X), CORE_ERROR_SUCCESS); // if NaN allowed

    CoreErrorStatus st = CORE_ERROR_SUCCESS;
    double v0 = matrix_ops_get(Y, 0, 0, &st); ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    double v1 = matrix_ops_get(Y, 0, 1, &st); ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    double v2 = matrix_ops_get(Y, 0, 2, &st); ASSERT_EQ(st, CORE_ERROR_SUCCESS);
    // Only entries where X != 0 become NaN
    EXPECT_FALSE(std::isnan(v0));
    EXPECT_TRUE (std::isnan(v1));
    EXPECT_TRUE (std::isnan(v2));

    matrix_core_free(X);
    matrix_core_free(Y);
}
*/
