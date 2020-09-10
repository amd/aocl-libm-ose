/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include <libm_macros.h>
#include <libm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

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

static struct {
    float poly_tanf[7];
    float halfpi, invhalfpi;
    float huge;
    float halfpi1, halfpi2, halfpi3;
} tanf_data = {
    .huge      = 0x1.8p23,
    .halfpi    = 0x1.921fb6p0,
    .invhalfpi = 0x1.45f306p-1,
    .halfpi1   = -0x1.921fb6p0,
    .halfpi2   = 0x1.777a5cp-25,
    .halfpi3   = 0x1.ee59dap-50,
    /*
     * Polynomial coefficients obtained using
     * fpminimax algorithm from Sollya
     */
    .poly_tanf = {
        0x1.555566p-2,
        0x1.110cdp-3,
        0x1.baf34p-5,
        0x1.5bf38ep-6,
        0x1.663acap-7,
        -0x1.07c6f4p-16,
        0x1.21cedap-8,
    },
};

#define  ALM_TANF_HUGE      tanf_data.huge
#define  ALM_TANF_HALFPI    tanf_data.halfpi
#define  ALM_TANF_INVHALFPI tanf_data.invhalfpi

#define  ALM_TANF_HALFPI1 tanf_data.halfpi1
#define  ALM_TANF_HALFPI2 tanf_data.halfpi2
#define  ALM_TANF_HALFPI3 tanf_data.halfpi3

#define  ALM_TANF_SIGN_MASK32 (1U<<31)

#define C1 tanf_data.poly_tanf[0]
#define C2 tanf_data.poly_tanf[1]
#define C3 tanf_data.poly_tanf[2]
#define C4 tanf_data.poly_tanf[3]
#define C5 tanf_data.poly_tanf[4]
#define C6 tanf_data.poly_tanf[5]
#define C7 tanf_data.poly_tanf[6]


/*
 * Implementation Notes:
 *  Convert given x into the form
 *
 *        |x| = N*(pi/2)+f
 *                        where N is integer and f in [-pi/4,+pi/4]
 *
 *             N = round(|x|*2/pi)
 *             f = |x| - N * pi/2
 *
 * tan(x) = tan(f)                when N is even
 *        = -cot(f) = -1/tan(f) when N is odd
 *
 * The term tan(f) can be approximated by using a polynomial
 */

float
ALM_PROTO_FAST(tanf)(float x)
{
    float      F, poly, result = 0.0;
    uint32_t   sign, n;
    uint32_t   ux = asuint32(x);

    if (ux == 0)
        return 0.0f;

    sign = ux & (ALM_TANF_SIGN_MASK32);

    /* x = abs(x) */
    x    = asfloat(ux & ~ALM_TANF_SIGN_MASK32);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */
    float nn =  x * ALM_TANF_INVHALFPI + ALM_TANF_HUGE;

    /* n = (int)n */
    n  = asuint32(nn);

    nn -= ALM_TANF_HUGE;

    /* F = xd - (n * π/2)
     * F = n*halfpi1+x ;
     * F = n*halfpi2+F ;
     * F = n*halfpi3+F ;
     */
    F = x + nn * ALM_TANF_HALFPI1;
    F = F + nn * ALM_TANF_HALFPI2;
    F = F + nn * ALM_TANF_HALFPI3;

    uint32_t odd = (n & 0x1);

    // Calculate the polynomial approximation
    //  = x+C1*x^3+C2*x^5+C3*x^7+C4*x^9+C5*x^11+C6*x^13+C7*x^15
    poly   = POLY_EVAL_ODD_15(F, C1, C2, C3, C4, C5, C6, C7);

    result = asfloat(sign ^ asuint32(poly));

    if (odd)
        result = -1.0/result;

    return result;
}
