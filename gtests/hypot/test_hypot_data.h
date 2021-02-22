#ifndef __TEST_HYPOT_DATA_H__
#define __TEST_HYPOT_DATA_H__
/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */

#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the hypot() routine. These test cases are not exhaustive */
static libm_test_special_data_f64
test_hypot_conformance_data[] = {
    {0x7FF4001000000000,0x7FF4001000000000,FE_INVALID,0x7ffc001000000000,}, //hypot(snan,snan)
    {0x7FFC001000000000,0x7FF4001000000000,0         ,0x7ffc001000000000,}, //hypot(nan,snan)
    {0x3FF0000000000000,0x7FF4001000000000,0         ,0x7ffc001000000000,}, //hypot(1.0,snan)
    {0x7FF4001000000000,0x3FF0000000000000,FE_INVALID,0x7ffc001000000000,}, //hypot(snan,1.0)
    {0x3FE0000000000000,0x7FF4001000000000,0         ,0x7ffc001000000000,}, //hypot(0.5,snan)
    {0x7FF4001000000000,0x3FE0000000000000,FE_INVALID,0x7ffc001000000000,}, //hypot(snan,0.5)
    {0x7ff8000000000000,0x7ff8000000000000,0         ,0x7ff8000000000000,}, //hypot(qnan,qnan)
    {0x7ff8000000000000,0x3ff0000000000000,0         ,0x7ffc001000000000,}, //hypot(qnan,1)
    {0x3ff0000000000000,0x7ff8000000000000,0         ,0x3ff0000000000000,},//hypot(1,qnan)
    {0xfff8000000000000,0xfff8000000000000,0         ,0xfff8000000000000,},  //hypot(-qnan, -qnan)
    {0x7ff0000000000000,0x7ff0000000000000,0         ,0x7ff0000000000000,}, //hypot(inf, inf)=inf
    {0xfff0000000000000,0x7ff0000000000000,0         ,0x7ff0000000000000,}, //hypot(-inf, inf)=inf
    {0xfff0000000000000,0xfff0000000000000,0         ,0x0,},                  //hypot(-inf, -inf)=0
//newly added 
    {0x40f0000000000000,0x7feffffffffffd3a, 0,0x404fffffffffffff},  // MM hypot(6.553600e+004, 6.400000e+001) = 1.797693e+308
    {0x4000000000000000,0x7feffffffffffd3a, 0,0x408fffffffffffff}, // MM hypot(2.000000e+000, 1.024000e+003) = 1.797693e+308
    {0xcf81afd6ec0e1411,0x0000000000000000, 0,0xc7eff933bffffc3c}, //VCQA#1332, regression point from SQL  underflow
    {0x7fac7b1f3cac7433,0x3ff0000000000000, 0,0x0000000000000000}, 	// 3: hypot(1.000000000000e+307, +0)=	1.000000000000e+000
    {0x7fac7b1f3cac7433,0x3ff0000000000000, 0,0x8000000000000000}, 	// 3: hypot(1.000000000000e+307, -0)=	1.000000000000e+000
    {0x7fac7b1f3cac7433,0x0000000000000000, 0,0xffac7b1f3cac7433}, 	// 4: hypot(1.000000000000e+307)=	0.000000000000e+000   underflow
    {0xffac7b1f3cac7433,0x0000000000000000, 0,0xffac7b1f3cac7433}, 	// 5: hypot(-1.000000000000e+307)=	0.000000000000e+000
    {0x0031fa182c40c60d,0x3ff0000000000000, 0,0x0031fa182c40c60d}, 	// 6: hypot(1.000000000000e-307)=	1.000000000000e+000
    {0x0000000000000000,0x3ff0000000000000, 0,0x0000000000000000}, 	// 7: hypot(0.000000000000e+000)=	1.000000000000e+000
    {0x4021e3017be56364,0x40a0d68c2e7b795a, 0,0x400c06fdc25b187b}, 	// 8: hypot(8.943370696784e+000)=	2.155273792132e+003
    {0x3fc573f8057439df,0x3eb3f6da33716413, 0,0x401e8ca7e04e31a7}, 	// 9: hypot(1.676015879279e-001)=	1.189963041678e-006
    {0x401526cae18eb77f,0x40fc1712061c29d2, 0,0x401bfd158e752770}, 	// 10: hypot(5.287883304916e+000)=	1.150571264917e+005
    {0x3fe9383ab322cb3d,0x3fefab210e0396f1, 0,0x3fa664a771479db4}, 	// 11: hypot(7.881139277218e-001)=	9.896397851669e-001
    {0x3f777255f2d27c89,0x3feceed63a638534, 0,0x3f93fba1eb4c1ff4}, 	// 12: hypot(5.724273430274e-003)=	9.041548862844e-001
    {0x403584c37e828423,0x40257986f7e30616, 0,0x3fe8c04d0b6234c4}, 	// 13: hypot(2.151860800444e+001)=	1.073735785147e+001
    {0x407c86106e4309be,0x34308d8318ad965a, 0,0xc0354691264a785d}, 	// 14: hypot(4.563790114039e+002)=	2.637010250922e-057
    {0x3f65ef068f1e80cd,0x3ab2da7eece7d710, 0,0x40239afa4b94fcbd}, 	// 15: hypot(2.677452860510e-003)=	6.091894156354e-026
    {0x400c5802b788457b,0x40c002000274ceea, 0,0x401c7ecc58dbc978}, 	// 16: hypot(3.542973932120e+000)=	8.196000074960e+003
    {0x3fc7de70d38b84ab,0x49a458e8db855522, 0,0xc050075c416580b9}, 	// 17: hypot(1.864758522925e-001)=	5.808128287168e+046
    {0x3fb2ee3f8bc8fa26,0x3ff05eb04e05b2cb, 0,0xbf81f8c13b36cd39}, 	// 18: hypot(7.394787943574e-002)=	1.023117356085e+000
    {0x40608ce87cec11a0,0x3ff331b743d5fcb8, 0,0x3fa313027d609c72}, 	// 19: hypot(1.324033798800e+002)=	1.199637665733e+000
    {0x3fe23fb357b8260f,0x3fe82f43f1d848ef, 0,0x3fdfe8b1e9297c37}, 	// 20: hypot(5.702759469288e-001)=	7.557697032483e-001
    {0x3ee3d5de431afb45,0x3abced5df0869a2b, 0,0x4013ed54291ca336}, 	// 21: hypot(9.458266592486e-006)=	9.346887162818e-026
    {0x3f815884e974ce7c,0x4014478f417dd1f5, 0,0xbfd5c649ed552720}, 	// 22: hypot(8.469618200798e-003)=	5.069882415108e+000
    {0x40542f695c7a3bd2,0x3fcab890aae01e54, 0,0xbfd6d503724443de}, 	// 23: hypot(8.074080573975e+001)=	2.087574800828e-001
    {0x3f75b2e0e1231f68,0x3ff39c1e9e93c66b, 0,0xbfa3e05ec618fc3c}, 	// 24: hypot(5.297544894525e-003)=	1.225615138476e+000
    {0x401813103e13f316,0x3796160711540be1, 0,0xc049c8db97ff54bd}, 	// 25: hypot(6.018616647692e+000)=	6.338384163651e-041
    {0x3fa9d58fcdfcb489,0x29f5e5c25c056086, 0,0x405465a8f91f228f}, 	// 26: hypot(5.045747174732e-002)=	1.491818601073e-106
    {0x40683343b1944b6e,0x3ff558ef7b5f5cd4, 0,0x3fac0927494b12ea}, 	// 27: hypot(1.936020133873e+002)=	1.334212762760e+000
    {0x4020e69458a094c0,0x3ff23ed91ab65f00, 0,0x3faf81981d3eb3b8}, 	// 28: hypot(8.450350541689e+000)=	1.140343765588e+000
    {0x4079d300475c2f5e,0x3b42ed765159bdaf, 0,0xc021344226e9793e}, 	// 29: hypot(4.131875680541e+002)=	3.131306541540e-023
    {0x3f9f73bf6ec4639e,0x2df0f0271d19fd47, 0,0x404ca626c650afd1}, 	// 30: hypot(3.071498025483e-002)=	2.128657931514e-087
    {0x3fe099dae7e6fbe3,0x41e8843956f18eac, 0,0xc040b22f5d07e39c}, 	// 31: hypot(5.187811402646e-001)=	3.290548919549e+009
    {0x3fcc37928b4b0066,0x404a27973dc66bba, 0,0xc004ef9e1cde7d45}, 	// 32: hypot(2.204459362165e-001)=	5.230930301846e+001
    {0x40151fd9269bf123,0x3ff2316f1de1038c, 0,0x3fb3c2b16194d880}, 	// 33: hypot(5.281101802128e+000)=	1.137068859782e+000
    {0x3fadf92a024149c7,0x3feeb49cd5f52ccb, 0,0x3f8dcc751b10eb66}, 	// 34: hypot(5.854159619834e-002)=	9.595474413740e-001
    {0x3fb584d8a9649a11,0x3f8c5d51c3f959bb, 0,0x3ffba6c37708b4c7}, 	// 35: hypot(8.405832419940e-002)=	1.384986762347e-002
    {0x40068209ee0007dc,0x07a4ee2efd3c2035, 0,0xc082dbd8359543f8}, 	// 36: hypot(2.813495501877e+000)=	7.738050249698e-272
    {0x4097fba54a4cc661,0x3b0fef57583bc358, 0,0xc01d7acff9a42c2b}, 	// 37: hypot(1.534911416244e+003)=	3.301994010471e-024
    {0x40448fa640cebc0f,0x60014edbc3a6d6f6, 0,0x4057ec9d946a87a1}, 	// 38: hypot(4.112226114363e+001)=	2.900786089752e+154
    {0x3fef66b8af6e513f,0x3fee043fe9af7a85, 0,0x400b19d54ddec50a}, 	// 39: hypot(9.812892366777e-001)=	9.380187572641e-001
    {0x401affbbbbef43f4,0x40182e8c7479cc90, 0,0x3fee273e2b5139d0}, 	// 40: hypot(6.749739586333e+000)=	6.045457668242e+000
    {0x3fed17d0f9378252,0x3ff022495f45aa45, 0,0xbfb6683aea72060d}, 	// 41: hypot(9.091572635189e-001)=	1.008370754391e+000
    {0x3f697e8380f362ef,0x41354cb64bea6aba, 0,0xc0039be91950bce8}, 	// 42: hypot(3.112084239188e-003)=	1.395894296546e+006
    {0x40700f7f1a40cb0c,0x3dbe2134c32d2734, 0,0xc0039be91950bce8}, 	// 43: hypot(2.569685308963e+002)=	2.740281283009e-011
    {0x4006740622611915,0x2638aad2f1626247, 0,0xc07144dda1fdc6c4}, 	// 44: hypot(2.806652325237e+000)=	1.457615873192e-124
    {0x4016f9701ccf292c,0x3fd33b5ef7ab9835, 0,0xbfe6025c13b98696}, 	// 45: hypot(5.743591737885e+000)=	3.004987162261e-001
    {0x3fb8520b0a0f62cc,0x4018c0548848a0a2, 0,0xbfe8c70858ba191d}, 	// 46: hypot(9.500187869652e-002)=	6.187822465356e+000
    {0x4021c606927119e8,0x513a0627b3ae89d3, 0,0x4055f2eb0596b3c2}, 	// 47: hypot(8.886768890673e+000)=	1.974847631796e+083
    {0x3ff10d2949233825,0x4049c2f2800dea12, 0,0x404ef8208d0c4c7a}, 	// 48: hypot(1.065713201231e+000)=	5.152302551917e+001
    {0x408482e3e67944e2,0x3ff093c0f996a5ce, 0,0x3f7660702f8afa6c}, 	// 49: hypot(6.563612794375e+002)=	1.036072707133e+000
    {0x3fc2266705a4b585,0x3fed4992ef85fb61, 0,0x3fa737b1d35432f6}, 	// 50: hypot(1.417969491535e-001)=	9.152311971846e-001
    {0x3f84780f05105163,0x4047be118f8171ff, 0,0xbfead25e893d4cdc}, 	// 51: hypot(9.994618742021e-003)=	4.748491090603e+001
    {0x3fe02b798c641dd3,0x452a03ac0ca8627c, 0,0xc0553fbcbcb0cefd}, 	// 52: hypot(5.053069822324e-001)=	1.572470608075e+025
    {0x40229b731ecfd109,0x3ff078219bfd325c, 0,0x3f8a8b048497fd89}, 	// 53: hypot(9.303612673634e+000)=	1.029328927357e+000
    {0x408d632415d16ce8,0x400842652122abfb, 0,0x3fc4bda9d0ad3f38}, 	// 54: hypot(9.403926197397e+002)=	3.032419451590e+000
    {0x40936171564bf818,0x3fe5b510226de88d, 0,0xbfabe5259a4cd194}, 	// 55: hypot(1.240360680758e+003)=	6.783524200867e-001
    {0x3f9edd43bf8ee9c0,0x401f543db84414a2, 0,0xbfe2cee95f4da0ad}, 	// 56: hypot(3.014093262366e-002)=	7.832266692310e+000
    {0x4033561d633a800f,0x4003ea783264378e, 0,0x3fd3b5140e278140}, 	// 57: hypot(1.933638591936e+001)=	2.489487069782e+000
    {0x3f788d811b24e029,0x4114c66c9db968b3, 0,0xc003ea2bbf26a17e}, 	// 58: hypot(5.994324036060e-003)=	3.403791540276e+005
    {0x3fa96d554bdf28ac,0x3d8ac41b963ccff6, 0,0x4021a9fd243bd880}, 	// 59: hypot(4.966227103394e-002)=	3.042947121476e-012
    {0x3fafa4982eda9507,0x400b1063afff9142, 0,0xbfdc04fc48137377}, 	// 60: hypot(6.180263111063e-002)=	3.383002638804e+000
    {0x3fae22807850f625,0x4091d90b5bbf6c51, 0,0xc003e277ae62e399}, 	// 61: hypot(5.885697811798e-002)=	1.142261092177e+003
    {0x4057994e1ca319d5,0x59c944075c8cb9a2, 0,0x404f869ab476dcff}, 	// 62: hypot(9.439539256981e+001)=	3.340413293301e+124
    {0x40042b8c76774661,0x3fb8f4b4342f7da4, 0,0xc00423cdf3fe3b6a}, 	// 63: hypot(2.521264005198e+000)=	9.748388551770e-002
    {0x4012c66adb0cfd59,0x3ebcfd3cf8cbf2e4, 0,0xc021299787559452}, 	// 64: hypot(4.693766997021e+000)=	1.727891660141e-006
    {0x3f545b0b372c3cbf,0x399c1d57af20217d, 0,0x4024f731dbd7e3a0}, 	// 65: hypot(1.242409660964e-003)=	3.465394367871e-031
    {0x404c6d9b10d8ad15,0x3671745cae03ec52, 0,0xc03a0dca5f69dc3a}, 	// 66: hypot(5.685629473286e+001)=	1.910860855425e-046
    {0x3f6249fadcd18a70,0x4c7c09cd4a4c95d2, 0,0xc036cd04b9c2f126}, 	// 67: hypot(2.232542005502e-003)=	2.815987053543e+060
    {0x4001668312ad7541,0x3feec002b13c531a, 0,0xbfaa40e8a9ec4d10}, 	// 68: hypot(2.175054689330e+000)=	9.609387838014e-001
    {0x3f9c4dd5ff03b86f,0x42e60b0643fe1b8c, 0,0xc02255e91b7d4808}, 	// 69: hypot(2.764067048059e-002)=	1.938928445647e+014
    {0x40222f50f707d048,0x1485cf2d81491ec7, 0,0xc06b42faad7576dd}, 	// 70: hypot(9.092414588646e+000)=	8.292290735345e-210
    {0x3fe21868ba4210da,0x1c08ef4cb728b1c4, 0,0x4085d2cc5d78bc2e}, 	// 71: hypot(5.654796254971e-001)=	1.260195076433e-173
    {0x3f7ab2aaab14164d,0x42f097e3be63fbb6, 0,0xc01a7866ec3b5ad6}, 	// 72: hypot(6.518046067190e-003)=	2.919127521853e+014
    {0x3f920c5e187ebd13,0x4014fac9afe12e84, 0,0xbfda438cc5f95a9e}, 	// 73: hypot(1.762530350214e-002)=	5.244909999961e+000
    {0x405897e1295c1e0c,0x4125285862d71061, 0,0x4007727a525aacbb}, 	// 74: hypot(9.837311777111e+001)=	6.932921930471e+005
    {0x40138e40409795af,0x406f507275f4b6d2, 0,0x400bd82523463392}, 	// 75: hypot(4.888916978123e+000)=	2.505139722614e+002
    {0x40bd7da8cf4897e5,0x72a28b0f1dd07f17, 0,0x404f7c5e3d3bfb25}, 	// 76: hypot(7.549659412896e+003)=	1.582672912517e+244
    {0x4041e1ced3f3c5ad,0x40d32dd55e5c8963, 0,0x40061be0b89188dd}, 	// 77: hypot(3.576412438777e+001)=	1.963933388437e+004
    {0x401cbf819c4c995c,0x4008d7b79320f998, 0,0x3fe26280440b3af6}, 	// 78: hypot(7.187017862487e+000)=	3.105330609745e+000
    {0x3fc6e19a2ffc07ce,0x409c890c3c9ec837, 0,0xc01172a3b5a9e6f4}, 	// 79: hypot(1.787598356532e-001)=	1.826261949998e+003
    {0x4041195327f82deb,0x4039ef60f6a64562, 0,0x3fed7e9210adc251}, 	// 80: hypot(3.419785022371e+001)=	2.593507329521e+001
    {0x401b9301dc94e6ae,0x41523faab6bb3e47, 0,0x401fde1331a352d5}, 	// 81: hypot(6.893561789120e+000)=	4.783786855178e+006
    {0x4056bafb91e4b34c,0x3f1094fea10301d8, 0,0xc00126691ecf64cd}, 	// 82: hypot(9.092160460791e+001)=	6.325534946239e-005
    {0x3fcfacd13df58595,0x3376a37ebb2033aa, 0,0x4058c1523347473e}, 	// 83: hypot(2.474614670310e-001)=	8.805042527879e-061
    {0x4076b4c8f389e828,0x3ff34d4d36cb9ede, 0,0x3fa04b6afa1e6c21}, 	// 84: hypot(3.632990603816e+002)=	1.206372465176e+000
    {0x40160e2c19e60411,0x3ff0c2e1345cc65d, 0,0x3f9be0fa1e031383}, 	// 85: hypot(5.513840107597e+000)=	1.047578053036e+000
    {0x4010b9a553c9bc29,0x3f2befa32bce9607, 0,0xc017a2d8e937153f}, 	// 86: hypot(4.181294736088e+000)=	2.131354030130e-004
    {0x3f5e062b6cf1aafa,0x3fd3f349bfcfe206, 0,0x3fc7accf1ed54c5e}, 	// 87: hypot(1.832525642181e-003)=	3.117241261639e-001
    {0x40111aa5cfff00ee,0x42d2383d4f2359a0, 0,0x4036087c966fbbfc}, 	// 88: hypot(4.276023149432e+000)=	8.013102426045e+013

};

