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
    IMPLEMENTATION NOTES:

    The definition of tanh(x) is sinh(x)/cosh(x), which is also equivalent
    to the following three formulae:
    1.  (exp(x) - exp(-x))/(exp(x) + exp(-x))
    2.  (1 - (2/(exp(2*x) + 1 )))
    3.  (exp(2*x) - 1)/(exp(2*x) + 1)
    but computationally, some formulae are better on some ranges.
  
    When |x| is small such that tanh(x) = x, 
	    if x is 0, return x
		else raise exception underflow | inexact.
		if x is NaN return NaN.
    Use a [3,3] Remez approximation on [0,0.9].
        when y is between 0.0 and 0.9,
            return y + (A0*y^3 + A1*y^5 + A2*y^7 - A3*y^9)/
                    (B0 + B1*y^2 + B2*y^4 + B3*y^6)

       Use a [3,3] Remez approximation on [0.9,1]. 
        when y is between 0.9, 1.0, 
             return y + (C0*y^3 + C1*y^5 + C2*y^7 - C3*y^9)/
                          (D0 + D1*y^2 + D2*y^4 + D3*y^6)    
*/

#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>

static struct {
    const double thirtytwo_by_log2, log2_by_32_lead, \
                 log2_by_32_tail, large_threshold;
    double poly_tanh_A[4], poly_tanh_B[4], poly_tanh_C[4], poly_tanh_D[4];
} tanh_data = {
        .thirtytwo_by_log2 = 4.61662413084468283841e+01, /* 0x40471547652b82fe */
        .log2_by_32_lead = 2.16608493356034159660e-02,   /* 0x3f962e42fe000000 */
        .log2_by_32_tail = 5.68948749532545630390e-11,   /* 0x3dcf473de6af278e */
        .large_threshold = 20.0,                         /* 0x4034000000000000 */

        /* polynomial coefficients */
        .poly_tanh_A = {
            -0.274030424656179760118928e0,
            -0.176016349003044679402273e-1,
            -0.200047621071909498730453e-3,
            0.142077926378834722618091e-7,
        },
        .poly_tanh_B = {
            0.822091273968539282568011e0, 0.381641414288328849317962e0,
            0.201562166026937652780575e-1, 0.2091140262529164482568557e-3,
        },
        .poly_tanh_C = {
            -0.227793870659088295252442e0, -0.146173047288731678404066e-1,
            -0.165597043903549960486816e-3, 0.115475878996143396378318e-7,
        },
        .poly_tanh_D = {
            0.683381611977295894959554e0, 0.317204558977294374244770e0,
            0.167358775461896562588695e-1, 0.173076050126225961768710e-3,
        },
};

#define thirtytwo_by_log2 tanh_data.thirtytwo_by_log2
#define log2_by_32_lead   tanh_data.log2_by_32_lead
#define log2_by_32_tail   tanh_data.log2_by_32_tail
#define large_threshold   tanh_data.large_threshold

#define A0 tanh_data.poly_tanh_A[0]
#define A1 tanh_data.poly_tanh_A[1]
#define A2 tanh_data.poly_tanh_A[2]
#define A3 tanh_data.poly_tanh_A[3]

#define B0 tanh_data.poly_tanh_B[0]
#define B1 tanh_data.poly_tanh_B[1]
#define B2 tanh_data.poly_tanh_B[2]
#define B3 tanh_data.poly_tanh_B[3]

#define C0 tanh_data.poly_tanh_C[0]
#define C1 tanh_data.poly_tanh_C[1]
#define C2 tanh_data.poly_tanh_C[2]
#define C3 tanh_data.poly_tanh_C[3]

#define D0 tanh_data.poly_tanh_D[0]
#define D1 tanh_data.poly_tanh_D[1]
#define D2 tanh_data.poly_tanh_D[2]
#define D3 tanh_data.poly_tanh_D[3]

/* function body */
double
ALM_PROTO_OPT(tanh)(double x) {
    uint64_t ux, aux, xneg;
    double y, z, p, z1, z2;
    int32_t m;

    ux = asuint64(x);

    /* Special cases */
    aux = ux & ~SIGNBIT_DP64;

    /* |x| small enough that tanh(x) = x */
    if (unlikely(aux < 0x3e30000000000000)) {  
        /* if x == pos/neg zero, return x. */    
        if (aux == POS_ZERO_F64) {
            return x;
        }

        else {
#ifdef WINDOWS
            return x;
#else
            return __alm_handle_error(ux, AMD_F_INEXACT|AMD_F_UNDERFLOW);
#endif
        }
    }

    /* |x| is NaN */
    else if (unlikely(x != x)) { 
#ifdef WINDOWS
        return __alm_handle_error(ux|QNANBITPATT_DP64, AMD_F_NONE);
#else
        return x;
#endif
    }

    xneg = (aux != ux);

    y = x;

    /* if x is negative */
    if (xneg) { 
        y = -x;
    }

    if (y > large_threshold) {
        /* If x is large then exp(-x) is negligible and
        formula 1 reduces to plus or minus 1.0 */
        z = 1.0;
    }

    else if (y <= 1.0) {
        double y2 = y * y;

        if (y < 0.9) {
            /* Use a [3,3] Remez approximation on [0,0.9]. */
            z = y + POLY_EVAL_TANH(y, A0, A1, A2, A3)/     \
                        POLY_EVAL_EVEN_6(y, B0, B1, B2, B3);
        }

        /* Use a [3,3] Remez approximation on [0.9,1]. */
        else {
            z = y + POLY_EVAL_TANH(y, C0, C1, C2, C3) / 
                     (D0 + (D1 + (D2 + D3 * y2 ) * y2 ) * y2);
        }
    }

    /* Compute p = exp(2*y) + 1. The code is basically inlined
         from exp_amd. */
    else {
        splitexp(2 * y, 1.0, thirtytwo_by_log2, log2_by_32_lead,
            log2_by_32_tail, &m, &z1, &z2);
        p = scaleDouble_2(z1 + z2, m) + 1.0;

        /* Now reconstruct tanh from p. */
        z = (1.0 - 2.0/p);
    }

    /* if x is negative */
    if (xneg) {
        z = - z;
    }

    return z;
}



