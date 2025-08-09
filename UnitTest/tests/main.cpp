#include <gtest/gtest.h>

extern "C" {
#include "matrix_core.h"
#include "matrix_ops.h"
}

TEST(MatrixCore_Create, ReturnsNullWhenSizeIsInvalid) {
    MatrixCoreStatus err = MATRIX_CORE_SUCCESS;
    Matrix* m = matrix_core_create(-1, 3, &err);
    EXPECT_EQ(m, nullptr);
    EXPECT_NE(err, MATRIX_CORE_SUCCESS);
}

TEST(MatrixCore_CreateSquare, AllocatesAndFrees) {
    MatrixCoreStatus err = MATRIX_CORE_SUCCESS;
    Matrix* m = matrix_core_create_square(3, &err);
    ASSERT_NE(m, nullptr);
    EXPECT_EQ(err, MATRIX_CORE_SUCCESS);
    EXPECT_EQ(m->rows, 3);
    EXPECT_EQ(m->cols, 3);

    EXPECT_EQ(matrix_core_free(m), MATRIX_CORE_SUCCESS);
}

TEST(MatrixCore_LastError, RecordsOriginFileAndLine) {
    // わざとエラーを出す
    MatrixCoreStatus s = matrix_core_free(NULL);
    EXPECT_NE(s, MATRIX_CORE_SUCCESS);

    MatrixError e = matrix_core_get_last_error();
    EXPECT_NE(e.code, MATRIX_CORE_SUCCESS);
    EXPECT_NE(e.file, nullptr);
    EXPECT_GT(e.line, 0);
}