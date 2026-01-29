/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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
/*
 * High-performance cross-platform LD_PRELOAD shim for GCC libm functions
 * Optimized for minimal overhead with one-time symbol loading
 * Compatible with Windows and Linux without pthread dependency
 */

// GCC Version Detection Macros for libm Compatibility
// These macros handle different GCC versions and their corresponding libm/libmvec capabilities
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "alm_test.h"

// GLIBC version detection for libmvec availability
#ifdef __GLIBC__
    #define GLIBC_VERSION (__GLIBC__ * 100 + __GLIBC_MINOR__)
#else
    #define GLIBC_VERSION 0
#endif

// ----------------------------------------------------------------------------
// Scalar libm availability macros derived from libm.abilist
// Reference: https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/unix/sysv/linux/x86_64/64/libm.abilist
// Tiers:
//   2.2.5  : Baseline set (sin, cos, exp, log, pow, sqrt, etc.)
//   2.15   : __*_finite optimized variants
//   2.27   : Corrected exp/log/pow implementations
//   2.35   : hypot, fmod precision fixes
//   2.38   : Additional fmod fixes
//   2.39   : exp10 standardized
//   2.40   : exp10m1, exp2m1, log10p1, log2p1, logp1
//   2.41   : *pi family (acospi, asinpi, atanpi, atan2pi, cospi, sinpi, tanpi)
//   2.42   : compound, pown, powr, rootn, rsqrt
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// GLIBC 2.22 - Initial libmvec release (baseline vector functions)
// Reference: https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/unix/sysv/linux/x86_64/libmvec.abilist
// Available: cos, exp, log, sin, pow (vv), sincos (vvv)
// All vector widths: _ZGVbN* (SSE), _ZGVcN* (SSE compat), _ZGVdN* (AVX2), _ZGVeN* (AVX512)
// ----------------------------------------------------------------------------
#if GLIBC_VERSION >= 222
    #define GCC_HAS_VECTOR_COS 1
    #define GCC_HAS_VECTOR_EXP 1
    #define GCC_HAS_VECTOR_LOG 1
    #define GCC_HAS_VECTOR_SIN 1
    #define GCC_HAS_VECTOR_POW 1
    #define GCC_HAS_VECTOR_SINCOS 1
#else
    #define GCC_HAS_VECTOR_COS 0
    #define GCC_HAS_VECTOR_EXP 0
    #define GCC_HAS_VECTOR_LOG 0
    #define GCC_HAS_VECTOR_SIN 0
    #define GCC_HAS_VECTOR_POW 0
    #define GCC_HAS_VECTOR_SINCOS 0
#endif

// ----------------------------------------------------------------------------
// GLIBC 2.35 - Extended libmvec functions
// Reference: https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/unix/sysv/linux/x86_64/libmvec.abilist
// Added: acos, acosh, asin, asinh, atan, atanh, cbrt, cosh, erf, erfc,
//        exp10, exp2, expm1, log10, log1p, log2, sinh, tan, tanh,
//        atan2 (vv), hypot (vv)
// All vector widths supported
// ----------------------------------------------------------------------------
#if GLIBC_VERSION >= 235
    #define GCC_HAS_VECTOR_ACOS 1
    #define GCC_HAS_VECTOR_ACOSH 1
    #define GCC_HAS_VECTOR_ASIN 1
    #define GCC_HAS_VECTOR_ASINH 1
    #define GCC_HAS_VECTOR_ATAN 1
    #define GCC_HAS_VECTOR_ATANH 1
    #define GCC_HAS_VECTOR_CBRT 1
    #define GCC_HAS_VECTOR_COSH 1
    #define GCC_HAS_VECTOR_ERF 1
    #define GCC_HAS_VECTOR_ERFC 1
    #define GCC_HAS_VECTOR_EXP10 1
    #define GCC_HAS_VECTOR_EXP2 1
    #define GCC_HAS_VECTOR_EXPM1 1
    #define GCC_HAS_VECTOR_LOG10 1
    #define GCC_HAS_VECTOR_LOG1P 1
    #define GCC_HAS_VECTOR_LOG2 1
    #define GCC_HAS_VECTOR_SINH 1
    #define GCC_HAS_VECTOR_TAN 1
    #define GCC_HAS_VECTOR_TANH 1
    #define GCC_HAS_VECTOR_ATAN2 1
    #define GCC_HAS_VECTOR_HYPOT 1
#else
    #define GCC_HAS_VECTOR_ACOS 0
    #define GCC_HAS_VECTOR_ACOSH 0
    #define GCC_HAS_VECTOR_ASIN 0
    #define GCC_HAS_VECTOR_ASINH 0
    #define GCC_HAS_VECTOR_ATAN 0
    #define GCC_HAS_VECTOR_ATANH 0
    #define GCC_HAS_VECTOR_CBRT 0
    #define GCC_HAS_VECTOR_COSH 0
    #define GCC_HAS_VECTOR_ERF 0
    #define GCC_HAS_VECTOR_ERFC 0
    #define GCC_HAS_VECTOR_EXP10 0
    #define GCC_HAS_VECTOR_EXP2 0
    #define GCC_HAS_VECTOR_EXPM1 0
    #define GCC_HAS_VECTOR_LOG10 0
    #define GCC_HAS_VECTOR_LOG1P 0
    #define GCC_HAS_VECTOR_LOG2 0
    #define GCC_HAS_VECTOR_SINH 0
    #define GCC_HAS_VECTOR_TAN 0
    #define GCC_HAS_VECTOR_TANH 0
    #define GCC_HAS_VECTOR_ATAN2 0
    #define GCC_HAS_VECTOR_HYPOT 0
#endif

// ----------------------------------------------------------------------------
// SQRT vector support - INTENTIONALLY DISABLED
// Note: While sqrt appears in some libmvec implementations, it's not
// consistently exported in libmvec.abilist. Disabled to prevent undefined
// symbol errors at runtime.
// ----------------------------------------------------------------------------
#define GCC_HAS_VECTOR_SQRT 0

#define GCC_VERSION_INFO_STRING \
	"GCC " STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__) "." STRINGIFY(__GNUC_PATCHLEVEL__) \
	", GLIBC " STRINGIFY(__GLIBC__) "." STRINGIFY(__GLIBC_MINOR__)
#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

#define SHIM_EXPORT __attribute__((visibility("default")))

// ============================================================================
// FUNCTION POINTER TYPEDEFS ORGANIZED BY VARIANT
// ============================================================================
// --- Single Precision Scalar (ss) Functions ---
typedef float (*gcc_acos_ss_func_t)(float);
typedef float (*gcc_acosh_ss_func_t)(float);
typedef float (*gcc_asin_ss_func_t)(float);
typedef float (*gcc_asinh_ss_func_t)(float);
typedef float (*gcc_atan_ss_func_t)(float);
typedef float (*gcc_atan2_ss_func_t)(float, float);
typedef float (*gcc_atanh_ss_func_t)(float);
typedef float (*gcc_cbrt_ss_func_t)(float);
typedef float (*gcc_ceil_ss_func_t)(float);
typedef float (*gcc_copysign_ss_func_t)(float, float);
typedef float (*gcc_cos_ss_func_t)(float);
typedef float (*gcc_cosh_ss_func_t)(float);
typedef float (*gcc_cospi_ss_func_t)(float);
typedef float (*gcc_erf_ss_func_t)(float);
typedef float (*gcc_erfc_ss_func_t)(float);
typedef float (*gcc_exp_ss_func_t)(float);
typedef float (*gcc_exp10_ss_func_t)(float);
typedef float (*gcc_exp2_ss_func_t)(float);
typedef float (*gcc_expm1_ss_func_t)(float);
typedef float (*gcc_fabs_ss_func_t)(float);
typedef float (*gcc_fdim_ss_func_t)(float, float);
typedef int (*gcc_finite_ss_func_t)(float);
typedef float (*gcc_floor_ss_func_t)(float);
typedef float (*gcc_fma_ss_func_t)(float, float, float);
typedef float (*gcc_fmax_ss_func_t)(float, float);
typedef float (*gcc_fmin_ss_func_t)(float, float);
typedef float (*gcc_fmod_ss_func_t)(float, float);
typedef float (*gcc_frexp_ss_func_t)(float, int*);
typedef float (*gcc_hypot_ss_func_t)(float, float);
typedef int (*gcc_ilogb_ss_func_t)(float);
typedef float (*gcc_ldexp_ss_func_t)(float, int);
typedef long long (*gcc_llrint_ss_func_t)(float);
typedef long long (*gcc_llround_ss_func_t)(float);
typedef float (*gcc_log_ss_func_t)(float);
typedef float (*gcc_log10_ss_func_t)(float);
typedef float (*gcc_log1p_ss_func_t)(float);
typedef float (*gcc_log2_ss_func_t)(float);
typedef float (*gcc_logb_ss_func_t)(float);
typedef long (*gcc_lrint_ss_func_t)(float);
typedef long (*gcc_lround_ss_func_t)(float);
typedef float (*gcc_modf_ss_func_t)(float, float*);
typedef float (*gcc_nearbyint_ss_func_t)(float);
typedef float (*gcc_nextafter_ss_func_t)(float, float);
typedef float (*gcc_nexttoward_ss_func_t)(float, long double);
typedef float (*gcc_pow_ss_func_t)(float, float);
typedef float (*gcc_remainder_ss_func_t)(float, float);
typedef float (*gcc_remquo_ss_func_t)(float, float, int*);
typedef float (*gcc_rint_ss_func_t)(float);
typedef float (*gcc_round_ss_func_t)(float);
typedef float (*gcc_scalbln_ss_func_t)(float, long);
typedef float (*gcc_scalbn_ss_func_t)(float, int);
typedef float (*gcc_sin_ss_func_t)(float);
typedef void (*gcc_sincos_ss_func_t)(float, float*, float*);
typedef float (*gcc_sinh_ss_func_t)(float);
typedef float (*gcc_sinpi_ss_func_t)(float);
typedef float (*gcc_sqrt_ss_func_t)(float);
typedef float (*gcc_tan_ss_func_t)(float);
typedef float (*gcc_tanh_ss_func_t)(float);
typedef float (*gcc_tanpi_ss_func_t)(float);
typedef float (*gcc_trunc_ss_func_t)(float);

// --- Double Precision Scalar (sd) Functions ---
typedef double (*gcc_acos_sd_func_t)(double);
typedef double (*gcc_acosh_sd_func_t)(double);
typedef double (*gcc_asin_sd_func_t)(double);
typedef double (*gcc_asinh_sd_func_t)(double);
typedef double (*gcc_atan_sd_func_t)(double);
typedef double (*gcc_atan2_sd_func_t)(double, double);
typedef double (*gcc_atanh_sd_func_t)(double);
typedef double (*gcc_cbrt_sd_func_t)(double);
typedef double (*gcc_ceil_sd_func_t)(double);
typedef double (*gcc_copysign_sd_func_t)(double, double);
typedef double (*gcc_cos_sd_func_t)(double);
typedef double (*gcc_cosh_sd_func_t)(double);
typedef double (*gcc_cospi_sd_func_t)(double);
typedef double (*gcc_erf_sd_func_t)(double);
typedef double (*gcc_erfc_sd_func_t)(double);
typedef double (*gcc_exp_sd_func_t)(double);
typedef double (*gcc_exp10_sd_func_t)(double);
typedef double (*gcc_exp2_sd_func_t)(double);
typedef double (*gcc_expm1_sd_func_t)(double);
typedef double (*gcc_fabs_sd_func_t)(double);
typedef double (*gcc_fdim_sd_func_t)(double, double);
typedef int (*gcc_finite_sd_func_t)(double);
typedef double (*gcc_floor_sd_func_t)(double);
typedef double (*gcc_fma_sd_func_t)(double, double, double);
typedef double (*gcc_fmax_sd_func_t)(double, double);
typedef double (*gcc_fmin_sd_func_t)(double, double);
typedef double (*gcc_fmod_sd_func_t)(double, double);
typedef double (*gcc_frexp_sd_func_t)(double, int*);
typedef double (*gcc_hypot_sd_func_t)(double, double);
typedef int (*gcc_ilogb_sd_func_t)(double);
typedef double (*gcc_ldexp_sd_func_t)(double, int);
typedef long long (*gcc_llrint_sd_func_t)(double);
typedef long long (*gcc_llround_sd_func_t)(double);
typedef double (*gcc_log_sd_func_t)(double);
typedef double (*gcc_log10_sd_func_t)(double);
typedef double (*gcc_log1p_sd_func_t)(double);
typedef double (*gcc_log2_sd_func_t)(double);
typedef double (*gcc_logb_sd_func_t)(double);
typedef long (*gcc_lrint_sd_func_t)(double);
typedef long (*gcc_lround_sd_func_t)(double);
typedef double (*gcc_modf_sd_func_t)(double, double*);
typedef double (*gcc_nearbyint_sd_func_t)(double);
typedef double (*gcc_nextafter_sd_func_t)(double, double);
typedef double (*gcc_nexttoward_sd_func_t)(double, long double);
typedef double (*gcc_pow_sd_func_t)(double, double);
typedef double (*gcc_remainder_sd_func_t)(double, double);
typedef double (*gcc_remquo_sd_func_t)(double, double, int*);
typedef double (*gcc_rint_sd_func_t)(double);
typedef double (*gcc_round_sd_func_t)(double);
typedef double (*gcc_scalbln_sd_func_t)(double, long);
typedef double (*gcc_scalbn_sd_func_t)(double, int);
typedef double (*gcc_sin_sd_func_t)(double);
typedef void (*gcc_sincos_sd_func_t)(double, double*, double*);
typedef double (*gcc_sinh_sd_func_t)(double);
typedef double (*gcc_sinpi_sd_func_t)(double);
typedef double (*gcc_sqrt_sd_func_t)(double);
typedef double (*gcc_tan_sd_func_t)(double);
typedef double (*gcc_tanh_sd_func_t)(double);
typedef double (*gcc_tanpi_sd_func_t)(double);
typedef double (*gcc_trunc_sd_func_t)(double);

