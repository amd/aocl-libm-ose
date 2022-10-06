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
 *   double atan(double x)
 *
 * Spec:
 *   atan(1)    = pi/4
 *   atan(0)    = 0
 *   atan(-0)   = -0
 *   atan(+inf) = pi/2
 *   atan(-inf) = -pi/2
 *   atan(nan)  = nan
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute atan(double x)
 * Let y = x
 * Let x = |x|
 *
 * Based on the value of x, atan(x) is calculated as,
 *
 * 1. If x > 2,
 *      atan(x) = pi/2 - atan(1/x)
 *
 * 2. Otherwise,
 *
 *      atan(x) is calculated using a polynomial obtained by the Remez algorithm
 *      the domain [0, 2] is split up into various subdomains with different
 *      polynomial coefficients in each domain
 *      For x > 0.5 we must use Dekker's method of simulated quadruple precision
 *      in order to obtain a ULP error below 1. This impacts performance.
 *
 * If y < 0, then atan(y) = -atan(x)
 */


#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>


static struct {
                double piby_2;
                double pi[2], poly_atan1[14], poly_atan2[16];
                uint64_t poly_atan3H[5][16], poly_atan3T[5][16];
              } atan_data = {
                .piby_2    = 0x1.921fb54442d18p0,
                .pi        = {
                    0, 0x1.921fb54442d18p0,
                },

                .poly_atan1 = {
                    0x1p0, -0x1.5555555555555p-2, 0x1.999999999999ap-3, -0x1.2492492492492p-3,
                    0x1.c71c71c71c6bbp-4, -0x1.745d1745ce62ap-4, 0x1.3b13b13a13da4p-4,
                    -0x1.111110d877b34p-4, 0x1.e1e1d0673273bp-5, -0x1.af2683d56d189p-5,
                    0x1.85f2e2bfbd825p-5, -0x1.622e2461715a2p-5, 0x1.35a89baa35be8p-5,
                    -0x1.978df56b573cbp-6,
                },


                .poly_atan2 = {
                    0x1.fffffffffffffp-1, -0x1.555555555549bp-2, 0x1.9999999994314p-3,
                    -0x1.24924923ccdcbp-3, 0x1.c71c71a0771fcp-4, -0x1.745d1484a4613p-4,
                    0x1.3b138b5441a92p-4, -0x1.110f7f665ef2ap-4, 0x1.e1c7f2f54a391p-5,
                    -0x1.ae804a467f02fp-5, 0x1.82c0abef7e1fap-5, -0x1.56df7418ffaf5p-5,
                    0x1.1ea668d405438p-5, -0x1.9cf66b20819bap-6, 0x1.b54143fea99bbp-7,
                    -0x1.e8fc042f5ec18p-9,
                },

                .poly_atan3H = {
                    {0x3feffffffff8a250, 0xbfd5555552d29c2e, 0x3fc99999331ef665,
                     0xbfc2491f7e6cf3d9, 0x3fbc716d671cb81e, 0xbfb7438749afdca6,
                     0x3fb3a5c1c73d6401, 0xbfb0e3f07d05f2ff, 0x3fad0328310d1678,
                     0xbfa827a8b0bb38c5, 0x3fa2a58fe22ba855, 0xbf9934c562d3540e,
                     0x3f8bd9336d7767b0, 0xbf770c34293bd24f, 0x3f58fdb9cc27404a,
                     0xbf2a5731f700d07f},
                    {0x3fefffffd93ae48e, 0xbfd5554e976e276e, 0x3fc9990c4516c641,
                     0xbfc24584ba664fb1, 0x3fbc5052f0c38b41, 0xbfb6d2495bae8cb8,
                     0x3fb27aab914479b3, 0xbface9a09ded1add, 0x3fa4eb51e90547c6,
                     0xbf9ac717f6960f77, 0x3f8d085ee231060c, 0xbf79825ab5667d6d,
                     0x3f61423b76de1ffe, 0xbf40c1a698ba364e, 0x3f14af542400f08c,
                     0xbed8508712e8b787},
                    {0x3feffff0dae86532, 0xbfd553c54149f839, 0x3fc98605b1ed670b,
                     0xbfc1fb9f5d853e88, 0x3fbabcd06f1230ad, 0xbfb39c640e5862bb,
                     0x3faadc71dfe0d2b7, 0xbfa069b9174c5d14, 0x3f914126330f12af,
                     0xbf7e45fe88f68f11, 0x3f65894cbe906c2a, 0xbf481157e9f1a9aa,
                     0x3f24420ff205c902, 0xbef816d14f381c64, 0x3ec201ecc7052541,
                     0xbe796e0d2e9ad474},
                    {0x3feffebf3f2fb03b, 0xbfd53fde8714ab5d, 0x3fc8e9463cceec3c,
                     0xbfc0784f0aa6bf56, 0x3fb57c722f5dd81a, 0xbfa9a6947cb6201b,
                     0x3f9abc6b149e5d97, 0xbf879a2774ad3b07, 0x3f714448447f3c8a,
                     0xbf548b345fa24b01, 0x3f33790382677b46, 0xbf0ca4aca8c6c15b,
                     0x3edf771d760865a5, 0xbea845561319bc80, 0x3e67707712cb2ea4,
                     0xbe15512df60d53ef},
                    {0x3feff51135d5f4ef, 0xbfd4d5873109cfe4, 0x3fc6c1701ade3063,
                     0xbfb9ecf6238ad313, 0x3faaf5577a41b119, 0xbf981f720212dbb3,
                     0x3f820c497807f751, 0xbf66374c6e66e956, 0x3f46396231aeee3d,
                     0xbf21d70669aac186, 0x3ef69b64461aeccc, 0xbec614e0616a5970,
                     0x3e900771db2a9be4, 0xbe5047bd2e48a5d5, 0x3e04a4b747014796,
                     0xbda8985590bbd3a1}
                },

                .poly_atan3T = {
                    {0x3c787799f4d80a98, 0xbc74fd81a0adbb92, 0x3c69eb2e0384b99c,
                     0xbc6ad29d3eba030a, 0x3c5c192a31ff521d, 0x3c5cd2db18658d16,
                     0x3c144209d3d81810, 0x3c598de05e6916d1, 0xbc3e915911100288,
                     0x3be7b97bc15976c0, 0x3c48663a070eec55, 0xbc2033d279fcea46,
                     0x3c127b3a00bceff6, 0xbc057093f437934c, 0xbbfc318ba21f9cf3,
                     0x3bc0986dac09cb7a},
                    {0xbc702840caa5e664, 0xbc44edf08a2efba0, 0xbc5c22096a9e61d8,
                     0x3c6de4ea835fc22e, 0x3c5666028d10d51d, 0x3c50b0dbd700d535,
                     0x3c54d2a5aa80f048, 0x3c402879790a62f1, 0xbc4ac60979cc3b4e,
                     0xbc2ea747cbc3770c, 0x3c2de8c9410f0625, 0xbbc9320325b88020,
                     0xbbab9b34b25eb2c0, 0xbbce115549398e08, 0x3b97b7e987add018,
                     0x3b624182cbbf84e0},
                    {0xbc719530e7adebac, 0x3c76dffb19496e45, 0x3c5005675527f87a,
                     0x3c0b67d411c87bc0, 0xbc3b3b5d33381364, 0x3c281439debc8e18,
                     0x3c36fa56ccc2616c, 0x3c28c976e59e21f4, 0x3c262c733728fffc,
                     0xbc1cc84961264666, 0x3bd450ff78ed51b0, 0xbbe5e13b749e08a5,
                     0x3b904cc7da5dc148, 0xbb9e55ef8998d6bf, 0xbb67e93430a1bc0c,
                     0xbb08fca96191de0c},
                    {0x3c7c4ed3d6f355f2, 0x3c77ba9e37e56352, 0xbc672487469842c8,
                     0xbc58495767a18464, 0x3c5ceb8f5bf5eaf8, 0x3c403c8a4984c1cf,
                     0x3c31d867eeb50a1d, 0x3befe79b8b2dbe90, 0xbbf4d823b7461b08,
                     0xbbe3fb2a1313abe2, 0xbbd12a9885044678, 0x3ba71b4f7496ce10,
                     0xbb7c44f569d96736, 0x3b28946fa8a2236c, 0xbb051e7cfb50787b,
                     0x3abbb833c31b19c5},
                    {0x3c8d7a7a01f82052, 0xbc1c9d6dfe978080, 0xbc6411a7708c95f9,
                     0xbc5059f2a208a54a, 0xbc47cddeffedc1a0, 0xbc1e67e46aa41994,
                     0xbc2d9ba393518dde, 0xbbd9b62cb65df908, 0x3bd3625704b93fb2,
                     0x3bcb20da3eb28f2f, 0xbb8728c80a00cf94, 0xbb60230de4e961b9,
                     0xbb2fee32a9266b42, 0x3afbfaf26bb9d9b2, 0xba71bf95d6a03230,
                     0xba45a5a1963459b9}
                },
};

