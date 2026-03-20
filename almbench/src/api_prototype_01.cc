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
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include "dll_utils.h"

#include "alm_test.h"
#include "api_template.h"
#include "ulp.h"
#include "packer.h"
#include "alm_mp_funcs.h"
#include "api_runner.h"

/*
 * unit_test:
 * Executes a test function on a single input and compares the result
 * with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void unit_test(struct InParams<T, U> *ipp,
                      UL (*ref_func)(U),
                      void (*shim_func)(struct InParams<T, U> *),
                      struct YamlOutputs<U> *yop)
{
    yop->exception_raised = run_libm_api_with_exceptions<T, U>(shim_func, ipp);
    U* ip = reinterpret_cast<U*>(&ipp->ip[0]);
    U* op = reinterpret_cast<U*>(&ipp->op[0]);

    UL mpfrop = ref_func(ip[0]);
    double ulp;
    ulp_data udata;
    int uflag = update_ulp(op[0], mpfrop, udata, ulp);

    yop->iptr[0] = ip;
    yop->optr[0] = op;
    yop->ulp     = &ulp;
    yop->status  = &uflag;
    write_yaml_output<U>(yop);
}

/*
 * range_test:
 * Executes a test function over a range of inputs for non-VRA APIs.
 * Compares each result with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void range_test(struct InParams<T, U> *ipp,
                       UL (*ref_func)(U),
                       void (*shim_func)(struct InParams<T, U> *),
                       struct YamlOutputs<U> *yop)
{
    uint64_t elem        = sizeof(T) / sizeof(U);
    auto &range          = ipp->range[0];
    uint64_t count       = align_to(range.count, elem);
    uint64_t N           = align_to(count, elem);
    yop->n[0]            = elem;
    double max_ulp[MAX_ELEM] = {0.0};
    int status[MAX_ELEM] = {0};
    yop->ulp             = &max_ulp[0];
    yop->status          = &status[0];

    Runner<T, U>   runner(shim_func, yop->test_mode);
    MultiStepGenerator<U> val(range.srt, range.stp, count, range.type, elem);

    FloatPacker<T> fp;
    ulp_data udata;
    double ulp;

    for (uint64_t i = 0; i < N; ++i) {
        U *ip = val.wrap_next();
        ipp->ip[0] = fp.pack(ip);

        yop->duration = runner.run(ipp);

        U *op = (U *)&ipp->op[0];
        for (uint64_t j = 0; j < elem; ++j) {
            UL mpfrop  = ref_func(ip[j]);
            status[j]  = update_ulp(op[j], mpfrop, udata, ulp);
            max_ulp[j] = ulp;
        }

        yop->iptr[0] = ip;
        yop->optr[0] = op;
        write_yaml_output<U>(yop);
    }
}

/*
 * range_test_vra:
 * Executes a test function over a range of inputs for VRA APIs.
 * Compares each result with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void range_test_vra(struct InParams<T, U> *ipp,
                           UL (*ref_func)(U),
                           void (*shim_func)(struct InParams<T, U> *),
                           struct YamlOutputs<U> *yop)
{
    uint64_t elem  = sizeof(T) / sizeof(U);
    auto &range    = ipp->range[0];
    uint64_t count = range.count;
    uint64_t N     = align_to(count, elem);
    count =  (count >= 100) ? 100 : count ;
    ipp->count     = count;
    std::vector<U> op(count);
    std::vector<double> max_ulp(count);
    std::vector<int> status(count);

    ipp->optr[0]   = op.data();
    yop->n[0]      = count;
    yop->ulp       = max_ulp.data();
    yop->optr[0]   = op.data();
    yop->status    = status.data();

    ulp_data udata;
    double ulp;

    Runner<T, U>   runner(shim_func, yop->test_mode);
    MultiStepGenerator<U> val(range.srt, range.stp, range.count, range.type, count);

    for (uint64_t i = 0; i < N; ++i) {
        U *ip = val.wrap_next();
        ipp->iptr[0] = ip;

        yop->duration = runner.run(ipp);

        for (uint64_t j = 0; j < count; ++j) {
            UL mpfrop  = ref_func(ip[j]);
            status[j]  = update_ulp(op[j], mpfrop, udata, ulp);
            max_ulp[j] = ulp;
        }

        yop->iptr[0] = ip;
        write_yaml_output<U>(yop);
    }
}

/*
 * api_prototype_01:
 * Main dispatcher function that selects the appropriate test mode
 * (unit_test, range, or VRA) and executes the test.
 */
template <typename T, typename U>
int api_prototype_01(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop)
{
    using UL = typename mpfr::op_type<U>::mopt;

    auto shim_func = load_function<void (*)(struct InParams<T, U> *)>(alibs->pshimlib, libapi);
    auto ref_func  = load_function<UL (*)(U)>(alibs->preflib, refapi);

    if (ipp->range.empty()) {
        unit_test<T, U, UL>(ipp, ref_func, shim_func, yop);
    } else if (!yop->is_vra) {
        range_test<T, U, UL>(ipp, ref_func, shim_func, yop);
    } else {
        range_test_vra<T, U, UL>(ipp, ref_func, shim_func, yop);
    }

    return 0;
}

/*
 * Template instantiations:
 * Explicitly instantiate the api_prototype_01 function for supported
 * scalar and SIMD types with float and double precision.
 */
template int api_prototype_01<float, float>(
    struct AlmLibs *,
    struct InParams<float, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_01<double, double>(
    struct AlmLibs *,
    struct InParams<double, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);

template int api_prototype_01<libm::AlignedM128, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_01<libm::AlignedM128d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);

template int api_prototype_01<libm::AlignedM256, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_01<libm::AlignedM256d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);

#ifdef __AVX512F__
template int api_prototype_01<libm::AlignedM512, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_01<libm::AlignedM512d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);
#endif