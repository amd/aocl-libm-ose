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

#include <fn_macros.h>
#include <libm/types.h>
#include <libm/entry_pt.h>
#include "entry_pt_macros.h"

LIBM_DECL_FN_MAP(cbrtf);
LIBM_DECL_FN_MAP(cbrt);

LIBM_DECL_FN_MAP(cosf);
LIBM_DECL_FN_MAP(cos);

LIBM_DECL_FN_MAP(exp10f);
LIBM_DECL_FN_MAP(exp10);

LIBM_DECL_FN_MAP(exp2f);
LIBM_DECL_FN_MAP(exp2);

LIBM_DECL_FN_MAP(expf);
LIBM_DECL_FN_MAP(exp);

LIBM_DECL_FN_MAP(expm1f);
LIBM_DECL_FN_MAP(expm1);

LIBM_DECL_FN_MAP(fmaf);
LIBM_DECL_FN_MAP(fma);

LIBM_DECL_FN_MAP(fabs);
LIBM_DECL_FN_MAP(fabsf);

LIBM_DECL_FN_MAP(log10f);
LIBM_DECL_FN_MAP(log10);

LIBM_DECL_FN_MAP(log1pf);
LIBM_DECL_FN_MAP(log1p);

LIBM_DECL_FN_MAP(log2f);
LIBM_DECL_FN_MAP(log2);

LIBM_DECL_FN_MAP(logf);
LIBM_DECL_FN_MAP(log);

LIBM_DECL_FN_MAP(powf);
LIBM_DECL_FN_MAP(pow);

LIBM_DECL_FN_MAP(sincosf);
LIBM_DECL_FN_MAP(sincos);

LIBM_DECL_FN_MAP(sinf);
LIBM_DECL_FN_MAP(sin);

LIBM_DECL_FN_MAP(acos);
LIBM_DECL_FN_MAP(acosf);

LIBM_DECL_FN_MAP(acosh);
LIBM_DECL_FN_MAP(acoshf);

LIBM_DECL_FN_MAP(asin);
LIBM_DECL_FN_MAP(asinf);

LIBM_DECL_FN_MAP(asinh);
LIBM_DECL_FN_MAP(asinhf);

LIBM_DECL_FN_MAP(atan2);
LIBM_DECL_FN_MAP(atan2f);

LIBM_DECL_FN_MAP(atan);
LIBM_DECL_FN_MAP(atanf);

LIBM_DECL_FN_MAP(atanh);
LIBM_DECL_FN_MAP(atanhf);

LIBM_DECL_FN_MAP(ceil);
LIBM_DECL_FN_MAP(ceilf);

LIBM_DECL_FN_MAP(cosh);
LIBM_DECL_FN_MAP(coshf);

LIBM_DECL_FN_MAP(cospi);
LIBM_DECL_FN_MAP(cospif);

LIBM_DECL_FN_MAP(finite);
LIBM_DECL_FN_MAP(finitef);

LIBM_DECL_FN_MAP(floor);
LIBM_DECL_FN_MAP(floorf);

LIBM_DECL_FN_MAP(frexp);
LIBM_DECL_FN_MAP(frexpf);

LIBM_DECL_FN_MAP(hypot);
LIBM_DECL_FN_MAP(hypotf);

LIBM_DECL_FN_MAP(ilogb);
LIBM_DECL_FN_MAP(ilogbf);

LIBM_DECL_FN_MAP(ldexp);
LIBM_DECL_FN_MAP(ldexpf);

LIBM_DECL_FN_MAP(llrint);
LIBM_DECL_FN_MAP(llrintf);

LIBM_DECL_FN_MAP(llround);
LIBM_DECL_FN_MAP(llroundf);

LIBM_DECL_FN_MAP(logb);
LIBM_DECL_FN_MAP(logbf);

LIBM_DECL_FN_MAP(lrint);
LIBM_DECL_FN_MAP(lrintf);

LIBM_DECL_FN_MAP(lround);
LIBM_DECL_FN_MAP(lroundf);

LIBM_DECL_FN_MAP(modf);
LIBM_DECL_FN_MAP(modff);

LIBM_DECL_FN_MAP(nan);
LIBM_DECL_FN_MAP(nanf);

LIBM_DECL_FN_MAP(nextafter);
LIBM_DECL_FN_MAP(nextafterf);

LIBM_DECL_FN_MAP(nexttoward);
LIBM_DECL_FN_MAP(nexttowardf);

