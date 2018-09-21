#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

int FN_PROTOTYPE(ilogbf)(float x)
{

    /* Check for input range */
    UT32 checkbits;
    int expbits;
    U32 manbits;
    U32 zerovalue;
    checkbits.f32=x;

    /* Clear the sign bit and check if the value is zero nan or inf.*/
    zerovalue = (checkbits.u32 & ~SIGNBIT_SP32);

    if(zerovalue == 0)
    {
        /* Raise domain error as the number zero*/
        _amd_handle_errorf("ilogbf", _FpCodeLog, (unsigned int)INT_MIN, _SING, AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
        return INT_MIN;
    }

    if( zerovalue == EXPBITS_SP32 )
    {
        /* Raise domain error as the number is inf */
        //if negative inf raise an exception
        //if positive inf don't raise and exception
        if (x<0.0)
                _amd_handle_errorf("ilogbf", _FpCodeLog, (unsigned int)INT_MAX, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
        else
                _amd_handle_errorf("ilogbf", _FpCodeLog, (unsigned int)INT_MAX, 0, AMD_F_NONE, 0, x, 0.0, 1);
        return INT_MAX;
    }

    if( zerovalue > EXPBITS_SP32 )
    {
        /* Raise exception as the number is inf */
#ifdef WINDOWS
        _amd_handle_errorf("ilogbf", _FpCodeLog, (unsigned int)INT_MIN, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
        return INT_MIN;
#else
        //x = x+x;
        //x+x is not sufficient here since we return an integer and in 
        //optimization mode the compiler tends to optimize out the 
        //x+x operation if done.
        if (zerovalue >= 0x7fC00000)
                _amd_handle_errorf("ilogbf", _FpCodeLog, (unsigned int)INT_MIN, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
        else    
                _amd_handle_errorf("ilogbf", _FpCodeLog, (unsigned int)INT_MIN, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
        return INT_MIN;
#endif
    }

    expbits = (int) (( checkbits.u32 << 1) >> 24);

    if(expbits == 0 && (checkbits.u32 & MANTBITS_SP32 )!= 0)
    {
        /* the value is denormalized */
      manbits = checkbits.u32 & MANTBITS_SP32;
      expbits = EMIN_SP32;
      while (manbits < IMPBIT_SP32)
        {
          manbits <<= 1;
          expbits--;
        }
    }
    else
    {
        expbits-=EXPBIAS_SP32;
    }

    return expbits;
}
weak_alias (ilogbf, FN_PROTOTYPE(ilogbf))
