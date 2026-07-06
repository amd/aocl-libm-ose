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

/* API tests for amd_opt_stablesort_getsize_64f and amd_opt_stablesort_ascend_64f. */

#include "stablesort_testutil.hpp"

#include <climits>

// Public entry points (ALM_PROTO_OPT expansion for this TU).
extern "C" {
int amd_opt_stablesort_getsize_64f(int len, int *workspace_size);
int amd_opt_stablesort_ascend_64f(const double *src, int src_stride_bytes,
                                  int *dst_index, int len, void *workspace);
}

using namespace aoclsort_test;

namespace {

enum { STS_OK = 0, STS_NULL = -1, STS_LEN = -2, STS_STRIDE = -3, STS_OVERFLOW = -4 };

// Sort a strided buffer through the public API using an EXACTLY getsize-sized
// workspace; assert success + stability + that src bytes are untouched.
void expect_api_sorts(const std::vector<double> &keys, int stride)
{
    const size_t n = keys.size();
    auto buf = make_strided(keys, stride);
    const std::vector<uint8_t> before = buf;

    int wss = 0;
    ASSERT_EQ(amd_opt_stablesort_getsize_64f((int)n, &wss), STS_OK);
    ASSERT_GT(wss, 0);

    std::vector<uint8_t> ws((size_t)wss); /* EXACTLY getsize-sized workspace */
    std::vector<int32_t> out(n);
    ASSERT_EQ(amd_opt_stablesort_ascend_64f((const double *)buf.data(), stride,
                                            out.data(), (int)n, ws.data()),
              STS_OK);
    EXPECT_TRUE(stable_sorted(keys, out.data(), n));
    EXPECT_EQ(before, buf) << "argsort must not modify src";
}

} // namespace

// amd_opt_stablesort_ascend_64f status matrix.
AOCLSORT_TEST(ApiAscend, InvalidInputs)
{
    double src[4] = {1, 2, 3, 4};
    int idx[4];
    int wss = 0;
    amd_opt_stablesort_getsize_64f(4, &wss);
    std::vector<uint8_t> ws((size_t)wss);
    const int st = (int)sizeof(double);

    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(nullptr, st, idx, 4, ws.data()), STS_NULL); // src null
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, st, nullptr, 4, ws.data()), STS_NULL); // dst null
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, st, idx, 0, ws.data()), STS_LEN); // len 0
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, st, idx, -5, ws.data()), STS_LEN); // len -ve
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, 0, idx, 4, ws.data()), STS_STRIDE); // stride 0
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, 7, idx, 4, ws.data()), STS_STRIDE); // stride < 8
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, -8, idx, 4, ws.data()), STS_STRIDE); // stride -ve
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, st, idx, 4, nullptr), STS_NULL); // workspace null
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_ascend_64f(src, st, idx, 4, ws.data()), STS_OK); // valid
}

// amd_opt_stablesort_getsize_64f status matrix.
AOCLSORT_TEST(ApiGetsize, InvalidAndValid)
{
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_getsize_64f(4, nullptr), STS_NULL); // workspace null
    int wss = 0;
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_getsize_64f(0, &wss), STS_LEN); // len 0
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_getsize_64f(-1, &wss), STS_LEN); // len -ve
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_getsize_64f(INT_MAX, &wss), STS_OVERFLOW); // len = INT_MAX
    AOCLSORT_EXPECT_EQ(amd_opt_stablesort_getsize_64f(1000, &wss), STS_OK); // valid
    AOCLSORT_EXPECT_LT(0, wss);
}

// Key embedded at a non-zero field offset in a record (strided)
AOCLSORT_TEST(ApiAscend, EmbeddedKeyNonDestructive)
{
    struct Rec { int32_t tag; double key; };
    const size_t n = 4096;
    std::vector<Rec> recs(n);
    std::mt19937_64 rng(123);
    std::uniform_real_distribution<double> u(-1e3, 1e3);
    std::vector<double> keys(n);
    for (size_t i = 0; i < n; ++i) {
        recs[i].tag = (int32_t)i;
        recs[i].key = u(rng);
        keys[i] = recs[i].key;
    }
    std::vector<uint8_t> before((uint8_t *)recs.data(),
                                (uint8_t *)recs.data() + n * sizeof(Rec)); // reference copy

    int wss = 0;
    ASSERT_EQ(amd_opt_stablesort_getsize_64f((int)n, &wss), STS_OK);
    std::vector<uint8_t> ws((size_t)wss);
    std::vector<int32_t> out(n);
    ASSERT_EQ(amd_opt_stablesort_ascend_64f(&recs[0].key, (int)sizeof(Rec),
                                            out.data(), (int)n, ws.data()),
              STS_OK);
    EXPECT_TRUE(stable_sorted(keys, out.data(), n));
    std::vector<uint8_t> after((uint8_t *)recs.data(),
                               (uint8_t *)recs.data() + n * sizeof(Rec));
    EXPECT_EQ(before, after); // validates that src bytes are not modified
}

