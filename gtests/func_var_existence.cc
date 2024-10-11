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

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include "func_var_existence.h"

#include <iostream>

using namespace std;

inline vector<string> getSupportedVariants(string func)
{
    vector<string> vars;
    if(libm_funcs.find(func) != libm_funcs.end())
        vars = libm_funcs[func];
    return vars;
}

inline bool isVariantSupported(string func, string variant)
{
    if(libm_funcs.find(func) != libm_funcs.end())
    {
        vector<string> vars = libm_funcs[func];
        if(find(vars.begin(), vars.end(), variant) != vars.end())
            return true;
    }
    return false;
}

string validateFilterData(string func, string filter_data)
{
    vector<string> supported_vars = getSupportedVariants(func);
    stringstream ss(filter_data);
    string new_filter_data, temp;

    while(getline(ss, temp, ':'))
    {
        if( (temp.find("SCALAR_FLOAT") != string::npos) || (temp.find("SCALAR_COMPLEX_FLOAT") != string::npos) )
        {
            if(find(supported_vars.begin(), supported_vars.end(), "s1f") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if( (temp.find("SCALAR_DOUBLE") != string::npos) || (temp.find("SCALAR_COMPLEX_DOUBLE") != string::npos) )
        {
            if(find(supported_vars.begin(), supported_vars.end(), "s1d") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_2DOUBLES") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrd2") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_4DOUBLES") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrd4") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_8DOUBLES") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrd8") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_4FLOATS") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrs4") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_8FLOATS") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrs8") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_16FLOATS") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrs16") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_ARRAY_DOUBLE") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrda") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
        else if(temp.find("VECTOR_ARRAY_FLOAT") != string::npos)
        {
            if(find(supported_vars.begin(), supported_vars.end(), "vrsa") != supported_vars.end())
                new_filter_data.append(temp + ":");
        }
    }
    return new_filter_data;
}