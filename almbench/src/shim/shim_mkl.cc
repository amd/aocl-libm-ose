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
 * High-performance cross-platform LD_PRELOAD shim for Intel MKL functions
 * Optimized for minimal overhead with one-time symbol loading
 * Compatible with Windows and Linux without pthread dependency
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include "alm_test.h"

#ifdef _WIN32
    #include <windows.h>
    #define SHIM_EXPORT __declspec(dllexport)
    #define LOAD_SYMBOL(lib, name) GetProcAddress(lib, name)
    #define LOAD_LIBRARY(name) LoadLibraryA(name)
    #define LIB_HANDLE HMODULE
#else
    #include <dlfcn.h>
    #define SHIM_EXPORT __attribute__((visibility("default")))
    #define LOAD_SYMBOL(lib, name) dlsym(lib, name)
    #define LOAD_LIBRARY(name) dlopen(name, RTLD_LAZY)
    #define LIB_HANDLE void*
#endif

// ============================================================================
// FUNCTION POINTER TYPEDEFS ORGANIZED BY VARIANT
// ============================================================================
// --- Single Precision Scalar (ss) Functions ---
typedef float (*mkl_acos_ss_func_t)(float);
typedef float (*mkl_acosh_ss_func_t)(float);
typedef float (*mkl_asin_ss_func_t)(float);
typedef float (*mkl_asinh_ss_func_t)(float);
typedef float (*mkl_atan_ss_func_t)(float);
typedef float (*mkl_atan2_ss_func_t)(float, float);
typedef float (*mkl_atanh_ss_func_t)(float);
typedef float (*mkl_cbrt_ss_func_t)(float);
typedef float (*mkl_ceil_ss_func_t)(float);
typedef float (*mkl_copysign_ss_func_t)(float, float);
typedef float (*mkl_cos_ss_func_t)(float);
typedef float (*mkl_cosh_ss_func_t)(float);
typedef float (*mkl_cospi_ss_func_t)(float);
typedef float (*mkl_erf_ss_func_t)(float);
typedef float (*mkl_erfc_ss_func_t)(float);
typedef float (*mkl_exp_ss_func_t)(float);
typedef float (*mkl_exp10_ss_func_t)(float);
typedef float (*mkl_exp2_ss_func_t)(float);
typedef float (*mkl_expm1_ss_func_t)(float);
typedef float (*mkl_fabs_ss_func_t)(int, float*, float*);
typedef float (*mkl_fdim_ss_func_t)(float, float);
typedef float (*mkl_finite_ss_func_t)(float);
typedef float (*mkl_floor_ss_func_t)(float);
typedef float (*mkl_fma_ss_func_t)(float, float, float);
typedef float (*mkl_fmax_ss_func_t)(float, float);
typedef float (*mkl_fmin_ss_func_t)(float, float);
typedef float (*mkl_fmod_ss_func_t)(float, float);
typedef float (*mkl_frexp_ss_func_t)(float, int*);
typedef float (*mkl_hypot_ss_func_t)(float, float);
typedef int (*mkl_ilogb_ss_func_t)(float);
typedef float (*mkl_ldexp_ss_func_t)(float, int);
typedef long long (*mkl_llrint_ss_func_t)(float);
typedef long long (*mkl_llround_ss_func_t)(float);
typedef float (*mkl_log_ss_func_t)(float);
typedef float (*mkl_log10_ss_func_t)(float);
typedef float (*mkl_log1p_ss_func_t)(float);
typedef float (*mkl_log2_ss_func_t)(float);
typedef float (*mkl_logb_ss_func_t)(float);
typedef long (*mkl_lrint_ss_func_t)(float);
typedef long (*mkl_lround_ss_func_t)(float);
typedef float (*mkl_modf_ss_func_t)(float, float*);
typedef float (*mkl_nearbyint_ss_func_t)(float);
typedef float (*mkl_nextafter_ss_func_t)(float, float);
typedef float (*mkl_nexttoward_ss_func_t)(float, long double);
typedef float (*mkl_pow_ss_func_t)(float, float);
typedef float (*mkl_remainder_ss_func_t)(float, float);
typedef float (*mkl_remquo_ss_func_t)(float, float, int*);
typedef float (*mkl_rint_ss_func_t)(float);
typedef float (*mkl_round_ss_func_t)(float);
typedef float (*mkl_scalbln_ss_func_t)(float, long);
typedef float (*mkl_scalbn_ss_func_t)(float, int);
typedef float (*mkl_sin_ss_func_t)(float);
typedef void (*mkl_sincos_ss_func_t)(float, float*, float*);
typedef float (*mkl_sinh_ss_func_t)(float);
typedef float (*mkl_sinpi_ss_func_t)(float);
typedef float (*mkl_sqrt_ss_func_t)(float);
typedef float (*mkl_tan_ss_func_t)(float);
typedef float (*mkl_tanh_ss_func_t)(float);
typedef float (*mkl_tanpi_ss_func_t)(float);
typedef float (*mkl_trunc_ss_func_t)(float);

// --- Double Precision Scalar (sd) Functions ---
typedef double (*mkl_acos_sd_func_t)(double);
typedef double (*mkl_acosh_sd_func_t)(double);
typedef double (*mkl_asin_sd_func_t)(double);
typedef double (*mkl_asinh_sd_func_t)(double);
typedef double (*mkl_atan_sd_func_t)(double);
typedef double (*mkl_atan2_sd_func_t)(double, double);
typedef double (*mkl_atanh_sd_func_t)(double);
typedef double (*mkl_cbrt_sd_func_t)(double);
typedef double (*mkl_ceil_sd_func_t)(double);
typedef double (*mkl_copysign_sd_func_t)(double, double);
typedef double (*mkl_cos_sd_func_t)(double);
typedef double (*mkl_cosh_sd_func_t)(double);
typedef double (*mkl_cospi_sd_func_t)(double);
typedef double (*mkl_erf_sd_func_t)(double);
typedef double (*mkl_erfc_sd_func_t)(double);
typedef double (*mkl_exp_sd_func_t)(double);
typedef double (*mkl_exp10_sd_func_t)(double);
typedef double (*mkl_exp2_sd_func_t)(double);
typedef double (*mkl_expm1_sd_func_t)(double);
typedef double (*mkl_fabs_sd_func_t)(int, double*, double*);
typedef double (*mkl_fdim_sd_func_t)(double, double);
typedef double (*mkl_finite_sd_func_t)(double);
typedef double (*mkl_floor_sd_func_t)(double);
typedef double (*mkl_fma_sd_func_t)(double, double, double);
typedef double (*mkl_fmax_sd_func_t)(double, double);
typedef double (*mkl_fmin_sd_func_t)(double, double);
typedef double (*mkl_fmod_sd_func_t)(double, double);
typedef double (*mkl_frexp_sd_func_t)(double, int*);
typedef double (*mkl_hypot_sd_func_t)(double, double);
typedef int (*mkl_ilogb_sd_func_t)(double);
typedef double (*mkl_ldexp_sd_func_t)(double, int);
typedef long long (*mkl_llrint_sd_func_t)(double);
typedef long long (*mkl_llround_sd_func_t)(double);
typedef double (*mkl_log_sd_func_t)(double);
typedef double (*mkl_log10_sd_func_t)(double);
typedef double (*mkl_log1p_sd_func_t)(double);
typedef double (*mkl_log2_sd_func_t)(double);
typedef double (*mkl_logb_sd_func_t)(double);
typedef long (*mkl_lrint_sd_func_t)(double);
typedef long (*mkl_lround_sd_func_t)(double);
typedef double (*mkl_modf_sd_func_t)(double, double*);
typedef double (*mkl_nearbyint_sd_func_t)(double);
typedef double (*mkl_nextafter_sd_func_t)(double, double);
typedef double (*mkl_nexttoward_sd_func_t)(double, long double);
typedef double (*mkl_pow_sd_func_t)(double, double);
typedef double (*mkl_remainder_sd_func_t)(double, double);
typedef double (*mkl_remquo_sd_func_t)(double, double, int*);
typedef double (*mkl_rint_sd_func_t)(double);
typedef double (*mkl_round_sd_func_t)(double);
typedef double (*mkl_scalbln_sd_func_t)(double, long);
typedef double (*mkl_scalbn_sd_func_t)(double, int);
typedef double (*mkl_sin_sd_func_t)(double);
typedef void (*mkl_sincos_sd_func_t)(double, double*, double*);
typedef double (*mkl_sinh_sd_func_t)(double);
typedef double (*mkl_sinpi_sd_func_t)(double);
typedef double (*mkl_sqrt_sd_func_t)(double);
typedef double (*mkl_tan_sd_func_t)(double);
typedef double (*mkl_tanh_sd_func_t)(double);
typedef double (*mkl_tanpi_sd_func_t)(double);
typedef double (*mkl_trunc_sd_func_t)(double);

// --- Double Precision 128-bit Vector (vrd2) Functions ---
typedef __m128d (*mkl_acos_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_asin_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_atan_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_cbrt_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_cos_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_cosh_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_erf_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_erfc_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_exp_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_exp10_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_exp2_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_fabs_vrd2_func_t)(int, double*, double*);
typedef __m128d (*mkl_linearfrac_vrd2_func_t)(int, const double*, const double*, double, double, double, double, double*);
typedef __m128d (*mkl_log_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_log10_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_log1p_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_log2_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_pow_vrd2_func_t)(__m128d, __m128d);
typedef void (*mkl_powx_vrd2_func_t)(int, double*, double, double*);
typedef __m128d (*mkl_sin_vrd2_func_t)(__m128d);
typedef void (*mkl_sincos_vrd2_func_t)(__m128d, __m128d*, __m128d*);
typedef __m128d (*mkl_sqrt_vrd2_func_t)(__m128d);
typedef __m128d (*mkl_tan_vrd2_func_t)(__m128d);

// --- Single Precision 128-bit Vector (vrs4) Functions ---
typedef __m128 (*mkl_acos_vrs4_func_t)(__m128);
typedef __m128 (*mkl_asin_vrs4_func_t)(__m128);
typedef __m128 (*mkl_atan_vrs4_func_t)(__m128);
typedef __m128 (*mkl_cbrt_vrs4_func_t)(__m128);
typedef __m128 (*mkl_cos_vrs4_func_t)(__m128);
typedef __m128 (*mkl_cosh_vrs4_func_t)(__m128);
typedef __m128 (*mkl_erf_vrs4_func_t)(__m128);
typedef __m128 (*mkl_erfc_vrs4_func_t)(__m128);
typedef __m128 (*mkl_exp_vrs4_func_t)(__m128);
typedef __m128 (*mkl_exp10_vrs4_func_t)(__m128);
typedef __m128 (*mkl_exp2_vrs4_func_t)(__m128);
typedef __m128 (*mkl_expm1_vrs4_func_t)(__m128);
typedef __m128 (*mkl_fabs_vrs4_func_t)(int, float*, float*);
typedef __m128 (*mkl_linearfrac_vrs4_func_t)(int, const float*,
    const float*, float, float, float, float, float*);
typedef __m128 (*mkl_log_vrs4_func_t)(__m128);
typedef __m128 (*mkl_log10_vrs4_func_t)(__m128);
typedef __m128 (*mkl_log1p_vrs4_func_t)(__m128);
typedef __m128 (*mkl_log2_vrs4_func_t)(__m128);
typedef __m128 (*mkl_pow_vrs4_func_t)(__m128, __m128);
typedef void (*mkl_powx_vrs4_func_t)(int, float*, float, float*);
typedef __m128 (*mkl_sin_vrs4_func_t)(__m128);
typedef void (*mkl_sincos_vrs4_func_t)(__m128, __m128*, __m128*);
typedef __m128 (*mkl_sqrt_vrs4_func_t)(__m128);
typedef __m128 (*mkl_tan_vrs4_func_t)(__m128);
typedef __m128 (*mkl_tanh_vrs4_func_t)(__m128);

// --- Double Precision 256-bit Vector (vrd4) Functions ---
typedef __m256d (*mkl_acos_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_asin_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_atan_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_cos_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_erf_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_erfc_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_exp_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_exp2_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_fabs_vrd4_func_t)(int, double*, double*);
typedef __m256d (*mkl_linearfrac_vrd4_func_t)(int, const double*, const double*, double, double, double, double, double*);
typedef __m256d (*mkl_log_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_log2_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_pow_vrd4_func_t)(__m256d, __m256d);
typedef void (*mkl_powx_vrd4_func_t)(int, double*, double, double*);
typedef __m256d (*mkl_sin_vrd4_func_t)(__m256d);
typedef void (*mkl_sincos_vrd4_func_t)(__m256d, __m256d*, __m256d*);
typedef __m256d (*mkl_sqrt_vrd4_func_t)(__m256d);
typedef __m256d (*mkl_tan_vrd4_func_t)(__m256d);

// --- Single Precision 256-bit Vector (vrs8) Functions ---
typedef __m256 (*mkl_acos_vrs8_func_t)(__m256);
typedef __m256 (*mkl_asin_vrs8_func_t)(__m256);
typedef __m256 (*mkl_atan_vrs8_func_t)(__m256);
typedef __m256 (*mkl_cos_vrs8_func_t)(__m256);
typedef __m256 (*mkl_cosh_vrs8_func_t)(__m256);
typedef __m256 (*mkl_erf_vrs8_func_t)(__m256);
typedef __m256 (*mkl_erfc_vrs8_func_t)(__m256);
typedef __m256 (*mkl_exp_vrs8_func_t)(__m256);
typedef __m256 (*mkl_exp2_vrs8_func_t)(__m256);
typedef __m256 (*mkl_fabs_vrs8_func_t)(int, float*, float*);
typedef __m256 (*mkl_linearfrac_vrs8_func_t)(int, const float*,
    const float*, float, float, float, float, float*);
