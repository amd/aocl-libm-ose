#include "libm_amd.h"
#include "libm/amd_funcs_internal.h"
#include "libm_util_amd.h"

float FN_PROTOTYPE_REF_BAS64(fmaf)(float a, float b, float c)
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


