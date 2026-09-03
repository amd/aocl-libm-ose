/*
* Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
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
* ISO-IEC-10967-2: Elementary Numerical Functions
* Signature:
*   float asinhf(float x)
*
* Spec:
*   asinhf(+/-inf) = +/-inf
*   asinhf(+/-0)   = +/-0
*
* Implementation Notes
* ---------------------
* Split at |x| = 1.094 (bit pattern 0x3F8C0831).
*
* Small-x path (|x| <= 1.094):
*   asinhf(x) = x * P(x^2)
*   P is a degree-9 minimax polynomial for asinh(sqrt(t))/sqrt(t) on [0, 1.094^2].
*   Coefficients are evaluated in double to avoid accumulation of float Horner
*   rounding errors; the final result is < 0.5 ULP over the entire small-x range.
*
* Large-x path (|x| > 1.094):
*   asinhf(x) = ln(|x|) + F(1/x^2)
*   The natural log is computed inline using the same table-assisted algorithm
*   as ALM_PROTO_OPT(logf): 256-entry lookup + degree-2 polynomial correction.
*   F(t) = ln(1 + sqrt(1+t)) uses a two-piece Chebyshev approximation split at
*   T_CUT = 0.293407 (corresponding to |x| = 1.846):
*     piece 1 (t < T_CUT, |x| > 1.846): degree-5 in t;
*       max relative error 2^-27.81; threshold 2^-23.60.
*     piece 2 (t >= T_CUT, 1.094 < |x| <= 1.846): degree-5 in u = t - T_CUT;
*       max relative error 2^-25.08; threshold 2^-24.78.
*   lnx and F are accumulated in double, giving combined error < 1 ULP.
*/

#include <math.h>
#include <stdint.h>
#include "libm_util_amd.h"
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include "logf_data.h"

#define LOGF_N          8
#define MASK_MANT_ALL7  0x007f8000u  // top 8 mantissa bits

// |x| < 2^-12: asinhf(x) rounds to x
#define ASINHF_TINY_THRESHOLD  0x39800000u

// small-path / large-path split at |x| = 1.094
#define ASINHF_SMALL_LIMIT     0x3F8C0831u

/*
 * Small-path: degree-9 double-precision minimax P(t) for
 * asinh(sqrt(t))/sqrt(t), t = x^2, on [0, 1.094^2].
 * Coefficients computed by Remez exchange to full double precision
 * (max poly error ~1.65e-9 = 0.028 ULP of the float output range).
 * Evaluated in double to avoid float Horner rounding accumulation;
 * final float result is < 0.5 ULP over the entire small-x range.
 */

#define ASINHF_P9  -0x1.f2116dc52312cp-13
#define ASINHF_P8   0x1.be748b6942e7dp-10
#define ASINHF_P7  -0x1.74999d117c53ep-8
#define ASINHF_P6   0x1.8fe394ba66bbdp-7
#define ASINHF_P5  -0x1.4a530ec8907f9p-6
#define ASINHF_P4   0x1.e7b59dfd7c10ap-6
#define ASINHF_P3  -0x1.6ce184a043fa5p-5
#define ASINHF_P2   0x1.332a090c29d65p-4
#define ASINHF_P1  -0x1.55552dbc02122p-3
#define ASINHF_P0   0x1.fffffff1cabf7p-1

// ln(2) in head+tail split, matching logf_data
#define LOG2_HEAD  0x1.62e3p-1
#define LOG2_TAIL  0x1.2fefa2p-17

// degree-2 log correction polynomial coefficients
#define LOG_C1  0x1.0p-1       // 1/2
#define LOG_C2  0x1.555556p-2  // 1/3

// F(t) piecewise split: T_CUT = 0.293407, |x| = 1.846
#define T_CUT   0x1.2c72e275ab7dcp-2

// F(t) piece 1 (t < T_CUT): degree-5 in t; error 2^-27.81
#define F_P1_C5   0x1.b5fecfeae78b3p-7
#define F_P1_C4  -0x1.fb3efaa1e1fccp-6
#define F_P1_C3   0x1.a6edebb6b6d34p-5
#define F_P1_C2  -0x1.7fe076117bfb6p-4
#define F_P1_C1   0x1.ffff9d3f20adcp-3
#define F_P1_C0   0x1.62e43008e58f8p-1

// F(t) piece 2 (t >= T_CUT): degree-5 in u = t - T_CUT; error 2^-25.08
#define F_P2_C5   0x1.a9ff2e1522ae0p-9
#define F_P2_C4  -0x1.74422fe7658b1p-7
#define F_P2_C3   0x1.a8695e59b6880p-6
#define F_P2_C2  -0x1.f27ea34f741b2p-5
#define F_P2_C1   0x1.a546246e62dcfp-3
#define F_P2_C0   0x1.84e1acb7bb42fp-1

