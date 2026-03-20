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

#include <iostream>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include "alm_test.h"
#include "ulp.h"

/*
 * FloatTraits:
 * Provides bit-level traits for float and double types.
 */
template <typename S>
struct FloatTraits;

template <>
struct FloatTraits<float> {
    using uint = uint32_t;
    static constexpr int exponent_bits = 8;
    static constexpr int exponent_bias = 127;
    static constexpr int precision = 24;
};

template <>
struct FloatTraits<double> {
    using uint = uint64_t;
    static constexpr int exponent_bits = 11;
    static constexpr int exponent_bias = 1023;
    static constexpr int precision = 53;
};

/*
 * FloatUnion:
 * Union to reinterpret a floating-point value as an integer.
 */
template <typename S>
union FloatUnion {
    S f;
    typename FloatTraits<S>::uint i;
};

/*
 * ulprep:
 * Computes the ULP (unit in the last place) for a given value.
 */
template <typename S>
S ulprep(S val)
{
    static_assert(std::is_floating_point<S>::value, "S must be a floating point type");

    using Traits = FloatTraits<S>;
    FloatUnion<S> c = { .f = val };

    int32_t expo = ((c.i >> (std::numeric_limits<S>::digits - 1)) &
                    ((1 << Traits::exponent_bits) - 1)) - Traits::exponent_bias;

    expo = expo - (Traits::precision - 1);
    return std::pow(2, expo);
}

/*
 * is_ninf:
 * Checks if a value is negative infinity.
 */
template <typename S>
bool is_ninf(S val)
{
    return std::isinf(val) && std::signbit(val);
}

/*
 * is_inf_neg:
 * Alternative check for negative infinity.
 */
template <typename S>
bool is_inf_neg(S val)
{
    return std::isinf(val) && (val < 0.0);
}

/*
 * compute_ulp:
 * Computes the ULP error between actual and expected values.
 */
template <typename S, typename L>
double compute_ulp(S aop, L _mpfr)
{
    S mpfr = static_cast<S>(_mpfr);
    static const S fmax = std::numeric_limits<S>::max();
    static const S f_low = std::numeric_limits<S>::lowest();

    if (std::isnan(aop) && std::isnan(mpfr)) {
        return 0.0;
    }

    if (std::isnan(aop) || std::isnan(mpfr)) {
        return INFINITY;
    }

    if (std::isinf(aop) && (std::isinf(mpfr) || (mpfr > fmax))) {
        return 0.0;
    }

    if (is_ninf(aop) && (is_ninf(mpfr) || (mpfr < f_low))) {
        return 0.0;
    }

    if (std::isinf(aop) && std::isinf(mpfr)) {
        return INFINITY;
    }

    if (std::isfinite(aop)) {
        if (mpfr < fmax) {
            return std::abs(aop - _mpfr) / ulprep<S>(mpfr);
        }

        if ((mpfr > fmax) || std::isinf(mpfr)) {
            return std::abs(aop - fmax) / ulprep<S>(fmax) + 1;
        }

        if (is_ninf(_mpfr) || (mpfr < f_low)) {
            return std::abs(aop - f_low) / ulprep<S>(f_low) + 1;
        }
    }

    if (std::isinf(aop)) {
        return std::abs((fmax - _mpfr) / ulprep<S>(mpfr)) + 1;
    }

    if (is_inf_neg(aop)) {
        return std::abs((f_low - _mpfr) / ulprep<S>(mpfr)) + 1;
    }

    return 0.0;
}

/*
 * update_ulp:
 * Updates the maximum ULP error and checks against the threshold.
 */
template <typename S, typename L>
int update_ulp(S aop, L mpfrop, struct ulp_data &udata, double &ulp)
{
    ulp = compute_ulp<S, L>(aop, mpfrop);
    int res = TESTCASE_PASS;

    if (std::isinf(ulp) || std::isnan(ulp)) {
        res = TESTCASE_FAIL;
    } else {
        if ((ulp - udata.max_ulp_err) > 0) {
            udata.max_ulp_err = ulp;
        }

        if ((ulp - udata.ulp_threshold) > 0.0) {
            res = TESTCASE_FAIL;
        }
    }

    return res;
}

/* Explicit template instantiations */
template int update_ulp<float, double>(float amdop, double mpfrop,
                                        struct ulp_data &udata, double &ulp);
template int update_ulp<double, long double>(double amdop, long double mpfrop,
                                              struct ulp_data &udata, double &ulp);

/*
 * Global ULP threshold management
 * Thread-safe implementation using static variable
 */
namespace {
    static double global_ulp_threshold = 0.5; // Default threshold
}

/*
 * set_global_ulp_threshold:
 * Sets the global default ULP threshold for new ulp_data instances.
 */
void set_global_ulp_threshold(double threshold) {
    if (threshold >= 0.0) {
        global_ulp_threshold = threshold;
    }
}

/*
 * get_global_ulp_threshold:
 * Gets the current global default ULP threshold.
 */
double get_global_ulp_threshold() {
    return global_ulp_threshold;
}