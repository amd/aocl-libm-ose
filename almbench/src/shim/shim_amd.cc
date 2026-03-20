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
 * High-performance cross-platform LD_PRELOAD shim for AMD math library
 * Optimized for minimal overhead with one-time symbol loading
 * Compatible with Windows and Linux without pthread dependency
 */

// Enable debug mode for detailed logging and diagnostics
//#define AMD_SHIM_DEBUG

#if AMD_SHIM_DEBUG
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...) ((void)0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include "alm_test.h"

#ifdef _WIN32
    #include <windows.h>
    #include <libloaderapi.h>
    #define SHIM_EXPORT __declspec(dllexport)
    #define LOAD_LIBRARY(name) LoadLibraryA(name)
    #define LIB_HANDLE HMODULE
    #ifdef AMD_SHIM_DEBUG
        // Function to load symbols with error checking
        static void* load_symbol_with_checks(LIB_HANDLE lib, const char* symbol_name) {
            printf("Attempting to load symbol: %s\n", symbol_name);

            // First try to get the module handle if already loaded
            HMODULE amdlibm_handle = GetModuleHandleA(lib);
            if (!amdlibm_handle) {
                printf("libalm.dll not found in process, attempting to load...\n");

                // Try to load the library
                amdlibm_handle = LoadLibraryA(lib);
                if (!amdlibm_handle) {
                    DWORD error = GetLastError();
                    printf("ERROR: Failed to load libalm.dll (Error: %lu)\n", error);
                    printf("Make sure libalm.dll is in the same directory as the executable or in your PATH\n");
                    return NULL;
                }
                printf("SUCCESS: Loaded libalm.dll\n");
            }

            void* symbol = GetProcAddress(amdlibm_handle, symbol_name);
            if (!symbol) {
                DWORD error = GetLastError();
                printf("ERROR: Failed to load symbol '%s' from libalm.dll (Error: %lu)\n", symbol_name, error);
                return NULL;
            }

            printf("SUCCESS: Loaded symbol '%s' at address %p\n", symbol_name, symbol);
            return symbol;
        }

        #define LOAD_SYMBOL(lib, name) load_symbol_with_checks(lib, name)
    #else
        #define LOAD_SYMBOL(lib, name) GetProcAddress(lib, name)
    #endif

#else
    #include <dlfcn.h>
    #define SHIM_EXPORT __attribute__((visibility("default")))
    #define LOAD_LIBRARY(name) dlopen(name, RTLD_LAZY)
    #define LIB_HANDLE void*
    #ifdef AMD_SHIM_DEBUG
	    // Function to load symbols with error checking
	    static void* load_symbol_with_checks(LIB_HANDLE lib, const char* symbol_name) {
	        printf("Attempting to load symbol: %s\n", symbol_name);

	        void* symbol = dlsym(RTLD_DEFAULT, symbol_name);
	        if (!symbol) {
	            const char* error = dlerror();
	            printf("ERROR: Failed to load symbol '%s' - %s\n", symbol_name, error ? error : "Unknown error");
	            return NULL;
	        }

	        printf("SUCCESS: Loaded symbol '%s' at address %p\n", symbol_name, symbol);
	        return symbol;
	    }

	    #define LOAD_SYMBOL(lib, name) load_symbol_with_checks(lib, name)
    #else
	    #define LOAD_SYMBOL(lib, name) dlsym(lib, name)
    #endif
#endif

// ============================================================================
// FUNCTION POINTER TYPEDEFS ORGANIZED BY VARIANT
// ============================================================================
// --- Single Precision Scalar (ss) Functions ---
typedef float (*amd_acos_ss_func_t)(float);
typedef float (*amd_acosh_ss_func_t)(float);
typedef float (*amd_asin_ss_func_t)(float);
typedef float (*amd_asinh_ss_func_t)(float);
typedef float (*amd_atan_ss_func_t)(float);
typedef float (*amd_atan2_ss_func_t)(float, float);
typedef float (*amd_atanh_ss_func_t)(float);
typedef float (*amd_cbrt_ss_func_t)(float);
typedef float (*amd_ceil_ss_func_t)(float);
typedef float (*amd_copysign_ss_func_t)(float, float);
typedef float (*amd_cos_ss_func_t)(float);
typedef float (*amd_cosh_ss_func_t)(float);
typedef float (*amd_cospi_ss_func_t)(float);
typedef float (*amd_erf_ss_func_t)(float);
typedef float (*amd_erfc_ss_func_t)(float);
typedef float (*amd_exp_ss_func_t)(float);
typedef float (*amd_exp10_ss_func_t)(float);
typedef float (*amd_exp2_ss_func_t)(float);
typedef float (*amd_expm1_ss_func_t)(float);
typedef float (*amd_fabs_ss_func_t)(float);
typedef float (*amd_fdim_ss_func_t)(float, float);
typedef float (*amd_finite_ss_func_t)(float);
typedef float (*amd_floor_ss_func_t)(float);
typedef float (*amd_fma_ss_func_t)(float, float, float);
typedef float (*amd_fmax_ss_func_t)(float, float);
typedef float (*amd_fmin_ss_func_t)(float, float);
typedef float (*amd_fmod_ss_func_t)(float, float);
typedef float (*amd_frexp_ss_func_t)(float, int*);
typedef float (*amd_hypot_ss_func_t)(float, float);
typedef int (*amd_ilogb_ss_func_t)(float);
typedef float (*amd_ldexp_ss_func_t)(float, int);
typedef long long (*amd_llrint_ss_func_t)(float);
typedef long long (*amd_llround_ss_func_t)(float);
typedef float (*amd_log_ss_func_t)(float);
typedef float (*amd_log10_ss_func_t)(float);
typedef float (*amd_log1p_ss_func_t)(float);
typedef float (*amd_log2_ss_func_t)(float);
typedef float (*amd_logb_ss_func_t)(float);
typedef long (*amd_lrint_ss_func_t)(float);
typedef long (*amd_lround_ss_func_t)(float);
typedef float (*amd_modf_ss_func_t)(float, float*);
typedef float (*amd_nearbyint_ss_func_t)(float);
typedef float (*amd_nextafter_ss_func_t)(float, float);
typedef float (*amd_nexttoward_ss_func_t)(float, long double);
typedef float (*amd_pow_ss_func_t)(float, float);
typedef float (*amd_remainder_ss_func_t)(float, float);
typedef float (*amd_remquo_ss_func_t)(float, float, int*);
typedef float (*amd_rint_ss_func_t)(float);
typedef float (*amd_round_ss_func_t)(float);
typedef float (*amd_scalbln_ss_func_t)(float, long);
typedef float (*amd_scalbn_ss_func_t)(float, int);
typedef float (*amd_sin_ss_func_t)(float);
typedef void (*amd_sincos_ss_func_t)(float, float*, float*);
typedef float (*amd_sinh_ss_func_t)(float);
typedef float (*amd_sinpi_ss_func_t)(float);
typedef float (*amd_sqrt_ss_func_t)(float);
typedef float (*amd_tan_ss_func_t)(float);
typedef float (*amd_tanh_ss_func_t)(float);
typedef float (*amd_tanpi_ss_func_t)(float);
typedef float (*amd_trunc_ss_func_t)(float);

// --- Double Precision Scalar (sd) Functions ---
typedef double (*amd_acos_sd_func_t)(double);
typedef double (*amd_acosh_sd_func_t)(double);
typedef double (*amd_asin_sd_func_t)(double);
typedef double (*amd_asinh_sd_func_t)(double);
typedef double (*amd_atan_sd_func_t)(double);
typedef double (*amd_atan2_sd_func_t)(double, double);
typedef double (*amd_atanh_sd_func_t)(double);
typedef double (*amd_cbrt_sd_func_t)(double);
typedef double (*amd_ceil_sd_func_t)(double);
typedef double (*amd_copysign_sd_func_t)(double, double);
typedef double (*amd_cos_sd_func_t)(double);
typedef double (*amd_cosh_sd_func_t)(double);
typedef double (*amd_cospi_sd_func_t)(double);
typedef double (*amd_erf_sd_func_t)(double);
typedef double (*amd_erfc_sd_func_t)(double);
typedef double (*amd_exp_sd_func_t)(double);
typedef double (*amd_exp10_sd_func_t)(double);
typedef double (*amd_exp2_sd_func_t)(double);
typedef double (*amd_expm1_sd_func_t)(double);
typedef double (*amd_fabs_sd_func_t)(double);
typedef double (*amd_fdim_sd_func_t)(double, double);
typedef double (*amd_finite_sd_func_t)(double);
typedef double (*amd_floor_sd_func_t)(double);
typedef double (*amd_fma_sd_func_t)(double, double, double);
typedef double (*amd_fmax_sd_func_t)(double, double);
typedef double (*amd_fmin_sd_func_t)(double, double);
typedef double (*amd_fmod_sd_func_t)(double, double);
typedef double (*amd_frexp_sd_func_t)(double, int*);
typedef double (*amd_hypot_sd_func_t)(double, double);
typedef int (*amd_ilogb_sd_func_t)(double);
typedef double (*amd_ldexp_sd_func_t)(double, int);
typedef long long (*amd_llrint_sd_func_t)(double);
typedef long long (*amd_llround_sd_func_t)(double);
typedef double (*amd_log_sd_func_t)(double);
typedef double (*amd_log10_sd_func_t)(double);
typedef double (*amd_log1p_sd_func_t)(double);
typedef double (*amd_log2_sd_func_t)(double);
typedef double (*amd_logb_sd_func_t)(double);
typedef long (*amd_lrint_sd_func_t)(double);
typedef long (*amd_lround_sd_func_t)(double);
typedef double (*amd_modf_sd_func_t)(double, double*);
typedef double (*amd_nearbyint_sd_func_t)(double);
typedef double (*amd_nextafter_sd_func_t)(double, double);
typedef double (*amd_nexttoward_sd_func_t)(double, long double);
typedef double (*amd_pow_sd_func_t)(double, double);
typedef double (*amd_remainder_sd_func_t)(double, double);
typedef double (*amd_remquo_sd_func_t)(double, double, int*);
typedef double (*amd_rint_sd_func_t)(double);
typedef double (*amd_round_sd_func_t)(double);
typedef double (*amd_scalbln_sd_func_t)(double, long);
typedef double (*amd_scalbn_sd_func_t)(double, int);
typedef double (*amd_sin_sd_func_t)(double);
typedef void (*amd_sincos_sd_func_t)(double, double*, double*);
typedef double (*amd_sinh_sd_func_t)(double);
typedef double (*amd_sinpi_sd_func_t)(double);
typedef double (*amd_sqrt_sd_func_t)(double);
typedef double (*amd_tan_sd_func_t)(double);
typedef double (*amd_tanh_sd_func_t)(double);
typedef double (*amd_tanpi_sd_func_t)(double);
typedef double (*amd_trunc_sd_func_t)(double);

// --- Double Precision 128-bit Vector (vrd2) Functions ---
typedef __m128d (*amd_acos_vrd2_func_t)(__m128d);
typedef __m128d (*amd_asin_vrd2_func_t)(__m128d);
typedef __m128d (*amd_atan_vrd2_func_t)(__m128d);
typedef __m128d (*amd_cbrt_vrd2_func_t)(__m128d);
typedef __m128d (*amd_cos_vrd2_func_t)(__m128d);
typedef __m128d (*amd_cosh_vrd2_func_t)(__m128d);
typedef __m128d (*amd_erf_vrd2_func_t)(__m128d);
typedef __m128d (*amd_erfc_vrd2_func_t)(__m128d);
typedef __m128d (*amd_exp_vrd2_func_t)(__m128d);
typedef __m128d (*amd_exp10_vrd2_func_t)(__m128d);
typedef __m128d (*amd_exp2_vrd2_func_t)(__m128d);
typedef __m128d (*amd_fabs_vrd2_func_t)(__m128d);
typedef __m128d (*amd_linearfrac_vrd2_func_t)(__m128d, __m128d, double, double, double, double);
typedef __m128d (*amd_log_vrd2_func_t)(__m128d);
typedef __m128d (*amd_log10_vrd2_func_t)(__m128d);
typedef __m128d (*amd_log1p_vrd2_func_t)(__m128d);
typedef __m128d (*amd_log2_vrd2_func_t)(__m128d);
typedef __m128d (*amd_pow_vrd2_func_t)(__m128d, __m128d);
typedef __m128d (*amd_powx_vrd2_func_t)(__m128d, double);
typedef __m128d (*amd_sin_vrd2_func_t)(__m128d);
typedef void (*amd_sincos_vrd2_func_t)(__m128d, __m128d*, __m128d*);
typedef __m128d (*amd_sqrt_vrd2_func_t)(__m128d);
typedef __m128d (*amd_tan_vrd2_func_t)(__m128d);

// --- Single Precision 128-bit Vector (vrs4) Functions ---
typedef __m128 (*amd_acos_vrs4_func_t)(__m128);
typedef __m128 (*amd_asin_vrs4_func_t)(__m128);
typedef __m128 (*amd_atan_vrs4_func_t)(__m128);
typedef __m128 (*amd_cbrt_vrs4_func_t)(__m128);
typedef __m128 (*amd_cos_vrs4_func_t)(__m128);
typedef __m128 (*amd_cosh_vrs4_func_t)(__m128);
typedef __m128 (*amd_erf_vrs4_func_t)(__m128);
typedef __m128 (*amd_erfc_vrs4_func_t)(__m128);
typedef __m128 (*amd_exp_vrs4_func_t)(__m128);
typedef __m128 (*amd_exp10_vrs4_func_t)(__m128);
typedef __m128 (*amd_exp2_vrs4_func_t)(__m128);
typedef __m128 (*amd_expm1_vrs4_func_t)(__m128);
typedef __m128 (*amd_fabs_vrs4_func_t)(__m128);
typedef __m128 (*amd_linearfrac_vrs4_func_t)(__m128, __m128, float, float, float, float);
typedef __m128 (*amd_log_vrs4_func_t)(__m128);
typedef __m128 (*amd_log10_vrs4_func_t)(__m128);
typedef __m128 (*amd_log1p_vrs4_func_t)(__m128);
typedef __m128 (*amd_log2_vrs4_func_t)(__m128);
typedef __m128 (*amd_pow_vrs4_func_t)(__m128, __m128);
typedef __m128 (*amd_powx_vrs4_func_t)(__m128, float);
typedef __m128 (*amd_sin_vrs4_func_t)(__m128);
typedef void (*amd_sincos_vrs4_func_t)(__m128, __m128*, __m128*);
typedef __m128 (*amd_sqrt_vrs4_func_t)(__m128);
typedef __m128 (*amd_tan_vrs4_func_t)(__m128);
typedef __m128 (*amd_tanh_vrs4_func_t)(__m128);

// --- Double Precision 256-bit Vector (vrd4) Functions ---
typedef __m256d (*amd_acos_vrd4_func_t)(__m256d);
typedef __m256d (*amd_asin_vrd4_func_t)(__m256d);
typedef __m256d (*amd_atan_vrd4_func_t)(__m256d);
typedef __m256d (*amd_cos_vrd4_func_t)(__m256d);
typedef __m256d (*amd_erf_vrd4_func_t)(__m256d);
typedef __m256d (*amd_erfc_vrd4_func_t)(__m256d);
typedef __m256d (*amd_exp_vrd4_func_t)(__m256d);
typedef __m256d (*amd_exp2_vrd4_func_t)(__m256d);
typedef __m256d (*amd_fabs_vrd4_func_t)(__m256d);
typedef __m256d (*amd_linearfrac_vrd4_func_t)(__m256d, __m256d, double, double, double, double);
typedef __m256d (*amd_log_vrd4_func_t)(__m256d);
typedef __m256d (*amd_log2_vrd4_func_t)(__m256d);
typedef __m256d (*amd_pow_vrd4_func_t)(__m256d, __m256d);
typedef __m256d (*amd_powx_vrd4_func_t)(__m256d, double);
typedef __m256d (*amd_sin_vrd4_func_t)(__m256d);
typedef void (*amd_sincos_vrd4_func_t)(__m256d, __m256d*, __m256d*);
typedef __m256d (*amd_sqrt_vrd4_func_t)(__m256d);
typedef __m256d (*amd_tan_vrd4_func_t)(__m256d);

// --- Single Precision 256-bit Vector (vrs8) Functions ---
typedef __m256 (*amd_acos_vrs8_func_t)(__m256);
typedef __m256 (*amd_asin_vrs8_func_t)(__m256);
typedef __m256 (*amd_atan_vrs8_func_t)(__m256);
typedef __m256 (*amd_cos_vrs8_func_t)(__m256);
typedef __m256 (*amd_cosh_vrs8_func_t)(__m256);
typedef __m256 (*amd_erf_vrs8_func_t)(__m256);
typedef __m256 (*amd_erfc_vrs8_func_t)(__m256);
typedef __m256 (*amd_exp_vrs8_func_t)(__m256);
typedef __m256 (*amd_exp2_vrs8_func_t)(__m256);
typedef __m256 (*amd_fabs_vrs8_func_t)(__m256);
typedef __m256 (*amd_linearfrac_vrs8_func_t)(__m256, __m256, float, float, float, float);
typedef __m256 (*amd_log_vrs8_func_t)(__m256);
typedef __m256 (*amd_log10_vrs8_func_t)(__m256);
typedef __m256 (*amd_log2_vrs8_func_t)(__m256);
typedef __m256 (*amd_pow_vrs8_func_t)(__m256, __m256);
typedef __m256 (*amd_powx_vrs8_func_t)(__m256, float);
typedef __m256 (*amd_sin_vrs8_func_t)(__m256);
typedef void (*amd_sincos_vrs8_func_t)(__m256, __m256*, __m256*);
typedef __m256 (*amd_sqrt_vrs8_func_t)(__m256);
typedef __m256 (*amd_tan_vrs8_func_t)(__m256);
typedef __m256 (*amd_tanh_vrs8_func_t)(__m256);

// --- Single Precision Array (vrsa) Functions ---
typedef void (*amd_acos_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_add_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_addfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_asin_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_atan_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_cbrt_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_cos_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_cosh_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_div_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_divfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_erf_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_erfc_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_exp_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_exp10_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_exp2_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_expm1_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_fabs_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_fmax_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_fmaxfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_fmin_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_fminfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_linearfrac_vrsa_func_t)(int, const float*,
    const float*, float, float, float, float, float*);
typedef void (*amd_log_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_log10_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_log1p_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_log2_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_mul_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_mulfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_pow_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_powx_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_sin_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_sincos_vrsa_func_t)(int, const float*, float*, float*);
typedef void (*amd_sqrt_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_sub_vrsa_func_t)(int, const float*, const float*, float*);
typedef void (*amd_subfi_vrsa_func_t)(int, const float*, float, float*);
typedef void (*amd_tan_vrsa_func_t)(int, const float*, float*);
typedef void (*amd_tanh_vrsa_func_t)(int, const float*, float*);

// --- Double Precision Array (vrda) Functions ---
typedef void (*amd_acos_vrda_func_t)(int, const double*, double*);
typedef void (*amd_add_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_addi_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_asin_vrda_func_t)(int, const double*, double*);
typedef void (*amd_atan_vrda_func_t)(int, const double*, double*);
typedef void (*amd_cbrt_vrda_func_t)(int, const double*, double*);
typedef void (*amd_cos_vrda_func_t)(int, const double*, double*);
typedef void (*amd_cosh_vrda_func_t)(int, const double*, double*);
typedef void (*amd_div_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_divi_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_erf_vrda_func_t)(int, const double*, double*);
typedef void (*amd_erfc_vrda_func_t)(int, const double*, double*);
typedef void (*amd_exp_vrda_func_t)(int, const double*, double*);
typedef void (*amd_exp10_vrda_func_t)(int, const double*, double*);
typedef void (*amd_exp2_vrda_func_t)(int, const double*, double*);
typedef void (*amd_expm1_vrda_func_t)(int, const double*, double*);
typedef void (*amd_fabs_vrda_func_t)(int, const double*, double*);
typedef void (*amd_fmax_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_fmaxi_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_fmin_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_fmini_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_linearfrac_vrda_func_t)(int, const double*,
    const double*, double, double, double, double, double*);
typedef void (*amd_log_vrda_func_t)(int, const double*, double*);
typedef void (*amd_log10_vrda_func_t)(int, const double*, double*);
typedef void (*amd_log1p_vrda_func_t)(int, const double*, double*);
typedef void (*amd_log2_vrda_func_t)(int, const double*, double*);
typedef void (*amd_mul_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_muli_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_pow_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_powx_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_sin_vrda_func_t)(int, const double*, double*);
typedef void (*amd_sincos_vrda_func_t)(int, const double*, double*, double*);
typedef void (*amd_sqrt_vrda_func_t)(int, const double*, double*);
typedef void (*amd_sub_vrda_func_t)(int, const double*, const double*, double*);
typedef void (*amd_subi_vrda_func_t)(int, const double*, double, double*);
typedef void (*amd_tan_vrda_func_t)(int, const double*, double*);

#ifdef __AVX512F__
// --- Double Precision 512-bit Vector (vrd8) Functions ---
typedef __m512d (*amd_asin_vrd8_func_t)(__m512d);
typedef __m512d (*amd_atan_vrd8_func_t)(__m512d);
typedef __m512d (*amd_cos_vrd8_func_t)(__m512d);
typedef __m512d (*amd_erf_vrd8_func_t)(__m512d);
typedef __m512d (*amd_erfc_vrd8_func_t)(__m512d);
typedef __m512d (*amd_exp_vrd8_func_t)(__m512d);
typedef __m512d (*amd_exp2_vrd8_func_t)(__m512d);
typedef __m512d (*amd_linearfrac_vrd8_func_t)(__m512d, __m512d, double, double, double, double);
typedef __m512d (*amd_log_vrd8_func_t)(__m512d);
typedef __m512d (*amd_log2_vrd8_func_t)(__m512d);
typedef __m512d (*amd_pow_vrd8_func_t)(__m512d, __m512d);
typedef __m512d (*amd_powx_vrd8_func_t)(__m512d, double);
typedef __m512d (*amd_sin_vrd8_func_t)(__m512d);
typedef void (*amd_sincos_vrd8_func_t)(__m512d, __m512d*, __m512d*);
typedef __m512d (*amd_sqrt_vrd8_func_t)(__m512d);
typedef __m512d (*amd_tan_vrd8_func_t)(__m512d);

// --- Single Precision 512-bit Vector (vrs16) Functions ---
typedef __m512 (*amd_acos_vrs16_func_t)(__m512);
typedef __m512 (*amd_asin_vrs16_func_t)(__m512);
typedef __m512 (*amd_atan_vrs16_func_t)(__m512);
typedef __m512 (*amd_cos_vrs16_func_t)(__m512);
typedef __m512 (*amd_erf_vrs16_func_t)(__m512);
typedef __m512 (*amd_erfc_vrs16_func_t)(__m512);
typedef __m512 (*amd_exp_vrs16_func_t)(__m512);
typedef __m512 (*amd_exp2_vrs16_func_t)(__m512);
typedef __m512 (*amd_linearfrac_vrs16_func_t)(__m512, __m512, float, float, float, float);
typedef __m512 (*amd_log_vrs16_func_t)(__m512);
typedef __m512 (*amd_log10_vrs16_func_t)(__m512);
typedef __m512 (*amd_log2_vrs16_func_t)(__m512);
typedef __m512 (*amd_pow_vrs16_func_t)(__m512, __m512);
typedef __m512 (*amd_powx_vrs16_func_t)(__m512, float);
typedef __m512 (*amd_sin_vrs16_func_t)(__m512);
typedef void (*amd_sincos_vrs16_func_t)(__m512, __m512*, __m512*);
typedef __m512 (*amd_sqrt_vrs16_func_t)(__m512);
typedef __m512 (*amd_tan_vrs16_func_t)(__m512);
typedef __m512 (*amd_tanh_vrs16_func_t)(__m512);
#endif

// Global function pointers (loaded once during initialization) - organized by variant
static struct {
    // ============================================================================
    // SINGLE PRECISION SCALAR (ss) VARIANTS
    // ============================================================================
    amd_acos_ss_func_t acosf;
    amd_acosh_ss_func_t acoshf;
    amd_asin_ss_func_t asinf;
    amd_asinh_ss_func_t asinhf;
    amd_atan_ss_func_t atanf;
    amd_atan2_ss_func_t atan2f;
    amd_atanh_ss_func_t atanhf;
    amd_cbrt_ss_func_t cbrtf;
    amd_ceil_ss_func_t ceilf;
    amd_copysign_ss_func_t copysignf;
    amd_cos_ss_func_t cosf;
    amd_cosh_ss_func_t coshf;
    amd_cospi_ss_func_t cospif;
    amd_erf_ss_func_t erff;
    amd_erfc_ss_func_t erfcf;
    amd_exp_ss_func_t expf;
    amd_exp10_ss_func_t exp10f;
    amd_exp2_ss_func_t exp2f;
    amd_expm1_ss_func_t expm1f;
    amd_fabs_ss_func_t fabsf;
    amd_fdim_ss_func_t fdimf;
    amd_finite_ss_func_t finitef;
    amd_floor_ss_func_t floorf;
    amd_fma_ss_func_t fmaf;
    amd_fmax_ss_func_t fmaxf;
    amd_fmin_ss_func_t fminf;
    amd_fmod_ss_func_t fmodf;
    amd_frexp_ss_func_t frexpf;
    amd_hypot_ss_func_t hypotf;
    amd_ilogb_ss_func_t ilogbf;
    amd_ldexp_ss_func_t ldexpf;
    amd_llrint_ss_func_t llrintf;
    amd_llround_ss_func_t llroundf;
    amd_log_ss_func_t logf;
    amd_log10_ss_func_t log10f;
    amd_log1p_ss_func_t log1pf;
    amd_log2_ss_func_t log2f;
    amd_logb_ss_func_t logbf;
    amd_lrint_ss_func_t lrintf;
    amd_lround_ss_func_t lroundf;
    amd_modf_ss_func_t modff;
    amd_nearbyint_ss_func_t nearbyintf;
    amd_nextafter_ss_func_t nextafterf;
    amd_nexttoward_ss_func_t nexttowardf;
    amd_pow_ss_func_t powf;
    amd_remainder_ss_func_t remainderf;
    amd_remquo_ss_func_t remquof;
    amd_rint_ss_func_t rintf;
    amd_round_ss_func_t roundf;
    amd_scalbln_ss_func_t scalblnf;
    amd_scalbn_ss_func_t scalbnf;
    amd_sin_ss_func_t sinf;
    amd_sincos_ss_func_t sincosf;
    amd_sinh_ss_func_t sinhf;
    amd_sinpi_ss_func_t sinpif;
    amd_sqrt_ss_func_t sqrtf;
    amd_tan_ss_func_t tanf;
    amd_tanh_ss_func_t tanhf;
    amd_tanpi_ss_func_t tanpif;
    amd_trunc_ss_func_t truncf;

    // ============================================================================
    // DOUBLE PRECISION SCALAR (sd) VARIANTS
    // ============================================================================
    amd_acos_sd_func_t acos;
    amd_acosh_sd_func_t acosh;
    amd_asin_sd_func_t asin;
    amd_asinh_sd_func_t asinh;
    amd_atan_sd_func_t atan;
    amd_atan2_sd_func_t atan2;
    amd_atanh_sd_func_t atanh;
    amd_cbrt_sd_func_t cbrt;
    amd_ceil_sd_func_t ceil;
    amd_copysign_sd_func_t copysign;
    amd_cos_sd_func_t cos;
    amd_cosh_sd_func_t cosh;
    amd_cospi_sd_func_t cospi;
    amd_erf_sd_func_t erf;
    amd_erfc_sd_func_t erfc;
    amd_exp_sd_func_t exp;
    amd_exp10_sd_func_t exp10;
    amd_exp2_sd_func_t exp2;
    amd_expm1_sd_func_t expm1;
    amd_fabs_sd_func_t fabs;
    amd_fdim_sd_func_t fdim;
    amd_finite_sd_func_t finite;
    amd_floor_sd_func_t floor;
    amd_fma_sd_func_t fma;
    amd_fmax_sd_func_t fmax;
    amd_fmin_sd_func_t fmin;
    amd_fmod_sd_func_t fmod;
    amd_frexp_sd_func_t frexp;
    amd_hypot_sd_func_t hypot;
    amd_ilogb_sd_func_t ilogb;
    amd_ldexp_sd_func_t ldexp;
    amd_llrint_sd_func_t llrint;
    amd_llround_sd_func_t llround;
    amd_log_sd_func_t log;
    amd_log10_sd_func_t log10;
    amd_log1p_sd_func_t log1p;
    amd_log2_sd_func_t log2;
    amd_logb_sd_func_t logb;
    amd_lrint_sd_func_t lrint;
    amd_lround_sd_func_t lround;
    amd_modf_sd_func_t modf;
    amd_nearbyint_sd_func_t nearbyint;
    amd_nextafter_sd_func_t nextafter;
    amd_nexttoward_sd_func_t nexttoward;
    amd_pow_sd_func_t pow;
    amd_remainder_sd_func_t remainder;
    amd_remquo_sd_func_t remquo;
    amd_rint_sd_func_t rint;
    amd_round_sd_func_t round;
    amd_scalbln_sd_func_t scalbln;
    amd_scalbn_sd_func_t scalbn;
    amd_sin_sd_func_t sin;
    amd_sincos_sd_func_t sincos;
    amd_sinh_sd_func_t sinh;
    amd_sinpi_sd_func_t sinpi;
    amd_sqrt_sd_func_t sqrt;
    amd_tan_sd_func_t tan;
    amd_tanh_sd_func_t tanh;
    amd_tanpi_sd_func_t tanpi;
    amd_trunc_sd_func_t trunc;

    // ============================================================================
    // DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
    // ============================================================================
    amd_acos_vrd2_func_t acos_vrd2;
    amd_asin_vrd2_func_t asin_vrd2;
    amd_atan_vrd2_func_t atan_vrd2;
    amd_cbrt_vrd2_func_t cbrt_vrd2;
    amd_cos_vrd2_func_t cos_vrd2;
    amd_cosh_vrd2_func_t cosh_vrd2;
    amd_erf_vrd2_func_t erf_vrd2;
    amd_erfc_vrd2_func_t erfc_vrd2;
    amd_exp_vrd2_func_t exp_vrd2;
    amd_exp10_vrd2_func_t exp10_vrd2;
    amd_exp2_vrd2_func_t exp2_vrd2;
    amd_fabs_vrd2_func_t fabs_vrd2;
    amd_linearfrac_vrd2_func_t linearfrac_vrd2;
    amd_log_vrd2_func_t log_vrd2;
    amd_log10_vrd2_func_t log10_vrd2;
    amd_log1p_vrd2_func_t log1p_vrd2;
    amd_log2_vrd2_func_t log2_vrd2;
    amd_pow_vrd2_func_t pow_vrd2;
    amd_powx_vrd2_func_t powx_vrd2;
    amd_sin_vrd2_func_t sin_vrd2;
    amd_sincos_vrd2_func_t sincos_vrd2;
    amd_sqrt_vrd2_func_t sqrt_vrd2;
    amd_tan_vrd2_func_t tan_vrd2;

    // ============================================================================
    // SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
    // ============================================================================
    amd_acos_vrs4_func_t acos_vrs4;
    amd_asin_vrs4_func_t asin_vrs4;
    amd_atan_vrs4_func_t atan_vrs4;
    amd_cbrt_vrs4_func_t cbrt_vrs4;
    amd_cos_vrs4_func_t cos_vrs4;
    amd_cosh_vrs4_func_t cosh_vrs4;
    amd_erf_vrs4_func_t erf_vrs4;
    amd_erfc_vrs4_func_t erfc_vrs4;
    amd_exp_vrs4_func_t exp_vrs4;
    amd_exp10_vrs4_func_t exp10_vrs4;
    amd_exp2_vrs4_func_t exp2_vrs4;
    amd_expm1_vrs4_func_t expm1_vrs4;
    amd_fabs_vrs4_func_t fabs_vrs4;
    amd_linearfrac_vrs4_func_t linearfrac_vrs4;
    amd_log_vrs4_func_t log_vrs4;
    amd_log10_vrs4_func_t log10_vrs4;
    amd_log1p_vrs4_func_t log1p_vrs4;
    amd_log2_vrs4_func_t log2_vrs4;
    amd_pow_vrs4_func_t pow_vrs4;
    amd_powx_vrs4_func_t powx_vrs4;
    amd_sin_vrs4_func_t sin_vrs4;
    amd_sincos_vrs4_func_t sincos_vrs4;
    amd_sqrt_vrs4_func_t sqrt_vrs4;
    amd_tan_vrs4_func_t tan_vrs4;
    amd_tanh_vrs4_func_t tanh_vrs4;

    // ============================================================================
    // DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
    // ============================================================================
    amd_acos_vrd4_func_t acos_vrd4;
    amd_asin_vrd4_func_t asin_vrd4;
    amd_atan_vrd4_func_t atan_vrd4;
    amd_cos_vrd4_func_t cos_vrd4;
    amd_erf_vrd4_func_t erf_vrd4;
    amd_erfc_vrd4_func_t erfc_vrd4;
    amd_exp_vrd4_func_t exp_vrd4;
    amd_exp2_vrd4_func_t exp2_vrd4;
    amd_fabs_vrd4_func_t fabs_vrd4;
    amd_linearfrac_vrd4_func_t linearfrac_vrd4;
    amd_log_vrd4_func_t log_vrd4;
    amd_log2_vrd4_func_t log2_vrd4;
    amd_pow_vrd4_func_t pow_vrd4;
    amd_powx_vrd4_func_t powx_vrd4;
    amd_sin_vrd4_func_t sin_vrd4;
    amd_sincos_vrd4_func_t sincos_vrd4;
    amd_sqrt_vrd4_func_t sqrt_vrd4;
    amd_tan_vrd4_func_t tan_vrd4;

    // ============================================================================
    // SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
    // ============================================================================
    amd_acos_vrs8_func_t acos_vrs8;
    amd_asin_vrs8_func_t asin_vrs8;
    amd_atan_vrs8_func_t atan_vrs8;
    amd_cos_vrs8_func_t cos_vrs8;
    amd_cosh_vrs8_func_t cosh_vrs8;
    amd_erf_vrs8_func_t erf_vrs8;
    amd_erfc_vrs8_func_t erfc_vrs8;
    amd_exp_vrs8_func_t exp_vrs8;
    amd_exp2_vrs8_func_t exp2_vrs8;
    amd_fabs_vrs8_func_t fabs_vrs8;
    amd_linearfrac_vrs8_func_t linearfrac_vrs8;
    amd_log_vrs8_func_t log_vrs8;
    amd_log10_vrs8_func_t log10_vrs8;
    amd_log2_vrs8_func_t log2_vrs8;
    amd_pow_vrs8_func_t pow_vrs8;
    amd_powx_vrs8_func_t powx_vrs8;
    amd_sin_vrs8_func_t sin_vrs8;
    amd_sincos_vrs8_func_t sincos_vrs8;
    amd_sqrt_vrs8_func_t sqrt_vrs8;
    amd_tan_vrs8_func_t tan_vrs8;
    amd_tanh_vrs8_func_t tanh_vrs8;

    // ============================================================================
    // SINGLE PRECISION ARRAY (vrsa) VARIANTS
    // ============================================================================
    amd_acos_vrsa_func_t acos_vrsa;
    amd_add_vrsa_func_t add_vrsa;
    amd_addfi_vrsa_func_t addfi_vrsa;
    amd_asin_vrsa_func_t asin_vrsa;
    amd_atan_vrsa_func_t atan_vrsa;
    amd_cbrt_vrsa_func_t cbrt_vrsa;
    amd_cos_vrsa_func_t cos_vrsa;
    amd_cosh_vrsa_func_t cosh_vrsa;
    amd_div_vrsa_func_t div_vrsa;
    amd_divfi_vrsa_func_t divfi_vrsa;
    amd_erf_vrsa_func_t erf_vrsa;
    amd_erfc_vrsa_func_t erfc_vrsa;
    amd_exp_vrsa_func_t exp_vrsa;
    amd_exp10_vrsa_func_t exp10_vrsa;
    amd_exp2_vrsa_func_t exp2_vrsa;
    amd_expm1_vrsa_func_t expm1_vrsa;
    amd_fabs_vrsa_func_t fabs_vrsa;
    amd_fmax_vrsa_func_t fmax_vrsa;
    amd_fmaxfi_vrsa_func_t fmaxfi_vrsa;
    amd_fmin_vrsa_func_t fmin_vrsa;
    amd_fminfi_vrsa_func_t fminfi_vrsa;
    amd_linearfrac_vrsa_func_t linearfrac_vrsa;
    amd_log_vrsa_func_t log_vrsa;
    amd_log10_vrsa_func_t log10_vrsa;
    amd_log1p_vrsa_func_t log1p_vrsa;
    amd_log2_vrsa_func_t log2_vrsa;
    amd_mul_vrsa_func_t mul_vrsa;
    amd_mulfi_vrsa_func_t mulfi_vrsa;
    amd_pow_vrsa_func_t pow_vrsa;
    amd_powx_vrsa_func_t powx_vrsa;
    amd_sin_vrsa_func_t sin_vrsa;
    amd_sincos_vrsa_func_t sincos_vrsa;
    amd_sqrt_vrsa_func_t sqrt_vrsa;
    amd_sub_vrsa_func_t sub_vrsa;
    amd_subfi_vrsa_func_t subfi_vrsa;
    amd_tan_vrsa_func_t tan_vrsa;
    amd_tanh_vrsa_func_t tanh_vrsa;

    // ============================================================================
    // DOUBLE PRECISION ARRAY (vrda) VARIANTS
    // ============================================================================
    amd_acos_vrda_func_t acos_vrda;
    amd_add_vrda_func_t add_vrda;
    amd_addi_vrda_func_t addi_vrda;
    amd_asin_vrda_func_t asin_vrda;
    amd_atan_vrda_func_t atan_vrda;
    amd_cbrt_vrda_func_t cbrt_vrda;
    amd_cos_vrda_func_t cos_vrda;
    amd_cosh_vrda_func_t cosh_vrda;
    amd_div_vrda_func_t div_vrda;
    amd_divi_vrda_func_t divi_vrda;
    amd_erf_vrda_func_t erf_vrda;
    amd_erfc_vrda_func_t erfc_vrda;
    amd_exp_vrda_func_t exp_vrda;
    amd_exp10_vrda_func_t exp10_vrda;
    amd_exp2_vrda_func_t exp2_vrda;
    amd_expm1_vrda_func_t expm1_vrda;
    amd_fabs_vrda_func_t fabs_vrda;
    amd_fmax_vrda_func_t fmax_vrda;
    amd_fmaxi_vrda_func_t fmaxi_vrda;
    amd_fmin_vrda_func_t fmin_vrda;
    amd_fmini_vrda_func_t fmini_vrda;
    amd_linearfrac_vrda_func_t linearfrac_vrda;
    amd_log_vrda_func_t log_vrda;
    amd_log10_vrda_func_t log10_vrda;
    amd_log1p_vrda_func_t log1p_vrda;
    amd_log2_vrda_func_t log2_vrda;
    amd_mul_vrda_func_t mul_vrda;
    amd_muli_vrda_func_t muli_vrda;
    amd_pow_vrda_func_t pow_vrda;
    amd_powx_vrda_func_t powx_vrda;
    amd_sin_vrda_func_t sin_vrda;
    amd_sincos_vrda_func_t sincos_vrda;
    amd_sqrt_vrda_func_t sqrt_vrda;
    amd_sub_vrda_func_t sub_vrda;
    amd_subi_vrda_func_t subi_vrda;
    amd_tan_vrda_func_t tan_vrda;

#ifdef __AVX512F__
    // ============================================================================
    // DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
    // ============================================================================
    amd_asin_vrd8_func_t asin_vrd8;
    amd_atan_vrd8_func_t atan_vrd8;
    amd_cos_vrd8_func_t cos_vrd8;
    amd_erf_vrd8_func_t erf_vrd8;
    amd_erfc_vrd8_func_t erfc_vrd8;
    amd_exp_vrd8_func_t exp_vrd8;
    amd_exp2_vrd8_func_t exp2_vrd8;
    amd_linearfrac_vrd8_func_t linearfrac_vrd8;
    amd_log_vrd8_func_t log_vrd8;
    amd_log2_vrd8_func_t log2_vrd8;
    amd_pow_vrd8_func_t pow_vrd8;
    amd_powx_vrd8_func_t powx_vrd8;
    amd_sin_vrd8_func_t sin_vrd8;
    amd_sincos_vrd8_func_t sincos_vrd8;
    amd_sqrt_vrd8_func_t sqrt_vrd8;
    amd_tan_vrd8_func_t tan_vrd8;

    // ============================================================================
    // SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
    // ============================================================================
    amd_acos_vrs16_func_t acos_vrs16;
    amd_asin_vrs16_func_t asin_vrs16;
    amd_atan_vrs16_func_t atan_vrs16;
    amd_cos_vrs16_func_t cos_vrs16;
    amd_erf_vrs16_func_t erf_vrs16;
    amd_erfc_vrs16_func_t erfc_vrs16;
    amd_exp_vrs16_func_t exp_vrs16;
    amd_exp2_vrs16_func_t exp2_vrs16;
    amd_linearfrac_vrs16_func_t linearfrac_vrs16;
    amd_log_vrs16_func_t log_vrs16;
    amd_log10_vrs16_func_t log10_vrs16;
    amd_log2_vrs16_func_t log2_vrs16;
    amd_pow_vrs16_func_t pow_vrs16;
    amd_powx_vrs16_func_t powx_vrs16;
    amd_sin_vrs16_func_t sin_vrs16;
    amd_sincos_vrs16_func_t sincos_vrs16;
    amd_sqrt_vrs16_func_t sqrt_vrs16;
    amd_tan_vrs16_func_t tan_vrs16;
    amd_tanh_vrs16_func_t tanh_vrs16;
#endif

} amd_funcs = {};


static bool symbols_loaded = false;

template<typename T>
static T load_amd_symbol(LIB_HANDLE lib, const char* name) {
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

#ifndef _WIN32
static void check_amd_installation() {
    const char* ld_library_path = getenv("LD_LIBRARY_PATH");
    std::cout << "LD_LIBRARY_PATH: " << ld_library_path << std::endl;
    if (!ld_library_path) {
        printf("LD_LIBRARY_PATH does not contain AMD paths\n");
    }
}
#endif

// Automatic initialization at library load time
static void init_amd_symbols(void) {

    // Try to load Intel MKL libraries in order of preference
    LIB_HANDLE amd_core = nullptr;

#ifdef _WIN32
    // Windows AMD library names
    amd_core = LOAD_LIBRARY("libalm.dll");
    if (!amd_core) {
        printf("Error: Failed to load AMD Math Function library (libalm.dll)\n");
    }
#else
    check_amd_installation();

    // Linux MKL library loading
    amd_core = LOAD_LIBRARY("libalm.so");
    if (!amd_core) {
        printf("Error: Failed to load AMD Math Function library (libalm.so)\n");
    }
#endif

    // Load symbols organized by variant
    // ============================================================================
    // SINGLE PRECISION SCALAR (ss) VARIANTS
    // ============================================================================
    amd_funcs.acosf = load_amd_symbol<amd_acos_ss_func_t>(amd_core, "amd_acosf");
    amd_funcs.acoshf = load_amd_symbol<amd_acosh_ss_func_t>(amd_core, "amd_acoshf");
    amd_funcs.asinf = load_amd_symbol<amd_asin_ss_func_t>(amd_core, "amd_asinf");
    amd_funcs.asinhf = load_amd_symbol<amd_asinh_ss_func_t>(amd_core, "amd_asinhf");
    amd_funcs.atanf = load_amd_symbol<amd_atan_ss_func_t>(amd_core, "amd_atanf");
    amd_funcs.atan2f = load_amd_symbol<amd_atan2_ss_func_t>(amd_core, "amd_atan2f");
    amd_funcs.atanhf = load_amd_symbol<amd_atanh_ss_func_t>(amd_core, "amd_atanhf");
    amd_funcs.cbrtf = load_amd_symbol<amd_cbrt_ss_func_t>(amd_core, "amd_cbrtf");
    amd_funcs.ceilf = load_amd_symbol<amd_ceil_ss_func_t>(amd_core, "amd_ceilf");
    amd_funcs.copysignf = load_amd_symbol<amd_copysign_ss_func_t>(amd_core, "amd_copysignf");
    amd_funcs.cosf = load_amd_symbol<amd_cos_ss_func_t>(amd_core, "amd_cosf");
    amd_funcs.coshf = load_amd_symbol<amd_cosh_ss_func_t>(amd_core, "amd_coshf");
    amd_funcs.cospif = load_amd_symbol<amd_cospi_ss_func_t>(amd_core, "amd_cospif");
    amd_funcs.erff = load_amd_symbol<amd_erf_ss_func_t>(amd_core, "amd_erff");
    amd_funcs.erfcf = load_amd_symbol<amd_erfc_ss_func_t>(amd_core, "amd_erfcf");
    amd_funcs.expf = load_amd_symbol<amd_exp_ss_func_t>(amd_core, "amd_expf");
    amd_funcs.exp10f = load_amd_symbol<amd_exp10_ss_func_t>(amd_core, "amd_exp10f");
    amd_funcs.exp2f = load_amd_symbol<amd_exp2_ss_func_t>(amd_core, "amd_exp2f");
    amd_funcs.expm1f = load_amd_symbol<amd_expm1_ss_func_t>(amd_core, "amd_expm1f");
    amd_funcs.fabsf = load_amd_symbol<amd_fabs_ss_func_t>(amd_core, "amd_fabsf");
    amd_funcs.fdimf = load_amd_symbol<amd_fdim_ss_func_t>(amd_core, "amd_fdimf");
    amd_funcs.finitef = load_amd_symbol<amd_finite_ss_func_t>(amd_core, "amd_finitef");
    amd_funcs.floorf = load_amd_symbol<amd_floor_ss_func_t>(amd_core, "amd_floorf");
    amd_funcs.fmaf = load_amd_symbol<amd_fma_ss_func_t>(amd_core, "amd_fmaf");
    amd_funcs.fmaxf = load_amd_symbol<amd_fmax_ss_func_t>(amd_core, "amd_fmaxf");
    amd_funcs.fminf = load_amd_symbol<amd_fmin_ss_func_t>(amd_core, "amd_fminf");
    amd_funcs.fmodf = load_amd_symbol<amd_fmod_ss_func_t>(amd_core, "amd_fmodf");
    amd_funcs.frexpf = load_amd_symbol<amd_frexp_ss_func_t>(amd_core, "amd_frexpf");
    amd_funcs.hypotf = load_amd_symbol<amd_hypot_ss_func_t>(amd_core, "amd_hypotf");
    amd_funcs.ilogbf = load_amd_symbol<amd_ilogb_ss_func_t>(amd_core, "amd_ilogbf");
    amd_funcs.ldexpf = load_amd_symbol<amd_ldexp_ss_func_t>(amd_core, "amd_ldexpf");
    amd_funcs.llrintf = load_amd_symbol<amd_llrint_ss_func_t>(amd_core, "amd_llrintf");
    amd_funcs.llroundf = load_amd_symbol<amd_llround_ss_func_t>(amd_core, "amd_llroundf");
    amd_funcs.logf = load_amd_symbol<amd_log_ss_func_t>(amd_core, "amd_logf");
    amd_funcs.log10f = load_amd_symbol<amd_log10_ss_func_t>(amd_core, "amd_log10f");
    amd_funcs.log1pf = load_amd_symbol<amd_log1p_ss_func_t>(amd_core, "amd_log1pf");
    amd_funcs.log2f = load_amd_symbol<amd_log2_ss_func_t>(amd_core, "amd_log2f");
    amd_funcs.logbf = load_amd_symbol<amd_logb_ss_func_t>(amd_core, "amd_logbf");
    amd_funcs.lrintf = load_amd_symbol<amd_lrint_ss_func_t>(amd_core, "amd_lrintf");
    amd_funcs.lroundf = load_amd_symbol<amd_lround_ss_func_t>(amd_core, "amd_lroundf");
    amd_funcs.modff = load_amd_symbol<amd_modf_ss_func_t>(amd_core, "amd_modff");
    amd_funcs.nearbyintf = load_amd_symbol<amd_nearbyint_ss_func_t>(amd_core, "amd_nearbyintf");
    amd_funcs.nextafterf = load_amd_symbol<amd_nextafter_ss_func_t>(amd_core, "amd_nextafterf");
    amd_funcs.nexttowardf = load_amd_symbol<amd_nexttoward_ss_func_t>(amd_core, "amd_nexttowardf");
    amd_funcs.powf = load_amd_symbol<amd_pow_ss_func_t>(amd_core, "amd_powf");
    amd_funcs.remainderf = load_amd_symbol<amd_remainder_ss_func_t>(amd_core, "amd_remainderf");
    amd_funcs.remquof = load_amd_symbol<amd_remquo_ss_func_t>(amd_core, "amd_remquof");
    amd_funcs.rintf = load_amd_symbol<amd_rint_ss_func_t>(amd_core, "amd_rintf");
    amd_funcs.roundf = load_amd_symbol<amd_round_ss_func_t>(amd_core, "amd_roundf");
    amd_funcs.scalblnf = load_amd_symbol<amd_scalbln_ss_func_t>(amd_core, "amd_scalblnf");
    amd_funcs.scalbnf = load_amd_symbol<amd_scalbn_ss_func_t>(amd_core, "amd_scalbnf");
    amd_funcs.sinf = load_amd_symbol<amd_sin_ss_func_t>(amd_core, "amd_sinf");
    amd_funcs.sincosf = load_amd_symbol<amd_sincos_ss_func_t>(amd_core, "amd_sincosf");
    amd_funcs.sinhf = load_amd_symbol<amd_sinh_ss_func_t>(amd_core, "amd_sinhf");
    amd_funcs.sinpif = load_amd_symbol<amd_sinpi_ss_func_t>(amd_core, "amd_sinpif");
    amd_funcs.sqrtf = load_amd_symbol<amd_sqrt_ss_func_t>(amd_core, "amd_sqrtf");
    amd_funcs.tanf = load_amd_symbol<amd_tan_ss_func_t>(amd_core, "amd_tanf");
    amd_funcs.tanhf = load_amd_symbol<amd_tanh_ss_func_t>(amd_core, "amd_tanhf");
    amd_funcs.tanpif = load_amd_symbol<amd_tanpi_ss_func_t>(amd_core, "amd_tanpif");
    amd_funcs.truncf = load_amd_symbol<amd_trunc_ss_func_t>(amd_core, "amd_truncf");

    // ============================================================================
    // DOUBLE PRECISION SCALAR (sd) VARIANTS
    // ============================================================================
    amd_funcs.acos = load_amd_symbol<amd_acos_sd_func_t>(amd_core, "amd_acos");
    amd_funcs.acosh = load_amd_symbol<amd_acosh_sd_func_t>(amd_core, "amd_acosh");
    amd_funcs.asin = load_amd_symbol<amd_asin_sd_func_t>(amd_core, "amd_asin");
    amd_funcs.asinh = load_amd_symbol<amd_asinh_sd_func_t>(amd_core, "amd_asinh");
    amd_funcs.atan = load_amd_symbol<amd_atan_sd_func_t>(amd_core, "amd_atan");
    amd_funcs.atan2 = load_amd_symbol<amd_atan2_sd_func_t>(amd_core, "amd_atan2");
    amd_funcs.atanh = load_amd_symbol<amd_atanh_sd_func_t>(amd_core, "amd_atanh");
    amd_funcs.cbrt = load_amd_symbol<amd_cbrt_sd_func_t>(amd_core, "amd_cbrt");
    amd_funcs.ceil = load_amd_symbol<amd_ceil_sd_func_t>(amd_core, "amd_ceil");
    amd_funcs.copysign = load_amd_symbol<amd_copysign_sd_func_t>(amd_core, "amd_copysign");
    amd_funcs.cos = load_amd_symbol<amd_cos_sd_func_t>(amd_core, "amd_cos");
    amd_funcs.cosh = load_amd_symbol<amd_cosh_sd_func_t>(amd_core, "amd_cosh");
    amd_funcs.cospi = load_amd_symbol<amd_cospi_sd_func_t>(amd_core, "amd_cospi");
    amd_funcs.erf = load_amd_symbol<amd_erf_sd_func_t>(amd_core, "amd_erf");
    amd_funcs.erfc = load_amd_symbol<amd_erfc_sd_func_t>(amd_core, "amd_erfc");
    amd_funcs.exp = load_amd_symbol<amd_exp_sd_func_t>(amd_core, "amd_exp");
    amd_funcs.exp10 = load_amd_symbol<amd_exp10_sd_func_t>(amd_core, "amd_exp10");
    amd_funcs.exp2 = load_amd_symbol<amd_exp2_sd_func_t>(amd_core, "amd_exp2");
    amd_funcs.expm1 = load_amd_symbol<amd_expm1_sd_func_t>(amd_core, "amd_expm1");
    amd_funcs.fabs = load_amd_symbol<amd_fabs_sd_func_t>(amd_core, "amd_fabs");
    amd_funcs.fdim = load_amd_symbol<amd_fdim_sd_func_t>(amd_core, "amd_fdim");
    amd_funcs.finite = load_amd_symbol<amd_finite_sd_func_t>(amd_core, "amd_finite");
    amd_funcs.floor = load_amd_symbol<amd_floor_sd_func_t>(amd_core, "amd_floor");
    amd_funcs.fma = load_amd_symbol<amd_fma_sd_func_t>(amd_core, "amd_fma");
    amd_funcs.fmax = load_amd_symbol<amd_fmax_sd_func_t>(amd_core, "amd_fmax");
    amd_funcs.fmin = load_amd_symbol<amd_fmin_sd_func_t>(amd_core, "amd_fmin");
    amd_funcs.fmod = load_amd_symbol<amd_fmod_sd_func_t>(amd_core, "amd_fmod");
    amd_funcs.frexp = load_amd_symbol<amd_frexp_sd_func_t>(amd_core, "amd_frexp");
    amd_funcs.hypot = load_amd_symbol<amd_hypot_sd_func_t>(amd_core, "amd_hypot");
    amd_funcs.ilogb = load_amd_symbol<amd_ilogb_sd_func_t>(amd_core, "amd_ilogb");
    amd_funcs.ldexp = load_amd_symbol<amd_ldexp_sd_func_t>(amd_core, "amd_ldexp");
    amd_funcs.llrint = load_amd_symbol<amd_llrint_sd_func_t>(amd_core, "amd_llrint");
    amd_funcs.llround = load_amd_symbol<amd_llround_sd_func_t>(amd_core, "amd_llround");
    amd_funcs.log = load_amd_symbol<amd_log_sd_func_t>(amd_core, "amd_log");
    amd_funcs.log10 = load_amd_symbol<amd_log10_sd_func_t>(amd_core, "amd_log10");
    amd_funcs.log1p = load_amd_symbol<amd_log1p_sd_func_t>(amd_core, "amd_log1p");
    amd_funcs.log2 = load_amd_symbol<amd_log2_sd_func_t>(amd_core, "amd_log2");
    amd_funcs.logb = load_amd_symbol<amd_logb_sd_func_t>(amd_core, "amd_logb");
    amd_funcs.lrint = load_amd_symbol<amd_lrint_sd_func_t>(amd_core, "amd_lrint");
    amd_funcs.lround = load_amd_symbol<amd_lround_sd_func_t>(amd_core, "amd_lround");
    amd_funcs.modf = load_amd_symbol<amd_modf_sd_func_t>(amd_core, "amd_modf");
    amd_funcs.nearbyint = load_amd_symbol<amd_nearbyint_sd_func_t>(amd_core, "amd_nearbyint");
    amd_funcs.nextafter = load_amd_symbol<amd_nextafter_sd_func_t>(amd_core, "amd_nextafter");
    amd_funcs.nexttoward = load_amd_symbol<amd_nexttoward_sd_func_t>(amd_core, "amd_nexttoward");
    amd_funcs.pow = load_amd_symbol<amd_pow_sd_func_t>(amd_core, "amd_pow");
    amd_funcs.remainder = load_amd_symbol<amd_remainder_sd_func_t>(amd_core, "amd_remainder");
    amd_funcs.remquo = load_amd_symbol<amd_remquo_sd_func_t>(amd_core, "amd_remquo");
    amd_funcs.rint = load_amd_symbol<amd_rint_sd_func_t>(amd_core, "amd_rint");
    amd_funcs.round = load_amd_symbol<amd_round_sd_func_t>(amd_core, "amd_round");
    amd_funcs.scalbln = load_amd_symbol<amd_scalbln_sd_func_t>(amd_core, "amd_scalbln");
    amd_funcs.scalbn = load_amd_symbol<amd_scalbn_sd_func_t>(amd_core, "amd_scalbn");
    amd_funcs.sin = load_amd_symbol<amd_sin_sd_func_t>(amd_core, "amd_sin");
    amd_funcs.sincos = load_amd_symbol<amd_sincos_sd_func_t>(amd_core, "amd_sincos");
    amd_funcs.sinh = load_amd_symbol<amd_sinh_sd_func_t>(amd_core, "amd_sinh");
    amd_funcs.sinpi = load_amd_symbol<amd_sinpi_sd_func_t>(amd_core, "amd_sinpi");
    amd_funcs.sqrt = load_amd_symbol<amd_sqrt_sd_func_t>(amd_core, "amd_sqrt");
    amd_funcs.tan = load_amd_symbol<amd_tan_sd_func_t>(amd_core, "amd_tan");
    amd_funcs.tanh = load_amd_symbol<amd_tanh_sd_func_t>(amd_core, "amd_tanh");
    amd_funcs.tanpi = load_amd_symbol<amd_tanpi_sd_func_t>(amd_core, "amd_tanpi");
    amd_funcs.trunc = load_amd_symbol<amd_trunc_sd_func_t>(amd_core, "amd_trunc");

    // ============================================================================
    // DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrd2 = load_amd_symbol<amd_acos_vrd2_func_t>(amd_core, "amd_vrd2_acos");
    amd_funcs.asin_vrd2 = load_amd_symbol<amd_asin_vrd2_func_t>(amd_core, "amd_vrd2_asin");
    amd_funcs.atan_vrd2 = load_amd_symbol<amd_atan_vrd2_func_t>(amd_core, "amd_vrd2_atan");
    amd_funcs.cbrt_vrd2 = load_amd_symbol<amd_cbrt_vrd2_func_t>(amd_core, "amd_vrd2_cbrt");
    amd_funcs.cos_vrd2 = load_amd_symbol<amd_cos_vrd2_func_t>(amd_core, "amd_vrd2_cos");
    amd_funcs.cosh_vrd2 = load_amd_symbol<amd_cosh_vrd2_func_t>(amd_core, "amd_vrd2_cosh");
    amd_funcs.erf_vrd2 = load_amd_symbol<amd_erf_vrd2_func_t>(amd_core, "amd_vrd2_erf");
    amd_funcs.erfc_vrd2 = load_amd_symbol<amd_erfc_vrd2_func_t>(amd_core, "amd_vrd2_erfc");
    amd_funcs.exp_vrd2 = load_amd_symbol<amd_exp_vrd2_func_t>(amd_core, "amd_vrd2_exp");
    amd_funcs.exp10_vrd2 = load_amd_symbol<amd_exp10_vrd2_func_t>(amd_core, "amd_vrd2_exp10");
    amd_funcs.exp2_vrd2 = load_amd_symbol<amd_exp2_vrd2_func_t>(amd_core, "amd_vrd2_exp2");
    amd_funcs.fabs_vrd2 = load_amd_symbol<amd_fabs_vrd2_func_t>(amd_core, "amd_vrd2_fabs");
    amd_funcs.linearfrac_vrd2 = load_amd_symbol<amd_linearfrac_vrd2_func_t>(amd_core, "amd_vrd2_linearfrac");
    amd_funcs.log_vrd2 = load_amd_symbol<amd_log_vrd2_func_t>(amd_core, "amd_vrd2_log");
    amd_funcs.log10_vrd2 = load_amd_symbol<amd_log10_vrd2_func_t>(amd_core, "amd_vrd2_log10");
    amd_funcs.log1p_vrd2 = load_amd_symbol<amd_log1p_vrd2_func_t>(amd_core, "amd_vrd2_log1p");
    amd_funcs.log2_vrd2 = load_amd_symbol<amd_log2_vrd2_func_t>(amd_core, "amd_vrd2_log2");
    amd_funcs.pow_vrd2 = load_amd_symbol<amd_pow_vrd2_func_t>(amd_core, "amd_vrd2_pow");
    amd_funcs.powx_vrd2 = load_amd_symbol<amd_powx_vrd2_func_t>(amd_core, "amd_vrd2_powx");
    amd_funcs.sin_vrd2 = load_amd_symbol<amd_sin_vrd2_func_t>(amd_core, "amd_vrd2_sin");
    amd_funcs.sincos_vrd2 = load_amd_symbol<amd_sincos_vrd2_func_t>(amd_core, "amd_vrd2_sincos");
    amd_funcs.sqrt_vrd2 = load_amd_symbol<amd_sqrt_vrd2_func_t>(amd_core, "amd_vrd2_sqrt");
    amd_funcs.tan_vrd2 = load_amd_symbol<amd_tan_vrd2_func_t>(amd_core, "amd_vrd2_tan");

    // ============================================================================
    // SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrs4 = load_amd_symbol<amd_acos_vrs4_func_t>(amd_core, "amd_vrs4_acosf");
    amd_funcs.asin_vrs4 = load_amd_symbol<amd_asin_vrs4_func_t>(amd_core, "amd_vrs4_asinf");
    amd_funcs.atan_vrs4 = load_amd_symbol<amd_atan_vrs4_func_t>(amd_core, "amd_vrs4_atanf");
    amd_funcs.cbrt_vrs4 = load_amd_symbol<amd_cbrt_vrs4_func_t>(amd_core, "amd_vrs4_cbrtf");
    amd_funcs.cos_vrs4 = load_amd_symbol<amd_cos_vrs4_func_t>(amd_core, "amd_vrs4_cosf");
    amd_funcs.cosh_vrs4 = load_amd_symbol<amd_cosh_vrs4_func_t>(amd_core, "amd_vrs4_coshf");
    amd_funcs.erf_vrs4 = load_amd_symbol<amd_erf_vrs4_func_t>(amd_core, "amd_vrs4_erff");
    amd_funcs.erfc_vrs4 = load_amd_symbol<amd_erfc_vrs4_func_t>(amd_core, "amd_vrs4_erfcf");
    amd_funcs.exp_vrs4 = load_amd_symbol<amd_exp_vrs4_func_t>(amd_core, "amd_vrs4_expf");
    amd_funcs.exp10_vrs4 = load_amd_symbol<amd_exp10_vrs4_func_t>(amd_core, "amd_vrs4_exp10f");
    amd_funcs.exp2_vrs4 = load_amd_symbol<amd_exp2_vrs4_func_t>(amd_core, "amd_vrs4_exp2f");
    amd_funcs.expm1_vrs4 = load_amd_symbol<amd_expm1_vrs4_func_t>(amd_core, "amd_vrs4_expm1f");
    amd_funcs.fabs_vrs4 = load_amd_symbol<amd_fabs_vrs4_func_t>(amd_core, "amd_vrs4_fabsf");
    amd_funcs.linearfrac_vrs4 = load_amd_symbol<amd_linearfrac_vrs4_func_t>(amd_core, "amd_vrs4_linearfracf");
    amd_funcs.log_vrs4 = load_amd_symbol<amd_log_vrs4_func_t>(amd_core, "amd_vrs4_logf");
    amd_funcs.log10_vrs4 = load_amd_symbol<amd_log10_vrs4_func_t>(amd_core, "amd_vrs4_log10f");
    amd_funcs.log1p_vrs4 = load_amd_symbol<amd_log1p_vrs4_func_t>(amd_core, "amd_vrs4_log1pf");
    amd_funcs.log2_vrs4 = load_amd_symbol<amd_log2_vrs4_func_t>(amd_core, "amd_vrs4_log2f");
    amd_funcs.pow_vrs4 = load_amd_symbol<amd_pow_vrs4_func_t>(amd_core, "amd_vrs4_powf");
    amd_funcs.powx_vrs4 = load_amd_symbol<amd_powx_vrs4_func_t>(amd_core, "amd_vrs4_powxf");
    amd_funcs.sin_vrs4 = load_amd_symbol<amd_sin_vrs4_func_t>(amd_core, "amd_vrs4_sinf");
    amd_funcs.sincos_vrs4 = load_amd_symbol<amd_sincos_vrs4_func_t>(amd_core, "amd_vrs4_sincosf");
    amd_funcs.sqrt_vrs4 = load_amd_symbol<amd_sqrt_vrs4_func_t>(amd_core, "amd_vrs4_sqrtf");
    amd_funcs.tan_vrs4 = load_amd_symbol<amd_tan_vrs4_func_t>(amd_core, "amd_vrs4_tanf");
    amd_funcs.tanh_vrs4 = load_amd_symbol<amd_tanh_vrs4_func_t>(amd_core, "amd_vrs4_tanhf");

    // ============================================================================
    // DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrd4 = load_amd_symbol<amd_acos_vrd4_func_t>(amd_core, "amd_vrd4_acos");
    amd_funcs.asin_vrd4 = load_amd_symbol<amd_asin_vrd4_func_t>(amd_core, "amd_vrd4_asin");
    amd_funcs.atan_vrd4 = load_amd_symbol<amd_atan_vrd4_func_t>(amd_core, "amd_vrd4_atan");
    amd_funcs.cos_vrd4 = load_amd_symbol<amd_cos_vrd4_func_t>(amd_core, "amd_vrd4_cos");
    amd_funcs.erf_vrd4 = load_amd_symbol<amd_erf_vrd4_func_t>(amd_core, "amd_vrd4_erf");
    amd_funcs.erfc_vrd4 = load_amd_symbol<amd_erfc_vrd4_func_t>(amd_core, "amd_vrd4_erfc");
    amd_funcs.exp_vrd4 = load_amd_symbol<amd_exp_vrd4_func_t>(amd_core, "amd_vrd4_exp");
    amd_funcs.exp2_vrd4 = load_amd_symbol<amd_exp2_vrd4_func_t>(amd_core, "amd_vrd4_exp2");
    amd_funcs.fabs_vrd4 = load_amd_symbol<amd_fabs_vrd4_func_t>(amd_core, "amd_vrd4_fabs");
    amd_funcs.linearfrac_vrd4 = load_amd_symbol<amd_linearfrac_vrd4_func_t>(amd_core, "amd_vrd4_linearfrac");
    amd_funcs.log_vrd4 = load_amd_symbol<amd_log_vrd4_func_t>(amd_core, "amd_vrd4_log");
    amd_funcs.log2_vrd4 = load_amd_symbol<amd_log2_vrd4_func_t>(amd_core, "amd_vrd4_log2");
    amd_funcs.pow_vrd4 = load_amd_symbol<amd_pow_vrd4_func_t>(amd_core, "amd_vrd4_pow");
    amd_funcs.powx_vrd4 = load_amd_symbol<amd_powx_vrd4_func_t>(amd_core, "amd_vrd4_powx");
    amd_funcs.sin_vrd4 = load_amd_symbol<amd_sin_vrd4_func_t>(amd_core, "amd_vrd4_sin");
    amd_funcs.sincos_vrd4 = load_amd_symbol<amd_sincos_vrd4_func_t>(amd_core, "amd_vrd4_sincos");
    amd_funcs.sqrt_vrd4 = load_amd_symbol<amd_sqrt_vrd4_func_t>(amd_core, "amd_vrd4_sqrt");
    amd_funcs.tan_vrd4 = load_amd_symbol<amd_tan_vrd4_func_t>(amd_core, "amd_vrd4_tan");

    // ============================================================================
    // SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrs8 = load_amd_symbol<amd_acos_vrs8_func_t>(amd_core, "amd_vrs8_acosf");
    amd_funcs.asin_vrs8 = load_amd_symbol<amd_asin_vrs8_func_t>(amd_core, "amd_vrs8_asinf");
    amd_funcs.atan_vrs8 = load_amd_symbol<amd_atan_vrs8_func_t>(amd_core, "amd_vrs8_atanf");
    amd_funcs.cos_vrs8 = load_amd_symbol<amd_cos_vrs8_func_t>(amd_core, "amd_vrs8_cosf");
    amd_funcs.cosh_vrs8 = load_amd_symbol<amd_cosh_vrs8_func_t>(amd_core, "amd_vrs8_coshf");
    amd_funcs.erf_vrs8 = load_amd_symbol<amd_erf_vrs8_func_t>(amd_core, "amd_vrs8_erff");
    amd_funcs.erfc_vrs8 = load_amd_symbol<amd_erfc_vrs8_func_t>(amd_core, "amd_vrs8_erfcf");
    amd_funcs.exp_vrs8 = load_amd_symbol<amd_exp_vrs8_func_t>(amd_core, "amd_vrs8_expf");
    amd_funcs.exp2_vrs8 = load_amd_symbol<amd_exp2_vrs8_func_t>(amd_core, "amd_vrs8_exp2f");
    amd_funcs.fabs_vrs8 = load_amd_symbol<amd_fabs_vrs8_func_t>(amd_core, "amd_vrs8_fabsf");
    amd_funcs.linearfrac_vrs8 = load_amd_symbol<amd_linearfrac_vrs8_func_t>(amd_core, "amd_vrs8_linearfracf");
    amd_funcs.log_vrs8 = load_amd_symbol<amd_log_vrs8_func_t>(amd_core, "amd_vrs8_logf");
    amd_funcs.log10_vrs8 = load_amd_symbol<amd_log10_vrs8_func_t>(amd_core, "amd_vrs8_log10f");
    amd_funcs.log2_vrs8 = load_amd_symbol<amd_log2_vrs8_func_t>(amd_core, "amd_vrs8_log2f");
    amd_funcs.pow_vrs8 = load_amd_symbol<amd_pow_vrs8_func_t>(amd_core, "amd_vrs8_powf");
    amd_funcs.powx_vrs8 = load_amd_symbol<amd_powx_vrs8_func_t>(amd_core, "amd_vrs8_powxf");
    amd_funcs.sin_vrs8 = load_amd_symbol<amd_sin_vrs8_func_t>(amd_core, "amd_vrs8_sinf");
    amd_funcs.sincos_vrs8 = load_amd_symbol<amd_sincos_vrs8_func_t>(amd_core, "amd_vrs8_sincosf");
    amd_funcs.sqrt_vrs8 = load_amd_symbol<amd_sqrt_vrs8_func_t>(amd_core, "amd_vrs8_sqrtf");
    amd_funcs.tan_vrs8 = load_amd_symbol<amd_tan_vrs8_func_t>(amd_core, "amd_vrs8_tanf");
    amd_funcs.tanh_vrs8 = load_amd_symbol<amd_tanh_vrs8_func_t>(amd_core, "amd_vrs8_tanhf");

    // ============================================================================
    // SINGLE PRECISION ARRAY (vrsa) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrsa = load_amd_symbol<amd_acos_vrsa_func_t>(amd_core, "amd_vrsa_acosf");
    amd_funcs.add_vrsa = load_amd_symbol<amd_add_vrsa_func_t>(amd_core, "amd_vrsa_addf");
    amd_funcs.addfi_vrsa = load_amd_symbol<amd_addfi_vrsa_func_t>(amd_core, "amd_vrsa_addfi");
    amd_funcs.asin_vrsa = load_amd_symbol<amd_asin_vrsa_func_t>(amd_core, "amd_vrsa_asinf");
    amd_funcs.atan_vrsa = load_amd_symbol<amd_atan_vrsa_func_t>(amd_core, "amd_vrsa_atanf");
    amd_funcs.cbrt_vrsa = load_amd_symbol<amd_cbrt_vrsa_func_t>(amd_core, "amd_vrsa_cbrtf");
    amd_funcs.cos_vrsa = load_amd_symbol<amd_cos_vrsa_func_t>(amd_core, "amd_vrsa_cosf");
    amd_funcs.cosh_vrsa = load_amd_symbol<amd_cosh_vrsa_func_t>(amd_core, "amd_vrsa_coshf");
    amd_funcs.div_vrsa = load_amd_symbol<amd_div_vrsa_func_t>(amd_core, "amd_vrsa_divf");
    amd_funcs.divfi_vrsa = load_amd_symbol<amd_divfi_vrsa_func_t>(amd_core, "amd_vrsa_divfi");
    amd_funcs.erf_vrsa = load_amd_symbol<amd_erf_vrsa_func_t>(amd_core, "amd_vrsa_erff");
    amd_funcs.erfc_vrsa = load_amd_symbol<amd_erfc_vrsa_func_t>(amd_core, "amd_vrsa_erfcf");
    amd_funcs.exp_vrsa = load_amd_symbol<amd_exp_vrsa_func_t>(amd_core, "amd_vrsa_expf");
    amd_funcs.exp10_vrsa = load_amd_symbol<amd_exp10_vrsa_func_t>(amd_core, "amd_vrsa_exp10f");
    amd_funcs.exp2_vrsa = load_amd_symbol<amd_exp2_vrsa_func_t>(amd_core, "amd_vrsa_exp2f");
    amd_funcs.expm1_vrsa = load_amd_symbol<amd_expm1_vrsa_func_t>(amd_core, "amd_vrsa_expm1f");
    amd_funcs.fabs_vrsa = load_amd_symbol<amd_fabs_vrsa_func_t>(amd_core, "amd_vrsa_fabsf");
    amd_funcs.fmax_vrsa = load_amd_symbol<amd_fmax_vrsa_func_t>(amd_core, "amd_vrsa_fmaxf");
    amd_funcs.fmaxfi_vrsa = load_amd_symbol<amd_fmaxfi_vrsa_func_t>(amd_core, "amd_vrsa_fmaxfi");
    amd_funcs.fmin_vrsa = load_amd_symbol<amd_fmin_vrsa_func_t>(amd_core, "amd_vrsa_fminf");
    amd_funcs.fminfi_vrsa = load_amd_symbol<amd_fminfi_vrsa_func_t>(amd_core, "amd_vrsa_fminfi");
    amd_funcs.linearfrac_vrsa = load_amd_symbol<amd_linearfrac_vrsa_func_t>(amd_core, "amd_vrsa_linearfracf");
    amd_funcs.log_vrsa = load_amd_symbol<amd_log_vrsa_func_t>(amd_core, "amd_vrsa_logf");
    amd_funcs.log10_vrsa = load_amd_symbol<amd_log10_vrsa_func_t>(amd_core, "amd_vrsa_log10f");
    amd_funcs.log1p_vrsa = load_amd_symbol<amd_log1p_vrsa_func_t>(amd_core, "amd_vrsa_log1pf");
    amd_funcs.log2_vrsa = load_amd_symbol<amd_log2_vrsa_func_t>(amd_core, "amd_vrsa_log2f");
    amd_funcs.mul_vrsa = load_amd_symbol<amd_mul_vrsa_func_t>(amd_core, "amd_vrsa_mulf");
    amd_funcs.mulfi_vrsa = load_amd_symbol<amd_mulfi_vrsa_func_t>(amd_core, "amd_vrsa_mulfi");
    amd_funcs.pow_vrsa = load_amd_symbol<amd_pow_vrsa_func_t>(amd_core, "amd_vrsa_powf");
    amd_funcs.powx_vrsa = load_amd_symbol<amd_powx_vrsa_func_t>(amd_core, "amd_vrsa_powxf");
    amd_funcs.sin_vrsa = load_amd_symbol<amd_sin_vrsa_func_t>(amd_core, "amd_vrsa_sinf");
    amd_funcs.sincos_vrsa = load_amd_symbol<amd_sincos_vrsa_func_t>(amd_core, "amd_vrsa_sincosf");
    amd_funcs.sqrt_vrsa = load_amd_symbol<amd_sqrt_vrsa_func_t>(amd_core, "amd_vrsa_sqrtf");
    amd_funcs.sub_vrsa = load_amd_symbol<amd_sub_vrsa_func_t>(amd_core, "amd_vrsa_subf");
    amd_funcs.subfi_vrsa = load_amd_symbol<amd_subfi_vrsa_func_t>(amd_core, "amd_vrsa_subfi");
    amd_funcs.tan_vrsa = load_amd_symbol<amd_tan_vrsa_func_t>(amd_core, "amd_vrsa_tanf");
    amd_funcs.tanh_vrsa = load_amd_symbol<amd_tanh_vrsa_func_t>(amd_core, "amd_vrsa_tanhf");

    // ============================================================================
    // DOUBLE PRECISION ARRAY (vrda) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrda = load_amd_symbol<amd_acos_vrda_func_t>(amd_core, "amd_vrda_acos");
    amd_funcs.add_vrda = load_amd_symbol<amd_add_vrda_func_t>(amd_core, "amd_vrda_add");
    amd_funcs.addi_vrda = load_amd_symbol<amd_addi_vrda_func_t>(amd_core, "amd_vrda_addi");
    amd_funcs.asin_vrda = load_amd_symbol<amd_asin_vrda_func_t>(amd_core, "amd_vrda_asin");
    amd_funcs.atan_vrda = load_amd_symbol<amd_atan_vrda_func_t>(amd_core, "amd_vrda_atan");
    amd_funcs.cbrt_vrda = load_amd_symbol<amd_cbrt_vrda_func_t>(amd_core, "amd_vrda_cbrt");
    amd_funcs.cos_vrda = load_amd_symbol<amd_cos_vrda_func_t>(amd_core, "amd_vrda_cos");
    amd_funcs.cosh_vrda = load_amd_symbol<amd_cosh_vrda_func_t>(amd_core, "amd_vrda_cosh");
    amd_funcs.div_vrda = load_amd_symbol<amd_div_vrda_func_t>(amd_core, "amd_vrda_div");
    amd_funcs.divi_vrda = load_amd_symbol<amd_divi_vrda_func_t>(amd_core, "amd_vrda_divi");
    amd_funcs.erf_vrda = load_amd_symbol<amd_erf_vrda_func_t>(amd_core, "amd_vrda_erf");
    amd_funcs.erfc_vrda = load_amd_symbol<amd_erfc_vrda_func_t>(amd_core, "amd_vrda_erfc");
    amd_funcs.exp_vrda = load_amd_symbol<amd_exp_vrda_func_t>(amd_core, "amd_vrda_exp");
    amd_funcs.exp10_vrda = load_amd_symbol<amd_exp10_vrda_func_t>(amd_core, "amd_vrda_exp10");
    amd_funcs.exp2_vrda = load_amd_symbol<amd_exp2_vrda_func_t>(amd_core, "amd_vrda_exp2");
    amd_funcs.expm1_vrda = load_amd_symbol<amd_expm1_vrda_func_t>(amd_core, "amd_vrda_expm1");
    amd_funcs.fabs_vrda = load_amd_symbol<amd_fabs_vrda_func_t>(amd_core, "amd_vrda_fabs");
    amd_funcs.fmax_vrda = load_amd_symbol<amd_fmax_vrda_func_t>(amd_core, "amd_vrda_fmax");
    amd_funcs.fmaxi_vrda = load_amd_symbol<amd_fmaxi_vrda_func_t>(amd_core, "amd_vrda_fmaxi");
    amd_funcs.fmin_vrda = load_amd_symbol<amd_fmin_vrda_func_t>(amd_core, "amd_vrda_fmin");
    amd_funcs.fmini_vrda = load_amd_symbol<amd_fmini_vrda_func_t>(amd_core, "amd_vrda_fmini");
    amd_funcs.linearfrac_vrda = load_amd_symbol<amd_linearfrac_vrda_func_t>(amd_core, "amd_vrda_linearfrac");
    amd_funcs.log_vrda = load_amd_symbol<amd_log_vrda_func_t>(amd_core, "amd_vrda_log");
    amd_funcs.log10_vrda = load_amd_symbol<amd_log10_vrda_func_t>(amd_core, "amd_vrda_log10");
    amd_funcs.log1p_vrda = load_amd_symbol<amd_log1p_vrda_func_t>(amd_core, "amd_vrda_log1p");
    amd_funcs.log2_vrda = load_amd_symbol<amd_log2_vrda_func_t>(amd_core, "amd_vrda_log2");
    amd_funcs.mul_vrda = load_amd_symbol<amd_mul_vrda_func_t>(amd_core, "amd_vrda_mul");
    amd_funcs.muli_vrda = load_amd_symbol<amd_muli_vrda_func_t>(amd_core, "amd_vrda_muli");
    amd_funcs.pow_vrda = load_amd_symbol<amd_pow_vrda_func_t>(amd_core, "amd_vrda_pow");
    amd_funcs.powx_vrda = load_amd_symbol<amd_powx_vrda_func_t>(amd_core, "amd_vrda_powx");
    amd_funcs.sin_vrda = load_amd_symbol<amd_sin_vrda_func_t>(amd_core, "amd_vrda_sin");
    amd_funcs.sincos_vrda = load_amd_symbol<amd_sincos_vrda_func_t>(amd_core, "amd_vrda_sincos");
    amd_funcs.sqrt_vrda = load_amd_symbol<amd_sqrt_vrda_func_t>(amd_core, "amd_vrda_sqrt");
    amd_funcs.sub_vrda = load_amd_symbol<amd_sub_vrda_func_t>(amd_core, "amd_vrda_sub");
    amd_funcs.subi_vrda = load_amd_symbol<amd_subi_vrda_func_t>(amd_core, "amd_vrda_subi");
    amd_funcs.tan_vrda = load_amd_symbol<amd_tan_vrda_func_t>(amd_core, "amd_vrda_tan");

#ifdef __AVX512F__
    // ============================================================================
    // DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
    // ============================================================================
    amd_funcs.asin_vrd8 = load_amd_symbol<amd_asin_vrd8_func_t>(amd_core, "amd_vrd8_asin");
    amd_funcs.atan_vrd8 = load_amd_symbol<amd_atan_vrd8_func_t>(amd_core, "amd_vrd8_atan");
    amd_funcs.cos_vrd8 = load_amd_symbol<amd_cos_vrd8_func_t>(amd_core, "amd_vrd8_cos");
    amd_funcs.erf_vrd8 = load_amd_symbol<amd_erf_vrd8_func_t>(amd_core, "amd_vrd8_erf");
    amd_funcs.erfc_vrd8 = load_amd_symbol<amd_erfc_vrd8_func_t>(amd_core, "amd_vrd8_erfc");
    amd_funcs.exp_vrd8 = load_amd_symbol<amd_exp_vrd8_func_t>(amd_core, "amd_vrd8_exp");
    amd_funcs.exp2_vrd8 = load_amd_symbol<amd_exp2_vrd8_func_t>(amd_core, "amd_vrd8_exp2");
    amd_funcs.linearfrac_vrd8 = load_amd_symbol<amd_linearfrac_vrd8_func_t>(amd_core, "amd_vrd8_linearfrac");
    amd_funcs.log_vrd8 = load_amd_symbol<amd_log_vrd8_func_t>(amd_core, "amd_vrd8_log");
    amd_funcs.log2_vrd8 = load_amd_symbol<amd_log2_vrd8_func_t>(amd_core, "amd_vrd8_log2");
    amd_funcs.pow_vrd8 = load_amd_symbol<amd_pow_vrd8_func_t>(amd_core, "amd_vrd8_pow");
    amd_funcs.powx_vrd8 = load_amd_symbol<amd_powx_vrd8_func_t>(amd_core, "amd_vrd8_powx");
    amd_funcs.sin_vrd8 = load_amd_symbol<amd_sin_vrd8_func_t>(amd_core, "amd_vrd8_sin");
    amd_funcs.sincos_vrd8 = load_amd_symbol<amd_sincos_vrd8_func_t>(amd_core, "amd_vrd8_sincos");
    amd_funcs.sqrt_vrd8 = load_amd_symbol<amd_sqrt_vrd8_func_t>(amd_core, "amd_vrd8_sqrt");
    amd_funcs.tan_vrd8 = load_amd_symbol<amd_tan_vrd8_func_t>(amd_core, "amd_vrd8_tan");

    // ============================================================================
    // SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
    // ============================================================================
    amd_funcs.acos_vrs16 = load_amd_symbol<amd_acos_vrs16_func_t>(amd_core, "amd_vrs16_acosf");
    amd_funcs.asin_vrs16 = load_amd_symbol<amd_asin_vrs16_func_t>(amd_core, "amd_vrs16_asinf");
    amd_funcs.atan_vrs16 = load_amd_symbol<amd_atan_vrs16_func_t>(amd_core, "amd_vrs16_atanf");
    amd_funcs.cos_vrs16 = load_amd_symbol<amd_cos_vrs16_func_t>(amd_core, "amd_vrs16_cosf");
    amd_funcs.erf_vrs16 = load_amd_symbol<amd_erf_vrs16_func_t>(amd_core, "amd_vrs16_erff");
    amd_funcs.erfc_vrs16 = load_amd_symbol<amd_erfc_vrs16_func_t>(amd_core, "amd_vrs16_erfcf");
    amd_funcs.exp_vrs16 = load_amd_symbol<amd_exp_vrs16_func_t>(amd_core, "amd_vrs16_expf");
    amd_funcs.exp2_vrs16 = load_amd_symbol<amd_exp2_vrs16_func_t>(amd_core, "amd_vrs16_exp2f");
    amd_funcs.linearfrac_vrs16 = load_amd_symbol<amd_linearfrac_vrs16_func_t>(amd_core, "amd_vrs16_linearfracf");
    amd_funcs.log_vrs16 = load_amd_symbol<amd_log_vrs16_func_t>(amd_core, "amd_vrs16_logf");
    amd_funcs.log10_vrs16 = load_amd_symbol<amd_log10_vrs16_func_t>(amd_core, "amd_vrs16_log10f");
    amd_funcs.log2_vrs16 = load_amd_symbol<amd_log2_vrs16_func_t>(amd_core, "amd_vrs16_log2f");
    amd_funcs.pow_vrs16 = load_amd_symbol<amd_pow_vrs16_func_t>(amd_core, "amd_vrs16_powf");
    amd_funcs.powx_vrs16 = load_amd_symbol<amd_powx_vrs16_func_t>(amd_core, "amd_vrs16_powxf");
    amd_funcs.sin_vrs16 = load_amd_symbol<amd_sin_vrs16_func_t>(amd_core, "amd_vrs16_sinf");
    amd_funcs.sincos_vrs16 = load_amd_symbol<amd_sincos_vrs16_func_t>(amd_core, "amd_vrs16_sincosf");
    amd_funcs.sqrt_vrs16 = load_amd_symbol<amd_sqrt_vrs16_func_t>(amd_core, "amd_vrs16_sqrtf");
    amd_funcs.tan_vrs16 = load_amd_symbol<amd_tan_vrs16_func_t>(amd_core, "amd_vrs16_tanf");
    amd_funcs.tanh_vrs16 = load_amd_symbol<amd_tanh_vrs16_func_t>(amd_core, "amd_vrs16_tanhf");
#endif

    symbols_loaded = true;
}

#ifdef _WIN32
// Windows DLL entry point for automatic initialization
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    // Suppress unused parameter warnings
    (void)hinstDLL;
    (void)lpvReserved;

    HMODULE hAmdLib;

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            hAmdLib = GetModuleHandleA("libalm.dll");
            if (!hAmdLib) {
                hAmdLib = LoadLibraryA("libalm.dll");
                if (!hAmdLib) {
                    DEBUG_PRINT("AMD library (libalm.dll) not found - continuing without it\n");
                }
            }
            init_amd_symbols();
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}
#else
// Linux constructor for automatic initialization
__attribute__((constructor))
static void lib_init(void) {
    init_amd_symbols();
}
#endif

extern "C" {

SHIM_EXPORT const char* get_vendor_name() {
    return "AMD";
}

// ============================================================================
// High-performance shim functions - Direct function pointer calls (no checks)
// Organized by variant to match struct definition order
// ============================================================================
// SINGLE PRECISION SCALAR (ss) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.acosf(ipp->ip[0]);
}

SHIM_EXPORT void shim_acosh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.acoshf(ipp->ip[0]);
}

