/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
#include "libm_inlines_amd.h"
#include "libm_special.h"
#include <libm/amd_funcs_internal.h>


// tan(x + xx) approximation valid on the interval [-pi/4,pi/4].
// If recip is true return -1/tan(x + xx) instead.
static inline double tan_piby4(double x, double xx, int recip)
{
  double r, t1, t2, xl;
  int transform = 0;
  static const double
     piby4_lead = 7.85398163397448278999e-01, /* 0x3fe921fb54442d18 */
     piby4_tail = 3.06161699786838240164e-17; /* 0x3c81a62633145c06 */

  /* In order to maintain relative precision transform using the identity:
     tan(pi/4-x) = (1-tan(x))/(1+tan(x)) for arguments close to pi/4.
     Similarly use tan(x-pi/4) = (tan(x)-1)/(tan(x)+1) close to -pi/4. */

  if (x > 0.68)
    {
      transform = 1;
      x = piby4_lead - x;
      xl = piby4_tail - xx;
      x += xl;
      xx = 0.0;
    }
  else if (x < -0.68)
    {
      transform = -1;
      x = piby4_lead + x;
      xl = piby4_tail + xx;
      x += xl;
      xx = 0.0;
    }

  /* Core Remez [2,3] approximation to tan(x+xx) on the
     interval [0,0.68]. */

  r = x*x + 2.0 * x * xx;
  t1 = x;
  t2 = xx + x*r*
    (0.372379159759792203640806338901e0 +
     (-0.229345080057565662883358588111e-1 +
      0.224044448537022097264602535574e-3*r)*r)/
    (0.111713747927937668539901657944e1 +
     (-0.515658515729031149329237816945e0 +
      (0.260656620398645407524064091208e-1 -
       0.232371494088563558304549252913e-3*r)*r)*r);

  /* Reconstruct tan(x) in the transformed case. */

  if (transform)
    {
      double t;
      t = t1 + t2;
      if (recip)
         return transform*(2*t/(t-1) - 1.0);
      else
         return transform*(1.0 - 2*t/(1+t));
    }

  if (recip)
    {
      /* Compute -1.0/(t1 + t2) accurately */
      double trec, trec_top, z1, z2, t;
      unsigned long long u;
      t = t1 + t2;
      GET_BITS_DP64(t, u);
      u &= 0xffffffff00000000;
      PUT_BITS_DP64(u, z1);
      z2 = t2 - (z1 - t1);
      trec = -1.0 / t;
      GET_BITS_DP64(trec, u);
      u &= 0xffffffff00000000;
      PUT_BITS_DP64(u, trec_top);
      return trec_top + trec * ((1.0 + trec_top * z1) + trec_top * z2);

    }
  else
    return t1 + t2;
}



double ALM_PROTO_REF(tanpi)(double x)
{
    double r, dx, xsgn, xodd;
    unsigned long long ux;
    const double pi = 3.1415926535897932384626433832795;

    xsgn = x > 0.0 ? 1.0 : -1.0;
    GET_BITS_DP64(x, ux);
    ux = ux & ~SIGNBIT_DP64;

    // Handle +-Inf and NaN
    if (ux >= PINFBITPATT_DP64)
    {
		PUT_BITS_DP64(INDEFBITPATT_DP64,x);
		return x;
	}

    // If x >= 2^53, it is an even integer
    if (ux > 0x433fffffffffffffL)
	return xsgn * 0.0;

    // If x >= 2^52, it is an integer
    // Given the previous test, we also know that the last bits of ux are the
    // same as the last bits of (long)ux
    if (ux > 0x432fffffffffffffL)
	return xsgn * (ux & 0x1L ? -0.0 : 0.0);

    dx = x * xsgn;

    GET_BITS_DP64(dx, ux);
    // Take care of small arguments
    if (dx <= 0.25) {
        if (ux < 0x3f10000000000000) {
	    if (ux < 0x3e40000000000000)
		return x * pi;
	    dx = x * pi;
	    return dx + dx*dx*dx*(1.0 / 3.0);
	}
	return tan_piby4(x*pi, 0.0, 0);
    }

    ux = (unsigned long long)dx;
    r = dx - (double)ux;
    xodd = xsgn * (ux & 0x1 ? -1.0 : 1.0);

    if (r == 0.0)
	return xodd * 0.0;

    if (r <= 0.25)
	return xsgn * tan_piby4(r*pi, 0.0, 0);

    if (r < 0.5)
	return -xsgn * tan_piby4((0.5 - r)*pi, 0.0, 1);

    if (r == 0.5)
    {
		PUT_BITS_DP64(PINFBITPATT_DP64,x);
		return xodd * x;
	}

    if (r <= 0.75)
	return xsgn * tan_piby4((r - 0.5)*pi, 0.0, 1);

    // r < 1
    return -xsgn * tan_piby4((1.0 - r)*pi, 0.0, 0);
}

