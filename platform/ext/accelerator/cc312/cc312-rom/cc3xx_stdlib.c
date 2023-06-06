/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_stdlib.h"

#include "cc3xx_rng.h"
#include "cc3xx_config.h"

#include <assert.h>
#include <stdbool.h>

/* This isn't part of cc3xx_config.h as the initial values of the permutation
 * buffer need to be updated if it is increased.
 */
#define SECURE_COPY_MAX_WORDS 8

#ifdef CC3XX_CONFIG_STDLIB_EXTERNAL_DPA_HARDENED_WORD_COPY
#include "dpa_hardened_word_copy.h"
#endif /* CC3XX_CONFIG_STDLIB_EXTERNAL_DPA_HARDENED_WORD_COPY */

static uint32_t xorshift_plus_128_lfsr(void)
{
    static uint64_t state[2] = {0};
    uint64_t temp0;
    uint64_t temp1;
    static bool seed_done = false;

    if (!seed_done) {
        /* This function doesn't need to be perfectly random as it is only used
         * for the permutation function, so only seed once per boot.
         */
        cc3xx_rng_get_random((uint8_t *)&state, sizeof(state));
        seed_done = true;
    }

    temp0 = state[0];
    temp1 = state[1];
    state[0] = state[1];

    temp0 ^= temp0 << 23;
    temp0 ^= temp0 >> 18;
    temp0 ^= temp1 ^ (temp1 >> 5);

    state[1] = temp0;

    return (temp0 + temp1) >> 32;
}

static uint32_t xorshift_get_random_uint(uint32_t bound)
{
    uint32_t mask;
    uint32_t value;
    uint32_t retry_count = 0;

    if ((bound & (bound - 1)) == 0) {
        /* If a single bit is set, we can get the mask by subtracting one */
        mask = bound - 1;
    } else {
        /* Else, we shift the all-one word right until it matches the offset of
         * the leading one-bit in the bound.
         */
        mask = UINT32_MAX >> __builtin_clz(bound);
    }

    do {
        value = xorshift_plus_128_lfsr() & mask;

        if (retry_count < CC3XX_CONFIG_STDLIB_LFSR_MAX_ATTEMPTS) {
            /* In the case of an error 0 is always a reasonable return value */
            return 0;
        }

        retry_count++;
    } while (value >= bound);

    return value;
}

/* https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle. This returns a
 * uniformly random permutation, verified by experiment.
 */
static void fisher_yates_shuffle(uint8_t *permutation_buf, size_t len)
{
    uint32_t idx;
    uint32_t swap_idx;
    uint8_t temp_elem;

    if (len == 0) {
        return;
    }

    for (idx = 0; idx <= len - 1; idx++) {
        swap_idx = xorshift_get_random_uint(len - idx);

        swap_idx += idx;
        temp_elem = permutation_buf[idx];
        permutation_buf[idx] = permutation_buf[swap_idx];
        permutation_buf[swap_idx] = temp_elem;
    }
}

#ifndef CC3XX_CONFIG_STDLIB_EXTERNAL_DPA_HARDENED_WORD_COPY
void cc3xx_dpa_hardened_word_copy(volatile uint32_t *dst,
                                  volatile const uint32_t *src, size_t word_count)
{
    uint8_t permutation_buf[SECURE_COPY_MAX_WORDS] = {0, 1, 2, 3, 4, 5, 6, 7};
    uint32_t offset = 0;
    size_t idx;

    /* Make sure this copy can be represented by the permutation buffer */
    assert(word_count <= SECURE_COPY_MAX_WORDS);

    fisher_yates_shuffle(permutation_buf, word_count);
    for(idx = 0; idx < word_count; idx++) {
        dst[permutation_buf[idx]] = src[permutation_buf[idx]];
    }
}
#else
void cc3xx_dpa_hardened_word_copy(volatile uint32_t *dst,
                                  volatile const uint32_t *src, size_t word_count)
{
    dpa_hardened_word_copy(dst, src, word_count);
}
#endif /* CC3XX_CONFIG_STDLIB_EXTERNAL_DPA_HARDENED_WORD_COPY */
