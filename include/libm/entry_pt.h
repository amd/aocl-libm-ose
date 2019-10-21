/*
 * Copyright (C) 2018,2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */
#ifndef __AMD_LIBM_ENTRY_PT_H__
#define __AMD_LIBM_ENTRY_PT_H__

#define G_ENTRY_PT(fn) (*g_amd_libm_ep_##fn)
#define G_ENTRY_PT_PTR(fn) g_amd_libm_ep_##fn

extern double G_ENTRY_PT(acos)(double x);
extern double G_ENTRY_PT(acosh)(double x);
extern double G_ENTRY_PT(asin)(double x);
extern double G_ENTRY_PT(asinh)(double x);
extern double G_ENTRY_PT(atan2)(double x, double y);
extern double G_ENTRY_PT(atan)(double x);
extern double G_ENTRY_PT(atanh)(double x);
extern double G_ENTRY_PT(cbrt)(double x);
extern double G_ENTRY_PT(ceil)(double x);
extern double G_ENTRY_PT(copysign)(double x, double y);
extern double G_ENTRY_PT(cos)(double x);
extern double G_ENTRY_PT(cosh)(double x);
extern double G_ENTRY_PT(cospi)(double x);
extern double G_ENTRY_PT(exp10)(double x);
extern double G_ENTRY_PT(exp2)(double x);
extern double G_ENTRY_PT(exp)(double x);
extern double G_ENTRY_PT(expm1)(double x);
extern double G_ENTRY_PT(fabs)(double x);
extern double G_ENTRY_PT(fastpow)(double x, double y);
extern double G_ENTRY_PT(fdim)(double x, double y);
extern double G_ENTRY_PT(floor)(double x);
extern double G_ENTRY_PT(fma)(double x, double y, double z);
extern double G_ENTRY_PT(fmax)(double x, double y);
extern double G_ENTRY_PT(fmin)(double x, double y);
extern double G_ENTRY_PT(fmod)(double x, double y);
extern double G_ENTRY_PT(frexp)(double value, int *exp);
extern double G_ENTRY_PT(hypot)(double x, double y);
extern double G_ENTRY_PT(ldexp)(double x, int exp);
extern double G_ENTRY_PT(log10)(double x);
extern double G_ENTRY_PT(log1p)(double x);
extern double G_ENTRY_PT(log2)(double x);
extern double G_ENTRY_PT(logb)(double x);
extern double G_ENTRY_PT(log)(double x);
extern double G_ENTRY_PT(modf)(double x, double *iptr);
extern double G_ENTRY_PT(nan)(const char *tagp);
extern double G_ENTRY_PT(nearbyint)(double x);
extern double G_ENTRY_PT(nextafter)(double x, double y);
extern double G_ENTRY_PT(nexttoward)(double x, long double y);
extern double G_ENTRY_PT(pow)(double x, double y);
extern double G_ENTRY_PT(remainder)(double x, double y);
extern double G_ENTRY_PT(remquo) (double x, double y, int *quo);
extern double G_ENTRY_PT(rint)(double x);
extern double G_ENTRY_PT(round)(double f);
extern double G_ENTRY_PT(scalbln)(double x, long int n);
extern double G_ENTRY_PT(scalbn)(double x, int n);
extern double G_ENTRY_PT(sin)(double x);
extern double G_ENTRY_PT(sinh)(double x);
extern double G_ENTRY_PT(sinpi)(double x);
extern double G_ENTRY_PT(sqrt)(double x);
extern double G_ENTRY_PT(tan)(double x);
extern double G_ENTRY_PT(tanh)(double x);
extern double G_ENTRY_PT(tanpi)(double x);
extern double G_ENTRY_PT(trunc)(double x);

/*
 * Single Precision functions
 */
