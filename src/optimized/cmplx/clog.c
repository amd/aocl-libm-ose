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
 *
 * Implementation Notes
 * --------------------
 * Signature:
 *   double complex clog(double complex x)
 *
 *   High ULP errors for values where absolute value of the complex input is close to 1.
 *
 *   IEEE SPEC:
 *
 *   If z = -0 + 0i, the result is -INFINITY + πi and FE_DIVBYZERO is raised
 *   If z =  0 + 0i, the result is -INFINITY + 0i and FE_DIVBYZERO is raised
 *   IF z =  x + INFINITYi (for any finite x), the result is INFINITY + πi/2
 *   If z =  x + NaNi (for any finite x), the result is NaN + NaNi and FE_INVALID may be raised
 *   If z = -INFINITY + yi (for any finite positive y), the result is INFINITY + πi
 *   If z = +INFINITY + yi (for any finite positive y), the result is INFINITY + 0i
 *   If z = -INFINITY + INFINITYi, the result is INFINITY + 3πi/4
 *   If z = INFINITY + INFINITYi, the result is INFINITY + πi/4
 *   If z = +/-INFINITY + NaNi, the result is INFINITY + NaNi
 *   If z = NaN + yi (for any finite y), the result is NaN + NaNi and FE_INVALID may be raised
 *   If z = NaN + INFINITYi, the result is INFINITY + NaNi
 *   If z = NaN + NaNi, the result is NaN + NaNi
 *
 *   Algorithm
 *
 *   Let x = a + I*b
 *
 *   x in polar form = r * e^(i*theta)
 *
 *   r is the magnitude or the absolute value or x.
 *
 *   theta = atan2(y/x)
 *
 *   log(x) = log(r) + (I * theta)
 *
 *
 */
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <math.h>

fc64_t
ALM_PROTO_OPT(clog)(fc64_t x) {

    double theta, p, a, b, ah, al, bh, bl;

    double asquare, bsquare, abs_h, abs_t;

    a = creal(x);

    b = cimag(x);

    if(a < b) {

        double t = a;
        a = b;
        b = t;

    }

    /*FIRST split a and b */

    ah = asdouble(asuint64(a) & 0xfffffffff8000000UL);

    al = a - ah;

    bh = asdouble(asuint64(b) & 0xfffffffff8000000UL);

    bl = b - bh;

    /*compute a * a + b * b using high precision two double word numbers */

    asquare = a * a;

    bsquare = b * b;

    abs_h = asquare + bsquare;

    abs_t = (((asquare - abs_h) + bsquare) + ((ah * ah - asquare) + 2 * ah * al) + al * al) +
            ((bh * bh - bsquare) + 2 * bh * bl) + bl * bl;

    p= 0.5 * log(abs_h + abs_t);

    theta = atan2(cimag(x), creal(x));

    return CMPLX(p, theta);

}

