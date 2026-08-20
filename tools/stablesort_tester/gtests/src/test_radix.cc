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

/* Radix sort helper functions tests. */

#include "stablesort_testutil.hpp"

using namespace aoclsort_test;

namespace {

// Constants queried from src to ensure tests are consistent with the implementation.
alm_sort_test_consts K()
{
    alm_sort_test_consts c;
    alm_sort_test_get_consts(&c);
    return c;
}

// Doubles in [1,2): identical sign+exponent -> constant top bits, so a radix
// digit above the mantissa is a constant-skip.
std::vector<double> constant_exp_keys(size_t n, uint64_t seed)
{
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> u(1.0, 2.0);
    std::vector<double> v(n);
    for (size_t i = 0; i < n; ++i) v[i] = u(rng);
    return v;
}

} // namespace

// alm_sort_test_choose_bits boundaries tests.
AOCLSORT_TEST(ChooseBits, Boundaries)
{
    const alm_sort_test_consts c = K();
    const size_t gate  = (size_t)c.subl2_gate;
    const size_t big   = gate + 1; // one past SUBL2_GATE: triggers the need-computation path

    // n <= subl2_gate -> short-circuit
    AOCLSORT_EXPECT_EQ(alm_sort_test_choose_bits(gate, c.radix_bits, 0), c.radix_bits);
    // n > subl2_gate -> need-computation runs
    AOCLSORT_EXPECT_LT(alm_sort_test_choose_bits(big,  c.radix_bits, 0), c.radix_bits);

    // frame >= coarse_depth -> short-circuit
    AOCLSORT_EXPECT_EQ(alm_sort_test_choose_bits(big, c.radix_bits, c.coarse_depth),
                       c.radix_bits);
    // frame < coarse_depth -> need-computation runs
    AOCLSORT_EXPECT_LT(alm_sort_test_choose_bits(big, c.radix_bits, c.coarse_depth - 1),
                       c.radix_bits);

    // bits_hi < radix_bits -> avail clamped to bits_hi
    AOCLSORT_EXPECT_EQ(alm_sort_test_choose_bits(gate, c.radix_bits - 1, 0),
                       c.radix_bits - 1);
    // bits_hi >= radix_bits -> avail = radix_bits
    AOCLSORT_EXPECT_EQ(alm_sort_test_choose_bits(gate, c.radix_bits + 1, 0),
                       c.radix_bits);

    // need-computation: n >> need <= target_leaf, need in [1, avail]
    int b = alm_sort_test_choose_bits(big, c.radix_bits, 0);
    AOCLSORT_EXPECT_GE(b, 1);
    AOCLSORT_EXPECT_LE(b, c.radix_bits);
    AOCLSORT_EXPECT_LE(big >> b, (size_t)c.target_leaf);
    // need is tight: one fewer bit would exceed target_leaf
    if (b > 1) { AOCLSORT_EXPECT_GT(big >> (b - 1), (size_t)c.target_leaf); }

    // need > avail -> clamp to avail: n_slice needs more than radix_bits digits
    // to reach target_leaf (n_slice > target_leaf << radix_bits).
    const size_t huge = (size_t)c.target_leaf << (c.radix_bits + 1);
    AOCLSORT_EXPECT_EQ(alm_sort_test_choose_bits(huge, 64, 0), c.radix_bits);
}

// alm_sort_test_insertion tests.
AOCLSORT_TEST(Insertion, StabilityAndBounds)
{
    // single element insertion
    { std::vector<int32_t> out(1, -1); auto b = make_strided({3.0}, 8);
      alm_sort_test_insertion(b.data(), 8, out.data(), 1);
      AOCLSORT_EXPECT_EQ(out[0], 0); }

    // multiple elements insertion
    const alm_sort_test_consts c = K();
    for (size_t n : {size_t(2), size_t(16), (size_t)c.insertion_threshold}) {
        for (Dist d : {Dist::FewUnique, Dist::Sorted, Dist::Reverse, Dist::AllEqual}) {
            auto keys = gen(d, n, test_seed(n));
            auto out = run_kernel(alm_sort_test_insertion, keys, 8);
            AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
        }
    }
}

// alm_sort_test_leaf_lsd8 tests.
AOCLSORT_TEST(LeafLsd8, SizePassGrid)
{
    const alm_sort_test_consts c = K();
    for (size_t n : {(size_t)c.insertion_threshold + 1 /* min n that takes leaf_lsd8 */, (size_t)c.leaf_cap /* max n that takes leaf_lsd8 */}) {
        for (int bits : {8, 16, 24}) { // generate keys so that we get 1, 2, 3 passes
            auto keys = low_bits_keys(n, bits, test_seed(n ^ (size_t)bits), /*const_mid=*/false);
            auto buf = make_strided(keys, 8);
            std::vector<int32_t> out(n);
            alm_sort_test_leaf_lsd8(buf.data(), 8, out.data(), n, bits);
            AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
        }
        // 3-pass with a constant middle byte -> pass 1 skips (no swap, parity flip).
        auto keys = low_bits_keys(n, 24, test_seed(n ^ 24), /*const_mid=*/true);
        auto buf = make_strided(keys, 8);
        std::vector<int32_t> out(n);
        alm_sort_test_leaf_lsd8(buf.data(), 8, out.data(), n, 24);
        AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
    }
}

