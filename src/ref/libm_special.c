#include "libm_util_amd.h"
#include "libm_special.h"

#if defined(WIN64)  |  defined(WINDOWS)
#else
static inline void _amd_raise_fp_exc(int flags)
{
    if((flags & AMD_F_UNDERFLOW) == AMD_F_UNDERFLOW)
    {
        double a =  0x1.0p-1022;
        __asm __volatile ("mulsd %1, %0" :  "+x" (a): "x" (a));
    }

    if((flags & AMD_F_OVERFLOW) == AMD_F_OVERFLOW)
    {
        double a = 0x1.fffffffffffffp1023;
        __asm __volatile ("mulsd %1, %0" :  "+x" (a): "x" (a));
    }

    if((flags & AMD_F_DIVBYZERO) == AMD_F_DIVBYZERO)
    {
        double a = 1.0, b = 0.0;
        __asm __volatile ("divsd %1, %0" :  "+x" (a) : "x" (b));
    }

    if((flags & AMD_F_INVALID) == AMD_F_INVALID)
    {
        double a = 0.0;
        __asm __volatile ("divsd %1, %0" :  "+x" (a): "x" (a));
    }
}


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
        )
{

    double z;
    //To avoid compiler warnings
    //These variables are coming from the microsoft based exception handling routines.
    //We just refer them and leave it.
    fname  = NULL;
    opcode = 0;
    type   = 0;
    error  = 0;
    arg1   = 0.0;
    arg2   = 0.0;
    nargs  = 0;
    PUT_BITS_DP64(value, z);
    _amd_raise_fp_exc(flags);
    return z;
 }

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
        )
{

  float z;
  //To avoid compiler warnings
  //These variables are coming from the microsoft based exception handling routines.
  //We just refer them and leave it.
  fname  = NULL;
  opcode = 0;
  type   = 0;
  error  = 0;
  arg1   = 0.0;
  arg2   = 0.0;
  nargs  = 0;
  PUT_BITS_SP32(value, z);
  _amd_raise_fp_exc(flags);
  return z;
}
#endif

double _sincos_special_underflow(double x, char *name,_FP_OPERATION_CODE code) 
{
        UT64 xu;
        xu.f64 = x;
	return  _amd_handle_error(name, code, xu.u64, _DOMAIN,AMD_F_UNDERFLOW | AMD_F_INEXACT, EDOM, x, 0.0, 1);
}