LIBM_DECL_FN_MAP(remainder_piby2);
LIBM_DECL_FN_MAP(remainder_piby2d2f);

LIBM_DECL_FN_MAP(remquo);
LIBM_DECL_FN_MAP(remquof);

LIBM_DECL_FN_MAP(rint);
LIBM_DECL_FN_MAP(rintf);

//LIBM_DECL_FN_MAP(round);
LIBM_DECL_FN_MAP(roundf);

LIBM_DECL_FN_MAP(scalbln);
LIBM_DECL_FN_MAP(scalblnf);

LIBM_DECL_FN_MAP(scalbn);
LIBM_DECL_FN_MAP(scalbnf);

LIBM_DECL_FN_MAP(sinh);
LIBM_DECL_FN_MAP(sinhf);

LIBM_DECL_FN_MAP(sinpi);
LIBM_DECL_FN_MAP(sinpif);

LIBM_DECL_FN_MAP(sqrt);
LIBM_DECL_FN_MAP(sqrtf);

LIBM_DECL_FN_MAP(tan);
LIBM_DECL_FN_MAP(tanf);

LIBM_DECL_FN_MAP(tanh);
LIBM_DECL_FN_MAP(tanhf);

LIBM_DECL_FN_MAP(tanpi);
LIBM_DECL_FN_MAP(tanpif);

LIBM_DECL_FN_MAP(trunc);
LIBM_DECL_FN_MAP(truncf);

LIBM_DECL_FN_MAP(copysign);
LIBM_DECL_FN_MAP(copysignf);

LIBM_DECL_FN_MAP(fdim);
LIBM_DECL_FN_MAP(fdimf);

LIBM_DECL_FN_MAP(fmax);
LIBM_DECL_FN_MAP(fmaxf);

LIBM_DECL_FN_MAP(fmin);
LIBM_DECL_FN_MAP(fminf);

LIBM_DECL_FN_MAP(fmod);
LIBM_DECL_FN_MAP(fmodf);

LIBM_DECL_FN_MAP(remainder);
LIBM_DECL_FN_MAP(remainderf);

LIBM_DECL_FN_MAP(nearbyint);
LIBM_DECL_FN_MAP(nearbyintf);

LIBM_DECL_FN_MAP(vrd2_cbrt);
LIBM_DECL_FN_MAP(vrd2_cos);
LIBM_DECL_FN_MAP(vrd2_cosh);
LIBM_DECL_FN_MAP(vrd2_exp10);
LIBM_DECL_FN_MAP(vrd2_exp2);
LIBM_DECL_FN_MAP(vrd2_exp);
LIBM_DECL_FN_MAP(vrd2_expm1);
LIBM_DECL_FN_MAP(vrd2_log10);
LIBM_DECL_FN_MAP(vrd2_log1p);
LIBM_DECL_FN_MAP(vrd2_log2);
LIBM_DECL_FN_MAP(vrd2_log);
LIBM_DECL_FN_MAP(vrd2_pow);
LIBM_DECL_FN_MAP(vrd2_sin);
LIBM_DECL_FN_MAP(vrd2_tan);

LIBM_DECL_FN_MAP(vrd4_exp2);
LIBM_DECL_FN_MAP(vrd4_exp);
LIBM_DECL_FN_MAP(vrd4_expm1);
LIBM_DECL_FN_MAP(vrd4_log);
LIBM_DECL_FN_MAP(vrd4_pow);
LIBM_DECL_FN_MAP(vrd4_sin);
LIBM_DECL_FN_MAP(vrd4_cos);
LIBM_DECL_FN_MAP(vrd4_tan);

LIBM_DECL_FN_MAP(vrs4_cbrtf);
LIBM_DECL_FN_MAP(vrs4_cosf);
LIBM_DECL_FN_MAP(vrs4_exp10f);
LIBM_DECL_FN_MAP(vrs4_exp2f);
LIBM_DECL_FN_MAP(vrs4_expf);
LIBM_DECL_FN_MAP(vrs4_expm1f);
LIBM_DECL_FN_MAP(vrs4_log10f);
LIBM_DECL_FN_MAP(vrs4_log1pf);
LIBM_DECL_FN_MAP(vrs4_log2f);
LIBM_DECL_FN_MAP(vrs4_logf);
LIBM_DECL_FN_MAP(vrs4_powf);
LIBM_DECL_FN_MAP(vrs4_sinf);
LIBM_DECL_FN_MAP(vrs4_tanf);
LIBM_DECL_FN_MAP(vrs4_coshf);
LIBM_DECL_FN_MAP(vrs4_tanhf);

