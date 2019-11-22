//R.obtained=FN_PROTOTYPE(tanf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_tanf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_tanf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {F32_POS_SNAN, F32_POS_SNAN, EXC_CHK_INVL },  //
    {F32_NEG_SNAN, F32_NEG_SNAN, EXC_CHK_INVL },  //
    {F32_POS_INF,  F32_POS_QNAN, EXC_CHK_INVL },  //
    {F32_NEG_INF,  F32_POS_QNAN, EXC_CHK_INVL },  //
    {F32_POS_QNAN, F32_POS_QNAN, EXC_CHK_NONE },  //
    {F32_NEG_QNAN, F32_NEG_QNAN, EXC_CHK_NONE },  //
    {F32_POS_ZERO, F32_POS_ZERO, EXC_CHK_NONE },  //
    {F32_NEG_ZERO, F32_NEG_ZERO, EXC_CHK_NONE },  //

    {0x00000039, 0x00000039, EXC_CHK_UNFL},  // min_float*180/pi
    {0x387BA882, 0x387BA882, EXC_CHK_UNFL},  // 6e-05, < 2^(-13)
	{0x3F490FDB, 0x3f800000, EXC_CHK_NONE},
    {0x3C000001, 0x3c0000ac, EXC_CHK_NONE},
    {0x39000001, 0x39000001, EXC_CHK_NONE},
    {0x3f800000, 0x3fc75923, EXC_CHK_NONE},  //1
    {0x40000000, 0xc00bd7b1, EXC_CHK_NONE},  //2
    {0x40490FDB, 0x33bbbd2e, EXC_CHK_NONE},  //pi
    {0x40C90FDB, 0x343bbd2e, EXC_CHK_NONE},  //2pi
    {0x41200000, 0x3f25fafa, EXC_CHK_NONE},  //10
    {0x447A0000, 0x3fbc3395, EXC_CHK_NONE},  //1000
    {0xC8C35000, 0x3e137717, EXC_CHK_NONE},  //-4e5
    {0x48F42400, 0xbe390c74, EXC_CHK_NONE},  //500000	
    {0x48F4CCC8, 0xbf876fff, EXC_CHK_NONE},  //501350.25 > 5.0e5
    {0xC93F6800, 0x3e1fa759, EXC_CHK_NONE},  //-784000      
    {0x7fc00000, 0x7fc00001, EXC_CHK_NONE},  //NaN
    {0x7fc00001, 0x7fc00001, EXC_CHK_NONE},  //NaN
    {0x7f800001, 0x7fc00001 ,EXC_CHK_INVL},  // + INF
    {0x7fef8000, 0x7fc00001, EXC_CHK_NONE},  // + QNAN
    {0xffef8000, 0x7fc00001, EXC_CHK_NONE},  // - QNAN      
    {0x80000000, 0x80000000, EXC_CHK_NONE},	 // - 0
    {0x43FCE5F1, 0x320fd1de, EXC_CHK_NONE},  //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550, 0xb79a62a2, EXC_CHK_NONE},  //182165.25, remainder is ~piby4
    {0x42FCE5F1, 0x3f800000, EXC_CHK_NONE},  //126.449104
 	  
	//Special case for verified the ulps
    {0x5123e87f, 0x318a4ed8, EXC_CHK_NONE},  //4.399877E+10, close to pi/2
    {0x50a3e87f, 0xcdeceba8, EXC_CHK_NONE},  //2.19993846E+10, close to pi/2, ported from OpenCL

};

