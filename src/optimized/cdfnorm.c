/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
  Signature:
  double amd_cdfnorm(double a)

  Computes the standard normal cumulative distribution function (CDF):
  
    Φ(a) = (1/√(2π)) ∫_{-∞}^{a} exp(-t²/2) dt
  
  This can be expressed in terms of the error function:
  
    Φ(a) = (1/2)[1 + erf(a/√2)]
         = (1/2) erfc(-a/√2)
  
  where erf(x) = (2/√π) ∫₀ˣ exp(-t²) dt
  and erfc(x) = 1 - erf(x)

  SPECIFICATION:
    Φ(+∞) = 1
    Φ(-∞) = 0
    Φ(NaN) = NaN
    Φ is monotonically increasing

  IMPLEMENTATION NOTES
  =======================

  The implementation uses a four-region approach optimized for accuracy across
  the entire double precision domain:
  
  Region 1 [a > -0.5]: Positive and small negative arguments
    Use transformation Φ(a) = (1/2)[1 + erf(a/√2)] with AMD's optimized erf.
  
  Region 2 [-1.5 < a ≤ -0.5]: Moderate negative arguments  
    Use transformation Φ(a) = (1/2)erfc(-a/√2) with AMD's optimized erfc.
    The transformation x = -a/√2 is computed in extended precision
    to minimize rounding error, then converted to double for the erfc call.
  
  Region 3 [-10 < a ≤ -1.5]: Critical negative range
    This region requires special handling due to precision challenges. We use:
    
    (a) Extended precision transformation: x = -a/√2
    
    (b) High-accuracy rational approximations: Piecewise rational functions
        R(z) = P(z)/Q(z) that approximate erfc(z).
        Four subintervals cover [0.5, 28]:
        - [0.5, 1.5]: degree (5,6) centered at z=0.5
        - [1.5, 2.5]: degree (5,5) centered at z=1.5  
        - [2.5, 4.5]: degree (5,5) centered at z=3.5
        - [4.5, 28]: degree (6,6) using 1/z transformation
    
    (c) Precise exp(-z²) evaluation using extended precision arithmetic.
  
  Region 4 [a ≤ -10]: Extreme negative tail
    Use asymptotic expansion from Abramowitz & Stegun 26.2.12:
    
      Φ(a) = φ(a)/|a| · [1 - 1/a² + 1·3/a⁴ - 1·3·5/a⁶ + ...]
      
    where φ(a) = (1/√(2π)) exp(-a²/2) is the standard normal PDF.
  
  PLATFORM-SPECIFIC IMPLEMENTATION:
    - Linux: Uses long double (80-bit extended precision) for Regions 3 & 4.
    - Windows: Uses double-double arithmetic (dd_t) since Windows long double
      is only 64-bit. This provides consistent accuracy across platforms.
  
  Saturation:
    - a ≥ 37.5: Returns 1.0
    - a ≤ -39: Returns 0.0 (underflow to zero in double precision)
*/

#include <math.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/typehelper.h>
#include <libm/types.h>
#include <libm_macros.h>
#include <libm_util_amd.h>
#include <libm/poly.h>

#if defined(_WIN32)

static struct {
  /* Basic constants */
  const double zero;
  const double half;
  const double one;
  const double sqrth;                       /* 1/√2 in double precision */
  const double asymptotic_threshold;        /* -10.0 */
  const double highprec_erfc_threshold;     /* -1.5 */
  const double round_to_one_cut;            /* 37.5 */
  const double underflow_to_zero_cut;       /* -39.0 */
  
  /* Double-double constants: 1/√2 */
  const double sqrth_dd_hi;
  const double sqrth_dd_lo;
  
  /* Double-double constants: 1/√(2π) */
  const double one_over_sqrt_2pi_hi;
  const double one_over_sqrt_2pi_lo;
  
  /* Taylor series coefficients for exp(r) */
  const double exp_taylor[7];
  
  /* Rational approximation coefficients (double precision) */
  const double erfc_0p5_1p5_Y;
  const double erfc_0p5_1p5_P[6];
  const double erfc_0p5_1p5_Q[7];
  
  const double erfc_1p5_2p5_Y;
  const double erfc_1p5_2p5_P[6];
  const double erfc_1p5_2p5_Q[6];
  
  const double erfc_2p5_4p5_Y;
  const double erfc_2p5_4p5_P[6];
  const double erfc_2p5_4p5_Q[6];
  
