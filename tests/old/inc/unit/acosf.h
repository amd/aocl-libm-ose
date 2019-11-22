//R.obtained=FN_PROTOTYPE(acosf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_acosf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_acosf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
   {0x00000001, 0x3fc90fdb,  EXC_CHK_NONE},  // denormal min
   {0x0005fde6, 0x3fc90fdb,  EXC_CHK_NONE},  // denormal intermediate
   {0x007fffff, 0x3fc90fdb,  EXC_CHK_NONE},  // denormal max
   {0x80000001, 0x3fc90fdb,  EXC_CHK_NONE},  // -denormal min
   {0x805def12, 0x3fc90fdb,  EXC_CHK_NONE},  // -denormal intermediate
   {0x807FFFFF, 0x3fc90fdb,  EXC_CHK_NONE},  // -denormal max
   {0x00800000, 0x3fc90fdb,  EXC_CHK_NONE},  // normal min
   {0x43b3c4ea, 0x7fc00000,  EXC_CHK_INVL},  // normal intermediate
   {0x7f7fffff, 0x7fc00000,  EXC_CHK_INVL},  // normal max
   {0x80800000, 0x3fc90fdb,  EXC_CHK_NONE},  // -normal min
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
   {0x00000000, 0x3fc90fdb,  EXC_CHK_NONE},  // 0
   {0x37C0F01F, 0x3fc90f1a,  EXC_CHK_NONE},  // 0.000023
   {0x3EFFFEB0, 0x3f860af3,  EXC_CHK_NONE},  // 0.49999
   {0x3F0000C9, 0x3f860a1e,  EXC_CHK_NONE},  // 0.500012
   {0x80000000, 0x3fc90fdb,  EXC_CHK_NONE},  // -0
   {0xb7C0F01F, 0x3fc9109c,  EXC_CHK_NONE},  // -0.000023
   {0xbEFFFEB0, 0x40060a61,  EXC_CHK_NONE},  // -0.49999
   {0xbF0000C9, 0x40060acc,  EXC_CHK_NONE},  // -0.500012
   {0x3f800000, 0x00000000,  EXC_CHK_NONE},  // 1
   {0x3f700001, 0x3eb5f9bf,  EXC_CHK_NONE},  // 0.93750006
   {0x3F87FFFE, 0x7fc00000,  EXC_CHK_INVL},  // 1.0624998
   {0x3FBFFFAC, 0x7fc00000,  EXC_CHK_INVL},  // 1.49999
   {0x3FC00064, 0x7fc00000,  EXC_CHK_INVL},  // 1.500012
   {0xbf800000, 0x40490fdb,  EXC_CHK_NONE},  // -1
   {0xbf700001, 0x403250a3,  EXC_CHK_NONE},  // -0.93750006
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
   {0x3f800000, 0x00000000,  EXC_CHK_NONE},  //    
   {0x3f800001, 0x7fc00000,  EXC_CHK_INVL},	 //
   {0x3f7fffff, 0x39b504f3,  EXC_CHK_NONE},	 //
   {0xbf800000, 0x40490fdb,  EXC_CHK_NONE},	 //
   {0xbf800001, 0x7fc00000,  EXC_CHK_INVL},	 //
   {0xbf7fffff, 0x40490a32,  EXC_CHK_NONE},	 //
   {0x39000000, 0x3fc90bdb,  EXC_CHK_NONE},	 //
   {0x39000001, 0x3fc90bdb,  EXC_CHK_NONE},	 //
   {0x38ffffff, 0x3fc90bdb,  EXC_CHK_NONE},	 //
   {0xb9000000, 0x3fc913db,  EXC_CHK_NONE},	 //
   {0xb9000001, 0x3fc913db,  EXC_CHK_NONE},	 //
   {0xb8ffffff, 0x3fc913db,  EXC_CHK_NONE},	 //
   {0x3f000000, 0x3f860a92,  EXC_CHK_NONE},	 //
   {0x3f000001, 0x3f860a91,  EXC_CHK_NONE},	 //
   {0x3effffff, 0x3f860a92,  EXC_CHK_NONE},	 //
   {0xbf000000, 0x40060a92,  EXC_CHK_NONE},	 //
   {0xbf000001, 0x40060a92,  EXC_CHK_NONE},	 //
   {0xbeffffff, 0x40060a92,  EXC_CHK_NONE},  //
   {0xbf800000, 0x40490fdb,  EXC_CHK_NONE},  //
   {0xbf7f259c, 0x4043d5ed,  EXC_CHK_NONE},	 //
   {0xbf7d70d5, 0x40400136,  EXC_CHK_NONE},	 //
   {0xbf7ae1ab, 0x403c3e0e,  EXC_CHK_NONE},	 //
   {0xbf77781e, 0x40387e52,  EXC_CHK_NONE},	 //
   {0xbf73342e, 0x4034bdba,  EXC_CHK_NONE},	 //
   {0xbf6e15dc, 0x4030fa20,  EXC_CHK_NONE},	 //
   {0xbf681d27, 0x402d3201,  EXC_CHK_NONE},	 //
   {0xbf614a0f, 0x4029641b,  EXC_CHK_NONE},	 //
   {0xbf599c94, 0x40258f3e,  EXC_CHK_NONE},	 //
   {0xbf5114b6, 0x4021b23e,  EXC_CHK_NONE},	 //
   {0xbf47b275, 0x401dcbe1,  EXC_CHK_NONE},	 //
   {0xbf3d75d1, 0x4019dae0,  EXC_CHK_NONE},	 //
   {0xbf325eca, 0x4015ddd8,  EXC_CHK_NONE},	 //
   {0xbf266d60, 0x4011d349,  EXC_CHK_NONE},	 //
   {0xbf19a193, 0x400db98b,  EXC_CHK_NONE},	 //
   {0xbf0bfb63, 0x40098ec6,  EXC_CHK_NONE},	 //
   {0xbefaf5a0, 0x400550ec,  EXC_CHK_NONE},  //
   {0xbedc3fb5, 0x4000fda5,  EXC_CHK_NONE},  //
   {0xbebbd504, 0x3ff92487,  EXC_CHK_NONE},	 //
   {0xbe99b58d, 0x3ff01759,  EXC_CHK_NONE},	 //
   {0xbe6bc2a0, 0x3fe6cc7a,  EXC_CHK_NONE},	 //
   {0xbe20b09b, 0x3fdd3b45,  EXC_CHK_NONE},	 //
   {0xbda46a15, 0x3fd35951,  EXC_CHK_NONE},	 //
   {0xb99fdd00, 0x3fc919d8,  EXC_CHK_NONE},	 //
   {0x3da9fd72, 0x3fbe6ce2,  EXC_CHK_NONE},	 //
   {0x3e2db6ec, 0x3fb33df9,  EXC_CHK_NONE},	 //
   {0x3e84ec55, 0x3fa77230,  EXC_CHK_NONE},	 //
   {0x3eb4b1fa, 0x3f9ae4c0,  EXC_CHK_NONE},	 //
   {0x3ee62c65, 0x3f8d6120,  EXC_CHK_NONE},	 //
   {0x3f0cadcb, 0x3f7d2f01,  EXC_CHK_NONE},	 //
   {0x3f271fc6, 0x3f5c0721,  EXC_CHK_NONE},	 //
   {0x3f426c24, 0x3f3553c9,  EXC_CHK_NONE},	 //
   {0x3f5e92e5, 0x3f0449cc,  EXC_CHK_NONE},	 //
   {0x3f7b9409, 0x3e3e991d,  EXC_CHK_NONE},	 //

   //acos special exception checks
  {0x3f800000, 0x00000000, EXC_CHK_NONE },  //	
  {F32_POS_HNORMAL,F32_POS_QNAN,EXC_CHK_INVL},
  {F32_NEG_HNORMAL,F32_POS_QNAN,EXC_CHK_INVL},
  {F32_POS_SNAN, F32_POS_QNAN,EXC_CHK_INVL },  //
  {F32_NEG_SNAN, F32_NEG_QNAN,EXC_CHK_INVL },  //
  {F32_POS_INF, 0x7fc00000,EXC_CHK_INVL },  //
  {F32_NEG_INF, 0x7fc00000,EXC_CHK_INVL },  //
  {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
  {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //
  {0x40000000, F32_POS_QNAN,EXC_CHK_INVL},  // input 2

};

