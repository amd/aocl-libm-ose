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
#include <stdexcept>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include "dll_utils.h"

#include "alm_test.h"
#include "api_template.h"
#include "ulp.h"
#include "packer.h"
#include "alm_mp_funcs.h"
#include "api_runner.h"
#include "generator.h"
#include "yaml_batch_writer.h"

/*
 * build_bivariate_generators:
 * Creates generator pair for two-input functions. Exactly one range
 * may be 'derived', in which case the other is the primary generator.
 * Returns {arr1, arr2, first_derived, primary_count}.
 * Throws std::runtime_error if derived func is unknown.
 */
template <typename U>
struct GenPair {
    std::unique_ptr<IGenerator<U>> arr1;
    std::unique_ptr<IGenerator<U>> arr2;
    bool first_derived;
    uint64_t primary_count;
};

/*
 * make_derived_pair:
 * Creates a primary MultiStepGenerator and a DerivedGenerator from it.
 * Throws std::runtime_error if derived func is not found.
 */
template <typename U>
static std::pair<std::unique_ptr<MultiStepGenerator<U>>,
                 std::unique_ptr<DerivedGenerator<U>>>
make_derived_pair(const InpRng<U>& primary_rng, uint64_t primary_count,
                  const DerivedConfig<U>& dcfg, size_t array_size)
{
    auto primary = std::make_unique<MultiStepGenerator<U>>(
        primary_rng.srt, primary_rng.stp, primary_count,
        primary_rng.type, array_size);

    auto &fmap = derived_func_map<U>();
    auto it = fmap.find(dcfg.func);
    if (it == fmap.end())
        throw std::runtime_error("Unknown derived func: " + dcfg.func);

    auto derived = std::make_unique<DerivedGenerator<U>>(
        *primary, dcfg.z_srt, dcfg.z_stp,
        dcfg.z_count, dcfg.z_type, array_size, it->second);

    return {std::move(primary), std::move(derived)};
}

template <typename U>
static GenPair<U> build_bivariate_generators(
    const InpRng<U>& x, const InpRng<U>& y,
    uint64_t xcount, uint64_t ycount, size_t array_size)
{
    GenPair<U> gp;
    gp.first_derived = (x.type == RangeType::E_Derived);

    if constexpr (!std::is_same_v<U, fc32_t> && !std::is_same_v<U, fc64_t>) {
        if (gp.first_derived) {
            auto [primary, derived] = make_derived_pair(y, ycount, *x.derived, array_size);
            gp.arr2 = std::move(primary);
            gp.arr1 = std::move(derived);
            gp.primary_count = ycount;
        } else if (y.type == RangeType::E_Derived) {
            auto [primary, derived] = make_derived_pair(x, xcount, *y.derived, array_size);
            gp.arr1 = std::move(primary);
            gp.arr2 = std::move(derived);
            gp.primary_count = xcount;
        } else {
            gp.arr1 = std::make_unique<MultiStepGenerator<U>>(
                x.srt, x.stp, xcount, x.type, array_size);
            gp.arr2 = std::make_unique<MultiStepGenerator<U>>(
                y.srt, y.stp, ycount, y.type, array_size);
            gp.primary_count = xcount;
        }
    } else {
        gp.arr1 = std::make_unique<MultiStepGenerator<U>>(
            x.srt, x.stp, xcount, x.type, array_size);
        gp.arr2 = std::make_unique<MultiStepGenerator<U>>(
            y.srt, y.stp, ycount, y.type, array_size);
        gp.primary_count = xcount;
    }

    return gp;
}

/*
 * next_pair:
 * Advances both generators in the correct order.
 * Primary must be advanced before derived (see DerivedGenerator contract).
 */
template <typename U>
static std::pair<U*, U*> next_pair(GenPair<U>& gp)
{
    U *ip1, *ip2;
    /* wrap_next for primary must be called before call for derived
     * to respect derived generator contract */
    if (gp.first_derived) {
        ip2 = gp.arr2->wrap_next();
        ip1 = gp.arr1->wrap_next();
    } else {
        ip1 = gp.arr1->wrap_next();
        ip2 = gp.arr2->wrap_next();
    }
    return {ip1, ip2};
}

/*
 * build_bivariate_subpair_mr:
 * Like build_bivariate_generators but for multi_range sub-gens:
 * array_size=1 and counts scaled by lane_width so each scalar sub-gen
 * covers (steps * lane_width / n) values, matching its actual call rate.
 * x and y are taken by value so z_count can be scaled before forwarding.
 */
