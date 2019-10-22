#include "libm_amd.h"
#include "libm/amd_funcs_internal.h"
#include "libm_util_amd.h"
#include "libm_special.h"
#include "libm_errno_amd.h"

double FN_PROTOTYPE_REF_BAS64(fma)(double aa, double bb, double cc)
{
    UT64 a, b, c, result;
    UT64 ah, at, bh, bt, am, bm, ps;
    UT64 p, r, t, z, pt, v, w, rr, vv, ww;
    UT64 rd, vd, wd, rrd, rtd, rid, zd, vwd;
    UT64 two52, twom53;
    long long ae, be, pe, ce, ze, rndc, spcl, spcl2, dnm;
    long long re, dnmrndc;

    two52.u64 = 0x4330000000000000;
    twom53.u64 = 0x3ca0000000000000;

    a.f64 = aa;
    b.f64 = bb;
    c.f64 = cc;

    ae = ((a.u64 & 0x7ff0000000000000) >> 52) - 1023;
    be = ((b.u64 & 0x7ff0000000000000) >> 52) - 1023;
    ce = ((c.u64 & 0x7ff0000000000000) >> 52) - 1023;


    /* special cases flags */
    spcl = ( (ae == 1024) || (be == 1024) || (ce == 1024));
    if (spcl)
    {
        spcl2 = ((ce == 1024) && (ae != 1024) && (be != 1024));
        if(((((a.u64 & 0x7fffffffffffffff) == 0) && ((b.u64 & 0x7fffffffffffffff) == 0x7ff0000000000000)) ||
            (((b.u64 & 0x7fffffffffffffff) == 0) && ((a.u64 & 0x7fffffffffffffff) == 0x7ff0000000000000))) &&
             ((c.u64 & 0x7fffffffffffffff) > 0x7ff0000000000000))
             result.f64 = cc+cc;        
        else
             result.f64 = spcl2?(cc+cc):(aa * bb + cc);
        return result.f64;
        //return _handle_error("fma", __amd_fma, result.u64, _DOMAIN, AMD_F_INVALID, EDOM, aa, 0.0, 1);
    }            
    if(((a.u64 & 0x7fffffffffffffff) == 0) ||
       ((b.u64 & 0x7fffffffffffffff) == 0) ||
       ((c.u64 & 0x7fffffffffffffff) == 0) )
    {
        return aa * bb + cc;
    }
        
    if((ce == 1024) && (ae != 1024) && (be != 1024))
    {
        return cc;
    }
    
    /* Regularize denormals */
    if(ae == -1023) { a.f64 *= two52.f64; ae = ((a.u64 & 0x7ff0000000000000) >> 52) - 1075; }
    if(be == -1023) { b.f64 *= two52.f64; be = ((b.u64 & 0x7ff0000000000000) >> 52) - 1075; }
    if(ce == -1023) { c.f64 *= two52.f64; ce = ((c.u64 & 0x7ff0000000000000) >> 52) - 1075; }

    pe = ae + be;
    
    ps.u64 = (a.u64 & 0x8000000000000000) ^ (b.u64 & 0x8000000000000000);

    /* Normalize a, b */
    a.u64 = (a.u64 & 0x000fffffffffffff) | 0x3ff0000000000000;
    b.u64 = (b.u64 & 0x000fffffffffffff) | 0x3ff0000000000000;

    /* Get head & tail parts of a, b */
    am.u64 = a.u64;
    am.f64 -= 1.0;
    ah.u64 = am.u64 & 0xfffffffff8000000;
    at.f64 = am.f64 - ah.f64;
    bm.u64 = b.u64;
    bm.f64 -= 1.0;
    bh.u64 = bm.u64 & 0xfffffffff8000000;
    bt.f64 = bm.f64 - bh.f64;

     /* Get head & tail parts of the product a*b */
    p.f64 = a.f64 * b.f64;
    pt.f64 = (((((((1.0 - p.f64) + am.f64) + bm.f64) + (ah.f64 * bh.f64)) + (ah.f64 * bt.f64)) + (at.f64 * bh.f64)) + (at.f64 * bt.f64));

    p.u64 = ((p.u64 & 0x8000000000000000) ^ ps.u64) | (p.u64 & 0x7fffffffffffffff);
    pt.u64 = ((pt.u64 & 0x8000000000000000) ^ ps.u64) | (pt.u64 & 0x7fffffffffffffff);

    /* store in temp variables */
    r.u64 = p.u64;
    v.u64 = pt.u64;
    w.u64 = c.u64;

    /* normalize c */
    c.u64 = ((c.u64 & 0x7fffffffffffffff) == 0) ? c.u64 : ((c.u64 & 0x800fffffffffffff) | ((ce - pe + 1023) << 52));

    /* carefully add p and c; these steps valid only when pe and ce are not far apart */
    rr.f64 = p.f64 + c.f64;
    t.f64 = ( (p.u64 & 0x7fffffffffffffff) > (c.u64 & 0x7fffffffffffffff) ) ?
                ( (p.f64 - rr.f64) + c.f64 ) : ( (c.f64 - rr.f64) + p.f64 );
    vv.f64 = t.f64 + pt.f64;
    ww.f64 = ( (t.u64 & 0x7fffffffffffffff) > (pt.u64 & 0x7fffffffffffffff) ) ?
                ( (t.f64 - vv.f64) + pt.f64 ) : ( (pt.f64 - vv.f64) + t.f64 );

    /* pick r,v,w based on how far apart pe and ce are */
    /* number 115 is safe; actual value close to 53+53+2 */

    r.u64 = (pe - ce < 115) ? rr.u64 : r.u64;
    v.u64 = (pe - ce < 115) ? vv.u64 : v.u64;
    w.u64 = (pe - ce < 115) ? ww.u64 : w.u64;
 
    /* additional drudgy stuff for denormals */
    
    rd.f64 = r.f64 + v.f64;
    t.f64 = (r.f64 - rd.f64) + v.f64;
    vd.f64 = t.f64 + w.f64;
    wd.f64 = (t.f64 - vd.f64) + w.f64;
    
    re = ((rd.u64 & 0x7ff0000000000000) >> 52) + pe;
    dnm = (re <= 0);
    rrd.u64 = (rd.u64 & (~(((long long)(0x1) << (1-re)) - 1)));
    rtd.f64 = rd.f64 - rrd.f64;
    vwd.f64 = vd.f64 + wd.f64;

    dnmrndc = ( ((rtd.u64 & 0x7ff0000000000000) == ((rrd.u64 & 0x7ff0000000000000) - ((long long)(52+re) << 52))) && 
            ((rtd.u64 & 0x000fffffffffffff) == 0) && ((vwd.u64 & 0x7fffffffffffffff) != 0) );
    rid.u64 = ((rtd.u64 & 0x8000000000000000) == (vwd.u64 & 0x8000000000000000)) ?
                (rrd.u64 & 0x8000000000000000) | ((rrd.u64 & 0x7ff0000000000000) - ((long long)(51+re) << 52)) : 0;
    zd.f64 = dnmrndc ? (rrd.f64 + rid.f64) : rd.f64;

    /* back to normals */
    /* identify if there was a rounding issue, and so correction is needed */
    rndc = ( (((r.u64 & 0x7ff0000000000000) - (v.u64 & 0x7ff0000000000000)) == 0x0350000000000000) &&
                ((v.u64 & 0x000fffffffffffff) == 0) && ((w.u64 & 0x7fffffffffffffff) != 0) );

    w.u64 = (w.u64 & 0x8000000000000000) | ((v.u64 & 0x7ff0000000000000) - 0x0340000000000000);
    v.f64 = rndc ? (v.f64 + w.f64) : v.f64;
    z.f64 = r.f64 + v.f64;

    z.f64 = dnm ? zd.f64 : z.f64;
    ze = ((z.u64 & 0x7ff0000000000000) >> 52) + pe;
    ze = dnm ? (ze+53) : ze;

    /* reconstruct return value */
    z.u64 = ((z.u64 & 0x7fffffffffffffff) == 0) ? z.u64 : (z.u64 & 0x800fffffffffffff) | ((ze << 52) & 0x7fffffffffffffff);
    z.f64 = dnm ? (z.f64 * twom53.f64) : z.f64;
    z.u64 = (ze <= 0) ? (z.u64 & 0x8000000000000000) : z.u64;
    z.u64 = (ze > 2046) ? ((z.u64 & 0x8000000000000000) | 0x7ff0000000000000) : z.u64;

    z.f64 = (ce - pe > 60) ? cc : z.f64;
    if((z.u64 & 0x7fffffffffffffff) == 0 )
        return __amd_handle_error("fma", __amd_fma, z.u64, _DOMAIN, AMD_F_UNDERFLOW, EDOM, aa, 0.0, 1);
    if((z.u64 & 0x7ff0000000000000) == 0x7ff0000000000000 )
        return __amd_handle_error("fma", __amd_fma, z.u64, _DOMAIN, AMD_F_OVERFLOW, EDOM, aa, 0.0, 1);

    //z.f64 = spcl ? (aa*bb + cc) : z.f64;
    //z.f64 = spcl2 ? cc : z.f64;
    return z.f64; 

}

