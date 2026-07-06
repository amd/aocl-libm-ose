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
 *   Ordering is by IEEE-754 total order: each key's bits are mapped to an
 *   unsigned "sortable" form (flip all bits for negatives, flip only the sign
 *   bit for positives) so that an unsigned radix sort yields ascending double
 *   order, with -0.0 < +0.0 and NaNs ordered by their bit patterns.
 *
 *   Input-aware dispatch (planner). Exactly one kernel runs per call:
 *     (A) Shivers adaptive merge sort (TimSort family) for small len (<= 2048)
 *         or structured (nearly-sorted / reverse) input.
 *     (B) Flat 11-bit LSD radix for mid sizes whose key-gather footprint stays
 *         L3-cache resident.
 *     (C) MSD-LSD hybrid radix (size-targeted MSD partition + insertion / 8-bit
 *         dense LSD leaf / strided-LSD fallback) for large random input.
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>

/*
 * Return-code contract for the APIs. Internal to this
 * translation unit and not part of the public amdlibm.h.
 */
typedef enum {
    AOCLSORT_STS_OK            =  0,  /* success                                */
    AOCLSORT_STS_NULL_PTR      = -1,  /* a required pointer argument was NULL    */
    AOCLSORT_STS_LENGTH_ERR    = -2,  /* len <= 0                                */
    AOCLSORT_STS_STRIDE_ERR    = -3,  /* src_stride_bytes < (int)sizeof(double)  */
    AOCLSORT_STS_SIZE_OVERFLOW = -4,  /* required workspace size exceeds INT_MAX */
} aoclsort_status;

/* ============================================================
 * Shared utilities
 * ============================================================ */

#define ALM_SORT_ALIGN64(x) (((x) + (size_t)63) & ~(size_t)63)

/*
 *   ALM_SORT_STATIC_ASSERT(cond, msg) -- compile-time check.
 *   ALM_SORT_ASSERT(cond, msg)        -- runtime check
 *   ALM_SORT_PREFETCH(addr, rw, loc)  -- rw: 0 read / 1 write; loc: 0..3 temporal.
 */
#define ALM_SORT_STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)
#define ALM_SORT_ASSERT(cond, msg)        assert((cond) && (msg))
#define ALM_SORT_PREFETCH(addr, rw, loc)  __builtin_prefetch((addr), (rw), (loc))

/*
 * Platform assumption made explicit here so it fails at compile time rather
 * than corrupt memory at runtime: the public API's `int` indices are
 * reinterpreted as int32_t (dst_index is cast to int32_t*).
 */
ALM_SORT_STATIC_ASSERT(sizeof(int) == sizeof(int32_t),
    "public API uses `int` indices that are reinterpreted as int32_t");

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

/* ============================================================
 * Sortedness probe: detects nearly-sorted / reverse / structured input
 * ============================================================ */

static inline bool alm_sort_is_structured(const uint8_t *keys_base,
                                          int32_t stride, size_t n)
{
    const int samples = 32;
    size_t step = n / (size_t)(samples + 1);
    if (step < 1)
        step = 1;

    int asc = 0;
    int total = 0;
    for (size_t i = 0; i + step < n && total < samples; i += step, ++total) {
        uint64_t a = alm_sort_strided_load(keys_base, stride, i);
        uint64_t b = alm_sort_strided_load(keys_base, stride, i + step);
        if (a <= b)
            ++asc;
    }
    if (total == 0)
        return false;
    /* 85% gate: route to Shivers only when the data is strongly run-ordered
     * (genuinely sorted/reverse sit at 100%/0%). Weakly-monotone real data
     * falls through to the LSD/radix gate, where it is parity-or-better. */
    return (asc * 20 >= total * 17) || ((total - asc) * 20 >= total * 17);
}

/* ============================================================
 * (A) Shivers adaptive merge sort (TimSort family)
 * ============================================================ */

#define ALM_SORT_MIN_GALLOP    7
#define ALM_SORT_RUN_STACK_CAP 128

typedef struct {
    const uint8_t *base;
    int32_t        stride;
} alm_sort_keys_t;

typedef struct {
    int32_t         *a;
    int32_t         *tmp;
    alm_sort_keys_t  keys;
    int              min_gallop;
    int              stack_size;
    int              min_merge;
    int             *run_base;   /* [ALM_SORT_RUN_STACK_CAP], from workspace */
    int             *run_len;    /* [ALM_SORT_RUN_STACK_CAP], from workspace */
} alm_sort_shivers_t;

static inline uint64_t alm_sort_key_at(alm_sort_keys_t keys, size_t idx)
{
    uint64_t u;
    memcpy(&u, keys.base + (intptr_t)idx * (intptr_t)keys.stride, sizeof(u));
    return alm_sort_to_sortable(u);
}

static inline void alm_sort_acopy(const int32_t *src, int sp,
                                  int32_t *dst, int dp, int len)
{
    memmove(dst + dp, src + sp, (size_t)len * sizeof(int32_t));
}

static inline int alm_sort_gallop_left(uint64_t key, const int32_t *arr,
                                       alm_sort_keys_t keys,
                                       int base, int len, int hint)
{
    int last_ofs = 0, ofs = 1;
    if (key > alm_sort_key_at(keys, (size_t)arr[base + hint])) {
        int max_ofs = len - hint;
        while (ofs < max_ofs && key > alm_sort_key_at(keys, (size_t)arr[base + hint + ofs])) {
            last_ofs = ofs; ofs = (ofs << 1) + 1; if (ofs <= 0) ofs = max_ofs;
        }
        if (ofs > max_ofs) ofs = max_ofs;
        last_ofs += hint; ofs += hint;
    } else {
        int max_ofs = hint + 1;
        while (ofs < max_ofs && key <= alm_sort_key_at(keys, (size_t)arr[base + hint - ofs])) {
            last_ofs = ofs; ofs = (ofs << 1) + 1; if (ofs <= 0) ofs = max_ofs;
        }
        if (ofs > max_ofs) ofs = max_ofs;
        int t = last_ofs; last_ofs = hint - ofs; ofs = hint - t;
    }
    ++last_ofs;
    while (last_ofs < ofs) {
        int m = last_ofs + ((ofs - last_ofs) >> 1);
        if (key > alm_sort_key_at(keys, (size_t)arr[base + m])) last_ofs = m + 1; else ofs = m;
    }
    return ofs;
}

/* Goal : return the count of elements <= K in arr[base, base+len)
 * i.e. the rightmost insertion point for K (equal keys land to its left).
 * Instead of a pure binary search, we leverage 'hint'.
 * Expectation is the element we are looking for is close to hint.
 * So we do an exponential search starting at hint
 * and stepping by 2x each time until we find the
 * range where the element is, and then binary search within it.
 * (ofs << 1) + 1 ensures [last_ofs, ofs) is power of 2 and gap free.
 */
