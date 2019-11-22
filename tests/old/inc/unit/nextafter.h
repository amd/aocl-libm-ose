//R.obtained=FN_PROTOTYPE(nextafter)(P1.input, P2.input)
//double,double,double,ExcFlags
//P1 P2 R E
//unsigned long long int|double, unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = nextafter(P1.V3[j],P2.V3[j])
//double double double ExcFlags
//ULP amd_ref_nextafter_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    { F64_POS_ZERO, F64_NEG_ZERO, F64_NEG_ZERO   , EXC_CHK_NONE},
    { F64_POS_ZERO, F64_NEG_ONE , F64_NEG_LDENORM, EXC_CHK_NONE},
    { F64_POS_ZERO, F64_POS_ONE , F64_POS_LDENORM, EXC_CHK_NONE},
    { F64_NEG_ZERO, F64_POS_ZERO, F64_POS_ZERO   , EXC_CHK_NONE},
    { F64_NEG_ZERO, F64_POS_ONE , F64_POS_LDENORM, EXC_CHK_NONE},
    { F64_NEG_ZERO, F64_NEG_ONE , F64_NEG_LDENORM, EXC_CHK_NONE},

	{ F64_POS_ONE , F64_POS_PI     , 0x3ff0000000000001LL, EXC_CHK_NONE  },
	{ F64_POS_ONE , F64_POS_LDENORM, 0x3fefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_POS_ONE , F64_NEG_ONE    , 0x3fefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_NEG_ONE , F64_POS_PI     , 0xbfefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_NEG_ONE , F64_NEG_LDENORM, 0xbfefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_NEG_ONE , F64_NEG_PI     , 0xbff0000000000001LL, EXC_CHK_NONE  },

	{ F64_POS_INF , F64_NEG_ONE , 0x7fefffffffffffff, EXC_CHK_NONE},
	{ F64_POS_INF , F64_POS_ONE , 0x7fefffffffffffff, EXC_CHK_NONE},
	{ F64_NEG_INF , F64_NEG_ONE , 0xffefffffffffffff, EXC_CHK_NONE},
	{ F64_NEG_INF , F64_POS_ONE , 0xffefffffffffffff, EXC_CHK_NONE},
	{ F64_NEG_INF , F64_NEG_QNAN, 0xfff8000000000000, EXC_CHK_NONE},
	{ F64_POS_ONE , F64_POS_INF , 0x3ff0000000000001, EXC_CHK_NONE  },

    { F64_POS_SNAN, F64_POS_ONE , F64_POS_SNAN_Q, EXC_CHK_INVL},
    { F64_POS_SNAN, F64_NEG_ONE , F64_POS_SNAN_Q, EXC_CHK_INVL},
    { F64_NEG_SNAN, F64_NEG_ONE , F64_NEG_SNAN_Q, EXC_CHK_INVL},
    { F64_NEG_SNAN, F64_POS_ONE , F64_NEG_SNAN_Q, EXC_CHK_INVL},
    { F64_POS_QNAN, F64_POS_ONE , F64_POS_QNAN  , EXC_CHK_NONE},
    { F64_POS_QNAN, F64_NEG_ONE , F64_POS_QNAN  , EXC_CHK_NONE},
    { F64_NEG_QNAN, F64_POS_ONE , F64_NEG_QNAN  , EXC_CHK_NONE},
    { F64_NEG_QNAN, F64_NEG_ONE , F64_NEG_QNAN  , EXC_CHK_NONE},

	//some random test cases
	{ 0x4100000000000000, 0x3f50000000000000, 0x40ffffffffffffff, EXC_CHK_NONE}, // 131072, 0.0009765625
	{ 0xc100000000000000, 0x3f50000000000000, 0xc0ffffffffffffff, EXC_CHK_NONE}, // -131072, 0.0009765625
	{ 0x3910000000000000, 0x4310000000c0b600, 0x3910000000000001, EXC_CHK_NONE}, // 7.70372E-34, 1.12589991E+15
	{ 0xb910000000000000, 0x4310000000c0b600, 0xb90fffffffffffff, EXC_CHK_NONE},
	{ 0x3fb0000000000000, 0xc090000000000000, 0x3fafffffffffffff, EXC_CHK_NONE}, //0.0625, -1024
	{ 0x3f8999999999999a, 0x3f8999999999999a, 0x3f8999999999999a, EXC_CHK_NONE}, //0.0125
	{ 0xffffffffffffffff, 0x3f8999999999999a, 0xfffffffffffffffe, EXC_CHK_NONE},

};

