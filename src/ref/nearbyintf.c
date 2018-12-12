#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_errno_amd.h"
#include "libm_special.h"


float FN_PROTOTYPE(nearbyintf)(float x)
{
    /* Check for input range */
	unsigned int mxscr_stat;
    UT32 checkbits,sign,val_2p23;
    checkbits.f32=x;

    /* Clear the sign bit and check if the value can be rounded(i.e check if exponent less than 23) */
    if( (checkbits.u32 & 0x7FFFFFFF) > 0x4B000000)
    {
      /* take care of nan or inf */
	if(	(checkbits.u32 & EXPBITS_SP32) == EXPBITS_SP32)
	{
        if((checkbits.u32 & MANTBITS_SP32) == 0x0)
        {
            // x is Inf
#ifdef WINDOWS
            return  __amd_handle_errorf("nearbyintf", __amd_nearbyint, checkbits.u32, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#else
            return  __amd_handle_errorf("nearbyintf", __amd_nearbyint, checkbits.u32, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#endif
		}
		else
		{
			// x is NaN
#ifdef WINDOWS
			return  __amd_handle_errorf("nearbyintf", __amd_nearbyint, checkbits.u32 | QNAN_MASK_32 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
			if (checkbits.u32 & QNAN_MASK_32)
			return  __amd_handle_errorf("nearbyintf", __amd_nearbyint, checkbits.u32 | QNAN_MASK_32 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
			else
			return  __amd_handle_errorf("nearbyintf", __amd_nearbyint, checkbits.u32 | QNAN_MASK_32 , _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
		}
	}
	else
		return x;
    }

    sign.u32 =  checkbits.u32 & 0x80000000;   
    val_2p23.u32 = sign.u32 | 0x4B000000;
	 
	mxscr_stat = _mm_getcsr();
	_mm_setcsr(mxscr_stat | 0x00000800);
    val_2p23.f32 = (x + val_2p23.f32) - val_2p23.f32;
	mxscr_stat = mxscr_stat & 0xffffffc0; // clear the exceptions
	_mm_setcsr(mxscr_stat);
    /*This extra line is to take care of denormals and various rounding modes*/
    val_2p23.u32 = ((val_2p23.u32 << 1) >> 1) | sign.u32;

    return (val_2p23.f32);   
}
weak_alias (nearbyintf, FN_PROTOTYPE(nearbyintf))

