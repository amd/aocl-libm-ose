
/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _ALM_SPECIAL_H_
#define _ALM_SPECIAL_H_

#include <stdint.h>
#include <math.h>

#include <libm_errno_amd.h>

/*
 * Error codes to specify if an input is or
 *  output will be special floating point numbers
 */

enum {
        ALM_E_IN_X_NEG  = 1<<8,         /* is a ZERO */
        ALM_E_IN_X_NAN  = 1<<7,         /* first arg is a NaN or QNaN */
        ALM_E_IN_X_INF  = 1<<6,         /* is '+/- INF' */
        ALM_E_IN_X_ZERO = 1<<5,         /* is a ZERO */
        ALM_E_DIV_BY_ZER0 = 1 << 2,

        /* For functions with 2 args */
        ALM_E_IN_Y_NAN  = 1<<4,         /*  */
        ALM_E_IN_Y_INF  = 1<<3,
        ALM_E_IN_Y_ZERO = 1<<2,
        ALM_E_IN_Y_NEG  = 1<<0,         /* is a ZERO */


        ALM_E_OUT_NAN  = 1<<16,
        ALM_E_OUT_INF  = 1<<17,
        ALM_E_OUT_ZERO = 1<<18,
};

typedef enum {
    __amd_unspecified,
    __amd_add,
    __amd_subtract,
    __amd_multiply,
    __amd_divide,
    __amd_squareroot,
    __amd_remainder,
    __amd_compare,
    __amd_convert,
    __amd_round,
    __amd_truncate,
    __amd_floor,
    __amd_ceil,
    __amd_acos,
    __amd_asin,
    __amd_atan,
    __amd_atan2,
    __amd_cabs,
    __amd_cos,
    __amd_cosh,
    __amd_exp,
    __amd_fabs,
    __amd_fmod,
    __amd_frexp,
    __amd_hypot,
    __amd_ldexp,
    __amd_log,
    __amd_log10,
    __amd_modf,
    __amd_pow,
    __amd_sin,
    __amd_sinh,
    __amd_tan,
    __amd_tanh,
    __amd_y0,
    __amd_y1,
    __amd_yn,
    __amd_logb,
    __amd_nextafter,
    __amd_negate,
    __amd_fmin,
    __amd_fmax,
    __amd_converttrunc,
    __amd_addps,
    __amd_addss,
    __amd_subps,
    __amd_subss,
    __amd_mulps,
    __amd_mulss,
    __amd_divps,
    __amd_divss,
    __amd_sqrtps,
    __amd_sqrtss,
    __amd_maxps,
    __amd_maxss,
    __amd_minps,
    __amd_minss,
    __amd_cmpps,
    __amd_cmpss,
    __amd_comiss,
    __amd_ucomiss,
    __amd_cvtpi2ps,
    __amd_cvtsi2ss,
    __amd_cvtps2pi,
    __amd_Cvtss2si,
    __amd_cvttps2pi,
    __amd_cvttss2si,
    __amd_addsubps,       /* XMMI for PNI */
    __amd_haddps,         /* XMMI for PNI */
    __amd_hsubps,         /* XMMI for PNI */
    __amd_roundps,        /* 66 0F 3A 08  */
    __amd_roundss,        /* 66 0F 3A 0A  */
    __amd_dpps,           /* 66 0F 3A 40  */
    __amd_addpd,         /* XMMI2 */
    __amd_addsd,
    __amd_subpd,
    __amd_subsd,
    __amd_mulpd,
    __amd_mulsd,
    __amd_divpd,
    __amd_divsd,
    __amd_sqrtpd,
    __amd_sqrtsd,
    __amd_maxpd,
    __amd_maxsd,
    __amd_minpd,
    __amd_minsd,
    __amd_cmppd,
    __amd_cmpsd,
    __amd_comisd,
    __amd_ucomisd,
    __amd_cvtpd2pi,   /* 66 2D */
    __amd_cvtsd2si,   /* F2 */
    __amd_cvttpd2pi,  /* 66 2C */
    __amd_cvttsd2si,  /* F2 */
    __amd_cvtps2pd,   /* 0F 5A */
    __amd_cvtss2sd,   /* F3 */
    __amd_cvtpd2ps,   /* 66 */
    __amd_cvtsd2ss,   /* F2 */
    __amd_cvtdq2ps,   /* 0F 5B */
    __amd_cvttps2dq,  /* F3 */
    __amd_cvtps2dq,   /* 66 */
    __amd_cvttpd2dq,  /* 66 0F E6 */
    __amd_cvtpd2dq,   /* F2 */
    __amd_addsubpd,   /* 66 0F D0 */
    __amd_haddpd,     /* 66 0F 7C */
    __amd_hsubpd,     /* 66 0F 7D */
    __amd_roundpd,    /* 66 0F 3A 09 */
    __amd_roundsd,    /* 66 0F 3A 0B */
    __amd_dppd,       /* 66 0F 3A 41 */
    __amd_fmaSingle,
    __amd_fmaDouble,
    __amd_fms,
    __amd_fmsSingle,
    __amd_fmsDouble,
    __amd_fnma,
    __amd_fnmaSingle,
    __amd_fnmaDouble,
    __amd_famin,
    __amd_famax,
} _AMDLIBM_CODE;

