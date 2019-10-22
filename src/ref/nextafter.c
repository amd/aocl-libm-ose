#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"


double FN_PROTOTYPE_REF(nextafter)(double x, double y)
{


    UT64 checkbits,checkbitsy;
    double dy = y;
    checkbits.f64=x;
	checkbitsy.f64 = y;




    /* check if the number is nan */
    if(((checkbits.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64 ))
    {
#ifdef WINDOWS
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
	    if (checkbits.u64 & QNAN_MASK_64)

		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
	    else
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
    }


    /* check if y is nan */
    if(((checkbitsy.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64 ))
    {
#ifdef WINDOWS
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbitsy.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, y, 0.0, 1);
#else
	    if (checkbitsy.u64 & QNAN_MASK_64)
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbitsy.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, y, 0.0, 1);
	    else
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
    }
	
	
	/* if x == y return y in the type of x */
    if( x == dy )
    {
        return dy;
    }

    if( x == 0.0)
    {
        checkbits.u64 = 1;
        if( dy > 0.0 )
             return checkbits.f64;
        else
            return -checkbits.f64;
    }


    /* compute the next heigher or lower value */

    if(((x>0.0) ^ (dy>x)) == 0)
    {
        checkbits.u64++;
    }
    else
    {
        checkbits.u64--;
    }

    /* check if the result is nan or inf */
    if(((checkbits.u64 & ~SIGNBIT_DP64) >= EXPBITS_DP64 ))
    {
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
    }

    return checkbits.f64;
}

