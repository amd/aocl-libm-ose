//R.obtained=FN_PROTOTYPE(tanhf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_tanhf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_tanhf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
   {0x00000001, 0x00000001,  EXC_CHK_UNFL},  // denormal min
   {0x0005fde6, 0x0005fde6,  EXC_CHK_UNFL},  // denormal intermediate
   {0x007fffff, 0x007fffff,  EXC_CHK_UNFL},  // denormal max
   {0x80000001, 0x80000001,  EXC_CHK_UNFL},  // -denormal min
   {0x805def12, 0x805def12,  EXC_CHK_UNFL},  // -denormal intermediate
   {0x807FFFFF, 0x807fffff,  EXC_CHK_UNFL},  // -denormal max
   {0x00800000, 0x00800000,  EXC_CHK_UNFL},  // normal min
   {0x43b3c4ea, 0x3f800000,  EXC_CHK_NONE},  // normal intermediate
   {0x7f7fffff, 0x3f800000,  EXC_CHK_NONE},  // normal max
   {0x80800000, 0x80800000,  EXC_CHK_UNFL},  // -normal min
   {0xc5812e71, 0xbf800000,  EXC_CHK_NONE},  // -normal intermediate
   {0xFF7FFFFF, 0xbf800000,  EXC_CHK_NONE},  // -normal max															
   {0x7F800000, 0x3f800000,  EXC_CHK_NONE},  // inf
   {0xfF800000, 0xbf800000,  EXC_CHK_NONE},  // -inf
   {0x7Fc00000, 0x7fc00000,  EXC_CHK_NONE},  // qnan min
   {0x7Fe1a570, 0x7fe1a570,  EXC_CHK_NONE},  // qnan intermediate
   {0x7FFFFFFF, 0x7fffffff,  EXC_CHK_NONE},  // qnan max
   {0xfFc00000, 0xffc00000,  EXC_CHK_NONE},  // indeterninate
   {0xFFC00001, 0xffc00001,  EXC_CHK_NONE},  // -qnan min
   {0xFFd2ba31, 0xffd2ba31,  EXC_CHK_NONE},  // -qnan intermediate
   {0xFFFFFFFF, 0xffffffff,  EXC_CHK_NONE},  // -qnan max															
   {0x7F800001, 0x7fc00001,  EXC_CHK_INVL},  // snan min
   {0x7Fa0bd90, 0x7fe0bd90,  EXC_CHK_INVL},  // snan intermediate
   {0x7FBFFFFF, 0x7fffffff,  EXC_CHK_INVL},  // snan max
   {0xFF800001, 0xffc00001,  EXC_CHK_INVL},  // -snan min
   {0xfF95fffa, 0xffd5fffa,  EXC_CHK_INVL},  // -snan intermediate
   {0xFFBFFFFF, 0xffffffff,  EXC_CHK_INVL},  // -snan max															
   {0x3FC90FDB, 0x3f6aca7f,  EXC_CHK_NONE},  // pi/2
   {0x40490FDB, 0x3f7f0bb0,  EXC_CHK_NONE},  // pi
   {0x40C90FDB, 0x3f7fff8b,  EXC_CHK_NONE},  // 2pi
   {0x402DF853, 0x3f7dc7bb,  EXC_CHK_NONE},  // e --
   {0x402DF854, 0x3f7dc7bb,  EXC_CHK_NONE},  // e
   {0x402DF855, 0x3f7dc7bb,  EXC_CHK_NONE},  // e ++															
   {0x37C0F01F, 0x37c0f01f,  EXC_CHK_UNFL},  // 0.000023
   {0x3EFFFEB0, 0x3eec9996,  EXC_CHK_NONE},  // 0.49999
   {0x3F0000C9, 0x3eec9bdb,  EXC_CHK_NONE},  // 0.500012
   {0xb7C0F01F, 0xb7c0f01f,  EXC_CHK_UNFL},  // -0.000023
   {0xbEFFFEB0, 0xbeec9996,  EXC_CHK_NONE},  // -0.49999
   {0xbF0000C9, 0xbeec9bdb,  EXC_CHK_NONE},  // -0.500012
   {0x3f800000, 0x3f42f7d6,  EXC_CHK_NONE},  // 1
   {0x3f700001, 0x3f3bec1d,  EXC_CHK_NONE},  // 0.93750006
   {0x3F87FFFE, 0x3f495fd8,  EXC_CHK_NONE},  // 1.0624998
   {0x3FBFFFAC, 0x3f67b7ad,  EXC_CHK_NONE},  // 1.49999
   {0x3FC00064, 0x3f67b7f0,  EXC_CHK_NONE},  // 1.500012
   {0xbf800000, 0xbf42f7d6,  EXC_CHK_NONE},  // -1
   {0xbf700001, 0xbf3bec1d,  EXC_CHK_NONE},  // -0.93750006
   {0xbF87FFFE, 0xbf495fd8,  EXC_CHK_NONE},  // -1.0624998
   {0xbFBFFFAC, 0xbf67b7ad,  EXC_CHK_NONE},  // -1.49999
   {0xbFC00064, 0xbf67b7f0,  EXC_CHK_NONE},  // -1.500012															
   {0x40000000, 0x3f76ca83,  EXC_CHK_NONE},  // 2
   {0xc0000000, 0xbf76ca83,  EXC_CHK_NONE},  // -2
   {0x41200000, 0x3f800000,  EXC_CHK_NONE},  // 10
   {0xc1200000, 0xbf800000,  EXC_CHK_NONE},  // -10
   {0x447A0000, 0x3f800000,  EXC_CHK_NONE},  // 1000
   {0xc47A0000, 0xbf800000,  EXC_CHK_NONE},  // -1000
   {0x4286CCCC, 0x3f800000,  EXC_CHK_NONE},  // 67.4
   {0xc286CCCC, 0xbf800000,  EXC_CHK_NONE},  // -67.4
   {0x44F7F333, 0x3f800000,  EXC_CHK_NONE},  // 1983.6
   {0xc4F7F333, 0xbf800000,  EXC_CHK_NONE},  // -1983.6
   {0x42AF0000, 0x3f800000,  EXC_CHK_NONE},  // 87.5
   {0xc2AF0000, 0xbf800000,  EXC_CHK_NONE},  // -87.5
   {0x48015E40, 0x3f800000,  EXC_CHK_NONE},  // 132473 
   {0xc8015E40, 0xbf800000,  EXC_CHK_NONE},  // -132473
   {0x4B000000, 0x3f800000,  EXC_CHK_NONE},  // 2^23
   {0x4B000001, 0x3f800000,  EXC_CHK_NONE},  // 2^23 + 1 
   {0x4AFFFFFF, 0x3f800000,  EXC_CHK_NONE},  // 2^23 -1 + 0.5
   {0xcB000000, 0xbf800000,  EXC_CHK_NONE},  // -2^23
   {0xcB000001, 0xbf800000,  EXC_CHK_NONE},  // -(2^23 + 1) 
   {0xcAFFFFFF, 0xbf800000,  EXC_CHK_NONE},  // -(2^23 -1 + 0.5)
   
   // special accuracy tests
   {0x38800000, 0x38800000,  EXC_CHK_UNFL},  //min= 0.00006103515625, small enough that cosh=1 or sinh(x) = 0 
   {0x387FFFFF, 0x387fffff,  EXC_CHK_UNFL}, //min - 1 bit
   {0x38800001, 0x38800001,  EXC_CHK_UNFL}, //min + 1 bit
   {0xF149F2C9, 0xbf800000,  EXC_CHK_NONE}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0xbf800000,  EXC_CHK_NONE}, //lambda + x < 1
   {0xF149F2CA, 0xbf800000,  EXC_CHK_NONE}, //lambda + x > 1
   {0x42B2D4FC, 0x3f800000,  EXC_CHK_NONE}, //max arg, x = 89.41598629223294,max coshf arg
   {0x42B2D4FB, 0x3f800000,  EXC_CHK_NONE}, //max arg - 1 bit
   {0x42B2D4FD, 0x3f800000,  EXC_CHK_NONE}, //max arg + 1 bit
   {0x42B2D4FF, 0x3f800000,  EXC_CHK_NONE}, // > max 
   {0x42B2D400, 0x3f800000,  EXC_CHK_NONE}, // < max
   {0x41A00000, 0x3f800000,  EXC_CHK_NONE}, //small_threshold = 20
   {0x41A80000, 0x3f800000,  EXC_CHK_NONE}, //small_threshold+1 = 21
   {0x41980000, 0x3f800000,  EXC_CHK_NONE}, //small_threshold - 1 = 19
   {0x39000000, 0x39000000,  EXC_CHK_NONE},
   {0x39000001, 0x39000001,  EXC_CHK_NONE},
   {0x38ffffff, 0x38ffffff,  EXC_CHK_UNFL},
   {0x4238aa3b, 0x3f800000,  EXC_CHK_NONE},
   {0x3cb17000, 0x3cb168e6,  EXC_CHK_NONE},
   {0x3585fdf4, 0x3585fdf4,  EXC_CHK_UNFL},
   {0x41200000, 0x3f800000,  EXC_CHK_NONE},
   {0x4120000f, 0x3f800000,  EXC_CHK_NONE},
   {0x411fffff, 0x3f800000,  EXC_CHK_NONE},
   {0x3F29465E, 0x3f14453b,  EXC_CHK_NONE}, // 0< x < 0.9
   {0x3F7C9A56, 0x3f4186e4,  EXC_CHK_NONE}, //0.9 < x < 1.0

    //tanh special exception checks
   {F32_POS_ZERO, 0x00000000,EXC_CHK_NONE },  //0	
   {F32_NEG_ZERO, 0x80000000,EXC_CHK_NONE },  //0
   {F32_POS_INF,  0x3f800000,EXC_CHK_NONE },  
   {F32_NEG_INF,  0xbf800000,EXC_CHK_NONE },  
   {F32_POS_SNAN, F32_POS_QNAN,EXC_CHK_INVL },  //
   {F32_NEG_SNAN, F32_NEG_QNAN,EXC_CHK_INVL },  //
   {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
   {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //
   {0x42BE0000,   0x3f800000,EXC_CHK_NONE },  //95

};

