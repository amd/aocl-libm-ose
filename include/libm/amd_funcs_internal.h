#ifndef __LIBM_AMD_FUNCS_INTERNAL_H__
#define __LIBM_AMD_FUNCS_INTERNAL_H__

#include <libm_macros.h>

/*
 * Copyright AMD Inc, 2008-2019, All rights reserved
 *
 * Author : Prem Mallappa <pmallapp@amd.com>
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*
* Double precision
*/
extern double FN_PROTOTYPE_FMA3(acos)(double x);
extern double FN_PROTOTYPE_FMA3(acosh)(double x);
extern double FN_PROTOTYPE_FMA3(asin)(double x);
extern double FN_PROTOTYPE_FMA3(asinh)(double x);
extern double FN_PROTOTYPE_FMA3(atan2)(double x, double y);
extern double FN_PROTOTYPE_FMA3(atan)(double x);
extern double FN_PROTOTYPE_FMA3(atanh)(double x);
extern double FN_PROTOTYPE_FMA3(cbrt)(double x);
extern double FN_PROTOTYPE_FMA3(ceil)(double x);
extern double FN_PROTOTYPE_FMA3(copysign)(double x, double y);
extern double FN_PROTOTYPE_FMA3(cos)(double x);
extern double FN_PROTOTYPE_FMA3(cosh)(double x);
extern double FN_PROTOTYPE_FMA3(cospi)(double x);
extern double FN_PROTOTYPE_FMA3(exp10)(double x);
extern double FN_PROTOTYPE_FMA3(exp2)(double x);
extern double FN_PROTOTYPE_FMA3(exp)(double x);
extern double FN_PROTOTYPE_FMA3(expm1)(double x);
extern double FN_PROTOTYPE_FMA3(fabs)(double x);
extern double FN_PROTOTYPE_FMA3(fastpow)(double x, double y);
extern double FN_PROTOTYPE_FMA3(fdim)(double x, double y);
extern double FN_PROTOTYPE_FMA3(floor)(double x);
extern double FN_PROTOTYPE_FMA3(fma)(double x, double y, double z);
extern double FN_PROTOTYPE_FMA3(fmax)(double x, double y);
extern double FN_PROTOTYPE_FMA3(fmin)(double x, double y);
extern double FN_PROTOTYPE_FMA3(fmod)(double x, double y);
extern double FN_PROTOTYPE_FMA3(frexp)(double value, int *exp);
extern double FN_PROTOTYPE_FMA3(hypot)(double x, double y);
extern double FN_PROTOTYPE_FMA3(ldexp)(double x, int exp);
extern double FN_PROTOTYPE_FMA3(log10)(double x);
extern double FN_PROTOTYPE_FMA3(log1p)(double x);
extern double FN_PROTOTYPE_FMA3(log2)(double x);
extern double FN_PROTOTYPE_FMA3(logb)(double x);
extern double FN_PROTOTYPE_FMA3(log)(double x);
extern double FN_PROTOTYPE_FMA3(modf)(double x, double *iptr);
extern double FN_PROTOTYPE_FMA3(nan)(const char *tagp);
extern double FN_PROTOTYPE_FMA3(nearbyint)(double x);
extern double FN_PROTOTYPE_FMA3(nextafter)(double x, double y);
extern double FN_PROTOTYPE_FMA3(nexttoward)(double x, long double y);
extern double FN_PROTOTYPE_FMA3(pow)(double x, double y);
extern double FN_PROTOTYPE_FMA3(remainder)(double x, double y);
extern double FN_PROTOTYPE_FMA3(remquo) (double x, double y, int *quo);
extern double FN_PROTOTYPE_FMA3(rint)(double x);
extern double FN_PROTOTYPE_FMA3(round)(double f);
extern double FN_PROTOTYPE_FMA3(scalbln)(double x, long int n);
extern double FN_PROTOTYPE_FMA3(scalbn)(double x, int n);
extern double FN_PROTOTYPE_FMA3(sin)(double x);
extern double FN_PROTOTYPE_FMA3(sinh)(double x);
extern double FN_PROTOTYPE_FMA3(sinpi)(double x);
extern double FN_PROTOTYPE_FMA3(sqrt)(double x);
extern double FN_PROTOTYPE_FMA3(tan)(double x);
extern double FN_PROTOTYPE_FMA3(tanh)(double x);
extern double FN_PROTOTYPE_FMA3(tanpi)(double x);
extern double FN_PROTOTYPE_FMA3(trunc)(double x);

