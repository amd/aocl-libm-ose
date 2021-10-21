/*
 * Copyright (C) 2019-2020, Advanced Micro Devices. All rights reserved.
 */

/*
 * Description: Faster version of powf()
 *
 * Reference : Self contained exponentian -Clark, Cody
 * powf(x, y) = x^y
 *           = 2^(y*log2(x))
 *           = 2^w where w = y*log2(x)
 *
 * Evaluate log2(x)
 *          Required Values:
 *							b = n/C where n = 0 to C
 *							a = 2^(-b)
 *
 * Convert x to the form:
 *							 x = 2^k*m  where m lies in [1/2,1)
 *							   = 2^(k-b(p))*m/a(p)find the index p, p lies in [0,C]
 *
 *			log2(x) = (k-b(p))+log2(m/a(p))
 *							=  s1 + s2s2 lies in [2^(-1/C), 2^(1/C)]
 *											Where s1 is straight forward and
 *														s2 is calculated by polynomial approximation
 *
 * The values of b, a and 1/a are saved in a Lookup Table
 *
 *			 w = y*log2(x)
 *			      w = w1 + w2 , for extended precision
 *			      where w1 = int(C*w)*1/C
 *							    w2 = w-w1
 *
 * powf(x, y) = 2^w
 *			      = 2^(w1+w2)
 *						= 2^w1 * 2^w2				    % convert w1 and w2 into below shown
 *						= 2^(l + j/C) * 2^w2
 *						= 2^l * 2^(j/C) * 2^w2  % where w2 lies in (-1/C,0]
 *
 * In the above the first term 2^l is calculated using left shift operators the
 * second term 2^(j/C) is calculated from look up table the third terms 2^w2 is
 * calculated by a polynomial approximation
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#if 1
#define EXPSHIFTBITS_SP32       23
#define MANTBITS_SP32           0x007fffff
#define HALFEXPBITS_SP32        0x3f000000
#define SIGN_BIAS               0x80000000
#endif

const struct log2f_data {
      struct {
          double B, A, inv_a;
      } tab[17];
    double c;
    double inv_c;
    double log2_poly[4], pow2_poly[4];
} __log2f_data = {
    .c =  0x1p4,
    .inv_c =  0x1p-4,
    .tab = {
        { 0 ,       0x1.0p0,               0x1.0p0 },
        { 0x1.0p-4, 0x1.ea4afa2a490d98p-1, 0x1.0b5586cf9890f8p0 },
        { 0x1.0p-3, 0x1.d5818dcfba487p-1,  0x1.172b83c7d517bp0 },
        { 0x1.8p-3, 0x1.c199bdd85529cp-1,  0x1.2387a6e7562388p0 },
        { 0x1.0p-2, 0x1.ae89f995ad3ad8p-1, 0x1.306fe0a31b715p0 },
        { 0x1.4p-2, 0x1.9c49182a3f09p-1,   0x1.3dea64c123422p0 },
        { 0x1.8p-2, 0x1.8ace5422aa0db8p-1, 0x1.4bfdad5362a27p0 },
        { 0x1.cp-2, 0x1.7a11473eb0187p-1,  0x1.5ab07dd4854298p0 },
        { 0x1.0p-1, 0x1.6a09e667f3bcc8p-1, 0x1.6a09e667f3bcc8p0 },
        { 0x1.2p-1, 0x1.5ab07dd4854298p-1, 0x1.7a11473eb0187p0 },
        { 0x1.4p-1, 0x1.4bfdad5362a27p-1,  0x1.8ace5422aa0db8p0 },
        { 0x1.6p-1, 0x1.3dea64c123422p-1,  0x1.9c49182a3f09p0 },
        { 0x1.8p-1, 0x1.306fe0a31b715p-1,  0x1.ae89f995ad3ad8p0 },
        { 0x1.ap-1, 0x1.2387a6e7562388p-1, 0x1.c199bdd85529cp0 },
        { 0x1.cp-1, 0x1.172b83c7d517bp-1,  0x1.d5818dcfba487p0 },
        { 0x1.ep-1, 0x1.0b5586cf9890f8p-1, 0x1.ea4afa2a490d98p0 },
        { 0x1.0p0 , 0x1.0p-1,              0x1.0p1 },
    },

    .log2_poly = {
        0x1.71547432c09b9p0,
        -0x1.715490933e352p-1,
        0x1.ecfe48bedff7p-2,
        -0x1.70c69f47da9a1p-2,
    },

    .pow2_poly = {
        0x1.62e42fee489cep-1,
        0x1.ebfbd52835d5ep-3,
        0x1.c6a30bc837418p-5,
        0x1.35c122ef66d3ap-7,
    },
};

#define C       __log2f_data.c
#define INVC    __log2f_data.inv_c
#define T       __log2f_data.tab
#define L       __log2f_data.log2_poly
#define S       __log2f_data.pow2_poly

#include <libm_macros.h>
#include <libm_util_amd.h>
#include <libm/compiler.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

/*
 * Get the index of mantissa in A using binary search
 */