extern struct logf_table logf_lookup[];

float
ALM_PROTO_OPT(asinhf)(float x)
{
    uint32_t ax = asuint32(x) & POS_BITSET_F32;
    float result = 0.0f;

    // Wraparound makes 0 -> 0xFFFFFFFF
    if (unlikely(ax - 1 >= POS_INF_F32 - 1))
    {
        // Zero, Inf or NaN: x+x propagates NaN and returns +/-inf unchanged
        // FE_INVALID raised for sNaN
        result = x + x;
    } else if (unlikely(ax < ASINHF_TINY_THRESHOLD)) {
        // Tiny: |x| < 2^-12; asinhf(x) ~= x
        ALM_RAISE_FE_INEXACT();
        result = x;
    } else if (ax <= ASINHF_SMALL_LIMIT) {
        // Small-x path: |x| <= 1.094f
        double xd = (double)x;
        double t  = xd * xd;
        // Horner evaluation of degree-8 minimax P(t) for
        // asinh(sqrt(t))/sqrt(t); evaluated in double to
        // avoid accumulation of float Horner rounding errors.
        double p = ASINHF_P9;
        p = p * t + ASINHF_P8;
        p = p * t + ASINHF_P7;
        p = p * t + ASINHF_P6;
        p = p * t + ASINHF_P5;
        p = p * t + ASINHF_P4;
        p = p * t + ASINHF_P3;
        p = p * t + ASINHF_P2;
        p = p * t + ASINHF_P1;
        p = p * t + ASINHF_P0;
        result = (float)(xd * p);
    } else {
        // Large-x path: asinh(x) = ln(|x|) + ln(1 + sqrt(1 + 1/x^2))
        //
        // ln(|x|) is computed inline using the logf table-assisted algorithm:
        //   |x| = m * 2^e  (m in [0.5, 1) after the HALFEXPBITS trick)
        //   idx = top 8 mantissa bits
        //   r   = (F - m) / F  (F = table center, finv = 1/F from table)
        //   ln(|x|) = e*ln2 + ln(F) - [r + r^2*(C1 + r*C2)]
        //   lnx and fp are accumulated in double to keep the combined result
        //   within 1 ULP when rounded to float.
        float absx = asfloat(ax);

        int32_t expo  = (int32_t)(ax >> EXPSHIFTBITS_SP32) - EMAX_SP32;
        double  dexpo = (double)expo;

        uint32_t mant  = ax & MANTBITS_SP32;
        uint32_t mant1 = ax & MASK_MANT_ALL7;
        uint32_t idx   = mant1 >> (EXPSHIFTBITS_SP32 - LOGF_N);  // >> 15

        float y = asfloat(mant  | HALFEXPBITS_SP32);
        float f = asfloat(mant1 | HALFEXPBITS_SP32);

        const struct logf_table* tbl = &logf_lookup[idx];
        double finv = tbl->f_inv;
        double r    = ((double)f - (double)y) * finv;

        double r2  = r * r;
        double q   = r + r2 * (LOG_C1 + r * LOG_C2);

        // Combine exponent and table head/tail with correction
        double lnx = (dexpo * LOG2_HEAD + (double)tbl->f_128_head)
            + ((double)tbl->f_128_tail + dexpo * LOG2_TAIL - q);

        // F(t) = ln(1 + sqrt(1+t)), t = 1/x^2: two-piece degree-5 Chebyshev.
        // Piece 1 (t < T_CUT, |x| > 1.846): polynomial in t; error 2^-27.81.
        // Piece 2 (t >= T_CUT):             polynomial in u=t-T_CUT; error 2^-25.08.
        double t = 1.0 / ((double)absx * (double)absx);
        double fp = 0.0;
        if (t < T_CUT) {
            fp =           F_P1_C5;
            fp = fp * t +  F_P1_C4;
            fp = fp * t +  F_P1_C3;
            fp = fp * t +  F_P1_C2;
            fp = fp * t +  F_P1_C1;
            fp = fp * t +  F_P1_C0;
        } else {
            double u = t - T_CUT;
            fp =           F_P2_C5;
            fp = fp * u +  F_P2_C4;
            fp = fp * u +  F_P2_C3;
            fp = fp * u +  F_P2_C2;
            fp = fp * u +  F_P2_C1;
            fp = fp * u +  F_P2_C0;
        }
        result = copysignf((float)(lnx + fp), x);
    }

    return result;
}
