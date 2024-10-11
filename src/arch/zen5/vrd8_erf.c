/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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
 * C implementation of erf double precision 512-bit vector version (v8d)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_erf(v_f64x8_t x)
 *
 * Maximum ULP is 2.03271
 *
 * Implementation Notes
 * ----------------------
 *  if x > 5.921875
 *     return 1.0
 *
 *  if 1.0 < x <= 5.921875
 *      return 1 - exp(x * (-1.128379198989930192746772277672040367511127479667880683087214917312973705421 + x * (-0.6366193111195503928974474361758894666404675632145002162894331610917819652684 + x * (-0.10277547084408357532105868282211330916733358058054124524473286591478952967105 + x * (1.913904214189711886762071087079941066171685447503716970303298596385024529787e-2 + x * (1.825913620408778114794169081827109689163333138218777377173097737573780074451e-4 + x * (-1.647411432107627860506561538441809818128299435687100607632403662627229969442e-3 + x * (5.21818424825711363023814805627743102562544825002695874303608394240491926905e-4 + x * (4.91627168817438044269386766419565454318509950036800085804021703321697091835e-5 + x * (-1.302981172729131079630523589391656092208513771111088247857933578594392780292e-4 + x * (7.615199404557680638356591201054196713693837373671412154758563642454062526076e-5 + x * (-2.96935339283727595127063299180979185095022523878782406229763314957081808351e-5 + x * (8.878165181834831585186432336594595410842906530722031490846761209704716619195e-6 + x * (-2.12714951505431619504049300115915574833256638114737995020961934817058311951e-6 + x * (4.146746535162880252608396592166861942608845052332004241597308855885179990494e-7 + x * (-6.59030545681603281811648088253177498666005293866364687961783648988232273307e-8 + x * (8.482541531526446204466824249004710358281109743716038706482471097755571216e-9 + x * (-8.717025659536669540755909448858779028619932413055195403437445698307186886677e-10 + x * (6.986310311662613657315676676388719057504624524859113603391352651484705086844e-11 + x * (-4.207783872043116961017859246460942240015393112860254295006478500309621030333e-12 + x * (1.790995692530692947437965495563199880874085822185316938795832479020883353374e-13 + x * (-4.801396646154358189127560929835283812276105659218143392174762905750106085853e-15 + x * 6.09464019738248521658138120195945654655592494197989524075299306234484609923e-17)))))))))))))))))))))
 *
 *  if x <= 1.0
 *       return x * (1.1283791670955125656539546480394381212700000251759 + x^2 * (-0.37612638903183519538684678924796441989200004437331 + x^2 * (0.112837916709441847162115702407296623359855842865354 + x^2 * (-2.68661706431114670328213173933416573498428131606314e-2 + x^2 * (5.2239776061184733153931752295100839160294359393454e-3 + x^2 * (-8.548325929314490260657717454060788378873328143637e-4 + x^2 * (1.2055293576900624102905702190107107910178870863567e-4 + x^2 * (-1.4924712302009784215659095812852066398064014024546e-5 + x^2 * (1.6447131571277696087513420311315555247123856186925e-6 + x^2 * (-1.6206313758481267251097356025630327970797016185629e-7 + x^2 * (1.3710980397970515999986433969994634679368669925207e-8 + x^2 * (-7.7794684888425169575250383296580432148481928654068e-10))))))))))))
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
#include <libm/poly-vec.h>
#include <libm/arch/zen5.h>

static const struct {
    v_u64x8_t   bound1, bound2;
    v_u64x8_t   mask;
    v_u64x8_t   one;
    v_f64x8_t   poly_1[12];
    v_f64x8_t   poly_2[22];
} v_erf_data = {
	      .bound1      =  _MM512_SET1_U64x8(0x3FF0000000000000U),
              .bound2      =  _MM512_SET1_U64x8(0x4017B00000000000U),
              .mask        =  _MM512_SET1_U64x8(0x7FFFFFFFFFFFFFFFU),
              .one         =  _MM512_SET1_U64x8(0x3FF0000000000000U),
             // Polynomial coefficients obtained using Remez algorithm in Sollya
              .poly_1 = {
                            _MM512_SET1_PD8(0x1.20dd750429b6d082d8f38c297p0),
			    _MM512_SET1_PD8(-0x1.812746b0379bccc7e43eecd0ae8p-2),
			    _MM512_SET1_PD8(0x1.ce2f21a040d15df6c56053f5b44p-4),
			    _MM512_SET1_PD8(-0x1.b82ce311fa93d8e3f0f9f708d1p-6),
			    _MM512_SET1_PD8(0x1.565bccf92b2f8deb3d7b5c92bf8p-8),
			    _MM512_SET1_PD8(-0x1.c02db03dd71d46eab5b32363754p-11),
			    _MM512_SET1_PD8(0x1.f9a2baa8fedd06ab119b76772ccp-14),
			    _MM512_SET1_PD8(-0x1.f4ca4d6f3e2e0dabae3fd947dbp-17),
			    _MM512_SET1_PD8(0x1.b97fd3d9926cf844bfec030c87p-20),
			    _MM512_SET1_PD8(-0x1.5c0726f04cb59d4a5cb66cbf38p-23),
			    _MM512_SET1_PD8(0x1.d71b0f1b08156479f71f2804d9cp-27),
			    _MM512_SET1_PD8(-0x1.abae491c2886060a263929c0d3p-31),
		        },
              .poly_2 = {
                            _MM512_SET1_PD8(-0x1.20dd758d25ff45f5d6e045f72a4p0),
			    _MM512_SET1_PD8(-0x1.45f2f7628562a0532ccbf40ecfp-1),
			    _MM512_SET1_PD8(-0x1.a4f7e461b39210d97cccc33a68cp-4),
			    _MM512_SET1_PD8(0x1.3992f604e0b0562e8feadba923cp-6),
			    _MM512_SET1_PD8(0x1.7eebfdb640bcbe5b2af9ed12508p-13),
			    _MM512_SET1_PD8(-0x1.afdbe8e55573b64f9fc61ccb634p-10),
			    _MM512_SET1_PD8(0x1.1195488d78110200f233e954778p-11),
			    _MM512_SET1_PD8(0x1.9c68216ea92406952d1667e8288p-15),
			    _MM512_SET1_PD8(-0x1.114144e09abcc132a93422999cp-13),
			    _MM512_SET1_PD8(0x1.3f6794bb9cb840fd47a36d08898p-14),
                            _MM512_SET1_PD8(-0x1.f22cc1d2f21fb3fbb1b2408b054p-16),
			    _MM512_SET1_PD8(0x1.29e6dbb394a45396edfd9b8a6p-17),
			    _MM512_SET1_PD8(-0x1.1d804d01ae12d4d3e8ff837f12p-19),
			    _MM512_SET1_PD8(0x1.bd40e69c394975b5cd06f59acb4p-22),
			    _MM512_SET1_PD8(-0x1.1b0d2cbfee91981c0ca484d24ep-24),
			    _MM512_SET1_PD8(0x1.23753970b571e663ffa6959bd0cp-27),
			    _MM512_SET1_PD8(-0x1.df393acf4588156726bab645208p-31),
			    _MM512_SET1_PD8(0x1.3342dc7d8ce26ca169ddcdc513p-34),
			    _MM512_SET1_PD8(-0x1.2818b20d1fbc5bd0fe4ea3143p-38),
			    _MM512_SET1_PD8(0x1.934bdf577f269369478ba51118p-43),
			    _MM512_SET1_PD8(-0x1.59fa2297bf413bbe4f84afbe354p-48),
			    _MM512_SET1_PD8(0x1.1910cfbd5c781371cab27e0c03p-54),
		        },
	       };
#define MASK      v_erf_data.mask
#define ONE       v_erf_data.one
#define BOUND1    v_erf_data.bound1
#define BOUND2    v_erf_data.bound2

/* Coefficients for 12-degree polynomial */
#define A1  v_erf_data.poly_1[0]
#define A2  v_erf_data.poly_1[1]
#define A3  v_erf_data.poly_1[2]
#define A4  v_erf_data.poly_1[3]
#define A5  v_erf_data.poly_1[4]
#define A6  v_erf_data.poly_1[5]
#define A7  v_erf_data.poly_1[6]
#define A8  v_erf_data.poly_1[7]
#define A9  v_erf_data.poly_1[8]
#define A10  v_erf_data.poly_1[9]
#define A11  v_erf_data.poly_1[10]
#define A12  v_erf_data.poly_1[11]

/* Coefficients for 22-degree polynomial */
#define B1  v_erf_data.poly_2[0]
#define B2  v_erf_data.poly_2[1]
#define B3  v_erf_data.poly_2[2]
#define B4  v_erf_data.poly_2[3]
#define B5  v_erf_data.poly_2[4]
#define B6  v_erf_data.poly_2[5]
#define B7  v_erf_data.poly_2[6]
#define B8  v_erf_data.poly_2[7]
#define B9  v_erf_data.poly_2[8]
#define B10  v_erf_data.poly_2[9]
#define B11  v_erf_data.poly_2[10]
#define B12  v_erf_data.poly_2[11]
#define B13  v_erf_data.poly_2[12]
#define B14  v_erf_data.poly_2[13]
#define B15  v_erf_data.poly_2[14]
#define B16  v_erf_data.poly_2[15]
#define B17  v_erf_data.poly_2[16]
#define B18  v_erf_data.poly_2[17]
#define B19  v_erf_data.poly_2[18]
#define B20  v_erf_data.poly_2[19]
#define B21  v_erf_data.poly_2[20]
#define B22  v_erf_data.poly_2[21]

#define SCALAR_ERF ALM_PROTO_OPT(erf)

static inline int test_condition_for_all(v_u64x8_t cond) {
    for(int i = 0; i < 8; i++) {
        if(cond[i] != 0xffffffffffffffff)
            return 0;
    }
    return 1;
}

v_f64x8_t
ALM_PROTO_ARCH_ZN5(vrd8_erf)(v_f64x8_t _x) {
    v_f64x8_t result;
    v_u64x8_t uvx = as_v8_u64_f64(_x) & MASK;
    v_u64x8_t sign = as_v8_u64_f64(_x) & ~MASK;
    v_f64x8_t x = as_v8_f64_u64(uvx);

    v_u64x8_t cond1 = uvx <= BOUND1;
    if(test_condition_for_all(cond1)) {
      /* Special implementation of Horner's method for polynomial through the origin */
      v_f64x8_t xsq = x*x;
      result = POLY_EVAL_HORNER_11(xsq, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) * x;
      result = as_v8_f64_u64(sign | as_v8_u64_f64(result));
      return result;
    }

    v_f64x8_t fONE = as_v8_f64_u64(ONE);

    v_u64x8_t cond2 = uvx <= BOUND2;
    v_u64x8_t cond2a = uvx > BOUND1;
    if(test_condition_for_all(cond2) && test_condition_for_all(cond2a)) {

      /* The polynomial evaluation has been split over two macros 
         because compilers were struggling to compile a single macro. */
      v_f64x8_t result1 = POLY_EVAL_HORNER_11(x, B11, B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22);
      v_f64x8_t result2 = POLY_EVAL_HORNER_10(x, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, result1) * x;
      
      result = fONE - amd_vrd8_exp(result2);
      result = as_v8_f64_u64(sign | as_v8_u64_f64(result));
      return result;
    }
    
    v_u64x8_t cond3 = uvx > BOUND2;
    if(test_condition_for_all(cond3)) {
        return as_v8_f64_u64(sign | ONE);
    }

    /* For the remaining cases, where the vector spans more than one region,
       call the scalar function. */
    for(uint64_t i = 0; i < 8; i++) {
        result[i] = SCALAR_ERF(_x[i]);
    }
    return result;
}