// --- Double Precision 128-bit Vector (vrd2) Functions ---
typedef __m128d (*gcc_acos_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_asin_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_atan_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_cbrt_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_cos_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_cosh_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_erf_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_erfc_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_exp_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_exp10_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_exp2_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_fabs_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_linearfrac_vrd2_func_t)(__m128d, __m128d, double, double, double, double);
typedef __m128d (*gcc_log_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_log10_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_log1p_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_log2_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_pow_vrd2_func_t)(__m128d, __m128d);
typedef __m128d (*gcc_powx_vrd2_func_t)(__m128d, double);
typedef __m128d (*gcc_sin_vrd2_func_t)(__m128d);
typedef void (*gcc_sincos_vrd2_func_t)(__m128d, __m128d*, __m128d*);
typedef __m128d (*gcc_sqrt_vrd2_func_t)(__m128d);
typedef __m128d (*gcc_tan_vrd2_func_t)(__m128d);

// --- Single Precision 128-bit Vector (vrs4) Functions ---
typedef __m128 (*gcc_acos_vrs4_func_t)(__m128);
typedef __m128 (*gcc_asin_vrs4_func_t)(__m128);
typedef __m128 (*gcc_atan_vrs4_func_t)(__m128);
typedef __m128 (*gcc_cbrt_vrs4_func_t)(__m128);
typedef __m128 (*gcc_cos_vrs4_func_t)(__m128);
typedef __m128 (*gcc_cosh_vrs4_func_t)(__m128);
typedef __m128 (*gcc_erf_vrs4_func_t)(__m128);
typedef __m128 (*gcc_erfc_vrs4_func_t)(__m128);
typedef __m128 (*gcc_exp_vrs4_func_t)(__m128);
typedef __m128 (*gcc_exp10_vrs4_func_t)(__m128);
typedef __m128 (*gcc_exp2_vrs4_func_t)(__m128);
typedef __m128 (*gcc_expm1_vrs4_func_t)(__m128);
typedef __m128 (*gcc_fabs_vrs4_func_t)(__m128);
typedef __m128 (*gcc_linearfrac_vrs4_func_t)(__m128, __m128, float, float, float, float);
typedef __m128 (*gcc_log_vrs4_func_t)(__m128);
typedef __m128 (*gcc_log10_vrs4_func_t)(__m128);
typedef __m128 (*gcc_log1p_vrs4_func_t)(__m128);
typedef __m128 (*gcc_log2_vrs4_func_t)(__m128);
typedef __m128 (*gcc_pow_vrs4_func_t)(__m128, __m128);
typedef __m128 (*gcc_powx_vrs4_func_t)(__m128, float);
typedef __m128 (*gcc_sin_vrs4_func_t)(__m128);
typedef void (*gcc_sincos_vrs4_func_t)(__m128, __m128*, __m128*);
typedef __m128 (*gcc_sqrt_vrs4_func_t)(__m128);
typedef __m128 (*gcc_tan_vrs4_func_t)(__m128);
typedef __m128 (*gcc_tanh_vrs4_func_t)(__m128);

// --- Double Precision 256-bit Vector (vrd4) Functions ---
typedef __m256d (*gcc_acos_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_asin_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_atan_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_cos_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_erf_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_erfc_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_exp_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_exp2_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_fabs_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_linearfrac_vrd4_func_t)(__m256d, __m256d, double, double, double, double);
typedef __m256d (*gcc_log_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_log2_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_pow_vrd4_func_t)(__m256d, __m256d);
typedef __m256d (*gcc_powx_vrd4_func_t)(__m256d, double);
typedef __m256d (*gcc_sin_vrd4_func_t)(__m256d);
typedef void (*gcc_sincos_vrd4_func_t)(__m256d, __m256d*, __m256d*);
typedef __m256d (*gcc_sqrt_vrd4_func_t)(__m256d);
typedef __m256d (*gcc_tan_vrd4_func_t)(__m256d);

// --- Single Precision 256-bit Vector (vrs8) Functions ---
typedef __m256 (*gcc_acos_vrs8_func_t)(__m256);
typedef __m256 (*gcc_asin_vrs8_func_t)(__m256);
typedef __m256 (*gcc_atan_vrs8_func_t)(__m256);
typedef __m256 (*gcc_cos_vrs8_func_t)(__m256);
typedef __m256 (*gcc_cosh_vrs8_func_t)(__m256);
typedef __m256 (*gcc_erf_vrs8_func_t)(__m256);
typedef __m256 (*gcc_erfc_vrs8_func_t)(__m256);
typedef __m256 (*gcc_exp_vrs8_func_t)(__m256);
typedef __m256 (*gcc_exp2_vrs8_func_t)(__m256);
typedef __m256 (*gcc_fabs_vrs8_func_t)(__m256);
typedef __m256 (*gcc_linearfrac_vrs8_func_t)(__m256, __m256, float, float, float, float);
typedef __m256 (*gcc_log_vrs8_func_t)(__m256);
typedef __m256 (*gcc_log10_vrs8_func_t)(__m256);
typedef __m256 (*gcc_log2_vrs8_func_t)(__m256);
typedef __m256 (*gcc_pow_vrs8_func_t)(__m256, __m256);
typedef __m256 (*gcc_powx_vrs8_func_t)(__m256, float);
typedef __m256 (*gcc_sin_vrs8_func_t)(__m256);
typedef void (*gcc_sincos_vrs8_func_t)(__m256, __m256*, __m256*);
typedef __m256 (*gcc_sqrt_vrs8_func_t)(__m256);
typedef __m256 (*gcc_tan_vrs8_func_t)(__m256);
typedef __m256 (*gcc_tanh_vrs8_func_t)(__m256);

// --- Single Precision Array (vrsa) Functions ---
typedef void (*gcc_acos_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_add_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_addfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_asin_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_atan_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_cbrt_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_cos_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_cosh_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_div_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_divfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_erf_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_erfc_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_exp_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_exp10_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_exp2_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_expm1_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_fabs_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_fmax_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_fmaxfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_fmin_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_fminfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_linearfrac_vrsa_func_t)(int, const float*,
    const float*, float, float, float, float, float*);
typedef void (*gcc_log_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_log10_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_log1p_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_log2_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_mul_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_mulfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_pow_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_powx_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_sin_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_sincos_vrsa_func_t)(int, const float*, float*, float*);
typedef void (*gcc_sqrt_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_sub_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*gcc_subfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*gcc_tan_vrsa_func_t)(int, const float*, float*);
typedef void (*gcc_tanh_vrsa_func_t)(int, const float*, float*);

// --- Double Precision Array (vrda) Functions ---
typedef void (*gcc_acos_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_add_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_addi_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_asin_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_atan_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_cbrt_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_cos_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_cosh_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_div_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_divi_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_erf_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_erfc_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_exp_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_exp10_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_exp2_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_expm1_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_fabs_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_fmax_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_fmaxi_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_fmin_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_fmini_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_linearfrac_vrda_func_t)(int, const double*, const double*, double, double, double, double, double*);
typedef void (*gcc_log_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_log10_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_log1p_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_log2_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_mul_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_muli_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_pow_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_powx_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_sin_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_sincos_vrda_func_t)(int, const double*, double*, double*);
typedef void (*gcc_sqrt_vrda_func_t)(int, const double*, double*);
typedef void (*gcc_sub_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*gcc_subi_vrda_func_t)(int, const double*, double, double*);
typedef void (*gcc_tan_vrda_func_t)(int, const double*, double*);

#ifdef __AVX512F__
// --- Double Precision 512-bit Vector (vrd8) Functions ---
typedef __m512d (*gcc_asin_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_atan_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_cos_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_erf_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_erfc_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_exp_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_exp2_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_linearfrac_vrd8_func_t)(__m512d, __m512d, double, double, double, double);
typedef __m512d (*gcc_log_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_log2_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_pow_vrd8_func_t)(__m512d, __m512d);
typedef __m512d (*gcc_powx_vrd8_func_t)(__m512d, double);
typedef __m512d (*gcc_sin_vrd8_func_t)(__m512d);
typedef void (*gcc_sincos_vrd8_func_t)(__m512d, __m512d*, __m512d*);
typedef __m512d (*gcc_sqrt_vrd8_func_t)(__m512d);
typedef __m512d (*gcc_tan_vrd8_func_t)(__m512d);

// --- Single Precision 512-bit Vector (vrs16) Functions ---
typedef __m512 (*gcc_acos_vrs16_func_t)(__m512);
typedef __m512 (*gcc_asin_vrs16_func_t)(__m512);
typedef __m512 (*gcc_atan_vrs16_func_t)(__m512);
typedef __m512 (*gcc_cos_vrs16_func_t)(__m512);
typedef __m512 (*gcc_erf_vrs16_func_t)(__m512);
typedef __m512 (*gcc_erfc_vrs16_func_t)(__m512);
typedef __m512 (*gcc_exp_vrs16_func_t)(__m512);
typedef __m512 (*gcc_exp2_vrs16_func_t)(__m512);
typedef __m512 (*gcc_linearfrac_vrs16_func_t)(__m512, __m512, float, float, float, float);
typedef __m512 (*gcc_log_vrs16_func_t)(__m512);
typedef __m512 (*gcc_log10_vrs16_func_t)(__m512);
typedef __m512 (*gcc_log2_vrs16_func_t)(__m512);
typedef __m512 (*gcc_pow_vrs16_func_t)(__m512, __m512);
typedef __m512 (*gcc_powx_vrs16_func_t)(__m512, float);
typedef __m512 (*gcc_sin_vrs16_func_t)(__m512);
typedef void (*gcc_sincos_vrs16_func_t)(__m512, __m512*, __m512*);
typedef __m512 (*gcc_sqrt_vrs16_func_t)(__m512);
typedef __m512 (*gcc_tan_vrs16_func_t)(__m512);
typedef __m512 (*gcc_tanh_vrs16_func_t)(__m512);
#endif

// ============================================================================
// GLOBAL FUNCTION POINTERS ORGANIZED BY VARIANT
// ============================================================================
static struct {
    // ============================================================================
    // SINGLE PRECISION SCALAR (ss) VARIANTS
    // ============================================================================
    gcc_acos_ss_func_t acosf;
    gcc_acosh_ss_func_t acoshf;
    gcc_asin_ss_func_t asinf;
    gcc_asinh_ss_func_t asinhf;
    gcc_atan_ss_func_t atanf;
    gcc_atan2_ss_func_t atan2f;
    gcc_atanh_ss_func_t atanhf;
    gcc_cbrt_ss_func_t cbrtf;
    gcc_ceil_ss_func_t ceilf;
    gcc_copysign_ss_func_t copysignf;
    gcc_cos_ss_func_t cosf;
    gcc_cosh_ss_func_t coshf;
    gcc_cospi_ss_func_t cospif;
    gcc_erf_ss_func_t erff;
    gcc_erfc_ss_func_t erfcf;
    gcc_exp_ss_func_t expf;
    gcc_exp10_ss_func_t exp10f;
    gcc_exp2_ss_func_t exp2f;
    gcc_expm1_ss_func_t expm1f;
    gcc_fabs_ss_func_t fabsf;
    gcc_fdim_ss_func_t fdimf;
    gcc_finite_ss_func_t finitef;
    gcc_floor_ss_func_t floorf;
    gcc_fma_ss_func_t fmaf;
    gcc_fmax_ss_func_t fmaxf;
    gcc_fmin_ss_func_t fminf;
    gcc_fmod_ss_func_t fmodf;
    gcc_frexp_ss_func_t frexpf;
    gcc_hypot_ss_func_t hypotf;
    gcc_ilogb_ss_func_t ilogbf;
    gcc_ldexp_ss_func_t ldexpf;
    gcc_llrint_ss_func_t llrintf;
    gcc_llround_ss_func_t llroundf;
    gcc_log_ss_func_t logf;
    gcc_log10_ss_func_t log10f;
    gcc_log1p_ss_func_t log1pf;
    gcc_log2_ss_func_t log2f;
    gcc_logb_ss_func_t logbf;
    gcc_lrint_ss_func_t lrintf;
    gcc_lround_ss_func_t lroundf;
    gcc_modf_ss_func_t modff;
    gcc_nearbyint_ss_func_t nearbyintf;
    gcc_nextafter_ss_func_t nextafterf;
    gcc_nexttoward_ss_func_t nexttowardf;
    gcc_pow_ss_func_t powf;
    gcc_remainder_ss_func_t remainderf;
    gcc_remquo_ss_func_t remquof;
    gcc_rint_ss_func_t rintf;
    gcc_round_ss_func_t roundf;
    gcc_scalbln_ss_func_t scalblnf;
    gcc_scalbn_ss_func_t scalbnf;
    gcc_sin_ss_func_t sinf;
    gcc_sincos_ss_func_t sincosf;
    gcc_sinh_ss_func_t sinhf;
    gcc_sinpi_ss_func_t sinpif;
    gcc_sqrt_ss_func_t sqrtf;
    gcc_tan_ss_func_t tanf;
    gcc_tanh_ss_func_t tanhf;
    gcc_tanpi_ss_func_t tanpif;
    gcc_trunc_ss_func_t truncf;

