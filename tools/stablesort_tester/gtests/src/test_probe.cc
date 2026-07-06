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

/* Planner unit tests. */

#include "stablesort_testutil.hpp"

using namespace aoclsort_test;

// Sample-count boundaries.
AOCLSORT_TEST(SampleCount, Boundaries)
{
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(0), (size_t)0);
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(1), (size_t)0);
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(2), (size_t)1);   // cap at pair count
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(17), (size_t)16); // floor at MIN_SAMPLES
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(4097), (size_t)16);
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(4098), (size_t)17); // scaling begins
    AOCLSORT_EXPECT_EQ(alm_sort_test_struct_sample_count(262144), (size_t)1024); // scaled
}

// alm_sort_test_sample_ascending_pairs tests .
// Use sorted and reverse-sorted inputs to ensure no RNG-value coupling.
// ASAN checks for sampling function not going out of bounds.
AOCLSORT_TEST(SampleAscendingPairs, ContractAndInvariants)
{
    for (size_t n : {size_t(2), size_t(2000), size_t(100000)}) {
        std::vector<double> asc(n), desc(n);
        for (size_t i = 0; i < n; ++i) { asc[i] = (double)i; desc[i] = (double)(n - i); }
        auto ab = make_strided(asc, 8), db = make_strided(desc, 8);

        size_t total = 12345;
        size_t got = alm_sort_test_sample_ascending_pairs(ab.data(), 8, n, &total);
        AOCLSORT_EXPECT_EQ(total, alm_sort_test_struct_sample_count(n));
        AOCLSORT_EXPECT_EQ(got, total);  // every adjacent pair is ascending

        got = alm_sort_test_sample_ascending_pairs(db.data(), 8, n, &total);
        AOCLSORT_EXPECT_EQ(got, (size_t)0); // every adjacent pair is descending
    }
}

// alm_sort_test_is_structured tests.
// Use ordered inputs to ensure no RNG-value coupling.
AOCLSORT_TEST(IsStructured, EdgeAndMonotoneInputs)
{
    { std::vector<double> v{1.0}; auto b = make_strided(v, 8);
      AOCLSORT_EXPECT_FALSE(alm_sort_test_is_structured(b.data(), 8, 1)); }

    { std::vector<double> up{1.0, 2.0}, dn{2.0, 1.0};
      auto bu = make_strided(up, 8), bd = make_strided(dn, 8);
      AOCLSORT_EXPECT_TRUE(alm_sort_test_is_structured(bu.data(), 8, 2));
      AOCLSORT_EXPECT_TRUE(alm_sort_test_is_structured(bd.data(), 8, 2)); }

    const size_t n = 5000; // ensures enough samples
    auto sorted  = make_strided(gen(Dist::Sorted, n, test_seed(n)), 8);
    auto reverse = make_strided(gen(Dist::Reverse, n, test_seed(n)), 8);
    auto equal   = make_strided(gen(Dist::AllEqual, n, test_seed(n)), 8);
    AOCLSORT_EXPECT_TRUE(alm_sort_test_is_structured(sorted.data(), 8, n));
    AOCLSORT_EXPECT_TRUE(alm_sort_test_is_structured(reverse.data(), 8, n));
    AOCLSORT_EXPECT_TRUE(alm_sort_test_is_structured(equal.data(), 8, n));
}

// alm_sort_test_is_structured should be deterministic across calls.
AOCLSORT_TEST(IsStructured, DeterministicAcrossCalls)
{
    auto buf = make_strided(gen(Dist::Uniform, 8192, test_seed(8192)), 8);
    int first = alm_sort_test_is_structured(buf.data(), 8, 8192);
    for (int i = 0; i < 4; ++i)
        AOCLSORT_EXPECT_EQ(alm_sort_test_is_structured(buf.data(), 8, 8192), first);
}
