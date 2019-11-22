//R.obtained=FN_PROTOTYPE(nexttoward)(P1.input, P2.input)
//double,long double,double,ExcFlags
//P1 P2 R E
//unsigned long long int|double, long  double |long  double,unsigned long long int|double, ExcFlags|ExcFlags
//R.V3[j] = nexttoward(P1.V3[j],P2.V3[j])
//double, long double, double, ExcFlags
//ULP amd_ref_nexttoward_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{


 	{ F64_POS_ZERO, -0, F64_POS_ZERO   , EXC_CHK_NONE},
    { F64_POS_ZERO, -1 , F64_NEG_LDENORM, EXC_CHK_NONE},
    { F64_POS_ZERO, 1 , F64_POS_LDENORM, EXC_CHK_NONE},
    { F64_NEG_ZERO, 0, F64_POS_ZERO   , EXC_CHK_NONE},
    { F64_NEG_ZERO, 1 , F64_POS_LDENORM, EXC_CHK_NONE},
    { F64_NEG_ZERO, -1 , F64_NEG_LDENORM, EXC_CHK_NONE},

	{ F64_POS_ONE , 3.14159274101257     , 0x3ff0000000000001LL, EXC_CHK_NONE  },
	{ F64_POS_ONE , 4.94065645841247E-324, 0x3fefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_POS_ONE , -1    , 0x3fefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_NEG_ONE , 3.14159274101257     , 0xbfefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_NEG_ONE , -4.94065645841247E-324, 0xbfefffffffffffffLL, EXC_CHK_NONE  },
	{ F64_NEG_ONE , -3.14159274101257     , 0xbff0000000000001LL, EXC_CHK_NONE  },

	{ F64_POS_INF , -1 , F64_POS_INF, EXC_CHK_NONE},
	{ F64_POS_INF , 1 , F64_POS_INF, EXC_CHK_NONE},
	{ F64_NEG_INF , -1 , F64_NEG_INF, EXC_CHK_NONE},
	{ F64_NEG_INF , 1 , F64_NEG_INF, EXC_CHK_NONE},
	/*{ F64_NEG_INF , F64_NEG_QNAN, F64_NEG_INF, EXC_CHK_NONE},
	{ F64_POS_ONE , F64_POS_INF , 0x3ff0000000000001LL, EXC_CHK_NONE  },*/

    { F64_POS_SNAN, 1 , F64_POS_SNAN_Q, EXC_CHK_INVL},
    { F64_POS_SNAN, -1 , F64_POS_SNAN_Q, EXC_CHK_INVL},
    { F64_NEG_SNAN, -1 , F64_NEG_SNAN_Q, EXC_CHK_INVL},
    { F64_NEG_SNAN, 1 , F64_NEG_SNAN_Q, EXC_CHK_INVL},
    { F64_POS_QNAN, 1 , F64_POS_QNAN  , EXC_CHK_NONE},
    { F64_POS_QNAN, -1 , F64_POS_QNAN  , EXC_CHK_NONE},
    { F64_NEG_QNAN, 1 , F64_NEG_QNAN  , EXC_CHK_NONE},
    { F64_NEG_QNAN, -1 , F64_NEG_QNAN  , EXC_CHK_NONE},

};