static inline int alm_sort_gallop_right(uint64_t key, const int32_t *arr,
                                        alm_sort_keys_t keys,
                                        int base, int len, int hint)
{
    int ofs = 1, last_ofs = 0;
    if (key < alm_sort_key_at(keys, (size_t)arr[base + hint])) {
        int max_ofs = hint + 1;
        while (ofs < max_ofs && key < alm_sort_key_at(keys, (size_t)arr[base + hint - ofs])) {
            last_ofs = ofs; ofs = (ofs << 1) + 1; if (ofs <= 0) ofs = max_ofs;
        }
        if (ofs > max_ofs) ofs = max_ofs;
        int t = last_ofs; last_ofs = hint - ofs; ofs = hint - t;
    } else {
        int max_ofs = len - hint;
        while (ofs < max_ofs && key >= alm_sort_key_at(keys, (size_t)arr[base + hint + ofs])) {
            last_ofs = ofs; ofs = (ofs << 1) + 1; if (ofs <= 0) ofs = max_ofs;
        }
        if (ofs > max_ofs) ofs = max_ofs;
        last_ofs += hint; ofs += hint;
    }
    /* arr[base + last_ofs] <= key < arr[base + ofs] now; find exact point. */
    ++last_ofs;
    while (last_ofs < ofs) {
        int m = last_ofs + ((ofs - last_ofs) >> 1);
        if (key < alm_sort_key_at(keys, (size_t)arr[base + m])) ofs = m; else last_ofs = m + 1;
    }
    return ofs;
}

static inline uint64_t alm_sort_shivers_key(const alm_sort_shivers_t *ts, int v)
{
    return alm_sort_key_at(ts->keys, (size_t)v);
}

static void alm_sort_shivers_reverse_range(alm_sort_shivers_t *ts, int lo, int hi)
{
    --hi;
    while (lo < hi) {
        int32_t t = ts->a[lo]; ts->a[lo] = ts->a[hi]; ts->a[hi] = t;
        ++lo; --hi;
    }
}

/* ascending : non-strict (>=) as its a left-to-right pass and ensures stability
 * descending : strict (<) as its a right-to-left pass and reverse will break stability
 * if arr starts with equal keys -> take ascending branch */
static int alm_sort_shivers_count_run(alm_sort_shivers_t *ts, int lo, int hi)
{
    int run_hi = lo + 1;
    if (run_hi == hi)
        return 1;
    if (alm_sort_shivers_key(ts, ts->a[run_hi++]) < alm_sort_shivers_key(ts, ts->a[lo])) {
        while (run_hi < hi && alm_sort_shivers_key(ts, ts->a[run_hi]) < alm_sort_shivers_key(ts, ts->a[run_hi - 1]))
            ++run_hi;
        alm_sort_shivers_reverse_range(ts, lo, run_hi);
    } else {
        while (run_hi < hi && !(alm_sort_shivers_key(ts, ts->a[run_hi]) < alm_sort_shivers_key(ts, ts->a[run_hi - 1])))
            ++run_hi;
    }
    return run_hi - lo;
}

/* If strictly (<) pivot, search in left half, else right half. This ensures stability by
 * placing equal keys in input order. */
static void alm_sort_shivers_binary_sort(alm_sort_shivers_t *ts, int lo, int hi, int start)
{
    if (start == lo)
        ++start; /* first element is trivially sorted */
    for (; start < hi; ++start) {
        int32_t pivot = ts->a[start];
        uint64_t pk = alm_sort_shivers_key(ts, pivot);
        int left = lo, right = start;
        while (left < right) {
            int mid = (left + right) >> 1;
            if (pk < alm_sort_shivers_key(ts, ts->a[mid])) right = mid; else left = mid + 1;
        }
        for (int k = start; k > left; --k)
            ts->a[k] = ts->a[k - 1];
        ts->a[left] = pivot;
    }
}

static void alm_sort_shivers_push_run(alm_sort_shivers_t *ts, int base, int len)
{
    ts->run_base[ts->stack_size] = base;
    ts->run_len[ts->stack_size]  = len;
    ++ts->stack_size;
}

static void alm_sort_shivers_merge_lo(alm_sort_shivers_t *ts, int base1, int len1,
                                      int base2, int len2)
{
    alm_sort_acopy(ts->a, base1, ts->tmp, 0, len1);
    int cursor1 = 0, cursor2 = base2, dest = base1;
    ts->a[dest++] = ts->a[cursor2++];
    if (--len2 == 0) { alm_sort_acopy(ts->tmp, cursor1, ts->a, dest, len1); return; }
    if (len1 == 1)  { alm_sort_acopy(ts->a, cursor2, ts->a, dest, len2); ts->a[dest + len2] = ts->tmp[cursor1]; return; }

    int mg = ts->min_gallop;
    while (true) {
        int count1 = 0, count2 = 0;
        do {
            if (alm_sort_shivers_key(ts, ts->a[cursor2]) < alm_sort_shivers_key(ts, ts->tmp[cursor1])) {
                ts->a[dest++] = ts->a[cursor2++]; ++count2; count1 = 0;
                if (--len2 == 0) goto done;
            } else {
                ts->a[dest++] = ts->tmp[cursor1++]; ++count1; count2 = 0;
                if (--len1 == 1) goto done;
            }
        } while ((count1 | count2) < mg);
        do {
            count1 = alm_sort_gallop_right(alm_sort_shivers_key(ts, ts->a[cursor2]), ts->tmp, ts->keys, cursor1, len1, 0);
            if (count1 != 0) {
                alm_sort_acopy(ts->tmp, cursor1, ts->a, dest, count1);
                dest += count1; cursor1 += count1; len1 -= count1;
                if (len1 <= 1) goto done;
            }
            ts->a[dest++] = ts->a[cursor2++];
            if (--len2 == 0) goto done;
            count2 = alm_sort_gallop_left(alm_sort_shivers_key(ts, ts->tmp[cursor1]), ts->a, ts->keys, cursor2, len2, 0);
            if (count2 != 0) {
                alm_sort_acopy(ts->a, cursor2, ts->a, dest, count2);
                dest += count2; cursor2 += count2; len2 -= count2;
                if (len2 == 0) goto done;
            }
            ts->a[dest++] = ts->tmp[cursor1++];
            if (--len1 == 1) goto done;
            --mg;
        } while (count1 >= ALM_SORT_MIN_GALLOP || count2 >= ALM_SORT_MIN_GALLOP);
        if (mg < 0) mg = 0;
        mg += 2;
    }
done:
    ts->min_gallop = mg < 1 ? 1 : mg;
    if (len1 == 1) { alm_sort_acopy(ts->a, cursor2, ts->a, dest, len2); ts->a[dest + len2] = ts->tmp[cursor1]; }
    else           { alm_sort_acopy(ts->tmp, cursor1, ts->a, dest, len1); }
}

