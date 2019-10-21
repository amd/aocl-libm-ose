#ifndef __LIBM_AMD_H__
#define __LIBM_AMD_H__

#include <immintrin.h>

#include <libm_macros.h>

__m256d FN_PROTOTYPE(vrd4_exp) (__m256d);

#endif /* __LIBM_AMD_H__ */
#ifndef LIBM_AMD_H_INCLUDED
#define LIBM_AMD_H_INCLUDED 1

#include "libm_macros.h"
#ifdef WIN64
#include <intrin.h>
#else
#include <emmintrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


 double FN_PROTOTYPE(cbrt)(double x);
 float FN_PROTOTYPE(cbrtf)(float x);

 double FN_PROTOTYPE(fabs)(double x);
 float FN_PROTOTYPE(fabsf)(float x);

double FN_PROTOTYPE(acos)(double x);
 float FN_PROTOTYPE(acosf)(float x);

 double FN_PROTOTYPE(acosh)(double x);
 float FN_PROTOTYPE(acoshf)(float x);

 double FN_PROTOTYPE(asin)(double x);
 float FN_PROTOTYPE(asinf)(float x);

 double FN_PROTOTYPE( asinh)(double x);
 float FN_PROTOTYPE(asinhf)(float x);

 double FN_PROTOTYPE( atan)(double x);
 float FN_PROTOTYPE(atanf)(float x);

 double FN_PROTOTYPE( atanh)(double x);
 float FN_PROTOTYPE(atanhf)(float x);

 double FN_PROTOTYPE( atan2)(double x, double y);
 float FN_PROTOTYPE(atan2f)(float x, float y);

 double FN_PROTOTYPE( ceil)(double x);
 float FN_PROTOTYPE(ceilf)(float x);


 double FN_PROTOTYPE( cos)(double x);
 float FN_PROTOTYPE(cosf)(float x);


 double FN_PROTOTYPE( exp)(double x);
 float FN_PROTOTYPE(expf)(float x);

 double FN_PROTOTYPE( expm1)(double x);
 float FN_PROTOTYPE(expm1f)(float x);

 double FN_PROTOTYPE( exp2)(double x);
 float FN_PROTOTYPE(exp2f)(float x);

 double FN_PROTOTYPE( exp10)(double x);
 float FN_PROTOTYPE(exp10f)(float x);


 double FN_PROTOTYPE( fdim)(double x, double y);
 float FN_PROTOTYPE(fdimf)(float x, float y);

#ifdef WINDOWS
 int FN_PROTOTYPE(finite)(double x);
 int FN_PROTOTYPE(finitef)(float x);
#else
 int FN_PROTOTYPE(finite)(double x);
 int FN_PROTOTYPE(finitef)(float x);
#endif

 double FN_PROTOTYPE( floor)(double x);
 float FN_PROTOTYPE(floorf)(float x);

 double FN_PROTOTYPE( fma)(double x, double y, double z);
 float FN_PROTOTYPE(fmaf)(float x, float y, float z);

 double FN_PROTOTYPE( fmax)(double x, double y);
 float FN_PROTOTYPE(fmaxf)(float x, float y);

 double FN_PROTOTYPE( fmin)(double x, double y);
 float FN_PROTOTYPE(fminf)(float x, float y);

 double FN_PROTOTYPE( fmod)(double x, double y);
 float FN_PROTOTYPE(fmodf)(float x, float y);

#ifdef WINDOWS
 double FN_PROTOTYPE( hypot)(double x, double y);
 float FN_PROTOTYPE(hypotf)(float x, float y);
#else
 double FN_PROTOTYPE( hypot)(double x, double y);
 float FN_PROTOTYPE(hypotf)(float x, float y);
#endif

 float FN_PROTOTYPE(ldexpf)(float x, int exp);

 double FN_PROTOTYPE(ldexp)(double x, int exp);

 double FN_PROTOTYPE( log)(double x);
 float FN_PROTOTYPE(logf)(float x);


 float FN_PROTOTYPE(log2f)(float x);

 double FN_PROTOTYPE( log10)(double x);
 float FN_PROTOTYPE(log10f)(float x);


 float FN_PROTOTYPE(log1pf)(float x);

