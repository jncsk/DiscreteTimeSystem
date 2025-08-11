#include <gtest/gtest.h>
extern "C" {
#include "bit_utils.h"
#include "core_error.h"
}

TEST(BitUtils_ToBinaryMSB, ReturnsCORE_ERROR_SUCCESS) {
    int bits[8] = { 0 };
    int out_len = 0;

    CoreErrorStatus err = bit_utils_to_binary_msb(5, bits, 8, &out_len);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(out_len, 8);

    // 5 (00000101) → MSB順では 00000101
    int expected[8] = { 0,0,0,0,0,1,0,0 };
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(bits[i], expected[i]);
    }
}

TEST(BitUtils_ToBinaryMSB, ReturnsCORE_ERROR_NULL) {
    int bits[8];
    int out_len;
    EXPECT_EQ(bit_utils_to_binary_msb(5, NULL, 8, &out_len), CORE_ERROR_NULL);
    EXPECT_EQ(bit_utils_to_binary_msb(5, bits, 8, NULL), CORE_ERROR_NULL);
}

TEST(BitUtils_ToBinaryMSB, ReturnsCORE_ERROR_INVALID_ARG) {
    int bits[8];
    int out_len;
    EXPECT_EQ(bit_utils_to_binary_msb(5, bits, -1, &out_len), CORE_ERROR_INVALID_ARG);
}

TEST(BitUtils_ToBinaryLSB, ReturnsCORE_ERROR_SUCCESS) {
    int bits[8] = { 0 };
    int out_len = 0;

    CoreErrorStatus err = bit_utils_to_binary_lsb(5, bits, 8, &out_len);
    EXPECT_EQ(err, CORE_ERROR_SUCCESS);
    EXPECT_EQ(out_len, 3); // 5 = 101 → LSB順 [1,0,1]

    int expected[3] = { 1,0,1 };
    for (int i = 0; i < out_len; i++) {
        EXPECT_EQ(bits[i], expected[i]);
    }
}

TEST(BitUtils_ToBinaryLSB, ReturnsCORE_ERROR_NULL) {
    int bits[8];
    int out_len;
    EXPECT_EQ(bit_utils_to_binary_lsb(5, NULL, 8, &out_len), CORE_ERROR_NULL);
    EXPECT_EQ(bit_utils_to_binary_lsb(5, bits, 8, NULL), CORE_ERROR_NULL);
}

TEST(BitUtils_ToBinaryLSB, ReturnsCORE_ERROR_INVALID_ARG) {
    int bits[8];
    int out_len;
    EXPECT_EQ(bit_utils_to_binary_lsb(5, bits, 0, &out_len), CORE_ERROR_INVALID_ARG);
}

TEST(BitUtils_PrintBinary, ReturnsCORE_ERROR_SUCCESS) {
    EXPECT_EQ(bit_utils_print_binary(5, 8), CORE_ERROR_SUCCESS);
}

TEST(BitUtils_PrintBinary, DefaultTo32Bits) {
    // bit_length が 0 → 32 ビット表示にフォールバック
    EXPECT_EQ(bit_utils_print_binary(5, 0), CORE_ERROR_SUCCESS);
}