SHIM_EXPORT void shim_asin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.asinf(ipp->ip[0]);
}

SHIM_EXPORT void shim_asinh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.asinhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.atanf(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.atan2f(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_atanh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.atanhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cbrt_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.cbrtf(ipp->ip[0]);
}

SHIM_EXPORT void shim_ceil_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.ceilf(ipp->ip[0]);
}

SHIM_EXPORT void shim_copysign_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.copysignf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_cos_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.cosf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cosh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.coshf(ipp->ip[0]);
}

SHIM_EXPORT void shim_cospi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.cospif(ipp->ip[0]);
}

SHIM_EXPORT void shim_erf_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.erff(ipp->ip[0]);
}

SHIM_EXPORT void shim_erfc_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.erfcf(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.expf(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp10_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.exp10f(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.exp2f(ipp->ip[0]);
}

SHIM_EXPORT void shim_expm1_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.expm1f(ipp->ip[0]);
}

SHIM_EXPORT void shim_fabs_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.fabsf(ipp->ip[0]);
}

SHIM_EXPORT void shim_fdim_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.fdimf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_finite_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(amd_funcs.finitef(ipp->ip[0]));
}

SHIM_EXPORT void shim_floor_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.floorf(ipp->ip[0]);
}

SHIM_EXPORT void shim_fma_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.fmaf(ipp->ip[0], ipp->ip[1], ipp->ip[2]);
}

