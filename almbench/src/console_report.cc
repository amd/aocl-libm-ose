/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

#include "console_report.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

static bool g_verbose_mode = false;

static constexpr double kPerfTimeNsUnset = std::numeric_limits<double>::max();

static bool is_valid_perf_time_ns(double time_ns)
{
    return time_ns > 0.0 && std::isfinite(time_ns) && time_ns < kPerfTimeNsUnset;
}

void set_verbose_mode(bool enabled)
{
    g_verbose_mode = enabled;
}

bool is_verbose_mode_enabled()
{
    return g_verbose_mode;
}

static std::string variant_to_datatype(const std::string &variant)
{
    static const std::unordered_map<std::string, std::string> kMap = {
        {"ss", "s1s"},   {"sd", "s1d"},
        {"vrs4", "v4s"}, {"vrs8", "v8s"}, {"vrs16", "v16s"},
        {"vrd2", "v2d"}, {"vrd4", "v4d"}, {"vrd8", "v8d"},
        {"vrsa", "vrsa"}, {"vrda", "vrda"},
        {"sc", "sc"},   {"sz", "sz"},
    };
    auto it = kMap.find(variant);
    return (it != kMap.end()) ? it->second : variant;
}

static std::string test_shape_label(const std::string &variant)
{
    if (variant == "vrsa" || variant == "vrda") {
        return "VecArr";
    }
    if (variant.rfind("vr", 0) == 0) {
        return "Vector";
    }
    return "Scalar";
}

static std::string test_category_label(const struct TestConfig &cfg)
{
    if (cfg.utflag) {
        return "Conformance";
    }
    if (cfg.test_mode == TestMode::E_PERFORMANCE) {
        return "Performance";
    }
    return "Accuracy";
}

static std::string library_benchmark_prefix(const std::string &vendor)
{
    if (vendor == "amd") {
        return "AoclLibm";
    }
    if (vendor == "glibc") {
        return "Glibc";
    }
    if (vendor == "mkl") {
        return "Mkl";
    }
    if (vendor.empty()) {
        return "Libm";
    }
    std::string name = vendor;
    name[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(name[0])));
    return name;
}

static std::string format_max_ulp_decimal(double max_ulp)
{
    std::ostringstream oss;
    if (!std::isfinite(max_ulp)) {
        oss << max_ulp;
        return oss.str();
    }
    oss << std::fixed << std::setprecision(6) << max_ulp;
    return oss.str();
}

struct AccuRowKey {
    std::string category;
    std::string api;
    std::string datatype;

    bool operator==(const AccuRowKey &o) const
    {
        return category == o.category && api == o.api && datatype == o.datatype;
    }
};

struct AccuRowKeyHash {
    size_t operator()(const AccuRowKey &k) const
    {
        return std::hash<std::string>()(k.category + k.api + k.datatype);
    }
};

struct AccuRow {
    std::string shape;
    uint64_t total = 0;
    uint64_t pass = 0;
    uint64_t fail = 0;
    double max_ulp_err = 0.0;
};

struct PerfRowKey {
    std::string api;
    std::string datatype;

    bool operator==(const PerfRowKey &o) const
    {
        return api == o.api && datatype == o.datatype;
    }
};

struct PerfRowKeyHash {
    size_t operator()(const PerfRowKey &k) const
    {
        return std::hash<std::string>()(k.api + k.datatype);
    }
};

struct PerfRow {
    std::string library;
    double time_ns = std::numeric_limits<double>::max();
    uint64_t elements_per_call = 0;
    uint64_t total = 0;
    uint64_t pass = 0;
    uint64_t fail = 0;
    double max_ulp_err = 0.0;
};

static std::string format_perf_benchmark_name(const std::string &library,
                                              const std::string &datatype,
                                              const std::string &api)
{
    std::ostringstream bench;
    bench << library << "_" << datatype << "(" << api << ")";
    return bench.str();
}

