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


/* tan(x) approximation valid on the interval [-pi/4,pi/4].
   If recip is true return -1/tan(x) instead. */
static inline double tanf_piby4(double x, int recip)
{
  double r, t;

  /* Core Remez [1,2] approximation to tan(x) on the
     interval [0,pi/4]. */
  r = x*x;
  t = x + x*r*
    (0.385296071263995406715129e0 -
     0.172032480471481694693109e-1 * r) /
    (0.115588821434688393452299e+1 +
     (-0.51396505478854532132342e0 +
      0.1844239256901656082986661e-1 * r) * r);

  if (recip)
    return -1.0 / t;
  else
    return t;
}


float  FN_PROTOTYPE_REF(tanpif)(float x)
{
    const float pi = 3.1415926535897932F;
    unsigned long long uxx;
    float xsgn = x > 0.0f ? 1.0f : -1.0f;
    unsigned int ux;
    double dx;
    float r, xodd;


    GET_BITS_SP32(x, ux);
    ux = ux & ~SIGNBIT_SP32;

    if (ux >= PINFBITPATT_SP32)
    {
        PUT_BITS_SP32(QNANBITPATT_SP32,x);
        return x;
    }

    // when x >= 2^24, the result is always even integer
    if (ux >= 0x4b800000)
	return xsgn * 0.0f;

    // when x >= 2^23, the result is always integer
    // Given the previous test, we also know the last bits of ux are the same
    // as the last bits of (int)ux
    if (ux >= 0x4b000000)
	return xsgn * (ux & 0x1 ? -0.0f : 0.0f);

    dx = x * xsgn;
    GET_BITS_DP64(dx, uxx);

    if (dx < 0.25) {
        if (uxx < 0x3f80000000000000) {
	    if (uxx < 0x3f20000000000000)
	        return x * pi;
	    dx = x * pi;
	    return (float)(dx + dx*dx*dx*(1.0 / 3.0));
        }
	return (float)tanf_piby4(x*pi, 0);
    }

    ux = (unsigned int)dx;
    r = (float)(dx - (double)ux);
    xodd = xsgn * (ux & 0x1 ? -1.0f : 1.0f);

    if (r == 0.0f)
	return xodd * 0.0f;

    double xdsgn = (double)xsgn;

    if (r <= 0.25f)
	return (float)(xdsgn * tanf_piby4(r*pi, 0));

    if (r < 0.5f)
	return (float)(-xdsgn * tanf_piby4((0.5f - r) * pi, 1));

    if (r == 0.5f)
    {
        PUT_BITS_SP32(PINFBITPATT_SP32,x);
        return xodd * x;
    }

    if (r <= 0.75f)
	return (float)(xdsgn * tanf_piby4((r - 0.5f) * pi, 1));

    // r < 1
    return (float)(-xdsgn * tanf_piby4((1.0f - r) * pi, 0));
}

