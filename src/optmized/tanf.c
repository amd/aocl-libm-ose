/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#include <stdint.h>

#include <libm_macros.h>
#include <libm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#define  ALM_TANF_SIGN_MASK   ~(1UL<<63)
#define  ALM_TANF_SIGN_MASK32 ~(1U<<31)


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float tanf(float x)
 *
 * Spec:
 *   tanf(n· 2π + π/4)  = 1       if n∈Z and |n· 2π + π/4|   <= big_angle_F
 *   tanf(n· 2π + 3π/4) = −1      if n∈Z and |n· 2π + 3π/4|  <= big angle rF
 *   tanf(x) = x                  if x ∈ F^(2·π) and tanf(x) != tan(x)
 *                                               and |x| < sqrt(epsilonF/rF)
 *   tanf(−x) = −tanf(x)          if x ∈ F^(2·π)
 */

static const struct {
    double huge;
    double halfpi1;
    double halfpi2;
    double invhalfpi;
    double poly[8];
} tanf_data = {
    .huge    = 0x1.8000000000000p52,
    .halfpi1 = 0x1.921fb54400000p0,
    .halfpi2 = 0x1.0b4611a626331p-34,
    .invhalfpi = 0x1.45f306dc9c882a53f85p-1,
    // Polynomial coefficients obtained using Remez algorithm from Sollya
    .poly = {
        0x1.ffffff99ac0468p-1,
        0x1.55559193ecf2bp-2,
        0x1.1106bf4ba8f408p-3,
        0x1.bbafbb6650308p-5,
        0x1.561638922df5fp-6,
        0x1.7f3a033a88788p-7,
        -0x1.ba0d41d26961f8p-11,
        0x1.3952b4eff28ac8p-8,
    },

};

#define ALM_TANF_HUGE_VAL    tanf_data.huge
#define ALM_TANF_HALFPI1     tanf_data.halfpi1
#define ALM_TANF_HALFPI2     tanf_data.halfpi2
#define ALM_TANF_INVHALFPI   tanf_data.invhalfpi

#define C0 tanf_data.poly[0]
#define C2 tanf_data.poly[1]
#define C4 tanf_data.poly[2]
#define C6 tanf_data.poly[3]
#define C8 tanf_data.poly[4]
#define C10 tanf_data.poly[5]
#define C12 tanf_data.poly[6]
#define C14 tanf_data.poly[7]


/*
 * Implementation Notes:
 *
 * float tanf(float x)
 *      A given x is reduced into the form:
 *
 *               |x| = (N * π/2) + F
 *
 *      Where N is an integer obtained using:
 *              N = round(x * 2/π)
 *      And F is a fraction part lying in the interval
 *              [-π/4, +π/4];
 *
 *      obtained as F = |x| - (N * π/2)
 *
 *      Thus tan(x) is given by
 *
 *              tan(x) = tan((N * π/2) + F) = tan(F)
 *              when N is even,
 *                     = -cot(F) = -1/tan(F)
 *              when N is odd, tan(F) is approximated using a polynomial
 *                      obtained from Remez approximation from Sollya.
 *
 */

float
ALM_PROTO_OPT(tanf)(float x)
{
    double     F, xd, tanx = 0.0;
    double     poly;
    uint64_t   sign, uxd, n;
    uint32_t   ux = asuint32(x);

    if(unlikely((ux - asuint32(0x1p-126)) >
                (asuint32(0x1p+127) - asuint32(0x1p-126)))) {

        if((ux  & ALM_TANF_SIGN_MASK32) >= 0x7f800000) {
            /*  infinity or NaN */
            return _tanf_special(x);
        }
    }

    if (ux == 0)
        return 0;

    xd = (double)x;

    uxd = asuint64(xd);

    sign = uxd & (~ALM_TANF_SIGN_MASK);

    /* fabs(x) */
    xd = asdouble(uxd & ALM_TANF_SIGN_MASK);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */
    double dn =  xd * ALM_TANF_INVHALFPI + ALM_TANF_HUGE_VAL;

    /* n = (int)dn */
    n   = asuint64(dn);

    dn -= ALM_TANF_HUGE_VAL;

    /* F = xd - (n * π/2) */
    F = xd - dn * ALM_TANF_HALFPI1;

    F = F - dn *  ALM_TANF_HALFPI2;

    uint64_t odd = (n << 63);

    /*
     * Calculate the polynomial approximation
     *  x * (C1 + C2*x^2 + C3*x^4 + C4*x^6 + C5*x^8 + C6*x^10 + C7*x^12 + C8*x^14)
     * polynomial is approximated as x*P(x^2),
		 * 15 degree, but only even terms are used
     */
    poly = POLY_EVAL_EVEN_15(F, C0, C2, C4, C6, C8, C10, C12, C14);

    tanx = asdouble(asuint64(poly) ^ sign);

    if (odd)
        tanx = -1.0/tanx;

    return eval_as_float(tanx);
}
