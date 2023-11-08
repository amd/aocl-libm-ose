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
 *
 * Signature: double atan2(double, double)
 *
 * atan2 computes the arc tangent of y/x and returns the result in radians.
 *
 * IEEE SPEC:
 *     if both the arguments are 0, domain errror is returned
 *     If x and y are both zero, domain error does not occur
 *     If x and y are both zero, range error does not occur either
 *     If y is zero, pole error does not occur
 *     If y is ±0 and x is negative or -0, ±π is returned
 *     If y is ±0 and x is positive or +0, ±0 is returned
 *     If y is ±∞ and x is finite, ±π/2 is returned
 *     If y is ±∞ and x is -∞, ±3π/4 is returned
 *     If y is ±∞ and x is +∞, ±π/4 is returned
 *     If x is ±0 and y is negative, -π/2 is returned
 *     If x is ±0 and y is positive, +π/2 is returned
 *     If x is -∞ and y is finite and positive, +π is returned
 *     If x is -∞ and y is finite and negative, -π is returned
 *     If x is +∞ and y is finite and positive, +0 is returned
 *     If x is +∞ and y is finite and negative, -0 is returned
 *     If either x is NaN or y is NaN, NaN is returned
 *
 *     Algorithm:
 *     1.  Let u = |x|, v = |y|
 *     2.  Let 0 < u < v
 *     3.  Let z = v/u
 *     4.  index = ceil(z * 256)
 *     5.  q = atan2(index-/256) where j varied from 16 to 256 is computed.
 *     6.  Let c = index * 256
 *     7.  Let m be the ubiased exponent of u
 *     8.  Scale u and v by 2^(-m/2)
 *     9.  Split u into head and tail by u1 and u2
 *     10. r = ((v - c * u1) - c * u2) / (u + c * v);
 *     11. Aproximate atan(r) using a polynomial
 *     12  q =  q + atan2(r)
 *     13. if(u > v) then q = pi/2 - q
 *     14. if(x < 0) then q = pi - q
 *     15. if(y < 0) then q = -q
 *
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include "libm_inlines_amd.h"

#include "atan2_data.h"
static struct {
    double pi, piby2, piby4, three_piby4;
    double pi_head, pi_tail, piby2_head, piby2_tail;
    double small_threshold, small_threshold1;
    double poly;
    double ALIGN(16) poly1[2];
    double ALIGN(16) poly2[5];
} atan2_data = {

              .pi = 3.1415926535897932e+00, /* 0x400921fb54442d18 */
              .piby2 = 1.5707963267948966e+00, /* 0x3ff921fb54442d18 */
              .piby4 = 7.8539816339744831e-01, /* 0x3fe921fb54442d18 */
              .three_piby4 = 2.3561944901923449e+00, /* 0x4002d97c7f3321d2 */
              .pi_head = 3.1415926218032836e+00, /* 0x400921fb50000000 */
              .pi_tail = 3.1786509547056392e-08, /* 0x3e6110b4611a6263 */
              .piby2_head = 1.5707963267948965e+00, /* 0x3ff921fb54442d18 */
              .piby2_tail = 6.1232339957367660e-17, /* 0x3c91a62633145c07 */
              .small_threshold = 0.0625,
              .small_threshold1 = 1.e-8,

              /*
               * Polynomial constants, 1/x! (reciprocal x)
               * To make better use of cache line,
               * we dont store 0! and 1!
               */
              .poly1 = {
                  0.33333333333224095522,
                  0.19999918038989143496,

              },
              .poly2 = { /* skip for 0/1 and 1/1 */
                       0.33333333333333170500,
                       0.19999999999393223405,    /* 1/3 */
                       0.14285713561807169030,    /* 1/4 */
                       0.11110736283514525407,    /* 1/5 */
                       0.90029810285449784439E-01,    /* 1/6 */
              },
};

#define SMALL_THRESHOLD1 atan2_data.small_threshold
#define SMALL_THRESHOLD2 atan2_data.small_threshold1
#define PI               atan2_data.pi
#define PI_HEAD          atan2_data.pi_head
#define PI_TAIL          atan2_data.pi_tail
#define PI_BY_2          atan2_data.piby2
#define PI_BY_2_HEAD     atan2_data.piby2_head
#define PI_BY_2_TAIL     atan2_data.piby2_tail
#define PI_BY_4          atan2_data.piby4
#define THREE_PI_BY_4    atan2_data.three_piby4
#define C0               atan2_data.poly1[0]
#define C1               atan2_data.poly1[1]

#define D0 atan2_data.poly2[0]
#define D1 atan2_data.poly2[1]
#define D2 atan2_data.poly2[2]
#define D3 atan2_data.poly2[3]
#define D4 atan2_data.poly2[4]
#define LEADING_26BITS 0xfffffffff8000000
#define MANT_MASK_20BITS 0xffffffff00000000
#define ATAN_TABLE atan_jby256