#define PI     atan_data.pi
#define PIBY_2 atan_data.piby_2

#define A0 atan_data.poly_atan1[0]
#define A1 atan_data.poly_atan1[1]
#define A2 atan_data.poly_atan1[2]
#define A3 atan_data.poly_atan1[3]
#define A4 atan_data.poly_atan1[4]
#define A5 atan_data.poly_atan1[5]
#define A6 atan_data.poly_atan1[6]
#define A7 atan_data.poly_atan1[7]
#define A8 atan_data.poly_atan1[8]
#define A9 atan_data.poly_atan1[9]
#define A10 atan_data.poly_atan1[10]
#define A11 atan_data.poly_atan1[11]
#define A12 atan_data.poly_atan1[12]
#define A13 atan_data.poly_atan1[13]

#define B0 atan_data.poly_atan2[0]
#define B1 atan_data.poly_atan2[1]
#define B2 atan_data.poly_atan2[2]
#define B3 atan_data.poly_atan2[3]
#define B4 atan_data.poly_atan2[4]
#define B5 atan_data.poly_atan2[5]
#define B6 atan_data.poly_atan2[6]
#define B7 atan_data.poly_atan2[7]
#define B8 atan_data.poly_atan2[8]
#define B9 atan_data.poly_atan2[9]
#define B10 atan_data.poly_atan2[10]
#define B11 atan_data.poly_atan2[11]
#define B12 atan_data.poly_atan2[12]
#define B13 atan_data.poly_atan2[13]
#define B14 atan_data.poly_atan2[14]
#define B15 atan_data.poly_atan2[15]