double _sin_cos_special(double x, char *name,_FP_OPERATION_CODE code)
{
    UT64 xu;
	xu.f64 = x;

    if((xu.u64 & EXPBITS_DP64) == EXPBITS_DP64)
    {
        // x is Inf or NaN
        if((xu.u64 & MANTBITS_DP64) == 0x0)
        {
            // x is Inf
            xu.u64 = INDEFBITPATT_DP64;
           return  _amd_handle_error(name, code, xu.u64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
		}
		else {
			// x is NaN
#if defined(WINDOWS)
			return  _amd_handle_error(name, code, xu.u64 | QNAN_MASK_64, _DOMAIN,0, EDOM, x, 0.0, 1);
#else
		       if( xu.u64 & QNAN_MASK_64)	
			return  _amd_handle_error(name, code, xu.u64, _DOMAIN,0, EDOM, x, 0.0, 1);
		       else
			return  _amd_handle_error(name, code, xu.u64 | QNAN_MASK_64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif

		}
		
	}

	return xu.f64;
}

float _sinf_cosf_special(float x, char *name,_FP_OPERATION_CODE code)
{
    UT32 xu;

	xu.f32 = x;

    if((xu.u32 & EXPBITS_SP32) == EXPBITS_SP32)
    {
        // x is Inf or NaN
        if((xu.u32 & MANTBITS_SP32) == 0x0)
        {
            // x is Inf	
            xu.u32 = INDEFBITPATT_SP32;
           return  _amd_handle_errorf(name, code, xu.u32, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
		}
		else {
			// x is NaN
#if defined(WINDOWS)
			return  _amd_handle_errorf(name, code, xu.u32 | QNAN_MASK_32, _DOMAIN,0, EDOM, x, 0.0, 1);
#else
		       if( xu.u32 & QNAN_MASK_32)	
			return  _amd_handle_errorf(name, code, xu.u32 , _DOMAIN,0, EDOM, x, 0.0, 1);
		       else
			return  _amd_handle_errorf(name, code, xu.u32 | QNAN_MASK_32, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif

		}
		
	}

	return xu.f32;
}

double _sin_special_underflow(double x)
{
	 return _sincos_special_underflow(x, "sin",_FpCodeSin);
}


float _sinf_special(float x)
{
	return _sinf_cosf_special(x, "sinf",_FpCodeSin);
}

double _sin_special(double x)
{
		return  _sin_cos_special(x, "sin",_FpCodeSin);
}

float _cosf_special(float x)
{
	return _sinf_cosf_special(x, "cosf",_FpCodeCos);
}

double _cos_special(double x)
{
	return _sin_cos_special(x, "cos",_FpCodeCos);
}

void _sincosf_special(float x, float *sy, float *cy)
{
    float xu = _sinf_cosf_special(x, "sincosf",_FpCodeSin);

	*sy = xu;
	*cy = xu;

	return;
}

void _sincos_special(double x, double *sy, double *cy)
{
    double xu = _sin_cos_special(x, "sincos",_FpCodeSin);

	*sy = xu;
	*cy = xu;

	return;
}

double _tan_special(double x)
{
		return  _sin_cos_special(x, "tan",_FpCodeTan);
}

float _tanf_special(float x)
{
		return  _sinf_cosf_special(x, "tanf",_FpCodeTan);
}

double _fabs_special(double x)
{
	    UT64 xu;
		xu.f64 = x;
		// x is NaN
#ifdef WINDOWS		// 
		return  _amd_handle_error("fabs", _FpCodeFabs, xu.u64 | QNAN_MASK_64, _DOMAIN,0, EDOM, x, 0.0, 1);
#else

		if (xu.u64 & QNAN_MASK_64)
			return  _amd_handle_error("fabs", _FpCodeFabs, xu.u64 | QNAN_MASK_64, _DOMAIN,AMD_F_NONE, EDOM, x, 0.0, 1);
		else
			return  _amd_handle_error("fabs", _FpCodeFabs, xu.u64 | QNAN_MASK_64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif

}

float _fabsf_special(float x)
{
	    UT32 xu;
		xu.f32 = x;
		// x is NaN
#ifdef WINDOWS		// 
		return  _amd_handle_errorf("fabsf", _FpCodeFabs, xu.u32 | QNAN_MASK_32, _DOMAIN,0, EDOM, x, 0.0, 1);
#else		
		if (xu.u32 & QNAN_MASK_32)
			return  _amd_handle_errorf("fabsf", _FpCodeFabs, xu.u32 | QNAN_MASK_32, _DOMAIN,AMD_F_NONE, EDOM, x, 0.0, 1);
		else
			return  _amd_handle_errorf("fabsf", _FpCodeFabs, xu.u32 | QNAN_MASK_32, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif		
}


double _cbrt_special(double x)
{
	    UT64 xu;
		xu.f64 = x;
		// x is NaN
		return  _amd_handle_error("cbrt",_FpCodeCbrt, xu.u64 | QNAN_MASK_64, _DOMAIN,0, EDOM, x, 0.0, 1);
}


float _cbrtf_special(float x)
{
	    UT32 xu;
		xu.f32 = x;
		// x is NaN
		return  _amd_handle_errorf("cbrtf",_FpCodeCbrt, xu.u32 | QNAN_MASK_32, _DOMAIN,0, EDOM, x, 0.0F, 1);
}



double _nearbyint_special(double x)
{

    UT64 checkbits;
    checkbits.f64=x;

      /* take care of nan or inf */
	if(	(checkbits.u64 & EXPBITS_DP64) == EXPBITS_DP64)
	{
        if((checkbits.u64 & MANTBITS_DP64) == 0x0)
        {
            // x is Inf	
#ifdef WINDOWS
            return  _amd_handle_error("nearbyint", _FpCodeNearbyint, checkbits.u64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
#else
			 return  _amd_handle_error("nearbyint", _FpCodeNearbyint, checkbits.u64, _DOMAIN,AMD_F_NONE, EDOM, x, 0.0, 1);
#endif

		}
		else 
		{
#ifdef WINDOWS
			return  _amd_handle_error("nearbyint", _FpCodeNearbyint, checkbits.u64 | QNAN_MASK_64, _DOMAIN,0, EDOM, x, 0.0, 1);
#else
		if (checkbits.u64 & QNAN_MASK_64)
			return  _amd_handle_error("nearbyint", _FpCodeNearbyint, checkbits.u64 | QNAN_MASK_64, _DOMAIN,AMD_F_NONE, EDOM, x, 0.0, 1);
		else
			return  _amd_handle_error("nearbyint", _FpCodeNearbyint, checkbits.u64 | QNAN_MASK_64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
		}
	}
	else
		return x;

}

#define EXP_X_NAN       1
#define EXP_Y_ZERO      2
#define EXP_Y_INF       3

float _expf_special(float x, float y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("expf", _FpCodeExp, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("expf", _FpCodeExp, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("expf", _FpCodeExp, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, 0.0, 1);
        }
        break;
    }
    return y;
}

float _exp2f_special(float x, float y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("exp2f", _FpCodeExp2, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("exp2f", _FpCodeExp2, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("exp2f", _FpCodeExp2, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, 0.0, 1);
        }
        break;
    }
    return y;
}
float _exp10f_special(float x, float y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("exp10f", _FpCodeExp10, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("exp10f", _FpCodeExp10, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("exp10f", _FpCodeExp10, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, 0.0, 1);
        }
        break;
    }
    return y;
}
float _expm1f_special(float x, float y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("expm1f", _FpCodeExpm1, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("expm1f", _FpCodeExpm1, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.u64 = 0; ym.f32[0] = y;
            _amd_handle_errorf("expm1f", _FpCodeExpm1, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, 0.0, 1);
        }
        break;
    }
    return y;
}

double _exp_special(double x, double y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp", _FpCodeExp, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp", _FpCodeExp, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp", _FpCodeExp, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE ,x, 0.0, 1);
        }
        break;
    }
    return y;
}

double _exp2_special(double x, double y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp2", _FpCodeExp2, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp2", _FpCodeExp2, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp2", _FpCodeExp2, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE ,x, 0.0, 1);
        }
        break;
    }
    return y;
}

double _exp10_special(double x, double y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp10", _FpCodeExp10, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp10", _FpCodeExp10, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("exp10", _FpCodeExp10, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE ,x, 0.0, 1);
        }
        break;
    }
    return y;
}
double _expm1_special(double x, double y, U32 code)
{
    switch(code)
    {
    case EXP_X_NAN:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("expm1", _FpCodeExpm1, ym.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
        }
        break;

    case EXP_Y_ZERO:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("expm1", _FpCodeExpm1, ym.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case EXP_Y_INF:
        {
            UT64 ym; ym.f64 = y;
            _amd_handle_error("expm1", _FpCodeExpm1, ym.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE ,x, 0.0, 1);
        }
        break;
    }
    return y;
}

float _truncf_special(float x, float r)
{
   UT64 rm; rm.u64 = 0; rm.f32[0] = r;
  _amd_handle_errorf("truncf", _FpCodeTruncate, rm.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
  return r;
}

double _trunc_special(double x, double r)
{
   UT64 rm; rm.f64 = r;
   _amd_handle_error("trunc", _FpCodeTruncate, rm.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
   return r;
}

double _round_special(double x, double r)
{
   UT64 rm; rm.f64 = r;
   _amd_handle_error("round", _FpCodeRound, rm.u64, _DOMAIN, 0, EDOM, x, 0.0, 1);
   return r;
}

#define LOG_X_ZERO      1
#define LOG_X_NEG       2
#define LOG_X_NAN       3


static float _logf_special_common(float x, float y, U32 fnCode, U32 errorCode, char *name)
{

    switch(errorCode)
    {
    case LOG_X_ZERO:
        {
			UT32 Y;
			Y.f32 = y;
           _amd_handle_errorf(name, fnCode, Y.u32, _SING, AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
        }
        break;

    case LOG_X_NEG:
        {
			UT32 Y;
			Y.f32 = y;
           _amd_handle_errorf(name, fnCode, Y.u32, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case LOG_X_NAN:
        {
#ifdef WIN64
			UT32 Y;
			Y.f32 = y;
            _amd_handle_errorf(name, fnCode, Y.u32, _DOMAIN,AMD_F_NONE, EDOM, x, 0.0, 1);
#else
            return x+x;
#endif      
         }
        break;
    }

    return y;
}



float _logf_special(float x, float y, U32 code)
{
    return _logf_special_common(x, y, _FpCodeLog, code, "logf");
}

float _log10f_special(float x, float y, U32 code)
{
    return _logf_special_common(x, y, _FpCodeLog10, code, "log10f");
}

float _log2f_special(float x, float y, U32 code)
{
    return _logf_special_common(x, y, _FpCodeLog, code, "log2f");
}

float _log1pf_special(float x, float y, U32 code)
{
    return _logf_special_common(x, y, _FpCodeLog1p, code, "log1pf");
}


static double _log_special_common(double x, double y, U32 fnCode, U32 errorCode, char *name)
{
	
    switch(errorCode)
    {
    case LOG_X_ZERO:
        {
			UT64 Y;
			Y.f64 = y;
           _amd_handle_error(name, fnCode, Y.u64, _SING, AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
        }
        break;

    case LOG_X_NEG:
        {
			UT64 Y;
			Y.f64 = y;
          _amd_handle_error(name, fnCode, Y.u64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case LOG_X_NAN:
        {
#ifdef  WIN64            
			UT64 Y;
			Y.f64 = y;
          _amd_handle_error(name, fnCode, Y.u64,  _DOMAIN, AMD_F_NONE,EDOM, x, 0.0, 1);
#else
         return x+x;
#endif          
        }
        break;
    }

    return y;
}

double _log_special(double x, double y, U32 code)
{

    return _log_special_common(x, y, _FpCodeLog, code, "log");
}

double _log10_special(double x, double y, U32 code)
{
    return _log_special_common(x, y, _FpCodeLog10, code, "log10");
}

double _log2_special(double x, double y, U32 code)
{
    return _log_special_common(x, y, _FpCodeLog, code, "log2");
}

double _log1p_special(double x, double y, U32 code)
{
    return _log_special_common(x, y, _FpCodeLog1p, code, "log1p");
}


float _fdimf_special(float x, float y,float r)
{
   UT64 rm; rm.u64 = 0; rm.f32[0] = r;
  _amd_handle_errorf("fdimf", _FpCodeFdim, rm.u64, _DOMAIN, 0, EDOM, x, y, 2);
  return r;
}

double _fdim_special(double x, double y,double r)
{
   UT64 rm; rm.f64 = r;
   _amd_handle_error("fdim", _FpCodeFdim, rm.u64, _DOMAIN, 0, EDOM, x, y, 2);
   return r;
}



double _fmax_special(double x, double y)
{
	UT64 xu,yu;
	xu.f64 = x;
	yu.f64 = y;
	if((xu.u64 & ~SIGNBIT_DP64) >  EXPBITS_DP64)
	{
		if((yu.u64 & ~SIGNBIT_DP64) >  EXPBITS_DP64)
#ifdef WINDOWS
			return _amd_handle_error("fmax", _FpCodeFmax, yu.u64 | QNAN_MASK_64, _DOMAIN, 0, EDOM, x, y, 2);
#else
			return _amd_handle_error("fmax", _FpCodeFmax, yu.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
#endif		
		else
			return _amd_handle_error("fmax", _FpCodeFmax, yu.u64, _DOMAIN, 0, EDOM, x, y, 2);
	}
	return _amd_handle_error("fmax", _FpCodeFmax, xu.u64, _DOMAIN, 0, EDOM, x, y, 2);
	
}


float _fmaxf_special(float x, float y)
{
	UT32 xu,yu;
	xu.f32 = x;
	yu.f32 = y;

	if((xu.u32 & ~SIGNBIT_SP32) >  EXPBITS_SP32)
	{
		if((yu.u32 & ~SIGNBIT_SP32) >  EXPBITS_SP32)
#ifdef WINDOWS
			return _amd_handle_errorf("fmaxf", _FpCodeFmax, yu.u32 | QNAN_MASK_32, _DOMAIN, 0, EDOM, x, y, 2);
#else
			return _amd_handle_errorf("fmaxf", _FpCodeFmax, yu.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
#endif		
		else
			return _amd_handle_errorf("fmaxf", _FpCodeFmax, yu.u32, _DOMAIN, 0, EDOM, x, y, 2);
	}
	return _amd_handle_errorf("fmaxf", _FpCodeFmax, xu.u32, _DOMAIN, 0, EDOM, x, y, 2);

	
}

double _fmin_special(double x, double y)
{

	UT64 xu,yu;
	xu.f64 = x;
	yu.f64 = y;
	if((xu.u64 & ~SIGNBIT_DP64) >  EXPBITS_DP64)
	{
		if((yu.u64 & ~SIGNBIT_DP64) >  EXPBITS_DP64)
#ifdef WINDOWS
			return _amd_handle_error("fmin", _FpCodeFmin, yu.u64 | QNAN_MASK_64, _DOMAIN, 0, EDOM, x, y, 2);
#else
			return _amd_handle_error("fmin", _FpCodeFmin, yu.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
#endif		
		else
			return _amd_handle_error("fmin", _FpCodeFmin, yu.u64, _DOMAIN, 0, EDOM, x, y, 2);
	}
	return _amd_handle_error("fmin", _FpCodeFmax, xu.u64, _DOMAIN, 0, EDOM, x, y, 2);

}


float _fminf_special(float x, float y)
{
	UT32 xu,yu;
	xu.f32 = x;
	yu.f32 = y;

	if((xu.u32 & ~SIGNBIT_SP32) >  EXPBITS_SP32)
	{
		if((yu.u32 & ~SIGNBIT_SP32) >  EXPBITS_SP32)
#ifdef WINDOWS
			return _amd_handle_errorf("fminf", _FpCodeFmin, yu.u32 | QNAN_MASK_32, _DOMAIN, 0, EDOM, x, y, 2);
#else
			return _amd_handle_errorf("fminf", _FpCodeFmin, yu.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
#endif		
		else
			return _amd_handle_errorf("fminf", _FpCodeFmin, yu.u32, _DOMAIN, 0, EDOM, x, y, 2);
	}
	return _amd_handle_errorf("fminf", _FpCodeFmin, xu.u32, _DOMAIN, 0, EDOM, x, y, 2);


	
}


#define REMAINDER_X_NAN       1
#define REMAINDER_Y_ZERO      2
#define REMAINDER_X_DIVIDEND_INF      3

float _remainderf_special(float x, float y,U32 errorCode)

{
	
    switch(errorCode)
    {
	/*All the three conditions are considered to be the same 
	 for Windows. It might be different for Linux. 
	 May have to come back for linux/WIndows.*/
	case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
			UT32 Y;
			Y.f32 = y;
          return _amd_handle_errorf("remainderf", _FpCodeRemainder, Y.u32,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
        }
        break;
    case REMAINDER_X_NAN:
          {
#ifdef WINDOWS
			UT32 Y;
			Y.f32 = y;
          _amd_handle_errorf("remainderf", _FpCodeRemainder, Y.u32,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
#else    
            return x+x;
#endif  
         }
        break;
    }
    return y;
}


double _remainder_special(double x, double y,U32 errorCode)

{
	
    switch(errorCode)
    {
	/*All the three conditions are considered to be the same 
	 for Windows. It might be different for Linux. 
	 May have to come back for linux/WIndows.*/
	case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
			UT64 Y;
			Y.f64 = y;
          _amd_handle_error("remainder", _FpCodeRemainder, Y.u64,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
        }
        break;
    case REMAINDER_X_NAN:
         {
#ifdef WINDOWS
			UT64 Y;
			Y.f64 = y;
          _amd_handle_error("remainder", _FpCodeRemainder, Y.u64,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
#else    
            return x+x;
#endif  
         }
        break;
    }
    return y;
}

double _fmod_special(double x, double y,U32 errorCode)

{
	
    switch(errorCode)
    {
	/*All the three conditions are considered to be the same 
	 for Windows. It might be different for Linux. 
	 May have to come back for linux/WIndows.*/
	case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
			UT64 Y;
			Y.f64 = y;
          _amd_handle_error("fmod", _FpCodeFmod, Y.u64,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
        }
        break;
    case REMAINDER_X_NAN:
        {
#ifdef WINDOWS
			UT64 Y;
			Y.f64 = y;
         _amd_handle_error("fmod", _FpCodeFmod, Y.u64,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
#else    
            return x+x;
#endif            
        }
        break;


    }
    return y;
}

float _fmodf_special(float x, float y,U32 errorCode)

{
	
    switch(errorCode)
    {
	/*All the three conditions are considered to be the same 
	 for Windows. It might be different for Linux. 
	 May have to come back for linux/WIndows.*/
	case REMAINDER_Y_ZERO:
    case REMAINDER_X_DIVIDEND_INF:
        {
			UT32 Y;
			Y.f32 = y;
          _amd_handle_errorf("fmodf", _FpCodeFmod, Y.u32,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
        }
        break;
 
        case REMAINDER_X_NAN:
        {
#ifdef WINDOWS
			UT32 Y;
			Y.f32 = y;
         _amd_handle_errorf("fmodf", _FpCodeFmod, Y.u32,  _DOMAIN, AMD_F_INVALID,EDOM, x, 0.0, 1);
#else    
            return x+x;
#endif            
        }
        break;

       
    }
    return y;
}



#define POW_X_ONE_Y_SNAN            1
#define POW_X_ZERO_Z_INF            2
#define POW_X_NAN                   3
#define POW_Y_NAN                   4
#define POW_X_NAN_Y_NAN             5
#define POW_X_NEG_Y_NOTINT          6
#define POW_Z_ZERO                  7
#define POW_Z_DENORMAL              8
#define POW_Z_INF                   9

double _pow_special(double x, double y, double z, U32 code)
{
	y=z;
    switch(code)
    {
    case POW_X_ONE_Y_SNAN:
        {
			UT64 zu;
			zu.f64 = z;
			_amd_handle_error("pow", _FpCodePow, zu.u64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case POW_X_ZERO_Z_INF:
        {
			UT64 zu;
			zu.f64 = z;
			_amd_handle_error("pow", _FpCodePow, zu.u64, _SING,AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
        }
        break;

    case POW_X_NAN:
    case POW_Y_NAN:
    case POW_X_NAN_Y_NAN:
        {
			UT64 zu;
			zu.f64 = z;
			_amd_handle_error("pow", _FpCodePow, zu.u64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case POW_X_NEG_Y_NOTINT:
        {
			UT64 zu;
			zu.f64 = z;
			_amd_handle_error("pow", _FpCodePow, zu.u64, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case POW_Z_ZERO:
    case POW_Z_DENORMAL:
        {
			UT64 zu;
			zu.f64 = z;
			_amd_handle_error("pow", _FpCodePow, zu.u64, _UNDERFLOW,AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case POW_Z_INF:
        {
			UT64 zu;
			zu.f64 = z;
			_amd_handle_error("pow", _FpCodePow, zu.u64, _OVERFLOW,AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, 0.0, 1);
        }
        break;
    }

    return z;
}


float _powf_special(float x, float y, float z, U32 code)
{
	y = z;
    switch(code)
    {
    case POW_X_ONE_Y_SNAN:
        {
            UT32 zu;
			zu.f32 = z;
			_amd_handle_errorf("powf", _FpCodePow, zu.u32, 0,AMD_F_INVALID, 0, x, 0.0, 1);
        }
        break;

    case POW_X_ZERO_Z_INF:
        {
            UT32 zu;
			zu.f32 = z;
			_amd_handle_errorf("powf", _FpCodePow, zu.u32, _SING,AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
        }
        break;

    case POW_X_NAN:
    case POW_Y_NAN:
    case POW_X_NAN_Y_NAN:
        {
           UT32 zu;
			zu.f32 = z;
			_amd_handle_errorf("powf", _FpCodePow, zu.u32, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case POW_X_NEG_Y_NOTINT:
        {
            UT32 zu;
			zu.f32 = z;
			_amd_handle_errorf("powf", _FpCodePow, zu.u32, _DOMAIN,AMD_F_INVALID, EDOM, x, 0.0, 1);
        }
        break;

    case POW_Z_ZERO:
        {
            UT32 zu;
			zu.f32 = z;
			_amd_handle_errorf("powf", _FpCodePow, zu.u32, _UNDERFLOW,AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, 0.0, 1);
        }
        break;

    case POW_Z_INF:
        {
            UT32 zu;
			zu.f32 = z;
			_amd_handle_errorf("powf", _FpCodePow, zu.u32, _OVERFLOW,AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, 0.0, 1);

        }
        break;
    }

    return z;
}
