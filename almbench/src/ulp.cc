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

 #include <cmath>
 #include <cstdint>
 #include <cstring>
 #include <limits>
 #include <type_traits>
 #include "alm_test.h"
 #include "ulp.h"

 /*
  * UlpTraits:
  * Provides bit-level traits for float and double types.
  */
 template <typename T>
 struct UlpTraits;

 template <>
 struct UlpTraits<float> {
     using uint_t = uint32_t;
     static constexpr float max_val = std::numeric_limits<float>::max();
     static constexpr float true_min = std::numeric_limits<float>::denorm_min();
 };

 template <>
 struct UlpTraits<double> {
     using uint_t = uint64_t;
     static constexpr double max_val = std::numeric_limits<double>::max();
     static constexpr double true_min = std::numeric_limits<double>::denorm_min();
 };

 /*
  * ulp_size_at:
  * Computes the ULP (unit in the last place) size at a given value
  * using nextafter — correct for all values including subnormals
  * and T_MAX.
  */
 template <typename T>
 static inline T ulp_size_at(T x)
 {
     T ax = std::abs(x);
     T nxt_up = std::nextafter(ax, std::numeric_limits<T>::infinity());

     if (std::isinf(nxt_up))
         return ax - std::nextafter(ax, T(0));

     return nxt_up - ax;
 }

 /*
  * floor_cast:
  * Safely narrow a value from wider type L to narrower type T without
  * ever rounding upward across a binade boundary.
  */
 template <typename T, typename L>
 static inline T floor_cast(L val)
 {
     T tval = static_cast<T>(val);
     if (tval > val && tval != std::numeric_limits<T>::infinity())
         return std::nextafter(tval, T(0));
     return tval;
 }

 /*
  * count_ulps:
  * Counts the integer number of representable T-precision values
  * between a and b using bit-pattern arithmetic.
  */
 template <typename T>
 static inline typename UlpTraits<T>::uint_t count_ulps(T a, T b)
 {
     using U = typename UlpTraits<T>::uint_t;
     U ua, ub;
     std::memcpy(&ua, &a, sizeof(T));
     std::memcpy(&ub, &b, sizeof(T));

     if (a < T(0)) ua = (U(1) << (sizeof(U) * 8 - 1)) - ua;
     if (b < T(0)) ub = (U(1) << (sizeof(U) * 8 - 1)) - ub;

     return (ua >= ub) ? (ua - ub) : (ub - ua);
 }

 /*
  * compute_ulp_bitwise:
  * Same-type ULP comparison when no wider precision is available.
  * Returns integer count of representable values between aop and ref.
  */
 template <typename T>
 static double compute_ulp_bitwise(T aop, T ref)
 {
     if (std::isnan(aop) && std::isnan(ref))
         return (std::signbit(aop) == std::signbit(ref)) ? 0.0 : INFINITY;
     if (std::isnan(aop) || std::isnan(ref))
         return INFINITY;
     if (aop != T(0) && ref != T(0) && std::signbit(aop) != std::signbit(ref))
         return INFINITY;
     /* +0 vs −0: ULP stays 0 (same as gtests getUlp); scalar conformance in update_ulp */
     if (aop == T(0) && ref == T(0))
         return 0.0;
     if (std::isinf(aop) && std::isinf(ref))
         return (std::signbit(aop) == std::signbit(ref)) ? 0.0 : INFINITY;

     /* One finite, one ∞ (or both finite): ∞'s bit pattern sits 1 ULP past
      * T_MAX in IEEE 754 encoding, so count_ulps gives the correct integer
      * distance for all combinations of finite and ∞ values. */
     return static_cast<double>(count_ulps(std::abs(aop), std::abs(ref)));
 }

 /*
  * compute_ulp:
  * Computes the ULP error between actual (aop) and reference (ref) values.
  *
  * Uses cross-precision fractional ULP when sizeof(T) < sizeof(L),
  * falls back to integer bit-distance when sizeof(T) == sizeof(L).
  *
  * Fixes vs the previous implementation:
  *   A — NaN sign:          signbit check distinguishes +NaN vs −NaN
  *   B — Finite sign:       sign mismatch guard returns INFINITY
  *   C — Overflow formula:  fractional overshoot past T_MAX, not hardcoded +1
  *   D — Binade cast:       floor_cast guarantees correct binade for ref
  *   E — Signed zero:       compute_ulp returns 0 for ±0 pairs; update_ulp fails
  *                          on scalar types when signs differ
  */
 template <typename S, typename L>
 double compute_ulp(S aop, L ref)
 {
     static_assert(std::is_floating_point<S>::value, "S must be floating-point");
     static_assert(std::is_floating_point<L>::value, "L must be floating-point");

     using Tr = UlpTraits<S>;
     const S fmax = Tr::max_val;

     if (sizeof(S) == sizeof(L))
         return compute_ulp_bitwise<S>(aop, static_cast<S>(ref));

     if (std::isnan(aop) && std::isnan(ref))
         return (std::signbit(aop) == std::signbit(ref)) ? 0.0 : INFINITY;
     if (std::isnan(aop) || std::isnan(ref))
         return INFINITY;
     if (aop != S(0) && ref != L(0) && std::signbit(aop) != std::signbit(ref))
         return INFINITY;
     if (aop == S(0) && ref == L(0))
         return 0.0;
     if (std::isinf(aop) && std::isinf(ref))
         return (std::signbit(aop) == std::signbit(ref)) ? 0.0 : INFINITY;
     if (std::isinf(aop) && std::isfinite(ref)) {
         L aref = std::fabs(ref);
         if (aref > static_cast<L>(fmax)) {
             S ulp = ulp_size_at<S>(fmax);
             double beyond = static_cast<double>(
                 (aref - static_cast<L>(fmax)) / static_cast<L>(ulp));
             if (beyond >= 0.5)
                 return 0.0;
             return 1.0 - beyond;
         }
         /* |ref| within S's range: ∞ is modeled as fmax + 1 ULP.
          * Distance = integer ULPs from |ref| to fmax + 1 − fractional offset. */
         S fref = floor_cast<S>(aref);
         S ulp_u = ulp_size_at<S>(fref);
         if (ulp_u == S(0))
             ulp_u = Tr::true_min;
         double fulp = static_cast<double>(
             (aref - static_cast<L>(fref)) / static_cast<L>(ulp_u));
         return static_cast<double>(count_ulps<S>(fref, fmax)) + 1.0 - fulp;
     }
     if (std::isinf(ref))
         return INFINITY;

     /* Overflow zone: aop is finite but |ref| exceeds S's representable range.
      * beyond = how many ULPs past fmax the reference sits.
      * When beyond >= 0.5 the correctly-rounded S result is ∞ (round-to-
      * nearest), so measure the distance from |aop| to ∞ (fmax + 1 ULP).
      * When beyond < 0.5 the correctly-rounded result is fmax, so add the
      * fractional overshoot. */
     if (std::isfinite(aop) && std::fabs(ref) > static_cast<L>(fmax)) {
         S ulp = ulp_size_at<S>(fmax);
         double beyond = static_cast<double>(
             (std::fabs(ref) - static_cast<L>(fmax)) / static_cast<L>(ulp));
         double base = static_cast<double>(count_ulps<S>(std::abs(aop), fmax));
         if (beyond >= 0.5)
             return base + 1.0;
         return base + beyond;
     }

     /* Normal path: both finite, |ref| within T's range. */
     L aref = std::fabs(ref);
     S aaop = std::abs(aop);
     S fref = floor_cast<S>(aref);

     S ulp_u = ulp_size_at<S>(fref);
     if (ulp_u == S(0))
         ulp_u = Tr::true_min;

     /* Fractional part: how far ref sits above fref, in units of ulp_u.
      * Keep subtraction in L precision — the gap can be sub-T-ULP. */
     double fulp = static_cast<double>(
         (aref - static_cast<L>(fref)) / static_cast<L>(ulp_u));

     if (static_cast<L>(aaop) > aref)
         fulp = -fulp;

     /* Integer part: distance from fref to aaop.
      * When they straddle a binade boundary, walk across each segment
      * so that count_ulps operates within a single ULP grid size. */
     S ulp_aop = ulp_size_at<S>(aaop);
     double iulp = 0.0;
     if (ulp_aop != ulp_u) {
         S lo = (fref < aaop) ? fref : aaop;
         S hi = (fref < aaop) ? aaop : fref;
         while (lo < hi) {
             S boundary;
             if (!std::isnormal(lo)) {
                 boundary = std::numeric_limits<S>::min();
             } else {
                 int exp;
                 std::frexp(lo, &exp);
                 boundary = std::ldexp(S(1), exp);
             }
             if (boundary <= lo)
                 boundary = std::nextafter(lo, std::numeric_limits<S>::infinity());
             S seg_end = (boundary < hi) ? boundary : hi;
             iulp += static_cast<double>(count_ulps<S>(lo, seg_end));
             lo = seg_end;
         }
     } else {
         iulp = static_cast<double>(count_ulps<S>(fref, aaop));
     }

     return iulp + fulp;
 }

 /* Integer-integer ULP: 0 if equal, absolute error otherwise.
  * Subtract in double to avoid signed-integer overflow UB. */
 static inline double compute_ulp(lint_t aop, lint_t ref)
 {
     return std::fabs(static_cast<double>(aop) - static_cast<double>(ref));
 }

 static inline double compute_ulp(llint_t aop, llint_t ref)
 {
     return std::fabs(static_cast<double>(aop) - static_cast<double>(ref));
 }

 /*
  * compute_relative_error:
  * Computes |aop - ref| / max(|ref|, MIN_NORMAL).
  *
  * The subtraction is done in L precision so that no bits are lost
  * when S is narrower than L.  The denominator is clamped to the
  * smallest normal number of type L so that subnormal ref values
  * do not inflate the ratio (IEEE 754-2008 §5.11 convention).
  *
  * Returns 0 when both are zero (including +0 vs −0); scalar signed-zero is
  * enforced in update_ulp. Complex (fc32/fc64) omits that gate like gtests
  * ConfVerifyComplex* on amd-main.
  * INFINITY for NaN/Inf or ref==0 mismatches (non-zero actual).
  */
 template <typename S, typename L>
 double compute_relative_error(S aop, L ref)
 {
     static_assert(std::is_floating_point<S>::value, "S must be floating-point");
     static_assert(std::is_floating_point<L>::value, "L must be floating-point");

     if (std::isnan(aop) && std::isnan(ref))
         return (std::signbit(aop) == std::signbit(ref)) ? 0.0 : INFINITY;
     if (std::isnan(aop) || std::isnan(ref))
         return INFINITY;
     if (std::isinf(aop) && std::isinf(ref))
         return (std::signbit(aop) == std::signbit(ref)) ? 0.0 : INFINITY;
     if (std::isinf(aop) || std::isinf(ref))
         return INFINITY;
     if (aop == S(0) && ref == L(0))
         return 0.0;
     if (ref == L(0))
         return INFINITY;

     L abs_ref = std::fabs(ref);
     L denom   = std::fmax(abs_ref, std::numeric_limits<L>::min());

     return static_cast<double>(
         std::fabs(static_cast<L>(aop) - ref) / denom);
 }

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
static inline double compute_relative_error(T aop, T ref)
{
    if (ref == 0)
        return (aop == 0) ? 0.0 : INFINITY;

    return std::abs(static_cast<double>(aop) - static_cast<double>(ref)) / std::abs(static_cast<double>(ref));
}

 /* ═══════════════════════════════════════════════════════════════════
  *  Complex ULP
  *
  *  Per-component scalar ULP, combined with L-inf norm: max(re, im).
  * ═══════════════════════════════════════════════════════════════════ */

 /*
  * complex_ulp_result:
  * Per-component ULP errors with norm accessors.
  */
 struct complex_ulp_result {
     double re;
     double im;

     double linf() const { return std::fmax(re, im); }
 };

 /* compute_ulp_components — returns both per-component errors */

 static inline complex_ulp_result
 compute_ulp_components(fc32_t aop, fc32_t ref)
 {
     return { compute_ulp<float, float>(fc_real(aop), fc_real(ref)),
              compute_ulp<float, float>(fc_imag(aop), fc_imag(ref)) };
 }

 static inline complex_ulp_result
 compute_ulp_components(fc32_t aop, fc64_t ref)
 {
     return { compute_ulp<float, double>(fc_real(aop), fc_real(ref)),
              compute_ulp<float, double>(fc_imag(aop), fc_imag(ref)) };
 }

 static inline complex_ulp_result
 compute_ulp_components(fc64_t aop, fc64_t ref)
 {
     return { compute_ulp<double, double>(fc_real(aop), fc_real(ref)),
              compute_ulp<double, double>(fc_imag(aop), fc_imag(ref)) };
 }

 static inline complex_ulp_result
 compute_ulp_components(fc64_t aop, fc128_t ref)
 {
     return { compute_ulp<double, long double>(fc_real(aop), fc_real(ref)),
              compute_ulp<double, long double>(fc_imag(aop), fc_imag(ref)) };
 }

 /* compute_ulp — complex overloads (L-inf norm) */

 static inline double
 compute_ulp(fc32_t aop, fc32_t ref)
 {
     return compute_ulp_components(aop, ref).linf();
 }

 static inline double
 compute_ulp(fc32_t aop, fc64_t ref)
 {
     return compute_ulp_components(aop, ref).linf();
 }

 static inline double
 compute_ulp(fc64_t aop, fc64_t ref)
 {
     return compute_ulp_components(aop, ref).linf();
 }

 static inline double
 compute_ulp(fc64_t aop, fc128_t ref)
 {
     return compute_ulp_components(aop, ref).linf();
 }

 /* ═══════════════════════════════════════════════════════════════════
  *  Complex Relative Error
  *
  *  Per-component scalar relative error, combined with L-inf norm.
  * ═══════════════════════════════════════════════════════════════════ */

 struct complex_relerr_result {
     double re;
     double im;

     double linf() const { return std::fmax(re, im); }
 };

 /* compute_relative_error_components — per-component relative errors */

 static inline complex_relerr_result
 compute_relative_error_components(fc32_t aop, fc32_t ref)
 {
     return { compute_relative_error<float, float>(fc_real(aop), fc_real(ref)),
              compute_relative_error<float, float>(fc_imag(aop), fc_imag(ref)) };
 }

 static inline complex_relerr_result
 compute_relative_error_components(fc32_t aop, fc64_t ref)
 {
     return { compute_relative_error<float, double>(fc_real(aop), fc_real(ref)),
              compute_relative_error<float, double>(fc_imag(aop), fc_imag(ref)) };
 }

 static inline complex_relerr_result
 compute_relative_error_components(fc64_t aop, fc64_t ref)
 {
     return { compute_relative_error<double, double>(fc_real(aop), fc_real(ref)),
              compute_relative_error<double, double>(fc_imag(aop), fc_imag(ref)) };
 }

 static inline complex_relerr_result
 compute_relative_error_components(fc64_t aop, fc128_t ref)
 {
     return { compute_relative_error<double, long double>(fc_real(aop), fc_real(ref)),
              compute_relative_error<double, long double>(fc_imag(aop), fc_imag(ref)) };
 }

 /* compute_relative_error — complex overloads (L-inf norm) */

 static inline double
 compute_relative_error(fc32_t aop, fc32_t ref)
 {
     return compute_relative_error_components(aop, ref).linf();
 }

 static inline double
 compute_relative_error(fc32_t aop, fc64_t ref)
 {
     return compute_relative_error_components(aop, ref).linf();
 }

 static inline double
 compute_relative_error(fc64_t aop, fc64_t ref)
 {
     return compute_relative_error_components(aop, ref).linf();
 }

 static inline double
 compute_relative_error(fc64_t aop, fc128_t ref)
 {
     return compute_relative_error_components(aop, ref).linf();
 }