static void alm_sort_shivers_merge_hi(alm_sort_shivers_t *ts, int base1, int len1,
                                      int base2, int len2)
{
    alm_sort_acopy(ts->a, base2, ts->tmp, 0, len2);
    int cursor1 = base1 + len1 - 1, cursor2 = len2 - 1, dest = base2 + len2 - 1;
    ts->a[dest--] = ts->a[cursor1--];
    if (--len1 == 0) { alm_sort_acopy(ts->tmp, 0, ts->a, dest - (len2 - 1), len2); return; }
    if (len2 == 1) {
        dest -= len1; cursor1 -= len1;
        alm_sort_acopy(ts->a, cursor1 + 1, ts->a, dest + 1, len1);
        ts->a[dest] = ts->tmp[cursor2];
        return;
    }

    int mg = ts->min_gallop;
    while (true) {
        int count1 = 0, count2 = 0;
        do {
            if (alm_sort_shivers_key(ts, ts->tmp[cursor2]) < alm_sort_shivers_key(ts, ts->a[cursor1])) {
                ts->a[dest--] = ts->a[cursor1--]; ++count1; count2 = 0;
                if (--len1 == 0) goto done;
            } else {
                ts->a[dest--] = ts->tmp[cursor2--]; ++count2; count1 = 0;
                if (--len2 == 1) goto done;
            }
        } while ((count1 | count2) < mg);
        do {
            count1 = len1 - alm_sort_gallop_right(alm_sort_shivers_key(ts, ts->tmp[cursor2]), ts->a, ts->keys, base1, len1, len1 - 1);
            if (count1 != 0) {
                dest -= count1; cursor1 -= count1; len1 -= count1;
                alm_sort_acopy(ts->a, cursor1 + 1, ts->a, dest + 1, count1);
                if (len1 == 0) goto done;
            }
            ts->a[dest--] = ts->tmp[cursor2--];
            if (--len2 == 1) goto done;
            count2 = len2 - alm_sort_gallop_left(alm_sort_shivers_key(ts, ts->a[cursor1]), ts->tmp, ts->keys, 0, len2, len2 - 1);
            if (count2 != 0) {
                dest -= count2; cursor2 -= count2; len2 -= count2;
                alm_sort_acopy(ts->tmp, cursor2 + 1, ts->a, dest + 1, count2);
                if (len2 <= 1) goto done;
            }
            ts->a[dest--] = ts->a[cursor1--];
            if (--len1 == 0) goto done;
            --mg;
        } while (count1 >= ALM_SORT_MIN_GALLOP || count2 >= ALM_SORT_MIN_GALLOP);
        if (mg < 0) mg = 0;
        mg += 2;
    }
done:
    ts->min_gallop = mg < 1 ? 1 : mg;
    if (len2 == 1) {
        dest -= len1; cursor1 -= len1;
        alm_sort_acopy(ts->a, cursor1 + 1, ts->a, dest + 1, len1);
        ts->a[dest] = ts->tmp[cursor2];
    } else {
        alm_sort_acopy(ts->tmp, 0, ts->a, dest - (len2 - 1), len2);
    }
}

static void alm_sort_shivers_merge_at(alm_sort_shivers_t *ts, int i)
{
    int base1 = ts->run_base[i], len1 = ts->run_len[i];
    int base2 = ts->run_base[i + 1], len2 = ts->run_len[i + 1];
    ts->run_len[i] = len1 + len2;
    if (i == ts->stack_size - 3) {
        ts->run_base[i + 1] = ts->run_base[i + 2];
        ts->run_len[i + 1]  = ts->run_len[i + 2];
    }
    --ts->stack_size;

    int k = alm_sort_gallop_right(alm_sort_shivers_key(ts, ts->a[base2]), ts->a, ts->keys, base1, len1, 0);
    base1 += k; len1 -= k;
    if (len1 == 0)
        return;
    len2 = alm_sort_gallop_left(alm_sort_shivers_key(ts, ts->a[base1 + len1 - 1]), ts->a, ts->keys, base2, len2, len2 - 1);
    if (len2 == 0)
        return;
    if (len1 <= len2) alm_sort_shivers_merge_lo(ts, base1, len1, base2, len2);
    else              alm_sort_shivers_merge_hi(ts, base1, len1, base2, len2);
}

static void alm_sort_shivers_merge_collapse(alm_sort_shivers_t *ts)
{
    while (ts->stack_size > 1) {
        int n = ts->stack_size - 3;
        int x = ts->run_len[n + 1] | ts->run_len[n + 2];
        /* Keep the stack alone iff the 3rd-from-top run's MSB is above BOTH top
         * runs'; otherwise merge it with its neighbour and re-check. Enforces
         * geometric growth top-to-bottom => O(log n) stack depth. */
        if (n < 0 || x <= (ts->run_len[n] & ~x))
            break;
        alm_sort_shivers_merge_at(ts, n);
    }
}

static void alm_sort_shivers_force_collapse(alm_sort_shivers_t *ts)
{
    while (ts->stack_size > 1) {
        int n = ts->stack_size - 2;
        if (n > 0 && ts->run_len[n - 1] < ts->run_len[n + 1])
            --n;
        alm_sort_shivers_merge_at(ts, n);
    }
}

static int alm_sort_shivers_min_run_length(alm_sort_shivers_t *ts, int n)
{
    int r = 0;
    while (n >= ts->min_merge) { r |= (n & 1); n >>= 1; }
    return n + r;
}

/*
 * Run-stack depth bound: merge_collapse keeps run lengths growing at least
 * geometrically from top to bottom, so k stacked runs imply total >= c^k <= n,
 * i.e. k = O(log n). n fits in int32_t, so worst-case depth is a small multiple
 * of log2(n) ~ 31, comfortably under ALM_SORT_RUN_STACK_CAP (128).
 */
static void alm_sort_shivers_run(alm_sort_shivers_t *ts, int n)
{
    if (n < 2)
        return;
    if (n < ts->min_merge) {
        int init_run_len = alm_sort_shivers_count_run(ts, 0, n);
        alm_sort_shivers_binary_sort(ts, 0, n, init_run_len);
        return;
    }
    int lo = 0, n_remaining = n;
    int min_run = alm_sort_shivers_min_run_length(ts, n);
    do {
        int run_len_cur = alm_sort_shivers_count_run(ts, lo, n);
        if (run_len_cur < min_run) {
            int force = n_remaining <= min_run ? n_remaining : min_run;
            alm_sort_shivers_binary_sort(ts, lo, lo + force, lo + run_len_cur);
            run_len_cur = force;
        }
        alm_sort_shivers_push_run(ts, lo, run_len_cur);
        alm_sort_shivers_merge_collapse(ts);
        lo += run_len_cur;
        n_remaining -= run_len_cur;
    } while (n_remaining != 0);
    alm_sort_shivers_force_collapse(ts);
}

static void alm_sort_shivers(const uint8_t *keys_base, int32_t stride,
                             int32_t *dst_idx, size_t n, int32_t *merge_tmp,
                             int *run_base, int *run_len, int min_merge)
{
    for (size_t i = 0; i < n; ++i)
        dst_idx[i] = (int32_t)i;

    alm_sort_shivers_t ts;
    ts.a = dst_idx;
    ts.tmp = merge_tmp;
    ts.keys.base = keys_base;
    ts.keys.stride = stride;
    ts.min_merge = min_merge < 16 ? 16 : min_merge;
    ts.min_gallop = ALM_SORT_MIN_GALLOP;
    ts.stack_size = 0;
    ts.run_base = run_base;
    ts.run_len  = run_len;
    alm_sort_shivers_run(&ts, (int)n);
}

/* ============================================================
 * (C) MSD-LSD hybrid radix
 * ============================================================ */

#define ALM_SORT_RADIX_BITS 11
#define ALM_SORT_RADIX      (1 << ALM_SORT_RADIX_BITS)

/*
 * Leaf / partition tuning.
 *   len <= ALM_SORT_INSERTION_THRESHOLD -> stable insertion sort
 *   len <= ALM_SORT_LEAF_CAP            -> 8-bit dense LSD leaf
 *   else                                -> size-targeted MSD partition + recurse
 */