static std::unordered_map<AccuRowKey, AccuRow, AccuRowKeyHash> g_accu_rows;
static std::unordered_map<PerfRowKey, PerfRow, PerfRowKeyHash> g_perf_rows;

void reset_console_reports()
{
    g_accu_rows.clear();
    g_perf_rows.clear();
}

template <typename U>
void report_accuracy_results(const struct YamlOutputs<U>* yop,
                             const struct ulp_data& udata,
                             uint64_t total_tests,
                             uint64_t fail_count)
{
    if (total_tests == 0) {
        return;
    }

    AccuRowKey key;
    key.category = test_category_label(yop->config);
    key.api = yop->api_name;
    key.datatype = variant_to_datatype(yop->variant);

    AccuRow &row = g_accu_rows[key];
    row.shape = test_shape_label(yop->variant);
    row.total += total_tests;
    row.fail += fail_count;
    row.pass += (total_tests - fail_count);
    if (udata.max_ulp_err > row.max_ulp_err) {
        row.max_ulp_err = udata.max_ulp_err;
    }
}

template <typename U>
void report_perf_results(const struct YamlOutputs<U> *yop,
                         double time_ns,
                         uint64_t elements_per_call,
                         const struct ulp_data &udata,
                         uint64_t total_tests,
                         uint64_t fail_count)
{
    if (elements_per_call == 0) {
        return;
    }

    PerfRowKey key;
    key.api = yop->api_name;
    key.datatype = variant_to_datatype(yop->variant);

    PerfRow &row = g_perf_rows[key];
    row.library = library_benchmark_prefix(yop->vendor);
    row.elements_per_call = elements_per_call;

    if (is_valid_perf_time_ns(time_ns) && time_ns < row.time_ns) {
        row.time_ns = time_ns;
    }

    row.total += total_tests;
    row.fail += fail_count;
    row.pass += (total_tests - fail_count);
    if (udata.max_ulp_err > row.max_ulp_err) {
        row.max_ulp_err = udata.max_ulp_err;
    }
}

static void print_separator()
{
    std::cout << "***************************"
                 "*******************************************************************"
              << std::endl;
}