LIBM_DECL_FN_MAP(vrs8_logf);
LIBM_DECL_FN_MAP(vrs8_log2f);
LIBM_DECL_FN_MAP(vrs8_expf);
LIBM_DECL_FN_MAP(vrs8_exp2f);
LIBM_DECL_FN_MAP(vrs8_powf);
LIBM_DECL_FN_MAP(vrs8_sinf);
LIBM_DECL_FN_MAP(vrs8_tanf);
LIBM_DECL_FN_MAP(vrs8_cosf);
LIBM_DECL_FN_MAP(vrs8_coshf);
LIBM_DECL_FN_MAP(vrs8_tanhf);

LIBM_DECL_FN_MAP(vrda_cbrt);
LIBM_DECL_FN_MAP(vrda_cos);
LIBM_DECL_FN_MAP(vrda_exp);
LIBM_DECL_FN_MAP(vrda_exp2);
LIBM_DECL_FN_MAP(vrda_exp10);
LIBM_DECL_FN_MAP(vrda_expm1);
LIBM_DECL_FN_MAP(vrda_log10);
LIBM_DECL_FN_MAP(vrda_log1p);
LIBM_DECL_FN_MAP(vrda_log2);
LIBM_DECL_FN_MAP(vrda_log);
LIBM_DECL_FN_MAP(vrda_pow);
LIBM_DECL_FN_MAP(vrda_sin);

LIBM_DECL_FN_MAP(vrsa_cbrtf);
LIBM_DECL_FN_MAP(vrsa_cosf);
LIBM_DECL_FN_MAP(vrsa_expf);
LIBM_DECL_FN_MAP(vrsa_exp2f);
LIBM_DECL_FN_MAP(vrsa_exp10f);
LIBM_DECL_FN_MAP(vrsa_expm1f);
LIBM_DECL_FN_MAP(vrsa_log10f);
LIBM_DECL_FN_MAP(vrsa_log1pf);
LIBM_DECL_FN_MAP(vrsa_log2f);
LIBM_DECL_FN_MAP(vrsa_logf);
LIBM_DECL_FN_MAP(vrsa_sinf);
LIBM_DECL_FN_MAP(vrsa_powf);

WEAK_LIBM_ALIAS(vrd2_cbrt, FN_PROTOTYPE(vrd2_cbrt));
WEAK_LIBM_ALIAS(vrd2_cos, FN_PROTOTYPE(vrd2_cos));
WEAK_LIBM_ALIAS(vrd2_exp10, FN_PROTOTYPE(vrd2_exp10));
WEAK_LIBM_ALIAS(vrd2_exp2, FN_PROTOTYPE(vrd2_exp2));
WEAK_LIBM_ALIAS(vrd2_exp, FN_PROTOTYPE(vrd2_exp));
WEAK_LIBM_ALIAS(vrd2_expm1, FN_PROTOTYPE(vrd2_expm1));
WEAK_LIBM_ALIAS(vrd2_log10, FN_PROTOTYPE(vrd2_log10));
WEAK_LIBM_ALIAS(vrd2_log1p, FN_PROTOTYPE(vrd2_log1p));
WEAK_LIBM_ALIAS(vrd2_log2, FN_PROTOTYPE(vrd2_log2));
WEAK_LIBM_ALIAS(vrd2_log, FN_PROTOTYPE(vrd2_log));
WEAK_LIBM_ALIAS(vrd2_sin, FN_PROTOTYPE(vrd2_sin));
WEAK_LIBM_ALIAS(vrd2_tan, FN_PROTOTYPE(vrd2_tan));

WEAK_LIBM_ALIAS(vrd4_exp, FN_PROTOTYPE(vrd4_exp));
WEAK_LIBM_ALIAS(vrd4_exp2, FN_PROTOTYPE(vrd4_exp2));
WEAK_LIBM_ALIAS(vrd4_log, FN_PROTOTYPE(vrd4_log));
WEAK_LIBM_ALIAS(vrd4_sin, FN_PROTOTYPE(vrd4_sin));

