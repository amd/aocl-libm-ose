/*
 * Copyright (C) 2018-2021, Advanced Micro Devices, Inc. All rights reserved.
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

#if   !defined (__ALM_FUNCS_INTERNAL_H__)
#elif !defined (ALM_PROTO_ARCH)
#error  "This file is not expected to be included explicitly"
#endif

#include <immintrin.h>
#if defined(_WIN64) || defined(_WIN32)
    #include <avx512fintrin.h>
#endif
#include <complex.h>
#include <libm/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Double precision
 */
extern double    ALM_PROTO_INTERNAL(acos)                 (double x);
extern double    ALM_PROTO_INTERNAL(acosh)                (double x);
extern double    ALM_PROTO_INTERNAL(asin)                 (double x);
extern double    ALM_PROTO_INTERNAL(asinh)                (double x);
extern double    ALM_PROTO_INTERNAL(atan2)                (double x, double y);
extern double    ALM_PROTO_INTERNAL(atan)                 (double x);
extern double    ALM_PROTO_INTERNAL(atanh)                (double x);
extern double    ALM_PROTO_INTERNAL(cbrt)                 (double x);
extern double    ALM_PROTO_INTERNAL(ceil)                 (double x);
extern double    ALM_PROTO_INTERNAL(copysign)             (double x, double y);
extern double    ALM_PROTO_INTERNAL(cos)                  (double x);
extern double    ALM_PROTO_INTERNAL(cosh)                 (double x);
extern double    ALM_PROTO_INTERNAL(cospi)                (double x);
extern double    ALM_PROTO_INTERNAL(exp10)                (double x);
extern double    ALM_PROTO_INTERNAL(exp2)                 (double x);
extern double    ALM_PROTO_INTERNAL(exp)                  (double x);
extern double    ALM_PROTO_INTERNAL(expm1)                (double x);
extern double    ALM_PROTO_INTERNAL(fabs)                 (double x);
extern double    ALM_PROTO_INTERNAL(fdim)                 (double x, double y);
extern double    ALM_PROTO_INTERNAL(floor)                (double x);
extern double    ALM_PROTO_INTERNAL(fma)                  (double x, double y, double z);
extern double    ALM_PROTO_INTERNAL(fmax)                 (double x, double y);
extern double    ALM_PROTO_INTERNAL(fmin)                 (double x, double y);
extern double    ALM_PROTO_INTERNAL(fmod)                 (double x, double y);
extern double    ALM_PROTO_INTERNAL(frexp)                (double value, int *exp);
extern double    ALM_PROTO_INTERNAL(hypot)                (double x, double y);
extern double    ALM_PROTO_INTERNAL(ldexp)                (double x, int exp);
extern double    ALM_PROTO_INTERNAL(log10)                (double x);
extern double    ALM_PROTO_INTERNAL(log1p)                (double x);
extern double    ALM_PROTO_INTERNAL(log2)                 (double x);
extern double    ALM_PROTO_INTERNAL(logb)                 (double x);
extern double    ALM_PROTO_INTERNAL(log)                  (double x);
extern double    ALM_PROTO_INTERNAL(modf)                 (double x, double *iptr);
extern double    ALM_PROTO_INTERNAL(nan)                  (const char *tagp);
extern double    ALM_PROTO_INTERNAL(nearbyint)            (double x);
extern double    ALM_PROTO_INTERNAL(nextafter)            (double x, double y);
extern double    ALM_PROTO_INTERNAL(nexttoward)           (double x, long double y);
extern double    ALM_PROTO_INTERNAL(pow)                  (double x, double y);
extern double    ALM_PROTO_INTERNAL(remainder)            (double x, double y);
extern double    ALM_PROTO_INTERNAL(remquo)               (double x, double y, int *quo);
extern double    ALM_PROTO_INTERNAL(rint)                 (double x);
extern double    ALM_PROTO_INTERNAL(round)                (double f);
extern double    ALM_PROTO_INTERNAL(scalbln)              (double x, long int n);
extern double    ALM_PROTO_INTERNAL(scalbn)               (double x, int n);
extern double    ALM_PROTO_INTERNAL(sin)                  (double x);
extern double    ALM_PROTO_INTERNAL(sinh)                 (double x);
extern double    ALM_PROTO_INTERNAL(sinpi)                (double x);
extern double    ALM_PROTO_INTERNAL(sqrt)                 (double x);
extern double    ALM_PROTO_INTERNAL(tan)                  (double x);
extern double    ALM_PROTO_INTERNAL(tanh)                 (double x);
extern double    ALM_PROTO_INTERNAL(tanpi)                (double x);
extern double    ALM_PROTO_INTERNAL(trunc)                (double x);

