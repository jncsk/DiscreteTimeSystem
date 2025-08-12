#include <gtest/gtest.h>

extern "C" {
#include "core_error.h"
#include "core_math_utils.h"
}

TEST(CoreMathUtils_Factorial, ReturnsSuccessForSmallN) {
    uint64_t out = 0;

    EXPECT_EQ(core_math_utils_factorial(0u, &out), CORE_ERROR_SUCCESS);
    EXPECT_EQ(out, 1u);

    out = 0;
    EXPECT_EQ(core_math_utils_factorial(1u, &out), CORE_ERROR_SUCCESS);
    EXPECT_EQ(out, 1u);

    out = 0;
    EXPECT_EQ(core_math_utils_factorial(5u, &out), CORE_ERROR_SUCCESS);
    EXPECT_EQ(out, 120u);
}

TEST(CoreMathUtils_Factorial, ReturnsSuccessFor20) {
    uint64_t out = 0;
    EXPECT_EQ(core_math_utils_factorial(20u, &out), CORE_ERROR_SUCCESS);
    EXPECT_EQ(out, 2432902008176640000ull); // 20!
}

TEST(CoreMathUtils_Factorial, ReturnsInvalidArgWhenTooLarge) {
    uint64_t out = 999; // unchanged sentinel
    EXPECT_EQ(core_math_utils_factorial(21u, &out), CORE_ERROR_INVALID_ARG);
    // é∏îséûÇÃ out ÇÃï€éùÇåµñßÇ…ï€èÿÇµÇ»Ç¢é¿ëïÇ‡Ç†ÇÈÇΩÇﬂÅAÇ±Ç±Ç≈ out ÇåüèÿÇµÇ»Ç¢/ÇµÇ»Ç≠ÇƒÇ‡OK
}

TEST(CoreMathUtils_Factorial, ReturnsNullWhenResultIsNull) {
    EXPECT_EQ(core_math_utils_factorial(0u, nullptr), CORE_ERROR_NULL);
}
