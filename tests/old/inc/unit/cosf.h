//R.obtained=FN_PROTOTYPE(cosf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_cosf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_cosf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()



static struct data input[] =
{
      {0x00000000, 0x3f800000,EXC_CHK_NONE },  //0
      {0x3F000000, 0x3f60a940,EXC_CHK_NONE },  //0.5
      {0x3F333333, 0x3F43CCB3,EXC_CHK_NONE },  //.7
      {0x3F4CCCCD, 0x3F325B5F,EXC_CHK_NONE },  //.8
      {0x3fc00000, 0x3D90DEAA,EXC_CHK_NONE },  //1.5
      {0x3FC90FDB, 0xb33bbd2e,EXC_CHK_NONE },  //1.57079637 = pi/2
      {0xC07AE148, 0xBF3647C6,EXC_CHK_NONE },  //-3.92

      {F32_POS_SNAN, F32_POS_SNAN,EXC_CHK_INVL },  //
      {F32_NEG_SNAN, F32_NEG_SNAN,EXC_CHK_INVL },  //
      {F32_POS_INF, 0x7fc00000,EXC_CHK_INVL },  //
      {F32_NEG_INF, 0x7fc00000,EXC_CHK_INVL },  //
      {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
      {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //

      {0x3F490FDB,0x3f3504f3,EXC_CHK_NONE},
      {0x3C000001,0x3f7ffe00,EXC_CHK_NONE},
      {0x39000001,0x3f800000,EXC_CHK_NONE},

      {0x3f800000, 0x3f0a5140,EXC_CHK_NONE},  //1
      {0x40000000, 0xbed51133,EXC_CHK_NONE},  //2
      {0x40490FDB, 0xbf800000,EXC_CHK_NONE},  //pi
      {0x40C90FDB, 0x3f800000,EXC_CHK_NONE},  //2pi
      {0x41200000, 0xbf56cd64,EXC_CHK_NONE},  //10
      {0x447A0000, 0x3f0ff813,EXC_CHK_NONE},  //1000
      {0xC8C35000, 0x3f7d62d4,EXC_CHK_NONE},  //-4e5
      {0x48F42400, 0xbf7beb6c,EXC_CHK_NONE},  //500000
      {0x48F4CCC8, 0xbf2fd6b2,EXC_CHK_NONE},  //501350.25 > 5.0e5
      {0xC93F6800, 0xbf7cf1b1,EXC_CHK_NONE},  //-784000

      {0x7fc00000, 0x7fc00000,EXC_CHK_NONE},  //NaN
      {0x7fc00001, 0x7fc00001,EXC_CHK_NONE},  //NaN
      {0x7f800001, 0x7fc00001,EXC_CHK_INVL},   // + INF
      {0x7fef8000, 0x7fef8000,EXC_CHK_NONE},   // + QNAN
      {0xffef8000, 0xffef8000,EXC_CHK_NONE},   // - QNAN

      {0x80000000, 0x3f800000,EXC_CHK_NONE},	// - 0
      {0x43FCE5F1, 0xbf800000,EXC_CHK_NONE},  //505.796417, remainder is ~0, very close multiple of piby2
      {0x4831E550, 0xbf800000,EXC_CHK_NONE},  //182165.25, remainder is ~piby4
      {0x42FCE5F1, 0x3F3504F3,EXC_CHK_NONE},  //126.449104

	  //Special case for verified the ulps
	  {0xd0a3e87f, 0xb10a4ed8, EXC_CHK_NONE},   //-2.19993846E+10, close to pi/2


};

