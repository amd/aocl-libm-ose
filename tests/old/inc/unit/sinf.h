//R.obtained=FN_PROTOTYPE(sinf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_sinf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_sinf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {0x00000000,  0x00000000,EXC_CHK_NONE},	// 0
    {0x387BA882,  0x387BA882,EXC_CHK_UNFL},   // 6e-05, < 2^(-13)
    {0x3B800000,  0x3B7FFFD5,EXC_CHK_NONE},   // 0.00390625 < 2^(-7)
    {0x3fc90de3,  0x3f800000,EXC_CHK_NONE},   // 1.57074 = pi/2 - 6e-05
    {0x3fc911d2,  0x3f800000,EXC_CHK_NONE},   // 1.57086 = pi/2 + 6e-05
    {0x3fc88fdb,  0x3f7fff80,EXC_CHK_NONE},   // 1.56689 = pi/2 - 0.00390625
    {0x3fc98fdb,  0x3f7fff80,EXC_CHK_NONE},   // 1.57470 = pi/2 + 0.00390625

    {F32_POS_SNAN, F32_POS_SNAN,EXC_CHK_INVL },  //
    {F32_NEG_SNAN, F32_NEG_SNAN,EXC_CHK_INVL },  //
    {F32_POS_INF, 0x7fc00000,EXC_CHK_INVL },  //
    {F32_NEG_INF, 0x7fc00000,EXC_CHK_INVL },  //
    {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
    {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //
    {F32_NEG_ZERO, F32_NEG_ZERO,EXC_CHK_NONE },  //

    {0x00000039,0x00000039,EXC_CHK_UNFL},     // min_float*180/pi

    {0x3f800000, 0x3f576aa4, EXC_CHK_NONE},  //1 
    {0x40000000, 0x3f68c7b7, EXC_CHK_NONE},  //2
    {0x40490FDB, 0xb3bbbd2e, EXC_CHK_NONE},  //pi
    {0x40C90FDB, 0x343bbd2e, EXC_CHK_NONE},  //2pi
    {0x41200000, 0xbf0b44f8, EXC_CHK_NONE},  //10
    {0x447A0000, 0x3f53ae61, EXC_CHK_NONE},  //1000
    {0xC8C35000, 0x3e11f59f, EXC_CHK_NONE},  //-4e5
    {0x48F42400, 0x3e361962, EXC_CHK_NONE},  //500000	
    {0x48F4CCC8, 0x3f3a0e4a, EXC_CHK_NONE},  //501350.25 > 5.0e5
    {0xC93F6800, 0xbe1dbf77, EXC_CHK_NONE},  //-784000
		    
    {0x7fc00000, 0x7fc00000, EXC_CHK_NONE},  //NaN
    {0x7fc00001, 0x7fc00001, EXC_CHK_NONE},  //NaN
    {0x7f800001, 0x7fc00001, EXC_CHK_INVL},   // + INF
    {0x7fef8000, 0x7fef8000, EXC_CHK_NONE},   // + QNAN
    {0xffef8000, 0xffef8000, EXC_CHK_NONE},   // - QNAN
		    
    {0x80000000, 0x80000000, EXC_CHK_NONE},	// - 0
    {0x43FCE5F1, 0xb20fd1de, EXC_CHK_NONE},  //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550, 0x379a62a2, EXC_CHK_NONE},  //182165.25, remainder is ~piby4
    {0x42FCE5F1, 0x3F3504F3, EXC_CHK_NONE},  //126.449104
 	  
	//Special case for verified the ulps
    {0x5123e87f, 0xb18a4ed8, EXC_CHK_NONE},   //4.399877E+10, close to pi/2

};

