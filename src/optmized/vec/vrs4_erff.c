/*
 * Copyright (C) 2019-2020, Advanced Micro Devices. All rights reserved.
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

/*
 * C implementation of erff single precision 128-bit vector version (v4s)
 *
 * Implementation Notes
 * ----------------------
 *  if x > 3.9192059040069580078125f
 *     return 1.0
 *
 *  else
 *      return x * (0x1.20dd7890d27e1cec99fce48c29cp0 + x * (-0x1.ab4bed70f238422edeeba9c558p-16 + x * (-0x1.80a1bd5878e0b0689c5ff4fcdd4p-2 +
 *             x * (-0x1.07cb4cde6a7d9528c8a732990e4p-8 + x * (0x1.092cba598f96f00ddc5854cf7cp-3 + x * (-0x1.51f0ce4ac87c55f11f685864714p-5 + 
 *             x * (0x1.4101f320bf8bc4d41c228faaa6cp-5 + x * (-0x1.2300882a7d1b712726997de80ep-4 + x * (0x1.d45745fff0e4b6d0604a9ab6284p-5 +
 *             x * (-0x1.9eb1491956e31ded96176d7c8acp-6 + x * (0x1.b9183fc75d326b9044bc63c9694p-8 + x * (-0x1.10e8f8c89ad8645e7d769cd596cp-10 +
 *             x * (0x1.224ffc80cc19957a48ecedad6c8p-14 + x * (0x1.12a30f42c71308321e7e7cb0174p-18 + x * (-0x1.155445e2e006723066d72d22ddcp-20 +
 *             x * 0x1.c6a4181da4ef76f22bd39bb5dcp-25)))))))))))))))
 * 
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>



static const struct {
    v_u32x4_t   bound;
    v_u32x4_t   mask;
    v_u32x4_t   one;
    v_f64x4_t   poly[16];
} v_erff_data = {
              .mask        =  _MM_SET1_I32(0x7FFFFFFF),
              .one         =  _MM_SET1_I32(0x3f800000),
              .bound       =  _MM_SET1_I32(0x407AD447),

             // Polynomial coefficients obtained using Remez algorithm

              .poly = {
                            _MM_SET1_PD4(0x1.20dd7890d27e1cec99fce48c29cp0),
                            _MM_SET1_PD4(-0x1.ab4bed70f238422edeeba9c558p-16),
                            _MM_SET1_PD4(-0x1.80a1bd5878e0b0689c5ff4fcdd4p-2),
                            _MM_SET1_PD4(-0x1.07cb4cde6a7d9528c8a732990e4p-8),
                            _MM_SET1_PD4(0x1.092cba598f96f00ddc5854cf7cp-3),
                            _MM_SET1_PD4(-0x1.51f0ce4ac87c55f11f685864714p-5),
                            _MM_SET1_PD4(0x1.4101f320bf8bc4d41c228faaa6cp-5),
                            _MM_SET1_PD4(-0x1.2300882a7d1b712726997de80ep-4),
                            _MM_SET1_PD4(0x1.d45745fff0e4b6d0604a9ab6284p-5),
                            _MM_SET1_PD4(-0x1.9eb1491956e31ded96176d7c8acp-6),
                            _MM_SET1_PD4(0x1.b9183fc75d326b9044bc63c9694p-8),
                            _MM_SET1_PD4(-0x1.10e8f8c89ad8645e7d769cd596cp-10),
                            _MM_SET1_PD4(0x1.224ffc80cc19957a48ecedad6c8p-14),
                            _MM_SET1_PD4(0x1.12a30f42c71308321e7e7cb0174p-18),
                            _MM_SET1_PD4(-0x1.155445e2e006723066d72d22ddcp-20),
                            _MM_SET1_PD4(0x1.c6a4181da4ef76f22bd39bb5dcp-25),


              },

};

#define MASK      v_erff_data.mask
#define ONE       v_erff_data.one
#define BOUND     v_erff_data.bound

/* Coefficients for 16-degree polynomial */
#define A1  v_erff_data.poly[0]
#define A2  v_erff_data.poly[1]
#define A3  v_erff_data.poly[2]
#define A4  v_erff_data.poly[3]
#define A5  v_erff_data.poly[4]
#define A6  v_erff_data.poly[5]
#define A7  v_erff_data.poly[6]
#define A8  v_erff_data.poly[7]
#define A9  v_erff_data.poly[8]
#define A10  v_erff_data.poly[9]
#define A11  v_erff_data.poly[10]
#define A12  v_erff_data.poly[11]
#define A13  v_erff_data.poly[12]
#define A14  v_erff_data.poly[13]
#define A15  v_erff_data.poly[14]
#define A16  v_erff_data.poly[15]

#define SCALAR_ERFF ALM_PROTO_OPT(erff)

v_f32x4_t
ALM_PROTO_OPT(vrs4_erff)(v_f32x4_t _x) {

    v_f32x4_t result;

    v_u32x4_t uvx = as_v4_u32_f32(_x) & MASK;

    v_u32x4_t sign =  as_v4_u32_f32(_x) & ~MASK;

    v_f32x4_t x = as_v4_f32_u32(uvx);

    v_f64x4_t _yd = _mm256_cvtps_pd(x);

    /* Special implementation of Horner's method for polynomial through the origin*/
    v_f64x4_t result1 = POLY_EVAL_HORNER_16_0(_yd, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16);

    v_f32x4_t result2 = _mm256_cvtpd_ps(result1);

    // Vectorized ternary operator to check for values that should be rounded to 1.0
    v_f32x4_t bound_float = as_v4_f32_u32(BOUND);

    v_f32x4_t mask1 = _mm_cmp_ps(bound_float,x, _CMP_LT_OQ);

    v_f32x4_t fONE = as_v4_f32_u32(ONE);

    result = _mm_blendv_ps(result2,fONE,mask1);

    result = as_v4_f32_u32(sign | as_v4_u32_f32(result));

    return result;

}


