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

/*
* ISO-IEC-10967-2: Elementary Numerical Functions
* Signature:
*   double acosh(double x)
*
* Spec:
*   acosh(+inf) = +inf
*   acosh(-inf) = nan
*   acosh(0)    = nan
*   acosh(nan)  = nan
*
********************************************
* Implementation Notes
* ---------------------
* To compute acosh(double x)
* Let x = |x|
*
* Based on the value of x, acosh(x) is calculated as,
* 1. If x < 1.0, return NAN

* 2. If x == 1.0 return 0.0

* 3. If x greater than 1/sqrt(epsilon) in magnitude are
        approximated by acosh(x) = ln(2) + ln(x).
        log_kernel_amd(x) returns xexp, r1, r2 such that
        log(x) = xexp*log(2) + r1 + r2.
        Add (xexp+1) * log(2) to z1,z2 to get the result acosh(x).
        The computed r1 is not subject to rounding error because
        (xexp+1) has at most 10 significant bits, log(2) has 24 significant
        bits, and r1 has up to 24 bits; and the exponents of r1
        and r2 differ by at most 6.

* 4. If 128.0 <= x <= 1/sqrt(epsilon)
        acosh for these arguments is approximated by
        acosh(x) = ln(x + sqrt(x*x-1))

* 5. If 1.0 < x <= 128.0
        5.1. If 1.5 <= x <= 128.0
                We use minimax polynomials,
                based on Abramowitz and Stegun 4.6.32 series
                expansion for acosh(x), with the log(2x) and 1/(2.2.x^2)
                terms removed. We compensate for these two terms later.
                5.1.1.	32.0 <= x <= 128.0
                        p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 /
                                d0 + d1*r + d2*r^2
                5.1.2.	for 8.0 <= x <= 32.0
                        p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 /
                                d1*r + d2*r^2 + d3*r^3 + d4*r^4
                5.1.3.	for 4.0 <= x <= 8.0
                        p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 + c5*r^5 /
                                d1*r + d2*r^2 + d3*r^3 + d4*r^4
                5.1.4.	for 2.0 <= x <= 4.0
                        p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 + c5*r^5 + c6*r^6 /
                                d1*r + d2*r^2 + d3*r^3 + d4*r^4 + c5*r^5 + c6*r^6
                5.1.5.	for 1.75 <= x <= 2.0
                        p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 + c5*r^5 + c6*r^6 /
                                d1*r + d2*r^2 + d3*r^3 + d4*r^4 + c5*r^5
                5.1.6.	for 1.5 <= x <= 1.75
                        p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 + c5*r^5 + c6*r^6 /
                                d1*r + d2*r^2 + d3*r^3 + d4*r^4 + c5*r^5

            Now (r1,r2) sum to log(2x). Subtract the term
            1/(2.2.x^2) = 0.25/t, and add poly/t, carefully
            to maintain precision. (Note that we add poly/t
            rather than poly because of the *x factor used
            when generating the minimax polynomial)

            For 1.0 <= x <= 1.5. It is hard to maintain accuracy here so
            we have to go to great lengths to do so.
            We compute the value
            t = x - 1.0 + sqrt(2.0*(x - 1.0) + (x - 1.0)*(x - 1.0))
            using simulated quad precision.

        5.2.If 1.0 <= x < 1.13 implies r <= 0.656. In this region
            we need to take extra care to maintain precision.
            We have t = r1 + r2 = (x - 1.0 + sqrt(x*x-1.0))
            to more than basic precision. We use the Taylor series
            for log(1+x), with terms after the O(x*x) term
            approximated by a [6,6] minimax polynomial.

            p(r) = c1*r + c2*r^2 + c3*r^3 + c4*r^4 + c5*r^5 + c6*r^6 + c7*r^7 + c8*r^8 /
                       d1*r + d2*r^2 + d3*r^3 + d4*r^4 + c5*r^5 + c6*r^6 + c7*r^7

            Now we can compute the result r = acosh(x) = log1p(t)
            using the formula t - 0.5*t*t + poly*t*t. Since t is
            represented as r1+r2, the formula becomes
            r = r1+r2 - 0.5*(r1+r2)*(r1+r2) + poly*(r1+r2)*(r1+r2).
            Expanding out, we get
            r = r1 + r2 - (0.5 + poly)*(r1*r1 + 2*r1*r2 + r2*r2)
            and ignoring negligible quantities we get
            r = r1 + r2 - 0.5*r1*r1 + r1*r2 + poly*t*t

            For 1.06 <= x <= 1.13 we must evaluate in extended precision
            to reach about 1 ulp accuracy (in this range the simple code
            above only manages about 1.5 ulp accuracy)
            Split poly, r1 and r2 into head and tail sections

        5.3.For arguments 1.13 <= x <= 1.5 the log1p function is good enough
*/


