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
#include <map>
#include <vector>
#include <string>
#include "dll_utils.h"
#include <filesystem>
#include "alm_test.h"
#include "api_template.h"
#include "alm_mp_funcs.h"
#include "ulp.h"

using namespace std;

/*Ensure the output directory exists and set the output file name based on test mode*/
template <typename S>
void check_outfile_dir(struct YamlOutputs<S> *yop) {
    /* * Ensure the output directory exists.
     * If the directory does not exist, create it.
     */
    namespace fs = std::filesystem;

    /* Get current working directory (where the .exe is run) */
    fs::path base_dir = fs::current_path() / "build";
    fs::path dir_path = base_dir / "libm_testsuite_results" / yop->api_name;
    if (!fs::exists(dir_path)) {
        try {
            if (!fs::create_directories(dir_path)) {
                std::cerr << "Failed to create directory: " << dir_path << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }

    /* * Set the output file name based on the test mode and variant.
     * If utflag is true, use "ut" prefix; otherwise, use "acc" or "perf" based on test_mode.
     */
    std::string outfile = "";
    if (yop->utflag) {
        outfile  = yop->vendor + "_conf_" + yop->api_name + "_" + yop->variant;
    } else if (yop->test_mode == TestMode::E_ACCURACY) {
        outfile  = yop->vendor + "_accu_" + yop->api_name + "_" + yop->variant;
    } else if (yop->test_mode == TestMode::E_PERFORMANCE) {
        outfile  = yop->vendor + "_perf_" + yop->api_name + "_" + yop->variant;
    } else {
        outfile  = yop->vendor + "_" + yop->api_name + "_" + yop->variant;
    }

    std::cout << "API Name with Variant : " << outfile << std::endl;

    fs::path full_path = dir_path / (outfile + ".yaml");
    yop->outfile = full_path.lexically_normal().string();
}

template <typename T, typename S>
void header_metadata(const struct InParams<T, S> *ipp,
                    const struct YamlOutputs<S> *yop,
                    struct TestMetadata<S> *metadata)
{
    metadata->library = yop->vendor;
    metadata->api_name = yop->api_name;
    metadata->test_id  = yop->test_id;
    metadata->description = yop->description;
    metadata->variant = yop->variant;
    metadata->test_type = yop->test_type;
    metadata->range = ipp->input_range;
    metadata->threshold = yop->ulp_threshold;
}

/*
 * api_prototype_to_string:
 * Converts an ApiTypes enum value to its corresponding string representation.
 */
string api_prototype_to_string(ApiTypes type)
{
    switch (type) {
    case API_PROTOTYPE_01: return "API_PROTOTYPE_01";
    case API_PROTOTYPE_02: return "API_PROTOTYPE_02";
    case API_PROTOTYPE_03: return "API_PROTOTYPE_03";
    case API_PROTOTYPE_04: return "API_PROTOTYPE_04";
    case API_PROTOTYPE_05: return "API_PROTOTYPE_05";
    case API_PROTOTYPE_06: return "API_PROTOTYPE_06";
    default: return "Unknown";
    }
}

/*
 * libm_funcs:
 * Map of function names to their supported variants.
 * Each function maps to a list of supported data types or vector forms.
 */
static const map<string, vector<string>> api_table_amd = {
    // { function_name, { supported_variants } }
    {"acos",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrsa", "vrda"}},
    {"acosh",     {"sd", "ss"}},
    {"add",       {"vrsa", "vrda"}},
    {"asin",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"asinh",     {"sd", "ss"}},
    {"atan",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"atan2",     {"sd", "ss"}},
    {"atanh",     {"sd", "ss"}},
    {"cbrt",      {"sd", "ss", "vrs4", "vrd2", "vrsa", "vrda"}},
    {"ceil",      {"sd", "ss"}},
    {"cexp",      {"sd", "ss"}},
    {"copysign",  {"sd", "ss"}},
    {"cos",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"cosh",      {"sd", "ss", "vrs4", "vrs8", "vrd2", "vrsa", "vrda"}},
    {"erf",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"erfc",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"exp",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"exp10",     {"sd", "ss", "vrs4", "vrd2", "vrsa", "vrda"}},
    {"exp2",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"expm1",     {"sd", "ss", "vrs4", "vrsa", "vrda"}},
    {"fabs",      {"sd", "ss", "vrs4", "vrs8", "vrd2", "vrd4", "vrsa", "vrda"}},
    {"fdim",      {"sd", "ss"}},
    {"floor",     {"sd", "ss"}},
    {"fmax",      {"sd", "ss", "vrsa", "vrda"}},
    {"fmin",      {"sd", "ss", "vrsa", "vrda"}},
    {"fmod",      {"sd", "ss"}},
    {"hypot",     {"sd", "ss"}},
    {"ldexp",     {"sd", "ss"}},
    {"linearfrac",{"vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"log",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"log1p",     {"sd", "ss", "vrs4", "vrd2", "vrsa", "vrda"}},
    {"log10",     {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrsa", "vrda"}},
    {"log2",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"logb",      {"sd", "ss"}},
    {"mul",       {"vrsa", "vrda"}},
    {"nextafter", {"sd", "ss"}},
    {"nearbyint", {"sd", "ss"}},
    {"pow",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"powx",      {"vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"remainder", {"sd", "ss"}},
    {"rint",      {"sd", "ss"}},
    {"round",     {"sd", "ss"}},
    {"sin",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"sincos",    {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"sinh",      {"sd", "ss"}},
    {"sub",       {"vrsa", "vrda"}},
    {"sqrt",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"tan",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8", "vrsa", "vrda"}},
    {"tanh",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrsa"}},
    {"trunc",     {"sd", "ss"}},
};

static const map<string, vector<string>> api_table_glibc = {

#define GLIBC_VERSION_CHECK(major, minor) (__GLIBC__ > (major) || (__GLIBC__ == (major) && __GLIBC_MINOR__ >= (minor)))

    // { function_name, { supported_variants } }
#if GLIBC_VERSION_CHECK(2,35)
    // All functions with full vector support in 2.42+
    {"acos",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4"}},
    {"acosh",     {"sd", "ss"}},
    {"add",       {}},
    {"asin",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"asinh",     {"sd", "ss"}},
    {"atan",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"atan2",     {"sd", "ss"}},
    {"atanh",     {"sd", "ss"}},
    {"cbrt",      {"sd", "ss", "vrs4", "vrd2"}},
    {"ceil",      {"sd", "ss"}},
    {"cexp",      {"sd", "ss"}},
    {"copysign",  {"sd", "ss"}},
    {"cos",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"cosh",      {"sd", "ss", "vrs4", "vrs8", "vrd2"}},
    {"erf",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"erfc",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp10",     {"sd", "ss", "vrs4", "vrd2"}},
    {"exp2",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"expm1",     {"sd", "ss", "vrs4"}},
    {"fabs",      {"sd", "ss"}},
    {"fdim",      {"sd", "ss"}},
    {"floor",     {"sd", "ss"}},
    {"fmax",      {"sd", "ss"}},
    {"fmin",      {"sd", "ss"}},
    {"fmod",      {"sd", "ss"}},
    {"hypot",     {"sd", "ss"}},
    {"ldexp",     {"sd", "ss"}},
    {"linearfrac",{}},
    {"log",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"log1p",     {"sd", "ss", "vrs4", "vrd2"}},
    {"log10",     {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2"}},
    {"log2",      {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"logb",      {"sd", "ss"}},
    {"mul",       {}},
    {"nextafter", {"sd", "ss"}},
    {"nearbyint", {"sd", "ss"}},
    {"pow",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"powx",      {}},
    {"remainder", {"sd", "ss"}},
    {"rint",      {"sd", "ss"}},
    {"round",     {"sd", "ss"}},
    {"sin",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sincos",    {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sinh",      {"sd", "ss"}},
    {"sqrt",      {"sd", "ss"}},
    {"sub",       {}},
    {"tan",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"tanh",      {"sd", "ss", "vrs4", "vrs8", "vrs16"}},
    {"trunc",     {"sd", "ss"}},

#elif GLIBC_VERSION_CHECK(2,22)
    // Basic vector support functions (available in 2.22+)
    {"acos",      {"sd", "ss"}},
    {"acosh",     {"sd", "ss"}},
    {"add",       {}},
    {"asin",      {"sd", "ss"}},
    {"asinh",     {"sd", "ss"}},
    {"atan",      {"sd", "ss"}},
    {"atan2",     {"sd", "ss"}},
    {"atanh",     {"sd", "ss"}},
    {"cbrt",      {"sd", "ss"}},
    {"ceil",      {"sd", "ss"}},
    {"cexp",      {"sd", "ss"}},
    {"copysign",  {"sd", "ss"}},
    {"cos",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"cosh",      {"sd", "ss"}},
    {"erf",       {"sd", "ss"}},
    {"erfc",      {"sd", "ss"}},
    {"exp",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp2",      {"sd", "ss"}},
    {"exp10",     {"sd", "ss"}},
    {"expm1",     {"sd", "ss"}},
    {"fabs",      {"sd", "ss"}},
    {"fdim",      {"sd", "ss"}},
    {"floor",     {"sd", "ss"}},
    {"fmax",      {"sd", "ss"}},
    {"fmin",      {"sd", "ss"}},
    {"fmod",      {"sd", "ss"}},
    {"hypot",     {"sd", "ss"}},
    {"ldexp",     {"sd", "ss"}},
    {"linearfrac",{}},
    {"log",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"log1p",     {"sd", "ss"}},
    {"log10",     {"sd", "ss"}},
    {"log2",      {"sd", "ss"}},
    {"logb",      {"sd", "ss"}},
    {"mul",       {"sd", "ss"}},
    {"nextafter", {"sd", "ss"}},
    {"nearbyint", {"sd", "ss"}},
    {"pow",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"powx",      {}},
    {"remainder", {"sd", "ss"}},
    {"rint",      {"sd", "ss"}},
    {"round",     {"sd", "ss"}},
    {"sin",       {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sincos",    {"sd", "ss", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sinh",      {"sd", "ss"}},
    {"sqrt",      {"sd", "ss"}},
    {"sub",       {}},
    {"tan",       {"sd", "ss"}},
    {"tanh",      {"sd", "ss"}},
    {"trunc",     {"sd", "ss"}},

#else
    // Minimal scalar-only support for older GLIBC versions
    {"acos",      {"sd", "ss"}},
    {"acosh",     {"sd", "ss"}},
    {"add",       {"sd", "ss"}},
    {"asin",      {"sd", "ss"}},
    {"asinh",     {"sd", "ss"}},
    {"atan",      {"sd", "ss"}},
    {"atan2",     {"sd", "ss"}},
    {"atanh",     {"sd", "ss"}},
    {"cbrt",      {"sd", "ss"}},
    {"ceil",      {"sd", "ss"}},
    {"cexp",      {"sd", "ss"}},
    {"copysign",  {"sd", "ss"}},
    {"cos",       {"sd", "ss"}},
    {"cosh",      {"sd", "ss"}},
    {"erf",       {"sd", "ss"}},
    {"erfc",      {"sd", "ss"}},
    {"exp",       {"sd", "ss"}},
    {"exp2",      {"sd", "ss"}},
    {"exp10",     {"sd", "ss"}},
    {"expm1",     {"sd", "ss"}},
    {"fabs",      {"sd", "ss"}},
    {"fdim",      {"sd", "ss"}},
    {"floor",     {"sd", "ss"}},
    {"fmax",      {"sd", "ss"}},
    {"fmin",      {"sd", "ss"}},
    {"fmod",      {"sd", "ss"}},
    {"hypot",     {"sd", "ss"}},
    {"ldexp",     {"sd", "ss"}},
    {"linearfrac",{}},
    {"log",       {"sd", "ss"}},
    {"log1p",     {"sd", "ss"}},
    {"log2",      {"sd", "ss"}},
    {"log10",     {"sd", "ss"}},
    {"logb",      {"sd", "ss"}},
    {"mul",       {"sd", "ss"}},
    {"nextafter", {"sd", "ss"}},
    {"nearbyint", {"sd", "ss"}},
    {"pow",       {"sd", "ss"}},
    {"powx",      {}},
    {"remainder", {"sd", "ss"}},
    {"rint",      {"sd", "ss"}},
    {"round",     {"sd", "ss"}},
    {"sin",       {"sd", "ss"}},
    {"sincos",    {"sd", "ss"}},
    {"sinh",      {"sd", "ss"}},
    {"sqrt",      {"sd", "ss"}},
    {"sub",       {"sd", "ss"}},
    {"tan",       {"sd", "ss"}},
    {"tanh",      {"sd", "ss"}},
    {"trunc",     {"sd", "ss"}},

#endif
};

/*
 * libm_api_names:
 * Map of API_types to their corresponding API names.
 */
const map<ApiTypes, vector<string>> libm_api_names = {
    {API_PROTOTYPE_01, {"acos", "asin", "atan", "acosh", "asinh", "atanh",
                        "cos", "sin", "tan", "cosh", "sinh", "tanh",
                        "exp", "exp2", "exp10", "expm1", "erfc",
                        "log", "log1p", "log2", "log10",
                        "logb", "cbrt", "ceil", "sqrt", "fabs",
                        "erf", "floor", "nearbyint",
                        "rint", "round", "trunc"}},
    {API_PROTOTYPE_02, {"add", "sub", "mul", "pow", "fmax", "fmin", "fdim", "fmod",
                        "remainder", "atan2", "hypot", "copysign", "nextafter", "ldexp"}},
    {API_PROTOTYPE_03, {"powx"}},
    {API_PROTOTYPE_04, {"sincos"}},
    {API_PROTOTYPE_05, {"linearfrac"}},
    {API_PROTOTYPE_06, {"ldexp"}}
};

/*
 * api_ends_with_f:
 * Removes trailing 'f' from function names, except for "erf".
 */
string api_ends_with_f(const string &api_name)
{
    if (api_name != "erf" && api_name.back() == 'f') {
        return api_name.substr(0, api_name.size() - 1);
    }
    return api_name;
}

/*
 * find_key_for_value:
 * Finds the ApiTypes key for a given function name.
 */
bool find_key_for_value(const map<ApiTypes, vector<string>> &map,
                        const string &value, ApiTypes &res)
{
    for (const auto &pair : map) {
        if (find(pair.second.begin(), pair.second.end(), value) != pair.second.end()) {
            res = pair.first;
            return true;
        }
    }
    return false;
}

/*
 * is_variant_supported:
 * Checks if a variant is supported for a given function.
 */
bool is_variant_supported(const string &func, const string &variant, const string &vendor)
{
    const map<string, vector<string>> &libm_funcs = (vendor == "glibc") ? api_table_glibc : api_table_amd;
    auto it = libm_funcs.find(func);
    if (it != libm_funcs.end()) {
        const auto &variants = it->second;
        return find(variants.begin(), variants.end(), variant) != variants.end();
    }
    return false;
}

/*
 * check_api_type:
 * Determines the API prototype type for a given function name.
 */
ApiTypes check_api_type(const string &api_name, string &variant)
{
    (void)variant;  // Unused parameter
    string func_name = api_ends_with_f(api_name);
    ApiTypes res;
    if (!find_key_for_value(libm_api_names, func_name, res)) {
        cerr << "The api_name " << func_name << " is not found in any api_types" << endl;
        exit(EXIT_FAILURE);
    }
    return res;
}

/*
 * check_api:
 * Validates if a function and variant combination is supported.
 */
bool check_api(const string &api_name, string &variant, const string &vendor)
{
    string func_name = api_ends_with_f(api_name);
    if (!is_variant_supported(func_name, variant, vendor)) {
        cerr << "For " << func_name << ", the variant " << variant << " doesn't exist" << endl;
        return false;
    }
    return true;
}

/*
 * deduce_shimapi:
 * Constructs the shim API symbol name.
 */
string deduce_shimapi(const string &apitype, string &variant)
{
    return "shim_" + apitype + "_" + variant;
}

/*
 * deduce_refapi:
 * Constructs the reference API function name.
 */
string deduce_refapi(const string &apiname, string &variant)
{
    string ref_api = apiname;
    if (apiname == "powx") {
         ref_api = "pow";
    }
    ref_api = "alm_mp_" + ref_api;
    if (variant == "ss" || variant == "vrsa" || variant == "vrs4" ||
        variant == "vrs8" || variant == "vrs16") {
        ref_api += "f";
    }
    return ref_api;
}

/*
 * is_vrary:
 * Checks if the variant is vector array.
 */
bool is_vrarr(string &variant)
{
    return (variant == "vrsa" || variant == "vrda");
}

/*
 * validate_api:
 * Template function to validate and dispatch API execution.
 */
template <typename T, typename U>
int validate_api(struct AlmLibs *alibs,
                 struct InParams<T, U> *ipp,
                 struct YamlOutputs<U> *yop)
{
    ApiTypes api_type = check_api_type(yop->api_name, yop->variant);
    if(!check_api(yop->api_name, yop->variant, yop->vendor)) {
        return -1;
    }

    yop->is_vra = is_vrarr(yop->variant);

    string shimapi = deduce_shimapi(yop->api_name, yop->variant);
    string refapi = deduce_refapi(yop->api_name, yop->variant);

    check_outfile_dir(yop);
    set_global_ulp_threshold(yop->ulp_threshold);

    switch (api_type) {
        case API_PROTOTYPE_01:
            api_prototype_01<T, U>(alibs, ipp, shimapi, refapi, yop);
            break;
        case API_PROTOTYPE_02:
            api_prototype_02<T, U>(alibs, ipp, shimapi, refapi, yop);
            break;
        case API_PROTOTYPE_03:
            api_prototype_03<T, U>(alibs, ipp, shimapi, refapi, yop);
            break;
        case API_PROTOTYPE_04:
            api_prototype_04<T, U>(alibs, ipp, shimapi, refapi, yop);
            break;
        case API_PROTOTYPE_05:
            api_prototype_05<T, U>(alibs, ipp, shimapi, refapi, yop);
            break;
        case API_PROTOTYPE_06:
            api_prototype_06<T, U>(alibs, ipp, shimapi, refapi, yop);
            break;
        default:
            cerr << "Unknown API type." << endl;
            return -1;
    }

    return 0;
}

/* Explicit template instantiations */
template int validate_api<float, float>(struct AlmLibs *, struct InParams<float, float>*, struct YamlOutputs<float>*);
template int validate_api<double, double>(struct AlmLibs *, struct InParams<double, double>*, struct YamlOutputs<double>*);
template int validate_api<libm::AlignedM128, float>(struct AlmLibs *, struct InParams<libm::AlignedM128, float>*, struct YamlOutputs<float>*);
template int validate_api<libm::AlignedM128d, double>(struct AlmLibs *, struct InParams<libm::AlignedM128d, double>*, struct YamlOutputs<double>*);
template int validate_api<libm::AlignedM256, float>(struct AlmLibs *, struct InParams<libm::AlignedM256, float>*, struct YamlOutputs<float>*);
template int validate_api<libm::AlignedM256d, double>(struct AlmLibs *, struct InParams<libm::AlignedM256d, double>*, struct YamlOutputs<double>*);
#ifdef __AVX512F__
template int validate_api<libm::AlignedM512, float>(struct AlmLibs *, struct InParams<libm::AlignedM512, float>*, struct YamlOutputs<float>*);
template int validate_api<libm::AlignedM512d, double>(struct AlmLibs *, struct InParams<libm::AlignedM512d, double>*, struct YamlOutputs<double>*);
#endif