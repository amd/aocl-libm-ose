//R.obtained=FN_PROTOTYPE(floorf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = floorf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_floorf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {F32_POS_SNAN,F32_POS_QNAN,EXC_CHK_INVL}, // invalid
    {F32_NEG_SNAN,F32_POS_QNAN,EXC_CHK_INVL}, // invalid
    {F32_POS_SNAN_Q,F32_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F32_NEG_SNAN_Q,F32_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F32_POS_QNAN,F32_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F32_NEG_QNAN,F32_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F32_POS_INF,F32_POS_INF,EXC_CHK_NONE}, // invalid
    {F32_NEG_INF,F32_NEG_INF,EXC_CHK_NONE}, // invalid
    {F32_POS_ONE,F32_POS_ONE,EXC_CHK_NONE}, // invalid
    {F32_NEG_ONE,F32_NEG_ONE,EXC_CHK_NONE}, // invalid
    {F32_NEG_ZERO,F32_NEG_ZERO,EXC_CHK_NONE}, // invalid
    {F32_POS_ZERO,F32_POS_ZERO,EXC_CHK_NONE}, // invalid
    {F32_POS_HDENORM,F32_POS_ZERO,EXC_CHK_NONE}, //
    {F32_NEG_HDENORM,F32_NEG_ONE,EXC_CHK_NONE}, //
    {F32_POS_LDENORM,F32_POS_ZERO,EXC_CHK_NONE}, //
    {F32_NEG_LDENORM,F32_NEG_ONE,EXC_CHK_NONE}, //

    {F32_POS_HNORMAL,F32_POS_HNORMAL,EXC_CHK_NONE}, //
    {F32_NEG_HNORMAL,F32_NEG_HNORMAL,EXC_CHK_NONE}, //
    {F32_POS_LNORMAL,F32_POS_ZERO,EXC_CHK_NONE}, //
    {F32_NEG_HNORMAL,F32_NEG_HNORMAL,EXC_CHK_NONE}, //
    {F32_POS_PI,0x40400000,EXC_CHK_NONE}, //
    {F32_NEG_PI,0xc0800000,EXC_CHK_NONE}, //
    {F32_POS_PIBY2,0x3f800000,EXC_CHK_NONE}, //
    {F32_NEG_PIBY2,0xc0000000,EXC_CHK_NONE}, //
    {0x400851EB,0x40000000,EXC_CHK_NONE}, //
    {0x4b800000,0x4b800000,EXC_CHK_NONE}, // equal to 2^24
    {0x4c000000,0x4c000000,EXC_CHK_NONE}, // greater than 2^24

};

