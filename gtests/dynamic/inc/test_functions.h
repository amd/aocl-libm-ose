/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#include "utils.h"

int test_s1f    (funcf,   const char*);
int test_s1d    (func,    const char*);
int test_s1f_2  (funcf_2, const char*);
int test_s1d_2  (func_2,  const char*);

/* cmplx scalar */
int test_s1f_cmplx (funcf_cmplx, const char*);
int test_s1d_cmplx (func_cmplx, const char*);
int test_s1f_cmplx_2 (funcf_cmplx_2, const char*);
int test_s1d_cmplx_2 (func_cmplx_2, const char*);

int test_v2d    (func_v2d,     const char*);
int test_v2d_2  (func_v2d_2,   const char*);

int test_v4s    (funcf_v4s,    const char*);
int test_v4s_2  (funcf_v4s_2,  const char*);

int test_v4d    (func_v4d,     const char*);
int test_v4d_2  (func_v4d_2,   const char*);

int test_v8s    (funcf_v8s,    const char*);
int test_v8s_2  (funcf_v8s_2,  const char*);

int test_vas    (funcf_va,     const char*);
int test_vas_2  (funcf_va_2,   const char*);
int test_vad    (func_va,      const char*);
int test_vad_2  (func_va_2,    const char*);
