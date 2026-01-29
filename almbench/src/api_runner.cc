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

#include "alm_test.h"
#include "api_runner.h"
#include "numeric_wrapper.h"

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

template <typename T, typename U>
Runner<T, U>::Runner(void (*shim)(InParams<T, U>*), TestMode mode, uint64_t iterations)
    : shim_func(shim), iterations(iterations) {
    run_libm_api = (mode == TestMode::E_PERFORMANCE) ? &Runner::run_perf : &Runner::run_accu;
}

template <typename T, typename U>
double Runner<T, U>::run(InParams<T, U>* ipp) {
    return (this->*run_libm_api)(ipp);
}

template <typename T, typename U>
double Runner<T, U>::run_perf(InParams<T, U>* ipp) {
    std::vector<double> durations;
    durations.reserve(iterations);

    for (uint64_t t = 0; t < iterations; ++t) {
        timing_wrapper perf;
        perf.start();
        shim_func(ipp);
        durations.push_back(perf.stop());
    }

    double mtime = *std::min_element(durations.begin(), durations.end());
    return mtime;
}

template <typename T, typename U>
double Runner<T, U>::run_accu(InParams<T, U>* ipp) {
    shim_func(ipp);
    return 0.0;
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