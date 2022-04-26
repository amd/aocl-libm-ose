/*
 * Copyright (C) 2018-2022, Advanced Micro Devices. All rights reserved.
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

#include <immintrin.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/arch/zen4.h>

/*
 *   __m512 ALM_PROTO_OPT(vrs16_powf)(__m512, __m512);
 *
 * Spec:
 *   - A slightly relaxed version of the scalar powf.
 *   - Maximum ULP is expected to be less than 2.
 *   - Performance is expected to be double of scalar powf
 *
 *
 * Implementation Notes:
 *
 * Call vrs8_powf twice to compute powf() for 8 elements in each iteration
 * 
 * Refer vrs8_powf() for details of algorithm.
 *
 */

v_f32x8_t vrs8_powf(v_f32x8_t x, v_f32x8_t y);

v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_powf)(v_f32x16_t x,v_f32x16_t y) {

     v_f32x16_t ret;

     v_f32x8_t result[2], _x[2], _y[2];


     _y[0] = (v_f32x8_t){ y[0], y[1], y[2], y[3],
                          y[4], y[5], y[6], y[7] };


     _y[1] = (v_f32x8_t){ y[8], y[9], y[10], y[11],
                          y[12], y[13], y[14], y[15] };

     _x[0] = (v_f32x8_t){ x[0], x[1], x[2], x[3],
                          x[4], x[5], x[6], x[7] };


     _x[1] = (v_f32x8_t){ x[8], x[9], x[10], x[11],
                          x[12], x[13], x[14], x[15] };


     for(int32_t i=0; i < 2; i++) {

         result[i] =  vrs8_powf(_x[i], _y[i]);

     }

     ret =  _mm512_setr_ps(result[0][0], result[0][1], result[0][2], result[0][3],
                           result[0][4], result[0][5], result[0][6], result[0][7],
                           result[1][0], result[1][1], result[1][2], result[1][3],
                           result[1][4], result[1][5], result[1][6], result[1][7]);

    return ret;

}


