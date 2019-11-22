//R.obtained=FN_PROTOTYPE(exp10f)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_exp10f(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_exp10f_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    { F32_POS_SNAN , F32_POS_SNAN_Q ,EXC_CHK_INVL},
    { F32_NEG_SNAN , F32_NEG_SNAN_Q ,EXC_CHK_INVL},
    { F32_POS_QNAN , F32_POS_QNAN   ,EXC_CHK_NONE},
    { F32_NEG_QNAN , F32_NEG_QNAN   ,EXC_CHK_NONE},
    { F32_POS_INF  , F32_POS_INF    ,EXC_CHK_NONE},
    { F32_NEG_INF  , F32_POS_ZERO   ,EXC_CHK_NONE},
    { F32_POS_ONE  , 0x41200000     ,EXC_CHK_NONE},
    { F32_NEG_ONE  , 0x3dcccccd     ,EXC_CHK_NONE},
    { F32_POS_ZERO , F32_POS_ONE    ,EXC_CHK_NONE},
    { F32_NEG_ZERO , F32_POS_ONE    ,EXC_CHK_NONE},
    { F32_POS_HDENORM, F32_POS_ONE  ,EXC_CHK_NONE},
    { F32_NEG_HDENORM, F32_POS_ONE  ,EXC_CHK_NONE},
    { F32_POS_LDENORM, F32_POS_ONE  ,EXC_CHK_NONE},
    { F32_NEG_LDENORM, F32_POS_ONE  ,EXC_CHK_NONE},
    { F32_POS_HNORMAL, F32_POS_INF  ,EXC_CHK_OVFL},
    { F32_NEG_HNORMAL, F32_POS_ZERO ,EXC_CHK_UNFL},
    { F32_POS_LNORMAL, F32_POS_ONE  ,EXC_CHK_NONE},
    { F32_NEG_LNORMAL, F32_POS_ONE  ,EXC_CHK_NONE},    
    {F32_POS_PIBY2,0x4214e309,EXC_CHK_NONE},  //pi/2
    {0x3c000000,0x3f8252cc,EXC_CHK_NONE},  //0.0078125
    {0x3c7fffff,0x3f84b063,EXC_CHK_NONE},  //0.0156249991
    {0x3f012345,0x404c77b1,EXC_CHK_NONE},  //0.504444
    {0x3f800000,0x41200000,EXC_CHK_NONE},  //1
    {0x40000000,0x42c80000,EXC_CHK_NONE},  //2
    {0x33d6bf95,0x3f800002,EXC_CHK_NONE},  //1.0000000e-7
    {F32_POS_PI,0x44ad2e98,EXC_CHK_NONE},  //pi
    {0x40c90fdb,0x49ea5003,EXC_CHK_NONE},  //2pi
    {0x41200000,0x501502f9,EXC_CHK_NONE},  //10
    {0x447a0000,0x7f800000,EXC_CHK_OVFL},  //1000
    {0x42800000,0x7f800000,EXC_CHK_OVFL},  //64
    {0x42160000,0x7dbe529a,EXC_CHK_NONE},  //37.5
    {0x42180000,0x7e967699,EXC_CHK_NONE},  //38
    {0x42c00000,0x7f800000,EXC_CHK_OVFL},  //96
    {0xc2af0000,0x00000000,EXC_CHK_UNFL},  //-87.5
    {0xc2e00000,0x00000000,EXC_CHK_UNFL},  //-112
    {0xc3000000,0x00000000,EXC_CHK_UNFL},  //-128
    {0xc2aeac4f,0x00000000,EXC_CHK_UNFL},  //-87.3365  smallest normal result
    {0xc2aeac50,0x00000000,EXC_CHK_UNFL},  //-87.3365  largest denormal result
    {0xc2ce0000,0x00000000,EXC_CHK_UNFL},  //-103
    {0x421A209A,0x7f7fffb3,EXC_CHK_NONE},  //38.531   largest value  --
    {0x421A209B,0x7f800000,EXC_CHK_OVFL},  //38.531   largest value
    {0x421A209C,0x7f800000,EXC_CHK_OVFL},  //38.531   overflow
    {0x50000000,0x7f800000,EXC_CHK_OVFL},  //large   overflow
    {0xc20a1eb8,0x061ce9e4,EXC_CHK_NONE}, // -34.53
    {0xc6de66b6,0x00000000,EXC_CHK_UNFL}, // -28467.3555
    {0xbe99999a,0x3f004dce,EXC_CHK_NONE}, // -0.3
    {0xbf247208,0x3e694f95,EXC_CHK_NONE}, // -0.642365
    {0xbf000000,0x3ea1e89b,EXC_CHK_NONE}, // -0.5
    {0x3e99999a,0x3fff64c2,EXC_CHK_NONE}, // 0.3
    {0x3f247208,0x408c72a3,EXC_CHK_NONE}, // 0.642365
    {0x3f000000,0x404a62c2,EXC_CHK_NONE}, // 0.5
    {0x420a1eb8,0x78d0d408,EXC_CHK_NONE}, // 34.53
    {0x46de66b6,0x7f800000,EXC_CHK_OVFL}, // 28467.3555
    {0xc2c80000,0x00000000,EXC_CHK_UNFL}, // -100
    {0x42c80000,0x7f800000,EXC_CHK_OVFL}, // 100

};
