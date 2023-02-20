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
*   double cosh(double x)
*
* Spec:
*   cosh(0)    = 1
*   cosh(inf) = NaN
*   cosh(NaN)  = NaN
*
*
********************************************
* Implementation Notes
* ---------------------
* To compute cosh(double x)
		aux = |x|
		for aux < 0x3e30000000000000, return 1.0

		Derived from sinh subroutine
		After dealing with special cases the computation is split into
		regions as follows:

		abs(x) >= max_cosh_arg:
		cosh(x) = sign(x)*Inf

		abs(x) >= small_threshold:
		cosh(x) = sign(x)*exp(abs(x))/2 computed using the
		splitexp and scaleDouble functions as for exp_amd().

		abs(x) < small_threshold:
		compute p = exp(y) - 1 and then z = 0.5*(p+(p/(p+1.0)))
		cosh(x) is then sign(x)*z.
*/


#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include "cosh_data.h"

static struct {
    const double max_cosh_arg, thirtytwo_by_log2, log2_by_32_lead, log2_by_32_tail, small_threshold;
    double poly_cosh_A[7], poly_cosh_B[7];
} cosh_data = {
    .max_cosh_arg = 7.10475860073943977113e+02, /* 0x408633ce8fb9f87e */
    .thirtytwo_by_log2 = 4.61662413084468283841e+01, /* 0x40471547652b82fe */
    .log2_by_32_lead = 2.16608493356034159660e-02, /* 0x3f962e42fe000000 */
    .log2_by_32_tail = 5.68948749532545630390e-11, /* 0x3dcf473de6af278e */
    .small_threshold = 20.0,  /* small_threshold = 8*BASEDIGITS_DP64*0.30102999566398119521373889*/
    .poly_cosh_A = {
        0.166666666666666667013899e0,
        0.833333333333329931873097e-2,
        0.198412698413242405162014e-3,
        0.275573191913636406057211e-5,
        0.250521176994133472333666e-7,
        0.160576793121939886190847e-9,
        0.7746188980094184251527126e-12,
    },
    .poly_cosh_B = {
        0.500000000000000005911074e0,
        0.416666666666660876512776e-1,
        0.138888888889814854814536e-2,
        0.248015872460622433115785e-4,
        0.275573350756016588011357e-6,
        0.208744349831471353536305e-8,
        0.1163921388172173692062032e-10,
    },
};

#define max_cosh_arg      cosh_data.max_cosh_arg
#define thirtytwo_by_log2 cosh_data.thirtytwo_by_log2
#define log2_by_32_lead   cosh_data.log2_by_32_lead
#define log2_by_32_tail   cosh_data.log2_by_32_tail
#define small_threshold   cosh_data.small_threshold

#define A1                cosh_data.poly_cosh_A[0]
#define A2                cosh_data.poly_cosh_A[1]
#define A3                cosh_data.poly_cosh_A[2]
#define A4                cosh_data.poly_cosh_A[3]
#define A5                cosh_data.poly_cosh_A[4]
#define A6                cosh_data.poly_cosh_A[5]
#define A7                cosh_data.poly_cosh_A[6]

#define B1                cosh_data.poly_cosh_B[0]
#define B2                cosh_data.poly_cosh_B[1]
#define B3                cosh_data.poly_cosh_B[2]
#define B4                cosh_data.poly_cosh_B[3]
#define B5                cosh_data.poly_cosh_B[4]
#define B6                cosh_data.poly_cosh_B[5]
#define B7                cosh_data.poly_cosh_B[6]

double ALM_PROTO_OPT(cosh)(double x) {

    uint64_t ux, aux, xneg;
    double y, z, z1, z2;
    int m;

    /* Special cases */
    ux = asuint64(x);

    aux = ux & ~SIGNBIT_DP64;

    if (aux < 0x3e30000000000000) { /* |x| small enough that cosh(x) = 1 */
        return 1.0;
    }
    else if (aux >= PINFBITPATT_DP64) { /* |x| is NaN or Inf */
        if (x != x) {                   /* if x is a NaN */
#ifdef WINDOWS
            return __alm_handle_error(ux | QNANBITPATT_DP64, AMD_F_NONE);
#else
            return x + x;
#endif
        }
        else { /* x is infinity */
            //x = asdouble(PINFBITPATT_DP64);
            return INFINITY;
        }
    }

    xneg = (aux != ux);
    y = x;

    if (xneg) {
        y = -x;
    }

    /* Return +/-infinity with overflow flag */
    if (y >= max_cosh_arg) {
        return __alm_handle_error(PINFBITPATT_DP64, AMD_F_OVERFLOW);
    }
    else if (y >= small_threshold) {
        /* In this range y is large enough so that
           the negative exponential is negligible,
           so cosh(y) is approximated by sign(x)*exp(y)/2. The
           code below is an inlined version of that from
           exp() with two changes (it operates on
           y instead of x, and the division by 2 is
           done by reducing m by 1). */
        splitexp(y, 1.0, thirtytwo_by_log2, log2_by_32_lead,
                log2_by_32_tail, &m, &z1, &z2);
        m -= 1;

        if (m >= EMIN_DP64 && m <= EMAX_DP64)
            z = scaleDouble_1((z1 + z2), m);
        else
            z = scaleDouble_2((z1 + z2), m);
    }
    else {
        /* In this range we find the integer part y0 of y
           and the increment dy = y - y0. We then compute

           z = sinh(y) = sinh(y0)cosh(dy) + cosh(y0)sinh(dy)
           z = cosh(y) = cosh(y0)cosh(dy) + sinh(y0)sinh(dy)

           where sinh(y0) and cosh(y0) are tabulated above. */
        int ind;
        double dy, dy2, sdy, cdy;
        ind = (int)y;
        dy = y - ind;
        dy2 = dy * dy;

        sdy = dy * dy2 * (POLY_EVAL_EVEN_7(dy, A1, A2, A3, A4, A5, A6, A7));
        cdy = dy2 * (POLY_EVAL_EVEN_7(dy, B1, B2, B3, B4, B5, B6, B7));

        /* At this point sinh(dy) is approximated by dy + sdy, and cosh(dy) is approximated by 1 + cdy.
        Shift some significant bits from dy to cdy. */
        z = ((((((cosh_tail[ind] * cdy + sinh_tail[ind] * sdy)
                                                        + sinh_tail[ind] * dy) + cosh_tail[ind])
                                        + cosh_lead[ind] * cdy) + sinh_lead[ind] * sdy)
                        + sinh_lead[ind] * dy) + cosh_lead[ind];
    }

    return z;
}