/*
 * Single Precision functions
 */
extern float     ALM_PROTO_INTERNAL(acosf)                (float x);
extern float     ALM_PROTO_INTERNAL(acoshf)               (float x);
extern float     ALM_PROTO_INTERNAL(asinf)                (float x);
extern float     ALM_PROTO_INTERNAL(asinhf)               (float x);
extern float     ALM_PROTO_INTERNAL(atan2f)               (float x, float y);
extern float     ALM_PROTO_INTERNAL(atanf)                (float x);
extern float     ALM_PROTO_INTERNAL(atanhf)               (float x);
extern float     ALM_PROTO_INTERNAL(cbrtf)                (float x);
extern float     ALM_PROTO_INTERNAL(ceilf)                (float x);
extern float     ALM_PROTO_INTERNAL(copysignf)            (float x, float y);
extern float     ALM_PROTO_INTERNAL(cosf)                 (float x);
extern float     ALM_PROTO_INTERNAL(coshf)                (float fx);
extern float     ALM_PROTO_INTERNAL(cospif)               (float x);
extern float     ALM_PROTO_INTERNAL(exp10f)               (float x);
extern float     ALM_PROTO_INTERNAL(exp2f)                (float x);
extern float     ALM_PROTO_INTERNAL(expf)                 (float x);
extern float     ALM_PROTO_INTERNAL(expm1f)               (float x);
extern float     ALM_PROTO_INTERNAL(fabsf)                (float x);
extern float     ALM_PROTO_INTERNAL(fdimf)                (float x, float y);
extern float     ALM_PROTO_INTERNAL(floorf)               (float x);
extern float     ALM_PROTO_INTERNAL(fmaf)                 (float x, float y, float z);
extern float     ALM_PROTO_INTERNAL(fmaxf)                (float x, float y);
extern float     ALM_PROTO_INTERNAL(fminf)                (float x, float y);
extern float     ALM_PROTO_INTERNAL(fmodf)                (float x, float y);
extern float     ALM_PROTO_INTERNAL(frexpf)               (float value, int *exp);
extern float     ALM_PROTO_INTERNAL(hypotf)               (float x, float y);
extern float     ALM_PROTO_INTERNAL(ldexpf)               (float x, int exp);
extern float     ALM_PROTO_INTERNAL(log10f)               (float x);
extern float     ALM_PROTO_INTERNAL(log1pf)               (float x);
extern float     ALM_PROTO_INTERNAL(log2f)                (float x);
extern float     ALM_PROTO_INTERNAL(logbf)                (float x);
extern float     ALM_PROTO_INTERNAL(logf)                 (float x);
extern float     ALM_PROTO_INTERNAL(modff)                (float x, float *iptr);
extern float     ALM_PROTO_INTERNAL(nanf)                 (const char *tagp);
extern float     ALM_PROTO_INTERNAL(nearbyintf)           (float x);
extern float     ALM_PROTO_INTERNAL(nextafterf)           (float x, float y);
extern float     ALM_PROTO_INTERNAL(nexttowardf)          (float x, long double y);
extern float     ALM_PROTO_INTERNAL(powf)                 (float x, float y);
extern float     ALM_PROTO_INTERNAL(remainderf)           (float x, float y);
extern float     ALM_PROTO_INTERNAL(remquof)              (float x, float y, int *quo);
extern float     ALM_PROTO_INTERNAL(rintf)                (float x);
extern float     ALM_PROTO_INTERNAL(roundf)               (float f);
extern float     ALM_PROTO_INTERNAL(scalblnf)             (float x, long int n);
extern float     ALM_PROTO_INTERNAL(scalbnf)              (float x, int n);
extern float     ALM_PROTO_INTERNAL(sinf)                 (float x);
extern float     ALM_PROTO_INTERNAL(sinhf)                (float x);
extern float     ALM_PROTO_INTERNAL(sinpif)               (float x);
extern float     ALM_PROTO_INTERNAL(sqrtf)                (float x);
extern float     ALM_PROTO_INTERNAL(tanf)                 (float x);
extern float     ALM_PROTO_INTERNAL(tanhf)                (float x);
extern float     ALM_PROTO_INTERNAL(tanpif)               (float x);
extern float     ALM_PROTO_INTERNAL(truncf)               (float x);

