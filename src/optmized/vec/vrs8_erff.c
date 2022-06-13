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
 * C implementation of erff single precision 256-bit vector version (v8s)
 *
 * Implementation Notes
 * ----------------------
 *  if x > 3.9192059040069580078125f
 *     return 1.0
 *
 *  if 0.974708974361419677734375f < x <= 3.9192059040069580078125f
 *      return -9.334529419817705e-2 + x * (1.770500458970012 + x * (-1.9873639831178735 + x * (3.272308933339834 +
 *              x * (-4.402751805448494 + x * (3.7608972096338276 + x * (-2.0966214980804354 +
 *              x * (0.7930106628290651 + x * (-0.20671156774709162 + x * (3.677264344574519e-2 +
 *              x * (-4.2806518649629016e-3 + x * (2.94780430932192e-4 + x * (-9.12466047071725e-6))))))))))));
 *
 *
 *  if 5.40909968549385666847229003906E-5f x < 0.974708974361419677734375f
 *      return -1.7016133165416813e-11f + x * (1.1283791722401213f + x * (-2.574859935324604e-7f + x * (-0.3761213392501202f +
 *               x * (-5.1212385080044055e-5f + x * (0.11314475379382989f + x * (-1.1672466078260007e-3f + x * (-2.393903580433729e-2f +
 *               x * (-4.896886935591104e-3f +  x * (1.0606170926673056e-2f + x * (-3.656783296404107e-3f + x * 4.0345739971510983e-4f))))))
 *
 *  if 6.60290488951886800350621342659E-9f < x < 5.40909968549385666847229003906E-5f
 *       return -1.860188690104156e-15f + x * (1.1283791677145343f + x * (-3.0517569546146566e-5f));
 *
 *  if x <= 6.60290488951886800350621342659E-9f
 *       return  x + 0x1.06eba8p-3f * x;
 *
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
    v_f32x8_t   tblsz_byln2;
    v_f32x8_t   ln2_tbl_head, ln2_tbl_tail;
    v_f32x8_t   huge;
    v_u32x8_t   all_true, bound1, bound2, bound3, bound4;
    v_i32x8_t   mask;
    v_u32x8_t   one;
    v_f32x8_t   poly_1;
    v_f32x8_t   poly_2[3];
    v_f32x8_t   poly_3[12];
    v_f64x4_t   poly_4[13];
} v_erff_data = {
              .tblsz_byln2 =  _MM256_SET1_PS8(0x1.71547652b82fep+0f),
              .ln2_tbl_head = _MM256_SET1_PS8(0x1.63p-1),
              .ln2_tbl_tail = _MM256_SET1_PS8(-0x1.bd0104p-13),
              .huge        =  _MM256_SET1_PS8(0x1.8p+23) ,
              .bound1     =   _MM256_SET1_I32(0x31E2DFC4),
              .bound2      =  _MM256_SET1_I32(0x3862DFC4),
              .bound3      =  _MM256_SET1_I32(0x3F798687),
              .bound4      =  _MM256_SET1_I32(0x407AD445),
              .all_true    =  _MM256_SET1_I32(0xffffffff),
              .mask        =  _MM256_SET1_I32(0x7FFFFFFF),
              .one         =  _MM256_SET1_I32(0x3f800000),

             // Polynomial coefficients obtained using Remez algorithm
              .poly_1   =     _MM256_SET1_PS8(0x1.06eba8p-3f),


              .poly_2 = {
                              _MM256_SET1_PS8(-1.860188690104156e-15f),
                              _MM256_SET1_PS8(1.1283791677145343f),
                              _MM256_SET1_PS8(-3.0517569546146566e-5f),
              },

              .poly_3 = {
                              _MM256_SET1_PS8(-1.7016133165416813e-11f),
                              _MM256_SET1_PS8(1.1283791722401213f),
                              _MM256_SET1_PS8(-2.574859935324604e-7f),
                              _MM256_SET1_PS8(-0.3761213392501202f),
                              _MM256_SET1_PS8(-5.1212385080044055e-5f),
                              _MM256_SET1_PS8(0.11314475379382989f),
                              _MM256_SET1_PS8(-1.1672466078260007e-3f),
                              _MM256_SET1_PS8(-2.393903580433729e-2f),
                              _MM256_SET1_PS8(-4.896886935591104e-3f),
                              _MM256_SET1_PS8(1.0606170926673056e-2f),
                              _MM256_SET1_PS8(-3.656783296404107e-3f),
                              _MM256_SET1_PS8(4.0345739971510983e-4f),


              },

              .poly_4 = {
                              _MM_SET1_PD4(-9.334529419817705e-2),
                              _MM_SET1_PD4(1.770500458970012),
                              _MM_SET1_PD4(-1.9873639831178735),
                              _MM_SET1_PD4(3.272308933339834),
                              _MM_SET1_PD4(-4.402751805448494),
                              _MM_SET1_PD4(3.7608972096338276),
                              _MM_SET1_PD4(-2.0966214980804354),
                              _MM_SET1_PD4(0.7930106628290651),
                              _MM_SET1_PD4(-0.20671156774709162),
                              _MM_SET1_PD4(3.677264344574519e-2),
                              _MM_SET1_PD4(-4.2806518649629016e-3),
                              _MM_SET1_PD4(2.94780430932192e-4),
                              _MM_SET1_PD4(-9.12466047071725e-6),


              },


};

#define TBL_LN2   v_erff_data.tblsz_byln2
#define LN2_TBL_H v_erff_data.ln2_tbl_head
#define LN2_TBL_T v_erff_data.ln2_tbl_tail
#define EXPF_BIAS v_erff_data.expf_bias
#define EXP_HUGE  v_erff_data.huge
#define BOUND1    v_erff_data.bound1
#define BOUND2    v_erff_data.bound2
#define BOUND3    v_erff_data.bound3
#define BOUND4    v_erff_data.bound4
#define ALL_TRUE  v_erff_data.all_true
#define MASK      v_erff_data.mask
#define ONE       v_erff_data.one

#define A1 v_erff_data.poly_1

/* Coefficients for 3-degree polynomial */
#define B1 v_erff_data.poly_2[0]
#define B2 v_erff_data.poly_2[1]
#define B3 v_erff_data.poly_2[2]

/* Coefficients for 11-degree polynomial */
#define C1  v_erff_data.poly_3[0]
#define C2  v_erff_data.poly_3[1]
#define C3  v_erff_data.poly_3[2]
#define C4  v_erff_data.poly_3[3]
#define C5  v_erff_data.poly_3[4]
#define C6  v_erff_data.poly_3[5]
#define C7  v_erff_data.poly_3[6]
#define C8  v_erff_data.poly_3[7]
#define C9  v_erff_data.poly_3[8]
#define C10 v_erff_data.poly_3[9]
#define C11 v_erff_data.poly_3[10]
#define C12 v_erff_data.poly_3[11]

/* Coefficients for 12-degree polynomial */
#define D1  v_erff_data.poly_4[0]
#define D2  v_erff_data.poly_4[1]
#define D3  v_erff_data.poly_4[2]
#define D4  v_erff_data.poly_4[3]
#define D5  v_erff_data.poly_4[4]
#define D6  v_erff_data.poly_4[5]
#define D7  v_erff_data.poly_4[6]
#define D8  v_erff_data.poly_4[7]
#define D9  v_erff_data.poly_4[8]
#define D10 v_erff_data.poly_4[9]
#define D11 v_erff_data.poly_4[10]
#define D12 v_erff_data.poly_4[11]
#define D13 v_erff_data.poly_4[12]



#define SCALAR_ERFF ALM_PROTO_OPT(erff)


/*
*/

static inline int test_condition_for_all(v_u32x8_t cond) {

    for(int i =0; i < 8; i++) {

        if(cond[i] != 0xffffffff)
            return 0;

    }

    return 1;
}

v_f32x8_t
ALM_PROTO_OPT(vrs8_erff)(v_f32x8_t _x) {

    v_f32x8_t result;

    v_u32x8_t uvx = as_v8_u32_f32(_x) & MASK;

    v_u32x8_t sign =  as_v8_u32_f32(_x) & ~MASK;

    v_f32x8_t x = as_v8_f32_u32(uvx);

    v_u32x8_t cond1 = uvx <= BOUND1;

    if(test_condition_for_all(cond1)) {

       result = x + A1 * x;

       result = as_v8_f32_u32(sign | as_v8_u32_f32(result));

       return result;

    }

    v_u32x8_t cond2 = uvx <= BOUND2;

    if(test_condition_for_all(cond2)) {

       result  = B1 + x * (B2 + x * (B3));

       result = as_v8_f32_u32(sign | as_v8_u32_f32(result));

       return result;

    }

    v_u32x8_t cond3 = uvx <= BOUND3;

    if(test_condition_for_all(cond3)) {

        result = POLY_EVAL_HORNER_11(x, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);

        result = as_v8_f32_u32(sign | as_v8_u32_f32(result));

        return result;

    }

    v_u32x8_t cond4 = uvx <= BOUND4;

    if(test_condition_for_all(cond4)) {

        v_f32x4_t _y1 = _mm256_extractf128_ps(x, 0);

        v_f32x4_t _y2 = _mm256_extractf128_ps(x, 1);

        v_f64x4_t _y1d = _mm256_cvtps_pd(_y1);

        v_f64x4_t _y2d = _mm256_cvtps_pd(_y2);

        v_f64x4_t result1 = POLY_EVAL_HORNER_12(_y1d, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13);

        v_f64x4_t result2 = POLY_EVAL_HORNER_12(_y2d, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13);

        result =  _mm256_setr_m128(_mm256_cvtpd_ps(result1), _mm256_cvtpd_ps(result2));

        result = as_v8_f32_u32(sign | as_v8_u32_f32(result));

        return result;

    }

    cond4 = uvx > BOUND4;

    if(test_condition_for_all(cond4)) {

        return as_v8_f32_u32(sign | ONE);

    }

    for(uint32_t i = 0; i < 8; i++) {

        result[i] = SCALAR_ERFF(_x[i]);

    }

    return result;

}


