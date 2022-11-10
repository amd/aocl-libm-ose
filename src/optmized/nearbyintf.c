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

#include "libm_util_amd.h"
#include "libm_errno_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>


float ALM_PROTO_OPT(nearbyintf)(float x)
{
    /* Check for input range */
    //unsigned int mxscr_stat;
    UT32 checkbits,val_2p23;
    uint32_t sign;
    checkbits.f32=x;

    // Clear the sign bit and check if the value can be rounded(i.e check if exponent less than 23)
    if( (checkbits.u32 & POS_BITSET_F32) > EXP_VAL_23_F32)   // -> EXP value is 150 (-127 bias equals 23)
    {
        // Check nan or inf
        if(    (checkbits.u32 & EXPBITS_SP32) == EXPBITS_SP32)
        {
            if((checkbits.u32 & MANTBITS_SP32) == POS_ZERO_F32)
            {
                // x is Inf
#ifdef WINDOWS
                return  __alm_handle_errorf(checkbits.u32, AMD_F_INVALID);
#else
                return  __alm_handle_errorf(checkbits.u32, AMD_F_NONE);
#endif
            }
            else
            {
                // x is NaN
#ifdef WINDOWS
                return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32, AMD_F_NONE);
#else
                if (checkbits.u32 & QNAN_MASK_32)
                    return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32, AMD_F_NONE);
                else
                    return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32, AMD_F_INVALID);
#endif
            }
        }
        else
            return x;
    }

    sign =  checkbits.u32 & SIGNBIT_SP32;
    val_2p23.u32 = sign | EXP_VAL_23_F32;

    // By default MASKING flags are all set to 1, so all exceptions are masked.
    // When an an exception is masked, the processor simply sets the exception flag and continues the operation.
    // NOTE: If the exception is unmasked, the flag is also set but additionally an exception handler is invoked.
    /*
    mxscr_stat = _mm_getcsr();
    if(unlikely( (mxscr_stat & _MM_MASK_UNDERFLOW) != _MM_MASK_UNDERFLOW))
    {
        _mm_setcsr(mxscr_stat | _MM_MASK_UNDERFLOW); // _MM_MASK_UNDERFLOW = 0x00000800
        val_2p23.f32 = (x + val_2p23.f32) - val_2p23.f32;
        mxscr_stat = mxscr_stat & 0xffffffc0; // clear the exceptions
        _mm_setcsr(mxscr_stat);
    }
    */
    // This extra line is to take care of denormals and various rounding modes
    val_2p23.u32 = ((val_2p23.u32 << 1) >> 1) | sign;

    return (val_2p23.f32);
}
