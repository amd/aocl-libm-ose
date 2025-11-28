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

#pragma once

#include <cmath>
#include <random>
#include <memory>
#include <iterator>
#include <limits>
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
        using pointer = S *;
        using reference = S &;

        Iterator(IGenerator *generator)
            : generator(generator), value(generator ? generator->next() : NULL) {}

        S operator*() const { return *value; }

        Iterator &operator++()
        {
            if (generator->has_next()) {
                value = generator->next();
            } else {
                generator = NULL;
                value = NULL;
            }
            return *this;
        }

        bool operator<=(const Iterator &other) const
        {
            return *value <= *other.value;
        }

    private:
        IGenerator *generator;
        S *value;
    };

    Iterator begin() { return Iterator(this); }
    Iterator end() { return Iterator(NULL); }
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

