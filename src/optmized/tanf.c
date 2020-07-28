/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */


#include <stdint.h>

#include <libm_macros.h>
#include <libm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#define  ALM_SIGN_MASK   ~(1UL<<63)
#define  ALM_SIGN_MASK32 ~(1U<<31)


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float tanf(float x)
 *
 * Spec:
 *   tanf(n· 2π + π/4)  = 1       if n∈Z and |n· 2π + π/4|   <= big_angle_F
 *   tanf(n· 2π + 3π/4) = −1      if n∈Z and |n· 2π + 3π/4|  <= big angle rF
 *   tanf(x) = x                  if x ∈ F^(2·π) and tanf(x) != tan(x)
 *                                               and |x| < sqrt(epsilonF/rF)
 *   tanf(−x) = −tanf(x)          if x ∈ F^(2·π)
 */

static const struct {
    double huge;
    //double pihi, pilow;
    double halfpi, invhalfpi;
    double poly_tanf[8];
} tanf_data = {
    .huge      = 0x1.8000000000000p52,
    .halfpi  = 0x1.921fb54442d18469899p0,
    //.pihi      = 0x1.921fb50000000p1,
    //.pilow     = 0x1.110b4611a6263p-25,
    .invhalfpi = 0x1.45f306dc9c882a53f85p-1,
    // Polynomial coefficients obtained using Remez algorithm from Sollya
    .poly_tanf = {
        0x1.ffffff99ac0468p-1,
        0x1.55559193ecf2bp-2,
        0x1.1106bf4ba8f408p-3,
        0x1.bbafbb6650308p-5,
        0x1.561638922df5fp-6,
        0x1.7f3a033a88788p-7,
        -0x1.ba0d41d26961f8p-11,
        0x1.3952b4eff28ac8p-8,
    },

};

#define ALM_HUGE_VAL    tanf_data.huge
#define ALM_HALFPI      tanf_data.halfpi
#define ALM_PI_HIGH     tanf_data.pihi
#define ALM_PI_LOW      tanf_data.pilow
#define ALM_INVHALFPI   tanf_data.invhalfpi
#define C1 tanf_data.poly_tanf[0]
#define C2 tanf_data.poly_tanf[1]
#define C3 tanf_data.poly_tanf[2]
#define C4 tanf_data.poly_tanf[3]
#define C5 tanf_data.poly_tanf[4]
#define C6 tanf_data.poly_tanf[5]
#define C7 tanf_data.poly_tanf[6]
#define C8 tanf_data.poly_tanf[7]


/*
 * Implementation Notes:
 *
 * float tanf(float x)
 *      A given x is reduced into the form:
 *
 *               |x| = (N * π/2) + F
 *
 *      Where N is an integer obtained using:
 *              N = round(x * 2/π)
 *      And F is a fraction part lying in the interval
 *              [-π/4, +π/4];
 *
 *      obtained as F = |x| - (N * π/2)
 *
 *      Thus tan(x) is given by
 *
 *              tan(x) = tan((N * π/2) + F) = tan(F)
 *              when N is even,
 *                     = -cot(F) = -1/tan(F)
 *              when N is odd, tan(F) is approximated using a polynomial
 *                      obtained from Remez approximation from Sollya.
 *
 */

float
ALM_PROTO_OPT(tanf)(float x)
{
    double     F, xd, tanx = 0.0;
    double     poly;
    uint64_t   sign, uxd, n;
    uint32_t   ux = asuint32(x);

    if(unlikely((ux - asuint32(0x1p-126)) >
                (asuint32(0x1p+127) - asuint32(0x1p-126)))) {

        if((ux  & ALM_SIGN_MASK32) >= 0x7f800000) {
            /*  infinity or NaN */
            return _tanf_special(x);
        }
    }

    if (ux == 0)
        return 0;

    xd = (double)x;

    uxd = asuint64(xd);

    sign = uxd & (~ALM_SIGN_MASK);

    /* fabs(x) */
    xd = asdouble(uxd & ALM_SIGN_MASK);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */
    double dn =  xd * ALM_INVHALFPI + ALM_HUGE_VAL;

    /* n = (int)dn */
    n   = asuint64(dn);

    dn -= ALM_HUGE_VAL;

    /* F = xd - (n * π/2) */
    F = xd - dn * ALM_HALFPI;

    uint64_t odd = (n << 63);

    /*
     * Calculate the polynomial approximation
     *  x * (C1 + C2*x^2 + C3*x^4 + C4*x^6 + C5*x^8 + C6*x^10 + C7*x^12 + C8*x^14)
     * polynomial is approximated as x*P(x^2)
     */
    poly = POLY_EVAL_EVEN_14(F, C1, C2, C3, C4, C5, C6, C7, C8);

    tanx = asdouble(asuint64(poly) ^ sign);

    if (odd)
        tanx = -1.0/tanx;

    return eval_as_float(tanx);
}
