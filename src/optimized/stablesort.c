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
 * TimSort implementation with Shivers merge policy derived from :
 * C++ implementation of timsort from cpp-sort project:
 *      cpp-sort/include/cpp-sort/detail/timsort.h
 *      cpp-sort/include/cpp-sort/sorters/adaptive_shivers_sorter.h
 *
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2015-2025 Morwenn.
 * Copyright (c) 2021 Igor Kushnir <igorkuo@gmail.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Note : Timsort will be included as part of the input-aware dispatch planned 
 * in subsequent commits
 */

/*
 * Stable Sort (Keep this section isolated from other libm content)
 *
 *   Signatures:
 *     int amd_stablesort_getsize_64f(int len, int *workspace_size);
 *     int amd_stablesort_ascend_64f(const double *src, int src_stride_bytes,
 *                                   int *dst_index, int len, void *workspace);
 *
 *   amd_stablesort_ascend_64f produces, in dst_index, a stable ascending
 *   permutation of [0, len) ordering the (possibly strided) double-precision
 *   keys in src. Equal keys retain their original relative order. The caller
 *   owns the scratch (workspace); amd_stablesort_getsize_64f reports how many
 *   bytes it must provide for a given len.
 *
 *   In this first version, every input is sorted with a flat, index
 *   only LSD radix sort; input-aware dispatch planned in subsequent commits.
 *
 *   Ordering is by IEEE-754 total order: each key's bits are mapped to an
 *   unsigned "sortable" form (flip all bits for negatives, flip only the sign
 *   bit for positives) so that an unsigned radix sort yields ascending double
 *   order, with -0.0 < +0.0 and NaNs ordered by their bit patterns.
 */

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>

/* Flat 11-bit LSD over a 64-bit key: 5 x 11-bit digits + 1 x 9-bit digit. */
#define LSD_BITS    11
#define LSD_NB      (1 << LSD_BITS)                 /* 2048 buckets per pass */
#define LSD_PASSES  6
#define LSD_PF_DIST 16                              /* key-gather prefetch distance */

static inline size_t alm_sort_align64(size_t x)
{
    return (x + 63) & ~(size_t)63;
}

/*
 * Map an IEEE-754 bit pattern to an unsigned key whose natural order matches
 * ascending floating-point total order. For negatives (sign bit set) all bits
 * are flipped; for non-negatives only the sign bit is flipped.
 */
static inline uint64_t alm_sort_to_sortable(uint64_t u)
{
    int64_t s = -(int64_t)(u >> 63);
    return u ^ ((uint64_t)s | 0x8000000000000000ULL);
}

static inline uint64_t alm_sort_strided_load(const uint8_t *base,
                                             int32_t stride, size_t i)
{
    uint64_t u;
    memcpy(&u, base + (intptr_t)i * (intptr_t)stride, sizeof(u));
    return alm_sort_to_sortable(u);
}

/*
 * Flat 11-bit LSD over the whole array (index-only, IPP-style). `base` holds
 * the 4N index scratch (id_a); `pDstIdx` is the ping-pong partner. Histograms
 * and offsets are carved from `base` past the index scratch. Keys are re-read
 * from the strided source each pass. Stable; result lands in pDstIdx.
 */