typedef __m256 (*mkl_log_vrs8_func_t)(__m256);
typedef __m256 (*mkl_log10_vrs8_func_t)(__m256);
typedef __m256 (*mkl_log2_vrs8_func_t)(__m256);
typedef __m256 (*mkl_pow_vrs8_func_t)(__m256, __m256);
typedef void (*mkl_powx_vrs8_func_t)(int, float*, float, float*);
typedef __m256 (*mkl_sin_vrs8_func_t)(__m256);
typedef void (*mkl_sincos_vrs8_func_t)(__m256, __m256*, __m256*);
typedef __m256 (*mkl_sqrt_vrs8_func_t)(__m256);
typedef __m256 (*mkl_tan_vrs8_func_t)(__m256);
typedef __m256 (*mkl_tanh_vrs8_func_t)(__m256);

// --- Single Precision Array (vrsa) Functions ---
typedef void (*mkl_acos_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_add_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_addfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_asin_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_atan_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_cbrt_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_cos_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_cosh_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_div_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_divfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_erf_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_erfc_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_exp_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_exp10_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_exp2_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_expm1_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_fabs_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_fmax_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_fmaxfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_fmin_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_fminfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_linearfrac_vrsa_func_t)(int, const float*,
    const float*, float, float, float, float, float*);
typedef void (*mkl_log_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_log10_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_log1p_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_log2_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_mul_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_mulfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_pow_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_powx_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_sin_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_sincos_vrsa_func_t)(int, const float*, float*, float*);
typedef void (*mkl_sqrt_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_sub_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*mkl_subfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*mkl_tan_vrsa_func_t)(int, const float*, float*);
typedef void (*mkl_tanh_vrsa_func_t)(int, const float*, float*);

// --- Double Precision Array (vrda) Functions ---
typedef void (*mkl_acos_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_add_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_addi_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_asin_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_atan_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_cbrt_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_cos_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_cosh_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_div_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_divi_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_erf_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_erfc_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_exp_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_exp10_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_exp2_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_expm1_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_fabs_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_fmax_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_fmaxi_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_fmin_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_fmini_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_linearfrac_vrda_func_t)(int, const double*, const double*, double, double, double, double, double*);
typedef void (*mkl_log_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_log10_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_log1p_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_log2_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_mul_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_muli_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_pow_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_powx_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_sin_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_sincos_vrda_func_t)(int, const double*, double*, double*);
typedef void (*mkl_sqrt_vrda_func_t)(int, const double*, double*);
typedef void (*mkl_sub_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*mkl_subi_vrda_func_t)(int, const double*, double, double*);
typedef void (*mkl_tan_vrda_func_t)(int, const double*, double*);

#ifdef __AVX512F__
// --- Double Precision 512-bit Vector (vrd8) Functions ---
typedef __m512d (*mkl_asin_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_atan_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_cos_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_erf_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_erfc_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_exp_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_exp2_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_linearfrac_vrd8_func_t)(int, const double*, const double*, double, double, double, double, double*);
typedef __m512d (*mkl_log_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_log2_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_pow_vrd8_func_t)(__m512d, __m512d);
typedef void (*mkl_powx_vrd8_func_t)(int, double *, double, double *);
typedef __m512d (*mkl_sin_vrd8_func_t)(__m512d);
typedef void (*mkl_sincos_vrd8_func_t)(__m512d, __m512d*, __m512d*);
typedef __m512d (*mkl_sqrt_vrd8_func_t)(__m512d);
typedef __m512d (*mkl_tan_vrd8_func_t)(__m512d);

// --- Single Precision 512-bit Vector (vrs16) Functions ---
typedef __m512 (*mkl_acos_vrs16_func_t)(__m512);
typedef __m512 (*mkl_asin_vrs16_func_t)(__m512);
typedef __m512 (*mkl_atan_vrs16_func_t)(__m512);
typedef __m512 (*mkl_cos_vrs16_func_t)(__m512);
typedef __m512 (*mkl_erf_vrs16_func_t)(__m512);
typedef __m512 (*mkl_erfc_vrs16_func_t)(__m512);
typedef __m512 (*mkl_exp_vrs16_func_t)(__m512);
typedef __m512 (*mkl_exp10_vrs16_func_t)(__m512);
typedef __m512 (*mkl_exp2_vrs16_func_t)(__m512);
typedef __m512 (*mkl_linearfrac_vrs16_func_t)(int, const float*,
    const float*, float, float, float, float, float*);
typedef __m512 (*mkl_log_vrs16_func_t)(__m512);
typedef __m512 (*mkl_log10_vrs16_func_t)(__m512);
typedef __m512 (*mkl_log2_vrs16_func_t)(__m512);
typedef __m512 (*mkl_pow_vrs16_func_t)(__m512, __m512);
typedef void (*mkl_powx_vrs16_func_t)(int, float *, float, float*);
typedef __m512 (*mkl_sin_vrs16_func_t)(__m512);
typedef void (*mkl_sincos_vrs16_func_t)(__m512, __m512*, __m512*);
typedef __m512 (*mkl_sqrt_vrs16_func_t)(__m512);
typedef __m512 (*mkl_tan_vrs16_func_t)(__m512);
typedef __m512 (*mkl_tanh_vrs16_func_t)(__m512);
#endif

// Global function pointers (loaded once during initialization) - organized by variant
static struct {
    // ============================================================================
    // SINGLE PRECISION SCALAR (ss) VARIANTS
    // ============================================================================
    mkl_acos_ss_func_t acosf;
    mkl_acosh_ss_func_t acoshf;
    mkl_asin_ss_func_t asinf;
    mkl_asinh_ss_func_t asinhf;
    mkl_atan_ss_func_t atanf;
    mkl_atan2_ss_func_t atan2f;
    mkl_atanh_ss_func_t atanhf;
    mkl_cbrt_ss_func_t cbrtf;
    mkl_ceil_ss_func_t ceilf;
    mkl_copysign_ss_func_t copysignf;
    mkl_cos_ss_func_t cosf;
    mkl_cosh_ss_func_t coshf;
    mkl_cospi_ss_func_t cospif;
    mkl_erf_ss_func_t erff;
    mkl_erfc_ss_func_t erfcf;
    mkl_exp_ss_func_t expf;
    mkl_exp10_ss_func_t exp10f;
    mkl_exp2_ss_func_t exp2f;
    mkl_expm1_ss_func_t expm1f;
    mkl_fabs_ss_func_t fabsf;
    mkl_fdim_ss_func_t fdimf;
    mkl_finite_ss_func_t finitef;
    mkl_floor_ss_func_t floorf;
    mkl_fma_ss_func_t fmaf;
    mkl_fmax_ss_func_t fmaxf;
    mkl_fmin_ss_func_t fminf;
    mkl_fmod_ss_func_t fmodf;
    mkl_frexp_ss_func_t frexpf;
    mkl_hypot_ss_func_t hypotf;
    mkl_ilogb_ss_func_t ilogbf;
    mkl_ldexp_ss_func_t ldexpf;
    mkl_llrint_ss_func_t llrintf;
    mkl_llround_ss_func_t llroundf;
    mkl_log_ss_func_t logf;
    mkl_log10_ss_func_t log10f;
    mkl_log1p_ss_func_t log1pf;
    mkl_log2_ss_func_t log2f;
    mkl_logb_ss_func_t logbf;
    mkl_lrint_ss_func_t lrintf;
    mkl_lround_ss_func_t lroundf;
    mkl_modf_ss_func_t modff;
    mkl_nearbyint_ss_func_t nearbyintf;
    mkl_nextafter_ss_func_t nextafterf;
    mkl_nexttoward_ss_func_t nexttowardf;
    mkl_pow_ss_func_t powf;
    mkl_remainder_ss_func_t remainderf;
    mkl_remquo_ss_func_t remquof;
    mkl_rint_ss_func_t rintf;
    mkl_round_ss_func_t roundf;
    mkl_scalbln_ss_func_t scalblnf;
    mkl_scalbn_ss_func_t scalbnf;
    mkl_sin_ss_func_t sinf;
    mkl_sincos_ss_func_t sincosf;
    mkl_sinh_ss_func_t sinhf;
    mkl_sinpi_ss_func_t sinpif;
    mkl_sqrt_ss_func_t sqrtf;
    mkl_tan_ss_func_t tanf;
    mkl_tanh_ss_func_t tanhf;
    mkl_tanpi_ss_func_t tanpif;
    mkl_trunc_ss_func_t truncf;

    // ============================================================================
    // DOUBLE PRECISION SCALAR (sd) VARIANTS
    // ============================================================================
    mkl_acos_sd_func_t acos;
    mkl_acosh_sd_func_t acosh;
    mkl_asin_sd_func_t asin;
    mkl_asinh_sd_func_t asinh;
    mkl_atan_sd_func_t atan;
    mkl_atan2_sd_func_t atan2;
    mkl_atanh_sd_func_t atanh;
    mkl_cbrt_sd_func_t cbrt;
    mkl_ceil_sd_func_t ceil;
    mkl_copysign_sd_func_t copysign;
    mkl_cos_sd_func_t cos;
    mkl_cosh_sd_func_t cosh;
    mkl_cospi_sd_func_t cospi;
    mkl_erf_sd_func_t erf;
    mkl_erfc_sd_func_t erfc;
    mkl_exp_sd_func_t exp;
    mkl_exp10_sd_func_t exp10;
    mkl_exp2_sd_func_t exp2;
    mkl_expm1_sd_func_t expm1;
    mkl_fabs_sd_func_t fabs;
    mkl_fdim_sd_func_t fdim;
    mkl_finite_sd_func_t finite;
    mkl_floor_sd_func_t floor;
    mkl_fma_sd_func_t fma;
    mkl_fmax_sd_func_t fmax;
    mkl_fmin_sd_func_t fmin;
    mkl_fmod_sd_func_t fmod;
    mkl_frexp_sd_func_t frexp;
    mkl_hypot_sd_func_t hypot;
    mkl_ilogb_sd_func_t ilogb;
    mkl_ldexp_sd_func_t ldexp;
    mkl_llrint_sd_func_t llrint;
    mkl_llround_sd_func_t llround;
    mkl_log_sd_func_t log;
    mkl_log10_sd_func_t log10;
    mkl_log1p_sd_func_t log1p;
    mkl_log2_sd_func_t log2;
    mkl_logb_sd_func_t logb;
    mkl_lrint_sd_func_t lrint;
    mkl_lround_sd_func_t lround;
    mkl_modf_sd_func_t modf;
    mkl_nearbyint_sd_func_t nearbyint;
    mkl_nextafter_sd_func_t nextafter;
    mkl_nexttoward_sd_func_t nexttoward;
    mkl_pow_sd_func_t pow;
    mkl_remainder_sd_func_t remainder;
    mkl_remquo_sd_func_t remquo;
    mkl_rint_sd_func_t rint;
    mkl_round_sd_func_t round;
    mkl_scalbln_sd_func_t scalbln;
    mkl_scalbn_sd_func_t scalbn;
    mkl_sin_sd_func_t sin;
    mkl_sincos_sd_func_t sincos;
    mkl_sinh_sd_func_t sinh;
    mkl_sinpi_sd_func_t sinpi;
    mkl_sqrt_sd_func_t sqrt;
    mkl_tan_sd_func_t tan;
    mkl_tanh_sd_func_t tanh;
    mkl_tanpi_sd_func_t tanpi;
    mkl_trunc_sd_func_t trunc;

    // ============================================================================
    // DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
    // ============================================================================
    mkl_acos_vrd2_func_t acos_vrd2;
    mkl_asin_vrd2_func_t asin_vrd2;
    mkl_atan_vrd2_func_t atan_vrd2;
    mkl_cbrt_vrd2_func_t cbrt_vrd2;
    mkl_cos_vrd2_func_t cos_vrd2;
    mkl_cosh_vrd2_func_t cosh_vrd2;
    mkl_erf_vrd2_func_t erf_vrd2;
    mkl_erfc_vrd2_func_t erfc_vrd2;
    mkl_exp_vrd2_func_t exp_vrd2;
    mkl_exp10_vrd2_func_t exp10_vrd2;
    mkl_exp2_vrd2_func_t exp2_vrd2;
    mkl_fabs_vrd2_func_t fabs_vrd2;
    mkl_linearfrac_vrd2_func_t linearfrac_vrd2;
    mkl_log_vrd2_func_t log_vrd2;
    mkl_log10_vrd2_func_t log10_vrd2;
    mkl_log1p_vrd2_func_t log1p_vrd2;
    mkl_log2_vrd2_func_t log2_vrd2;
    mkl_pow_vrd2_func_t pow_vrd2;
    mkl_powx_vrd2_func_t powx_vrd2;
    mkl_sin_vrd2_func_t sin_vrd2;
    mkl_sincos_vrd2_func_t sincos_vrd2;
    mkl_sqrt_vrd2_func_t sqrt_vrd2;
    mkl_tan_vrd2_func_t tan_vrd2;

    // ============================================================================
    // SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
    // ============================================================================
    mkl_acos_vrs4_func_t acos_vrs4;
    mkl_asin_vrs4_func_t asin_vrs4;
    mkl_atan_vrs4_func_t atan_vrs4;
    mkl_cbrt_vrs4_func_t cbrt_vrs4;
    mkl_cos_vrs4_func_t cos_vrs4;
    mkl_cosh_vrs4_func_t cosh_vrs4;
    mkl_erf_vrs4_func_t erf_vrs4;
    mkl_erfc_vrs4_func_t erfc_vrs4;
    mkl_exp_vrs4_func_t exp_vrs4;
    mkl_exp10_vrs4_func_t exp10_vrs4;
    mkl_exp2_vrs4_func_t exp2_vrs4;
    mkl_expm1_vrs4_func_t expm1_vrs4;
    mkl_fabs_vrs4_func_t fabs_vrs4;
    mkl_linearfrac_vrs4_func_t linearfrac_vrs4;
    mkl_log_vrs4_func_t log_vrs4;
    mkl_log10_vrs4_func_t log10_vrs4;
    mkl_log1p_vrs4_func_t log1p_vrs4;
    mkl_log2_vrs4_func_t log2_vrs4;
    mkl_pow_vrs4_func_t pow_vrs4;
    mkl_powx_vrs4_func_t powx_vrs4;
    mkl_sin_vrs4_func_t sin_vrs4;
    mkl_sincos_vrs4_func_t sincos_vrs4;
    mkl_sqrt_vrs4_func_t sqrt_vrs4;
    mkl_tan_vrs4_func_t tan_vrs4;
    mkl_tanh_vrs4_func_t tanh_vrs4;