static inline int
binarysearch(double_t m)
{
    int lo= 0, hi = ARRAY_SIZE(T) - 1, middle, p;

    while (lo < hi) {
        middle = (hi - lo) >> 2;
        if (T[middle].A > m)
            lo = middle;
        else
            hi = middle;
    }

    if (T[lo].A * (T[lo].A-m) < T[lo].A * (m-T[lo].A))
        p = lo;
    else
        p = lo;

    return p;
}

#define ABSOLUTE_VALUE   0x7FFFFFFFFFFFFFFF /*  */

static inline uint32_t checkint(uint64_t u)
{
    int32_t u_exp = ((u & ABSOLUTE_VALUE) >> EXPSHIFTBITS_DP64);
    /*
     * See whether u is an integer.
     * status = 0 - not an integer.
     * status = 1 - odd
     * status = 2 - even
     */
    if (u_exp < 0x3ff)  return 0;

    if (u_exp > 0x3ff + EXPSHIFTBITS_DP64)
        return 2;

    if (u & ((1ULL << (0x3ff + EXPSHIFTBITS_DP64 - u_exp)) - 1))
        return 0;

    if (u & (1ULL << (0x3ff + EXPSHIFTBITS_DP64 - u_exp)))
        return 1; /*  odd integer */

    return 2;
}

static inline int issignaling(double x)
{
    uint64_t ix;
    ix = asuint64(x);
    return 2 * (ix ^ QNAN_MASK_64) > 2 * QNANBITPATT_DP64;

}

/*
 * Check if input is the bit representation of 0, infinity or nan.
 */
static inline int checkzeroinfnan(uint64_t i)
{
    return 2 * i - 1 >= 2 * EXPBITS_DP64 - 1;
}

#define ALM_POW_ARG_MAX   PINFBITPATT_SP32
#define ALM_POW_ARG_MIN   0x3F8800000

