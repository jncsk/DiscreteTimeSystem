#include <gtest/gtest.h>
extern "C" {
  #include "matrix_core.h"
  #include "matrix_ops.h"
  #include "state_space.h"
}

// Helper: matrix is valid & sized
static void ExpectMatrixSize(const Matrix* M, int r, int c) {
    ASSERT_NE(M, nullptr);
    EXPECT_EQ(M->rows, r);
    EXPECT_EQ(M->cols, c);
    ASSERT_NE(M->data, nullptr);
}

// ===== Create/Free Success =====
TEST(StateSpace_Create, GivenValidDims_WhenCreate_ThenReturnsSuccessAndValidMatrices) {
    MatrixCoreStatus err = MATRIX_CORE_SUCCESS;
    const int n = 3, m = 2, p = 4;

    StateSpaceModel* sys = state_space_create(n, m, p, &err);
    ASSERT_NE(sys, nullptr);
    EXPECT_EQ(err, MATRIX_CORE_SUCCESS);

    ExpectMatrixSize(sys->A, n, n);
    ExpectMatrixSize(sys->B, n, m);
    ExpectMatrixSize(sys->C, p, n);

    EXPECT_EQ(state_space_free(sys), MATRIX_CORE_SUCCESS);
}

// ===== Create Error: Bad dims =====
TEST(StateSpace_Create, GivenNonPositiveDims_WhenCreate_ThenReturnsErrOutOfBoundsAndNull) {
    MatrixCoreStatus err = MATRIX_CORE_SUCCESS;

    StateSpaceModel* sys = state_space_create(0, 2, 3, &err);
    EXPECT_EQ(sys, nullptr);
    EXPECT_EQ(err, MATRIX_CORE_ERR_OUT_OF_BOUNDS);

    sys = state_space_create(2, -1, 3, &err);
    EXPECT_EQ(sys, nullptr);
    EXPECT_EQ(err, MATRIX_CORE_ERR_OUT_OF_BOUNDS);
}

// ===== Free Error: Null =====
TEST(StateSpace_Free, GivenNullModel_WhenFree_ThenReturnsErrNull) {
    EXPECT_EQ(state_space_free(NULL), MATRIX_CORE_ERR_NULL);
}

// ===== Free Safety: Partially allocated cleanup =====
TEST(StateSpace_Free, GivenPartiallyAllocatedModel_WhenFree_ThenCleansUpSafely) {
    // Manually craft a partially allocated model
    MatrixCoreStatus err = MATRIX_CORE_SUCCESS;
    StateSpaceModel* sys = (StateSpaceModel*)calloc(1, sizeof(StateSpaceModel));
    ASSERT_NE(sys, nullptr);

    sys->A = matrix_core_create(2, 2, &err);
    ASSERT_NE(sys->A, nullptr);

    // B, C left as NULL on purpose
    EXPECT_EQ(state_space_free(sys), MATRIX_CORE_SUCCESS);
}

