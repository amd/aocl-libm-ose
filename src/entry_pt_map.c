/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
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


/*
 * LIBM_DECL_FN_MAP is used to declare and map AOCL Libm functions.
 * The functions will be mapped as "amd_<func_name>".
 */

/* Scalar Arithmetic Functions */

LIBM_DECL_FN_MAP(copysign);
LIBM_DECL_FN_MAP(copysignf);

LIBM_DECL_FN_MAP(finite);
LIBM_DECL_FN_MAP(finitef);

LIBM_DECL_FN_MAP(nan);
LIBM_DECL_FN_MAP(nanf);

LIBM_DECL_FN_MAP(fmaf);
LIBM_DECL_FN_MAP(fma);

LIBM_DECL_FN_MAP(fdim);
LIBM_DECL_FN_MAP(fdimf);

LIBM_DECL_FN_MAP(fmax);
LIBM_DECL_FN_MAP(fmaxf);

LIBM_DECL_FN_MAP(fmin);
LIBM_DECL_FN_MAP(fminf);

LIBM_DECL_FN_MAP(powf);
LIBM_DECL_FN_MAP(pow);

LIBM_DECL_FN_MAP(sqrt);
LIBM_DECL_FN_MAP(sqrtf);

LIBM_DECL_FN_MAP(cbrtf);
LIBM_DECL_FN_MAP(cbrt);

LIBM_DECL_FN_MAP(fabs);
LIBM_DECL_FN_MAP(fabsf);

LIBM_DECL_FN_MAP(floor);
LIBM_DECL_FN_MAP(floorf);

LIBM_DECL_FN_MAP(ceil);
LIBM_DECL_FN_MAP(ceilf);

LIBM_DECL_FN_MAP(trunc);
LIBM_DECL_FN_MAP(truncf);

LIBM_DECL_FN_MAP(nextafter);
LIBM_DECL_FN_MAP(nextafterf);

LIBM_DECL_FN_MAP(nexttoward);
LIBM_DECL_FN_MAP(nexttowardf);

LIBM_DECL_FN_MAP(nearbyint);
LIBM_DECL_FN_MAP(nearbyintf);

LIBM_DECL_FN_MAP(llround);
LIBM_DECL_FN_MAP(llroundf);

LIBM_DECL_FN_MAP(lround);
LIBM_DECL_FN_MAP(lroundf);

LIBM_DECL_FN_MAP(round);
LIBM_DECL_FN_MAP(roundf);

LIBM_DECL_FN_MAP(llrint);
LIBM_DECL_FN_MAP(llrintf);

LIBM_DECL_FN_MAP(lrint);
LIBM_DECL_FN_MAP(lrintf);

LIBM_DECL_FN_MAP(rint);
LIBM_DECL_FN_MAP(rintf);

LIBM_DECL_FN_MAP(remainder);
LIBM_DECL_FN_MAP(remainderf);

LIBM_DECL_FN_MAP(fmod);
LIBM_DECL_FN_MAP(fmodf);

LIBM_DECL_FN_MAP(modf);
LIBM_DECL_FN_MAP(modff);

LIBM_DECL_FN_MAP(remquo);
LIBM_DECL_FN_MAP(remquof);

LIBM_DECL_FN_MAP(scalbln);
LIBM_DECL_FN_MAP(scalblnf);

LIBM_DECL_FN_MAP(scalbn);
LIBM_DECL_FN_MAP(scalbnf);

LIBM_DECL_FN_MAP(remainder_piby2);
LIBM_DECL_FN_MAP(remainder_piby2d2f);


/* Scalar Exponential Functions */

LIBM_DECL_FN_MAP(expf);
LIBM_DECL_FN_MAP(exp);

LIBM_DECL_FN_MAP(exp2f);
LIBM_DECL_FN_MAP(exp2);

LIBM_DECL_FN_MAP(exp10f);
LIBM_DECL_FN_MAP(exp10);

LIBM_DECL_FN_MAP(expm1f);
LIBM_DECL_FN_MAP(expm1);

LIBM_DECL_FN_MAP(ldexp);
LIBM_DECL_FN_MAP(ldexpf);

LIBM_DECL_FN_MAP(frexp);
LIBM_DECL_FN_MAP(frexpf);


/* Scalar Logarithmic Functions */

LIBM_DECL_FN_MAP(logf);
LIBM_DECL_FN_MAP(log);

LIBM_DECL_FN_MAP(log2f);
LIBM_DECL_FN_MAP(log2);

LIBM_DECL_FN_MAP(log10f);
LIBM_DECL_FN_MAP(log10);

LIBM_DECL_FN_MAP(log1pf);
LIBM_DECL_FN_MAP(log1p);

LIBM_DECL_FN_MAP(ilogb);
LIBM_DECL_FN_MAP(ilogbf);

LIBM_DECL_FN_MAP(logb);
LIBM_DECL_FN_MAP(logbf);


/* Scalar Trigonometric Functions */

LIBM_DECL_FN_MAP(hypot);
LIBM_DECL_FN_MAP(hypotf);

LIBM_DECL_FN_MAP(sinf);
LIBM_DECL_FN_MAP(sin);

LIBM_DECL_FN_MAP(cosf);
LIBM_DECL_FN_MAP(cos);

LIBM_DECL_FN_MAP(tan);
LIBM_DECL_FN_MAP(tanf);

LIBM_DECL_FN_MAP(sinh);
LIBM_DECL_FN_MAP(sinhf);

LIBM_DECL_FN_MAP(cosh);
LIBM_DECL_FN_MAP(coshf);

LIBM_DECL_FN_MAP(tanh);
LIBM_DECL_FN_MAP(tanhf);

LIBM_DECL_FN_MAP(asin);
LIBM_DECL_FN_MAP(asinf);

