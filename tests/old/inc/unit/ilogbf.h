//R.obtained=FN_PROTOTYPE(ilogbf)(P1.input)
//float,int,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|int,ExcFlags|ExcFlags
//R.V3[j] = ilogbf(P1.V3[j])
//float int ExcFlags
//ULP amd_ref_ilogbf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {F32_POS_SNAN    ,  0x80000000,EXC_CHK_INVL},
    {F32_NEG_SNAN    ,  0x80000000,EXC_CHK_INVL},
    {F32_POS_SNAN_Q  ,  0x80000000,EXC_CHK_NONE},
    {F32_NEG_SNAN_Q  ,  0x80000000,EXC_CHK_NONE},
    {F32_POS_QNAN    ,  0x80000000,EXC_CHK_NONE},
	{F32_NEG_QNAN    ,  0x80000000,EXC_CHK_NONE},
	{F32_POS_INF     ,  0x7FFFFFFF,EXC_CHK_NONE},
	{F32_NEG_INF     ,  0x7FFFFFFF,EXC_CHK_INVL},
    {F32_POS_ONE     ,  0x00000000,EXC_CHK_NONE},
    {F32_NEG_ONE     ,  0x00000000,EXC_CHK_NONE},
    {F32_POS_ZERO    ,  0x80000000,EXC_CHK_ZERO},
    {F32_NEG_ZERO    ,  0x80000000,EXC_CHK_ZERO},
	{F32_POS_HDENORM ,  0xffffff81,EXC_CHK_NONE},
	{F32_NEG_HDENORM ,  0xffffff81,EXC_CHK_NONE},
	{F32_POS_LDENORM ,  0xffffff6b,EXC_CHK_NONE},
    {F32_NEG_LDENORM ,  0xffffff6b,EXC_CHK_NONE},
    {F32_POS_HNORMAL ,  0x0000007f,EXC_CHK_NONE},
    {F32_NEG_HNORMAL ,  0x0000007f,EXC_CHK_NONE},
    {F32_POS_LNORMAL ,  0xffffff82,EXC_CHK_NONE},
	{F32_NEG_LNORMAL ,  0xffffff82,EXC_CHK_NONE},
	{F32_POS_PI      ,  0x00000001,EXC_CHK_NONE},
    {F32_NEG_PI      ,  0x00000001,EXC_CHK_NONE},
 	{F32_POS_PIBY2   ,  0x00000000,EXC_CHK_NONE},
    {F32_NEG_PIBY2   ,  0x00000000,EXC_CHK_NONE}

};
