/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * B2 test API for stablesort.c internals.
 *
 * These wrappers are defined inside src/optimized/stablesort.c under
 * `#ifdef ALM_SORT_UNIT_TEST` (compiled a second time only into the gtest
 * target). They expose the file-local `static` kernels and a few internal
 * tuning constants so unit tests can drive them directly, without changing the
 * shipped library (production libm never defines ALM_SORT_UNIT_TEST).
 *
 * `keys_base` points at element 0's key; element i's key is the 8-byte double at
 * byte offset `i * stride` (same contract as the public API). `out_idx` receives
 * a permutation of [0, n). Kernels that need scratch/pools allocate them
 * internally, mirroring the production workspace layout.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Internal tuning constants (mirrors of the ALM_SORT_* macros). */
typedef struct {
    int insertion_threshold;
    int leaf_cap;
    int radix_bits;
    int small_radix_bits;
    int subl2_gate;
    int coarse_depth;
    int target_leaf;
    int max_msd_frames;
    int run_stack_cap;
    int lsd_min_n;
    int min_merge;          /* the value the planner passes to Shivers (16)  */
    int struct_min_samples;
    int struct_sample_rate;
} alm_sort_test_consts;

void alm_sort_test_get_consts(alm_sort_test_consts *out);

/* Total-order key mapping (KEY-1). */
uint64_t alm_sort_test_to_sortable(uint64_t u);

/* Sortedness probe (SAMP-1/2, PLN-1). */
size_t   alm_sort_test_struct_sample_count(size_t n);
size_t   alm_sort_test_sample_ascending_pairs(const void *keys_base,
                                              int32_t stride, size_t n,
                                              size_t *out_total);
int      alm_sort_test_is_structured(const void *keys_base, int32_t stride,
                                     size_t n);

/* Radix bit planner (RDX-1). */
int      alm_sort_test_choose_bits(size_t n_slice, int bits_hi, int frame_idx);

/* Gallop searches (GAL-1/2/3). `key` is a pre-mapped sortable key; `arr` holds
 * indices into the keys addressed by (keys_base, stride). */
int      alm_sort_test_gallop_left(uint64_t key, const int32_t *arr,
                                   const void *keys_base, int32_t stride,
                                   int base, int len, int hint);
int      alm_sort_test_gallop_right(uint64_t key, const int32_t *arr,
                                    const void *keys_base, int32_t stride,
                                    int base, int len, int hint);

/* Kernels. Each sorts the n strided keys, writing the index permutation into
 * out_idx; scratch/pools are allocated internally. */
void     alm_sort_test_insertion(const void *keys_base, int32_t stride,
                                 int32_t *out_idx, size_t n);
void     alm_sort_test_leaf_lsd8(const void *keys_base, int32_t stride,
                                 int32_t *out_idx, size_t n, int bits_hi);
void     alm_sort_test_lsd_fallback(const void *keys_base, int32_t stride,
                                    int32_t *out_idx, size_t n, int bits_hi);
void     alm_sort_test_msd(const void *keys_base, int32_t stride,
                           int32_t *out_idx, size_t n, int bits_hi);
void     alm_sort_test_lsd(const void *keys_base, int32_t stride,
                           int32_t *out_idx, size_t n);

#ifdef __cplusplus
} /* extern "C" */
#endif
