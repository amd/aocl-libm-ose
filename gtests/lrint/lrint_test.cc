/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include "gtest.h"
#include <fenv.h>
#include <stdint.h>
#include <limits.h>
#include <cstring>

#include "test_lrint_data.h"

/*
 * Forward declarations for the AOCL lrint family.
 * These are declared extern "C" to match the C linkage of the library.
 */
extern "C" {
    long      amd_lrint(double x);
    long      amd_lrintf(float x);
    long long amd_llrint(double x);
    long long amd_llrintf(float x);
}

/* Reinterpret uint64_t bits as double. */
static inline double bits_to_double(uint64_t bits)
{
    double v;
    std::memcpy(&v, &bits, sizeof(v));
    return v;
}

/* Reinterpret uint32_t bits as float. */
static inline float bits_to_float(uint32_t bits)
{
    float v;
    std::memcpy(&v, &bits, sizeof(v));
    return v;
}

/* ------------------------------------------------------------------ */
/* lrint(double)                                                       */
/* ------------------------------------------------------------------ */

TEST(lrint, SPECIALCASE_DOUBLE)
{
    for (size_t i = 0; i < sizeof(LrintF64Cases) / sizeof(LrintF64Cases[0]); ++i) {
        const struct LrintF64Data &tc = LrintF64Cases[i];
        double x = bits_to_double(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long result = amd_lrint(x);

        EXPECT_EQ(result, tc.m_out)
            << "lrint(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "lrint(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "lrint(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }

#if LONG_MAX == 0x7fffffffL
    /* On 32-bit long, LONG_MAX + 0.5 = 2147483647.5 rounds to 2^31 in
       FE_TONEAREST (nearest-even; 2^31 is even), which overflows long.
       The correct result is LONG_MIN with FE_INVALID raised. */
    {
        int saved_round = fegetround();
        fesetround(FE_TONEAREST);
        feclearexcept(FE_ALL_EXCEPT);
        double x = (double)LONG_MAX + 0.5;
        long result = amd_lrint(x);
        fesetround(saved_round);
        EXPECT_EQ(result, LONG_MIN)
            << "lrint(LONG_MAX + 0.5) with FE_TONEAREST: expected LONG_MIN";
        EXPECT_NE(fetestexcept(FE_INVALID), 0)
            << "lrint(LONG_MAX + 0.5) with FE_TONEAREST: expected FE_INVALID";
    }
#endif
}

/* ------------------------------------------------------------------ */
/* llrint(double)                                                      */
/* ------------------------------------------------------------------ */

TEST(llrint, SPECIALCASE_DOUBLE)
{
    for (size_t i = 0; i < sizeof(LlrintF64Cases) / sizeof(LlrintF64Cases[0]); ++i) {
        const struct LlrintF64Data &tc = LlrintF64Cases[i];
        double x = bits_to_double(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long long result = amd_llrint(x);

        EXPECT_EQ(result, tc.m_out)
            << "llrint(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "llrint(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "llrint(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}

/* ------------------------------------------------------------------ */
/* lrintf(float)                                                       */
/* ------------------------------------------------------------------ */

TEST(lrintf, SPECIALCASE_FLOAT)
{
    for (size_t i = 0; i < sizeof(LrintF32Cases) / sizeof(LrintF32Cases[0]); ++i) {
        const struct LrintF32Data &tc = LrintF32Cases[i];
        float x = bits_to_float(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long result = amd_lrintf(x);

        EXPECT_EQ(result, tc.m_out)
            << "lrintf(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "lrintf(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "lrintf(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}

/* ------------------------------------------------------------------ */
/* llrintf(float)                                                      */
/* ------------------------------------------------------------------ */

TEST(llrintf, SPECIALCASE_FLOAT)
{
    for (size_t i = 0; i < sizeof(LlrintF32Cases) / sizeof(LlrintF32Cases[0]); ++i) {
        const struct LlrintF32Data &tc = LlrintF32Cases[i];
        float x = bits_to_float(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long long result = amd_llrintf(x);

        EXPECT_EQ(result, tc.m_out)
            << "llrintf(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "llrintf(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "llrintf(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}

