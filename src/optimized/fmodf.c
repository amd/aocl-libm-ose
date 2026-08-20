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

/******************************************
 * Implementation Notes:
 *
 * Prototype:
 * float fmodf(float x, float y)
 *
 * Algorithm:
 * fmodf(x, y) = x - n*y, where n = trunc(x/y).
 *
 * Integer fast path (exponent difference shift <= 40):
 *   Extract 24-bit float significands Mx, My.  Compute rem = Mx * 2^d mod My
 *   using 64-bit arithmetic (Mx < 2^24 and shift <= 40, so Mx * 2^d < 2^64).
 *   No floating-point operations, so no exceptions are raised.
 *   FE_UNDERFLOW is raised explicitly for subnormal results on Linux.
 */

#include <float.h>
#include <math.h>
#include <stdint.h>

#ifdef __linux__
#include <fenv.h>
#endif

#include "libm_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#if defined(__GNUC__) || defined(__clang__)

#define CLZ32(x) __builtin_clz(x)

#elif defined(_MSC_VER)

#include <intrin.h>
static inline int alm_clz32(uint32_t x)
{
    unsigned long idx;
    _BitScanReverse(&idx, x);
    return 31 - (int)idx;
}
#define CLZ32(x) alm_clz32(x)

#else

#error "Intrinsic for counting leading zeroes not found"

#endif

#define MAXSHIFT (64 - MANTLENGTH_SP32)   // 40

typedef struct
{
    uint32_t m;  // 24-bit significand (including implicit 1)
    int e;       // biased exponent
} F32ExpMan;

// Extract a single precision value into a mantissa and biased exponent
// Handles subnormal values
static inline F32ExpMan F32Extract(uint32_t fax)
{
    int lz;
    return unlikely(fax < POS_LNORMAL_F32) ?
        lz = CLZ32(fax),
        (F32ExpMan) {  // Subnormal values; shift leftmost 1 into implied bit
            .m = fax << (lz - (32 - MANTLENGTH_SP32)),
            .e = (32 - MANTLENGTH_SP32 + 1) - lz
        } :
        (F32ExpMan) {  // Normal values
            .m = (fax & MANTBITS_SP32) | IMPBIT_SP32,
            .e = (int)(fax >> EXPSHIFTBITS_SP32)
        };
}

float ALM_PROTO_OPT(fmodf)(float x, float y)
{
    uint32_t fax = asuint32(x) & POS_BITSET_F32;  // |x| bit pattern
    uint32_t fay = asuint32(y) & POS_BITSET_F32;  // |y| bit pattern
    float result = x;  // Default result value = x; saves sign bit for later

    // All error conditions are caught by one predicted-untaken branch
    if (unlikely(((fay - 1) | fax) >= POS_INF_F32))
    {
        if (fay > POS_INF_F32)
        {   // |y| NaN
            result = x * y;
        }
        else if (fax > POS_INF_F32)
        {   // |x| NaN
            result = x + x;
        }
        else if ((fax == POS_INF_F32) || (fay == 0))
        {   // |x| == Inf || y == 0
            result = __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
        }
        else
        {   // False positive ((fay-1) | fax) >= POS_INF_F32; continue normally
            goto noerror;
        }
    }
    else noerror: if (likely(fax >= fay))
    {   // |x| >= |y|
        F32ExpMan fpx = F32Extract(fax);  // Mantissa and biased exponent of x
        F32ExpMan fpy = F32Extract(fay);  // Mantissa and biased exponent of y
        int     shift = fpx.e - fpy.e;    // result = (x * 2^shift) mod y
        uint32_t  rem = fpx.m;

        // While shift > 40, compute (x * 2^40) mod y
        while (unlikely(shift > MAXSHIFT)) {
            rem = (uint32_t)(((uint64_t)rem << MAXSHIFT) % fpy.m);
            shift -= MAXSHIFT;
        }

        // Compute (x * 2^shift) mod y
        rem = (uint32_t)(((uint64_t)rem << shift) % fpy.m);

        if (likely(rem != 0)) {
            // k = number of bits to shift rem left to position implied bit
            int k = CLZ32(rem) - (32 - MANTLENGTH_SP32);

            if (k < fpy.e)
            {
                // k < fpy.e for normals, where fpy.e - k is biased result exponent
                rem = ((uint32_t)(fpy.e - k) << EXPSHIFTBITS_SP32)
                    | ((rem << k) & MANTBITS_SP32);
            } else {
                // For k >= fpy.e, result is subnormal and shifted in mantissa bits
                rem = likely(fpy.e > 0) ? rem << (fpy.e - 1) : rem >> (1 - fpy.e);
#ifdef __linux__
                // On Linux, raise FE_UNDERFLOW for glibc compatibility
                feraiseexcept(FE_UNDERFLOW);
#endif
            }
        }

        // Result is same sign as original x with exponent and mantissa in rem
        result = asfloat(rem | (asuint32(result) & SIGNBIT_SP32));
    }
    return result;
}