void print_console_reports()
{
    if (!g_accu_rows.empty()) {
        std::vector<AccuRowKey> conf_keys;
        std::vector<AccuRowKey> accu_keys;
        for (const auto &entry : g_accu_rows) {
            if (entry.first.category == "Conformance") {
                conf_keys.push_back(entry.first);
            } else {
                accu_keys.push_back(entry.first);
            }
        }
        auto sort_keys = [](std::vector<AccuRowKey> &keys) {
            std::sort(keys.begin(), keys.end(),
                      [](const AccuRowKey &a, const AccuRowKey &b) {
                          if (a.api != b.api) return a.api < b.api;
                          return a.datatype < b.datatype;
                      });
        };
        sort_keys(conf_keys);
        sort_keys(accu_keys);

        auto print_accu_section = [&](const char *title,
                                      const std::vector<AccuRowKey> &keys) {
            if (keys.empty()) {
                return;
            }
            std::cout << title << std::endl;
            print_separator();
            std::cout << std::left
                      << std::setw(12) << "TEST"
                      << std::setw(12) << "TYPE"
                      << std::setw(12) << "API"
                      << std::setw(12) << "DATATYPE"
                      << std::setw(12) << "No.Tests"
                      << std::setw(12) << "< ULP"
                      << std::setw(12) << "> ULP"
                      << "MAX ULP ERR"
                      << std::endl;
            print_separator();
            for (const auto &key : keys) {
                const AccuRow &row = g_accu_rows.at(key);
                std::cout << std::left
                          << std::setw(12) << row.shape
                          << std::setw(12) << key.category
                          << std::setw(12) << key.api
                          << std::setw(12) << key.datatype
                          << std::setw(12) << row.total
                          << std::setw(12) << row.pass
                          << std::setw(12) << row.fail
                          << format_max_ulp_decimal(row.max_ulp_err)
                          << std::endl;
            }
            std::cout << std::endl;
        };

        print_accu_section("Conformance ", conf_keys);
        print_accu_section("Accuracy ", accu_keys);
    }

    if (!g_perf_rows.empty()) {
        std::cout << "Performance — Detail (benchmark timing)" << std::endl;
        print_separator();
        std::cout << std::left
                  << std::setw(40) << "Benchmark"
                  << std::setw(10) << "time_ns"
                  << std::setw(10) << "Elements"
                  << std::setw(10) << "MOPS"
                  << std::setw(10) << "No.Tests"
                  << std::setw(10) << "< ULP"
                  << std::setw(10) << "> ULP"
                  << "MAX ULP ERR"
                  << std::endl;
        print_separator();

        std::vector<PerfRowKey> keys;
        keys.reserve(g_perf_rows.size());
        for (const auto &entry : g_perf_rows) {
            keys.push_back(entry.first);
        }
        std::sort(keys.begin(), keys.end(),
                  [](const PerfRowKey &a, const PerfRowKey &b) {
                      if (a.api != b.api) return a.api < b.api;
                      return a.datatype < b.datatype;
                  });

        for (const auto &key : keys) {
            const PerfRow &row = g_perf_rows.at(key);
            if (!is_valid_perf_time_ns(row.time_ns)) {
                continue;
            }
            const double mops =
                row.elements_per_call * 1000.0 / row.time_ns;
            const std::string bench_name =
                format_perf_benchmark_name(row.library, key.datatype, key.api);
            std::cout << std::left
                      << std::setw(40) << bench_name
                      << std::setw(10) << std::fixed << std::setprecision(0)
                      << row.time_ns
                      << std::setw(10) << row.elements_per_call
                      << std::setw(10) << std::fixed << std::setprecision(3) << mops
                      << std::setw(10) << row.total
                      << std::setw(10) << row.pass
                      << std::setw(10) << row.fail
                      << format_max_ulp_decimal(row.max_ulp_err)
                      << std::endl;
        }
        std::cout << std::endl;
    }
}

template void report_accuracy_results<float>(const struct YamlOutputs<float>* yop,
                                             const struct ulp_data& udata,
                                             uint64_t total_tests,
                                             uint64_t fail_count);
template void report_accuracy_results<double>(const struct YamlOutputs<double>* yop,
                                              const struct ulp_data& udata,
                                              uint64_t total_tests,
                                              uint64_t fail_count);
template void report_accuracy_results<fc32_t>(const struct YamlOutputs<fc32_t>* yop,
                                              const struct ulp_data& udata,
                                              uint64_t total_tests,
                                              uint64_t fail_count);
template void report_accuracy_results<fc64_t>(const struct YamlOutputs<fc64_t>* yop,
                                              const struct ulp_data& udata,
                                              uint64_t total_tests,
                                              uint64_t fail_count);

template void report_perf_results<float>(const struct YamlOutputs<float>* yop,
                                         double time_ns,
                                         uint64_t elements_per_call,
                                         const struct ulp_data& udata,
                                         uint64_t total_tests,
                                         uint64_t fail_count);
template void report_perf_results<double>(const struct YamlOutputs<double>* yop,
                                          double time_ns,
                                          uint64_t elements_per_call,
                                          const struct ulp_data& udata,
                                          uint64_t total_tests,
                                          uint64_t fail_count);
template void report_perf_results<fc32_t>(const struct YamlOutputs<fc32_t>* yop,
                                          double time_ns,
                                          uint64_t elements_per_call,
                                          const struct ulp_data& udata,
                                          uint64_t total_tests,
                                          uint64_t fail_count);
template void report_perf_results<fc64_t>(const struct YamlOutputs<fc64_t>* yop,
                                          double time_ns,
                                          uint64_t elements_per_call,
                                          const struct ulp_data& udata,
                                          uint64_t total_tests,
                                          uint64_t fail_count);