LIBM_DECL_FN_MAP(acos);
LIBM_DECL_FN_MAP(acosf);

LIBM_DECL_FN_MAP(atan);
LIBM_DECL_FN_MAP(atanf);

LIBM_DECL_FN_MAP(asinh);
LIBM_DECL_FN_MAP(asinhf);

LIBM_DECL_FN_MAP(acosh);
LIBM_DECL_FN_MAP(acoshf);

LIBM_DECL_FN_MAP(atanh);
LIBM_DECL_FN_MAP(atanhf);

LIBM_DECL_FN_MAP(sinpi);
LIBM_DECL_FN_MAP(sinpif);

LIBM_DECL_FN_MAP(cospi);
LIBM_DECL_FN_MAP(cospif);

LIBM_DECL_FN_MAP(tanpi);
LIBM_DECL_FN_MAP(tanpif);

LIBM_DECL_FN_MAP(sincosf);
LIBM_DECL_FN_MAP(sincos);

LIBM_DECL_FN_MAP(atan2);
LIBM_DECL_FN_MAP(atan2f);


/* Scalar Error Functions */

LIBM_DECL_FN_MAP(erf);
LIBM_DECL_FN_MAP(erff);


/* Scalar Complex Number Variant Functions */

LIBM_DECL_FN_MAP(cexp);
LIBM_DECL_FN_MAP(cexpf);

LIBM_DECL_FN_MAP(cpowf);
LIBM_DECL_FN_MAP(cpow);

LIBM_DECL_FN_MAP(clog);
LIBM_DECL_FN_MAP(clogf);


/* Vector Array Variants */

LIBM_DECL_FN_MAP(vrsa_powf);
LIBM_DECL_FN_MAP(vrsa_cbrtf);
LIBM_DECL_FN_MAP(vrsa_expf);
LIBM_DECL_FN_MAP(vrsa_exp2f);
LIBM_DECL_FN_MAP(vrsa_exp10f);
LIBM_DECL_FN_MAP(vrsa_expm1f);
LIBM_DECL_FN_MAP(vrsa_logf);
LIBM_DECL_FN_MAP(vrsa_log2f);
LIBM_DECL_FN_MAP(vrsa_log10f);
LIBM_DECL_FN_MAP(vrsa_log1pf);
LIBM_DECL_FN_MAP(vrsa_sinf);
LIBM_DECL_FN_MAP(vrsa_cosf);

LIBM_DECL_FN_MAP(vrsa_addf);
LIBM_DECL_FN_MAP(vrsa_subf);
LIBM_DECL_FN_MAP(vrsa_mulf);
LIBM_DECL_FN_MAP(vrsa_divf);
LIBM_DECL_FN_MAP(vrsa_fmaxf);
LIBM_DECL_FN_MAP(vrsa_fminf);

LIBM_DECL_FN_MAP(vrsa_addfi);
LIBM_DECL_FN_MAP(vrsa_subfi);
LIBM_DECL_FN_MAP(vrsa_mulfi);
LIBM_DECL_FN_MAP(vrsa_divfi);
LIBM_DECL_FN_MAP(vrsa_fmaxfi);
LIBM_DECL_FN_MAP(vrsa_fminfi);

LIBM_DECL_FN_MAP(vrda_cbrt);
LIBM_DECL_FN_MAP(vrda_exp);
LIBM_DECL_FN_MAP(vrda_exp2);
LIBM_DECL_FN_MAP(vrda_exp10);
LIBM_DECL_FN_MAP(vrda_expm1);
LIBM_DECL_FN_MAP(vrda_log);
LIBM_DECL_FN_MAP(vrda_log2);
LIBM_DECL_FN_MAP(vrda_log10);
LIBM_DECL_FN_MAP(vrda_log1p);
LIBM_DECL_FN_MAP(vrda_sin);
LIBM_DECL_FN_MAP(vrda_cos);

LIBM_DECL_FN_MAP(vrda_add);
LIBM_DECL_FN_MAP(vrda_sub);
LIBM_DECL_FN_MAP(vrda_mul);
LIBM_DECL_FN_MAP(vrda_div);
LIBM_DECL_FN_MAP(vrda_fmax);
LIBM_DECL_FN_MAP(vrda_fmin);

LIBM_DECL_FN_MAP(vrda_addi);
LIBM_DECL_FN_MAP(vrda_subi);
LIBM_DECL_FN_MAP(vrda_muli);
LIBM_DECL_FN_MAP(vrda_divi);
LIBM_DECL_FN_MAP(vrda_fmaxi);
LIBM_DECL_FN_MAP(vrda_fmini);


/* Vector Functions */

LIBM_DECL_FN_MAP(vrd2_pow);
LIBM_DECL_FN_MAP(vrd2_powx);
LIBM_DECL_FN_MAP(vrd2_cbrt);
LIBM_DECL_FN_MAP(vrd2_exp);
LIBM_DECL_FN_MAP(vrd2_exp2);
LIBM_DECL_FN_MAP(vrd2_exp10);
LIBM_DECL_FN_MAP(vrd2_log);
LIBM_DECL_FN_MAP(vrd2_log2);
LIBM_DECL_FN_MAP(vrd2_log10);
LIBM_DECL_FN_MAP(vrd2_log1p);
LIBM_DECL_FN_MAP(vrd2_sin);
LIBM_DECL_FN_MAP(vrd2_cos);
LIBM_DECL_FN_MAP(vrd2_tan);
LIBM_DECL_FN_MAP(vrd2_cosh);
LIBM_DECL_FN_MAP(vrd2_atan);
LIBM_DECL_FN_MAP(vrd2_erf);
LIBM_DECL_FN_MAP(vrd2_sqrt);

