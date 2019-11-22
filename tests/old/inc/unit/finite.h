//R.obtained=FN_PROTOTYPE(finite)(P1.input)
//double,int,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  int|int,ExcFlags|ExcFlags
//R.V3[j] = finite(P1.V3[j])
//double int ExcFlags
//ULP amd_ref_finite_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {F64_POS_SNAN   , 0x00000000,EXC_CHK_INVL},
    {F64_NEG_SNAN   , 0x00000000,EXC_CHK_INVL},
    {F64_POS_SNAN_Q , 0x00000000,EXC_CHK_NONE},
    {F64_NEG_SNAN_Q , 0x00000000,EXC_CHK_NONE},
    {F64_POS_QNAN   , 0x00000000,EXC_CHK_NONE},
    {F64_NEG_QNAN   , 0x00000000,EXC_CHK_NONE},
    {F64_POS_INF    , 0x00000000,EXC_CHK_NONE},
    {F64_NEG_INF    , 0x00000000,EXC_CHK_NONE},
    {F64_POS_ONE    , 0x00000001,EXC_CHK_NONE},
    {F64_NEG_ONE    , 0x00000001,EXC_CHK_NONE},
    {F64_POS_ZERO   , 0x00000001,EXC_CHK_NONE},
    {F64_NEG_ZERO   , 0x00000001,EXC_CHK_NONE},
    {F64_POS_HDENORM, 0x00000001,EXC_CHK_NONE},
    {F64_NEG_HDENORM, 0x00000001,EXC_CHK_NONE},
    {F64_POS_LDENORM, 0x00000001,EXC_CHK_NONE},
    {F64_NEG_LDENORM, 0x00000001,EXC_CHK_NONE},
    {F64_POS_HNORMAL, 0x00000001,EXC_CHK_NONE},
    {F64_NEG_HNORMAL, 0x00000001,EXC_CHK_NONE},
    {F64_POS_LNORMAL, 0x00000001,EXC_CHK_NONE},
    {F64_NEG_LNORMAL, 0x00000001,EXC_CHK_NONE},
    {F64_POS_PI     , 0x00000001,EXC_CHK_NONE},
    {F64_NEG_PI     , 0x00000001,EXC_CHK_NONE},
    {F64_POS_PIBY2  , 0x00000001,EXC_CHK_NONE},
    {F64_NEG_PIBY2  , 0x00000001,EXC_CHK_NONE}

};