/*

 * Single Precision functions
 */
extern float FN_PROTOTYPE_FMA3(acosf)(float x);
extern float FN_PROTOTYPE_FMA3(acoshf)(float x);
extern float FN_PROTOTYPE_FMA3(asinf)(float x);
extern float FN_PROTOTYPE_FMA3(asinhf)(float x);
extern float FN_PROTOTYPE_FMA3(atan2f)(float x, float y);
extern float FN_PROTOTYPE_FMA3(atanf)(float x);
extern float FN_PROTOTYPE_FMA3(atanhf)(float x);
extern float FN_PROTOTYPE_FMA3(cbrtf)(float x);
extern float FN_PROTOTYPE_FMA3(ceilf)(float x);
extern float FN_PROTOTYPE_FMA3(copysignf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(cosf)(float x);
extern float FN_PROTOTYPE_FMA3(coshf)(float fx);
extern float FN_PROTOTYPE_FMA3(cospif)(float x);
extern float FN_PROTOTYPE_FMA3(exp10f)(float x);
extern float FN_PROTOTYPE_FMA3(exp2f)(float x);
extern float FN_PROTOTYPE_FMA3(expf)(float x);
extern float FN_PROTOTYPE_FMA3(expm1f)(float x);
extern float FN_PROTOTYPE_FMA3(fabsf)(float x);
extern float FN_PROTOTYPE_FMA3(fdimf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(floorf)(float x);
extern float FN_PROTOTYPE_FMA3(fmaf)(float x, float y, float z);
extern float FN_PROTOTYPE_FMA3(fmaxf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(fminf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(fmodf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(frexpf)(float value, int *exp);
extern float FN_PROTOTYPE_FMA3(hypotf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(ldexpf)(float x, int exp);
extern float FN_PROTOTYPE_FMA3(log10f)(float x);
extern float FN_PROTOTYPE_FMA3(log1pf)(float x);
extern float FN_PROTOTYPE_FMA3(log2f)(float x);
extern float FN_PROTOTYPE_FMA3(logbf)(float x);
extern float FN_PROTOTYPE_FMA3(logf)(float x);
extern float FN_PROTOTYPE_FMA3(modff)(float x, float *iptr);
extern float FN_PROTOTYPE_FMA3(nanf)(const char *tagp);
extern float FN_PROTOTYPE_FMA3(nearbyintf)(float x);
extern float FN_PROTOTYPE_FMA3(nextafterf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(nexttowardf)(float x, long double y);
extern float FN_PROTOTYPE_FMA3(powf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(remainderf)(float x, float y);
extern float FN_PROTOTYPE_FMA3(remquof) (float x, float y, int *quo);
extern float FN_PROTOTYPE_FMA3(rintf)(float x);
extern float FN_PROTOTYPE_FMA3(roundf)(float f);
extern float FN_PROTOTYPE_FMA3(scalblnf)(float x, long int n);
extern float FN_PROTOTYPE_FMA3(scalbnf)(float x, int n);
extern float FN_PROTOTYPE_FMA3(sinf)(float x);
extern float FN_PROTOTYPE_FMA3(sinhf)(float x);
extern float FN_PROTOTYPE_FMA3(sinpif)(float x);
extern float FN_PROTOTYPE_FMA3(sqrtf)(float x);
extern float FN_PROTOTYPE_FMA3(tanf)(float x);
extern float FN_PROTOTYPE_FMA3(tanhf)(float x);
extern float FN_PROTOTYPE_FMA3(tanpif)(float x);
extern float FN_PROTOTYPE_FMA3(truncf)(float x);

/*
 * Integer variants
 */
extern int FN_PROTOTYPE_FMA3(finite)(double x);
extern int FN_PROTOTYPE_FMA3(finitef)(float x);
extern int FN_PROTOTYPE_FMA3(ilogb)(double x);
extern int FN_PROTOTYPE_FMA3(ilogbf)(float x);
extern long int FN_PROTOTYPE_FMA3(lrint)(double x);
extern long int FN_PROTOTYPE_FMA3(lrintf)(float x);
extern long int FN_PROTOTYPE_FMA3(lround)(double d);
extern long int FN_PROTOTYPE_FMA3(lroundf)(float f);
extern long long int FN_PROTOTYPE_FMA3(llrint)(double x);
extern long long int FN_PROTOTYPE_FMA3(llrintf)(float x);
extern long long int FN_PROTOTYPE_FMA3(llround)(double d);
extern long long int FN_PROTOTYPE_FMA3(llroundf)(float f);


#include <immintrin.h>

/*
 * Vector Single precision
 */
extern __m128 FN_PROTOTYPE_FMA3(vrs4_cbrtf)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_cosf)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_exp10f)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_exp2f)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_expf)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_expm1f)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_log10f)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_log1pf)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_log2f)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_logf)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_powxf)(__m128 x, float y);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_sinf)(__m128 x);
extern __m128 FN_PROTOTYPE_FMA3(vrs4_tanf)(__m128 x);

