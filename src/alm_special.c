/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/types.h>

/*#if defined(WIN64) | defined(WINDOWS)
  #else            */
static inline void __amd_raise_fp_exc(int flags)
{
    if ((flags & AMD_F_UNDERFLOW) == AMD_F_UNDERFLOW) {
        double a = 0x1.0p-1022;
        __asm __volatile("mulsd %1, %0":"+x"(a):"x"(a));
    }
    if ((flags & AMD_F_OVERFLOW) == AMD_F_OVERFLOW) {
        double a = 0x1.fffffffffffffp1023;
	__asm __volatile("mulsd %1, %0":"+x"(a):"x"(a));
    }
    if ((flags & AMD_F_DIVBYZERO) == AMD_F_DIVBYZERO) {
        double a = 1.0, b = 0.0;
        __asm __volatile("divsd %1, %0":"+x"(a):"x"(b));
    }
    if ((flags & AMD_F_INVALID) == AMD_F_INVALID) {
        double a = 0.0;
        __asm __volatile("divsd %1, %0":"+x"(a):"x"(a));
    }
}

double __alm_handle_error(uint64_t value, int flags)
{
    double z;

    PUT_BITS_DP64(value, z);
    __amd_raise_fp_exc(flags);
    return z;
}

float __alm_handle_errorf(uint64_t value, int flags)
{
    float z;

    PUT_BITS_SP32(value, z);
    __amd_raise_fp_exc(flags);
    return z;
}
/*#endif*/


double alm_log_special(double y, U32 error_code) {
    flt64_t ym = {.d = y};

    switch (error_code) {
    case ALM_E_IN_X_INF:
        __alm_handle_error(ym.u, AMD_F_INVALID);
        break;

    case ALM_E_IN_X_ZERO:
        __alm_handle_error(ym.u, AMD_F_NONE);
        break;

    case ALM_E_IN_X_NEG:
        __alm_handle_error(ym.u, AMD_F_INVALID);
        break;

    case ALM_E_IN_X_NAN:
        __alm_handle_error(ym.u, AMD_F_INVALID);
        break;

    case ALM_E_DIV_BY_ZER0:
        __alm_handle_error(ym.u, AMD_F_DIVBYZERO);
        break;

    case ALM_E_OUT_ZERO:
        default:
        break;
  }

    return y;
}

float
alm_acosf_special(float x, uint32_t code)
{
    flt32_t fl = {.f = x};
    if (code == ALM_E_IN_X_NAN)
    {
        /* Return invalid if it's a NaN */
        if (fl.u & QNAN_MASK_32)
            return __alm_handle_errorf(fl.u|0x00400000,
                                       AMD_F_NONE);
        else
            return  __alm_handle_errorf(fl.u|0x00400000,
                                        AMD_F_INVALID
                                        );
    }
    else
        return  __alm_handle_errorf(INDEFBITPATT_SP32,
                                    AMD_F_INVALID);
}

double
alm_acos_special(double x, uint32_t code)
{
    flt64_t fl = {.d = x};
    if (code == ALM_E_IN_X_NAN)
    {
        /* Return invalid if it's a NaN */
        if (fl.u & QNAN_MASK_64)
            return __alm_handle_error(fl.u|QNAN_MASK_64,
                                       AMD_F_NONE);
        else
            return  __alm_handle_error(fl.u|QNAN_MASK_64,
                                        AMD_F_INVALID
                                        );
    }
    else
        return  __alm_handle_error(INDEFBITPATT_DP64,
                                   AMD_F_INVALID);
}

float
alm_asinf_special(float x, uint32_t code)
{
    flt32_t fl = {.f = x};
    if (code == ALM_E_IN_X_NAN)
    {
        /* Return invalid if it's a NaN */
        if (fl.u & QNAN_MASK_32)
            return __alm_handle_errorf(fl.u|0x00400000,
                                       AMD_F_NONE);
        else
            return  __alm_handle_errorf(fl.u|0x00400000,
                                        AMD_F_INVALID
                                        );
    }
    else
        return  __alm_handle_errorf(INDEFBITPATT_SP32,
                                    AMD_F_INVALID);
}

float _atanf_special_overflow(float x)
{
    UT32 xu;
    xu.f32 = x;
    return __alm_handle_errorf(xu.u32, AMD_F_OVERFLOW);
}

double
alm_atan_special(double x) {
    flt64_t fl = {.d = x};
    return __alm_handle_error(fl.u, AMD_F_INVALID);
}