/*
 * Integer variants
 */
extern int              ALM_PROTO_INTERNAL(finite)        (double x);
extern int              ALM_PROTO_INTERNAL(finitef)       (float x);
extern int              ALM_PROTO_INTERNAL(ilogb)         (double x);
extern int              ALM_PROTO_INTERNAL(ilogbf)        (float x);
extern long int         ALM_PROTO_INTERNAL(lrint)         (double x);
extern long int         ALM_PROTO_INTERNAL(lrintf)        (float x);
extern long int         ALM_PROTO_INTERNAL(lround)        (double d);
extern long int         ALM_PROTO_INTERNAL(lroundf)       (float f);
extern long long int    ALM_PROTO_INTERNAL(llrint)        (double x);
extern long long int    ALM_PROTO_INTERNAL(llrintf)       (float x);
extern long long int    ALM_PROTO_INTERNAL(llround)       (double d);
extern long long int    ALM_PROTO_INTERNAL(llroundf)      (float f);


/*
 * Complex variants
 */
extern fc64_t   ALM_PROTO_INTERNAL(cexp)          (fc64_t f);
extern fc32_t   ALM_PROTO_INTERNAL(cexpf)         (fc32_t f);
extern fc64_t   ALM_PROTO_INTERNAL(clog)          (fc64_t f);
extern fc32_t   ALM_PROTO_INTERNAL(clogf)         (fc32_t f);
extern fc64_t   ALM_PROTO_INTERNAL(cpow)          (fc64_t x, fc64_t y);
extern fc32_t   ALM_PROTO_INTERNAL(cpowf)         (fc32_t x, fc32_t y);


#include <immintrin.h>

/*
 * Vector Single precision
 */
extern __m128    ALM_PROTO_INTERNAL(vrs4_acosf)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_cbrtf)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_cosf)            (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_exp10f)          (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_exp2f)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_expf)            (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_expm1f)          (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_log10f)          (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_log1pf)          (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_log2f)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_logf)            (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_powf)            (__m128 x, __m128 y);
extern __m128    ALM_PROTO_INTERNAL(vrs4_sinf)            (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_tanf)            (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_coshf)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_tanhf)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_asinf)           (__m128 x);
extern __m128    ALM_PROTO_INTERNAL(vrs4_atanf)           (__m128 x);


/*
 * Vector Single precision, 8 elements
 */
extern __m256    ALM_PROTO_INTERNAL(vrs8_cbrtf)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_cosf)            (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_acosf)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_exp10f)          (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_exp2f)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_expf)            (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_expm1f)          (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_log10f)          (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_log1pf)          (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_log2f)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_logf)            (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_powf)            (__m256 x, __m256 y);
extern __m256    ALM_PROTO_INTERNAL(vrs8_sinf)            (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_tanf)            (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_atanf)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_coshf)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_tanhf)           (__m256 x);
extern __m256    ALM_PROTO_INTERNAL(vrs8_asinf)           (__m256 x);


/*
 * Vector Single precision, 8 elements
 */
extern __m512    ALM_PROTO_INTERNAL(vrs16_cbrtf)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_cosf)            (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_acosf)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_exp10f)          (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_exp2f)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_expf)            (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_expm1f)          (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_log10f)          (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_log1pf)          (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_log2f)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_logf)            (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_powf)            (__m512 x, __m512 y);
extern __m512    ALM_PROTO_INTERNAL(vrs16_sinf)            (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_asinf)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_tanf)            (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_atanf)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_coshf)           (__m512 x);
extern __m512    ALM_PROTO_INTERNAL(vrs16_tanhf)           (__m512 x);


/*
 * Vector Double precision
 */
extern __m128d   ALM_PROTO_INTERNAL(vrd2_cbrt)     (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_cosh)     (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_cos)      (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_exp10)    (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_exp2)     (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_exp)      (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_expm1)    (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_log10)    (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_log1p)    (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_log2)     (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_log)      (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_pow)      (__m128d x, __m128d y);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_sin)      (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_tan)      (__m128d x);
extern __m128d   ALM_PROTO_INTERNAL(vrd2_atan)     (__m128d x);

/*
 * Vector double precision, 4 element
 */
