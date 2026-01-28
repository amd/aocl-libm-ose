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

#include <map>
#include <algorithm>
#include "generator.h"
#include "libm_yaml.h"
#include "alm_test.h"


/*
 * ApiTypes:
 * Enumeration of supported API prototypes.
 * Each type corresponds to a different function signature.
 */
enum ApiTypes {
    API_PROTOTYPE_01 = 1,  /* One input, one output */
    API_PROTOTYPE_02,      /* Two inputs, one output */
    API_PROTOTYPE_03,      /* Two inputs (one scalar), one output (e.g., powx) */
    API_PROTOTYPE_04,      /* One input, two outputs (e.g., sincos) */
    API_PROTOTYPE_05,      /* Six inputs, one output (e.g., LinearFrac) */
    API_PROTOTYPE_06       /* Two inputs(one is integer), one output (e.g., ldexp) */
};


void process_libm(struct AlmLibs *alibs, const std::vector<struct YamlInputs> &params, TestMode test_mode );

template <typename T, typename U>
int validate_api(struct AlmLibs *alibs, struct InParams<T, U> *ipp, struct YamlOutputs<U> *yop);




/*
 * API prototype function declarations.
 * Each function validates a specific API signature using the provided libraries.
 */
template <typename T, typename U>
int api_prototype_01(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop);

template <typename T, typename U>
int api_prototype_02(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop);

template <typename T, typename U>
int api_prototype_03(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop);

template <typename T, typename U>
int api_prototype_04(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop);

template <typename T, typename U>
int api_prototype_05(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop);

template <typename T, typename U>
int api_prototype_06(struct AlmLibs *alibs,
                     struct InParams<T, U> *ipp,
                     const std::string &libapi,
                     const std::string &refapi,
                     struct YamlOutputs<U> *yop);