double
alm_asin_special(double x, uint32_t code)
{
    flt64_t fl = {.d = x};
    if (code == ALM_E_IN_X_NAN)
    {
        /* Return invalid if it's a NaN */
        if (fl.u & QNAN_MASK_64)
            return __alm_handle_error(fl.u|QNAN_MASK_64,
                                       AMD_F_NONE);
        else
            return  __alm_handle_error(fl.u|QNAN_MASK_64,
                                        AMD_F_INVALID);
    }
    else
        return  __alm_handle_error(INDEFBITPATT_DP64,
                                    AMD_F_INVALID);
}


float _sinf_cosf_special(float x, char *name, uint32_t code)
{
    UT32 xu;
    xu.f32 = x;
    if ((xu.u32 & EXPBITS_SP32) == EXPBITS_SP32) {
        // x is Inf or NaN
        if ((xu.u32 & MANTBITS_SP32) == 0x0) {
            // x is Inf
            xu.u32 = INDEFBITPATT_SP32;
            return __alm_handle_errorf(xu.u32, AMD_F_INVALID);
        } else {
            // x is NaN
#if defined(WINDOWS)
            return __alm_handle_errorf(xu.u32 | QNAN_MASK_32, 0);

#else               /*  */
            if (xu.u32 & QNAN_MASK_32)
                return __alm_handle_errorf(xu.u32, 0);

            else
                return __alm_handle_errorf(xu.u32 | QNAN_MASK_32, AMD_F_INVALID);

#endif              /*  */
        }
    }
    return xu.f32;
}


double _sincos_special_underflow(double x, char *name, uint32_t code)
{
    UT64 xu;
    xu.f64 = x;
    return __alm_handle_error(xu.u64,
                  AMD_F_UNDERFLOW | AMD_F_INEXACT);
}

float _sinf_cosf_special_underflow(float x, char *name, uint32_t code)
{
    UT32 xu;
    xu.f32 = x;
    return __alm_handle_errorf(xu.u32,
                    AMD_F_UNDERFLOW | AMD_F_INEXACT);
}

double _sin_cos_special(double x, char *name, uint32_t code)
{
    UT64 xu;
    xu.f64 = x;
    if ((xu.u64 & EXPBITS_DP64) == EXPBITS_DP64) {

        // x is Inf or NaN
        if ((xu.u64 & MANTBITS_DP64) == 0x0) {

            // x is Inf
            xu.u64 = INDEFBITPATT_DP64;
            return __alm_handle_error(xu.u64, AMD_F_INVALID);
        } else {

            // x is NaN
#if defined(WINDOWS)
            return __alm_handle_error(xu.u64 | QNAN_MASK_64, 0);

#else               /*  */
            if (xu.u64 & QNAN_MASK_64)
                return __alm_handle_error(xu.u64, 0);

            else
                return __alm_handle_error(xu.u64 | QNAN_MASK_64,
                              AMD_F_INVALID);

#endif              /*  */
        }
    }
    return xu.f64;
}

void _sincosf_special(float x, float *sy, float *cy)
{
    float xu = _sinf_cosf_special(x, "sincosf", __amd_sin);
    *sy = xu;
    *cy = xu;
    return;
}

void _sincos_special(double x, double *sy, double *cy)
{
    double xu = _sin_cos_special(x, "sincos", __amd_sin);
    *sy = xu;
    *cy = xu;
    return;
}

/* trig functions */
double _sin_special_underflow(double x)
{
    return _sincos_special_underflow(x, "sin", __amd_sin);
}

float _sinf_special(float x)
{
    return _sinf_cosf_special(x, "sinf", __amd_sin);
}

double _sin_special(double x)
{
    return _sin_cos_special(x, "sin", __amd_sin);
}

float _cosf_special(float x)
{
    return _sinf_cosf_special(x, "cosf",__amd_cos);
}

double _cos_special(double x)
{
    return _sin_cos_special(x, "cos",__amd_cos);
}

double _tan_special(double x)
{
    return _sin_cos_special(x, "tan", __amd_tan);
}

float _tanf_special(float x)
{

    UT32 xu = {.f32 = x};
    if ((xu.u32 & ~SIGNBIT_SP32) < 0x39000000) {
        __alm_handle_errorf(xu.u32, AMD_F_UNDERFLOW);
    }
    return _sinf_cosf_special(x, "tanf", __amd_tan);
}

float _tanhf_special(float x)
{
    UT32 xu = {.f32 = x};
    return __alm_handle_errorf(xu.u32, AMD_F_INEXACT|AMD_F_UNDERFLOW);
}

