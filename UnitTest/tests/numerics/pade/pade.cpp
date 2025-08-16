#include <gtest/gtest.h>

extern "C" {
#include "pade_exp_coeffs.h"
}

namespace {

struct PadeParam {
    int m;
    int coeffCount;
};

class PadeExpGetTableTest : public ::testing::TestWithParam<PadeParam> {};

TEST_P(PadeExpGetTableTest, ReturnsValidTableForSupportedOrder) {
    const PadeParam param = GetParam();
    const PadeExpTable* tbl = pade_exp_get_table(param.m);
    ASSERT_NE(tbl, nullptr);
    EXPECT_EQ(tbl->m, param.m);
    ASSERT_NE(tbl->even, nullptr);
    ASSERT_NE(tbl->odd, nullptr);
    EXPECT_EQ(tbl->even_len, param.coeffCount);
    EXPECT_EQ(tbl->odd_len, param.coeffCount);
}

INSTANTIATE_TEST_SUITE_P(
    SupportedOrders,
    PadeExpGetTableTest,
    ::testing::Values(
        PadeParam{3, 2},
        PadeParam{5, 3},
        PadeParam{7, 4},
        PadeParam{9, 5},
        PadeParam{13, 7}
    ));

TEST(PadeExpGetTableTest, UnsupportedOrdersReturnNull) {
    const int unsupported[] = {0, 1, 2, 4, 6, 8, 10, 11, 12, 14};
    for (int m : unsupported) {
        EXPECT_EQ(pade_exp_get_table(m), nullptr);
    }
}

} // namespace