#if defined(WIN64)  |  defined(WINDOWS)
#include <fpieee.h>
#define EXCEPTION_S							_exception
#else
#define EXCEPTION_S                         exception
#endif  /* #if defined(WIN64)  |  defined(WINDOWS) */

#define ALM_ERR_DOMAIN     1		/* argument domain error */
#define ALM_ERR_SING       2		/* argument singularity */
#define ALM_ERR_OVERFLOW   3		/* overflow range error */
#define ALM_ERR_UNDERFLOW  4		/* underflow range error */
#define ALM_ERR_TLOSS      5		/* total loss of precision */
#define ALM_ERR_PLOSS      6		/* partial loss of precision */

double __alm_handle_error(
        uint64_t value,
        int flags
        );

float __alm_handle_errorf(
        uint64_t value,
        int flags
        );

double  alm_pow_special (double y, uint32_t code);
float   alm_powf_special (float y, uint32_t code);

double  alm_exp_special (double y, uint32_t code);
float   alm_expf_special (float y, uint32_t code);

double  alm_log_special(double y, uint32_t code);
float   alm_logf_special(float y, uint32_t code);

float   alm_acosf_special(float x, uint32_t code);
double  alm_acos_special(double x, uint32_t code);

double  alm_asin_special(double x, uint32_t code);
float   alm_asinf_special(float x, uint32_t code);

float   _atanf_special_overflow(float x);
double  alm_atan_special(double x);

extern  float _cbrtf_special            (float x);
extern  float _cosf_special             (float x);
extern  float _fabsf_special            (float x);
extern  float _fdimf_special            (float x, float y, float r);
extern  float _fmaxf_special            (float x, float y);
extern  float _fminf_special            (float x, float y);
extern  float _fmodf_special            (float x, float y, uint32_t code);
extern  float _remainderf_special       (float x, float y, uint32_t code);
extern  float _sinf_cosf_special        (float x, char *name, uint32_t code);
extern  float _sinf_special             (float x);
extern  float _tanf_special             (float x);
extern  float _tanhf_special            (float x);
extern  float _truncf_special           (float x, float r);
extern  void _sincosf_special           (float x, float *sy, float *cy);
extern  float _sinf_cosf_special_underflow(float x, char *name, uint32_t code);

extern  double _cbrt_special            (double x);
extern  double _cos_special             (double x);
extern  double _fabs_special            (double x);
extern  double _fdim_special            (double x, double y, double r);
extern  double _fmax_special            (double x, double y);
extern  double _fmin_special            (double x, double y);
extern  double _fmod_special            (double x, double y, uint32_t code);
extern  double _nearbyint_special       (double x);
extern  double _remainder_special       (double x, double y, uint32_t code);
extern  double _round_special           (double x, double r);
extern  double _sin_cos_special         (double x, char *name, uint32_t code);
extern  double _sincos_special_underflow (double x, char *name, uint32_t code);
extern  double _sin_special             (double x);
extern  double _sin_special_underflow   (double x);
extern  double _tan_special             (double x);
extern  double _trunc_special           (double x, double r);
extern  void _sincos_special            (double x, double *sy, double *cy);

#endif
