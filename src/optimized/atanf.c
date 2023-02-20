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
/* Contains implementation of float atanf(float x)
 *
 * sign = sign(x)
 * x = abs(x)
 *
 * Argument reduction to range [-7/16,7/16]
 * Use the following identities
 * x = (x-t)/(1+t*x); where t=0,1/2,1,3/2 based on x values within range
 * then use core approximation: Remez(2,2) on [-7/16,7/16]
 */
#include <libm_util_amd.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/alm_special.h>
#include <libm/poly.h>

static struct {
    double range[6],value[5], poly_atanf[6], piby2;
} atanf_data = {
    // Values of absolute inputs for different x values in comments
    .range = {
        0x3ec0000000000000,  /* 2.0^(-19) */
        0x3fdc000000000000,  /* 7./16. */
        0x3fe6000000000000,  /* 11./16. */
        0x3ff3000000000000,  /* 19./16. */
        0x4003800000000000,  /* 39./16. */
        0x4190000000000000,  /* 2^26  */
    },
    // Values of different arctan inputs
    .value = {
        0.0,/*arctan(0.0)*/
        4.63647609000806093515e-01,/*arctan(0.5)*/
        7.85398163397448278999e-01,/*arctan(1.0)*/
        9.82793723247329054082e-01,/*arctan(1.5)*/
        1.57079632679489655800e+00,/*arctan(infinity)*/
    },
    .poly_atanf = {
        0.296528598819239217902158651186e0,
        0.192324546402108583211697690500e0,
        0.470677934286149214138357545549e-2,
        0.889585796862432286486651434570e0,
        0.111072499995399550138837673349e1,
        0.299309699959659728404442796915e0,
    },
    .piby2 = 1.5707963267948966e+00,
};
#define RANGE atanf_data.range
#define VALUE atanf_data.value
#define PIBY2 atanf_data.piby2
#define C0 atanf_data.poly_atanf[0]
#define C1 atanf_data.poly_atanf[1]
#define C2 atanf_data.poly_atanf[2]
#define C3 atanf_data.poly_atanf[3]
#define C4 atanf_data.poly_atanf[4]
#define C5 atanf_data.poly_atanf[5]
/*
 ********************************************
 * Implementation Notes
 * ---------------------
 * sign = sign(xi)
 * xi = |xi|
 *
 * Argument reduction: Use the following identities
 *
 * 1. If xi < 2.0^(-19),
 *   atanf(xi)= xi
 *
 * 2. If x < 7/16,
 *      t=0
 * 3. If x < 11/16,
 *      t=1/2
 * 4. If x < 19/16,
 *      t=1
 * 5. If x < 39/16,
 *      t=3/2
 * 6. If x < 2^26),
 *      x=-1/x
 * x = (x-t)/(1+t*x);
 * Core approximation: Remez(2,2) on [-7/16,7/16]
 */
float
ALM_PROTO_OPT(atanf)(float fx)
{
    uint32_t fux = asuint32(fx);
    double x = fx;
    uint64_t ux = asuint64(x);
    /* Find properties of argument fx. */
    uint64_t aux = ux & ~SIGNBIT_DP64;
    uint64_t xneg = ux & SIGNBIT_DP64;

    if (unlikely(aux > PINFBITPATT_DP64)){
    /* fx is NaN */
        #ifdef WINDOWS
        return  __alm_handle_errorf(fux|0x00400000, 0);
        #else
        return fx; /* Raise invalid if it's a signalling NaN */
        #endif
    }

    double c;
    if (xneg) x = -x;
    /* Argument reduction to range [-7/16,7/16] */

    if (aux < RANGE[0]){
      /* x is a good approximation to atan(x) */
        if (aux == 0x0000000000000000)
            return fx;
        else
        #ifdef WINDOWS
            return fx ; //valf_with_flags(fx, AMD_F_INEXACT);
        #else
            return  __alm_handle_errorf(fux, AMD_F_UNDERFLOW|AMD_F_INEXACT);
        #endif
    }
    else if (aux < RANGE[1]){
        c = VALUE[0];
    }else if (aux < RANGE[2]){
        x = (2.0 * x - 1.0) / (2.0 + x);
        c = VALUE[1];
    }
    else if (aux < RANGE[3]){
        x = (x - 1.0) / (1.0 + x);
        c = VALUE[2];
    }
    else if (aux < RANGE[4]){
        x = (x - 1.5) / (1.0 + 1.5 * x);
        c = VALUE[3];
    }
    else{
        if (aux > RANGE[5]){
            float f = xneg ? (float)-PIBY2 : (float)PIBY2;
            return _atanf_special_overflow(f);
        }
        x = -1.0 / x;
        c = VALUE[4];
    }
    if(x == 0.0)
        return xneg ? -(float)c : (float)c;

    double p1 = POLY_EVAL_ODD_7(x, C0, C1, C2);
    double p2 = POLY_EVAL_ODD_7(x, C3, C4, C5);
    double s = x * x;
    double p = x * s * (p1 - x) / (p2 - x);
    double z;
    if (xneg){
        z = (p - x) - c;
    }else{
        z = c - (p - x);
    }
    return (float)z;
}
