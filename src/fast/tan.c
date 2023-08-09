/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/alm_special.h>

#include <libm/amd_funcs_internal.h>

#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double tan(double x)
 *
 * Spec:
 *   tan(n· 2π + π/4)  = 1       if n∈Z and |n· 2π + π/4|   <= big_angle_F
 *   tan(n· 2π + 3π/4) = −1      if n∈Z and |n· 2π + 3π/4|  <= big angle rF
 *   tan(x) = x                  if x ∈ F^(2·π) and tan(x) != tan(x)
 *                                               and |x| < sqrt(epsilonF/rF)
 *   tan(−x) = −tan(x)          if x ∈ F^(2·π)
 */

static struct {
    double invhalfpi, huge;
    double halfpi1, halfpi2, halfpi3;
    double poly[14];
} tan_data = {
    .huge      = 0x1.8000000000000p52,
    //.halfpi   = 0x1.921fb54442d18p0,
    .invhalfpi = 0x1.45f306dc9c882a53f85p-1,
    .halfpi1   = 0x1.921fb54442d18p0,
    .halfpi2   = 0x1.1a62633145c07p-54,
    .halfpi3   = -0x1.f1976b7ed8fbcp-110,

    /*
     * Polynomial coefficients obtained using
     * fpminimax algorithm from Sollya
     */
    .poly = {
        0x1.55555555554bep-2,
        0x1.1111111119f2ap-3,
        0x1.ba1ba1b38733cp-5,
        0x1.664f49c8b63e3p-6,
        0x1.226e0f7f17778p-7,
        0x1.d6d989f491431p-9,
        0x1.7d57d7c375c03p-10,
        0x1.38148605a1756p-11,
        0x1.d15fa298b8b17p-13,
        0x1.20250b03ea768p-13,
        -0x1.cd6072c36a433p-16,
        0x1.7b1cbff8d88e6p-14,
        -0x1.7c588d6a4c96ep-15,
        0x1.5fc28759e55bap-16,
    },

};

#define ALM_TAN_HUGE_VAL        tan_data.huge
//#define ALM_TAN_HALFPI          tan_data.halfpi
#define ALM_TAN_HALFPI1         tan_data.halfpi1
#define ALM_TAN_HALFPI2         tan_data.halfpi2
#define ALM_TAN_HALFPI3         tan_data.halfpi3
#define ALM_TAN_INVHALFPI       tan_data.invhalfpi

#define C1  tan_data.poly[0]
#define C3  tan_data.poly[1]
#define C5  tan_data.poly[2]
#define C7  tan_data.poly[3]
#define C9  tan_data.poly[4]
#define C11 tan_data.poly[5]
#define C13 tan_data.poly[6]
#define C15 tan_data.poly[7]
#define C17 tan_data.poly[8]
#define C19 tan_data.poly[9]
#define C21 tan_data.poly[10]
#define C23 tan_data.poly[11]
#define C25 tan_data.poly[12]
#define C27 tan_data.poly[13]

#define ALM_TAN_SIGN_MASK     (1UL<<63)
#define ALM_TAN_ARG_MIN       (0x1p-1022)
#define ALM_TAN_ARG_MAX       (0x1p+1023)

/*
 * Implementation Notes:
 *
 * double tan(double x)
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
 *              when N is odd,
 *                     = -cot(F) = -1/tan(F)
 *              when N is even, tan(F) is approximated using a polynomial
 *                      obtained from Remez approximation from Sollya.
 *
 */
double
ALM_PROTO_FAST(tan)(double x)
{
    double   dx, F, poly, result = 0.0;
    uint64_t n, sign;
    uint64_t ux = asuint64(x);

    if (unlikely ((ux - asuint64(ALM_TAN_ARG_MIN)) >
                  (asuint64(ALM_TAN_ARG_MAX) - asuint64(ALM_TAN_ARG_MIN)))) {

        if ((ux & ~ALM_TAN_SIGN_MASK) >= PINFBITPATT_DP64) {
            /* inf or NaN */
            return _tan_special(x);
        }
    }

    if (ux == 0)
        return 0.0;

    sign = ux & ALM_TAN_SIGN_MASK;

    dx = asdouble(ux & ~ALM_TAN_SIGN_MASK);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */
    double dn =  dx * ALM_TAN_INVHALFPI + ALM_TAN_HUGE_VAL;

    /* n = (int)dn */
    n   = asuint64(dn);

    dn -= ALM_TAN_HUGE_VAL;

    /*
     * Get the fraction part
     *   F = xd - (n * π/2)
     */
    F = dx - dn * ALM_TAN_HALFPI1; // F = x - n*pi1/2
    F = F - dn * ALM_TAN_HALFPI2;  // F = F - n*pi2/2
    F = F - dn * ALM_TAN_HALFPI3;  // F = F - n*pi3/2

    uint64_t odd = (n << 63);

    /*
     * Calculate the polynomial approximation
     *  poly = x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 + \
     *          C6*x^13 + C7*x^15 + C8*x^17 + C9*x^19 + \
     *          C10*x^21 + C11*x^23 + C12*x^25 + C13*x^27 + C14*x^29;
     *
     *       = x + x * G*(C6 + G*(C7 + G*(C8 +
     *                  G*(C9 + G*(C10+ G*(C11 + G*(C12 +
     *                          G*(C13 + C14*G))))
     *                  ))))
     *
     * Polynomial is approximated as x+x*P(G)  where G = x^2
     */

    poly = POLY_EVAL_ODD_29(F, C1, C3, C5, C7, C9,
                            C11, C13, C15, C17, C19,
                            C21, C23, C25, C27);

    result = asdouble(asuint64(poly) ^ sign);

    if (odd)
        result = -1.0/result;

    return result;
}

strong_alias (__tan_finite, ALM_PROTO_FAST(tan))
weak_alias (amd_tan, ALM_PROTO_FAST(tan))
weak_alias (tan, ALM_PROTO_FAST(tan))
