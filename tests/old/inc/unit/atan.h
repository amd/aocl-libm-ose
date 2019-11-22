//R.obtained=FN_PROTOTYPE(atan)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_atan(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_atan_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

      {0x3ff300000000abcdLL, 0x3febde70ed442e77,EXC_CHK_NONE },  //0	
      {0x3fe700000000abcdLL, 0x3fe3f13fb89f083c,EXC_CHK_NONE },  //0	
      {0x3fd700000000abcdLL, 0x3fd61484030a6808,EXC_CHK_NONE },  //0	
      {0x3fdc00000000abcdLL, 0x3fda64eec3ccb430,EXC_CHK_NONE },  //0	
      {0x6ff300000000abcdLL, 0x3ff921fb54442d18,EXC_CHK_NONE },  //0	
      {0xc36300000000abcdLL, 0xbff921fb54442d18,EXC_CHK_NONE },  //0	

      {F64_POS_ZERO, F64_POS_ZERO,EXC_CHK_NONE },  //0	
      {F64_NEG_ZERO, F64_NEG_ZERO,EXC_CHK_NONE },  //0
      {F64_POS_INF, 0x3ff921fb54442d18LL,EXC_CHK_NONE  },  //1.57079637 = pi/2	
      {F64_NEG_INF, 0xbff921fb54442d18LL,EXC_CHK_NONE  },  //1.57079637 = pi/2	

      {F64_POS_SNAN, F64_POS_SNAN,EXC_CHK_INVL },  //
      {F64_NEG_SNAN, F64_NEG_SNAN,EXC_CHK_INVL },  //
      {F64_POS_QNAN, F64_POS_QNAN,EXC_CHK_NONE },  //
      {F64_NEG_QNAN, F64_NEG_QNAN,EXC_CHK_NONE },  //
  {F64_POS_LDENORM,F64_POS_LDENORM,EXC_CHK_UNFL},
  {F64_NEG_LDENORM,F64_NEG_LDENORM,EXC_CHK_UNFL},
  {F64_POS_HDENORM,F64_POS_HDENORM,EXC_CHK_UNFL},
  {F64_NEG_HDENORM,F64_NEG_HDENORM,EXC_CHK_UNFL},


};
