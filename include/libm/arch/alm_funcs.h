/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 */
#if !defined  _LIBM_ARCH_ALM_PROTO_H_
#define       _LIBM_ARCH_ALM_PROTO_H_

#ifndef    ALM_PROTO_CHANGE_INTERNAL
#error   "This file is not expected to be included externally"
#endif

#include    <libm/types.h>

extern     f64_t   ALM_PROTO(acos)        (f64_t x);
extern     f64_t   ALM_PROTO(acosh)       (f64_t x);
extern     f64_t   ALM_PROTO(asin)        (f64_t x);
extern     f64_t   ALM_PROTO(asinh)       (f64_t x);
extern     f64_t   ALM_PROTO(atan2)       (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(atan)        (f64_t x);
extern     f64_t   ALM_PROTO(atanh)       (f64_t x);
extern     f64_t   ALM_PROTO(cbrt)        (f64_t x);
extern     f64_t   ALM_PROTO(ceil)        (f64_t x);
extern     f64_t   ALM_PROTO(copysign)    (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(cos)         (f64_t x);
extern     f64_t   ALM_PROTO(cosh)        (f64_t x);
extern     f64_t   ALM_PROTO(cospi)       (f64_t x);
extern     f64_t   ALM_PROTO(exp10)       (f64_t x);
extern     f64_t   ALM_PROTO(exp2)        (f64_t x);
extern     f64_t   ALM_PROTO(exp)         (f64_t x);
extern     f64_t   ALM_PROTO(expm1)       (f64_t x);
extern     f64_t   ALM_PROTO(fabs)        (f64_t x);
extern     f64_t   ALM_PROTO(fastpow)     (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(fdim)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(floor)       (f64_t x);
extern     f64_t   ALM_PROTO(fma)         (f64_t x, f64_t y, f64_t z);
extern     f64_t   ALM_PROTO(fmax)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(fmin)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(fmod)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(frexp)       (f64_t value, int *exp);
extern     f64_t   ALM_PROTO(hypot)       (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(ldexp)       (f64_t x, int exp);
extern     f64_t   ALM_PROTO(log10)       (f64_t x);
extern     f64_t   ALM_PROTO(log1p)       (f64_t x);
extern     f64_t   ALM_PROTO(log2)        (f64_t x);
extern     f64_t   ALM_PROTO(logb)        (f64_t x);
extern     f64_t   ALM_PROTO(log)         (f64_t x);
extern     f64_t   ALM_PROTO(modf)        (f64_t x, f64_t *iptr);
extern     f64_t   ALM_PROTO(nan)         (const char *tagp);
extern     f64_t   ALM_PROTO(nearbyint)   (f64_t x);
extern     f64_t   ALM_PROTO(nextafter)   (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(nexttoward)  (f64_t x, long double y);
extern     f64_t   ALM_PROTO(pow)         (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(remainder)   (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO(remquo)      (f64_t x, f64_t y, int *quo);
extern     f64_t   ALM_PROTO(rint)        (f64_t x);
extern     f64_t   ALM_PROTO(round)       (f64_t f);
extern     f64_t   ALM_PROTO(scalbln)     (f64_t x, long int n);
extern     f64_t   ALM_PROTO(scalbn)      (f64_t x, int n);
extern     f64_t   ALM_PROTO(sin)         (f64_t x);
extern     f64_t   ALM_PROTO(sinh)        (f64_t x);
extern     f64_t   ALM_PROTO(sinpi)       (f64_t x);
extern     f64_t   ALM_PROTO(sqrt)        (f64_t x);
extern     f64_t   ALM_PROTO(tan)         (f64_t x);
extern     f64_t   ALM_PROTO(tanh)        (f64_t x);
extern     f64_t   ALM_PROTO(tanpi)       (f64_t x);
extern     f64_t   ALM_PROTO(trunc)       (f64_t x);

extern     f32_t   ALM_PROTO(acosf)         (f32_t x);
extern     f32_t   ALM_PROTO(acoshf)        (f32_t x);
extern     f32_t   ALM_PROTO(asinf)         (f32_t x);
extern     f32_t   ALM_PROTO(asinhf)        (f32_t x);
extern     f32_t   ALM_PROTO(atan2f)        (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(atanf)         (f32_t x);
extern     f32_t   ALM_PROTO(atanhf)        (f32_t x);
extern     f32_t   ALM_PROTO(cbrtf)         (f32_t x);
extern     f32_t   ALM_PROTO(ceilf)         (f32_t x);
extern     f32_t   ALM_PROTO(copysignf)     (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(cosf)          (f32_t x);
extern     f32_t   ALM_PROTO(coshf)         (f32_t fx);
extern     f32_t   ALM_PROTO(cospif)        (f32_t x);
extern     f32_t   ALM_PROTO(exp10f)        (f32_t x);
extern     f32_t   ALM_PROTO(exp2f)         (f32_t x);
extern     f32_t   ALM_PROTO(expf)          (f32_t x);
extern     f32_t   ALM_PROTO(expm1f)        (f32_t x);
extern     f32_t   ALM_PROTO(fabsf)         (f32_t x);
extern     f32_t   ALM_PROTO(fdimf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(floorf)        (f32_t x);
extern     f32_t   ALM_PROTO(fmaf)          (f32_t x, f32_t y, f32_t z);
extern     f32_t   ALM_PROTO(fmaxf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(fminf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(fmodf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(frexpf)        (f32_t value, int *exp);
extern     f32_t   ALM_PROTO(hypotf)        (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(ldexpf)        (f32_t x, int exp);
extern     f32_t   ALM_PROTO(log10f)        (f32_t x);
extern     f32_t   ALM_PROTO(log1pf)        (f32_t x);
extern     f32_t   ALM_PROTO(log2f)         (f32_t x);
extern     f32_t   ALM_PROTO(logbf)         (f32_t x);
extern     f32_t   ALM_PROTO(logf)          (f32_t x);
extern     f32_t   ALM_PROTO(modff)         (f32_t x, f32_t *iptr);
extern     f32_t   ALM_PROTO(nanf)          (const char *tagp);
extern     f32_t   ALM_PROTO(nearbyintf)    (f32_t x);
extern     f32_t   ALM_PROTO(nextafterf)    (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(nexttowardf)   (f32_t x, long double y);
extern     f32_t   ALM_PROTO(powf)          (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(remainderf)    (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO(remquof)       (f32_t x, f32_t y, int *quo);
extern     f32_t   ALM_PROTO(rintf)         (f32_t x);
extern     f32_t   ALM_PROTO(roundf)        (f32_t f);
extern     f32_t   ALM_PROTO(scalblnf)      (f32_t x, long int n);
extern     f32_t   ALM_PROTO(scalbnf)       (f32_t x, int n);
extern     f32_t   ALM_PROTO(sinf)          (f32_t x);
extern     f32_t   ALM_PROTO(sinhf)         (f32_t x);
extern     f32_t   ALM_PROTO(sinpif)        (f32_t x);
extern     f32_t   ALM_PROTO(sqrtf)         (f32_t x);
extern     f32_t   ALM_PROTO(tanf)          (f32_t x);
extern     f32_t   ALM_PROTO(tanhf)         (f32_t x);
extern     f32_t   ALM_PROTO(tanpif)        (f32_t x);
extern     f32_t   ALM_PROTO(truncf)        (f32_t x);


extern     int             ALM_PROTO(finite)    (f64_t x);
extern     int             ALM_PROTO(finitef)   (f32_t x);
extern     int             ALM_PROTO(ilogb)     (f64_t x);
extern     int             ALM_PROTO(ilogbf)    (f32_t x);
extern     long int        ALM_PROTO(lrint)     (f64_t x);
extern     long int        ALM_PROTO(lrintf)    (f32_t x);
extern     long int        ALM_PROTO(lround)    (f64_t d);
extern     long int        ALM_PROTO(lroundf)   (f32_t f);
extern     long long int   ALM_PROTO(llrint)    (f64_t x);
extern     long long int   ALM_PROTO(llrintf)   (f32_t x);
extern     long long int   ALM_PROTO(llround)   (f64_t d);
extern     long long int   ALM_PROTO(llroundf)  (f32_t f);



extern     __m128   ALM_PROTO(vrs4_cbrtf)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_cosf)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_exp10f)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_exp2f)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_expf)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_expm1f)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_log10f)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_log1pf)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_log2f)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_logf)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_powxf)        (__m128 x, f32_t y);
extern     __m128   ALM_PROTO(vrs4_sinf)        (__m128 x);
extern     __m128   ALM_PROTO(vrs4_tanf)        (__m128 x);
extern     __m128d   ALM_PROTO(vrd2_cbrt)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_cosh)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_cos)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_exp10)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_exp2)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_exp)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_expm1)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_log10)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_log1p)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_log2)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_log)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_pow)        (__m128d x, __m128d y);
extern     __m128d   ALM_PROTO(vrd2_sin)        (__m128d x);
extern     __m128d   ALM_PROTO(vrd2_tan)        (__m128d x);
extern     __m256d   ALM_PROTO(vrd4_exp)        (__m256d x);
extern     __m256d   ALM_PROTO(vrd4_exp2)        (__m256d x);
extern     __m256d   ALM_PROTO(vrd4_expm1)        (__m256d x);
extern     __m256d   ALM_PROTO(vrd4_log)        (__m256d x);
extern     __m256d   ALM_PROTO(vrd4_pow)        (__m256d x, __m256d y);
extern     void   ALM_PROTO(sincos)        (f64_t x, f64_t *s, f64_t *c);
extern     void   ALM_PROTO(sincosf)        (f32_t x, f32_t *s, f32_t *c);
extern     void   ALM_PROTO(vrd2_sincos)        (__m128d x, __m128d* ys, __m128d* yc);
extern     void   ALM_PROTO(vrs4_sincosf)        (__m128 x, __m128* ys, __m128* yc);

