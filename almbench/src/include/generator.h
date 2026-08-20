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

 #include <cmath>
 #include <complex>
 #include <random>
 #include <memory>
 #include <iterator>
 #include <limits>
 #include <functional>
 #include <unordered_map>
 #include <string>
 #include <vector>
 #include <numeric>
 #include <immintrin.h>

 #include "alm_test.h"


 namespace libm {

 /* Type traits to map float/double to corresponding unsigned int types */
 template <typename T>
 struct uint_info;

 template <>
 struct uint_info<float> {
     using uint = uint32_t;
 };

 template <>
 struct uint_info<double> {
     using uint = uint64_t;
 };

 /*
  * complex_component_type<T>:
  * Scalar type T for std::complex<T> and for C complex types from alm_test.h (fc32_t, fc64_t, ...).
  */
 template <typename T>
 struct complex_component_type;

 template <typename U>
 struct complex_component_type<std::complex<U>> {
     using type = U;
 };

 template <>
 struct complex_component_type<fc32_t> {
     using type = float;
 };

 template <>
 struct complex_component_type<fc64_t> {
     using type = double;
 };

 template <>
 struct complex_component_type<fc128_t> {
     using type = long double;
 };

 /*
  * Build C complex values (alm_test.h fc32_t / fc64_t) from real and imaginary parts.
  */
 #if defined(_WIN32)
 inline fc32_t fc_pack(float re, float im)
 {
     fc32_t z;
     z._Val[0] = re;
     z._Val[1] = im;
     return z;
 }

 inline fc64_t fc_pack(double re, double im)
 {
     fc64_t z;
     z._Val[0] = re;
     z._Val[1] = im;
     return z;
 }
 #else
 /* GCC/Clang C++: __builtin_complex is a C-only builtin; use __real__/__imag__. */
 inline fc32_t fc_pack(float re, float im)
 {
     fc32_t z;
     __real__ z = re;
     __imag__ z = im;
     return z;
 }

 inline fc64_t fc_pack(double re, double im)
 {
     fc64_t z;
     __real__ z = re;
     __imag__ z = im;
     return z;
 }
 #endif

 } /* namespace libm */

 /* Union to reinterpret float/double as unsigned int */
 template <typename T, typename U = typename libm::uint_info<T>::uint>
 union Float2Uint {
     T f;
     U i;
 };

 /*
  * IGenerator:
  * Abstract base class for all input generators.
  */
 template <typename S>
 class IGenerator {
 public:
     virtual S *next() = 0;
     virtual bool has_next() const = 0;
     virtual void reset() = 0;
     virtual uint64_t get_index() = 0;
     virtual ~IGenerator() {}

     /* Wraps next() with reset fallback */
     virtual S *wrap_next()
     {
         if (!has_next()) {
             reset();
         }
         S *array = next();
         if (!array) {
             std::cerr << "Error: next() returned nullptr" << std::endl;
         }
         return array;
     }

     /* Iterator for generator traversal */
     class Iterator {
     public:
         using iterator_category = std::random_access_iterator_tag;
         using value_type = S;
         using difference_type = std::ptrdiff_t;
         using pointer = S*;
         using reference = S&;

         Iterator(IGenerator *generator)
             : generator(generator), value(generator ? generator->next() : nullptr) {}

         S operator*() const { return *value; }

         Iterator& operator++()
         {
             if (generator->has_next()) {
                 value = generator->next();
             } else {
                 generator = nullptr;
                 value = nullptr;
             }
             return *this;
         }

         bool operator<=(const Iterator& other) const
         {
             return *value <= *other.value;
         }

     private:
         IGenerator *generator;
         S *value;
     };

     Iterator begin() { return Iterator(this); }
     Iterator end() { return Iterator(nullptr); }
 };

 /*
  * BitGenerator:
  * Generates values by stepping through bit patterns.
  */
 template <typename S, typename U = typename libm::uint_info<S>::uint>
 class BitGenerator : public IGenerator<S> {
 public:
     BitGenerator(S rmin, S rmax, S bitstep);

     S *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;

 private:
     S bitstep;
     Float2Uint<S, U> start, stop, value;
     uint64_t i, bd, count, mxiter;
 };

 /*
  * LinearGenerator:
  * Generates linearly spaced values between rmin and rmax.
  */
 template <typename S>
 class LinearGenerator : public IGenerator<S> {
 public:
     LinearGenerator(S rmin, S rmax, uint64_t mxiter);

     S *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;

 private:
     S rmin, rmax, value, step;
     uint64_t mxiter, i;
 };

 /*
  * ExponentialGenerator:
  * Generates exponentially spaced values between rmin and rmax.
  */
 template <typename S>
 class ExponentialGenerator : public IGenerator<S> {
 public:
     ExponentialGenerator(S rmin, S rmax, uint64_t mxiter);

     S *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;

 private:
     S rmin, rmax, value, step;
     uint64_t mxiter, i;
     bool negate; // end point sign
 };

 /*
  * RandomGenerator:
  * Generates uniformly distributed random floating-point values.
  */
 template <typename S>
 class RandomGenerator : public IGenerator<S> {
 public:
     RandomGenerator(S rmin, S rmax, uint64_t cnt);

     S *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;

 private:
     S rmin, rmax, value;
     uint64_t mxiter, i;
     std::random_device rd;
     std::mt19937 gen;
     std::uniform_real_distribution<> dist;
 };

 /*
  * IntegerRandomGenerator:
  * Generates uniformly distributed random integer values.
  */
 template <typename U>
 class IntegerRandomGenerator : public IGenerator<U> {
 public:
     IntegerRandomGenerator(U rmin, U rmax, uint64_t cnt);

     U *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;

 private:
     U rmin, rmax, value;
     uint64_t mxiter, i;
     std::random_device rd;
     std::mt19937 gen;
     std::uniform_int_distribution<> dist;
 };

 /*
  * FillBuffer:
  * Fills a buffer with a constant value.
  */
 template <typename S>
 class FillBuffer : public IGenerator<S> {
 public:
     FillBuffer(S rmin, uint64_t mxiter);

     S *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;

 private:
     S rmin, value;
     uint64_t mxiter, i;
 };

 /*
  * MultiStepGenerator:
  * Generates values using a sub-generator and fills an array.
  */
 template <typename S>
 class MultiStepGenerator : public IGenerator<S> {
 public:
     MultiStepGenerator(S rmin, S rmax, uint64_t step,
                        RangeType step_type, size_t array_size);
     ~MultiStepGenerator() = default;

     S *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;
     S *get_array() const;

 private:
     S rmin, rmax;
     uint64_t step;
     RangeType step_type;
     size_t array_size;
     std::unique_ptr<IGenerator<S>> generator;

     // Custom deleter for aligned memory
     struct AlignedDeleter {
         void operator()(S* ptr) const {
             operator delete[](ptr, std::align_val_t(64));
         }
     };
     std::unique_ptr<S[], AlignedDeleter> array;
 };

 /*
  * MultiStepGenerator<std::complex<T>>:
  * Partial specialization — independent real and imaginary ranges (InpRng / RangeType).
  */
 template <typename T>
 class MultiStepGenerator<std::complex<T>> : public IGenerator<std::complex<T>> {
 public:
     MultiStepGenerator(std::complex<T> rmin, std::complex<T> rmax,
                        uint64_t step, RangeType step_type, size_t array_size);
     ~MultiStepGenerator() = default;

     std::complex<T> *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;
     std::complex<T> *get_array() const;

 private:
     size_t array_size;
     std::unique_ptr<IGenerator<T>> real_gen;
     std::unique_ptr<IGenerator<T>> imag_gen;

     struct AlignedDeleter {
         void operator()(std::complex<T> *ptr) const
         {
             operator delete[](ptr, std::align_val_t(64));
         }
     };
     std::unique_ptr<std::complex<T>[], AlignedDeleter> array;
 };

 /*
  * MultiStepGenerator<fc32_t> / <fc64_t>:
  * C complex API inputs (alm_test.h fc32_t, fc64_t) — same real/imag generator pairing.
  */
 template <>
 class MultiStepGenerator<fc32_t> : public IGenerator<fc32_t> {
 public:
     MultiStepGenerator(fc32_t rmin, fc32_t rmax, uint64_t step, RangeType step_type,
                        size_t array_size);
     ~MultiStepGenerator() = default;

     fc32_t *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;
     fc32_t *get_array() const;

 private:
     size_t array_size;
     std::unique_ptr<IGenerator<float>> real_gen;
     std::unique_ptr<IGenerator<float>> imag_gen;

     struct AlignedDeleter {
         void operator()(fc32_t *ptr) const
         {
             operator delete[](ptr, std::align_val_t(64));
         }
     };
     std::unique_ptr<fc32_t[], AlignedDeleter> array;
 };

 template <>
 class MultiStepGenerator<fc64_t> : public IGenerator<fc64_t> {
 public:
     MultiStepGenerator(fc64_t rmin, fc64_t rmax, uint64_t step, RangeType step_type,
                        size_t array_size);
     ~MultiStepGenerator() = default;

     fc64_t *next() override;
     bool has_next() const override;
     void reset() override;
     uint64_t get_index() override;
     fc64_t *get_array() const;

 private:
     size_t array_size;
     std::unique_ptr<IGenerator<double>> real_gen;
     std::unique_ptr<IGenerator<double>> imag_gen;

     struct AlignedDeleter {
         void operator()(fc64_t *ptr) const
         {
             operator delete[](ptr, std::align_val_t(64));
         }
     };
     std::unique_ptr<fc64_t[], AlignedDeleter> array;
 };/* Derived-function type alias */