extern __m256d   ALM_PROTO_INTERNAL(vrd4_cbrt)     (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_cosh)     (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_cos)      (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_exp10)    (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_exp2)     (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_exp)      (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_expm1)    (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_log10)    (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_log1p)    (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_log2)     (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_log)      (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_pow)      (__m256d x, __m256d y);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_sin)      (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_tan)      (__m256d x);
extern __m256d   ALM_PROTO_INTERNAL(vrd4_atan)     (__m256d x);

/*
 * Vector double precision, 8 elements
 */
extern __m512d   ALM_PROTO_INTERNAL(vrd8_cbrt)     (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_cosh)     (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_cos)      (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_exp10)    (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_exp2)     (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_exp)      (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_expm1)    (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_log10)    (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_log1p)    (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_log2)     (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_log)      (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_pow)      (__m512d x, __m512d y);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_sin)      (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_asin)     (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_tan)      (__m512d x);
extern __m512d   ALM_PROTO_INTERNAL(vrd8_atan)     (__m512d x);



extern void      ALM_PROTO_INTERNAL(sincos)        (double x, double *s, double *c);
extern void      ALM_PROTO_INTERNAL(sincosf)       (float x, float *s, float *c);
extern void      ALM_PROTO_INTERNAL(vrd2_sincos)   (__m128d x, __m128d* ys, __m128d* yc);
extern void      ALM_PROTO_INTERNAL(vrs4_sincosf)  (__m128 x, __m128* ys, __m128* yc);

/*
* Vector Array versions
*/
extern void      ALM_PROTO_INTERNAL(vrda_cbrt)     (int len, double *src, double* dst );
extern void      ALM_PROTO_INTERNAL(vrda_cos)      (int n, double *x, double *y);
extern void      ALM_PROTO_INTERNAL(vrda_exp10)    (int n, double* x, double* y);
extern void      ALM_PROTO_INTERNAL(vrda_exp2)     (int n, double* x, double* y);
extern void      ALM_PROTO_INTERNAL(vrda_exp)      (int n, double* x, double* y);
extern void      ALM_PROTO_INTERNAL(vrda_expm1)    (int n, double* x, double* y);
extern void      ALM_PROTO_INTERNAL(vrda_log10)    (int n, double *src, double* dst);
extern void      ALM_PROTO_INTERNAL(vrda_log1p)    (int n, double *src, double* dst);
extern void      ALM_PROTO_INTERNAL(vrda_log2)     (int n, double *src, double* dst);
extern void      ALM_PROTO_INTERNAL(vrda_log)      (int n, double *src, double* dst);
extern void      ALM_PROTO_INTERNAL(vrda_sincos)   (int n, double *, double *, double *);
extern void      ALM_PROTO_INTERNAL(vrda_sin)      (int n, double *x, double *y);

extern void      ALM_PROTO_INTERNAL(vrsa_cbrtf)    (int len, float *src, float* dst);
extern void      ALM_PROTO_INTERNAL(vrsa_cosf)     (int n, float *x, float *y);
extern void      ALM_PROTO_INTERNAL(vrsa_exp10f)   (int n, float* x, float* y);
extern void      ALM_PROTO_INTERNAL(vrsa_exp2f)    (int n, float* x, float* y);
extern void      ALM_PROTO_INTERNAL(vrsa_expf)     (int n, float* x, float* y);
extern void      ALM_PROTO_INTERNAL(vrsa_expm1f)   (int n, float* x, float* y);
extern void      ALM_PROTO_INTERNAL(vrsa_log10f)   (int n, float *src, float* dst);
extern void      ALM_PROTO_INTERNAL(vrsa_log1pf)   (int n, float *src, float* dst);
extern void      ALM_PROTO_INTERNAL(vrsa_log2f)    (int n, float *src, float* dst);
extern void      ALM_PROTO_INTERNAL(vrsa_logf)     (int n, float *src, float* dst);
extern void      ALM_PROTO_INTERNAL(vrsa_powf)     (int n, float *s1, float *s2, float* d);
extern void      ALM_PROTO_INTERNAL(vrsa_powxf)    (int n, float *s1, float s2, float* d);
extern void      ALM_PROTO_INTERNAL(vrsa_sincosf)  (int n, float *x, float *ys, float *yc);
extern void      ALM_PROTO_INTERNAL(vrsa_sinf)     (int n, float *x, float *y);

#ifdef __cplusplus
}
#endif