    // ============================================================================
    // DOUBLE PRECISION SCALAR (sd) VARIANTS
    // ============================================================================
    gcc_acos_sd_func_t acos;
    gcc_acosh_sd_func_t acosh;
    gcc_asin_sd_func_t asin;
    gcc_asinh_sd_func_t asinh;
    gcc_atan_sd_func_t atan;
    gcc_atan2_sd_func_t atan2;
    gcc_atanh_sd_func_t atanh;
    gcc_cbrt_sd_func_t cbrt;
    gcc_ceil_sd_func_t ceil;
    gcc_copysign_sd_func_t copysign;
    gcc_cos_sd_func_t cos;
    gcc_cosh_sd_func_t cosh;
    gcc_cospi_sd_func_t cospi;
    gcc_erf_sd_func_t erf;
    gcc_erfc_sd_func_t erfc;
    gcc_exp_sd_func_t exp;
    gcc_exp10_sd_func_t exp10;
    gcc_exp2_sd_func_t exp2;
    gcc_expm1_sd_func_t expm1;
    gcc_fabs_sd_func_t fabs;
    gcc_fdim_sd_func_t fdim;
    gcc_finite_sd_func_t finite;
    gcc_floor_sd_func_t floor;
    gcc_fma_sd_func_t fma;
    gcc_fmax_sd_func_t fmax;
    gcc_fmin_sd_func_t fmin;
    gcc_fmod_sd_func_t fmod;
    gcc_frexp_sd_func_t frexp;
    gcc_hypot_sd_func_t hypot;
    gcc_ilogb_sd_func_t ilogb;
    gcc_ldexp_sd_func_t ldexp;
    gcc_llrint_sd_func_t llrint;
    gcc_llround_sd_func_t llround;
    gcc_log_sd_func_t log;
    gcc_log10_sd_func_t log10;
    gcc_log1p_sd_func_t log1p;
    gcc_log2_sd_func_t log2;
    gcc_logb_sd_func_t logb;
    gcc_lrint_sd_func_t lrint;
    gcc_lround_sd_func_t lround;
    gcc_modf_sd_func_t modf;
    gcc_nearbyint_sd_func_t nearbyint;
    gcc_nextafter_sd_func_t nextafter;
    gcc_nexttoward_sd_func_t nexttoward;
    gcc_pow_sd_func_t pow;
    gcc_remainder_sd_func_t remainder;
    gcc_remquo_sd_func_t remquo;
    gcc_rint_sd_func_t rint;
    gcc_round_sd_func_t round;
    gcc_scalbln_sd_func_t scalbln;
    gcc_scalbn_sd_func_t scalbn;
    gcc_sin_sd_func_t sin;
    gcc_sincos_sd_func_t sincos;
    gcc_sinh_sd_func_t sinh;
    gcc_sinpi_sd_func_t sinpi;
    gcc_sqrt_sd_func_t sqrt;
    gcc_tan_sd_func_t tan;
    gcc_tanh_sd_func_t tanh;
    gcc_tanpi_sd_func_t tanpi;
    gcc_trunc_sd_func_t trunc;

    // ============================================================================
    // DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
    // ============================================================================
    gcc_acos_vrd2_func_t acos_vrd2;
    gcc_asin_vrd2_func_t asin_vrd2;
    gcc_atan_vrd2_func_t atan_vrd2;
    gcc_cbrt_vrd2_func_t cbrt_vrd2;
    gcc_cos_vrd2_func_t cos_vrd2;
    gcc_cosh_vrd2_func_t cosh_vrd2;
    gcc_erf_vrd2_func_t erf_vrd2;
    gcc_erfc_vrd2_func_t erfc_vrd2;
    gcc_exp_vrd2_func_t exp_vrd2;
    gcc_exp10_vrd2_func_t exp10_vrd2;
    gcc_exp2_vrd2_func_t exp2_vrd2;
    gcc_fabs_vrd2_func_t fabs_vrd2;
    gcc_linearfrac_vrd2_func_t linearfrac_vrd2;
    gcc_log_vrd2_func_t log_vrd2;
    gcc_log10_vrd2_func_t log10_vrd2;
    gcc_log1p_vrd2_func_t log1p_vrd2;
    gcc_log2_vrd2_func_t log2_vrd2;
    gcc_pow_vrd2_func_t pow_vrd2;
    gcc_powx_vrd2_func_t powx_vrd2;
    gcc_sin_vrd2_func_t sin_vrd2;
    gcc_sincos_vrd2_func_t sincos_vrd2;
    gcc_sqrt_vrd2_func_t sqrt_vrd2;
    gcc_tan_vrd2_func_t tan_vrd2;

    // ============================================================================
    // SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
    // ============================================================================
    gcc_acos_vrs4_func_t acos_vrs4;
    gcc_asin_vrs4_func_t asin_vrs4;
    gcc_atan_vrs4_func_t atan_vrs4;
    gcc_cbrt_vrs4_func_t cbrt_vrs4;
    gcc_cos_vrs4_func_t cos_vrs4;
    gcc_cosh_vrs4_func_t cosh_vrs4;
    gcc_erf_vrs4_func_t erf_vrs4;
    gcc_erfc_vrs4_func_t erfc_vrs4;
    gcc_exp_vrs4_func_t exp_vrs4;
    gcc_exp10_vrs4_func_t exp10_vrs4;
    gcc_exp2_vrs4_func_t exp2_vrs4;
    gcc_expm1_vrs4_func_t expm1_vrs4;
    gcc_fabs_vrs4_func_t fabs_vrs4;
    gcc_linearfrac_vrs4_func_t linearfrac_vrs4;
    gcc_log_vrs4_func_t log_vrs4;
    gcc_log10_vrs4_func_t log10_vrs4;
    gcc_log1p_vrs4_func_t log1p_vrs4;
    gcc_log2_vrs4_func_t log2_vrs4;
    gcc_pow_vrs4_func_t pow_vrs4;
    gcc_powx_vrs4_func_t powx_vrs4;
    gcc_sin_vrs4_func_t sin_vrs4;
    gcc_sincos_vrs4_func_t sincos_vrs4;
    gcc_sqrt_vrs4_func_t sqrt_vrs4;
    gcc_tan_vrs4_func_t tan_vrs4;
    gcc_tanh_vrs4_func_t tanh_vrs4;

    // ============================================================================
    // DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
    // ============================================================================
    gcc_acos_vrd4_func_t acos_vrd4;
    gcc_asin_vrd4_func_t asin_vrd4;
    gcc_atan_vrd4_func_t atan_vrd4;
    gcc_cos_vrd4_func_t cos_vrd4;
    gcc_erf_vrd4_func_t erf_vrd4;
    gcc_erfc_vrd4_func_t erfc_vrd4;
    gcc_exp_vrd4_func_t exp_vrd4;
    gcc_exp2_vrd4_func_t exp2_vrd4;
    gcc_fabs_vrd4_func_t fabs_vrd4;
    gcc_linearfrac_vrd4_func_t linearfrac_vrd4;
    gcc_log_vrd4_func_t log_vrd4;
    gcc_log2_vrd4_func_t log2_vrd4;
    gcc_pow_vrd4_func_t pow_vrd4;
    gcc_powx_vrd4_func_t powx_vrd4;
    gcc_sin_vrd4_func_t sin_vrd4;
    gcc_sincos_vrd4_func_t sincos_vrd4;
    gcc_sqrt_vrd4_func_t sqrt_vrd4;
    gcc_tan_vrd4_func_t tan_vrd4;

    // ============================================================================
    // SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
    // ============================================================================
    gcc_acos_vrs8_func_t acos_vrs8;
    gcc_asin_vrs8_func_t asin_vrs8;
    gcc_atan_vrs8_func_t atan_vrs8;
    gcc_cos_vrs8_func_t cos_vrs8;
    gcc_cosh_vrs8_func_t cosh_vrs8;
    gcc_erf_vrs8_func_t erf_vrs8;
    gcc_erfc_vrs8_func_t erfc_vrs8;
    gcc_exp_vrs8_func_t exp_vrs8;
    gcc_exp2_vrs8_func_t exp2_vrs8;
    gcc_fabs_vrs8_func_t fabs_vrs8;
    gcc_linearfrac_vrs8_func_t linearfrac_vrs8;
    gcc_log_vrs8_func_t log_vrs8;
    gcc_log10_vrs8_func_t log10_vrs8;
    gcc_log2_vrs8_func_t log2_vrs8;
    gcc_pow_vrs8_func_t pow_vrs8;
    gcc_powx_vrs8_func_t powx_vrs8;
    gcc_sin_vrs8_func_t sin_vrs8;
    gcc_sincos_vrs8_func_t sincos_vrs8;
    gcc_sqrt_vrs8_func_t sqrt_vrs8;
    gcc_tan_vrs8_func_t tan_vrs8;
    gcc_tanh_vrs8_func_t tanh_vrs8;

    // ============================================================================
    // SINGLE PRECISION ARRAY (vrsa) VARIANTS
    // ============================================================================
    gcc_acos_vrsa_func_t acos_vrsa;
    gcc_add_vrsa_func_t add_vrsa;
    gcc_addfi_vrsa_func_t addfi_vrsa;
    gcc_asin_vrsa_func_t asin_vrsa;
    gcc_atan_vrsa_func_t atan_vrsa;
    gcc_cbrt_vrsa_func_t cbrt_vrsa;
    gcc_cos_vrsa_func_t cos_vrsa;
    gcc_cosh_vrsa_func_t cosh_vrsa;
    gcc_div_vrsa_func_t div_vrsa;
    gcc_divfi_vrsa_func_t divfi_vrsa;
    gcc_erf_vrsa_func_t erf_vrsa;
    gcc_erfc_vrsa_func_t erfc_vrsa;
    gcc_exp_vrsa_func_t exp_vrsa;
    gcc_exp10_vrsa_func_t exp10_vrsa;
    gcc_exp2_vrsa_func_t exp2_vrsa;
    gcc_expm1_vrsa_func_t expm1_vrsa;
    gcc_fabs_vrsa_func_t fabs_vrsa;
    gcc_fmax_vrsa_func_t fmax_vrsa;
    gcc_fmaxfi_vrsa_func_t fmaxfi_vrsa;
    gcc_fmin_vrsa_func_t fmin_vrsa;
    gcc_fminfi_vrsa_func_t fminfi_vrsa;
    gcc_linearfrac_vrsa_func_t linearfrac_vrsa;
    gcc_log_vrsa_func_t log_vrsa;
    gcc_log10_vrsa_func_t log10_vrsa;
    gcc_log1p_vrsa_func_t log1p_vrsa;
    gcc_log2_vrsa_func_t log2_vrsa;
    gcc_mul_vrsa_func_t mul_vrsa;
    gcc_mulfi_vrsa_func_t mulfi_vrsa;
    gcc_pow_vrsa_func_t pow_vrsa;
    gcc_powx_vrsa_func_t powx_vrsa;
    gcc_sin_vrsa_func_t sin_vrsa;
    gcc_sincos_vrsa_func_t sincos_vrsa;
    gcc_sqrt_vrsa_func_t sqrt_vrsa;
    gcc_sub_vrsa_func_t sub_vrsa;
    gcc_subfi_vrsa_func_t subfi_vrsa;
    gcc_tan_vrsa_func_t tan_vrsa;
    gcc_tanh_vrsa_func_t tanh_vrsa;

