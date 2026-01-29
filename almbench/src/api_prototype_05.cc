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
y[i] = (scalea*a[i]+shifta)/(scaleb*b[i]+shiftb)
Threshold Limitations on Input Parameters
    2^(EMIN/2) ≤ |scalea| ≤ 2^((EMAX-2)/2)
    2^(EMIN/2) ≤ |scaleb| ≤ 2^((EMAX-2)/2)

    |shifta| ≤ 2^(EMAX-2)
    |shiftb| ≤ 2^(EMAX-2)

    2^(EMIN/2) ≤ a[i] ≤ 2^((EMAX-2)/2)
    2^(EMIN/2) ≤ b[i] ≤ 2^((EMAX-2)/2)

    a[i] ≠ - (shifta / scalea) ⋅ (1 - δ1), where |δ1| ≤ 2^(1 - (p - 1)/2)
    b[i] ≠ - (shiftb / scaleb) ⋅ (1 - δ2), where |δ2| ≤ 2^(1 - (p - 1)/2)

for single precision, EMIN = -126,  EMAX = 127,  p = 24
for double precision, EMIN = -1022, EMAX = 1023, p = 53
*/

/*
 * unit_test:
 * Executes a test function on a single input and compares the result
 * with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void unit_test(struct InParams<T, U>* ipp,
                      UL (*ref_func)(U, U, U, U, U, U),
                      void (*shim_func)(struct InParams<T, U> *),
                      struct YamlOutputs<U> *yop)
{
    yop->exception_raised = run_libm_api_with_exceptions<T, U>(shim_func, ipp);

    U* ip = reinterpret_cast<U*>(&ipp->ip[0]);
    U* op = reinterpret_cast<U*>(&ipp->op[0]);

    UL mpfrop = ref_func(ip[0], ip[1], ip[2], ip[3], ip[4], ip[5]);
    double ulp;
    ulp_data udata;
    int uflag = update_ulp(op[0], mpfrop, udata, ulp);

    for (int i = 0; i < 6; ++i)
        yop->iptr[i] = &ip[i];

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
static void range_test(struct InParams<T, U>* ipp,
                       UL (*ref_func)(U, U, U, U, U, U),
                       void (*shim_func)(struct InParams<T, U> *),
                      struct YamlOutputs<U> *yop)
{
    uint64_t elem   = sizeof(T) / sizeof(U);
    auto& r1        = ipp->range[0];
    auto& r2        = ipp->range[1];
    auto& r3        = ipp->range[2];
    auto& r4        = ipp->range[3];
    auto& r5        = ipp->range[4];
    auto& r6        = ipp->range[5];
    uint64_t acount = align_to(r1.count, elem);
    uint64_t N      = align_to(acount, elem);
    double ulp;
    yop->n[0]            = elem;
    yop->n[1]            = elem;
    yop->n[2]            = 1;
    yop->n[3]            = 1;
    yop->n[4]            = 1;
    yop->n[5]            = 1;
    double max_ulp[MAX_ELEM] = {0.0};
    int status[MAX_ELEM] = {0};
    yop->ulp             = max_ulp;
    yop->status          = &status[0];

    Runner<T, U>   runner(shim_func, yop->test_mode);
    MultiStepGenerator<U> g0(r1.srt, r1.stp, r1.count, r1.type, elem);
    MultiStepGenerator<U> g1(r2.srt, r2.stp, r2.count, r2.type, elem);
    MultiStepGenerator<U> g2(r3.srt, r3.stp, r3.count, r3.type, 1);
    MultiStepGenerator<U> g3(r4.srt, r4.stp, r4.count, r4.type, 1);
    MultiStepGenerator<U> g4(r5.srt, r5.stp, r5.count, r5.type, 1);
    MultiStepGenerator<U> g5(r6.srt, r6.stp, r6.count, r6.type, 1);

    FloatPacker<T> fp;
    ulp_data udata;


    U* ipa  = reinterpret_cast<U*>(&ipp->ip[0]);
    U* ipb  = reinterpret_cast<U*>(&ipp->ip[1]);
    U* ipxa = reinterpret_cast<U*>(&ipp->ip[2]);
    U* ipsa = reinterpret_cast<U*>(&ipp->ip[3]);
    U* ipxb = reinterpret_cast<U*>(&ipp->ip[4]);
    U* ipsb = reinterpret_cast<U*>(&ipp->ip[5]);
    U* op   = reinterpret_cast<U*>(&ipp->op[0]);

    for (uint64_t i = 0; i < N; ++i) {
        ipp->ip[0] = fp.pack(g0.wrap_next());
        ipp->ip[1] = fp.pack(g1.wrap_next());
        ipp->ip[2] = T{*g2.wrap_next()};
        ipp->ip[3] = T{*g3.wrap_next()};
        ipp->ip[4] = T{*g4.wrap_next()};
        ipp->ip[5] = T{*g5.wrap_next()};

       yop->duration = runner.run(ipp);

        for (uint64_t j = 0; j < elem; ++j) {
            UL mpfrop = ref_func(ipa[j], ipb[j], ipxa[0], ipsa[0], ipxb[0], ipsb[0]);
            status[j] = update_ulp(op[j], mpfrop, udata, ulp);
            max_ulp[j] = ulp;
        }

        yop->iptr[0] = ipa;
        yop->iptr[1] = ipb;
        yop->iptr[2] = ipxa;
        yop->iptr[3] = ipsa;
        yop->iptr[4] = ipxb;
        yop->iptr[5] = ipsb;
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
static void range_test_vra(struct InParams<T, U>* ipp,
                           UL (*ref_func)(U, U, U, U, U, U),
                           void (*shim_func)(struct InParams<T, U> *),
                           YamlOutputs<U>* yop)
{
    uint64_t elem  = sizeof(T) / sizeof(U);
    auto& r        = ipp->range;
    uint64_t count = r[0].count;
    uint64_t N     = align_to(count, elem);
    count =  (count >= 100) ? 100 : count ;

    std::vector<U> op(count);
    std::vector<double> max_ulp(count);
    std::vector<int> status(count);
    ipp->count    = count;
    ipp->optr[0]  = op.data();
    yop->n[0]     = count;
    yop->n[1]     = count;
    yop->n[2]     = 1;
    yop->n[3]     = 1;
    yop->n[4]     = 1;
    yop->n[5]     = 1;
    yop->optr[0]  = op.data();
    yop->ulp      = max_ulp.data();
    yop->status   = status.data();


    Runner<T, U>   runner(shim_func, yop->test_mode);
    MultiStepGenerator<U> g0(r[0].srt, r[0].stp, r[0].count, r[0].type, count);
    MultiStepGenerator<U> g1(r[1].srt, r[1].stp, r[1].count, r[1].type, count);
    MultiStepGenerator<U> g2(r[2].srt, r[2].stp, r[2].count, r[2].type, 1);
    MultiStepGenerator<U> g3(r[3].srt, r[3].stp, r[3].count, r[3].type, 1);
    MultiStepGenerator<U> g4(r[4].srt, r[4].stp, r[4].count, r[4].type, 1);
    MultiStepGenerator<U> g5(r[5].srt, r[5].stp, r[5].count, r[5].type, 1);

    ulp_data udata;
    double ulp;

    /* Cast input/output buffers */
    U* ipa  = reinterpret_cast<U*>(&ipp->ip[0]);
    U* ipb  = reinterpret_cast<U*>(&ipp->ip[1]);
    U* ipxa = reinterpret_cast<U*>(&ipp->ip[2]);
    U* ipsa = reinterpret_cast<U*>(&ipp->ip[3]);
    U* ipxb = reinterpret_cast<U*>(&ipp->ip[4]);
    U* ipsb = reinterpret_cast<U*>(&ipp->ip[5]);

    for (uint64_t i = 0; i < N; ++i) {
        ipp->iptr[0] = g0.wrap_next();
        ipp->iptr[1] = g1.wrap_next();
        ipp->ip[2] = T{*g2.wrap_next()};
        ipp->ip[3] = T{*g3.wrap_next()};
        ipp->ip[4] = T{*g4.wrap_next()};
        ipp->ip[5] = T{*g5.wrap_next()};

        yop->duration = runner.run(ipp);

        for (uint64_t j = 0; j < count; ++j) {
            UL mpfrop = ref_func(ipa[j], ipb[j], ipxa[0], ipsa[0], ipxb[0], ipsb[0]);
            status[j] = update_ulp(op[j], mpfrop, udata, ulp);
            max_ulp[j] = ulp;
        }

        yop->iptr[0] = ipa;
        yop->iptr[1] = ipb;
        yop->iptr[2] = ipxa;
        yop->iptr[3] = ipsa;
        yop->iptr[4] = ipxb;
        yop->iptr[5] = ipsb;
        write_yaml_output<U>(yop);
    }
}