/*fabs*/
double _fabs_special(double x)
{
    UT64 xu;
    xu.f64 = x;

    // x is NaN
#ifdef WINDOWS          //
    return __alm_handle_error(xu.u64 | QNAN_MASK_64, 0);

#else               /*  */
    if (xu.u64 & QNAN_MASK_64)
        return __alm_handle_error(xu.u64 | QNAN_MASK_64, AMD_F_NONE);

    else
        return __alm_handle_error(xu.u64 | QNAN_MASK_64, AMD_F_INVALID);

#endif              /*  */
}

float _fabsf_special(float x)
{
    UT32 xu;
    xu.f32 = x;

    // x is NaN
#ifdef WINDOWS          //
    return __alm_handle_errorf(xu.u32 | QNAN_MASK_32, 0);

#else               /*  */
    if (xu.u32 & QNAN_MASK_32)
        return __alm_handle_errorf(xu.u32 | QNAN_MASK_32, AMD_F_NONE);

    else
        return __alm_handle_errorf(xu.u32 | QNAN_MASK_32, AMD_F_INVALID);

#endif              /*  */
}

double _cbrt_special(double x)
{
    UT64 xu;
    xu.f64 = x;

    // x is NaN
    return __alm_handle_error(xu.u64 | QNAN_MASK_64, 0);
}

float _cbrtf_special(float x)
{
    UT32 xu;
    xu.f32 = x;

    // x is NaN
    return __alm_handle_errorf(xu.u32 | QNAN_MASK_32, 0);
}


/* exp, log, pow*/
float alm_expf_special(float y, U32 code) {
    flt32_t ym = {.f = y};

    switch (code) {
    case ALM_E_IN_X_NAN:
        __alm_handle_errorf(ym.u, 0);
        break;

    case ALM_E_IN_X_ZERO:
        __alm_handle_errorf(ym.u,
                            AMD_F_INEXACT | AMD_F_UNDERFLOW);
        break;

    case ALM_E_IN_X_INF:
        __alm_handle_errorf(ym.u,
                            AMD_F_INEXACT | AMD_F_OVERFLOW);
        break;

    default:
        break;
    }

    return y;
}

double alm_exp_special(double y, U32 code) {
    flt64_t ym = {.d = y};

    switch (code) {
    case ALM_E_IN_X_NAN:
        __alm_handle_error(ym.u, 0);
        break;
    case ALM_E_IN_X_ZERO:
        __alm_handle_error(ym.u, AMD_F_INEXACT | AMD_F_UNDERFLOW);
        break;
    case ALM_E_IN_X_INF:
        __alm_handle_error(ym.u, AMD_F_INEXACT | AMD_F_OVERFLOW);
        break;
    default:
        break;
    }
    return y;
}


/* pow */
#define POW_X_ONE_Y_SNAN            1
#define POW_X_ZERO_Z_INF            2
#define POW_X_NAN                   3
#define POW_Y_NAN                   4
#define POW_X_NAN_Y_NAN             5
#define POW_X_NEG_Y_NOTINT          6
#define POW_Z_ZERO                  7
#define POW_Z_DENORMAL              8
#define POW_Z_INF                   9

double alm_pow_special(double z, U32 code) {
    flt64_t zu = {.d = z};

    switch (code) {
    case POW_X_ONE_Y_SNAN:
        __alm_handle_error(zu.u, AMD_F_INVALID);
        break;
    case POW_X_ZERO_Z_INF:
        __alm_handle_error(zu.u, AMD_F_DIVBYZERO);
        break;
    case POW_X_NAN:
    case POW_Y_NAN:
    case POW_X_NAN_Y_NAN:
        __alm_handle_error(zu.u, AMD_F_INVALID);
        break;
    case POW_X_NEG_Y_NOTINT:
         __alm_handle_error(zu.u, AMD_F_INVALID);
        break;
    case POW_Z_ZERO:
    case POW_Z_DENORMAL:
        __alm_handle_error(zu.u, AMD_F_INEXACT | AMD_F_UNDERFLOW);
        break;
    case POW_Z_INF:
        __alm_handle_error(zu.u, AMD_F_INEXACT | AMD_F_OVERFLOW);
        break;
    default:
        break;
    }
    return z;
}

