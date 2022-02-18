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
 * hypot(a,b) = sqrt(a^2 + b^2)
 *
 * IEEE SPEC:
 * if one of the arguments is ±∞, hypot returns +∞ even if the other argument is NaN
 * otherwise, if any of the arguments is NaN, NaN is returned
 *
 * Algorithm:
 *
 * Returns sqrt(x*x + y*y) with no overflow or underflow unless 
 * the result warrants it.
 * Multiplication of squares x and y are to be done in higher precision.
 * x and y are split into (xh,xl) and (yh,yt)
 *
 * xh and yh contain only upto the fist 25 bits of precision of x and y respectively
 *
 * head = x^2 + y^2
 *
 * tail = ((x*x - head) + y*y) + (((xh * xh - x*x) + xl*xl + 2*xl*xh) + (yh * yh - y*y) + yl*yl + 2*yh*yl)
 *
 * hypot(x,y) = sqrtd(head+tail)
 *
 */

#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>

#define SIX_HUNDRED  0x2580000000000000
#define MASL_LAST_27 0xfffffffff8000000

double ALM_PROTO_OPT(hypot)(double x, double y) {

    const double large = 1.79769313486231570815e+308; /* 0x7fefffffffffffff */

    double u, r, retval, hx, tx, x2, hy, ty, y2, hs, ts;

    uint64_t xexp, yexp, ux, uy;

    int32_t dexp, expadjust,x_is_nan,y_is_nan;

    uint64_t val;

    ux = asuint64(x);

    ux &= ~SIGNBIT_DP64;

    uy = asuint64(y);

    uy &= ~SIGNBIT_DP64;

    xexp = (ux >> EXPSHIFTBITS_DP64);

    yexp = (uy >> EXPSHIFTBITS_DP64);

    x_is_nan = (ux > POS_INF_F64);

    y_is_nan = (uy > POS_INF_F64);

    val = PINFBITPATT_DP64;

    if (ux >= POS_INF_F64 || uy >= POS_INF_F64) {

        if ((ux == POS_INF_F64) || (uy == POS_INF_F64)) {

            if(x_is_nan) {
            #ifdef WINDOWS
                return __alm_handle_error(asuint64(x), 0);
            #else
                if(!(ux & QNAN_MASK_64)) //x is snan
                    return __alm_handle_error(asuint64(x), AMD_F_INVALID);
            #endif
            }
            if(y_is_nan) {
            #ifdef WINDOWS
                return __alm_handle_error(asuint64(y), 0);
            #else
                if(!(uy & QNAN_MASK_64)) //y is snan
                    return __alm_handle_error(asuint64(y), AMD_F_INVALID);
            #endif
            }
            return asdouble(val);;
        }

        /* Both x and y is NaN, and neither is infinity.
           Raise invalid if it's a signalling NaN */
        #ifdef WINDOWS
        return __alm_handle_error(asuint64(x) | QNAN_MASK_64, 0);
        #else
        if(!(ux & QNAN_MASK_64)) //x is snan
            return __alm_handle_error(asuint64(x) | QNAN_MASK_64, AMD_F_INVALID);
        #endif

    }

    /* Set x = abs(x) and y = abs(y) */

    x = asdouble(ux);

    y = asdouble(uy);

    /* The difference in exponents between x and y */
    dexp = (int32_t)(xexp - yexp);

    expadjust = 0;

    if (ux == 0)
        return y;
    else if (uy == 0)
        return x;
    else if (dexp > MANTLENGTH_DP64 + 1 || dexp < -MANTLENGTH_DP64 - 1)
    /* One of x and y is insignificant compared to the other */
        return x + y; /* Raise inexact */
    else if (xexp > EXPBIAS_DP64 + 500 || yexp > EXPBIAS_DP64 + 500) {

      /* Danger of overflow; scale down by 2**600. */
        expadjust = 600;

        ux -= SIX_HUNDRED;

        x = asdouble(ux);

        uy -= SIX_HUNDRED;

        y = asdouble(uy);
    }
    else if (xexp < EXPBIAS_DP64 - 500 || yexp < EXPBIAS_DP64 - 500) {
      /* Danger of underflow; scale up by 2**600. */
        expadjust = -600;

        if (xexp == 0) {
            /*x is denormal - handle by adding 601 to the exponent
              and then subtracting a correction for the implicit bit */
            x = asdouble(ux + SIX_HUNDRED);

            x -= 9.23297861778573578076e-128; /* 0x2590000000000000 */

            ux = asuint64(x);
        }
        else {
            /* x is normal - just increase the exponent by 600 */
            ux += SIX_HUNDRED;

            x = asdouble(ux);
        }
        if (yexp == 0) {

            y = asdouble(uy + 0x2590000000000000); /*9.23297861778573578076e-128*/

            y -= 9.23297861778573578076e-128; /* 0x2590000000000000 */

            uy = asuint64(y);
        }
        else {

            uy += SIX_HUNDRED;

            y = asdouble(uy);
        }
    }


#ifdef FAST_BUT_GREATER_THAN_ONE_ULP
    /* Not awful, but results in accuracy loss larger than 1 ulp */
    r = x * x + y * y;
#else
    /* Slower but more accurate */

    /* Sort so that x is greater than y */
    if (x < y) {

        u = y;

        y = x;

        x = u;

        ux = asuint64(x);

        uy = asuint64(y);

    }

    /* Split x into hx and tx, head and tail */
    hx = asdouble(ux & MASL_LAST_27);

    tx = x - hx;

    hy = asdouble(uy & MASL_LAST_27);

    ty = y - hy;

    /* Compute r = x*x + y*y with extra precision */
    x2 = x * x;

    y2 = y * y;

    hs = x2 + y2;

    if (dexp == 0) {
        /* We take most care when x and y have equal exponents,
           i.e. are almost the same size */
        ts = (((x2 - hs) + y2) + ((hx * hx - x2) + 2 * hx * tx) + tx * tx) +
             ((hy * hy - y2) + 2 * hy * ty) + ty * ty;

    }
    else {

        ts = (((x2 - hs) + y2) + ((hx * hx - x2) + 2 * hx * tx) + tx * tx);
    }

    r = hs + ts;
#endif

    retval = sqrt(r);

  /* If necessary scale the result back. This may lead to
     overflow but if so that's the correct result. */
    retval = scaleDouble_1(retval, expadjust);

    if (retval > large) {
    /* The result overflowed. Deal with errno. */
        return __alm_handle_error(PINFBITPATT_DP64, AMD_F_INEXACT|AMD_F_OVERFLOW);

    }
    else if((x != 0.0) && (y != 0.0)) {

        ux = asuint64(retval);

        ux >>= EXPSHIFTBITS_DP64;

    }
    if(ux == 0x0) {

        return __alm_handle_error(asuint64(retval), AMD_F_INEXACT|AMD_F_UNDERFLOW);

    }

    return retval;
}