template <typename S>
using DerivedFuncT = std::function<S(S, S)>;

/* Registry of named combining functions for DerivedGenerator */
template <typename S>
inline const std::unordered_map<std::string, DerivedFuncT<S>>& derived_func_map()
{
    static const std::unordered_map<std::string, DerivedFuncT<S>> map = {
        {"mul",     [](S a, S b) { return a * b; }},
        {"neg_mul", [](S a, S b) { return -(a * b); }}
    };
    return map;
}

/*
 * DerivedGenerator:
 * Generates a second input array by applying a combining function
 * to the primary generator's output and an auxiliary sub-generator.
 * Useful for bivariate functions like atan2(y, x) where independent
 * generators for y and x do not cover generation of desirable <x,y>
 * pairs. With DerivedGenerator the desired relationship between
 * x and y (y = f(x, z)) can be specified using DerivedFuncT<S>.
 * E.g. for atan2(y,x) we want to test for different z = y/x ratios,
 * so we can use "mul" with z_gen to generate y = x * z, where x is
 * generated by the primary generator, z is generated by the auxiliary
 * sub-generator and y is derived from the DerivedFuncT<S> = "mul".
 */
template <typename S>
class DerivedGenerator : public IGenerator<S> {
public:
    DerivedGenerator(MultiStepGenerator<S> &primary,
                     S z_srt, S z_stp, uint64_t z_count,
                     RangeType z_type, size_t array_size,
                     DerivedFuncT<S> func);
    ~DerivedGenerator() = default;

