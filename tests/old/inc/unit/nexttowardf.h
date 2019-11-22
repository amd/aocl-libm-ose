//R.obtained=FN_PROTOTYPE(nexttowardf)(P1.input, P2.input)
//float,long double, float,ExcFlags
//P1 P2 R E
//unsigned int|float, long double|long double, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = nexttowardf(P1.V3[j], P2.V3[j])
//float,long double,float,ExcFlags
//ULP amd_ref_nexttowardf_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

 	{ F32_POS_ZERO, -0, F32_POS_ZERO   , EXC_CHK_NONE},
    { F32_POS_ZERO, -1 , F32_NEG_LDENORM, EXC_CHK_NONE},
    { F32_POS_ZERO, 1 , F32_POS_LDENORM, EXC_CHK_NONE},
    { F32_NEG_ZERO, 0, F32_POS_ZERO   , EXC_CHK_NONE},
    { F32_NEG_ZERO, 1 , F32_POS_LDENORM, EXC_CHK_NONE},
    { F32_NEG_ZERO, -1 , F32_NEG_LDENORM, EXC_CHK_NONE},

	{ F32_POS_ONE , 3.14159274101257, 0x3f800001, EXC_CHK_NONE  },
	{ F32_POS_ONE , 4.94065645841247E-324, 0x3f7fffff, EXC_CHK_NONE  },
	{ F32_POS_ONE , -1    , 0x3f7fffff, EXC_CHK_NONE  },
	{ F32_NEG_ONE , 3.14159274101257     , 0xbf7fffff, EXC_CHK_NONE  },
	{ F32_NEG_ONE , -4.94065645841247E-324, 0xbf7fffff, EXC_CHK_NONE  },
	{ F32_NEG_ONE , -3.14159274101257    , 0xbf800001, EXC_CHK_NONE  },

	{ F32_POS_INF , -1 , F32_POS_INF, EXC_CHK_NONE},
	{ F32_POS_INF , 1 , F32_POS_INF, EXC_CHK_NONE},
	{ F32_NEG_INF , -1 , F32_NEG_INF, EXC_CHK_NONE},
	{ F32_NEG_INF , 1 , F32_NEG_INF, EXC_CHK_NONE},
	/*{ F32_NEG_INF , F64_NEG_QNAN, F32_NEG_INF, EXC_CHK_NONE},
	{ F32_POS_ONE , F64_POS_INF , 0x3f800001 , EXC_CHK_NONE  },*/

    { F32_POS_SNAN, 1 , F32_POS_SNAN_Q, EXC_CHK_INVL},
    { F32_POS_SNAN, -1 , F32_POS_SNAN_Q, EXC_CHK_INVL},
    { F32_NEG_SNAN, -1 , F32_NEG_SNAN_Q, EXC_CHK_INVL},
    { F32_NEG_SNAN, 1 , F32_NEG_SNAN_Q, EXC_CHK_INVL},
    { F32_POS_QNAN, 1 , F32_POS_QNAN  , EXC_CHK_NONE},
    { F32_POS_QNAN, -1 , F32_POS_QNAN  , EXC_CHK_NONE},
    { F32_NEG_QNAN, 1 , F32_NEG_QNAN  , EXC_CHK_NONE},
    { F32_NEG_QNAN, -1 , F32_NEG_QNAN  , EXC_CHK_NONE},

	{ 0x3c4ccccd, 0.0125, 0x3c4ccccc, EXC_CHK_NONE},
	{ 0x3c4ccccd, 0.0125000029802322, 0x3c4cccce, EXC_CHK_NONE},

};

