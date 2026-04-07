/*
 * Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
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

 #pragma once

 #include <iostream>
 #include <string>
 #include <vector>
 #include <typeinfo>
 #include <immintrin.h>
 #include <cstdint>
 #include <cfenv>
 #include <cmath>
 #include <optional>
 #include "dll_utils.h"

 #pragma STDC FENV_ACCESS ON

 #ifdef _WIN32
     #undef max
     #undef min
 #endif

 #define MAX_IPPTR 6
 #define MAX_OPPTR 2
 #define MAX_ELEM  16

 #if (defined (_WIN64) || defined (_WIN32))
   #include <complex.h>
   #if defined (__clang__)
     /* Clang on Windows */
     extern "C" {
       typedef _C_float_complex    _Fcomplex;
       typedef _C_double_complex   _Dcomplex;
       typedef _C_ldouble_complex  _Lcomplex;
     }
   #endif
   typedef    _Fcomplex            fc32_t;
   typedef    _Dcomplex            fc64_t;
   typedef    _Lcomplex            fc128_t;

   /* Portable component accessors for MSVC complex types */
   static inline float       fc_real(fc32_t  z) { return z._Val[0]; }
   static inline float       fc_imag(fc32_t  z) { return z._Val[1]; }
   static inline double      fc_real(fc64_t  z) { return z._Val[0]; }
   static inline double      fc_imag(fc64_t  z) { return z._Val[1]; }
   static inline long double fc_real(fc128_t z) { return z._Val[0]; }
   static inline long double fc_imag(fc128_t z) { return z._Val[1]; }
   static inline void fc_set_real(fc32_t&  z, float       v) { z._Val[0] = v; }
   static inline void fc_set_real(fc64_t&  z, double      v) { z._Val[0] = v; }
   static inline void fc_set_real(fc128_t& z, long double v) { z._Val[0] = v; }
   static inline void fc_set_imag(fc32_t&  z, float       v) { z._Val[1] = v; }
   static inline void fc_set_imag(fc64_t&  z, double      v) { z._Val[1] = v; }
   static inline void fc_set_imag(fc128_t& z, long double v) { z._Val[1] = v; }
 #else
   /* Linux — __complex__ is the GCC/Clang keyword valid in both C and C++ */
   #if defined(__clang__)
     #pragma clang diagnostic push
     #pragma clang diagnostic ignored "-Wc99-extensions"
   #endif
   typedef    float __complex__       fc32_t;
   typedef    double __complex__      fc64_t;
   typedef    long double __complex__ fc128_t;
   #if defined(__clang__)
     #pragma clang diagnostic pop
   #endif

   /* Portable component accessors for GCC/Clang complex types */
   static inline float       fc_real(fc32_t  z) { return __real__ z; }
   static inline float       fc_imag(fc32_t  z) { return __imag__ z; }
   static inline double      fc_real(fc64_t  z) { return __real__ z; }
   static inline double      fc_imag(fc64_t  z) { return __imag__ z; }
   static inline long double fc_real(fc128_t z) { return __real__ z; }
   static inline long double fc_imag(fc128_t z) { return __imag__ z; }
   static inline void fc_set_real(fc32_t&  z, float       v) { __real__ z = v; }
   static inline void fc_set_real(fc64_t&  z, double      v) { __real__ z = v; }
   static inline void fc_set_real(fc128_t& z, long double v) { __real__ z = v; }
   static inline void fc_set_imag(fc32_t&  z, float       v) { __imag__ z = v; }
   static inline void fc_set_imag(fc64_t&  z, double      v) { __imag__ z = v; }
   static inline void fc_set_imag(fc128_t& z, long double v) { __imag__ z = v; }
 #endif

enum class TestMode{
    E_UNITTEST,
    E_ACCURACY,
    E_PERFORMANCE,
    E_KNOWNTEST
};

/*
 * PerfMode:
 * Selects measurement strategy for performance tests.
 * E_THROUGHPUT: independent calls, CPU can overlap them (default)
 * E_LATENCY: serialized calls with data dependency, CPU cannot pipeline iterations
 */
enum class PerfMode {
    E_THROUGHPUT,
    E_LATENCY
};

/*
 * BenchArgs:
 * CLI-facing benchmark knobs set by the user at runtime.
 */
struct BenchArgs {
    TestMode test_mode;
    PerfMode perf_mode;

    BenchArgs()
        : test_mode(TestMode::E_ACCURACY),
          perf_mode(PerfMode::E_THROUGHPUT) {}
};

/*
 * TestConfig:
 * Holds test execution configuration that controls how tests are run.
 * Grouped separately from output data for clarity.
 */
struct TestConfig {
    TestMode    test_mode;                   /* Test mode (accuracy or performance) */
    PerfMode    perf_mode;                   /* Measurement strategy (throughput or latency) */
    bool        utflag;                      /* Unit test flag (true if no ranges) */
    bool        is_vra;                      /* Vectorized real array flag */
    double      ulp_threshold;               /* ULP threshold for accuracy tests */
    uint64_t    warmup_count;                /* Number of untimed warmup calls before measurement */
    uint64_t    batch_size;                  /* Number of API calls per timed batch */

