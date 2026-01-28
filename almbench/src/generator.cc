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


#include "generator.h"

/* BitGenerator: Generates values by stepping through bit patterns */
template <typename S, typename U>
BitGenerator<S, U>::BitGenerator(S rmin, S rmax, S bitstep)
    : bitstep(bitstep), start{rmin}, stop{rmax}, i(0)
{
    bd = std::abs((int64_t)stop.i - (int64_t)start.i);
    count = bd / bitstep;
    mxiter = count + 1;
}

template <typename S, typename U>
S *BitGenerator<S, U>::next()
{
    value.i = start.i + i * bitstep;
    i++;
    return &value.f;
}

template <typename S, typename U>
bool BitGenerator<S, U>::has_next() const
{
    return (i < mxiter);
}

template <typename S, typename U>
uint64_t BitGenerator<S, U>::get_index()
{
    return i;
}

template <typename S, typename U>
void BitGenerator<S, U>::reset()
{
    value.i = start.i;
    i = 0;
}

/* LinearGenerator: Generates linearly spaced values */
template <typename S>
LinearGenerator<S>::LinearGenerator(S rmin, S rmax, uint64_t mxiter)
    : rmin(rmin), rmax(rmax), value(rmin),
      step((rmax - rmin) / mxiter), mxiter(mxiter + 1), i(0) {}

template <typename S>
S *LinearGenerator<S>::next()
{
    value = rmin + (i * step);
    i++;
    if (value > rmax) {
        value = rmax;
    }
    return &value;
}

template <typename S>
bool LinearGenerator<S>::has_next() const
{
    return (i < mxiter);
}

template <typename S>
uint64_t LinearGenerator<S>::get_index()
{
    return i;
}

template <typename S>
void LinearGenerator<S>::reset()
{
    value = rmin;
    i = 0;
}

/* ExponentialGenerator: Generates exponentially spaced values */
template <typename S>
ExponentialGenerator<S>::ExponentialGenerator(S rmn, S rmx, uint64_t mxiter)
    : rmin(std::log(std::fabs(rmn))),
      rmax(std::log(std::fabs(rmx))),
      value(rmin),
      step((rmax - rmin) / (S)mxiter),
      mxiter(mxiter + 1),
      i(0) {}

template <typename S>
S *ExponentialGenerator<S>::next()
{
    value = std::exp(rmin + (i * step));
    i++;
    if (std::signbit(rmax)) {
        value = -value;
    }
    return &value;
}

template <typename S>
bool ExponentialGenerator<S>::has_next() const
{
    return (i < mxiter);
}

template <typename S>
uint64_t ExponentialGenerator<S>::get_index()
{
    return i;
}

template <typename S>
void ExponentialGenerator<S>::reset()
{
    value = rmin;
    i = 0;
}

/* RandomGenerator: Generates random floating-point values */
template <typename S>
RandomGenerator<S>::RandomGenerator(S rmin, S rmax, uint64_t cnt)
    : rmin(rmin), rmax(rmax), value(rmin),
      mxiter(cnt + 1), i(0), rd(), gen(rd()), dist(rmin, rmax) {}

template <typename S>
S *RandomGenerator<S>::next()
{
    value = dist(gen);
    i++;
    return &value;
}

template <typename S>
bool RandomGenerator<S>::has_next() const
{
    return (i < mxiter) &&
           std::abs(value - rmax) > std::numeric_limits<S>::epsilon();
}

template <typename S>
uint64_t RandomGenerator<S>::get_index()
{
    return i;
}

template <typename S>
void RandomGenerator<S>::reset()
{
    value = rmin;
    i = 0;
}

/* IntegerRandomGenerator: Generates random integer values */
template <typename S>
IntegerRandomGenerator<S>::IntegerRandomGenerator(S rmin, S rmax, uint64_t cnt)
    : rmin(rmin), rmax(rmax), value(rmin),
      mxiter(cnt + 1), i(0), rd(), gen(rd()), dist(rmin, rmax) {}

template <typename S>
S *IntegerRandomGenerator<S>::next()
{
    value = dist(gen);
    i++;
    return &value;
}

template <typename S>
bool IntegerRandomGenerator<S>::has_next() const
{
    return (i < mxiter) &&
           std::abs(value - rmax) > std::numeric_limits<S>::epsilon();
}

template <typename S>
uint64_t IntegerRandomGenerator<S>::get_index()
{
    return i;
}

template <typename S>
void IntegerRandomGenerator<S>::reset()
{
    value = rmin;
    i = 0;
}

/* FillBuffer: Fills buffer with constant value */
template <typename S>
FillBuffer<S>::FillBuffer(S rmin, uint64_t mxiter)
    : rmin(rmin), value(rmin), mxiter(mxiter + 1), i(0) {}

template <typename S>
S *FillBuffer<S>::next()
{
    value = rmin;
    i++;
    return &value;
}

template <typename S>
bool FillBuffer<S>::has_next() const
{
    return (i < mxiter);
}

template <typename S>
uint64_t FillBuffer<S>::get_index()
{
    return i;
}

template <typename S>
void FillBuffer<S>::reset()
{
    value = rmin;
    i = 0;
}

/* MultiStepGenerator: Uses sub-generator to fill an array */
template <typename S>
MultiStepGenerator<S>::MultiStepGenerator(S rmin, S rmax, uint64_t step,
                                          RangeType step_type, size_t array_size)
    : rmin(rmin), rmax(rmax), step(step),
      step_type(step_type), array_size(array_size),
      array(nullptr)
{
    switch (step_type) {
    case E_Bitstep:
        if constexpr (!std::is_same_v<S, int>) {
            generator = std::make_unique<BitGenerator<S>>(rmin, rmax, step);
        } else {
            throw std::invalid_argument("BitGenerator cannot be used with int type");
        }
        break;
    case E_Linear:
        generator = std::make_unique<LinearGenerator<S>>(rmin, rmax, step);
        break;
    case E_Expstep:
        generator = std::make_unique<ExponentialGenerator<S>>(rmin, rmax, step);
        break;
    case E_Random:
        generator = std::make_unique<RandomGenerator<S>>(rmin, rmax, step);
        break;
    case E_Integer:
        generator = std::make_unique<IntegerRandomGenerator<S>>(rmin, rmax, step);
        break;
    case E_Fixedval:
        generator = std::make_unique<FillBuffer<S>>(rmin, step);
        break;
    case E_Simple:
    case E_MAX:
        break;
    default:
        std::cout << "Invalid RangeType" << std::endl;
        break;
    }

    // Use std::unique_ptr with aligned allocation
    S* raw_array = new (std::align_val_t(64)) S[array_size];
    array.reset(raw_array);
}

template <typename S>
S *MultiStepGenerator<S>::next()
{
    for (size_t i = 0; i < array_size; i++) {
        array[i] = *generator->next();
    }
    return array.get();
}

template <typename S>
bool MultiStepGenerator<S>::has_next() const
{
    return generator->has_next();
}

template <typename S>
S *MultiStepGenerator<S>::get_array() const
{
    return array.get();
}

template <typename S>
void MultiStepGenerator<S>::reset()
{
    generator->reset();
}

template <typename S>
uint64_t MultiStepGenerator<S>::get_index()
{
    return generator->get_index();
}

/* Explicit instantiations */
template class MultiStepGenerator<float>;
template class MultiStepGenerator<double>;