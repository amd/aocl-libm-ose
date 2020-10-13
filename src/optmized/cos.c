/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double cos(double x)
 *
 * Spec:
 *  cos(0)    = 1
 *  cos(-0)   = 1
 *  cos(inf)  = NaN
 *  cos(-inf) = NaN
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute cos(double x)
 *
 * 1. Argument Reduction
 *      Let input  x be represented as,
 *          |x| = N * pi + f        (1) | N is an integer,
 *                                        -pi/2 <= f <= pi/2
 *
 *      From (2), N = int(x / pi + 1/2) - 1/2
 *                f = |x| - (N * pi)
 *
 * 2. Polynomial Evaluation
 *       From (1) and (2),cos(f) can be calculated using a polynomial
 *       cos(f) = f + C1 * f^3 + C2 * f^5 + C3 * f^7 + C4 * f^9 +
 *                C5 * f^11 + C6 * f^13 + C7 * f^15 + C8 * f^17
 *
 * 3. Reconstruction
 *      Hence, cos(x) = sin(x + pi/2) = sign(x) * cos(f)
 *
 * MAX ULP of current implementation : 2
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
        double poly_cos[8];
        double pi, halfpi, invpi;
        double pi1, pi2, pi3;
        double half;
        } cos_data = {
                        .pi = 0x1.921fb54442d188p1,
                        .halfpi = 0x1.921fb54442d18p0,
                        .invpi = 0x1.45f306dc9c883p-2,
                        .pi1 = -0x1.921fb54442d18p+1,
                        .pi2 = -0x1.1a62633145c07p-53,
                        .pi3 = 0x1.f1976b7ed8fbcp-109,
                        .half = 0x1p-1,
                        .poly_cos = {
                                        -0x1.5555555555555p-3,
                                        0x1.11111111110bp-7,
                                        -0x1.a01a01a013e1ap-13,
                                        0x1.71de3a524f063p-19,
                                        -0x1.ae6454b5dc0b5p-26,
                                        0x1.6123c686ad6b4p-33,
                                        -0x1.ae420dc08fd52p-41,
                                        0x1.880ff69a83bbep-49,
                                    },
        };

#define COS_PI      cos_data.pi
#define COS_HALF_PI cos_data.halfpi
#define COS_INV_PI  cos_data.invpi
#define COS_PI1     cos_data.pi1
#define COS_PI2     cos_data.pi2
#define COS_PI3     cos_data.pi3
#define COS_HALF    cos_data.half

#define C1 cos_data.poly_cos[0]
#define C2 cos_data.poly_cos[1]
#define C3 cos_data.poly_cos[2]
#define C4 cos_data.poly_cos[3]
#define C5 cos_data.poly_cos[4]
#define C6 cos_data.poly_cos[5]
#define C7 cos_data.poly_cos[6]
#define C8 cos_data.poly_cos[7]

#define COS_SIGN_MASK 0x7FFFFFFFFFFFFFFF
#define COS_INF       0x7ff0000000000000

double
ALM_PROTO_OPT(cos)(double x)
{

    double dinput, dn, frac, poly, result;

    uint64_t n, ixd, odd;

    ixd = asuint64(x);

    /* Check for special cases */
    if(unlikely((ixd & COS_SIGN_MASK) >= COS_INF)) {
        return _cosf_special(x);
    }

    /* Remove sign from the input */
    ixd = ixd & COS_SIGN_MASK;
    dinput = asdouble(ixd);

    const double_t ALM_HUGE = 0x1.8000000000000p52;

    /* Get n = int(x / pi + 1/2) - 1/2 */
    dn = ((dinput * COS_INV_PI) + COS_HALF) + ALM_HUGE;
    n = asuint64(dn);
    dn = dn - ALM_HUGE;
    dn = dn - COS_HALF;

    /* frac = x - (n*pi) */
    frac = dinput + dn * COS_PI1;
    frac = frac + dn * COS_PI2;
    frac = frac + dn * COS_PI3;

    /* Check if n is odd or not */
    odd = n << 63;

    /* Compute cos(f) using the polynomial
     * f + C1 * f^3 + C2 * f^5 + C3 * f^7 + C4 * f^9 +
     * C5 * f^11 + C6 * f^13 + C7 * f^15 + C8 * f^17
     */
    poly = frac + POLY_EVAL_ODD_17(frac, C1, C2, C3, C4, C5, C6, C7, C8);
    /*
    double f = frac * frac;
    poly = C8 * f + C7;
    poly = poly * f + C6;
    poly = poly * f + C5;
    poly = poly * f + C4;
    poly = poly * f + C3;
    poly = poly * f + C2;
    poly = poly * f + C1;
    poly = poly * f;
    poly = poly * frac + frac;
    */

    //poly = POLY_EVAL_ODD_17_H(frac, C1, C2, C3, C4, C5, C6, C7, C8);

    /* If n is odd, result is negative */
    if (odd)
        result = -poly;
    else
        result = poly;

    return result;

}