#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include "libm_inlines_amd.h"
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include "kern/sqrt_pos.c"

static struct {
    const uint64_t recrteps;
    const double log2_lead, log2_tail;
    double poly_acosh_A[4], poly_acosh_B[3], poly_acosh_C[4], poly_acosh_D[4], poly_acosh_E[5], \
           poly_acosh_F[4], poly_acosh_G[6], poly_acosh_H[6], poly_acosh_I[6], poly_acosh_J[5], \
           poly_acosh_K[6], poly_acosh_L[5], poly_acosh_M[8], poly_acosh_N[7];
} acosh_data = {
                  .recrteps = 0x4196a09e667f3bcd, /* 1/sqrt(eps) = 9.49062656242515593767e+07 */

                  /* log2_lead and log2_tail sum to an extra-precise version of log(2) */
                  .log2_lead = 6.93147122859954833984e-01, /* 0x3fe62e42e0000000 */
                  .log2_tail = 5.76999904754328540596e-08, /* 0x3e6efa39ef35793c */
                  .poly_acosh_A = {
                       0.45995704464157438175e-9,
                       -0.89080839823528631030e-9,
                       -0.10370522395596168095e-27,
                       0.35255386405811106347e-32,
                   },
                   .poly_acosh_B = {
                       0.21941191335882074014e-8,
                       -0.10185073058358334569e-7,
                       0.95019562478430648685e-8,
                   },
                   .poly_acosh_C = {
                       -0.54903656589072526589e-10,
                       0.27646792387218569776e-9,
                       -0.26912957240626571979e-9,
                       0.86712268396736384286e-29,
                   },
                   .poly_acosh_D = {
                       -0.24327683788655520643e-9,
                       0.20633757212593175571e-8,
                       -0.45438330985257552677e-8,
                       0.28707154390001678580e-8,
                   },
                   .poly_acosh_E = {
                       -0.20827370596738166108e-6,
                       0.10232136919220422622e-5,
                       -0.98094503424623656701e-6,
                       -0.11615338819596146799e-18,
                       0.44511847799282297160e-21,
                   },
                   .poly_acosh_F = {
                       -0.92579451630913718588e-6,
                       0.76997374707496606639e-5,
                       -0.16727286999128481170e-4,
                       0.10463413698762590251e-4,
                   },
                   .poly_acosh_G = {
                       -0.122195030526902362060e-7,
                       0.157894522814328933143e-6,
                       -0.579951798420930466109e-6,
                       0.803568881125803647331e-6,
                       -0.373906657221148667374e-6,
                       0.317856399083678204443e-21,
                   },
                   .poly_acosh_H = {
                       -0.516260096352477148831e-7,
                       0.894662592315345689981e-6,
                       -0.475662774453078218581e-5,
                       0.107249291567405130310e-4,
                       -0.107871445525891289759e-4,
                       0.398833767702587224253e-5,
                   },
                   .poly_acosh_I = {
                       0.1437926821253825186e-3,
                       -0.1034078230246627213e-2,
                       0.2015310005461823437e-2,
                       -0.1159685218876828075e-2,
                       -0.9267353551307245327e-11,
                       0.2880267770324388034e-12,
                   },
                   .poly_acosh_J = {
                       0.6305521447028109891e-3,
                       -0.6816525887775002944e-2,
                       0.2228081831550003651e-1,
                       -0.2836886105406603318e-1,
                       0.1236997707206036752e-1,
                   },
                   .poly_acosh_K = {
                       0.7471936607751750826e-3,
                       -0.4849405284371905506e-2,
                       0.8823068059778393019e-2,
                       -0.4825395461288629075e-2,
                       -0.1001984320956564344e-8,
                       0.4299919281586749374e-10,
                   },
                   .poly_acosh_L = {
                       0.3322359141239411478e-2,
                       -0.3293525930397077675e-1,
                       0.1011351440424239210e0,
                       -0.1227083591622587079e0,
                       0.5147099404383426080e-1,
                   },
                   .poly_acosh_M = {
                       0.30893760556597282162e-21,
                       0.10513858797132174471e0,
                       0.27834538302122012381e0,
                       0.27223638654807468186e0,
                       0.12038958198848174570e0,
                       0.23357202004546870613e-1,
                       0.15208417992520237648e-2,
                       0.72741030690878441996e-7,
                   },
                   .poly_acosh_N = {
                       0.31541576391396523486e0,
                       0.10715979719991342022e1,
                       0.14311581802952004012e1,
                       0.94928647994421895988e0,
                       0.32396235926176348977e0,
                       0.52566134756985833588e-1,
                       0.30477895574211444963e-2,
                   },
};

