#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"



long int FN_PROTOTYPE(lrintf)(float x)
{

    UT32 checkbits,val_2p23;
    checkbits.f32=x;

    /* Clear the sign bit and check if the value can be rounded */

    if( (checkbits.u32 & 0x7FFFFFFF) > 0x4B000000)
    {
        /* number cant be rounded raise an exception */
        /* Number exceeds the representable range could be nan or inf also*/
			 _amd_handle_errorf("lrintf",_FpCodeLrint, (long long int) x, _DOMAIN,
							0, EDOM, x, 0.0,1);
			 return (long int) x;
    }


    val_2p23.u32 = (checkbits.u32 & 0x80000000) | 0x4B000000;

   /* Add and sub 2^23 to round the number according to the current rounding direction */

    return (long int) ((x + val_2p23.f32) - val_2p23.f32);
}
weak_alias (lrintf, FN_PROTOTYPE(lrintf))
