/*
 * Copyright (C) 2019-2022, Advanced Micro Devices. All rights reserved.
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
 *  else:
 *   we split the domain [0,3.91...] into subintervals and choose the polynomial
 *   approximant accordingly
 *   if inputs fall into multiple subintervals use the fallback option: 16th degree
 *   polynomial which covers the whole domain.
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
#define AMD_LIBM_FMA_USABLE 1
#include <libm/poly-vec.h>



static const struct {
    v_f32x4_t   bound;
    v_u32x4_t   mask;
    v_u32x4_t   one;
    uint32_t    u0p8, u2p2, u2p85, u3p35, ubound, u1p6, u1p1;
    v_f64x4_t   poly[16], poly_upper[5], poly_2p85to3p35[5], poly_2p2to2p85[6],
                poly_0p8to1p1[5], poly_1p1to1p6[6],poly_lower[5], poly_1p6to2p2[6];
} v_erff_data = {
              .mask        =  _MM_SET1_I32(0x7FFFFFFF),
              .one         =  _MM_SET1_I32(0x3f800000),
              .u0p8        =  0x3F4CCCCD,
              .u2p2        =  0x400CCCCD,
              .u1p6        =  0x3FCCCCCD,
              .u1p1        =  0x3F8CCCCD,
              .u2p85       =  0x40366666,
              .u3p35       =  0x40566666,
              .ubound      =  0x407AD447,
              .bound       =  _MM_SET1_PS4(0x1.f5a88ep1f),

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

            .poly_lower = {
                            _MM_SET1_PD4(0x1.20dd7279852d20bae6293720f14p0),
                            _MM_SET1_PD4(-0x1.81243af5b5c54b0958e8db62664p-2),
                            _MM_SET1_PD4(0x1.cd9759622bccb1ad8309b733b34p-4),
                            _MM_SET1_PD4(-0x1.adabe1d651ec58b1b05c9f01ffcp-6),
                            _MM_SET1_PD4(0x1.0836f5b874583d1db146de5e3ecp-8),
              },

            .poly_0p8to1p1 = {
                            _MM_SET1_PD4(0x1.1abfcc1af4cd7f215ca6bd8a168p0),
                            _MM_SET1_PD4(0x1.13a805beaf4883ac4e360adfdb8p-3),
                            _MM_SET1_PD4(-0x1.5bcdf1cb5caa6b8ebe28fd3fa24p-1),
                            _MM_SET1_PD4(0x1.574e13b89f4ec23aa52542d3b28p-2),
                            _MM_SET1_PD4(-0x1.acc348ec49e2dcdceee392cab78p-5),
            },

            .poly_1p1to1p6 = {
                            _MM_SET1_PD4(0x1.0f968e94fb7b2279e4eafb4b62cp0),
                            _MM_SET1_PD4(0x1.3fb8db3fc664509c4e2bf4b0658p-2),
                            _MM_SET1_PD4(-0x1.ee5532daa8f8512ef6fd14e59bcp-1),
                            _MM_SET1_PD4(0x1.1fc6442863d1bca9a3b6c03347p-1),
                            _MM_SET1_PD4(-0x1.1ee27db8b7d842bac8a0d5fdc48p-3),
                            _MM_SET1_PD4(0x1.acf02b0a0ea2a8140e6a977ebfcp-7),
            },
 
             .poly_1p6to2p2 = {
                            _MM_SET1_PD4(0x1.22d221489d624aa5523733865dp0),
                            _MM_SET1_PD4(0x1.d7a49c8f0306987727478abb198p-4),
                            _MM_SET1_PD4(-0x1.8757236070ea53394d1d6a52194p-1),
                            _MM_SET1_PD4(0x1.dabf372d8e15bee97cad5ea4f8p-2),
                            _MM_SET1_PD4(-0x1.e1c154247566b869c5488bda2bcp-4),
                            _MM_SET1_PD4(0x1.7244c1e7f7e139524ce49f615acp-7),
             },

            .poly_2p2to2p85 = {
                            _MM_SET1_PD4(0x1.aacd34384ec3af14f882486d01cp0),
                            _MM_SET1_PD4(-0x1.18ac56e96aba8abddfab7dd104cp0),
                            _MM_SET1_PD4(0x1.61e9777b93cf4870cf450ec3fe4p-2),
                            _MM_SET1_PD4(-0x1.808bb5ba94bad0ed37575e552ep-5),
                            _MM_SET1_PD4(0x1.2ebd1f7681bbd73977b55ff653cp-12),
                            _MM_SET1_PD4(0x1.7f01e5c1a867dcd1edded96bd38p-12),
              },

            .poly_2p85to3p35 = {
                            _MM_SET1_PD4(0x1.95ba342ce2475d69aa502f92744p0),
                            _MM_SET1_PD4(-0x1.018e7e574815f4d72fb88a661b4p0),
                            _MM_SET1_PD4(0x1.4758086212787a68d23a74f5dap-2),
                            _MM_SET1_PD4(-0x1.a062b81875e9b8b426eded26c54p-5),
                            _MM_SET1_PD4(0x1.a7fbdc59fad76aec356f4bfe11cp-9),
            },

            .poly_upper = {

                           _MM_SET1_PD4(0x1.60a6516b34c64cb4a9d93a01658p0),
                           _MM_SET1_PD4(-0x1.84637afb18dc950d3e7faf70dbcp-1),
                           _MM_SET1_PD4(0x1.ab79e6bcc5c0ab5509263eaa7p-3),
                           _MM_SET1_PD4(-0x1.d630d604604ed7695a3938a306p-6),
                           _MM_SET1_PD4(0x1.9d76f035a49dec452a1c1f94824p-10),
             },

};

#define MASK      v_erff_data.mask
#define ONE       v_erff_data.one
#define U0P8      v_erff_data.u0p8
#define U2P2      v_erff_data.u2p2
#define U1P1      v_erff_data.u1p1
#define U1P6      v_erff_data.u1p6
#define U2P85     v_erff_data.u2p85
#define U3P35     v_erff_data.u3p35
#define UBOUND    v_erff_data.ubound
#define BOUND     v_erff_data.bound

/* Coefficients for 16-degree polynomial approximating entire domain*/
#define A1   v_erff_data.poly[0]
#define A2   v_erff_data.poly[1]
#define A3   v_erff_data.poly[2]
#define A4   v_erff_data.poly[3]
#define A5   v_erff_data.poly[4]
#define A6   v_erff_data.poly[5]
#define A7   v_erff_data.poly[6]
#define A8   v_erff_data.poly[7]
#define A9   v_erff_data.poly[8]
#define A10  v_erff_data.poly[9]
#define A11  v_erff_data.poly[10]
#define A12  v_erff_data.poly[11]
#define A13  v_erff_data.poly[12]
#define A14  v_erff_data.poly[13]
#define A15  v_erff_data.poly[14]
#define A16  v_erff_data.poly[15]

