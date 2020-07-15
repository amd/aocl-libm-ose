/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float sinf(float x)
 *
 * Spec:
 *   sinf(0)    = 0
 *   sinf(-0)   = -0
 *   sinf(inf)  = NaN
 *   sinf(-inf) = NaN
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * checks for special cases
 * if ( x < 0x1p-126) raise undeflow exception and return x
 * if ( asint(x) > infinity) return x with overflow exception and 
 * return x.
 * if x is NaN then raise invalid FP operation exception and return x.
 *
 * 1. Argument reduction
 * Convert given x into the form
 * |x| = N * pi + f where N is an integer and f lies in [-pi/2,pi/2]
 * N is obtained by : N = round(x/pi)
 * f is obtained by : f = abs(x)-N*pi
 * sin(x) = sin(N * pi + f) = sin(N * pi)*cos(f) + cos(N*pi)*sin(f)
 * sin(x) = sign(x)*sin(f)*(-1)**N
 * f = abs(x) - N * pi
 *
 * 2. Polynomial approximation 
 * The term sin(f) where [-pi/2 < f < pi/2] can be approximated by 
 * using a polynomial computed using sollya using the Remez 
 * algorithm to determine the coeffecients and the maximum ulp is 1.
 *
 *
 ******************************************
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
    const double invpi, pi, pi1, pi2;
    double poly_sinf[5];
 } sinf_data = {
     .pi = 0x1.921fb54442d18p1,
     .pi1 = 0x1.921fb50000000p1,
     .pi2 = 0x1.110b4611a6263p-25,
     .invpi = 0x1.45f306dc9c883p-2,
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sinf = {
	  0x1.p0,
         -0x1.55554d018df8bp-3,
          0x1.110f0293a5dcbp-7,
         -0x1.9f781a0aebdb9p-13,
          0x1.5e2a3e7550c85p-19,
     },
};


#define pi    sinf_data.pi
#define pi1   sinf_data.pi1
#define pi2   sinf_data.pi2
#define invpi sinf_data.invpi

#define C1  sinf_data.poly_sinf[0]
#define C3  sinf_data.poly_sinf[1]
#define C5  sinf_data.poly_sinf[2]
#define C7  sinf_data.poly_sinf[3]
#define C9  sinf_data.poly_sinf[4]

#define SIGN_MASK   0x7FFFFFFFFFFFFFFF
#define SIGN_MASK32 0x7FFFFFFF

static inline uint32_t abstop12(float x)
{
    return (asuint32(x) & SIGN_MASK32) >> 20;
}

float _sinf_special(float x);
double _sin_special_underflow(double x);

float
ALM_PROTO_OPT(sinf)(float x)
{

    double xd, F, poly, result;
    uint64_t n;
    uint64_t uxd, sign = 0;

    /* sin(inf) = sin(-inf) = sin(NaN) = NaN */

    uint32_t ux = asuint32(x);

    if(unlikely((ux - asuint32(0x1p-126)) > (0x7f800000 - asuint32(0x1p-126)))) {

        if((ux  & SIGN_MASK32) >= 0x7f800000) {
            /* infinity or NaN */
            return _sinf_special(x);
        }

        if(abstop12(x) < abstop12(0x1p-126)) {
             _sin_special_underflow(x);
             return x;
        }
    }

    const double_t ALM_SHIFT = 0x1.8000000000000p52;

    xd = (double)x;

    uxd = asuint64(xd);

    sign = uxd & (~SIGN_MASK);

    xd = asdouble(uxd & SIGN_MASK);

    double dn =  xd * invpi + ALM_SHIFT;

    n = asuint64(dn);

    dn -= ALM_SHIFT;

    F = xd - dn * pi1;

    /* Get the fraction part */
    F = F - dn * pi2;

    uint64_t odd =  n << 63;

    /* Calculate the polynomial approximation.
     *
     * sin(F) = x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
     *
     */

    poly = POLY_EVAL_ODD_9(F, C1, C3, C5, C7, C9);

    result = asdouble(asuint64(poly) ^ sign ^ odd);

    return (float)result;
}