// alm_sort_test_lsd_fallback tests.
AOCLSORT_TEST(LsdFallback, PassParityAndSkip)
{
    const alm_sort_test_consts c = K();
    const size_t n = (size_t)c.leaf_cap + 1; // min n that takes lsd_fallback
    // bits=16 and 24 : 2 and 3 pass sorts respectively
    for (int bits : {16, 24}) {
        auto keys = low_bits_keys(n, bits, test_seed(n ^ (size_t)bits), /*const_mid=*/false);
        auto buf = make_strided(keys, 8);
        std::vector<int32_t> out(n);
        alm_sort_test_lsd_fallback(buf.data(), 8, out.data(), n, bits);
        AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
    }
    // bits=24: 3-pass sort, constant middle byte -> pass 1 skips (no swap, parity flip)
    auto keys = low_bits_keys(n, 24, test_seed(n ^ 24), /*const_mid=*/true);
    auto buf = make_strided(keys, 8);
    std::vector<int32_t> out(n);
    alm_sort_test_lsd_fallback(buf.data(), 8, out.data(), n, 24);
    AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
}

// alm_sort_test_msd tests boundaries.
AOCLSORT_TEST(Msd, BaseCasesAndPartition)
{
    const alm_sort_test_consts c = K();
    for (size_t n : {size_t(1) /* trivially sorted */,
                     size_t(2) /* first non-trivial insertion sort input */,
                     (size_t)c.insertion_threshold /* last n that takes insertion sort */,
                     (size_t)c.insertion_threshold + 1 /* first n that takes leaf_lsd8 */,
                     (size_t)c.leaf_cap /* last n that takes leaf_lsd8 */,
                     (size_t)c.leaf_cap + 1 /* first n that takes lsd_fallback */}) {
        auto keys = gen(Dist::Uniform, n, test_seed(n));
        auto buf = make_strided(keys, 8);
        std::vector<int32_t> out(n);
        alm_sort_test_msd(buf.data(), 8, out.data(), n, 64);
        AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
    }
}

// alm_sort_test_msd tests constant top digit (varying low bits)
// -> single-bucket skip path.
AOCLSORT_TEST(Msd, ConstantTopDigitSkip)
{
    const size_t n = 5000;
    auto keys = constant_exp_keys(n, 11);
    auto buf = make_strided(keys, 8);
    std::vector<int32_t> out(n);
    alm_sort_test_msd(buf.data(), 8, out.data(), n, 64);
    AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
}

// alm_sort_test_msd child bucket with digits exhausted (next_bits <= 0): a
// single full-width pass (b == bits_hi == radix_bits) consumes all bits, and
// colliding keys leave a bucket of size > 1 that is copied verbatim.
AOCLSORT_TEST(Msd, ChildBitsExhaustedMemcpy)
{
    const alm_sort_test_consts c = K();
    const size_t n = (size_t)c.leaf_cap + 1; // > LEAF_CAP -> partitions once
    // Only radix_bits significant key bits: partition by them consumes bits_hi
    // entirely (next_bits == 0). n > 2^radix_bits guarantees a colliding bucket.
    auto keys = low_bits_keys(n, c.radix_bits, test_seed(n), /*const_mid=*/false);
    auto buf = make_strided(keys, 8);
    std::vector<int32_t> out(n);
    alm_sort_test_msd(buf.data(), 8, out.data(), n, c.radix_bits);
    AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));
}

// alm_sort_test_lsd test.
AOCLSORT_TEST(FlatLsd, RandomAndConstantHighDigit)
{
    // Flat LSD is a fixed 6-pass kernel with no n-based dispatch; the two
    // cases test distinct histogram shapes rather than boundary sizes.
    const size_t n = 100000; // comfortably above LSD_MIN_N (65536)

    // Uniform: all 2048 buckets roughly equally loaded in every pass.
    auto keys = gen(Dist::Uniform, n, test_seed(n));
    auto buf = make_strided(keys, 8);
    std::vector<int32_t> out(n);
    alm_sort_test_lsd(buf.data(), 8, out.data(), n);
    AOCLSORT_EXPECT_TRUE(stable_sorted(keys, out.data(), n));

    // Constant exponent: top digit(s) all identical -> those passes skip.
    auto keys2 = constant_exp_keys(n, test_seed(n));
    auto buf2 = make_strided(keys2, 8);
    std::vector<int32_t> out2(n);
    alm_sort_test_lsd(buf2.data(), 8, out2.data(), n);
    AOCLSORT_EXPECT_TRUE(stable_sorted(keys2, out2.data(), n));
}
