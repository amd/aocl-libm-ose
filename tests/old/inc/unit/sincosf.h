//FN_PROTOTYPE(sincosf)(P1.input, &P2.obtained, &P3.obtained)
//float, float,float ,ExcFlags
//P1 P2 P3 E
//unsigned int|float, unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//amd_ref_sincosf(P1.V3[j],&P2.V3[j],&P3.V3[j])
//float float float ExcFlags
//ULP amd_ref_sincosf_ULP(P1.V3[j],&P2.V3[j],&P3.V3[j],&testdata[k].P2.ulp[j], &testdata[k].P2.relative_error[j])
//testdata[j].P2.MaxUlp()

static struct data input[] =
{
   {0xbf000000, 0xbef57744,  0x3f60a940, EXC_CHK_NONE},
   {0xbf000001, 0xbef57745,  0x3f60a940, EXC_CHK_NONE},
   {0xbeffffff, 0xbef57743,  0x3f60a940, EXC_CHK_NONE},
   {0xbf800000, 0xbf576aa4,  0x3f0a5140, EXC_CHK_NONE},
   {0xbf7f259c, 0xbf56f457,  0x3f0b08d3, EXC_CHK_NONE},
   {0xbf7d70d5, 0xbf5605e7,  0x3f0c76c7, EXC_CHK_NONE},
   {0xbf7ae1ab, 0xbf549bb1,  0x3f0e98b5, EXC_CHK_NONE},
   {0xbf77781e, 0xbf52b04a,  0x3f116aed, EXC_CHK_NONE},
   {0xbf73342e, 0xbf503c92,  0x3f14e863, EXC_CHK_NONE},
   {0xbf6e15dc, 0xbf4d37ca,  0x3f190a8b, EXC_CHK_NONE},
   {0xbf681d27, 0xbf4997b1,  0x3f1dc93c, EXC_CHK_NONE},
   {0xbf614a0f, 0xbf4550a9,  0x3f231a81, EXC_CHK_NONE},
   {0xbf599c94, 0xbf4055e1,  0x3f28f26b, EXC_CHK_NONE},
   {0xbf5114b6, 0xbf3a9992,  0x3f2f42de, EXC_CHK_NONE},
   {0xbf47b275, 0xbf340d3b,  0x3f35fb5a, EXC_CHK_NONE},
   {0xbf3d75d1, 0xbf2ca1f4,  0x3f3d08c9, EXC_CHK_NONE}, 
   {0xbf325eca, 0xbf2448c5,  0x3f445546, EXC_CHK_NONE},
   {0xbf266d60, 0xbf1af30c,  0x3f4bc7ed, EXC_CHK_NONE},
   {0xbf19a193, 0xbf1092fe,  0x3f5344b1, EXC_CHK_NONE},
   {0xbf0bfb63, 0xbf051c20,  0x3f5aac38, EXC_CHK_NONE},
   {0xbefaf5a0, 0xbef107d4,  0x3f61dbc7, EXC_CHK_NONE},
   {0xbedc3fb5, 0xbed584c4,  0x3f68ad3d, EXC_CHK_NONE},
   {0xbebbd504, 0xbeb7a5a8,  0x3f6ef721, EXC_CHK_NONE},
   {0xbe99b58d, 0xbe976920,  0x3f748ccd, EXC_CHK_NONE},
   {0xbe6bc2a0, 0xbe69aed3,  0x3f793eb2, EXC_CHK_NONE},
   {0xbe20b09b, 0xbe2007fb,  0x3f7cdabe, EXC_CHK_NONE},
   {0xbda46a15, 0xbda43ce3,  0x3f7f2ced, EXC_CHK_NONE},
   {0xb99fdd00, 0xb99fdd00,  0x3f7fffff, EXC_CHK_NONE},
   {0x3da9fd72, 0x3da9cb7e,  0x3f7f1e60, EXC_CHK_NONE},
   {0x3e2db6ec, 0x3e2ce1ed,  0x3f7c533c, EXC_CHK_NONE},
   {0x3e84ec55, 0x3e836f5d,  0x3f776bce, EXC_CHK_NONE},
   {0x3eb4b1fa, 0x3eb0f7ae,  0x3f7038d7, EXC_CHK_NONE},
   {0x3ee62c65, 0x3ede7f91,  0x3f669050, EXC_CHK_NONE},
   {0x3f0cadcb, 0x3f05b464,  0x3f5a4f3f, EXC_CHK_NONE},
   {0x3f271fc6, 0x3f1b80e9,  0x3f4b5bc1, EXC_CHK_NONE},
   {0x3f426c24, 0x3f30439f,  0x3f39a71c, EXC_CHK_NONE},
   {0x3f5e92e5, 0x3f4392ec,  0x3f252ff3, EXC_CHK_NONE},
   {0x3f7b9409, 0x3f54fed8,  0x3f0e0470, EXC_CHK_NONE},
   {0x3f800000, 0x3f576aa4,  0x3f0a5140, EXC_CHK_NONE},
   {0x40584817, 0xbe7139db,  0xbf78cb9c, EXC_CHK_NONE},
   {0x40d84817, 0x3eea6fe8,  0x3f639641, EXC_CHK_NONE},
   {0x41323612, 0xbf7d668d,  0x3e118e0a, EXC_CHK_NONE},
   {0x41f6e585, 0xbf06a60a,  0x3f59ba88, EXC_CHK_NONE},
   {0x4283dc5f, 0x3d3035e6,  0xbf7fc354, EXC_CHK_NONE},
   {0x42e8afbb, 0xbdd535be,  0xbf7e9be3, EXC_CHK_NONE},
   {0x43859b33, 0xbe346c21,  0xbf7bfeb9, EXC_CHK_NONE},
   {0x4401a272, 0xbe32b5dc,  0xbf7c123d, EXC_CHK_NONE},
   {0x44599481, 0xbdcad88a,  0xbf7ebdc0, EXC_CHK_NONE},
   {0x44a551e3, 0x3d530b39,  0xbf7fa8f3, EXC_CHK_NONE},
   {0x48490495, 0xbf64cf5a,  0x3ee5a034, EXC_CHK_NONE},
   {0x511bd2ce, 0xbe2384ad,  0xbf7cb709, EXC_CHK_NONE},
   {0x62bdb16c, 0x3f7ee8c7,  0xbdbcd9c6, EXC_CHK_NONE},
   {0xbf800000, 0xbf576aa4,  0x3f0a5140, EXC_CHK_NONE},
   {0xc0584817, 0x3e7139db,  0xbf78cb9c, EXC_CHK_NONE},
   {0xc0d84817, 0xbeea6fe8,  0x3f639641, EXC_CHK_NONE},
   {0xc1323612, 0x3f7d668d,  0x3e118e0a, EXC_CHK_NONE},
   {0xc1f6e585, 0x3f06a60a,  0x3f59ba88, EXC_CHK_NONE},
   {0xc283dc5f, 0xbd3035e6,  0xbf7fc354, EXC_CHK_NONE},
   {0xc2e8afbb, 0x3dd535be,  0xbf7e9be3, EXC_CHK_NONE},
   {0xc3859b33, 0x3e346c21,  0xbf7bfeb9, EXC_CHK_NONE},
   {0xc401a272, 0x3e32b5dc,  0xbf7c123d, EXC_CHK_NONE},
   {0xc4599481, 0x3dcad88a,  0xbf7ebdc0, EXC_CHK_NONE},
   {0xc4a551e3, 0xbd530b39,  0xbf7fa8f3, EXC_CHK_NONE},
   {0xc8490495, 0x3f64cf5a,  0x3ee5a034, EXC_CHK_NONE},
   {0xd11bd2ce, 0x3e2384ad,  0xbf7cb709, EXC_CHK_NONE},
   {0xe2bdb16c, 0xbf7ee8c7,  0xbdbcd9c6, EXC_CHK_NONE},

