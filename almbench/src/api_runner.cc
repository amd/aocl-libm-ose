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

#include "alm_test.h"
#include "api_runner.h"
#include "numeric_wrapper.h"
#include <type_traits>

/*
 * load_function:
 * Load a function symbol from a shared library using platform-specific APIs.
 * If the symbol is not found, print an error, close the library,
 * and exit the program.
 */
template <typename FuncType>
FuncType load_function(DL_HANDLE lib, const std::string &name)
{
    void *symbol = DL_SYM(lib, name.c_str());

    if (!symbol) {
        std::cerr << "Failed to load symbol '" << name << "': " << DL_ERROR() << std::endl;
        DL_CLOSE(lib);
        std::exit(EXIT_FAILURE);
    }

    return reinterpret_cast<FuncType>(symbol);
}

/*
 * Explicit template instantiations for various function signatures.
 * These ensure the compiler generates the required code for each type.
 */
template double (*load_function(DL_HANDLE, const std::string&))(float);
template long double (*load_function(DL_HANDLE, const std::string&))(double);

template lint_t (*load_function(DL_HANDLE, const std::string&))(float);
template lint_t (*load_function(DL_HANDLE, const std::string&))(double);

template llint_t (*load_function(DL_HANDLE, const std::string&))(float);
template llint_t (*load_function(DL_HANDLE, const std::string&))(double);

template double (*load_function(DL_HANDLE, const std::string&))(float, float);
template long double (*load_function(DL_HANDLE, const std::string&))(double, double);

template void (*load_function(DL_HANDLE, const std::string&))(float, double*, double*);
template void (*load_function(DL_HANDLE, const std::string&))(double, long double*, long double*);

template double (*load_function(DL_HANDLE, const std::string&))(float, float, float, float, float, float);
template long double (*load_function(DL_HANDLE, const std::string&))(double, double, double, double, double, double);

template void (*load_function(DL_HANDLE, const std::string&))(InParams<float, float>*);
template void (*load_function(DL_HANDLE, const std::string&))(InParams<double, double>*);

template void (*load_function(DL_HANDLE, const std::string&))(InParams<libm::AlignedM128, float>*);
template void (*load_function(DL_HANDLE, const std::string&))(InParams<libm::AlignedM128d, double>*);

template void (*load_function(DL_HANDLE, const std::string&))(InParams<libm::AlignedM256, float>*);
template void (*load_function(DL_HANDLE, const std::string&))(InParams<libm::AlignedM256d, double>*);

#ifdef __AVX512F__
template void (*load_function(DL_HANDLE, const std::string&))(InParams<libm::AlignedM512, float>*);
template void (*load_function(DL_HANDLE, const std::string&))(InParams<libm::AlignedM512d, double>*);
#endif

template fc64_t (*load_function(DL_HANDLE, const std::string&))(fc32_t);
template fc128_t (*load_function(DL_HANDLE, const std::string&))(fc64_t);

/* Two-argument complex (e.g. cpow): mpfr::op_type<U>::mopt for U = fc32_t / fc64_t */
template fc64_t (*load_function(DL_HANDLE, const std::string&))(fc32_t, fc32_t);
template fc128_t (*load_function(DL_HANDLE, const std::string&))(fc64_t, fc64_t);

template void (*load_function(DL_HANDLE, const std::string&))(InParams<fc32_t, fc32_t>*);
template void (*load_function(DL_HANDLE, const std::string&))(InParams<fc64_t, fc64_t>*);


/*
 * run_libm_api_with_exceptions:
 * Executes a shim function and checks if any floating-point exceptions were raised.
 * Returns the raised exception flags.
 */