extern float G_ENTRY_PT(acosf)(float x);
extern float G_ENTRY_PT(acoshf)(float x);
extern float G_ENTRY_PT(asinf)(float x);
extern float G_ENTRY_PT(asinhf)(float x);
extern float G_ENTRY_PT(atan2f)(float x, float y);
extern float G_ENTRY_PT(atanf)(float x);
extern float G_ENTRY_PT(atanhf)(float x);
extern float G_ENTRY_PT(cbrtf)(float x);
extern float G_ENTRY_PT(ceilf)(float x);
extern float G_ENTRY_PT(copysignf)(float x, float y);
extern float G_ENTRY_PT(cosf)(float x);
extern float G_ENTRY_PT(coshf)(float fx);
extern float G_ENTRY_PT(cospif)(float x);
extern float G_ENTRY_PT(exp10f)(float x);
extern float G_ENTRY_PT(exp2f)(float x);
extern float G_ENTRY_PT(expf)(float x);
extern float G_ENTRY_PT(expm1f)(float x);
extern float G_ENTRY_PT(fabsf)(float x);
extern float G_ENTRY_PT(fdimf)(float x, float y);
extern float G_ENTRY_PT(floorf)(float x);
extern float G_ENTRY_PT(fmaf)(float x, float y, float z);
extern float G_ENTRY_PT(fmaxf)(float x, float y);
extern float G_ENTRY_PT(fminf)(float x, float y);
extern float G_ENTRY_PT(fmodf)(float x, float y);
extern float G_ENTRY_PT(frexpf)(float value, int *exp);
extern float G_ENTRY_PT(hypotf)(float x, float y);
extern float G_ENTRY_PT(ldexpf)(float x, int exp);
extern float G_ENTRY_PT(log10f)(float x);
extern float G_ENTRY_PT(log1pf)(float x);
extern float G_ENTRY_PT(log2f)(float x);
extern float G_ENTRY_PT(logbf)(float x);
extern float G_ENTRY_PT(logf)(float x);
extern float G_ENTRY_PT(modff)(float x, float *iptr);
extern float G_ENTRY_PT(nanf)(const char *tagp);
extern float G_ENTRY_PT(nearbyintf)(float x);
extern float G_ENTRY_PT(nextafterf)(float x, float y);
extern float G_ENTRY_PT(nexttowardf)(float x, long double y);
extern float G_ENTRY_PT(powf)(float x, float y);
extern float G_ENTRY_PT(remainderf)(float x, float y);
extern float G_ENTRY_PT(remquof) (float x, float y, int *quo);
extern float G_ENTRY_PT(rintf)(float x);
extern float G_ENTRY_PT(roundf)(float f);
extern float G_ENTRY_PT(scalblnf)(float x, long int n);
extern float G_ENTRY_PT(scalbnf)(float x, int n);
extern float G_ENTRY_PT(sinf)(float x);
extern float G_ENTRY_PT(sinhf)(float x);
extern float G_ENTRY_PT(sinpif)(float x);
extern float G_ENTRY_PT(sqrtf)(float x);
extern float G_ENTRY_PT(tanf)(float x);
extern float G_ENTRY_PT(tanhf)(float x);
extern float G_ENTRY_PT(tanpif)(float x);
extern float G_ENTRY_PT(truncf)(float x);

/*
 * Integer variants
 */
extern int G_ENTRY_PT(finite)(double x);
extern int G_ENTRY_PT(finitef)(float x);
extern int G_ENTRY_PT(ilogb)(double x);
extern int G_ENTRY_PT(ilogbf)(float x);
extern long int G_ENTRY_PT(lrint)(double x);
extern long int G_ENTRY_PT(lrintf)(float x);
extern long int G_ENTRY_PT(lround)(double d);
extern long int G_ENTRY_PT(lroundf)(float f);
extern long long int G_ENTRY_PT(llrint)(double x);
extern long long int G_ENTRY_PT(llrintf)(float x);
extern long long int G_ENTRY_PT(llround)(double d);
extern long long int G_ENTRY_PT(llroundf)(float f);
 
extern void    G_ENTRY_PT(vrda_exp)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_exp2)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_exp10)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_expm1)(int, double*, double*);
extern void    G_ENTRY_PT(vrsa_expf)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_exp2f)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_exp10f)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_expm1f)(int, float*, float*);
extern void    G_ENTRY_PT(vrda_cbrt)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_log)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_log10)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_log1p)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_log2)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_cos)(int, double*, double*);
extern void    G_ENTRY_PT(vrda_sin)(int, double*, double*);

extern void    G_ENTRY_PT(vrsa_cosf)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_log1pf)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_sinf)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_log2f)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_cbrtf)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_log10f)(int, float*, float*);
extern void    G_ENTRY_PT(vrsa_logf)(int, float*, float*);

#ifndef _MSC_VER
extern void  G_ENTRY_PT(sincos)(double,double *,double *);
extern void  G_ENTRY_PT(sincosf)(float,float *,float *);
#endif

#include <immintrin.h>

extern __m128  G_ENTRY_PT(vrs4_expf)(__m128);
extern __m128  G_ENTRY_PT(vrs4_exp2f)(__m128);
extern __m128  G_ENTRY_PT(vrs4_exp10f)(__m128);
extern __m128  G_ENTRY_PT(vrs4_expm1f)(__m128);
extern __m128  G_ENTRY_PT(vrs4_logf)(__m128);
extern __m128  G_ENTRY_PT(vrs4_log2f)(__m128);
extern __m128  G_ENTRY_PT(vrs4_log10f)(__m128);
extern __m128  G_ENTRY_PT(vrs4_log1pf)(__m128);
extern __m128  G_ENTRY_PT(vrs4_cosf)(__m128);
extern __m128  G_ENTRY_PT(vrs4_sinf)(__m128);
extern __m128  G_ENTRY_PT(vrs4_tanf)(__m128);
extern __m128  G_ENTRY_PT(vrs4_cbrtf)(__m128);

extern __m128d G_ENTRY_PT(vrd2_exp)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_exp2)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_exp10)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_expm1)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_log)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_log2)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_log10)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_log1p)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_cbrt)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_cos)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_sin)(__m128d);
extern __m128d G_ENTRY_PT(vrd2_tan)(__m128d);

extern __m256d G_ENTRY_PT(vrd4_log)(__m256d);
extern __m256d G_ENTRY_PT(vrd4_exp)(__m256d);
extern __m256d G_ENTRY_PT(vrd4_exp2)(__m256d);

#endif	/* __AMD_LIBM_ENTRY_PT_H__ */