	  //special cases for exceptions
      { F32_NEG_ZERO, F32_NEG_ZERO  , F32_POS_ONE    , EXC_CHK_NONE},
      { F32_POS_ZERO, F32_POS_ZERO  , F32_POS_ONE    , EXC_CHK_NONE},
      { F32_POS_ONE , 0x3f576aa4    , 0x3f0a5140     , EXC_CHK_NONE},
      { F32_NEG_ONE , 0xbf576aa4    , 0x3f0a5140     , EXC_CHK_NONE},
      { F32_POS_SNAN, F32_POS_SNAN_Q, F32_POS_SNAN_Q , EXC_CHK_INVL},
      { F32_NEG_SNAN, F32_NEG_SNAN_Q, F32_NEG_SNAN_Q , EXC_CHK_INVL},
      { F32_POS_QNAN, F32_POS_QNAN  , F32_POS_QNAN   , EXC_CHK_NONE},
      { F32_NEG_QNAN, F32_NEG_QNAN  , F32_NEG_QNAN   , EXC_CHK_NONE},
      { F32_POS_HDENORM, F32_POS_HDENORM, F32_POS_ONE , EXC_CHK_UNFL},
      { F32_NEG_HDENORM, F32_NEG_HDENORM, F32_POS_ONE , EXC_CHK_UNFL},
      { F32_POS_LDENORM, F32_POS_LDENORM, F32_POS_ONE , EXC_CHK_UNFL},
      { F32_NEG_LDENORM, F32_NEG_LDENORM, F32_POS_ONE , EXC_CHK_UNFL},
      { F32_POS_HNORMAL, 0xbf0599b3     , 0x3f5a5f96  , EXC_CHK_NONE},//according to NAG answer
      { F32_NEG_HNORMAL, 0x3f0599b3     , 0x3f5a5f96  , EXC_CHK_NONE},
      { F32_POS_LNORMAL, F32_POS_LNORMAL, F32_POS_ONE , EXC_CHK_UNFL},
      { F32_NEG_LNORMAL, F32_NEG_LNORMAL, F32_POS_ONE , EXC_CHK_UNFL},

