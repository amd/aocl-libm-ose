/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*/

#ifndef __LIBM_SPECIAL_H__
#define __LIBM_SPECIAL_H__

#include <math.h>
#include <libm_errno_amd.h>
#if defined(WIN64)  |  defined(WINDOWS)

#include <fpieee.h>
#define EXCEPTION_S _exception
#define __amd_handle_error _handle_error
#define __amd_handle_errorf _handle_errorf

double _handle_error(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        double arg1,
        double arg2,
        int nargs
        );

float _handle_errorf(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        float arg1,
        float arg2,
        int nargs
        );

#else //Windows
#define EXCEPTION_S exception

double __amd_handle_error(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        double arg1,
        double arg2,
        int nargs
        );

float __amd_handle_errorf(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        float arg1,
        float arg2,
        int nargs
        );



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

#define _DOMAIN     1   /* argument domain error */
#define _SING       2   /* argument singularity */
#define _OVERFLOW   3   /* overflow range error */
#define _UNDERFLOW  4   /* underflow range error */
#define _TLOSS      5   /* total loss of precision */
#define _PLOSS      6   /* partial loss of precision */
#endif

typedef enum 
{
	__amd_asinh = (__amd_dppd + 20),
	__amd_acosh,
	__amd_atanh,
	__amd_cbrt,
	__amd_exp10,
	__amd_exp2,
	__amd_expm1,
	__amd_lrint,
	__amd_rint,
	__amd_nexttoward,
	__amd_nearbyint,
	__amd_fdim,
	__amd_finite,
	__amd_scalbn,
	__amd_scalbln,
	__amd_log1p,
	__amd_log2,
	__amd_llrint,
	__amd_llround,
	__amd_lround,
	__amd_remquo,
        __amd_fma
}_AMDLIBM_EXTERNAL;

#endif // __LIBM_SPECIAL_H__


