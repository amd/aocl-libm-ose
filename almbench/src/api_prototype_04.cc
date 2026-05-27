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
#include "yaml_batch_writer.h"

/*
 * unit_test:
 * Executes a test function on a single input and compares the result
 * with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void unit_test(struct InParams<T, U>* ipp,
                      void (*ref_func)(U, UL*, UL*),
                      void (*shim_func)(struct InParams<T, U> *),
                      struct YamlOutputs<U> *yop,
                      YamlBatchWriter<U> *writer)
{
    yop->exception_raised = run_libm_api_with_exceptions<T, U>(shim_func, ipp);

    U* ip = reinterpret_cast<U*>(&ipp->ip[0]);
    U* ops = reinterpret_cast<U*>(&ipp->op[0]);
    U* opc = reinterpret_cast<U*>(&ipp->op[1]);

    UL msin, mcos;
    ref_func(ip[0], &msin, &mcos);
    double ulp;
    int uflag = update_ulp(ops[0], msin, writer->stats().udata, ulp);
    double ulp_sin = ulp;
    if (update_ulp(opc[0], mcos, writer->stats().udata, ulp) == TESTCASE_FAIL) {
        uflag = TESTCASE_FAIL;
    }
    ulp = std::max(ulp_sin, ulp);

    yop->n[0]      = 1;
    yop->iptr[0]   = ip;
    yop->optr[0]   = ops;
    yop->optr[1]   = opc;
    yop->ulp       = &ulp;
    yop->status    = &uflag;

    writer->push(yop);
}

/*
 * range_test:
 * Executes a test function over a range of inputs for non-VRA APIs.
 * Compares each result with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void range_test(struct InParams<T, U>* ipp,
                       void (*ref_func)(U, UL*, UL*),
                       void (*shim_func)(struct InParams<T, U> *),
                       struct YamlOutputs<U> *yop,
                       YamlBatchWriter<U> *writer)
{
    uint64_t elem  = sizeof(T) / sizeof(U);
    auto& range    = ipp->range[0];
    uint64_t count = align_to(range.count, elem);
    uint64_t N     = align_to(count, elem);
    yop->n[0]      = elem;
    double max_ulp[MAX_ELEM] = {0.0};
    int status[MAX_ELEM] = {0};
    yop->ulp             = max_ulp;
    yop->status          = &status[0];

    Runner<T, U>   runner(shim_func, yop->config);
    MultiStepGenerator<U> val(range.srt, range.stp, range.count, range.type, elem);

    FloatPacker<T> fp;
    double ulp;

    for (uint64_t i = 0; i < N; ++i) {
        U* ip = val.wrap_next();
        ipp->ip[0] = fp.pack(ip);

        yop->duration = runner.run(ipp);

        U* ops = reinterpret_cast<U*>(&ipp->op[0]);
        U* opc = reinterpret_cast<U*>(&ipp->op[1]);
        UL msin, mcos;
        int uflag;
        for (uint64_t j = 0; j < elem; ++j) {
            ref_func(ip[j], &msin, &mcos);
            uflag = update_ulp(ops[j], msin, writer->stats().udata, ulp);
            double ulp_sin = ulp;
            if (update_ulp(opc[j], mcos, writer->stats().udata, ulp) == TESTCASE_FAIL) {
                uflag = TESTCASE_FAIL;
            }
            status[j]  = uflag;
            max_ulp[j] = std::max(ulp_sin, ulp);
        }

        yop->iptr[0] = ip;
        yop->optr[0] = ops;
        yop->optr[1] = opc;
        writer->push(yop);
    }
}

/*
 * range_test_vra:
 * Executes a test function over a range of inputs for VRA APIs.
 * Compares each result with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void range_test_vra(struct InParams<T, U>* ipp,
                           void (*ref_func)(U, UL*, UL*),
                           void (*shim_func)(struct InParams<T, U> *),
                           YamlOutputs<U>* yop,
                           YamlBatchWriter<U> *writer)
{
    uint64_t elem  = sizeof(T) / sizeof(U);
    auto& range    = ipp->range[0];
    uint64_t count = range.count;
    uint64_t N     = align_to(count, elem);
    count =  (count >= 100) ? 100 : count ;

    std::vector<U> ops(count), opc(count);
    std::vector<double> max_ulp(count);
    std::vector<int> status(count);
    ipp->count   = count;
    ipp->optr[0] = ops.data();
    ipp->optr[1] = opc.data();
    yop->n[0]    = count;
    yop->optr[0] = ops.data();
    yop->optr[1] = opc.data();
    yop->ulp     = max_ulp.data();
    yop->status  = status.data();

    Runner<T, U>   runner(shim_func, yop->config);
    MultiStepGenerator<U> val(range.srt, range.stp, range.count, range.type, count);

    double ulp;

    for (uint64_t i = 0; i < N; ++i) {
        U* ip = val.wrap_next();
        ipp->iptr[0] = ip;

        yop->duration = runner.run(ipp);

        UL msin, mcos;
        int uflag;
        for (uint64_t j = 0; j < count; ++j) {
            ref_func(ip[j], &msin, &mcos);
            uflag = update_ulp(ops[j], msin, writer->stats().udata, ulp);
            double ulp_sin = ulp;
            if (update_ulp(opc[j], mcos, writer->stats().udata, ulp) == TESTCASE_FAIL) {
                uflag = TESTCASE_FAIL;
            }
            status[j]  = uflag;
            max_ulp[j] = std::max(ulp_sin, ulp);
        }

        yop->iptr[0] = ip;
        writer->push(yop);
    }
}

/*
 * api_prototype_04:
 * main dispatcher for dual-output APIs like sincos
 * selects unit, range, or vra test mode
 */