  const double erfc_4p5_28_Y;
  const double erfc_4p5_28_P[7];
  const double erfc_4p5_28_Q[7];
  
} cdfnorm_data = {
  .zero = 0x0p+0,
  .half = 0x1p-1,
  .one  = 0x1p+0,
  .sqrth = 0x1.6a09e667f3bcdp-1,
  .asymptotic_threshold = -0x1.4p+3,
  .highprec_erfc_threshold = -0x1.8p+0,
  .round_to_one_cut = 0x1.2cp+5,
  .underflow_to_zero_cut = -0x1.38p+5,
  
  .sqrth_dd_hi = 0x1.6a09e667f3bcdp-1,
  .sqrth_dd_lo = -0x1.bdd3413b26456p-55,
  
  .one_over_sqrt_2pi_hi = 0x1.9884533d436510p-2,
  .one_over_sqrt_2pi_lo = 0x1.4p-56,
  
  .exp_taylor = {
    0x1p+0, 0x1p+0, 0x1p-1, 0x1.5555555555555p-3,
    0x1.5555555555555p-5, 0x1.1111111111111p-7, 0x1.6c16c16c16c17p-10,
  },
  
  .erfc_0p5_1p5_Y = 0x1.9fadap-2,
  .erfc_0p5_1p5_P = {
    -0x1.91c770d9d3c38p-4, 0x1.6cc761c5cdfcfp-3, 0x1.872cf216e5f4bp-3,
    0x1.6c17f596974e2p-4, 0x1.3f917b098b969p-6, 0x1.d8f8dba7bf515p-10,
  },
  .erfc_0p5_1p5_Q = {
    0x1p0, 0x1.d8fbb46b1d05ep0, 0x1.6d20b0730d505p0, 0x1.27f68988b9f55p-1,
    0x1.fb4b28dd2f600p-4, 0x1.738a6d80ccf02p-7, 0x1.c5000f148a68ep-19,
  },
  
  .erfc_1p5_2p5_Y = 0x1.0371ep-1,
  .erfc_1p5_2p5_P = {
    -0x1.8ef3808bfbe3ap-6, 0x1.3ca7645df54c9p-5, 0x1.680608a7660760p-5,
    0x1.1fd54bd6a6245p-6, 0x1.a89fbecd94db8p-9, 0x1.ee97293c42ffap-13,
  },
  .erfc_1p5_2p5_Q = {
    0x1p0, 0x1.8a37ddbaa96d0p0, 0x1.f6fd9e749acf6p-1,
    0x1.4d8cee8caf21ap-2, 0x1.cdf6fb1d81fe3p-5, 0x1.0cf09d7005348p-8,
  },
  
  .erfc_2p5_4p5_Y = 0x1.14c64p-1,
  .erfc_2p5_4p5_P = {
    0x1.83066cd0bf15ap-9, 0x1.c22e68cf6b2c5p-7, 0x1.13840e0e2140cp-7,
    0x1.16f46da1af76bp-9, 0x1.066d54ccd32a6p-12, 0x1.7be0b780f7e4ep-17,
  },
  .erfc_2p5_4p5_Q = {
    0x1p0, 0x1.0acc2fcb1378ap0, 0x1.c538522309af6p-2,
    0x1.88993f34aac72p-4, 0x1.5b48e7dd47e9ap-7, 0x1.f6b2fb953735bp-12,
  },
  
  .erfc_4p5_28_Y = 0x1.1da64p-1,
  .erfc_4p5_28_P = {
    0x1.9b9a82141fdacp-8, 0x1.1f5bd4085c48dp-6, -0x1.b383063c1348dp-3,
    -0x1.601c882adf2bep-1, -0x1.46a330b7811e2p1, -0x1.9d17fc9bea3fbp1, -0x1.68a52784ed044p1,
  },
  .erfc_4p5_28_Q = {
    0x1p0, 0x1.65732e0a14960p1, 0x1.61d0ae6a5ca5ep3, 0x1.fdc7da2dc1122p3,
    0x1.6efce09ed824fp4, 0x1.b0349176f08c7p3, 0x1.5efb5c445a48fp2,
  },
};

