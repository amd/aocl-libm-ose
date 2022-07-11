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
/* Contains implementation of float sinh( x)
 * After dealing with special cases the computation is split into regions as follows:

 *  abs(x) >= max_sinh_arg:
 *  sinh(x) = sign(x)*Inf

 *  abs(x) >= small_threshold:
 *  sinh(x) = sign(x)*exp(abs(x))/2 computed using the
 *  splitexp and scaleDouble functions as for exp_amd().

 *  abs(x) < small_threshold:
 *  compute p = exp(y) - 1 and then z = 0.5*(p+(p/(p+1.0)))
 *  sinh(x) is then sign(x)*z.

 * Tabulated values of sinh(i) and cosh(i) for i = 0,...,36 is used
 */
#include <libm_util_amd.h>
#include "libm_inlines_amd.h"
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>

extern double sinh_head[], cosh_head[], sinh_tail[], cosh_tail[];

static struct {
    double max_sinh_arg, thirtytwo_by_log2, log2_by_32_lead, log2_by_32_tail, small_threshold;
    } sinh_data = {
        .max_sinh_arg = 7.10475860073943977113e+02, /* 0x408633ce8fb9f87e */
        .thirtytwo_by_log2 = 4.61662413084468283841e+01, /* 0x40471547652b82fe */
        .log2_by_32_lead = 2.16608493356034159660e-02, /* 0x3f962e42fe000000 */
        .log2_by_32_tail = 5.68948749532545630390e-11, /* 0x3dcf473de6af278e */
        .small_threshold = 8 * BASEDIGITS_DP64 * 0.30102999566398119521373889,
    };

#define SDY0 0.166666666666666667013899e0
#define SDY1 0.833333333333329931873097e-2
#define SDY2 0.198412698413242405162014e-3
#define SDY3 0.275573191913636406057211e-5
#define SDY4 0.250521176994133472333666e-7
#define SDY5 0.160576793121939886190847e-9
#define SDY6 0.7746188980094184251527126e-12

#define CDY0 0.500000000000000005911074e0
#define CDY1 0.416666666666660876512776e-1
#define CDY2 0.138888888889814854814536e-2
#define CDY3 0.248015872460622433115785e-4
#define CDY4 0.275573350756016588011357e-6
#define CDY5 0.208744349831471353536305e-8
#define CDY6 0.1163921388172173692062032e-10

#ifdef WINDOWS
#pragma function(sinh)
#endif

double ALM_PROTO_OPT(sinh)(double x)
{
  unsigned long long  aux, xneg;
  double y, z;

  /* Special cases */
  uint64_t ux = asuint64(x);
  aux = ux & ~SIGNBIT_DP64;

  if (aux < 0x3e30000000000000) {/* |x| small enough that sinh(x) = x */
        if (aux == 0x0000000000000000) {
            /* x is +/-zero. Return the same zero. */
            return x;
        } else {
#ifdef WINDOWS
            return x;
#else
            return __alm_handle_error(ux, AMD_F_INEXACT | AMD_F_UNDERFLOW);
#endif
        }
    }else if (aux == 0x7ff0000000000000) {/* |x| is NaN or Inf */
        return x;
    }else if (aux > 0x7ff0000000000000){
#ifdef WINDOWS
        return __alm_handle_error(ux | QNANBITPATT_DP64, AMD_F_NONE);
#else
        return x + x;
#endif
   }
  xneg = (aux != ux);
  y = x;
  if (xneg) y = -x;

  if (y >= sinh_data.max_sinh_arg) {
      /* Return infinity with overflow flag. */
      if (xneg)
        return __alm_handle_error(NINFBITPATT_DP64, AMD_F_OVERFLOW);
      else
        return __alm_handle_error(PINFBITPATT_DP64, AMD_F_OVERFLOW);
  } else if (y >= sinh_data.small_threshold) {
      /* In this range y is large enough so that
         the negative exponential is negligible,
         so sinh(y) is approximated by sign(x)*exp(y)/2. The
         code below is an inlined version of that from
         exp() with two changes (it operates on
         y instead of x, and the division by 2 is
         done by reducing m by 1). */
      int m;
      double z1, z2;
      splitexp(y, 1.0, sinh_data.thirtytwo_by_log2, sinh_data.log2_by_32_lead,
               sinh_data.log2_by_32_tail, &m, &z1, &z2);
      m -= 1;
      /* scaleDouble_1 is always safe because the argument x was
         float, rather than double */
      if (m >= EMIN_DP64 && m <= EMAX_DP64)
        z = scaleDouble_1((z1 + z2), m);
      else
        z = scaleDouble_2((z1 + z2), m);
  } else {
      /* In this range we find the integer part y0 of y
         and the increment dy = y - y0. We then compute
         z = sinh(y) = sinh(y0)cosh(dy) + cosh(y0)sinh(dy)
         where sinh(y0) and cosh(y0) are tabulated above. */
      int ind = (int)y;
      double dy = y - ind;
      double dy2 = dy * dy;
      double sdy = dy * POLY_EVAL_SINHF(dy2, SDY0, SDY1, SDY2, SDY3, SDY4, SDY5, SDY6);
      double cdy = POLY_EVAL_SINHF(dy2, CDY0, CDY1, CDY2, CDY3, CDY4, CDY5, CDY6);
      /* At this point sinh(dy) is approximated by dy + sdy.
      Shift some significant bits from dy to sdy. */

      ux = asuint64(dy);
      ux &= 0xfffffffff8000000;
	  double sdy1 = asdouble(ux);
      double sdy2 = sdy + (dy - sdy1);

      z = ((((((cosh_tail[ind] * sdy2 + sinh_tail[ind] * cdy)
	       + cosh_tail[ind] * sdy1) + sinh_tail[ind])
	     + cosh_head[ind] * sdy2) + sinh_head[ind] * cdy)
	   + cosh_head[ind] * sdy1) + sinh_head[ind];
    }

  if (xneg) z = -z;
  return z;
}