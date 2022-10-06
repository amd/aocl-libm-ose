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
 *
 * Signature:
 * float amd_erff(float x);
 * Computes the error function of x
 *
 * Maximum ULP is 4
 * This implementation is not IEEE 754 compliant and does not add NaN
 *
 * Implementation Notes
 * ----------------------
 *  if x > 3.9192059040069580078125f
 *     return 1.0
 *
 *  else:
 *   we split the domain [0,3.91...] into subintervals and choose the polynomial
 *   approximant accordingly.
 *
 * if x is +/- zero, return zero
 * if x is +/- infinity, 1 is returned
 * if x is NaN, NaN is returned
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#define BOUND1 0.8f
#define BOUND2 1.1f
#define BOUND3 1.6f
#define BOUND4 2.2f
#define BOUND5 2.85f
#define BOUND6 3.35f
#define BOUND7 3.9192059040069580078125f

#define A1 0x1.20dd7279852d20bae6293720f14p0
#define A2 -0x1.81243af5b5c54b0958e8db62664p-2
#define A3 0x1.cd9759622bccb1ad8309b733b34p-4
#define A4 -0x1.adabe1d651ec58b1b05c9f01ffcp-6
#define A5 0x1.0836f5b874583d1db146de5e3ecp-8

#define B1 0x1.1abfcc1af4cd7f215ca6bd8a168p0
#define B2 0x1.13a805beaf4883ac4e360adfdb8p-3
#define B3 -0x1.5bcdf1cb5caa6b8ebe28fd3fa24p-1
#define B4 0x1.574e13b89f4ec23aa52542d3b28p-2
#define B5 -0x1.acc348ec49e2dcdceee392cab78p-5

#define C1 0x1.0f968e94fb7b2279e4eafb4b62cp0
#define C2 0x1.3fb8db3fc664509c4e2bf4b0658p-2
#define C3 -0x1.ee5532daa8f8512ef6fd14e59bcp-1
#define C4 0x1.1fc6442863d1bca9a3b6c03347p-1
#define C5 -0x1.1ee27db8b7d842bac8a0d5fdc48p-3
#define C6 0x1.acf02b0a0ea2a8140e6a977ebfcp-7

#define D1 0x1.22d221489d624aa5523733865dp0
#define D2 0x1.d7a49c8f0306987727478abb198p-4
#define D3 -0x1.8757236070ea53394d1d6a52194p-1
#define D4 0x1.dabf372d8e15bee97cad5ea4f8p-2
#define D5 -0x1.e1c154247566b869c5488bda2bcp-4
#define D6 0x1.7244c1e7f7e139524ce49f615acp-7

#define E1 0x1.aacd34384ec3af14f882486d01cp0
#define E2 -0x1.18ac56e96aba8abddfab7dd104cp0
#define E3 0x1.61e9777b93cf4870cf450ec3fe4p-2
#define E4 -0x1.808bb5ba94bad0ed37575e552ep-5
#define E5 0x1.2ebd1f7681bbd73977b55ff653cp-12
#define E6 0x1.7f01e5c1a867dcd1edded96bd38p-12

#define F1 0x1.95ba342ce2475d69aa502f92744p0
#define F2 -0x1.018e7e574815f4d72fb88a661b4p0
#define F3 0x1.4758086212787a68d23a74f5dap-2
#define F4 -0x1.a062b81875e9b8b426eded26c54p-5
#define F5 0x1.a7fbdc59fad76aec356f4bfe11cp-9

#define G1 0x1.60a6516b34c64cb4a9d93a01658p0
#define G2 -0x1.84637afb18dc950d3e7faf70dbcp-1
#define G3 0x1.ab79e6bcc5c0ab5509263eaa7p-3
#define G4 -0x1.d630d604604ed7695a3938a306p-6
#define G5 0x1.9d76f035a49dec452a1c1f94824p-10


float ALM_PROTO_FAST(erff)(float _x) {

    uint32_t ux = asuint32(_x);

    uint32_t sign = ux & SIGNBIT_SP32;

    ux = ux & ~SIGNBIT_SP32;

    float x = asfloat(ux);

    double dx = (double)x;

    float result;

    if ( ux <= asuint32(BOUND1)) {

        dx = dx * dx;

        dx = POLY_EVAL_HORNER_5(dx, A1, A2, A3, A4, A5);
        
        result = _x * (float)dx;

        return result;

    } else if (ux <= asuint32(BOUND2)) {

        dx = POLY_EVAL_HORNER_5(dx, B1, B2, B3, B4, B5);
        
        result = _x * (float)dx;

        return result;

    } else if ((ux <= asuint32(BOUND3))) {

        dx = POLY_EVAL_HORNER_6(dx, C1, C2, C3, C4, C5, C6);
        
        result = _x * (float)dx;

        return result;

    } else if ((ux <= asuint32(BOUND4))) {

        dx = POLY_EVAL_HORNER_6_0(dx, D1, D2, D3, D4, D5, D6);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if ((ux <= asuint32(BOUND5))) {

        dx = POLY_EVAL_HORNER_6(dx, E1, E2, E3, E4, E5, E6);
        
        result = _x * (float)dx;

        return result;

    } else if ((ux <= asuint32(BOUND6))) {

        dx = POLY_EVAL_HORNER_5(dx, F1, F2, F3, F4, F5);
        
        result = _x * (float)dx;

        return result;

    } else if ((ux <= asuint32(BOUND7))) {

        dx = POLY_EVAL_HORNER_5_0(dx, G1, G2, G3, G4, G5);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else {

        result = 1.0f;

        result = asfloat(sign | asuint32(result));

        return result;

    }

}

strong_alias (__erff_finite, ALM_PROTO_FAST(erff))
weak_alias (amd_erff, ALM_PROTO_FAST(erff))
weak_alias (erff, ALM_PROTO_FAST(erff))