#define SQRTH_DD          ((dd_t){cdfnorm_data.sqrth_dd_hi, cdfnorm_data.sqrth_dd_lo})
#define ONE_OVER_SQRT_2PI ((dd_t){cdfnorm_data.one_over_sqrt_2pi_hi, cdfnorm_data.one_over_sqrt_2pi_lo})
#define EXP_TAYLOR        cdfnorm_data.exp_taylor
#define ERFC_0P5_1P5_Y    cdfnorm_data.erfc_0p5_1p5_Y
#define ERFC_0P5_1P5_P    cdfnorm_data.erfc_0p5_1p5_P
#define ERFC_0P5_1P5_Q    cdfnorm_data.erfc_0p5_1p5_Q
#define ERFC_1P5_2P5_Y    cdfnorm_data.erfc_1p5_2p5_Y
#define ERFC_1P5_2P5_P    cdfnorm_data.erfc_1p5_2p5_P
#define ERFC_1P5_2P5_Q    cdfnorm_data.erfc_1p5_2p5_Q
#define ERFC_2P5_4P5_Y    cdfnorm_data.erfc_2p5_4p5_Y
#define ERFC_2P5_4P5_P    cdfnorm_data.erfc_2p5_4p5_P
#define ERFC_2P5_4P5_Q    cdfnorm_data.erfc_2p5_4p5_Q
#define ERFC_4P5_28_Y     cdfnorm_data.erfc_4p5_28_Y
#define ERFC_4P5_28_P     cdfnorm_data.erfc_4p5_28_P
#define ERFC_4P5_28_Q     cdfnorm_data.erfc_4p5_28_Q

#else /* Linux */
/*
 * Linux version: Use long double (80-bit extended precision).
 */
static struct {
  const double zero;
  const double half;
  const double one;
  const double sqrth;
  const long double sqrth_ld;
  const long double half_ld;
  const long double one_over_sqrt_two_pi_l;
  const double asymptotic_threshold;
  const double highprec_erfc_threshold;
  const double round_to_one_cut;
  const double underflow_to_zero_cut;
} cdfnorm_data = {
  .zero = 0x0p+0,
  .half = 0x1p-1,
  .one  = 0x1p+0,
  .sqrth = 0x1.6a09e667f3bcdp-1,
  .sqrth_ld = 0x1.6a09e667f3bcc908b2fb1366eap-1L,
  .half_ld = 0x1p-1L,
  .one_over_sqrt_two_pi_l = 0x1.9884533d43651814ap-2L,
  .asymptotic_threshold = -0x1.4p+3,
  .highprec_erfc_threshold = -0x1.8p+0,
  .round_to_one_cut = 0x1.2cp+5,
  .underflow_to_zero_cut = -0x1.38p+5,
};

#define HALF_LD                  cdfnorm_data.half_ld
#define SQRTH_LD                 cdfnorm_data.sqrth_ld
#define ONE_OVER_SQRT_TWO_PI_L   cdfnorm_data.one_over_sqrt_two_pi_l

/* Long double rational approximation coefficients */
static const long double erfc_rat_0p5_1p5_Y = 0x1.9fadap-2L;
static const long double erfc_rat_0p5_1p5_P[] = {
    -0x1.91c770d9d3c378012fa57cc694ce716591a80918c7bd5c4d0cf1eb2a5d17b1e8p-4L,
    0x1.6cc761c5cdfcfd8ee48f265b3d8e5a3da9536bda662efc9c8e82d1a461e30f36p-3L,
    0x1.872cf216e5f4b573433889c587c1b3016a248772906b087293ce5662c1eec14ep-3L,
    0x1.6c17f596974e16c72314b6e90e4ddb8c8614aece90d2e193cf60b493d215b44ap-4L,
    0x1.3f917b098b968f8d01b27ed0635da59b3c4663c402e9559e22fec9494a19cb14p-6L,
    0x1.d8f8dba7bf514ee23417ffce9730bfc01d897aa6d7fa50e72aa73ebc02f7ddecp-10L,
};
static const long double erfc_rat_0p5_1p5_Q[] = {
    0x1p0L,
    0x1.d8fbb46b1d05e157a5c6d4607d98856895309b06c3198f783f071dbc656b9ep0L,
    0x1.6d20b0730d504fd57921b09e74414d3447ec07bfe25cbcb0989153d51977b0fp0L,
    0x1.27f68988b9f54ff64b02ad9cc7c338d10004ff42d8299b18c3009a1459c3a752p-1L,
    0x1.fb4b28dd2f600e9a4b3e6b10cdf6f7b815610f27b5f006c64985c00e8040c416p-4L,
    0x1.738a6d80ccf02a2cdf16084a0de14c5355be4f87c4adbbe383ada2d07c39a18ep-7L,
    0x1.c5000f148a68e3c0e8efc4dcbfa6be4cf501a39af6919f621d61fc9ae691af6p-19L,
};