template <typename T, typename U>
int api_prototype_04(struct AlmLibs *alibs,
                     struct InParams<T, U>* ipp,
                     const std::string& libapi,
                     const std::string& refapi,
                     YamlOutputs<U>* yop,
                     YamlBatchWriter<U> *writer)
{
    using UL = typename mpfr::op_type<U>::mopt;

    auto shim_func = load_function<void (*)(struct InParams<T, U> *)>(alibs->pshimlib, libapi);
    auto ref_func  = load_function<void (*)(U, UL*, UL*)>(alibs->preflib, refapi);

    if (ipp->range.empty()) {
        unit_test<T, U, UL>(ipp, ref_func, shim_func, yop, writer);
    } else if (!yop->config.is_vra) {
        range_test<T, U, UL>(ipp, ref_func, shim_func, yop, writer);
    } else {
        range_test_vra<T, U, UL>(ipp, ref_func, shim_func, yop, writer);
    }

    return 0;
}

/*
 * Template instantiations:
 * Explicitly instantiate the api_prototype_04 function for supported
 * scalar and SIMD types with float and double precision.
 */
template int api_prototype_04<float, float>(
    struct AlmLibs *,
    struct InParams<float, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_04<double, double>(
    struct AlmLibs *,
    struct InParams<double, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

template int api_prototype_04<libm::AlignedM128, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_04<libm::AlignedM128d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

template int api_prototype_04<libm::AlignedM256, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_04<libm::AlignedM256d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

#ifdef __AVX512F__
template int api_prototype_04<libm::AlignedM512, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_04<libm::AlignedM512d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);
#endif

/*
 * C complex scalars (fc32_t / fc64_t): primary template is real-only; complex not supported.
 */
template <>
int api_prototype_04<fc32_t, fc32_t>(struct AlmLibs *,
                                     struct InParams<fc32_t, fc32_t> *,
                                     const std::string &,
                                     const std::string &,
                                     YamlOutputs<fc32_t> *,
                                     YamlBatchWriter<fc32_t> *)
{
    return -1;
}

template <>
int api_prototype_04<fc64_t, fc64_t>(struct AlmLibs *,
                                     struct InParams<fc64_t, fc64_t> *,
                                     const std::string &,
                                     const std::string &,
                                     YamlOutputs<fc64_t> *,
                                     YamlBatchWriter<fc64_t> *)
{
    return -1;
}