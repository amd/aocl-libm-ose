#ifndef __LIBM_SPECIAL_H__
#define __LIBM_SPECIAL_H__

#include <math.h>
#include <libm_errno_amd.h>
#if defined(WIN64)  |  defined(WINDOWS)

#include <fpieee.h>
#define EXCEPTION_S _exception
#define _amd_handle_error _handle_error
#define _amd_handle_errorf _handle_errorf

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

double _amd_handle_error(
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

float _amd_handle_errorf(
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
    _FpCodeUnspecified,
    _FpCodeAdd,
    _FpCodeSubtract,
    _FpCodeMultiply,
    _FpCodeDivide,
    _FpCodeSquareRoot,
    _FpCodeRemainder,
    _FpCodeCompare,
    _FpCodeConvert,
    _FpCodeRound,
    _FpCodeTruncate,
    _FpCodeFloor,
    _FpCodeCeil,
    _FpCodeAcos,
    _FpCodeAsin,
    _FpCodeAtan,
    _FpCodeAtan2,
    _FpCodeCabs,
    _FpCodeCos,
    _FpCodeCosh,
    _FpCodeExp,
    _FpCodeFabs,
    _FpCodeFmod,
    _FpCodeFrexp,
    _FpCodeHypot,
    _FpCodeLdexp,
    _FpCodeLog,
    _FpCodeLog10,
    _FpCodeModf,
    _FpCodePow,
    _FpCodeSin,
    _FpCodeSinh,
    _FpCodeTan,
    _FpCodeTanh,
    _FpCodeY0,
    _FpCodeY1,
    _FpCodeYn,
    _FpCodeLogb,
    _FpCodeNextafter,
    _FpCodeNegate, 
    _FpCodeFmin,         /* XMMI */
    _FpCodeFmax,         /* XMMI */
    _FpCodeConvertTrunc, /* XMMI */
    _XMMIAddps,          /* XMMI */
    _XMMIAddss,
    _XMMISubps,
    _XMMISubss,
    _XMMIMulps,
    _XMMIMulss,
    _XMMIDivps,
    _XMMIDivss,
    _XMMISqrtps,
    _XMMISqrtss,
    _XMMIMaxps,
    _XMMIMaxss,
    _XMMIMinps,
    _XMMIMinss,
    _XMMICmpps,
    _XMMICmpss,
    _XMMIComiss,
    _XMMIUComiss,
    _XMMICvtpi2ps,
    _XMMICvtsi2ss,
    _XMMICvtps2pi,
    _XMMICvtss2si,
    _XMMICvttps2pi,
    _XMMICvttss2si,
    _XMMIAddsubps,       /* XMMI for PNI */
    _XMMIHaddps,         /* XMMI for PNI */
    _XMMIHsubps,         /* XMMI for PNI */
    _XMMIRoundps,        /* 66 0F 3A 08  */ 
    _XMMIRoundss,        /* 66 0F 3A 0A  */
    _XMMIDpps,           /* 66 0F 3A 40  */
    _XMMI2Addpd,         /* XMMI2 */ 
    _XMMI2Addsd,
    _XMMI2Subpd,
    _XMMI2Subsd,
    _XMMI2Mulpd,
    _XMMI2Mulsd,
    _XMMI2Divpd,
    _XMMI2Divsd,
    _XMMI2Sqrtpd,
    _XMMI2Sqrtsd,
    _XMMI2Maxpd,
    _XMMI2Maxsd,
    _XMMI2Minpd,
    _XMMI2Minsd,
    _XMMI2Cmppd,
    _XMMI2Cmpsd,
    _XMMI2Comisd,
    _XMMI2UComisd,
    _XMMI2Cvtpd2pi,   /* 66 2D */
    _XMMI2Cvtsd2si,   /* F2 */
    _XMMI2Cvttpd2pi,  /* 66 2C */
    _XMMI2Cvttsd2si,  /* F2 */
    _XMMI2Cvtps2pd,   /* 0F 5A */
    _XMMI2Cvtss2sd,   /* F3 */
    _XMMI2Cvtpd2ps,   /* 66 */
    _XMMI2Cvtsd2ss,   /* F2 */
    _XMMI2Cvtdq2ps,   /* 0F 5B */
    _XMMI2Cvttps2dq,  /* F3 */
    _XMMI2Cvtps2dq,   /* 66 */
    _XMMI2Cvttpd2dq,  /* 66 0F E6 */
    _XMMI2Cvtpd2dq,   /* F2 */
    _XMMI2Addsubpd,   /* 66 0F D0 */
    _XMMI2Haddpd,     /* 66 0F 7C */
    _XMMI2Hsubpd,     /* 66 0F 7D */
    _XMMI2Roundpd,    /* 66 0F 3A 09 */
    _XMMI2Roundsd,    /* 66 0F 3A 0B */
    _XMMI2Dppd,       /* 66 0F 3A 41 */
    _FpCodeFmaSingle,
    _FpCodeFmaDouble,
    _FpCodeFms,
    _FpCodeFmsSingle,
    _FpCodeFmsDouble,
    _FpCodeFnma,
    _FpCodeFnmaSingle,
    _FpCodeFnmaDouble,
    _FpCodeFamin,
    _FpCodeFamax,
} _FP_OPERATION_CODE;

#define _DOMAIN     1   /* argument domain error */
#define _SING       2   /* argument singularity */
#define _OVERFLOW   3   /* overflow range error */
#define _UNDERFLOW  4   /* underflow range error */
#define _TLOSS      5   /* total loss of precision */
#define _PLOSS      6   /* partial loss of precision */
#endif

typedef enum 
{
	_FpCodeAsinh = (_XMMI2Dppd + 20),
	_FpCodeAcosh,
	_FpCodeAtanh,
	_FpCodeCbrt,
	_FpCodeExp10,
	_FpCodeExp2,
	_FpCodeExpm1,
	_FpCodeLrint,
	_FpCodeRint,
	_FpCodeNexttoward,
	_FpCodeNearbyint,
	_FpCodeFdim,
	_FpCodeFinite,
	_FpCodeScalbn,
	_FpCodeScalbln,
	_FpCodeLog1p,
	_FpCodeLog2,
	_FpCodeLlrint,
	_FpCodeLlround,
	_FpCodeLround,
	_FpCodeRemquo,
    _FpCodeFma
}_FP_OPERATION_CODE_EXT;

#endif // __LIBM_SPECIAL_H__