/* Coefficients for polynomials approximating the various subintervals */
#define B1   v_erff_data.poly_upper[0]
#define B2   v_erff_data.poly_upper[1]
#define B3   v_erff_data.poly_upper[2]
#define B4   v_erff_data.poly_upper[3]
#define B5   v_erff_data.poly_upper[4]

#define C1   v_erff_data.poly_2p85to3p35[0]
#define C2   v_erff_data.poly_2p85to3p35[1]
#define C3   v_erff_data.poly_2p85to3p35[2]
#define C4   v_erff_data.poly_2p85to3p35[3]
#define C5   v_erff_data.poly_2p85to3p35[4]

#define D1   v_erff_data.poly_2p2to2p85[0]
#define D2   v_erff_data.poly_2p2to2p85[1]
#define D3   v_erff_data.poly_2p2to2p85[2]
#define D4   v_erff_data.poly_2p2to2p85[3]
#define D5   v_erff_data.poly_2p2to2p85[4]
#define D6   v_erff_data.poly_2p2to2p85[5]

#define E1  v_erff_data.poly_1p6to2p2[0]
#define E2  v_erff_data.poly_1p6to2p2[1]
#define E3  v_erff_data.poly_1p6to2p2[2]
#define E4  v_erff_data.poly_1p6to2p2[3]
#define E5  v_erff_data.poly_1p6to2p2[4]
#define E6  v_erff_data.poly_1p6to2p2[5]

#define F1   v_erff_data.poly_1p1to1p6[0]
#define F2   v_erff_data.poly_1p1to1p6[1]
#define F3   v_erff_data.poly_1p1to1p6[2]
#define F4   v_erff_data.poly_1p1to1p6[3]
#define F5   v_erff_data.poly_1p1to1p6[4]
#define F6   v_erff_data.poly_1p1to1p6[5]

#define G1   v_erff_data.poly_0p8to1p1[0]
#define G2   v_erff_data.poly_0p8to1p1[1]
#define G3   v_erff_data.poly_0p8to1p1[2]
#define G4   v_erff_data.poly_0p8to1p1[3]
#define G5   v_erff_data.poly_0p8to1p1[4]

