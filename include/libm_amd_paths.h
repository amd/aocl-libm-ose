#ifndef LIBM_AMD_PATH_H_INCLUDED
#define LIBM_AMD_PATH_H_INCLUDED 1

#include "libm_macros.h"
#ifdef WIN64
#include <intrin.h>
#else

#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5))
#include <x86intrin.h>
#else
#include <immintrin.h>
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif


 double FN_PROTOTYPE_BAS64( fma)(double x, double y, double z);
 double FN_PROTOTYPE_BDOZR( fma)(double x, double y, double z);
 double FN_PROTOTYPE_FMA3( fma)(double x, double y, double z);

 float FN_PROTOTYPE_BAS64(fmaf)(float x, float y, float z);
 float FN_PROTOTYPE_BDOZR(fmaf)(float x, float y, float z);
 float FN_PROTOTYPE_FMA3(fmaf)(float x, float y, float z);

 double FN_PROTOTYPE_BAS64(exp)(double x);
 double FN_PROTOTYPE_BDOZR(exp)(double x);
 double FN_PROTOTYPE_FMA3(exp)(double x);

 double FN_PROTOTYPE_BAS64(exp2)(double x);
 double FN_PROTOTYPE_BDOZR(exp2)(double x);
 double FN_PROTOTYPE_FMA3(exp2)(double x);

 double FN_PROTOTYPE_BAS64(exp10)(double x);
 double FN_PROTOTYPE_BDOZR(exp10)(double x);
 double FN_PROTOTYPE_FMA3(exp10)(double x);

 double FN_PROTOTYPE_BAS64(expm1)(double x);
 double FN_PROTOTYPE_BDOZR(expm1)(double x);
 double FN_PROTOTYPE_FMA3(expm1)(double x);
 
 double FN_PROTOTYPE_BAS64(pow)(double x,double y);
 double FN_PROTOTYPE_FMA3(pow)(double x,double y);
 double FN_PROTOTYPE_BDOZR(pow)(double x,double y);
 double FN_PROTOTYPE_FMA3(fastpow)(double x,double y);

 float FN_PROTOTYPE_BAS64(expf)(float x);
 float FN_PROTOTYPE_BDOZR(expf)(float x);
 float FN_PROTOTYPE_FMA3(expf)(float x);

 float FN_PROTOTYPE_BAS64(exp2f)(float x);
 float FN_PROTOTYPE_BDOZR(exp2f)(float x);
 float FN_PROTOTYPE_FMA3(exp2f)(float x);

 float FN_PROTOTYPE_BAS64(exp10f)(float x);
 float FN_PROTOTYPE_BDOZR(exp10f)(float x);
 float FN_PROTOTYPE_FMA3(exp10f)(float x);

 float FN_PROTOTYPE_BAS64(expm1f)(float x);
 float FN_PROTOTYPE_BDOZR(expm1f)(float x);
 float FN_PROTOTYPE_FMA3(expm1f)(float x);
 
 float FN_PROTOTYPE_BAS64(powf)(float x,float y);
 float FN_PROTOTYPE_FMA3(powf)(float x,float y);
 float FN_PROTOTYPE_BDOZR(powf)(float x,float y);


 __m128 FN_PROTOTYPE_BAS64(vrs4_expf)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_expf)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_expf)(__m128 x);

 __m128 FN_PROTOTYPE_BAS64(vrs4_exp2f)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_exp2f)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_exp2f)(__m128 x);

 __m128 FN_PROTOTYPE_BAS64(vrs4_exp10f)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_exp10f)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_exp10f)(__m128 x);

 __m128 FN_PROTOTYPE_BAS64(vrs4_expm1f)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_expm1f)(__m128 x);
__m128 FN_PROTOTYPE_FMA3(vrs4_expm1f)(__m128 x);

 __m128d FN_PROTOTYPE_BAS64(vrd2_exp)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_exp)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_exp)(__m128d x);
 __m256d FN_PROTOTYPE_FMA3(vrd4_exp)(__m256d x);
 __m256d FN_PROTOTYPE_FMA3(vrd4_log)(__m256d x);
