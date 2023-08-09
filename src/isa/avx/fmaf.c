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

#include "libm/amd_funcs_internal.h"
#include "libm_util_amd.h"

float ALM_PROTO_BAS64(fmaf)(float a, float b, float c)
{
    double dc, dp, dr;
    long long ir;
    UT32 aa, bb, cc, result;
    int ae,be,ce;
    union { double d; long long l; } dru, iru;
    int spcl, spcl2;
    
    aa.f32 = a;
    bb.f32 = b;
    cc.f32 = c;
    
    ae = ((aa.u32 & 0x7f800000) >> 23) - 127;
    be = ((bb.u32 & 0x7f800000) >> 23) - 127;
    ce = ((cc.u32 & 0x7f800000) >> 23) - 127;
    
    spcl = ( (ae == 128) || (be == 128) || (ce == 128));
    if (spcl)
    {
        spcl2 = ((ce == 128) && (ae != 128) && (be != 128));
        if(((((aa.u32 & 0x7fffffff) == 0) && ((bb.u32 & 0x7fffffff) == 0x7f800000)) ||
            (((bb.u32 & 0x7fffffff) == 0) && ((aa.u32 & 0x7fffffff) == 0x7f800000))) &&
             ((cc.u32 & 0x7fffffff) > 0x7f800000))
            result.f32 = c+c;        
        else
            result.f32 = spcl2?(c+c):(a * b + c);
        return result.f32;
        //return _handle_error("fma", __amd_CodeFma, result.u64, DOMAIN, AMD_F_INVALID, EDOM, aa, 0.0, 1);
    }   
    
    if(((aa.u32 & 0x7fffffff) == 0) ||
       ((bb.u32 & 0x7fffffff) == 0) ||
       ((cc.u32 & 0x7fffffff) == 0) )
    {
        return a * b + c;
    }
    
    dp = (double)a * (double)b;
    dc = c;
    dr = dp + dc;
    dru.d = dr;
    ir = dru.l;

    if ((ir & 0x000000001fffffffL) == 0x0000000010000000L) {
        int ec = ((cc.u32 >> 23) & 0xff) - 127;
        int ep = ((aa.u32 >> 23) & 0xff) + ((bb.u32 >> 23) & 0xff) - 2*127;
        long long jr, ir52 = (ir & 0x7ff0000000000000L) - 0x0340000000000000L;

    jr = ir52 | (long long)(dr - dc > dp) << 63;
    ir = (ec - ep > 53 - 48) & (dr - dc != dp) ? jr : 0L;

        jr = ir52 | (long long)(dr - dp > dc) << 63;
    ir = (ep - ec > 53 - 24) & (dr - dp != dc) ? jr : ir;

    iru.l = ir;
    dr += iru.d;
    }

    return (float)dr;
}


