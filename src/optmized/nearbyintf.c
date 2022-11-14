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
#include <libm/typehelper.h>

float ALM_PROTO_OPT(nearbyintf)(float x)
{
    uint32_t checkbits_u32;
    uint32_t sign;
    float val_2p23_f32;
    checkbits_u32 = asuint32(x);

    // Clear the sign bit and check if the value can be rounded(i.e check if exponent less than 23)
    if((checkbits_u32 & POS_BITSET_F32) > EXP_VAL_23_F32)
    {
        // Check nan or inf
        if((checkbits_u32 & EXPBITS_SP32) == EXPBITS_SP32)
        {
            if((checkbits_u32 & MANTBITS_SP32) == POS_ZERO_F32)
            {
                // x is Inf
#ifdef WINDOWS
                return  __alm_handle_errorf(checkbits_u32, AMD_F_INVALID);
#else
                return  __alm_handle_errorf(checkbits_u32, AMD_F_NONE);
#endif
            }
            else
            {
                // x is NaN
#ifdef WINDOWS
                return  __alm_handle_errorf(checkbits_u32 | QNAN_MASK_32, AMD_F_NONE);
#else
                if(checkbits_u32 & QNAN_MASK_32)
                    return  __alm_handle_errorf(checkbits_u32 | QNAN_MASK_32, AMD_F_NONE);
                else
                    return  __alm_handle_errorf(checkbits_u32 | QNAN_MASK_32, AMD_F_INVALID);
#endif
            }
        }
        else
            return x;
    }

    sign =  checkbits_u32 & SIGNBIT_SP32;

    val_2p23_f32 = asfloat(sign | EXP_VAL_23_F32);
    val_2p23_f32 = (x + val_2p23_f32) - val_2p23_f32;
    val_2p23_f32 = asfloat(asuint32(val_2p23_f32) | sign);

    return val_2p23_f32;
}
