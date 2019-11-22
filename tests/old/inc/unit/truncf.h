//R.obtained=FN_PROTOTYPE(truncf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_truncf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_truncf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    /*Constant inputs from the Constants.h*/
    {F32_POS_SNAN,F32_POS_QNAN,EXC_CHK_INVL}, // invalid
    {F32_NEG_SNAN,F32_POS_QNAN,EXC_CHK_INVL}, // invalid
    {F32_POS_SNAN_Q,F32_POS_QNAN,EXC_CHK_NONE}, // 
    {F32_NEG_SNAN_Q,F32_POS_QNAN,EXC_CHK_NONE}, // 
    {F32_POS_QNAN,F32_POS_QNAN,EXC_CHK_NONE}, // 
    {F32_NEG_QNAN,F32_POS_QNAN,EXC_CHK_NONE}, // 
    {F32_POS_INF,F32_POS_INF,EXC_CHK_NONE}, // 
    {F32_NEG_INF,F32_NEG_INF,EXC_CHK_NONE}, // 
    {F32_POS_ONE,F32_POS_ONE,EXC_CHK_NONE}, // 
    {F32_NEG_ONE,F32_NEG_ONE,EXC_CHK_NONE}, // 
    {F32_NEG_ZERO,F32_NEG_ZERO,EXC_CHK_NONE}, // 
    {F32_POS_ZERO,F32_POS_ZERO,EXC_CHK_NONE}, // 
    {F32_POS_HDENORM,F32_POS_ZERO,EXC_CHK_NONE}, // 
    {F32_NEG_HDENORM,F32_NEG_ZERO,EXC_CHK_NONE}, // 
    {F32_POS_LDENORM,F32_POS_ZERO,EXC_CHK_NONE}, // 
    {F32_NEG_LDENORM,F32_NEG_ZERO,EXC_CHK_NONE}, //

    {F32_POS_HNORMAL,F32_POS_HNORMAL,EXC_CHK_NONE}, // 
    {F32_NEG_HNORMAL,F32_NEG_HNORMAL,EXC_CHK_NONE}, // 
    {F32_POS_LNORMAL,F32_POS_ZERO,EXC_CHK_NONE}, // 
    {F32_NEG_HNORMAL,F32_NEG_HNORMAL,EXC_CHK_NONE}, // 

    {F32_POS_PI,0x40400000,EXC_CHK_NONE}, // 
    {F32_NEG_PI,0xc0400000,EXC_CHK_NONE}, // 
    {F32_POS_PIBY2,0x3f800000,EXC_CHK_NONE}, // 
    {F32_NEG_PIBY2,0xbf800000,EXC_CHK_NONE}, // 
    /*Some Common Inputs*/
    {0x3fF12345,0x3f800000,EXC_CHK_NONE},
    {0xbfF12345,0xbf800000,EXC_CHK_NONE},
    {0x3fb12345,0x3f800000,EXC_CHK_NONE},
    {0xbfb12345,0xbf800000,EXC_CHK_NONE},
    {0x30b12345,0x00000000,EXC_CHK_NONE},
    {0xb0b12345,0x80000000,EXC_CHK_NONE},
    {0x400851EB,0x40000000,EXC_CHK_NONE}, // 
    {0x4b800000,0x4b800000,EXC_CHK_NONE}, // equal to 2^24
    {0xcb800000,0xcb800000,EXC_CHK_NONE}, // equal to 2^24
    {0x4b001234,0x4b001234,EXC_CHK_NONE}, // less than 2^24
    {0xcb001234,0xcb001234,EXC_CHK_NONE}, // less than 2^24
    {0x4ab70123,0x4ab70122,EXC_CHK_NONE}, // less than 2^24
    {0xcab70123,0xcab70122,EXC_CHK_NONE}, // less than 2^24
    {0x4c000000,0x4c000000,EXC_CHK_NONE}, // greater than 2^24
    {0xcc000000,0xcc000000,EXC_CHK_NONE}, // greater than 2^24
    {0x43ABCDEF,0x43ab8000,EXC_CHK_NONE}, // less than 2^24
    {0xC3ABCDEF,0xc3ab8000,EXC_CHK_NONE}, // less than 2^24

};