    // ============================================================================
    // DOUBLE PRECISION ARRAY (vrda) VARIANTS
    // ============================================================================
    gcc_acos_vrda_func_t acos_vrda;
    gcc_add_vrda_func_t add_vrda;
    gcc_addi_vrda_func_t addi_vrda;
    gcc_asin_vrda_func_t asin_vrda;
    gcc_atan_vrda_func_t atan_vrda;
    gcc_cbrt_vrda_func_t cbrt_vrda;
    gcc_cos_vrda_func_t cos_vrda;
    gcc_cosh_vrda_func_t cosh_vrda;
    gcc_div_vrda_func_t div_vrda;
    gcc_divi_vrda_func_t divi_vrda;
    gcc_erf_vrda_func_t erf_vrda;
    gcc_erfc_vrda_func_t erfc_vrda;
    gcc_exp_vrda_func_t exp_vrda;
    gcc_exp10_vrda_func_t exp10_vrda;
    gcc_exp2_vrda_func_t exp2_vrda;
    gcc_expm1_vrda_func_t expm1_vrda;
    gcc_fabs_vrda_func_t fabs_vrda;
    gcc_fmax_vrda_func_t fmax_vrda;
    gcc_fmaxi_vrda_func_t fmaxi_vrda;
    gcc_fmin_vrda_func_t fmin_vrda;
    gcc_fmini_vrda_func_t fmini_vrda;
    gcc_linearfrac_vrda_func_t linearfrac_vrda;
    gcc_log_vrda_func_t log_vrda;
    gcc_log10_vrda_func_t log10_vrda;
    gcc_log1p_vrda_func_t log1p_vrda;
    gcc_log2_vrda_func_t log2_vrda;
    gcc_mul_vrda_func_t mul_vrda;
    gcc_muli_vrda_func_t muli_vrda;
    gcc_pow_vrda_func_t pow_vrda;
    gcc_powx_vrda_func_t powx_vrda;
    gcc_sin_vrda_func_t sin_vrda;
    gcc_sincos_vrda_func_t sincos_vrda;
    gcc_sqrt_vrda_func_t sqrt_vrda;
    gcc_sub_vrda_func_t sub_vrda;
    gcc_subi_vrda_func_t subi_vrda;
    gcc_tan_vrda_func_t tan_vrda;

#ifdef __AVX512F__
    // ============================================================================
    // DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
    // ============================================================================
    gcc_asin_vrd8_func_t asin_vrd8;
    gcc_atan_vrd8_func_t atan_vrd8;
    gcc_cos_vrd8_func_t cos_vrd8;
    gcc_erf_vrd8_func_t erf_vrd8;
    gcc_erfc_vrd8_func_t erfc_vrd8;
    gcc_exp_vrd8_func_t exp_vrd8;
    gcc_exp2_vrd8_func_t exp2_vrd8;
    gcc_linearfrac_vrd8_func_t linearfrac_vrd8;
    gcc_log_vrd8_func_t log_vrd8;
    gcc_log2_vrd8_func_t log2_vrd8;
    gcc_pow_vrd8_func_t pow_vrd8;
    gcc_powx_vrd8_func_t powx_vrd8;
    gcc_sin_vrd8_func_t sin_vrd8;
    gcc_sincos_vrd8_func_t sincos_vrd8;
    gcc_sqrt_vrd8_func_t sqrt_vrd8;
    gcc_tan_vrd8_func_t tan_vrd8;

    // ============================================================================
    // SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
    // ============================================================================
    gcc_acos_vrs16_func_t acos_vrs16;
    gcc_asin_vrs16_func_t asin_vrs16;
    gcc_atan_vrs16_func_t atan_vrs16;
    gcc_cos_vrs16_func_t cos_vrs16;
    gcc_erf_vrs16_func_t erf_vrs16;
    gcc_erfc_vrs16_func_t erfc_vrs16;
    gcc_exp_vrs16_func_t exp_vrs16;
    gcc_exp2_vrs16_func_t exp2_vrs16;
    gcc_linearfrac_vrs16_func_t linearfrac_vrs16;
    gcc_log_vrs16_func_t log_vrs16;
    gcc_log10_vrs16_func_t log10_vrs16;
    gcc_log2_vrs16_func_t log2_vrs16;
    gcc_pow_vrs16_func_t pow_vrs16;
    gcc_powx_vrs16_func_t powx_vrs16;
    gcc_sin_vrs16_func_t sin_vrs16;
    gcc_sincos_vrs16_func_t sincos_vrs16;
    gcc_sqrt_vrs16_func_t sqrt_vrs16;
    gcc_tan_vrs16_func_t tan_vrs16;
    gcc_tanh_vrs16_func_t tanh_vrs16;
#endif

} gcc_funcs = {};

// ============================================================================
// GCC VECTORIZED LIBM FUNCTION DECLARATIONS WITH VERSION-AWARE CONDITIONAL COMPILATION
// ============================================================================
extern "C" {
    // ========================================================================
    // SCALAR FUNCTIONS - GLIBC 2.2.5 BASELINE (always available)
    // ========================================================================
    // Trigonometric functions
    float sinf(float); double sin(double);
    float cosf(float); double cos(double);
    float tanf(float); double tan(double);
    void sincosf(float, float*, float*); void sincos(double, double*, double*);
    float asinf(float); double asin(double);
    float acosf(float); double acos(double);
    float atanf(float); double atan(double);
    float atan2f(float, float); double atan2(double, double);

    // Hyperbolic functions
    float sinhf(float); double sinh(double);
    float coshf(float); double cosh(double);
    float tanhf(float); double tanh(double);
    float asinhf(float); double asinh(double);
    float acoshf(float); double acosh(double);
    float atanhf(float); double atanh(double);

    // Exponential and logarithmic functions
    float expf(float); double exp(double);
    float exp2f(float); double exp2(double);
    float exp10f(float); double exp10(double);
    float expm1f(float); double expm1(double);
    float logf(float); double log(double);
    float log2f(float); double log2(double);
    float log10f(float); double log10(double);
    float log1pf(float); double log1p(double);

    // Power functions
    float powf(float, float); double pow(double, double);
    float sqrtf(float); double sqrt(double);
    float cbrtf(float); double cbrt(double);

    // Error functions
    float erff(float); double erf(double);
    float erfcf(float); double erfc(double);

    // Other baseline functions
    float fabsf(float); double fabs(double);

    // ========================================================================
    // SCALAR FUNCTIONS - GLIBC 2.41+ (pi-related trigonometric)
    // ========================================================================
#if GLIBC_VERSION >= 241
    float sinpif(float); double sinpi(double);
    float cospif(float); double cospi(double);
    float tanpif(float); double tanpi(double);
#endif

    // ========================================================================
    // VECTORIZED FUNCTIONS - CONDITIONAL BASED ON GCC/GLIBC VERSION
    // ========================================================================

    // ========================================================================
    // GLIBC 2.22 BASELINE VECTOR FUNCTIONS (cos, exp, log, sin, pow, sincos)
    // ========================================================================
#if GLIBC_VERSION >= 222

    // Vectorized SIN functions
    __m128 _ZGVbN4v_sinf(__m128);
    __m128d _ZGVbN2v_sin(__m128d);
    __m256 _ZGVdN8v_sinf(__m256);
    __m256d _ZGVdN4v_sin(__m256d);

    // Vectorized COS functions
    __m128 _ZGVbN4v_cosf(__m128);
    __m128d _ZGVbN2v_cos(__m128d);
    __m256 _ZGVdN8v_cosf(__m256);
    __m256d _ZGVdN4v_cos(__m256d);

    // Vectorized SINCOS functions
    void _ZGVbN4vvv_sincosf(__m128, __m128*, __m128*);
    void _ZGVbN2vvv_sincos(__m128d, __m128d*, __m128d*);
    void _ZGVdN8vvv_sincosf(__m256, __m256*, __m256*);
    void _ZGVdN4vvv_sincos(__m256d, __m256d*, __m256d*);

    // Vectorized EXP functions
    __m128 _ZGVbN4v_expf(__m128);
    __m128d _ZGVbN2v_exp(__m128d);
    __m256 _ZGVdN8v_expf(__m256);
    __m256d _ZGVdN4v_exp(__m256d);

    // Vectorized LOG functions
    __m128 _ZGVbN4v_logf(__m128);
    __m128d _ZGVbN2v_log(__m128d);
    __m256 _ZGVdN8v_logf(__m256);
    __m256d _ZGVdN4v_log(__m256d);

    // Vectorized POW functions
    __m128 _ZGVbN4vv_powf(__m128, __m128);
    __m128d _ZGVbN2vv_pow(__m128d, __m128d);
    __m256 _ZGVdN8vv_powf(__m256, __m256);
    __m256d _ZGVdN4vv_pow(__m256d, __m256d);

#ifdef __AVX512F__
    // Vectorized SIN functions
    __m512 _ZGVeN16v_sinf(__m512);
    __m512d _ZGVeN8v_sin(__m512d);

    // Vectorized COS functions
    __m512 _ZGVeN16v_cosf(__m512);
    __m512d _ZGVeN8v_cos(__m512d);

    // Vectorized SINCOS functions
    void _ZGVeN16vvv_sincosf(__m512, __m512*, __m512*);
    void _ZGVeN8vvv_sincos(__m512d, __m512d*, __m512d*);

    // Vectorized EXP functions
    __m512 _ZGVeN16v_expf(__m512);
    __m512d _ZGVeN8v_exp(__m512d);

    // Vectorized LOG functions
    __m512 _ZGVeN16v_logf(__m512);
    __m512d _ZGVeN8v_log(__m512d);

    // Vectorized POW functions
    __m512 _ZGVeN16vv_powf(__m512, __m512);
    __m512d _ZGVeN8vv_pow(__m512d, __m512d);
#endif

#endif // GLIBC_VERSION >= 222

    // ========================================================================
    // GLIBC 2.35+ EXTENDED VECTOR FUNCTIONS (21 additional functions)
    // ========================================================================
#if GLIBC_VERSION >= 235

    // Vectorized TAN functions
    __m128 _ZGVbN4v_tanf(__m128);
    __m128d _ZGVbN2v_tan(__m128d);
    __m256 _ZGVdN8v_tanf(__m256);
    __m256d _ZGVdN4v_tan(__m256d);

    // Vectorized ASIN functions
    __m128 _ZGVbN4v_asinf(__m128);
    __m128d _ZGVbN2v_asin(__m128d);
    __m256 _ZGVdN8v_asinf(__m256);
    __m256d _ZGVdN4v_asin(__m256d);

    // Vectorized ACOS functions
    __m128 _ZGVbN4v_acosf(__m128);
    __m128d _ZGVbN2v_acos(__m128d);
    __m256 _ZGVdN8v_acosf(__m256);
    __m256d _ZGVdN4v_acos(__m256d);

    // Vectorized ATAN functions
    __m128 _ZGVbN4v_atanf(__m128);
    __m128d _ZGVbN2v_atan(__m128d);
    __m256 _ZGVcN8v_atanf(__m256);
    __m256d _ZGVcN4v_atan(__m256d);
    __m256 _ZGVdN8v_atanf(__m256);
    __m256d _ZGVdN4v_atan(__m256d);

    // Vectorized SINH functions
    __m128 _ZGVbN4v_sinhf(__m128);
    __m128d _ZGVbN2v_sinh(__m128d);
    __m256 _ZGVdN8v_sinhf(__m256);
    __m256d _ZGVdN4v_sinh(__m256d);

    // Vectorized COSH functions
    __m128 _ZGVbN4v_coshf(__m128);
    __m128d _ZGVbN2v_cosh(__m128d);
    __m256 _ZGVdN8v_coshf(__m256);
    __m256d _ZGVdN4v_cosh(__m256d);

    // Vectorized TANH functions
    __m128 _ZGVbN4v_tanhf(__m128);
    __m128d _ZGVbN2v_tanh(__m128d);
    __m256 _ZGVdN8v_tanhf(__m256);
    __m256d _ZGVdN4v_tanh(__m256d);

    // Vectorized ASINH functions
    __m128 _ZGVbN4v_asinhf(__m128);
    __m128d _ZGVbN2v_asinh(__m128d);
    __m256 _ZGVcN8v_asinhf(__m256);
    __m256d _ZGVcN4v_asinh(__m256d);
    __m256 _ZGVdN8v_asinhf(__m256);
    __m256d _ZGVdN4v_asinh(__m256d);

    // Vectorized ACOSH functions
    __m128 _ZGVbN4v_acoshf(__m128);
    __m128d _ZGVbN2v_acosh(__m128d);
    __m256 _ZGVcN8v_acoshf(__m256);
    __m256d _ZGVcN4v_acosh(__m256d);
    __m256 _ZGVdN8v_acoshf(__m256);
    __m256d _ZGVdN4v_acosh(__m256d);

    // Vectorized ATANH functions
    __m128 _ZGVbN4v_atanhf(__m128);
    __m128d _ZGVbN2v_atanh(__m128d);
    __m256 _ZGVcN8v_atanhf(__m256);
    __m256d _ZGVcN4v_atanh(__m256d);
    __m256 _ZGVdN8v_atanhf(__m256);
    __m256d _ZGVdN4v_atanh(__m256d);

    // Vectorized EXP2 functions
    __m128 _ZGVbN4v_exp2f(__m128);
    __m128d _ZGVbN2v_exp2(__m128d);
    __m256 _ZGVdN8v_exp2f(__m256);
    __m256d _ZGVdN4v_exp2(__m256d);

    // Vectorized EXP10 functions
    __m128 _ZGVbN4v_exp10f(__m128);
    __m128d _ZGVbN2v_exp10(__m128d);
    __m256 _ZGVdN8v_exp10f(__m256);
    __m256d _ZGVdN4v_exp10(__m256d);

    // Vectorized EXPM1 functions
    __m128 _ZGVbN4v_expm1f(__m128);
    __m128d _ZGVbN2v_expm1(__m128d);
    __m256 _ZGVdN8v_expm1f(__m256);
    __m256d _ZGVdN4v_expm1(__m256d);

    // Vectorized LOG2 functions
    __m128 _ZGVbN4v_log2f(__m128);
    __m128d _ZGVbN2v_log2(__m128d);
    __m256 _ZGVdN8v_log2f(__m256);
    __m256d _ZGVdN4v_log2(__m256d);

    // Vectorized LOG10 functions
    __m128 _ZGVbN4v_log10f(__m128);
    __m128d _ZGVbN2v_log10(__m128d);
    __m256 _ZGVdN8v_log10f(__m256);
    __m256d _ZGVdN4v_log10(__m256d);

    // Vectorized LOG1P functions
    __m128 _ZGVbN4v_log1pf(__m128);
    __m128d _ZGVbN2v_log1p(__m128d);
    __m256 _ZGVdN8v_log1pf(__m256);
    __m256d _ZGVdN4v_log1p(__m256d);

    // Vectorized CBRT functions
    __m128 _ZGVbN4v_cbrtf(__m128);
    __m128d _ZGVbN2v_cbrt(__m128d);
    __m256 _ZGVdN8v_cbrtf(__m256);
    __m256d _ZGVdN4v_cbrt(__m256d);

    // ======================== ERROR FUNCTIONS ===========================
    // Vectorized ERF functions
    __m128 _ZGVbN4v_erff(__m128);
    __m128d _ZGVbN2v_erf(__m128d);
    __m256 _ZGVdN8v_erff(__m256);
    __m256d _ZGVdN4v_erf(__m256d);

    // Vectorized ERFC functions
    __m128 _ZGVbN4v_erfcf(__m128);
    __m128d _ZGVbN2v_erfc(__m128d);
    __m256 _ZGVdN8v_erfcf(__m256);
    __m256d _ZGVdN4v_erfc(__m256d);

    // Vectorized ATAN2 functions
    __m128 _ZGVbN4vv_atan2f(__m128, __m128);
    __m128d _ZGVbN2vv_atan2(__m128d, __m128d);
    __m256 _ZGVdN8vv_atan2f(__m256, __m256);
    __m256d _ZGVdN4vv_atan2(__m256d, __m256d);

    // Vectorized HYPOT functions
    __m128 _ZGVbN4vv_hypotf(__m128, __m128);
    __m128d _ZGVbN2vv_hypot(__m128d, __m128d);
    __m256 _ZGVdN8vv_hypotf(__m256, __m256);
    __m256d _ZGVdN4vv_hypot(__m256d, __m256d);

#ifdef __AVX512F__
    // Vectorized TAN functions
    __m512 _ZGVeN16v_tanf(__m512);
    __m512d _ZGVeN8v_tan(__m512d);

    // Vectorized ASIN functions
    __m512 _ZGVeN16v_asinf(__m512);
    __m512d _ZGVeN8v_asin(__m512d);

    // Vectorized ACOS functions
    __m512 _ZGVeN16v_acosf(__m512);
    __m512d _ZGVeN8v_acos(__m512d);

    // Vectorized ATAN functions
    __m512 _ZGVeN16v_atanf(__m512);
    __m512d _ZGVeN8v_atan(__m512d);

    // Vectorized SINH functions
    __m512 _ZGVeN16v_sinhf(__m512);
    __m512d _ZGVeN8v_sinh(__m512d);

    // Vectorized COSH functions
    __m512 _ZGVeN16v_coshf(__m512);
    __m512d _ZGVeN8v_cosh(__m512d);

    // Vectorized TANH functions
    __m512 _ZGVeN16v_tanhf(__m512);
    __m512d _ZGVeN8v_tanh(__m512d);

    // Vectorized ASINH functions
    __m512 _ZGVeN16v_asinhf(__m512);
    __m512d _ZGVeN8v_asinh(__m512d);

    // Vectorized ACOSH functions
    __m512 _ZGVeN16v_acoshf(__m512);
    __m512d _ZGVeN8v_acosh(__m512d);

    // Vectorized ATANH functions
    __m512 _ZGVeN16v_atanhf(__m512);
    __m512d _ZGVeN8v_atanh(__m512d);

    // Vectorized EXP2 functions
    __m512 _ZGVeN16v_exp2f(__m512);
    __m512d _ZGVeN8v_exp2(__m512d);

    // Vectorized EXP10 functions
    __m512 _ZGVeN16v_exp10f(__m512);
    __m512d _ZGVeN8v_exp10(__m512d);

    // Vectorized EXPM1 functions
    __m512 _ZGVeN16v_expm1f(__m512);
    __m512d _ZGVeN8v_expm1(__m512d);

    // Vectorized LOG2 functions
    __m512 _ZGVeN16v_log2f(__m512);
    __m512d _ZGVeN8v_log2(__m512d);

    // Vectorized LOG10 functions
    __m512 _ZGVeN16v_log10f(__m512);
    __m512d _ZGVeN8v_log10(__m512d);

    // Vectorized LOG1P functions
    __m512 _ZGVeN16v_log1pf(__m512);
    __m512d _ZGVeN8v_log1p(__m512d);

    // Vectorized CBRT functions
    __m512 _ZGVeN16v_cbrtf(__m512);
    __m512d _ZGVeN8v_cbrt(__m512d);

    // Vectorized ERF functions
    __m512 _ZGVeN16v_erff(__m512);
    __m512d _ZGVeN8v_erf(__m512d);

    // Vectorized ERFC functions
    __m512 _ZGVeN16v_erfcf(__m512);
    __m512d _ZGVeN8v_erfc(__m512d);

    // Vectorized ATAN2 functions
    __m512 _ZGVeN16vv_atan2f(__m512, __m512);
    __m512d _ZGVeN8vv_atan2(__m512d, __m512d);

    // Vectorized HYPOT functions
    __m512 _ZGVeN16vv_hypotf(__m512, __m512);
    __m512d _ZGVeN8vv_hypot(__m512d, __m512d);
#endif

#endif // GLIBC_VERSION >= 235

}

