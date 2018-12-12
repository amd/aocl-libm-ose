#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"




float FN_PROTOTYPE(nextafterf)(float x, float y)
{


    UT32 checkbits,checkbitsy;
    float dy = y;
    checkbits.f32=x;
	checkbitsy.f32 = y;



    /* check if the number is nan */
    if(((checkbits.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32 ))
    {
#ifdef WINDOWS
		return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbits.u32| QNAN_MASK_32, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
	    if (checkbits.u32 & QNAN_MASK_32)
		return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbits.u32| QNAN_MASK_32, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
	    else
		return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbits.u32| QNAN_MASK_32, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
    }


    /* check if the number is nan */
    if(((checkbitsy.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32 ))
    {
#ifdef WINDOWS
		return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbitsy.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_NONE, EDOM, y, 0.0, 1);
#else
	    if (checkbitsy.u32 & QNAN_MASK_32)
		return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbitsy.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_NONE, EDOM, y, 0.0, 1);
	    else
		return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbitsy.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_INVALID, EDOM, y, 0.0, 1);
#endif
    }

    /* if x == y return y in the type of x */
    if( x == dy )
    {
        return  dy;
    }


    if( x == 0.0)
    {
        checkbits.u32 = 1;
        if( dy > 0.0 )
             return checkbits.f32;
        else
            return -checkbits.f32;
    }


    /* compute the next heigher or lower value */
    if(((x>0.0F) ^ (dy>x)) == 0)
    {
        checkbits.u32++;
    }
    else
    {
        checkbits.u32--;
    }

    /* check if the result is nan or inf */
    if(((checkbits.u32 & ~SIGNBIT_SP32) >= EXPBITS_SP32 ))
    {
			return  __amd_handle_errorf("nextafterf", __amd_nextafter, checkbits.u32 | QNAN_MASK_32, _DOMAIN,0, EDOM, x, 0.0, 1);
    }

    return checkbits.f32;
}
weak_alias (nextafterf, FN_PROTOTYPE(nextafterf))