static const long double erfc_rat_1p5_2p5_Y = 0x1.0371ep-1L;
static const long double erfc_rat_1p5_2p5_P[] = {
    -0x1.8ef3808bfbe39c5f54dd0d713c429ce52b8749ddfbe080d650e28fa886982b6ap-6L,
    0x1.3ca7645df54c8baeb469e285d9521134bcd1935f7e96f7bd6f7542ea235e80eep-5L,
    0x1.680608a7660755f3af7da71a1e4c65f5ffeb072d561899881f423b1978f12cc8p-5L,
    0x1.1fd54bd6a624465997484b4444727a9823e26079d2dd54e793f89e494da42bbep-6L,
    0x1.a89fbecd94db79d0c63c57f303d1a5ac93ab3f6b22cdc136b487249ffa1a918p-9L,
    0x1.ee97293c42ffa838461963bfa64b332c367d86273fc620f58e890bcac257a9dcp-13L,
};
static const long double erfc_rat_1p5_2p5_Q[] = {
    0x1p0L,
    0x1.8a37ddbaa96d02eac08687f0e337bc103251f8f0689ee0b3c90ece35975aa5e8p0L,
    0x1.f6fd9e749acf5662fc975c083b74d8404e5320e34f628637fe7ae18881a2c2dep-1L,
    0x1.4d8cee8caf21a7af6a484d8dd16f20c1b198dca7a8eb0501c9e4eb5c69da9e24p-2L,
    0x1.cdf6fb1d81fe2c8f1f83e7b4c1e1fb796ad64a2171b0b1cd275431fb9cc5a848p-5L,
    0x1.0cf09d70053478a159ad57ccc29609e87bfedc3fde0cf0291a1c9aae22f3b2c6p-8L,
};

static const long double erfc_rat_2p5_4p5_Y = 0x1.14c64p-1L;
static const long double erfc_rat_2p5_4p5_P[] = {
    0x1.83066cd0bf159e333b9d5e9b2009007ca4885b2c846292fc6bc9c6ec9bb1ef4cp-9L,
    0x1.c22e68cf6b2c50d48c02fb449660ff455b21715f667b0a644d2a0e1f59d0f7c8p-7L,
    0x1.13840e0e2140c4578ad109e98639aa781af37281e76040f74387b9382220c08ap-7L,
    0x1.16f46da1af76b75deac5a46a04c3558ed83391c9ac3c6e3f8b763d30ce65cce6p-9L,
    0x1.066d54ccd32a5aed4f2eff83f7cdddb6b62f1361920a24267bb7c089e2246166p-12L,
    0x1.7be0b780f7e4ead83d67afa5c9deaacedcd9e1555513337efa28d922d4beb322p-17L,
};
static const long double erfc_rat_2p5_4p5_Q[] = {
    0x1p0L,
    0x1.0acc2fcb1378a97419ac57b1a88dc3a50400787548d78645ea36c5fef650e508p0L,
    0x1.c538522309af57f95d14315fff51fb4ab095420f06d793f36dd9b955bb411086p-2L,
    0x1.88993f34aac71a8f087c076e03d56032e8f15885b234edd8914a85da10c04edcp-4L,
    0x1.5b48e7dd47e9a832830159b56fa4f05b29d73163de15afc994efe5dc62169a66p-7L,
    0x1.f6b2fb953735b6d21a976fe6f8452ea234b6386453960e80cfa7d3608b19deccp-12L,
};