#define ALM_SORT_INSERTION_THRESHOLD ((size_t)32)
#define ALM_SORT_TARGET_LEAF         ((size_t)1024)  /* partition aims for ~this bucket size */
#define ALM_SORT_LEAF_CAP            ((size_t)2048)
#define ALM_SORT_SUBL2_GATE          ((size_t)32768)
#define ALM_SORT_COARSE_DEPTH        2               /* depth >= this forced to full radix bits */

/*
 * Bounded MSD frames + strided-LSD overflow fallback. Only ALM_SORT_MAX_MSD_FRAMES
 * cached frames are allocated; any bucket recursing past the last frame finishes in
 * alm_sort_lsd_fallback() (pool-free), so any value >= 1 is correct for any input.
 */
#define ALM_SORT_MAX_MSD_FRAMES 3
ALM_SORT_STATIC_ASSERT(ALM_SORT_MAX_MSD_FRAMES >= 1, "need at least one MSD frame");

#define ALM_SORT_SMALL_RADIX_BITS 8
#define ALM_SORT_SMALL_RADIX      (1 << ALM_SORT_SMALL_RADIX_BITS)

/*
 * Leaf pool: dense (tkey, idx) ping-pong sized for ALM_SORT_LEAF_CAP + an 8-bit
 * histogram/offsets pair (256 entries each). One leaf is live at a time.
 */
#define ALM_SORT_LEAF_TKEY_BYTES (ALM_SORT_LEAF_CAP * sizeof(uint64_t))
#define ALM_SORT_LEAF_IDX_BYTES  (ALM_SORT_LEAF_CAP * sizeof(int32_t))
#define ALM_SORT_LEAF_HIST_BYTES ((size_t)ALM_SORT_SMALL_RADIX * sizeof(uint32_t))
#define ALM_SORT_LEAF_POOL_BYTES \
    (2 * ALM_SORT_LEAF_TKEY_BYTES + 2 * ALM_SORT_LEAF_IDX_BYTES + 2 * ALM_SORT_LEAF_HIST_BYTES)

#define ALM_SORT_LEAF_TKEY_A_OFF  ((size_t)0)
#define ALM_SORT_LEAF_TKEY_B_OFF  (ALM_SORT_LEAF_TKEY_A_OFF + ALM_SORT_LEAF_TKEY_BYTES)
#define ALM_SORT_LEAF_IDX_A_OFF   (ALM_SORT_LEAF_TKEY_B_OFF + ALM_SORT_LEAF_TKEY_BYTES)
#define ALM_SORT_LEAF_IDX_B_OFF   (ALM_SORT_LEAF_IDX_A_OFF  + ALM_SORT_LEAF_IDX_BYTES)
#define ALM_SORT_LEAF_HIST_OFF    (ALM_SORT_LEAF_IDX_B_OFF  + ALM_SORT_LEAF_IDX_BYTES)
#define ALM_SORT_LEAF_OFFSETS_OFF (ALM_SORT_LEAF_HIST_OFF   + ALM_SORT_LEAF_HIST_BYTES)
#define ALM_SORT_LEAF_PF_DIST     16  /* leaf key-gather prefetch distance */

/*
 * MSD frame (W2'): a SINGLE reused hist[ALM_SORT_RADIX] array per depth.
 * counts -> constant-check -> in-place prefix into cursors -> scatter mutates it
 * into bucket ends -> recursion recovers bounds from a running cursor.
 */
#define ALM_SORT_HIST_BYTES      ((size_t)ALM_SORT_RADIX * sizeof(uint32_t))
#define ALM_SORT_MSD_FRAME_BYTES ALM_SORT_ALIGN64(ALM_SORT_HIST_BYTES)
#define ALM_SORT_MSD_POOL_BYTES  (ALM_SORT_MSD_FRAME_BYTES * ALM_SORT_MAX_MSD_FRAMES)
#define ALM_SORT_MSD_HIST_OFF    ((size_t)0)

/*
 * Flat 11-bit LSD for mid sizes. Pure index-only LSD: indices live in
 * the 4N scratch + dst_idx ping-pong (NO dense key copy -- keys are re-read from
 * the strided source each pass). All six digit histograms (5x11 + 1x9 bits) are
 * fused into a single counting sweep; the histograms REUSE the leaf+msd pool
 * region, idle on this path. Gated by ALM_SORT_LSD_MIN_N and the L3 footprint cap.
 */
#define ALM_SORT_LSD_PF_DIST       16
#define ALM_SORT_LSD_MIN_N         ((size_t)65536)
#define ALM_SORT_LSD_L3_BYTES      (32ull * 1024 * 1024)  /* 32 MiB L3 / CCX */
#define ALM_SORT_LSD_MAX_FOOTPRINT ALM_SORT_LSD_L3_BYTES
#define ALM_SORT_LSD_BITS          11
#define ALM_SORT_LSD_NB            (1 << ALM_SORT_LSD_BITS)                          /* 2048 */
#define ALM_SORT_LSD_PASSES        ((64 + ALM_SORT_LSD_BITS - 1) / ALM_SORT_LSD_BITS) /* 6 */
ALM_SORT_STATIC_ASSERT(ALM_SORT_LSD_BITS == 11 && ALM_SORT_LSD_PASSES == 6,
               "fused count below hard-codes 6 immediate 11-bit shifts");
#define ALM_SORT_LSD_HIST_BYTES    ((size_t)ALM_SORT_LSD_PASSES * ALM_SORT_LSD_NB * sizeof(uint32_t)) /* 48 KiB */
#define ALM_SORT_LSD_OFFSETS_BYTES ((size_t)ALM_SORT_LSD_NB * sizeof(uint32_t))                       /*  8 KiB */
ALM_SORT_STATIC_ASSERT(ALM_SORT_LSD_HIST_BYTES + ALM_SORT_LSD_OFFSETS_BYTES
                   <= ALM_SORT_LEAF_POOL_BYTES + ALM_SORT_MSD_POOL_BYTES,
               "LSD histograms must fit in the reused leaf+msd pool region");

/*
 * The Shivers path overlays its scratch on the radix path's regions (safe ONLY
 * because exactly one kernel runs per call). merge_tmp aliases the N-int32
 * scratch; the run-stack (run_base + run_len, ALM_SORT_RUN_STACK_CAP each) is
 * carved at the START of the leaf pool, so it must fit in the leaf pool.
 */
ALM_SORT_STATIC_ASSERT(2u * (size_t)ALM_SORT_RUN_STACK_CAP * sizeof(int) <= ALM_SORT_LEAF_POOL_BYTES,
               "Shivers run-stack must fit in the leaf pool region it overlays");