// ============================================================================
// SYMBOL INITIALIZATION FUNCTION ORGANIZED BY AVAILABILITY MACROS
// ============================================================================
static void init_gcc_symbols(void) {
    printf("GCC Shim: Initializing symbols from %s\n", GCC_VERSION_INFO_STRING);
    printf("GCC_HAS_VECTOR_SINCOS : %d\n", GCC_HAS_VECTOR_SINCOS);
    // ============================================================================
    // SINGLE PRECISION SCALAR (ss) VARIANTS - GLIBC 2.2.5 BASELINE
    // ============================================================================
    gcc_funcs.acosf = acosf;
    gcc_funcs.acoshf = acoshf;
    gcc_funcs.asinf = asinf;
    gcc_funcs.asinhf = asinhf;
    gcc_funcs.atanf = atanf;
    gcc_funcs.atan2f = atan2f;
    gcc_funcs.atanhf = atanhf;
    gcc_funcs.cbrtf = cbrtf;
    gcc_funcs.ceilf = ceilf;
    gcc_funcs.copysignf = copysignf;
    gcc_funcs.cosf = cosf;
    gcc_funcs.coshf = coshf;
    gcc_funcs.erff = erff;
    gcc_funcs.erfcf = erfcf;
    gcc_funcs.expf = expf;
    gcc_funcs.exp10f = exp10f;
    gcc_funcs.exp2f = exp2f;
    gcc_funcs.expm1f = expm1f;
    gcc_funcs.fabsf = fabsf;
    gcc_funcs.fdimf = fdimf;
    gcc_funcs.finitef = finitef;
    gcc_funcs.floorf = floorf;
    gcc_funcs.fmaf = fmaf;
    gcc_funcs.fmaxf = fmaxf;
    gcc_funcs.fminf = fminf;
    gcc_funcs.fmodf = fmodf;
    gcc_funcs.frexpf = frexpf;
    gcc_funcs.hypotf = hypotf;
    gcc_funcs.ilogbf = ilogbf;
    gcc_funcs.ldexpf = ldexpf;
    gcc_funcs.llrintf = llrintf;
    gcc_funcs.llroundf = llroundf;
    gcc_funcs.logf = logf;
    gcc_funcs.log10f = log10f;
    gcc_funcs.log1pf = log1pf;
    gcc_funcs.log2f = log2f;
    gcc_funcs.logbf = logbf;
    gcc_funcs.lrintf = lrintf;
    gcc_funcs.lroundf = lroundf;
    gcc_funcs.modff = modff;
    gcc_funcs.nearbyintf = nearbyintf;
    gcc_funcs.nextafterf = nextafterf;
    gcc_funcs.nexttowardf = nexttowardf;
    gcc_funcs.powf = powf;
    gcc_funcs.remainderf = remainderf;
    gcc_funcs.remquof = remquof;
    gcc_funcs.rintf = rintf;
    gcc_funcs.roundf = roundf;
    gcc_funcs.scalblnf = scalblnf;
    gcc_funcs.scalbnf = scalbnf;
    gcc_funcs.sinf = sinf;
    gcc_funcs.sincosf = sincosf;
    gcc_funcs.sinhf = sinhf;
    gcc_funcs.sqrtf = sqrtf;
    gcc_funcs.tanf = tanf;
    gcc_funcs.tanhf = tanhf;
    gcc_funcs.truncf = truncf;
    gcc_funcs.sinpif = nullptr;
    gcc_funcs.cospif = nullptr;
    gcc_funcs.tanpif = nullptr;
    // GLIBC 2.41+ pi-related functions
#if GLIBC_VERSION >= 241
    gcc_funcs.sinpif = sinpif;
    gcc_funcs.cospif = cospif;
    gcc_funcs.tanpif = tanpif;
#endif

    // ============================================================================
    // DOUBLE PRECISION SCALAR (sd) VARIANTS - GLIBC 2.2.5 BASELINE
    // ============================================================================
    gcc_funcs.acos = acos;
    gcc_funcs.acosh = acosh;
    gcc_funcs.asin = asin;
    gcc_funcs.asinh = asinh;
    gcc_funcs.atan = atan;
    gcc_funcs.atan2 = atan2;
    gcc_funcs.atanh = atanh;
    gcc_funcs.cbrt = cbrt;
    gcc_funcs.ceil = ceil;
    gcc_funcs.copysign = copysign;
    gcc_funcs.cos = cos;
    gcc_funcs.cosh = cosh;
    gcc_funcs.erf = erf;
    gcc_funcs.erfc = erfc;
    gcc_funcs.exp = exp;
    gcc_funcs.exp10 = exp10;
    gcc_funcs.exp2 = exp2;
    gcc_funcs.expm1 = expm1;
    gcc_funcs.fabs = fabs;
    gcc_funcs.fdim = fdim;
    gcc_funcs.finite = finite;
    gcc_funcs.floor = floor;
    gcc_funcs.fma = fma;
    gcc_funcs.fmax = fmax;
    gcc_funcs.fmin = fmin;
    gcc_funcs.fmod = fmod;
    gcc_funcs.frexp = frexp;
    gcc_funcs.hypot = hypot;
    gcc_funcs.ilogb = ilogb;
    gcc_funcs.ldexp = ldexp;
    gcc_funcs.llrint = llrint;
    gcc_funcs.llround = llround;
    gcc_funcs.log = log;
    gcc_funcs.log10 = log10;
    gcc_funcs.log1p = log1p;
    gcc_funcs.log2 = log2;
    gcc_funcs.logb = logb;
    gcc_funcs.lrint = lrint;
    gcc_funcs.lround = lround;
    gcc_funcs.modf = modf;
    gcc_funcs.nearbyint = nearbyint;
    gcc_funcs.nextafter = nextafter;
    gcc_funcs.nexttoward = nexttoward;
    gcc_funcs.pow = pow;
    gcc_funcs.remainder = remainder;
    gcc_funcs.remquo = remquo;
    gcc_funcs.rint = rint;
    gcc_funcs.round = round;
    gcc_funcs.scalbln = scalbln;
    gcc_funcs.scalbn = scalbn;
    gcc_funcs.sin = sin;
    gcc_funcs.sincos = sincos;
    gcc_funcs.sinh = sinh;
    gcc_funcs.sqrt = sqrt;
    gcc_funcs.tan = tan;
    gcc_funcs.tanh = tanh;
    gcc_funcs.trunc = trunc;
    gcc_funcs.sinpi = nullptr;
    gcc_funcs.cospi = nullptr;
    gcc_funcs.tanpi = nullptr;
    // GLIBC 2.41+ pi-related functions
#if GLIBC_VERSION >= 241
    gcc_funcs.sinpi = sinpi;
    gcc_funcs.cospi = cospi;
    gcc_funcs.tanpi = tanpi;
#endif

    // ============================================================================
    // VECTOR FUNCTION INITIALIZATION - ORGANIZED BY GLIBC VERSION
    // ============================================================================

    // Double precision 128-bit (vrd2)
    gcc_funcs.acos_vrd2 = nullptr;
    gcc_funcs.asin_vrd2 = nullptr;
    gcc_funcs.atan_vrd2 = nullptr;
    gcc_funcs.cbrt_vrd2 = nullptr;
    gcc_funcs.cos_vrd2 = nullptr;
    gcc_funcs.cosh_vrd2 = nullptr;
    gcc_funcs.erf_vrd2 = nullptr;
    gcc_funcs.erfc_vrd2 = nullptr;
    gcc_funcs.exp_vrd2 = nullptr;
    gcc_funcs.exp10_vrd2 = nullptr;
    gcc_funcs.exp2_vrd2 = nullptr;
    gcc_funcs.fabs_vrd2 = nullptr;
    gcc_funcs.linearfrac_vrd2 = nullptr;
    gcc_funcs.log_vrd2 = nullptr;
    gcc_funcs.log10_vrd2 = nullptr;
    gcc_funcs.log1p_vrd2 = nullptr;
    gcc_funcs.log2_vrd2 = nullptr;
    gcc_funcs.pow_vrd2 = nullptr;
    gcc_funcs.powx_vrd2 = nullptr;
    gcc_funcs.sin_vrd2 = nullptr;
    gcc_funcs.sincos_vrd2 = nullptr;
    gcc_funcs.sqrt_vrd2 = nullptr;
    gcc_funcs.tan_vrd2 = nullptr;

    // Single precision 128-bit (vrs4)
    gcc_funcs.acos_vrs4 = nullptr;
    gcc_funcs.asin_vrs4 = nullptr;
    gcc_funcs.atan_vrs4 = nullptr;
    gcc_funcs.cbrt_vrs4 = nullptr;
    gcc_funcs.cos_vrs4 = nullptr;
    gcc_funcs.cosh_vrs4 = nullptr;
    gcc_funcs.erf_vrs4 = nullptr;
    gcc_funcs.erfc_vrs4 = nullptr;
    gcc_funcs.exp_vrs4 = nullptr;
    gcc_funcs.exp10_vrs4 = nullptr;
    gcc_funcs.exp2_vrs4 = nullptr;
    gcc_funcs.expm1_vrs4 = nullptr;
    gcc_funcs.fabs_vrs4 = nullptr;
    gcc_funcs.linearfrac_vrs4 = nullptr;
    gcc_funcs.log_vrs4 = nullptr;
    gcc_funcs.log10_vrs4 = nullptr;
    gcc_funcs.log1p_vrs4 = nullptr;
    gcc_funcs.log2_vrs4 = nullptr;
    gcc_funcs.pow_vrs4 = nullptr;
    gcc_funcs.powx_vrs4 = nullptr;
    gcc_funcs.sin_vrs4 = nullptr;
    gcc_funcs.sincos_vrs4 = nullptr;
    gcc_funcs.sqrt_vrs4 = nullptr;
    gcc_funcs.tan_vrs4 = nullptr;
    gcc_funcs.tanh_vrs4 = nullptr;

    // Double precision 256-bit (vrd4)
    gcc_funcs.acos_vrd4 = nullptr;
    gcc_funcs.asin_vrd4 = nullptr;
    gcc_funcs.atan_vrd4 = nullptr;
    gcc_funcs.cos_vrd4 = nullptr;
    gcc_funcs.erf_vrd4 = nullptr;
    gcc_funcs.erfc_vrd4 = nullptr;
    gcc_funcs.exp_vrd4 = nullptr;
    gcc_funcs.exp2_vrd4 = nullptr;
    gcc_funcs.fabs_vrd4 = nullptr;
    gcc_funcs.linearfrac_vrd4 = nullptr;
    gcc_funcs.log_vrd4 = nullptr;
    gcc_funcs.log2_vrd4 = nullptr;
    gcc_funcs.pow_vrd4 = nullptr;
    gcc_funcs.powx_vrd4 = nullptr;
    gcc_funcs.sin_vrd4 = nullptr;
    gcc_funcs.sincos_vrd4 = nullptr;
    gcc_funcs.sqrt_vrd4 = nullptr;
    gcc_funcs.tan_vrd4 = nullptr;

    // Single precision 256-bit (vrs8)
    gcc_funcs.acos_vrs8 = nullptr;
    gcc_funcs.asin_vrs8 = nullptr;
    gcc_funcs.atan_vrs8 = nullptr;
    gcc_funcs.cos_vrs8 = nullptr;
    gcc_funcs.cosh_vrs8 = nullptr;
    gcc_funcs.erf_vrs8 = nullptr;
    gcc_funcs.erfc_vrs8 = nullptr;
    gcc_funcs.exp_vrs8 = nullptr;
    gcc_funcs.exp2_vrs8 = nullptr;
    gcc_funcs.fabs_vrs8 = nullptr;
    gcc_funcs.linearfrac_vrs8 = nullptr;
    gcc_funcs.log_vrs8 = nullptr;
    gcc_funcs.log10_vrs8 = nullptr;
    gcc_funcs.log2_vrs8 = nullptr;
    gcc_funcs.pow_vrs8 = nullptr;
    gcc_funcs.powx_vrs8 = nullptr;
    gcc_funcs.sin_vrs8 = nullptr;
    gcc_funcs.sincos_vrs8 = nullptr;
    gcc_funcs.sqrt_vrs8 = nullptr;
    gcc_funcs.tan_vrs8 = nullptr;
    gcc_funcs.tanh_vrs8 = nullptr;

    // Array-based variants (not supported by GCC - remain nullptr)
    gcc_funcs.acos_vrsa = nullptr;
    gcc_funcs.add_vrsa = nullptr;
    gcc_funcs.addfi_vrsa = nullptr;
    gcc_funcs.asin_vrsa = nullptr;
    gcc_funcs.atan_vrsa = nullptr;
    gcc_funcs.cbrt_vrsa = nullptr;
    gcc_funcs.cos_vrsa = nullptr;
    gcc_funcs.cosh_vrsa = nullptr;
    gcc_funcs.div_vrsa = nullptr;
    gcc_funcs.divfi_vrsa = nullptr;
    gcc_funcs.erf_vrsa = nullptr;
    gcc_funcs.erfc_vrsa = nullptr;
    gcc_funcs.exp_vrsa = nullptr;
    gcc_funcs.exp10_vrsa = nullptr;
    gcc_funcs.exp2_vrsa = nullptr;
    gcc_funcs.expm1_vrsa = nullptr;
    gcc_funcs.fabs_vrsa = nullptr;
    gcc_funcs.fmax_vrsa = nullptr;
    gcc_funcs.fmaxfi_vrsa = nullptr;
    gcc_funcs.fmin_vrsa = nullptr;
    gcc_funcs.fminfi_vrsa = nullptr;
    gcc_funcs.linearfrac_vrsa = nullptr;
    gcc_funcs.log_vrsa = nullptr;
    gcc_funcs.log10_vrsa = nullptr;
    gcc_funcs.log1p_vrsa = nullptr;
    gcc_funcs.log2_vrsa = nullptr;
    gcc_funcs.mul_vrsa = nullptr;
    gcc_funcs.mulfi_vrsa = nullptr;
    gcc_funcs.pow_vrsa = nullptr;
    gcc_funcs.powx_vrsa = nullptr;
    gcc_funcs.sin_vrsa = nullptr;
    gcc_funcs.sincos_vrsa = nullptr;
    gcc_funcs.sqrt_vrsa = nullptr;
    gcc_funcs.sub_vrsa = nullptr;
    gcc_funcs.subfi_vrsa = nullptr;
    gcc_funcs.tan_vrsa = nullptr;
    gcc_funcs.tanh_vrsa = nullptr;

    gcc_funcs.acos_vrda = nullptr;
    gcc_funcs.add_vrda = nullptr;
    gcc_funcs.addi_vrda = nullptr;
    gcc_funcs.asin_vrda = nullptr;
    gcc_funcs.atan_vrda = nullptr;
    gcc_funcs.cbrt_vrda = nullptr;
    gcc_funcs.cos_vrda = nullptr;
    gcc_funcs.cosh_vrda = nullptr;
    gcc_funcs.div_vrda = nullptr;
    gcc_funcs.divi_vrda = nullptr;
    gcc_funcs.erf_vrda = nullptr;
    gcc_funcs.erfc_vrda = nullptr;
    gcc_funcs.exp_vrda = nullptr;
    gcc_funcs.exp10_vrda = nullptr;
    gcc_funcs.exp2_vrda = nullptr;
    gcc_funcs.expm1_vrda = nullptr;
    gcc_funcs.fabs_vrda = nullptr;
    gcc_funcs.fmax_vrda = nullptr;
    gcc_funcs.fmaxi_vrda = nullptr;
    gcc_funcs.fmin_vrda = nullptr;
    gcc_funcs.fmini_vrda = nullptr;
    gcc_funcs.linearfrac_vrda = nullptr;
    gcc_funcs.log_vrda = nullptr;
    gcc_funcs.log10_vrda = nullptr;
    gcc_funcs.log1p_vrda = nullptr;
    gcc_funcs.log2_vrda = nullptr;
    gcc_funcs.mul_vrda = nullptr;
    gcc_funcs.muli_vrda = nullptr;
    gcc_funcs.pow_vrda = nullptr;
    gcc_funcs.powx_vrda = nullptr;
    gcc_funcs.sin_vrda = nullptr;
    gcc_funcs.sincos_vrda = nullptr;
    gcc_funcs.sqrt_vrda = nullptr;
    gcc_funcs.sub_vrda = nullptr;
    gcc_funcs.subi_vrda = nullptr;
    gcc_funcs.tan_vrda = nullptr;

#ifdef __AVX512F__
    // AVX512 variants (512-bit)
    gcc_funcs.asin_vrd8 = nullptr;
    gcc_funcs.atan_vrd8 = nullptr;
    gcc_funcs.cos_vrd8 = nullptr;
    gcc_funcs.erf_vrd8 = nullptr;
    gcc_funcs.erfc_vrd8 = nullptr;
    gcc_funcs.exp_vrd8 = nullptr;
    gcc_funcs.exp2_vrd8 = nullptr;
    gcc_funcs.linearfrac_vrd8 = nullptr;
    gcc_funcs.log_vrd8 = nullptr;
    gcc_funcs.log2_vrd8 = nullptr;
    gcc_funcs.pow_vrd8 = nullptr;
    gcc_funcs.powx_vrd8 = nullptr;
    gcc_funcs.sin_vrd8 = nullptr;
    gcc_funcs.sincos_vrd8 = nullptr;
    gcc_funcs.sqrt_vrd8 = nullptr;
    gcc_funcs.tan_vrd8 = nullptr;

    gcc_funcs.acos_vrs16 = nullptr;
    gcc_funcs.asin_vrs16 = nullptr;
    gcc_funcs.atan_vrs16 = nullptr;
    gcc_funcs.cos_vrs16 = nullptr;
    gcc_funcs.erf_vrs16 = nullptr;
    gcc_funcs.erfc_vrs16 = nullptr;
    gcc_funcs.exp_vrs16 = nullptr;
    gcc_funcs.exp2_vrs16 = nullptr;
    gcc_funcs.linearfrac_vrs16 = nullptr;
    gcc_funcs.log_vrs16 = nullptr;
    gcc_funcs.log10_vrs16 = nullptr;
    gcc_funcs.log2_vrs16 = nullptr;
    gcc_funcs.pow_vrs16 = nullptr;
    gcc_funcs.powx_vrs16 = nullptr;
    gcc_funcs.sin_vrs16 = nullptr;
    gcc_funcs.sincos_vrs16 = nullptr;
    gcc_funcs.sqrt_vrs16 = nullptr;
    gcc_funcs.tan_vrs16 = nullptr;
    gcc_funcs.tanh_vrs16 = nullptr;
#endif

    // ========================================================================
    // STEP 2: Initialize GLIBC 2.22 baseline vector functions
    // ========================================================================
#if GLIBC_VERSION >= 222
    // SIN, COS, SINCOS, EXP, LOG, POW - 128-bit (SSE)
    gcc_funcs.sin_vrd2 = _ZGVbN2v_sin;
    gcc_funcs.sin_vrs4 = _ZGVbN4v_sinf;
    gcc_funcs.cos_vrd2 = _ZGVbN2v_cos;
    gcc_funcs.cos_vrs4 = _ZGVbN4v_cosf;
    gcc_funcs.sincos_vrd2 = _ZGVbN2vvv_sincos;
    gcc_funcs.sincos_vrs4 = _ZGVbN4vvv_sincosf;
    gcc_funcs.exp_vrd2 = _ZGVbN2v_exp;
    gcc_funcs.exp_vrs4 = _ZGVbN4v_expf;
    gcc_funcs.log_vrd2 = _ZGVbN2v_log;
    gcc_funcs.log_vrs4 = _ZGVbN4v_logf;
    gcc_funcs.pow_vrd2 = _ZGVbN2vv_pow;
    gcc_funcs.pow_vrs4 = _ZGVbN4vv_powf;

    // SIN, COS, SINCOS, EXP, LOG, POW - 256-bit (AVX2)
    gcc_funcs.sin_vrd4 = _ZGVdN4v_sin;
    gcc_funcs.sin_vrs8 = _ZGVdN8v_sinf;
    gcc_funcs.cos_vrd4 = _ZGVdN4v_cos;
    gcc_funcs.cos_vrs8 = _ZGVdN8v_cosf;
    gcc_funcs.sincos_vrd4 = _ZGVdN4vvv_sincos;
    gcc_funcs.sincos_vrs8 = _ZGVdN8vvv_sincosf;
    gcc_funcs.exp_vrd4 = _ZGVdN4v_exp;
    gcc_funcs.exp_vrs8 = _ZGVdN8v_expf;
    gcc_funcs.log_vrd4 = _ZGVdN4v_log;
    gcc_funcs.log_vrs8 = _ZGVdN8v_logf;
    gcc_funcs.pow_vrd4 = _ZGVdN4vv_pow;
    gcc_funcs.pow_vrs8 = _ZGVdN8vv_powf;

#ifdef __AVX512F__
    // SIN, COS, SINCOS, EXP, LOG, POW - 512-bit (AVX512)
    gcc_funcs.sin_vrd8 = _ZGVeN8v_sin;
    gcc_funcs.sin_vrs16 = _ZGVeN16v_sinf;
    gcc_funcs.cos_vrd8 = _ZGVeN8v_cos;
    gcc_funcs.cos_vrs16 = _ZGVeN16v_cosf;
    gcc_funcs.sincos_vrd8 = _ZGVeN8vvv_sincos;
    gcc_funcs.sincos_vrs16 = _ZGVeN16vvv_sincosf;
    gcc_funcs.exp_vrd8 = _ZGVeN8v_exp;
    gcc_funcs.exp_vrs16 = _ZGVeN16v_expf;
    gcc_funcs.log_vrd8 = _ZGVeN8v_log;
    gcc_funcs.log_vrs16 = _ZGVeN16v_logf;
    gcc_funcs.pow_vrd8 = _ZGVeN8vv_pow;
    gcc_funcs.pow_vrs16 = _ZGVeN16vv_powf;
#endif
#endif // GLIBC_VERSION >= 222

#if GLIBC_VERSION >= 235
    // ACOS, ASIN, ATAN - 128-bit (SSE)
    gcc_funcs.acos_vrd2 = _ZGVbN2v_acos;
    gcc_funcs.acos_vrs4 = _ZGVbN4v_acosf;
    gcc_funcs.asin_vrd2 = _ZGVbN2v_asin;
    gcc_funcs.asin_vrs4 = _ZGVbN4v_asinf;
    gcc_funcs.atan_vrd2 = _ZGVbN2v_atan;
    gcc_funcs.atan_vrs4 = _ZGVbN4v_atanf;

    // TAN, CBRT, ERF, ERFC - 128-bit (SSE)
    gcc_funcs.tan_vrd2 = _ZGVbN2v_tan;
    gcc_funcs.tan_vrs4 = _ZGVbN4v_tanf;
    gcc_funcs.cbrt_vrd2 = _ZGVbN2v_cbrt;
    gcc_funcs.cbrt_vrs4 = _ZGVbN4v_cbrtf;
    gcc_funcs.erf_vrd2 = _ZGVbN2v_erf;
    gcc_funcs.erf_vrs4 = _ZGVbN4v_erff;
    gcc_funcs.erfc_vrd2 = _ZGVbN2v_erfc;
    gcc_funcs.erfc_vrs4 = _ZGVbN4v_erfcf;

    // SINH, COSH, TANH - 128-bit (SSE)
    gcc_funcs.cosh_vrd2 = _ZGVbN2v_cosh;
    gcc_funcs.cosh_vrs4 = _ZGVbN4v_coshf;
    gcc_funcs.tanh_vrs4 = _ZGVbN4v_tanhf;

    // EXP2, EXP10, EXPM1 - 128-bit (SSE)
    gcc_funcs.exp2_vrd2 = _ZGVbN2v_exp2;
    gcc_funcs.exp2_vrs4 = _ZGVbN4v_exp2f;
    gcc_funcs.exp10_vrd2 = _ZGVbN2v_exp10;
    gcc_funcs.exp10_vrs4 = _ZGVbN4v_exp10f;
    gcc_funcs.expm1_vrs4 = _ZGVbN4v_expm1f;

    // LOG2, LOG10, LOG1P - 128-bit (SSE)
    gcc_funcs.log2_vrd2 = _ZGVbN2v_log2;
    gcc_funcs.log2_vrs4 = _ZGVbN4v_log2f;
    gcc_funcs.log10_vrd2 = _ZGVbN2v_log10;
    gcc_funcs.log10_vrs4 = _ZGVbN4v_log10f;
    gcc_funcs.log1p_vrd2 = _ZGVbN2v_log1p;
    gcc_funcs.log1p_vrs4 = _ZGVbN4v_log1pf;

    // ACOS, ASIN, ATAN - 256-bit (AVX2)
    gcc_funcs.acos_vrd4 = _ZGVdN4v_acos;
    gcc_funcs.acos_vrs8 = _ZGVdN8v_acosf;
    gcc_funcs.asin_vrd4 = _ZGVdN4v_asin;
    gcc_funcs.asin_vrs8 = _ZGVdN8v_asinf;
    gcc_funcs.atan_vrd4 = _ZGVdN4v_atan;
    gcc_funcs.atan_vrs8 = _ZGVdN8v_atanf;

    // TAN, ERF, ERFC - 256-bit (AVX2)
    gcc_funcs.tan_vrd4 = _ZGVdN4v_tan;
    gcc_funcs.tan_vrs8 = _ZGVdN8v_tanf;
    gcc_funcs.erf_vrd4 = _ZGVdN4v_erf;
    gcc_funcs.erf_vrs8 = _ZGVdN8v_erff;
    gcc_funcs.erfc_vrd4 = _ZGVdN4v_erfc;
    gcc_funcs.erfc_vrs8 = _ZGVdN8v_erfcf;

    // COSH - 256-bit (AVX2)
    gcc_funcs.cosh_vrs8 = _ZGVdN8v_coshf;
    gcc_funcs.tanh_vrs8 = _ZGVdN8v_tanhf;

    // EXP2 - 256-bit (AVX2)
    gcc_funcs.exp2_vrd4 = _ZGVdN4v_exp2;
    gcc_funcs.exp2_vrs8 = _ZGVdN8v_exp2f;

    // LOG2, LOG10 - 256-bit (AVX2)
    gcc_funcs.log2_vrd4 = _ZGVdN4v_log2;
    gcc_funcs.log2_vrs8 = _ZGVdN8v_log2f;
    gcc_funcs.log10_vrs8 = _ZGVdN8v_log10f;

#ifdef __AVX512F__
    // ASIN, ATAN - 512-bit (AVX512)
    gcc_funcs.asin_vrd8 = _ZGVeN8v_asin;
    gcc_funcs.asin_vrs16 = _ZGVeN16v_asinf;
    gcc_funcs.atan_vrd8 = _ZGVeN8v_atan;
    gcc_funcs.atan_vrs16 = _ZGVeN16v_atanf;

    // ACOS, TAN - 512-bit (AVX512)
    gcc_funcs.acos_vrs16 = _ZGVeN16v_acosf;
    gcc_funcs.tan_vrd8 = _ZGVeN8v_tan;
    gcc_funcs.tan_vrs16 = _ZGVeN16v_tanf;

    gcc_funcs.tanh_vrs16 = _ZGVeN16v_tanhf;

    // ERF, ERFC - 512-bit (AVX512)
    gcc_funcs.erf_vrd8 = _ZGVeN8v_erf;
    gcc_funcs.erf_vrs16 = _ZGVeN16v_erff;
    gcc_funcs.erfc_vrd8 = _ZGVeN8v_erfc;
    gcc_funcs.erfc_vrs16 = _ZGVeN16v_erfcf;

    // EXP2, EXP10 - 512-bit (AVX512)
    gcc_funcs.exp2_vrd8 = _ZGVeN8v_exp2;
    gcc_funcs.exp2_vrs16 = _ZGVeN16v_exp2f;

    // LOG2, LOG10 - 512-bit (AVX512)
    gcc_funcs.log2_vrd8 = _ZGVeN8v_log2;
    gcc_funcs.log2_vrs16 = _ZGVeN16v_log2f;
    gcc_funcs.log10_vrs16 = _ZGVeN16v_log10f;
#endif
#endif // GLIBC_VERSION >= 235

}

