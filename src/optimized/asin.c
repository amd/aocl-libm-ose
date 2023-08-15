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

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <libm/poly.h>
#include "kern/sqrt_pos.c"

static struct {
    double piby2_tail, hpiby2_head, piby2, poly_asin_num[6], poly_asin_deno[5];
    } asin_data = {
          .piby2_tail = 6.1232339957367660e-17,  /* 0x3c91a62633145c07 */
          .hpiby2_head = 7.8539816339744831e-01, /* 0x3fe921fb54442d18 */
          .piby2       = 1.5707963267948965e+00, /* 0x3ff921fb54442d18 */
          .poly_asin_num = {
              0.227485835556935010735943483075,
              -0.445017216867635649900123110649,
              0.275558175256937652532686256258,
              -0.0549989809235685841612020091328,
              0.00109242697235074662306043804220,
              0.0000482901920344786991880522822991,
          },
          .poly_asin_deno = {
              1.36491501334161032038194214209,
              -3.28431505720958658909889444194,
              2.76568859157270989520376345954,
              -0.943639137032492685763471240072,
              0.105869422087204370341222318533,
          },
};

#define PIBY2_TAIL  asin_data.piby2_tail
#define PIBY2       asin_data.piby2
#define HPIBY2_HEAD asin_data.hpiby2_head

#define C1 asin_data.poly_asin_num[0]
#define C2 asin_data.poly_asin_num[1]
#define C3 asin_data.poly_asin_num[2]
#define C4 asin_data.poly_asin_num[3]
#define C5 asin_data.poly_asin_num[4]
#define C6 asin_data.poly_asin_num[5]

#define D1 asin_data.poly_asin_deno[0]
#define D2 asin_data.poly_asin_deno[1]
#define D3 asin_data.poly_asin_deno[2]
#define D4 asin_data.poly_asin_deno[3]
#define D5 asin_data.poly_asin_deno[4]

double ALM_PROTO_OPT(asin)(double x) {
  /* Computes arcsin(x).
     The argument is first reduced by noting that arcsin(x)
     is invalid for abs(x) > 1 and arcsin(-x) = -arcsin(x).
     For denormal and small arguments arcsin(x) = x to machine
     accuracy. Remaining argument ranges are handled as follows.
     For abs(x) <= 0.5 use
     arcsin(x) = x + x^3*R(x^2)
     where R(x^2) is a rational minimax approximation to
     (arcsin(x) - x)/x^3.
     For abs(x) > 0.5 exploit the identity:
      arcsin(x) = pi/2 - 2*arcsin(sqrt(1-x)/2)
     together with the above rational approximation, and
     reconstruct the terms carefully.
    */

    double u, v, y, s = 0.0, r, poly_num, poly_deno;
    int32_t xexp, xnan, transform = 0;

    uint64_t ux, aux, xneg;

    ux = asuint64 (x);
    /* Get absolute value of input */
    aux  = (ux & ~SIGNBIT_DP64);

    xneg = (ux & SIGNBIT_DP64);
    xnan = (aux > PINFBITPATT_DP64);
    xexp = (int32_t)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;

    /* Special cases */
    if (unlikely(xnan))
        return alm_asin_special(x, ALM_E_IN_X_NAN);

    else if (xexp < -28) /* y small enough that arcsin(x) = x */
        return x;

    else if (unlikely(xexp >= 0)) { /* abs(x) >= 1.0 */
        if (x == 1.0)
            return PIBY2;
        else if (x == -1.0)
            return -(PIBY2);
        else
            return alm_asin_special(x, ALM_E_OUT_NAN);
    }

    y = x;
    if (xneg)
        y = -x;

    transform = (xexp >= -1); /* abs(x) >= 0.5 */

    if (transform) { /* Transform y into the range [0,0.5) */
        r = 0.5 * (1.0 - y);
#ifdef WINDOWS
        /* VC++ intrinsic call */
        _mm_store_sd(&s, _mm_sqrt_sd(_mm_setzero_pd(), _mm_load_sd(&r)));
#else
        /* Hammer sqrt instruction */
        //asm volatile ("sqrtsd %1, %0" : "=x" (s) : "x" (r));
        //s = sqrt(r);
        s = ALM_PROTO_KERN(sqrt)(r);

#endif
        y = s;
    }
    else
        r = y*y;

    poly_num = POLY_EVAL_6(r, C1, C2, C3, C4, C5, C6);
    poly_deno = POLY_EVAL_5(r, D1, D2, D3, D4, D5);
    u = r * poly_num / poly_deno;

    if (transform) { /* Reconstruct asin carefully in transformed region */
        double c, s1, p, q;
        uint64_t us;

        us = asuint64(s);

        s1 = asdouble(0xffffffff00000000 & us);

        c = (r - s1 * s1)/(s + s1);
        p = 2.0*s*u - (PIBY2_TAIL - 2.0*c);
        q = HPIBY2_HEAD - 2.0*s1;
        v = HPIBY2_HEAD - (p-q);
    }

    else {
#ifdef WINDOWS
      /* Use a temporary variable to prevent VC++ rearranging
            y + y*u
         into
            y * (1 + u)
         and getting an incorrectly rounded result */
      double tmp;
      tmp = y * u;
      v = y + tmp;
#else
      v = y + y*u;
#endif
    }

    if (xneg)
        return -v;
    return v;
}
