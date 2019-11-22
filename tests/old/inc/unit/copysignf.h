//R.obtained=FN_PROTOTYPE(copysignf)(P1.input, P2.input)
//float, float,float,ExcFlags
//P1 P2 R E
//unsigned int|float, unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = copysignf(P1.V3[j],P2.V3[j])
//float float float ExcFlags
//ULP amd_ref_copysignf_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

{F32_POS_ZERO,F32_NEG_ZERO, F32_NEG_ZERO, EXC_CHK_NONE },  //0
      {F32_NEG_ZERO,F32_NEG_ZERO, F32_NEG_ZERO, EXC_CHK_NONE },  //0

      {F32_POS_ZERO,F32_POS_ZERO, F32_POS_ZERO, EXC_CHK_NONE },  //0
      {F32_NEG_ZERO,F32_POS_ZERO, F32_POS_ZERO, EXC_CHK_NONE },  //0

      {0xBF800000,F32_POS_ZERO, 0x3F800000, EXC_CHK_NONE },
      {0xBF800000,F32_NEG_ZERO, 0xBF800000, EXC_CHK_NONE },


      {0x3F800000,F32_NEG_INF, 0xBF800000, EXC_CHK_NONE },
      {0xBF800000,F32_POS_INF, 0x3F800000, EXC_CHK_NONE },


      {0x3F800000,F32_POS_INF, 0x3F800000, EXC_CHK_NONE },
      {0xBF800000,F32_NEG_INF, 0xBF800000, EXC_CHK_NONE },
};