/* ============================================================
 * Shared workspace layout (single source of truth)
 *
 * The caller's scratch (aligned to 64 -> `base`) is partitioned into two
 * top-level regions:
 *   region 0 @ 0                   : the N-element int32 index scratch
 *                                    (ping-pong partner of dst_idx).
 *   region 1 @ ALM_SORT_POOL_OFF(n): the "pool", reused by whichever single
 *                                    kernel runs (they never coexist):
 *       radix   -> leaf pool, then the MSD frame pool
 *       lsd     -> fused histograms, then the scatter offsets
 *       shivers -> the run-stack (run_base then run_len); merge_tmp aliases
 *                  region 0.
 *
 * Offsets grow downward; the three region-1 rows are mutually-exclusive
 * overlays of the same bytes (only one kernel runs per call):
 *
 *   byte offset                                                 accessor
 *   -----------  +---------------------------------------------+
 *   0            | region 0: index scratch                     |  INDEX_PTR
 *                |   N x int32   (== merge_tmp for shivers)    |  MERGE_TMP_PTR
 *   POOL_OFF(n)  +====================== region 1: pool =======+
 *                | radix   | leaf pool          | MSD frames   |  LEAF_POOL_PTR
 *                |         |                    |              |  MSD_POOL_PTR
 *                |---------+--------------------+--------------|
 *                | lsd     | histograms         | offsets      |  LSD_HIST_PTR
 *                |         | PASSES x NB x u32  |              |  LSD_OFFSETS_PTR
 *                |---------+--------------------+--------------|
 *                | shivers | run_base           | run_len      |  RUN_BASE_PTR
 *                |         | RUN_STACK_CAP x int| ... x int    |  RUN_LEN_PTR
 *   WORKSPACE_   +---------------------------------------------+
 *   BYTES(n)-64
 *   (+64 alignment slack precedes `base` inside the caller's buffer)
 *
 * Every region pointer is derived ONLY through the macros below (the code does
 * plain `PTR(base, n)` lookups, never ad-hoc base+offset arithmetic), so the
 * regions cannot accidentally overlap or be misread, and getsize() sizes the
 * buffer through the very same expressions.
 * ============================================================ */
#define ALM_SORT_INDEX_BYTES(n)  ALM_SORT_ALIGN64((size_t)(n) * sizeof(int32_t))
#define ALM_SORT_POOL_OFF(n)     ALM_SORT_INDEX_BYTES(n)

#define ALM_SORT_WORKSPACE_BYTES(n)                                          \
    ((size_t)64 + ALM_SORT_INDEX_BYTES(n)                                    \
     + ALM_SORT_LEAF_POOL_BYTES + ALM_SORT_MSD_POOL_BYTES)

/* Region 0 -- N int32 index scratch (radix/lsd) and Shivers merge_tmp alias. */
#define ALM_SORT_INDEX_PTR(base, n)       ((int32_t *)(base))
#define ALM_SORT_MERGE_TMP_PTR(base, n)   ((int32_t *)(base))

/* Start of region 1 (the shared pool). Each kernel anchors its region-1 view
 * here and reinterprets the same bytes.
 * Region 1's size is set by the radix path (the largest consumer); the lsd and
 * shivers footprints are asserted above to fit within it. */
#define ALM_SORT_POOL_PTR(base, n)        ((uint8_t *)(base) + ALM_SORT_POOL_OFF(n))

/* Region 1 -- radix view: leaf pool (at pool start) then the MSD frame pool. */
#define ALM_SORT_LEAF_POOL_PTR(base, n)   ALM_SORT_POOL_PTR(base, n)
#define ALM_SORT_MSD_POOL_PTR(base, n)    (ALM_SORT_POOL_PTR(base, n) + ALM_SORT_LEAF_POOL_BYTES)

/* Region 1 -- lsd view: fused histograms (at pool start) then scatter offsets. */
#define ALM_SORT_LSD_HIST_PTR(base, n)    ((uint32_t *)ALM_SORT_POOL_PTR(base, n))
#define ALM_SORT_LSD_OFFSETS_PTR(base, n) \
    (ALM_SORT_LSD_HIST_PTR(base, n) + (size_t)ALM_SORT_LSD_PASSES * ALM_SORT_LSD_NB)

/* Region 1 -- shivers view: run-stack at pool start (run_base then run_len). */
#define ALM_SORT_RUN_BASE_PTR(base, n)    ((int *)ALM_SORT_POOL_PTR(base, n))
#define ALM_SORT_RUN_LEN_PTR(base, n)     (ALM_SORT_RUN_BASE_PTR(base, n) + ALM_SORT_RUN_STACK_CAP)

/*
 * Stable linear insertion sort of a small index slice on the full 64-bit key.
 *
 * STABILITY CONTRACT: this is a stability-PRESERVING sort, not a
 * stability-ESTABLISHING one. It shifts only on a strict key inversion and stops
 * on equality, so it keeps the incoming relative order of equal-key elements. The
 * caller MUST therefore pass a slice in which equal-key elements are already in
 * ascending original-index order. Every current caller satisfies this: the index
 * array starts as the ascending identity and every MSD/LSD pass that can precede
 * a leaf is a stable forward-scan counting scatter.
 */
static void alm_sort_insertion(const uint8_t *keys_base, int32_t stride,
                               const int32_t *slice_curr,
                               int32_t *dst_idx,
                               size_t start, size_t end)
{
    const size_t n_slice = end - start;
    if (n_slice == 0)
        return;

    int32_t *dst = dst_idx + start;
    if (slice_curr + start != dst)
        memcpy(dst, slice_curr + start, n_slice * sizeof(int32_t));

    for (size_t i = 1; i < n_slice; ++i) {
        const int32_t cur_idx = dst[i];
        const uint64_t cur_key = alm_sort_strided_load(keys_base, stride, (size_t)cur_idx);
        size_t j = i;
        while (j > 0) {
            const int32_t prev_idx = dst[j - 1];
            const uint64_t prev_key = alm_sort_strided_load(keys_base, stride, (size_t)prev_idx);
            /* Strict inversion only; stop on equality (see STABILITY CONTRACT). */
            if (prev_key > cur_key) {
                dst[j] = prev_idx;
                --j;
            } else {
                break;
            }
        }
        dst[j] = cur_idx;
    }
}

/*
 * Dense LSD leaf for a single MSD bucket. Caller contract (guaranteed by both
 * call sites in alm_sort_msd):
 *   * ALM_SORT_INSERTION_THRESHOLD < n_slice <= ALM_SORT_LEAF_CAP
 *   * bits_hi > 0
 * The gather is prefetched (the random strided load dominates; the index stream
 * slice_curr[] is sequential). Result lands stably in dst_idx[start, end).
 */
