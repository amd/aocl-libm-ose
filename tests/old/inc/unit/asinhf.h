//R.obtained=FN_PROTOTYPE(asinhf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = asinhf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_asinhf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
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
   {0x43b3c4ea, 0x40d27ebb,  EXC_CHK_NONE},  // normal intermediate
   {0x7f7fffff, 0x42b2d4fc,  EXC_CHK_NONE},  // normal max
   {0x80800000, 0x80800000,  EXC_CHK_UNFL},  // -normal min
   {0xc5812e71, 0xc1105255,  EXC_CHK_NONE},  // -normal intermediate
   {0xFF7FFFFF, 0xc2b2d4fc,  EXC_CHK_NONE},  // -normal max															
   {0x7F800000, 0x7f800000,  EXC_CHK_NONE},  // inf
   {0xfF800000, 0xff800000,  EXC_CHK_NONE},  // -inf
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
   {0x3FC90FDB, 0x3f9de027,  EXC_CHK_NONE},  // pi/2
   {0x40490FDB, 0x3fee5fb5,  EXC_CHK_NONE},  // pi
   {0x40C90FDB, 0x40226315,  EXC_CHK_NONE},  // 2pi
   {0x402DF853, 0x3fdcd955,  EXC_CHK_NONE},  // e --
   {0x402DF854, 0x3fdcd956,  EXC_CHK_NONE},  // e
   {0x402DF855, 0x3fdcd956,  EXC_CHK_NONE},  // e ++															
   {0x00000000, 0x00000000,  EXC_CHK_NONE},  // 0
   {0x37C0F01F, 0x37c0f01f,  EXC_CHK_UNFL},  // 0.000023
   {0x3EFFFEB0, 0x3ef66039,  EXC_CHK_NONE},  // 0.49999
   {0x3F0000C9, 0x3ef662cd,  EXC_CHK_NONE},  // 0.500012
   {0x80000000, 0x80000000,  EXC_CHK_NONE},  // -0
   {0xb7C0F01F, 0xb7c0f01f,  EXC_CHK_UNFL},  // -0.000023
   {0xbEFFFEB0, 0xbef66039,  EXC_CHK_NONE},  // -0.49999
   {0xbF0000C9, 0xbef662cd,  EXC_CHK_NONE},  // -0.500012
   {0x3f800000, 0x3f61a1b3,  EXC_CHK_NONE},  // 1
   {0x3f700001, 0x3f5623ae,  EXC_CHK_NONE},  // 0.93750006
   {0x3F87FFFE, 0x3f6cc53a,  EXC_CHK_NONE},  // 1.0624998
   {0x3FBFFFAC, 0x3f98edd2,  EXC_CHK_NONE},  // 1.49999
   {0x3FC00064, 0x3f98ee38,  EXC_CHK_NONE},  // 1.500012
   {0xbf800000, 0xbf61a1b3,  EXC_CHK_NONE},  // -1
   {0xbf700001, 0xbf5623ae,  EXC_CHK_NONE},  // -0.93750006
   {0xbF87FFFE, 0xbf6cc53a,  EXC_CHK_NONE},  // -1.0624998
   {0xbFBFFFAC, 0xbf98edd2,  EXC_CHK_NONE},  // -1.49999
   {0xbFC00064, 0xbf98ee38,  EXC_CHK_NONE},  // -1.500012															
   {0x40000000, 0x3fb8c90c,  EXC_CHK_NONE},  // 2
   {0xc0000000, 0xbfb8c90c,  EXC_CHK_NONE},  // -2
   {0x41200000, 0x403fe2e3,  EXC_CHK_NONE},  // 10
   {0xc1200000, 0xc03fe2e3,  EXC_CHK_NONE},  // -10
   {0x447A0000, 0x40f33a98,  EXC_CHK_NONE},  // 1000
   {0xc47A0000, 0xc0f33a98,  EXC_CHK_NONE},  // -1000
   {0x4286CCCC, 0x409cec51,  EXC_CHK_NONE},  // 67.4
   {0xc286CCCC, 0xc09cec51,  EXC_CHK_NONE},  // -67.4
   {0x44F7F333, 0x410492b4,  EXC_CHK_NONE},  // 1983.6
   {0xc4F7F333, 0xc10492b4,  EXC_CHK_NONE},  // -1983.6
   {0x42AF0000, 0x40a54632,  EXC_CHK_NONE},  // 87.5
   {0xc2AF0000, 0xc0a54632,  EXC_CHK_NONE},  // -87.5
   {0x48015E40, 0x4147cbe7,  EXC_CHK_NONE},  // 132473 
   {0xc8015E40, 0xc147cbe7,  EXC_CHK_NONE},  // -132473
   {0x4B000000, 0x41851592,  EXC_CHK_NONE},  // 2^23
   {0x4B000001, 0x41851592,  EXC_CHK_NONE},  // 2^23 + 1 
   {0x4AFFFFFF, 0x41851592,  EXC_CHK_NONE},  // 2^23 -1 + 0.5
   {0xcB000000, 0xc1851592,  EXC_CHK_NONE},  // -2^23
   {0xcB000001, 0xc1851592,  EXC_CHK_NONE},  // -(2^23 + 1) 
   {0xcAFFFFFF, 0xc1851592,  EXC_CHK_NONE},  // -(2^23 -1 + 0.5)

   // special accuracy tests
   {0x38800000, 0x38800000,  EXC_CHK_UNFL}, // 
   {0x387FFFFF, 0x387fffff,  EXC_CHK_UNFL}, // 
   {0x38800001, 0x38800001,  EXC_CHK_UNFL}, // 
   {0xF149F2C9, 0xc28b8a99,  EXC_CHK_NONE}, // 
   {0xF149F2C8, 0xc28b8a99,  EXC_CHK_NONE}, // 
   {0xF149F2CA, 0xc28b8a99,  EXC_CHK_NONE}, // 
   {0x42B2D4FC, 0x40a5f7a1,  EXC_CHK_NONE}, // 
   {0x42B2D4FB, 0x40a5f7a0,  EXC_CHK_NONE}, // 
   {0x42B2D4FD, 0x40a5f7a1,  EXC_CHK_NONE}, // 
   {0x42B2D4FF, 0x40a5f7a1,  EXC_CHK_NONE}, // 
   {0x42B2D400, 0x40a5f773,  EXC_CHK_NONE}, // 
   {0x41A00000, 0x406c20d5,  EXC_CHK_NONE}, // 
   {0x41A80000, 0x406f3f42,  EXC_CHK_NONE}, // 
   {0x41980000, 0x4068d98c,  EXC_CHK_NONE}, // 
   {0x39800000, 0x39800000,  EXC_CHK_NONE}, //
   {0x39800001, 0x39800001,  EXC_CHK_NONE},	//
   {0x397fffff, 0x397fffff,  EXC_CHK_UNFL},	//
   {0x46000000, 0x411b43d5,  EXC_CHK_NONE},	//
   {0x46000001, 0x411b43d5,  EXC_CHK_NONE},	//
   {0x45ffffff, 0x411b43d5,  EXC_CHK_NONE},	//
   {0x3F317217, 0x3f25a4a3,  EXC_CHK_NONE},	//
   {0x00000000, 0x00000000,  EXC_CHK_NONE},	//
   {0x80000000, 0x80000000,  EXC_CHK_NONE},	//
   {0x40800000, 0x40060fc5,  EXC_CHK_NONE},	//
   {0x40800001, 0x40060fc6,  EXC_CHK_NONE},	//
   {0x407fffff, 0x40060fc5,  EXC_CHK_NONE},	//
   {0x40000000, 0x3fb8c90c,  EXC_CHK_NONE},	//
   {0x40000001, 0x3fb8c90d,  EXC_CHK_NONE},	//
   {0x3ffffffb, 0x3fb8c90a,  EXC_CHK_NONE}, //

   //test case reported
   {0xc9f5087f, 0xc173498a, EXC_CHK_NONE},
   {0xca20fa65, 0xc177a7d7, EXC_CHK_NONE},
   {0xca8272da, 0xc17f61a0, EXC_CHK_NONE},
   {0xc935c441, 0xc1636b0f, EXC_CHK_NONE},
   {0xc6daf501, 0xc12ef1e2, EXC_CHK_NONE},
   {0xc7b775d0, 0xc1424ba7, EXC_CHK_NONE},


};

