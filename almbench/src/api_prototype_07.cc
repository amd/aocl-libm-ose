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

/*
 * api_prototype_07.cc
 *
 * Bench path for APIs with one floating-point input and one integer output
 * (e.g., lround, llround). Input type U is float or double; output type I
 * is an integral type.
 *
 * The shim stores its integer result in InParams::iop.{l,ll}.
 * The reference (mparith) function returns I directly.
 * ULP comparison uses update_ulp<I, I>.
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <string_view>
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

namespace {

/*
 * Integer-return API identification.
 *
 * libmApisIntegerRet lists the exact YAML api_name values (e.g. "lround",
 * "lroundf") whose mparith oracle returns an integral type.
 *
 * logical_api_base_name strips trailing 'f' (except "erf") to obtain the
 * logical function name used by return_integer().
 */
static std::string_view logical_api_base_name(const std::string &api_name)
{
    if (api_name != "erf" && !api_name.empty() && api_name.back() == 'f')
        return std::string_view(api_name.data(), api_name.size() - 1u);
    return std::string_view(api_name);
}

enum class ReturnTypeInteger { None, Lint, Llong };

static ReturnTypeInteger return_integer(std::string_view logical_base)
{
    if (logical_base == "lround")
        return ReturnTypeInteger::Lint;
    if (logical_base == "llround")
        return ReturnTypeInteger::Llong;
    return ReturnTypeInteger::None;
}

/*
 * read_op:
 * Read the correct union member from InParams::int_op based on type I.
 */
template <typename I, typename T, typename U>
static I read_op(const struct InParams<T, U> *ipp)
{
    if constexpr (std::is_same_v<I, llint_t>)
        return static_cast<I>(ipp->iop.ll);
    else if constexpr (std::is_same_v<I, lint_t>)
        return static_cast<I>(ipp->iop.l);
    else
        return static_cast<I>(ipp->iop.i);
}

/*
 * to_hex_int:
 * Format an integer value as a full-width hex string (sizeof(I) bytes).
 */
template <typename I>
static std::string to_hex_int(I val)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setfill('0')
       << std::setw(sizeof(I) * 2)
       << static_cast<typename std::make_unsigned<I>::type>(val);
    return ss.str();
}

/*
 * unit_test_integer:
 * Executes a single-input unit test for an integer-returning API.
 */
template <typename T, typename U, typename I>
static void unit_test_integer(struct InParams<T, U> *ipp,
                              I (*ref_func)(U),
                              void (*shim_func)(struct InParams<T, U> *),
                              struct YamlOutputs<U> *yop,
                              YamlBatchWriter<U> *writer)
{
    ipp->iop = {};
    yop->exception_raised = run_libm_api_with_exceptions<T, U>(shim_func, ipp);
    U *ip = reinterpret_cast<U *>(&ipp->ip[0]);
    I op = read_op<I>(ipp);

    I mpfrop = ref_func(ip[0]);
    double ulp;
    int uflag = update_ulp<I, I>(op, mpfrop, writer->stats().udata, ulp);

    yop->n[0]          = 1;
    yop->iptr[0]       = ip;
    yop->optr[0]       = nullptr;
    yop->op_hex_str[0] = to_hex_int(op);
    yop->ulp           = &ulp;
    yop->status        = &uflag;

    writer->push(yop);
}

/*
 * range_test_integer:
 * Executes a range sweep for non-VRA integer-returning APIs.
 */
template <typename T, typename U, typename I>
static void range_test_integer(struct InParams<T, U> *ipp,
                               I (*ref_func)(U),
                               void (*shim_func)(struct InParams<T, U> *),
                               struct YamlOutputs<U> *yop,
                               YamlBatchWriter<U> *writer)
{
    uint64_t elem        = sizeof(T) / sizeof(U);
    auto &range          = ipp->range[0];
    uint64_t count       = align_to(range.count, elem);
    uint64_t N           = align_to(count, elem);
    yop->n[0]            = elem;
    double max_ulp[MAX_ELEM] = {0.0};
    int status[MAX_ELEM] = {0};
    yop->ulp             = max_ulp;
    yop->status          = status;
    std::string opstr[16] = {};



    Runner<T, U>   runner(shim_func, yop->config);
    MultiStepGenerator<U> val(range.srt, range.stp, count, range.type, elem);

    FloatPacker<T> fp;
    double ulp;

    for (uint64_t i = 0; i < N; ++i) {
        U *ip = val.wrap_next();
        ipp->ip[0] = fp.pack(ip);

        yop->duration = runner.run(ipp);

        I op_val = read_op<I>(ipp);
        for (uint64_t j = 0; j < elem; ++j) {
            I mpfrop  = ref_func(ip[j]);
            status[j]  = update_ulp<I, I>(op_val, mpfrop, writer->stats().udata, ulp);
            max_ulp[j] = ulp;

            opstr[j] = to_hex_int(op_val);
        }

        yop->iptr[0] = ip;
        yop->optr[0] = nullptr;
        yop->opstr   = opstr;
        writer->push(yop);
    }
}