LIBM_DECL_FN_MAP(vrd4_pow);
LIBM_DECL_FN_MAP(vrd4_exp);
LIBM_DECL_FN_MAP(vrd4_exp2);
LIBM_DECL_FN_MAP(vrd4_log);
LIBM_DECL_FN_MAP(vrd4_log2);
LIBM_DECL_FN_MAP(vrd4_sin);
LIBM_DECL_FN_MAP(vrd4_cos);
LIBM_DECL_FN_MAP(vrd4_tan);
LIBM_DECL_FN_MAP(vrd4_atan);
LIBM_DECL_FN_MAP(vrd4_sincos);
LIBM_DECL_FN_MAP(vrd4_erf);
LIBM_DECL_FN_MAP(vrd4_sqrt);

LIBM_DECL_FN_MAP(vrd8_pow);
LIBM_DECL_FN_MAP(vrd8_exp);
LIBM_DECL_FN_MAP(vrd8_exp2);
LIBM_DECL_FN_MAP(vrd8_log);
LIBM_DECL_FN_MAP(vrd8_log2);
LIBM_DECL_FN_MAP(vrd8_sin);
LIBM_DECL_FN_MAP(vrd8_cos);
LIBM_DECL_FN_MAP(vrd8_tan);
LIBM_DECL_FN_MAP(vrd8_asin);
LIBM_DECL_FN_MAP(vrd8_atan);
LIBM_DECL_FN_MAP(vrd8_sincos);
LIBM_DECL_FN_MAP(vrd8_erf);

LIBM_DECL_FN_MAP(vrs4_powf);
LIBM_DECL_FN_MAP(vrs4_cbrtf);
LIBM_DECL_FN_MAP(vrs4_expf);
LIBM_DECL_FN_MAP(vrs4_exp2f);
LIBM_DECL_FN_MAP(vrs4_exp10f);
LIBM_DECL_FN_MAP(vrs4_expm1f);
LIBM_DECL_FN_MAP(vrs4_logf);
LIBM_DECL_FN_MAP(vrs4_log2f);
LIBM_DECL_FN_MAP(vrs4_log10f);
LIBM_DECL_FN_MAP(vrs4_log1pf);
LIBM_DECL_FN_MAP(vrs4_sinf);
LIBM_DECL_FN_MAP(vrs4_cosf);
LIBM_DECL_FN_MAP(vrs4_tanf);
LIBM_DECL_FN_MAP(vrs4_coshf);
LIBM_DECL_FN_MAP(vrs4_tanhf);
LIBM_DECL_FN_MAP(vrs4_asinf);
LIBM_DECL_FN_MAP(vrs4_acosf);
LIBM_DECL_FN_MAP(vrs4_atanf);
LIBM_DECL_FN_MAP(vrs4_erff);

LIBM_DECL_FN_MAP(vrs8_powf);
LIBM_DECL_FN_MAP(vrs8_expf);
LIBM_DECL_FN_MAP(vrs8_exp2f);
LIBM_DECL_FN_MAP(vrs8_logf);
LIBM_DECL_FN_MAP(vrs8_log2f);
LIBM_DECL_FN_MAP(vrs8_log10f);
LIBM_DECL_FN_MAP(vrs8_sinf);
LIBM_DECL_FN_MAP(vrs8_cosf);
LIBM_DECL_FN_MAP(vrs8_tanf);
LIBM_DECL_FN_MAP(vrs8_coshf);
LIBM_DECL_FN_MAP(vrs8_tanhf);
LIBM_DECL_FN_MAP(vrs8_asinf);
LIBM_DECL_FN_MAP(vrs8_acosf);
LIBM_DECL_FN_MAP(vrs8_atanf);
LIBM_DECL_FN_MAP(vrs8_erff);

LIBM_DECL_FN_MAP(vrs16_powf);
LIBM_DECL_FN_MAP(vrs16_expf);
LIBM_DECL_FN_MAP(vrs16_exp2f);
LIBM_DECL_FN_MAP(vrs16_logf);
LIBM_DECL_FN_MAP(vrs16_log2f);
LIBM_DECL_FN_MAP(vrs16_log10f);
LIBM_DECL_FN_MAP(vrs16_sinf);
LIBM_DECL_FN_MAP(vrs16_cosf);
LIBM_DECL_FN_MAP(vrs16_tanf);
LIBM_DECL_FN_MAP(vrs16_tanhf);
LIBM_DECL_FN_MAP(vrs16_asinf);
LIBM_DECL_FN_MAP(vrs16_acosf);
LIBM_DECL_FN_MAP(vrs16_atanf);
LIBM_DECL_FN_MAP(vrs16_erff);



/*
 * WEAK_LIBM_ALIAS is used to map "amd_<func_name>" to "<func_name>".
 * This will enable applications to call AOCL-Libm functions directly without using the "amd_" prefix.
 */

/* Scalar Arithmetic Functions */

WEAK_LIBM_ALIAS(copysignf, FN_PROTOTYPE(copysignf));
WEAK_LIBM_ALIAS(copysign, FN_PROTOTYPE(copysign));
WEAK_LIBM_ALIAS(finitef, FN_PROTOTYPE(finitef));
WEAK_LIBM_ALIAS(finite, FN_PROTOTYPE(finite));
WEAK_LIBM_ALIAS(nanf, FN_PROTOTYPE(nanf));
WEAK_LIBM_ALIAS(nan, FN_PROTOTYPE(nan));

WEAK_LIBM_ALIAS(fmaf, FN_PROTOTYPE(fmaf));
WEAK_LIBM_ALIAS(fma, FN_PROTOTYPE(fma));
WEAK_LIBM_ALIAS(fdimf, FN_PROTOTYPE(fdimf));
WEAK_LIBM_ALIAS(fdim, FN_PROTOTYPE(fdim));
WEAK_LIBM_ALIAS(fmaxf, FN_PROTOTYPE(fmaxf));
WEAK_LIBM_ALIAS(fmax, FN_PROTOTYPE(fmax));
WEAK_LIBM_ALIAS(fminf, FN_PROTOTYPE(fminf));
WEAK_LIBM_ALIAS(fmin, FN_PROTOTYPE(fmin));

