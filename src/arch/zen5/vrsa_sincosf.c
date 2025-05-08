/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
#define ALM_OVERRIDE 1
#include <libm/arch/zen5.h>
#include <libm_macros.h>
#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>

void ALM_PROTO_ARCH_ZN5(vrsa_sincosf)(int length, float *x, float *sin, float *cos)
{
    int j = 0;
    __m512 opsin, opcos;

    if(likely(length >= FLOAT_ELEMENTS_512_BIT))
    {
        for (j = 0; j <= length - FLOAT_ELEMENTS_512_BIT; j += FLOAT_ELEMENTS_512_BIT)
        {
            __m512 ip16 = _mm512_loadu_ps(&x[j]);
            ALM_PROTO(vrs16_sincosf)(ip16, &opsin, &opcos);
            _mm512_storeu_ps(&sin[j], opsin);
            _mm512_storeu_ps(&cos[j], opcos);
        }
    }
    int remainder = length -j;
    if(remainder)
    {
        __m512 zero = _mm512_set1_ps(0);
        __mmask16 mask =  0xFFFF >> ( 16 - remainder );
        __m512 ip16 = _mm512_mask_loadu_ps(zero, mask, &x[j]);
        ALM_PROTO(vrs16_sincosf)(ip16, &opsin, &opcos);
        _mm512_mask_storeu_ps(&sin[j], mask, opsin);
        _mm512_mask_storeu_ps(&cos[j], mask, opcos);
    }
}
