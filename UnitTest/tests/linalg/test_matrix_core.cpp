#include <gtest/gtest.h>

extern "C" {
#include "matrix_core.h"
#include "matrix_ops.h"
#include "core_error.h"
}

TEST(MatrixCore_Create, ReturnsCORE_ERROR_SUCCESS) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(2, 3, &err);

    ASSERT_NE(m, nullptr);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(m->rows, 2);
    EXPECT_EQ(m->cols, 3);
    EXPECT_NE(m->data, nullptr);

    EXPECT_DOUBLE_EQ(matrix_ops_set(m, 1, 2, 42.0), CORE_ERROR_SUCCESS);

    CoreErrorStatus e2 = CORE_ERROR_SUCCESS;
    double v = matrix_ops_get(m, 1, 2, &e2);
    EXPECT_EQ(e2, CORE_ERROR_SUCCESS);
    EXPECT_EQ(v, 42.0);
    
    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixCore_Create, ReturnsCORE_ERROR_OUT_OF_BOUNDS) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m1 = matrix_core_create(-1, 3, &err);
    EXPECT_EQ(m1, nullptr);
    EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS);

    Matrix* m2 = matrix_core_create(0, 3, &err);
    EXPECT_EQ(m2, nullptr);
    EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS);

    Matrix* m3 = matrix_core_create(1, 0, &err);
    EXPECT_EQ(m3, nullptr);
    EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS);
}

TEST(MatrixCore_CreateSquare, ReturnsCORE_ERROR_SUCCESS) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create_square(3, &err);

    ASSERT_NE(m, nullptr);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(m->rows, 3);
    EXPECT_EQ(m->cols, 3);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixCore_CreateSquare, ReturnsCORE_ERROR_OUT_OF_BOUNDS) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create_square(-1, &err);

    EXPECT_EQ(err, CORE_ERROR_OUT_OF_BOUNDS);
    EXPECT_EQ(m, nullptr);
}

TEST(MatrixCore_Free, ReturnsCORE_ERROR_SUCCESS) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = matrix_core_create(5, 4, &err);

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_SUCCESS);
}

TEST(MatrixCore_Free, ReturnsCORE_ERROR_NULL) {
    CoreErrorStatus err = CORE_ERROR_SUCCESS;
    Matrix* m = NULL;

    EXPECT_EQ(matrix_core_free(m), CORE_ERROR_NULL);
}

TEST(MatrixCore_LastError, RecordsOriginFileAndLine) {
    // ÇÌÇ¥Ç∆ÉGÉâÅ[ÇèoÇ∑
    CoreErrorStatus s = matrix_core_free(NULL);
    EXPECT_NE(s, CORE_ERROR_SUCCESS);

    CoreError e = matrix_core_get_last_error();
    EXPECT_NE(e.code, CORE_ERROR_SUCCESS);
    EXPECT_NE(e.file, nullptr);
    EXPECT_GT(e.line, 0);
}
