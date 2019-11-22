//R.obtained=FN_PROTOTYPE(acoshf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_acoshf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_acoshf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
   {0x00000001, 0xffc00000,  EXC_CHK_INVL},  // denormal min
   {0x0005fde6, 0xffc00000,  EXC_CHK_INVL},  // denormal intermediate
   {0x007fffff, 0xffc00000,  EXC_CHK_INVL},  // denormal max
   {0x80000001, 0xffc00000,  EXC_CHK_INVL},  // -denormal min
   {0x805def12, 0xffc00000,  EXC_CHK_INVL},  // -denormal intermediate
   {0x807FFFFF, 0xffc00000,  EXC_CHK_INVL},  // -denormal max
   {0x00800000, 0xffc00000,  EXC_CHK_INVL},  // normal min
   {0x43b3c4ea, 0x40d27eb3,  EXC_CHK_NONE},  // normal intermediate
   {0x7f7fffff, 0x42b2d4fc,  EXC_CHK_NONE},  // normal max
   {0x80800000, 0xffc00000,  EXC_CHK_INVL},  // -normal min
   {0xc5812e71, 0xffc00000,  EXC_CHK_INVL},  // -normal intermediate
   {0xFF7FFFFF, 0xffc00000,  EXC_CHK_INVL},  // -normal max															
   {0x7F800000, 0x7f800000,  EXC_CHK_NONE},  // inf
   {0xfF800000, 0xffc00000,  EXC_CHK_INVL},  // -inf
   {0x7Fc00000, 0x7fc00000,  EXC_CHK_NONE},  // qnan min
   {0x7Fe1a570, 0x7fe1a570,  EXC_CHK_NONE},  // qnan intermediate
   {0x7FFFFFFF, 0x7fffffff,  EXC_CHK_NONE},  // qnan max
   {0xfFc00000, 0xffc00000,  EXC_CHK_NONE},  // indeterninate
   {0xFFC00001, 0xffc00000,  EXC_CHK_NONE},  // -qnan min
   {0xFFd2ba31, 0xffc00000,  EXC_CHK_NONE},  // -qnan intermediate
   {0xFFFFFFFF, 0xffc00000,  EXC_CHK_NONE},  // -qnan max															
   {0x7F800001, 0x7fc00001,  EXC_CHK_INVL},  // snan min
   {0x7Fa0bd90, 0x7fe0bd90,  EXC_CHK_INVL},  // snan intermediate
   {0x7FBFFFFF, 0x7fffffff,  EXC_CHK_INVL},  // snan max
   {0xFF800001, 0xffc00000,  EXC_CHK_INVL},  // -snan min
   {0xfF95fffa, 0xffc00000,  EXC_CHK_INVL},  // -snan intermediate
   {0xFFBFFFFF, 0xffc00000,  EXC_CHK_INVL},  // -snan max															
   {0x3FC90FDB, 0x3f82f91f,  EXC_CHK_NONE},  // pi/2
   {0x40490FDB, 0x3fe7e018,  EXC_CHK_NONE},  // pi
   {0x40C90FDB, 0x4021938d,  EXC_CHK_NONE},  // 2pi
   {0x402DF853, 0x3fd42777,  EXC_CHK_NONE},  // e --
   {0x402DF854, 0x3fd42777,  EXC_CHK_NONE},  // e
   {0x402DF855, 0x3fd42778,  EXC_CHK_NONE},  // e ++															
   {0x00000000, 0xffc00000,  EXC_CHK_INVL},  // 0
   {0x37C0F01F, 0xffc00000,  EXC_CHK_INVL},  // 0.000023
   {0x3EFFFEB0, 0xffc00000,  EXC_CHK_INVL},  // 0.49999
   {0x3F0000C9, 0xffc00000,  EXC_CHK_INVL},  // 0.500012
   {0x80000000, 0xffc00000,  EXC_CHK_INVL},  // -0
   {0xb7C0F01F, 0xffc00000,  EXC_CHK_INVL},  // -0.000023
   {0xbEFFFEB0, 0xffc00000,  EXC_CHK_INVL},  // -0.49999
   {0xbF0000C9, 0xffc00000,  EXC_CHK_INVL},  // -0.500012
   {0x3f800000, 0x00000000,  EXC_CHK_NONE},  // 1
   {0x3f700001, 0xffc00000,  EXC_CHK_INVL},  // 0.93750006
   {0x3F87FFFE, 0x3eb416d6,  EXC_CHK_NONE},  // 1.0624998
   {0x3FBFFFAC, 0x3f7660cf,  EXC_CHK_NONE},  // 1.49999
   {0x3FC00064, 0x3f766218,  EXC_CHK_NONE},  // 1.500012
   {0xbf800000, 0xffc00000,  EXC_CHK_INVL},  // -1
   {0xbf700001, 0xffc00000,  EXC_CHK_INVL},  // -0.93750006
   {0xbF87FFFE, 0xffc00000,  EXC_CHK_INVL},  // -1.0624998
   {0xbFBFFFAC, 0xffc00000,  EXC_CHK_INVL},  // -1.49999
   {0xbFC00064, 0xffc00000,  EXC_CHK_INVL},  // -1.500012															
   {0x40000000, 0x3fa89214,  EXC_CHK_NONE},  // 2
   {0xc0000000, 0xffc00000,  EXC_CHK_INVL},  // -2
   {0x41200000, 0x403f90f7,  EXC_CHK_NONE},  // 10
   {0xc1200000, 0xffc00000,  EXC_CHK_INVL},  // -10
   {0x447A0000, 0x40f33a97,  EXC_CHK_NONE},  // 1000
   {0xc47A0000, 0xffc00000,  EXC_CHK_INVL},  // -1000
   {0x4286CCCC, 0x409ceb6a,  EXC_CHK_NONE},  // 67.4
   {0xc286CCCC, 0xffc00000,  EXC_CHK_INVL},  // -67.4
   {0x44F7F333, 0x410492b4,  EXC_CHK_NONE},  // 1983.6
   {0xc4F7F333, 0xffc00000,  EXC_CHK_INVL},  // -1983.6
   {0x42AF0000, 0x40a545a9,  EXC_CHK_NONE},  // 87.5
   {0xc2AF0000, 0xffc00000,  EXC_CHK_INVL},  // -87.5
   {0x48015E40, 0x4147cbe7,  EXC_CHK_NONE},  // 132473 
   {0xc8015E40, 0xffc00000,  EXC_CHK_INVL},  // -132473
   {0x4B000000, 0x41851592,  EXC_CHK_NONE},  // 2^23
   {0x4B000001, 0x41851592,  EXC_CHK_NONE},  // 2^23 + 1 
   {0x4AFFFFFF, 0x41851592,  EXC_CHK_NONE},  // 2^23 -1 + 0.5
   {0xcB000000, 0xffc00000,  EXC_CHK_INVL},  // -2^23
   {0xcB000001, 0xffc00000,  EXC_CHK_INVL},  // -(2^23 + 1) 
   {0xcAFFFFFF, 0xffc00000,  EXC_CHK_INVL},  // -(2^23 -1 + 0.5)

    // special accuracy tests
   {0x38800000, 0xffc00000,  EXC_CHK_INVL}, // 
   {0x387FFFFF, 0xffc00000,  EXC_CHK_INVL}, // 
   {0x38800001, 0xffc00000,  EXC_CHK_INVL}, // 
   {0x7f7fffec, 0x42b2d4fc,  EXC_CHK_NONE}, // 
   {0x7f7fff6c, 0x42b2d4fb,  EXC_CHK_NONE}, // 
   {0x42B2D4FD, 0x40a5f71e,  EXC_CHK_NONE}, // 
   {0x42B2D4FF, 0x40a5f71e,  EXC_CHK_NONE}, // 
   {0x7f7f820b, 0x42b2d400,  EXC_CHK_NONE}, // 
   {0x4d675844, 0x41a00000,  EXC_CHK_NONE}, // 
   {0x4e1d3710, 0x41a80000,  EXC_CHK_NONE}, // 
   {0x4caa36c8, 0x41980000,  EXC_CHK_NONE}, // 
   {0x46000000, 0x411b43d5,  EXC_CHK_NONE}, //
   {0x46000001, 0x411b43d5,  EXC_CHK_NONE},	//
   {0x45ffffff, 0x411b43d5,  EXC_CHK_NONE},	//
   {0x3F317217, 0xffc00000,  EXC_CHK_INVL},	//
   {0x40000000, 0x3fa89214,  EXC_CHK_NONE},	//
   {0x40000001, 0x3fa89215,  EXC_CHK_NONE},	//
   {0x3fffffff, 0x3fa89213,  EXC_CHK_NONE},	//
   {0x3f800000, 0x00000000,  EXC_CHK_NONE}, //
   {0x40584817, 0x3ff1b07f,  EXC_CHK_NONE}, //
   {0x40d84817, 0x40264d67,  EXC_CHK_NONE}, //
   {0x41323612, 0x40467f16,  EXC_CHK_NONE}, //
   {0x41f6e585, 0x4083eace,  EXC_CHK_NONE}, //
   {0x4283dc5f, 0x409c36cd,  EXC_CHK_NONE}, //
   {0x42e8afbb, 0x40ae63b9,  EXC_CHK_NONE}, //
   {0x43859b33, 0x40c8ff7e,  EXC_CHK_NONE}, //
   {0x4401a272, 0x40de368f,  EXC_CHK_NONE},	//
   {0x44599481, 0x40eec8c6,  EXC_CHK_NONE},	//
   {0x44a551e3, 0x40fc2cd1,  EXC_CHK_NONE},	//
   {0x48490495, 0x414ed924,  EXC_CHK_NONE},	//
   {0x511bd2ce, 0x41c9332f,  EXC_CHK_NONE},	//
   {0x62bdb16c, 0x42466d64,  EXC_CHK_NONE},	// 


};