template <typename T, typename U>
int run_libm_api_with_exceptions(void (*shim_func)(InParams<T, U>*), InParams<T, U>* ipp) {
    std::feclearexcept(FE_ALL_EXCEPT);
    shim_func(ipp);
    int raised_exception = std::fetestexcept(FE_ALL_EXCEPT);
    std::feclearexcept(FE_ALL_EXCEPT);
    return raised_exception;
}
/*
 * Explicit template instantiations for run_libm_api_with_exceptions.
 * These ensure the compiler generates the required code for each type.
 */
template int run_libm_api_with_exceptions<float, float>(void (*)(InParams<float, float>*), InParams<float, float>*);
template int run_libm_api_with_exceptions<double, double>(void (*)(InParams<double, double>*), InParams<double, double>*);

template int run_libm_api_with_exceptions<libm::AlignedM128, float>(void (*)(InParams<libm::AlignedM128, float>*), InParams<libm::AlignedM128, float>*);
template int run_libm_api_with_exceptions<libm::AlignedM128d, double>(void (*)(InParams<libm::AlignedM128d, double>*), InParams<libm::AlignedM128d, double>*);

template int run_libm_api_with_exceptions<libm::AlignedM256, float>(void (*)(InParams<libm::AlignedM256, float>*), InParams<libm::AlignedM256, float>*);
template int run_libm_api_with_exceptions<libm::AlignedM256d, double>(void (*)(InParams<libm::AlignedM256d, double>*), InParams<libm::AlignedM256d, double>*);

#ifdef __AVX512F__
template int run_libm_api_with_exceptions<libm::AlignedM512, float>(void (*)(InParams<libm::AlignedM512, float>*), InParams<libm::AlignedM512, float>*);
template int run_libm_api_with_exceptions<libm::AlignedM512d, double>(void (*)(InParams<libm::AlignedM512d, double>*), InParams<libm::AlignedM512d, double>*);
#endif

template int run_libm_api_with_exceptions<fc32_t, fc32_t>(void (*)(InParams<fc32_t, fc32_t>*), InParams<fc32_t, fc32_t>*);
template int run_libm_api_with_exceptions<fc64_t, fc64_t>(void (*)(InParams<fc64_t, fc64_t>*), InParams<fc64_t, fc64_t>*);

template <typename T, typename U>
Runner<T, U>::Runner(void (*shim)(InParams<T, U>*), const TestConfig &config, uint64_t iterations)
    : shim_func(shim), iterations(iterations),
      warmup_count(config.warmup_count), batch_size(config.batch_size),
      is_vra(config.is_vra) {
    if (config.test_mode == TestMode::E_PERFORMANCE) {
        run_libm_api = (config.perf_mode == PerfMode::E_LATENCY)
            ? &Runner::run_perf_latency
            : &Runner::run_perf;
    } else {
        run_libm_api = &Runner::run_accu;
    }
}

template <typename T, typename U>
double Runner<T, U>::run(InParams<T, U>* ipp) {
    return (this->*run_libm_api)(ipp);
}

template <typename T, typename U>
double Runner<T, U>::run_perf(InParams<T, U>* ipp) {
    /* Warmup: execute untimed calls to prime caches and/or branch predictors */
    for (uint64_t w = 0; w < warmup_count; ++w) {
        shim_func(ipp);
    }

    /* Timed iterations: each iteration calls the API batch_size times */
    std::vector<double> durations;
    durations.reserve(iterations);

    for (uint64_t t = 0; t < iterations; ++t) {
        timing_wrapper perf;
        perf.start();
        for (uint64_t b = 0; b < batch_size; ++b) {
            shim_func(ipp);
        }
        durations.push_back(perf.stop());
    }

    double mtime = *std::min_element(durations.begin(), durations.end());
    return mtime / static_cast<double>(batch_size);
}

template <typename T, typename U>
double Runner<T, U>::run_accu(InParams<T, U>* ipp) {
    shim_func(ipp);
    return 0.0;
}