template <typename U>
static GenPair<U> build_bivariate_subpair_mr(
    InpRng<U> x, InpRng<U> y, size_t lane_width)
{
    const uint64_t xcount = x.count * static_cast<uint64_t>(lane_width);
    const uint64_t ycount = y.count * static_cast<uint64_t>(lane_width);
    if (x.derived) x.derived->z_count *= lane_width;
    if (y.derived) y.derived->z_count *= lane_width;
    return build_bivariate_generators(x, y, xcount, ycount, 1);
}

/*
 * BivariateMRGen:
 * Bivariate counterpart to MultiRangeGenerator for multi_range tests.
 * Holds N GenPair sub-gens (one per ref, padded to lane_width) and cycles
 * through all C(N, lane_width) lane combinations, placing one scalar per
 * lane from the combo-selected sub-gen into the output buffers.
 */
template <typename U>
struct BivariateMRGen {
    struct AlignedDeleter {
        void operator()(U *ptr) const {
            operator delete[](ptr, std::align_val_t(64));
        }
    };

    size_t lane_width;
    std::vector<GenPair<U>> subs;
    std::vector<std::vector<size_t>> combos;
    size_t combo_idx = 0;
    std::unique_ptr<U[], AlignedDeleter> buf1;
    std::unique_ptr<U[], AlignedDeleter> buf2;

    BivariateMRGen(const std::vector<std::vector<InpRng<U>>> &per_ref_ranges,
                   size_t lane_width_)
        : lane_width(lane_width_), combo_idx(0)
    {
        const size_t n_orig = per_ref_ranges.size();
        const size_t n      = std::max(n_orig, lane_width);

        subs.reserve(n);
        for (size_t k = 0; k < n; ++k) {
            const auto &two = per_ref_ranges[k % n_orig];
            if (two.size() != 2)
                throw std::runtime_error(
                    "BivariateMRGen: each ref must have 2 ranges");
            subs.push_back(
                build_bivariate_subpair_mr(two[0], two[1], lane_width));
        }

        combos = enumerate_combinations(n, lane_width);
        if (combos.empty())
            throw std::logic_error(
                "enumerate_combinations returned empty: n=" + std::to_string(n) +
                " lane_width=" + std::to_string(lane_width) +
                " — precondition violated (n >= lane_width must hold)");

        buf1.reset(new (std::align_val_t(64)) U[lane_width]);
        buf2.reset(new (std::align_val_t(64)) U[lane_width]);
    }

    std::pair<U *, U *> next_pair_mr()
    {
        const auto &combo = combos[combo_idx];
        for (size_t k = 0; k < lane_width; ++k) {
            auto [a1, a2] = next_pair(subs[combo[k]]);
            buf1[k] = *a1;
            buf2[k] = *a2;
        }
        combo_idx = (combo_idx + 1) % combos.size();
        return {buf1.get(), buf2.get()};
    }
};

/*
 * unit_test:
 * Executes a test function on a single input and compares the result
 * with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void unit_test(struct InParams<T, U> *ipp,
                      UL (*ref_func)(U, U),
                      void (*shim_func)(struct InParams<T, U> *),
                      struct YamlOutputs<U> *yop,
                      YamlBatchWriter<U> *writer)
{
    yop->exception_raised = run_libm_api_with_exceptions<T, U>(shim_func, ipp);

    U* ip = reinterpret_cast<U*>(&ipp->ip[0]);
    U* op = reinterpret_cast<U*>(&ipp->op[0]);

    UL mpfrop = ref_func(ip[0], ip[1]);
    double ulp;
    int uflag = update_ulp(op[0], mpfrop, writer->stats().udata, ulp);

    yop->n[0]      = 1;
    yop->n[1]      = 1;
    yop->iptr[0]   = ip;
    yop->iptr[1]   = ip + 1;
    yop->optr[0]   = op;
    yop->ulp       = &ulp;
    yop->status    = &uflag;

    writer->push(yop);
}

/*
 * run_bivariate_iters:
 * Shared iteration kernel for bivariate range and multi_range tests.
 * Runs N steps, calling get_pair() each iteration to obtain the next
 * (ip1, ip2) input vector pair, then packs, runs, computes ULP error,
 * and writes output.
 * GetInputPair is any zero-argument callable returning std::pair<U*, U*>.
 */