float alm_powf_special(float z, U32 code) {
    flt32_t zu = {.f = z};

    switch (code) {
    case POW_X_ONE_Y_SNAN:
        __alm_handle_errorf(zu.u, AMD_F_INVALID);
        break;
    case POW_X_ZERO_Z_INF:
        __alm_handle_errorf(zu.u, AMD_F_DIVBYZERO);
        break;
    case POW_X_NAN:
    case POW_Y_NAN:
    case POW_X_NAN_Y_NAN:
        __alm_handle_errorf(zu.u, AMD_F_INVALID);
        break;
    case POW_X_NEG_Y_NOTINT:
        __alm_handle_errorf(zu.u, AMD_F_INVALID);
        break;
    case POW_Z_ZERO:
        __alm_handle_errorf(zu.u, AMD_F_INEXACT | AMD_F_UNDERFLOW);
        break;
    case POW_Z_INF:
        __alm_handle_errorf(zu.u, AMD_F_INEXACT | AMD_F_OVERFLOW);
        break;
    default:
        break;
    }
    return z;
}

/*log*/
#define LOG_X_ZERO      1
#define LOG_X_NEG       2
#define LOG_X_NAN       3
float alm_logf_special(float y, U32 errorCode) {
    flt32_t ym = {.f = y};

    switch (errorCode) {
    case ALM_E_IN_X_ZERO:
        __alm_handle_errorf(ym.u, AMD_F_DIVBYZERO);
        break;
    case ALM_E_IN_X_NEG:
        __alm_handle_errorf(ym.u, AMD_F_INVALID);
        break;
    case ALM_E_DIV_BY_ZER0:
        __alm_handle_errorf(ym.u, AMD_F_DIVBYZERO);
        break;
    case ALM_E_IN_X_NAN:
        {
#ifdef WIN64
        __alm_handle_errorf(ym.u, AMD_F_NONE);
#else               /*  */
        return y + y;
#endif              /*  */
        }
        break;
    default:
        break;
    }
    return y;
}

/* coshf */
float alm_coshf_special(float y, U32 errorCode) {
    flt32_t ym = {.f = y};

    switch (errorCode) {
    case ALM_E_OVERFLOW:
        __alm_handle_errorf(ym.u, AMD_F_OVERFLOW);
        break;
    default:
        break;
    }
    return y;
}

double _nearbyint_special(double x)
{
    UT64 checkbits;
    checkbits.f64 = x;

    /* take care of nan or inf */
    if ((checkbits.u64 & EXPBITS_DP64) == EXPBITS_DP64) {
        if ((checkbits.u64 & MANTBITS_DP64) == 0x0) {
            // x is Inf
#ifdef WINDOWS
            return __alm_handle_error(checkbits.u64, AMD_F_INVALID);
#else               /*  */
            return __alm_handle_error(checkbits.u64, AMD_F_NONE);
#endif              /*  */
        }
        else {
#ifdef WINDOWS
            return __alm_handle_error(checkbits.u64 | QNAN_MASK_64, 0);
#else               /*  */
            if (checkbits.u64 & QNAN_MASK_64)
                return __alm_handle_error(checkbits.u64 | QNAN_MASK_64, AMD_F_NONE);
            else
                return __alm_handle_error(checkbits.u64 | QNAN_MASK_64, AMD_F_INVALID);
#endif              /*  */
        }
    }
    else
        return x;
}

float _truncf_special(float x, float r)
{
    UT64 rm;
    rm.u64 = 0;
    rm.f32[0] = r;
    __alm_handle_errorf(rm.u64, 0);
    return r;
}

double _trunc_special(double x, double r)
{
    UT64 rm;
    rm.f64 = r;
    __alm_handle_error(rm.u64, 0);
    return r;
}

double _round_special(double x, double r)
{
    UT64 rm;
    rm.f64 = r;
    __alm_handle_error(rm.u64, 0);
    return r;
}

float _fdimf_special(float x, float y, float r)
{
    UT64 rm;
    rm.u64 = 0;
    rm.f32[0] = r;
    __alm_handle_errorf(rm.u64, 0);
    return r;
}

double _fdim_special(double x, double y, double r)
{
    UT64 rm;
    rm.f64 = r;
    __alm_handle_error(rm.u64, 0);
    return r;
}