    // ============================================================================
    // DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
    // ============================================================================
    mkl_acos_vrd4_func_t acos_vrd4;
    mkl_asin_vrd4_func_t asin_vrd4;
    mkl_atan_vrd4_func_t atan_vrd4;
    mkl_cos_vrd4_func_t cos_vrd4;
    mkl_erf_vrd4_func_t erf_vrd4;
    mkl_erfc_vrd4_func_t erfc_vrd4;
    mkl_exp_vrd4_func_t exp_vrd4;
    mkl_exp2_vrd4_func_t exp2_vrd4;
    mkl_fabs_vrd4_func_t fabs_vrd4;
    mkl_linearfrac_vrd4_func_t linearfrac_vrd4;
    mkl_log_vrd4_func_t log_vrd4;
    mkl_log2_vrd4_func_t log2_vrd4;
    mkl_pow_vrd4_func_t pow_vrd4;
    mkl_powx_vrd4_func_t powx_vrd4;
    mkl_sin_vrd4_func_t sin_vrd4;
    mkl_sincos_vrd4_func_t sincos_vrd4;
    mkl_sqrt_vrd4_func_t sqrt_vrd4;
    mkl_tan_vrd4_func_t tan_vrd4;

    // ============================================================================
    // SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
    // ============================================================================
    mkl_acos_vrs8_func_t acos_vrs8;
    mkl_asin_vrs8_func_t asin_vrs8;
    mkl_atan_vrs8_func_t atan_vrs8;
    mkl_cos_vrs8_func_t cos_vrs8;
    mkl_cosh_vrs8_func_t cosh_vrs8;
    mkl_erf_vrs8_func_t erf_vrs8;
    mkl_erfc_vrs8_func_t erfc_vrs8;
    mkl_exp_vrs8_func_t exp_vrs8;
    mkl_exp2_vrs8_func_t exp2_vrs8;
    mkl_fabs_vrs8_func_t fabs_vrs8;
    mkl_linearfrac_vrs8_func_t linearfrac_vrs8;
    mkl_log_vrs8_func_t log_vrs8;
    mkl_log10_vrs8_func_t log10_vrs8;
    mkl_log2_vrs8_func_t log2_vrs8;
    mkl_pow_vrs8_func_t pow_vrs8;
    mkl_powx_vrs8_func_t powx_vrs8;
    mkl_sin_vrs8_func_t sin_vrs8;
    mkl_sincos_vrs8_func_t sincos_vrs8;
    mkl_sqrt_vrs8_func_t sqrt_vrs8;
    mkl_tan_vrs8_func_t tan_vrs8;
    mkl_tanh_vrs8_func_t tanh_vrs8;

    // ============================================================================
    // SINGLE PRECISION ARRAY (vrsa) VARIANTS
    // ============================================================================
    mkl_acos_vrsa_func_t acos_vrsa;
    mkl_add_vrsa_func_t add_vrsa;
    mkl_addfi_vrsa_func_t addfi_vrsa;
    mkl_asin_vrsa_func_t asin_vrsa;
    mkl_atan_vrsa_func_t atan_vrsa;
    mkl_cbrt_vrsa_func_t cbrt_vrsa;
    mkl_cos_vrsa_func_t cos_vrsa;
    mkl_cosh_vrsa_func_t cosh_vrsa;
    mkl_div_vrsa_func_t div_vrsa;
    mkl_divfi_vrsa_func_t divfi_vrsa;
    mkl_erf_vrsa_func_t erf_vrsa;
    mkl_erfc_vrsa_func_t erfc_vrsa;
    mkl_exp_vrsa_func_t exp_vrsa;
    mkl_exp10_vrsa_func_t exp10_vrsa;
    mkl_exp2_vrsa_func_t exp2_vrsa;
    mkl_expm1_vrsa_func_t expm1_vrsa;
    mkl_fabs_vrsa_func_t fabs_vrsa;
    mkl_fmax_vrsa_func_t fmax_vrsa;
    mkl_fmaxfi_vrsa_func_t fmaxfi_vrsa;
    mkl_fmin_vrsa_func_t fmin_vrsa;
    mkl_fminfi_vrsa_func_t fminfi_vrsa;
    mkl_linearfrac_vrsa_func_t linearfrac_vrsa;
    mkl_log_vrsa_func_t log_vrsa;
    mkl_log10_vrsa_func_t log10_vrsa;
    mkl_log1p_vrsa_func_t log1p_vrsa;
    mkl_log2_vrsa_func_t log2_vrsa;
    mkl_mul_vrsa_func_t mul_vrsa;
    mkl_mulfi_vrsa_func_t mulfi_vrsa;
    mkl_pow_vrsa_func_t pow_vrsa;
    mkl_powx_vrsa_func_t powx_vrsa;
    mkl_sin_vrsa_func_t sin_vrsa;
    mkl_sincos_vrsa_func_t sincos_vrsa;
    mkl_sqrt_vrsa_func_t sqrt_vrsa;
    mkl_sub_vrsa_func_t sub_vrsa;
    mkl_subfi_vrsa_func_t subfi_vrsa;
    mkl_tan_vrsa_func_t tan_vrsa;
    mkl_tanh_vrsa_func_t tanh_vrsa;

    // ============================================================================
    // DOUBLE PRECISION ARRAY (vrda) VARIANTS
    // ============================================================================
    mkl_acos_vrda_func_t acos_vrda;
    mkl_add_vrda_func_t add_vrda;
    mkl_addi_vrda_func_t addi_vrda;
    mkl_asin_vrda_func_t asin_vrda;
    mkl_atan_vrda_func_t atan_vrda;
    mkl_cbrt_vrda_func_t cbrt_vrda;
    mkl_cos_vrda_func_t cos_vrda;
    mkl_cosh_vrda_func_t cosh_vrda;
    mkl_div_vrda_func_t div_vrda;
    mkl_divi_vrda_func_t divi_vrda;
    mkl_erf_vrda_func_t erf_vrda;
    mkl_erfc_vrda_func_t erfc_vrda;
    mkl_exp_vrda_func_t exp_vrda;
    mkl_exp10_vrda_func_t exp10_vrda;
    mkl_exp2_vrda_func_t exp2_vrda;
    mkl_expm1_vrda_func_t expm1_vrda;
    mkl_fabs_vrda_func_t fabs_vrda;
    mkl_fmax_vrda_func_t fmax_vrda;
    mkl_fmaxi_vrda_func_t fmaxi_vrda;
    mkl_fmin_vrda_func_t fmin_vrda;
    mkl_fmini_vrda_func_t fmini_vrda;
    mkl_linearfrac_vrda_func_t linearfrac_vrda;
    mkl_log_vrda_func_t log_vrda;
    mkl_log10_vrda_func_t log10_vrda;
    mkl_log1p_vrda_func_t log1p_vrda;
    mkl_log2_vrda_func_t log2_vrda;
    mkl_mul_vrda_func_t mul_vrda;
    mkl_muli_vrda_func_t muli_vrda;
    mkl_pow_vrda_func_t pow_vrda;
    mkl_powx_vrda_func_t powx_vrda;
    mkl_sin_vrda_func_t sin_vrda;
    mkl_sincos_vrda_func_t sincos_vrda;
    mkl_sqrt_vrda_func_t sqrt_vrda;
    mkl_sub_vrda_func_t sub_vrda;
    mkl_subi_vrda_func_t subi_vrda;
    mkl_tan_vrda_func_t tan_vrda;

#ifdef __AVX512F__
    // ============================================================================
    // DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
    // ============================================================================
    mkl_asin_vrd8_func_t asin_vrd8;
    mkl_atan_vrd8_func_t atan_vrd8;
    mkl_cos_vrd8_func_t cos_vrd8;
    mkl_erf_vrd8_func_t erf_vrd8;
    mkl_erfc_vrd8_func_t erfc_vrd8;
    mkl_exp_vrd8_func_t exp_vrd8;
    mkl_exp2_vrd8_func_t exp2_vrd8;
    mkl_linearfrac_vrd8_func_t linearfrac_vrd8;
    mkl_log_vrd8_func_t log_vrd8;
    mkl_log2_vrd8_func_t log2_vrd8;
    mkl_pow_vrd8_func_t pow_vrd8;
    mkl_powx_vrd8_func_t powx_vrd8;
    mkl_sin_vrd8_func_t sin_vrd8;
    mkl_sincos_vrd8_func_t sincos_vrd8;
    mkl_sqrt_vrd8_func_t sqrt_vrd8;
    mkl_tan_vrd8_func_t tan_vrd8;

    // ============================================================================
    // SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
    // ============================================================================
    mkl_acos_vrs16_func_t acos_vrs16;
    mkl_asin_vrs16_func_t asin_vrs16;
    mkl_atan_vrs16_func_t atan_vrs16;
    mkl_cos_vrs16_func_t cos_vrs16;
    mkl_erf_vrs16_func_t erf_vrs16;
    mkl_erfc_vrs16_func_t erfc_vrs16;
    mkl_exp_vrs16_func_t exp_vrs16;
    mkl_exp10_vrs16_func_t exp10_vrs16;
    mkl_exp2_vrs16_func_t exp2_vrs16;
    mkl_linearfrac_vrs16_func_t linearfrac_vrs16;
    mkl_log_vrs16_func_t log_vrs16;
    mkl_log10_vrs16_func_t log10_vrs16;
    mkl_log2_vrs16_func_t log2_vrs16;
    mkl_pow_vrs16_func_t pow_vrs16;
    mkl_powx_vrs16_func_t powx_vrs16;
    mkl_sin_vrs16_func_t sin_vrs16;
    mkl_sincos_vrs16_func_t sincos_vrs16;
    mkl_sqrt_vrs16_func_t sqrt_vrs16;
    mkl_tan_vrs16_func_t tan_vrs16;
    mkl_tanh_vrs16_func_t tanh_vrs16;
#endif

} mkl_funcs = {};


static bool symbols_loaded = false;

template<typename T>
static T load_mkl_symbol(LIB_HANDLE lib, const char* name) {
    if (!lib) {
        printf("Warning: Cannot load symbol '%s' - library handle is null\n", name);
        return nullptr;
    }
    void* symbol = LOAD_SYMBOL(lib, name);
    if (!symbol) {
        printf("Warning: Symbol '%s' not found in library\n", name);
#ifndef _WIN32
        printf("dlsym error: %s\n", dlerror());
#endif
    }
    return reinterpret_cast<T>(symbol);
}