    TestConfig()
        : test_mode(TestMode::E_ACCURACY),
          perf_mode(PerfMode::E_THROUGHPUT),
          utflag(false), is_vra(false),
          ulp_threshold(0.0),
          warmup_count(0), batch_size(1000) {}
};

 extern bool verbose;

 /*
  * AlmLibs:
  * Holds pointers to shim and reference libraries.
  */
 struct AlmLibs {
     DL_HANDLE pshimlib;
     DL_HANDLE preflib;
 };

 /*
  * valf:
  * Union to represent a float as a 32-bit unsigned integer.
  */
 union valf {
     float    f;
     uint32_t u;
 };

 /*
  * val:
  * Union to represent a double as a 64-bit unsigned integer.
  */
 union val {
     double   d;
     uint64_t u;
 };

/*
 * Bit-level type-punning helpers.
 */
static inline uint32_t asuint32(float f)   { valf v = {.f = f}; return v.u; }
static inline float    asfloat(uint32_t u) { valf v = {.u = u}; return v.f; }
static inline uint64_t asuint64(double d)  { val  v = {.d = d}; return v.u; }
static inline double   asdouble(uint64_t u){ val  v = {.u = u}; return v.d; }

 namespace libm {
     /* SIMD wrapper types */
     struct AlignedM128   { __m128 data; };
     struct AlignedM128d  { __m128d data; };
     struct AlignedM256   { __m256 data; };
     struct AlignedM256d  { __m256d data; };

     #ifdef __AVX512F__
     struct AlignedM512   { __m512 data; };
     struct AlignedM512d  { __m512d data; };
     #endif

     /*
     * type_info<T>:
     * Provides metadata about SIMD types.
     */
     template <typename T>
     struct type_info {
         using real_type = T;
         static constexpr bool is_simd = false;
     };

     template <>
     struct type_info<AlignedM128> {
         using real_type = float;
         static constexpr bool is_simd = true;
     };

     template <>
     struct type_info<AlignedM128d> {
         using real_type = double;
         static constexpr bool is_simd = true;
     };

     template <>
     struct type_info<AlignedM256> {
         using real_type = float;
         static constexpr bool is_simd = true;
     };

     template <>
     struct type_info<AlignedM256d> {
         using real_type = double;
         static constexpr bool is_simd = true;
     };

     #ifdef __AVX512F__
     template <>
     struct type_info<AlignedM512> {
         using real_type = float;
         static constexpr bool is_simd = true;
     };

     template <>
     struct type_info<AlignedM512d> {
         using real_type = double;
         static constexpr bool is_simd = true;
     };
     #endif

     template <>
     struct type_info<fc32_t> {
         using real_type = fc32_t;
         static constexpr bool is_simd = false;
     };

     template <>
     struct type_info<fc64_t> {
         using real_type = fc64_t;
         static constexpr bool is_simd = false;
     };
 } /* namespace libm */

 /*
  * TestStatus:
  * Enumeration of test case statuses.
  */
 enum TestStatus {
     TESTCASE_FAIL,
     TESTCASE_PASS
 };

 /*
  * RangeType:
  * Enumeration of supported input range generation strategies.
  */
 enum RangeType {
     E_Simple,
     E_Integer,
     E_Fixedval,
     E_Random,
     E_Linear,
     E_Expstep,
     E_Bitstep,
     E_Derived,
    E_MAX
 };

/*
 * DerivedConfig:
 * Configuration for derived (bivariate) input generation.
 * The second array is computed from the first via a registered function.
 */
template <typename U>
struct DerivedConfig {
    U         z_srt;     /* Start value for derived sub-generator */
    U         z_stp;     /* Stop value for derived sub-generator */
    RangeType z_type;    /* Range generation type for derived sub-generator */
    uint64_t  z_count;   /* Number of values for derived sub-generator */
    std::string func;    /* Name of combining function (e.g. "mul") */
};

 /*
  * InpRng:
  * Templated structure for typed input ranges.
  */
 template <typename U>
 struct InpRng {
     U         srt;       /* Start value */
     U         stp;       /* Stop value */
     RangeType type;      /* Range generation type */
     uint64_t  count;     /* Number of values to generate */
     std::optional<DerivedConfig<U>> derived; /* Config for derived generation (used when type == E_Derived) */
};

 /*
  * InParams:
  * Templated structure to hold input parameters for validation.
  */
 template <typename T, typename U>
 struct InParams {
     U         *iptr[MAX_IPPTR];             /* Input pointers */
     U         *optr[MAX_OPPTR];             /* Output pointers */
     uint64_t   count;                       /* Number of elements */
     T          ip[MAX_IPPTR];               /* Single input values */
     T          op[MAX_OPPTR];               /* Single output values */
     U          xv;                          /* Expected output value */
     int        xxv;                         /* Expected exception */
     std::vector<InpRng<U>> range;           /* Input ranges */
     void      *sys;
 };