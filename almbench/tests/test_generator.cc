/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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
 * Unit tests for Generators
 *
 * Philosophy: Minimal sanity tests for generators only.
 * Keeping the tests minimal on purpose as our focus is 
 * to test the library that ships and not almbench per say
 * (which is not part of AOCL package).
 * 
 * However these minimal tests give us confidence the generators are working
 * as expected.
 */

#include <cstdio>
#include <cmath>
#include <vector>
#include "generator.h"
#include "alm_test.h"

/* -------------------------------------------------------------------------- */
/* Helpers                                                                     */
/* -------------------------------------------------------------------------- */

static int tests_passed = 0;
static int tests_failed = 0;

#define CHECK(cond, msg) do { \
    if (cond) { \
        tests_passed++; \
    } else { \
        tests_failed++; \
        printf("  FAIL: %s\n", (msg)); \
    } \
} while (0)

#define TEST_GROUP(name) printf("\n[%s]\n", (name))

static bool combos_equal(const std::vector<std::vector<size_t>> &got,
                         const std::vector<std::vector<size_t>> &exp)
{
    return got == exp;
}

/* -------------------------------------------------------------------------- */
/* Test 1: enumerate_combinations                                              */
/* -------------------------------------------------------------------------- */

static void test_enumerate_combinations()
{
    TEST_GROUP("enumerate_combinations");

    /* n=4, r=2 — C(4,2)=6 combos */
    {
        auto got = enumerate_combinations(4, 2);
        std::vector<std::vector<size_t>> exp = {
            {0,1},{0,2},{0,3},{1,2},{1,3},{2,3}
        };
        CHECK(got.size() == 6,     "n=4 r=2: result count == 6");
        CHECK(combos_equal(got, exp), "n=4 r=2: combos match expected");
    }

    /* n=3, r=3 — C(3,3)=1 combo */
    {
        auto got = enumerate_combinations(3, 3);
        std::vector<std::vector<size_t>> exp = { {0,1,2} };
        CHECK(got.size() == 1,         "n=3 r=3: result count == 1");
        CHECK(combos_equal(got, exp),  "n=3 r=3: combo is {0,1,2}");
    }

    /* n=0, r non-zero — empty */
    {
        auto got = enumerate_combinations(0, 2);
        CHECK(got.empty(), "n=0 r=2: returns empty");
    }

    /* r=0 — empty */
    {
        auto got = enumerate_combinations(4, 0);
        CHECK(got.empty(), "n=4 r=0: returns empty");
    }

    /* r > n — empty */
    {
        auto got = enumerate_combinations(2, 3);
        CHECK(got.empty(), "n=2 r=3 (r>n): returns empty");
    }
}

/* -------------------------------------------------------------------------- */
/* Test 2: MultiRangeGenerator lane values                                    */
/* -------------------------------------------------------------------------- */
/*
 * Setup:
 *   lane_width = 2,  n = 3 sub-gens
 *   Combos (C(3,2)=3): {0,1}, {0,2}, {1,2}  (lexicographic)
 *
 *   Each InpRng has rng.count = 1.
 *   make_multistep_subgen multiplies by lane_width=2, so the scalar
 *   LinearGenerator receives count=2 → produces 3 values: rmin, rmin+step, rmax.
 *
 *   Gen0: LinearGenerator(0,  2,  2) → 0.0, 1.0, 2.0
 *   Gen1: LinearGenerator(10, 12, 2) → 10.0, 11.0, 12.0
 *   Gen2: LinearGenerator(20, 22, 2) → 20.0, 21.0, 22.0
 *
 *   Outer iter call trace (wrap_next resets when has_next() == false):
 *     iter 0  combo {0,1}:  g0→0.0  g1→10.0   → [0.0,  10.0]
 *     iter 1  combo {0,2}:  g0→1.0  g2→20.0   → [1.0,  20.0]
 *     iter 2  combo {1,2}:  g1→11.0 g2→21.0   → [11.0, 21.0]
 *     iter 3  combo {0,1}:  g0→2.0  g1→12.0   → [2.0,  12.0]
 *     iter 4  combo {0,2}:  g0 wraps→0.0  g2→22.0  → [0.0, 22.0]
 *     iter 5  combo {1,2}:  g1 wraps→10.0 g2 wraps→20.0 → [10.0, 20.0]
 */
static void test_multi_range_generator_samples()
{
    TEST_GROUP("MultiRangeGenerator samples (lane_width=2, n=3)");

    const size_t lane_width = 2;

    /* Build InpRng for each sub-domain */
    auto make_rng = [](double srt, double stp) {
        InpRng<double> r{};
        r.srt   = srt;
        r.stp   = stp;
        r.type  = E_Linear;
        r.count = 1;       /* make_multistep_subgen will multiply by lane_width */
        return r;
    };

    std::vector<InpRng<double>> ranges = {
        make_rng( 0.0,  2.0),   /* gen0: 0,1,2 */
        make_rng(10.0, 12.0),   /* gen1: 10,11,12 */
        make_rng(20.0, 22.0),   /* gen2: 20,21,22 */
    };

    MultiRangeGenerator<double> gen(ranges, lane_width);

    /* Expected {lane0, lane1} for 6 outer iterations */
    struct Expected { double l0, l1; };
    const Expected exp[6] = {
        { 0.0, 10.0},
        { 1.0, 20.0},
        {11.0, 21.0},
        { 2.0, 12.0},
        { 0.0, 22.0},
        {10.0, 20.0},
    };

    for (int k = 0; k < 6; ++k) {
        double *buf = gen.wrap_next();
        char msg[64];

        std::snprintf(msg, sizeof(msg), "iter %d: lane0 == %.1f", k, exp[k].l0);
        CHECK(buf[0] == exp[k].l0, msg);

        std::snprintf(msg, sizeof(msg), "iter %d: lane1 == %.1f", k, exp[k].l1);
        CHECK(buf[1] == exp[k].l1, msg);
    }
}

/* -------------------------------------------------------------------------- */
/* main                                                                        */
/* -------------------------------------------------------------------------- */

int main()
{
    printf("=== almbench generator tests ===\n");

    test_enumerate_combinations();
    test_multi_range_generator_samples();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);
    printf("=== Test %s ===\n", tests_failed == 0 ? "PASSED" : "FAILED");

    return tests_failed == 0 ? 0 : 1;
}
