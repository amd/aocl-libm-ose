/*
 * Copyright (C) 2024-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __FUNC_VAR_EXISTENCE_H__
#define __FUNC_VAR_EXISTENCE_H__

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "almstruct.h"
#include "libm_tests.h"

using namespace std;

/* map<string, vector<string>> libm_funcs
 * map that contains the math function as key.
 * Its supported variants are stored as values.
 */
static map<string, vector<string>> libm_funcs = {
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
    /*
     * GLIBC: supported variants per glibc version (aligned with almbench).
     */
#if GLIBC_VERSION_CHECK(2, 35)
    {"acos",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"acosh",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"add",       {}},
    {"asin",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"asinh",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"atan",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"atan2",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"atanh",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"cbrt",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"ceil",      {"s1d", "s1f"}},
    {"cexp",      {"s1d", "s1f"}},
    // {"clog",      {"s1d", "s1f"}},   // disabled for now (GLIBC tables only)
    {"copysign",  {"s1d", "s1f"}},
    {"cos",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"cosh",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"erf",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"erfc",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp10",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp2",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"expm1",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"fabs",      {"s1d", "s1f"}},
    {"fdim",      {"s1d", "s1f"}},
    {"floor",     {"s1d", "s1f"}},
    {"fmax",      {"s1d", "s1f"}},
    {"fmin",      {"s1d", "s1f"}},
    {"fmod",      {"s1d", "s1f"}},
    {"hypot",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"ldexp",     {"s1d", "s1f"}},
    {"linearfrac",{}},
    {"log",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"log1p",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"log10",     {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"log2",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"logb",      {"s1d", "s1f"}},
    {"mul",       {}},
    {"nearbyint", {"s1d", "s1f"}},
    {"nextafter", {"s1d", "s1f"}},
    {"pow",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"powx",      {}},
    {"remainder", {"s1d", "s1f"}},
    {"rint",      {"s1d", "s1f"}},
    {"round",     {"s1d", "s1f"}},
    {"sin",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sincos",    {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sinh",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sqrt",      {"s1d", "s1f"}},
    {"sub",       {}},
    {"tan",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"tanh",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"trunc",     {"s1d", "s1f"}}
#elif GLIBC_VERSION_CHECK(2, 22)
    {"acos",      {"s1d", "s1f"}},
    {"acosh",     {"s1d", "s1f"}},
    {"add",       {}},
    {"asin",      {"s1d", "s1f"}},
    {"asinh",     {"s1d", "s1f"}},
    {"atan",      {"s1d", "s1f"}},
    {"atan2",     {"s1d", "s1f"}},
    {"atanh",     {"s1d", "s1f"}},
    {"cbrt",      {"s1d", "s1f"}},
    {"ceil",      {"s1d", "s1f"}},
    {"cexp",      {"s1d", "s1f"}},
    // {"clog",      {"s1d", "s1f"}},  // disabled for now (GLIBC tables only)
    {"copysign",  {"s1d", "s1f"}},
    {"cos",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"cosh",      {"s1d", "s1f"}},
    {"erf",       {"s1d", "s1f"}},
    {"erfc",      {"s1d", "s1f"}},
    {"exp",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"exp10",     {"s1d", "s1f"}},
    {"exp2",      {"s1d", "s1f"}},
    {"expm1",     {"s1d", "s1f"}},
    {"fabs",      {"s1d", "s1f"}},
    {"fdim",      {"s1d", "s1f"}},
    {"floor",     {"s1d", "s1f"}},
    {"fmax",      {"s1d", "s1f"}},
    {"fmin",      {"s1d", "s1f"}},
    {"fmod",      {"s1d", "s1f"}},
    {"hypot",     {"s1d", "s1f"}},
    {"ldexp",     {"s1d", "s1f"}},
    {"linearfrac",{}},
    {"log",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"log1p",     {"s1d", "s1f"}},
    {"log10",     {"s1d", "s1f"}},
    {"log2",      {"s1d", "s1f"}},
    {"logb",      {"s1d", "s1f"}},
    {"mul",       {}},
    {"nearbyint", {"s1d", "s1f"}},
    {"nextafter", {"s1d", "s1f"}},
    {"pow",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"powx",      {}},
    {"remainder", {"s1d", "s1f"}},
    {"rint",      {"s1d", "s1f"}},
    {"round",     {"s1d", "s1f"}},
    {"sin",       {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sincos",    {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrd2", "vrd4", "vrd8"}},
    {"sinh",      {"s1d", "s1f"}},
    {"sqrt",      {"s1d", "s1f"}},
    {"sub",       {}},
    {"tan",       {"s1d", "s1f"}},
    {"tanh",      {"s1d", "s1f"}},
    {"trunc",     {"s1d", "s1f"}}
#else
    {"acos",      {"s1d", "s1f"}},
    {"acosh",     {"s1d", "s1f"}},
    {"add",       {}},
    {"asin",      {"s1d", "s1f"}},
    {"asinh",     {"s1d", "s1f"}},
    {"atan",      {"s1d", "s1f"}},
    {"atan2",     {"s1d", "s1f"}},
    {"atanh",     {"s1d", "s1f"}},
    {"cbrt",      {"s1d", "s1f"}},
    {"ceil",      {"s1d", "s1f"}},
    {"cexp",      {"s1d", "s1f"}},
    // {"clog",      {"s1d", "s1f"}}, // disabled for now (GLIBC tables only)
    {"copysign",  {"s1d", "s1f"}},
    {"cos",       {"s1d", "s1f"}},
    {"cosh",      {"s1d", "s1f"}},
    {"erf",       {"s1d", "s1f"}},
    {"erfc",      {"s1d", "s1f"}},
    {"exp",       {"s1d", "s1f"}},
    {"exp10",     {"s1d", "s1f"}},
    {"exp2",      {"s1d", "s1f"}},
    {"expm1",     {"s1d", "s1f"}},
    {"fabs",      {"s1d", "s1f"}},
    {"fdim",      {"s1d", "s1f"}},
    {"floor",     {"s1d", "s1f"}},
    {"fmax",      {"s1d", "s1f"}},
    {"fmin",      {"s1d", "s1f"}},
    {"fmod",      {"s1d", "s1f"}},
    {"hypot",     {"s1d", "s1f"}},
    {"ldexp",     {"s1d", "s1f"}},
    {"linearfrac",{}},
    {"log",       {"s1d", "s1f"}},
    {"log1p",     {"s1d", "s1f"}},
    {"log10",     {"s1d", "s1f"}},
    {"log2",      {"s1d", "s1f"}},
    {"logb",      {"s1d", "s1f"}},
    {"mul",       {}},
    {"nearbyint", {"s1d", "s1f"}},
    {"nextafter", {"s1d", "s1f"}},
    {"pow",       {"s1d", "s1f"}},
    {"powx",      {}},
    {"remainder", {"s1d", "s1f"}},
    {"rint",      {"s1d", "s1f"}},
    {"round",     {"s1d", "s1f"}},
    {"sin",       {"s1d", "s1f"}},
    {"sincos",    {"s1d", "s1f"}},
    {"sinh",      {"s1d", "s1f"}},
    {"sqrt",      {"s1d", "s1f"}},
    {"sub",       {}},
    {"tan",       {"s1d", "s1f"}},
    {"tanh",      {"s1d", "s1f"}},
    {"trunc",     {"s1d", "s1f"}}
#endif
#else
    // { func,   { supported variants} }
    {"acos",      {"s1d", "s1f", "vrd2", "vrd4", "vrs4", "vrs8", "vrs16", "vrsa", "vrda"}},
    {"acosh",     {"s1d", "s1f"}},
    {"asin",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrsa", "vrda"}},
    {"asinh",     {"s1d", "s1f"}},
    {"atan",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrsa", "vrda"}},
    {"atan2",     {"s1d", "s1f", "vrd2", "vrd4", "vrd8"}},
    {"atanh",     {"s1d", "s1f"}},
    {"cbrt",      {"s1d", "s1f", "vrd2", "vrs4", "vrs8", "vrda", "vrsa"}},
    {"ceil",      {"s1d", "s1f"}},
    {"cexp",      {"s1d", "s1f"}},
    {"clog",      {"s1d", "s1f"}},
    {"copysign",  {"s1d", "s1f"}},
    {"cos",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"cosh",      {"s1d", "s1f", "vrd2", "vrs4", "vrs8", "vrsa", "vrda"}},
    {"erf",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrsa", "vrda"}},
    {"erfinv",    {"s1d", "vrd2", "vrd4", "vrd8", "vrda", /*"s1f", "vrs4", "vrs8", "vrs16", "vrsa"*/}},
    {"exp",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"exp2",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"exp10",     {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"expm1",     {"s1d", "s1f", "vrs4", "vrda", "vrsa"}},
    {"fabs",      {"s1d", "s1f", "vrd2", "vrd4", "vrs4", "vrs8", "vrda", "vrsa"}},
    {"fdim",      {"s1d", "s1f"}},
    {"floor",     {"s1d", "s1f"}},
    {"fmax",      {"s1d", "s1f", "vrda", "vrsa"}},
    {"fmin",      {"s1d", "s1f", "vrda", "vrsa"}},
    {"fmod",      {"s1d", "s1f"}},
    {"hypot",     {"s1d", "s1f"}},
    {"log",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"log1p",     {"s1d", "s1f", "vrd2", "vrs4", "vrda", "vrsa"}},
    {"log2",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"log10",     {"s1d", "s1f", "vrd2", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"logb",      {"s1d", "s1f"}},
    {"nearbyint", {"s1d", "s1f"}},
    {"pow",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"powx",      {"vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrsa", "vrda"}},
    {"remainder", {"s1d", "s1f"}},
    {"rint",      {"s1d", "s1f"}},
    {"round",     {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"sincos",    {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"sin",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"sinh",      {"s1d", "s1f"}},
    {"sqrt",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"tan",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrsa", "vrda"}},
    {"tanh",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16", "vrsa"}},
    {"trunc",     {"s1d", "s1f"}},
    {"linearfrac",{"vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"add",       {"vrda", "vrsa"}},
    {"sub",       {"vrda", "vrsa"}},
    {"mul",       {"vrda", "vrsa"}},
    {"nextafter", {"s1d", "s1f"}},
    {"ldexp",     {"s1d", "s1f"}},
    {"erfc",      {"s1d", "vrd2", "vrd4", "vrd8", "vrda", "s1f", "vrs4", "vrs8", "vrs16", "vrsa"}},
    {"cdfnorm",   {"s1d", "vrd2", "vrd4", "vrd8", "vrda"}},
    {"cdfnorminv",{"s1d", "vrd2", "vrd4", "vrd8", "vrda"}},
    {"erfcinv",   {"s1d", "vrd2", "vrd4", "vrd8", "vrda"}},
#endif
};

/* vector<string> getSupportedVariants(string func)
 * returns the list of variants supported by that math function
 */
vector<string> getSupportedVariants(string func);

/* bool isVariantSupported(string func, string variant)
 * returns boolean true if given variant is supported by that math function.
 * returns boolean false otherwise.
 */
bool isVariantSupported(string func, string variant);

/* string validateFilterData(string func, string filter_data)
 * return the valid filter-data to be applied for a given math function.
 * This ensures that when GTest FW executes RUN_ALL_TESTS() macro,
 * only the supported variants of that function is running.
 */
string validateFilterData(string func, string filter_data);

/* string validateInplaceFilterData(string func, string filter_data)
 * return the valid filter-data to be applied for a given math function
 * for IN-PLACE tests.
 */
string validateInplaceFilterData(string func, string filter_data);

/* string validateFilterData(InputParams *params, string filter_data)
 * return the valid filter-data to be applied for a given math function.
 */
string validateFilterDataConf(InputParams *params, string filter_data);

#endif