// Automatic initialization at library load time
static void init_mkl_symbols(void) {
    // Run diagnostic check first

    // Try to load Intel MKL libraries in order of preference
    LIB_HANDLE mkl_core = nullptr;
    LIB_HANDLE mkl_vml = nullptr;
    LIB_HANDLE mkl_vma = nullptr;

    //const char* mkl_root = getenv("MKLROOT");
    //const char* path_env = getenv("PATH");
    //char intel_path[512];
#ifdef _WIN32
    // Windows: Load Intel Math Libraries
    // Scalar functions are in libmmd.dll (Intel Math Library)
    mkl_core = LOAD_LIBRARY("libmmd.dll");
    if (!mkl_core) {
        printf("Warning: Please add the MKL library path to the PATH environment variable.\n");
    }
    // Vector functions are in svml_dispmd.dll (Short Vector Math Library)
    mkl_vml = LOAD_LIBRARY("svml_dispmd.dll");
    if (!mkl_vml) {
       printf("Warning: Please add the MKL library path to the PATH environment variable.\n");
    }
    // Use SVML for VMA functions as well
    //mkl_vma = mkl_vml;
    mkl_vml = LOAD_LIBRARY("svml_dispmd.dll");
    if (!mkl_vml) {
       printf("Warning: Please add the MKL library path to the PATH environment variable.\n");
    }
    // Load MKL Runtime for VMA array functions (vsAcos, vdAcos, etc.)
    mkl_vma = LOAD_LIBRARY("mkl_rt.2.dll");
    if (!mkl_vma) {
        printf("Warning: Please add the MKL library path to the PATH environment variable.\n");
    }
#else
// Linux version
    // Linux MKL library loading
    mkl_core = LOAD_LIBRARY("libimf.so");
    if (!mkl_core) {
        printf("Error: Please add the MKL library path to the LD_LIBRARY_PATH environment variable.\n");
    }

    mkl_vml = LOAD_LIBRARY("libsvml.so");
    if (!mkl_vml) {
        printf("Error: Please add the MKL library path to the LD_LIBRARY_PATH environment variable.\n");
    }
    if (!mkl_vml) mkl_vml = mkl_core;

    // Load MKL VMA (Vector Math Array) library - use same as core for most functions
    mkl_vma = LOAD_LIBRARY("libmkl_rt.so");
    if (!mkl_vma) {
        printf("Error: Please add the MKL library path to the LD_LIBRARY_PATH environment variable.\n");
    }
    if (!mkl_vma) mkl_vma = mkl_core; // Final fallback to core library

#endif

    // Fallback to system libraries if MKL not found
    if (!mkl_core) {
        printf("Falling back to system math library\n");
#ifdef _WIN32
        mkl_core = LOAD_LIBRARY("msvcrt.dll");
#else
        mkl_core = RTLD_DEFAULT;
#endif
        mkl_vml = mkl_core;
    }

    // Load symbols organized by variant
    // ============================================================================
    // SINGLE PRECISION SCALAR (ss) VARIANTS
    // ============================================================================
    mkl_funcs.acosf = load_mkl_symbol<mkl_acos_ss_func_t>(mkl_core, "acosf");
    mkl_funcs.acoshf = load_mkl_symbol<mkl_acosh_ss_func_t>(mkl_core, "acoshf");
    mkl_funcs.asinf = load_mkl_symbol<mkl_asin_ss_func_t>(mkl_core, "asinf");
    mkl_funcs.asinhf = load_mkl_symbol<mkl_asinh_ss_func_t>(mkl_core, "asinhf");
    mkl_funcs.atanf = load_mkl_symbol<mkl_atan_ss_func_t>(mkl_core, "atanf");
    mkl_funcs.atan2f = load_mkl_symbol<mkl_atan2_ss_func_t>(mkl_core, "atan2f");
    mkl_funcs.atanhf = load_mkl_symbol<mkl_atanh_ss_func_t>(mkl_core, "atanhf");
    mkl_funcs.cbrtf = load_mkl_symbol<mkl_cbrt_ss_func_t>(mkl_core, "cbrtf");
    mkl_funcs.ceilf = load_mkl_symbol<mkl_ceil_ss_func_t>(mkl_core, "ceilf");
    mkl_funcs.copysignf = load_mkl_symbol<mkl_copysign_ss_func_t>(mkl_core, "copysignf");
    mkl_funcs.cosf = load_mkl_symbol<mkl_cos_ss_func_t>(mkl_core, "cosf");
    mkl_funcs.coshf = load_mkl_symbol<mkl_cosh_ss_func_t>(mkl_core, "coshf");
    mkl_funcs.cospif = load_mkl_symbol<mkl_cospi_ss_func_t>(mkl_core, "cospif");
    mkl_funcs.erff = load_mkl_symbol<mkl_erf_ss_func_t>(mkl_core, "erff");
    mkl_funcs.erfcf = load_mkl_symbol<mkl_erfc_ss_func_t>(mkl_core, "erfcf");
    mkl_funcs.expf = load_mkl_symbol<mkl_exp_ss_func_t>(mkl_core, "expf");
    mkl_funcs.exp10f = load_mkl_symbol<mkl_exp10_ss_func_t>(mkl_core, "exp10f");
    mkl_funcs.exp2f = load_mkl_symbol<mkl_exp2_ss_func_t>(mkl_core, "exp2f");
    mkl_funcs.expm1f = load_mkl_symbol<mkl_expm1_ss_func_t>(mkl_core, "expm1f");
    mkl_funcs.fabsf = load_mkl_symbol<mkl_fabs_ss_func_t>(mkl_vma, "vsAbs");
    mkl_funcs.fdimf = load_mkl_symbol<mkl_fdim_ss_func_t>(mkl_core, "fdimf");
    mkl_funcs.finitef = load_mkl_symbol<mkl_finite_ss_func_t>(mkl_core, "finitef");
    mkl_funcs.floorf = load_mkl_symbol<mkl_floor_ss_func_t>(mkl_core, "floorf");
    mkl_funcs.fmaf = load_mkl_symbol<mkl_fma_ss_func_t>(mkl_core, "fmaf");
    mkl_funcs.fmaxf = load_mkl_symbol<mkl_fmax_ss_func_t>(mkl_core, "fmaxf");
    mkl_funcs.fminf = load_mkl_symbol<mkl_fmin_ss_func_t>(mkl_core, "fminf");
    mkl_funcs.fmodf = load_mkl_symbol<mkl_fmod_ss_func_t>(mkl_core, "fmodf");
    mkl_funcs.frexpf = load_mkl_symbol<mkl_frexp_ss_func_t>(mkl_core, "frexpf");
    mkl_funcs.hypotf = load_mkl_symbol<mkl_hypot_ss_func_t>(mkl_core, "hypotf");
    mkl_funcs.ilogbf = load_mkl_symbol<mkl_ilogb_ss_func_t>(mkl_core, "ilogbf");
    mkl_funcs.ldexpf = load_mkl_symbol<mkl_ldexp_ss_func_t>(mkl_core, "ldexpf");
    mkl_funcs.llrintf = load_mkl_symbol<mkl_llrint_ss_func_t>(mkl_core, "llrintf");
    mkl_funcs.llroundf = load_mkl_symbol<mkl_llround_ss_func_t>(mkl_core, "llroundf");
    mkl_funcs.logf = load_mkl_symbol<mkl_log_ss_func_t>(mkl_core, "logf");
    mkl_funcs.log10f = load_mkl_symbol<mkl_log10_ss_func_t>(mkl_core, "log10f");
    mkl_funcs.log1pf = load_mkl_symbol<mkl_log1p_ss_func_t>(mkl_core, "log1pf");
    mkl_funcs.log2f = load_mkl_symbol<mkl_log2_ss_func_t>(mkl_core, "log2f");
    mkl_funcs.logbf = load_mkl_symbol<mkl_logb_ss_func_t>(mkl_core, "logbf");
    mkl_funcs.lrintf = load_mkl_symbol<mkl_lrint_ss_func_t>(mkl_core, "lrintf");
    mkl_funcs.lroundf = load_mkl_symbol<mkl_lround_ss_func_t>(mkl_core, "lroundf");
    mkl_funcs.modff = load_mkl_symbol<mkl_modf_ss_func_t>(mkl_core, "modff");
    mkl_funcs.nearbyintf = load_mkl_symbol<mkl_nearbyint_ss_func_t>(mkl_core, "nearbyintf");
    mkl_funcs.nextafterf = load_mkl_symbol<mkl_nextafter_ss_func_t>(mkl_core, "nextafterf");
    mkl_funcs.nexttowardf = load_mkl_symbol<mkl_nexttoward_ss_func_t>(mkl_core, "nexttowardf");
    mkl_funcs.powf = load_mkl_symbol<mkl_pow_ss_func_t>(mkl_core, "powf");
    mkl_funcs.remainderf = load_mkl_symbol<mkl_remainder_ss_func_t>(mkl_core, "remainderf");
    mkl_funcs.remquof = load_mkl_symbol<mkl_remquo_ss_func_t>(mkl_core, "remquof");
    mkl_funcs.rintf = load_mkl_symbol<mkl_rint_ss_func_t>(mkl_core, "rintf");
    mkl_funcs.roundf = load_mkl_symbol<mkl_round_ss_func_t>(mkl_core, "roundf");
    mkl_funcs.scalblnf = load_mkl_symbol<mkl_scalbln_ss_func_t>(mkl_core, "scalblnf");
    mkl_funcs.scalbnf = load_mkl_symbol<mkl_scalbn_ss_func_t>(mkl_core, "scalbnf");
    mkl_funcs.sinf = load_mkl_symbol<mkl_sin_ss_func_t>(mkl_core, "sinf");
    mkl_funcs.sincosf = load_mkl_symbol<mkl_sincos_ss_func_t>(mkl_core, "sincosf");
    mkl_funcs.sinhf = load_mkl_symbol<mkl_sinh_ss_func_t>(mkl_core, "sinhf");
    mkl_funcs.sinpif = load_mkl_symbol<mkl_sinpi_ss_func_t>(mkl_core, "sinpif");
    mkl_funcs.sqrtf = load_mkl_symbol<mkl_sqrt_ss_func_t>(mkl_core, "sqrtf");
    mkl_funcs.tanf = load_mkl_symbol<mkl_tan_ss_func_t>(mkl_core, "tanf");
    mkl_funcs.tanhf = load_mkl_symbol<mkl_tanh_ss_func_t>(mkl_core, "tanhf");
    mkl_funcs.tanpif = load_mkl_symbol<mkl_tanpi_ss_func_t>(mkl_core, "tanpif");
    mkl_funcs.truncf = load_mkl_symbol<mkl_trunc_ss_func_t>(mkl_core, "truncf");

    // ============================================================================
    // DOUBLE PRECISION SCALAR (sd) VARIANTS
    // ============================================================================
    mkl_funcs.acos = load_mkl_symbol<mkl_acos_sd_func_t>(mkl_core, "acos");
    mkl_funcs.acosh = load_mkl_symbol<mkl_acosh_sd_func_t>(mkl_core, "acosh");
    mkl_funcs.asin = load_mkl_symbol<mkl_asin_sd_func_t>(mkl_core, "asin");
    mkl_funcs.asinh = load_mkl_symbol<mkl_asinh_sd_func_t>(mkl_core, "asinh");
    mkl_funcs.atan = load_mkl_symbol<mkl_atan_sd_func_t>(mkl_core, "atan");
    mkl_funcs.atan2 = load_mkl_symbol<mkl_atan2_sd_func_t>(mkl_core, "atan2");
    mkl_funcs.atanh = load_mkl_symbol<mkl_atanh_sd_func_t>(mkl_core, "atanh");
    mkl_funcs.cbrt = load_mkl_symbol<mkl_cbrt_sd_func_t>(mkl_core, "cbrt");
    mkl_funcs.ceil = load_mkl_symbol<mkl_ceil_sd_func_t>(mkl_core, "ceil");
    mkl_funcs.copysign = load_mkl_symbol<mkl_copysign_sd_func_t>(mkl_core, "copysign");
    mkl_funcs.cos = load_mkl_symbol<mkl_cos_sd_func_t>(mkl_core, "cos");
    mkl_funcs.cosh = load_mkl_symbol<mkl_cosh_sd_func_t>(mkl_core, "cosh");
    mkl_funcs.cospi = load_mkl_symbol<mkl_cospi_sd_func_t>(mkl_core, "cospi");
    mkl_funcs.erf = load_mkl_symbol<mkl_erf_sd_func_t>(mkl_core, "erf");
    mkl_funcs.erfc = load_mkl_symbol<mkl_erfc_sd_func_t>(mkl_core, "erfc");
    mkl_funcs.exp = load_mkl_symbol<mkl_exp_sd_func_t>(mkl_core, "exp");
    mkl_funcs.exp10 = load_mkl_symbol<mkl_exp10_sd_func_t>(mkl_core, "exp10");
    mkl_funcs.exp2 = load_mkl_symbol<mkl_exp2_sd_func_t>(mkl_core, "exp2");
    mkl_funcs.expm1 = load_mkl_symbol<mkl_expm1_sd_func_t>(mkl_core, "expm1");
    mkl_funcs.fabs = load_mkl_symbol<mkl_fabs_sd_func_t>(mkl_vma, "vdAbs");
    mkl_funcs.fdim = load_mkl_symbol<mkl_fdim_sd_func_t>(mkl_core, "fdim");
    mkl_funcs.finite = load_mkl_symbol<mkl_finite_sd_func_t>(mkl_core, "finite");
    mkl_funcs.floor = load_mkl_symbol<mkl_floor_sd_func_t>(mkl_core, "floor");
    mkl_funcs.fma = load_mkl_symbol<mkl_fma_sd_func_t>(mkl_core, "fma");
    mkl_funcs.fmax = load_mkl_symbol<mkl_fmax_sd_func_t>(mkl_core, "fmax");
    mkl_funcs.fmin = load_mkl_symbol<mkl_fmin_sd_func_t>(mkl_core, "fmin");
    mkl_funcs.fmod = load_mkl_symbol<mkl_fmod_sd_func_t>(mkl_core, "fmod");
    mkl_funcs.frexp = load_mkl_symbol<mkl_frexp_sd_func_t>(mkl_core, "frexp");
    mkl_funcs.hypot = load_mkl_symbol<mkl_hypot_sd_func_t>(mkl_core, "hypot");
    mkl_funcs.ilogb = load_mkl_symbol<mkl_ilogb_sd_func_t>(mkl_core, "ilogb");
    mkl_funcs.ldexp = load_mkl_symbol<mkl_ldexp_sd_func_t>(mkl_core, "ldexp");
    mkl_funcs.llrint = load_mkl_symbol<mkl_llrint_sd_func_t>(mkl_core, "llrint");
    mkl_funcs.llround = load_mkl_symbol<mkl_llround_sd_func_t>(mkl_core, "llround");
    mkl_funcs.log = load_mkl_symbol<mkl_log_sd_func_t>(mkl_core, "log");
    mkl_funcs.log10 = load_mkl_symbol<mkl_log10_sd_func_t>(mkl_core, "log10");
    mkl_funcs.log1p = load_mkl_symbol<mkl_log1p_sd_func_t>(mkl_core, "log1p");
    mkl_funcs.log2 = load_mkl_symbol<mkl_log2_sd_func_t>(mkl_core, "log2");
    mkl_funcs.logb = load_mkl_symbol<mkl_logb_sd_func_t>(mkl_core, "logb");
    mkl_funcs.lrint = load_mkl_symbol<mkl_lrint_sd_func_t>(mkl_core, "lrint");
    mkl_funcs.lround = load_mkl_symbol<mkl_lround_sd_func_t>(mkl_core, "lround");
    mkl_funcs.modf = load_mkl_symbol<mkl_modf_sd_func_t>(mkl_core, "modf");
    mkl_funcs.nearbyint = load_mkl_symbol<mkl_nearbyint_sd_func_t>(mkl_core, "nearbyint");
    mkl_funcs.nextafter = load_mkl_symbol<mkl_nextafter_sd_func_t>(mkl_core, "nextafter");
    mkl_funcs.nexttoward = load_mkl_symbol<mkl_nexttoward_sd_func_t>(mkl_core, "nexttoward");
    mkl_funcs.pow = load_mkl_symbol<mkl_pow_sd_func_t>(mkl_core, "pow");
    mkl_funcs.remainder = load_mkl_symbol<mkl_remainder_sd_func_t>(mkl_core, "remainder");
    mkl_funcs.remquo = load_mkl_symbol<mkl_remquo_sd_func_t>(mkl_core, "remquo");
    mkl_funcs.rint = load_mkl_symbol<mkl_rint_sd_func_t>(mkl_core, "rint");
    mkl_funcs.round = load_mkl_symbol<mkl_round_sd_func_t>(mkl_core, "round");
    mkl_funcs.scalbln = load_mkl_symbol<mkl_scalbln_sd_func_t>(mkl_core, "scalbln");
    mkl_funcs.scalbn = load_mkl_symbol<mkl_scalbn_sd_func_t>(mkl_core, "scalbn");
    mkl_funcs.sin = load_mkl_symbol<mkl_sin_sd_func_t>(mkl_core, "sin");
    mkl_funcs.sincos = load_mkl_symbol<mkl_sincos_sd_func_t>(mkl_core, "sincos");
    mkl_funcs.sinh = load_mkl_symbol<mkl_sinh_sd_func_t>(mkl_core, "sinh");
    mkl_funcs.sinpi = load_mkl_symbol<mkl_sinpi_sd_func_t>(mkl_core, "sinpi");
    mkl_funcs.sqrt = load_mkl_symbol<mkl_sqrt_sd_func_t>(mkl_core, "sqrt");
    mkl_funcs.tan = load_mkl_symbol<mkl_tan_sd_func_t>(mkl_core, "tan");
    mkl_funcs.tanh = load_mkl_symbol<mkl_tanh_sd_func_t>(mkl_core, "tanh");
    mkl_funcs.tanpi = load_mkl_symbol<mkl_tanpi_sd_func_t>(mkl_core, "tanpi");
    mkl_funcs.trunc = load_mkl_symbol<mkl_trunc_sd_func_t>(mkl_core, "trunc");

    // ============================================================================
    // DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
    // ============================================================================
    mkl_funcs.acos_vrd2 = load_mkl_symbol<mkl_acos_vrd2_func_t>(mkl_vml, "__svml_acos2");
    mkl_funcs.asin_vrd2 = load_mkl_symbol<mkl_asin_vrd2_func_t>(mkl_vml, "__svml_asin2");
    mkl_funcs.atan_vrd2 = load_mkl_symbol<mkl_atan_vrd2_func_t>(mkl_vml, "__svml_atan2");
    mkl_funcs.cbrt_vrd2 = load_mkl_symbol<mkl_cbrt_vrd2_func_t>(mkl_vml, "__svml_cbrt2");
    mkl_funcs.cos_vrd2 = load_mkl_symbol<mkl_cos_vrd2_func_t>(mkl_vml, "__svml_cos2");
    mkl_funcs.cosh_vrd2 = load_mkl_symbol<mkl_cosh_vrd2_func_t>(mkl_vml, "__svml_cosh2");
    mkl_funcs.erf_vrd2 = load_mkl_symbol<mkl_erf_vrd2_func_t>(mkl_vml, "__svml_erf2");
    mkl_funcs.erfc_vrd2 = load_mkl_symbol<mkl_erfc_vrd2_func_t>(mkl_vml, "__svml_erfc2");
    mkl_funcs.exp_vrd2 = load_mkl_symbol<mkl_exp_vrd2_func_t>(mkl_vml, "__svml_exp2");
    mkl_funcs.exp10_vrd2 = load_mkl_symbol<mkl_exp10_vrd2_func_t>(mkl_vml, "__svml_exp102");
    mkl_funcs.exp2_vrd2 = load_mkl_symbol<mkl_exp2_vrd2_func_t>(mkl_vml, "__svml_exp22");
    mkl_funcs.fabs_vrd2 = load_mkl_symbol<mkl_fabs_vrd2_func_t>(mkl_vma, "vdAbs");
    mkl_funcs.linearfrac_vrd2 = load_mkl_symbol<mkl_linearfrac_vrd2_func_t>(mkl_vma, "vdLinearFrac");
    mkl_funcs.log_vrd2 = load_mkl_symbol<mkl_log_vrd2_func_t>(mkl_vml, "__svml_log2");
    mkl_funcs.log10_vrd2 = load_mkl_symbol<mkl_log10_vrd2_func_t>(mkl_vml, "__svml_log102");
    mkl_funcs.log1p_vrd2 = load_mkl_symbol<mkl_log1p_vrd2_func_t>(mkl_vml, "__svml_log1p2");
    mkl_funcs.log2_vrd2 = load_mkl_symbol<mkl_log2_vrd2_func_t>(mkl_vml, "__svml_log22");
    mkl_funcs.pow_vrd2 = load_mkl_symbol<mkl_pow_vrd2_func_t>(mkl_vml, "__svml_pow2");
    mkl_funcs.powx_vrd2 = load_mkl_symbol<mkl_powx_vrd2_func_t>(mkl_vma, "vdPowx");
    mkl_funcs.sin_vrd2 = load_mkl_symbol<mkl_sin_vrd2_func_t>(mkl_vml, "__svml_sin2");
    mkl_funcs.sincos_vrd2 = load_mkl_symbol<mkl_sincos_vrd2_func_t>(mkl_vml, "__svml_sincos2");
    mkl_funcs.sqrt_vrd2 = load_mkl_symbol<mkl_sqrt_vrd2_func_t>(mkl_vml, "__svml_sqrt2");
    mkl_funcs.tan_vrd2 = load_mkl_symbol<mkl_tan_vrd2_func_t>(mkl_vml, "__svml_tan2");

    // ============================================================================
    // SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
    // ============================================================================
    mkl_funcs.acos_vrs4 = load_mkl_symbol<mkl_acos_vrs4_func_t>(mkl_vml, "__svml_acosf4");
    mkl_funcs.asin_vrs4 = load_mkl_symbol<mkl_asin_vrs4_func_t>(mkl_vml, "__svml_asinf4");
    mkl_funcs.atan_vrs4 = load_mkl_symbol<mkl_atan_vrs4_func_t>(mkl_vml, "__svml_atanf4");
    mkl_funcs.cbrt_vrs4 = load_mkl_symbol<mkl_cbrt_vrs4_func_t>(mkl_vml, "__svml_cbrtf4");
    mkl_funcs.cos_vrs4 = load_mkl_symbol<mkl_cos_vrs4_func_t>(mkl_vml, "__svml_cosf4");
    mkl_funcs.cosh_vrs4 = load_mkl_symbol<mkl_cosh_vrs4_func_t>(mkl_vml, "__svml_coshf4");
    mkl_funcs.erf_vrs4 = load_mkl_symbol<mkl_erf_vrs4_func_t>(mkl_vml, "__svml_erff4");
    mkl_funcs.erfc_vrs4 = load_mkl_symbol<mkl_erfc_vrs4_func_t>(mkl_vml, "__svml_erfcf4");
    mkl_funcs.exp_vrs4 = load_mkl_symbol<mkl_exp_vrs4_func_t>(mkl_vml, "__svml_expf4");
    mkl_funcs.exp10_vrs4 = load_mkl_symbol<mkl_exp10_vrs4_func_t>(mkl_vml, "__svml_exp10f4");
    mkl_funcs.exp2_vrs4 = load_mkl_symbol<mkl_exp2_vrs4_func_t>(mkl_vml, "__svml_exp2f4");
    mkl_funcs.expm1_vrs4 = load_mkl_symbol<mkl_expm1_vrs4_func_t>(mkl_vml, "__svml_expm1f4");
    mkl_funcs.fabs_vrs4 = load_mkl_symbol<mkl_fabs_vrs4_func_t>(mkl_vma, "vsAbs");
    mkl_funcs.linearfrac_vrs4 = load_mkl_symbol<mkl_linearfrac_vrs4_func_t>(mkl_vma, "vsLinearFrac");
    mkl_funcs.log_vrs4 = load_mkl_symbol<mkl_log_vrs4_func_t>(mkl_vml, "__svml_logf4");
    mkl_funcs.log10_vrs4 = load_mkl_symbol<mkl_log10_vrs4_func_t>(mkl_vml, "__svml_log10f4");
    mkl_funcs.log1p_vrs4 = load_mkl_symbol<mkl_log1p_vrs4_func_t>(mkl_vml, "__svml_log1pf4");
    mkl_funcs.log2_vrs4 = load_mkl_symbol<mkl_log2_vrs4_func_t>(mkl_vml, "__svml_log2f4");
    mkl_funcs.pow_vrs4 = load_mkl_symbol<mkl_pow_vrs4_func_t>(mkl_vml, "__svml_powf4");
    mkl_funcs.powx_vrs4 = load_mkl_symbol<mkl_powx_vrs4_func_t>(mkl_vma, "vsPowx");
    mkl_funcs.sin_vrs4 = load_mkl_symbol<mkl_sin_vrs4_func_t>(mkl_vml, "__svml_sinf4");
    mkl_funcs.sincos_vrs4 = load_mkl_symbol<mkl_sincos_vrs4_func_t>(mkl_vml, "__svml_sincosf4");
    mkl_funcs.sqrt_vrs4 = load_mkl_symbol<mkl_sqrt_vrs4_func_t>(mkl_vml, "__svml_sqrtf4");
    mkl_funcs.tan_vrs4 = load_mkl_symbol<mkl_tan_vrs4_func_t>(mkl_vml, "__svml_tanf4");
    mkl_funcs.tanh_vrs4 = load_mkl_symbol<mkl_tanh_vrs4_func_t>(mkl_vml, "__svml_tanhf4");

    // ============================================================================
    // DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
    // ============================================================================
    mkl_funcs.acos_vrd4 = load_mkl_symbol<mkl_acos_vrd4_func_t>(mkl_vml, "__svml_acos4");
    mkl_funcs.asin_vrd4 = load_mkl_symbol<mkl_asin_vrd4_func_t>(mkl_vml, "__svml_asin4");
    mkl_funcs.atan_vrd4 = load_mkl_symbol<mkl_atan_vrd4_func_t>(mkl_vml, "__svml_atan4");
    mkl_funcs.cos_vrd4 = load_mkl_symbol<mkl_cos_vrd4_func_t>(mkl_vml, "__svml_cos4");
    mkl_funcs.erf_vrd4 = load_mkl_symbol<mkl_erf_vrd4_func_t>(mkl_vml, "__svml_erf4");
    mkl_funcs.erfc_vrd4 = load_mkl_symbol<mkl_erfc_vrd4_func_t>(mkl_vml, "__svml_erfc4");
    mkl_funcs.exp_vrd4 = load_mkl_symbol<mkl_exp_vrd4_func_t>(mkl_vml, "__svml_exp4");
    mkl_funcs.exp2_vrd4 = load_mkl_symbol<mkl_exp2_vrd4_func_t>(mkl_vml, "__svml_exp24");
    mkl_funcs.fabs_vrd4 = load_mkl_symbol<mkl_fabs_vrd4_func_t>(mkl_vma, "vdAbs");
    mkl_funcs.linearfrac_vrd4 = load_mkl_symbol<mkl_linearfrac_vrd4_func_t>(mkl_vma, "vdLinearFrac");
    mkl_funcs.log_vrd4 = load_mkl_symbol<mkl_log_vrd4_func_t>(mkl_vml, "__svml_log4");
    mkl_funcs.log2_vrd4 = load_mkl_symbol<mkl_log2_vrd4_func_t>(mkl_vml, "__svml_log24");
    mkl_funcs.pow_vrd4 = load_mkl_symbol<mkl_pow_vrd4_func_t>(mkl_vml, "__svml_pow4");
    mkl_funcs.powx_vrd4 = load_mkl_symbol<mkl_powx_vrd4_func_t>(mkl_vma, "vdPowx");
    mkl_funcs.sin_vrd4 = load_mkl_symbol<mkl_sin_vrd4_func_t>(mkl_vml, "__svml_sin4");
    mkl_funcs.sincos_vrd4 = load_mkl_symbol<mkl_sincos_vrd4_func_t>(mkl_vml, "__svml_sincos4");
    mkl_funcs.sqrt_vrd4 = load_mkl_symbol<mkl_sqrt_vrd4_func_t>(mkl_vml, "__svml_sqrt4");
    mkl_funcs.tan_vrd4 = load_mkl_symbol<mkl_tan_vrd4_func_t>(mkl_vml, "__svml_tan4");

    // ============================================================================
    // SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
    // ============================================================================
    mkl_funcs.acos_vrs8 = load_mkl_symbol<mkl_acos_vrs8_func_t>(mkl_vml, "__svml_acosf8");
    mkl_funcs.asin_vrs8 = load_mkl_symbol<mkl_asin_vrs8_func_t>(mkl_vml, "__svml_asinf8");
    mkl_funcs.atan_vrs8 = load_mkl_symbol<mkl_atan_vrs8_func_t>(mkl_vml, "__svml_atanf8");
    mkl_funcs.cos_vrs8 = load_mkl_symbol<mkl_cos_vrs8_func_t>(mkl_vml, "__svml_cosf8");
    mkl_funcs.cosh_vrs8 = load_mkl_symbol<mkl_cosh_vrs8_func_t>(mkl_vml, "__svml_coshf8");
    mkl_funcs.erf_vrs8 = load_mkl_symbol<mkl_erf_vrs8_func_t>(mkl_vml, "__svml_erff8");
    mkl_funcs.erfc_vrs8 = load_mkl_symbol<mkl_erfc_vrs8_func_t>(mkl_vml, "__svml_erfcf8");
    mkl_funcs.exp_vrs8 = load_mkl_symbol<mkl_exp_vrs8_func_t>(mkl_vml, "__svml_expf8");
    mkl_funcs.exp2_vrs8 = load_mkl_symbol<mkl_exp2_vrs8_func_t>(mkl_vml, "__svml_exp2f8");
    mkl_funcs.fabs_vrs8 = load_mkl_symbol<mkl_fabs_vrs8_func_t>(mkl_vma, "vsAbs");
    mkl_funcs.linearfrac_vrs8 = load_mkl_symbol<mkl_linearfrac_vrs8_func_t>(mkl_vma, "vsLinearFrac");
    mkl_funcs.log_vrs8 = load_mkl_symbol<mkl_log_vrs8_func_t>(mkl_vml, "__svml_logf8");
    mkl_funcs.log10_vrs8 = load_mkl_symbol<mkl_log10_vrs8_func_t>(mkl_vml, "__svml_log10f8");
    mkl_funcs.log2_vrs8 = load_mkl_symbol<mkl_log2_vrs8_func_t>(mkl_vml, "__svml_log2f8");
    mkl_funcs.pow_vrs8 = load_mkl_symbol<mkl_pow_vrs8_func_t>(mkl_vml, "__svml_powf8");
    mkl_funcs.powx_vrs8 = load_mkl_symbol<mkl_powx_vrs8_func_t>(mkl_vma, "vsPowx");
    mkl_funcs.sin_vrs8 = load_mkl_symbol<mkl_sin_vrs8_func_t>(mkl_vml, "__svml_sinf8");
    mkl_funcs.sincos_vrs8 = load_mkl_symbol<mkl_sincos_vrs8_func_t>(mkl_vml, "__svml_sincosf8");
    mkl_funcs.sqrt_vrs8 = load_mkl_symbol<mkl_sqrt_vrs8_func_t>(mkl_vml, "__svml_sqrtf8");
    mkl_funcs.tan_vrs8 = load_mkl_symbol<mkl_tan_vrs8_func_t>(mkl_vml, "__svml_tanf8");
    mkl_funcs.tanh_vrs8 = load_mkl_symbol<mkl_tanh_vrs8_func_t>(mkl_vml, "__svml_tanhf8");

    // ============================================================================
    // SINGLE PRECISION ARRAY (vrsa) VARIANTS - MKL VML Array Functions
    // ============================================================================
    mkl_funcs.acos_vrsa = load_mkl_symbol<mkl_acos_vrsa_func_t>(mkl_vma, "vsAcos");
    mkl_funcs.add_vrsa = load_mkl_symbol<mkl_add_vrsa_func_t>(mkl_vma, "vsAdd");
    mkl_funcs.addfi_vrsa = load_mkl_symbol<mkl_addfi_vrsa_func_t>(mkl_vma, "vsAdd"); // No separate _fi function in VML
    mkl_funcs.asin_vrsa = load_mkl_symbol<mkl_asin_vrsa_func_t>(mkl_vma, "vsAsin");
    mkl_funcs.atan_vrsa = load_mkl_symbol<mkl_atan_vrsa_func_t>(mkl_vma, "vsAtan");
    mkl_funcs.cbrt_vrsa = load_mkl_symbol<mkl_cbrt_vrsa_func_t>(mkl_vma, "vsCbrt");
    mkl_funcs.cos_vrsa = load_mkl_symbol<mkl_cos_vrsa_func_t>(mkl_vma, "vsCos");
    mkl_funcs.cosh_vrsa = load_mkl_symbol<mkl_cosh_vrsa_func_t>(mkl_vma, "vsCosh");
    mkl_funcs.div_vrsa = load_mkl_symbol<mkl_div_vrsa_func_t>(mkl_vma, "vsDiv");
    mkl_funcs.divfi_vrsa = load_mkl_symbol<mkl_divfi_vrsa_func_t>(mkl_vma, "vsDiv"); // No separate _fi function in VML
    mkl_funcs.erf_vrsa = load_mkl_symbol<mkl_erf_vrsa_func_t>(mkl_vma, "vsErf");
    mkl_funcs.erfc_vrsa = load_mkl_symbol<mkl_erfc_vrsa_func_t>(mkl_vma, "vsErfc");
    mkl_funcs.exp_vrsa = load_mkl_symbol<mkl_exp_vrsa_func_t>(mkl_vma, "vsExp");
    mkl_funcs.exp10_vrsa = load_mkl_symbol<mkl_exp10_vrsa_func_t>(mkl_vma, "vsExp10");
    mkl_funcs.exp2_vrsa = load_mkl_symbol<mkl_exp2_vrsa_func_t>(mkl_vma, "vsExp2");
    mkl_funcs.expm1_vrsa = load_mkl_symbol<mkl_expm1_vrsa_func_t>(mkl_vma, "vsExpm1");
    mkl_funcs.fabs_vrsa = load_mkl_symbol<mkl_fabs_vrsa_func_t>(mkl_vma, "vsAbs");
    mkl_funcs.fmax_vrsa = load_mkl_symbol<mkl_fmax_vrsa_func_t>(mkl_vma, "vsFmax");
    mkl_funcs.fmaxfi_vrsa = load_mkl_symbol<mkl_fmaxfi_vrsa_func_t>(mkl_vma, "vsFmax"); // No separate _fi function in VML
    mkl_funcs.fmin_vrsa = load_mkl_symbol<mkl_fmin_vrsa_func_t>(mkl_vma, "vsFmin");
    mkl_funcs.fminfi_vrsa = load_mkl_symbol<mkl_fminfi_vrsa_func_t>(mkl_vma, "vsFmin"); // No separate _fi function in VML
    mkl_funcs.linearfrac_vrsa = load_mkl_symbol<mkl_linearfrac_vrsa_func_t>(mkl_vma, "vsLinearFrac");
    mkl_funcs.log_vrsa = load_mkl_symbol<mkl_log_vrsa_func_t>(mkl_vma, "vsLn");
    mkl_funcs.log10_vrsa = load_mkl_symbol<mkl_log10_vrsa_func_t>(mkl_vma, "vsLog10");
    mkl_funcs.log1p_vrsa = load_mkl_symbol<mkl_log1p_vrsa_func_t>(mkl_vma, "vsLog1p");
    mkl_funcs.log2_vrsa = load_mkl_symbol<mkl_log2_vrsa_func_t>(mkl_vma, "vsLog2");
    mkl_funcs.mul_vrsa = load_mkl_symbol<mkl_mul_vrsa_func_t>(mkl_vma, "vsMul");
    mkl_funcs.mulfi_vrsa = load_mkl_symbol<mkl_mulfi_vrsa_func_t>(mkl_vma, "vsMul"); // No separate _fi function in VML
    mkl_funcs.pow_vrsa = load_mkl_symbol<mkl_pow_vrsa_func_t>(mkl_vma, "vsPow");
    mkl_funcs.powx_vrsa = load_mkl_symbol<mkl_powx_vrsa_func_t>(mkl_vma, "vsPowx");
    mkl_funcs.sin_vrsa = load_mkl_symbol<mkl_sin_vrsa_func_t>(mkl_vma, "vsSin");
    mkl_funcs.sincos_vrsa = load_mkl_symbol<mkl_sincos_vrsa_func_t>(mkl_vma, "vsSinCos");
    mkl_funcs.sqrt_vrsa = load_mkl_symbol<mkl_sqrt_vrsa_func_t>(mkl_vma, "vsSqrt");
    mkl_funcs.sub_vrsa = load_mkl_symbol<mkl_sub_vrsa_func_t>(mkl_vma, "vsSub");
    mkl_funcs.subfi_vrsa = load_mkl_symbol<mkl_subfi_vrsa_func_t>(mkl_vma, "vsSub"); // No separate _fi function in VML
    mkl_funcs.tan_vrsa = load_mkl_symbol<mkl_tan_vrsa_func_t>(mkl_vma, "vsTan");
    mkl_funcs.tanh_vrsa = load_mkl_symbol<mkl_tanh_vrsa_func_t>(mkl_vma, "vsTanh");

    // ============================================================================
    // DOUBLE PRECISION ARRAY (vrda) VARIANTS - MKL VML Double Array Functions
    // ============================================================================
    mkl_funcs.acos_vrda = load_mkl_symbol<mkl_acos_vrda_func_t>(mkl_vma, "vdAcos");
    mkl_funcs.add_vrda = load_mkl_symbol<mkl_add_vrda_func_t>(mkl_vma, "vdAdd");
    mkl_funcs.addi_vrda = load_mkl_symbol<mkl_addi_vrda_func_t>(mkl_vma, "vdAdd"); // No separate _i function in VML
    mkl_funcs.asin_vrda = load_mkl_symbol<mkl_asin_vrda_func_t>(mkl_vma, "vdAsin");
    mkl_funcs.atan_vrda = load_mkl_symbol<mkl_atan_vrda_func_t>(mkl_vma, "vdAtan");
    mkl_funcs.cbrt_vrda = load_mkl_symbol<mkl_cbrt_vrda_func_t>(mkl_vma, "vdCbrt");
    mkl_funcs.cos_vrda = load_mkl_symbol<mkl_cos_vrda_func_t>(mkl_vma, "vdCos");
    mkl_funcs.cosh_vrda = load_mkl_symbol<mkl_cosh_vrda_func_t>(mkl_vma, "vdCosh");
    mkl_funcs.div_vrda = load_mkl_symbol<mkl_div_vrda_func_t>(mkl_vma, "vdDiv");
    mkl_funcs.divi_vrda = load_mkl_symbol<mkl_divi_vrda_func_t>(mkl_vma, "vdDiv"); // No separate _i function in VML
    mkl_funcs.erf_vrda = load_mkl_symbol<mkl_erf_vrda_func_t>(mkl_vma, "vdErf");
    mkl_funcs.erfc_vrda = load_mkl_symbol<mkl_erfc_vrda_func_t>(mkl_vma, "vdErfc");
    mkl_funcs.exp_vrda = load_mkl_symbol<mkl_exp_vrda_func_t>(mkl_vma, "vdExp");
    mkl_funcs.exp10_vrda = load_mkl_symbol<mkl_exp10_vrda_func_t>(mkl_vma, "vdExp10");
    mkl_funcs.exp2_vrda = load_mkl_symbol<mkl_exp2_vrda_func_t>(mkl_vma, "vdExp2");
    mkl_funcs.expm1_vrda = load_mkl_symbol<mkl_expm1_vrda_func_t>(mkl_vma, "vdExpm1");
    mkl_funcs.fabs_vrda = load_mkl_symbol<mkl_fabs_vrda_func_t>(mkl_vma, "vdAbs");
    mkl_funcs.fmax_vrda = load_mkl_symbol<mkl_fmax_vrda_func_t>(mkl_vma, "vdFmax");
    mkl_funcs.fmaxi_vrda = load_mkl_symbol<mkl_fmaxi_vrda_func_t>(mkl_vma, "vdFmax"); // No separate _i function in VML
    mkl_funcs.fmin_vrda = load_mkl_symbol<mkl_fmin_vrda_func_t>(mkl_vma, "vdFmin");
    mkl_funcs.fmini_vrda = load_mkl_symbol<mkl_fmini_vrda_func_t>(mkl_vma, "vdFmin"); // No separate _i function in VML
    mkl_funcs.linearfrac_vrda = load_mkl_symbol<mkl_linearfrac_vrda_func_t>(mkl_vma, "vdLinearFrac");
    mkl_funcs.log_vrda = load_mkl_symbol<mkl_log_vrda_func_t>(mkl_vma, "vdLn");
    mkl_funcs.log10_vrda = load_mkl_symbol<mkl_log10_vrda_func_t>(mkl_vma, "vdLog10");
    mkl_funcs.log1p_vrda = load_mkl_symbol<mkl_log1p_vrda_func_t>(mkl_vma, "vdLog1p");
    mkl_funcs.log2_vrda = load_mkl_symbol<mkl_log2_vrda_func_t>(mkl_vma, "vdLog2");
    mkl_funcs.mul_vrda = load_mkl_symbol<mkl_mul_vrda_func_t>(mkl_vma, "vdMul");
    mkl_funcs.muli_vrda = load_mkl_symbol<mkl_muli_vrda_func_t>(mkl_vma, "vdMul"); // No separate _i function in VML
    mkl_funcs.pow_vrda = load_mkl_symbol<mkl_pow_vrda_func_t>(mkl_vma, "vdPow");
    mkl_funcs.powx_vrda = load_mkl_symbol<mkl_powx_vrda_func_t>(mkl_vma, "vdPowx");
    mkl_funcs.sin_vrda = load_mkl_symbol<mkl_sin_vrda_func_t>(mkl_vma, "vdSin");
    mkl_funcs.sincos_vrda = load_mkl_symbol<mkl_sincos_vrda_func_t>(mkl_vma, "vdSinCos");
    mkl_funcs.sqrt_vrda = load_mkl_symbol<mkl_sqrt_vrda_func_t>(mkl_vma, "vdSqrt");
    mkl_funcs.sub_vrda = load_mkl_symbol<mkl_sub_vrda_func_t>(mkl_vma, "vdSub");
    mkl_funcs.subi_vrda = load_mkl_symbol<mkl_subi_vrda_func_t>(mkl_vma, "vdSub"); // No separate _i function in VML
    mkl_funcs.tan_vrda = load_mkl_symbol<mkl_tan_vrda_func_t>(mkl_vma, "vdTan");

#ifdef __AVX512F__
    // ============================================================================
    // DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
    // ============================================================================
    mkl_funcs.exp_vrd8 = load_mkl_symbol<mkl_exp_vrd8_func_t>(mkl_vml, "__svml_exp8");
    mkl_funcs.sin_vrd8 = load_mkl_symbol<mkl_sin_vrd8_func_t>(mkl_vml, "__svml_sin8");
    mkl_funcs.cos_vrd8 = load_mkl_symbol<mkl_cos_vrd8_func_t>(mkl_vml, "__svml_cos8");
    mkl_funcs.tan_vrd8 = load_mkl_symbol<mkl_tan_vrd8_func_t>(mkl_vml, "__svml_tan8");
    mkl_funcs.pow_vrd8 = load_mkl_symbol<mkl_pow_vrd8_func_t>(mkl_vml, "__svml_pow8");
    mkl_funcs.powx_vrd8 = load_mkl_symbol<mkl_powx_vrd8_func_t>(mkl_vma, "vdPowx");
    mkl_funcs.sincos_vrd8 = load_mkl_symbol<mkl_sincos_vrd8_func_t>(mkl_vml, "__svml_sincos8");
    mkl_funcs.sqrt_vrd8 = load_mkl_symbol<mkl_sqrt_vrd8_func_t>(mkl_vml, "__svml_sqrt8");
    mkl_funcs.linearfrac_vrd8 = load_mkl_symbol<mkl_linearfrac_vrd8_func_t>(mkl_vma, "vdLinearFrac");
    mkl_funcs.asin_vrd8 = load_mkl_symbol<mkl_asin_vrd8_func_t>(mkl_vml, "__svml_asin8");
    mkl_funcs.atan_vrd8 = load_mkl_symbol<mkl_atan_vrd8_func_t>(mkl_vml, "__svml_atan8");
    mkl_funcs.erf_vrd8 = load_mkl_symbol<mkl_erf_vrd8_func_t>(mkl_vml, "__svml_erf8");
    mkl_funcs.erfc_vrd8 = load_mkl_symbol<mkl_erfc_vrd8_func_t>(mkl_vml, "__svml_erfc8");
    mkl_funcs.exp2_vrd8 = load_mkl_symbol<mkl_exp2_vrd8_func_t>(mkl_vml, "__svml_exp28");
    mkl_funcs.log_vrd8 = load_mkl_symbol<mkl_log_vrd8_func_t>(mkl_vml, "__svml_log8");
    mkl_funcs.log2_vrd8 = load_mkl_symbol<mkl_log2_vrd8_func_t>(mkl_vml, "__svml_log28");

    // ============================================================================
    // SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
    // ============================================================================
    mkl_funcs.exp_vrs16 = load_mkl_symbol<mkl_exp_vrs16_func_t>(mkl_vml, "__svml_expf16");
    mkl_funcs.sin_vrs16 = load_mkl_symbol<mkl_sin_vrs16_func_t>(mkl_vml, "__svml_sinf16");
    mkl_funcs.cos_vrs16 = load_mkl_symbol<mkl_cos_vrs16_func_t>(mkl_vml, "__svml_cosf16");
    mkl_funcs.tan_vrs16 = load_mkl_symbol<mkl_tan_vrs16_func_t>(mkl_vml, "__svml_tanf16");
    mkl_funcs.tanh_vrs16 = load_mkl_symbol<mkl_tanh_vrs16_func_t>(mkl_vml, "__svml_tanhf16");
    mkl_funcs.pow_vrs16 = load_mkl_symbol<mkl_pow_vrs16_func_t>(mkl_vml, "__svml_powf16");
    mkl_funcs.powx_vrs16 = load_mkl_symbol<mkl_powx_vrs16_func_t>(mkl_vma, "vsPowx");
    mkl_funcs.sincos_vrs16 = load_mkl_symbol<mkl_sincos_vrs16_func_t>(mkl_vml, "__svml_sincosf16");
    mkl_funcs.sqrt_vrs16 = load_mkl_symbol<mkl_sqrt_vrs16_func_t>(mkl_vml, "__svml_sqrtf16");
    mkl_funcs.linearfrac_vrs16 = load_mkl_symbol<mkl_linearfrac_vrs16_func_t>(mkl_vma, "vsLinearFrac");
    mkl_funcs.acos_vrs16 = load_mkl_symbol<mkl_acos_vrs16_func_t>(mkl_vml, "__svml_acosf16");
    mkl_funcs.asin_vrs16 = load_mkl_symbol<mkl_asin_vrs16_func_t>(mkl_vml, "__svml_asinf16");
    mkl_funcs.atan_vrs16 = load_mkl_symbol<mkl_atan_vrs16_func_t>(mkl_vml, "__svml_atanf16");
    mkl_funcs.erf_vrs16 = load_mkl_symbol<mkl_erf_vrs16_func_t>(mkl_vml, "__svml_erff16");
    mkl_funcs.erfc_vrs16 = load_mkl_symbol<mkl_erfc_vrs16_func_t>(mkl_vml, "__svml_erfcf16");
    mkl_funcs.exp10_vrs16 = load_mkl_symbol<mkl_exp10_vrs16_func_t>(mkl_vml, "__svml_exp10f16");
    mkl_funcs.exp2_vrs16 = load_mkl_symbol<mkl_exp2_vrs16_func_t>(mkl_vml, "__svml_exp2f16");
    mkl_funcs.log_vrs16 = load_mkl_symbol<mkl_log_vrs16_func_t>(mkl_vml, "__svml_logf16");
    mkl_funcs.log10_vrs16 = load_mkl_symbol<mkl_log10_vrs16_func_t>(mkl_vml, "__svml_log10f16");
    mkl_funcs.log2_vrs16 = load_mkl_symbol<mkl_log2_vrs16_func_t>(mkl_vml, "__svml_log2f16");
#endif

    symbols_loaded = true;
}

