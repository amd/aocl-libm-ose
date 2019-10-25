/* Contains implementation of float logf(float x)
 * x = 2^m(1+A)
 * x = 2^m*(1 + (G + g)) where 1 <= 1+G < 2 and g< 2^(-8)
 * Let F = 1+G and f = g
 * x = 2^m*(F + f), so 1 <= F < 2, f < 2^(-8)
 * x = (2^m) * 2 * (F/2+f/2)
 * So, 0.5 <= F/2 < 1,   f/2 < 2^-9
 * logf(X) = logf(2^m) + logf(2) + logf(F/2 + f/2)
 * logf(x) = logf(2^m) + logf(2) + logf(F/2*(1 + f/F))
 * logf(x) = logf(2^m) + logf(2) + logf(F/2*(1 + f/F))
 * logf(x) = logf(2^m) + logf(2) + logf(F/2) + logf(1 + f/F)
 * logf(x) = logf(2^m) + logf(F) + logf(1 + f/F)
 * logf(x) = m*logf(2) + logf(F) + logf(1 + r) where r = 1+f/F
 */

#include <stdint.h>
#include "libm_amd.h"
#include <math.h>
#include <float.h>
#include "libm_util_amd.h"
#include "libm_special.h"
#include "libm_inlines_amd.h"
#include "libm_errno_amd.h"
#include <libm/typehelper.h>
#include <libm/compiler.h>

#define MASK_MANT_ALL7 0x007f0000
#define MASK_MANT8 0x00008000
#define MASK_SIGN 0x7FFFFFFF
#define N 7
#define TABLE_SIZE (1ULL << N)
#define NEAR_ONE_THRESHOLD 0x3d800000

struct logf_table {
    uint32_t f_inv, f_128_head, f_128_tail;
};

static struct {
    float_t poly_logf[4];
    float_t L__real_log2_head, L__real_log2_tail;
} logf_data = {
              .L__real_log2_head  = 6.93115234375E-1,
              .L__real_log2_tail  = 3.19461832987E-5,
              /*
               * Polynomial constants, 1/x! (reciprocal x)
               */
              .poly_logf = { /* skip for 0/1 and 1/1 */
                       0.5f, /* 1/2 */
                       3.333333432674407958984375E-1f, /* 1/3 */
                       8.33333333333317923934e-02f,
                       1.25000000037717509602e-02,
              },
};

extern struct logf_table logf_lookup[];

#define C1 logf_data.poly_logf[0]
#define C2 logf_data.poly_logf[1]
#define C3 logf_data.poly_logf[2]
#define C4 logf_data.poly_logf[3]

#define LOG2_HEAD logf_data.L__real_log2_head
#define LOG2_TAIL logf_data.L__real_log2_tail

float FN_PROTOTYPE_OPT(logf)(float x)
{

	float_t y, f, finv, f1, r, rsquare, r2, w, result;
    float_t a0, temp;
    uint32_t ux;

    ux = asuint32(x);

    int32_t exponent = (ux >> EXPSHIFTBITS_SP32) - EMAX_SP32;

    float_t exp_float = (float_t)exponent;

    uint32_t mant = ux & MANTBITS_SP32;

    if (unlikely (ux - 0x00800000 >= 0x7f800000 - 0x00800000))
    {
         /* x < 0x1p-126 or inf or nan. */
        if (ux * 2 == 0)
            return -1.0/0.0; /* log(0) = -inf */

        if (ux == 0x7f800000) /* log(inf) = inf */
	        return x;

        if ((ux & 0x80000000) || ux * 2 >= 0xff000000)
	        return sqrt(x); /* Return NaN */

        /* x has to be denormal */
        float_t d = asfloat(((ux &0x007FFFFF) | 0x3f800000));

        d = d - 1.0f;

        ux = asuint32(d);

        exponent = (ux >> EXPSHIFTBITS_SP32) - 253;

        exp_float = (float)exponent;

        mant = ux & MANTBITS_SP32;
    }

	temp = x - 1.0f;

    uint32_t mant1 = (ux & MASK_MANT_ALL7);

    mant1 += ((ux & MASK_MANT8) << 1);

    uint32_t t1 = asuint32(temp);

    if (unlikely((t1 & MASK_SIGN) <= NEAR_ONE_THRESHOLD)) {
        /* x is near one (0.9735 < x < 1.0625) */
        r = x - 1.0f;

        w = r / (2.0f + r);

        float_t correction = w * r;

        w += w;

        float_t v = w * w;

        r2 = (w  * v * (C3 + C4 * v)) - correction;

        result = r2 + r;

        return result;
    }

    uint32_t index = mant1 >> (EXPSHIFTBITS_SP32 - N);

    y = asfloat(mant |= HALFEXPBITS_SP32);
    f = asfloat(mant1 |= HALFEXPBITS_SP32);
    f1 = f - y;
    finv = asfloat(logf_lookup[index].f_inv);
    r = f1 * finv;
    a0 = C1 + C2 * r;
    rsquare = r * r;

    float_t poly = r + rsquare * a0;

    float_t t = (exp_float * LOG2_TAIL) - poly;

    w = (LOG2_HEAD * exp_float) + asfloat(logf_lookup[index].f_128_tail);

    result = (asfloat(logf_lookup[index].f_128_head) + t) + w;

    return result;
}

