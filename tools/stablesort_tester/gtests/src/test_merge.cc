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

/* Merge sort helper functions test. */

#include "stablesort_testutil.hpp"

using namespace aoclsort_test;

namespace {

struct GalRun {
    std::vector<double> vals;
    std::vector<int32_t> arr;   // identity indices into vals
    std::vector<uint8_t> buf;   // strided key bytes
};

GalRun make_run(const std::vector<double> &vals)
{
    GalRun r;
    r.vals = vals;
    r.arr.resize(vals.size());
    for (size_t i = 0; i < vals.size(); ++i) r.arr[i] = (int32_t)i;
    r.buf = make_strided(vals, 8);
    return r;
}

// count of elements < probe
int gleft(const GalRun &r, double probe, int hint)
{
    return alm_sort_test_gallop_left(total_key(probe), r.arr.data(), r.buf.data(),
                                     8, 0, (int)r.vals.size(), hint);
}

// count of elements <= probe
int gright(const GalRun &r, double probe, int hint)
{
    return alm_sort_test_gallop_right(total_key(probe), r.arr.data(), r.buf.data(),
                                      8, 0, (int)r.vals.size(), hint);
}

} // namespace

// gallop left/right tests.
AOCLSORT_TEST(Gallop, OutOfRangeAndSingleton)
{
    GalRun r = make_run({1.0, 2.0, 3.0, 4.0, 5.0});
    const int len = (int)r.vals.size();
    AOCLSORT_EXPECT_EQ(gleft(r, -100.0, 0), 0);
    AOCLSORT_EXPECT_EQ(gright(r, -100.0, 0), 0);
    AOCLSORT_EXPECT_EQ(gleft(r, 100.0, 0), len);
    AOCLSORT_EXPECT_EQ(gright(r, 100.0, 0), len);

    GalRun s = make_run({7.0});
    AOCLSORT_EXPECT_EQ(gleft(s, 6.0, 0), 0);
    AOCLSORT_EXPECT_EQ(gright(s, 6.0, 0), 0);
    AOCLSORT_EXPECT_EQ(gleft(s, 7.0, 0), 0);   // first index of the equal key
    AOCLSORT_EXPECT_EQ(gright(s, 7.0, 0), 1);  // past the equal key
    AOCLSORT_EXPECT_EQ(gleft(s, 8.0, 0), 1);
    AOCLSORT_EXPECT_EQ(gright(s, 8.0, 0), 1);
}

// Equal-key block. left = first equal (strictly <), right = past last
// equal (<=); their gap is the block width.
AOCLSORT_TEST(Gallop, EqualBlockLeftRightSplit)
{
    // value 5.0 occupies indices [3, 7).
    GalRun r = make_run({1.0, 2.0, 3.0, 5.0, 5.0, 5.0, 5.0, 8.0, 9.0});
    AOCLSORT_EXPECT_EQ(gleft(r, 5.0, 0), 3);
    AOCLSORT_EXPECT_EQ(gright(r, 5.0, 0), 7);
    AOCLSORT_EXPECT_LT(gleft(r, 5.0, 0), gright(r, 5.0, 0));
}

// Bad/extreme hints converge to the same answer as a good hint.
AOCLSORT_TEST(Gallop, HintSelfCorrection)
{
    std::vector<double> vals(64);
    for (size_t i = 0; i < vals.size(); ++i) vals[i] = (double)(i * 2); // distinct
    GalRun r = make_run(vals);
    const int len = (int)vals.size();
    // Between indices 37 and 38 probe -> expect 38
    const double probe = 2.0 * 37 + 1.0;
    for (int hint : {0, len - 1, 1, len / 4, len - 2}) { // same answer regardless of hint
        AOCLSORT_EXPECT_EQ(gleft(r, probe, hint), 38);
        AOCLSORT_EXPECT_EQ(gright(r, probe, hint), 38);
    }
    // Exact-hit probe (left/right assymetry) -> expect 37/38
    const double hit = 2.0 * 37;
    for (int hint : {0, len - 1, 37, 10}) {
        AOCLSORT_EXPECT_EQ(gleft(r, hit, hint), 37);
        AOCLSORT_EXPECT_EQ(gright(r, hit, hint), 38);
    }
}