#ifdef _WIN32
// Windows DLL entry point for automatic initialization
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        init_mkl_symbols();
    }
    return TRUE;
}
#else
// Linux constructor for automatic initialization
__attribute__((constructor))
static void lib_init(void) {
    init_mkl_symbols();
}
#endif

extern "C" {

SHIM_EXPORT const char* get_vendor_name() {
    return "MKL";
}


// ============================================================================
// SINGLE PRECISION SCALAR (ss) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.acosf(ipp->ip[0]);
}

SHIM_EXPORT void shim_acosh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.acoshf(ipp->ip[0]);
}

SHIM_EXPORT void shim_asin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.asinf(ipp->ip[0]);
}

SHIM_EXPORT void shim_asinh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.asinhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.atanf(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.atan2f(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_atanh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.atanhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cbrt_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.cbrtf(ipp->ip[0]);
}

SHIM_EXPORT void shim_ceil_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.ceilf(ipp->ip[0]);
}

SHIM_EXPORT void shim_copysign_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.copysignf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_cos_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.cosf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cosh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.coshf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cospi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.cospif(ipp->ip[0]);
}

SHIM_EXPORT void shim_erf_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.erff(ipp->ip[0]);
}

SHIM_EXPORT void shim_erfc_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.erfcf(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.expf(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp10_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.exp10f(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.exp2f(ipp->ip[0]);
}

SHIM_EXPORT void shim_expm1_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.expm1f(ipp->ip[0]);
}

SHIM_EXPORT void shim_fabs_ss(InParams<float, float> *ipp) {
    float *ip = (float *)&ipp->ip[0];
    float *op = (float *)&ipp->op[0];
    mkl_funcs.fabsf(1, ip, op);
}

SHIM_EXPORT void shim_fdim_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.fdimf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_finite_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(mkl_funcs.finitef(ipp->ip[0]));
}