/*
 * Vector Single precision
 */
extern __m128d FN_PROTOTYPE_FMA3(vrd2_cbrt)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_cosh)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_cos)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_exp10)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_exp2)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_exp)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_expm1)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_log10)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_log1p)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_log2)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_log)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_pow)(__m128d x, __m128d y);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_sin)(__m128d x);
extern __m128d FN_PROTOTYPE_FMA3(vrd2_tan)(__m128d x);

/*
 * Vector double precision, 4 element
 */
extern __m256d FN_PROTOTYPE_FMA3(vrd4_exp)(__m256d x);
extern __m256d FN_PROTOTYPE_FMA3(vrd4_exp2)(__m256d x);
extern __m256d FN_PROTOTYPE_FMA3(vrd4_expm1)(__m256d x);
extern __m256d FN_PROTOTYPE_FMA3(vrd4_log)(__m256d x);
extern __m256d FN_PROTOTYPE_FMA3(vrd4_pow)(__m256d x, __m256d y);

extern void FN_PROTOTYPE_FMA3(sincos)(double x, double *s, double *c);
extern void FN_PROTOTYPE_FMA3(sincosf)(float x, float *s, float *c);
extern void FN_PROTOTYPE_FMA3(vrd2_sincos)(__m128d x, __m128d* ys, __m128d* yc);
extern void FN_PROTOTYPE_FMA3(vrs4_sincosf)(__m128 x, __m128* ys, __m128* yc);

/*
* Vector Array versions
*/
extern void FN_PROTOTYPE_FMA3(vrda_cbrt)(int len, double *src, double* dst );
extern void FN_PROTOTYPE_FMA3(vrda_cos)(int n, double *x, double *y);
extern void FN_PROTOTYPE_FMA3(vrda_exp10)(int len, double* x, double* y);
extern void FN_PROTOTYPE_FMA3(vrda_exp2)(int len, double* x, double* y);
extern void FN_PROTOTYPE_FMA3(vrda_exp)(int len, double* x, double* y);
extern void FN_PROTOTYPE_FMA3(vrda_expm1)(int len, double* x, double* y);
extern void FN_PROTOTYPE_FMA3(vrda_log10)(int len, double *src, double* dst );
extern void FN_PROTOTYPE_FMA3(vrda_log1p)(int len, double *src, double* dst );
extern void FN_PROTOTYPE_FMA3(vrda_log2)(int len, double *src, double* dst );
extern void FN_PROTOTYPE_FMA3(vrda_log)(int len, double *src, double* dst );
extern void FN_PROTOTYPE_FMA3(vrda_sincos)(int n, double *x, double *ys, double *yc);
extern void FN_PROTOTYPE_FMA3(vrda_sin)(int n, double *x, double *y);