__m128d FN_PROTOTYPE_FMA3(vrd2_pow)(__m128d x,__m128d y);
 __m128d FN_PROTOTYPE_BAS64(vrd2_exp2)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_exp2)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_exp2)(__m128d x);
 __m256d FN_PROTOTYPE_FMA3(vrd4_exp2)(__m256d x);
 __m256d FN_PROTOTYPE_AVX2(vrd4_exp2)(__m256d x);
 __m256d FN_PROTOTYPE_FMA3(vrd4_pow)(__m256d x,__m256d y);
 __m256d FN_PROTOTYPE_AVX2(vrd4_pow)(__m256d x,__m256d y);
 __m128d FN_PROTOTYPE_BAS64(vrd2_exp10)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_exp10)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_exp10)(__m128d x);

 __m128d FN_PROTOTYPE_BAS64(vrd2_expm1)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_expm1)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_expm1)(__m128d x);

 void FN_PROTOTYPE_BAS64(vrda_exp)(int len, double* x, double* y);
 void FN_PROTOTYPE_BDOZR(vrda_exp)(int len, double* x, double* y);
 void FN_PROTOTYPE_FMA3(vrda_exp)(int len, double* x, double* y);

 void FN_PROTOTYPE_BAS64(vrda_exp2)(int len, double* x, double* y);
 void FN_PROTOTYPE_BDOZR(vrda_exp2)(int len, double* x, double* y);
 void FN_PROTOTYPE_FMA3(vrda_exp2)(int len, double* x, double* y);

 void FN_PROTOTYPE_BAS64(vrda_exp10)(int len, double* x, double* y);
 void FN_PROTOTYPE_BDOZR(vrda_exp10)(int len, double* x, double* y);
  void FN_PROTOTYPE_FMA3(vrda_exp10)(int len, double* x, double* y);

 void FN_PROTOTYPE_BAS64(vrda_expm1)(int len, double* x, double* y);
 void FN_PROTOTYPE_BDOZR(vrda_expm1)(int len, double* x, double* y);
 void FN_PROTOTYPE_FMA3(vrda_expm1)(int len, double* x, double* y);

 void FN_PROTOTYPE_BAS64(vrda_cbrt)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_cbrt)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_cbrt)(int len, double* x, double* y); 
 
 void FN_PROTOTYPE_BAS64(vrda_log10)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_log10)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_log10)(int len, double* x, double* y); 

 void FN_PROTOTYPE_BAS64(vrda_log)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_log)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_log)(int len, double* x, double* y); 

 void FN_PROTOTYPE_BAS64(vrda_cos)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_cos)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_cos)(int len, double* x, double* y); 

 void FN_PROTOTYPE_BAS64(vrda_log1p)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_log1p)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_log1p)(int len, double* x, double* y); 

 void FN_PROTOTYPE_BAS64(vrda_log2)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_log2)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_log2)(int len, double* x, double* y); 

 void FN_PROTOTYPE_BAS64(vrda_sin)(int len, double* x, double* y); 
 void FN_PROTOTYPE_BDOZR(vrda_sin)(int len, double* x, double* y); 
 void FN_PROTOTYPE_FMA3(vrda_sin)(int len, double* x, double* y); 
 

