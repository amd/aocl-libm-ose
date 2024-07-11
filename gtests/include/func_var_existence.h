/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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

using namespace std;

/* map<string, vector<string>> libm_funcs
 * map that contains the math function as key.
 * Its supported variants are stored as values.
 */
static map<string, vector<string>> libm_funcs = {
    // { func,   { supported variants} }
    {"acos",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16"}},
    {"acosh",     {"s1d", "s1f"}},
    {"asin",      {"s1d", "s1f", "vrd8", "vrs4", "vrs8", "vrs16"}},
    {"asinh",     {"s1d", "s1f"}},
    {"atan",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16"}},
    {"atan2",     {"s1d", "s1f"}},
    {"atanh",     {"s1d", "s1f"}},
    {"cbrt",      {"s1d", "s1f", "vrd2", "vrs4", "vrda", "vrsa"}},
    {"ceil",      {"s1d", "s1f"}},
    {"cexp",      {"s1d", "s1f"}},
    {"copysign",  {"s1d", "s1f"}},
    {"cos",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"cosh",      {"s1d", "s1f", "vrd2", "vrs4", "vrs8"}},
    {"erf",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16"}},
    {"exp",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"exp2",      {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"exp10",     {"s1d", "s1f", "vrd2", "vrs4", "vrda", "vrsa"}},
    {"expm1",     {"s1d", "s1f", "vrs4", "vrda", "vrsa"}},
    {"fabs",      {"s1d", "s1f"}},
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
    {"powx",      {"vrd2", "vrd4"}},
    {"remainder", {"s1d", "s1f"}},
    {"rint",      {"s1d", "s1f"}},
    {"round",     {"s1d", "s1f"}},
    {"sin",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16", "vrda", "vrsa"}},
    {"sinh",      {"s1d", "s1f"}},
    {"sqrt",      {"s1d", "s1f", "vrd2", "vrd4"}},
    {"tan",       {"s1d", "s1f", "vrd2", "vrd4", "vrd8", "vrs4", "vrs8", "vrs16"}},
    {"tanh",      {"s1d", "s1f", "vrs4", "vrs8", "vrs16"}},
    {"trunc",     {"s1d", "s1f"}},
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

#endif
