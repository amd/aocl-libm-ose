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
 #include <cmath>
 #include "alm_test.h"

 /*
 * SetGlobalUlpThreshold:
 * Sets a global default ULP threshold for new ulp_data instances.
 * This affects the default constructor of ulp_data.
 */
void SetGlobalUlpThreshold(double threshold);

 /*
  * GetGlobalUlpThreshold:
  * Gets the current global default ULP threshold.
  */
 double GetGlobalUlpThreshold();


 /*
  * ulp_data:
  * Holds ULP error metrics for a test case.
  *
  */
 struct ulp_data {
     double   max_ulp_err;        /* Maximum ULP error observed */
     double   max_relative_err;   /* Maximum relative error observed */
     double   ulp_threshold;      /* Acceptable ULP threshold */
     uint64_t ulp_exceed_count;   /* Count of test points where ulp > ulp_threshold */
     uint64_t exact_count;        /* Count of test points where ulp == 0 */
     uint64_t rounded_count;      /* Count of test points where 0 < ulp <= 0.5 */
     uint64_t inaccurate_count;   /* Count of test points where ulp > 0.5 */

     ulp_data()
         : max_ulp_err(0.0), max_relative_err(0.0),
           ulp_threshold(GetGlobalUlpThreshold()),
           ulp_exceed_count(0),
           exact_count(0), rounded_count(0), inaccurate_count(0) {}

     ulp_data(double threshold)
         : max_ulp_err(0.0), max_relative_err(0.0),
           ulp_threshold(threshold),
           ulp_exceed_count(0),
           exact_count(0), rounded_count(0), inaccurate_count(0) {}
 };

 /*
  * mpfr::op_type:
  * Type traits to map float/double to higher-precision types for MPFR comparison.
  */
 namespace mpfr {
     template <typename T>
     struct op_type;

     /* Specialization for float */
     template <>
     struct op_type<float> {
         using mopt = double;
     };

     /* Specialization for double */
     template <>
     struct op_type<double> {
         using mopt = long double;
     };

     /* C complex — reference from mparith matches cexp/clog (promoted result type). */
     template <>
     struct op_type<fc32_t> {
         using mopt = fc64_t;
     };

     template <>
     struct op_type<fc64_t> {
         using mopt = fc128_t;
     };
 } /* namespace mpfr */

 /* ULP threshold management utilities */

 /*
  * set_ulp_threshold:
  * Sets the ULP threshold for a ulp_data structure.
  */
 inline void set_ulp_threshold(struct ulp_data &udata, double threshold) {
     udata.ulp_threshold = threshold;
 }

 /*
  * get_ulp_threshold:
  * Gets the current ULP threshold from a ulp_data structure.
  */
 inline double get_ulp_threshold(const struct ulp_data &udata) {
     return udata.ulp_threshold;
 }

 /*
  * reset_ulp_threshold:
  * Resets ULP threshold to the default value (0.5).
  */
 inline void reset_ulp_threshold(struct ulp_data &udata) {
     udata.ulp_threshold = 0.5;
 }

 /*
  * is_ulp_acceptable:
  * Checks if a ULP error is within the acceptable threshold.
  */
 inline bool is_ulp_acceptable(const struct ulp_data &udata, double ulp_error) {
     return ulp_error <= udata.ulp_threshold;
 }

 /*
  * compare_ulp_thresholds:
  * Compares two ULP thresholds and returns:
  * -1 if first < second, 0 if equal, 1 if first > second
  */
 inline int compare_ulp_thresholds(double threshold1, double threshold2) {
     const double epsilon = 1e-15;
     if (std::abs(threshold1 - threshold2) < epsilon) {
        return 0;
    }
     return (threshold1 < threshold2) ? -1 : 1;
 }

 /* ULP comparison and reporting utilities */

 /*
  * update_ulp:
  * Updates ULP error data based on actual and expected values.
  */
 template <typename S, typename L>
 int update_ulp(S actual, L expected, struct ulp_data &udata, double &ulp);

 /*
  * PrintUlpResults01:
  * Prints ULP results for single-input functions.
  */
 template <typename S, typename L>
 void PrintUlpResults01(S ip, S op, L mpfrop, double ulp);

 /*
  * PrintUlpResults02:
  * Prints ULP results for two-input functions.
  */
 template <typename S, typename L>
 void PrintUlpResults02(S inp1, S inp2, S ap, L xp, double ulp);

 /*
  * PrintUlpResults:
  * Prints ULP results for variadic input functions.
  */
 template <typename S, typename L, typename... Args>
 void PrintUlpResults(S ap, L xp, double ulp, Args... inps);