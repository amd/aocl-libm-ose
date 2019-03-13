
#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_amd_paths.h>
#include <libm_special.h>

#if !defined(ENABLE_DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("O2")
#endif  /* !DEBUG */

#include <libm_macros.h>
#include <libm_types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

#define N 8
#define TABLE_SIZE (1ULL << N)
#define MAX_POLYDEGREE  8

#define HUGE    log_data.Huge

/*
 * Algorithm:
 *       With input 'X'
 *     1. Filter out exceptional cases
 *
 *     2. if e^(-1/16) < X < e^(1/16)
 *           then, f = X - 1, calcualate log(1+f) using PROCEEDURE2, and EXIT
 *	     otherwise, move to Step3
 *     3. Find uniq integer 'm' such that
 *		1 <= 2^-m . X <= 2  (i.e., 1 <= (1/2^m).X <= 2)
 *
 *		Determine 'f' and 'F' such that,
 *		 X = 2^m(F + f)
 *		 F = 1 + j.2^(-7)   j = 0,1,2,...,2^(7)
 *		 f <= 2^(-8)
 *
 *		Invoke PROCEEDURE1 to compute log(X)
 */

/*
 * Find T1 and T2, such that
 *	T1 is largest working-precision number smaller than e^(-1/16)
 *	T2 is the smallest working precision larger than e^(1/16)
 *
 *	  (-1/16)            (1/16)
 *	e^        <   X  <  e^
 *   can be
 *	T1 < X < T2
 */


#if N == 8
#define POLY_DEGREE 6
extern double log_table_256[];
extern double log_f_inv_256[];
#define TAB_F_INV log_f_inv_256
#define TAB_LOG   log_table_256
#define MANT_MASK_N  (0x000FF00000000000ULL)
#define MANT_MASK_N1 (0x0000080000000000ULL)
#elif N == 9
#define POLY_DEGREE 5
extern double log_table_512[];
extern double log_f_inv_512[];

#elif N == 10
#define POLY_DEGREE 4
extern double log_table_1024[];
extern double log_f_inv_1024[];
#define TAB_F_INV log_f_inv_1024
#define TAB_LOG   log_table_1024
#define MANT_MASK_N  (0x000FFC0000000000ULL)
#define MANT_MASK_N1 (0x0000020000000000ULL)
#endif

#define MANT_BITS_MASK          (TABLE_SIZE - 1)
#define MANT1_BITS_MASK         (1ULL << (N + 1))

static struct {
    double ALIGN(16) poly[MAX_POLYDEGREE];
    double_t ln2_lead, ln2_tail;
} log_data = {
#if N == 8
#endif
              //.ln2  = 0x1.62e42fefa39efp-1, /* ln(2) */
              .ln2_lead = 0x1.62e42e0000000p-1,
              .ln2_tail = 0x1.efa39ef35793cp-25,
              /*
               * Polynomial constants, 1/x! (reciprocal x)
               * To make better use of cache line,
               * we dont store 0! and 1!
               */
              .poly = {	/* skip for 0/1 and 1/1 */
                       0x1.0000000000000p-1,	/* 1/2 */
                       0x1.5555555555555p-2,	/* 1/3 */
                       0x1.0000000000000p-2,	/* 1/4 */
                       0x1.999999999999ap-3,	/* 1/5 */
                       0x1.5555555555555p-3,	/* 1/6 */
                       0x1.2492492492492p-3,	/* 1/7 */
                       0x1.0000000000000p-3,	/* 1/8 */
                       0x1.c71c71c71c71cp-4,	/* 1/9 */
              },
};

#define C2	log_data.poly[0]
#define C3	log_data.poly[1]
#define C4	log_data.poly[2]
#define C5	log_data.poly[3]
#define C6	log_data.poly[4]
#define C7	log_data.poly[5]
#define C8	log_data.poly[6]
#define LN2_LEAD log_data.ln2_lead
#define LN2_TAIL log_data.ln2_tail

struct log_table {
    double lead, tail;
};

double _log_special(double x, double y, uint32_t code);

static inline uint64_t top20(double x)
{
    /* 12 are the exponent bits */
    return asuint64(x) >> (64 - 20);
}

static inline uint64_t top12(double x)
{
    /* 12 are the exponent bits */
    return asuint64(x) >> (64 - 12);
}

/*
 * log(x) or ln(x)
 *	x is a quiet NaN, return x.
 *	x is a signaling NaN, raise the invalid operation exception and return a quiet NaN.
 *	If x = +inf, return x quietly.
 *	If x = +0 or -0, return -w, and raise the divide-by-zero exception.
 *	If x c 0, return a quiet NaN, and raise the invalid operation exception.
 *	If x = 1, return +O.
 */