/*
 * signed_zero_bits_mismatch:
 * Scalar +0 vs −0: treat as conformance failure while compute_ulp reports 0.
 * Used only for non-complex update_ulp (gtests scalar ConfVerify*).
 */
 template <typename S, typename L>
 static inline bool signed_zero_bits_mismatch(S aop, L ref)
 {
     return aop == S(0) && ref == L(0) && std::signbit(aop) != std::signbit(ref);
 }

template <typename S, typename L>
static inline bool signed_zero_mismatch(S aop, L ref)
{
    if constexpr (std::is_same_v<S, fc32_t> || std::is_same_v<S, fc64_t>) {
        (void)aop;
        (void)ref;
        return false;
    } else {
        return signed_zero_bits_mismatch(aop, ref);
    }
}

 /*
  * update_ulp:
  * Updates the maximum ULP / relative error, classifies the test point
  * into one of three accuracy buckets, and checks against the threshold.
  */
 template <typename S, typename L>
 int update_ulp(S aop, L mpfrop, struct ulp_data &udata, double &ulp)
 {
     ulp = compute_ulp(aop, mpfrop);
     const bool sz_fail = signed_zero_mismatch(aop, mpfrop);
     int res = TESTCASE_PASS;

     if (std::isinf(ulp) || std::isnan(ulp)) {
         udata.max_ulp_err = INFINITY;
         udata.inaccurate_count++;
         res = TESTCASE_FAIL;
     } else {
         if (ulp > udata.max_ulp_err)
             udata.max_ulp_err = ulp;

        if (sz_fail) {
            /* ULP is 0 like gtests; still fail conformance on sign of zero.
             * Bucket by ulp_data: exact_count for ulp==0, not inaccurate_count (ulp > 0.5). */
            res = TESTCASE_FAIL;
            udata.exact_count++;
        } else {
            if (ulp == 0.0)
                udata.exact_count++;
            else if (ulp <= 0.5)
                udata.rounded_count++;
            else
                udata.inaccurate_count++;

            if (ulp > udata.ulp_threshold) {
                res = TESTCASE_FAIL;
                udata.ulp_exceed_count++;
            }
        }
     }

     double relerr = compute_relative_error(aop, mpfrop);

     if (std::isinf(relerr) || std::isnan(relerr)) {
         udata.max_relative_err = INFINITY;
     } else {
         if (relerr > udata.max_relative_err)
             udata.max_relative_err = relerr;
     }
     return res;
 }

 /* Explicit template instantiations */
 template int update_ulp<float, double>(float amdop, double mpfrop,
                                         struct ulp_data &udata, double &ulp);
 template int update_ulp<float, float>(float amdop, float mpfrop,
                                         struct ulp_data &udata, double &ulp);

 template int update_ulp<double, double>(double amdop, double mpfrop,
                                               struct ulp_data &udata, double &ulp);
 template int update_ulp<double, long double>(double amdop, long double mpfrop,
                                               struct ulp_data &udata, double &ulp);

