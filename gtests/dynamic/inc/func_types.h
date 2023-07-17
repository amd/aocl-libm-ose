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
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef FUNC_TYPES_H_INCLUDED
#define FUNC_TYPES_H_INCLUDED

#include <immintrin.h>
#include <libm/types.h>

/*func ptr types*/
/*scalar*/
typedef float  (*funcf)     (float);
typedef float  (*funcf_2)   (float, float);
typedef double (*func)      (double);
typedef double (*func_2)    (double, double);
typedef float  (*funcf_int) (float, int);
typedef double (*func_int)  (double, int);

/* complex scalar */
typedef fc32_t (*funcf_cmplx) (fc32_t);
typedef fc64_t (*func_cmplx) (fc64_t);
typedef fc32_t (*funcf_cmplx_2) (fc32_t, fc32_t);
typedef fc64_t (*func_cmplx_2) (fc64_t, fc64_t);

/*other scalar*/
typedef float  (*funcf_nan)    (const char*);
typedef double (*func_nan)     (const char*);
typedef float  (*funcf_remquo) (float, float, int*);
typedef double (*func_remquo)  (double, double, int*);
typedef void   (*funcf_sincos) (float, float*, float*);
typedef void   (*func_sincos)  (double, double*, double*);

/*vector*/
typedef __m128d (*func_v2d)    (__m128d);
typedef __m128d (*func_v2d_2)  (__m128d, __m128d);
typedef __m256d (*func_v4d)    (__m256d);
typedef __m256d (*func_v4d_2)  (__m256d, __m256d);
typedef __m128  (*funcf_v4s)   (__m128);
typedef __m128  (*funcf_v4s_2) (__m128, __m128);
typedef __m256  (*funcf_v8s)   (__m256);
typedef __m256  (*funcf_v8s_2) (__m256, __m256);
typedef void    (*func_vrd4_sincos) (__m256d, __m256d*, __m256d*);

/*array vector*/
typedef void (*funcf_va)   (int, float*, float*);
typedef void (*funcf_va_2) (int, float*, float*, float*);
typedef void (*func_va)    (int, double*, double*);
typedef void (*func_va_2)  (int, double*, double*, double*);

/* avx512 functions */
#if defined(__AVX512__)
typedef __m512d (*func_v8d)     (__m512d);
typedef __m512d (*func_v8d_2)   (__m512d, __m512d);

typedef __m512  (*funcf_v16s)   (__m512);
typedef __m512  (*funcf_v16s_2) (__m512, __m512);

typedef void    (*func_vrd8_sincos) (__m512d, __m512d*, __m512d*);
#endif

#endif