    S *next() override;
    bool has_next() const override;
    void reset() override;
    uint64_t get_index() override;

private:
    MultiStepGenerator<S> &primary;
    MultiStepGenerator<S> z_gen;
    DerivedFuncT<S> func;
    size_t array_size;

    struct AlignedDeleter {
        void operator()(S* ptr) const {
            operator delete[](ptr, std::align_val_t(64));
        }
    };
    std::unique_ptr<S[], AlignedDeleter> array;
};

/*
 * enumerate_combinations:
 * Returns all r-element combinations of indices in [0, n), in lexicographic
 * order. nCr entries, each a sorted vector of length r. If n == 0 or r == 0
 * or r > n, returns an empty vector.
 */
std::vector<std::vector<size_t>> enumerate_combinations(size_t n, size_t r);

/*
 * MultiRangeGenerator:
 * Generates per-iteration vectors whose lanes are drawn from N independent
 * sub-generators, cycling through nCr lane combinations. Used for univariate
 * APIs to simulate cross-subdomain workloads in vector code.
 *
 * If the number of referenced sub-domains n < lane_width, the ref list is
 * cycled to lane_width and a single trivial combo is used. Otherwise all
 * C(n, lane_width) combinations are enumerated in lexicographic order and
 * round-robined across successive next() calls.
 */
template <typename S>
class MultiRangeGenerator : public IGenerator<S> {
public:
    MultiRangeGenerator(const std::vector<InpRng<S>> &per_ref_ranges,
                        size_t lane_width);
    ~MultiRangeGenerator() = default;

    S *next() override;
    bool has_next() const override;
    void reset() override;
    uint64_t get_index() override;

private:
    size_t lane_width;
    std::vector<std::unique_ptr<IGenerator<S>>> subs;  // size = max(n, lane_width)
    std::vector<std::vector<size_t>>            combos;
    size_t   combo_idx;
    uint64_t i;

    struct AlignedDeleter {
        void operator()(S* ptr) const {
            operator delete[](ptr, std::align_val_t(64));
        }
    };
    std::unique_ptr<S[], AlignedDeleter> buf;
};