#ifdef WINDOWS
 double FN_PROTOTYPE( logb)(double x);
 float FN_PROTOTYPE(logbf)(float x);
#else
 double FN_PROTOTYPE( logb)(double x);
 float FN_PROTOTYPE(logbf)(float x);
#endif

 double FN_PROTOTYPE( modf)(double x, double *iptr);
 float FN_PROTOTYPE(modff)(float x, float *iptr);

 double FN_PROTOTYPE( pow)(double x, double y);
 float FN_PROTOTYPE(powf)(float x, float y);
 double FN_PROTOTYPE(fastpow)(double x, double y);

  double FN_PROTOTYPE(zen_pow)(double x, double y); // VK New "pow" function added.

double FN_PROTOTYPE( remainder)(double x, double y);
 float FN_PROTOTYPE(remainderf)(float x, float y);

 double FN_PROTOTYPE(sin)(double x);
 float FN_PROTOTYPE(sinf)(float x);

  double FN_PROTOTYPE(sinpi)(double x);
  float FN_PROTOTYPE(sinpif)(float x);

  double FN_PROTOTYPE(cospi)(double x);
  float FN_PROTOTYPE(cospif)(float x);

 void FN_PROTOTYPE(sincos)(double x, double *s, double *c);
 void FN_PROTOTYPE(sincosf)(float x, float *s, float *c);

 double FN_PROTOTYPE( sinh)(double x);
 float FN_PROTOTYPE(sinhf)(float x);

 double FN_PROTOTYPE( sqrt)(double x);
 float FN_PROTOTYPE(sqrtf)(float x);

 double FN_PROTOTYPE( tan)(double x);
 float FN_PROTOTYPE(tanf)(float x);


 double FN_PROTOTYPE(tanpi)(double x);
 float FN_PROTOTYPE(tanpif)(float x);

 double FN_PROTOTYPE( tanh)(double x);
 float FN_PROTOTYPE(tanhf)(float x);

 double FN_PROTOTYPE( trunc)(double x);
 float FN_PROTOTYPE(truncf)(float x);

 double FN_PROTOTYPE( log1p)(double x);
 double FN_PROTOTYPE( log2)(double x);

 double FN_PROTOTYPE(cosh)(double x);
 float FN_PROTOTYPE(coshf)(float fx);

 double FN_PROTOTYPE(frexp)(double value, int *exp);
 float FN_PROTOTYPE(frexpf)(float value, int *exp);
 int FN_PROTOTYPE(ilogb)(double x);
 int FN_PROTOTYPE(ilogbf)(float x);

 long long int FN_PROTOTYPE(llrint)(double x);
 long long int FN_PROTOTYPE(llrintf)(float x);
 long int FN_PROTOTYPE(lrint)(double x);
 long int FN_PROTOTYPE(lrintf)(float x);
 long int FN_PROTOTYPE(lround)(double d);
 long int FN_PROTOTYPE(lroundf)(float f);
 double  FN_PROTOTYPE(nan)(const char *tagp);
 float  FN_PROTOTYPE(nanf)(const char *tagp);
 float FN_PROTOTYPE(nearbyintf)(float x);
 double FN_PROTOTYPE(nearbyint)(double x);
 double FN_PROTOTYPE(nextafter)(double x, double y);
 float FN_PROTOTYPE(nextafterf)(float x, float y);
 double FN_PROTOTYPE(nexttoward)(double x, long double y);
 float FN_PROTOTYPE(nexttowardf)(float x, long double y);
 double FN_PROTOTYPE(rint)(double x);
 float FN_PROTOTYPE(rintf)(float x);
 float FN_PROTOTYPE(roundf)(float f);
 double FN_PROTOTYPE(round)(double f);
 double FN_PROTOTYPE(scalbln)(double x, long int n);
 float FN_PROTOTYPE(scalblnf)(float x, long int n);
 double FN_PROTOTYPE(scalbn)(double x, int n);
 float FN_PROTOTYPE(scalbnf)(float x, int n);
 long long int FN_PROTOTYPE(llroundf)(float f);
 long long int FN_PROTOTYPE(llround)(double d);
 float FN_PROTOTYPE(remquof) (float x, float y, int *quo);
 double FN_PROTOTYPE(remquo) (double x, double y, int *quo);

