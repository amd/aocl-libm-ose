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


#include "precision.h"


#if defined(FLOAT)
#define FUNC_SINCOS alm_mp_sincosf

#elif defined(DOUBLE)
#define FUNC_SINCOS alm_mp_sincos

#else
#error
#endif

void FUNC_SINCOS(REAL x, REAL *s, REAL *c)
{
    REAL ys, yc;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTAN(xmp, params, result_sin, result_cos, result_tan, &ifail);


    MPTOD(result_sin, params, &ys, &ifail);
    MPTOD(result_cos, params, &yc, &ifail);

    *s = ys;
    *c = yc;

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);
}

void FUNC_SINCOS_ULP(REAL x, REAL s, REAL c,double   *sulps, double   *sreldiff)
{
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;
	REAL reldiff,ulps;
    REAL rel_1,ulp_1;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTAN(xmp, params, result_sin, result_cos, result_tan, &ifail);

   reldiff = MPRELDIFF(s, base, mantis, emin, emax,
                      result_sin, params,&ulps, &ifail);

   rel_1 = MPRELDIFF(c, base, mantis, emin, emax,
                      result_cos, params,&ulp_1, &ifail);

    *sreldiff =  reldiff > rel_1? reldiff : rel_1;
    *sulps =  ulps > ulp_1? ulps : ulp_1;




    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);
}
