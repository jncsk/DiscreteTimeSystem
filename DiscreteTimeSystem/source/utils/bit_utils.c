#pragma once

#include "bit_utils.h"

int bit_utils_decompose(uint32_t value, int* bits, int maxBits)
{
	int i = 0;
	while (value > 0) {
		bits[i] = value % 2;
		value = value / 2;
		i++;
	}

    // 足りない分を0埋め
    while (i < maxBits) {
        bits[i++] = 0;
    }

    // 逆順に並べ替え（MSBから格納したい場合）
    for (int j = 0; j < i / 2; j++) {
        int temp = bits[j];
        bits[j] = bits[i - 1 - j];
        bits[i - 1 - j] = temp;
    }

    return i;
};


void bit_utils_print_binary(uint32_t value, int bit_length)
{
    if (bit_length <= 0 || bit_length > 32) {
        bit_length = 32;  // デフォルト値
    }

    for (int i = bit_length - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

