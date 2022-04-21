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

#ifndef __TEST_SIN_DATA_H__
#define __TEST_SIN_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the sinf() routine.
 * These test cases are not exhaustive
 * Test data added as per GLIBC output
 */
static libm_test_special_data_f32
test_sinf_conformance_data[] = {
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID,}, //sinf(nan)=nan
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID,},   //sinf(-nan)=-nan
    {POS_QNAN_F32, POS_QNAN_F32, 0,},  //sinf(qnan) = qnan
    {NEG_QNAN_F32, NEG_QNAN_F32, 0,},  //sinf(-qnan) = -qnan
    {POS_ZERO_F32, POS_ZERO_F32, 0,},   //sinf(0)=0
    {NEG_ZERO_F32, NEG_ZERO_F32, 0,},      //sinf(-0) = -0
    {POS_ONE_F32, 0x3f576aa4, FE_INEXACT,}, //sin(1)
    {POS_INF_F32, 0xffc00000, FE_INVALID,},      //inf
    {NEG_INF_F32, 0xffc00000, FE_INVALID,},      //-inf
    {NEG_ONE_F32, 0xbf576aa4, FE_INEXACT,}, //-1
    {POS_PI_F32, 0x0, 0,}, //sin(pi)=0
    {0x3B800000, 0x3B7FFFD5, 0},    //sini
    {0x3FC90FDB, 0x3f800000, 0},     //sin(Pi/2)=1
    /*some vals taken from old test framework*/
    {0x3fc90de3, 0x3f800000, 0},// 1.57074 = pi/2 - 6e-05
    {0x3fc911d2, 0x3f800000, 0},//// 1.57086 = pi/2 + 6e-05
    {0x3fc88fdb, 0x3f7fff80, 0}, // 1.56689 = pi/2 - 0.00390625
    {0x3fc98fdb, 0x3f7fff80, 0}, // 1.57470 = pi/2 + 0.00390625
    {0x40000000, 0x3f68c7b7, 0}, //sin(2)
    {0x41200000, 0xbf0b44f8, 0}, //10
    {0xC8C35000, 0x3e11f59f, 0}, //-4e5
    {0x48F42400, 0x3e361962, 0}, //50000
    {0x48F4CCC8, 0x3f3a0e4a, 0}, //501350.25 > 5.0e5
    {0xC93F6800, 0xbe1dbf77, 0}, //-784000
    {0x43FCE5F1, 0xb20fd1de, 0}, //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550, 0x379a62a2, 0}, //182165.25, remainder is ~piby4
    {0x42FCE5F1, 0x3F3504F3, 0}, //126.449104
    #if defined(_WIN64) || defined(_WIN32)
        {0x5123e87f, 0xb18a4ed8, 1}, //4.399877E+10, close to pi/2
    #else
        {0x5123e87f, 0xb18a4ed8, 32}, //4.399877E+10, close to pi/2
    #endif
    // newly added
    {0x34000000, 0x34000000, 0}, // sinf(1.19209e-007 = full prec)=1.192093e-007
    {0x00000001, 0x00000001, 0}, // sinf(1.4013e-045 = smallest)=1.401299e-045
    {0x40490fdb, 0xb3bbbd2e, 0}, // sinf(3.14159 = pi)=-8.742278e-008
    {0xc0490fdb, 0x33bbbd2e, 0}, // sinf(-3.14159 = -pi)=8.742278e-008
    {0x40c90fdb, 0x343bbd2e, 0}, // sinf(6.28319 = 2pi)=1.748456e-007
    {0xc0c90fdb, 0xb43bbd2e, 0}, // sinf(-6.28319 = -2pi)=-1.748456e-007
    {0x41fb5ed1, 0xb5155386, 0}, // sinf(31.4159 = +10pi)=-5.562837e-007
    {0x4203f267, 0x3f800000, 0}, // sinf(32.9867 = 21pi/2)=1.000000e+000
    {0x42c90fdb, 0x363bbd2e, 0}, // sinf(100.531 = 32pi)=2.797529e-006
    {0x42cc341a, 0x3f800000, 0}, //sinf(102.102 = 65pi/2)= 1.000000e+000

    {0x3ec90fdb, 0x3ec3ef16, 0},    // 0: sin(0.392699092627)=  0.382683442461
    {0x3f490fdb, 0x3f3504f3, 0},    // 1: sin(0.785398185253)=  0.707106796641
    {0x3f96cbe4, 0x3f6c835e, 0},    // 2: sin(1.178097248077)=  0.923879533652
    {0x3fc90fdb, 0x3f800000, 0},    // 3: sin(1.570796370506)=  1.000000000000
    {0x3ffb53d1, 0x3f6c835f, 0},    // 4: sin(1.963495373726)=  0.923879545816
    {0x4016cbe4, 0x3f3504f3, 0},    // 5: sin(2.356194496155)=  0.707106776970
    {0x402feddf, 0x3ec3ef17, 0},    // 6: sin(2.748893499374)=  0.382683499362
    {0x40490fdb, 0xb3bbbd2e, 0},    // 7: sin(3.141592741013)=  -0.000000087423
    {0x406231d6, 0xbec3ef15, 0},    // 8: sin(3.534291744232)=  -0.382683440628
    {0x407b53d1, 0xbf3504f2, 0},    // 9: sin(3.926990747452)=  -0.707106732018
    {0x408a3ae6, 0xbf6c835e, 0},    // 10: sin(4.319689750671)= -0.923879475869
    {0x4096cbe4, 0xbf800000, 0},    // 11: sin(4.712388992310)= -1.000000000000
    {0x40a35ce2, 0xbf6c835d, 0},    // 12: sin(5.105088233948)= -0.923879466742
    {0x40afeddf, 0xbf3504f5, 0},    // 13: sin(5.497786998749)= -0.707106883741
    {0x40bc7edd, 0xbec3ef15, 0},    // 14: sin(5.890486240387)= -0.382683418594
    {0x40c90fdb, 0x343bbd2e, 0},    // 15: sin(6.283185482025)= 0.000000174846
    {0xbec90fdb, 0xbec3ef16, 0},    // 16: sin(-0.392699092627)=    -0.382683442461
    {0xbf490fdb, 0xbf3504f3, 0},    // 17: sin(-0.785398185253)=    -0.707106796641
    {0xbf96cbe4, 0xbf6c835e, 0},    // 18: sin(-1.178097248077)=    -0.923879533652
    {0xbfc90fdb, 0xbf800000, 0},    // 19: sin(-1.570796370506)=    -1.000000000000
    {0xbffb53d1, 0xbf6c835f, 0},    // 20: sin(-1.963495373726)=    -0.923879545816
    {0xc016cbe4, 0xbf3504f3, 0},    // 21: sin(-2.356194496155)=    -0.707106776970
    {0xc02feddf, 0xbec3ef17, 0},    // 22: sin(-2.748893499374)=    -0.382683499362
    {0xc0490fdb, 0x33bbbd2e, 0},    // 23: sin(-3.141592741013)=    0.000000087423
    {0xc06231d6, 0x3ec3ef15, 0},    // 24: sin(-3.534291744232)=    0.382683440628
    {0xc07b53d1, 0x3f3504f2, 0},    // 25: sin(-3.926990747452)=    0.707106732018
    {0xc08a3ae6, 0x3f6c835e, 0},    // 26: sin(-4.319689750671)=    0.923879475869
    {0xc096cbe4, 0x3f800000, 0},    // 27: sin(-4.712388992310)=    1.000000000000
    {0xc0a35ce2, 0x3f6c835d, 0},    // 28: sin(-5.105088233948)=    0.923879466742
    {0xc0afeddf, 0x3f3504f5, 0},    // 29: sin(-5.497786998749)=    0.707106883741
    {0xc0bc7edd, 0x3ec3ef15, 0},    // 30: sin(-5.890486240387)=    0.382683418594
    {0xc0c90fdb, 0xb43bbd2e, 0},    // 31: sin(-6.283185482025)=    -0.000000174846
    {0xc100f282, 0xbf7aa0bf, 0},    // 32: sin(-8.059206008911)=    -0.979015284506
    {0x80000000, 0x80000000, 0},    // 33: sin(0.000000000000)= 0.000000000000
    {0x80000000, 0x80000000, 0},    // 34: sin(0.000000000000)= 0.000000000000
    {0xc081e530, 0x3f4b4e7a, 0},    // 35: sin(-4.059226989746)=    0.794166228295
    {0x80000000, 0x80000000, 0},    // 36: sin(0.000000000000)= 0.000000000000
    {0xc071ebc6, 0x3f188ed8, 0},    // 37: sin(-3.780015468597)=    0.595929645730
    {0x4052950b, 0xbe17c35f, 0},    // 38: sin(3.290346860886)= -0.148206213033
    {0xc0763ed7, 0x3f261714, 0},    // 39: sin(-3.847585439682)=    0.648789627409
    {0x40fe95bd, 0x3f7eacbb, 0},    // 40: sin(7.955778598785)= 0.994823161763
    {0xc10b719b, 0xbf26c7d3, 0},    // 41: sin(-8.715235710144)=    -0.651486563068
    {0x410438d2, 0x3f6acb4a, 0},    // 42: sin(8.263872146606)= 0.917164460006
    {0xc0f87e00, 0xbf7efef0, 0},    // 43: sin(-7.765380859375)=    -0.996077518366
    {0xc0e5ab38, 0xbf479158, 0},    // 44: sin(-7.177150726318)=    -0.779561513939
    {0x00000000, 0x00000000, 0},    // 45: sin(0.000000000000)= 0.000000000000
    {0x407a247b, 0xbf31a317, 0},    // 46: sin(3.908476591110)= -0.693894808066
    {0xc0999344, 0x3f7f090f, 0},    // 47: sin(-4.799226760864)=    0.996231968665
    {0x80000000, 0x80000000, 0},    // 48: sin(0.000000000000)= 0.000000000000
    {0x80000000, 0x80000000, 0},    // 49: sin(0.000000000000)= 0.000000000000
    {0x00000000, 0x00000000, 0},    // 50: sin(0.000000000000)= 0.000000000000
    {0x401cccc5, 0x3f2344a4, 0},    // 51: sin(2.449998140335)= 0.637766134505
    // addded from ancient libm repo
    {0x3f000000, 0x3ef57744},  //0.5
    {0x3f333333, 0x3f24eb73},  //.7
    {0x3f4ccccd, 0x3f37a4a6},  //.8
    {0x3fc00000, 0x3f7f5bd5},  //1.5
    {0xc07ae148, 0x3f33BFDD},  //-3.92

    {0x3f800000, 0x3f576aa4},  //1
    {0x447a0000, 0x3F53AE61},  //1000
};

