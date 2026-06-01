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
#include <ctime>
#include <filesystem>
#include <fstream>
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
    double mops = 0.0;
    uint64_t total = 0;
    uint64_t fail = 0;
    double max_ulp_err = 0.0;
};

static void update_perf_row_mops(PerfRow &row)
{
    if (is_valid_perf_time_ns(row.time_ns)) {
        row.mops = static_cast<double>(row.elements_per_call) * 1000.0 / row.time_ns;
    } else {
        row.mops = 0.0;
    }
}

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
    update_perf_row_mops(row);

    row.total += total_tests;
    row.fail += fail_count;
    if (udata.max_ulp_err > row.max_ulp_err) {
        row.max_ulp_err = udata.max_ulp_err;
    }
}

namespace {

constexpr const char kHtmlReportsDir[] = "build/libm_testsuite_reports";
constexpr const char kYamlResultsDir[] = "build/libm_testsuite_results";
constexpr const char kHtmlDefaultFile[] = "almbench_default_console.html";
constexpr const char kHtmlVerboseModeFile[] = "almbench_verbose_mode.html";

struct ReportView {
    std::vector<AccuRowKey> conf_keys;
    std::vector<AccuRowKey> accu_keys;
    std::vector<PerfRowKey> perf_keys;
};

bool compare_accu_key(const AccuRowKey &a, const AccuRowKey &b)
{
    if (a.api != b.api) {
        return a.api < b.api;
    }
    return a.datatype < b.datatype;
}

bool compare_perf_key(const PerfRowKey &a, const PerfRowKey &b)
{
    if (a.api != b.api) {
        return a.api < b.api;
    }
    return a.datatype < b.datatype;
}

void sort_accu_keys(std::vector<AccuRowKey> &keys)
{
    std::sort(keys.begin(), keys.end(), compare_accu_key);
}

ReportView build_report_view()
{
    ReportView view;
    for (const auto &entry : g_accu_rows) {
        if (entry.first.category == "Conformance") {
            view.conf_keys.push_back(entry.first);
        } else {
            view.accu_keys.push_back(entry.first);
        }
    }
    sort_accu_keys(view.conf_keys);
    sort_accu_keys(view.accu_keys);

    view.perf_keys.reserve(g_perf_rows.size());
    for (const auto &entry : g_perf_rows) {
        view.perf_keys.push_back(entry.first);
    }
    std::sort(view.perf_keys.begin(), view.perf_keys.end(), compare_perf_key);
    return view;
}

bool report_view_empty(const ReportView &view)
{
    return view.conf_keys.empty() && view.accu_keys.empty() && view.perf_keys.empty();
}

std::string html_escape(const std::string &value)
{
    std::string out;
    out.reserve(value.size());
    for (char c : value) {
        switch (c) {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += "&quot;"; break;
        default: out += c; break;
        }
    }
    return out;
}

std::filesystem::path reports_directory()
{
    return std::filesystem::current_path() / kHtmlReportsDir;
}

const char *report_html_filename()
{
    return is_verbose_mode_enabled() ? kHtmlVerboseModeFile : kHtmlDefaultFile;
}

const char *report_mode_label()
{
    return is_verbose_mode_enabled() ? "--verbose-mode" : "default (console)";
}

std::string timestamp_string()
{
    std::time_t now = std::time(nullptr);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &now);
#else
    localtime_r(&now, &local_tm);
#endif
    char buffer[32] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);
    return buffer;
}

void append_separator(std::ostream &out)
{
    out << "***************************"
           "*******************************************************************"
        << std::endl;
}

void append_accu_section(std::ostream &out,
                         const char *title,
                         const std::vector<AccuRowKey> &keys)
{
    if (keys.empty()) {
        return;
    }
    out << title << std::endl;
    append_separator(out);
    out << std::left
        << std::setw(12) << "TEST"
        << std::setw(12) << "TYPE"
        << std::setw(12) << "API"
        << std::setw(12) << "DATATYPE"
        << std::setw(12) << "No.Tests"
        << std::setw(18) << "> ULP threshold"
        << "MAX ULP ERR"
        << std::endl;
    append_separator(out);
    for (const auto &key : keys) {
        const AccuRow &row = g_accu_rows.at(key);
        out << std::left
            << std::setw(12) << row.shape
            << std::setw(12) << key.category
            << std::setw(12) << key.api
            << std::setw(12) << key.datatype
            << std::setw(12) << row.total
            << std::setw(18) << row.fail
            << format_max_ulp_decimal(row.max_ulp_err)
            << std::endl;
    }
    out << std::endl;
}

void append_perf_section(std::ostream &out, const ReportView &view)
{
    if (view.perf_keys.empty()) {
        return;
    }

    out << "Performance — Detail (benchmark timing)" << std::endl;
    append_separator(out);
    out << std::left
        << std::setw(40) << "Benchmark"
        << std::setw(10) << "time_ns"
        << std::setw(10) << "Elements"
        << std::setw(10) << "MOPS"
        << std::setw(10) << "No.Tests"
        << std::setw(18) << "> ULP threshold"
        << "MAX ULP ERR"
        << std::endl;
    append_separator(out);

    for (const auto &key : view.perf_keys) {
        const PerfRow &row = g_perf_rows.at(key);
        if (!is_valid_perf_time_ns(row.time_ns)) {
            continue;
        }
        const std::string bench_name =
            format_perf_benchmark_name(row.library, key.datatype, key.api);
        out << std::left
            << std::setw(40) << bench_name
            << std::setw(10) << std::fixed << std::setprecision(0) << row.time_ns
            << std::setw(10) << row.elements_per_call
            << std::setw(10) << std::fixed << std::setprecision(3) << row.mops
            << std::setw(10) << row.total
            << std::setw(18) << row.fail
            << format_max_ulp_decimal(row.max_ulp_err)
            << std::endl;
    }
    out << std::endl;
}