/* Test cases to check for exceptions for the hypot() routine. These test cases are not exhaustive */
static libm_test_special_data_f32
test_hypotf_conformance_data[] = {
    {0x7fbfffff, 0x7fa00000, FE_INVALID, 0x7fbfffff}, //nan, snan
    {0x7fa00000, 0x7fa00000, FE_INVALID, 0x7fa00000}, //hypotf(snan, snan)
    {0x3f800000, 0x7fa00000, 0         , 0x3f800000},//1.0, nan
    {0x3f800000, 0x7ffe0000, 0         , 0x3f800000},//1.0, qnan
    {0x3f000000, 0x7fa00000, FE_INVALID, 0x7fa00000}, // 0.5, snan
    {0x7fa00000, 0x3f000000, FE_INVALID, 0x7fe00000},//snan, 0.5
    {0x7ffe0000, 0x7ffe0000, 0         , 0x7ffe0000}, //qnan , qnan
    {0xffc00000, 0xffc00000, 0         , 0xffc00000}, //-qnan, -qnan=-qnan
    {0x7ffe0000, 0x3f800000, 0         , 0x7ffe0000}, //qnan, 1.0
    {0x3f800000, 0x7ffe0000, 0         , 0x3f800000}, //1.0, qnan
    {0x7f800000, 0x7f800000, 0         , 0x7f800000},    //inf, inf=inf
    {0x7f800000, 0xff800000, 0         , 0x0},         //inf, -inf =0
    {0xff800000, 0xff800000, 0         , 0x0},         //-inf, -inf=0
   // newly added  
    {0x40000000, 0x42fe0000, 0         , 0x7f000000}, // hypotf(2.000000e+000, 1.270000e+002) = 1.701412e+038
    {0x47800000, 0x40fe0000, 0         , 0x7f000000}, // hypotf(6.553600e+004, 7.937500e+000) = 1.701412e+038
    {0x40000000, 0xc3150000, 0         , 0x00000001}, // hypotf(2.000000e+000, -1.490000e+002) = 1.401298e-045
    {0x47800000, 0x00000001, 0         , 0xc1150000}, // hypotf(6.553600e+004, -9.312500e+000) = 1.401298e-045
    {0x7f800000, 0x3f800000, 0         , 0x00000000}, 	// 3: hypot(1.#INF00000000)=	1.000000000000
    {0x7f800000, 0x00000000, 0         , 0xff800000}, 	// 4: hypot(1.#INF00000000)=	0.000000000000
    {0xff800000, 0x00000000, 0         , 0xff800000}, 	// 5: hypot(-1.#INF00000000)=	0.000000000000
    {0x00000000, 0x3f800000, 0         , 0x00000000}, 	// 6: hypot(0.000000000000)=	1.000000000000
    {0x00000000, 0x3f800000, 0         , 0x00000000}, 	// 7: hypot(0.000000000000)=	1.000000000000
    {0x410f180c, 0x4506b462, 0         , 0x406037ee}, 	// 8: hypot(8.943370819092)=	2155.273812512344
    {0x3e2b9fc0, 0x359fb6d0, 0         , 0x40f4653f}, 	// 9: hypot(0.167601585388)=	0.000001189963
    {0x40a93657, 0x47e0b88a, 0         , 0x40dfe8ac}, 	// 10: hypot(5.287883281708)=	115057.081676902220
    {0x3f49c1d6, 0x3f7d5908, 0         , 0x3d33353c}, 	// 11: hypot(0.788113951683)=	0.989639786079
    {0x3bbb92b0, 0x3f6776b2, 0         , 0x3c9fdd0f}, 	// 12: hypot(0.005724273622)=	0.904154890489
    {0x41ac261c, 0x412bcc37, 0         , 0x3f460268}, 	// 13: hypot(21.518608093262)=	10.737357187046
    {0x43e43083, 0x00000000, 0         , 0xc1aa3489}, 	// 14: hypot(456.378997802734)=	0.000000000000  underflow
    {0x3b2f7834, 0x1596d408, 0         , 0x411cd7d2}, 	// 15: hypot(0.002677452751)=	0.000000000000
    {0x4062c016, 0x46001002, 0         , 0x40e3f663}, 	// 16: hypot(3.542973995209)=	8196.002217932017
    {0x3e3ef387, 0x7f800000, 0         , 0xc2803ae2}, 	// 17: hypot(0.186475858092)=	58081134573255415000000000000000000000000000000.000000000000 overflow
    {0x3d9771fc, 0x3f82f582, 0         , 0xbc0fc60a}, 	// 18: hypot(0.073947876692)=	1.023117356790
    {0x43046744, 0x3f998dba, 0         , 0x3d189814}, 	// 19: hypot(132.403381347656)=	1.199637668018
    {0x3f11fd9b, 0x3f417a20, 0         , 0x3eff458f}, 	// 20: hypot(0.570275962353)=	0.755769717062
    {0x371eaef2, 0x15e76b07, 0         , 0x409f6aa1}, 	// 21: hypot(0.000009458267)=	0.000000000000
    {0x3c0ac427, 0x40a23c7a, 0         , 0xbeae324f}, 	// 22: hypot(0.008469617926)=	5.069882170794
    {0x42a17b4b, 0x3e55c485, 0         , 0xbeb6a81c}, 	// 23: hypot(80.740806579590)=	0.208757467583
    {0x3bad9707, 0x3f9ce0f5, 0         , 0xbd1f02f6}, 	// 24: hypot(0.005297544878)=	1.225615134065
    {0x40c09882, 0x0000b0b0, 0         , 0xc24e46dd}, 	// 25: hypot(6.018616676331)=	0.000000000000
    {0x3d4eac7e, 0x00000000, 0         , 0x42a32d48}, 	// 26: hypot(0.050457470119)=	0.000000000000  underflow
    {0x43419a1e, 0x3faac77c, 0         , 0x3d60493a}, 	// 27: hypot(193.602020263672)=	1.334212757754
    {0x410734a3, 0x3f91f6c9, 0         , 0x3d7c0cc1}, 	// 28: hypot(8.450350761414)=	1.140343768193
    {0x43ce9802, 0x1a176bc0, 0         , 0xc109a211}, 	// 29: hypot(413.187561035156)=	0.000000000000
    {0x3cfb9dfb, 0x00000000, 0         , 0x42653136}, 	// 30: hypot(0.030714979395)=	0.000000000000 underflow
    {0x3f04ced7, 0x4f4421da, 0         , 0xc205917b}, 	// 31: hypot(0.518781125546)=	3290552801.441031500000
    {0x3e61bc94, 0x42513cbb, 0         , 0xc0277cf1}, 	// 32: hypot(0.220445930958)=	52.309308129131
    {0x40a8fec9, 0x3f918b79, 0         , 0x3d9e158b}, 	// 33: hypot(5.281101703644)=	1.137068857448
    {0x3d6fc950, 0x3f75a4e7, 0         , 0x3c6e63a9}, 	// 34: hypot(0.058541595936)=	0.959547440920
    {0x3dac26c5, 0x3c62ea8c, 0         , 0x3fdd361c}, 	// 35: hypot(0.084058322012)=	0.013849865855
    {0x4034104f, 0x00000000, 0         , 0xc416dec2}, 	// 36: hypot(2.813495397568)=	0.000000000000   underflow
    {0x44bfdd2a, 0x187f7ab2, 0         , 0xc0ebd680}, 	// 37: hypot(1534.911376953125)=	0.000000000000
    {0x42247d32, 0x7f800000, 0         , 0x42bf64ed}, 	// 38: hypot(41.122261047363)=	29008152148692444000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000     overflow
    {0x3f7b35c5, 0x3f7021fe, 0         , 0x4058ceaa}, 	// 39: hypot(0.981289207935)=	0.938018666021 
    {0x40d7fdde, 0x40c17463, 0         , 0x3f7139f1}, 	// 40: hypot(6.749739646912)=	6.045457476023
    {0x3f68be88, 0x3f81124b, 0         , 0xbdb341d7}, 	// 41: hypot(0.909157276154)=	1.008370752931
    {0x3b4bf41c, 0x49aa65b6, 0         , 0xc01cdf49}, 	// 42: hypot(0.003112084232)=	1395894.705461315600
    {0x43807bf9, 0x2df109a8, 0         , 0xc08c4085}, 	// 43: hypot(256.968536376953)=	0.000000000027
    {0x4033a031, 0x00000000, 0         , 0xc38a26ed}, 	// 44: hypot(2.806652307510)=	0.000000000000 underflow
    {0x40b7cb81, 0x3e99daf7, 0         , 0xbf3012e1}, 	// 45: hypot(5.743591785431)=	0.300498702505
    {0x3dc29058, 0x40c602a5, 0         , 0xbf463843}, 	// 46: hypot(0.095001876354)=	6.187822780802
    {0x410e3035, 0x7f800000, 0         , 0x42af9758}, 	// 47: hypot(8.886769294739)=	197484976680567650000000000000000000000000000000000000000000000000000000000000000000.000000000000  overflow
    {0x3f88694a, 0x424e1778, 0         , 0x4277c104}, 	// 48: hypot(1.065713167191)=	51.522918486233 
    {0x4424171f, 0x3f849e08, 0         , 0x3bb30381}, 	// 49: hypot(656.361267089844)=	1.036072705507
    {0x3e113338, 0x3f6a4c97, 0         , 0x3d39bd8f}, 	// 50: hypot(0.141796946526)=	0.915231193778
    {0x3c23c078, 0x423df08c, 0         , 0xbf5692f4}, 	// 51: hypot(0.009994618595)=	47.484907729096
    {0x3f015bcc, 0x69501d9d, 0         , 0xc2a9fde6}, 	// 52: hypot(0.505306959152)=	15724775596758904000000000.000000000000
    {0x4114db99, 0x3f83c10d, 0         , 0x3c545824}, 	// 53: hypot(9.303612709045)=	1.029328927101
    {0x446b1921, 0x4042132a, 0         , 0x3e25ed4f}, 	// 54: hypot(940.392639160156)=	3.032419609876
    {0x449b0b8b, 0x3f2da881, 0         , 0xbd5f292d}, 	// 55: hypot(1240.360717773438)=	0.678352415777
    {0x3cf6ea1e, 0x40faa1ee, 0         , 0xbf16774b}, 	// 56: hypot(0.030140932649)=	7.832266723979
    {0x419ab0eb, 0x401f53c1, 0         , 0x3e9da8a0}, 	// 57: hypot(19.336385726929)=	2.489486964949
    {0x3bc46c09, 0x48a63365, 0         , 0xc01f515e}, 	// 58: hypot(0.005994324107)=	340379.155057530210
    {0x3d4b6aaa, 0x2c5620df, 0         , 0x410d4fe9}, 	// 59: hypot(0.049662269652)=	0.000000000003
    {0x3d7d24c1, 0x4058831d, 0         , 0xbee027e2}, 	// 60: hypot(0.061802629381)=	3.383002609411
    {0x3d711404, 0x448ec858, 0         , 0xc01f13bd}, 	// 61: hypot(0.058856979012)=	1142.260702213961
    {0x42bcca71, 0x7f800000, 0         , 0x427c34d6}, 	// 62: hypot(94.395393371582)=	33404359717217514000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000  overflow
    {0x40215c64, 0x3dc7a5a0, 0         , 0xc0211e70}, 	// 63: hypot(2.521264076233)=	0.097483870539
    {0x40963357, 0x35e7e9eb, 0         , 0xc1094cbc}, 	// 64: hypot(4.693767070770)=	0.000001727892
    {0x3aa2d85a, 0x0ce0eab5, 0         , 0x4127b98f}, 	// 65: hypot(0.001242409693)=	0.000000000000
    {0x42636cd9, 0x00000000, 0         , 0xc1d06e53}, 	// 66: hypot(56.856296539307)=	0.000000000000  underflow
    {0x3b124fd7, 0x7f800000, 0         , 0xc1b66826}, 	// 67: hypot(0.002232542029)=	2815992780004244000000000000000000000000000000000000000000000.000000000000 overflow
    {0x400b3419, 0x3f760016, 0         , 0xbd520745}, 	// 68: hypot(2.175054788589)=	0.960938782415
    {0x3ce26eb0, 0x57305837, 0         , 0xc112af49}, 	// 69: hypot(0.027640670538)=	193892934407110.620000000000
    {0x41117a88, 0x00000000, 0         , 0xc35a17d5}, 	// 70: hypot(9.092414855957)=	0.000000000000 underflow
    {0x3f10c346, 0x00000000, 0         , 0x442e9663}, 	// 71: hypot(0.565479636192)=	0.000000000000 underflow
    {0x3bd59555, 0x5784bf17, 0         , 0xc0d3c337}, 	// 72: hypot(0.006518045906)=	291912532165666.000000000000
    {0x3c9062f1, 0x40a7d64d, 0         , 0xbed21c66}, 	// 73: hypot(0.017625303939)=	5.244909828768
    {0x42c4bf09, 0x492942c8, 0         , 0x403b93d3}, 	// 74: hypot(98.373115539551)=	693292.470398477980
    {0x409c7202, 0x437a8393, 0         , 0x405ec129}, 	// 75: hypot(4.888916969299)=	250.513960989377
    {0x45ebed46, 0x7f800000, 0         , 0x427be2f2}, 	// 76: hypot(7549.659179687500)=	15826744932255766000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000  // overflow
    {0x420f0e77, 0x46996eae, 0         , 0x4030df06}, 	// 77: hypot(35.764125823975)=	19639.339953557399
    {0x40e5fc0d, 0x4046bdbc, 0         , 0x3f131402}, 	// 78: hypot(7.187017917633)=	3.105330577302
    {0x3e370cd1, 0x44e44869, 0         , 0xc08b951e}, 	// 79: hypot(0.178759828210)=	1826.262765990879
    {0x4208ca99, 0x41cf7b09, 0         , 0x3f6bf491}, 	// 80: hypot(34.197849273682)=	25.935075245414
    {0x40dc980f, 0x4a91fd5a, 0         , 0x40fef09a}, 	// 81: hypot(6.893561840057)=	4783789.113292915700
    {0x42b5d7dd, 0x3884a7f4, 0         , 0xc0093349}, 	// 82: hypot(90.921607971191)=	0.000063255342
    {0x3e7d668a, 0x00000000, 0         , 0x42c60a92}, 	// 83: hypot(0.247461467981)=	0.000000000000 underflow
    {0x43b5a648, 0x3f9a6a6a, 0         , 0x3d025b58}, 	// 84: hypot(363.299072265625)=	1.206372471301
    {0x40b07161, 0x3f86170a, 0         , 0x3cdf07d1}, 	// 85: hypot(5.513840198517)=	1.047578053714
    {0x4085cd2b, 0x395f7d18, 0         , 0xc0bd16c7}, 	// 86: hypot(4.181294918060)=	0.000213135390
    {0x3af0315b, 0x3e9f9a4e, 0         , 0x3e3d6679}, 	// 87: hypot(0.001832525595)=	0.311724123615
    {0x4088d52e, 0x5691c1e7, 0         , 0x41b043e5}, 	// 88: hypot(4.276022911072)=	80130992213125.000000000000

};

#endif	/*__TEST_HYPOT_DATA_H__*/