void FN_PROTOTYPE_BAS64(vrsa_expf)(int len, float* x, float* y);
 void FN_PROTOTYPE_BDOZR(vrsa_expf)(int len, float* x, float* y);
 void FN_PROTOTYPE_FMA3(vrsa_expf)(int len, float* x, float* y);

 void FN_PROTOTYPE_BAS64(vrsa_exp2f)(int len, float* x, float* y);
 void FN_PROTOTYPE_BDOZR(vrsa_exp2f)(int len, float* x, float* y);
 void FN_PROTOTYPE_FMA3(vrsa_exp2f)(int len, float* x, float* y);

 void FN_PROTOTYPE_BAS64(vrsa_exp10f)(int len, float* x, float* y);
 void FN_PROTOTYPE_BDOZR(vrsa_exp10f)(int len, float* x, float* y);
 void FN_PROTOTYPE_FMA3(vrsa_exp10f)(int len, float* x, float* y);

 void FN_PROTOTYPE_BAS64(vrsa_expm1f)(int len, float* x, float* y);
 void FN_PROTOTYPE_BDOZR(vrsa_expm1f)(int len, float* x, float* y);
 void FN_PROTOTYPE_FMA3(vrsa_expm1f)(int len, float* x, float* y);


 void FN_PROTOTYPE_BAS64(vrsa_cosf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_cosf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_cosf)(int len, float* x, float* y); 

 void FN_PROTOTYPE_BAS64(vrsa_log1pf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_log1pf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_log1pf)(int len, float* x, float* y); 
 
 void FN_PROTOTYPE_BAS64(vrsa_sinf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_sinf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_sinf)(int len, float* x, float* y); 

 void FN_PROTOTYPE_BAS64(vrsa_log2f)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_log2f)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_log2f)(int len, float* x, float* y); 

 void FN_PROTOTYPE_BAS64(vrsa_cbrtf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_cbrtf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_cbrtf)(int len, float* x, float* y); 

 void FN_PROTOTYPE_BAS64(vrsa_log10f)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_log10f)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_log10f)(int len, float* x, float* y); 

 void FN_PROTOTYPE_BAS64(vrsa_logf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_BDOZR(vrsa_logf)(int len, float* x, float* y); 
 void FN_PROTOTYPE_FMA3(vrsa_logf)(int len, float* x, float* y); 

 double FN_PROTOTYPE_BAS64(cbrt)(double x);
 double FN_PROTOTYPE_BDOZR(cbrt)(double x);
 double FN_PROTOTYPE_FMA3(cbrt)(double x);

 float FN_PROTOTYPE_BAS64(cbrtf)(float x);
 float FN_PROTOTYPE_BDOZR(cbrtf)(float x);
 float FN_PROTOTYPE_FMA3(cbrtf)(float x);


 __m128d FN_PROTOTYPE_BDOZR(vrd2_cbrt)(__m128d z);
 __m128d FN_PROTOTYPE_BAS64(vrd2_cbrt)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_cbrt)(__m128d x);

 __m128 FN_PROTOTYPE_BDOZR(vrs4_cbrtf)(__m128  z);
 __m128 FN_PROTOTYPE_BAS64(vrs4_cbrtf)(__m128  x);
__m128 FN_PROTOTYPE_FMA3(vrs4_cbrtf)(__m128  x);

 __m128d FN_PROTOTYPE_BDOZR(vrd2_log)(__m128d  x);
 __m128d FN_PROTOTYPE_BAS64(vrd2_log)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_log)(__m128d x);

 __m128d FN_PROTOTYPE_BDOZR(vrd2_log2)(__m128d x);
 __m128d FN_PROTOTYPE_BAS64(vrd2_log2)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_log2)(__m128d x);

 __m128d FN_PROTOTYPE_BDOZR(vrd2_log10)(__m128d x);
 __m128d FN_PROTOTYPE_BAS64(vrd2_log10)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_log10)(__m128d x);

 __m128d FN_PROTOTYPE_BDOZR(vrd2_log1p)(__m128d x);
 __m128d FN_PROTOTYPE_BAS64(vrd2_log1p)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_log1p)(__m128d x);

 __m128 FN_PROTOTYPE_BDOZR(vrs4_logf)(__m128 z);
 __m128 FN_PROTOTYPE_BAS64(vrs4_logf)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_logf)(__m128 x);

 __m128 FN_PROTOTYPE_BDOZR(vrs4_log2f)(__m128 z);
 __m128 FN_PROTOTYPE_BAS64(vrs4_log2f)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_log2f)(__m128 x);

 __m128 FN_PROTOTYPE_BDOZR(vrs4_log10f)(__m128 z);
 __m128 FN_PROTOTYPE_BAS64(vrs4_log10f)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_log10f)(__m128 x);

 __m128 FN_PROTOTYPE_BDOZR(vrs4_log1pf)(__m128 z);
 __m128 FN_PROTOTYPE_BAS64(vrs4_log1pf)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_log1pf)(__m128 x);

 float FN_PROTOTYPE_BAS64(logf)(float x);
 float FN_PROTOTYPE_BDOZR(logf)(float x);
 float FN_PROTOTYPE_FMA3(logf)(float x);

 float FN_PROTOTYPE_BAS64(log2f)(float x);
 float FN_PROTOTYPE_BDOZR(log2f)(float x);
 float FN_PROTOTYPE_FMA3(log2f)(float x);

 float FN_PROTOTYPE_BAS64(log10f)(float x);
 float FN_PROTOTYPE_BDOZR(log10f)(float x);
 float FN_PROTOTYPE_FMA3(log10f)(float x);

 float FN_PROTOTYPE_BAS64(log1pf)(float x);
 float FN_PROTOTYPE_BDOZR(log1pf)(float x);
 float FN_PROTOTYPE_FMA3(log1pf)(float x);

 double FN_PROTOTYPE_BAS64(log)(double x);
 double FN_PROTOTYPE_BDOZR(log)(double x);
 double FN_PROTOTYPE_FMA3(log)(double x);

 double FN_PROTOTYPE_BAS64(log2)(double x);
 double FN_PROTOTYPE_BDOZR(log2)(double x);
 double FN_PROTOTYPE_FMA3(log2)(double x);

 double FN_PROTOTYPE_BAS64(log10)(double x);
 double FN_PROTOTYPE_BDOZR(log10)(double x);
 double FN_PROTOTYPE_FMA3(log10)(double x);

 double FN_PROTOTYPE_BAS64(log1p)(double x);
 double FN_PROTOTYPE_BDOZR(log1p)(double x);
