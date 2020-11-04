/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float cosf(float x)
 *
 * Spec:
 *  cosf(0)    = 1
 *  cosf(-0)   = 1
 *  cosf(inf)  = NaN
 *  cosf(-inf) = NaN
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute cosf(float x)
 * Using the identity,
 * cos(x) = sin(x + pi/2)           (1)
 *
 * 1. Argument Reduction
 *      Adding pi/2 to x, x is now x + pi/2
 *      Now, let x be represented as,
 *          |x| = N * pi + f        (2) | N is an integer,
 *                                        -pi/2 <= f <= pi/2
 *
 *      From (2), N = int(x / pi)
 *                f = |x| - (N * pi)
 *
 * 2. Polynomial Evaluation
 *       From (1) and (2),sin(f) can be calculated using a polynomial
 *       sin(f) = f*(1 + C1*f^2 + C2*f^4 + C3*f^6 + c4*f^8)
 *
 * 3. Reconstruction
 *      Hence, cos(x) = sin(x + pi/2) = (-1)^N * sin(f)
 *
 * MAX ULP of current implementation : 1
 */

#include <stdint.h>

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct {

            double poly_cosf[4];
            double half_pi, inv_pi, pi_head, pi_tail;
            } cosf_data = {
                            .half_pi = 0x1.921fb54442d18p0,
                            .pi_head = 0x1.921fb50000000p1,
                            .pi_tail = 0x1.110b4611a6263p-25,
                            .inv_pi  = 0x1.45f306dc9c883p-2,
              .poly_cosf = {
                            -0x1.55554d018df8bp-3,
                            0x1.110f0293a5dcbp-7,
                            -0x1.9f781a0aebdb9p-13,
                            0x1.5e2a3e7550c85p-19,
                           },
    };

#define HALF_PI cosf_data.half_pi
#define INV_PI  cosf_data.inv_pi
#define PI_HEAD cosf_data.pi_head
#define PI_TAIL cosf_data.pi_tail

#define C1 cosf_data.poly_cosf[0]
#define C2 cosf_data.poly_cosf[1]
#define C3 cosf_data.poly_cosf[2]
#define C4 cosf_data.poly_cosf[3]

#define SIGN_MASK 0x7FFFFFFFFFFFFFFF
#define SIGN_MASK32 0x7FFFFFFF

float _cosf_special(float x);
double _cos_special_underflow(double x);

static inline uint32_t abstop12(float x)
{
    return(asuint32(x) & SIGN_MASK32) >> 20;
}

float
ALM_PROTO_OPT(cosf)(float x)
{

    double dinput,frac,poly,result;
    uint64_t ixd;

    uint32_t ux = asuint32(x);

    // Check for special cases
    if(unlikely((ux - asuint32(0x1p-126)) > (0x7f800000 - asuint32(0x1p-126)))) {

        if((ux  & SIGN_MASK32) >= 0x7f800000) {
            // infinity or NaN
            return _sinf_cosf_special(x, "cosf", __amd_cos);
        }

        if(abstop12(x) < abstop12(0x1p-126)) {
            // Underflow
             _sinf_cosf_special_underflow(x, "cosf", __amd_cos);
             return x;
        }
    }

    // Convert input to double precision
    dinput = (double)x;
    ixd = asuint64(dinput);

    // Remove sign from input
    dinput = asdouble(ixd & SIGN_MASK);

    const double_t ALM_HUGE = 0x1.8000000000000p52;

    // x + pi/2
    dinput = dinput + HALF_PI;

    // Get n = int (x/pi)
    double dn  = (dinput * INV_PI) + ALM_HUGE;
    uint64_t n = asuint64(dn);
    dn = dn - ALM_HUGE;

    // frac = x - (n*pi)
    frac = dinput - (dn * PI_HEAD);
    frac = frac - (dn * PI_TAIL);

    // Check if n is odd or not
    uint64_t odd = n << 63;

    /* Compute sin(f) using the polynomial
       x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
    */
    poly = POLY_EVAL_ODD_9(frac, 1.0, C1, C2, C3, C4);

    // If n is odd, result is negative
    if(odd)
        result = -poly;
    else
        result = poly;

    return (float)result;

}