float
ALM_PROTO_FAST(powf)(float x, float y)
{
    double_t k, m, s1, s2;
    uint32_t ux, uy, mant;
    int p, sign_bias = 1;
    double s22, A1, A2, A3, log2x;
    double w, w1, w2, w22, poly, z, P, scale, result;
    unsigned int pdash;
    int iw1, i, mdash;

    ux = asuint32(x);
    uy = asuint32(y);

    if (unlikely (ux - ALM_POW_ARG_MIN >=
                  ALM_POW_ARG_MAX - ALM_POW_ARG_MIN ||
                  checkzeroinfnan(uy))) {

        /*
         * All x less than infinity, NaN and y = zero, infinity or NAN caught
         * here x < 0x1p-126 or inf or nan. Either (x < 0x1p-126 or inf or nan)
         * or (y is 0 or inf or nan).
         *
         */
        if (unlikely (checkzeroinfnan(uy))) {
            if (2 * uy == 0)
                return issignaling(x) ? x + y : 1.0f;

            if (ux == ONEEXPBITS_SP32)
                return issignaling(y) ? x + y : 1.0f;

            if (2 * ux > 2u * 0x7f800000 || 2 * uy > 2u * 0x7f800000)
                return x + y;

            if (2 * ux == TWOEXPBITS_SP32)
                return 1.0f;

            /*  |x|<1 && y==inf or |x|>1 && y==-inf.  */
            if ((2 * ux < TWOEXPBITS_SP32) == !(uy & 0x80000000))
                return 0.0f;

            return y * y;
        }

        if (unlikely (checkzeroinfnan(ux))) {
            float x2 = x * x;

            /*  x is -0 and y is odd */
            if (ux & 0x80000000 && checkint(uy) == 1) {
                x2 = -x2;
                sign_bias = (int)SIGN_BIAS;
            }

            if (2 * ux == 0 && uy & 0x80000000) {
                x = 1.0f / 0.0f;
                return asfloat((uint32_t)(sign_bias >> 23) | ux);
            }

            /*  if y is negative, return 1/x else return x */
            return (uy & 0x80000000 ? (1 / x2) : x2);
        }

        /*  x and y are non-zero finite  */
        if (ux & 0x80000000) {
            /*  x is negative */
            /*  Finite x < 0 */
            uint32_t yint = checkint (uy);
            if (yint == 0)      return (float)sqrt(x);
            if (yint == 1)      sign_bias = -1;

            ux &= 0x7fffffff; /*  x is negative, y is integer */
            x = asfloat(ux);

            return x * y;
        }

    }

    /* Get the exponent */
    k = (double)(ux >> EXPSHIFTBITS_SP32) - EMAX_SP32;

    /* Get mantissa, m is in [1/2 1) */
    mant = ux & MANTBITS_SP32;
    m = asfloat (mant |= HALFEXPBITS_SP32);

    /* Get the index of m in A */
    p = binarysearch(m);
    s1 = k - T[p].B;
    s2 = (m * T[p].inv_a) - 1.0;        /* s2 is in [2^(-1/C), 2^(1/C)] */

    /* Evaluate polynomial for s2 */
    s22 = s2*s2;
    A1 = L[0] + L[1] * s2;
    A2 = L[2] + L[3] * s2;
    A3 = A1 + s22 * A2;

    log2x = s1 + s2 * A3;

    w = (double)y * log2x;

    /* Split w into two parts i.e. w = w1+w2 */
    w1 =  trunc(C * w) * INVC;
    w2 = w - w1;
    iw1 = (int)trunc(C * w1);

    /* Set value of i based on iw1 */
    i = (iw1 < 0) ? 0 : 1;

    /* Check is w2 < 0 if not make it */
    if (w2 > 0) {
        w1 = w1 + INVC;
        w2 = w2 - INVC;   /* w2 lies in (-1/C,0] */
        iw1 = iw1 + 1;
    }

    mdash = (int)trunc(w1) + i;
    pdash = (unsigned int)((int)C * mdash - iw1);

    /* Find 2^w2 using polynomial evaluation */
    w22 = w2*w2;
    A1 = S[0] + S[1] * w2;
    A2 = S[2] + S[3] * w2;
    A3 = A1 + w22 * A2;
    poly = w2 * A3;
    z = T[pdash].A + T[pdash].A * poly;

    P = (mdash + 127) << 23;
    scale = asfloat((uint32_t)P);
    result = scale * z;

    return (float)(sign_bias * result);
}

strong_alias (__powf_finite, ALM_PROTO_FAST(powf))
weak_alias (amd_powf, ALM_PROTO_FAST(powf))
weak_alias (powf, ALM_PROTO_FAST(powf))