static const long double erfc_rat_4p5_28_Y = 0x1.1da64p-1L;
static const long double erfc_rat_4p5_28_P[] = {
    0x1.9b9a82141fdabcf25f50ef97231068e3648b528353cac936a625313038c9fb2ep-8L,
    0x1.1f5bd4085c48cc6326f1451ce94c46da1cc73fe423f112c38c82f4e4f94958b6p-6L,
    -0x1.b383063c1348c938edfee2f7b8cf6416bc08fedfb69cf8d0c018b0a94811af1ep-3L,
    -0x1.601c882adf2bed07f45964eb74480d1f122d2ffb2dc6c5aa5ae485418a3c515cp-1L,
    -0x1.46a330b7811e1537b1f7d64f3a7b4227507507fd4b20190414b18a6d582bbf8ap1L,
    -0x1.9d17fc9bea3fa91784b9e6c29af353f590f76f96bd8b574bee3a1d13a1adfbc4p1L,
    -0x1.68a52784ed043843c51ce54763cfb77027a977fbb2e084416cef658e6282d7bcp1L,
};
static const long double erfc_rat_4p5_28_Q[] = {
    0x1p0L,
    0x1.65732e0a149600664ad235d2a3c13bbe1ba743c77b1f23f6e13cc2df4fab246ap1L,
    0x1.61d0ae6a5ca5e3fcd2fc84c10e7877996eec3c8481280e35409f20e4b6830188p3L,
    0x1.fdc7da2dc11218a4e6b99f8834db9339f440005128236c97005afd7cd3356182p3L,
    0x1.6efce09ed824fa02250286c034a4bbff2527c12c59c14cbe8b654c0288dbe792p4L,
    0x1.b0349176f08c74f649e984e5ae2b00c927e1a288f17525b2afd8a05791c65614p3L,
    0x1.5efb5c445a48f73b0de602deb3187d6dc4c6f891f7338d2f69daeb79c7cbd29ap2L,
};

#endif /* WINDOWS */

/* Common access macros */
#define ZERO                     cdfnorm_data.zero
#define HALF                     cdfnorm_data.half
#define ONE                      cdfnorm_data.one
#define SQRTH                    cdfnorm_data.sqrth
#define ASYMPTOTIC_THRESHOLD     cdfnorm_data.asymptotic_threshold
#define HIGHPREC_ERFC_THRESHOLD  cdfnorm_data.highprec_erfc_threshold
#define HI_CUT                   cdfnorm_data.round_to_one_cut
#define LO_CUT                   cdfnorm_data.underflow_to_zero_cut

#define INF                 0x7ff0000000000000ULL
#define UPPER32_MASK        0x7fffffffU
#define INF_NAN             0x7ff00000U


/*============================================================================
 * HELPER FUNCTIONS - Platform-specific implementations
 *============================================================================*/

#if defined(_WIN32)
/*
 * Windows: Double-double arithmetic for extended precision
 */

/*
 * Extended precision exponential for negative arguments using double-double.
 */
static inline dd_t
exp_negative_dd(dd_t x) {
    double exp_main = ALM_PROTO_OPT(exp)(x.hi);
    double r = x.lo;
    double exp_lo = POLY_EVAL_HORNER_7(r, EXP_TAYLOR[0], EXP_TAYLOR[1], EXP_TAYLOR[2],
                                       EXP_TAYLOR[3], EXP_TAYLOR[4], EXP_TAYLOR[5], EXP_TAYLOR[6]);
    return dd_two_prod(exp_main, exp_lo);
}

/*
 * Asymptotic expansion for extreme negative arguments (a ≤ -10).
 */
static inline double
asymptotic_expansion_tail(double a) {
    dd_t a_dd = dd_from_d(a);
    dd_t sum = dd_from_d(1.0);
    dd_t a_sq = dd_mul(a_dd, a_dd);
    dd_t g = dd_from_d(1.0);
    dd_t last_term = dd_from_d(DBL_MAX);
    int n = 1;
    
    while (n < 30) {
        double coef1 = 4.0 * n - 3.0;
        dd_t x = dd_div_d(dd_from_d(coef1), dd_to_d(a_sq));
        double coef2 = 4.0 * n - 1.0;
        dd_t y = dd_mul(x, dd_div_d(dd_from_d(coef2), dd_to_d(a_sq)));
        dd_t x_minus_y = dd_sub(x, y);
        dd_t term = dd_mul(g, x_minus_y);
        sum = dd_sub(sum, term);
        g = dd_mul(g, y);
        n++;
        
        dd_t abs_term = dd_abs(term);
        if (dd_ge(abs_term, last_term)) break;
        dd_t abs_sum = dd_abs(sum);
        double threshold = dd_to_d(abs_sum) * DBL_EPSILON * 0.1;
        if (dd_to_d(abs_term) < threshold) break;
        last_term = abs_term;
    }
    
    dd_t neg_half = dd_from_d(-0.5);
    dd_t exp_arg = dd_mul(neg_half, a_sq);
    dd_t exp_val = exp_negative_dd(exp_arg);
    dd_t phi = dd_mul(exp_val, ONE_OVER_SQRT_2PI);
    dd_t neg_phi = dd_neg(phi);
    dd_t numerator = dd_mul(neg_phi, sum);
    dd_t result = dd_div(numerator, a_dd);
    
    return dd_to_d(result);
}