WEAK_LIBM_ALIAS(powf, FN_PROTOTYPE(powf));
WEAK_LIBM_ALIAS(pow, FN_PROTOTYPE(pow));
WEAK_LIBM_ALIAS(sqrtf, FN_PROTOTYPE(sqrtf));
WEAK_LIBM_ALIAS(sqrt, FN_PROTOTYPE(sqrt));
WEAK_LIBM_ALIAS(cbrtf, FN_PROTOTYPE(cbrtf));
WEAK_LIBM_ALIAS(cbrt, FN_PROTOTYPE(cbrt));

WEAK_LIBM_ALIAS(fabsf, FN_PROTOTYPE(fabsf));
WEAK_LIBM_ALIAS(fabs, FN_PROTOTYPE(fabs));
WEAK_LIBM_ALIAS(floorf, FN_PROTOTYPE(floorf));
WEAK_LIBM_ALIAS(floor, FN_PROTOTYPE(floor));
WEAK_LIBM_ALIAS(ceilf, FN_PROTOTYPE(ceilf));
WEAK_LIBM_ALIAS(ceil, FN_PROTOTYPE(ceil));
WEAK_LIBM_ALIAS(truncf, FN_PROTOTYPE(truncf));
WEAK_LIBM_ALIAS(trunc, FN_PROTOTYPE(trunc));

WEAK_LIBM_ALIAS(nextafterf, FN_PROTOTYPE(nextafterf));
WEAK_LIBM_ALIAS(nextafter, FN_PROTOTYPE(nextafter));
WEAK_LIBM_ALIAS(nexttowardf, FN_PROTOTYPE(nexttowardf));
WEAK_LIBM_ALIAS(nexttoward, FN_PROTOTYPE(nexttoward));
WEAK_LIBM_ALIAS(nearbyintf, FN_PROTOTYPE(nearbyintf));
WEAK_LIBM_ALIAS(nearbyint, FN_PROTOTYPE(nearbyint));

WEAK_LIBM_ALIAS(llroundf, FN_PROTOTYPE(llroundf));
WEAK_LIBM_ALIAS(llround, FN_PROTOTYPE(llround));
WEAK_LIBM_ALIAS(lroundf, FN_PROTOTYPE(lroundf));
WEAK_LIBM_ALIAS(lround, FN_PROTOTYPE(lround));
WEAK_LIBM_ALIAS(roundf, FN_PROTOTYPE(roundf));
WEAK_LIBM_ALIAS(round, FN_PROTOTYPE(round));

WEAK_LIBM_ALIAS(llrintf, FN_PROTOTYPE(llrintf));
WEAK_LIBM_ALIAS(llrint, FN_PROTOTYPE(llrint));
WEAK_LIBM_ALIAS(lrintf, FN_PROTOTYPE(lrintf));
WEAK_LIBM_ALIAS(lrint, FN_PROTOTYPE(lrint));
WEAK_LIBM_ALIAS(rintf, FN_PROTOTYPE(rintf));
WEAK_LIBM_ALIAS(rint, FN_PROTOTYPE(rint));

WEAK_LIBM_ALIAS(remainderf, FN_PROTOTYPE(remainderf));
WEAK_LIBM_ALIAS(remainder, FN_PROTOTYPE(remainder));
WEAK_LIBM_ALIAS(fmodf, FN_PROTOTYPE(fmodf));
WEAK_LIBM_ALIAS(fmod, FN_PROTOTYPE(fmod));
WEAK_LIBM_ALIAS(modff, FN_PROTOTYPE(modff));
WEAK_LIBM_ALIAS(modf, FN_PROTOTYPE(modf));
WEAK_LIBM_ALIAS(remquof, FN_PROTOTYPE(remquof));
WEAK_LIBM_ALIAS(remquo, FN_PROTOTYPE(remquo));

WEAK_LIBM_ALIAS(scalblnf, FN_PROTOTYPE(scalblnf));
WEAK_LIBM_ALIAS(scalbln, FN_PROTOTYPE(scalbln));
WEAK_LIBM_ALIAS(scalbnf, FN_PROTOTYPE(scalbnf));
WEAK_LIBM_ALIAS(scalbn, FN_PROTOTYPE(scalbn));


/* Scalar Exponential Functions */

WEAK_LIBM_ALIAS(expf, FN_PROTOTYPE(expf));
WEAK_LIBM_ALIAS(exp, FN_PROTOTYPE(exp));

WEAK_LIBM_ALIAS(exp2f, FN_PROTOTYPE(exp2f));
WEAK_LIBM_ALIAS(exp2, FN_PROTOTYPE(exp2));

WEAK_LIBM_ALIAS(exp10f, FN_PROTOTYPE(exp10f));
WEAK_LIBM_ALIAS(exp10, FN_PROTOTYPE(exp10));

WEAK_LIBM_ALIAS(expm1f, FN_PROTOTYPE(expm1f));
WEAK_LIBM_ALIAS(expm1, FN_PROTOTYPE(expm1));

WEAK_LIBM_ALIAS(ldexpf, FN_PROTOTYPE(ldexpf));
WEAK_LIBM_ALIAS(ldexp, FN_PROTOTYPE(ldexp));

WEAK_LIBM_ALIAS(frexpf, FN_PROTOTYPE(frexpf));
WEAK_LIBM_ALIAS(frexp, FN_PROTOTYPE(frexp));


/* Scalar Logarithmic Functions */

WEAK_LIBM_ALIAS(logf, FN_PROTOTYPE(logf));
WEAK_LIBM_ALIAS(log, FN_PROTOTYPE(log));