// Linux constructor for automatic initialization
__attribute__((constructor))
static void lib_init(void) {
    init_gcc_symbols();
}

extern "C" {

SHIM_EXPORT const char* get_vendor_name() {
    return "GLIBC";
}

// High-performance shim functions with null pointer safety checks
// Organized by variant to match struct definition order
// ============================================================================
// SINGLE PRECISION SCALAR (ss) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.acosf(ipp->ip[0]);
}

SHIM_EXPORT void shim_acosh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.acoshf(ipp->ip[0]);
}

SHIM_EXPORT void shim_asin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.asinf(ipp->ip[0]);
}

SHIM_EXPORT void shim_asinh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.asinhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.atanf(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.atan2f(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_atanh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.atanhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cbrt_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.cbrtf(ipp->ip[0]);
}

SHIM_EXPORT void shim_ceil_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.ceilf(ipp->ip[0]);
}

SHIM_EXPORT void shim_copysign_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.copysignf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_cos_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.cosf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cosh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.coshf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cospi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.cospif(ipp->ip[0]);
}

SHIM_EXPORT void shim_erf_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.erff(ipp->ip[0]);
}

SHIM_EXPORT void shim_erfc_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.erfcf(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.expf(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp10_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.exp10f(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.exp2f(ipp->ip[0]);
}

SHIM_EXPORT void shim_expm1_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.expm1f(ipp->ip[0]);
}

SHIM_EXPORT void shim_fabs_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.fabsf(ipp->ip[0]);
}