static libm_test_special_data_f64
test_sin_conformance_data[] = {
    {POS_ZERO_F64, POS_ZERO_F64, 0},           //sin(0)=0
    {NEG_ZERO_F64, NEG_ZERO_F64, 0},           //sin(-0)=-0
    {POS_ONE_F64, 0x3feaed548f090cee, FE_INEXACT},          //sin(1)=0.84
    {POS_INF_F64, 0xfff8000000000000, FE_INVALID,}, //sin(inf)=inf
    {NEG_INF_F64, 0xfff8000000000000, FE_INVALID,}, //sin(-inf)=-inf
    {POS_SNAN_F64, 0x7ff4001000000000, FE_INVALID,}, //sin(snan)=snan
    {NEG_SNAN_F64, 0xfff2000000000000, FE_INVALID,}, //sin(-snan)=-snan
    {POS_QNAN_F64, 0x7ff87ff7e0000000, 0,},          //sin(qnan)=qnan
    {NEG_QNAN_F64, 0xfffa000000000000, 0,}, //sin(-qnan)
    {POS_PI_F64,         POS_ZERO_F64, 0,},  //sin(pi)=0
    {0x3ff921fb544486e0, 0x0000000000000000, 0,},  //sin(n*Pi)=sin(pi) for any n=1,2,3..
    {0x4012D97C7F336528, 0x0000000000000000, 0,}, //sin(270)

    {0xc000000000000000, 0xbfed18f6ead1b446, 0}, //-2
    {0x3f18000000000000, 0x3F17FFFFFF700000, 0}, //9.1552734375E-05  =  2^-14 *1.5
    {0xbf18000000000000, 0xbF17FFFFFF700000, 0}, //-9.1552734375E-05     = -2^-14 *1.5
    {0x412E848abcdef000, 0xbfee94b095ab2499, 0}, //1000005.3688883781 > 5e5
    {0x400921FB54442D18, 0x3ca1a62633145c07, 0}, //3.1415926535897931 = pi
    {0x400f6a7a2955385e, 0xbfe6a09e667f3bcc, 0},
    {0x400f6a7a2955385d, 0xbfe6a09e667f3bc9, 0},
    {0x400f6a7a2955385f, 0xbfe6a09e667f3bce, 0},
    {0x3e38000000000000, 0x3E38000000000000, 0}, // 5.5879354476928711E-09 = 2^-28*1.5
    {0xbe38000000000000, 0xBE38000000000000, 0}, // -5.5879354476928711E-09 = -2^-28*1.5
    {0x3f18000000000000, 0x3F17FFFFFF700000, 0}, //9.1552734375E-05  =  2^-14 *1.5
    {0xbf18000000000000, 0xbF17FFFFFF700000, 0}, //-9.1552734375E-05     = -2^-14 *1.5
    {0x3fe0000000000000, 0x3fdeaee8744b05f0, 0}, //0.5
    {0xbfe0000000000000, 0xBFDEAEE8744B05F0, 0}, //-0.5
    {0x3FEFFEE4E9DABC6B, 0x3FEAECBB972204E8, 0}, //0.999865013834904
    {0x3fe921fb54442d18, 0x3FE6A09E667F3BCC, 0}, //0.78539816339744828 = pi/4
    {0xbfe921fb54442d18, 0xBFE6A09E667F3BCC, 0}, //-0.78539816339744828 =-pi/4
    {0x412E848abcdef000, 0xbfee94b095ab2499, 0}, // 1000005.3688883781 > 5e5
    {0x439332270327F466, 0xbfefff7a431a18dc, 0}, // 3.4580273251621926E+17

    {0x3ff921fb54442d18LL, 0x3ff0000000000000LL, 0},  // 1.5707963267948966 = pi/2
    {0x400921fb54442d18LL, 0x3ca1a62633145c07LL, 0},  // 3.1415926535897931 = pi
    {0x4012D97C7F3321D2LL, 0xbff0000000000000LL, 0},  // 4.71238898038469 = 3pi/2
    {0x401921fb54442d18LL, 0xbcb1a62633145c07LL, 0},  // 6.2831853071795862 = 2pi
    {0x402921fb54442d18LL, 0xbcc1a62633145c07LL, 0},  // 12.566370614359172 = 4pi
    {0x410921fb54442d18LL, 0xbda1a62633145c07LL, 0},  // 205887.41614566068 =  (2^16)pi
    {0x403921fb54442d18LL, 0xbcd1a62633145c07LL, 0},  // 25.132741228718345 = 8pi
    {0x403921fb54442d19LL, 0x3ce72cece675d1fdLL, 0},  // 25.132741228718348 close to 8pi
    {0x3ff921fb57442d18LL, 0x3fefffffffffffffLL, 0},  // 1.5707963379707675 close to pi/2
    {0x400921fb52442d18LL, 0x3e5000000234c4c6LL, 0},  // 3.1415926386886319 close to pi
    {0x410921fb56442d18LL, 0x3f4fffffa6412185LL, 0},  // 205887.41712222318 close to (2^16)pi
    {0xbff921fb57442d18LL, 0xbfefffffffffffffLL, 0},  //-1.5707963379707675 close to - pi/2
    {0xc00921fb54442d18LL, 0xbca1a62633145c07LL, 0},  //-3.1415926535897931 = - pi
    {0x400921f554442d18LL, 0x3ee7fffffffeda62LL, 0},  // 3.1415812094979962 close to pi
    {0xc00921f554442d18LL, 0xbee7fffffffeda62LL, 0},  //-3.1415812094979962 close to -pi
    {0xbff921f557442d18LL, 0xbfeffffffffdc23fLL, 0},  //-1.570790615924869 close to -pi/2
    {0xbff9217557442d18LL, 0xbfeffffffb9df240LL, 0},  //-1.570668545612369 close to -pi/2
    {0x400921fb56442d18LL, 0xbe4ffffffb967673LL, 0},  // 3.1415926684909543 close to pi
    {0x4012D98C7F3321D2LL, 0xbfefffffff000000LL, 0},  // 4.71245001554094 close to 3pi/2
    //newly added
    {0x400921fb54442d18LL, 0x3ca1a62633145c07LL, 0}, // sin(3.14159 = pi)=0
    {0xc00921fb54442d18LL, 0xbca1a62633145c07LL, 0}, // sin(-3.14159 = -pi)=0
    {0x401921fb54442d18LL, 0xbcb1a62633145c07LL, 0}, // sin(6.28319 = 2pi)=0
    {0xc01921fb54442d18LL, 0x3cb1a62633145c07LL, 0}, // sin(-6.28319 = -2pi)=0
    {0x403f6a7a2955385eLL, 0xbcd60fafbfd97309LL, 0}, // sin(31.4159 = 10pi)=0
    {0x405921fb54442d18LL, 0xbcf1a62633145c07LL, 0}, // sin(100.531 = 32pi)=0

    {0x3cb0000000000000LL, 0x3cafffffffffffffLL, 0}, // sin(2.22045e-016 = full prec)=2.220446e-016
    {0x0000000000000001LL, 0x0000000000000001LL, 0}, // sin(4.94066e-324 = smallest)=4.940e-324

    {0x7fe0000001ffbcacLL, 0x3ff0000000000000LL, 0}, // sin(8.9884657412464672e+307 = very close to max double)=1.000000000000
    {0x40407e4cef4cbd97LL, 0x3fefffffffffffffLL, 0}, // sin(32.9867 = 21pi/2)=1.000000e+000
    {0x4059868341953dccLL, 0x3fefffffffffffffLL, 0}, // sin(102.102 = 65pi/2)=1.000000e+000

    {0x3fd921fb54442d18LL, 0x3fd87de2a6aea963LL, 0},    // sin(0.392699081699)= 0.382683432365
    {0x3fe921fb54442d18LL, 0x3fe6a09e667f3bccLL, 0},    // sin(0.785398163397)= 0.707106781187
    {0x3ff2d97c7f3321d2LL, 0x3fed906dcf328d46LL, 0},    // sin(1.178097245096)= 0.923879532511
    {0x3ff921fb54442d18LL, 0x3ff0000000000000LL, 0},    // sin(1.570796326795)= 1.000000000000
    {0x3fff6a7a2955385eLL, 0x3fed906dcf328d46LL, 0},    // sin(1.963495408494)= 0.923879532511
    {0x4002d97c7f3321d2LL, 0x3fe6a09e667f3bcdLL, 0},    // sin(2.356194490192)= 0.707106781187
    {0x4005fdbbe9bba775LL, 0x3fd87de2a6aea965LL, 0},    // sin(2.748893571891)= 0.382683432365
    {0x400c463abeccb2bbLL, 0xbfd87de2a6aea961LL, 0},    // sin(3.534291735289)= -0.382683432365
    {0x400f6a7a2955385eLL, 0xbfe6a09e667f3bccLL, 0},    // sin(3.926990816987)= -0.707106781187
    {0x4011475cc9eedf00LL, 0xbfed906bcf328d44LL, 0},    // sin(4.319689898686)= -0.923879532511
    {0x4012d97c7f3321d2LL, 0xbff0000000000000LL, 0},    // sin(4.712388980385)= -1.000000000000
    {0x40146b9c347764a4LL, 0xbfed906bcf328d45LL, 0},    // sin(5.105088062083)= -0.923879532511
    {0x4015fdbbe9bba775LL, 0xbfe6a09e667f3bceLL, 0},    // sin(5.497787143782)= -0.707106781187
    {0x40178fdb9effea46LL, 0xbfd87de2a6aea96eLL, 0},    // sin(5.890486225481)= -0.382683432365
    {0xbfd921fb54442d18LL, 0xbfd87de2a6aea963LL, 0},    // sin(-0.392699081699)=    -0.382683432365
    {0xbfe921fb54442d18LL, 0xbfe6a09e667f3bccLL, 0},    // sin(-0.785398163397)=    -0.707106781187
    {0xbff2d97c7f3321d2LL, 0xbfed906bcf328d46LL, 0},    // sin(-1.178097245096)=    -0.923879532511
    {0xbff921fb54442d18LL, 0xbff0000000000000LL, 0},    // sin(-1.570796326795)=    -1.000000000000
    {0xbfff6a7a2955385eLL, 0xbfed906bcf328d46LL, 0},    // sin(-1.963495408494)=    -0.923879532511
    {0xc002d97c7f3321d2LL, 0xbfe6a09e667f3bcdLL, 0},    // sin(-2.356194490192)=    -0.707106781187
    {0xc005fdbbe9bba775LL, 0xbfd87de2a6aea965LL, 0},    // sin(-2.748893571891)=    -0.382683432365
    {0xc00c463abeccb2bbLL, 0x3fd87de2a6aea961LL, 0},    // sin(-3.534291735289)=    0.382683432365
    {0xc00f6a7a2955385eLL, 0x3fe6a09e667f3bccLL, 0},    // sin(-3.926990816987)=    0.707106781187
    {0xc011475cc9eedf00LL, 0x3fed906bcf328d44LL, 0},    // sin(-4.319689898686)=    0.923879532511
    {0xc012d97c7f3321d2LL, 0x3ff0000000000000LL, 0},    // sin(-4.712388980385)=    1.000000000000
    {0xc0146b9c347764a4LL, 0x3fed906bcf328d45LL, 0},    // sin(-5.105088062083)=    0.923879532511
    {0xc015fdbbe9bba775LL, 0x3fe6a09e667f3bceLL, 0},    // sin(-5.497787143782)=    0.707106781187
    {0xc0178fdb9effea46LL, 0x3fd87de2a6aea96eLL, 0},    // sin(-5.890486225481)=    0.382683432365
    {0xc0201e5040097167LL, 0xbfef5417dc88d953LL, 0},    // sin(-8.059206010010)=    -0.979015284282
    {0x89b8254bfc0c9a3bLL, 0x89b8254bfc0c9a3bLL, 0},    // sin(-0.000000000000)=    -0.000000000000
    {0x9bcf10572f0d0b62LL, 0x9bcf10572f0d0b62LL, 0},    // sin(-0.000000000000)=    -0.000000000000
    {0xc0103ca602810042LL, 0x3fe969cf576ffa60LL, 0},    // sin(-4.059227027057)=    0.794166250969
    {0xa0c41c2aefd3f2b1LL, 0xa0c41c2aefd3f2b1LL, 0},    // sin(-0.000000000000)=    -0.000000000000
    {0xc00e3d78b6995cf5LL, 0x3fe311daee318dfdLL, 0},    // sin(-3.780015398555)=    0.595929589483
    {0x400a52a1648e1512LL, 0xbfc2f86c1b18a78fLL, 0},    // sin(3.290346894823)= -0.148206246596
    {0xc00ec7dae5c2205aLL, 0x3fe4c2e2887d99a1LL, 0},    // sin(-3.847585482585)=    0.648789660056
    {0x401fd2b7a8bf5589LL, 0x3fefd5975b05e863LL, 0},    // sin(7.955778729132)= 0.994823148517
    {0xc0216e336075e013LL, 0xbfe4d8fa53ae583eLL, 0},    // sin(-8.715235723867)=    -0.651486552658
    {0x4020871a38ce46adLL, 0x3fed596975f7fda6LL, 0},    // sin(8.263871932202)= 0.917164545448
    {0xc01f0fc004e4af8fLL, 0xbfefdfddf92bfc5dLL, 0},    // sin(-7.765380932291)=    -0.996077524818
    {0xc01cb566f5676ceeLL, 0xbfe8f22ac7db24b0LL, 0},    // sin(-7.177150568426)=    -0.779561415047
    {0x30a93e9db15c19e7LL, 0x30a93e9db15c19e7LL, 0},    // sin(0.000000000000)= 0.000000000000
    {0x400f448f6535e00fLL, 0xbfe63462f17241b8LL, 0},    // sin(3.908476629931)= -0.693894836020
    {0xc013326877ba5aaeLL, 0x3fefe121e351e782LL, 0},    // sin(-4.799226637601)=    0.996231979355
    {0xad424b7a323fec56LL, 0xad424b7a323fec56LL, 0},    // sin(-0.000000000000)=    -0.000000000000
    {0x96ca3a8c4e1a36daLL, 0x96ca3a8c4e1a36daLL, 0},    // sin(-0.000000000000)=    -0.000000000000
    {0x0611f25d4e96f5f0LL, 0x0611f25d4e96f5f0LL, 0},    // sin(0.000000000000)= 0.000000000000
    {0x40039998a2086896LL, 0x3fe468948002f939LL, 0},    // sin(2.449998155481)= 0.637766122840
    // added from ancient libm repo
    {0x0000000000000000, 0x0000000000000000},  // 0
    {0x8000000000000000, 0x8000000000000000},  // -0

    //Scalar Precision cos - 0.5*x*xx term
    {0x3FE9E0C8F112AB1E, 0x3FE725F420C5EF11},  // 0.80868956646495227

    //Scalar precision cos +xx term, Spotted by vector check
    {0x40306B51F0157E66, 0xBFE4E39E91D623F6},  // 16.41921902203321
    {0x402DDF5ADB92C01A, 0x3FE650DDA3993550},  // 14.93624006430041
    {0x402DDB778A9EBD8A, 0x3FE67D4903528613},  // 14.928646404149521

    //near piby4
    {0x401c462b9064a63b, 0x3FE6A0488506901A},  // 7.0685255586081963

    //very close to piby4
    {0x3FE921FB54442D19, 0x3FE6A09E667F3BCD},  // 0.78539816339744839
    {0x3fe921fb54442d20, 0x3FE6A09E667F3BD2},  // 0.78539816339744917

    //cos expdiff is < 15
    {0x3FF10CA44655D48A, 0x3FEC0098D09C9F69},  // 1.0655863521072058
    {0x400923E979F8B36A, 0xBF4EE25AFBAE2B42},  // 3.1425351647398445
    {0x4002DAE59BB5C33E, 0x3FE69CA0ABE92212},  // 2.3568832554668964
    {0x4015FDCA5F9A0E38, 0xBFE6A04C98FCC90E},  // 5.4978423059251824
    {0x40b93bda357dadda, 0x3FE586F2678A5A63},  // 6459.8523787068789
    {0x40F63525129291FF, 0x3FE33228B38E66C1},  // 90962.317034311578
    {0x3FF9207824B27C17, 0x3FEFFFFFDB667252},  // 1.5704270776257141
    {0x4025fe9b31eb183d, 0xBFEFFFFCF504B054},  // 10.997277793834764

    //cos expdiff is > 15
    {0x4046C6CBC45DC8DE, 0x3FF0000000000000},  // 45.553093477052
    {0x411FA317083EE0A2, 0xBFEA3D31BD2D673C},  // 518341.75805235852

    //integer
#ifdef ACCEPTABLE
    //MS & Gcc answer for the following case 0x3fe14a280fb5068c
    {0x3FF0000000000000, 0x3feaed548f090cee},  // 1
#else
    {0x3FF0000000000000, 0x3feaed548f090cee},  // 1
#endif
    {0x4000000000000000, 0x3fed18f6ead1b446},  // 2
    {0x4008000000000000, 0x3FC210386DB6D55B},  // 3
    {0x4024000000000000, 0xBFE1689EF5F34F52},  // 10
    {0x4002CCCCCCCCCCCD, 0x3FE6C463C3C6C9E8},
    {0x400F5C28F5C28F5C, 0xBFE677FB7E636920},
    {0x7506ac5b262ca1ff, 0x3FF0000000000000},
    {0xC002CCCCCCCCCCCD, 0xBFE6C463C3C6C9E8},
    {0xC00F5C28F5C28F5C, 0x3FE677FB7E636920},
    {0xF506ac5b262ca1ff, 0xBFF0000000000000},
    {0x7ff0000000000000, 0x7ff8000000000000},
    {0xfff0000000000000, 0xfff8000000000000},
    {0x7ff8000000000000, 0x7ff8000000000000},
    {0xfff8000000000000, 0xfff8000000000000},
    {0xfff4000000000000, 0xfffc000000000000},
    {0x7ff4000000000000, 0x7ffc000000000000},

    {0x3FE0000000000000LL, 0x3FDEAEE8744B05F0},
    {0xBFE83DCA43349085LL, 0xBFE5FD095C9B6AB0},
    {0xBFE6EF49DAE5CECELL, 0xBFE5056342FA56AA},
};

#endif  /*__TEST_SIN_DATA_H__*/
