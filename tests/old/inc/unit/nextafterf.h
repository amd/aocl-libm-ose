//R.obtained=FN_PROTOTYPE(nextafterf)(P1.input, P2.input)
//float,float,float,ExcFlags
//P1 P2 R E
//unsigned int|float, unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = nextafterf(P1.V3[j],P2.V3[j])
//float float float ExcFlags
//ULP amd_ref_nextafterf_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    { F32_POS_ZERO, F32_NEG_ZERO, F32_NEG_ZERO   , EXC_CHK_NONE},
    { F32_POS_ZERO, F32_NEG_ONE , F32_NEG_LDENORM, EXC_CHK_NONE},
    { F32_POS_ZERO, F32_POS_ONE , F32_POS_LDENORM, EXC_CHK_NONE},
    { F32_NEG_ZERO, F32_POS_ZERO, F32_POS_ZERO   , EXC_CHK_NONE},
    { F32_NEG_ZERO, F32_POS_ONE , F32_POS_LDENORM, EXC_CHK_NONE},
    { F32_NEG_ZERO, F32_NEG_ONE , F32_NEG_LDENORM, EXC_CHK_NONE},

	{ F32_POS_ONE , F32_POS_PI     , 0x3f800001, EXC_CHK_NONE  },
	{ F32_POS_ONE , F32_POS_LDENORM, 0x3f7fffff, EXC_CHK_NONE  },
	{ F32_POS_ONE , F32_NEG_ONE    , 0x3f7fffff, EXC_CHK_NONE  },
	{ F32_NEG_ONE , F32_POS_PI     , 0xbf7fffff, EXC_CHK_NONE  },
	{ F32_NEG_ONE , F32_NEG_LDENORM, 0xbf7fffff, EXC_CHK_NONE  },
	{ F32_NEG_ONE , F32_NEG_PI     , 0xbf800001, EXC_CHK_NONE  },

	{ F32_POS_INF , F32_NEG_ONE , 0x7f7fffff, EXC_CHK_NONE},
	{ F32_POS_INF , F32_POS_ONE , 0x7f7fffff, EXC_CHK_NONE},
	{ F32_NEG_INF , F32_NEG_ONE , 0xff7fffff, EXC_CHK_NONE},
	{ F32_NEG_INF , F32_POS_ONE , 0xff7fffff, EXC_CHK_NONE},
	{ F32_NEG_INF , F32_NEG_QNAN, 0xffc00000, EXC_CHK_NONE},
	{ F32_POS_ONE , F32_POS_INF , 0x3f800001 , EXC_CHK_NONE  },

    { F32_POS_SNAN, F32_POS_ONE , F32_POS_SNAN_Q, EXC_CHK_INVL},
    { F32_POS_SNAN, F32_NEG_ONE , F32_POS_SNAN_Q, EXC_CHK_INVL},
    { F32_NEG_SNAN, F32_NEG_ONE , F32_NEG_SNAN_Q, EXC_CHK_INVL},
    { F32_NEG_SNAN, F32_POS_ONE , F32_NEG_SNAN_Q, EXC_CHK_INVL},
    { F32_POS_QNAN, F32_POS_ONE , F32_POS_QNAN  , EXC_CHK_NONE},
    { F32_POS_QNAN, F32_NEG_ONE , F32_POS_QNAN  , EXC_CHK_NONE},
    { F32_NEG_QNAN, F32_POS_ONE , F32_NEG_QNAN  , EXC_CHK_NONE},
    { F32_NEG_QNAN, F32_NEG_ONE , F32_NEG_QNAN  , EXC_CHK_NONE},

	//some random test cases
	{ 0x48000000, 0x3a800000, 0x47ffffff, EXC_CHK_NONE},
	{ 0xc8000000, 0x3a800000, 0xc7ffffff, EXC_CHK_NONE},
	{ 0x08800000, 0x58800000, 0x08800001, EXC_CHK_NONE},
	{ 0x88800000, 0x58800000, 0x887fffff, EXC_CHK_NONE},
	{ 0x3d800000, 0xc4800000, 0x3d7fffff, EXC_CHK_NONE},
	{ 0x3c4ccccd, 0x3c4ccccd, 0x3c4ccccd, EXC_CHK_NONE},
	{ 0xffffffff, 0x3c4ccccd, 0xffffffff, EXC_CHK_NONE},

};