static void alm_sort_do_lsd(const uint8_t *keys_base, int32_t stride,
                            int32_t *pDstIdx, size_t n, uint8_t *base)
{
    int32_t  *id_a    = (int32_t *)base;
    uint32_t *hist    = (uint32_t *)(base + alm_sort_align64(n * sizeof(int32_t)));
    uint32_t *offsets = hist + (size_t)LSD_PASSES * LSD_NB;

    static const int      pass_sh[LSD_PASSES] = {0, 11, 22, 33, 44, 55};
    static const uint64_t pass_mk[LSD_PASSES] = {0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x1ff};
    static const int      pass_nb[LSD_PASSES] = {LSD_NB, LSD_NB, LSD_NB, LSD_NB, LSD_NB, 1 << 9};

    /* Fused count: one sequential sweep reads each key once and bumps all six
     * digit histograms with immediate 11-bit shifts. */
    memset(hist, 0, (size_t)LSD_PASSES * LSD_NB * sizeof(uint32_t));
    for (size_t i = 0; i < n; ++i) {
        uint64_t k = alm_sort_strided_load(keys_base, stride, i);
        ++hist[0 * LSD_NB + (uint32_t)( k        & 0x7ff)];
        ++hist[1 * LSD_NB + (uint32_t)((k >> 11) & 0x7ff)];
        ++hist[2 * LSD_NB + (uint32_t)((k >> 22) & 0x7ff)];
        ++hist[3 * LSD_NB + (uint32_t)((k >> 33) & 0x7ff)];
        ++hist[4 * LSD_NB + (uint32_t)((k >> 44) & 0x7ff)];
        ++hist[5 * LSD_NB + (uint32_t)((k >> 55) & 0x1ff)];
    }

    /* Pass 0: scatter identity positions by digit 0 (keys read sequentially). */
    {
        uint32_t s = 0;
        for (int d = 0; d < LSD_NB; ++d) { offsets[d] = s; s += hist[d]; }
        for (size_t i = 0; i < n; ++i) {
            uint64_t k = alm_sort_strided_load(keys_base, stride, i);
            uint32_t d = (uint32_t)(k & 0x7ff);
            id_a[offsets[d]++] = (int32_t)i;
        }
    }
    int32_t *id_src = id_a;
    int32_t *id_dst = pDstIdx;

    /* Passes 1..5: read indices in order, re-read each key (random access into
     * the strided source), scatter. Per-pass constant-digit skip (order
     * preserving). */
    for (int p = 1; p < LSD_PASSES; ++p) {
        const int      shift = pass_sh[p];
        const uint64_t mask  = pass_mk[p];
        const int      nb    = pass_nb[p];
        uint32_t *hp = hist + (size_t)p * LSD_NB;

        const uint64_t k0 = alm_sort_strided_load(keys_base, stride,
                                                  (size_t)id_src[0]);
        const uint32_t d0 = (uint32_t)((k0 >> shift) & mask);
        if (hp[d0] == (uint32_t)n) continue;   /* all in same bucket: no scatter, no swap */

        uint32_t s = 0;
        for (int d = 0; d < nb; ++d) { offsets[d] = s; s += hp[d]; }

        /* The key gather keys_base[id_src[j]] is random; id_src is streamed in
         * order, so prefetch the key LSD_PF_DIST iterations ahead to overlap
         * the gather latency. Split so the bounds check is hoisted out of the
         * body: the main loop prefetches unconditionally, the tail does not. */
        const size_t lim = n > LSD_PF_DIST ? n - LSD_PF_DIST : 0;
        for (size_t j = 0; j < lim; ++j) {
            __builtin_prefetch(keys_base + (intptr_t)id_src[j + LSD_PF_DIST]
                                          * (intptr_t)stride, 0, 0);
            int32_t  i = id_src[j];
            uint64_t k = alm_sort_strided_load(keys_base, stride, (size_t)i);
            uint32_t d = (uint32_t)((k >> shift) & mask);
            id_dst[offsets[d]++] = i;
        }
        for (size_t j = lim; j < n; ++j) {
            int32_t  i = id_src[j];
            uint64_t k = alm_sort_strided_load(keys_base, stride, (size_t)i);
            uint32_t d = (uint32_t)((k >> shift) & mask);
            id_dst[offsets[d]++] = i;
        }
        int32_t *tmp = id_src; id_src = id_dst; id_dst = tmp;
    }

    if (id_src != pDstIdx)
        memcpy(pDstIdx, id_src, n * sizeof(int32_t));
}

/*
 * Report the scratch size (bytes) needed by amd_stablesort_ascend_64f for the
 * given len. Layout mirrors the LSD kernel: 64 bytes of alignment slack, the
 * 4N (aligned) index scratch, then the fused histogram and offset tables.
 * Returns 0 on success, -1 on invalid arguments.
 */
int ALM_PROTO_OPT(stablesort_getsize_64f)(int len, int *workspace_size)
{
    if (workspace_size == NULL)
        return -1;

    if (len < 0) {
        *workspace_size = 0;
        return -1;
    }

    size_t n = (size_t)len;
    size_t bytes = (size_t)64 // alignment slack for aligning the caller's base up to 64 bytes
                 + alm_sort_align64(n * sizeof(int32_t))
                 + (size_t)LSD_PASSES * LSD_NB * sizeof(uint32_t)
                 + (size_t)LSD_NB * sizeof(uint32_t);

    if (bytes > (size_t)INT_MAX) { // too large for int
        *workspace_size = 0;
        return -1;
    }

    *workspace_size = (int)bytes;
    return 0;
}

/*
 * Stable ascending argsort of the strided double-precision keys in src.
 * dst_index receives a permutation of [0, len). Returns 0 on success, -1 on
 * invalid arguments.
 */
int ALM_PROTO_OPT(stablesort_ascend_64f)(const double *src, int src_stride_bytes,
                                         int *dst_index, int len, void *workspace)
{
    if (src == NULL || dst_index == NULL)
        return -1;

    if (len < 0)
        return -1;

    if (len == 0)
        return 0;

    if (workspace == NULL)
        return -1;

    size_t        n         = (size_t)len;
    int32_t       stride    = (int32_t)src_stride_bytes;
    const uint8_t *keys_base = (const uint8_t *)src;
    int32_t       *pDstIdx   = (int32_t *)dst_index;

    /* The caller's workspace may be unaligned; align the working base to 64. */
    uint8_t *base = (uint8_t *)workspace;
    base = (uint8_t *)alm_sort_align64((uintptr_t)base);

    alm_sort_do_lsd(keys_base, stride, pDstIdx, n, base);

    return 0;
}