SHIM_EXPORT void shim_fmax_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.fmaxf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.fminf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmod_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.fmodf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_frexp_ss(InParams<float, float> *ipp) {
    int exp;
    ipp->op[0] = amd_funcs.frexpf(ipp->ip[0], &exp);
    ipp->op[1] = static_cast<float>(exp);
}

SHIM_EXPORT void shim_hypot_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.hypotf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_ilogb_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(amd_funcs.ilogbf(ipp->ip[0]));
}

SHIM_EXPORT void shim_ldexp_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.ldexpf(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_llrint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(amd_funcs.llrintf(ipp->ip[0]));
}

SHIM_EXPORT void shim_llround_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(amd_funcs.llroundf(ipp->ip[0]));
}

SHIM_EXPORT void shim_log_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.logf(ipp->ip[0]);
}

SHIM_EXPORT void shim_log10_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.log10f(ipp->ip[0]);
}

SHIM_EXPORT void shim_log1p_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.log1pf(ipp->ip[0]);
}

SHIM_EXPORT void shim_log2_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.log2f(ipp->ip[0]);
}

SHIM_EXPORT void shim_logb_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.logbf(ipp->ip[0]);
}

SHIM_EXPORT void shim_lrint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(amd_funcs.lrintf(ipp->ip[0]));
}

