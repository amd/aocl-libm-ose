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


#include "generator.h"

#include <complex>
#include <stdexcept>
#include <type_traits>

namespace {

template <typename T>
std::unique_ptr<IGenerator<T>> make_scalar_generator(T rmin, T rmax, uint64_t step,
                                                     RangeType step_type)
{
    switch (step_type) {
    case E_Bitstep:
        if constexpr (!std::is_same_v<T, int>) {
            return std::make_unique<BitGenerator<T>>(rmin, rmax, static_cast<T>(step));
        } else {
            throw std::invalid_argument("BitGenerator cannot be used with int type");
        }
    case E_Linear:
        return std::make_unique<LinearGenerator<T>>(rmin, rmax, step);
    case E_Expstep:
        return std::make_unique<ExponentialGenerator<T>>(rmin, rmax, step);
    case E_Random:
        return std::make_unique<RandomGenerator<T>>(rmin, rmax, step);
    case E_Integer:
        return std::make_unique<IntegerRandomGenerator<T>>(rmin, rmax, step);
    case E_Fixedval:
        return std::make_unique<FillBuffer<T>>(rmin, step);
    case E_Simple:
    case E_MAX:
        break;
    default:
        std::cout << "Invalid RangeType" << std::endl;
        break;
    }
    return nullptr;
}

} /* namespace */

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
    return i < mxiter;
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
    generator = make_scalar_generator(rmin, rmax, step, step_type);

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

/* MultiStepGenerator<std::complex<T>> */
template <typename T>
MultiStepGenerator<std::complex<T>>::MultiStepGenerator(std::complex<T> rmin,
                                                        std::complex<T> rmax,
                                                        uint64_t step,
                                                        RangeType step_type,
                                                        size_t array_size)
    : array_size(array_size),
      real_gen(make_scalar_generator(rmin.real(), rmax.real(), step, step_type)),
      imag_gen(make_scalar_generator(rmin.imag(), rmax.imag(), step, step_type))
{
    std::complex<T> *raw_array =
        new (std::align_val_t(64)) std::complex<T>[array_size];
    array.reset(raw_array);
}

template <typename T>
std::complex<T> *MultiStepGenerator<std::complex<T>>::next()
{
    for (size_t i = 0; i < array_size; i++) {
        array[i] = std::complex<T>(*real_gen->next(), *imag_gen->next());
    }
    return array.get();
}

template <typename T>
bool MultiStepGenerator<std::complex<T>>::has_next() const
{
    return real_gen->has_next() && imag_gen->has_next();
}

template <typename T>
std::complex<T> *MultiStepGenerator<std::complex<T>>::get_array() const
{
    return array.get();
}

template <typename T>
void MultiStepGenerator<std::complex<T>>::reset()
{
    real_gen->reset();
    imag_gen->reset();
}

template <typename T>
uint64_t MultiStepGenerator<std::complex<T>>::get_index()
{
    return real_gen->get_index();
}

/* MultiStepGenerator<fc32_t> */
MultiStepGenerator<fc32_t>::MultiStepGenerator(fc32_t rmin, fc32_t rmax, uint64_t step,
                                               RangeType step_type, size_t array_size)
    : array_size(array_size),
      real_gen(make_scalar_generator(fc_real(rmin), fc_real(rmax), step, step_type)),
      imag_gen(make_scalar_generator(fc_imag(rmin), fc_imag(rmax), step, step_type))
{
    fc32_t *raw_array = new (std::align_val_t(64)) fc32_t[array_size];
    array.reset(raw_array);
}

fc32_t *MultiStepGenerator<fc32_t>::next()
{
    for (size_t i = 0; i < array_size; i++) {
        array[i] = libm::fc_pack(*real_gen->next(), *imag_gen->next());
    }
    return array.get();
}

bool MultiStepGenerator<fc32_t>::has_next() const
{
    return real_gen->has_next() && imag_gen->has_next();
}

fc32_t *MultiStepGenerator<fc32_t>::get_array() const
{
    return array.get();
}

void MultiStepGenerator<fc32_t>::reset()
{
    real_gen->reset();
    imag_gen->reset();
}

uint64_t MultiStepGenerator<fc32_t>::get_index()
{
    return real_gen->get_index();
}

/* MultiStepGenerator<fc64_t> */
MultiStepGenerator<fc64_t>::MultiStepGenerator(fc64_t rmin, fc64_t rmax, uint64_t step,
                                               RangeType step_type, size_t array_size)
    : array_size(array_size),
      real_gen(make_scalar_generator(fc_real(rmin), fc_real(rmax), step, step_type)),
      imag_gen(make_scalar_generator(fc_imag(rmin), fc_imag(rmax), step, step_type))
{
    fc64_t *raw_array = new (std::align_val_t(64)) fc64_t[array_size];
    array.reset(raw_array);
}

fc64_t *MultiStepGenerator<fc64_t>::next()
{
    for (size_t i = 0; i < array_size; i++) {
        array[i] = libm::fc_pack(*real_gen->next(), *imag_gen->next());
    }
    return array.get();
}

bool MultiStepGenerator<fc64_t>::has_next() const
{
    return real_gen->has_next() && imag_gen->has_next();
}

fc64_t *MultiStepGenerator<fc64_t>::get_array() const
{
    return array.get();
}

void MultiStepGenerator<fc64_t>::reset()
{
    real_gen->reset();
    imag_gen->reset();
}

uint64_t MultiStepGenerator<fc64_t>::get_index()
{
    return real_gen->get_index();
}

/* Explicit instantiations */
template class MultiStepGenerator<float>;
template class MultiStepGenerator<double>;
template class MultiStepGenerator<std::complex<float>>;
template class MultiStepGenerator<std::complex<double>>;