WEAK_LIBM_ALIAS(log2f, FN_PROTOTYPE(log2f));
WEAK_LIBM_ALIAS(log2, FN_PROTOTYPE(log2));

WEAK_LIBM_ALIAS(log10f, FN_PROTOTYPE(log10f));
WEAK_LIBM_ALIAS(log10, FN_PROTOTYPE(log10));

WEAK_LIBM_ALIAS(log1pf, FN_PROTOTYPE(log1pf));
WEAK_LIBM_ALIAS(log1p, FN_PROTOTYPE(log1p));

WEAK_LIBM_ALIAS(ilogbf, FN_PROTOTYPE(ilogbf));
WEAK_LIBM_ALIAS(ilogb, FN_PROTOTYPE(ilogb));

WEAK_LIBM_ALIAS(logbf, FN_PROTOTYPE(logbf));
WEAK_LIBM_ALIAS(logb, FN_PROTOTYPE(logb));


/* Scalar Trigonometric Functions */

WEAK_LIBM_ALIAS(hypotf, FN_PROTOTYPE(hypotf));
WEAK_LIBM_ALIAS(hypot, FN_PROTOTYPE(hypot));
WEAK_LIBM_ALIAS(sinf, FN_PROTOTYPE(sinf));
WEAK_LIBM_ALIAS(sin, FN_PROTOTYPE(sin));
WEAK_LIBM_ALIAS(cosf, FN_PROTOTYPE(cosf));
WEAK_LIBM_ALIAS(cos, FN_PROTOTYPE(cos));
WEAK_LIBM_ALIAS(tanf, FN_PROTOTYPE(tanf));
WEAK_LIBM_ALIAS(tan, FN_PROTOTYPE(tan));

WEAK_LIBM_ALIAS(sinhf, FN_PROTOTYPE(sinhf));
WEAK_LIBM_ALIAS(sinh, FN_PROTOTYPE(sinh));
WEAK_LIBM_ALIAS(coshf, FN_PROTOTYPE(coshf));
WEAK_LIBM_ALIAS(cosh, FN_PROTOTYPE(cosh));
WEAK_LIBM_ALIAS(tanhf, FN_PROTOTYPE(tanhf));
WEAK_LIBM_ALIAS(tanh, FN_PROTOTYPE(tanh));

WEAK_LIBM_ALIAS(asinf, FN_PROTOTYPE(asinf));
WEAK_LIBM_ALIAS(asin, FN_PROTOTYPE(asin));
WEAK_LIBM_ALIAS(acosf, FN_PROTOTYPE(acosf));
WEAK_LIBM_ALIAS(acos, FN_PROTOTYPE(acos));
WEAK_LIBM_ALIAS(atanf, FN_PROTOTYPE(atanf));
WEAK_LIBM_ALIAS(atan, FN_PROTOTYPE(atan));

WEAK_LIBM_ALIAS(asinhf, FN_PROTOTYPE(asinhf));
WEAK_LIBM_ALIAS(asinh, FN_PROTOTYPE(asinh));
WEAK_LIBM_ALIAS(acoshf, FN_PROTOTYPE(acoshf));
WEAK_LIBM_ALIAS(acosh, FN_PROTOTYPE(acosh));
WEAK_LIBM_ALIAS(atanhf, FN_PROTOTYPE(atanhf));
WEAK_LIBM_ALIAS(atanh, FN_PROTOTYPE(atanh));

WEAK_LIBM_ALIAS(sinpif, FN_PROTOTYPE(sinpif));
WEAK_LIBM_ALIAS(sinpi, FN_PROTOTYPE(sinpi));
WEAK_LIBM_ALIAS(cospif, FN_PROTOTYPE(cospif));
WEAK_LIBM_ALIAS(cospi, FN_PROTOTYPE(cospi));
WEAK_LIBM_ALIAS(tanpif, FN_PROTOTYPE(tanpif));
WEAK_LIBM_ALIAS(tanpi, FN_PROTOTYPE(tanpi));

WEAK_LIBM_ALIAS(sincosf, FN_PROTOTYPE(sincosf));
WEAK_LIBM_ALIAS(sincos, FN_PROTOTYPE(sincos));

WEAK_LIBM_ALIAS(atan2f, FN_PROTOTYPE(atan2f));
WEAK_LIBM_ALIAS(atan2, FN_PROTOTYPE(atan2));


/* Scalar Error Functions */

WEAK_LIBM_ALIAS(erff, FN_PROTOTYPE(erff));
WEAK_LIBM_ALIAS(erf, FN_PROTOTYPE(erf));


/* Scalar Complex Number Variant Functions */

WEAK_LIBM_ALIAS(cexpf, FN_PROTOTYPE(cexpf));
WEAK_LIBM_ALIAS(cexp, FN_PROTOTYPE(cexp));
WEAK_LIBM_ALIAS(cpowf, FN_PROTOTYPE(cpowf));
WEAK_LIBM_ALIAS(cpow, FN_PROTOTYPE(cpow));
WEAK_LIBM_ALIAS(clogf, FN_PROTOTYPE(clogf));
WEAK_LIBM_ALIAS(clog, FN_PROTOTYPE(clog));


/* Vector Array Variants */