SHIM_EXPORT void shim_lround_ss(InParams<float, float> *ipp) {
    ipp->op[0] = static_cast<float>(amd_funcs.lroundf(ipp->ip[0]));
}

SHIM_EXPORT void shim_modf_ss(InParams<float, float> *ipp) {
    float ipart;
    ipp->op[0] = amd_funcs.modff(ipp->ip[0], &ipart);
    ipp->op[1] = ipart;
}

SHIM_EXPORT void shim_nearbyint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.nearbyintf(ipp->ip[0]);
}

SHIM_EXPORT void shim_nextafter_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.nextafterf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_nexttoward_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.nexttowardf(ipp->ip[0], static_cast<long double>(ipp->ip[1]));
}

SHIM_EXPORT void shim_pow_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.powf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remainder_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.remainderf(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remquo_ss(InParams<float, float> *ipp) {
    int quo;
    ipp->op[0] = amd_funcs.remquof(ipp->ip[0], ipp->ip[1], &quo);
    ipp->op[1] = static_cast<float>(quo);
}

SHIM_EXPORT void shim_rint_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.rintf(ipp->ip[0]);
}

SHIM_EXPORT void shim_round_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.roundf(ipp->ip[0]);
}

SHIM_EXPORT void shim_scalbln_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.scalblnf(ipp->ip[0], static_cast<long>(ipp->ip[1]));
}