extern void FN_PROTOTYPE_FMA3(vrsa_cbrtf)(int len, float *src, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_cosf)(int n, float *x, float *y);
extern void FN_PROTOTYPE_FMA3(vrsa_exp10f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_FMA3(vrsa_exp2f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_FMA3(vrsa_expf)(int len, float* x, float* y);
extern void FN_PROTOTYPE_FMA3(vrsa_expm1f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_FMA3(vrsa_log10f)(int len, float *src, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_log1pf)(int len, float *src, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_log2f)(int len, float *src, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_logf)(int len, float *src, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_powf)(int len, float *src1, float *src2, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_powxf)(int len, float *src1, float src2, float* dst );
extern void FN_PROTOTYPE_FMA3(vrsa_sincosf)(int n, float *x, float *ys, float *yc);
extern void FN_PROTOTYPE_FMA3(vrsa_sinf)(int n, float *x, float *y);



/*
 * AVX2 Versions
 */
/*
 * Vector double precision, 4 element
 */
extern __m256d FN_PROTOTYPE_AVX2(vrd4_exp2)(__m256d x);
extern __m256d FN_PROTOTYPE_AVX2(vrd4_pow)(__m256d x,__m256d y);


/*
 * Old base 64 functions
 */
extern float FN_PROTOTYPE_BAS64(cbrtf)(float x);
extern float FN_PROTOTYPE_BAS64(copysignf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(cosf)(float x);
extern float FN_PROTOTYPE_BAS64(exp10f)(float x);
extern float FN_PROTOTYPE_BAS64(exp2f)(float x);
extern float FN_PROTOTYPE_BAS64(expf)(float x);
extern float FN_PROTOTYPE_BAS64(expm1f)(float x);
extern float FN_PROTOTYPE_BAS64(fdimf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(fmaf)(float x, float y, float z);
extern float FN_PROTOTYPE_BAS64(fminf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(fmodf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(fabsf)(float x);
extern float FN_PROTOTYPE_BAS64(fmaxf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(log10f)(float x);
extern float FN_PROTOTYPE_BAS64(log1pf)(float x);
extern float FN_PROTOTYPE_BAS64(log2f)(float x);
extern float FN_PROTOTYPE_BAS64(logf)(float x);
extern float FN_PROTOTYPE_BAS64(powf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(remainderf)(float x, float y);
extern float FN_PROTOTYPE_BAS64(sinf)(float x);
extern float FN_PROTOTYPE_BAS64(tanf)(float x);
extern float FN_PROTOTYPE_BAS64(truncf)(float x);
extern double FN_PROTOTYPE_BAS64(fmax)(double x, double y);
extern double FN_PROTOTYPE_BAS64(fmin)(double x, double y);
extern double FN_PROTOTYPE_BAS64(fdim)(double x, double y);
extern double FN_PROTOTYPE_BAS64(cbrt)(double x);
extern double FN_PROTOTYPE_BAS64(cos)(double x);
extern double FN_PROTOTYPE_BAS64(exp10)(double x);
extern double FN_PROTOTYPE_BAS64(exp2)(double x);
extern double FN_PROTOTYPE_BAS64(exp)(double x);
extern double FN_PROTOTYPE_BAS64(expm1)(double x);
extern double FN_PROTOTYPE_BAS64(fabs)(double x);
extern double FN_PROTOTYPE_BAS64( fma)(double x, double y, double z);
extern double FN_PROTOTYPE_BAS64(fmod)(double x, double y);
extern double FN_PROTOTYPE_BAS64(log10)(double x);
extern double FN_PROTOTYPE_BAS64(log1p)(double x);
extern double FN_PROTOTYPE_BAS64(log2)(double x);
extern double FN_PROTOTYPE_BAS64(log)(double x);
extern double FN_PROTOTYPE_BAS64(pow)(double x,double y);
extern double FN_PROTOTYPE_BAS64(sin)(double x);
extern double FN_PROTOTYPE_BAS64(remainder)(double x, double y);
extern double FN_PROTOTYPE_BAS64(tan)(double x);
extern double FN_PROTOTYPE_BAS64(trunc)(double x);
extern double FN_PROTOTYPE_BAS64(copysign)(double x, double y);
extern void FN_PROTOTYPE_BAS64(sincos)(double x,double *s, double *c);
extern void FN_PROTOTYPE_BAS64(sincosf)(float x,float *s, float *c);
extern void FN_PROTOTYPE_BAS64(vrda_cbrt)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_cos)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_exp10)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_exp2)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_exp)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_expm1)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_log10)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_log1p)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_log2)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_log)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrda_sin)(int len, double* x, double* y);
extern void FN_PROTOTYPE_BAS64(vrsa_cbrtf)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_cosf)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_exp10f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_exp2f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_expf)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_expm1f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_log10f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_log1pf)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_log2f)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_logf)(int len, float* x, float* y);
extern void FN_PROTOTYPE_BAS64(vrsa_sinf)(int len, float* x, float* y);