double _fmax_special(double x, double y)
{
    UT64 xu, yu;
    xu.f64 = x;
    yu.f64 = y;
    if ((xu.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64) {
        if ((yu.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64)
#ifdef WINDOWS
            return __alm_handle_error(yu.u64 | QNAN_MASK_64, 0);

#else               /*  */
            return __alm_handle_error(yu.u64 | QNAN_MASK_64, AMD_F_INVALID);

#endif              /*  */
        else
            return __alm_handle_error(yu.u64, 0);
    }
    return __alm_handle_error(xu.u64, 0);
}

float _fmaxf_special(float x, float y)
{
    UT32 xu, yu;
    xu.f32 = x;
    yu.f32 = y;
    if ((xu.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32) {
        if ((yu.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32)
#ifdef WINDOWS
            return __alm_handle_errorf(yu.u32 | QNAN_MASK_32, 0);

#else               /*  */
            return __alm_handle_errorf(yu.u32 | QNAN_MASK_32, AMD_F_INVALID);

#endif              /*  */
        else
            return __alm_handle_errorf(yu.u32, 0);
    }
    return __alm_handle_errorf(xu.u32, 0);
}

double _fmin_special(double x, double y)
{
    UT64 xu, yu;
    xu.f64 = x;
    yu.f64 = y;
    if ((xu.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64) {
        if ((yu.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64)
#ifdef WINDOWS
            return __alm_handle_error(yu.u64 | QNAN_MASK_64, 0);

#else               /*  */
            return __alm_handle_error(yu.u64 | QNAN_MASK_64, AMD_F_INVALID);

#endif              /*  */
        else
            return __alm_handle_error(yu.u64, 0);
    }
    return __alm_handle_error(xu.u64, 0);
}

float _fminf_special(float x, float y)
{
    UT32 xu, yu;
    xu.f32 = x;
    yu.f32 = y;
    if ((xu.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32) {
        if ((yu.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32)
#ifdef WINDOWS
            return __alm_handle_errorf(yu.u32 | QNAN_MASK_32, 0);

#else               /*  */
            return __alm_handle_errorf(yu.u32 | QNAN_MASK_32, AMD_F_INVALID);

#endif              /*  */
        else
            return __alm_handle_errorf(yu.u32, 0);
    }
    return __alm_handle_errorf(xu.u32, 0);
}

#define REMAINDER_X_NAN       1
#define REMAINDER_Y_ZERO      2
#define REMAINDER_X_DIVIDEND_INF      3
float _remainderf_special(float x, float y, U32 errorCode)
{
    switch (errorCode) {
        /*All the three conditions are considered to be the same
           for Windows. It might be different for Linux.
           May have to come back for linux/WIndows. */
    case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
            UT32 Y;
            Y.f32 = y;
            return __alm_handle_errorf(Y.u32, AMD_F_INVALID);
        }
        break;
    case REMAINDER_X_NAN:
        {
#ifdef WINDOWS
            UT32 Y;
            Y.f32 = y;
            __alm_handle_errorf(Y.u32, AMD_F_INVALID);
#else               /*  */
            return x + x;
#endif              /*  */
        }
        break;
    default:
        break;
    }
    return y;
}

double _remainder_special(double x, double y, U32 errorCode)
{
    switch (errorCode) {
        /*All the three conditions are considered to be the same
           for Windows. It might be different for Linux.
           May have to come back for linux/WIndows. */
    case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
            UT64 Y;
            Y.f64 = y;
            __alm_handle_error(Y.u64, AMD_F_INVALID);
        }
        break;
    case REMAINDER_X_NAN:
        {
#ifdef WINDOWS
            UT64 Y;
            Y.f64 = y;
            __alm_handle_error(Y.u64, AMD_F_INVALID);
#else               /*  */
            return x + x;
#endif              /*  */
        }
        break;
    default:
        break;
    }
    return y;
}

double _fmod_special(double x, double y, U32 errorCode)
{
    switch (errorCode) {
        /*All the three conditions are considered to be the same
           for Windows. It might be different for Linux.
           May have to come back for linux/WIndows. */
    case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
            UT64 Y;
            Y.f64 = y;
            __alm_handle_error(Y.u64, AMD_F_INVALID);
        }
        break;
    case REMAINDER_X_NAN:
        {
#ifdef WINDOWS
            UT64 Y;
            Y.f64 = y;
            __alm_handle_error(Y.u64, AMD_F_INVALID);
#else               /*  */
            return x + x;
#endif              /*  */
        }
        break;
    default:
        break;
    }
    return y;
}

float _fmodf_special(float x, float y, U32 errorCode)
{
    switch (errorCode) {
        /*All the three conditions are considered to be the same
           for Windows. It might be different for Linux.
           May have to come back for linux/WIndows. */
    case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
            UT32 Y;
            Y.f32 = y;
            __alm_handle_errorf(Y.u32, AMD_F_INVALID);
        }
        break;
    case REMAINDER_X_NAN:
        {
#ifdef WINDOWS
            UT32 Y;
            Y.f32 = y;
            __alm_handle_errorf(Y.u32, AMD_F_INVALID);

#else               /*  */
            return x + x;
#endif              /*  */
        }
        break;
    default:
        break;
    }
    return y;
}