/* DerivedGenerator implementation */
template <typename S>
DerivedGenerator<S>::DerivedGenerator(MultiStepGenerator<S> &primary,
                                     S z_srt, S z_stp, uint64_t z_count,
                                     RangeType z_type, size_t array_size,
                                     DerivedFuncT<S> func)
    : primary(primary),
      z_gen(z_srt, z_stp, z_count, z_type, array_size),
      func(std::move(func)),
      array_size(array_size),
      array(nullptr)
{
    // Use std::unique_ptr with aligned allocation
    S* raw_array = new (std::align_val_t(64)) S[array_size];
    array.reset(raw_array);
}

/*
 * DerivedGenerator::next():
 * Combines the primary array with the z sub-generator element-wise.
 * Fills array_size elements per call (one SIMD vector's worth).
 *
 * Primary and z_gen may have different step counts (count vs z_count).
 * If z_count < count, z_gen wraps around (repeating earlier z values).
 * If z_count > count, primary wraps first. This allows independent
 * control of x-sweep density vs z-sweep density in the YAML config.
 *
 * Implicit contracts expected from the caller:
 *  - primary.wrap_next() must be called BEFORE derived.wrap_next()
 *    so that get_array() returns the current iteration's x values.
 *  - primary, z_gen, and this generator must all share the same
 *    array_size (enforced by passing the same value at construction).
 */
template <typename S>
S *DerivedGenerator<S>::next()
{
    S *x = primary.get_array();
    S *z = z_gen.wrap_next();
    for (size_t i = 0; i < array_size; i++) {
        array[i] = func(x[i], z[i]);
    }
    return array.get();
}

template <typename S>
bool DerivedGenerator<S>::has_next() const
{
    return z_gen.has_next();
}

template <typename S>
void DerivedGenerator<S>::reset()
{
    z_gen.reset();
}

template <typename S>
uint64_t DerivedGenerator<S>::get_index()
{
    return z_gen.get_index();
}

template class DerivedGenerator<float>;
template class DerivedGenerator<double>;

/*
 * enumerate_combinations:
 * Lexicographic nCr enumeration over indices [0, n). Each result is sorted
 * ascending, length r. Returns empty vector when r == 0, n == 0, or r > n.
 */
std::vector<std::vector<size_t>> enumerate_combinations(size_t n, size_t r)
{
    std::vector<std::vector<size_t>> out;
    if (r == 0 || n == 0 || r > n) {
        return out;
    }

    std::vector<size_t> idx(r);
    std::iota(idx.begin(), idx.end(), 0);

    /* Generate using successor rule for lexicographic combinations */
    while (true) {
        out.push_back(idx);
        ptrdiff_t i = static_cast<ptrdiff_t>(r) - 1;
        while (i >= 0 && idx[i] == n - r + static_cast<size_t>(i)) {
            --i;
        }
        if (i < 0) {
            break;
        }
        ++idx[i];
        for (size_t j = static_cast<size_t>(i) + 1; j < r; ++j) {
            idx[j] = idx[j - 1] + 1;
        }
    }
    return out;
}

/*
 * make_multistep_subgen:
 * Helper that constructs a scalar MultiStepGenerator<S> (array_size=1) from
 * a typed InpRng<S>.  rng.count arrives as (steps / n) from libm_process;
 * we multiply by lane_width here so each sub-gen covers (steps * lane_width / n)
 * scalar values, matching the call rate each sub-gen actually receives
 * across all outer iterations.
 */
template <typename S>
static std::unique_ptr<MultiStepGenerator<S>>
make_multistep_subgen(const InpRng<S> &rng, size_t lane_width)
{
    return std::make_unique<MultiStepGenerator<S>>(
        rng.srt, rng.stp, rng.count * lane_width, rng.type, 1);
}

/* MultiRangeGenerator implementation */
template <typename S>
MultiRangeGenerator<S>::MultiRangeGenerator(
    const std::vector<InpRng<S>> &per_ref_ranges, size_t lane_width_)
    : lane_width(lane_width_), combo_idx(0), i(0), buf(nullptr)
{
    const size_t n_orig = per_ref_ranges.size();
    const size_t n      = std::max(n_orig, lane_width);

    subs.reserve(n);
    for (size_t k = 0; k < n; ++k) {
        const auto &rng = per_ref_ranges[k % n_orig];
        subs.push_back(make_multistep_subgen<S>(rng, lane_width));
    }

    combos = enumerate_combinations(n, lane_width);
    if (combos.empty()) {
        throw std::logic_error(
            "enumerate_combinations returned empty: n=" + std::to_string(n) +
            " lane_width=" + std::to_string(lane_width) +
            " — precondition violated (n >= lane_width must hold)");
    }

    S *raw = new (std::align_val_t(64)) S[lane_width];
    buf.reset(raw);
}

template <typename S>
S *MultiRangeGenerator<S>::next()
{
    const auto &combo = combos[combo_idx];
    for (size_t k = 0; k < lane_width; ++k) {
        buf[k] = *subs[combo[k]]->wrap_next();
    }
    combo_idx = (combo_idx + 1) % combos.size();
    ++i;
    return buf.get();
}

template <typename S>
bool MultiRangeGenerator<S>::has_next() const
{
    /*
     * MultiRangeGenerator is unbounded; the outer caller bounds total
     * iterations via steps_mr. wrap_next() in sub-gens handles their own
     * reset transparently.
     */
    return true;
}

template <typename S>
void MultiRangeGenerator<S>::reset()
{
    for (auto &g : subs) g->reset();
    combo_idx = 0;
    i = 0;
}

template <typename S>
uint64_t MultiRangeGenerator<S>::get_index()
{
    return i;
}

template class MultiRangeGenerator<float>;
template class MultiRangeGenerator<double>;
