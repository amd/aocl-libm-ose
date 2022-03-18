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

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdlib.h>
#include "func_types.h"
#if defined(_WIN64) || defined(_WIN32)
  #include <Windows.h>
#else
  #include <dlfcn.h>
#endif
#include "string.h"
#include "stdio.h"

/*utlity functions*/
/* used to load data into array vector function inputs */
float  GenerateRangeFloat   (float  min,  float  max);
double GenerateRangeDouble  (double min,  double max);

/* check error */
int CheckError(void);

/*struct to load func specific stuffs */
#if defined(_WIN64) || defined(_WIN32)
struct FuncData {
    /* scalar func types*/
    funcf        s1f   ;
    func         s1d   ;
    funcf_2      s1f_2 ;
    func_2       s1d_2 ;

	 /* complex */
    funcf_cmplx  s1f_cmplx ;
    func_cmplx   s1d_cmplx ;
    funcf_cmplx_2 s1f_cmplx_2 ;
    func_cmplx_2 s1d_cmplx_2 ;

    /* vector func types */
    func_v2d     v2d   ;
    func_v2d_2   v2d_2 ;
    func_v4d     v4d   ;
    func_v4d_2   v4d_2 ;
    funcf_v4s    v4s   ;
    funcf_v4s_2  v4s_2 ;
    funcf_v8s    v8s   ;
    funcf_v8s_2  v8s_2 ;
    funcf_va     vas   ;
    funcf_va_2   vas_2 ;
    func_va      vad   ;
    func_va_2    vad_2 ;
    #if defined(__AVX512__)
    func_v8d     v8d   ;
    func_v8d_2   v8d_2 ;
    funcf_v16s   v16s  ;
    funcf_v16s_2 v16s_2;
    #endif
};
#else
struct FuncData {
    /* scalar func types*/
    funcf        s1f   = NULL;
    func         s1d   = NULL;
    funcf_2      s1f_2 = NULL;
    func_2       s1d_2 = NULL;

    /* complex */
    funcf_cmplx  s1f_cmplx = NULL;
    func_cmplx   s1d_cmplx = NULL;
    funcf_cmplx_2 s1f_cmplx_2 = NULL;
    func_cmplx_2 s1d_cmplx_2 = NULL;

    /* vector func types */
    func_v2d     v2d   = NULL;
    func_v2d_2   v2d_2 = NULL;
    func_v4d     v4d   = NULL;
    func_v4d_2   v4d_2 = NULL;
    funcf_v4s    v4s   = NULL;
    funcf_v4s_2  v4s_2 = NULL;
    funcf_v8s    v8s   = NULL;
    funcf_v8s_2  v8s_2 = NULL;
    funcf_va     vas   = NULL;
    funcf_va_2   vas_2 = NULL;
    func_va      vad   = NULL;
    func_va_2    vad_2 = NULL;
    #if defined(__AVX512__)
    func_v8d     v8d    = NULL;
    func_v8d_2   v8d_2  = NULL;
    funcf_v16s   v16s   = NULL;
    funcf_v16s_2 v16s_2 = NULL;
    #endif
};
#endif

char *concatenate(const char *a, const char *b, const char *c);

int test_func(void* handle, struct FuncData * data, const char * func_name);

#endif