/*
 * High-precision erfc using double-double arithmetic.
 */
static inline dd_t
erfc_highaccuracy_dd(dd_t z_dd) {
    double z = z_dd.hi;
    dd_t Y_dd, P_dd, Q_dd, t_dd;
    
    if (z < 1.5) {
        Y_dd = dd_from_d(ERFC_0P5_1P5_Y);
        t_dd = dd_add_d(z_dd, -0.5);
        P_dd = DD_POLY_EVAL_HORNER_6(t_dd, ERFC_0P5_1P5_P);
        Q_dd = DD_POLY_EVAL_HORNER_7(t_dd, ERFC_0P5_1P5_Q);
    } else if (z < 2.5) {
        Y_dd = dd_from_d(ERFC_1P5_2P5_Y);
        t_dd = dd_add_d(z_dd, -1.5);
        P_dd = DD_POLY_EVAL_HORNER_6(t_dd, ERFC_1P5_2P5_P);
        Q_dd = DD_POLY_EVAL_HORNER_6(t_dd, ERFC_1P5_2P5_Q);
    } else if (z < 4.5) {
        Y_dd = dd_from_d(ERFC_2P5_4P5_Y);
        t_dd = dd_add_d(z_dd, -3.5);
        P_dd = DD_POLY_EVAL_HORNER_6(t_dd, ERFC_2P5_4P5_P);
        Q_dd = DD_POLY_EVAL_HORNER_6(t_dd, ERFC_2P5_4P5_Q);
    } else {
        Y_dd = dd_from_d(ERFC_4P5_28_Y);
        t_dd = dd_div_d(dd_from_d(1.0), z);
        P_dd = DD_POLY_EVAL_HORNER_7(t_dd, ERFC_4P5_28_P);
        Q_dd = DD_POLY_EVAL_HORNER_7(t_dd, ERFC_4P5_28_Q);
    }
    
    dd_t ratio = dd_div(P_dd, Q_dd);
    dd_t result = dd_add(Y_dd, ratio);
    dd_t z_sq = dd_mul(z_dd, z_dd);
    dd_t neg_z_sq = dd_neg(z_sq);
    dd_t exp_neg_z2 = exp_negative_dd(neg_z_sq);
    result = dd_mul(result, exp_neg_z2);
    result = dd_div(result, z_dd);
    
    return result;
}

#else /* Linux */
/*
 * Linux: Long double (80-bit extended precision)
 */

/*
 * Extended precision exponential for negative arguments.
 */
static inline long double
exp_negative_extended(long double x) {
    double x_d = (double)x;
    long double x_rem = x - (long double)x_d;
    
    double exp_main = ALM_PROTO_OPT(exp)(x_d);
    
    long double exp_rem = 0x1p+0L;
    if (x_rem != 0x0p+0L) {
        long double term = x_rem;
        exp_rem += term;
        term *= x_rem * 0x1p-1L;
        exp_rem += term;
        term *= x_rem * 0x1.5555555555555556p-2L;
        exp_rem += term;
        term *= x_rem * 0x1p-2L;
        exp_rem += term;
        term *= x_rem * 0x1.999999999999999ap-3L;
        exp_rem += term;
        term *= x_rem * 0x1.5555555555555556p-3L;
        exp_rem += term;
    }
    
    return (long double)exp_main * exp_rem;
}

/*
 * Asymptotic expansion for extreme negative arguments (a ≤ -10).
 */
