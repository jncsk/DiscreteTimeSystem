#include <gtest/gtest.h>

extern "C" {
#include "core_matrix.h"
#include "core_error.h"
}

TEST(CoreError_LastError, RecordsOriginFileAndLine) {
    // ÇÌÇ¥Ç∆ÉGÉâÅ[ÇèoÇ∑
    CoreErrorStatus s = matrix_core_free(NULL);
    EXPECT_NE(s, CORE_ERROR_SUCCESS);

    CoreError e = matrix_core_get_last_error();
    EXPECT_NE(e.code, CORE_ERROR_SUCCESS);
    EXPECT_NE(e.file, nullptr);
    EXPECT_GT(e.line, 0);
}