static void alm_sort_leaf_lsd8(const uint8_t *keys_base, int32_t stride,
                               const int32_t *slice_curr,
                               int32_t *dst_idx,
                               uint8_t *leaf_pool,
                               size_t start, size_t end,
                               int bits_hi)
{
    const size_t n_slice = end - start;
    ALM_SORT_ASSERT(n_slice > ALM_SORT_INSERTION_THRESHOLD && n_slice <= ALM_SORT_LEAF_CAP && bits_hi > 0,
           "alm_sort_leaf_lsd8 contract: INSERTION_THRESHOLD < n_slice <= LEAF_CAP, bits_hi > 0");

    uint64_t *tkey_a  = (uint64_t *)(leaf_pool + ALM_SORT_LEAF_TKEY_A_OFF);
    uint64_t *tkey_b  = (uint64_t *)(leaf_pool + ALM_SORT_LEAF_TKEY_B_OFF);
    int32_t  *idx_a   = (int32_t *) (leaf_pool + ALM_SORT_LEAF_IDX_A_OFF);
    int32_t  *idx_b   = (int32_t *) (leaf_pool + ALM_SORT_LEAF_IDX_B_OFF);
    uint32_t *hist    = (uint32_t *)(leaf_pool + ALM_SORT_LEAF_HIST_OFF);
    uint32_t *offsets = (uint32_t *)(leaf_pool + ALM_SORT_LEAF_OFFSETS_OFF);

    /* Gather keys once from the strided source, prefetching LEAF_PF_DIST ahead.
     * Split loop hoists the bound check out of the body. */
    const size_t lim = n_slice > ALM_SORT_LEAF_PF_DIST ? n_slice - ALM_SORT_LEAF_PF_DIST : 0;
    for (size_t j = 0; j < lim; ++j) {
        ALM_SORT_PREFETCH(keys_base +
            (intptr_t)slice_curr[start + j + ALM_SORT_LEAF_PF_DIST] * (intptr_t)stride, 0, 0);
        int32_t i = slice_curr[start + j];
        tkey_a[j] = alm_sort_strided_load(keys_base, stride, (size_t)i);
        idx_a[j]  = i;
    }
    for (size_t j = lim; j < n_slice; ++j) {
        int32_t i = slice_curr[start + j];
        tkey_a[j] = alm_sort_strided_load(keys_base, stride, (size_t)i);
        idx_a[j]  = i;
    }

    uint64_t *tk_src = tkey_a; uint64_t *tk_dst = tkey_b;
    int32_t  *id_src = idx_a;  int32_t  *id_dst = idx_b;

    const int n_passes = (bits_hi + ALM_SORT_SMALL_RADIX_BITS - 1) / ALM_SORT_SMALL_RADIX_BITS;

    for (int p = 0; p < n_passes; ++p) {
        const int shift = p * ALM_SORT_SMALL_RADIX_BITS;
        const int rem   = bits_hi - shift;
        const int bb    = rem < ALM_SORT_SMALL_RADIX_BITS ? rem : ALM_SORT_SMALL_RADIX_BITS;
        const uint64_t mask = ((uint64_t)1 << bb) - 1;
        const int nb = 1 << bb;

        memset(hist, 0, (size_t)nb * sizeof(uint32_t));
        for (size_t j = 0; j < n_slice; ++j) {
            uint32_t d = (uint32_t)((tk_src[j] >> shift) & mask);
            ++hist[d];
        }

        const uint32_t d0 = (uint32_t)((tk_src[0] >> shift) & mask);
        if (hist[d0] == (uint32_t)n_slice)
            continue;

        uint32_t s = 0;
        for (int d = 0; d < nb; ++d) { offsets[d] = s; s += hist[d]; }

        for (size_t j = 0; j < n_slice; ++j) {
            uint32_t d   = (uint32_t)((tk_src[j] >> shift) & mask);
            uint32_t off = offsets[d]++;
            tk_dst[off] = tk_src[j];
            id_dst[off] = id_src[j];
        }

        { uint64_t *t = tk_src; tk_src = tk_dst; tk_dst = t; }
        { int32_t  *t = id_src; id_src = id_dst; id_dst = t; }
    }

    memcpy(dst_idx + start, id_src, n_slice * sizeof(int32_t));
}

/*
 * n_slice <= 32K (L1d resident): radix-11, 1 MSD pass -> ~16 elem/bucket -> insertion
 * leaves. n_slice > 32K: pick a narrower digit so one pass lands buckets under
 * LEAF_CAP in few passes.
 */
static inline int alm_sort_choose_bits(size_t n_slice, int bits_hi, int frame_idx)
{
    int avail = bits_hi < ALM_SORT_RADIX_BITS ? bits_hi : ALM_SORT_RADIX_BITS;
    if (n_slice <= ALM_SORT_SUBL2_GATE || frame_idx >= ALM_SORT_COARSE_DEPTH)
        return avail;
    int need = 0;
    while ((n_slice >> need) > ALM_SORT_TARGET_LEAF) ++need;
    if (need < 1)     need = 1;
    if (need > avail) need = avail;
    return need;
}

/*
 * Uncached strided LSD fallback. Sorts [start,end) by the low bits_hi bits once
 * ALM_SORT_MAX_MSD_FRAMES MSD frames are exhausted. Borrows the leaf pool's
 * 256-entry hist/offsets regions + the two global index buffers. No stack
 * allocation. Stable. Caller contract: n_slice > ALM_SORT_LEAF_CAP and bits_hi > 0.
 */
static void alm_sort_lsd_fallback(const uint8_t *keys_base, int32_t stride,
                                  int32_t *slice_curr, int32_t *slice_alt,
                                  int32_t *dst_idx, uint8_t *leaf_pool,
                                  size_t start, size_t end, int bits_hi)
{
    const size_t n_slice = end - start;
    ALM_SORT_ASSERT(n_slice > ALM_SORT_LEAF_CAP && bits_hi > 0,
           "alm_sort_lsd_fallback contract: n_slice > LEAF_CAP, bits_hi > 0");
    uint32_t *hist    = (uint32_t *)(leaf_pool + ALM_SORT_LEAF_HIST_OFF);
    uint32_t *offsets = (uint32_t *)(leaf_pool + ALM_SORT_LEAF_OFFSETS_OFF);
    int32_t *src = slice_curr;
    int32_t *dst = slice_alt;
    const int n_passes = (bits_hi + ALM_SORT_SMALL_RADIX_BITS - 1) / ALM_SORT_SMALL_RADIX_BITS;
    for (int p = 0; p < n_passes; ++p) {
        const int shift = p * ALM_SORT_SMALL_RADIX_BITS;
        const int rem   = bits_hi - shift;
        const int bb    = rem < ALM_SORT_SMALL_RADIX_BITS ? rem : ALM_SORT_SMALL_RADIX_BITS;
        const uint64_t mask = ((uint64_t)1 << bb) - 1;
        const int nb = 1 << bb;
        memset(hist, 0, (size_t)nb * sizeof(uint32_t));
        for (size_t j = start; j < end; ++j) {
            uint64_t k = alm_sort_strided_load(keys_base, stride, (size_t)src[j]);
            ++hist[(uint32_t)((k >> shift) & mask)];
        }
        uint64_t k0 = alm_sort_strided_load(keys_base, stride, (size_t)src[start]);
        const uint32_t d0 = (uint32_t)((k0 >> shift) & mask);
        if (hist[d0] == (uint32_t)n_slice)
            continue;
        uint32_t s = (uint32_t)start;
        for (int d = 0; d < nb; ++d) { offsets[d] = s; s += hist[d]; }
        for (size_t j = start; j < end; ++j) {
            int32_t  i = src[j];
            uint64_t k = alm_sort_strided_load(keys_base, stride, (size_t)i);
            uint32_t d = (uint32_t)((k >> shift) & mask);
            dst[offsets[d]++] = i;
        }
        { int32_t *t = src; src = dst; dst = t; }
    }
    if (src != dst_idx)
        memcpy(dst_idx + start, src + start, n_slice * sizeof(int32_t));
}