WEAK_LIBM_ALIAS(vrsa_powf, FN_PROTOTYPE(vrsa_powf));
WEAK_LIBM_ALIAS(vrsa_cbrtf, FN_PROTOTYPE(vrsa_cbrtf));
WEAK_LIBM_ALIAS(vrsa_expf, FN_PROTOTYPE(vrsa_expf));
WEAK_LIBM_ALIAS(vrsa_exp2f, FN_PROTOTYPE(vrsa_exp2f));
WEAK_LIBM_ALIAS(vrsa_exp10f, FN_PROTOTYPE(vrsa_exp10f));
WEAK_LIBM_ALIAS(vrsa_expm1f, FN_PROTOTYPE(vrsa_expm1f));
WEAK_LIBM_ALIAS(vrsa_logf, FN_PROTOTYPE(vrsa_logf));
WEAK_LIBM_ALIAS(vrsa_log2f, FN_PROTOTYPE(vrsa_log2f));
WEAK_LIBM_ALIAS(vrsa_log10f, FN_PROTOTYPE(vrsa_log10f));
WEAK_LIBM_ALIAS(vrsa_log1pf, FN_PROTOTYPE(vrsa_log1pf));
WEAK_LIBM_ALIAS(vrsa_sinf, FN_PROTOTYPE(vrsa_sinf));
WEAK_LIBM_ALIAS(vrsa_cosf, FN_PROTOTYPE(vrsa_cosf));

WEAK_LIBM_ALIAS(vrsa_addf, FN_PROTOTYPE(vrsa_addf));
WEAK_LIBM_ALIAS(vrsa_subf, FN_PROTOTYPE(vrsa_subf));
WEAK_LIBM_ALIAS(vrsa_mulf, FN_PROTOTYPE(vrsa_mulf));
WEAK_LIBM_ALIAS(vrsa_divf, FN_PROTOTYPE(vrsa_divf));
WEAK_LIBM_ALIAS(vrsa_fmaxf, FN_PROTOTYPE(vrsa_fmaxf));
WEAK_LIBM_ALIAS(vrsa_fminf, FN_PROTOTYPE(vrsa_fminf));

WEAK_LIBM_ALIAS(vrsa_addfi, FN_PROTOTYPE(vrsa_addfi));
WEAK_LIBM_ALIAS(vrsa_subfi, FN_PROTOTYPE(vrsa_subfi));
WEAK_LIBM_ALIAS(vrsa_mulfi, FN_PROTOTYPE(vrsa_mulfi));
WEAK_LIBM_ALIAS(vrsa_divfi, FN_PROTOTYPE(vrsa_divfi));
WEAK_LIBM_ALIAS(vrsa_fmaxfi, FN_PROTOTYPE(vrsa_fmaxfi));
WEAK_LIBM_ALIAS(vrsa_fminfi, FN_PROTOTYPE(vrsa_fminfi));

WEAK_LIBM_ALIAS(vrda_cbrt, FN_PROTOTYPE(vrda_cbrt));
WEAK_LIBM_ALIAS(vrda_exp, FN_PROTOTYPE(vrda_exp));
WEAK_LIBM_ALIAS(vrda_exp2, FN_PROTOTYPE(vrda_exp2));
WEAK_LIBM_ALIAS(vrda_exp10, FN_PROTOTYPE(vrda_exp10));
WEAK_LIBM_ALIAS(vrda_expm1, FN_PROTOTYPE(vrda_expm1));
WEAK_LIBM_ALIAS(vrda_log, FN_PROTOTYPE(vrda_log));
WEAK_LIBM_ALIAS(vrda_log2, FN_PROTOTYPE(vrda_log2));
WEAK_LIBM_ALIAS(vrda_log10, FN_PROTOTYPE(vrda_log10));
WEAK_LIBM_ALIAS(vrda_log1p, FN_PROTOTYPE(vrda_log1p));
WEAK_LIBM_ALIAS(vrda_sin, FN_PROTOTYPE(vrda_sin));
WEAK_LIBM_ALIAS(vrda_cos, FN_PROTOTYPE(vrda_cos));

WEAK_LIBM_ALIAS(vrda_add, FN_PROTOTYPE(vrda_add));
WEAK_LIBM_ALIAS(vrda_sub, FN_PROTOTYPE(vrda_sub));
WEAK_LIBM_ALIAS(vrda_mul, FN_PROTOTYPE(vrda_mul));
WEAK_LIBM_ALIAS(vrda_div, FN_PROTOTYPE(vrda_div));
WEAK_LIBM_ALIAS(vrda_fmax, FN_PROTOTYPE(vrda_fmax));
WEAK_LIBM_ALIAS(vrda_fmin, FN_PROTOTYPE(vrda_fmin));

WEAK_LIBM_ALIAS(vrda_addi, FN_PROTOTYPE(vrda_addi));
WEAK_LIBM_ALIAS(vrda_subi, FN_PROTOTYPE(vrda_subi));
WEAK_LIBM_ALIAS(vrda_muli, FN_PROTOTYPE(vrda_muli));
WEAK_LIBM_ALIAS(vrda_divi, FN_PROTOTYPE(vrda_divi));
WEAK_LIBM_ALIAS(vrda_fmaxi, FN_PROTOTYPE(vrda_fmaxi));
WEAK_LIBM_ALIAS(vrda_fmini, FN_PROTOTYPE(vrda_fmini));


/* Vector Functions */