// Different distributions, sizes, and strides.
AOCLSORT_TEST(ApiAscend, FunctionalMatrix)
{
    const Dist dists[] = {Dist::Sorted,      Dist::Reverse,   Dist::Uniform,
                          Dist::AllEqual,    Dist::Staircase, Dist::NearlySorted,
                          Dist::FewUnique,   Dist::Special};
    const size_t sizes[] = {1, 2, 10, 2048, 5000, 70000}; /* trivial, min, non-power-of-2 */
    const int strides[] = {8, 64, 12 /* unaligned */};
    for (Dist d : dists)
        for (size_t n : sizes)
            for (int st : strides)
                expect_api_sorts(gen(d, n, test_seed(n)), st);
}

// Missed dispatch paths in FunctionalMatrix
AOCLSORT_TEST(ApiAscend, MissedDispatchPaths)
{
    // n >= LSD_MIN_N but footprint > L3 -> fall through to MSD-LSD radix.
    const size_t n = 65536;      // == LSD_MIN_N
    const int stride = 1024;     // 1024 * 65536 = 64 MiB > 32 MiB L3 cap
    expect_api_sorts(gen(Dist::Uniform, n, test_seed(n)), stride);
}

// workspace overflow test (under ASAN) on adversarial inputs that maximally
// use the scratch workspace
AOCLSORT_TEST(Memory, ExactWorkspaceAdversarial)
{
    /* shivers path */
    {
        const size_t n = 2048; /* Take shivers path */
        /* Zigzag pattern ( magnitudes strictly increasing but the sign alternates every element ) ->
         * no gallops beyond 1 -> maximizes short runs -> maximizes pushes on the shivers run stack. */
        std::vector<double> v(n);
        for (size_t i = 0; i < n; ++i) v[i] = (i & 1) ? -(double)i : (double)i;
        expect_api_sorts(v, 8);
    }

    /* radix paths */
    for (size_t n : {size_t(5000) /* MSD radix path */, size_t(100000) /* Flat LSD path */}) { 
        /* Exponential clusters almost everything near 0 with a thin tail ->
        * Top MSD digit dumps the vast majority into one/few buckets ->
        * Each MSD pass barely partitions, deep recursion. */
        std::vector<double> v(n);
        std::mt19937_64 rng(5);
        std::exponential_distribution<double> e(1.0);
        for (size_t i = 0; i < n; ++i) v[i] = e(rng);
        expect_api_sorts(v, 8);

        /* FewUnique again dumps to few buckets but triggers constant-digit skip */
        expect_api_sorts(gen(Dist::FewUnique, n, test_seed(n)), 8);
    }
    /* Forces MSD recursion to exhaust ALM_SORT_MAX_MSD_FRAMES
     * and enter the strided-LSD fallback (the deepest radix workspace path). */
    expect_api_sorts(msd_deep_recursion_keys(3000 /* > LEAF_CAP + 3 */, 7), 8);
}

#ifdef TEST_EXTREME
// Resource-heavy edge cases. Off by default; needs ~25 GB RAM.
AOCLSORT_TEST(Extreme, HugeStrideAndGiantRun)
{
    // stride = INT_MAX with a matching buffer.
    const int n = 3;
    const long stride = INT_MAX;
    std::vector<uint8_t> buf((size_t)(n - 1) * (size_t)stride + sizeof(double), 0);
    double vals[3] = {3.0, 1.0, 2.0};
    for (int i = 0; i < n; ++i)
        std::memcpy(buf.data() + (size_t)i * (size_t)stride, &vals[i], sizeof(double));
    int wss = 0;
    ASSERT_EQ(amd_opt_stablesort_getsize_64f(n, &wss), STS_OK);
    std::vector<uint8_t> ws((size_t)wss);
    int out[3];
    ASSERT_EQ(amd_opt_stablesort_ascend_64f((const double *)buf.data(), (int)stride,
                                            out, n, ws.data()), STS_OK);
    std::vector<double> keys(vals, vals + n);
    EXPECT_TRUE(stable_sorted(keys, out, n));

    /* A single sorted run longer than 2^30 to drive the gallop probe
     * to its maximum depth on left/right branches. Confirms the
     * huge-run gallop returns the correct end boundaries (0 and big).*/
    const size_t big = (size_t(1) << 30) + 16;
    std::vector<double> up(big);
    for (size_t i = 0; i < big; ++i) up[i] = (double)i;
    auto r = make_strided(up, 8);
    std::vector<int32_t> arr(big);
    for (size_t i = 0; i < big; ++i) arr[i] = (int32_t)i;
    EXPECT_EQ(alm_sort_test_gallop_left(total_key(-1.0) /* below all */, arr.data(), r.data(), 8,
                                        0, (int)big, (int)big - 1), 0 /* worst hint */);
    EXPECT_EQ(alm_sort_test_gallop_right(total_key(1e18) /* above all */, arr.data(), r.data(), 8,
                                         0, (int)big, 0), (int)big /* worst hint */);
}
#endif