template int update_ulp<lint_t, lint_t>(lint_t amdop, lint_t mpfrop,
                                        struct ulp_data &udata, double &ulp);
template int update_ulp<llint_t, llint_t>(llint_t amdop, llint_t mpfrop,
                                            struct ulp_data &udata, double &ulp);

 template int update_ulp<fc32_t, fc32_t>(fc32_t amdop, fc32_t mpfrop,
                                               struct ulp_data &udata, double &ulp);
 template int update_ulp<fc32_t, fc64_t>(fc32_t amdop, fc64_t mpfrop,
                                               struct ulp_data &udata, double &ulp);

 template int update_ulp<fc64_t, fc64_t>(fc64_t amdop, fc64_t mpfrop,
                                               struct ulp_data &udata, double &ulp);
 template int update_ulp<fc64_t, fc128_t>(fc64_t amdop, fc128_t mpfrop,
                                               struct ulp_data &udata, double &ulp);

 /*
  * Global ULP threshold management
  * Not thread-safe: callers must ensure single-threaded access
  * or external synchronisation when setting the threshold.
  */
 namespace {
     static double global_ulp_threshold = 0.5;
 }

 /*
 * SetGlobalUlpThreshold:
 * Sets the global default ULP threshold for new ulp_data instances.
 */
void SetGlobalUlpThreshold(double threshold) {
     if (threshold >= 0.0) {
         global_ulp_threshold = threshold;
     }
 }

 /*
  * GetGlobalUlpThreshold:
  * Gets the current global default ULP threshold.
  */
 double GetGlobalUlpThreshold() {
     return global_ulp_threshold;
 }