SHIM_EXPORT void shim_floor_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.floorf(ipp->ip[0]);
}

SHIM_EXPORT void shim_fma_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.fmaf(ipp->ip[0], ipp->ip[1], ipp->ip[2]);
}

SHIM_EXPORT void shim_fmax_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.fmaxf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.fminf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmod_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.fmodf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_frexp_ss(InParams<float, float> *ipp) {
    int exp;
    ipp->op[0] = mkl_funcs.frexpf(ipp->ip[0], &exp);
    ipp->op[1] = static_cast<float>(exp);
}

SHIM_EXPORT void shim_hypot_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.hypotf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_ilogb_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(mkl_funcs.ilogbf(ipp->ip[0]));
}

SHIM_EXPORT void shim_ldexp_ss(InParams<float, float> *ipp) {
#ifndef _WIN32
    ipp->op[0] = mkl_funcs.ldexpf(ipp->ip[0], static_cast<int>(ipp->ip[1]));
#else
    printf("ldexp is not supported in windows MKL\n");
#endif
}

SHIM_EXPORT void shim_llrint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(mkl_funcs.llrintf(ipp->ip[0]));
}

SHIM_EXPORT void shim_llround_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(mkl_funcs.llroundf(ipp->ip[0]));
}

SHIM_EXPORT void shim_log_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.logf(ipp->ip[0]);
}

