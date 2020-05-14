/*
 * Copyright (C) 2018-2019, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <libm_util_amd.h>
#include <libm_special.h>

#include <libm_macros.h>
#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <emmintrin.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */
#include <libm/poly-vec.h>

#define VRS4_LOGF_POLY_DEGREE 10

#define VRS4_LOGF_MAX_POLY_SIZE 12

static const struct {
    v_i32x4_t v_min,
        v_max,
        v_mask,
        v_off;
    v_f32x4_t v_one;
    v_f32x4_t ln2;
    v_f32x4_t poly[VRS4_LOGF_MAX_POLY_SIZE];
} v_logf_data = {
    .v_min  = _MM_SET1_I32(0x00800000),
    .v_max  = _MM_SET1_I32(0x7f800000),
    .v_mask = _MM_SET1_I32(MANTBITS_SP32),
    .v_off  = _MM_SET1_I32(0x3f2aaaab),
    .v_one  = _MM_SET1_PS4(1.0f),
    .ln2    = _MM_SET1_PS4(0x1.62e43p-1f), /* ln(2) */
    .poly = {
            _MM_SET1_PS4(0.0f),
            _MM_SET1_PS4(0x1.0p0f),      /* 1.0 */
#if VRS4_LOGF_POLY_DEGREE == 7
            _MM_SET1_PS4(-0x1.000478p-1f),
            _MM_SET1_PS4(0x1.556906p-2f),
            _MM_SET1_PS4(-0x1.fc0968p-3f),
            _MM_SET1_PS4(0x1.93bf0cp-3f),
            _MM_SET1_PS4(-0x1.923814p-3f),
            _MM_SET1_PS4(0x1.689e5ep-3f),
#elif VRS4_LOGF_POLY_DEGREE == 8
            _MM_SET1_PS4(-0x1.ffff9p-2f),
            _MM_SET1_PS4(0x1.5561aep-2f),
            _MM_SET1_PS4(-0x1.002598p-2f),
            _MM_SET1_PS4(0x1.952ef4p-3f),
            _MM_SET1_PS4(-0x1.4dfa02p-3f),
            _MM_SET1_PS4(0x1.6023a8p-3f),
            _MM_SET1_PS4(-0x1.460368p-3f),
#elif VRS4_LOGF_POLY_DEGREE == 10
            _MM_SET1_PS4(-0x1.000004p-1f),
            _MM_SET1_PS4(0x1.55545ep-2f),
            _MM_SET1_PS4(-0x1.fffbp-3f),
            _MM_SET1_PS4(0x1.99fc74p-3f),
            _MM_SET1_PS4(-0x1.56061ep-3f),
            _MM_SET1_PS4(0x1.1cba2ap-3f),
            _MM_SET1_PS4(-0x1.ea79f2p-4f),
            _MM_SET1_PS4(0x1.26f1fep-3f),
            _MM_SET1_PS4(-0x1.17b714p-3f),
#else
#error "Unknown Polynomial degree"
#endif
    },
};


#define V_MIN   v_logf_data.v_min
#define V_MAX   v_logf_data.v_max
#define V_MASK  v_logf_data.v_mask
#define V_OFF   v_logf_data.v_off
#define V_ONE   v_logf_data.v_one
#define LN2     v_logf_data.ln2


/*
 * Short names for polynomial coefficients
 */
#define C0       v_logf_data.poly[0]
#define C1       v_logf_data.poly[1]
#define C2       v_logf_data.poly[2]
#define C3       v_logf_data.poly[3]
#define C4       v_logf_data.poly[4]
#define C5       v_logf_data.poly[5]
#define C6       v_logf_data.poly[6]
#define C7       v_logf_data.poly[7]
#define C8       v_logf_data.poly[8]
#define C9       v_logf_data.poly[9]
#define C10      v_logf_data.poly[10]
#define C11      v_logf_data.poly[11]

#define V_I32(x) x.i32x4
#define V_F32(x) x.f32x4

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float logf(float x)
 *
 * Spec:
 *   logf(x)
 *          = logf(x)           if x ∈ F and x > 0
 *          = x                 if x = qNaN
 *          = 0                 if x = 1
 *          = -inf              if x = (-0, 0}
 *          = NaN               otherwise
 *
 * Assumptions/Expectations
 *      - ULP is derived to be << 4 (always)
 *	- Some FPU Exceptions may not be available
 *      - Performance is at least 3x
 *
 * Implementation Notes:
 *  1. Range Reduction:
 *      x = 2^n*(1+f)                                          .... (1)
 *         where n is exponent and is an integer
 *             (1+f) is mantissa ∈ [1,2). i.e., 1 ≤ 1+f < 2    .... (2)
 *
 *    From (1), taking log on both sides
 *      log(x) = log(2^n * (1+f))
 *             = log(2^n) + log(1+f)
 *             = n*log(2) + log(1+f)                           .... (3)
 *
 *      let z = 1 + f
 *             log(z) = log(k) + log(z) - log(k)
 *             log(z) = log(kz) - log(k)
 *
 *    From (2), range of z is [1, 2)
 *       by simply dividing range by 'k', z is in [1/k, 2/k)  .... (4)
 *       Best choice of k is the one which gives equal and opposite values
 *       at extrema        +-      -+
 *              1          | 2      |
 *             --- - 1 = - |--- - 1 |
 *              k          | k      |                         .... (5)
 *                         +-      -+
 *
 *       Solving for k, k = 3/2,
 *    From (4), using 'k' value, range is therefore [-0.3333, 0.3333]
 *
 *  2. Polynomial Approximation:
 *     More information refer to tools/sollya/vrs4_logf.sollya
 *
 *     7th Deg -   Error abs: 0x1.04c4ac98p-22   rel: 0x1.2216e6f8p-19
 *     6th Deg -   Error abs: 0x1.179e97d8p-19   rel: 0x1.db676c1p-17
 *
 */


static inline v_f32x4_t
logf_specialcase(v_f32x4_t _x,
                 v_f32x4_t result,
                 v_i32x4_t cond)
{
    return v_call_f32(FN_PROTOTYPE(logf), _x, result, cond);
}


v_f32x4_t
FN_PROTOTYPE_OPT(vrs4_logf)(v_f32x4_t _x)
{
    v_f32x4_t q, r, n;

    v_32x4 vx = {.f32x4 = _x};

    v_i32x4_t cond = (vx.i32x4 - V_MIN >= V_MAX - V_MIN);

    vx.i32x4 -= V_OFF;

    n = v4_to_f32_s32(vx.i32x4 >> 23);

    vx.i32x4 &= V_MASK;

    vx.i32x4 += V_OFF;

    r = vx.f32x4 - V_ONE;

#if VRS4_LOGF_POLY_DEGREE == 7
    /* n*ln2 + r + r2*(C1, + r*C2 + r2*(C3 + r*C4 + r2*(C5 + r*C6 + r2*(C7)))) */
    q = POLY_EVAL_7(r, C0, C1, C2, C3, C4, C5, C6, C7);
#elif VRS4_LOGF_POLY_DEGREE == 8
    q = POLY_EVAL_8(r, C0, C1, C2, C3, C4, C5, C6, C7, C8);
#elif VRS4_LOGF_POLY_DEGREE == 10
    q = POLY_EVAL_10(r, C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10);
#endif

    q = n * LN2 + q;

    if (unlikely(v4_any_u32_loop(cond))) {
        return logf_specialcase(_x, q, cond);
    }

    return q;
}


