#include "core_error.h"
#include "bit_utils.h"

CoreErrorStatus bit_utils_to_binary_msb(uint32_t value, int* bits, int maxBits, int* out_len) {
    if (!bits || !out_len) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (maxBits <= 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

	int i = 0;

    // Convert to binary (LSB first)
	while (value > 0 && i < maxBits) {
		bits[i] = value % 2;
		value = value / 2;
		i++;
	}

    // Padding with zeros if necessary
    while (i < maxBits) {
        bits[i++] = 0;
    }

    // Reverse the order (to store starting from the MSB)
    for (int j = 0; j < i / 2; j++) {
        int temp = bits[j];
        bits[j] = bits[i - 1 - j];
        bits[i - 1 - j] = temp;
    }

    *out_len = i; 
    return CORE_ERROR_SUCCESS;
};

CoreErrorStatus bit_utils_to_binary_lsb(uint32_t value, int* bits, int maxBits, int* out_len)
{
    if (!bits || !out_len) {
        CORE_ERROR_RETURN(CORE_ERROR_NULL);
    }
    if (maxBits <= 0) {
        CORE_ERROR_RETURN(CORE_ERROR_INVALID_ARG);
    }

    int i = 0;
    while (value > 0 && i < maxBits) {
        bits[i++] = (int)(value & 1u);
        value >>= 1;
    }

    *out_len = i;
    return CORE_ERROR_SUCCESS;
};

CoreErrorStatus bit_utils_print_binary(uint32_t value, int bit_length)
{
    if (bit_length <= 0 || bit_length > 32) {
        bit_length = 32;  // Default value
    }

    for (int i = bit_length - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");

    return CORE_ERROR_SUCCESS;
}