SHIM_EXPORT void shim_log10_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.log10f(ipp->ip[0]);
}

SHIM_EXPORT void shim_log1p_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.log1pf(ipp->ip[0]);
}

SHIM_EXPORT void shim_log2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.log2f(ipp->ip[0]);
}

SHIM_EXPORT void shim_logb_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.logbf(ipp->ip[0]);
}

SHIM_EXPORT void shim_lrint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(mkl_funcs.lrintf(ipp->ip[0]));
}

SHIM_EXPORT void shim_lround_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(mkl_funcs.lroundf(ipp->ip[0]));
}

SHIM_EXPORT void shim_modf_ss(InParams<float, float> *ipp) {
    float ipart;
    ipp->op[0] = mkl_funcs.modff(ipp->ip[0], &ipart);
    ipp->op[1] = ipart;
}

SHIM_EXPORT void shim_nearbyint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.nearbyintf(ipp->ip[0]);
}

SHIM_EXPORT void shim_nextafter_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.nextafterf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_nexttoward_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.nexttowardf(ipp->ip[0], static_cast<long double>(ipp->ip[1]));
}

SHIM_EXPORT void shim_pow_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.powf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remainder_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.remainderf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remquo_ss(InParams<float, float> *ipp) {
    int quo;
    ipp->op[0] = mkl_funcs.remquof(ipp->ip[0], ipp->ip[1], &quo);
    ipp->op[1] = static_cast<float>(quo);
}

SHIM_EXPORT void shim_rint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.rintf(ipp->ip[0]);
}

SHIM_EXPORT void shim_round_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.roundf(ipp->ip[0]);
}

SHIM_EXPORT void shim_scalbln_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.scalblnf(ipp->ip[0], static_cast<long>(ipp->ip[1]));
}

SHIM_EXPORT void shim_scalbn_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.scalbnf(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_sin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.sinf(ipp->ip[0]);
}

SHIM_EXPORT void shim_sincos_ss(InParams<float, float> *ipp) {
    mkl_funcs.sincosf(ipp->ip[0], &ipp->op[0], &ipp->op[1]);
}

SHIM_EXPORT void shim_sinh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.sinhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_sinpi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.sinpif(ipp->ip[0]);
}

SHIM_EXPORT void shim_sqrt_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.sqrtf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tan_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.tanf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.tanhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanpi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.tanpif(ipp->ip[0]);
}

SHIM_EXPORT void shim_trunc_ss(InParams<float, float> *ipp) {
    ipp->op[0] = mkl_funcs.truncf(ipp->ip[0]);
}

// ============================================================================
// DOUBLE PRECISION SCALAR (sd) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.acos(ipp->ip[0]);
}

SHIM_EXPORT void shim_acosh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.acosh(ipp->ip[0]);
}

SHIM_EXPORT void shim_asin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.asin(ipp->ip[0]);
}

SHIM_EXPORT void shim_asinh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.asinh(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.atan(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.atan2(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_atanh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.atanh(ipp->ip[0]);
}

SHIM_EXPORT void shim_cbrt_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.cbrt(ipp->ip[0]);
}

SHIM_EXPORT void shim_ceil_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.ceil(ipp->ip[0]);
}

SHIM_EXPORT void shim_copysign_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.copysign(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_cos_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.cos(ipp->ip[0]);
}

SHIM_EXPORT void shim_cosh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.cosh(ipp->ip[0]);
}

SHIM_EXPORT void shim_cospi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.cospi(ipp->ip[0]);
}

SHIM_EXPORT void shim_erf_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.erf(ipp->ip[0]);
}

SHIM_EXPORT void shim_erfc_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.erfc(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.exp(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp10_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.exp10(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.exp2(ipp->ip[0]);
}

SHIM_EXPORT void shim_expm1_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.expm1(ipp->ip[0]);
}

SHIM_EXPORT void shim_fabs_sd(InParams<double, double> *ipp) {
    double *ip = (double*)&ipp->ip[0];
    double *op = (double*)&ipp->op[0];
    mkl_funcs.fabs(1, ip, op);
}

SHIM_EXPORT void shim_fdim_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.fdim(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_finite_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(mkl_funcs.finite(ipp->ip[0]));
}

SHIM_EXPORT void shim_floor_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.floor(ipp->ip[0]);
}

SHIM_EXPORT void shim_fma_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.fma(ipp->ip[0], ipp->ip[1], ipp->ip[2]);
}

SHIM_EXPORT void shim_fmax_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.fmax(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.fmin(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmod_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.fmod(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_frexp_sd(InParams<double, double> *ipp) {
    int exp;
    ipp->op[0] = mkl_funcs.frexp(ipp->ip[0], &exp);
    ipp->op[1] = static_cast<double>(exp);
}

SHIM_EXPORT void shim_hypot_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.hypot(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_ilogb_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(mkl_funcs.ilogb(ipp->ip[0]));
}

SHIM_EXPORT void shim_ldexp_sd(InParams<double, double> *ipp) {
#ifndef _WIN32
    ipp->op[0] = mkl_funcs.ldexp(ipp->ip[0], static_cast<int>(ipp->ip[1]));
#else
    printf("ldexp is not supported in windows MKL\n");
#endif
}

SHIM_EXPORT void shim_llrint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(mkl_funcs.llrint(ipp->ip[0]));
}

SHIM_EXPORT void shim_llround_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(mkl_funcs.llround(ipp->ip[0]));
}

SHIM_EXPORT void shim_log_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.log(ipp->ip[0]);
}

SHIM_EXPORT void shim_log10_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.log10(ipp->ip[0]);
}

SHIM_EXPORT void shim_log1p_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.log1p(ipp->ip[0]);
}

SHIM_EXPORT void shim_log2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.log2(ipp->ip[0]);
}

SHIM_EXPORT void shim_logb_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.logb(ipp->ip[0]);
}

SHIM_EXPORT void shim_lrint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(mkl_funcs.lrint(ipp->ip[0]));
}

SHIM_EXPORT void shim_lround_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(mkl_funcs.lround(ipp->ip[0]));
}

SHIM_EXPORT void shim_modf_sd(InParams<double, double> *ipp) {
    double ipart;
    ipp->op[0] = mkl_funcs.modf(ipp->ip[0], &ipart);
    ipp->op[1] = ipart;
}

SHIM_EXPORT void shim_nearbyint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.nearbyint(ipp->ip[0]);
}

SHIM_EXPORT void shim_nextafter_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.nextafter(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_nexttoward_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.nexttoward(ipp->ip[0], static_cast<long double>(ipp->ip[1]));
}

SHIM_EXPORT void shim_pow_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.pow(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remainder_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.remainder(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remquo_sd(InParams<double, double> *ipp) {
    int quo;
    ipp->op[0] = mkl_funcs.remquo(ipp->ip[0], ipp->ip[1], &quo);
    ipp->op[1] = static_cast<double>(quo);
}

SHIM_EXPORT void shim_rint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.rint(ipp->ip[0]);
}

SHIM_EXPORT void shim_round_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.round(ipp->ip[0]);
}