extern     void   ALM_PROTO(vrda_cbrt)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO(vrda_cos)        (int n, f64_t *x, f64_t *y);
extern     void   ALM_PROTO(vrda_exp10)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO(vrda_exp2)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO(vrda_exp)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO(vrda_expm1)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO(vrda_log10)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO(vrda_log1p)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO(vrda_log2)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO(vrda_log)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO(vrda_sincos)        (int n, f64_t *x, f64_t *ys, f64_t *yc);
extern     void   ALM_PROTO(vrda_sin)        (int n, f64_t *x, f64_t *y);
extern     void   ALM_PROTO(vrsa_cbrtf)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO(vrsa_cosf)        (int n, f32_t *x, f32_t *y);
extern     void   ALM_PROTO(vrsa_exp10f)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO(vrsa_exp2f)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO(vrsa_expf)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO(vrsa_expm1f)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO(vrsa_log10f)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO(vrsa_log1pf)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO(vrsa_log2f)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO(vrsa_logf)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO(vrsa_powf)        (int len, f32_t *src1, f32_t *src2, f32_t* dst );
extern     void   ALM_PROTO(vrsa_powxf)        (int len, f32_t *src1, f32_t src2, f32_t* dst );
extern     void   ALM_PROTO(vrsa_sincosf)        (int n, f32_t *x, f32_t *ys, f32_t *yc);
extern     void   ALM_PROTO(vrsa_sinf)        (int n, f32_t *x, f32_t *y);

#endif   /* __LIBM_ARCH_ALM_FUNCS_H__ */
