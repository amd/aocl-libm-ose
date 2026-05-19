/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

static const struct {
    v_f64x8_t half;
    v_f64x8_t one;
    v_f64x8_t sign;
} v_rnd8_data  = {
    .half = _MM512_SET1_PD8(0.5),
    .one  = _MM512_SET1_PD8(1.0),
    .sign = _MM512_SET1_PD8(-0.0),
};
#define HALF8_DP64      v_rnd8_data.half
#define SIGN8_DP64      v_rnd8_data.sign
#define ONE8_DP64       v_rnd8_data.one

v_f64x8_t
ALM_PROTO_OPT(vrd8_round)(v_f64x8_t x)
{
    v_f64x8_t sign_bits = _mm512_and_pd(x, SIGN8_DP64);
    v_f64x8_t abs_x = _mm512_andnot_pd(SIGN8_DP64, x);
    v_f64x8_t rounded_even = _mm512_roundscale_pd(abs_x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    v_f64x8_t remainder = _mm512_sub_pd(abs_x, rounded_even);
    __mmask8 is_tie = _mm512_cmp_pd_mask(remainder, HALF8_DP64, _CMP_EQ_OQ);
    v_f64x8_t adjustment = _mm512_maskz_mov_pd(is_tie, ONE8_DP64);
    v_f64x8_t result_abs = _mm512_add_pd(rounded_even, adjustment);
    v_f64x8_t result = _mm512_or_pd(result_abs, sign_bits);
    return result;
}