/*
 * run_perf_latency:
 * Measures single-call latency by introducing a data dependency between
 * successive iterations: the low significand bits of the first input element
 * are overwritten with bits from the previous output before each call.
 * This prevents the CPU from pipelining iterations, yielding true latency
 * rather than a throughput-amortised figure.
 */
template <typename T, typename U>
double Runner<T, U>::run_perf_latency(InParams<T, U>* ipp) {
    /* Masks for dependency injection (low significand bits). */
    static constexpr uint64_t DEP_MASK_64 = 0xFFULL;
    static constexpr uint32_t DEP_MASK_32 = 0xFFU;

    U *inp0  = is_vra ? ipp->iptr[0] : reinterpret_cast<U*>(&ipp->ip[0]);
    U *outp0 = is_vra ? ipp->optr[0] : reinterpret_cast<U*>(&ipp->op[0]);
    if (!inp0 || !outp0) {
        return 0.0; /* Invalid pointers */
    }

    /* Warmup: execute untimed calls to prime caches and/or branch predictors */
    for (uint64_t w = 0; w < warmup_count; ++w) {
        shim_func(ipp);
    }

    /* Timed iterations: each iteration calls the API batch_size times */
    std::vector<double> durations;
    durations.reserve(iterations);

    for (uint64_t t = 0; t < iterations; ++t) {
        uint64_t dep = 0;
        timing_wrapper perf;
        perf.start();
        for (uint64_t b = 0; b < batch_size; ++b) {
            /* Replace low significand bits of the first input element with
             * bits extracted from the previous output. Force the CPU to wait
             * for the store to complete before issuing the next call. */
            if constexpr (std::is_same_v<U, double>) {
                uint64_t bits = asuint64(inp0[0]);
                inp0[0] = asdouble((bits & ~DEP_MASK_64) | (dep & DEP_MASK_64));
            } else if constexpr (std::is_same_v<U, float>) {
                uint32_t bits = asuint32(inp0[0]);
                inp0[0] = asfloat((bits & ~DEP_MASK_32) | (static_cast<uint32_t>(dep) & DEP_MASK_32));
            } else if constexpr (std::is_same_v<U, fc64_t>) {
                uint64_t bits = asuint64(fc_real(inp0[0]));
                fc_set_real(inp0[0], asdouble((bits & ~DEP_MASK_64) | (dep & DEP_MASK_64)));
            } else if constexpr (std::is_same_v<U, fc32_t>) {
                uint32_t bits = asuint32(fc_real(inp0[0]));
                fc_set_real(inp0[0], asfloat((bits & ~DEP_MASK_32) | (static_cast<uint32_t>(dep) & DEP_MASK_32)));
            }

            shim_func(ipp);

            /* Extract bits from first output element for next iteration. */
            if constexpr (std::is_same_v<U, double>) {
                dep = asuint64(outp0[0]);
            } else if constexpr (std::is_same_v<U, float>) {
                dep = asuint32(outp0[0]);
            } else if constexpr (std::is_same_v<U, fc64_t>) {
                dep = asuint64(fc_real(outp0[0]));
            } else if constexpr (std::is_same_v<U, fc32_t>) {
                dep = asuint32(fc_real(outp0[0]));
            }
        }
        durations.push_back(perf.stop());
    }

    double mtime = *std::min_element(durations.begin(), durations.end());
    return mtime / static_cast<double>(batch_size);
}

/* Explicit template instantiations for Runner class.
 * These ensure the compiler generates the required code for each type.
 */
template class Runner<float, float>;
template class Runner<double, double>;
template class Runner<libm::AlignedM128, float>;
template class Runner<libm::AlignedM128d, double>;
template class Runner<libm::AlignedM256, float>;
template class Runner<libm::AlignedM256d, double>;
#ifdef __AVX512F__
template class Runner<libm::AlignedM512, float>;
template class Runner<libm::AlignedM512d, double>;
#endif

template class Runner<fc32_t, fc32_t>;
template class Runner<fc64_t, fc64_t>;