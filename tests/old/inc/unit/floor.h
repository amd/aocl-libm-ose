//R.obtained=FN_PROTOTYPE(floor)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = floor(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_floor_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {F64_POS_SNAN,F64_POS_QNAN,EXC_CHK_INVL}, // invalid
    {F64_NEG_SNAN,F64_POS_QNAN,EXC_CHK_INVL}, // invalid
    {F64_POS_SNAN_Q,F64_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F64_NEG_SNAN_Q,F64_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F64_POS_QNAN,F64_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F64_NEG_QNAN,F64_POS_QNAN,EXC_CHK_NONE}, // invalid
    {F64_POS_INF,F64_POS_INF,EXC_CHK_NONE}, // invalid
    {F64_NEG_INF,F64_NEG_INF,EXC_CHK_NONE}, // invalid
    {F64_POS_ONE,F64_POS_ONE,EXC_CHK_NONE}, // invalid
    {F64_NEG_ONE,F64_NEG_ONE,EXC_CHK_NONE}, // invalid
    {F64_NEG_ZERO,F64_NEG_ZERO,EXC_CHK_NONE}, // invalid
    {F64_POS_ZERO,F64_POS_ZERO,EXC_CHK_NONE}, // invalid
    {F64_POS_HDENORM,F64_POS_ZERO,EXC_CHK_NONE}, //
    {F64_NEG_HDENORM,F64_NEG_ONE,EXC_CHK_NONE}, //
    {F64_POS_LDENORM,F64_POS_ZERO,EXC_CHK_NONE}, //
    {F64_NEG_LDENORM,F64_NEG_ONE,EXC_CHK_NONE}, //

    {F64_POS_HNORMAL,F64_POS_HNORMAL,EXC_CHK_NONE}, //
    {F64_NEG_HNORMAL,F64_NEG_HNORMAL,EXC_CHK_NONE}, //
    {F64_POS_LNORMAL,F64_POS_ZERO,EXC_CHK_NONE}, //
    {F64_NEG_HNORMAL,F64_NEG_HNORMAL,EXC_CHK_NONE}, //
    {F64_POS_PI,0x4008000000000000LL,EXC_CHK_NONE}, //
    {F64_NEG_PI,0xC010000000000000LL,EXC_CHK_NONE}, //
    {F64_POS_PIBY2,0x3fF0000000000000LL,EXC_CHK_NONE}, //
    {F64_NEG_PIBY2,0xC000000000000000LL,EXC_CHK_NONE}, //

};

