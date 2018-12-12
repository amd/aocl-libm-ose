#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

/* Returns 0 if x is infinite or NaN, otherwise returns 1 */

int FN_PROTOTYPE(finitef)(float x)
{

  unsigned int ux,ax;
  GET_BITS_SP32(x, ux);
  ax = ux & (~SIGNBIT_SP32);
  if (ax > 0x7f800000)
    /* x is NaN */
    #ifdef WINDOWS
         return (int)__amd_handle_errorf("finitef", __amd_finite, 0, _DOMAIN, 0, EDOM, x, 0.0, 1);
    #else
         if(!(ax & 0x00400000)) //x is snan
           return (int)__amd_handle_errorf("finitef", __amd_finite, 0, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
    #endif

  return (int)((ax - PINFBITPATT_SP32) >> 31);

}
weak_alias (finitef, FN_PROTOTYPE(finitef))