#define recrteps  acosh_data.recrteps
#define log2_lead acosh_data.log2_lead
#define log2_tail acosh_data.log2_tail

#define A1 acosh_data.poly_acosh_A[0]
#define A2 acosh_data.poly_acosh_A[1]
#define A3 acosh_data.poly_acosh_A[2]
#define A4 acosh_data.poly_acosh_A[3]

#define B1 acosh_data.poly_acosh_B[0]
#define B2 acosh_data.poly_acosh_B[1]
#define B3 acosh_data.poly_acosh_B[2]

#define C1 acosh_data.poly_acosh_C[0]
#define C2 acosh_data.poly_acosh_C[1]
#define C3 acosh_data.poly_acosh_C[2]
#define C4 acosh_data.poly_acosh_C[3]

#define D1 acosh_data.poly_acosh_D[0]
#define D2 acosh_data.poly_acosh_D[1]
#define D3 acosh_data.poly_acosh_D[2]
#define D4 acosh_data.poly_acosh_D[3]

#define E1 acosh_data.poly_acosh_E[0]
#define E2 acosh_data.poly_acosh_E[1]
#define E3 acosh_data.poly_acosh_E[2]
#define E4 acosh_data.poly_acosh_E[3]
#define E5 acosh_data.poly_acosh_E[4]

#define F1 acosh_data.poly_acosh_F[0]
#define F2 acosh_data.poly_acosh_F[1]
#define F3 acosh_data.poly_acosh_F[2]
#define F4 acosh_data.poly_acosh_F[3]

#define G1 acosh_data.poly_acosh_G[0]
#define G2 acosh_data.poly_acosh_G[1]
#define G3 acosh_data.poly_acosh_G[2]
#define G4 acosh_data.poly_acosh_G[3]
#define G5 acosh_data.poly_acosh_G[4]
#define G6 acosh_data.poly_acosh_G[5]

#define H1 acosh_data.poly_acosh_H[0]
#define H2 acosh_data.poly_acosh_H[1]
#define H3 acosh_data.poly_acosh_H[2]
#define H4 acosh_data.poly_acosh_H[3]
#define H5 acosh_data.poly_acosh_H[4]
#define H6 acosh_data.poly_acosh_H[5]

#define I1 acosh_data.poly_acosh_I[0]
#define I2 acosh_data.poly_acosh_I[1]
#define I3 acosh_data.poly_acosh_I[2]
#define I4 acosh_data.poly_acosh_I[3]
#define I5 acosh_data.poly_acosh_I[4]
#define I6 acosh_data.poly_acosh_I[5]

#define J1 acosh_data.poly_acosh_J[0]
#define J2 acosh_data.poly_acosh_J[1]
#define J3 acosh_data.poly_acosh_J[2]
#define J4 acosh_data.poly_acosh_J[3]
#define J5 acosh_data.poly_acosh_J[4]

#define K1 acosh_data.poly_acosh_K[0]
#define K2 acosh_data.poly_acosh_K[1]
#define K3 acosh_data.poly_acosh_K[2]
#define K4 acosh_data.poly_acosh_K[3]
#define K5 acosh_data.poly_acosh_K[4]
#define K6 acosh_data.poly_acosh_K[5]

#define L1 acosh_data.poly_acosh_L[0]
#define L2 acosh_data.poly_acosh_L[1]
#define L3 acosh_data.poly_acosh_L[2]
#define L4 acosh_data.poly_acosh_L[3]
#define L5 acosh_data.poly_acosh_L[4]