double ALM_PROTO_OPT(atan2)(double y, double x) {

    double u, v, vbyu, q1, q2, s, u1, u2, uu, c, r;
    double vu1, vu2, poly;
    uint32_t swap_vu, index;
    int32_t m, xexp, yexp, diffexp;
    uint64_t ux, ui, aux, xneg, uy, auy, yneg;

    ux = asuint64(x);

    uy = asuint64(y);

    aux = ux & ~SIGNBIT_DP64;

    auy = uy & ~SIGNBIT_DP64;

    xexp = (int32_t)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);

    yexp = (int32_t)((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);

    xneg = ux & SIGNBIT_DP64;

    yneg = uy & SIGNBIT_DP64;

    diffexp = yexp - xexp;

    if ((aux >= PINFBITPATT_DP64) || (auy >= PINFBITPATT_DP64)) {

        if((aux == PINFBITPATT_DP64) && (auy == PINFBITPATT_DP64)) {

            if(xneg) {

                return asdouble(yneg | asuint64(THREE_PI_BY_4)); // return val_with_flags(-three_piby4,AMD_F_INEXACT);

            }

            return asdouble(yneg | asuint64(PI_BY_4)); // return val_with_flags(-piby4,AMD_F_INEXACT);
        }

        if(aux > PINFBITPATT_DP64) {

            return x + x; /* Raise invalid if it's a signalling NaN */

        }

        if(auy > PINFBITPATT_DP64) {

            return y + y; /* Raise invalid if it's a signalling NaN */

        }

        if(ux == PINFBITPATT_DP64) { /* x is +infinity, y is finite */

            return asdouble(yneg | 0);

        }

    }
    else if (auy == 0) {
        /* Zero y gives +-0 for positive x
         and +-pi for negative x */
        if (xneg) {

            return asdouble(yneg | asuint64(PI));//return val_with_flags(-pi,AMD_F_INEXACT);

        }
        else return y;
    }
    else if (aux == 0) {
        /* Zero x gives +- pi/2
         depending on sign of y */
        return asdouble(yneg | asuint64(PI_BY_2));//val_with_flags(-piby2,AMD_F_INEXACT);

    }

  /* Scale up both x and y if they are both below 1/4.
     This avoids any possible later denormalised arithmetic. */

    if ((xexp < 1021 && yexp < 1021)) {

        scaleUpDouble1024(ux, &ux);

        scaleUpDouble1024(uy, &uy);

        x = asdouble(ux);

        y = asdouble(uy);

        xexp = (int32_t)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);

        yexp = (int32_t)((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);

        diffexp = yexp - xexp;
    }

    if (diffexp > 56) {
    /* abs(y)/abs(x) > 2^56 => arctan(x/y)
       is insignificant compared to piby2 */
        return asdouble(yneg | asuint64(PI_BY_2));

    }
    else if (diffexp < -28 && (!xneg)) {
       /* x positive and dominant over y by a factor of 2^28.
               In this case atan(y/x) is y/x to machine accuracy. */

        if (diffexp < -1074) {/* Result underflows */

            return __alm_handle_error(yneg | 0, AMD_F_INEXACT | AMD_F_UNDERFLOW);

        }
        else {
            if (diffexp < -1022) {
              /* Result will likely be denormalized */
                y = scaleDouble_1(y, 100);
                y /= x;
                /* Now y is 2^100 times the true result. Scale it back down. */
                uy = asuint64(y);

                scaleDownDouble(uy, 100, &uy);

                y = asdouble(uy);

                if ((uy & EXPBITS_DP64) == 0)
                    return  __alm_handle_error(uy, AMD_F_INEXACT | AMD_F_UNDERFLOW);
             }
             else
                 return y / x;
        }
    }
    else if (diffexp < -56 && xneg) {
        /* abs(x)/abs(y) > 2^56 and x < 0 => arctan(y/x)
         is insignificant compared to pi */
        return asdouble(yneg | asuint64(PI));

    }

    /* General case: take absolute values of arguments */

    u = x;
    v = y;
    
    if (xneg) u = -x;
    if (yneg) v = -y;

    /* Swap u and v if necessary to obtain 0 < v < u. Compute v/u. */

    swap_vu = (u < v);

    if (swap_vu) { uu = u; u = v; v = uu; }

    vbyu = v / u;

    if (vbyu > SMALL_THRESHOLD1) {
        /* General values of v/u. Use a look-up
           table and series expansion. */

        index = (uint32_t)(256 * vbyu + 0.5);

        q1 = ATAN_TABLE[index - 16].head;

        q2 = ATAN_TABLE[index - 16].tail;

        c = index * 1.0 / 256.0;

        ui = asuint64(u);

        m = (int32_t)((ui & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;

        u = scaleDouble_2(u, -m);

        v = scaleDouble_2(v, -m);

        ui = asuint64(u);

        u1 = asdouble(LEADING_26BITS & ui); /* 26 leading bits of u */

        u2 = u - u1;

        r = ((v - c * u1) - c * u2) / (u + c * v);

      /* Polynomial approximation to atan(r) */

        s = r * r;

        q2 = q2 + r - r * (s * (C0 - s * C1));

    }
    else if (vbyu < SMALL_THRESHOLD2) {
        /* v/u is small enough that atan(v/u) = v/u */
        q1 = 0.0;

        q2 = vbyu;

    }
    else {  /* vbyu <= 0.0625 */

      /* Small values of v/u. Use a series expansion
         computed carefully to minimise cancellation */

        ui = asuint64(u);

        u1 = asdouble(MANT_MASK_20BITS & ui);

        ui = asuint64(vbyu);

        vu1 = asdouble(MANT_MASK_20BITS & ui);

        u2 = u - u1;

        vu2 = vbyu - vu1;

        q1 = 0.0;

        s  = vbyu * vbyu;

        poly = vbyu * s * (D0 - s * (D1 - s * (D2 - s * (D3 - s * D4))));

        q2 = vbyu + ((((v - u1 * vu1) - u2 * vu1) - u * vu2) / u) - poly;
    }

  /* Tidy-up according to which quadrant the arguments lie in */

    if (swap_vu) {q1 = PI_BY_2_HEAD - q1; q2 = PI_BY_2_TAIL - q2;}

    if (xneg) {

        q1 = PI_HEAD - q1;

        q2 = PI_TAIL - q2;

    }

    q1 = q1 + q2;

    return asdouble(yneg | asuint64(q1));

}


