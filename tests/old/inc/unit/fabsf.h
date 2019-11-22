//R.obtained=FN_PROTOTYPE(fabsf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = fabsf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_fabsf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
   { F32_POS_SNAN , F32_POS_SNAN_Q ,EXC_CHK_INVL},
    { F32_NEG_SNAN , F32_NEG_SNAN_Q ,EXC_CHK_INVL},
    { F32_POS_QNAN , F32_POS_QNAN   ,EXC_CHK_NONE},
    { F32_NEG_QNAN , F32_NEG_QNAN   ,EXC_CHK_NONE},
    { F32_POS_INF  , F32_POS_INF    ,EXC_CHK_NONE},
    { F32_NEG_INF  , F32_POS_INF    ,EXC_CHK_NONE},
    { F32_POS_ONE  , F32_POS_ONE    ,EXC_CHK_NONE},
    { F32_NEG_ONE  , F32_POS_ONE    ,EXC_CHK_NONE},
    { F32_POS_ZERO , F32_POS_ZERO   ,EXC_CHK_NONE},
    { F32_NEG_ZERO , F32_POS_ZERO   ,EXC_CHK_NONE},
    { F32_POS_HDENORM, F32_POS_HDENORM, EXC_CHK_NONE},
    { F32_NEG_HDENORM, F32_POS_HDENORM, EXC_CHK_NONE},
    { F32_POS_LDENORM, F32_POS_LDENORM, EXC_CHK_NONE},
    { F32_NEG_LDENORM, F32_POS_LDENORM, EXC_CHK_NONE},
    { F32_POS_HNORMAL, F32_POS_HNORMAL, EXC_CHK_NONE},
    { F32_NEG_HNORMAL, F32_POS_HNORMAL, EXC_CHK_NONE},
    { F32_POS_LNORMAL, F32_POS_LNORMAL, EXC_CHK_NONE},
    { F32_NEG_LNORMAL, F32_POS_LNORMAL, EXC_CHK_NONE},
    {F32_POS_PIBY2, F32_POS_PIBY2, EXC_CHK_NONE},  //pi/2
    {F32_NEG_PIBY2, F32_POS_PIBY2, EXC_CHK_NONE},  //pi/2
    {0xc2c80000,0x42c80000,EXC_CHK_NONE}, // -100
    {0x42c80000,0x42c80000,EXC_CHK_NONE}, // 100

};

