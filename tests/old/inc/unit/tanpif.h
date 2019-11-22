//R.obtained=FN_PROTOTYPE(tanpif)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_tanpif(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_tanpif_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {F32_POS_SNAN, 0x7fc00001, EXC_CHK_INVL },  //
    {F32_NEG_SNAN, 0x7fc00001, EXC_CHK_INVL },  //
    {F32_POS_INF,  0x7fc00001, EXC_CHK_INVL },  //
    {F32_NEG_INF,  0x7fc00001, EXC_CHK_INVL },  //
    {F32_POS_QNAN, 0x7fc00001, EXC_CHK_NONE },  //
    {F32_NEG_QNAN, 0x7fc00001, EXC_CHK_NONE },  //
    {F32_POS_ZERO, 0x00000000, EXC_CHK_NONE },  //
    {F32_NEG_ZERO, 0x80000000, EXC_CHK_NONE },  //
    {0x00000039,   0xb3       ,  EXC_CHK_UNFL},  // min_float*180/pi
    {0x387BA882,   0x3945a6e4 ,  EXC_CHK_UNFL},  // 6e-05, < 2^(-13)
	{0x3F490FDB,   0xbf4c91c5 ,  EXC_CHK_NONE},
    {0x3C000001,   0x3cc91a33 ,  EXC_CHK_NONE},
    {0x39000001,   0x39c90fdd ,EXC_CHK_NONE},
    {0x3f800000,   0x80000000 ,EXC_CHK_NONE},  //1
    {0x40000000,   0x0        ,EXC_CHK_NONE},  //2
    {0x40490FDB,   0x3ef410c6 ,EXC_CHK_NONE},  //pi
    {0x40C90FDB,   0x3f9deab3 ,EXC_CHK_NONE},  //2pi
    {0x41200000,   0x0        ,EXC_CHK_NONE},  //10
    {0x447A0000,   0x0        ,EXC_CHK_NONE},  //1000
    {0xC8C35000,   0x80000000 ,EXC_CHK_NONE},  //-4e5
    {0x48F42400,   0x0        ,EXC_CHK_NONE},  //500000
    {0x48F4CCC8,   0x3f800000 ,EXC_CHK_NONE},  //501350.25 > 5.0e5
    {0xC93F6800,   0x80000000 ,EXC_CHK_NONE},  //-784000
    {0x7fc00000,   0x7fc00001 ,EXC_CHK_NONE},  //NaN
    {0x7fc00001,   0x7fc00001 ,EXC_CHK_NONE},  //NaN
    {0x7f800001,   0x7fc00001 ,EXC_CHK_INVL},  // + INF
    {0x7fef8000,   0x7fc00001 ,EXC_CHK_NONE},  // + QNAN
    {0xffef8000,   0x7fc00001 ,EXC_CHK_NONE},  // - QNAN
    {0x80000000,   0x80000000 ,EXC_CHK_NONE},	 // - 0
    {0x43FCE5F1,   0xbf3e6f0e ,EXC_CHK_NONE},  //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550,   0x3f800000 ,EXC_CHK_NONE},  //182165.25, remainder is ~piby4
    {0x42FCE5F1,   0x40c66cbb ,EXC_CHK_NONE},  //126.449104
    {0x5123e87f,   0x0        ,EXC_CHK_NONE},  //4.399877E+10, close to pi/2
    {0x50a3e87f,   0x0        ,EXC_CHK_NONE},  //2.19993846E+10, close to pi/2, ported from OpenCL

};