extern __m128d FN_PROTOTYPE_BAS64(vrd2_cbrt)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_cos)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_exp10)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_exp2)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_exp)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_expm1)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_log10)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_log1p)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_log2)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_log)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_sin)(__m128d x);
extern __m128d FN_PROTOTYPE_BAS64(vrd2_tan)(__m128d x);

extern __m128 FN_PROTOTYPE_BAS64(vrs4_cbrtf)(__m128  x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_cosf)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_exp10f)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_exp2f)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_expf)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_expm1f)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_log10f)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_log1pf)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_log2f)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_logf)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_sinf)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_tanf)(__m128 x);
extern __m128 FN_PROTOTYPE_BAS64(vrs4_powf)(__m128 x, __m128 y);

/*
 * Referene Implementations
 */


extern double FN_PROTOTYPE_REF(acos)(double x);
extern double FN_PROTOTYPE_REF(acosh)(double x);
extern double FN_PROTOTYPE_REF(zen_pow)(double x, double y);
extern double FN_PROTOTYPE_REF(asin)(double x);
extern double FN_PROTOTYPE_REF(asinh)(double x);
extern double FN_PROTOTYPE_REF(atan2)(double y, double x);
extern double FN_PROTOTYPE_REF(atan)(double x);
extern double FN_PROTOTYPE_REF(atanh)(double x);
extern double FN_PROTOTYPE_REF(ceil)(double x);
extern double FN_PROTOTYPE_REF(cosh)(double x);
extern double FN_PROTOTYPE_REF(cospi)(double x);
extern double FN_PROTOTYPE_REF(floor)(double x);
extern double FN_PROTOTYPE_REF(fmax)(double x, double y);
extern float FN_PROTOTYPE_REF(fmaxf)(float x, float y);
extern double FN_PROTOTYPE_REF(fmod)(double x, double y);
extern double FN_PROTOTYPE_REF(frexp)(double value, int *exp);
extern double FN_PROTOTYPE_REF(hypot)(double x, double y);
extern double FN_PROTOTYPE_REF(ldexp)(double x, int n);
extern double FN_PROTOTYPE_REF(logb)(double x);
extern double FN_PROTOTYPE_REF(modf)(double x, double *iptr);
extern double FN_PROTOTYPE_REF(nan)(const char *tagp);
extern double FN_PROTOTYPE_REF(nearbyint)(double x);
extern double FN_PROTOTYPE_REF(nextafter)(double x, double y);
extern double FN_PROTOTYPE_REF(nexttoward)(double x, long double y);
extern double FN_PROTOTYPE_REF(remquo)(double x, double y, int *quo);
extern double FN_PROTOTYPE_REF(rint)(double x);
extern double FN_PROTOTYPE_REF(scalbln)(double x, long int n);
extern double FN_PROTOTYPE_REF(scalbn)(double x, int n);
extern double FN_PROTOTYPE_REF(sinh)(double x);
extern double FN_PROTOTYPE_REF(sinpi)(double x);
extern double FN_PROTOTYPE_REF(sqrt)(double x);
extern double FN_PROTOTYPE_REF(tan)(double x);
extern double FN_PROTOTYPE_REF(tanh)(double x);
extern double FN_PROTOTYPE_REF(tanpi)(double x);

extern float FN_PROTOTYPE_REF(acosf)(float x);
extern float FN_PROTOTYPE_REF(acoshf)(float x);
extern float FN_PROTOTYPE_REF(asinf)(float x);
extern float FN_PROTOTYPE_REF(asinhf)(float x);
extern float FN_PROTOTYPE_REF(atan2f)(float fy, float fx);
extern float FN_PROTOTYPE_REF(atanf)(float fx);
extern float FN_PROTOTYPE_REF(atanhf)(float x);
extern float FN_PROTOTYPE_REF(ceilf)(float x);
extern float FN_PROTOTYPE_REF(coshf)(float fx);
extern float FN_PROTOTYPE_REF(cospif)(float x);
extern float FN_PROTOTYPE_REF(floorf)(float x);
extern float FN_PROTOTYPE_REF(fmodf)(float fy, float fx);
extern float FN_PROTOTYPE_REF(frexpf)(float value, int *exp);
extern float FN_PROTOTYPE_REF(hypotf)(float x, float y);
extern float FN_PROTOTYPE_REF(ldexpf)(float x, int n);
extern float FN_PROTOTYPE_REF(logbf)(float x);
extern float FN_PROTOTYPE_REF(modff)(float x, float *iptr);
extern float FN_PROTOTYPE_REF(nanf)(const char *tagp);
extern float FN_PROTOTYPE_REF(nearbyintf)(float x);
extern float FN_PROTOTYPE_REF(nextafterf)(float x, float y);
extern float FN_PROTOTYPE_REF(nexttowardf)(float x, long double y);
extern float FN_PROTOTYPE_REF(remquof) (float x, float y, int *quo);
extern float FN_PROTOTYPE_REF(rintf)(float x);
extern float FN_PROTOTYPE_REF(roundf)(float f);
extern float FN_PROTOTYPE_REF(scalblnf)(float x, long int n);
extern float FN_PROTOTYPE_REF(scalbnf)(float x, int n);
extern float FN_PROTOTYPE_REF(sinhf)(float fx);
extern float FN_PROTOTYPE_REF(sinpif)(float x);
extern float FN_PROTOTYPE_REF(sqrtf)(float x);
extern float FN_PROTOTYPE_REF(tanf)(float x);
extern float FN_PROTOTYPE_REF(tanhf)(float x);
extern float FN_PROTOTYPE_REF(tanpif)(float x);
extern float FN_PROTOTYPE_REF(truncf)(float x);