/*
 * range_test_vra_integer:
 * Executes a VRA range sweep for integer-returning APIs.
 */
template <typename T, typename U, typename I>
static void range_test_vra_integer(struct InParams<T, U> *ipp,
                                   I (*ref_func)(U),
                                   void (*shim_func)(struct InParams<T, U> *),
                                   struct YamlOutputs<U> *yop,
                                   YamlBatchWriter<U> *writer)
{
    uint64_t elem  = sizeof(T) / sizeof(U);
    auto &range    = ipp->range[0];
    uint64_t count = range.count;
    uint64_t N     = align_to(count, elem);
    count =  (count >= 100) ? 100 : count ;
    ipp->count     = count;
    std::vector<double> max_ulp(count);
    std::vector<int> status(count);
    std::vector<std::string> opstr(count);

    yop->n[0]      = count;
    yop->ulp       = max_ulp.data();
    yop->optr[0]   = nullptr;
    yop->status    = status.data();
    yop->opstr     = opstr.data();

    Runner<T, U>   runner(shim_func, yop->config);
    MultiStepGenerator<U> val(range.srt, range.stp, range.count, range.type, count);

    double ulp;

    for (uint64_t i = 0; i < N; ++i) {
        U *ip = val.wrap_next();
        ipp->iptr[0] = ip;

        yop->duration = runner.run(ipp);

        for (uint64_t j = 0; j < count; ++j) {
            I mpfrop  = ref_func(ip[j]);
            I op_val  = read_op<I>(ipp);
            status[j] = update_ulp<I, I>(op_val, mpfrop, writer->stats().udata, ulp);
            max_ulp[j] = ulp;

            opstr[j] = to_hex_int(op_val);
        }

        yop->iptr[0] = ip;
        writer->push(yop);
    }
}

/* Dispatch unit / range / VRA for integer oracle. */
template <typename T, typename U, typename I>
static int run_integer(struct InParams<T, U> *ipp,
                       I (*ref_func)(U),
                       void (*shim_func)(struct InParams<T, U> *),
                       struct YamlOutputs<U> *yop,
                       YamlBatchWriter<U> *writer)
{
    if (ipp->range.empty()) {
        unit_test_integer<T, U, I>(ipp, ref_func, shim_func, yop, writer);
    } else if (!yop->config.is_vra) {
        range_test_integer<T, U, I>(ipp, ref_func, shim_func, yop, writer);
    } else {
        range_test_vra_integer<T, U, I>(ipp, ref_func, shim_func, yop, writer);
    }
    return 0;
}

} /* namespace */

/*
 * api_prototype_07:
 * Entry point for APIs with one floating-point input and one integer output
 * (e.g., lround, llround). Determines the integer return type from the
 * api_name, loads shim and reference functions, and dispatches to the
 * appropriate test mode.
 */
template <typename T, typename U>
int api_prototype_07(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop,
                     YamlBatchWriter<U> *writer)
{
    const ReturnTypeInteger ret_kind =
        return_integer(logical_api_base_name(yop->api_name));
    if (ret_kind == ReturnTypeInteger::None) {
        std::cerr << "api_prototype_07: integer ref " << refapi
                  << " needs a return-type mapping for api " << yop->api_name
                  << " (add return_integer / libmApisIntegerRet)." << std::endl;
        return -1;
    }

    auto shim_func = load_function<void (*)(struct InParams<T, U> *)>(
        alibs->pshimlib, libapi.c_str());

    switch (ret_kind) {
    case ReturnTypeInteger::Lint: {
        using I = typename mpfr::op_type<lint_t>::mopt;
        auto ref_func = load_function<I (*)(U)>(alibs->preflib, refapi);
        return run_integer<T, U, I>(ipp, ref_func, shim_func, yop, writer);
    }
    case ReturnTypeInteger::Llong: {
        using I = typename mpfr::op_type<llint_t>::mopt;
        auto ref_func = load_function<I (*)(U)>(alibs->preflib, refapi);
        return run_integer<T, U, I>(ipp, ref_func, shim_func, yop, writer);
    }
    default:
        return -1;
    }
}

/*
 * Template instantiations:
 * Explicitly instantiate api_prototype_07 for scalar float and double.
 * Integer-returning APIs (lround, llround) are scalar-only.
 */
template int api_prototype_07<float, float>(
    struct AlmLibs *,
    struct InParams<float, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_07<double, double>(
    struct AlmLibs *,
    struct InParams<double, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

template int api_prototype_07<libm::AlignedM128, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_07<libm::AlignedM128d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

template int api_prototype_07<libm::AlignedM256, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_07<libm::AlignedM256d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

#ifdef __AVX512F__
template int api_prototype_07<libm::AlignedM512, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_07<libm::AlignedM512d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);
#endif