WEAK_LIBM_ALIAS(vrd2_pow, FN_PROTOTYPE(vrd2_pow));
WEAK_LIBM_ALIAS(vrd2_powx, FN_PROTOTYPE(vrd2_powx));
WEAK_LIBM_ALIAS(vrd2_cbrt, FN_PROTOTYPE(vrd2_cbrt));
WEAK_LIBM_ALIAS(vrd2_exp, FN_PROTOTYPE(vrd2_exp));
WEAK_LIBM_ALIAS(vrd2_exp2, FN_PROTOTYPE(vrd2_exp2));
WEAK_LIBM_ALIAS(vrd2_exp10, FN_PROTOTYPE(vrd2_exp10));
WEAK_LIBM_ALIAS(vrd2_log, FN_PROTOTYPE(vrd2_log));
WEAK_LIBM_ALIAS(vrd2_log2, FN_PROTOTYPE(vrd2_log2));
WEAK_LIBM_ALIAS(vrd2_log10, FN_PROTOTYPE(vrd2_log10));
WEAK_LIBM_ALIAS(vrd2_log1p, FN_PROTOTYPE(vrd2_log1p));
WEAK_LIBM_ALIAS(vrd2_sin, FN_PROTOTYPE(vrd2_sin));
WEAK_LIBM_ALIAS(vrd2_cos, FN_PROTOTYPE(vrd2_cos));
WEAK_LIBM_ALIAS(vrd2_tan, FN_PROTOTYPE(vrd2_tan));
WEAK_LIBM_ALIAS(vrd2_cosh, FN_PROTOTYPE(vrd2_cosh));
WEAK_LIBM_ALIAS(vrd2_atan, FN_PROTOTYPE(vrd2_atan));
WEAK_LIBM_ALIAS(vrd2_erf, FN_PROTOTYPE(vrd2_erf));
WEAK_LIBM_ALIAS(vrd2_sqrt, FN_PROTOTYPE(vrd2_sqrt));

WEAK_LIBM_ALIAS(vrd4_pow, FN_PROTOTYPE(vrd4_pow));
WEAK_LIBM_ALIAS(vrd4_exp, FN_PROTOTYPE(vrd4_exp));
WEAK_LIBM_ALIAS(vrd4_exp2, FN_PROTOTYPE(vrd4_exp2));
WEAK_LIBM_ALIAS(vrd4_log, FN_PROTOTYPE(vrd4_log));
WEAK_LIBM_ALIAS(vrd4_log2, FN_PROTOTYPE(vrd4_log2));
WEAK_LIBM_ALIAS(vrd4_sin, FN_PROTOTYPE(vrd4_sin));
WEAK_LIBM_ALIAS(vrd4_cos, FN_PROTOTYPE(vrd4_cos));
WEAK_LIBM_ALIAS(vrd4_tan, FN_PROTOTYPE(vrd4_tan));
WEAK_LIBM_ALIAS(vrd4_atan, FN_PROTOTYPE(vrd4_atan));
WEAK_LIBM_ALIAS(vrd4_sincos, FN_PROTOTYPE(vrd4_sincos));
WEAK_LIBM_ALIAS(vrd4_erf, FN_PROTOTYPE(vrd4_erf));
WEAK_LIBM_ALIAS(vrd4_sqrt, FN_PROTOTYPE(vrd4_sqrt));

WEAK_LIBM_ALIAS(vrd8_pow, FN_PROTOTYPE(vrd8_pow));
WEAK_LIBM_ALIAS(vrd8_exp, FN_PROTOTYPE(vrd8_exp));
WEAK_LIBM_ALIAS(vrd8_exp2, FN_PROTOTYPE(vrd8_exp2));
WEAK_LIBM_ALIAS(vrd8_log, FN_PROTOTYPE(vrd8_log));
WEAK_LIBM_ALIAS(vrd8_log2, FN_PROTOTYPE(vrd8_log2));
WEAK_LIBM_ALIAS(vrd8_sin, FN_PROTOTYPE(vrd8_sin));
WEAK_LIBM_ALIAS(vrd8_cos, FN_PROTOTYPE(vrd8_cos));
WEAK_LIBM_ALIAS(vrd8_tan, FN_PROTOTYPE(vrd8_tan));
WEAK_LIBM_ALIAS(vrd8_asin, FN_PROTOTYPE(vrd8_asin));
WEAK_LIBM_ALIAS(vrd8_atan, FN_PROTOTYPE(vrd8_atan));
WEAK_LIBM_ALIAS(vrd8_sincos, FN_PROTOTYPE(vrd8_sincos));
WEAK_LIBM_ALIAS(vrd8_erf, FN_PROTOTYPE(vrd8_erf));

WEAK_LIBM_ALIAS(vrs4_powf, FN_PROTOTYPE(vrs4_powf));
WEAK_LIBM_ALIAS(vrs4_cbrtf, FN_PROTOTYPE(vrs4_cbrtf));
WEAK_LIBM_ALIAS(vrs4_expf, FN_PROTOTYPE(vrs4_expf));
WEAK_LIBM_ALIAS(vrs4_exp2f, FN_PROTOTYPE(vrs4_exp2f));
WEAK_LIBM_ALIAS(vrs4_exp10f, FN_PROTOTYPE(vrs4_exp10f));
WEAK_LIBM_ALIAS(vrs4_expm1f, FN_PROTOTYPE(vrs4_expm1f));
WEAK_LIBM_ALIAS(vrs4_logf, FN_PROTOTYPE(vrs4_logf));
WEAK_LIBM_ALIAS(vrs4_log2f, FN_PROTOTYPE(vrs4_log2f));
WEAK_LIBM_ALIAS(vrs4_log10f, FN_PROTOTYPE(vrs4_log10f));
WEAK_LIBM_ALIAS(vrs4_log1pf, FN_PROTOTYPE(vrs4_log1pf));
WEAK_LIBM_ALIAS(vrs4_sinf, FN_PROTOTYPE(vrs4_sinf));
WEAK_LIBM_ALIAS(vrs4_cosf, FN_PROTOTYPE(vrs4_cosf));
WEAK_LIBM_ALIAS(vrs4_tanf, FN_PROTOTYPE(vrs4_tanf));
WEAK_LIBM_ALIAS(vrs4_coshf, FN_PROTOTYPE(vrs4_coshf));
WEAK_LIBM_ALIAS(vrs4_tanhf, FN_PROTOTYPE(vrs4_tanhf));
WEAK_LIBM_ALIAS(vrs4_asinf, FN_PROTOTYPE(vrs4_asinf));
WEAK_LIBM_ALIAS(vrs4_acosf, FN_PROTOTYPE(vrs4_acosf));
WEAK_LIBM_ALIAS(vrs4_atanf, FN_PROTOTYPE(vrs4_atanf));
WEAK_LIBM_ALIAS(vrs4_erff, FN_PROTOTYPE(vrs4_erff));

