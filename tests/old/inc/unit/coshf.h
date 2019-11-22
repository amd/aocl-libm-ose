//R.obtained=FN_PROTOTYPE(coshf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_coshf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_coshf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
   {0x00000001, 0x3f800000,  EXC_CHK_NONE},  // denormal min
   {0x0005fde6, 0x3f800000,  EXC_CHK_NONE},  // denormal intermediate
   {0x007fffff, 0x3f800000,  EXC_CHK_NONE},  // denormal max
   {0x80000001, 0x3f800000,  EXC_CHK_NONE},  // -denormal min
   {0x805def12, 0x3f800000,  EXC_CHK_NONE},  // -denormal intermediate
   {0x807FFFFF, 0x3f800000,  EXC_CHK_NONE},  // -denormal max
   {0x00800000, 0x3f800000,  EXC_CHK_NONE},  // normal min
   {0x43b3c4ea, 0x7f800000,  EXC_CHK_OVFL},  // normal intermediate
   {0x7f7fffff, 0x7f800000,  EXC_CHK_OVFL},  // normal max
   {0x80800000, 0x3f800000,  EXC_CHK_NONE},  // -normal min
   {0xc5812e71, 0x7f800000,  EXC_CHK_OVFL},  // -normal intermediate
   {0xFF7FFFFF, 0x7f800000,  EXC_CHK_OVFL},  // -normal max															
   {0x7F800000, 0x7f800000,  EXC_CHK_NONE},  // inf
   {0xfF800000, 0x7f800000,  EXC_CHK_NONE},  // -inf
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
   {0x3FC90FDB, 0x40209662,  EXC_CHK_NONE},  // pi/2
   {0x40490FDB, 0x413978a5,  EXC_CHK_NONE},  // pi
   {0x40C90FDB, 0x4385df97,  EXC_CHK_NONE},  // 2pi
   {0x402DF853, 0x40f38620,  EXC_CHK_NONE},  // e --
   {0x402DF854, 0x40f38624,  EXC_CHK_NONE},  // e
   {0x402DF855, 0x40f38628,  EXC_CHK_NONE},  // e ++															
   {0x00000000, 0x3f800000,  EXC_CHK_NONE},  // 0
   {0x37C0F01F, 0x3f800000,  EXC_CHK_NONE},  // 0.000023
   {0x3EFFFEB0, 0x3f9055e0,  EXC_CHK_NONE},  // 0.49999
   {0x3F0000C9, 0x3f905641,  EXC_CHK_NONE},  // 0.500012
   {0x80000000, 0x3f800000,  EXC_CHK_NONE},  // -0
   {0xb7C0F01F, 0x3f800000,  EXC_CHK_NONE},  // -0.000023
   {0xbEFFFEB0, 0x3f9055e0,  EXC_CHK_NONE},  // -0.49999
   {0xbF0000C9, 0x3f905641,  EXC_CHK_NONE},  // -0.500012
   {0x3f800000, 0x3fc583ab,  EXC_CHK_NONE},  // 1
   {0x3f700001, 0x3fbc7e14,  EXC_CHK_NONE},  // 0.93750006
   {0x3F87FFFE, 0x3fcf4ed3,  EXC_CHK_NONE},  // 1.0624998
   {0x3FBFFFAC, 0x40168d88,  EXC_CHK_NONE},  // 1.49999
   {0x3FC00064, 0x40168e4c,  EXC_CHK_NONE},  // 1.500012
   {0xbf800000, 0x3fc583ab,  EXC_CHK_NONE},  // -1
   {0xbf700001, 0x3fbc7e14,  EXC_CHK_NONE},  // -0.93750006
   {0xbF87FFFE, 0x3fcf4ed3,  EXC_CHK_NONE},  // -1.0624998
   {0xbFBFFFAC, 0x40168d88,  EXC_CHK_NONE},  // -1.49999
   {0xbFC00064, 0x40168e4c,  EXC_CHK_NONE},  // -1.500012															
   {0x40000000, 0x4070c7d0,  EXC_CHK_NONE},  // 2
   {0xc0000000, 0x4070c7d0,  EXC_CHK_NONE},  // -2
   {0x41200000, 0x462c14ef,  EXC_CHK_NONE},  // 10
   {0xc1200000, 0x462c14ef,  EXC_CHK_NONE},  // -10
   {0x447A0000, 0x7f800000,  EXC_CHK_OVFL},  // 1000
   {0xc47A0000, 0x7f800000,  EXC_CHK_OVFL},  // -1000
   {0x4286CCCC, 0x6f96eb6f,  EXC_CHK_NONE},  // 67.4
   {0xc286CCCC, 0x6f96eb6f,  EXC_CHK_NONE},  // -67.4
   {0x44F7F333, 0x7f800000,  EXC_CHK_OVFL},  // 1983.6
   {0xc4F7F333, 0x7f800000,  EXC_CHK_OVFL},  // -1983.6
   {0x42AF0000, 0x7e16bab3,  EXC_CHK_NONE},  // 87.5
   {0xc2AF0000, 0x7e16bab3,  EXC_CHK_NONE},  // -87.5
   {0x48015E40, 0x7f800000,  EXC_CHK_OVFL},  // 132473 
   {0xc8015E40, 0x7f800000,  EXC_CHK_OVFL},  // -132473
   {0x4B000000, 0x7f800000,  EXC_CHK_OVFL},  // 2^23
   {0x4B000001, 0x7f800000,  EXC_CHK_OVFL},  // 2^23 + 1 
   {0x4AFFFFFF, 0x7f800000,  EXC_CHK_OVFL},  // 2^23 -1 + 0.5
   {0xcB000000, 0x7f800000,  EXC_CHK_OVFL},  // -2^23
   {0xcB000001, 0x7f800000,  EXC_CHK_OVFL},  // -(2^23 + 1) 
   {0xcAFFFFFF, 0x7f800000,  EXC_CHK_OVFL},  // -(2^23 -1 + 0.5)

   // special accuracy tests
   {0x38800000, 0x3f800000,  EXC_CHK_NONE},  //min= 0.00006103515625, small enough that cosh(x) = 1
   {0x387FFFFF, 0x3f800000,  EXC_CHK_NONE}, //min - 1 bit
   {0x38800001, 0x3f800000,  EXC_CHK_NONE}, //min + 1 bit
   {0xF149F2C9, 0x7f800000,  EXC_CHK_OVFL}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  EXC_CHK_OVFL}, //lambda + x < 1
   {0xF149F2CA, 0x7f800000,  EXC_CHK_OVFL}, //lambda + x > 1
   {0x42B2D4FC, 0x7f7fffec,  EXC_CHK_NONE}, //max arg, x = 89.41598629223294,max coshf arg
   {0x42B2D4FB, 0x7f7fff6c,  EXC_CHK_NONE}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  EXC_CHK_OVFL}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  EXC_CHK_OVFL}, // > max 
   {0x42B2D400, 0x7f7f820b,  EXC_CHK_NONE}, // < max
   {0x41A00000, 0x4d675844,  EXC_CHK_NONE}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  EXC_CHK_NONE}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  EXC_CHK_NONE}, //small_threshold - 1 = 19  

    //cosh special exception checks
   {F32_POS_ZERO, 0x3f800000,EXC_CHK_NONE },  //0	
   {F32_NEG_ZERO, 0x3f800000,EXC_CHK_NONE },  //0
   {F32_POS_INF, F32_POS_INF,EXC_CHK_NONE },  
   {F32_NEG_INF, F32_POS_INF,EXC_CHK_NONE },  
   {F32_POS_SNAN, F32_POS_SNAN,EXC_CHK_INVL },  //
   {F32_NEG_SNAN, F32_NEG_SNAN,EXC_CHK_INVL },  //
   {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
   {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //
   {0x42BE0000, F32_POS_INF,EXC_CHK_OVFL },  //95

};

