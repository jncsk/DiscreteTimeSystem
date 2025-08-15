#include <gtest/gtest.h>

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
    ASSERT_NE(A, nullptr); ASSERT_EQ(err, CORE_ERROR_SUCCESS);

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

