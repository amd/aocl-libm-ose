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

#define AMD_LIBM_VEC_EXPERIMENTAL 1


#include <external/amdlibm_vec.h>

#define MAKE_GLIBC_VEC_FUNC(name, data_type, amd_name)          \
                                  data_type name(data_type v) { \
                                      return amd_name(v);       \
                                  }

#define MAKE_GLIBC_VEC_FUNC2(name, data_type, amd_name)\
                                   data_type name(data_type v, data_type z) { \
                                       return amd_name(v, z);                 \
                                   }

/* May be moved to specific header file if created */
#define GLIBC_VEC_FUNC_DECL(name, data_type)   data_type name(data_type v)
#define GLIBC_VEC_FUNC2_DECL(name, data_type)  data_type name(data_type v, data_type z)

/* Function Declaration */
GLIBC_VEC_FUNC_DECL(_ZGVbN2v_sin,    __m128d);
GLIBC_VEC_FUNC_DECL(_ZGVbN4v_sinf,   __m128);
GLIBC_VEC_FUNC_DECL(_ZGVbN2v_cos,    __m128d);
GLIBC_VEC_FUNC_DECL(_ZGVbN4v_cosf,   __m128);
GLIBC_VEC_FUNC_DECL(_ZGVbN2v_exp,    __m128d);
GLIBC_VEC_FUNC_DECL(_ZGVbN4v_expf,   __m128);
GLIBC_VEC_FUNC_DECL(_ZGVbN2v_log,    __m128d);
GLIBC_VEC_FUNC_DECL(_ZGVbN4v_logf,   __m128);
GLIBC_VEC_FUNC2_DECL(_ZGVbN2vv_pow,  __m128d);
GLIBC_VEC_FUNC2_DECL(_ZGVbN4vv_powf, __m128);
GLIBC_VEC_FUNC2_DECL(_ZGVdN4vv_pow,  __m256d);
GLIBC_VEC_FUNC2_DECL(_ZGVdN8vv_powf, __m256);

GLIBC_VEC_FUNC_DECL(_ZGVdN8v_sinf, __m256);
GLIBC_VEC_FUNC_DECL(_ZGVdN8v_cosf, __m256);
GLIBC_VEC_FUNC_DECL(_ZGVdN4v_cos,  __m256d);
GLIBC_VEC_FUNC_DECL(_ZGVdN4v_exp,  __m256d);
GLIBC_VEC_FUNC_DECL(_ZGVdN8v_expf, __m256);
GLIBC_VEC_FUNC_DECL(_ZGVdN4v_log,  __m256d);
GLIBC_VEC_FUNC_DECL(_ZGVdN8v_logf, __m256);

GLIBC_VEC_FUNC_DECL(_ZGVdN4v_sin, __m256d);

/* SSE - "_ZGVb" prefix */

MAKE_GLIBC_VEC_FUNC(_ZGVbN2v_sin,  __m128d, amd_vrd2_sin )
MAKE_GLIBC_VEC_FUNC(_ZGVbN4v_sinf, __m128,  amd_vrs4_sinf)

MAKE_GLIBC_VEC_FUNC(_ZGVbN2v_cos,  __m128d, amd_vrd2_cos )
MAKE_GLIBC_VEC_FUNC(_ZGVbN4v_cosf, __m128,  amd_vrs4_cosf)

MAKE_GLIBC_VEC_FUNC(_ZGVbN2v_exp,  __m128d, amd_vrd2_exp )
MAKE_GLIBC_VEC_FUNC(_ZGVbN4v_expf, __m128,  amd_vrs4_expf)

MAKE_GLIBC_VEC_FUNC(_ZGVbN2v_log,  __m128d, amd_vrd2_log )
MAKE_GLIBC_VEC_FUNC(_ZGVbN4v_logf, __m128,  amd_vrs4_logf)

MAKE_GLIBC_VEC_FUNC2(_ZGVbN2vv_pow,  __m128d, amd_vrd2_pow )
MAKE_GLIBC_VEC_FUNC2(_ZGVbN4vv_powf, __m128,  amd_vrs4_powf)

/* AVX  - "_ZGVc" prefix */
MAKE_GLIBC_VEC_FUNC(_ZGVdN4v_sin,  __m256d, amd_vrd4_sin )
MAKE_GLIBC_VEC_FUNC(_ZGVdN8v_sinf, __m256,  amd_vrs8_sinf)

/* AVX2 - "_ZGVd" prefix */
/* not implemented in amd-libm yet */
MAKE_GLIBC_VEC_FUNC(_ZGVdN4v_cos,  __m256d, amd_vrd4_cos )
MAKE_GLIBC_VEC_FUNC(_ZGVdN8v_cosf, __m256,  amd_vrs8_cosf)

MAKE_GLIBC_VEC_FUNC(_ZGVdN4v_exp,  __m256d, amd_vrd4_exp )
MAKE_GLIBC_VEC_FUNC(_ZGVdN8v_expf, __m256,  amd_vrs8_expf)

MAKE_GLIBC_VEC_FUNC(_ZGVdN4v_log,  __m256d, amd_vrd4_log )
MAKE_GLIBC_VEC_FUNC(_ZGVdN8v_logf, __m256,  amd_vrs8_logf)

MAKE_GLIBC_VEC_FUNC2(_ZGVdN4vv_pow,  __m256d, amd_vrd4_pow )
MAKE_GLIBC_VEC_FUNC2(_ZGVdN8vv_powf, __m256,  amd_vrs8_powf)