SHIM_EXPORT void shim_scalbn_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.scalbnf(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_sin_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.sinf(ipp->ip[0]);
}

SHIM_EXPORT void shim_sincos_ss(InParams<float, float> *ipp) {
    amd_funcs.sincosf(ipp->ip[0], &ipp->op[0], &ipp->op[1]);
}

SHIM_EXPORT void shim_sinh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.sinhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_sinpi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.sinpif(ipp->ip[0]);
}

SHIM_EXPORT void shim_sqrt_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.sqrtf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tan_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.tanf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanh_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.tanhf(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanpi_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.tanpif(ipp->ip[0]);
}

SHIM_EXPORT void shim_trunc_ss(InParams<float, float> *ipp) {
    ipp->op[0] = amd_funcs.truncf(ipp->ip[0]);
}

// ============================================================================
// DOUBLE PRECISION SCALAR (sd) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.acos(ipp->ip[0]);
}

SHIM_EXPORT void shim_acosh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.acosh(ipp->ip[0]);
}

SHIM_EXPORT void shim_asin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.asin(ipp->ip[0]);
}

SHIM_EXPORT void shim_asinh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.asinh(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.atan(ipp->ip[0]);
}

SHIM_EXPORT void shim_atan2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.atan2(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_atanh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.atanh(ipp->ip[0]);
}

