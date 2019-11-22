//R.obtained=FN_PROTOTYPE(copysign)(P1.input, P2.input)
//double, double,double,ExcFlags
//P1 P2 R E
//unsigned long long int|double, unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = copysign(P1.V3[j],P2.V3[j])
//double double double ExcFlags
//ULP amd_ref_copysign_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

{F64_POS_ZERO,F64_NEG_ZERO, F64_NEG_ZERO, EXC_CHK_NONE },  //0
      {F64_NEG_ZERO,F64_NEG_ZERO, F64_NEG_ZERO, EXC_CHK_NONE },  //0

      {F64_POS_ZERO,F64_POS_ZERO, F64_POS_ZERO, EXC_CHK_NONE },  //0
      {F64_NEG_ZERO,F64_POS_ZERO, F64_POS_ZERO, EXC_CHK_NONE },  //0

      {0xBFF0000000000000,F64_POS_ZERO, 0x3FF0000000000000, EXC_CHK_NONE },
      {0xBFF0000000000000,F64_NEG_ZERO, 0xBFF0000000000000, EXC_CHK_NONE },


      {0x3FF0000000000000,F64_NEG_INF, 0xBFF0000000000000, EXC_CHK_NONE },
      {0xBFF0000000000000,F64_POS_INF, 0x3FF0000000000000, EXC_CHK_NONE },


      {0x3FF0000000000000,F64_POS_INF, 0x3FF0000000000000, EXC_CHK_NONE },
      {0xBFF0000000000000,F64_NEG_INF, 0xBFF0000000000000, EXC_CHK_NONE },

};