WEAK_LIBM_ALIAS(vrs4_cbrtf, FN_PROTOTYPE(vrs4_cbrtf));
WEAK_LIBM_ALIAS(vrs4_cosf, FN_PROTOTYPE(vrs4_cosf));
WEAK_LIBM_ALIAS(vrs4_exp10f, FN_PROTOTYPE(vrs4_exp10f));
WEAK_LIBM_ALIAS(vrs4_exp2f, FN_PROTOTYPE(vrs4_exp2f));
WEAK_LIBM_ALIAS(vrs4_expf, FN_PROTOTYPE(vrs4_expf));
WEAK_LIBM_ALIAS(vrs4_expm1f, FN_PROTOTYPE(vrs4_expm1f));
WEAK_LIBM_ALIAS(vrs4_log10f, FN_PROTOTYPE(vrs4_log10f));
WEAK_LIBM_ALIAS(vrs4_log1pf, FN_PROTOTYPE(vrs4_log1pf));
WEAK_LIBM_ALIAS(vrs4_log2f, FN_PROTOTYPE(vrs4_log2f));
WEAK_LIBM_ALIAS(vrs4_logf, FN_PROTOTYPE(vrs4_logf));
WEAK_LIBM_ALIAS(vrs4_sinf, FN_PROTOTYPE(vrs4_sinf));
WEAK_LIBM_ALIAS(vrs4_tanf, FN_PROTOTYPE(vrs4_tanf));

WEAK_LIBM_ALIAS(vrs8_exp2f, FN_PROTOTYPE(vrs8_exp2f));
WEAK_LIBM_ALIAS(vrs8_logf, FN_PROTOTYPE(vrs8_logf));
WEAK_LIBM_ALIAS(vrs8_tanf, FN_PROTOTYPE(vrs8_tanf));

WEAK_LIBM_ALIAS(vrda_cbrt, FN_PROTOTYPE(vrda_cbrt));
WEAK_LIBM_ALIAS(vrda_cos, FN_PROTOTYPE(vrda_cos));
WEAK_LIBM_ALIAS(vrda_exp, FN_PROTOTYPE(vrda_exp));
WEAK_LIBM_ALIAS(vrda_exp2, FN_PROTOTYPE(vrda_exp2));
WEAK_LIBM_ALIAS(vrda_exp10, FN_PROTOTYPE(vrda_exp10));
WEAK_LIBM_ALIAS(vrda_expm1, FN_PROTOTYPE(vrda_expm1));
WEAK_LIBM_ALIAS(vrda_log10, FN_PROTOTYPE(vrda_log10));
WEAK_LIBM_ALIAS(vrda_log1p, FN_PROTOTYPE(vrda_log1p));
WEAK_LIBM_ALIAS(vrda_log2, FN_PROTOTYPE(vrda_log2));
WEAK_LIBM_ALIAS(vrda_sin, FN_PROTOTYPE(vrda_sin));


WEAK_LIBM_ALIAS(vrsa_cbrtf, FN_PROTOTYPE(vrsa_cbrtf));
WEAK_LIBM_ALIAS(vrsa_cosf, FN_PROTOTYPE(vrsa_cosf));
WEAK_LIBM_ALIAS(vrsa_expf, FN_PROTOTYPE(vrsa_expf));
WEAK_LIBM_ALIAS(vrsa_exp2f, FN_PROTOTYPE(vrsa_exp2f));
WEAK_LIBM_ALIAS(vrsa_exp10f, FN_PROTOTYPE(vrsa_exp10f));
WEAK_LIBM_ALIAS(vrsa_expm1f, FN_PROTOTYPE(vrsa_expm1f));
WEAK_LIBM_ALIAS(vrsa_log10f, FN_PROTOTYPE(vrsa_log10f));
WEAK_LIBM_ALIAS(vrsa_log1pf, FN_PROTOTYPE(vrsa_log1pf));
WEAK_LIBM_ALIAS(vrsa_log2f, FN_PROTOTYPE(vrsa_log2f));
WEAK_LIBM_ALIAS(vrsa_logf, FN_PROTOTYPE(vrsa_logf));
WEAK_LIBM_ALIAS(vrsa_sinf, FN_PROTOTYPE(vrsa_sinf));
WEAK_LIBM_ALIAS(vrsa_powf, FN_PROTOTYPE(vrsa_powf));

/*
 * Some aliases required by NAG tests
 */

/*
 * We dont need libm aliases in developer mode, otherwise all calls
 * to GLIBC ends up in libm
 */
#if !defined(DEVELOPER)
#include "entry_pt_map_libm._c"
#endif