std::string format_console_report(const ReportView &view)
{
    std::ostringstream out;
    if (!view.conf_keys.empty() || !view.accu_keys.empty()) {
        append_accu_section(out, "Conformance ", view.conf_keys);
        append_accu_section(out, "Accuracy ", view.accu_keys);
    }
    append_perf_section(out, view);
    return out.str();
}

void append_accu_html_table(std::ostringstream &html,
                            const char *section_title,
                            const std::vector<AccuRowKey> &keys)
{
    if (keys.empty()) {
        return;
    }

    html << "<section><h2>" << html_escape(section_title) << "</h2><table>"
         << "<thead><tr>"
         << "<th>TEST</th><th>TYPE</th><th>API</th><th>DATATYPE</th>"
         << "<th>No.Tests</th><th>&gt; ULP threshold</th><th>MAX ULP ERR</th>"
         << "</tr></thead><tbody>\n";

    for (const auto &key : keys) {
        const AccuRow &row = g_accu_rows.at(key);
        const char *row_class = row.fail > 0 ? " class=\"fail-row\"" : "";
        html << "<tr" << row_class << ">"
             << "<td>" << html_escape(row.shape) << "</td>"
             << "<td>" << html_escape(key.category) << "</td>"
             << "<td>" << html_escape(key.api) << "</td>"
             << "<td>" << html_escape(key.datatype) << "</td>"
             << "<td>" << row.total << "</td>"
             << "<td>" << row.fail << "</td>"
             << "<td>" << html_escape(format_max_ulp_decimal(row.max_ulp_err)) << "</td>"
             << "</tr>\n";
    }
    html << "</tbody></table></section>\n";
}

void append_perf_html_table(std::ostringstream &html, const ReportView &view)
{
    if (view.perf_keys.empty()) {
        return;
    }

    html << "<section><h2>Performance — Detail (benchmark timing)</h2><table>"
         << "<thead><tr>"
         << "<th>Benchmark</th><th>time_ns</th><th>Elements</th><th>MOPS</th>"
         << "<th>No.Tests</th><th>&gt; ULP threshold</th><th>MAX ULP ERR</th>"
         << "</tr></thead><tbody>\n";

    for (const auto &key : view.perf_keys) {
        const PerfRow &row = g_perf_rows.at(key);
        if (!is_valid_perf_time_ns(row.time_ns)) {
            continue;
        }
        const std::string bench_name =
            format_perf_benchmark_name(row.library, key.datatype, key.api);
        const char *row_class = row.fail > 0 ? " class=\"fail-row\"" : "";
        html << "<tr" << row_class << ">"
             << "<td>" << html_escape(bench_name) << "</td>"
             << "<td>" << std::fixed << std::setprecision(0) << row.time_ns << "</td>"
             << "<td>" << row.elements_per_call << "</td>"
             << "<td>" << std::fixed << std::setprecision(3) << row.mops << "</td>"
             << "<td>" << row.total << "</td>"
             << "<td>" << row.fail << "</td>"
             << "<td>" << html_escape(format_max_ulp_decimal(row.max_ulp_err)) << "</td>"
             << "</tr>\n";
    }
    html << "</tbody></table></section>\n";
}

void write_html_report(const ReportView &view)
{
    const std::filesystem::path report_dir = reports_directory();
    std::error_code ec;
    std::filesystem::create_directories(report_dir, ec);
    if (ec) {
        std::cerr << "Warning: failed to create HTML report directory "
                  << report_dir << ": " << ec.message() << std::endl;
        return;
    }

    const std::filesystem::path html_path = report_dir / report_html_filename();
    std::ofstream out(html_path);
    if (!out) {
        std::cerr << "Warning: failed to write HTML report: " << html_path << std::endl;
        return;
    }

    std::ostringstream body;
    append_accu_html_table(body, "Conformance", view.conf_keys);
    append_accu_html_table(body, "Accuracy", view.accu_keys);
    append_perf_html_table(body, view);

    out << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
        << "<meta charset=\"UTF-8\">\n"
        << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << "<title>LibM Testsuite Report</title>\n"
        << "<style>\n"
        << "body{font-family:'Segoe UI',Tahoma,sans-serif;margin:1.5rem;}\n"
        << "table{border-collapse:collapse;width:100%;max-width:72rem;margin-bottom:1.5rem;}\n"
        << "th,td{border:1px solid #ccc;padding:0.35rem 0.5rem;text-align:left;}\n"
        << "th{background:#f4f4f4;}\n"
        << "tr.fail-row td{background:#fdecea;}\n"
        << "section{margin-bottom:1.5rem;}\n"
        << "</style>\n</head>\n<body>\n"
        << "<h1>LibM Testsuite Report</h1>"
        << "<p>Mode: " << html_escape(report_mode_label()) << "</p>"
        << "<p>Generated: " << html_escape(timestamp_string()) << "</p>";
    if (is_verbose_mode_enabled()) {
        out << "<p>YAML dumps: "
            << html_escape(
                   (std::filesystem::current_path() / kYamlResultsDir)
                       .lexically_normal()
                       .string())
            << "</p>";
    }
    out << body.str() << "</body>\n</html>\n";

    std::cout << "HTML summary report: " << html_path.lexically_normal() << std::endl;
}

}  // namespace

void print_console_reports()
{
    if (g_accu_rows.empty() && g_perf_rows.empty()) {
        return;
    }

    const ReportView view = build_report_view();
    if (report_view_empty(view)) {
        return;
    }

    std::cout << format_console_report(view);
    write_html_report(view);
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