SHIM_EXPORT void shim_cbrt_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.cbrt(ipp->ip[0]);
}

SHIM_EXPORT void shim_ceil_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.ceil(ipp->ip[0]);
}

SHIM_EXPORT void shim_copysign_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.copysign(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_cos_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.cos(ipp->ip[0]);
}

SHIM_EXPORT void shim_cosh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.cosh(ipp->ip[0]);
}

SHIM_EXPORT void shim_cospi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.cospi(ipp->ip[0]);
}

SHIM_EXPORT void shim_erf_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.erf(ipp->ip[0]);
}

SHIM_EXPORT void shim_erfc_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.erfc(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.exp(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp10_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.exp10(ipp->ip[0]);
}

SHIM_EXPORT void shim_exp2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.exp2(ipp->ip[0]);
}

SHIM_EXPORT void shim_expm1_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.expm1(ipp->ip[0]);
}

SHIM_EXPORT void shim_fabs_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.fabs(ipp->ip[0]);
}

SHIM_EXPORT void shim_fdim_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.fdim(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_finite_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(amd_funcs.finite(ipp->ip[0]));
}

SHIM_EXPORT void shim_floor_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.floor(ipp->ip[0]);
}

SHIM_EXPORT void shim_fma_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.fma(ipp->ip[0], ipp->ip[1], ipp->ip[2]);
}