#define SIGN_MASK   0x7FFFFFFFFFFFFFFFUL

double ALM_PROTO_OPT(atan)(double x)
{
    double poly, result;
    int64_t n = 0;
    uint64_t sign;

    uint64_t ux = asuint64(x);

    /* Get sign of input value */
    sign = ux & (~SIGN_MASK);

    /* Get absolute value of input */
    ux = ux & SIGN_MASK;
    x = asdouble(ux);

    /* Check for special cases */
    if(unlikely(ux > PINFBITPATT_DP64)) {
        /* x is inf */
        if((ux & MANTBITS_DP64) == 0x0) {
            return asdouble(sign ^ asuint64(PIBY_2));
        }
        else {
            /* x is nan */
            return alm_atan_special(asdouble(sign ^ ux));
        }
    }

    /* If x > 2, then we use the identity
     * atan(x) = pi/2 - atan(1/x)
     */
    if (x > 2.0) {
        n = 1;
        x = 1.0/x;
    }

    if (x <=0.25) {
        poly = POLY_EVAL_ODD_27(x, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
    } else if (x <=0.5) {
        poly = POLY_EVAL_ODD_31(x, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15);

    } else {
        /* need to use simulated quad precision here */
        double x2 = x*x;
        /* split x2 into x2H and x2T */
	    uint64_t ux2 = asuint64(x2);
	    ux2 &= 0xfffffffff8000000;
	    double x2H = asdouble(ux2);
	    double x2T = x2 - x2H;
        int j;
        if (x > 1.6)
            j = 4;
        else if (x > 1.3)
            j = 3;
        else if (x > 1.0)
            j =2;
        else if (x > 0.75)
            j = 1;
        else
            j = 0;
        double sH = asdouble(atan_data.poly_atan3H[j][15]);
        double sL = asdouble(atan_data.poly_atan3T[j][15]);
        double rH, rL, tH, tL, cH, cL, z, sHH, sHT;
        uint64_t usH;
        for (int i = 14 ; i >=0 ; i--) {
	        /* [rH, rL] = TwoProduct(sH,x2) */
	        rH = sH * x2;
            /* split sH into sHH and sHT */
	        usH = asuint64(sH);
	        usH &= 0xfffffffff8000000;
	        sHH = asdouble(usH);
	        sHT = sH - sHH;

            rL = sHT*x2T-(((rH-sHH*x2H)-sHT*x2H)-sHH*x2T);

            /* [tH, tL] = FastTwoSum(rH,(sL*x2)) */
	        z = sL * x2;
	        tH = rH + z;
	        tL = z - (tH-rH);

	        /* [cH, cL] = FastTwoSum(tH,(tL+rL)) */
            z = tL + rL;
	        cH = tH + z;
	        cL = z - (cH-tH);

            /* [rH, rL] = TwoSum(cH,AH[i]) */
	        rH = cH + asdouble(atan_data.poly_atan3H[j][i]);
	        z = rH - cH;
	        rL = asdouble(atan_data.poly_atan3T[j][i]) + (cH-(rH-z))+(asdouble(atan_data.poly_atan3H[j][i])-z);

            /* [sH, sL] = FastTwoSum(rH,(rL+cL)) */
	        z = rL + cL;
	        sH = rH + z;
	        sL = z - (sH-rH);
        }
        poly = sH * x + sL * x;

    }

    if (n > 0)
        poly = - poly;

    /* atan(x) = pi/2 - atan(1/x) */
    result = PI[n] + poly;

    /* atan(-x) = -atan(x) */
    return asdouble(asuint64(result) ^ sign);
}
