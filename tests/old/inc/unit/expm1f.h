//R.obtained=FN_PROTOTYPE(expm1f)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_expm1f(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_expm1f_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    { F32_POS_SNAN , F32_POS_SNAN_Q ,EXC_CHK_INVL},
    { F32_NEG_SNAN , F32_NEG_SNAN_Q ,EXC_CHK_INVL},
    { F32_POS_QNAN , F32_POS_QNAN   ,EXC_CHK_NONE},
    { F32_NEG_QNAN , F32_NEG_QNAN   ,EXC_CHK_NONE},
    { F32_POS_INF  , F32_POS_INF    ,EXC_CHK_NONE},
    { F32_NEG_INF  , F32_NEG_ONE   ,EXC_CHK_NONE},
    { F32_POS_ONE  , 0x3fdbf0a9     ,EXC_CHK_NONE},
    { F32_NEG_ONE  , 0xbf21d2a7     ,EXC_CHK_NONE},
    { F32_POS_ZERO , F32_POS_ZERO    ,EXC_CHK_NONE},
    { F32_NEG_ZERO , F32_NEG_ZERO    ,EXC_CHK_NONE},
    { F32_POS_HDENORM, F32_POS_HDENORM  ,EXC_CHK_UNFL},
    { F32_NEG_HDENORM, F32_NEG_HDENORM  ,EXC_CHK_UNFL},
    { F32_POS_LDENORM, F32_POS_LDENORM  ,EXC_CHK_UNFL},
    { F32_NEG_LDENORM, F32_NEG_LDENORM  ,EXC_CHK_UNFL},
    { F32_POS_HNORMAL, F32_POS_INF  ,EXC_CHK_OVFL},
    { F32_NEG_HNORMAL, F32_NEG_ONE ,EXC_CHK_NONE},
    { F32_POS_LNORMAL, F32_POS_LNORMAL  ,EXC_CHK_UNFL},
    { F32_NEG_LNORMAL, F32_NEG_LNORMAL  ,EXC_CHK_UNFL},
    {F32_POS_PIBY2,0x4073dedd,EXC_CHK_NONE},  //pi/2
    {0x3c000000,0x3c008056,EXC_CHK_NONE},  //0.0078125
    {0x3c7fffff,0x3c810156,EXC_CHK_NONE},  //0.0156249991
    {0x3f012345,0x3f27f3e3,EXC_CHK_NONE},  //0.5044444
    {0x3f800000,0x3fdbf0a9,EXC_CHK_NONE},  //1
    {0x40000000,0x40cc7326,EXC_CHK_NONE},  //2
    {0x33d6bf95,0x33d6bf96,EXC_CHK_NONE},  //1.0000000e-7
    {F32_POS_PI,0x41b12025,EXC_CHK_NONE},  //pi
    {0x40c90fdb,0x44059f79,EXC_CHK_NONE},  //2pi
    {0x41200000,0x46ac12ee,EXC_CHK_NONE},  //10
    {0x447a0000,0x7f800000,EXC_CHK_OVFL},  //1000
    {0x42800000,0x6da12cc1,EXC_CHK_NONE},  //64
    {0x42af0000,0x7e96bab3,EXC_CHK_NONE},  //87.5
    {0x42b00000,0x7ef882b7,EXC_CHK_NONE},  //88
    {0x42c00000,0x7f800000,EXC_CHK_OVFL},  //96
    {0xc2af0000,0xbf800000,EXC_CHK_NONE},  //-87.5
    {0xc2e00000,0xbf800000,EXC_CHK_NONE},  //-112
    {0xc3000000,0xbf800000,EXC_CHK_NONE},  //-128
    {0xc2aeac4f,0xbf800000,EXC_CHK_NONE},  //-87.3365  smallest normal result
    {0xc2aeac50,0xbf800000,EXC_CHK_NONE},  //-87.3365  largest denormal result
    {0xc2ce0000,0xbf800000,EXC_CHK_NONE},  //-103
    {0x42b17216,0x7f7fff04,EXC_CHK_NONE},  //88.7228   largest value  --
    {0x42b17217,0x7f7fff84,EXC_CHK_NONE},  //88.7228   largest value
    {0x42b17218,0x7f800000,EXC_CHK_OVFL},  //88.7228   overflow
    {0x50000000,0x7f800000,EXC_CHK_OVFL},  //large   overflow
    {0xc20a1eb8,0xbf800000,EXC_CHK_NONE}, // -34.53
    {0xc6de66b6,0xbf800000,EXC_CHK_NONE}, // -28467.3555
    {0xbe99999a,0xbe84b37a,EXC_CHK_NONE}, // -0.3
    {0xbf247208,0xbef2a9fc,EXC_CHK_NONE}, // -0.642365
    {0xbf000000,0xbec974d0,EXC_CHK_NONE}, // -0.5
    {0x3e99999a,0x3eb320b2,EXC_CHK_NONE}, // 0.3
    {0x3f247208,0x3f66a60f,EXC_CHK_NONE}, // 0.642365
    {0x3f000000,0x3f261299,EXC_CHK_NONE}, // 0.5
    {0x420a1eb8,0x586162f9,EXC_CHK_NONE}, // 34.53
    {0x46de66b6,0x7f800000,EXC_CHK_OVFL}, // 28467.3555
    {0xc2c80000,0xbf800000,EXC_CHK_NONE}, // -100
    {0x42c80000,0x7f800000,EXC_CHK_OVFL}, // 100

};