template <typename T, typename U, typename UL, typename GetInputPair>
static void run_bivariate_iters(uint64_t N, GetInputPair get_pair,
                                struct InParams<T, U> *ipp,
                                UL (*ref_func)(U, U),
                                Runner<T, U> &runner, FloatPacker<T> &fp,
                                uint64_t elem, double *max_ulp, int *status,
                                struct YamlOutputs<U> *yop,
                                YamlBatchWriter<U> *writer)
{
    double ulp;
    for (uint64_t i = 0; i < N; ++i) {
        auto [ip1, ip2] = get_pair();
        ipp->ip[0] = fp.pack(ip1);
        ipp->ip[1] = fp.pack(ip2);

        yop->duration = runner.run(ipp);

        U *op = reinterpret_cast<U *>(&ipp->op[0]);
        for (uint64_t j = 0; j < elem; ++j) {
            UL mpfrop  = ref_func(ip1[j], ip2[j]);
            status[j]  = update_ulp(op[j], mpfrop, writer->stats().udata, ulp);
            max_ulp[j] = ulp;
        }

        yop->iptr[0] = ip1;
        yop->iptr[1] = ip2;
        yop->optr[0] = op;
        writer->push(yop);
    }
}

/*
 * range_test:
 * Executes a test function over a range of inputs for non-VRA APIs.
 * Compares each result with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void range_test(struct InParams<T, U>* ipp,
                       UL (*ref_func)(U, U),
                       void (*shim_func)(struct InParams<T, U> *),
                       struct YamlOutputs<U> *yop,
                       YamlBatchWriter<U> *writer)
{
    uint64_t elem        = sizeof(T) / sizeof(U);
    auto& x              = ipp->range[0];
    auto& y              = ipp->range[1];
    uint64_t xcount      = align_to(x.count, elem);
    uint64_t ycount      = align_to(y.count, elem);
    yop->n[0]            = elem;
    yop->n[1]            = elem;
    double max_ulp[MAX_ELEM] = {0.0};
    int status[MAX_ELEM] = {0};
    yop->ulp             = &max_ulp[0];
    yop->status          = &status[0];

    Runner<T, U>   runner(shim_func, yop->config);

    auto gp = build_bivariate_generators<U>(x, y, xcount, ycount, elem);
    uint64_t N = align_to(gp.primary_count, elem);

    FloatPacker<T> fp;

    run_bivariate_iters(N, [&]{ return next_pair(gp); },
                        ipp, ref_func, runner, fp, elem, max_ulp, status, yop, writer);
}

/*
 * range_test_vra:
 * Executes a test function over a range of inputs for VRA APIs.
 * Compares each result with a reference implementation using ULP error.
 */
template <typename T, typename U, typename UL>
static void range_test_vra(struct InParams<T, U>* ipp,
                           UL (*ref_func)(U, U),
                           void (*shim_func)(struct InParams<T, U> *),
                           struct YamlOutputs<U> *yop,
                           YamlBatchWriter<U> *writer)
{
    uint64_t elem = sizeof(T) / sizeof(U);
    auto& x        = ipp->range[0];
    auto& y        = ipp->range[1];
    uint64_t count = x.count;
    count =  (count >= 100) ? 100 : count ;

    ipp->count     = count;
    std::vector<U>  op(count);
    std::vector<double> max_ulp(count);
    std::vector<int> status(count);

    ipp->optr[0]  = op.data();
    yop->n[0]     = count;
    yop->n[1]     = count;
    yop->optr[0]  = op.data();
    yop->ulp      = max_ulp.data();
    yop->status   = status.data();

    double ulp;

    Runner<T, U>   runner(shim_func, yop->config);

    auto gp = build_bivariate_generators<U>(x, y, x.count, y.count, count);
    uint64_t N = align_to(gp.primary_count, elem);

    for (uint64_t i = 0; i < N; ++i) {
        auto [ip1, ip2] = next_pair(gp);
        ipp->iptr[0] = ip1;
        ipp->iptr[1] = ip2;

        yop->duration = runner.run(ipp);

        for (uint64_t j = 0; j < count; ++j) {
            UL mpfrop = ref_func(ip1[j], ip2[j]);
            status[j] = update_ulp(op[j], mpfrop, writer->stats().udata, ulp);
            max_ulp[j] = ulp;
        }

        yop->iptr[0] = ip1;
        yop->iptr[1] = ip2;
        writer->push(yop);
    }
}

