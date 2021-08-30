/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   v_f32x8_t vrs8_coshf(v_f32x_t x)
 *
 * Spec:
 *   coshf(|x| > 89.415985107421875) = Infinity
 *   coshf(Infinity)  = infinity
 *   coshf(-Infinity) = infinity
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * cosh(x) = (exp(x) + exp(-x))/2
 * cosh(-x) = +cosh(x)
 *
 * checks for special cases
 * if ( asint(x) > infinity) return x with overflow exception and
 * return x.
 * if x is NaN then raise invalid FP operation exception and return x.
 *
 *  coshf = v/2 * exp(x - log(v)) where v = 0x1.0000e8p-1
 *
 */

#include <stdint.h>
#include <emmintrin.h>
#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct {
    v_u32x8_t arg_max, sign_mask;
    v_f32x8_t logV, invV2, halfV;
    v_f32x8_t one;
 } v8_coshf_data = {
        .sign_mask = _MM256_SET1_PS8(0x7FFFFFFF),
        .arg_max = _MM256_SET1_PS8(0x42B2D4FC),
        .logV = _MM256_SET1_PS8(0x1.62e6p-1f),
        .invV2 = _MM256_SET1_PS8(0x1.fffc6p-3f),
        .halfV = _MM256_SET1_PS8(0x1.0000e8p0f),
        .one = _MM256_SET1_PS8(1.0f),
};

#define SIGN_MASK v8_coshf_data.sign_mask
#define LOGV      v8_coshf_data.logV
#define INVV2     v8_coshf_data.invV2
#define ONE       v8_coshf_data.one
#define HALFVM1   v8_coshf_data.halfVm1
#define HALFV     v8_coshf_data.halfV
#define ARG_MAX   v8_coshf_data.arg_max

v_f32x8_t ALM_PROTO(vrs8_expf)(v_f32x8_t);
float ALM_PROTO(coshf)(float);

static inline v_f32x8_t
coshf_specialcase(v_f32x8_t _x, v_f32x8_t result, v_i32x8_t cond)
{
    return call_v8_f32(ALM_PROTO(coshf), _x, result, cond);
}


v_f32x8_t ALM_PROTO_OPT(vrs8_coshf)(v_f32x8_t x)
{

    v_f32x8_t z, y, result;

    v_u32x8_t ux = as_v8_u32_f32(x) & SIGN_MASK;

    y = as_v8_f32_u32(ux);

    v_u32x8_t cond = ux > ARG_MAX;

    z = ALM_PROTO(vrs8_expf)(y - LOGV);

    result = HALFV * (z + INVV2 * ONE / z);

    if(unlikely(any_v8_u32_loop(cond))) {

        return coshf_specialcase(x, result, cond);

    }

    return result;

}




