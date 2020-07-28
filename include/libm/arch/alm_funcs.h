/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 */
#if !defined  _LIBM_ARCH_ALM_PROTO_ARCH_H_
#define       _LIBM_ARCH_ALM_PROTO_ARCH_H_

#ifndef    ALM_PROTO_ARCH_CHANGE_INTERNAL
#error   "This file is not expected to be included externally"
#endif

#include    <libm/types.h>

extern     f64_t   ALM_PROTO_ARCH(acos)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(acosh)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(asin)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(asinh)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(atan2)       (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(atan)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(atanh)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(cbrt)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(ceil)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(copysign)    (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(cos)         (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(cosh)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(cospi)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(exp10)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(exp2)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(exp)         (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(expm1)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(fabs)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(fastpow)     (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(fdim)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(floor)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(fma)         (f64_t x, f64_t y, f64_t z);
extern     f64_t   ALM_PROTO_ARCH(fmax)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(fmin)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(fmod)        (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(frexp)       (f64_t value, int *exp);
extern     f64_t   ALM_PROTO_ARCH(hypot)       (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(ldexp)       (f64_t x, int exp);
extern     f64_t   ALM_PROTO_ARCH(log10)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(log1p)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(log2)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(logb)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(log)         (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(modf)        (f64_t x, f64_t *iptr);
extern     f64_t   ALM_PROTO_ARCH(nan)         (const char *tagp);
extern     f64_t   ALM_PROTO_ARCH(nearbyint)   (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(nextafter)   (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(nexttoward)  (f64_t x, long double y);
extern     f64_t   ALM_PROTO_ARCH(pow)         (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(remainder)   (f64_t x, f64_t y);
extern     f64_t   ALM_PROTO_ARCH(remquo)      (f64_t x, f64_t y, int *quo);
extern     f64_t   ALM_PROTO_ARCH(rint)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(round)       (f64_t f);
extern     f64_t   ALM_PROTO_ARCH(scalbln)     (f64_t x, long int n);
extern     f64_t   ALM_PROTO_ARCH(scalbn)      (f64_t x, int n);
extern     f64_t   ALM_PROTO_ARCH(sin)         (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(sinh)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(sinpi)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(sqrt)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(tan)         (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(tanh)        (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(tanpi)       (f64_t x);
extern     f64_t   ALM_PROTO_ARCH(trunc)       (f64_t x);

extern     f32_t   ALM_PROTO_ARCH(acosf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(acoshf)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(asinf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(asinhf)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(atan2f)        (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(atanf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(atanhf)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(cbrtf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(ceilf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(copysignf)     (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(cosf)          (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(coshf)         (f32_t fx);
extern     f32_t   ALM_PROTO_ARCH(cospif)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(exp10f)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(exp2f)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(expf)          (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(expm1f)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(fabsf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(fdimf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(floorf)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(fmaf)          (f32_t x, f32_t y, f32_t z);
extern     f32_t   ALM_PROTO_ARCH(fmaxf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(fminf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(fmodf)         (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(frexpf)        (f32_t value, int *exp);
extern     f32_t   ALM_PROTO_ARCH(hypotf)        (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(ldexpf)        (f32_t x, int exp);
extern     f32_t   ALM_PROTO_ARCH(log10f)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(log1pf)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(log2f)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(logbf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(logf)          (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(modff)         (f32_t x, f32_t *iptr);
extern     f32_t   ALM_PROTO_ARCH(nanf)          (const char *tagp);
extern     f32_t   ALM_PROTO_ARCH(nearbyintf)    (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(nextafterf)    (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(nexttowardf)   (f32_t x, long double y);
extern     f32_t   ALM_PROTO_ARCH(powf)          (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(remainderf)    (f32_t x, f32_t y);
extern     f32_t   ALM_PROTO_ARCH(remquof)       (f32_t x, f32_t y, int *quo);
extern     f32_t   ALM_PROTO_ARCH(rintf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(roundf)        (f32_t f);
extern     f32_t   ALM_PROTO_ARCH(scalblnf)      (f32_t x, long int n);
extern     f32_t   ALM_PROTO_ARCH(scalbnf)       (f32_t x, int n);
extern     f32_t   ALM_PROTO_ARCH(sinf)          (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(sinhf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(sinpif)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(sqrtf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(tanf)          (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(tanhf)         (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(tanpif)        (f32_t x);
extern     f32_t   ALM_PROTO_ARCH(truncf)        (f32_t x);


extern     int             ALM_PROTO_ARCH(finite)    (f64_t x);
extern     int             ALM_PROTO_ARCH(finitef)   (f32_t x);
extern     int             ALM_PROTO_ARCH(ilogb)     (f64_t x);
extern     int             ALM_PROTO_ARCH(ilogbf)    (f32_t x);
extern     long int        ALM_PROTO_ARCH(lrint)     (f64_t x);
extern     long int        ALM_PROTO_ARCH(lrintf)    (f32_t x);
extern     long int        ALM_PROTO_ARCH(lround)    (f64_t d);
extern     long int        ALM_PROTO_ARCH(lroundf)   (f32_t f);
extern     long long int   ALM_PROTO_ARCH(llrint)    (f64_t x);
extern     long long int   ALM_PROTO_ARCH(llrintf)   (f32_t x);
extern     long long int   ALM_PROTO_ARCH(llround)   (f64_t d);
extern     long long int   ALM_PROTO_ARCH(llroundf)  (f32_t f);



extern     __m128   ALM_PROTO_ARCH(vrs4_cbrtf)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_cosf)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_exp10f)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_exp2f)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_expf)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_expm1f)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_log10f)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_log1pf)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_log2f)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_logf)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_powxf)        (__m128 x, f32_t y);
extern     __m128   ALM_PROTO_ARCH(vrs4_sinf)        (__m128 x);
extern     __m128   ALM_PROTO_ARCH(vrs4_tanf)        (__m128 x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_cbrt)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_cosh)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_cos)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_exp10)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_exp2)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_exp)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_expm1)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_log10)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_log1p)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_log2)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_log)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_pow)        (__m128d x, __m128d y);
extern     __m128d   ALM_PROTO_ARCH(vrd2_sin)        (__m128d x);
extern     __m128d   ALM_PROTO_ARCH(vrd2_tan)        (__m128d x);
extern     __m256d   ALM_PROTO_ARCH(vrd4_exp)        (__m256d x);
extern     __m256d   ALM_PROTO_ARCH(vrd4_exp2)        (__m256d x);
extern     __m256d   ALM_PROTO_ARCH(vrd4_expm1)        (__m256d x);
extern     __m256d   ALM_PROTO_ARCH(vrd4_log)        (__m256d x);
extern     __m256d   ALM_PROTO_ARCH(vrd4_pow)        (__m256d x, __m256d y);
extern     void   ALM_PROTO_ARCH(sincos)        (f64_t x, f64_t *s, f64_t *c);
extern     void   ALM_PROTO_ARCH(sincosf)        (f32_t x, f32_t *s, f32_t *c);
extern     void   ALM_PROTO_ARCH(vrd2_sincos)        (__m128d x, __m128d* ys, __m128d* yc);
extern     void   ALM_PROTO_ARCH(vrs4_sincosf)        (__m128 x, __m128* ys, __m128* yc);

extern     void   ALM_PROTO_ARCH(vrda_cbrt)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO_ARCH(vrda_cos)        (int n, f64_t *x, f64_t *y);
extern     void   ALM_PROTO_ARCH(vrda_exp10)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO_ARCH(vrda_exp2)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO_ARCH(vrda_exp)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO_ARCH(vrda_expm1)        (int len, f64_t* x, f64_t* y);
extern     void   ALM_PROTO_ARCH(vrda_log10)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO_ARCH(vrda_log1p)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO_ARCH(vrda_log2)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO_ARCH(vrda_log)        (int len, f64_t *src, f64_t* dst );
extern     void   ALM_PROTO_ARCH(vrda_sincos)        (int n, f64_t *x, f64_t *ys, f64_t *yc);
extern     void   ALM_PROTO_ARCH(vrda_sin)        (int n, f64_t *x, f64_t *y);
extern     void   ALM_PROTO_ARCH(vrsa_cbrtf)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_cosf)        (int n, f32_t *x, f32_t *y);
extern     void   ALM_PROTO_ARCH(vrsa_exp10f)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO_ARCH(vrsa_exp2f)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO_ARCH(vrsa_expf)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO_ARCH(vrsa_expm1f)        (int len, f32_t* x, f32_t* y);
extern     void   ALM_PROTO_ARCH(vrsa_log10f)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_log1pf)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_log2f)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_logf)        (int len, f32_t *src, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_powf)        (int len, f32_t *src1, f32_t *src2, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_powxf)        (int len, f32_t *src1, f32_t src2, f32_t* dst );
extern     void   ALM_PROTO_ARCH(vrsa_sincosf)        (int n, f32_t *x, f32_t *ys, f32_t *yc);
extern     void   ALM_PROTO_ARCH(vrsa_sinf)        (int n, f32_t *x, f32_t *y);

#endif   /* __LIBM_ARCH_ALM_FUNCS_H__ */
