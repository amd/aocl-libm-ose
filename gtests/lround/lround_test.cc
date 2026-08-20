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

#include "test_lround_data.h"

/*
 * Forward declarations for the AOCL lround family.
 * These are declared extern "C" to match the C linkage of the library.
 */
extern "C" {
    long      amd_lround(double x);
    long      amd_lroundf(float x);
    long long amd_llround(double x);
    long long amd_llroundf(float x);
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
/* lround(double)                                                      */
/* ------------------------------------------------------------------ */

TEST(lround, SPECIALCASE_DOUBLE)
{
    for (size_t i = 0; i < sizeof(LroundF64Cases) / sizeof(LroundF64Cases[0]); ++i) {
        const struct LroundF64Data &tc = LroundF64Cases[i];
        double x = bits_to_double(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long result = amd_lround(x);

#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
        EXPECT_EQ(result, tc.m_out)
            << "lround(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";
#else
        if (tc.m_excepts == 0) {
            EXPECT_EQ(result, tc.m_out)
                << "lround(" << x << "): got " << result << ", expected " << tc.m_out
                << " (case " << i << ")";
        }
#endif

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "lround(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "lround(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}

/* ------------------------------------------------------------------ */
/* llround(double)                                                     */
/* ------------------------------------------------------------------ */

TEST(llround, SPECIALCASE_DOUBLE)
{
    for (size_t i = 0; i < sizeof(LlroundF64Cases) / sizeof(LlroundF64Cases[0]); ++i) {
        const struct LlroundF64Data &tc = LlroundF64Cases[i];
        double x = bits_to_double(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long long result = amd_llround(x);

#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
        EXPECT_EQ(result, tc.m_out)
            << "llround(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";
#else
        if (tc.m_excepts == 0) {
            EXPECT_EQ(result, tc.m_out)
                << "llround(" << x << "): got " << result << ", expected " << tc.m_out
                << " (case " << i << ")";
        }
#endif

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "llround(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "llround(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}

/* ------------------------------------------------------------------ */
/* lroundf(float)                                                      */
/* ------------------------------------------------------------------ */

TEST(lroundf, SPECIALCASE_FLOAT)
{
    for (size_t i = 0; i < sizeof(LroundF32Cases) / sizeof(LroundF32Cases[0]); ++i) {
        const struct LroundF32Data &tc = LroundF32Cases[i];
        float x = bits_to_float(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long result = amd_lroundf(x);

#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
        EXPECT_EQ(result, tc.m_out)
            << "lroundf(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";
#else
        if (tc.m_excepts == 0) {
            EXPECT_EQ(result, tc.m_out)
                << "lroundf(" << x << "): got " << result << ", expected " << tc.m_out
                << " (case " << i << ")";
        }
#endif

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "lroundf(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "lroundf(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}

/* ------------------------------------------------------------------ */
/* llroundf(float)                                                     */
/* ------------------------------------------------------------------ */

TEST(llroundf, SPECIALCASE_FLOAT)
{
    for (size_t i = 0; i < sizeof(LlroundF32Cases) / sizeof(LlroundF32Cases[0]); ++i) {
        const struct LlroundF32Data &tc = LlroundF32Cases[i];
        float x = bits_to_float(tc.m_in);

        feclearexcept(FE_ALL_EXCEPT);
        long long result = amd_llroundf(x);

#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
        EXPECT_EQ(result, tc.m_out)
            << "llroundf(" << x << "): got " << result << ", expected " << tc.m_out
            << " (case " << i << ")";
#else
        if (tc.m_excepts == 0) {
            EXPECT_EQ(result, tc.m_out)
                << "llroundf(" << x << "): got " << result << ", expected " << tc.m_out
                << " (case " << i << ")";
        }
#endif

        if (tc.m_excepts != 0) {
            EXPECT_NE(fetestexcept(FE_INVALID), 0)
                << "llroundf(" << x << "): expected FE_INVALID but it was not raised"
                << " (case " << i << ")";
        } else {
            EXPECT_EQ(fetestexcept(FE_INVALID), 0)
                << "llroundf(" << x << "): FE_INVALID raised unexpectedly"
                << " (case " << i << ")";
        }
    }
}