#define M1 acosh_data.poly_acosh_M[0]
#define M2 acosh_data.poly_acosh_M[1]
#define M3 acosh_data.poly_acosh_M[2]
#define M4 acosh_data.poly_acosh_M[3]
#define M5 acosh_data.poly_acosh_M[4]
#define M6 acosh_data.poly_acosh_M[5]
#define M7 acosh_data.poly_acosh_M[6]
#define M8 acosh_data.poly_acosh_M[7]

#define N1 acosh_data.poly_acosh_N[0]
#define N2 acosh_data.poly_acosh_N[1]
#define N3 acosh_data.poly_acosh_N[2]
#define N4 acosh_data.poly_acosh_N[3]
#define N5 acosh_data.poly_acosh_N[4]
#define N6 acosh_data.poly_acosh_N[5]
#define N7 acosh_data.poly_acosh_N[6]

double ALM_PROTO_OPT(acosh)(double x) {
    uint64_t ux;
    double _r, rarg, _r1, _r2;
    int32_t xexp;
    uint64_t xneg, xinf_nan;

    ux = asuint64(x);

    /* get sign of x */
    xneg = (ux & SIGNBIT_DP64);

    /* if expbits are all set */
    xinf_nan = ((ux & EXPBITS_DP64) == EXPBITS_DP64);

    /* special case checks */
    if (xinf_nan) {/* x is either NaN or infinity */
        if (unlikely(x != x)) { /* if x is NaN */
#ifdef WINDOWS
            return __alm_handle_error(ux|0x0008000000000000, AMD_F_NONE);
#else
            return __alm_handle_error(ux|0x0008000000000000, AMD_F_INVALID);
#endif
        }

        else { /* x is infinity */
			if (xneg) // negative infinity return nan raise error
			    return __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
			return x;
        }
    }

    else if ((xneg) || (ux <= 0x3ff0000000000000)) {
        /* x <= 1.0 */
        if (ux == 0x3ff0000000000000)
            return 0.0; /* x = 1.0; return zero. */
        /* if x < 1.0, return nan */
        return __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
    }


    if (ux > recrteps) {
        /* Arguments greater than 1/sqrt(epsilon) in magnitude are
        approximated by acosh(x) = ln(2) + ln(x) */
        /* log_kernel_amd(x) returns xexp, r1, r2 such that
        log(x) = xexp*log(2) + r1 + r2 */
        log_kernel_amd64(x, ux, &xexp, &_r1, &_r2);
        /* Add (xexp+1) * log(2) to z1,z2 to get the result acosh(x).
        The computed r1 is not subject to rounding error because
        (xexp+1) has at most 10 significant bits, log(2) has 24 significant
        bits, and r1 has up to 24 bits; and the exponents of r1
        and r2 differ by at most 6. */
        _r1 = ((xexp+1) * log2_lead + _r1);
        _r2 = ((xexp+1) * log2_tail + _r2);
        return _r1 + _r2;
    }

    else if (ux >= 0x4060000000000000) {
        /* 128.0 <= x <= 1/sqrt(epsilon) */
        /* acosh for these arguments is approximated by
        acosh(x) = ln(x + sqrt(x*x-1)) */
        rarg = (x * x) - 1.0;
        /* Use assembly instruction to compute _r = sqrt(rarg); */
        //ASMSQRT(rarg,_r);
        _r = ALM_PROTO_KERN(sqrt)(rarg);
        _r += x;

        ux = asuint64(_r);

        log_kernel_amd64(_r, ux, &xexp, &_r1, &_r2);
        _r1 = (xexp * log2_lead + _r1);
        _r2 = (xexp * log2_tail + _r2);
        return _r1 + _r2;
    }

    else {
        /* 1.0 < x <= 128.0 */
        double u1, u2, v1, v2, w1, w2, hx, tx, t, r, s, p1, p2, a1, a2, c1, c2,poly;
        if (ux >= 0x3ff8000000000000) {
            /* 1.5 <= x <= 128.0 */
            /* We use minimax polynomials,
             based on Abramowitz and Stegun 4.6.32 series
             expansion for acosh(x), with the log(2x) and 1/(2.2.x^2)
             terms removed. We compensate for these two terms later.
            */
            t = x * x;
            if (ux >= 0x4040000000000000) {
            /* [3,2] for 32.0 <= x <= 128.0 */
                poly = POLY_EVAL_4(t, A1, A2, A3, A4) /
                      (0.21941191335882074014e-8 +
                      (-0.10185073058358334569e-7 +
                         0.95019562478430648685e-8 * t) * t);
            }
            else if (ux >= 0x4020000000000000) {
            /* [3,3] for 8.0 <= x <= 32.0 */
                poly = POLY_EVAL_4(t, C1, C2, C3, C4)/
                    POLY_EVAL_4(t, D1, D2, D3, D4);
            }
            else if (ux >= 0x4010000000000000) {
            /* [4,3] for 4.0 <= x <= 8.0 */
                poly = POLY_EVAL_5(t, E1, E2, E3, E4, E5) /
                     POLY_EVAL_4(t, F1, F2, F3, F4);
            }
            else if (ux >= 0x4000000000000000) {
            /* [5,5] for 2.0 <= x <= 4.0 */
                poly = POLY_EVAL_6(t, G1, G2, G3, G4, G5, G6) /
                      POLY_EVAL_6(t, H1, H2, H3, H4, H5, H6);
            }
            else if (ux >= 0x3ffc000000000000) {
              /* [5,4] for 1.75 <= x <= 2.0 */
                poly = POLY_EVAL_6(t, I1, I2, I3, I4, I5, I6) /
                      POLY_EVAL_5(t, J1, J2, J3, J4, J5);
            }
            else {
              /* [5,4] for 1.5 <= x <= 1.75 */
                poly = POLY_EVAL_6(t, K1, K2, K3, K4, K5, K6) /
                      POLY_EVAL_5(t, L1, L2, L3, L4, L5);
            }

            ux = asuint64(x);

            log_kernel_amd64(x, ux, &xexp, &_r1, &_r2);

            _r1 = ((xexp + 1) * log2_lead + _r1);
            _r2 = ((xexp + 1) * log2_tail + _r2);
             /* Now (r1,r2) sum to log(2x). Subtract the term
             1/(2.2.x^2) = 0.25/t, and add poly/t, carefully
             to maintain precision. (Note that we add poly/t
             rather than poly because of the *x factor used
             when generating the minimax polynomial) */
            v2 = (poly - 0.25) / t;
            r = v2 + _r1;
            s = ((_r1 - r) + v2) + _r2;
            v1 = r + s;
            return v1 + ((r - v1) + s);
        }

         /* Here 1.0 <= x <= 1.5. It is hard to maintain accuracy here so
         we have to go to great lengths to do so. */

         /* We compute the value
           t = x - 1.0 + sqrt(2.0*(x - 1.0) + (x - 1.0)*(x - 1.0))
           using simulated quad precision. */
         t = x - 1.0;
         u1 = t * 2.0;

         /* dekker_mul12(t,t,&v1,&v2); */
         ux = asuint64(t);
         ux &= 0xfffffffff8000000;

         hx = asdouble(ux);
         tx = t - hx;
         v1 = t * t;
         v2 = (((hx * hx - v1) + hx * tx) + tx * hx) + tx * tx;

         /* dekker_add2(u1,0.0,v1,v2,&w1,&w2); */
         r = u1 + v1;
         s = (((u1 - r) + v1) + v2);
         w1 = r + s;
         w2 = (r - w1) + s;

         /* dekker_sqrt2(w1,w2,&u1,&u2); */
         //ASMSQRT(w1,p1);
         p1 = ALM_PROTO_KERN(sqrt)(w1);

         ux = asuint64(p1);
         ux &= 0xfffffffff8000000;
         c1 = asdouble(ux);

         c2 = p1 - c1;
         a1 = p1 * p1;
         a2 = (((c1 * c1 - a1) + c1 * c2) + c2 * c1) + c2 * c2;
         p2 = (((w1 - a1) - a2) + w2) * 0.5 / p1;
         u1 = p1 + p2;
         u2 = (p1 - u1) + p2;

         /* dekker_add2(u1,u2,t,0.0,&v1,&v2); */
         r = u1 + t;
         s = (((u1 - r) + t)) + u2;
         _r1 = r + s;
         _r2 = (r - _r1) + s;
         t = _r1 + _r2;
         /* Check for x close to 1.0. */
         if (x < 1.13) {
             /* Here 1.0 <= x < 1.13 implies r <= 0.656. In this region
             we need to take extra care to maintain precision.
             We have t = r1 + r2 = (x - 1.0 + sqrt(x*x-1.0))
             to more than basic precision. We use the Taylor series
             for log(1+x), with terms after the O(x*x) term
             approximated by a [6,6] minimax polynomial. */
            double b1, b2, _c1, _c2, e1, e2, q1, q2, c, cc, hr1, tr1, hpoly, tpoly, hq1, tq1, hr2, tr2;

            poly = POLY_EVAL_8(t, M1, M2, M3, M4, M5, M6, M7, M8) /
                       POLY_EVAL_7(t, N1, N2, N3, N4, N5, N6, N7);

            /* Now we can compute the result r = acosh(x) = log1p(t)
             using the formula t - 0.5*t*t + poly*t*t. Since t is
             represented as r1+r2, the formula becomes
             r = r1+r2 - 0.5*(r1+r2)*(r1+r2) + poly*(r1+r2)*(r1+r2).
             Expanding out, we get
               r = r1 + r2 - (0.5 + poly)*(r1*r1 + 2*r1*r2 + r2*r2)
             and ignoring negligible quantities we get
               r = r1 + r2 - 0.5*r1*r1 + r1*r2 + poly*t*t
            */
            if (x < 1.06) {
                double b, _c, e;
                b = _r1 * _r2;
                _c = 0.5 * _r1 * _r1;
                e = poly * t * t;
                /* N.B. the order of additions and subtractions is important */
                r = (((_r2 - b) + e) - _c) + _r1;
                return r;
            }
            else {
                /* For 1.06 <= x <= 1.13 we must evaluate in extended precision
                 to reach about 1 ulp accuracy (in this range the simple code
                 above only manages about 1.5 ulp accuracy) */
               /* Split poly, r1 and r2 into head and tail sections */
                ux = asuint64(poly);

                ux &= 0xfffffffff8000000;
                hpoly = asdouble(ux);

                tpoly = poly - hpoly;
                ux = asuint64(_r1);
                ux &= 0xfffffffff8000000;
                hr1 = asdouble(ux);

                tr1 = _r1 - hr1;
                ux = asuint64(_r2);
                ux &= 0xfffffffff8000000;

                hr2 = asdouble(ux);

                tr2 = _r2 - hr2;

                /* e = poly*t*t */
                c = poly * _r1;
                cc = (((hpoly * hr1 - c) + hpoly * tr1) + tpoly * hr1) + tpoly * tr1;
                cc = poly * _r2 + cc;
                q1 = c + cc;
                q2 = (c - q1) + cc;
                ux = asuint64(q1);

                ux &= 0xfffffffff8000000;
                hq1 = asdouble(ux);
                tq1 = q1 - hq1;
                c = q1 * _r1;
                cc = (((hq1 * hr1 - c) + hq1 * tr1) + tq1 * hr1) + tq1 * tr1;
                cc = q1 * _r2 + q2 * _r1 + cc;
                e1 = c + cc;
                e2 = (c - e1) + cc;

                /* b = r1*r2 */
                b1 = _r1 * _r2;
                b2 = (((hr1 * hr2 - b1) + hr1 * tr2) + tr1 * hr2) + tr1 * tr2;

                /* c = 0.5*r1*r1 */
                _c1 = (0.5*_r1) * _r1;
                _c2 = (((0.5*hr1 * hr1 - _c1) + 0.5*hr1 * tr1) + 0.5*tr1 * hr1) + 0.5*tr1 * tr1;

                /* v = a + d - b */
                r = _r1 - b1;
                s = (((_r1 - r) - b1) - b2) + _r2;
                v1 = r + s;
                v2 = (r - v1) + s;

                /* w = (a + d - b) - c */
                r = v1 - _c1;
                s = (((v1 - r) - _c1) - _c2) + v2;
                w1 = r + s;
                w2 = (r - w1) + s;
                /* u = ((a + d - b) - c) + e */
                r = w1 + e1;
                s = (((w1 - r) + e1) + e2) + w2;
                u1 = r + s;
                u2 = (r - u1) + s;

                /* The result r = acosh(x) */
                r = u1 + u2;
                return r;
            }
        }
        else {
              /* For arguments 1.13 <= x <= 1.5 the log1p function
              is good enough */
            return ALM_PROTO_FMA3(log1p)(t);
        }
    }
}