/*
 * range_test_multi:
 * Executes a bivariate test that draws (x, y) pairs from N referenced
 * sub-domains, cycling lane assignments through nCr combinations.
 * Each lane k of both input vectors is taken from the same sub-pair, so
 * the per-lane (x, y) correlation declared by each ref is preserved.
 * Complex types are out of scope for v1.
 */
template <typename T, typename U, typename UL>
static void range_test_multi(struct InParams<T, U>* ipp,
                             UL (*ref_func)(U, U),
                             void (*shim_func)(struct InParams<T, U> *),
                             struct YamlOutputs<U> *yop,
                             YamlBatchWriter<U> *writer)
{
    if constexpr (std::is_same_v<U, fc32_t> || std::is_same_v<U, fc64_t>) {
        (void)ipp; (void)ref_func; (void)shim_func; (void)yop;
        std::cerr << "multi_range not supported for complex APIs in v1" << std::endl;
        return;
    }

    uint64_t elem               = sizeof(T) / sizeof(U);
    double max_ulp[MAX_ELEM]    = {0.0};
    int status[MAX_ELEM]        = {0};
    yop->n[0]                   = elem;
    yop->n[1]                   = elem;
    yop->ulp                    = &max_ulp[0];
    yop->status                 = &status[0];

    Runner<T, U> runner(shim_func, yop->config);

    /* Collect per-ref (x, y) range pairs. */
    std::vector<std::vector<InpRng<U>>> per_ref;
    per_ref.reserve(ipp->multi_range->refs.size());
    for (const auto &entry : ipp->multi_range->refs) {
        if (entry.ranges.size() != 2) {
            std::cerr << "multi_range arity mismatch in api_prototype_02" << std::endl;
            return;
        }
        per_ref.push_back(entry.ranges);
    }

    BivariateMRGen<U> gp(per_ref, static_cast<size_t>(elem));
    uint64_t N = ipp->steps_mr ? ipp->steps_mr : 1000ULL;
    FloatPacker<T> fp;

    run_bivariate_iters(N, [&]{ return gp.next_pair_mr(); },
                        ipp, ref_func, runner, fp, elem, max_ulp, status, yop, writer);
}

/*
 * api_prototype_02:
 * Main dispatcher function that selects the appropriate test mode
 * (unit_test, range, VRA, or multi_range) and executes the test.
 */
template <typename T, typename U>
int api_prototype_02(struct AlmLibs *alibs,
                     struct InParams<T, U>* ipp,
                     const std::string& libapi,
                     const std::string& refapi,
                     YamlOutputs<U>* yop,
                     YamlBatchWriter<U> *writer)
{
    using UL = typename mpfr::op_type<U>::mopt;

    auto shim_func = load_function<void (*)(struct InParams<T, U> *)>(alibs->pshimlib, libapi);
    auto ref_func  = load_function<UL (*)(U, U)>(alibs->preflib, refapi);

    if (ipp->multi_range.has_value()) {
        range_test_multi<T, U, UL>(ipp, ref_func, shim_func, yop, writer);
    } else if (ipp->range.empty()) {
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
 * Explicitly instantiate the api_prototype_02 function for supported
 * scalar and SIMD types with float and double precision.
 */
template int api_prototype_02<float, float>(
    struct AlmLibs *,
    struct InParams<float, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_02<double, double>(
    struct AlmLibs *,
    struct InParams<double, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

template int api_prototype_02<libm::AlignedM128, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_02<libm::AlignedM128d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM128d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

template int api_prototype_02<libm::AlignedM256, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_02<libm::AlignedM256d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM256d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);

#ifdef __AVX512F__
template int api_prototype_02<libm::AlignedM512, float>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512, float> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<float> *,
    YamlBatchWriter<float> *);

template int api_prototype_02<libm::AlignedM512d, double>(
    struct AlmLibs *,
    struct InParams<libm::AlignedM512d, double> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<double> *,
    YamlBatchWriter<double> *);
#endif

template int api_prototype_02<fc32_t, fc32_t>(
    struct AlmLibs *,
    struct InParams<fc32_t, fc32_t> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<fc32_t> *,
    YamlBatchWriter<fc32_t> *);

template int api_prototype_02<fc64_t, fc64_t>(
    struct AlmLibs *,
    struct InParams<fc64_t, fc64_t> *,
    const std::string &,
    const std::string &,
    struct YamlOutputs<fc64_t> *,
    YamlBatchWriter<fc64_t> *);