SHIM_EXPORT void shim_fdim_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.fdimf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_finite_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.finitef(ipp->ip[0]);
}

SHIM_EXPORT void shim_floor_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.floorf(ipp->ip[0]);
}

SHIM_EXPORT void shim_fma_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.fmaf(ipp->ip[0], ipp->ip[1], ipp->ip[2]);
}

SHIM_EXPORT void shim_fmax_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.fmaxf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.fminf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmod_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.fmodf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_frexp_ss(InParams<float, float> *ipp) {
    int exp;
    ipp->op[0] = gcc_funcs.frexpf(ipp->ip[0], &exp);
    ipp->op[1] = static_cast<float>(exp);
}

SHIM_EXPORT void shim_hypot_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.hypotf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_ilogb_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(gcc_funcs.ilogbf(ipp->ip[0]));
}

SHIM_EXPORT void shim_ldexp_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.ldexpf(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_llrint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(gcc_funcs.llrintf(ipp->ip[0]));
}

SHIM_EXPORT void shim_llround_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(gcc_funcs.llroundf(ipp->ip[0]));
}

SHIM_EXPORT void shim_log_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.logf(ipp->ip[0]);
}

SHIM_EXPORT void shim_log10_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.log10f(ipp->ip[0]);
}

SHIM_EXPORT void shim_log1p_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.log1pf(ipp->ip[0]);
}

SHIM_EXPORT void shim_log2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.log2f(ipp->ip[0]);
}

SHIM_EXPORT void shim_logb_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.logbf(ipp->ip[0]);
}

SHIM_EXPORT void shim_lrint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(gcc_funcs.lrintf(ipp->ip[0]));
}

SHIM_EXPORT void shim_lround_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(gcc_funcs.lroundf(ipp->ip[0]));
}