static inline double
asymptotic_expansion_tail(double a) {
    long double a_l = (long double)a;
    long double sum = 0x1p+0L;
    long double a_sq = a_l * a_l;
    long double g = 0x1p+0L;
    long double x, y, term;
    long double last_term = LDBL_MAX;
    int n = 1;
    
    while (n < 30) {
        x = (0x1p+2L * n - 0x1.8p+1L) / a_sq;
        y = x * ((0x1p+2L * n - 0x1p+0L) / a_sq);
        term = g * (x - y);
        sum -= term;
        g *= y;
        n++;
        
        long double abs_term = (term >= 0x0p+0L) ? term : -term;
        if (abs_term >= last_term) break;
        long double abs_sum = (sum >= 0x0p+0L) ? sum : -sum;
        if (abs_term < abs_sum * LDBL_EPSILON * 0x1.999999999999999ap-4L) break;
        last_term = abs_term;
    }
    
    long double exp_arg = -0x1p-1L * a_sq;
    long double pdf = exp_negative_extended(exp_arg) * ONE_OVER_SQRT_TWO_PI_L;
    long double result = -pdf * sum / a_l;
    
    return (double)result;
}

/*
 * High-precision erfc using long double.
 */
static inline long double
erfc_highaccuracy_extended(long double z_ext) {
    long double result;
    long double Y;
    
    if (z_ext < 0x1.8p+0L) {
        Y = erfc_rat_0p5_1p5_Y;
        long double t = z_ext - 0x1p-1L;
        long double P = POLY_EVAL_HORNER_6(t, erfc_rat_0p5_1p5_P[0], erfc_rat_0p5_1p5_P[1],
                                           erfc_rat_0p5_1p5_P[2], erfc_rat_0p5_1p5_P[3],
                                           erfc_rat_0p5_1p5_P[4], erfc_rat_0p5_1p5_P[5]);
        long double Q = POLY_EVAL_HORNER_7(t, erfc_rat_0p5_1p5_Q[0], erfc_rat_0p5_1p5_Q[1],
                                           erfc_rat_0p5_1p5_Q[2], erfc_rat_0p5_1p5_Q[3],
                                           erfc_rat_0p5_1p5_Q[4], erfc_rat_0p5_1p5_Q[5],
                                           erfc_rat_0p5_1p5_Q[6]);
        result = Y + P / Q;
    } else if (z_ext < 0x1.4p+1L) {
        Y = erfc_rat_1p5_2p5_Y;
        long double t = z_ext - 0x1.8p+0L;
        long double P = POLY_EVAL_HORNER_6(t, erfc_rat_1p5_2p5_P[0], erfc_rat_1p5_2p5_P[1],
                                           erfc_rat_1p5_2p5_P[2], erfc_rat_1p5_2p5_P[3],
                                           erfc_rat_1p5_2p5_P[4], erfc_rat_1p5_2p5_P[5]);
        long double Q = POLY_EVAL_HORNER_6(t, erfc_rat_1p5_2p5_Q[0], erfc_rat_1p5_2p5_Q[1],
                                           erfc_rat_1p5_2p5_Q[2], erfc_rat_1p5_2p5_Q[3],
                                           erfc_rat_1p5_2p5_Q[4], erfc_rat_1p5_2p5_Q[5]);
        result = Y + P / Q;
    } else if (z_ext < 0x1.2p+2L) {
        Y = erfc_rat_2p5_4p5_Y;
        long double t = z_ext - 0x1.cp+1L;
        long double P = POLY_EVAL_HORNER_6(t, erfc_rat_2p5_4p5_P[0], erfc_rat_2p5_4p5_P[1],
                                           erfc_rat_2p5_4p5_P[2], erfc_rat_2p5_4p5_P[3],
                                           erfc_rat_2p5_4p5_P[4], erfc_rat_2p5_4p5_P[5]);
        long double Q = POLY_EVAL_HORNER_6(t, erfc_rat_2p5_4p5_Q[0], erfc_rat_2p5_4p5_Q[1],
                                           erfc_rat_2p5_4p5_Q[2], erfc_rat_2p5_4p5_Q[3],
                                           erfc_rat_2p5_4p5_Q[4], erfc_rat_2p5_4p5_Q[5]);
        result = Y + P / Q;
    } else {
        Y = erfc_rat_4p5_28_Y;
        long double t = 0x1p+0L / z_ext;
        long double P = POLY_EVAL_HORNER_7(t, erfc_rat_4p5_28_P[0], erfc_rat_4p5_28_P[1],
                                           erfc_rat_4p5_28_P[2], erfc_rat_4p5_28_P[3],
                                           erfc_rat_4p5_28_P[4], erfc_rat_4p5_28_P[5],
                                           erfc_rat_4p5_28_P[6]);
        long double Q = POLY_EVAL_HORNER_7(t, erfc_rat_4p5_28_Q[0], erfc_rat_4p5_28_Q[1],
                                           erfc_rat_4p5_28_Q[2], erfc_rat_4p5_28_Q[3],
                                           erfc_rat_4p5_28_Q[4], erfc_rat_4p5_28_Q[5],
                                           erfc_rat_4p5_28_Q[6]);
        result = Y + P / Q;
    }
    
    /* Two-part exp(-z²) computation */
    double z_d = (double)z_ext;
    double z_hi_d = asdouble(asuint64(z_d) & 0xfffffffff8000000ULL);
    long double z_hi = (long double)z_hi_d;
    long double z_lo = z_ext - z_hi;
    long double z_sq = z_ext * z_ext;
    long double err_sq = ((z_hi * z_hi - z_sq) + 0x1p+1L * z_hi * z_lo) + z_lo * z_lo;
    long double exp_main = exp_negative_extended(-z_sq);
    long double exp_corr = exp_negative_extended(-err_sq);
    result *= exp_main * exp_corr / z_ext;
    
    return result;
}

