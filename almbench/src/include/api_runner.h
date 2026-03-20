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

#pragma once

#include <string>
#include <iostream>
#include "alm_test.h"
#include "api_template.h"
#include "ulp.h"
#include "packer.h"
#include "alm_mp_funcs.h"
#include "hrt_utils.h"

/*
 * load_function:
 * Template function to load a symbol from a shared library.
 * Returns a function pointer of the requested type.
 * Exits the program if the symbol is not found.
 */
template <typename FuncType>
FuncType load_function(DL_HANDLE lib, const std::string &name);

/*
*/
template <typename T, typename U>
int run_libm_api_with_exceptions(void (*shim_func)(InParams<T, U>*), InParams<T, U>* ipp);

template <typename T, typename U>
double run_libm_api(void (*shim_func)(InParams<T, U>*), InParams<T, U>* ipp);

template <typename T, typename U>
class Runner {
public:
    using RunnerFunc = double (Runner::*)(InParams<T, U>*);

    Runner(void (*shim)(InParams<T, U>*), TestMode mode, uint64_t iterations = 10);
    double run(InParams<T, U>* ipp);

private:
    void (*shim_func)(InParams<T, U>*);
    uint64_t iterations;
    RunnerFunc run_libm_api;

    double run_perf(InParams<T, U>* ipp);
    double run_accu(InParams<T, U>* ipp);
};
