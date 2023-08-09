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

#define A1 0x1.20dd74fb4a010a2afd4c5070338p0
#define A2 0x1.bb6adfe29cd72e02ee1b55ecc7p-22
#define A3 -0x1.812ad66cea0485a604c3bee754p-2
#define A4 0x1.689a10834b705621639eeb53548p-13
#define A5 0x1.c9bac884880d7edd8e3fecb9584p-4
#define A6 0x1.f76d2756b3857585a82bc1490acp-9
#define A7 -0x1.1c6e1bbf6350e35bd131d7b28f4p-5
#define A8 0x1.26ea86cfea825214bf1319f7488p-7
#define A9 0x1.bbf1527921e845261e6ac3012a4p-12

#define B1 0x1.216dfdd63ded3a555db91ab1968p0
#define B2 -0x1.316e90e0f569c20a92b33d4b1b4p-6
#define B3 -0x1.3b3c25eaf966493e719e3e101ap-2
#define B4 -0x1.1fe7e24e6a45235e12fe8e1c4b4p-3
#define B5 0x1.270d2d6e679818b4282a14f3118p-2
#define B6 -0x1.0af586b2978f6bb2dab464c6cdcp-3
#define B7 0x1.6a1ea2cb0094a1c8757907e70c4p-6
#define B8 -0x1.70ad24fa9aa8a35d78016cfddacp-11

#define C1 0x1.2430bb7872a1fba55661e01532cp0
#define C2 -0x1.6488921e9d39558014b7a53e8a8p-4
#define C3 -0x1.ec8d0aaf0be1edc1d0b00a94db4p-4
#define C4 -0x1.b7cf8ab2b9119b84c78e4e85924p-2
#define C5 0x1.1e3690934f38e06c20a93a14308p-1
#define C6 -0x1.250e3470871f8c91ce5584af77p-2
#define C7 0x1.2fdf00756033a27e033979bfb3p-4
#define C8 -0x1.2fb6a92336e4b26bd55ad67bd2cp-7
#define C9 0x1.9809da7c409deac691bd0b7325cp-12

#define D1 0x1.1b8b3b7cb8c183371675f71f64p0
#define D2 0x1.66f04abdaa24885c7850cdef25p-5
#define D3 -0x1.4f775d461f0a83f20d0faa316e4p-2
#define D4 -0x1.0f20fdbbac25543f9e6c2c7499cp-2
#define D5 0x1.00e1f3c4e15c9c242e74da54dc4p-1
#define D6 -0x1.2b7de40c528634f35cc608beadcp-2
#define D7 0x1.642833f6d17736322a5466ab0c8p-4
#define D8 -0x1.b9ad8e689f8aaff35cf9c6e3f5cp-7
#define D9 0x1.c8908d73595b60cc9259821eaecp-11

#define E1 0x1.42ab3a54e43e843b8691789e25p-1
#define E2 0x1.cc0d61ace2970252e9fa8eef094p0
#define E3 -0x1.9403d4dc1b20f44658d75ed45ap1
#define E4 0x1.2d4dcd474a659ff8a89e10aedccp1
#define E5 -0x1.0503697bf741f9e226aa39fad6p0
#define E6 0x1.1a02c0c45abc21a0255b3051868p-2
#define E7 -0x1.79368b2f9ce1358ecba1f1a843p-5
#define E8 0x1.1f87c31752a0b41c30edcf19c1cp-8
#define E9 -0x1.7fc20151886050b0c14fadf5aa8p-13

#define F1 0x1.bd2db6c457c74a70ee62090288p0
#define F2 -0x1.2c968671b83970884c6594db838p0
#define F3 0x1.53c57a4adbca4618908f7f2e2a4p-2
#define F4 0x1.5cf1081b477e56041bcd1631bacp-7
#define F5 -0x1.18f575d3fffabdbb9eb8ecc765cp-5
#define F6 0x1.432466d89ec79ff73d29a6dffb8p-7
#define F7 -0x1.47b44cc8548d805c3813245cb5p-10
#define F8 0x1.04f89258cf8d391f1c363cd4d9p-14

#define G1 0x1.e29ebcd655cbfaa69c42eb208a8p0
#define G2 -0x1.86807fda153a60b7930d583ba2cp0
#define G3 0x1.5f838d9e2fdf802bcaa238fb77cp-1
#define G4 -0x1.7c1c97e8ff0e7fee0be54d795d8p-3
#define G5 0x1.edb2f0ba255af078c4bdbf8b824p-6
#define G6 -0x1.6484bb091b3b7d8adab44fbe358p-9
#define G7 0x1.b9a5204a6080aaf93e64d776ccp-14

float ALM_PROTO_OPT(erff)(float _x) {

    uint32_t ux = asuint32(_x);

    uint32_t sign = ux & SIGNBIT_SP32;

    ux = ux & ~SIGNBIT_SP32;

    float x = asfloat(ux);

    double dx = (double)x;

    float result;

    if ( ux <= asuint32(BOUND1)) {

        dx = POLY_EVAL_HORNER_9_0(dx, A1, A2, A3, A4, A5, A6, A7, A8, A9);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if (ux <= asuint32(BOUND2)) {

        dx = POLY_EVAL_HORNER_8_0(dx, B1, B2, B3, B4, B5, B6, B7, B8);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if ((ux <= asuint32(BOUND3))) {

        dx = POLY_EVAL_HORNER_9_0(dx, C1, C2, C3, C4, C5, C6, C7, C8, C9);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if ((ux <= asuint32(BOUND4))) {

        dx = POLY_EVAL_HORNER_9_0(dx, D1, D2, D3, D4, D5, D6, D7, D8, D9);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if ((ux <= asuint32(BOUND5))) {

        dx = POLY_EVAL_HORNER_9_0(dx, E1, E2, E3, E4, E5, E6, E7, E8, E9);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if ((ux <= asuint32(BOUND6))) {

        dx = POLY_EVAL_HORNER_8_0(dx, F1, F2, F3, F4, F5, F6, F7, F8);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else if ((ux <= asuint32(BOUND7))) {

        dx = POLY_EVAL_HORNER_7_0(dx, G1, G2, G3, G4, G5, G6, G7);
        
        result = (float)dx;

        result = asfloat(sign | asuint32(result));

        return result;

    } else {

        /*x is NaN */
        if (ux > PINFBITPATT_SP32) {

            if(!(ux & QNAN_MASK_32)) /* x is snan */
                return __alm_handle_errorf(ux, AMD_F_INVALID);

            return _x;

        }

        result = 1.0f;

        result = asfloat(sign | asuint32(result));

        return result;

    }

}