SHIM_EXPORT void shim_fmax_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.fmax(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.fmin(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_fmod_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.fmod(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_frexp_sd(InParams<double, double> *ipp) {
    int exp;
    ipp->op[0] = amd_funcs.frexp(ipp->ip[0], &exp);
    ipp->op[1] = static_cast<double>(exp);
}

SHIM_EXPORT void shim_hypot_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.hypot(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_ilogb_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(amd_funcs.ilogb(ipp->ip[0]));
}

SHIM_EXPORT void shim_ldexp_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.ldexp(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_llrint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(amd_funcs.llrint(ipp->ip[0]));
}

SHIM_EXPORT void shim_llround_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(amd_funcs.llround(ipp->ip[0]));
}

SHIM_EXPORT void shim_log_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.log(ipp->ip[0]);
}

SHIM_EXPORT void shim_log10_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.log10(ipp->ip[0]);
}

SHIM_EXPORT void shim_log1p_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.log1p(ipp->ip[0]);
}

SHIM_EXPORT void shim_log2_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.log2(ipp->ip[0]);
}

SHIM_EXPORT void shim_logb_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.logb(ipp->ip[0]);
}

SHIM_EXPORT void shim_lrint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(amd_funcs.lrint(ipp->ip[0]));
}

SHIM_EXPORT void shim_lround_sd(InParams<double, double> *ipp) {
    ipp->op[0] = static_cast<double>(amd_funcs.lround(ipp->ip[0]));
}

SHIM_EXPORT void shim_modf_sd(InParams<double, double> *ipp) {
    double ipart;
    ipp->op[0] = amd_funcs.modf(ipp->ip[0], &ipart);
    ipp->op[1] = ipart;
}

SHIM_EXPORT void shim_nearbyint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.nearbyint(ipp->ip[0]);
}

SHIM_EXPORT void shim_nextafter_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.nextafter(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_nexttoward_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.nexttoward(ipp->ip[0], static_cast<long double>(ipp->ip[1]));
}

SHIM_EXPORT void shim_pow_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.pow(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remainder_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.remainder(ipp->ip[0], ipp->ip[1]);
}

SHIM_EXPORT void shim_remquo_sd(InParams<double, double> *ipp) {
    int quo;
    ipp->op[0] = amd_funcs.remquo(ipp->ip[0], ipp->ip[1], &quo);
    ipp->op[1] = static_cast<double>(quo);
}

SHIM_EXPORT void shim_rint_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.rint(ipp->ip[0]);
}

SHIM_EXPORT void shim_round_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.round(ipp->ip[0]);
}

SHIM_EXPORT void shim_scalbln_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.scalbln(ipp->ip[0], static_cast<long>(ipp->ip[1]));
}

SHIM_EXPORT void shim_scalbn_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.scalbn(ipp->ip[0], static_cast<int>(ipp->ip[1]));
}

SHIM_EXPORT void shim_sin_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.sin(ipp->ip[0]);
}

SHIM_EXPORT void shim_sincos_sd(InParams<double, double> *ipp) {
    amd_funcs.sincos(ipp->ip[0], &ipp->op[0], &ipp->op[1]);
}

SHIM_EXPORT void shim_sinh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.sinh(ipp->ip[0]);
}

SHIM_EXPORT void shim_sinpi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.sinpi(ipp->ip[0]);
}

SHIM_EXPORT void shim_sqrt_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.sqrt(ipp->ip[0]);
}

SHIM_EXPORT void shim_tan_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.tan(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanh_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.tanh(ipp->ip[0]);
}

SHIM_EXPORT void shim_tanpi_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.tanpi(ipp->ip[0]);
}

SHIM_EXPORT void shim_trunc_sd(InParams<double, double> *ipp) {
    ipp->op[0] = amd_funcs.trunc(ipp->ip[0]);
}

// ============================================================================
// DOUBLE PRECISION 128-BIT VECTOR (vrd2) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.acos_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.asin_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.atan_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cbrt_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.cbrt_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.cos_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.cosh_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.erf_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.erfc_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp10_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp10_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp2_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.fabs_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    __m128d ipa = ipp->ip[0].data;
    __m128d ipb = ipp->ip[1].data;
    double sca  = ipp->ip[2].data[0];
    double scb  = ipp->ip[3].data[0];
    double sfta = ipp->ip[4].data[0];
    double sftb = ipp->ip[5].data[0];
    ipp->op[0].data = amd_funcs.linearfrac_vrd2(ipa, ipb, sca, sfta, scb, sftb);
}