double FN_PROTOTYPE_FMA3(log1p)(double x);


 double FN_PROTOTYPE_BAS64(cos)(double x);
 double FN_PROTOTYPE_BDOZR(cos)(double x);
 double FN_PROTOTYPE_FMA3(cos)(double x);

 double FN_PROTOTYPE_BAS64(sin)(double x);
 double FN_PROTOTYPE_BDOZR(sin)(double x);
 double FN_PROTOTYPE_FMA3(sin)(double x);

 double FN_PROTOTYPE_BAS64(tan)(double x);
 double FN_PROTOTYPE_BDOZR(tan)(double x);
 double FN_PROTOTYPE_FMA3(tan)(double x);

 void FN_PROTOTYPE_BAS64(sincos)(double x,double *s,double *c);
 void FN_PROTOTYPE_BDOZR(sincos)(double x,double *s,double *c);
 void FN_PROTOTYPE_FMA3(sincos)(double x,double *s,double *c);

 void FN_PROTOTYPE_BAS64(sincosf)(float x,float *s,float *c);
 void FN_PROTOTYPE_BDOZR(sincosf)(float x,float *s,float *c);
 void FN_PROTOTYPE_FMA3(sincosf)(float x,float *s,float *c);

 __m128d FN_PROTOTYPE_BAS64(vrd2_cos)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_cos)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_cos)(__m128d x);

 __m128d FN_PROTOTYPE_BAS64(vrd2_sin)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_sin)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_sin)(__m128d x);

 __m128d FN_PROTOTYPE_BAS64(vrd2_tan)(__m128d x);
 __m128d FN_PROTOTYPE_BDOZR(vrd2_tan)(__m128d x);
 __m128d FN_PROTOTYPE_FMA3(vrd2_tan)(__m128d x);




 float FN_PROTOTYPE_BAS64(cosf)(float x);
 float FN_PROTOTYPE_BDOZR(cosf)(float x);
 float FN_PROTOTYPE_FMA3(cosf)(float x);

 float FN_PROTOTYPE_BAS64(sinf)(float x);
 float FN_PROTOTYPE_BDOZR(sinf)(float x);
 float FN_PROTOTYPE_FMA3(sinf)(float x);

 float FN_PROTOTYPE_BAS64(tanf)(float x);
 float FN_PROTOTYPE_BDOZR(tanf)(float x);
 float FN_PROTOTYPE_FMA3(tanf)(float x);



 __m128 FN_PROTOTYPE_BAS64(vrs4_cosf)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_cosf)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_cosf)(__m128 x);

 __m128 FN_PROTOTYPE_BAS64(vrs4_sinf)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_sinf)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_sinf)(__m128 x);

 __m128 FN_PROTOTYPE_BAS64(vrs4_tanf)(__m128 x);
 __m128 FN_PROTOTYPE_BDOZR(vrs4_tanf)(__m128 x);
 __m128 FN_PROTOTYPE_FMA3(vrs4_tanf)(__m128 x);




#ifdef __cplusplus
}
#endif

#endif /* LIBM_AMD_PATH_H_INCLUDED */
