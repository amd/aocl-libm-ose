//R.obtained=FN_PROTOTYPE(asinf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_asinf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_asinf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
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
   {0x43b3c4ea, 0x7fc00000,  EXC_CHK_INVL},  // normal intermediate
   {0x7f7fffff, 0x7fc00000,  EXC_CHK_INVL},  // normal max
   {0x80800000, 0x80800000,  EXC_CHK_UNFL},  // -normal min
   {0xc5812e71, 0x7fc00000,  EXC_CHK_INVL},  // -normal intermediate
   {0xFF7FFFFF, 0x7fc00000,  EXC_CHK_INVL},  // -normal max															
   {0x7F800000, 0x7fc00000,  EXC_CHK_INVL},  // inf
   {0xfF800000, 0x7fc00000,  EXC_CHK_INVL},  // -inf
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
   {0x3FC90FDB, 0x7fc00000,  EXC_CHK_INVL},  // pi/2
   {0x40490FDB, 0x7fc00000,  EXC_CHK_INVL},  // pi
   {0x40C90FDB, 0x7fc00000,  EXC_CHK_INVL},  // 2pi
   {0x402DF853, 0x7fc00000,  EXC_CHK_INVL},  // e --
   {0x402DF854, 0x7fc00000,  EXC_CHK_INVL},  // e
   {0x402DF855, 0x7fc00000,  EXC_CHK_INVL},  // e ++															
   {0x00000000, 0x00000000,  EXC_CHK_NONE},  // 0
   {0x37C0F01F, 0x37c0f01f,  EXC_CHK_UNFL},  // 0.000023
   {0x3EFFFEB0, 0x3f0609d0,  EXC_CHK_NONE},  // 0.49999
   {0x3F0000C9, 0x3f060b7a,  EXC_CHK_NONE},  // 0.500012
   {0x80000000, 0x80000000,  EXC_CHK_NONE},  // -0
   {0xb7C0F01F, 0xb7c0f01f,  EXC_CHK_UNFL},  // -0.000023
   {0xbEFFFEB0, 0xbf0609d0,  EXC_CHK_NONE},  // -0.49999
   {0xbF0000C9, 0xbf060b7a,  EXC_CHK_NONE},  // -0.500012
   {0x3f800000, 0x3fc90fdb,  EXC_CHK_NONE},  // 1
   {0x3f700001, 0x3f9b916b,  EXC_CHK_NONE},  // 0.93750006
   {0x3F87FFFE, 0x7fc00000,  EXC_CHK_INVL},  // 1.0624998
   {0x3FBFFFAC, 0x7fc00000,  EXC_CHK_INVL},  // 1.49999
   {0x3FC00064, 0x7fc00000,  EXC_CHK_INVL},  // 1.500012
   {0xbf800000, 0xbfc90fdb,  EXC_CHK_NONE},  // -1
   {0xbf700001, 0xbf9b916b,  EXC_CHK_NONE},  // -0.93750006
   {0xbF87FFFE, 0x7fc00000,  EXC_CHK_INVL},  // -1.0624998
   {0xbFBFFFAC, 0x7fc00000,  EXC_CHK_INVL},  // -1.49999
   {0xbFC00064, 0x7fc00000,  EXC_CHK_INVL},  // -1.500012															
   {0x40000000, 0x7fc00000,  EXC_CHK_INVL},  // 2
   {0xc0000000, 0x7fc00000,  EXC_CHK_INVL},  // -2
   {0x41200000, 0x7fc00000,  EXC_CHK_INVL},  // 10
   {0xc1200000, 0x7fc00000,  EXC_CHK_INVL},  // -10
   {0x447A0000, 0x7fc00000,  EXC_CHK_INVL},  // 1000
   {0xc47A0000, 0x7fc00000,  EXC_CHK_INVL},  // -1000
   {0x4286CCCC, 0x7fc00000,  EXC_CHK_INVL},  // 67.4
   {0xc286CCCC, 0x7fc00000,  EXC_CHK_INVL},  // -67.4
   {0x44F7F333, 0x7fc00000,  EXC_CHK_INVL},  // 1983.6
   {0xc4F7F333, 0x7fc00000,  EXC_CHK_INVL},  // -1983.6
   {0x42AF0000, 0x7fc00000,  EXC_CHK_INVL},  // 87.5
   {0xc2AF0000, 0x7fc00000,  EXC_CHK_INVL},  // -87.5
   {0x48015E40, 0x7fc00000,  EXC_CHK_INVL},  // 132473 
   {0xc8015E40, 0x7fc00000,  EXC_CHK_INVL},  // -132473
   {0x4B000000, 0x7fc00000,  EXC_CHK_INVL},  // 2^23
   {0x4B000001, 0x7fc00000,  EXC_CHK_INVL},  // 2^23 + 1 
   {0x4AFFFFFF, 0x7fc00000,  EXC_CHK_INVL},  // 2^23 -1 + 0.5
   {0xcB000000, 0x7fc00000,  EXC_CHK_INVL},  // -2^23
   {0xcB000001, 0x7fc00000,  EXC_CHK_INVL},  // -(2^23 + 1) 
   {0xcAFFFFFF, 0x7fc00000,  EXC_CHK_INVL},  // -(2^23 -1 + 0.5)  
   
   // special accuracy tests   
   {0x3f800000, 0x3fc90fdb,  EXC_CHK_NONE},  //  
   {0x3f800001, 0x7fc00000,  EXC_CHK_INVL},	 //
   {0x3f7fffff, 0x3fc9048a,  EXC_CHK_NONE},	 //
   {0xbf800000, 0xbfc90fdb,  EXC_CHK_NONE},	 //
   {0xbf800001, 0x7fc00000,  EXC_CHK_INVL},	 //
   {0xbf7fffff, 0xbfc9048a,  EXC_CHK_NONE},	 //
   {0x39000000, 0x39000000,  EXC_CHK_NONE},	 //
   {0x39000001, 0x39000001,  EXC_CHK_NONE},	 //
   {0x38ffffff, 0x38ffffff,  EXC_CHK_NONE},	 //
   {0xb9000000, 0xb9000000,  EXC_CHK_NONE},	 //
   {0xb9000001, 0xb9000001,  EXC_CHK_NONE},	 //
   {0xb8ffffff, 0xb8ffffff,  EXC_CHK_NONE},	 //
   {0x3f000000, 0x3f060a92,  EXC_CHK_NONE},	 //
   {0x3f000001, 0x3f060a93,  EXC_CHK_NONE},	 //
   {0x3effffff, 0x3f060a91,  EXC_CHK_NONE},	 //
   {0xbf000000, 0xbf060a92,  EXC_CHK_NONE},	 //
   {0xbf000001, 0xbf060a93,  EXC_CHK_NONE},	 //
   {0xbeffffff, 0xbf060a91,  EXC_CHK_NONE},  //
   {0xbf800000, 0xbfc90fdb,  EXC_CHK_NONE},  //
   {0xbf7f259c, 0xbfbe9c00,  EXC_CHK_NONE},	 //
   {0xbf7d70d5, 0xbfb6f292,  EXC_CHK_NONE},	 //
   {0xbf7ae1ab, 0xbfaf6c42,  EXC_CHK_NONE},	 //
   {0xbf77781e, 0xbfa7ecc9,  EXC_CHK_NONE},	 //
   {0xbf73342e, 0xbfa06b9a,  EXC_CHK_NONE},	 //
   {0xbf6e15dc, 0xbf98e466,  EXC_CHK_NONE},	 //
   {0xbf681d27, 0xbf915428,  EXC_CHK_NONE},	 //
   {0xbf614a0f, 0xbf89b85b,  EXC_CHK_NONE},	 //
   {0xbf599c94, 0xbf820ea2,  EXC_CHK_NONE},	 //
   {0xbf5114b6, 0xbf74a941,  EXC_CHK_NONE},	 //
   {0xbf47b275, 0xbf650fd0,  EXC_CHK_NONE},	 //
   {0xbf3d75d1, 0xbf554bcc,  EXC_CHK_NONE},	 //
   {0xbf325eca, 0xbf4557ac,  EXC_CHK_NONE},	 //
   {0xbf266d60, 0xbf352d6f,  EXC_CHK_NONE},	 //
   {0xbf19a193, 0xbf24c675,  EXC_CHK_NONE},	 //
   {0xbf0bfb63, 0xbf141b64,  EXC_CHK_NONE},	 //
   {0xbefaf5a0, 0xbf0323fc,  EXC_CHK_NONE},  //
   {0xbedc3fb5, 0xbee3adc1,  EXC_CHK_NONE},  //
   {0xbebbd504, 0xbec052b3,  EXC_CHK_NONE},	 //
   {0xbe99b58d, 0xbe9c1dfa,  EXC_CHK_NONE},	 //
   {0xbe6bc2a0, 0xbe6de4f9,  EXC_CHK_NONE},	 //
   {0xbe20b09b, 0xbe215b56,  EXC_CHK_NONE},	 //
   {0xbda46a15, 0xbda4976d,  EXC_CHK_NONE},	 //
   {0xb99fdd00, 0xb99fdd00,  EXC_CHK_NONE},	 //
   {0x3da9fd72, 0x3daa2f92,  EXC_CHK_NONE},	 //
   {0x3e2db6ec, 0x3e2e8f09,  EXC_CHK_NONE},	 //
   {0x3e84ec55, 0x3e8676ab,  EXC_CHK_NONE},	 //
   {0x3eb4b1fa, 0x3eb8ac6c,  EXC_CHK_NONE},	 //
   {0x3ee62c65, 0x3eeebaec,  EXC_CHK_NONE},	 //
   {0x3f0cadcb, 0x3f14f0b4,  EXC_CHK_NONE},	 //
   {0x3f271fc6, 0x3f361894,  EXC_CHK_NONE},	 //
   {0x3f426c24, 0x3f5ccbed,  EXC_CHK_NONE},	 //
   {0x3f5e92e5, 0x3f86eaf5,  EXC_CHK_NONE},	 //
   {0x3f7b9409, 0x3fb13cb7,  EXC_CHK_NONE},	 //

   //asin special exception checks
  {F32_POS_ZERO, F32_POS_ZERO, EXC_CHK_NONE },  //	
  {F32_NEG_ZERO, F32_NEG_ZERO, EXC_CHK_NONE },  //	
  {F32_POS_HNORMAL,F32_POS_QNAN,EXC_CHK_INVL},
  {F32_NEG_HNORMAL,F32_POS_QNAN,EXC_CHK_INVL},
  {F32_POS_SNAN, F32_POS_QNAN,EXC_CHK_INVL },  //
  {F32_NEG_SNAN, F32_NEG_QNAN,EXC_CHK_INVL },  //
  {F32_POS_INF, 0x7fc00000,EXC_CHK_INVL },  //
  {F32_NEG_INF, 0x7fc00000,EXC_CHK_INVL },  //
  {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
  {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //
  {0x40000000, F32_POS_QNAN,EXC_CHK_INVL},  // input 2
  {F32_POS_LDENORM,F32_POS_LDENORM,EXC_CHK_UNFL},
  {F32_NEG_LDENORM,F32_NEG_LDENORM,EXC_CHK_UNFL},
  {F32_POS_HDENORM,F32_POS_HDENORM,EXC_CHK_UNFL},
  {F32_NEG_HDENORM,F32_NEG_HDENORM,EXC_CHK_UNFL},


};

