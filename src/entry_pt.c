/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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
 * OR PROFITS; OR BUSINESS INTERRUPT_PTRION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/iface.h>
#include <libm/entry_pt.h>

#ifdef __cplusplus
extern "C"
{
#endif

alm_func_t        G_ENTRY_PT_PTR(acos);
alm_func_t        G_ENTRY_PT_PTR(acosh);
alm_func_t        G_ENTRY_PT_PTR(asin);
alm_func_t        G_ENTRY_PT_PTR(asinh);
alm_func_t        G_ENTRY_PT_PTR(atan2);
alm_func_t        G_ENTRY_PT_PTR(atan);
alm_func_t        G_ENTRY_PT_PTR(atanh);
alm_func_t        G_ENTRY_PT_PTR(cbrt);
alm_func_t        G_ENTRY_PT_PTR(ceil);
alm_func_t        G_ENTRY_PT_PTR(copysign);
alm_func_t        G_ENTRY_PT_PTR(cos);
alm_func_t        G_ENTRY_PT_PTR(cosh);
alm_func_t        G_ENTRY_PT_PTR(cospi);
alm_func_t        G_ENTRY_PT_PTR(exp10);
alm_func_t        G_ENTRY_PT_PTR(exp2);
alm_func_t        G_ENTRY_PT_PTR(exp);
alm_func_t        G_ENTRY_PT_PTR(expm1);
alm_func_t        G_ENTRY_PT_PTR(fabs);
alm_func_t        G_ENTRY_PT_PTR(fdim);
alm_func_t        G_ENTRY_PT_PTR(floor);
alm_func_t        G_ENTRY_PT_PTR(fma);
alm_func_t        G_ENTRY_PT_PTR(fmax);
alm_func_t        G_ENTRY_PT_PTR(fmin);
alm_func_t        G_ENTRY_PT_PTR(fmod);
alm_func_t        G_ENTRY_PT_PTR(frexp);
alm_func_t        G_ENTRY_PT_PTR(hypot);
alm_func_t        G_ENTRY_PT_PTR(ldexp);
alm_func_t        G_ENTRY_PT_PTR(log10);
alm_func_t        G_ENTRY_PT_PTR(log1p);
alm_func_t        G_ENTRY_PT_PTR(log2);
alm_func_t        G_ENTRY_PT_PTR(logb);
alm_func_t        G_ENTRY_PT_PTR(log);
alm_func_t        G_ENTRY_PT_PTR(modf);
alm_func_t        G_ENTRY_PT_PTR(nan);
alm_func_t        G_ENTRY_PT_PTR(nearbyint);
alm_func_t        G_ENTRY_PT_PTR(nextafter);
alm_func_t        G_ENTRY_PT_PTR(nexttoward);
alm_func_t        G_ENTRY_PT_PTR(pow);
alm_func_t        G_ENTRY_PT_PTR(remainder);
alm_func_t        G_ENTRY_PT_PTR(remquo);
alm_func_t        G_ENTRY_PT_PTR(rint);
alm_func_t        G_ENTRY_PT_PTR(round);
alm_func_t        G_ENTRY_PT_PTR(scalbln);
alm_func_t        G_ENTRY_PT_PTR(scalbn);
alm_func_t        G_ENTRY_PT_PTR(sin);
alm_func_t        G_ENTRY_PT_PTR(sinh);
alm_func_t        G_ENTRY_PT_PTR(sinpi);
alm_func_t        G_ENTRY_PT_PTR(sqrt);
alm_func_t        G_ENTRY_PT_PTR(tan);
alm_func_t        G_ENTRY_PT_PTR(tanh);
alm_func_t        G_ENTRY_PT_PTR(tanpi);
alm_func_t        G_ENTRY_PT_PTR(trunc);
alm_func_t        G_ENTRY_PT_PTR(erf);
  
/*
 * required by NAG tests
 */
alm_func_t        G_ENTRY_PT_PTR(remainder_piby2);
alm_func_t        G_ENTRY_PT_PTR(remainder_piby2d2f);

/*
 * Single Precision functions
 */
alm_func_t        G_ENTRY_PT_PTR(acosf);
alm_func_t        G_ENTRY_PT_PTR(acoshf);
alm_func_t        G_ENTRY_PT_PTR(asinf);
alm_func_t        G_ENTRY_PT_PTR(asinhf);
alm_func_t        G_ENTRY_PT_PTR(atan2f);
alm_func_t        G_ENTRY_PT_PTR(atanf);
alm_func_t        G_ENTRY_PT_PTR(atanhf);
alm_func_t        G_ENTRY_PT_PTR(cbrtf);
alm_func_t        G_ENTRY_PT_PTR(ceilf);
alm_func_t        G_ENTRY_PT_PTR(copysignf);
alm_func_t        G_ENTRY_PT_PTR(cosf);
alm_func_t        G_ENTRY_PT_PTR(coshf);
alm_func_t        G_ENTRY_PT_PTR(cospif);
alm_func_t        G_ENTRY_PT_PTR(exp10f);
alm_func_t        G_ENTRY_PT_PTR(exp2f);
alm_func_t        G_ENTRY_PT_PTR(expf);
alm_func_t        G_ENTRY_PT_PTR(expm1f);
alm_func_t        G_ENTRY_PT_PTR(fabsf);
alm_func_t        G_ENTRY_PT_PTR(fdimf);
alm_func_t        G_ENTRY_PT_PTR(floorf);
alm_func_t        G_ENTRY_PT_PTR(fmaf);
alm_func_t        G_ENTRY_PT_PTR(fmaxf);
alm_func_t        G_ENTRY_PT_PTR(fminf);
alm_func_t        G_ENTRY_PT_PTR(fmodf);
alm_func_t        G_ENTRY_PT_PTR(frexpf);
alm_func_t        G_ENTRY_PT_PTR(hypotf);
alm_func_t        G_ENTRY_PT_PTR(ldexpf);
alm_func_t        G_ENTRY_PT_PTR(log10f);
alm_func_t        G_ENTRY_PT_PTR(log1pf);
alm_func_t        G_ENTRY_PT_PTR(log2f);
alm_func_t        G_ENTRY_PT_PTR(logbf);
alm_func_t        G_ENTRY_PT_PTR(logf);
alm_func_t        G_ENTRY_PT_PTR(modff);
alm_func_t        G_ENTRY_PT_PTR(nanf);
alm_func_t        G_ENTRY_PT_PTR(nearbyintf);
alm_func_t        G_ENTRY_PT_PTR(nextafterf);
alm_func_t        G_ENTRY_PT_PTR(nexttowardf);
alm_func_t        G_ENTRY_PT_PTR(powf);
alm_func_t        G_ENTRY_PT_PTR(remainderf);
alm_func_t        G_ENTRY_PT_PTR(remquof);
alm_func_t        G_ENTRY_PT_PTR(rintf);
alm_func_t        G_ENTRY_PT_PTR(roundf);
alm_func_t        G_ENTRY_PT_PTR(scalblnf);
alm_func_t        G_ENTRY_PT_PTR(scalbnf);
alm_func_t        G_ENTRY_PT_PTR(sinf);
alm_func_t        G_ENTRY_PT_PTR(sinhf);
alm_func_t        G_ENTRY_PT_PTR(sinpif);
alm_func_t        G_ENTRY_PT_PTR(sqrtf);
alm_func_t        G_ENTRY_PT_PTR(tanf);
alm_func_t        G_ENTRY_PT_PTR(tanhf);
alm_func_t        G_ENTRY_PT_PTR(tanpif);
alm_func_t        G_ENTRY_PT_PTR(truncf);
alm_func_t        G_ENTRY_PT_PTR(erff);

/*
 * Complex variants
 */
alm_func_t        G_ENTRY_PT_PTR(cexpf);
alm_func_t        G_ENTRY_PT_PTR(cexp);
alm_func_t        G_ENTRY_PT_PTR(clogf);
alm_func_t        G_ENTRY_PT_PTR(clog);
alm_func_t        G_ENTRY_PT_PTR(cpowf);
alm_func_t        G_ENTRY_PT_PTR(cpow);

/*
 * Integer variants
 */
alm_func_t        G_ENTRY_PT_PTR(finite);
alm_func_t        G_ENTRY_PT_PTR(finitef);
alm_func_t        G_ENTRY_PT_PTR(ilogb);
alm_func_t        G_ENTRY_PT_PTR(ilogbf);
alm_func_t        G_ENTRY_PT_PTR(lrint);
alm_func_t        G_ENTRY_PT_PTR(lrintf);
alm_func_t        G_ENTRY_PT_PTR(lround);
alm_func_t        G_ENTRY_PT_PTR(lroundf);
alm_func_t        G_ENTRY_PT_PTR(llrint);
alm_func_t        G_ENTRY_PT_PTR(llrintf);
alm_func_t        G_ENTRY_PT_PTR(llround);
alm_func_t        G_ENTRY_PT_PTR(llroundf);

alm_func_t        G_ENTRY_PT_PTR(vrda_cbrt);
alm_func_t        G_ENTRY_PT_PTR(vrda_cos);
alm_func_t        G_ENTRY_PT_PTR(vrda_exp);
alm_func_t        G_ENTRY_PT_PTR(vrda_exp2);
alm_func_t        G_ENTRY_PT_PTR(vrda_exp10);
alm_func_t        G_ENTRY_PT_PTR(vrda_expm1);
alm_func_t        G_ENTRY_PT_PTR(vrda_log);
alm_func_t        G_ENTRY_PT_PTR(vrda_log10);
alm_func_t        G_ENTRY_PT_PTR(vrda_log1p);
alm_func_t        G_ENTRY_PT_PTR(vrda_log2);
alm_func_t        G_ENTRY_PT_PTR(vrda_sin);

alm_func_t        G_ENTRY_PT_PTR(vrda_add);
alm_func_t        G_ENTRY_PT_PTR(vrda_sub);
alm_func_t        G_ENTRY_PT_PTR(vrda_mul);
alm_func_t        G_ENTRY_PT_PTR(vrda_div);
alm_func_t        G_ENTRY_PT_PTR(vrda_fmax);
alm_func_t        G_ENTRY_PT_PTR(vrda_fmin);

alm_func_t        G_ENTRY_PT_PTR(vrda_addi);
alm_func_t        G_ENTRY_PT_PTR(vrda_subi);
alm_func_t        G_ENTRY_PT_PTR(vrda_muli);
alm_func_t        G_ENTRY_PT_PTR(vrda_divi);
alm_func_t        G_ENTRY_PT_PTR(vrda_fmaxi);
alm_func_t        G_ENTRY_PT_PTR(vrda_fmini);

alm_func_t        G_ENTRY_PT_PTR(vrsa_cosf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_expf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_exp2f);
alm_func_t        G_ENTRY_PT_PTR(vrsa_exp10f);
alm_func_t        G_ENTRY_PT_PTR(vrsa_expm1f);
alm_func_t        G_ENTRY_PT_PTR(vrsa_log1pf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_sinf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_log2f);
alm_func_t        G_ENTRY_PT_PTR(vrsa_cbrtf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_log10f);
alm_func_t        G_ENTRY_PT_PTR(vrsa_logf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_powf);

alm_func_t        G_ENTRY_PT_PTR(vrsa_addf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_subf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_mulf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_divf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_fmaxf);
alm_func_t        G_ENTRY_PT_PTR(vrsa_fminf);

alm_func_t        G_ENTRY_PT_PTR(vrsa_addfi);
alm_func_t        G_ENTRY_PT_PTR(vrsa_subfi);
alm_func_t        G_ENTRY_PT_PTR(vrsa_mulfi);
alm_func_t        G_ENTRY_PT_PTR(vrsa_divfi);
alm_func_t        G_ENTRY_PT_PTR(vrsa_fmaxfi);
alm_func_t        G_ENTRY_PT_PTR(vrsa_fminfi);

alm_func_t        G_ENTRY_PT_PTR(sincos);
alm_func_t        G_ENTRY_PT_PTR(sincosf);

alm_func_t        G_ENTRY_PT_PTR(vrs4_acosf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_cosf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_cbrtf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_expf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_exp2f);
alm_func_t        G_ENTRY_PT_PTR(vrs4_exp10f);
alm_func_t        G_ENTRY_PT_PTR(vrs4_expm1f);
alm_func_t        G_ENTRY_PT_PTR(vrs4_logf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_log2f);
alm_func_t        G_ENTRY_PT_PTR(vrs4_log10f);
alm_func_t        G_ENTRY_PT_PTR(vrs4_log1pf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_sinf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_tanf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_coshf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_tanhf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_powf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_asinf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_atanf);
alm_func_t        G_ENTRY_PT_PTR(vrs4_erff);

alm_func_t        G_ENTRY_PT_PTR(vrs8_logf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_log10f);
alm_func_t        G_ENTRY_PT_PTR(vrs8_expf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_exp2f);
alm_func_t        G_ENTRY_PT_PTR(vrs8_sinf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_tanf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_atanf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_cosf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_acosf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_coshf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_tanhf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_powf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_log2f);
alm_func_t        G_ENTRY_PT_PTR(vrs8_asinf);
alm_func_t        G_ENTRY_PT_PTR(vrs8_erff);

alm_func_t        G_ENTRY_PT_PTR(vrs16_logf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_expf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_erff);
alm_func_t        G_ENTRY_PT_PTR(vrs16_sinf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_asinf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_exp2f);
alm_func_t        G_ENTRY_PT_PTR(vrs16_tanf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_atanf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_cosf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_acosf);
//alm_func_t        G_ENTRY_PT_PTR(vrs16_coshf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_tanhf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_powf);
alm_func_t        G_ENTRY_PT_PTR(vrs16_log2f);
alm_func_t        G_ENTRY_PT_PTR(vrs16_log10f);

alm_func_t        G_ENTRY_PT_PTR(vrd2_cbrt);
alm_func_t        G_ENTRY_PT_PTR(vrd2_cos);
alm_func_t        G_ENTRY_PT_PTR(vrd2_cosh);
alm_func_t        G_ENTRY_PT_PTR(vrd2_exp);
alm_func_t        G_ENTRY_PT_PTR(vrd2_exp2);
alm_func_t        G_ENTRY_PT_PTR(vrd2_exp10);
alm_func_t        G_ENTRY_PT_PTR(vrd2_log);
alm_func_t        G_ENTRY_PT_PTR(vrd2_log2);
alm_func_t        G_ENTRY_PT_PTR(vrd2_log10);
alm_func_t        G_ENTRY_PT_PTR(vrd2_log1p);
alm_func_t        G_ENTRY_PT_PTR(vrd2_pow);
alm_func_t        G_ENTRY_PT_PTR(vrd2_sin);
alm_func_t        G_ENTRY_PT_PTR(vrd2_tan);
alm_func_t        G_ENTRY_PT_PTR(vrd2_atan);
alm_func_t        G_ENTRY_PT_PTR(vrd2_erf);

alm_func_t        G_ENTRY_PT_PTR(vrd4_cbrt);
alm_func_t        G_ENTRY_PT_PTR(vrd4_cos);
alm_func_t        G_ENTRY_PT_PTR(vrd4_exp);
alm_func_t        G_ENTRY_PT_PTR(vrd4_exp2);
alm_func_t        G_ENTRY_PT_PTR(vrd4_exp10);
alm_func_t        G_ENTRY_PT_PTR(vrd4_log);
alm_func_t        G_ENTRY_PT_PTR(vrd4_log2);
alm_func_t        G_ENTRY_PT_PTR(vrd4_log10);
alm_func_t        G_ENTRY_PT_PTR(vrd4_log1p);
alm_func_t        G_ENTRY_PT_PTR(vrd4_pow);
alm_func_t        G_ENTRY_PT_PTR(vrd4_sin);
alm_func_t        G_ENTRY_PT_PTR(vrd4_tan);
alm_func_t        G_ENTRY_PT_PTR(vrd4_atan);
alm_func_t        G_ENTRY_PT_PTR(vrd4_erf);
alm_func_t        G_ENTRY_PT_PTR(vrd4_sincos);

//alm_func_t        G_ENTRY_PT_PTR(vrd8_cbrt);
alm_func_t        G_ENTRY_PT_PTR(vrd8_cos);
alm_func_t        G_ENTRY_PT_PTR(vrd8_exp);
alm_func_t        G_ENTRY_PT_PTR(vrd8_exp2);
//alm_func_t        G_ENTRY_PT_PTR(vrd8_exp10);
//alm_func_t        G_ENTRY_PT_PTR(vrd8_expm1);
alm_func_t        G_ENTRY_PT_PTR(vrd8_log);
alm_func_t        G_ENTRY_PT_PTR(vrd8_log2);
//alm_func_t        G_ENTRY_PT_PTR(vrd8_log10);
//alm_func_t        G_ENTRY_PT_PTR(vrd8_log1p);
alm_func_t        G_ENTRY_PT_PTR(vrd8_pow);
alm_func_t        G_ENTRY_PT_PTR(vrd8_sin);
alm_func_t        G_ENTRY_PT_PTR(vrd8_asin);
alm_func_t        G_ENTRY_PT_PTR(vrd8_tan);
alm_func_t        G_ENTRY_PT_PTR(vrd8_atan);
alm_func_t        G_ENTRY_PT_PTR(vrd8_erf);
alm_func_t        G_ENTRY_PT_PTR(vrd8_sincos);

#ifdef __cplusplus
}
#endif

static void CONSTRUCTOR
init_map_entry_points(void)
{
    libm_iface_init();
}