#define H1   v_erff_data.poly_lower[0]
#define H2   v_erff_data.poly_lower[1]
#define H3   v_erff_data.poly_lower[2]
#define H4   v_erff_data.poly_lower[3]
#define H5   v_erff_data.poly_lower[4]

#define SCALAR_ERFF ALM_PROTO_OPT(erff)

v_f32x4_t
ALM_PROTO_OPT(vrs4_erff)(v_f32x4_t _x) {

    v_u32x4_t uvx = as_v4_u32_f32(_x) & MASK;

    v_f32x4_t x = as_v4_f32_u32(uvx);

    /* x now contains only positive values */

    /* vectorized code to find max values

    It's actually quicker to use non-vectorized code here, so commented out for now

    v_f32x4_t x2 = _mm_permute_ps(x, _MM_SHUFFLE(1, 0, 3, 2));
    v_f32x4_t x3 = _mm_max_ps(x, x2);
    v_f32x4_t x4 = _mm_permute_ps(x3, _MM_SHUFFLE(2, 3, 0, 1));
    v_f32x4_t x5 = _mm_max_ps(x3, x4);

    uint32_t uxmax = asuint32(x5[0]);

    */

    uint32_t uxmax = (uvx[0] > uvx[1]) ? uvx[0] : uvx[1];

    uxmax = (uvx[2] > uxmax) ? uvx[2] : uxmax;
    uxmax = (uvx[3] > uxmax) ? uvx[3] : uxmax;

    v_f32x4_t result;
    v_f64x4_t _yd = _mm256_cvtps_pd(x);

    if (uxmax <= U0P8) {

        _yd = _yd * _yd;
        _yd = POLY_EVAL_HORNER_5(_yd, H1, H2, H3, H4, H5);
        
        result = _x * _mm256_cvtpd_ps(_yd);
        return result;
    }
    
    /* Need to find min too now */
    uint32_t uxmin = (uvx[0] > uvx[1]) ? uvx[1] : uvx[0];

    uxmin = (uvx[2] < uxmin) ? uvx[2] : uxmin;
    uxmin = (uvx[3] < uxmin) ? uvx[3] : uxmin;

    v_u32x4_t sign;

    if (uxmin > U0P8 && uxmax <= U1P1) {

        _yd = POLY_EVAL_HORNER_5(_yd, G1, G2, G3, G4, G5);

        result = _x * _mm256_cvtpd_ps(_yd);

        return result;

    } else if (uxmin > U1P1 && uxmax <= U1P6) {

        _yd = POLY_EVAL_HORNER_6(_yd, F1, F2, F3, F4, F5, F6);

        result = _x * _mm256_cvtpd_ps(_yd);

        return result;

    } else if (uxmin > U1P6 && uxmax <= U2P2) {

        sign =  as_v4_u32_f32(_x) & ~MASK;

        _yd = POLY_EVAL_HORNER_6_0(_yd, E1, E2, E3, E4, E5, E6);

        result = _mm256_cvtpd_ps(_yd);

        result = as_v4_f32_u32(sign | as_v4_u32_f32(result));

        return result;

    } else if (uxmin > U2P2 && uxmax <= U2P85) {

        _yd = POLY_EVAL_HORNER_6(_yd, D1, D2, D3, D4, D5, D6);

        result = _x * _mm256_cvtpd_ps(_yd);

        return result;

    } else if (uxmin > U2P85 && uxmax <= U3P35) {

        _yd = POLY_EVAL_HORNER_5(_yd, C1, C2, C3, C4, C5);

        result = _x * _mm256_cvtpd_ps(_yd);

        return result;

    } else if (uxmin > U3P35){

        sign =  as_v4_u32_f32(_x) & ~MASK;

        _yd = POLY_EVAL_HORNER_5_0(_yd, B1, B2, B3, B4, B5);

        result = _mm256_cvtpd_ps(_yd);

        result = as_v4_f32_u32(sign | as_v4_u32_f32(result));

    } else {

        sign =  as_v4_u32_f32(_x) & ~MASK;

        _yd = POLY_EVAL_HORNER_16_0(_yd, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16);

        result = _mm256_cvtpd_ps(_yd);

        result = as_v4_f32_u32(sign | as_v4_u32_f32(result));

    }

    if (uxmax > UBOUND) {

        /* Vectorized ternary operator to check for values that should be rounded to 1.0 */

        v_f32x4_t mask1 = _mm_cmp_ps(BOUND, x, _CMP_LT_OQ);

        v_f32x4_t fONE = as_v4_f32_u32(ONE);

        result = _mm_blendv_ps(result, fONE, mask1);

        result = as_v4_f32_u32(sign | as_v4_u32_f32(result));
    }

    return result;

}