SHIM_EXPORT void shim_scalbln_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.scalbln(ipp->ip[0], static_cast<long>(ipp->ip[1]));
}

SHIM_EXPORT void shim_scalbn_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.scalbn(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_sin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.sin(ipp->ip[0]);
}

SHIM_EXPORT void shim_sincos_sd(InParams<double, double> *ipp) {
    mkl_funcs.sincos(ipp->ip[0], &ipp->op[0], &ipp->op[1]);
}

SHIM_EXPORT void shim_sinh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.sinh(ipp->ip[0]);
}

SHIM_EXPORT void shim_sinpi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.sinpi(ipp->ip[0]);
}

SHIM_EXPORT void shim_sqrt_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.sqrt(ipp->ip[0]);
}

SHIM_EXPORT void shim_tan_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.tan(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.tanh(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanpi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.tanpi(ipp->ip[0]);
}

SHIM_EXPORT void shim_trunc_sd(InParams<double, double> *ipp) {
    ipp->op[0] = mkl_funcs.trunc(ipp->ip[0]);
}

// ============================================================================
// DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.acos_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.asin_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.atan_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cbrt_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.cbrt_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.cos_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.cosh_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.erf_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.erfc_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp10_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp10_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp2_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    double *ip = (double*)&ipp->ip[0].data;
    double *op = (double*)&ipp->op[0].data;
    mkl_funcs.fabs_vrd2(2, ip, op);
}

SHIM_EXPORT void shim_linearfrac_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    double *ipa = (double *)&ipp->ip[0].data;
    double *ipb = (double *)&ipp->ip[1].data;
    double sca  = ipp->ip[2].data[0];
    double scb  = ipp->ip[3].data[0];
    double sfta = ipp->ip[4].data[0];
    double sftb = ipp->ip[5].data[0];
    double *op = (double *)&ipp->op[0].data;
    mkl_funcs.linearfrac_vrd2(2, ipa, ipb, sca, sfta, scb, sftb, op);
}

SHIM_EXPORT void shim_log_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log10_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log1p_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log1p_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log2_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.pow_vrd2(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    double *ipa = (double*)&ipp->ip[0].data;
    double  ipb = ipp->ip[1].data[0];
    double *op = (double*)&ipp->op[0].data;
    mkl_funcs.powx_vrd2(2, ipa, ipb, op);
}

SHIM_EXPORT void shim_sin_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.sin_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    mkl_funcs.sincos_vrd2(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.sqrt_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.tan_vrd2(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.acos_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.asin_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.atan_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cbrt_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.cbrt_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.cos_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.cosh_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.erf_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.erfc_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp10_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp10_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp2_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_expm1_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.expm1_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrs4(InParams<libm::AlignedM128, float> *ipp) {
	float *ip = (float*)&ipp->ip[0].data;
    float *op = (float*)&ipp->op[0].data;
    mkl_funcs.fabs_vrs4(4, ip, op);
}

SHIM_EXPORT void shim_linearfrac_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    float *ipa = (float *)&ipp->ip[0].data;
    float *ipb = (float *)&ipp->ip[1].data;
    float sca  = ipp->ip[2].data[0];
    float scb  = ipp->ip[3].data[0];
    float sfta = ipp->ip[4].data[0];
    float sftb = ipp->ip[5].data[0];
    float *op = (float *)&ipp->op[0].data;
    mkl_funcs.linearfrac_vrs4(4, ipa, ipb, sca, sfta, scb, sftb, op);
}

SHIM_EXPORT void shim_log_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log10_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log1p_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log1p_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log2_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.pow_vrs4(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    float *ipa = (float*)&ipp->ip[0].data;
    float  ipb = ipp->ip[1].data[0];
    float *op  = (float*)&ipp->op[0].data;
    mkl_funcs.powx_vrs4(4, ipa, ipb, op);
}

SHIM_EXPORT void shim_sin_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.sin_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    mkl_funcs.sincos_vrs4(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.sqrt_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.tan_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = mkl_funcs.tanh_vrs4(ipp->ip[0].data);
}

// ============================================================================
// DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.acos_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.asin_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.atan_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.cos_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.erf_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.erfc_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp2_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    double *ip = (double*)&ipp->ip[0].data;
    double *op = (double*)&ipp->op[0].data;
    mkl_funcs.fabs_vrd4(4, ip, op);
}

SHIM_EXPORT void shim_linearfrac_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    double *ipa = (double *)&ipp->ip[0].data;
    double *ipb = (double *)&ipp->ip[1].data;
    double sca  = ipp->ip[2].data[0];
    double scb  = ipp->ip[3].data[0];
    double sfta = ipp->ip[4].data[0];
    double sftb = ipp->ip[5].data[0];
    double *op = (double *)&ipp->op[0].data;
    mkl_funcs.linearfrac_vrd4(4, ipa, ipb, sca, sfta, scb, sftb, op);
}

SHIM_EXPORT void shim_log_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log2_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.pow_vrd4(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    double *ipa = (double*)&ipp->ip[0].data;
    double  ipb = ipp->ip[1].data[0];
    double *op  = (double*)&ipp->op[0].data;
    mkl_funcs.powx_vrd4(4, ipa, ipb, op);
}

SHIM_EXPORT void shim_sin_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.sin_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    mkl_funcs.sincos_vrd4(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.sqrt_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.tan_vrd4(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.acos_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.asin_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.atan_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.cos_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.cosh_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.erf_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.erfc_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp2_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    float *ip = (float*)&ipp->ip[0].data;
    float *op = (float*)&ipp->op[0].data;
    mkl_funcs.fabs_vrs8(8, ip, op);
}

SHIM_EXPORT void shim_linearfrac_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    float *ipa = (float *)&ipp->ip[0].data;
    float *ipb = (float *)&ipp->ip[1].data;
    float sca  = ipp->ip[2].data[0];
    float scb  = ipp->ip[3].data[0];
    float sfta = ipp->ip[4].data[0];
    float sftb = ipp->ip[5].data[0];
    float *op  = (float *)&ipp->op[0].data;
    mkl_funcs.linearfrac_vrs8(8, ipa, ipb, sca, scb, sfta, sftb, op);
}

SHIM_EXPORT void shim_log_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log10_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log2_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.pow_vrs8(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    float *ipa = (float*)&ipp->ip[0].data;
    float  ipb = ipp->ip[1].data[0];
    float *op  = (float*)&ipp->op[0].data;
    mkl_funcs.powx_vrs8(8, ipa, ipb, op);
}

SHIM_EXPORT void shim_sin_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.sin_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    mkl_funcs.sincos_vrs8(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.sqrt_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.tan_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = mkl_funcs.tanh_vrs8(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION ARRAY (vrsa) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.acos_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_add_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.add_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_addfi_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.addfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_asin_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.asin_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_atan_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.atan_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cbrt_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.cbrt_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cos_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.cos_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cosh_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.cosh_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_div_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.div_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_divfi_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.divfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_erf_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.erf_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_erfc_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.erfc_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.exp_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp10_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.exp10_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp2_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.exp2_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_expm1_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.expm1_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fabs_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.fabs_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmax_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.fmax_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmaxfi_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.fmaxfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmin_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.fmin_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fminfi_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.fminfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_linearfrac_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.linearfrac_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->ip[2], ipp->ip[3], ipp->ip[4], ipp->ip[5], ipp->optr[0]);
}

SHIM_EXPORT void shim_log_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.log_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log10_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.log10_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log1p_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.log1p_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log2_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.log2_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_mul_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.mul_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_mulfi_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.mulfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_pow_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.pow_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_powx_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.powx_vrsa(ipp->count, ipp->iptr[0], *ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_sin_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.sin_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sincos_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.sincos_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0], ipp->optr[1]);
}

SHIM_EXPORT void shim_sqrt_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.sqrt_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sub_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.sub_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_subfi_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.subfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_tan_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.tan_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_tanh_vrsa(InParams<float, float> *ipp) {
    mkl_funcs.tanh_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

// ============================================================================
// DOUBLE PRECISION ARRAY (vrda) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrda(InParams<double, double> *ipp) {
    mkl_funcs.acos_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_add_vrda(InParams<double, double> *ipp) {
    mkl_funcs.add_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_addi_vrda(InParams<double, double> *ipp) {
    mkl_funcs.addi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_asin_vrda(InParams<double, double> *ipp) {
    mkl_funcs.asin_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_atan_vrda(InParams<double, double> *ipp) {
    mkl_funcs.atan_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cbrt_vrda(InParams<double, double> *ipp) {
    mkl_funcs.cbrt_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cos_vrda(InParams<double, double> *ipp) {
    mkl_funcs.cos_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cosh_vrda(InParams<double, double> *ipp) {
    mkl_funcs.cosh_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_div_vrda(InParams<double, double> *ipp) {
    mkl_funcs.div_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_divi_vrda(InParams<double, double> *ipp) {
    mkl_funcs.divi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_erf_vrda(InParams<double, double> *ipp) {
    mkl_funcs.erf_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_erfc_vrda(InParams<double, double> *ipp) {
    mkl_funcs.erfc_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp_vrda(InParams<double, double> *ipp) {
    mkl_funcs.exp_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp10_vrda(InParams<double, double> *ipp) {
    mkl_funcs.exp10_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp2_vrda(InParams<double, double> *ipp) {
    mkl_funcs.exp2_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_expm1_vrda(InParams<double, double> *ipp) {
    mkl_funcs.expm1_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fabs_vrda(InParams<double, double> *ipp) {
    mkl_funcs.fabs_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmax_vrda(InParams<double, double> *ipp) {
    mkl_funcs.fmax_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmaxi_vrda(InParams<double, double> *ipp) {
    mkl_funcs.fmaxi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmin_vrda(InParams<double, double> *ipp) {
    mkl_funcs.fmin_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmini_vrda(InParams<double, double> *ipp) {
    mkl_funcs.fmini_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_linearfrac_vrda(InParams<double, double> *ipp) {
    mkl_funcs.linearfrac_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->ip[2], ipp->ip[3], ipp->ip[4], ipp->ip[5], ipp->optr[0]);
}

SHIM_EXPORT void shim_log_vrda(InParams<double, double> *ipp) {
    mkl_funcs.log_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log10_vrda(InParams<double, double> *ipp) {
    mkl_funcs.log10_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log1p_vrda(InParams<double, double> *ipp) {
    mkl_funcs.log1p_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log2_vrda(InParams<double, double> *ipp) {
    mkl_funcs.log2_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_mul_vrda(InParams<double, double> *ipp) {
    mkl_funcs.mul_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_muli_vrda(InParams<double, double> *ipp) {
    mkl_funcs.muli_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_pow_vrda(InParams<double, double> *ipp) {
    mkl_funcs.pow_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_powx_vrda(InParams<double, double> *ipp) {
    mkl_funcs.powx_vrda(ipp->count, ipp->iptr[0], *ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_sin_vrda(InParams<double, double> *ipp) {
    mkl_funcs.sin_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sincos_vrda(InParams<double, double> *ipp) {
    mkl_funcs.sincos_vrda(ipp->count, ipp->iptr[0], ipp->optr[0], ipp->optr[1]);
}

SHIM_EXPORT void shim_sqrt_vrda(InParams<double, double> *ipp) {
    mkl_funcs.sqrt_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sub_vrda(InParams<double, double> *ipp) {
    mkl_funcs.sub_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_subi_vrda(InParams<double, double> *ipp) {
    mkl_funcs.subi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_tan_vrda(InParams<double, double> *ipp) {
    mkl_funcs.tan_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

#ifdef __AVX512F__
// ============================================================================
// DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_asin_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.asin_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.atan_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.cos_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.erf_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.erfc_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.exp2_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    double *ipa = (double *)&ipp->ip[0].data;
    double *ipb = (double *)&ipp->ip[1].data;
    double sca  = ipp->ip[2].data[0];
    double scb  = ipp->ip[3].data[0];
    double sfta = ipp->ip[4].data[0];
    double sftb = ipp->ip[5].data[0];
    double *op  = (double *)&ipp->op[0].data;
    mkl_funcs.linearfrac_vrd8(8, ipa, ipb, sca, scb, sfta, sftb, op);
}

SHIM_EXPORT void shim_log_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.log2_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.pow_vrd8(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    double *ipa = (double*)&ipp->ip[0].data;
    double  ipb = ipp->ip[1].data[0];
    double *op  = (double*)&ipp->op[0].data;
    mkl_funcs.powx_vrd8(8, ipa, ipb, op);
}

SHIM_EXPORT void shim_sin_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.sin_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    mkl_funcs.sincos_vrd8(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.sqrt_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = mkl_funcs.tan_vrd8(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.acos_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.asin_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.atan_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.cos_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.erf_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.erfc_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.exp2_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    float *ipa = (float *)&ipp->ip[0].data;
    float *ipb = (float *)&ipp->ip[1].data;
    float sca  = ipp->ip[2].data[0];
    float scb  = ipp->ip[3].data[0];
    float sfta = ipp->ip[4].data[0];
    float sftb = ipp->ip[5].data[0];
    float *op  = (float *)&ipp->op[0].data;
    mkl_funcs.linearfrac_vrs16(16, ipa, ipb, sca, scb, sfta, sftb, op);
}

SHIM_EXPORT void shim_log_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log10_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.log2_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.pow_vrs16(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    float *ipa = (float*)&ipp->ip[0].data;
    float  ipb = ipp->ip[1].data[0];
    float *op  = (float*)&ipp->op[0].data;
    mkl_funcs.powx_vrs16(16, ipa, ipb, op);
}

SHIM_EXPORT void shim_sin_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.sin_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    mkl_funcs.sincos_vrs16(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.sqrt_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.tan_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = mkl_funcs.tanh_vrs16(ipp->ip[0].data);
}
#endif

} // extern "C"