WEAK_LIBM_ALIAS(vrs8_powf, FN_PROTOTYPE(vrs8_powf));
WEAK_LIBM_ALIAS(vrs8_expf, FN_PROTOTYPE(vrs8_expf));
WEAK_LIBM_ALIAS(vrs8_exp2f, FN_PROTOTYPE(vrs8_exp2f));
WEAK_LIBM_ALIAS(vrs8_logf, FN_PROTOTYPE(vrs8_logf));
WEAK_LIBM_ALIAS(vrs8_log2f, FN_PROTOTYPE(vrs8_log2f));
WEAK_LIBM_ALIAS(vrs8_log10f, FN_PROTOTYPE(vrs8_log10f));
WEAK_LIBM_ALIAS(vrs8_sinf, FN_PROTOTYPE(vrs8_sinf));
WEAK_LIBM_ALIAS(vrs8_cosf, FN_PROTOTYPE(vrs8_cosf));
WEAK_LIBM_ALIAS(vrs8_tanf, FN_PROTOTYPE(vrs8_tanf));
WEAK_LIBM_ALIAS(vrs8_coshf, FN_PROTOTYPE(vrs8_coshf));
WEAK_LIBM_ALIAS(vrs8_tanhf, FN_PROTOTYPE(vrs8_tanhf));
WEAK_LIBM_ALIAS(vrs8_asinf, FN_PROTOTYPE(vrs8_asinf));
WEAK_LIBM_ALIAS(vrs8_acosf, FN_PROTOTYPE(vrs8_acosf));
WEAK_LIBM_ALIAS(vrs8_atanf, FN_PROTOTYPE(vrs8_atanf));
WEAK_LIBM_ALIAS(vrs8_erff, FN_PROTOTYPE(vrs8_erff));

WEAK_LIBM_ALIAS(vrs16_powf, FN_PROTOTYPE(vrs16_powf));
WEAK_LIBM_ALIAS(vrs16_expf, FN_PROTOTYPE(vrs16_expf));
WEAK_LIBM_ALIAS(vrs16_exp2f, FN_PROTOTYPE(vrs16_exp2f));
WEAK_LIBM_ALIAS(vrs16_logf, FN_PROTOTYPE(vrs16_logf));
WEAK_LIBM_ALIAS(vrs16_log2f, FN_PROTOTYPE(vrs16_log2f));
WEAK_LIBM_ALIAS(vrs16_log10f, FN_PROTOTYPE(vrs16_log10f));
WEAK_LIBM_ALIAS(vrs16_sinf, FN_PROTOTYPE(vrs16_sinf));
WEAK_LIBM_ALIAS(vrs16_cosf, FN_PROTOTYPE(vrs16_cosf));
WEAK_LIBM_ALIAS(vrs16_tanf, FN_PROTOTYPE(vrs16_tanf));
WEAK_LIBM_ALIAS(vrs16_tanhf, FN_PROTOTYPE(vrs16_tanhf));
WEAK_LIBM_ALIAS(vrs16_asinf, FN_PROTOTYPE(vrs16_asinf));
WEAK_LIBM_ALIAS(vrs16_acosf, FN_PROTOTYPE(vrs16_acosf));
WEAK_LIBM_ALIAS(vrs16_atanf, FN_PROTOTYPE(vrs16_atanf));
WEAK_LIBM_ALIAS(vrs16_erff, FN_PROTOTYPE(vrs16_erff));



/*
 * SWLCSG-2283 & CPUPL-4422 - Weak Aliases with "__" prefix in order
 * to support GLIBC's "-mveclibabi=acml" option.
 *
 * WEAK_LIBM_ALIAS is used to map "amd_<func_name>" to "__<func_name>".
 *
 * GLIBC internally identifies following 13 AOCL Vector functions with "__" prefix:
 *
 * __vrd2_sin, __vrd2_cos, __vrd2_exp, __vrd2_log, __vrd2_log2, __vrd2_log10,
 * __vrs4_sinf, __vrs4_cosf, __vrs4_expf, __vrs4_logf, __vrs4_log2f, __vrs4_log10f
 * and __vrs4_powf
 */

WEAK_LIBM_ALIAS(__vrd2_sin, FN_PROTOTYPE(vrd2_sin));
WEAK_LIBM_ALIAS(__vrd2_cos, FN_PROTOTYPE(vrd2_cos));
WEAK_LIBM_ALIAS(__vrd2_exp, FN_PROTOTYPE(vrd2_exp));
WEAK_LIBM_ALIAS(__vrd2_log, FN_PROTOTYPE(vrd2_log));
WEAK_LIBM_ALIAS(__vrd2_log2, FN_PROTOTYPE(vrd2_log2));
WEAK_LIBM_ALIAS(__vrd2_log10, FN_PROTOTYPE(vrd2_log10));

WEAK_LIBM_ALIAS(__vrs4_sinf, FN_PROTOTYPE(vrs4_sinf));
WEAK_LIBM_ALIAS(__vrs4_cosf, FN_PROTOTYPE(vrs4_cosf));
WEAK_LIBM_ALIAS(__vrs4_expf, FN_PROTOTYPE(vrs4_expf));
WEAK_LIBM_ALIAS(__vrs4_logf, FN_PROTOTYPE(vrs4_logf));
WEAK_LIBM_ALIAS(__vrs4_log2f, FN_PROTOTYPE(vrs4_log2f));
WEAK_LIBM_ALIAS(__vrs4_log10f, FN_PROTOTYPE(vrs4_log10f));

WEAK_LIBM_ALIAS(__vrs4_powf, FN_PROTOTYPE(vrs4_powf));