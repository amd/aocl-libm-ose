/*
 * Copyright (C) 2008-2025 Advanced Micro Devices, Inc. All rights reserved.
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
 * This is a literal translation of ASM routine for fmod + optimizations
 *
 * Prototype:
 * double fmod(double x, double y)
 *
 * Algorithm:
 * As designed in the ASM fmod double variant routine.
 * The same algorithm is extended into C,
 * with optimizations wherever possible.
 *
 */

#include <stdint.h>
#include <math.h>
#include <float.h>

#include "libm_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#define BIT_MASK_27_BITS 0xfffffffff8000000

#define FMOD_X_NAN   1
#define FMOD_Y_ZERO  2
#define FMOD_X_INF   3

double ALM_PROTO_OPT(fmod)(double x, double y)
{
    uint64_t ax, ay;

    ax = asuint64(x);
    ay = asuint64(y);

    ax &= ~SIGNBIT_DP64;
    ay &= ~SIGNBIT_DP64;

    /* Check if y is NaN. If yes return NaN */
    if(unlikely(ay > POS_INF_F64))
    {
        return x * y;
    }

    /* Check if y is Zero. If yes, return NaN and raise exception*/
    if(unlikely(ay == 0))
    {
        return _fmod_special(x, asdouble(ay | QNANBITPATT_DP64), FMOD_Y_ZERO);
    }
    /* Check if x is NaN or INF */
    if(unlikely((ax & EXPBITS_DP64) >= EXPBITS_DP64))
    {
        /* X is NaN. Return NaN */
        if(ax > POS_INF_F64)
        {
            return x;

        }
        /* X is INF. Return NaN and raise exception */
        else
        {
            return _fmod_special(x, asdouble(ay | QNANBITPATT_DP64), FMOD_X_INF);
        }
    }

    if(unlikely((ay & EXPBITS_DP64) > EXPBITS_DP64))
    {
        return 1.0;
    }

    if(ax == ay)
    {
        return 0.0;
    }

    double adx = asdouble(ax);
    double ady = asdouble(ay);

    // Exponents of x and y
    uint64_t xe = (EXPBITS_DP64 & ax) >> 52;
    uint64_t ye = (EXPBITS_DP64 & ay) >> 52;

    if(adx < ady)
    {
        return x;
    }

    int64_t diff_exp = (int64_t)(xe - ye);
    if(xe == 0 || ye == 0 || diff_exp > 52)
    {
        /* This is a special case of fmod function applicable only for double variant.
         * The x87 assembly instruction FPREM1 (Floating Point Partial fmod) has to be used for accurate calculation.
         * Since its equivalent intrinsic is not available in C, the assembly variant is called here directly instead.
         */
        double result = __amd_bas64_fmod(x, y);
        return result;
    }

    double r = adx/ady;
    uint64_t temp = (uint64_t)r;
    r = (double)(temp);

    // Quad-Precision Multiplication of r and y
    uint64_t ur = asuint64(r);
    uint64_t uhy = ay & BIT_MASK_27_BITS;
    uint64_t uhr = ur & BIT_MASK_27_BITS;

    double hy = asdouble(uhy);
    double hr = asdouble(uhr);
    double ty = ady - hy;
    double tr = r - hr;

    double cc = (((((hy*hr) - (r*ady)) + (hy*tr)) + (ty*hr)) + (tr*ty));

    double c = r*ady;
    double v = adx - c;

    double w = (((adx - v) - c) - cc);

    w += v;

    if(w < 0)
    {
        w = w + ady;
    }

    if(x > 0)
    {
        return w;
    }
    w = 0.0 - w;

    return w;
}