double
FN_PROTOTYPE(log_v2)(double x)
{
    double_t q, r;
    double_t dexpo, j_times_half;
    uint64_t ux   = asuint64(x);
    uint64_t expo = top12(x);

#define FLAG_X_ZERO 0x1
#define FLAG_X_NEG  0x2
#define FLAG_X_NAN  0x3

    if (unlikely (expo >= 0x7ff)) {
        /* inf or nan */
        if (ux == 0x7ff0000000000000ULL)
            return x;

        /* -inf */
        if (isinf(x) == -1)
            return _log_special(x, asdouble(QNANBITPATT_DP64), FLAG_X_NEG);

        /* NaN */
        if (! (ux & QNANBITPATT_DP64))
            return _log_special(x, ux | QNANBITPATT_DP64 , FLAG_X_NAN);
    }

    if (unlikely (x <= 0.0))
        return _log_special(x, asdouble(QNANBITPATT_DP64), FLAG_X_NEG);

    flt64_t mant = {.i = ux & 0x000fffffffffffffULL};

    /* un-bias exponent  */
    expo -=  1023;

    /*
     * Denormals: Adjust mantissa,
     *          exponent is anyway 0 for subnormals
     */
    if (unlikely (expo == 0)) {
        mant.i |= 0x3ff0000000000000ULL;
        mant.d -= 1.0;
        uint64_t mant1 = mant.i >> 52;

        mant.i &= 0x000ffffffffffffULL;
        ux = mant.i;
        expo = cast_i64_to_double(mant1 - 2045);
    }

    uint64_t mant_n  = ux & MANT_MASK_N;
    uint64_t mant_n1 = ux & MANT_MASK_N1;

    /*
     * mant[52:52-N] + mant[52-N:52-N-1] << 1 (mistery)
     * not sure why...
     */
    uint64_t j = (mant_n + (mant_n1 << 1));

    dexpo = cast_i64_to_double(expo);

    /*****************
     * (x ~= 1.0) code path
     *****************/
    flt64_t one_minus_mant = {.d = x - 1.0};
    /* mask sign bit */
    uint64_t mant_without_sign = one_minus_mant.i & 0x7fffffffffffffffULL;
    if (unlikely (mant_without_sign < 0x3bf0000000000000LL)) {
        double_t r2, u, u2, u3, u7;
        static const double ca[5] = {
                       0.0,
                       0x1.55555555554e6p-4, // 1/2^2 * 3
                       0x1.9999999bac6d4p-7, // 1/2^4 * 5
                       0x1.2492307f1519fp-9, // 1/2^6 * 7
                       0x1.c8034c85dfff0p-12 // 1/2^8 * 9
        };

        /*
         * Less than threshold, no table lookup, no poly
         */
        r = one_minus_mant.d;

        r = one_minus_mant.d / (one_minus_mant.d + 2.0);

        q = r * one_minus_mant.d;       /* correction */

        r2 = r + r;

#define CA1 ca[1]
#define CA2 ca[2]
#define CA3 ca[3]
#define CA4 ca[4]

        u = r2 * r2;

        u2 = u * u;
        r = u2 * CA2 + u2 * CA4;

        u3 = u2 * u;
        r += u3 * CA1;

        u7 = u3 * (u2 * u2);
        r += u7 * CA3;

        q = r - q;

        return one_minus_mant.d + q;
    }

    mant.i        |= 0x3fe0000000000000ULL;               /* F */
    j_times_half   = asdouble(0x3fe0000000000000ULL | j); /* Y */

    j >>= (52 - N);

    /* f = F - Y */
    double_t f = j_times_half - mant.d;

    r = f * TAB_F_INV[j];

#define ESTRIN_SCHEME  0xee
#define HORNER_SCHEME  0xef

#define POLY_EVAL_METHOD HORNER_SCHEME

#ifndef POLY_EVAL_METHOD
#error "Polynomial evaluation method NOT defined"
#endif

#if POLY_EVAL_METHOD == HORNER_SCHEME
#if !defined(POLY_DEGREE)
#define POLY_DEGREE 6
#endif
#if POLY_DEGREE == 7
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * C7))))));
#elif   POLY_DEGREE == 6
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));
#elif POLY_DEGREE == 5
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));
#elif POLY_DEGREE == 4
    q = r * (1 + r * (C2 + r * (C3 + r * C4)));
#elif POLY_DEGREE == 3
    q = r * (1 + r * (C2 + r * C3));
#else  /* Poly order <=2 */
    q = r * (1 + r * C2);
#endif	/* Order <=2 && Order == 3 */

#elif POLY_EVAL_METHOD == ESTRIN_SCHEME
    /* Estrin's */
    // r + ((r*r)*(1/2 + (r*1/6))) +
    // ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

    r2 = r * r; 			/* r^2 */
    q = r + (r2 * (C2  + r * C3));

#if POLY_DEGREE == 4
    q += (r2 * r2) *  C4; /* r^4 * C4 */
#elif POLY_DEGREE == 5
    q += (r2 * r2) * (C4 + r*C5);
#elif POLY_DEGREE == 6
    q += (r2 * r2) * (C4 + r * (C5 + r*C6));
#endif
#else
#warning "POLY_EVAL_METHOD is not defined"
#endif  /* if HORNER_SCHEME || ESTRIN_SCHEME */


    struct log_table *tb_entry = &((struct log_table*)TAB_LOG)[j];

    /* m*log(2) + log(G) - poly */

    q  = (dexpo * LN2_TAIL) - q;
    q += tb_entry->tail;

    q += (dexpo * LN2_LEAD) + tb_entry->lead;

    return q;
}

#if !defined(ENABLE_DEBUG)
#pragma GCC pop_options
#endif
