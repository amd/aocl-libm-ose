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

/* Common helper functions tests. */

#include "stablesort_testutil.hpp"

using namespace aoclsort_test;

namespace {

double dbl(uint64_t bits)
{
    double d;
    std::memcpy(&d, &bits, sizeof(d));
    return d;
}

constexpr uint64_t kNegQNaN   = 0xFFF8000000000000ULL;
constexpr uint64_t kNegSNaN   = 0xFFF0000000000001ULL;
constexpr uint64_t kNegInf    = 0xFFF0000000000000ULL;
constexpr uint64_t kNegMinNrm = 0x8010000000000000ULL;
constexpr uint64_t kNegMinSub = 0x8000000000000001ULL;
constexpr uint64_t kNegZero   = 0x8000000000000000ULL;
constexpr uint64_t kPosZero   = 0x0000000000000000ULL;
constexpr uint64_t kPosMinSub = 0x0000000000000001ULL;
constexpr uint64_t kPosMinNrm = 0x0010000000000000ULL;
constexpr uint64_t kPosInf    = 0x7FF0000000000000ULL;
constexpr uint64_t kPosSNaN   = 0x7FF0000000000001ULL;
constexpr uint64_t kPosQNaN   = 0x7FF8000000000000ULL;
constexpr uint64_t kNegMaxNaN = 0xFFFFFFFFFFFFFFFFULL; // all-ones: the global min
constexpr uint64_t kPosMaxNaN = 0x7FFFFFFFFFFFFFFFULL; // mirror: the global max

} // namespace

// Monotonicity of special values, extreme values.
AOCLSORT_TEST(KeyMapping, TotalOrderChain)
{
    const double asc[] = {
        dbl(kNegMaxNaN),        // global min
        dbl(kNegQNaN),
        dbl(kNegSNaN),
        dbl(kNegInf),
        -1e300,
        -1.0,
        dbl(kNegMinNrm),
        dbl(kNegMinSub),
        dbl(kNegZero),          // -0 just below +0
        dbl(kPosZero),
        dbl(kPosMinSub),
        dbl(kPosMinNrm),
        1.0,
        1e300,
        dbl(kPosInf),
        dbl(kPosSNaN),
        dbl(kPosQNaN),
        dbl(kPosMaxNaN),        // global max
    };
    for (size_t i = 1; i < sizeof(asc) / sizeof(asc[0]); ++i)
        AOCLSORT_EXPECT_LT(total_key(asc[i - 1]), total_key(asc[i]));
}

// min, center, max
AOCLSORT_TEST(KeyMapping, ExactKeyAnchors)
{
    AOCLSORT_EXPECT_EQ(alm_sort_test_to_sortable(kNegMaxNaN), 0ULL);
    AOCLSORT_EXPECT_EQ(alm_sort_test_to_sortable(kPosZero), 0x8000000000000000ULL);
    AOCLSORT_EXPECT_EQ(alm_sort_test_to_sortable(kPosMaxNaN), 0xFFFFFFFFFFFFFFFFULL);
}