#ifdef WINDOWS
 double FN_PROTOTYPE(copysign)(double x, double y);
 float FN_PROTOTYPE(copysignf)(float x, float y);
#else
 double FN_PROTOTYPE(copysign)(double x, double y);
 float FN_PROTOTYPE(copysignf)(float x, float y);
#endif

__m128d FN_PROTOTYPE(vrd2_exp)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_exp2)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_exp10)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_expm1)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_sin)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_cos)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_cbrt)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_log)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_log2)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_log10)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_log1p)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_pow)(__m128d x, __m128d y);
__m128d FN_PROTOTYPE(vrd2_tan)(__m128d x);
__m128d FN_PROTOTYPE(vrd2_cosh)(__m128d x);

__m256d FN_PROTOTYPE(vrd4_exp)(__m256d x);
__m256d FN_PROTOTYPE(vrd4_exp2)(__m256d x);
__m256d FN_PROTOTYPE(vrd4_expm1)(__m256d x);
__m256d FN_PROTOTYPE(vrd4_log)(__m256d x);

__m128 FN_PROTOTYPE(vrs4_expf)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_exp2f)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_exp10f)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_expm1f)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_sinf)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_cosf)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_cbrtf)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_logf)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_log2f)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_log10f)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_log1pf)(__m128 x);
__m128 FN_PROTOTYPE(vrs4_powf)(__m128 x, __m128 y);
__m128 FN_PROTOTYPE(vrs4_powxf)(__m128 x, float y);

__m128 FN_PROTOTYPE(vrs4_tanf)(__m128 x);

void  FN_PROTOTYPE(vrd2_sincos)(__m128d x, __m128d* ys, __m128d* yc);
void  FN_PROTOTYPE(vrda_sincos)(int n, double *x, double *ys, double *yc);

void  FN_PROTOTYPE(vrs4_sincosf)(__m128 x, __m128* ys, __m128* yc);
void  FN_PROTOTYPE(vrsa_sincosf)(int n, float *x, float *ys, float *yc);

void FN_PROTOTYPE(vrda_exp)(int len, double* x, double* y);
void FN_PROTOTYPE(vrsa_expf)(int len, float* x, float* y);
void FN_PROTOTYPE(vrda_exp10)(int len, double* x, double* y);
void FN_PROTOTYPE(vrsa_exp10f)(int len, float* x, float* y);
void FN_PROTOTYPE(vrda_exp2)(int len, double* x, double* y);
void FN_PROTOTYPE(vrsa_exp2f)(int len, float* x, float* y);
void FN_PROTOTYPE(vrda_expm1)(int len, double* x, double* y);
void FN_PROTOTYPE(vrsa_expm1f)(int len, float* x, float* y);
void FN_PROTOTYPE(vrda_sin)(int n, double *x, double *y);
void FN_PROTOTYPE(vrsa_sinf)(int n, float *x, float *y);
void FN_PROTOTYPE(vrda_cos)(int n, double *x, double *y);
void FN_PROTOTYPE(vrsa_cosf)(int n, float *x, float *y);
void FN_PROTOTYPE(vrsa_cbrtf)( int len, float *src, float* dst );
void FN_PROTOTYPE(vrda_cbrt)( int len, double *src, double* dst );
void FN_PROTOTYPE(vrsa_logf)( int len, float *src, float* dst );
void FN_PROTOTYPE(vrda_log)( int len, double *src, double* dst );
void FN_PROTOTYPE(vrsa_log2f)( int len, float *src, float* dst );
void FN_PROTOTYPE(vrda_log2)( int len, double *src, double* dst );
void FN_PROTOTYPE(vrsa_log10f)( int len, float *src, float* dst );
void FN_PROTOTYPE(vrda_log10)( int len, double *src, double* dst );
void FN_PROTOTYPE(vrsa_log1pf)( int len, float *src, float* dst );
void FN_PROTOTYPE(vrda_log1p)( int len, double *src, double* dst );
void FN_PROTOTYPE(vrsa_powxf)( int len, float *src1, float src2, float* dst );
void FN_PROTOTYPE(vrsa_powf)( int len, float *src1, float *src2, float* dst );


#ifdef __cplusplus
}
#endif

#endif /* LIBM_AMD_H_INCLUDED */