/*
 * api_prototype_05:
 * main dispatcher for six-input APIs
 * selects unit, range, or vra test mode
 */
template <typename T, typename U>
int api_prototype_05(struct AlmLibs *alibs,
                     struct InParams<T, U>* ipp,
                     const std::string& libapi,
                     const std::string& refapi,
                     YamlOutputs<U>* yop)
{
    using UL = typename mpfr::op_type<U>::mopt;

    auto shim_func = load_function<void (*)(struct InParams<T, U> *)>(alibs->pshimlib, libapi);
    auto ref_func  = load_function<UL (*)(U, U, U, U, U, U)>(alibs->preflib, refapi);

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
 * Explicitly instantiate the api_prototype_05 function for supported
 * scalar and SIMD types with float and double precision.
 */
template int api_prototype_05<float, float>(
    struct AlmLibs *,
    struct InParams<float, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_05<double, double>(
    struct AlmLibs *,
    struct InParams<double, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);

template int api_prototype_05<libm::AlignedM128, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_05<libm::AlignedM128d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);

template int api_prototype_05<libm::AlignedM256, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_05<libm::AlignedM256d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);

#ifdef __AVX512F__
template int api_prototype_05<libm::AlignedM512, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *);

template int api_prototype_05<libm::AlignedM512d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *);
#endif