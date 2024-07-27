/*
 * Copyright (C) 2018-2024, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __AMD_LIBM_ENTRY_PT_PTR_H__
#define __AMD_LIBM_ENTRY_PT_PTR_H__

#define G_ENTRY_PT(fn) (*g_amd_libm_ep_##fn)

#define G_ENTRY_PT_ASM(fn) g_amd_libm_ep_##fn

#define G_ENTRY_PT_PTR(fn) g_amd_libm_ep_##fn

#ifndef alm_ep_func_t
typedef void (*alm_ep_func_t)(void);
#endif

extern alm_ep_func_t        G_ENTRY_PT_PTR(acos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(acosh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(asin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(asinh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(atan2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(atan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(atanh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cbrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(ceil);
extern alm_ep_func_t        G_ENTRY_PT_PTR(copysign);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cosh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cospi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(exp10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(exp2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(exp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(expm1);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fabs);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fdim);
extern alm_ep_func_t        G_ENTRY_PT_PTR(floor);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fma);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fmax);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fmin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fmod);
extern alm_ep_func_t        G_ENTRY_PT_PTR(frexp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(hypot);
extern alm_ep_func_t        G_ENTRY_PT_PTR(ldexp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log1p);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(logb);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log);
extern alm_ep_func_t        G_ENTRY_PT_PTR(modf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nearbyint);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nextafter);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nexttoward);
extern alm_ep_func_t        G_ENTRY_PT_PTR(pow);
extern alm_ep_func_t        G_ENTRY_PT_PTR(remainder);
extern alm_ep_func_t        G_ENTRY_PT_PTR(remquo);
extern alm_ep_func_t        G_ENTRY_PT_PTR(rint);
extern alm_ep_func_t        G_ENTRY_PT_PTR(round);
extern alm_ep_func_t        G_ENTRY_PT_PTR(scalbln);
extern alm_ep_func_t        G_ENTRY_PT_PTR(scalbn);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sinh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sinpi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sqrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(tan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(tanh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(tanpi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(trunc);
extern alm_ep_func_t        G_ENTRY_PT_PTR(erf);

/*
 * Single Precision functions
 */
extern alm_ep_func_t        G_ENTRY_PT_PTR(acosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(acoshf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(asinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(asinhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(atan2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(atanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(atanhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cbrtf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(ceilf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(copysignf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(coshf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cospif);
extern alm_ep_func_t        G_ENTRY_PT_PTR(exp10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(exp2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(expf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(expm1f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fabsf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fdimf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(floorf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fmaf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fmaxf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fminf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(fmodf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(frexpf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(hypotf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(ldexpf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log1pf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(log2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(logbf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(logf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(modff);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nearbyintf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nextafterf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(nexttowardf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(powf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(remainderf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(remquof);
extern alm_ep_func_t        G_ENTRY_PT_PTR(rintf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(roundf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(scalblnf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(scalbnf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sinhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sinpif);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sqrtf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(tanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(tanhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(tanpif);
extern alm_ep_func_t        G_ENTRY_PT_PTR(truncf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(erff);

/*
 * Complex variants
 */
extern alm_ep_func_t        G_ENTRY_PT_PTR(cexp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cexpf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(clog);
extern alm_ep_func_t        G_ENTRY_PT_PTR(clogf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cpow);
extern alm_ep_func_t        G_ENTRY_PT_PTR(cpowf);


/*
 * Integer variants
 */
extern alm_ep_func_t        G_ENTRY_PT_PTR(finite);
extern alm_ep_func_t        G_ENTRY_PT_PTR(finitef);
extern alm_ep_func_t        G_ENTRY_PT_PTR(ilogb);
extern alm_ep_func_t        G_ENTRY_PT_PTR(ilogbf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(lrint);
extern alm_ep_func_t        G_ENTRY_PT_PTR(lrintf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(lround);
extern alm_ep_func_t        G_ENTRY_PT_PTR(lroundf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(llrint);
extern alm_ep_func_t        G_ENTRY_PT_PTR(llrintf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(llround);
extern alm_ep_func_t        G_ENTRY_PT_PTR(llroundf);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_exp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_exp2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_exp10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_expm1);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_cbrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_log);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_log10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_log1p);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_log2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_cos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_sin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_pow);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_expf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_exp2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_exp10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_expm1f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_cosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_log1pf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_sinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_log2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_cbrtf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_log10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_logf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_powf);

extern alm_ep_func_t        G_ENTRY_PT_PTR(sincos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(sincosf);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_add);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_sub);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_mul);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_div);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_fmax);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_fmin);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_addf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_subf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_mulf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_divf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_fmaxf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_fminf);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_addi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_subi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_muli);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_divi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_fmaxi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_fmini);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_addfi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_subfi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_mulfi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_divfi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_fmaxfi);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_fminfi);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrda_fabs);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrsa_fabsf);

#include <immintrin.h>

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_acosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_expf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_exp2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_exp10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_expm1f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_logf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_powf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_log2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_log10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_log1pf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_cosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_coshf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_sinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_tanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_tanhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_cbrtf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_asinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_atanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_erff);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_fabsf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs4_sqrtf);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_cosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_acosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_coshf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_expf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_exp2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_log2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_log10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_logf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_sinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_tanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_atanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_tanhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_powf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_asinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_erff);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_fabsf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs8_sqrtf);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_cbrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_cos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_cosh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_exp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_exp2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_exp10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_expm1);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_log);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_log2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_log10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_log1p);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_pow);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_powx);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_sin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_tan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_atan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_erf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_sqrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_fabs);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd2_linearfrac);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_cbrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_cos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_cosh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_exp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_exp2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_exp10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_expm1);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_log);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_log2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_log10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_log1p);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_pow);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_powx);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_sin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_tan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_atan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_erf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_sincos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_sqrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_fabs);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd4_linearfrac);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_cbrt);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_cos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_cosh);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_exp);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_exp2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_exp10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_expm1);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_log);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_log2);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_log10);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_log1p);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_pow);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_sin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_asin);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_tan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_atan);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_erf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_sincos);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_linearfrac);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrd8_sqrt);

extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_expf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_powf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_exp2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_logf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_log2f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_log10f);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_sinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_asinf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_cosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_acosf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_tanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_atanf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_tanhf);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_erff);
extern alm_ep_func_t        G_ENTRY_PT_PTR(vrs16_sqrtf);


#endif	/* __AMD_LIBM_ENTRY_PT_PTR_H__ */