extern int FN_PROTOTYPE_REF(finite)(double x);
extern int FN_PROTOTYPE_REF(finitef)(float x);
extern int FN_PROTOTYPE_REF(ilogb)(double x);
extern int FN_PROTOTYPE_REF(ilogbf)(float x);

extern long long int FN_PROTOTYPE_REF(llrint)(double x);
extern long long int FN_PROTOTYPE_REF(llrintf)(float x);
extern long long int FN_PROTOTYPE_REF(llround)(double d);
extern long long int FN_PROTOTYPE_REF(llroundf)(float f);

extern long int FN_PROTOTYPE_REF(lrint)(double x);
extern long int FN_PROTOTYPE_REF(lrintf)(float x);
extern long int FN_PROTOTYPE_REF(lround)(double d);
extern long int FN_PROTOTYPE_REF(lroundf)(float f);

extern __m128d FN_PROTOTYPE_REF(vrd2_cosh)(__m128d x);
extern __m256d FN_PROTOTYPE_REF(vrd4_expm1)(__m256d x);

/*
 * OPTIMUM algorithm versions
 */
extern double FN_PROTOTYPE_OPT(exp2)(double x);
extern double FN_PROTOTYPE_OPT(exp)(double x);
extern double FN_PROTOTYPE_OPT(expm1)(double x);
extern double FN_PROTOTYPE_OPT(log2)(double x);
extern double FN_PROTOTYPE_OPT(log)(double x);
extern double FN_PROTOTYPE_OPT(logp1)(double x);

extern double FN_PROTOTYPE_OPT(pow)(double x, double y);
extern float FN_PROTOTYPE_OPT(powf)(float x, float y);

extern float FN_PROTOTYPE_OPT(exp2f)(float x);
extern float FN_PROTOTYPE_OPT(expf)(float x);
extern float FN_PROTOTYPE_OPT(expm1f)(float x);
extern float FN_PROTOTYPE_OPT(log2f)(float x);
extern float FN_PROTOTYPE_OPT(logf)(float x);
extern float FN_PROTOTYPE_OPT(logp1f)(float x);

extern __m128 FN_PROTOTYPE_OPT(vrs4_expf)(__m128 x);
extern __m128 FN_PROTOTYPE_OPT(vrs4_powf)(__m128 x, __m128 y);
extern __m256 FN_PROTOTYPE_OPT(vrs8_logf)(__m256 x);
extern __m256 FN_PROTOTYPE_OPT(vrs8_expf)(__m256 x);

extern __m256 FN_PROTOTYPE_OPT(vrs8_powf)(__m256 x, __m256 y);

extern __m256d FN_PROTOTYPE_OPT(vrd4_pow)(__m256d x, __m256d y);
extern __m128d FN_PROTOTYPE_OPT(vrd2_pow)(__m128d x, __m128d y);
extern __m256d FN_PROTOTYPE_OPT(vrd4_log)(__m256d x);

extern __m128d FN_PROTOTYPE_OPT(vrd2_exp)(__m128d x);


#if defined(__cplusplus)
}
#endif


#endif /* __LIBM_AMD_FUNCS_INTERNAL_H__ */