#endif /* WINDOWS */


/*============================================================================
 * MAIN FUNCTION
 *============================================================================*/

double ALM_PROTO_OPT(cdfnorm)(double a) {
  double y;
  uint64_t ux;
  uint32_t ix;
  
  /* Extract sign and check for special values */
  ux = asuint64(a);
  uint64_t sign = ux & SIGNBIT_DP64;
  ux &= ~SIGNBIT_DP64;
  ix = (uint32_t)((ux >> 32) & UPPER32_MASK);
  
  /* Handle infinities and NaN */
  if (unlikely(ix >= INF_NAN)) {
    if (ux == INF) {
      return sign ? ZERO : ONE;
    }
    return a + a;  /* NaN propagation */
  }
  
  /* Saturation for extreme values */
  if (a >= HI_CUT) {
    return ONE;
  }
  if (a <= LO_CUT) {
    return ZERO;
  }
  
  /*
   * Four-region implementation:
   *
   * Region 1 [a > -0.5]: Use Φ(a) = (1/2)[1 + erf(a/√2)]
   * Region 2 [-1.5 < a ≤ -0.5]: Use Φ(a) = (1/2)erfc(-a/√2) with extended transform
   * Region 3 [-10 < a ≤ -1.5]: Use extended precision throughout with rational approximations
   * Region 4 [a ≤ -10]: Use asymptotic expansion
   */
  
  if (a > -0.5) {
    /*
     * Region 1: Positive and small negative arguments
     * Use erf formulation: Φ(a) = 0.5 + 0.5·erf(a/√2)
     */
    double x = a * SQRTH;
    y = HALF + HALF * ALM_PROTO_OPT(erf)(x);
    
  } else if (a > HIGHPREC_ERFC_THRESHOLD) {  /* -1.5 < a <= -0.5 */
    /*
     * Region 2: Moderate negative arguments
     * Use erfc formulation: Φ(a) = 0.5·erfc(-a/√2)
     */
#if defined(_WIN32)
    dd_t a_dd = dd_from_d(a);
    dd_t neg_a_dd = dd_neg(a_dd);
    dd_t x_dd = dd_mul(neg_a_dd, SQRTH_DD);
    double x = dd_to_d(x_dd);
#else
    long double a_ext = (long double)a;
    long double x_ext = -a_ext * SQRTH_LD;
    double x = (double)x_ext;
#endif
    y = HALF * ALM_PROTO_OPT(erfc)(x);
    
  } else if (a > ASYMPTOTIC_THRESHOLD) {  /* -10 < a <= -1.5 */
    /*
     * Region 3: Critical negative range - use extended precision
     */
#if defined(_WIN32)
    dd_t a_dd = dd_from_d(a);
    dd_t neg_a_dd = dd_neg(a_dd);
    dd_t x_dd = dd_mul(neg_a_dd, SQRTH_DD);
    dd_t erfc_dd = erfc_highaccuracy_dd(x_dd);
    y = 0.5 * dd_to_d(erfc_dd);
#else
    long double a_ext = (long double)a;
    long double x_ext = -a_ext * SQRTH_LD;
    long double erfc_ext = erfc_highaccuracy_extended(x_ext);
    y = (double)(HALF_LD * erfc_ext);
#endif
    
  } else {
    /* Region 4: Extreme negative tail - use asymptotic expansion */
    y = asymptotic_expansion_tail(a);
  }
  
  return y;
}
