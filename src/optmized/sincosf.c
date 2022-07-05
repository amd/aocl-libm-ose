/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *   float cosf(float x)
 *
 * Spec:
 *   cos(0)    = 1
 *   cos(-0)   = -1
 *   cos(inf)  = NaN
 *   cos(-inf) = NaN
 *   sinf(0)    = 0
 *   sinf(-0)   = -0
 *   sinf(inf)  = NaN
 *   sinf(-inf) = NaN
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * Checks for special cases
 * if ( ux = infinity) raise overflow exception and return x
 * if x is NaN then raise invalid FP operation exception and return x.
 *
 * 1. Argument reduction
 * if |x| > 5e5 then
 *      __amd_remainder_piby2d2f((uint64_t)x, &r, &region)
 * else
 *      Argument reduction
 *      Let z = |x| * 2/pi
 *      z = dn + r, where dn = round(z)
 *      rhead =  dn * pi/2_head
 *      rtail = dn * pi/2_tail
 *      r = z – dn = |x| - rhead – rtail
 *      expdiff = exp(dn) – exp(r)
 *      if(expdiff) > 15)
 *      rtail = |x| - dn*pi/2_tail2
 *      r = |x| -  dn*pi/2_head -  dn*pi/2_tail1
 *          -  dn*pi/2_tail2  - (((rhead + rtail) – rhead )-rtail)
 *
 * 2. Polynomial approximation
 * if(dn is even)
 *       x4 = x2 * x2;
 *       s = 0.5 * x2;
 *       t =  1.0 - s;
 *       poly = x4 * (C1 + x2 * (C2 + x2 * (C3 + x2 * C4 )))
 *       r_cos = t + poly
 *       x3 = x2 * r
 *       poly = S2 + (r2 * (S3 + (r2 * (S4))))
 *       r_sin = r - ((x2 * (0.5*rr - x3 * poly)) - rr) - S1 * x3
 * else
 *       x3 = x2 * r
 *       poly = x3 * (S1 + x2 * (S2 + x2 * (S3 + x2 * S4)))
 *       r_cos = r + poly
 *       rr = rr * r;
 *       x4 = x2 * x2;
 *       s = 0.5 * x2;
 *       t =  s - 1.0;
 *       poly = x4 * (C1 + x2 * (C2 + x2 * (C3 + x2 * (C4))))
 *       r_sin = (((1.0 + t) - s) - rr) + poly – t
 * if((sign + 1) & 2)
 *       cos(x) = r_cos
 * else
 *       cos(x) = -r_cos;
 * if(((sign & region) | ((~sign) & (~region))) & 1)
 *       sin(x) = r_sin
 * else
 *       sin(x) -r_sin;
 * if |x| < pi/4 && |x| > 2.0^(-13)
 *   r = 0.5 * x2;
 *   t = 1 - r;
 *   cos(x) = t + ((1.0 - t) - r) + (x*x * (x*x * C1 + C2*x*x + C3*x*x
 *             + C4*x*x +x*x*C5 + x*x*C6)))
 *   sin(x) = x + (x * (r2 * (S1 + r2 * (S2 + r2 * (S3 + r2 * (S4)))))
 * if |x| < 2.0^(-13) && |x| > 2.0^(-27)
 *   cos(x) = 1.0 - x*x*0.5;;
 *   sin(x) = x - (x * x * x * (1/6))
 * else
 *   cos(x) = 1.0
 *   sin(x) = x
 ******************************************
*/

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/alm_special.h>
#include <stdalign.h>

static struct {
    const double twobypi, piby2_1, piby2_1tail, invpi, pi, pi1, pi2;
    const double piby2_2, piby2_2tail, ALM_SHIFT;
    const float one_by_six;
    double poly_sin[4];
    double poly_cos[4];
 } sin_cos_data = {
     .ALM_SHIFT = 0x1.8p+52,
     .one_by_six = 0.166666666666666f,
     .twobypi = 0x1.45f306dc9c883p-1,
     .piby2_1 = 0x1.921fb54400000p0,
     .piby2_1tail = 0x1.0b4611a626331p-34,
     .piby2_2 = 0x1.0b4611a600000p-34,
     .piby2_2tail = 0x1.3198a2e037073p-69,
     .pi = 0x1.921fb54442d18p1,
     .pi1 = 0x1.921fb50000000p1,
     .pi2 = 0x1.110b4611a6263p-25,
     .invpi = 0x1.45f306dc9c883p-2,
     /*
      * Polynomial coefficients
      */
     .poly_sin = {
         -0x1.5555555555555p-3,
         0x1.1111111110bb3p-7,
         -0x1.a01a019e83e5cp-13,
         0x1.71de3796cde01p-19,
     },

     .poly_cos = {
         0x1.5555555555555p-5,   /* 0.0416667 */
         -0x1.6c16c16c16967p-10, /* -0.00138889 */
         0x1.A01A019F4EC91p-16,  /* 2.48016e-005 */
         -0x1.27E4FA17F667Bp-22, /* -2.75573e-007 */
     },
};

void __amd_remainder_piby2d2f(uint64_t x, double *r, int *region);

#define pi          sin_cos_data.pi
#define pi1         sin_cos_data.pi1
#define pi2         sin_cos_data.pi2
#define invpi       sin_cos_data.invpi
#define TwobyPI     sin_cos_data.twobypi
#define PIby2_1     sin_cos_data.piby2_1
#define PIby2_1tail sin_cos_data.piby2_1tail
#define PIby2_2     sin_cos_data.piby2_2
#define PIby2_2tail sin_cos_data.piby2_2tail
#define PIby4       0x3F490FDB
#define FiveE6      0x4A989680
#define ONE_BY_SIX  sin_cos_data.one_by_six
#define ALM_SHIFT   sin_cos_data.ALM_SHIFT

#define S1  sin_cos_data.poly_sin[0]
#define S2  sin_cos_data.poly_sin[1]
#define S3  sin_cos_data.poly_sin[2]
#define S4  sin_cos_data.poly_sin[3]

#define C1  sin_cos_data.poly_cos[0]
#define C2  sin_cos_data.poly_cos[1]
#define C3  sin_cos_data.poly_cos[2]
#define C4  sin_cos_data.poly_cos[3]

#define SIGN_MASK32 0x7FFFFFFF
#define SIGN_MASK   0x7FFFFFFFFFFFFFFF
#define INF32       0x7F800000          /* Infinity */
#define SIN_SMALL   0x3C000000  /* 2.0^(-7) */
#define SIN_SMALLER 0x39000000  /* 2.0^(-13) */


void
ALM_PROTO_OPT(sincosf)(float x, float *sin, float *cos)
{
    double r, r_cos,r_sin, rhead, rtail;
    double xd, x2, x3, x4;
    double poly, t, s;
    uint64_t uy;
    int32_t region;
    alignas(16) uint32_t sign = 0;

    /* Get absolute value of input x */
    uint32_t uxf = asuint32(x);
    sign = uxf >> 31;
    uxf = uxf & SIGN_MASK32;

    if(unlikely(uxf >= INF32)) {
        // infinity or NaN //
        *sin = _sinf_special(x);
        *cos = _cosf_special(x);
        return;
    }

    /* ux > pi/4 */
    if(uxf > PIby4){

        float ax = asfloat(uxf);
        xd = (double)ax;

        /* ux > pi/4 */
        if(uxf < FiveE6){
            /* reduce  the argument to be in a range from -pi/4 to +pi/4
                by subtracting multiples of pi/2 */
            r = TwobyPI * xd; /* x * two_by_pi*/

            /* Get the exponent part */
            int32_t xexp = (int32_t)(uxf >> 23);

            /* dn = int(|x| * 2/pi) */
            double npi2d = r + ALM_SHIFT;
            uint64_t npi2 = asuint64(npi2d);
            npi2d -= ALM_SHIFT;

            /* rhead = x - dn * pi/2_head */
            rhead  = xd - npi2d * PIby2_1;

            /* rtail = dn * pi/2_tail */
            rtail  = npi2d * PIby2_1tail;

            /* r = |x| * 2/pi - dn */
            r = rhead - rtail;
            uy = asuint64(r);

            /* expdiff = exponent(dn) – exponent(r) */
            int64_t expdiff = xexp - (int32_t)((uy << 1) >> 53);
            region = (int32_t)npi2;

            if (expdiff  > 15) {
                t = rhead;
                /* rtail = |x| - dn*pi/2_tail2 */
                rtail =  npi2d * PIby2_2;
                /* r = |x| -  dn*pi/2_head -  dn*pi/2_tail1
                 *     -  dn*pi/2_tail2  - (((rhead + rtail)
                 *     – rhead )-rtail)
                 */
                rhead = t - rtail;
                rtail  = npi2d * PIby2_2tail - ((t - rhead) - rtail);
                r = rhead - rtail;
            }

        }
        else {
            /* Reduce x into range [-pi/4,pi/4] */
            __amd_remainder_piby2d2f(asuint64(xd), &r, &region);
        }

        x2 = r * r;

        if(region & 1) {
            /*cos calculation*/
            /*if region 1 or 3 then sin region */
            x3 = x2 * r;
            /* poly = x3 * (S1 + x2 * (S2 + x2 * (S3 + x2 * S4))) */
            r_cos =  r + (x3 * POLY_EVAL_3(x2, S1, S2, S3, S4));

            /*sin calculation*/
            /*cos region */
            x4 = x2 * x2;
            s = 0.5 * x2;
            t =  1.0 - s;
            poly = x4 * POLY_EVAL_3(x2, C1, C2, C3, C4);
            r_sin = t + poly;
         }
        else {
            /*cos calculation*/
            /* region 0 or 2 do a cos calculation */
            x4 = x2 * x2;
            s = 0.5 * x2;
            t =  1.0 - s;
            /* poly = x4 * (C1 + x2 * (C2 + x2 * (C3 + x2 * C4))) */
            poly = x4 * POLY_EVAL_3(x2, C1, C2, C3, C4);
            r_cos = t + poly;

            /*sin calculation*/
            /* region 0 or 2 do a sin calculation */
            x3 = x2 * r;
            r_sin =  r + (x3 * POLY_EVAL_3(x2, S1, S2, S3, S4));
        }

        int32_t region_sin = region;
        int32_t region_cos = region;

        region_sin >>= 1;
        region_cos += 1;

        if(((sign & (uint32_t)region_sin) |
                         ((~sign) & (uint32_t)(~region_sin))) & 1) {

            *sin = (float)r_sin;

        }
        else {

            *sin = (float)(-r_sin);
        }

        if(region_cos & 2) {
            /* If region is 2 or 3, sign is -ve */
            *cos = (float)-r_cos;
        }
        else {
            *cos = (float)(r_cos);
        }
        return;
    }
    else if(uxf >= SIN_SMALLER) {

        if(uxf >= SIN_SMALL) {
            /*cos calculation*/
             /* r = 0.5 * x2 */
            x2 = x * x;
            r = 0.5 * x2;
            t = 1 - r;
            /* cos(x) = t + ((1.0 - t) - r) + (x2 * (x2 * C1 + C2 * x2 + C3 * x2
             *          + C4 * x2 ))
             */
            s = t + ((1.0 - t) - r);
            *cos = (float)(s + (x2 * (x2 * POLY_EVAL_4(x2, C1, C2, C3, C4))));

            /*sin calculation*/
            /* x > 2.0^(-13) */
            xd = (double)x;
            x2 = xd * xd;
            *sin = (float)(xd + (xd * (x2 * POLY_EVAL_3(x2, S1, S2, S3, S4))));
            return;
        }

        *cos = 1.0f - (x * x * 0.5f);
        *sin = x - x * x * x * ONE_BY_SIX;
        return;
    }

    *cos = 1.0f;
    *sin = x;

}
