//R.obtained=FN_PROTOTYPE(logf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_logf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_logf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
        {0x6d6ab,0xc2b48831,EXC_CHK_NONE},
    { F32_POS_SNAN ,  0x7fc00001  ,EXC_CHK_INVL},
    { F32_NEG_SNAN ,  0xffc00001  ,EXC_CHK_INVL},
    { F32_POS_QNAN ,  0x7fc00000  ,EXC_CHK_NONE},
    { F32_NEG_QNAN ,  0xffc00000  ,EXC_CHK_NONE},
    { F32_POS_INF  ,  0x7f800000  ,EXC_CHK_NONE},
    { F32_NEG_INF  ,  0x7fc00000  ,EXC_CHK_INVL},
    { F32_POS_ONE  ,  0x00000000  ,EXC_CHK_NONE},
    { F32_NEG_ONE  ,  0x7fc00000  ,EXC_CHK_INVL},
    { F32_POS_ZERO ,  0xff800000  ,EXC_CHK_ZERO},
    { F32_NEG_ZERO ,  0xff800000  ,EXC_CHK_ZERO},
    { F32_POS_HDENORM,0xc2aeac50 ,EXC_CHK_NONE},
    { F32_NEG_HDENORM,0x7fc00000 ,EXC_CHK_INVL},

    { F32_POS_LDENORM,0xc2ce8ed0 ,EXC_CHK_NONE},
    { F32_NEG_LDENORM,0x7fc00000 ,EXC_CHK_INVL},
    { F32_POS_HNORMAL,0x42b17218 ,EXC_CHK_NONE},
    { F32_POS_LNORMAL,0xc2aeac50 ,EXC_CHK_NONE},
    { F32_NEG_HNORMAL,0x7fc00000 ,EXC_CHK_INVL},
    { F32_NEG_LNORMAL,0x7fc00000 ,EXC_CHK_INVL},    
    {F32_POS_PIBY2,   0x3ee735da ,EXC_CHK_NONE},  //pi/2
    {0x3c000000,0xc09b43d5,EXC_CHK_NONE},  //0.0078125
    {0x3f012345,0xbf2f2e21,EXC_CHK_NONE},  //0.504444
    {0x3F87AE14,0x3d6eab52,EXC_CHK_NONE},  //1.06
    {0x00000000,0xff800000,EXC_CHK_ZERO},  //0.0
    {0x3c7fffff,0xc0851592,EXC_CHK_NONE},  //0.0156249991
    {0x3f800000,0x00000000,EXC_CHK_NONE},  //1
    {0x40000000,0x3f317218,EXC_CHK_NONE},  //2
    {0x33d6bf95,0xc180f1dc,EXC_CHK_NONE},  //1.0000000e-7
    {F32_POS_PI,0x3f928683,EXC_CHK_NONE},  //pi
    {0x40c90fdb,0x3feb3f8e,EXC_CHK_NONE},  //2pi
    {0x41200000,0x40135d8e,EXC_CHK_NONE},  //10
    {0x447a0000,0x40dd0c55,EXC_CHK_NONE},  //1000
    {0x42800000,0x40851592,EXC_CHK_NONE},  //64
    {0x42af0000,0x408f17aa,EXC_CHK_NONE},  //87.5
    {0x42b00000,0x408f4658,EXC_CHK_NONE},  //88
    {0x42c00000,0x40920f24,EXC_CHK_NONE},  //96
    {0xc2af0000,0x7fc00000,EXC_CHK_INVL},  //-87.5
    {0xc2e00000,0x7fc00000,EXC_CHK_INVL},  //-112
    {0xc3000000,0x7fc00000,EXC_CHK_INVL},  //-128
    {0xc2aeac4f,0x7fc00000,EXC_CHK_INVL},  //-87.3365  smallest normal result
    {0xc2aeac50,0x7fc00000,EXC_CHK_INVL},  //-87.3365  largest denormal result
    {0xc2ce0000,0x7fc00000,EXC_CHK_INVL},  //-103
    {0x42b17216,0x408f895b,EXC_CHK_NONE},  //88.7228   largest value  --
    {0x42b17217,0x408f895b,EXC_CHK_NONE},  //88.7228   largest value
    {0x42b17218,0x408f895c,EXC_CHK_NONE},  //88.7228   overflow
    {0x50000000,0x41b6fda9,EXC_CHK_NONE},  //large   overflow
    {0xc20a1eb8,0x7fc00000,EXC_CHK_INVL}, // -34.53
    {0xc6de66b6,0x7fc00000,EXC_CHK_INVL}, // -28467.3555
    {0xbe99999a,0x7fc00000,EXC_CHK_INVL}, // -0.3
    {0xbf247208,0x7fc00000,EXC_CHK_INVL}, // -0.642365
    {0xbf000000,0x7fc00000,EXC_CHK_INVL}, // -0.5
    {0x3e99999a,0xbf9a1bc8,EXC_CHK_NONE}, // 0.3
    {0x3f247208,0xbee29c4a,EXC_CHK_NONE}, // 0.642365
    {0x3f000000,0xbf317218,EXC_CHK_NONE}, // 0.5
    {0x420a1eb8,0x4062ad51,EXC_CHK_NONE}, // 34.53
    {0x46de66b6,0x41241aae,EXC_CHK_NONE}, // 28467.3555
    {0xc2c80000,0x7fc00000,EXC_CHK_INVL}, // -100
    {0x42c80000,0x40935d8e,EXC_CHK_NONE}, // 100

};