static void alm_sort_msd(const uint8_t *keys_base, int32_t stride,
                         int32_t *slice_curr, int32_t *slice_alt,
                         int32_t *dst_idx,
                         uint8_t *leaf_pool, uint8_t *msd_pool, int frame_idx,
                         size_t start, size_t end, int bits_hi)
{
    while (true) {
        const size_t n_slice = end - start;

        if (bits_hi <= 0 || n_slice <= 1) {
            if (n_slice && slice_curr != dst_idx)
                memcpy(dst_idx + start, slice_curr + start, n_slice * sizeof(int32_t));
            return;
        }

        if (n_slice <= ALM_SORT_INSERTION_THRESHOLD) {
            alm_sort_insertion(keys_base, stride, slice_curr, dst_idx, start, end);
            return;
        }

        if (n_slice <= ALM_SORT_LEAF_CAP) {
            alm_sort_leaf_lsd8(keys_base, stride, slice_curr, dst_idx, leaf_pool, start, end, bits_hi);
            return;
        }

        /* Frame budget exhausted: finish this oversized bucket with the pool-free
         * strided LSD fallback instead of recursing. */
        if (frame_idx >= ALM_SORT_MAX_MSD_FRAMES) {
            alm_sort_lsd_fallback(keys_base, stride, slice_curr, slice_alt,
                                  dst_idx, leaf_pool, start, end, bits_hi);
            return;
        }
        ALM_SORT_ASSERT(frame_idx >= 0 && frame_idx < ALM_SORT_MAX_MSD_FRAMES,
               "frame_idx must index a valid MSD frame after the fallback gate");

        const int      b     = alm_sort_choose_bits(n_slice, bits_hi, frame_idx);
        const int      shift = bits_hi - b;
        const uint64_t mask  = ((uint64_t)1 << b) - 1;
        const int      nb    = 1 << b;

        uint8_t *frame = msd_pool + (size_t)frame_idx * ALM_SORT_MSD_FRAME_BYTES;
        /* Single reused array: counts -> cursors -> bucket ends (W2'). */
        uint32_t *hist_cur = (uint32_t *)(frame + ALM_SORT_MSD_HIST_OFF);

        memset(hist_cur, 0, (size_t)nb * sizeof(uint32_t));
        for (size_t j = start; j < end; ++j) {
            int32_t  i = slice_curr[j];
            uint64_t k = alm_sort_strided_load(keys_base, stride, (size_t)i);
            uint32_t d = (uint32_t)((k >> shift) & mask);
            ++hist_cur[d];
        }

        /* skip: all in same bucket. Hence no scatter, no swap. */
        const uint64_t k0 = alm_sort_strided_load(keys_base, stride, (size_t)slice_curr[start]);
        const uint32_t d0 = (uint32_t)((k0 >> shift) & mask);
        if (hist_cur[d0] == (uint32_t)n_slice) {
            bits_hi -= b;
            continue;
        }

        /* In-place prefix sum: counts -> per-bucket scatter cursors. */
        {
            uint32_t s = (uint32_t)start;
            for (int d = 0; d < nb; ++d) {
                uint32_t c = hist_cur[d];
                hist_cur[d] = s;
                s += c;
            }
        }

        for (size_t j = start; j < end; ++j) {
            int32_t  i = slice_curr[j];
            uint64_t k = alm_sort_strided_load(keys_base, stride, (size_t)i);
            uint32_t d = (uint32_t)((k >> shift) & mask);
            slice_alt[hist_cur[d]++] = i;
        }
        /* After the scatter hist_cur[d] == end of bucket d. Recover bounds from a
         * running cursor over those ends (W2', no bucket_starts array). */

        const int next_bits  = bits_hi - b;
        const int next_frame = frame_idx + 1;
        uint32_t prev = (uint32_t)start;
        for (int d = 0; d < nb; ++d) {
            const uint32_t bend = hist_cur[d];
            const uint32_t cnt  = bend - prev;
            if (cnt) {
                /* Inline the child's base-case dispatch (its slice_curr ==
                 * slice_alt here) to skip a recursive call for buckets that would
                 * immediately bottom out. */
                if (cnt == 1) {
                    dst_idx[prev] = slice_alt[prev];
                } else if (next_bits <= 0) {
                    memcpy(dst_idx + prev, slice_alt + prev, cnt * sizeof(int32_t));
                } else if (cnt <= ALM_SORT_INSERTION_THRESHOLD) {
                    alm_sort_insertion(keys_base, stride, slice_alt, dst_idx, prev, bend);
                } else if (cnt <= ALM_SORT_LEAF_CAP) {
                    alm_sort_leaf_lsd8(keys_base, stride, slice_alt, dst_idx, leaf_pool,
                                       prev, bend, next_bits);
                } else {
                    alm_sort_msd(keys_base, stride,
                                 slice_alt, slice_curr,
                                 dst_idx,
                                 leaf_pool, msd_pool, next_frame,
                                 prev, bend, next_bits);
                }
            }
            prev = bend;
        }
        return;
    }
}

/* ============================================================
 * (B) Flat LSD radix
 * ============================================================ */
/*
 * Flat 11-bit LSD over the whole array. `base` holds the
 * 4N index scratch (id_a); `dst_idx` is the ping-pong partner. Histograms reuse
 * the leaf+msd pool region. Keys are re-read from the strided source each pass.
 * Stable; result lands in dst_idx.
 */