SHIM_EXPORT void shim_log_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log10_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log1p_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log1p_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log2_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.pow_vrd2(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.powx_vrd2(ipp->ip[0].data, ipp->ip[1].data[0]);
}

SHIM_EXPORT void shim_sin_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.sin_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    amd_funcs.sincos_vrd2(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.sqrt_vrd2(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd2(InParams<libm::AlignedM128d, double> *ipp) {
    ipp->op[0].data = amd_funcs.tan_vrd2(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 128-BIT VECTOR (vrs4) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.acos_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.asin_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.atan_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cbrt_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.cbrt_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.cos_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.cosh_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.erf_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.erfc_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp10_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp10_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp2_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_expm1_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.expm1_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.fabs_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    __m128 ipa = ipp->ip[0].data;
    __m128 ipb = ipp->ip[1].data;
    float sca  = ipp->ip[2].data[0];
    float scb  = ipp->ip[3].data[0];
    float sfta = ipp->ip[4].data[0];
    float sftb = ipp->ip[5].data[0];
    ipp->op[0].data = amd_funcs.linearfrac_vrs4(ipa, ipb, sca, sfta, scb, sftb);
}

SHIM_EXPORT void shim_log_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.log_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.log10_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log1p_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.log1p_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.log2_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.pow_vrs4(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.powx_vrs4(ipp->ip[0].data, ipp->ip[1].data[0]);
}

SHIM_EXPORT void shim_sin_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.sin_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    amd_funcs.sincos_vrs4(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.sqrt_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.tan_vrs4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs4(InParams<libm::AlignedM128, float> *ipp) {
    ipp->op[0].data = amd_funcs.tanh_vrs4(ipp->ip[0].data);
}

// ============================================================================
// DOUBLE PRECISION 256-BIT VECTOR (vrd4) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.acos_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.asin_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.atan_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.cos_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.erf_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.erfc_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp2_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.fabs_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    __m256d ipa = ipp->ip[0].data;
    __m256d ipb = ipp->ip[1].data;
    double sca  = ipp->ip[2].data[0];
    double scb  = ipp->ip[3].data[0];
    double sfta = ipp->ip[4].data[0];
    double sftb = ipp->ip[5].data[0];
    ipp->op[0].data = amd_funcs.linearfrac_vrd4(ipa, ipb, sca, sfta, scb, sftb);
}

SHIM_EXPORT void shim_log_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log2_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.pow_vrd4(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.powx_vrd4(ipp->ip[0].data, ipp->ip[1].data[0]);
}

SHIM_EXPORT void shim_sin_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.sin_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    amd_funcs.sincos_vrd4(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.sqrt_vrd4(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd4(InParams<libm::AlignedM256d, double> *ipp) {
    ipp->op[0].data = amd_funcs.tan_vrd4(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 256-BIT VECTOR (vrs8) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.acos_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.asin_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.atan_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.cos_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cosh_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.cosh_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.erf_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.erfc_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp2_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_fabs_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.fabs_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    __m256 ipa = ipp->ip[0].data;
    __m256 ipb = ipp->ip[1].data;
    float sca  = ipp->ip[2].data[0];
    float scb  = ipp->ip[3].data[0];
    float sfta = ipp->ip[4].data[0];
    float sftb = ipp->ip[5].data[0];
    ipp->op[0].data = amd_funcs.linearfrac_vrs8(ipa, ipb, sca, scb, sfta, sftb);
}

SHIM_EXPORT void shim_log_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.log_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.log10_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.log2_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.pow_vrs8(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.powx_vrs8(ipp->ip[0].data, ipp->ip[1].data[0]);
}

SHIM_EXPORT void shim_sin_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.sin_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    amd_funcs.sincos_vrs8(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.sqrt_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.tan_vrs8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs8(InParams<libm::AlignedM256, float> *ipp) {
    ipp->op[0].data = amd_funcs.tanh_vrs8(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION ARRAY (vrsa) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrsa(InParams<float, float> *ipp) {
    amd_funcs.acos_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_add_vrsa(InParams<float, float> *ipp) {
    amd_funcs.add_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_addfi_vrsa(InParams<float, float> *ipp) {
    amd_funcs.addfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_asin_vrsa(InParams<float, float> *ipp) {
    amd_funcs.asin_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_atan_vrsa(InParams<float, float> *ipp) {
    amd_funcs.atan_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cbrt_vrsa(InParams<float, float> *ipp) {
    amd_funcs.cbrt_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cos_vrsa(InParams<float, float> *ipp) {
    amd_funcs.cos_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cosh_vrsa(InParams<float, float> *ipp) {
    amd_funcs.cosh_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_div_vrsa(InParams<float, float> *ipp) {
    amd_funcs.div_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_divfi_vrsa(InParams<float, float> *ipp) {
    amd_funcs.divfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_erf_vrsa(InParams<float, float> *ipp) {
    amd_funcs.erf_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_erfc_vrsa(InParams<float, float> *ipp) {
    amd_funcs.erfc_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp_vrsa(InParams<float, float> *ipp) {
    amd_funcs.exp_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp10_vrsa(InParams<float, float> *ipp) {
    amd_funcs.exp10_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp2_vrsa(InParams<float, float> *ipp) {
    amd_funcs.exp2_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_expm1_vrsa(InParams<float, float> *ipp) {
    amd_funcs.expm1_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fabs_vrsa(InParams<float, float> *ipp) {
    amd_funcs.fabs_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmax_vrsa(InParams<float, float> *ipp) {
    amd_funcs.fmax_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmaxfi_vrsa(InParams<float, float> *ipp) {
    amd_funcs.fmaxfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmin_vrsa(InParams<float, float> *ipp) {
    amd_funcs.fmin_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fminfi_vrsa(InParams<float, float> *ipp) {
    amd_funcs.fminfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_linearfrac_vrsa(InParams<float, float> *ipp) {
    amd_funcs.linearfrac_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->ip[2], ipp->ip[3], ipp->ip[4], ipp->ip[5], ipp->optr[0]);
}

SHIM_EXPORT void shim_log_vrsa(InParams<float, float> *ipp) {
    amd_funcs.log_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log10_vrsa(InParams<float, float> *ipp) {
    amd_funcs.log10_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log1p_vrsa(InParams<float, float> *ipp) {
    amd_funcs.log1p_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log2_vrsa(InParams<float, float> *ipp) {
    amd_funcs.log2_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_mul_vrsa(InParams<float, float> *ipp) {
    amd_funcs.mul_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_mulfi_vrsa(InParams<float, float> *ipp) {
    amd_funcs.mulfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_pow_vrsa(InParams<float, float> *ipp) {
    amd_funcs.pow_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_powx_vrsa(InParams<float, float> *ipp) {
    amd_funcs.powx_vrsa(ipp->count, ipp->iptr[0], *ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_sin_vrsa(InParams<float, float> *ipp) {
    amd_funcs.sin_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sincos_vrsa(InParams<float, float> *ipp) {
    amd_funcs.sincos_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0], ipp->optr[1]);
}

SHIM_EXPORT void shim_sqrt_vrsa(InParams<float, float> *ipp) {
    amd_funcs.sqrt_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sub_vrsa(InParams<float, float> *ipp) {
    amd_funcs.sub_vrsa(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_subfi_vrsa(InParams<float, float> *ipp) {
    amd_funcs.subfi_vrsa(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_tan_vrsa(InParams<float, float> *ipp) {
    amd_funcs.tan_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_tanh_vrsa(InParams<float, float> *ipp) {
    amd_funcs.tanh_vrsa(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

// ============================================================================
// DOUBLE PRECISION ARRAY (vrda) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrda(InParams<double, double> *ipp) {
    amd_funcs.acos_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_add_vrda(InParams<double, double> *ipp) {
    amd_funcs.add_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_addi_vrda(InParams<double, double> *ipp) {
    amd_funcs.addi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_asin_vrda(InParams<double, double> *ipp) {
    amd_funcs.asin_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_atan_vrda(InParams<double, double> *ipp) {
    amd_funcs.atan_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cbrt_vrda(InParams<double, double> *ipp) {
    amd_funcs.cbrt_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cos_vrda(InParams<double, double> *ipp) {
    amd_funcs.cos_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_cosh_vrda(InParams<double, double> *ipp) {
    amd_funcs.cosh_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_div_vrda(InParams<double, double> *ipp) {
    amd_funcs.div_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_divi_vrda(InParams<double, double> *ipp) {
    amd_funcs.divi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_erf_vrda(InParams<double, double> *ipp) {
    amd_funcs.erf_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_erfc_vrda(InParams<double, double> *ipp) {
    amd_funcs.erfc_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp_vrda(InParams<double, double> *ipp) {
    amd_funcs.exp_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp10_vrda(InParams<double, double> *ipp) {
    amd_funcs.exp10_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_exp2_vrda(InParams<double, double> *ipp) {
    amd_funcs.exp2_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_expm1_vrda(InParams<double, double> *ipp) {
    amd_funcs.expm1_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fabs_vrda(InParams<double, double> *ipp) {
    amd_funcs.fabs_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmax_vrda(InParams<double, double> *ipp) {
    amd_funcs.fmax_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmaxi_vrda(InParams<double, double> *ipp) {
    amd_funcs.fmaxi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmin_vrda(InParams<double, double> *ipp) {
    amd_funcs.fmin_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_fmini_vrda(InParams<double, double> *ipp) {
    amd_funcs.fmini_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_linearfrac_vrda(InParams<double, double> *ipp) {
    amd_funcs.linearfrac_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->ip[2], ipp->ip[3], ipp->ip[4], ipp->ip[5], ipp->optr[0]);
}

SHIM_EXPORT void shim_log_vrda(InParams<double, double> *ipp) {
    amd_funcs.log_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log10_vrda(InParams<double, double> *ipp) {
    amd_funcs.log10_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log1p_vrda(InParams<double, double> *ipp) {
    amd_funcs.log1p_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_log2_vrda(InParams<double, double> *ipp) {
    amd_funcs.log2_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_mul_vrda(InParams<double, double> *ipp) {
    amd_funcs.mul_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_muli_vrda(InParams<double, double> *ipp) {
    amd_funcs.muli_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_pow_vrda(InParams<double, double> *ipp) {
    amd_funcs.pow_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_powx_vrda(InParams<double, double> *ipp) {
    amd_funcs.powx_vrda(ipp->count, ipp->iptr[0], *ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_sin_vrda(InParams<double, double> *ipp) {
    amd_funcs.sin_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sincos_vrda(InParams<double, double> *ipp) {
    amd_funcs.sincos_vrda(ipp->count, ipp->iptr[0], ipp->optr[0], ipp->optr[1]);
}

SHIM_EXPORT void shim_sqrt_vrda(InParams<double, double> *ipp) {
    amd_funcs.sqrt_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

SHIM_EXPORT void shim_sub_vrda(InParams<double, double> *ipp) {
    amd_funcs.sub_vrda(ipp->count, ipp->iptr[0], ipp->iptr[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_subi_vrda(InParams<double, double> *ipp) {
    amd_funcs.subi_vrda(ipp->count, ipp->iptr[0], ipp->ip[1], ipp->optr[0]);
}

SHIM_EXPORT void shim_tan_vrda(InParams<double, double> *ipp) {
    amd_funcs.tan_vrda(ipp->count, ipp->iptr[0], ipp->optr[0]);
}

#ifdef __AVX512F__
// ============================================================================
// DOUBLE PRECISION 512-BIT VECTOR (vrd8) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_asin_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.asin_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.atan_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.cos_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.erf_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.erfc_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.exp2_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    __m512d ipa = ipp->ip[0].data;
    __m512d ipb = ipp->ip[1].data;
    double sca  = ipp->ip[2].data[0];
    double scb  = ipp->ip[3].data[0];
    double sfta = ipp->ip[4].data[0];
    double sftb = ipp->ip[5].data[0];
    ipp->op[0].data = amd_funcs.linearfrac_vrd8(ipa, ipb, sca, scb, sfta, sftb);
}

SHIM_EXPORT void shim_log_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.log2_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.pow_vrd8(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.powx_vrd8(ipp->ip[0].data, ipp->ip[1].data[0]);
}

SHIM_EXPORT void shim_sin_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.sin_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    amd_funcs.sincos_vrd8(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.sqrt_vrd8(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrd8(InParams<libm::AlignedM512d, double> *ipp) {
    ipp->op[0].data = amd_funcs.tan_vrd8(ipp->ip[0].data);
}

// ============================================================================
// SINGLE PRECISION 512-BIT VECTOR (vrs16) VARIANTS
// ============================================================================
SHIM_EXPORT void shim_acos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.acos_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_asin_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.asin_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_atan_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.atan_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_cos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.cos_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erf_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.erf_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_erfc_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.erfc_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_exp2_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.exp2_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_linearfrac_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    __m512 ipa = ipp->ip[0].data;
    __m512 ipb = ipp->ip[1].data;
    float sca  = ipp->ip[2].data[0];
    float scb  = ipp->ip[3].data[0];
    float sfta = ipp->ip[4].data[0];
    float sftb = ipp->ip[5].data[0];
    ipp->op[0].data = amd_funcs.linearfrac_vrs16(ipa, ipb, sca, scb, sfta, sftb);
}

SHIM_EXPORT void shim_log_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.log_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log10_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.log10_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_log2_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.log2_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_pow_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.pow_vrs16(ipp->ip[0].data, ipp->ip[1].data);
}

SHIM_EXPORT void shim_powx_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.powx_vrs16(ipp->ip[0].data, ipp->ip[1].data[0]);
}

SHIM_EXPORT void shim_sin_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.sin_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_sincos_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    amd_funcs.sincos_vrs16(ipp->ip[0].data, &ipp->op[0].data, &ipp->op[1].data);
}

SHIM_EXPORT void shim_sqrt_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.sqrt_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tan_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.tan_vrs16(ipp->ip[0].data);
}

SHIM_EXPORT void shim_tanh_vrs16(InParams<libm::AlignedM512, float> *ipp) {
    ipp->op[0].data = amd_funcs.tanh_vrs16(ipp->ip[0].data);
}
#endif

} // extern "C"