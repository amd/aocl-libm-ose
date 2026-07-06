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
 * Shared helpers for the stablesort unit tests: total-order key mapping, strided
 * key buffers, the stable-ascending-permutation checker, and input generators.
 */

#pragma once

#include "aoclsort_test.hpp"
#include "stablesort_testapi.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <vector>

namespace aoclsort_test {

inline uint64_t bits_of(double d)
{
    uint64_t u;
    std::memcpy(&u, &d, sizeof(u));
    return u;
}

/* Total-order key the sort implements (matches alm_sort_to_sortable). */
inline uint64_t total_key(double d) { return alm_sort_test_to_sortable(bits_of(d)); }

/* Inverse of the total-order mapping: the double whose total_key == K. */
inline double dbl_from_sortable(uint64_t K)
{
    uint64_t u = (K >> 63) ? (K ^ 0x8000000000000000ULL) : ~K;
    double d;
    std::memcpy(&d, &u, sizeof(d));
    return d;
}

/*
 * Keys whose total_key is HIGH_CONST | (low & (2^bits - 1)), i.e. they differ
 * only in the low `bits` bits. Sorting such keys by the low `bits` bits (what
 * leaf_lsd8 / lsd_fallback do) therefore equals a full sort. With const_mid set
 * (needs bits>16), bits 8..15 are fixed so an interior 8-bit LSD pass is a
 * constant-digit skip (exercises the no-swap parity flip).
 */
inline std::vector<double> low_bits_keys(size_t n, int bits, uint64_t seed,
                                         bool const_mid)
{
    std::mt19937_64 rng(seed);
    const uint64_t mask = (bits >= 64) ? ~0ULL : ((1ULL << bits) - 1);
    std::vector<double> v(n);
    for (size_t i = 0; i < n; ++i) {
        uint64_t low = rng() & mask;
        if (const_mid && bits > 16)
            low = (low & ~(0xFFULL << 8)) | (0x5AULL << 8);
        v[i] = dbl_from_sortable(0x4000000000000000ULL | low);
    }
    return v;
}

/*
 * Adversarial input that forces the MSD radix to exhaust ALM_SORT_MAX_MSD_FRAMES
 * and drop into the strided-LSD fallback. All keys = high|extra, where
 * high = total_key(1.0); the three siblings set bit 42 / 31 / 20 and the core
 * (everything else) varies only in the low 20 bits. The MSD reads 11-bit digits
 * from the MSB; window-by-window the digits are:
 *
 *   window (bits) | core   | A     | B     | C     | action
 *   --------------|--------|-------|-------|-------|-------------------------------
 *   [53..63]      | 0x5FF  | 0x5FF | 0x5FF | 0x5FF | all equal -> SKIP (same frame)
 *   [42..52]      | 0x400  | 0x401 | 0x400 | 0x400 | A forks -> PARTITION -> frame 1
 *   [31..41]      | 0x000  |   -   | 0x001 | 0x000 | B forks -> PARTITION -> frame 2
 *   [20..30]      | 0x000  |   -   |   -   | 0x001 | C forks -> PARTITION -> frame 3
 *   frame 3 entry: frame_idx==MAX_MSD_FRAMES, core still > LEAF_CAP -> hit FALLBACK
 *
 * At each level, the partition yields exactly 2 children : big core and tiny sibling 
 * This takes stack 1 step deeper each time. The core varying only in the low 20 bits also
 * keeps it non-structured (radix, not the shivers path taken). \
 * Needs n > LEAF_CAP + 3.
 */
inline std::vector<double> msd_deep_recursion_keys(size_t n, uint64_t seed)
{
    const uint64_t high = total_key(1.0);   /* fixed positive-double high bits */
    std::mt19937_64 rng(seed);
    std::vector<double> v;
    v.reserve(n);
    /* One sibling per level, forking the cascade at bits 42 / 31 / 20. */
    v.push_back(dbl_from_sortable(high | (1ULL << 42))); // A
    v.push_back(dbl_from_sortable(high | (1ULL << 31))); // B
    v.push_back(dbl_from_sortable(high | (1ULL << 20))); // C
    /* Core: differs only in the low 20 bits (below every forking window). */
    while (v.size() < n)
        v.push_back(dbl_from_sortable(high | (rng() & 0xFFFFFULL)));
    std::shuffle(v.begin(), v.end(), rng);
    return v;
}

/* Byte buffer where element i's key (a double) sits at offset i*stride_bytes. */
inline std::vector<uint8_t> make_strided(const std::vector<double> &keys,
                                         int stride_bytes)
{
    const size_t n = keys.size();
    const size_t bytes = (n == 0) ? sizeof(double)
                                  : (n - 1) * (size_t)stride_bytes + sizeof(double);
    std::vector<uint8_t> buf(bytes, 0);
    for (size_t i = 0; i < n; ++i)
        std::memcpy(buf.data() + i * (size_t)stride_bytes, &keys[i], sizeof(double));
    return buf;
}

/* True iff idx is a permutation of [0,n) that orders keys by total_key and, for
 * equal keys, keeps ascending original index (stability). */
inline ::testing::AssertionResult stable_sorted(const std::vector<double> &keys,
                                                const int32_t *idx, size_t n)
{
    std::vector<char> seen(n, 0);
    for (size_t k = 0; k < n; ++k) {
        int32_t v = idx[k];
        if (v < 0 || (size_t)v >= n)
            return ::testing::AssertionFailure()
                   << "idx[" << k << "]=" << v << " out of range [0," << n << ")";
        if (seen[(size_t)v])
            return ::testing::AssertionFailure() << "idx value " << v << " repeated";
        seen[(size_t)v] = 1;
    }
    for (size_t k = 1; k < n; ++k) {
        uint64_t a = total_key(keys[(size_t)idx[k - 1]]);
        uint64_t b = total_key(keys[(size_t)idx[k]]);
        if (a > b)
            return ::testing::AssertionFailure()
                   << "order broken at k=" << k << ": key(idx[" << (k - 1)
                   << "]=" << idx[k - 1] << ") > key(idx[" << k << "]=" << idx[k]
                   << ")";
        if (a == b && idx[k - 1] > idx[k])
            return ::testing::AssertionFailure()
                   << "stability broken at k=" << k << ": equal keys but idx "
                   << idx[k - 1] << " > " << idx[k];
    }
    return ::testing::AssertionSuccess();
}

/* Convenience: sort a strided buffer through a kernel wrapper (taking
 * keys_base, stride, out, n[, bits_hi]) and assert the result. */
inline std::vector<int32_t> run_kernel(void (*fn)(const void *, int32_t, int32_t *,
                                                  size_t),
                                       const std::vector<double> &keys, int stride)
{
    auto buf = make_strided(keys, stride);
    std::vector<int32_t> out(keys.size());
    fn(buf.data(), stride, out.data(), keys.size());
    return out;
}

/* Input distributions exercised by the API matrix. */
enum class Dist {
    Sorted,
    Reverse,
    Uniform,
    AllEqual,
    Staircase,
    NearlySorted,
    FewUnique,
    Special,
};

/* Canonical per-test seed: golden-ratio constant XOR'd with n so that each
 * input size gets a distinct, well-distributed seed with no shared low bits. */
inline uint64_t test_seed(size_t n)
{
    return 0x9E3779B97F4A7C15ULL ^ (uint64_t)n;
}

/* Distribution generators used by the API matrix. Deterministic for a seed. */
inline std::vector<double> gen(Dist dist, size_t n, uint64_t seed)
{
    std::vector<double> v(n);
    std::mt19937_64 rng(seed);
    switch (dist) {
    case Dist::Sorted:
        for (size_t i = 0; i < n; ++i) v[i] = (double)i;
        break;
    case Dist::Reverse:
        for (size_t i = 0; i < n; ++i) v[i] = (double)(n - i);
        break;
    case Dist::Uniform: {
        std::uniform_real_distribution<double> u(-1e6, 1e6);
        for (size_t i = 0; i < n; ++i) v[i] = u(rng);
        break;
    }
    case Dist::AllEqual:
        for (size_t i = 0; i < n; ++i) v[i] = 42.0;
        break;
    case Dist::Staircase: {
        const size_t step = n / 32 > 2 ? n / 32 : 2;
        for (size_t i = 0; i < n; ++i) v[i] = (double)(i / step);
        break;
    }
    case Dist::NearlySorted: {
        for (size_t i = 0; i < n; ++i) v[i] = (double)i;
        std::uniform_int_distribution<size_t> pick(0, n ? n - 1 : 0);
        for (size_t s = 0; s < n / 100; ++s) std::swap(v[pick(rng)], v[pick(rng)]); /* shuffle 1% of the array */
        break;
    }
    case Dist::FewUnique: {
        std::uniform_int_distribution<int> u(0, 7);
        for (size_t i = 0; i < n; ++i) v[i] = (double)u(rng);
        break;
    }
    case Dist::Special: {
        const double inf = std::numeric_limits<double>::infinity();
        const double qnan = std::numeric_limits<double>::quiet_NaN();
        const double specials[] = {inf, -inf, qnan, -qnan, 0.0, -0.0, 1.0, -1.0};
        std::uniform_real_distribution<double> u(-1e3, 1e3);
        for (size_t i = 0; i < n; ++i)
            v[i] = (i % 3 == 0) ? specials[i % 8] : u(rng); /* 1/3 of the array is special values */
        break;
    }
    }
    return v;
}

} // namespace aoclsort_test