static void alm_sort_lsd(const uint8_t *keys_base, int32_t stride,
                         int32_t *dst_idx, size_t n, uint8_t *base)
{
    int32_t  *id_a    = ALM_SORT_INDEX_PTR(base, n);
    uint32_t *hist    = ALM_SORT_LSD_HIST_PTR(base, n);
    uint32_t *offsets = ALM_SORT_LSD_OFFSETS_PTR(base, n);

    static const int      pass_sh[ALM_SORT_LSD_PASSES] = {0, 11, 22, 33, 44, 55};
    static const uint64_t pass_mk[ALM_SORT_LSD_PASSES] = {0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x1ff};
    static const int      pass_nb[ALM_SORT_LSD_PASSES] = {ALM_SORT_LSD_NB, ALM_SORT_LSD_NB, ALM_SORT_LSD_NB, ALM_SORT_LSD_NB, ALM_SORT_LSD_NB, 1 << 9};

    /* Fused count: one sequential sweep reads each key once and bumps all six
     * digit histograms with immediate 11-bit shifts. */
    memset(hist, 0, ALM_SORT_LSD_HIST_BYTES);
    for (size_t i = 0; i < n; ++i) {
        uint64_t k = alm_sort_strided_load(keys_base, stride, i);
        ++hist[0 * ALM_SORT_LSD_NB + (uint32_t)( k        & 0x7ff)];
        ++hist[1 * ALM_SORT_LSD_NB + (uint32_t)((k >> 11) & 0x7ff)];
        ++hist[2 * ALM_SORT_LSD_NB + (uint32_t)((k >> 22) & 0x7ff)];
        ++hist[3 * ALM_SORT_LSD_NB + (uint32_t)((k >> 33) & 0x7ff)];
        ++hist[4 * ALM_SORT_LSD_NB + (uint32_t)((k >> 44) & 0x7ff)];
        ++hist[5 * ALM_SORT_LSD_NB + (uint32_t)((k >> 55) & 0x1ff)];
    }

    /* Pass 0: scatter identity positions by digit 0 (keys read sequentially). */
    {
        uint32_t s = 0;
        for (int d = 0; d < ALM_SORT_LSD_NB; ++d) { offsets[d] = s; s += hist[d]; }
        for (size_t i = 0; i < n; ++i) {
            uint64_t k = alm_sort_strided_load(keys_base, stride, i);
            uint32_t d = (uint32_t)(k & 0x7ff);
            id_a[offsets[d]++] = (int32_t)i;
        }
    }
    int32_t *id_src = id_a;
    int32_t *id_dst = dst_idx;

    /* Passes 1..5: read indices in order, re-read each key (random access into the
     * strided source), scatter. Per-pass constant-digit skip (order-preserving). */
    for (int p = 1; p < ALM_SORT_LSD_PASSES; ++p) {
        const int      shift = pass_sh[p];
        const uint64_t mask  = pass_mk[p];
        const int      nb    = pass_nb[p];
        uint32_t *hp = hist + (size_t)p * ALM_SORT_LSD_NB;

        const uint64_t k0 = alm_sort_strided_load(keys_base, stride, (size_t)id_src[0]);
        const uint32_t d0 = (uint32_t)((k0 >> shift) & mask);
        if (hp[d0] == (uint32_t)n)
            continue;   /* all in same bucket: no scatter, no swap */

        uint32_t s = 0;
        for (int d = 0; d < nb; ++d) { offsets[d] = s; s += hp[d]; }
        /* The key gather is random; id_src is streamed in order, so prefetch the
         * key LSD_PF_DIST iterations ahead to overlap the gather latency. Split so
         * the bounds check is hoisted out of the body. */
        const size_t lim = n > ALM_SORT_LSD_PF_DIST ? n - ALM_SORT_LSD_PF_DIST : 0;
        for (size_t j = 0; j < lim; ++j) {
            ALM_SORT_PREFETCH(keys_base + (intptr_t)id_src[j + ALM_SORT_LSD_PF_DIST]
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
        { int32_t *t = id_src; id_src = id_dst; id_dst = t; }
    }

    if (id_src != dst_idx)
        memcpy(dst_idx, id_src, n * sizeof(int32_t));
}

static void alm_sort_radix(const uint8_t *keys_base, int32_t stride,
                           int32_t *dst_idx, size_t n, uint8_t *base)
{
    int32_t *scratch   = ALM_SORT_INDEX_PTR(base, n);
    uint8_t *leaf_pool = ALM_SORT_LEAF_POOL_PTR(base, n);
    uint8_t *msd_pool  = ALM_SORT_MSD_POOL_PTR(base, n);

    for (size_t i = 0; i < n; ++i)
        scratch[i] = (int32_t)i;

    alm_sort_msd(keys_base, stride,
                 scratch, dst_idx, dst_idx,
                 leaf_pool, msd_pool, 0,
                 0, n, 64);
}

/* ============================================================
 * Public API
 * ============================================================ */

/*
 * Report the scratch size (bytes) needed by amd_stablesort_ascend_64f for the
 * given len. Layout: 64 bytes of alignment slack, the 4N (aligned) index
 * scratch, then the leaf pool and the MSD frame pool.
 *
 * Returns an aoclsort_status: AOCLSORT_STS_OK (0) on success;
 *   AOCLSORT_STS_NULL_PTR      if workspace_size is NULL,
 *   AOCLSORT_STS_LENGTH_ERR    if len <= 0,
 *   AOCLSORT_STS_SIZE_OVERFLOW if the required size would exceed INT_MAX.
 */
int ALM_PROTO_OPT(stablesort_getsize_64f)(int len, int *workspace_size)
{
    if (workspace_size == NULL)
        return AOCLSORT_STS_NULL_PTR;

    if (len <= 0) {
        *workspace_size = 0;
        return AOCLSORT_STS_LENGTH_ERR;
    }

    size_t bytes = ALM_SORT_WORKSPACE_BYTES((size_t)len);

    if (bytes > (size_t)INT_MAX) {
        *workspace_size = 0;
        return AOCLSORT_STS_SIZE_OVERFLOW;
    }

    *workspace_size = (int)bytes;
    return AOCLSORT_STS_OK;
}

/*
 * Stable ascending argsort of the strided double-precision keys in src.
 * dst_index receives a permutation of [0, len).
 *
 * Returns an aoclsort_status: AOCLSORT_STS_OK (0) on success;
 *   AOCLSORT_STS_NULL_PTR   if src, dst_index, or workspace is NULL,
 *   AOCLSORT_STS_LENGTH_ERR if len <= 0,
 *   AOCLSORT_STS_STRIDE_ERR if src_stride_bytes < (int)sizeof(double)
 *                           (rejects 0 and negative strides).
 */
int ALM_PROTO_OPT(stablesort_ascend_64f)(const double *src, int src_stride_bytes,
                                         int *dst_index, int len, void *workspace)
{
    if (src == NULL || dst_index == NULL)
        return AOCLSORT_STS_NULL_PTR;

    if (len <= 0)
        return AOCLSORT_STS_LENGTH_ERR;

    if (src_stride_bytes < (int)sizeof(double))
        return AOCLSORT_STS_STRIDE_ERR;

    if (workspace == NULL)
        return AOCLSORT_STS_NULL_PTR;

    size_t         n         = (size_t)len;
    int32_t        stride    = (int32_t)src_stride_bytes;
    const uint8_t *keys_base = (const uint8_t *)src;
    int32_t       *dst_idx   = (int32_t *)dst_index;

    /* The caller's workspace may be unaligned; align the working base to 64. */
    uint8_t *base = (uint8_t *)workspace;
    base = (uint8_t *)ALM_SORT_ALIGN64((uintptr_t)base);

    /* The Shivers run-stack (2 * ALM_SORT_RUN_STACK_CAP ints) lives in the
     * workspace, carved from the region the radix path uses for the leaf pool --
     * unused by the Shivers path, so no extra N and no stack scratch. */
    int32_t *merge_tmp = ALM_SORT_MERGE_TMP_PTR(base, n);
    int *run_base = ALM_SORT_RUN_BASE_PTR(base, n);
    int *run_len  = ALM_SORT_RUN_LEN_PTR(base, n);

    /* Planner: delegates to different kernels based on the size and distribution
     * of the input. A second planner is present within the radix kernel. */
    if (n <= 2048) {
        alm_sort_shivers(keys_base, stride, dst_idx, n,
                         merge_tmp, run_base, run_len, 16);
        return AOCLSORT_STS_OK;
    }

    if (alm_sort_is_structured(keys_base, stride, n)) {
        alm_sort_shivers(keys_base, stride, dst_idx, n,
                         merge_tmp, run_base, run_len, 16);
        return AOCLSORT_STS_OK;
    }

    /* Mid sizes (non-structured): flat 11-bit LSD, gated by n (overhead) and by
     * key-gather footprint stride*n vs L3 (cache residency). */
    if (n >= ALM_SORT_LSD_MIN_N && (size_t)stride * n <= ALM_SORT_LSD_MAX_FOOTPRINT) {
        alm_sort_lsd(keys_base, stride, dst_idx, n, base);
        return AOCLSORT_STS_OK;
    }

    alm_sort_radix(keys_base, stride, dst_idx, n, base);
    return AOCLSORT_STS_OK;
}