SHIM_EXPORT void shim_modf_ss(InParams<float, float> *ipp) {
    float ipart;
    ipp->op[0] = gcc_funcs.modff(ipp->ip[0], &ipart);
    ipp->op[1] = ipart;
}

SHIM_EXPORT void shim_nearbyint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.nearbyintf(ipp->ip[0]);
}

SHIM_EXPORT void shim_nextafter_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.nextafterf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_nexttoward_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.nexttowardf(ipp->ip[0], static_cast<long double>(ipp->ip[1]));
}

SHIM_EXPORT void shim_pow_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.powf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remainder_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.remainderf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remquo_ss(InParams<float, float> *ipp) {
    int quo;
    ipp->op[0] = gcc_funcs.remquof(ipp->ip[0], ipp->ip[1], &quo);
    ipp->op[1] = static_cast<float>(quo);
}

SHIM_EXPORT void shim_rint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.rintf(ipp->ip[0]);
}

SHIM_EXPORT void shim_round_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.roundf(ipp->ip[0]);
}

SHIM_EXPORT void shim_scalbln_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.scalblnf(ipp->ip[0], static_cast<long>(ipp->ip[1]));
}

SHIM_EXPORT void shim_scalbn_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.scalbnf(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_sin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.sinf(ipp->ip[0]);
}

SHIM_EXPORT void shim_sincos_ss(InParams<float, float> *ipp) {
    gcc_funcs.sincosf(ipp->ip[0], &ipp->op[0], &ipp->op[1]);
}

SHIM_EXPORT void shim_sinh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.sinhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_sinpi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.sinpif(ipp->ip[0]);
}

SHIM_EXPORT void shim_sqrt_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.sqrtf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tan_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.tanf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.tanhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanpi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.tanpif(ipp->ip[0]);
}

SHIM_EXPORT void shim_trunc_ss(InParams<float, float> *ipp) {
    ipp->op[0] = gcc_funcs.truncf(ipp->ip[0]);
}

// ============================================================================
// DOUBLE PRECISION SCALAR (sd) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.acos(ipp->ip[0]);
}

SHIM_EXPORT void shim_acosh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.acosh(ipp->ip[0]);
}

SHIM_EXPORT void shim_asin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.asin(ipp->ip[0]);
}

SHIM_EXPORT void shim_asinh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.asinh(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.atan(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.atan2(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_atanh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.atanh(ipp->ip[0]);
}

SHIM_EXPORT void shim_cbrt_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.cbrt(ipp->ip[0]);
}

SHIM_EXPORT void shim_ceil_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.ceil(ipp->ip[0]);
}

SHIM_EXPORT void shim_copysign_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.copysign(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_cos_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.cos(ipp->ip[0]);
}

SHIM_EXPORT void shim_cosh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.cosh(ipp->ip[0]);
}

SHIM_EXPORT void shim_cospi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.cospi(ipp->ip[0]);
}

SHIM_EXPORT void shim_erf_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.erf(ipp->ip[0]);
}

SHIM_EXPORT void shim_erfc_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.erfc(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.exp(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp10_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.exp10(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.exp2(ipp->ip[0]);
}

SHIM_EXPORT void shim_expm1_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.expm1(ipp->ip[0]);
}

SHIM_EXPORT void shim_fabs_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.fabs(ipp->ip[0]);
}

SHIM_EXPORT void shim_fdim_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.fdim(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_finite_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(gcc_funcs.finite(ipp->ip[0]));
}

SHIM_EXPORT void shim_floor_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.floor(ipp->ip[0]);
}

SHIM_EXPORT void shim_fma_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.fma(ipp->ip[0], ipp->ip[1], ipp->ip[2]);
}

SHIM_EXPORT void shim_fmax_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.fmax(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.fmin(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmod_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.fmod(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_frexp_sd(InParams<double, double> *ipp) {
    int exp;
    ipp->op[0] = gcc_funcs.frexp(ipp->ip[0], &exp);
    ipp->op[1] = static_cast<double>(exp);
}

SHIM_EXPORT void shim_hypot_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.hypot(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_ilogb_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(gcc_funcs.ilogb(ipp->ip[0]));
}

SHIM_EXPORT void shim_ldexp_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.ldexp(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_llrint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(gcc_funcs.llrint(ipp->ip[0]));
}

SHIM_EXPORT void shim_llround_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(gcc_funcs.llround(ipp->ip[0]));
}

SHIM_EXPORT void shim_log_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.log(ipp->ip[0]);
}

SHIM_EXPORT void shim_log10_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.log10(ipp->ip[0]);
}

SHIM_EXPORT void shim_log1p_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.log1p(ipp->ip[0]);
}

SHIM_EXPORT void shim_log2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.log2(ipp->ip[0]);
}

SHIM_EXPORT void shim_logb_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.logb(ipp->ip[0]);
}

SHIM_EXPORT void shim_lrint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(gcc_funcs.lrint(ipp->ip[0]));
}

SHIM_EXPORT void shim_lround_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(gcc_funcs.lround(ipp->ip[0]));
}

SHIM_EXPORT void shim_modf_sd(InParams<double, double> *ipp) {
    double ipart;
    ipp->op[0] = gcc_funcs.modf(ipp->ip[0], &ipart);
    ipp->op[1] = ipart;
}

SHIM_EXPORT void shim_nearbyint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.nearbyint(ipp->ip[0]);
}

SHIM_EXPORT void shim_nextafter_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.nextafter(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_nexttoward_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.nexttoward(ipp->ip[0], static_cast<long double>(ipp->ip[1]));
}

SHIM_EXPORT void shim_pow_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.pow(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remainder_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.remainder(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remquo_sd(InParams<double, double> *ipp) {
    int quo;
    ipp->op[0] = gcc_funcs.remquo(ipp->ip[0], ipp->ip[1], &quo);
    ipp->op[1] = static_cast<double>(quo);
}

SHIM_EXPORT void shim_rint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.rint(ipp->ip[0]);
}

SHIM_EXPORT void shim_round_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.round(ipp->ip[0]);
}

SHIM_EXPORT void shim_scalbln_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.scalbln(ipp->ip[0], static_cast<long>(ipp->ip[1]));
}

SHIM_EXPORT void shim_scalbn_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.scalbn(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_sin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.sin(ipp->ip[0]);
}

SHIM_EXPORT void shim_sincos_sd(InParams<double, double> *ipp) {
    gcc_funcs.sincos(ipp->ip[0], &ipp->op[0], &ipp->op[1]);
}

SHIM_EXPORT void shim_sinh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.sinh(ipp->ip[0]);
}

SHIM_EXPORT void shim_sinpi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.sinpi(ipp->ip[0]);
}

SHIM_EXPORT void shim_sqrt_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.sqrt(ipp->ip[0]);
}

SHIM_EXPORT void shim_tan_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.tan(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.tanh(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanpi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.tanpi(ipp->ip[0]);
}

SHIM_EXPORT void shim_trunc_sd(InParams<double, double> *ipp) {
    ipp->op[0] = gcc_funcs.trunc(ipp->ip[0]);
}

// ============================================================================
// DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.acos_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.asin_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.atan_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cbrt_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.cbrt_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.cos_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.cosh_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.erf_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.erfc_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp10_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp10_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp2_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.fabs_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    std::cout << "LinearFrac is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log10_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log1p_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log1p_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log2_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.pow_vrd2(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    std::cout << "Powx is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.sin_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
     // Fallback to scalar implementation due to sincos for vector varients is not working
    double *input = (double*)&ipp->ip[0].data;
    double *sin_out = (double*)&ipp->op[0].data;
    double *cos_out = (double*)&ipp->op[1].data;

    for (int i = 0; i < 2; i++) {
        gcc_funcs.sincos(input[i], &sin_out[i], &cos_out[i]);
    }
}

SHIM_EXPORT void shim_sqrt_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.sqrt_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.tan_vrd2(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.acos_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.asin_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.atan_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cbrt_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.cbrt_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.cos_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.cosh_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.erf_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.erfc_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp10_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp10_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp2_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_expm1_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.expm1_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.fabs_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    std::cout << "LinearFrac is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log10_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log1p_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log1p_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log2_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.pow_vrs4(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    std::cout << "Powx is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.sin_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    // Fallback to scalar implementation due to sincos for vector varients is not working
    float *input = (float*)&ipp->ip[0].data;
    float *sin_out = (float*)&ipp->op[0].data;
    float *cos_out = (float*)&ipp->op[1].data;

    for (int i = 0; i < 4; i++) {
        gcc_funcs.sincosf(input[i], &sin_out[i], &cos_out[i]);
    }
}

SHIM_EXPORT void shim_sqrt_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.sqrt_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.tan_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = gcc_funcs.tanh_vrs4(ipp->ip[0].data);
}

// ============================================================================
// DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.acos_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.asin_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.atan_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.cos_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.erf_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.erfc_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp2_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.fabs_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    std::cout << "LinearFrac is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log2_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.pow_vrd4(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    std::cout << "Powx is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.sin_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    // Fallback to scalar implementation due to sincos for vector varients is not working
    double *input = (double*)&ipp->ip[0].data;
    double *sin_out = (double*)&ipp->op[0].data;
    double *cos_out = (double*)&ipp->op[1].data;

    for (int i = 0; i < 4; i++) {
        gcc_funcs.sincos(input[i], &sin_out[i], &cos_out[i]);
    }
}

SHIM_EXPORT void shim_sqrt_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.sqrt_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.tan_vrd4(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.acos_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.asin_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.atan_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.cos_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.cosh_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.erf_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.erfc_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp2_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.fabs_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    std::cout << "LinearFrac is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log10_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log2_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.pow_vrs8(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    std::cout << "Powx is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.sin_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
   // Fallback to scalar implementation due to sincos for vector varients is not working
    float *input = (float*)&ipp->ip[0].data;
    float *sin_out = (float*)&ipp->op[0].data;
    float *cos_out = (float*)&ipp->op[1].data;

    for (int i = 0; i < 8; i++) {
        gcc_funcs.sincosf(input[i], &sin_out[i], &cos_out[i]);
    }
}

SHIM_EXPORT void shim_sqrt_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.sqrt_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.tan_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = gcc_funcs.tanh_vrs8(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION ARRAY (vrsa) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_add_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_addfi_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_asin_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_atan_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_cbrt_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_cos_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_cosh_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_div_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_divfi_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_erf_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_erfc_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_exp_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_exp10_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_exp2_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_expm1_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fabs_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmax_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmaxfi_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmin_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fminfi_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_linearfrac_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log10_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log1p_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log2_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_mul_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_mulfi_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_pow_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_powx_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sincos_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sqrt_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sub_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_subfi_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_tan_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_tanh_vrsa(InParams<float, float> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

// ============================================================================
// DOUBLE PRECISION ARRAY (vrda) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_add_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_addi_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_asin_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_atan_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_cbrt_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_cos_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_cosh_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_div_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_divi_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_erf_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_erfc_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_exp_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_exp10_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_exp2_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_expm1_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fabs_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmax_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmaxi_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmin_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_fmini_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_linearfrac_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log10_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log1p_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log2_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_mul_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_muli_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_pow_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_powx_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sincos_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sqrt_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sub_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_subi_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_tan_vrda(InParams<double, double> *ipp) {
    std::cout << "Array-based operations not supported in GCC/GLIBC" << std::endl;
}

#ifdef __AVX512F__
// ============================================================================
// DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_asin_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.asin_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.atan_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.cos_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.erf_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.erfc_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.exp2_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    std::cout << "LinearFrac is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.log2_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.pow_vrd8(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    std::cout << "Powx is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.sin_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    // Fallback to scalar implementation due to sincos for vector varients is not working
    double *input = (double*)&ipp->ip[0].data;
    double *sin_out = (double*)&ipp->op[0].data;
    double *cos_out = (double*)&ipp->op[1].data;

    for (int i = 0; i < 8; i++) {
        gcc_funcs.sincos(input[i], &sin_out[i], &cos_out[i]);
    }
}

SHIM_EXPORT void shim_sqrt_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.sqrt_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = gcc_funcs.tan_vrd8(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.acos_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.asin_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.atan_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.cos_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.erf_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.erfc_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.exp2_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    std::cout << "LinearFrac is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_log_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log10_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.log2_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.pow_vrs16(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    std::cout << "Powx is not supported in GCC/GLIBC" << std::endl;
}

SHIM_EXPORT void shim_sin_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.sin_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    // Fallback to scalar implementation due to sincos for vector varients is not working
    float *input = (float*)&ipp->ip[0].data;
    float *sin_out = (float*)&ipp->op[0].data;
    float *cos_out = (float*)&ipp->op[1].data;

    for (int i = 0; i < 16; i++) {
        gcc_funcs.sincosf(input[i], &sin_out[i], &cos_out[i]);
    }
}

SHIM_EXPORT void shim_sqrt_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.sqrt_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.tan_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = gcc_funcs.tanh_vrs16(ipp->ip[0].data);
}
#endif

} // extern "C"