      //The following test cases are ported from AMD LibM
      {0x3F000000, 0x3EF57744, 0x3f60a940, EXC_CHK_NONE},  //0.5
      {0x3F333333, 0x3F24EB73, 0x3F43CCB3, EXC_CHK_NONE},  //.7
      {0x3F4CCCCD, 0x3F37A4A6, 0x3F325B5F, EXC_CHK_NONE},  //.8
      {0x3fc00000, 0x3F7F5BD5, 0x3D90DEAA, EXC_CHK_NONE},  //1.5
      {F32_POS_PIBY2, 0x3f800000, 0xb33bbd2e, EXC_CHK_NONE},  //1.57079637 = pi/2
      {0xC07AE148, 0x3F33BFDD, 0xBF3647C6, EXC_CHK_NONE},  //-3.92
      {0x40000000, 0x3F68c7b7, 0xbed51133, EXC_CHK_NONE},  //2
      {F32_POS_PI, 0xb3bbbd2e, 0xbf800000, EXC_CHK_NONE},  //pi
      {0x40C90FDB, 0x343bbd2e, 0x3f800000, EXC_CHK_NONE},  //2pi
      {0x41200000, 0xBF0B44F8, 0xBF56CD64, EXC_CHK_NONE},  //10
      {0x447A0000, 0x3F53AE61, 0x3F0FF813, EXC_CHK_NONE},  //1000
      {0xC8C35000, 0x3E11F59F, 0x3f7d62d4, EXC_CHK_NONE},  //-4e5

      //>=5.0e5 case
      {0x48F42400, 0x3E361962, 0xBF7BEB6C, EXC_CHK_NONE},  //500000
      {0x48F4CCC8, 0x3F3A0E4A, 0xBF2FD6B2, EXC_CHK_NONE},  //501350.25 > 5.0e5
      {0xC93F6800, 0xBE1DBF77, 0xBF7CF1B1, EXC_CHK_NONE},  //-784000

      //make case for expdiff > 15
      {0x43FCE5F1, 0xB20FD1DE, 0xbf800000, EXC_CHK_NONE},  //505.796417, remainder is ~0, very close multiple of piby2
      {0x42FCE5F1, 0x3F3504F3, 0x3F3504F3, EXC_CHK_NONE},  //126.449104
      {F32_POS_SNAN, F32_POS_SNAN,F32_POS_SNAN,EXC_CHK_INVL },  //
      {F32_NEG_SNAN, F32_NEG_SNAN,F32_NEG_SNAN,EXC_CHK_INVL },  //
      {F32_POS_INF,  0x7fc00000  ,0x7fc00000  ,EXC_CHK_INVL },  //
      {F32_NEG_INF,  0x7fc00000  ,0x7fc00000  ,EXC_CHK_INVL},  //
      {F32_POS_QNAN, F32_POS_QNAN,F32_POS_QNAN,EXC_CHK_NONE },  //
      {F32_NEG_QNAN, F32_NEG_QNAN,F32_NEG_QNAN,EXC_CHK_NONE },  //
      {F32_NEG_ZERO, F32_NEG_ZERO,0x3f800000,EXC_CHK_NONE },  //
      {0x387BA882,0x387BA882,0x3f800000, EXC_CHK_NONE},   // 6e-05, < 2^(-13)  // chenged from UNFL to NONE
      {0x00000039,0x00000039,0x3f800000,EXC_CHK_UNFL},     // min_float*180/pi
};

